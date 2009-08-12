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

#define JSC_PHP_FIX_PATHINFO_ENABLE_SET    1
#define JSC_CALLBACK_URI_SET              (1<<1)
#define JSC_WORKER_SET                    (1<<2)
#define JSC_MIN_CONNECTIONS_SET           (1<<3)
#define JSC_MAX_CONNECTIONS_SET           (1<<4)
#define JSC_KEEP_CONNECTIONS_SET          (1<<5)
#define JSC_MAX_POSTDATA_MEMORY_SET       (1<<6)
#define JSC_ACQUIRE_CONN_TIMEOUT_SET      (1<<7)
#define JSC_CONN_EXPIRE_TIME_SET          (1<<8)
#define JSC_NETWORK_DUMP_SET              (1<<9)

typedef struct {

	apr_table_t *default_init_env; //Not used yet.


	int php_fix_pathinfo_enable;

	char *callback_uri;

	struct jaxer_worker *worker;

    // Sharing one worker if multiple hosts have the same "worker"
    // do not free this.
	struct jaxer_worker *real_worker;

	// Connection pool config
	int min_connections;
	int max_connections;
	int keep_connections; // == max_connections

    // memory limit for postdata.  0 means do not save to diskfile at all.
    apr_off_t max_postdata_memory;

	// How long should we wait on a new connection before giving up (sec)
	int acquire_conn_timeout;

	// Not sure exactly what this means.  Set it to zero for now (sec)
	int conn_expire_time;

	apr_pool_t *pool;

	int network_dump;

    //remember which parameters were set from config
    int set_flags;
	
} jaxer_server_conf;


#define JDC_IS_PASS_THROUGH_SET 1

typedef struct {

	/* filter extensions */
	apr_hash_t *filter_ext_hash;

    /* filter content_types*/
    apr_hash_t *filter_ct_hash;

    /* whether jaxer should bypass this dir */
    int is_pass_through;

    /* List of handler/mime-types that we should not filter */
    apr_hash_t *filter_bypass_handler_hash;

    //remember which parameters were set from config
    int set_flags;


} jaxer_dir_conf;

void *create_jaxer_server_config(apr_pool_t * p, server_rec * s);

void *merge_jaxer_server_config(apr_pool_t * p, void *basev, void *overridesv);

void *create_jaxer_dir_config(apr_pool_t * p, char *dummy);

void *merge_jaxer_dir_config(apr_pool_t * p, void *basev, void *overridesv);


const char *add_default_env_vars(cmd_parms * cmd, void *sconf,
								 const char *name, const char *value);

apr_table_t *get_default_env_vars(request_rec * r);

const char *set_callback_uri(cmd_parms * cmd, void *dummy, const char *arg);

const char* set_jaxer_worker(cmd_parms * cmd, void *dummy,const char *host, const char *port);

jaxer_worker* get_jaxer_worker(server_rec * s);

const char *set_php_fix_pathinfo_enable(cmd_parms * cmd, void *dummy, const char *arg);

const char* set_max_connection(cmd_parms * cmd, void *dummy, const char *num);

const char* set_min_connection(cmd_parms * cmd, void *dummy, const char *num);

const char* set_wait_connection_timeout(cmd_parms * cmd, void *dummy, const char *num);

const char* set_postdata_memory_limit(cmd_parms * cmd, void *dummy, const char *num);

int get_php_fix_pathinfo_enable(server_rec * s);

const char *set_filter_config(cmd_parms * cmd, void *dummy, const char *extension);

const char *set_filter_config_ct(cmd_parms * cmd, void *dummy, const char *content_type);

const char *set_filter_config_bypass_handler(cmd_parms * cmd, void *dummy, const char *handler);

int should_filter_uri(const char *cgipath, request_rec * r);

int should_filter_content_type(const char *content_type, request_rec * r);

int should_bypass_handler(const char *handler, request_rec * r);

const char *set_pass_through(cmd_parms * cmd, void *dirconfig, const char* arg);

int get_is_pass_through(request_rec* r);

void jaxer_add_env_vars(request_rec * r);

int get_network_dump(server_rec * s);

const char* set_network_dump(cmd_parms * cmd, void *dummy, const char* arg);

#endif
