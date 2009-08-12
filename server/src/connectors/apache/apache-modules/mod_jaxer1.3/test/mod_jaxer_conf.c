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
#include "httpd.h"
#include "http_main.h"
#include "http_config.h"
#include "http_log.h"
#include "apr_lib.h"

#include "compat.h"
#include "mod_jaxer_conf.h"

// Whether we should do network dumping
int g_jxr_network_trace = 0;

#define DEFAULT_CALLBACK_URI				"/aptanaRPC"
#define DEFAULT_WORKER_HOSTNAME				"localhost"
#define DEFAULT_WORKER_PORT_NUMBER			4327

#define DEFAULT_MIN_CONNECTIONS				-1
#define DEFAULT_MAX_CONNECTIONS				-1
#define DEFAULT_CONN_EXPIRE_TIME			0
#define DEFAULT_ACQUIRE_CONN_TIMEOUT		5

static apr_status_t jaxer_server_config_cleanup(void* c)
{
	jaxer_server_conf *config = (jaxer_server_conf*) c;
	apr_status_t rv = APR_SUCCESS;
#pragma message(" need to rewrite")

#if 0
	jaxer_worker *w;
	apr_hash_t *wh;
	apr_hash_index_t *hi;
	pool *p = config->pool;
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

void *create_jaxer_server_config(pool * p, server_rec * s)
{
	jaxer_server_conf *config = ap_pcalloc(p, sizeof(*config));
	apr_status_t rv;

        if (global_apr_pool == NULL) {
            apr_initialize();
            apr_pool_create(&global_apr_pool, NULL);
        }

	config->default_init_env = apr_table_make(global_apr_pool, 20);
	
	config->php_fix_pathinfo_enable = 0;
	config->callback_uri = ap_psprintf(p, DEFAULT_CALLBACK_URI);

	config->worker_hash = apr_hash_make(global_apr_pool);
	if (!config->worker_hash)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, 0, global_apr_pool,
						  "mod_jaxer: create_jaxer_server_config failed to create worker list");
		return 0;
	}

	config->pool = ap_make_sub_pool(p);

	config->min_connections = DEFAULT_MIN_CONNECTIONS;
	config->max_connections = DEFAULT_MAX_CONNECTIONS;
	config->keep_connections = config->max_connections;
	config->acquire_conn_timeout = DEFAULT_ACQUIRE_CONN_TIMEOUT;
	config->conn_expire_time = DEFAULT_CONN_EXPIRE_TIME;

	config->worker_port_number = DEFAULT_WORKER_PORT_NUMBER;
	config->worker_hostname = ap_psprintf(p, DEFAULT_WORKER_HOSTNAME);

	config->network_dump = 0;
	// apr_pool_cleanup_register(p, config, jaxer_server_config_cleanup, apr_pool_cleanup_null);

	return config;
}

void *merge_jaxer_server_config(pool * p, void *basev,
								void *overridesv)
{
	int i;
	jaxer_server_conf *base = (jaxer_server_conf *) basev;
	jaxer_server_conf *overrides = (jaxer_server_conf *) overridesv;

	apr_hash_t *hbase = base->worker_hash;
	apr_hash_t *hover = overrides->worker_hash;
	apr_hash_index_t *hi;
	const void *wname;
	void* w;
	apr_ssize_t len;

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

	for(hi=apr_hash_first(global_apr_pool, hbase); hi; hi=apr_hash_next(hi))
	{
		apr_hash_this(hi, &wname, &len, &w);
		if (!apr_hash_get(hover, wname, len))
			apr_hash_set(hover, wname, len, w);
	}


	return overridesv;
}


void *create_jaxer_dir_config(pool * p, char *dummy)
{
	jaxer_dir_conf *config = ap_pcalloc(p, sizeof(jaxer_dir_conf));

        if (global_apr_pool == NULL) {
            apr_initialize();
            apr_pool_create(&global_apr_pool, NULL);
        }

	config->filter_ext_worker_hash = apr_hash_make(global_apr_pool);
    config->filter_ct_worker_hash = apr_hash_make(global_apr_pool);
    config->is_pass_through = 0;

	return (void *) config;
}

void *merge_jaxer_dir_config(pool * p, void *basev,
								void *overridesv)
{
	jaxer_dir_conf *base = (jaxer_dir_conf *) basev;
	jaxer_dir_conf *overrides = (jaxer_dir_conf *) overridesv;

	apr_hash_t *hbase = base->filter_ext_worker_hash;
	apr_hash_t *hover = overrides->filter_ext_worker_hash;
	
    apr_hash_t *hctbase = base->filter_ct_worker_hash;
	apr_hash_t *hctover = overrides->filter_ct_worker_hash;
	
    apr_hash_index_t *hi;
	const void *ename;
	void* w;
	apr_ssize_t len;

    if (base->is_pass_through)
    {
        overrides->is_pass_through = 1;
        return overridesv;
    }

	/*
	 * Add base to override if entry is not there.
	 * Do not override if already exists
	 */
	for(hi=apr_hash_first(global_apr_pool, hbase); hi; hi=apr_hash_next(hi))
	{
		apr_hash_this(hi, &ename, &len, &w);
		if (!apr_hash_get(hover, ename, len))
			apr_hash_set(hover, ename, len, w);
	}

    for(hi=apr_hash_first(global_apr_pool, hctbase); hi; hi=apr_hash_next(hi))
	{
		apr_hash_this(hi, &ename, &len, &w);
		if (!apr_hash_get(hctover, ename, len))
			apr_hash_set(hctover, ename, len, w);
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
	config->callback_uri = ap_psprintf(cmd->pool, arg);
	if (!config->callback_uri)
		return "Invalid Callback URI";

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

	return NULL;
}

const char* set_jaxer_worker(cmd_parms * cmd, void *dummy, const char *name, const char *host, const char *port)
{
	/*
	 * Each node will have info about one worker.
	 */
	server_rec *s = cmd->server;
	jaxer_worker *worker = 0;
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);

#pragma message ("Allow only one worker for now")
	if ( 0 < apr_hash_count(config->worker_hash))
	{
		return "JaxerWorker: Only one directive is allowed";
	}


	worker = (jaxer_worker*) ap_pcalloc(cmd->pool, sizeof(jaxer_worker));
	if ( worker == 0)
		return "JaxerWorker: Failed to allocate memory";

	worker->name = ap_psprintf(cmd->pool, name);
	if (!worker->name)
	{
		// It would be nice if we free the memory here.
		return "JaxerWorker: Invalid worker name";
	}

	worker->hostname = ap_psprintf(cmd->pool, host);
	if (!worker->hostname)
	{
		// It would be nice if we free the memory here.
		return "JaxerWorker: Invalid worker hostname";
	}

	if (!port || !apr_isdigit(port[0]))
		return "JaxerWorker: Port number must be numeric";

	worker->port_number = atoi(port);
	if (worker->port_number <= 0)
	{
		// It would be nice if we free the memory here.
		return "JaxerWorker: Invalid worker port number";
	}
	
	apr_hash_set( config->worker_hash, worker->name, strlen(worker->name), (void*) worker);


	return NULL;
}

