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
#include "apr_strings.h"
#include "apr_hash.h"
#include "http_main.h"
#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "apr_lib.h"

#include "mod_jaxer_conf.h"
extern module AP_MODULE_DECLARE_DATA jaxer_module;

// Whether we should do network dumping
//int g_jxr_network_trace = 0;

#define DEFAULT_CALLBACK_URI				"/aptanaRPC"
#define DEFAULT_WORKER_HOSTNAME				"127.0.0.1"
#define DEFAULT_WORKER_PORT_NUMBER			4327

#define DEFAULT_MIN_CONNECTIONS				-1
#define DEFAULT_MAX_CONNECTIONS				-1
#define DEFAULT_CONN_EXPIRE_TIME			0
#define DEFAULT_ACQUIRE_CONN_TIMEOUT		5
#define DEFAULT_MAX_POSTDATA_MEMORY         4096

static apr_status_t jaxer_server_config_cleanup(void* c)
{
	jaxer_server_conf *config = (jaxer_server_conf*) c;
	apr_status_t rv = APR_SUCCESS;
#pragma message(" need to rewrite")

#if 0
	jaxer_worker *w;
	apr_hash_t *wh;
	apr_hash_index_t *hi;
	apr_pool_t *p = config->pool;
	jaxer_connection *ac;
	int i;
	
	if (!config)
		return rv;


	for(hi = apr_hash_first(p, wh); hi; hi = apr_hash_next(hi))
	{
		apr_hash_this(hi, NULL, NULL, (void **)&w);

 		for(i=0;i<w->cache_size; i++)
		{
			ac = &w->ac_cache[i];
			if (ac->sock)
			{
				apr_socket_close(ac->sock);
				ac->sock = 0;
			}
		}
	}
#endif

	return rv;
}

void *create_jaxer_server_config(apr_pool_t * p, server_rec * s)
{
	jaxer_server_conf *config = apr_pcalloc(p, sizeof(*config));
	apr_status_t rv;

    config->set_flags = 0;

	config->default_init_env = apr_table_make(p, 20);
	
	config->php_fix_pathinfo_enable = 0;
	config->callback_uri = apr_pstrdup(p, DEFAULT_CALLBACK_URI);

#if 0
	config->worker_hash = apr_hash_make(p);
	if (!config->worker_hash)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, 0, p,
						  "mod_jaxer: create_jaxer_server_config failed to create worker list");
		return 0;
	}
#endif

	if ((rv = apr_pool_create(&config->pool, p)) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT | APLOG_NOERRNO, rv, p,
						  "mod_jaxer: create_jaxer_server_config failed to create subpool");
		return 0;
	}

	config->min_connections = DEFAULT_MIN_CONNECTIONS;
	config->max_connections = DEFAULT_MAX_CONNECTIONS;
	config->keep_connections = config->max_connections;
	config->acquire_conn_timeout = DEFAULT_ACQUIRE_CONN_TIMEOUT;
	config->conn_expire_time = DEFAULT_CONN_EXPIRE_TIME;
    config->max_postdata_memory = DEFAULT_MAX_POSTDATA_MEMORY;

    config->real_worker = 0;

	config->worker = (jaxer_worker*) apr_pcalloc(p, sizeof(jaxer_worker));
	if ( config->worker == 0)
    {
		ap_log_perror(APLOG_MARK, APLOG_CRIT | APLOG_NOERRNO, rv, p,
						  "mod_jaxer: create_jaxer_server_config failed to create worker");
        return 0;
    }
    config->worker->port_number = DEFAULT_WORKER_PORT_NUMBER;
	config->worker->hostname = apr_pstrdup(p, DEFAULT_WORKER_HOSTNAME);
    config->worker->server = s;
	config->worker->my_proto = MOD_JAXER_PROTOCOL_VERSION;
	config->worker->use_proto = MOD_JAXER_PROTOCOL_VERSION;
	config->worker->retry  = 0;

	config->network_dump = 0;
	// apr_pool_cleanup_register(p, config, jaxer_server_config_cleanup, apr_pool_cleanup_null);

	return config;
}

