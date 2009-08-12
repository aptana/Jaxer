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
#ifndef __MOD_JAXER_CONNECTIONS_H
#define __MOD_JAXER_CONNECTIONS_H

#include "apr_time.h"
#include "httpd.h"
#include "apr.h"
#include "apr_reslist.h"
#include "apr_network_io.h"
#include "mod_jaxer_protocol.h"

/*
 * Manage a connection pool and communication related stuff
 */

// #define MAX_SOCKET_CONNECTIONS	1024


typedef struct jaxer_worker
{
	char* name;		// Just a descriptive name
	char* hostname;
	int port_number;
	apr_sockaddr_t *remote_sa;
	apr_reslist_t* ac_cache;
	int nmin;
	int nkeep;
	int nmax;
	int exptime; // in secs
	int acquire_timeout;

	apr_pool_t *pool;
	apr_pool_t *res_pool;
	struct jaxer_worker *next;

	// int cache_size;
	// int min_connections;
	// int force_close_timeout;
	// jaxer_connection *ac_cache; // ac_cache[MAX_SOCKET_CONNECTIONS];
	
	
#if APR_HAS_THREADS
	apr_thread_mutex_t *mutex;
#endif

	/* The followings are not used yet */
#if 0
	int proto;
	int socket_timeout;
	int keepalive;
	int cache_timeout;
#endif
}jaxer_worker;

/* This should replace jaxer_socket */
typedef struct jaxer_connection
{
	apr_time_t start_time;		 /* the time of this connection create */
	apr_time_t last_active_time; /* last use */
	request_rec *request;
	apr_socket_t *sock;
	int inuse;
	jaxer_worker *worker;
	int has_error; // Whether an error was encountered during the request processing
}jaxer_connection;

int jxr_socket_sendfull(jaxer_connection *ac, const unsigned char *b, int len);
// apr_status_t jxr_get_or_create_connection(jaxer_worker *aworker, jaxer_connection **ac, request_rec* r);
apr_status_t jxr_send_message(jaxer_connection *ac, unsigned char *msg);
apr_status_t jxr_send_file_add_header(jaxer_connection * ac, char *fname, enum BlockType bType);
apr_status_t jxr_receive_message(jaxer_connection *ac, unsigned char **pmsg);
apr_status_t jxr_send_message_add_header(jaxer_connection *ac, const char* buf, int msglen, enum BlockType bType);
apr_status_t jaxer_connection_cleanup(void *theac);
apr_status_t jxr_connect(jaxer_connection *pac);
apr_status_t jxr_conn_setup(jaxer_worker* aw);
apr_status_t jxr_conn_open(jaxer_worker* aw, jaxer_connection **pac, request_rec *r);


#endif // __MOD_JAXER_CONNECTIONS_H

