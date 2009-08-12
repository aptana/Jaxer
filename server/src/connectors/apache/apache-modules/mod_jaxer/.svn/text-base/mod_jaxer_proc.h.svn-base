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
#ifndef FCGID_PM_PROC_H
#define FCGID_PM_PROC_H
#include "httpd.h"
#include "apr_pools.h"
#include "mod_jaxer_connection.h"

typedef struct jaxer_bucket_ctx_t {
	jaxer_connection *ac;
	// int has_error;
} jaxer_bucket_ctx;

typedef struct postdata_ctx {
	apr_bucket_brigade *bb;
    //char *filename;
    apr_file_t *tmpfile;
    apr_off_t bytes_read;
    int err;
    apr_pool_t *pool;
} postdata_ctx;


int jxr_do_handler_request(request_rec * r);
apr_status_t jxr_process_response_headers(jaxer_connection *ac, char *buf);
apr_status_t jxr_process_response_body(jaxer_connection *ac, apr_bucket_brigade *bb);
apr_status_t jxr_build_request_header_msg(request_rec* r, char** hdrmsg, enum eHeaderType header_type);
apr_status_t make_header_n_send_message(request_rec *r, apr_socket_t *sock, const char* buf, int msglen, char type);
apr_status_t jxr_init_worker(jaxer_worker *aworker, apr_pool_t *p, int min_size, int max_size, int exptime);
apr_status_t jxr_append_brigade(request_rec *r, apr_bucket_brigade *dest, apr_bucket_brigade *bb, int *eos_seen);
apr_status_t jxr_get_request_if_any(jaxer_connection *ac, char** pbuf);
apr_status_t jxr_send_postdata(jaxer_connection* ctx, postdata_ctx* post_bb, int should_add_end_request);
int jaxer_conn_is_https(conn_rec *c);
apr_status_t jxr_get_postdata_from_brigade(request_rec* r, apr_bucket_brigade *bb, postdata_ctx *ctx, int *eos_seen);

apr_status_t jaxer_postdata_cleanup(void* ctx);
// apr_status_t jxr_process_file_request(jaxer_connection *ac, const char* uri);
int jxr_is_debug_request(request_rec * r, char** jaxer_id);
int is_apache_dummy_request(request_rec *r);
#endif
