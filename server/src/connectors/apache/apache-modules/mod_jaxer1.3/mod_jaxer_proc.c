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
#include "httpd.h"
#include "http_core.h"
#include "http_request.h"
#include "apr_strings.h"
#include "apr_portable.h"
#include "apr_pools.h"
#include "http_log.h"
#include "mod_jaxer_proc.h"
#include "mod_jaxer_conf.h"
#include "mod_jaxer_protocol.h"
#include "mod_jaxer_message.h"
#include "apr_date.h"
#include "apr_lib.h"
#include "http_protocol.h"
#include "mod_jaxer_connection.h"
#ifndef APACHE1_3
#include "ap_mpm.h"
#endif
#include "compat.h"


static apr_size_t jxr_build_header(char* buf, int which, table *t)
{
	// If buf is NULL, we just computer the length required.  Otherwise, we fill
	// buf with the actual data.
	// which is ENV_PRMS or HDR_PRMS

	const array_header *env_arr = ap_table_elts(t);
    const table_entry *elts = (const table_entry *) env_arr->elts;
	int i;
    char *tz;
    
	apr_size_t pos;
	apr_size_t num_pos; // Remember where the number of headers will be.
	char type; //This is the type of this message.  EG HEADER, POST, BODY etc
	int num_hds = 0;

	// resetMessage(&pos);
	if (buf)
	{
		type = (which==ENV_PRMS) ? BLOCKTYPE_ENVIRO_VARS : BLOCKTYPE_HTTP_HEADER;
		jxr_msg_init(buf, &pos, type);
		num_pos = pos;

		// Place holder for actual number of headers
		jxr_msg_append_int16(buf, &pos, 0);
	}else
	{
		pos = 2 + sizeof(Jaxer_Header);
	}


    if (which == ENV_PRMS && !ap_table_get(t, "TZ"))
	{
        tz = getenv("TZ");
        if (tz != NULL)
		{
			if (buf)
			{
				jxr_msg_append_string(buf, &pos, "TZ");
				jxr_msg_append_string(buf, &pos, tz);
				num_hds++;
			}else
			{
				pos += 4; // Lengths
				pos += 2; // Length of "TZ"
				pos += (int) strlen(tz);
			}
        }
    }
    for (i = 0; i < env_arr->nelts; ++i)
	{
        if (!elts[i].key)
		{
            continue;
        }
		if (buf)
		{
			jxr_msg_append_string(buf, &pos, elts[i].key);
			jxr_msg_append_string(buf, &pos, elts[i].val);
			num_hds++;
		}else
		{
			pos += 4;
			pos += (int) strlen(elts[i].key);
			pos += (int) strlen(elts[i].val);
		}
	}
       
	if (buf)
	{
		// overriding
		jxr_msg_append_int16(buf, &num_pos, num_hds);

		// Fill the Header of message
		jxr_msg_end(buf, &pos);
	}

    return pos;
}

/* Build the begin request and environ request */
apr_status_t jxr_build_request_header_msg(request_rec* r, char** hdrmsg, int header_type)
{
	server_rec *main_server = r->server;
	table *table;
	apr_size_t hdr_len;
	
	// header_type has to be either ENV_PRMS or HDR_PRMS
	table = (header_type==ENV_PRMS) ? r->subprocess_env : r->headers_in;

	// Find the length of each header	
	hdr_len = jxr_build_header(0, header_type, table);

    *hdrmsg = ap_palloc(r->pool, hdr_len);
	if (! *hdrmsg)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r, "mod_jaxer: can't alloc memory for output bucket");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	jxr_build_header(*hdrmsg, header_type, table);

	return APR_SUCCESS;
}



static int set_cookie_doo_doo(void *v, const char *key, const char *val)
{
    apr_table_addn(v, key, val);
    return 1;
}

