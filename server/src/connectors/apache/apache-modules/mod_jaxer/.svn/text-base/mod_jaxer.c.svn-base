/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set sw=4 ts=4 et: */
/* ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 * 
 *  This program is distributed in the hope that it will be useful, but
 *  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 *  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 *  is prohibited.
 * 
 *  You can redistribute and/or modify this program under the terms of the GPL, 
 *  as published by the Free Software Foundation.  You should
 *  have received a copy of the GNU General Public License, Version 3 along
 *  with this program; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *  
 *  Aptana provides a special exception to allow redistribution of this file
 *  with certain other code and certain additional terms
 *  pursuant to Section 7 of the GPL. You may view the exception and these
 *  terms on the web at http://www.aptana.com/legal/gpl/.
 *  
 *  You may view the GPL, and Aptana's exception and additional terms in the file
 *  titled license-jaxer.html in the main distribution folder of this program.
 *  
 *  Any modifications to this file must keep this entire header intact.
 * 
 * ***** END LICENSE BLOCK ***** */
#include "ap_config.h"
#include "ap_mmn.h"
#include "httpd.h"
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
#include "mod_jaxer_filter.h"
#include "mod_jaxer_protocol.h"
#include "mod_jaxer_connection.h"
#include "../../../../mozilla/aptana/jaxerBuildId.h"

module AP_MODULE_DECLARE_DATA jaxer_module;

#define	MODULE_VERSION	"ModJaxer/" JAXER_BUILD_ID

static ap_filter_rec_t *jaxer_in_filter_handle;
static ap_filter_rec_t *jaxer_out_filter_handle;
static int g_php_fix_pathinfo_enable = 0;

APR_DECLARE_OPTIONAL_FN(int, ssl_is_https, (conn_rec *));
static APR_OPTIONAL_FN_TYPE(ssl_is_https) *conn_is_https = NULL;

int jaxer_conn_is_https(conn_rec *c)
{
    if (conn_is_https) {
        return conn_is_https(c);
    }
    else
        return 0;
}

static void jaxer_add_cgi_vars(request_rec * r)
{
	/* Work around cgi.fix_pathinfo = 1 in php.ini */
	if (g_php_fix_pathinfo_enable) {
		char *merge_path;
		apr_table_t *e = r->subprocess_env;

		/* "DOCUMENT_ROOT"/"SCRIPT_NAME" -> "SCRIPT_NAME" */
		const char *doc_root = apr_table_get(e, "DOCUMENT_ROOT");
		const char *script_name = apr_table_get(e, "SCRIPT_NAME");

		if (doc_root && script_name
			&& apr_filepath_merge(&merge_path, doc_root, script_name, 0,
								  r->pool) == APR_SUCCESS) {
			apr_table_setn(e, "SCRIPT_NAME", merge_path);
		}
	}
}

static void jaxer_add_callback_uri(request_rec * r)
{
	jaxer_server_conf *config =
		ap_get_module_config(r->server->module_config, &jaxer_module);

	if (config->callback_uri) 
	{
		apr_table_t *e = r->subprocess_env;
		
		apr_table_setn(e, "CALLBACK_URI", config->callback_uri);
	}
}

static int add_filters(request_rec *r)
{
    if (should_filter_uri(r->uri, r))
	{
		/* Yes, add filters.
         * "Referer" is used for DirectoryIndex until I have a better way.
         */
        
        if (r->method_number == M_POST || r->method_number == M_PUT || apr_table_get(r->headers_in, "Referer"))
			ap_add_input_filter_handle(jaxer_in_filter_handle, NULL, r, r->connection);
		ap_add_output_filter_handle(jaxer_out_filter_handle, NULL, r, r->connection);
	}

	return OK;
}


static int jaxer_handler(request_rec * r)
{
	int http_retcode;
    
    /* decline if this is passthrough area */
    jaxer_dir_conf *config =
		ap_get_module_config(r->per_dir_config, &jaxer_module);
    if (config->is_pass_through)
        return DECLINED;
	
	jaxer_add_callback_uri(r);

    
    if (strcmp(r->handler, "jaxer"))
		return DECLINED;

    if (is_apache_dummy_request(r))
        return DECLINED;

	ap_add_common_vars(r);
	ap_add_cgi_vars(r);
	apr_table_unset(r->subprocess_env, "GATEWAY_INTERFACE");
	jaxer_add_cgi_vars(r);
	jaxer_add_env_vars(r);

    apr_table_setn(r->subprocess_env, "JAXER_REQ_TYPE", apr_pstrdup(r->pool, "1"));


	http_retcode = jxr_do_handler_request(r);
	
	return (http_retcode == HTTP_OK ? OK : http_retcode);
}

static int is_same_worker(jaxer_worker* a, jaxer_worker* b)
{
    return (strcasecmp(a->hostname, b->hostname) == 0 && a->port_number == b->port_number);
}

