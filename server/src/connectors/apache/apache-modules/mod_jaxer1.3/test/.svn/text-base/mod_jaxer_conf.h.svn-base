/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
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
#ifndef FCGID_CONF_H
#define FCGID_CONF_H
#include "apr_user.h"
#include "http_config.h"
#include "mod_jaxer_connection.h"
#include "apr_hash.h"

#define JAXER_HANDLER "jaxer-handler"
#define JAXER_FILTER "jaxer-filter"
module MODULE_VAR_EXPORT jaxer_module;

apr_pool_t *global_apr_pool;

typedef struct {

	apr_table_t *default_init_env; //Not used yet.

	int php_fix_pathinfo_enable;

	char *callback_uri;

	char *worker_hostname;

	int worker_port_number;

	// Connection pool config
	int min_connections;
	int max_connections;
	int keep_connections; // == max_connections

	// How long should we wait on a new connection before giving up (sec)
	int acquire_conn_timeout;

	// Not sure exactly what this means.  Set it to zero for now (sec)
	int conn_expire_time;

	apr_hash_t* worker_hash;

	pool *pool;

	int was_initialized;

    int network_dump;
	
} jaxer_server_conf;


typedef struct {

	/* filter extension to worker hash */
	apr_hash_t *filter_ext_worker_hash;

    /* filter content_type to worker hash */
    apr_hash_t *filter_ct_worker_hash;

    /* whether jaxer should bypass this dir */
    int is_pass_through;

    apr_pool_t *reqPool;

    char *fnameIn;
    char *fnameOut;
} jaxer_dir_conf;

void *create_jaxer_server_config(pool * p, server_rec * s);

void *merge_jaxer_server_config(pool * p, void *basev, void *overridesv);

void *create_jaxer_dir_config(pool * p, char *dummy);

void *merge_jaxer_dir_config(pool * p, void *basev, void *overridesv);


const char *add_default_env_vars(cmd_parms * cmd, void *sconf,
								 const char *name, const char *value);

apr_table_t *get_default_env_vars(request_rec * r);

const char *set_callback_uri(cmd_parms * cmd, void *dummy, const char *arg);

const char* set_jaxer_worker(cmd_parms * cmd, void *dummy, const char *name, const char *host, const char *port);

jaxer_worker* get_worker_by_name(server_rec * s, const char* name);

const char *set_php_fix_pathinfo_enable(cmd_parms * cmd, void *dummy, const char *arg);

const char* set_max_connection(cmd_parms * cmd, void *dummy, const char *num);

const char* set_min_connection(cmd_parms * cmd, void *dummy, const char *num);

const char* set_wait_connection_timeout(cmd_parms * cmd, void *dummy, const char *num);

int get_php_fix_pathinfo_enable(server_rec * s);

const char *set_filter_worker_config(cmd_parms * cmd, void *dummy,
							   const char *worker, const char *extension);

const char *set_filter_worker_config_ct(cmd_parms * cmd, void *dummy,
							   const char *worker, const char *content_type);

char *get_worker_name_from_uri(const char *cgipath, request_rec * r);

char *get_worker_name_from_content_type(const char *content_type, request_rec * r);

const char *set_pass_through(cmd_parms * cmd, void *dirconfig);

void jaxer_add_env_vars(request_rec * r);

int get_network_dump(server_rec * s);

const char* set_network_dump(cmd_parms * cmd, void *dummy);

#endif