void *merge_jaxer_server_config(apr_pool_t * p, void *basev,
								void *overridesv)
{
	int i;
	jaxer_server_conf *base = (jaxer_server_conf *) basev;
	jaxer_server_conf *overrides = (jaxer_server_conf *) overridesv;

#if 0
	apr_hash_t *hbase = base->worker_hash;
	apr_hash_t *hover = overrides->worker_hash;
#endif

	/* Merge environment variables */
	const apr_array_header_t *baseenv_array =
		apr_table_elts(base->default_init_env);

	const apr_table_entry_t *baseenv_entry =
		(apr_table_entry_t *) baseenv_array->elts;

	for (i = 0; i < baseenv_array->nelts; ++i)
	{
		if (apr_table_get(overrides->default_init_env, baseenv_entry[i].key))
			continue;
		apr_table_set(overrides->default_init_env, baseenv_entry[i].key,
					  baseenv_entry[i].val);
	}

    //int php_fix_pathinfo_enable;
    if (!(overrides->set_flags & JSC_PHP_FIX_PATHINFO_ENABLE_SET))
        overrides->php_fix_pathinfo_enable = base->php_fix_pathinfo_enable;

    //char *callback_uri;
    if (!(overrides->set_flags & JSC_CALLBACK_URI_SET) && (base->set_flags & JSC_CALLBACK_URI_SET))
        overrides->callback_uri = apr_pstrdup(p, base->callback_uri);

    //jaxer_worker
    if(!(overrides->set_flags & JSC_WORKER_SET) && (base->set_flags & JSC_WORKER_SET))
    {
        overrides->worker->port_number = base->worker->port_number;
        overrides->worker->hostname = apr_pstrdup(p, base->worker->hostname);
    }

    if(!(overrides->set_flags & JSC_MIN_CONNECTIONS_SET))
        overrides->min_connections = base->min_connections;

    if(!(overrides->set_flags & JSC_MAX_CONNECTIONS_SET))
        overrides->max_connections = base->max_connections;

    if(!(overrides->set_flags & JSC_KEEP_CONNECTIONS_SET))
        overrides->keep_connections = base->keep_connections;

    if(!(overrides->set_flags & JSC_MAX_POSTDATA_MEMORY_SET))
        overrides->max_postdata_memory = base->max_postdata_memory;

    if(!(overrides->set_flags & JSC_ACQUIRE_CONN_TIMEOUT_SET))
        overrides->acquire_conn_timeout = base->acquire_conn_timeout;

    if(!(overrides->set_flags & JSC_CONN_EXPIRE_TIME_SET))
        overrides->conn_expire_time = base->conn_expire_time;

    if(!(overrides->set_flags & JSC_NETWORK_DUMP_SET))
        overrides->network_dump = base->network_dump;

	return overridesv;
}


void *create_jaxer_dir_config(apr_pool_t * p, char *dummy)
{
	jaxer_dir_conf *config = apr_pcalloc(p, sizeof(jaxer_dir_conf));


	config->filter_ext_hash = apr_hash_make(p);
    config->filter_ct_hash = apr_hash_make(p);
    config->filter_bypass_handler_hash = apr_hash_make(p);
    config->is_pass_through = 0;
    config->set_flags = 0;

	return (void *) config;
}

void *merge_jaxer_dir_config(apr_pool_t * p, void *basev,
								void *overridesv)
{
	jaxer_dir_conf *base = (jaxer_dir_conf *) basev;
	jaxer_dir_conf *overrides = (jaxer_dir_conf *) overridesv;

	apr_hash_t *hbase = base->filter_ext_hash;
	apr_hash_t *hover = overrides->filter_ext_hash;
	
    apr_hash_t *hctbase = base->filter_ct_hash;
	apr_hash_t *hctover = overrides->filter_ct_hash;
	
    apr_hash_t *hhbase = base->filter_bypass_handler_hash;
	apr_hash_t *hhover = overrides->filter_bypass_handler_hash;
	
    apr_hash_index_t *hi;
	const void *ename;
	void* w;
	apr_ssize_t len;

    // should we use the flag?
    if (base->is_pass_through)
    {
        overrides->is_pass_through = 1;
        return overridesv;
    }

	/*
	 * Add base to override if entry is not there.
	 * Do not override if already exists
	 */
	for(hi=apr_hash_first(p, hbase); hi; hi=apr_hash_next(hi))
	{
		apr_hash_this(hi, &ename, &len, &w);
		if (!apr_hash_get(hover, ename, len))
			apr_hash_set(hover, ename, len, w);
	}

    for(hi=apr_hash_first(p, hctbase); hi; hi=apr_hash_next(hi))
	{
		apr_hash_this(hi, &ename, &len, &w);
		if (!apr_hash_get(hctover, ename, len))
			apr_hash_set(hctover, ename, len, w);
	}

    for(hi=apr_hash_first(p, hhbase); hi; hi=apr_hash_next(hi))
	{
		apr_hash_this(hi, &ename, &len, &w);
		if (!apr_hash_get(hhover, ename, len))
			apr_hash_set(hhover, ename, len, w);
	}

	return overridesv;
}

const char *add_default_env_vars(cmd_parms * cmd, void *dummy,
								 const char *name, const char *value)
{
	jaxer_server_conf *config =
		ap_get_module_config(cmd->server->module_config,
							 &jaxer_module);

	apr_table_set(config->default_init_env, name, value ? value : "");
	return NULL;
}