static void initialize_child(apr_pool_t * pchild, server_rec * s)
{
	
	apr_status_t rv = APR_SUCCESS;

	jaxer_server_conf *config; 
	jaxer_server_conf *dconfig; 

    server_rec *t;
    server_rec *d;

    for (t=s; t; t=t->next)
    {
        config = ap_get_module_config(t->module_config, &jaxer_module);

        for (d=s; d!=t; d=d->next)
        {
            dconfig = ap_get_module_config(d->module_config, &jaxer_module);
            if ( dconfig->real_worker && ((!d->is_virtual && (config->set_flags & JSC_WORKER_SET) == 0) || is_same_worker(dconfig->worker, config->worker)))
            {
                config->real_worker = dconfig->real_worker;
                if (t->is_virtual)
                {
                    ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, pchild,
                        "mod_jaxer: initialize_child: virtual server:host=%s(line %d @%s) sharing connection pool with server:host=%s(line %d @%s)",
                        t->server_hostname, t->defn_line_number, t->defn_name,
                        d->server_hostname, d->defn_line_number, d->defn_name);
                }else
                {
                    ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, pchild,
                        "mod_jaxer: initialize_child: server:host=%s(line %d @%s) sharing connection pool with server:host=%s(line %d @%s)",
                        t->server_hostname, t->defn_line_number, t->defn_name,
                        d->server_hostname, d->defn_line_number, d->defn_name);
                }
            break;
            }
            d = d->next;
        }
        if (d==t)
        {
            if (t->is_virtual)
            {
                ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, pchild,
                    "mod_jaxer: initialize_child: creating connection pool for virtual server:host=%s(line %d @%s)",
                    t->server_hostname, t->defn_line_number, t->defn_name);
            }else
            {
                ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, pchild,
                    "mod_jaxer: initialize_child: creating connection pool for server:host=%s(line %d @%s)",
                    t->server_hostname, t->defn_line_number, t->defn_name);
            }
	        if (APR_SUCCESS != (rv=jxr_init_worker(config->worker, pchild, config->min_connections, config->max_connections, config->acquire_conn_timeout)))
		    {
                ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, pchild, "mod_jaxer: jxr_init_worker failed");
		    }
            config->real_worker = config->worker;
        }
    }
}

static int
jaxer_init(apr_pool_t * config_pool, apr_pool_t * plog, apr_pool_t * ptemp,
		   server_rec * s)
{
	apr_status_t rv = APR_SUCCESS;
	
	// Make sure we have at least one worker
	jaxer_server_conf *config = ap_get_module_config(s->module_config, &jaxer_module);
	//apr_hash_t *wh = config->worker_hash;

    // SSL
    conn_is_https = APR_RETRIEVE_OPTIONAL_FN(ssl_is_https);

    ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, plog, "%s: Initialization.", MODULE_VERSION);

	g_php_fix_pathinfo_enable = get_php_fix_pathinfo_enable(s);

    	
	ap_add_version_component(config_pool, MODULE_VERSION);

	return rv;
}

static const command_rec mod_jaxer_cmds[] = {
	AP_INIT_TAKE12("DefaultInitEnv", 				add_default_env_vars, 			NULL, RSRC_CONF,	"an environment variable name and optional value to pass to JaxerServer process"),
	AP_INIT_TAKE2("JaxerWorker",					set_jaxer_worker,				NULL, RSRC_CONF,	"hostname port"),
	AP_INIT_TAKE1("CallbackURI", 					set_callback_uri, 				NULL, RSRC_CONF,	"JaxerServer process Callback URI path"),
	AP_INIT_TAKE1("MinConnectionPoolSize", 			set_min_connection,				NULL, RSRC_CONF,	"Minimum connection pool size"),
	AP_INIT_TAKE1("MaxConnectionPoolSize", 			set_max_connection,				NULL, RSRC_CONF,	"Maximum connection pool size"),
	AP_INIT_TAKE1("WaitConnectionTimeOutSec",		set_wait_connection_timeout,	NULL, RSRC_CONF,	"Time to wait for a connection before giving up (sec)"),
	AP_INIT_TAKE1("JaxerPassThrough",               set_pass_through,               NULL, ACCESS_CONF,  "on or off"),
	AP_INIT_TAKE1("JaxerDumpNetworkData", 	        set_network_dump, 	            NULL, RSRC_CONF,	"on or off"),
	AP_INIT_ITERATE("JaxerFilter",                  set_filter_config,              NULL, OR_LIMIT,	    "one or more extensions"),
    AP_INIT_ITERATE("JaxerFilterContentType", 		set_filter_config_ct, 	        NULL, OR_LIMIT,	    "one or more content_types"),
    AP_INIT_ITERATE("JaxerDoNotFilterContentFromHandler", 
                                                    set_filter_config_bypass_handler,
                                                                                    NULL, OR_LIMIT,	    "one or more handlers."),
    AP_INIT_TAKE1("PostDataMemoryLimit", 			set_postdata_memory_limit,		NULL, RSRC_CONF,	"Memory limit for postdata before put into disk file (in bytes)"),
	
    {NULL}
};

static void register_hooks(apr_pool_t * p)
{
	ap_hook_post_config(jaxer_init, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_child_init(initialize_child, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_fixups(add_filters, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_handler(jaxer_handler, NULL, NULL, APR_HOOK_FIRST);

	/* Insert the Jaxer output filter */
	jaxer_in_filter_handle = ap_register_input_filter("JAXERINPUT", jaxer_in_filter, NULL, AP_FTYPE_CONTENT_SET);
	jaxer_out_filter_handle = ap_register_output_filter("JAXEROUTPUT", jaxer_out_filter, NULL, AP_FTYPE_CONTENT_SET-1);
}

module AP_MODULE_DECLARE_DATA jaxer_module = {
	STANDARD20_MODULE_STUFF,
	create_jaxer_dir_config,		/* create per-directory config structure */
	merge_jaxer_dir_config,		    /* merge per-directory config structures */
	create_jaxer_server_config,	    /* create per-server config structure */
	merge_jaxer_server_config,		/* merge per-server config structures */
	mod_jaxer_cmds, 				/* command apr_table_t */
	register_hooks					/* register hooks */
};