jaxer_worker* get_worker_by_name(server_rec * s, const char* name)
{
	/*
	 * For now, we have only one worker.  Whoever asks will get the only worker.
	 */
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);
	apr_hash_t *wh = config->worker_hash;
	jaxer_worker *w = apr_hash_get(wh, name, APR_HASH_KEY_STRING);
	
#pragma message("temp work to get the first and only worker if w is null")
	if (!w && apr_hash_count(wh) > 0)
	{
		apr_hash_index_t *hi;
		hi = apr_hash_first(global_apr_pool, wh);
		apr_hash_this(hi, NULL, NULL, (void**)&w);
	}

	
	return w;
}

#ifndef APACHE1_3
const char *set_filter_worker_config(cmd_parms * cmd, void *dirconfig,
							   const char *worker,
							   const char *extension)
{
	jaxer_dir_conf *config = (jaxer_dir_conf *) dirconfig;
	char *ext;

    /* Sanity check */
	if (worker == NULL || extension == NULL
		|| *extension != '.' || *(extension + 1) == '\0'
		|| strchr(extension, '/') || strchr(extension, '\\'))
		return "Invalid filter file extension";

	ext = apr_pstrdup(cmd->server->process->pconf, extension);
	ap_str_tolower(ext);
	/* Add the node now */
	apr_hash_set(config->filter_ext_worker_hash, ext, strlen(ext),
				 apr_pstrdup(cmd->server->process->pconf, worker));

	return NULL;
}

const char *set_filter_worker_config_ct(cmd_parms * cmd, void *dirconfig,
							   const char *worker,
							   const char *content_type)
{
	jaxer_dir_conf *config = (jaxer_dir_conf *) dirconfig;
	char *ct;

    /* Sanity check */
	if (worker == NULL || content_type == NULL || *content_type  == '\0')
		return "Invalid filter content_type";

	ct = apr_pstrdup(cmd->server->process->pconf, content_type);
	ap_str_tolower(ct);
	/* Add the node now */
	apr_hash_set(config->filter_ct_worker_hash, ct, strlen(ct),
				 apr_pstrdup(cmd->server->process->pconf, worker));

	return NULL;
}
#endif

char *get_worker_name_from_content_type(const char *content_type, request_rec * r)
{
	char *worker;
	char *ct;
	jaxer_dir_conf *config =
		ap_get_module_config(r->per_dir_config, &jaxer_module);

    if (config->is_pass_through)
        return NULL;

	ct = ap_pstrdup(r->pool, content_type);
	ap_str_tolower(ct);
	/* Search content_type in per_dir_config */
	if (config
		&& (worker = apr_hash_get(config->filter_ct_worker_hash, ct,
						 strlen(ct))))
		return worker;

	return NULL;
}

char *get_worker_name_from_uri(const char *cgipath, request_rec * r)
{
	char *extension;
	char *worker;
	char *ext;
	jaxer_dir_conf *config =
		ap_get_module_config(r->per_dir_config, &jaxer_module);

    if (config->is_pass_through)
        return NULL;

	/* Get file name extension */
	extension = strrchr(cgipath, '.');
	if (extension == NULL)
		return NULL;

	ext = ap_pstrdup(r->pool, extension);
	ap_str_tolower(ext);
	/* Search file name extension in per_dir_config */
	if (config
		&& (worker = apr_hash_get(config->filter_ext_worker_hash, ext,
						 strlen(ext))))
		return worker;

	return NULL;
}

int get_php_fix_pathinfo_enable(server_rec * s)
{
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);
	return config ? config->php_fix_pathinfo_enable : 0;
}

const char *set_pass_through(cmd_parms * cmd, void *dirconfig)
{
	jaxer_dir_conf *config = (jaxer_dir_conf *) dirconfig;
    config->is_pass_through = 1;

	return NULL;
}

const char* set_network_dump(cmd_parms * cmd, void *dummy)
{
	server_rec *s = cmd->server;
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);

    config->network_dump = 1;

    // Can we just use a global flag?
    g_jxr_network_trace = 1;
	return NULL;
}

int get_network_dump(server_rec * s)
{
	jaxer_server_conf *config =
		ap_get_module_config(s->module_config, &jaxer_module);
	return config ? config->network_dump : 0;
}