apr_table_t *get_default_env_vars(request_rec * r)
{
	jaxer_server_conf *config =
		ap_get_module_config(r->server->module_config, &jaxer_module);
	return config->default_init_env;
}


const char *set_callback_uri(cmd_parms * cmd, void *dummy, const char *arg)
{
	server_rec *s = cmd->server;
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);
	config->callback_uri = apr_pstrdup(cmd->pool, arg);
	if (!config->callback_uri)
		return "Invalid Callback URI";

    config->set_flags |= JSC_CALLBACK_URI_SET;

	return NULL;
}

const char* set_min_connection(cmd_parms * cmd, void *dummy, const char *num)
{
	server_rec *s = cmd->server;
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);

	if (!num || !apr_isdigit(num[0]))
		return "MinConnectionPoolSize: Number must be numeric";

	config->min_connections = atoi(num);
	if (config->min_connections < 0)
		return "MinConnectionPoolSize: Invalid value (value must be non-negative)";

    config->set_flags |= JSC_MIN_CONNECTIONS_SET;
	return NULL;
}

const char* set_max_connection(cmd_parms * cmd, void *dummy, const char *num)
{
	server_rec *s = cmd->server;
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);

	if (!num || !apr_isdigit(num[0]))
		return "MaxConnectionPoolSize: Number must be numeric";

	config->max_connections = atoi(num);
	if (config->max_connections <= 0)
		return "MaxConnectionPoolSize: Invalid value (value must be positive)";

	config->keep_connections = config->min_connections;
    config->set_flags |= JSC_MAX_CONNECTIONS_SET;
    config->set_flags |= JSC_KEEP_CONNECTIONS_SET;

	return NULL;
}

const char* set_postdata_memory_limit(cmd_parms * cmd, void *dummy, const char *num)
{
	server_rec *s = cmd->server;
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);

	if (!num || !apr_isdigit(num[0]))
		return "PostDataMemoryLimit: Value must be numeric";

	config->max_postdata_memory = atoi(num);
	if (config->max_postdata_memory < 0)
		return "PostDataMemoryLimit: Invalid value (value cannot be negative)";

    config->set_flags |= JSC_MAX_POSTDATA_MEMORY_SET;
	return NULL;
}

const char* set_wait_connection_timeout(cmd_parms * cmd, void *dummy, const char *num)
{
	server_rec *s = cmd->server;
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);

	if (!num || !apr_isdigit(num[0]))
		return "WaitConnectionTimeOutSec: Number must be numeric";

	// Given in seconds
	config->acquire_conn_timeout = atoi(num);
	if (config->acquire_conn_timeout < 0)
		return "WaitConnectionTimeOutSec: Invalid value (value must be non-negative)";

    config->set_flags |= JSC_ACQUIRE_CONN_TIMEOUT_SET;
	return NULL;
}

const char* set_jaxer_worker(cmd_parms * cmd, void *dummy, const char *host, const char *port)
{
	/*
	 * Each node will have info about one worker.
	 */
	server_rec *s = cmd->server;
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);
    jaxer_worker *worker = config->worker;

	worker->hostname = apr_pstrdup(cmd->pool, host);
	if (!worker->hostname)
	{
		return "JaxerWorker: Invalid worker hostname";
	}

	if (!port || !apr_isdigit(port[0]))
		return "JaxerWorker: Port number must be numeric";

	worker->port_number = atoi(port);
	if (worker->port_number <= 0)
	{
		return "JaxerWorker: Invalid worker port number";
	}
	
    worker->destroyed = 0;
	//apr_hash_set( config->worker_hash, worker->name, strlen(worker->name), (void*) worker);

    config->set_flags |= JSC_WORKER_SET;

	return NULL;
}

jaxer_worker* get_jaxer_worker(server_rec * s)
{
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);
    return config->real_worker;
}

const char *set_filter_config(cmd_parms * cmd, void *dirconfig,
							   const char *extension)
{
	jaxer_dir_conf *config = (jaxer_dir_conf *) dirconfig;
	char *ext;
    const char *p = extension;

    if (!p)
        return "Invalid filter extension";

    if (*p == '.') p++;

    /* Sanity check */
	if (*p == '\0' || strchr(p, '/') || strchr(p, '\\'))
		return "Invalid filter extension - it cannot be null and connot contain '/' or '\\'.";

    if (*p == '*' && strlen(p) != 1)
        return "Invalid filter extension";

    ext = apr_pstrdup(cmd->server->process->pconf, p);
	ap_str_tolower(ext);
	/* Add the node now */
	apr_hash_set(config->filter_ext_hash, ext, strlen(ext),
				 apr_pstrdup(cmd->server->process->pconf, "worker"));

	return NULL;
}