apr_status_t jxr_process_response_headers(request_rec *r, char *buf)
{
	apr_status_t rv = APR_SUCCESS;
	
	apr_size_t pos;
	apr_size_t len;
	char type;
	int nHeaders;
	int i;
	char name[MAX_STRING_LEN];
	char value[MAX_STRING_LEN];
	apr_size_t nlen, vlen;
	table *merge;
	table *cookie_table;
	char *w, *l;
	
	type = jxr_msg_get_type(buf);
	len = jxr_msg_get_length(buf,  &pos);

	if (type != BLOCKTYPE_HTTP_HEADER)
	{
		// Invalid data
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: invalid data type (%c) received, while expecting a header (%d)", type, BLOCKTYPE_HTTP_HEADER);
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* temporary place to hold headers to merge in later */
    merge = ap_make_table(r->pool, 10);

	cookie_table = ap_make_table(r->pool, 2);
    ap_table_do(set_cookie_doo_doo, cookie_table, r->err_headers_out, "Set-Cookie", NULL);

	nHeaders = jxr_msg_get_int16(buf, &pos);
	for (i=0; i<nHeaders; i++)
	{
		// Process one header -- name -- val
		nlen = jxr_msg_get_string(buf, &pos, name);
		vlen = jxr_msg_get_string(buf, &pos, value);

		w = name;
		l = value;

		if (!strcasecmp(w, "Content-type")) {
            char *tmp;

            /* Nuke trailing whitespace */

            char *endp = l + strlen(l) - 1;
            while (endp > l && apr_isspace(*endp)) {
                *endp-- = '\0';
            }

            tmp = ap_pstrdup(r->pool, l);
            ap_content_type_tolower(tmp);
            ap_set_content_type(r, tmp);
        }
        else if (!strcasecmp(w, "Status")) {
		/*
         * If the server returned a specific status, that's what
         * we'll use - otherwise we assume 200 OK.
         */
        
            r->status = atoi(l);
            r->status_line = ap_pstrdup(r->pool, l);
        }
        else if (!strcasecmp(w, "Location")) {
            ap_table_set(r->headers_out, w, l);
        }
        else if (!strcasecmp(w, "Content-Length")) {
            ap_table_set(r->headers_out, w, l);
        }
        else if (!strcasecmp(w, "Content-Range")) {
            ap_table_set(r->headers_out, w, l);
        }
        else if (!strcasecmp(w, "Transfer-Encoding")) {
            ap_table_set(r->headers_out, w, l);
        }
        else if (!strcasecmp(w, "Last-Modified")) {
        /*
         * If the script gave us a Last-Modified header, we can't just
         * pass it on blindly because of restrictions on future values.
         */
            ap_update_mtime(r, apr_date_parse_http(l));
            ap_set_last_modified(r);
        }
        else if (!strcasecmp(w, "Set-Cookie")) {
            ap_table_add(cookie_table, w, l);
        }
        else {
            ap_table_add(merge, w, l);
        }
	}


	// now merge stuff
	ap_overlap_tables(r->err_headers_out, merge,
        AP_OVERLAP_TABLES_MERGE);
    if (!ap_is_empty_table(cookie_table)) {
        /* the cookies have already been copied to the cookie_table */
        ap_table_unset(r->err_headers_out, "Set-Cookie");
        r->err_headers_out = ap_overlay_tables(r->pool,
            r->err_headers_out, cookie_table);
    }

	return rv;
}

apr_status_t jxr_process_response_body(jaxer_connection *ac, request_rec *r)
{
	apr_status_t rv = APR_SUCCESS;
	apr_size_t pos;
	char type;
	unsigned char *buf;

	//Read Body, build bb
	while (1)
	{
		apr_size_t datalen;
		
		rv = jxr_receive_message(ac, &buf);
		if ( rv != APR_SUCCESS)
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: received document data failed");
			return rv;
		}


		type = jxr_msg_get_type(buf);
		datalen = jxr_msg_get_length(buf,  &pos);

		if (type == BLOCKTYPE_ENDREQUEST)
		{
			// Done
			break;
		}else if ( type != BLOCKTYPE_DOCUMENT)
		{
			// Error
			compat_log_error(APLOG_MARK, APLOG_WARNING, rv, r->server, "mod_jaxer: invalid data type (%c) received, while expecting a body (%d)", type, BLOCKTYPE_DOCUMENT);
			return HTTP_INTERNAL_SERVER_ERROR;
		}

		ap_rwrite(buf + sizeof (Jaxer_Header), datalen, r);
	}

	return rv;
}


