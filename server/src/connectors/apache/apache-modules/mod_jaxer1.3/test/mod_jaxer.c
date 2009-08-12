#include "unistd.h"
#include "apr.h"
#include "ap_config.h"
#include "ap_mmn.h"
#include "httpd.h"
#include "http_main.h"
#include "http_core.h"
#include "http_request.h"
#include "http_protocol.h"
#include "apr_buckets.h"
#include "apr_thread_proc.h"
#include "apr_strings.h"
#include "util_script.h"
#include "http_log.h"
#include "mod_jaxer_conf.h"
#include "mod_jaxer_proc.h"
#include "mod_jaxer_protocol.h"
#include "mod_jaxer_connection.h"
#include "mod_jaxer_message.h"
#include "compat.h"

static int g_php_fix_pathinfo_enable = 0;

static void reset_request(request_rec *r, int length)
{
  r->remaining = length;
  r->read_length = 0;
  r->read_chunked = 0;
}

static void jaxer_initializer(server_rec *s, pool *p)
{
    if (global_apr_pool == NULL) {
        apr_initialize();
	apr_pool_create(&global_apr_pool, NULL); 
    }
        g_php_fix_pathinfo_enable = get_php_fix_pathinfo_enable(s);
}

int jaxer_conn_is_https(conn_rec *c)
{
    return 0;
}

static int jaxer_post_read(request_rec *r)
{
    /* A lot of the code requires APR, which means we need an APR pool,
     * but this is Apache 1.3.  So, for each new request, we simply create
     * an APR pool, and put it into the per_dir config area for mod_jaxer.
     */
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(r->per_dir_config, &jaxer_module);

    ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "in jaxer_post_read");

    apr_pool_create(&config->reqPool, global_apr_pool);
    ap_register_cleanup(r->pool, config->reqPool, apr_pool_destroy,
                        apr_pool_destroy);
    return OK;
}

static int jaxer_fixups(request_rec *r)
{
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(r->per_dir_config, &jaxer_module);
    jaxer_server_conf *sconfig =
        ap_get_module_config(r->server->module_config, &jaxer_module);
    char *ext;

    ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "in jaxer_fixups");

    if (config->is_pass_through) {
        ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "Skipping request because PassThrough is "
                      "configured for %s", r->uri);
        return DECLINED;
    }
    if (r->main != NULL) {
        return DECLINED;
    }
if (r->handler)
{
fprintf(stderr, "HANDLER=%s\n", r->handler);
}else
{
fprintf(stderr, "HANDLER is null\n");
}

    if (r->handler && !strcmp(r->handler, JAXER_HANDLER))
    {
    ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "in jaxer_fixups: is handler");
        return OK;
    }

if (r->uri)
{
fprintf(stderr, "uri=%s\n", r->uri);
}else
{
fprintf(stderr, "uri is null\n");
}
    // check to see if we are a filter
    // TEST
    ext = strrchr(r->uri, '.');
    if (ext && !strcmp(ext, ".html"))
    {
    ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "in jaxer_fixups: is filter");
        r->handler = JAXER_FILTER;
        return OK;
    }
    return DECLINED;

#if 0
    
    if (!strcmp(r->uri, sconfig->callback_uri)) {
        ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "in jaxer_fixups: is handler");
        r->handler = JAXER_HANDLER;
    }
    ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "in jaxer_fixups: is filter");
    r->handler = JAXER_FILTER;
    return OK;
#endif
}

static void merge_request_response(request_rec *r, request_rec *subreq)
{
    ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "in merge_request_response");
    ap_overlap_tables(r->headers_out, subreq->headers_out, 
                      AP_OVERLAP_TABLES_MERGE);

    r->status_line = ap_pstrdup(r->pool, subreq->status_line);
    r->status = subreq->status;
    r->content_type = ap_pstrdup(r->pool, subreq->content_type);
}