const char *set_filter_config_ct(cmd_parms * cmd, void *dirconfig,
							   const char *content_type)
{
	jaxer_dir_conf *config = (jaxer_dir_conf *) dirconfig;
	char *ct;

    /* Sanity check */
	if (content_type == NULL || *content_type  == '\0')
		return "Invalid filter content_type";

	ct = apr_pstrdup(cmd->server->process->pconf, content_type);
	ap_str_tolower(ct);
	/* Add the node now */
	apr_hash_set(config->filter_ct_hash, ct, strlen(ct),
				 apr_pstrdup(cmd->server->process->pconf, "worker"));

	return NULL;
}

const char *set_filter_config_bypass_handler(cmd_parms * cmd, void *dirconfig,
							   const char *handler)
{
	jaxer_dir_conf *config = (jaxer_dir_conf *) dirconfig;
	char *ct;

    /* Sanity check */
	if (handler == NULL || *handler  == '\0')
		return "Invalid handler";

	ct = apr_pstrdup(cmd->server->process->pconf, handler);
	ap_str_tolower(ct);
	/* Add the node now */
	apr_hash_set(config->filter_bypass_handler_hash, ct, strlen(ct),
				 apr_pstrdup(cmd->server->process->pconf, "worker"));

	return NULL;
}

int should_filter_content_type(const char *content_type, request_rec * r)
{
	char *ct;
    char *p;
	jaxer_dir_conf *config =
		ap_get_module_config(r->per_dir_config, &jaxer_module);

    if (config->is_pass_through)
        return 0;

	ct = apr_pstrdup(r->pool, content_type);
	ap_str_tolower(ct);
    if ((p=strchr(ct, ';')) != NULL)
        *p = 0;

	/* Search content_type in per_dir_config */
	if (config && (apr_hash_get(config->filter_ct_hash, ct, strlen(ct))))
		return 1;

	return 0;
}

int should_bypass_handler(const char *handler, request_rec * r)
{
	char *ct;
	jaxer_dir_conf *config =
		ap_get_module_config(r->per_dir_config, &jaxer_module);

    if (config->is_pass_through)
        return 1;

	ct = apr_pstrdup(r->pool, handler);
	ap_str_tolower(ct);
	/* Search handler in per_dir_config */
	if (config && (apr_hash_get(config->filter_bypass_handler_hash, ct,
						 strlen(ct))))
		return 1;

	return 0;
}

int should_filter_uri(const char *cgipath, request_rec * r)
{
	char *extension;
	char *ext;
	jaxer_dir_conf *config =
		ap_get_module_config(r->per_dir_config, &jaxer_module);

    if (config->is_pass_through)
        return 0;

    //If it is configured for all extensions, then just get the worker
    if ((apr_hash_get(config->filter_ext_hash, "*",1)))
        return 1;

	/* Get file name extension */
	extension = ap_strrchr_c(cgipath, '.');
	if (extension == NULL || strlen(extension) <= 1)
        return 0;

    // pass the dot
    extension++;

	ext = apr_pstrdup(r->pool, extension);
	ap_str_tolower(ext);
	/* Search file name extension in per_dir_config */
	if (config && (apr_hash_get(config->filter_ext_hash, ext, strlen(ext))))
		return 1;

	return 0;
}

int get_is_pass_through(request_rec* r)
{
    jaxer_dir_conf *config =
		ap_get_module_config(r->per_dir_config, &jaxer_module);

    return (config->is_pass_through) ? 1 : 0;
}

int get_php_fix_pathinfo_enable(server_rec * s)
{
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);
	return config ? config->php_fix_pathinfo_enable : 0;
}

const char *set_pass_through(cmd_parms * cmd, void *dirconfig, const char* arg)
{
	jaxer_dir_conf *config = (jaxer_dir_conf *) dirconfig;

    if (strcmp(arg, "on") && strcmp(arg, "off"))
		return "Invalid value for JaxerPassThrough.  Value must be on or off.";
    if (strcmp(arg, "on") == 0)
        config->is_pass_through = 1;
    else
        config->is_pass_through = 0;

    config->set_flags |= JDC_IS_PASS_THROUGH_SET;
	return NULL;
}

const char* set_network_dump(cmd_parms * cmd, void *dummy, const char* arg)
{
	server_rec *s = cmd->server;
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);

    if (strcmp(arg, "on") && strcmp(arg, "off"))
		return "Invalid value for JaxerDumpNetworkData.  Value must be on or off.";
    if (strcmp(arg, "on") == 0)
        config->network_dump = 1;
    else
        config->network_dump = 0;

    config->set_flags |= JSC_NETWORK_DUMP_SET;

    // Can we just use a global flag?
    //g_jxr_network_trace = config->network_dump;
	return NULL;
}

int get_network_dump(server_rec * s)
{
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);
	return config ? config->network_dump : 0;
}