static int handle_request(request_rec * r, int role,
			   char *hdr_msg,
			   char *env_msg)
{
	pool *request_pool = r->main ? r->main->pool : r->pool;
	server_rec *main_server = r->server;
	jaxer_bucket_ctx *ctx;
	apr_status_t rv;
	const char *location;
	unsigned char *buf;
	int got_header = 0;
    int postdata_sent = 0;
	jaxer_worker *worker; // Need to get this from config
	jaxer_connection *ac;
    int should_redirect = 0;
	
	worker = get_worker_by_name(main_server, "worker1");
	
	ctx = ap_pcalloc(request_pool, sizeof(*ctx));
	if (!ctx) {
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r,
					 "mod_jaxer: apr_calloc bucket_ctx failed in handle_request function");
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	
	if ((rv = jxr_conn_open(worker, &ctx->ac, r))!= APR_SUCCESS) 
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: can't connect to socket");
		ctx->ac->has_error = 1;
		return HTTP_SERVICE_UNAVAILABLE;
	}
	ac = ctx->ac;
	ap_register_cleanup(request_pool, ctx->ac, jaxer_connection_cleanup, apr_pool_cleanup_null);


	// SEND HEADER
	if ((rv = jxr_send_message(ctx->ac, hdr_msg)) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send HTTP header to jaxer server failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	
	// SEND ENV HEADER
	if ((rv = jxr_send_message(ctx->ac, env_msg)) != APR_SUCCESS) 
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send environment vars to jaxer server failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	
	/* TODO: May need to send an empty doc */
	if ((rv = jxr_send_message_add_header(ac, 0, 0, BLOCKTYPE_DOCUMENT)) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send emptty doc to jaxer error");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	if ((rv = jxr_send_message_add_header(ac, 0, 0, BLOCKTYPE_ENDREQUEST)) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: write end of (doc) request to jaxer server failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	

	/*
	 * Now get a response, to see if Jaxer wants anything until we get a header back
	 */
    got_header = 0;

	while (!got_header)
	{
		char msg_type;
        
		if ((rv = jxr_receive_message(ac, &buf)) != APR_SUCCESS)
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: check for request message failed");
			ac->has_error = 1;	
			return rv;
		}
        
        msg_type = jxr_msg_get_type(buf);
		switch ( msg_type)
		{
			// char uri[MAX_STRING_LEN];
			// apr_size_t pos, len;
		case BLOCKTYPE_REQUEST_POSTDATA:
			if (postdata_sent)
			{
				compat_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: requesting postdata again");
				ac->has_error = 1;	
				return rv;
			}
			if ((rv = jxr_send_postdata(ac, r)) != APR_SUCCESS)
			{
				compat_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: error while sending postdata");
				ac->has_error = 1;	
				return rv;
			}
			postdata_sent = 1;
			break;

        case BLOCKTYPE_HTTP_HEADER:
            got_header = 1;
            break;

		default:
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: invalid request (type=%d) received", msg_type);
			ac->has_error = 1;
            return (!APR_SUCCESS);
			break;
		}
	}

	if ((rv = jxr_process_response_headers(r, buf)) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: process response header failed");
		ac->has_error = 1;
		return rv;
	}

    /* Check redirect */
	location = ap_table_get(r->headers_out, "Location");
    if (location && location[0])
    {
        should_redirect = 1;
        compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: request (status=%d) will be redirected to %s",
            r->status, location);
    }

	if (should_redirect) {
		/* This redirect needs to be a GET no matter what the original 
		 * method was. 
		 */
		r->method = ap_pstrdup(r->pool, "GET");
		r->method_number = M_GET;

		/* We already read the message body (if any), so don't allow 
		 * the redirected request to think it has one. We can ignore 
		 * Transfer-Encoding, since we used REQUEST_CHUNKED_ERROR. 
		 */
		ap_table_unset(r->headers_in, "Content-Length");

        compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: redirecting request to %s",
            location);
		ap_internal_redirect_handler(location, r);
		return APR_SUCCESS;
	}

	/* Now pass to output filter */
	
	if (role == FCGI_RESPONDER && (rv = jxr_process_response_body(ac, r)) != APR_SUCCESS) {
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: process_response_body failed");
		return HTTP_INTERNAL_SERVER_ERROR;
	}
return HTTP_OK;
}

int bridge_request(request_rec * r, int role)
{
	apr_status_t rv = APR_SUCCESS;
	
	char *hdr_msg;
	char *env_msg;

	if ((rv =jxr_build_request_header_msg(r, &hdr_msg, HDR_PRMS) ) != APR_SUCCESS)
		return rv;
	if ((rv =jxr_build_request_header_msg(r, &env_msg, ENV_PRMS) ) != APR_SUCCESS)
		return rv;

	/* Bridge the request */
	return handle_request(r, role, hdr_msg, env_msg);
}