static void jaxer_add_callback_uri(request_rec * r)
{
    jaxer_server_conf *config =
        ap_get_module_config(r->server->module_config, &jaxer_module);

    if (config->callback_uri)
    {
        table *e = r->subprocess_env;
        ap_table_setn(e, "CALLBACK_URI", config->callback_uri);
    }
}

static void jaxer_add_cgi_vars(request_rec * r)
{
        jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(r->per_dir_config, &jaxer_module);

        /* Work around cgi.fix_pathinfo = 1 in php.ini */
        if (g_php_fix_pathinfo_enable) {
                char *merge_path;
                table *e = r->subprocess_env;

                /* "DOCUMENT_ROOT"/"SCRIPT_NAME" -> "SCRIPT_NAME" */
                const char *doc_root = ap_table_get(e, "DOCUMENT_ROOT");
                const char *script_name = ap_table_get(e, "SCRIPT_NAME");

                if (doc_root && script_name
                        && apr_filepath_merge(&merge_path, doc_root, script_name, 0,
                                              config->reqPool) == APR_SUCCESS) {
                        ap_table_setn(e, "SCRIPT_NAME", merge_path);
                }
        }
}

static int jaxer_handler(request_rec *r)
{
    jaxer_dir_conf *config = ap_get_module_config(r->per_dir_config, &jaxer_module);
    ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "in jaxer_handler");
    if (config->is_pass_through) {
        return DECLINED;
    }
    jaxer_add_callback_uri(r);
    ap_add_common_vars(r);
    ap_add_cgi_vars(r);
    jaxer_add_cgi_vars(r);
    jaxer_add_env_vars(r);

    return bridge_request(r, FCGI_RESPONDER);
}

static void initialize_child(server_rec *s, pool *pchild)
{
	
	apr_status_t rv = APR_SUCCESS;

	jaxer_server_conf *config; 

	jaxer_worker *w;
	apr_hash_t *wh; 
	apr_hash_index_t *hi;
    server_rec *t;

    ap_log_perror(APLOG_MARK, APLOG_INFO, 0, pchild, "in initialize_child");

    for (t=s; t; t=t->next)
    {
        config = ap_get_module_config(t->module_config, &jaxer_module);
        wh = config->worker_hash;

        if (t->is_virtual)
        {
            ap_log_perror(APLOG_MARK, APLOG_NOTICE, rv, pchild,
                "mod_jaxer: initialize_child initializing virtual server:host=%s(line %d @%s)",
                t->server_hostname, t->defn_line_number, t->defn_name);
        }else
        {
            ap_log_perror(APLOG_MARK, APLOG_NOTICE, rv, pchild,
                "mod_jaxer: initialize_child initializing server:host=%s(line %d @%s)",
                t->server_hostname, t->defn_line_number, t->defn_name);
        }

	
	    for (hi = apr_hash_first(global_apr_pool, wh); hi; hi = apr_hash_next(hi))
	    {
    		
		    apr_hash_this(hi, NULL, NULL, (void **)&w);

		    ap_log_perror(APLOG_MARK, APLOG_NOTICE, rv, pchild,
						      "mod_jaxer: initialize_child initializing worker %s(%s,%d)", w->name, w->hostname, w->port_number);

                /* It is ok to use the global_apr_pool here, because all we are
                 * going to do is create sub-pools with it.
                 */
	        if (APR_SUCCESS != (rv=jxr_init_worker(w, global_apr_pool, config->min_connections, config->max_connections, config->acquire_conn_timeout)))
		    {
			    ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, pchild,
						      "mod_jaxer: jxr_init_worker failed for %s", w->name);
		    }
	    }
    }
	
}

static int jaxer_filter_handler(request_rec *r)
{
    request_rec *subreq;
    BUFF *subBuff, *original;
    FILE *fileIn = NULL, *fileOut = NULL;
    int rc;
    int len_read = 0;
    int remaining = 0;
    char buffer[HUGE_STRING_LEN];
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(r->per_dir_config, &jaxer_module);

    ap_log_rerror(APLOG_MARK, APLOG_INFO, r, "in jaxer_filter_handler");

    config->fnameIn = ap_psprintf(r->pool, "/tmp/injaxer.%d", r->connection->child_num);
    config->fnameOut = ap_psprintf(r->pool, "/tmp/outjaxer.%d", r->connection->child_num);
    fileIn = ap_pfopen(r->pool, config->fnameIn, "w");
    if (fileIn == NULL) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, r, "Could not open file %s",
                      config->fnameIn);
    }
    fileOut = ap_pfopen(r->pool, config->fnameOut, "w");
    if (fileOut == NULL) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, r, "Could not open file %s",
                      config->fnameOut);
    }

    if ((rc = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)) != OK) {
        return rc;
    }
    remaining = r->remaining;
    if (ap_should_client_block(r)) {
        ap_hard_timeout("client_read", r);
        while ((len_read = ap_get_client_block(r, buffer, HUGE_STRING_LEN)) > 0) {
            ap_reset_timeout(r);
            fwrite(buffer, len_read, 1, fileIn);
        }
        ap_kill_timeout(r);
    }
    rewind(fileIn);

    subBuff = ap_bcreate(r->pool, B_RDWR);
    ap_bpushfd(subBuff, fileno(fileIn), fileno(fileOut));
    original = r->connection->client;

    subreq = ap_sub_req_method_uri((char *)r->method, r->uri, r);
    reset_request(subreq, remaining);

    subreq->connection->client = subBuff;
    ap_bsetflag(subreq->connection->client, B_CHUNK, 0);
    ap_run_sub_req(subreq);
    merge_request_response(r, subreq);
    ap_bflush(subBuff);

    ap_destroy_sub_req(subreq);
    unlink(config->fnameIn);

    r->connection->client = original;
    fileOut = ap_pfopen(r->pool, config->fnameOut, "a+");
    if (fileOut == NULL) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, r, "Could not open file %s",
                      config->fnameOut);
    }
    rc = jaxer_out_filter(r);
    unlink(config->fnameOut);
    return rc;
}

static const command_rec jaxer_cmds[] =
{
    {"JaxerPassThrough", set_pass_through, NULL, ACCESS_CONF, FLAG, 
     "Don't send this content through the Jaxer server."},
    {"DefaultInitEnv", add_default_env_vars, NULL, RSRC_CONF, TAKE12,
     "an environment variable name and optional value to pass to Jaxer Server process"},
    {"JaxerWorker", set_jaxer_worker, NULL, RSRC_CONF, TAKE3, 
     "Jaxer worker"},
    {"CallbackURI", set_callback_uri, NULL, RSRC_CONF, TAKE1,
     "JaxerServer process Callback URI path"},
    {"JaxerDumpNetworkData", set_network_dump, NULL, RSRC_CONF, FLAG,
     "Turn on mod_jaxer-jaxer network dumping"},
    {NULL}
};

static const handler_rec jaxer_handlers[] =
{
    {JAXER_HANDLER, jaxer_handler},
    {JAXER_FILTER, jaxer_filter_handler},
    {NULL}
};

module MODULE_VAR_EXPORT jaxer_module =
{
    STANDARD_MODULE_STUFF,
    jaxer_initializer,          /* initializer */
    create_jaxer_dir_config,    /* dir config creater */
    merge_jaxer_dir_config,     /* dir merger --- default is to override */
    create_jaxer_server_config, /* server config */
    merge_jaxer_server_config,  /* merge server config */
    jaxer_cmds,                 /* command table */
    jaxer_handlers,             /* handlers */
    NULL,                       /* filename translation */
    NULL,                       /* check_user_id */
    NULL,                       /* check auth */
    NULL,                       /* check access */
    NULL,                       /* type_checker */
    jaxer_fixups,              	/* fixups */
    NULL,                       /* logger */
    NULL,                       /* header parser */
    initialize_child,           /* child_init */
    NULL,                       /* child_exit */
    jaxer_post_read             /* post read-request */
};