apr_status_t jxr_init_worker(jaxer_worker *aworker, apr_pool_t *p, int min_size, int max_size, int acquire_conn_timeout)
{
	apr_status_t rv;
	int is_threaded;
    int mpm_threads = 1;
	
	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: initializing worker %s", aworker->name);

	rv = apr_pool_create(&aworker->pool, p);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: Failed to create subpool for jaxerworker %s", aworker->name);
		return rv;
	}

	rv = apr_pool_create(&aworker->res_pool, p);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: Failed to create res pool for jaxerworker %s", aworker->name);
		return rv;
	}


	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: created pools for worker %s", aworker->name);

#if APR_HAS_THREADS
	rv = apr_thread_mutex_create (&aworker->mutex, APR_THREAD_MUTEX_DEFAULT, p);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: Failed to create mutex for jaxerworker %s", aworker->name);
		return rv;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: mutex created worker %s", aworker->name);

#endif

#ifndef APACHE1_3
	 /* Set default connection cache size for multi-threaded MPMs */
	if (ap_mpm_query(AP_MPMQ_IS_THREADED, &is_threaded) == APR_SUCCESS &&
		is_threaded != AP_MPMQ_NOT_SUPPORTED)
	{
        	if (ap_mpm_query(AP_MPMQ_MAX_THREADS, &mpm_threads) != APR_SUCCESS)
			mpm_threads = 1;
	}
#endif

	// ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: number of threads=%d for %s", mpm_threads, aworker->name);

	/*
	 * cache_size >= min_connections && cache_size <= mpm_threads
	 * min_connections > mpm_threads/10 
	 */

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: connection pool config for %s: (min=%d max=%d)",
		aworker->name, min_size, max_size);

	if (min_size < 0)
	{
		// Default is 10% of the thread pool size.  min=1
		min_size = mpm_threads/10;
		if (min_size <= 0)
			min_size = 1;
	}else if (min_size > mpm_threads)
		min_size = mpm_threads;

	if (max_size <= 0 || max_size > mpm_threads)
		max_size = mpm_threads;

	if (max_size < min_size)
		max_size = min_size;

	aworker->acquire_timeout = acquire_conn_timeout;
	aworker->nmin = min_size;
	aworker->nkeep = max_size;
	aworker->nmax = max_size;
	aworker->exptime = 0;

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: using connection pool config for %s: (min=%d max=%d acquire-timeout=%d)",
		aworker->name, aworker->nmin, aworker->nmax, aworker->acquire_timeout);


	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: getting socket address for worker %s (host=%s port=%d)",
		aworker->name, aworker->hostname, aworker->port_number);

	rv = apr_sockaddr_info_get(&aworker->remote_sa, aworker->hostname, APR_INET, aworker->port_number, 0, p);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p,
				"mod_jaxer: apr_sockaddr_info_get failed with remote address %s and ip %d: return code=%d", aworker->hostname, aworker->port_number, rv);
		return rv;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: call jxr_conn_setup for worker %s", aworker->name);

	if ((rv = jxr_conn_setup(aworker)) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p,
				"mod_jaxer: jxr_conn_setup failed for worker %s", aworker->name);
		return rv;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: initialized worker %s", aworker->name);

	return rv;
}

#ifndef APACHE1_3
apr_status_t jxr_append_brigade(request_rec *r, apr_bucket_brigade *dest, apr_bucket_brigade *bb, int *eos_seen)
{
	apr_size_t max_msglen = MAX_PACKET_SIZE - sizeof(Jaxer_Header);
	apr_status_t rv;

	while (!APR_BRIGADE_EMPTY(bb)) 
	{
		apr_size_t readlen;
		const char *buffer;
		
		apr_bucket *e = APR_BRIGADE_FIRST(bb);

		if (APR_BUCKET_IS_EOS(e) )
		{
			apr_bucket_delete(e);
			if (eos_seen)
				*eos_seen = 1;
			continue;
		}
		if (APR_BUCKET_IS_METADATA(e)) {
			apr_bucket_delete(e);
			continue;
		}

		
		/* Read the bucket now */
		if ((rv = apr_bucket_read(e, &buffer, &readlen, APR_BLOCK_READ)) != APR_SUCCESS) 
		{
			ap_log_perror(APLOG_MARK, APLOG_INFO, rv, r->pool, "mod_jaxer: can't read data from handler");
			return rv;
		}
		
		if (readlen > max_msglen)
		{
        	apr_bucket_split(e, max_msglen);
		}else
		{
			APR_BUCKET_REMOVE(e);
			APR_BRIGADE_INSERT_TAIL(dest, e);
		}
	}
	if ((rv=apr_brigade_destroy(bb)) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_INFO, rv, r->pool, "mod_jaxer: failed to destroy brigade.");
		return rv;
	}

	return APR_SUCCESS;
}
#endif

typedef struct partial_apr_socket_t
{
	apr_pool_t *pool;
#if defined(WIN32)
	SOCKET socketdes;
#else
	int socketdes;
#endif
	/* other stuff follows */
}partial_apr_socket_t;

apr_status_t jxr_get_request_if_any(jaxer_connection *ac, char** pbuf)
{
	apr_status_t rv = APR_SUCCESS;
	request_rec *r = ac->request;
	server_rec *main_server = r->server;
	apr_socket_t *sock = ac->sock;
	struct timeval tv;

	fd_set fdread;
	int rc;

	*pbuf = 0;
	
	rv = apr_socket_opt_set(sock, APR_SO_NONBLOCK, 1);
	if (rv != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: turn on non-blocking failed");
		ac->has_error = 1;
		return rv;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	FD_ZERO(&fdread);
	FD_SET(((partial_apr_socket_t*)sock)->socketdes, &fdread);
			
	rc = select(1+ (int)((partial_apr_socket_t*)sock)->socketdes, &fdread, NULL, NULL, &tv);
	if (rc < 0)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rc, r, "mod_jaxer: socket select failed");
		ac->has_error = 1;
		return rc;
	}
	
	
	/* turn off non-blocking */
	rv = apr_socket_opt_set(sock, APR_SO_NONBLOCK, 0);
	if (rv != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: turn off non-blocking failed");
		return rv;
	}

	/* return if nothing */
	if (rc == 0)
		return rv;


	/* 
	 * Since we have only one socket, we know we have data for reading if we are here.
	 * read a message
	 */
	if ((rv=jxr_receive_message(ac, (unsigned char **)pbuf)) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: receive message failed");
		return rv;
	}
	return rv;
}

apr_status_t jxr_send_postdata(jaxer_connection* ac, request_rec *r)
{
	apr_status_t rv = APR_SUCCESS;
	server_rec *main_server = r->server;
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(r->per_dir_config, &jaxer_module);

    
		if ((rv = jxr_send_file_add_header(ac, config->fnameOut, BLOCKTYPE_POSTDATA)) != APR_SUCCESS) 
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send postdata to jaxer error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}

		// Send empty postdata
		if ((rv = jxr_send_message_add_header(ac, 0, 0, BLOCKTYPE_POSTDATA)) != APR_SUCCESS) 
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send postdata to jaxer error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}

    /* Send end of req */

	if ((rv = jxr_send_message_add_header(ac, 0, 0, BLOCKTYPE_ENDREQUEST)) != APR_SUCCESS) 
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send end request for postdata error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

    return rv;
}

void jaxer_add_env_vars(request_rec * r)
{
    /* 
     * Add additonal vars that are needed by jaxer.
     * REMOTE_USER
     * REMOTE_HOST
     * STATUS_CODE
     * HTTPS
     */
	apr_table_t *env = r->subprocess_env;
    jaxer_dir_conf *config =
		ap_get_module_config(r->per_dir_config, &jaxer_module);

    int is_ip;
    int ssl_on;
    const char * ru = ap_get_remote_logname(r);
#ifndef APACHE1_3
    const char * rh = ap_get_remote_host(r->connection, config, REMOTE_NAME, &is_ip);
#else
    const char * rh = ap_get_remote_host(r->connection, config, REMOTE_NAME);
#endif

    if (ru)
        apr_table_setn(env, "REMOTE_USER", ru);
    // else
    //    apr_table_setn(env, "REMOTE_USER", "");

    if (rh && !is_ip)
        ap_table_setn(env, "REMOTE_HOST", rh);

    ap_table_setn(env, "STATUS_CODE", ap_psprintf(r->pool, "%d", r->status));

    ssl_on = jaxer_conn_is_https(r->connection);
    ap_table_setn(env, "HTTPS", ap_psprintf(r->pool, "%s", (ssl_on != 0)? "on" : "off" ));
}
