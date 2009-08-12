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
#include "ap_mpm.h"

extern module AP_MODULE_DECLARE_DATA jaxer_module;

static apr_size_t jxr_build_header(char* buf, enum eHeaderType header_type, apr_table_t *t)
{
	// If buf is NULL, we just computer the length required.  Otherwise, we fill
	// buf with the actual data.
	// which is ENV_PRMS or eREQUEST_HEADER

	const apr_array_header_t *env_arr = apr_table_elts(t);
    const apr_table_entry_t *elts = (const apr_table_entry_t *) env_arr->elts;
	int i;
    char *tz;
    
	apr_size_t pos;
	apr_size_t num_pos; // Remember where the number of headers will be.
	char type; //This is the type of this message.  EG HEADER, POST, BODY etc
	int num_hds = 0;

	// resetMessage(&pos);
	if (buf)
	{
        if (header_type==eENVIRON_HDR)
            type = bt_Environment;
        else if (header_type==eREQUEST_HEADER)
            type = bt_RequestHeader;
        else
            type = bt_ResponseHeader;

		jxr_msg_init(buf, &pos, type);
		num_pos = pos;

		// Place holder for actual number of headers
		jxr_msg_append_int16(buf, &pos, 0);
	}else
	{
		pos = 2 + sizeof(Jaxer_Header);
	}


    if (header_type == eENVIRON_HDR && !apr_table_get(t, "TZ"))
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
apr_status_t jxr_build_request_header_msg(request_rec* r, char** hdrmsg, enum eHeaderType header_type)
{
	server_rec *main_server = r->server;
	apr_table_t *table;
	apr_size_t hdr_len;
	
	// header_type has to be either ENV_PRMS or eREQUEST_HEADER
    if (header_type==eENVIRON_HDR)
        table = r->subprocess_env;
    else if (header_type==eREQUEST_HEADER)
        table = r->headers_in;
    else
        table = r->headers_out;

	// Find the length of each header	
	hdr_len = jxr_build_header(0, header_type, table);

    *hdrmsg = apr_palloc(r->pool, hdr_len);
	if (! *hdrmsg)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r, "mod_jaxer: can't alloc memory for output bucket");
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

static apr_status_t jxr_recv_and_merge_msg(jaxer_connection *ac, char msg_type, unsigned char* old_partial_msg, apr_size_t old_partial_len, unsigned char** new_msg)
{
	// old_partial_msg is the last part (incomplete) of a message.  It cannot be processed without receiving 
	// the remaining part.
	// allocate a large buf to hold both the new and the old_partial
	// receive the new msg
	// add the partial as part of the new msg
	// correct the new msg header

	unsigned char *buf = 0;
	char type;
	apr_status_t rv = APR_SUCCESS;
	apr_size_t len, pos;
	request_rec* r = ac->request;
	apr_pool_t *p = r->pool; 

	if ((rv = jxr_receive_message(ac, &buf)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: receive additional header message failed");
		return rv;
	}
    
	// make sure this is the type expected
    type = jxr_msg_get_type(buf);
	if (type != msg_type)
	{
		// Invalid data
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: invalid data type (%c) received, while expecting a type (%d)", type, msg_type);
		return HTTP_INTERNAL_SERVER_ERROR;
	}


	len = jxr_msg_get_length(buf,  &pos);
	* new_msg = (unsigned char*)apr_palloc(p, old_partial_len + len + pos);
	if (! *new_msg)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: cannot allocate memory.");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	memcpy(*new_msg, buf, sizeof(Jaxer_Header));
	memcpy(*new_msg + sizeof(Jaxer_Header), old_partial_msg, old_partial_len);
	memcpy(*new_msg + sizeof(Jaxer_Header) + old_partial_len, buf+sizeof(Jaxer_Header), len);
	pos += len + old_partial_len;
	jxr_msg_end(*new_msg, &pos);

	return rv;
}

apr_status_t jxr_process_response_headers(jaxer_connection *ac, char *buf)
{
    // These are the headers sent back from Jaxer.
    // Here is the rule:
    //  1.  If the value is empty, then delete it
    //  2.  Otherwise, REPLACE it (delete existing if any, add new one)

	apr_status_t rv = APR_SUCCESS;
	request_rec* r = ac->request;
	
	apr_size_t pos, pos0;
	apr_size_t len;
	char type;
	int nHeaders;
	int i;
	char name[MAX_STRING_LEN];
	char value[MAX_STRING_LEN];
	apr_size_t nlen, vlen;
	apr_table_t *merge;
	apr_table_t *cookie_table;
	char *w, *l;
	char *buf2 = 0;
	
	type = jxr_msg_get_type(buf);
	len = jxr_msg_get_length(buf,  &pos);
	pos0 = pos;

	if (type != bt_ResponseHeader)
	{
		// Invalid data
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: invalid data type (%c) received, while expecting a header (%d)", type, bt_ResponseHeader);
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* temporary place to hold headers to merge in later */
    merge = apr_table_make(r->pool, 10);

	cookie_table = apr_table_make(r->pool, 2);
    apr_table_do(set_cookie_doo_doo, cookie_table, r->err_headers_out, "Set-Cookie", NULL);

	nHeaders = jxr_msg_get_int16(buf, &pos);
	for (i=0; i<nHeaders; i++)
	{
		// Process one header -- name -- val
		apr_size_t old_pos = pos;

		// Make sure we do not pass the end of the buf
		while (len < 2 || len-2 < pos-pos0)
		{
			// get next block of header
			if ((rv = jxr_recv_and_merge_msg(ac, bt_ResponseHeader, buf+pos, len-(pos-pos0), &buf2)) != APR_SUCCESS)
			{
				return rv;
			}
			buf = buf2;
			len = jxr_msg_get_length(buf,  &pos);
			pos0 = pos;
 		}

		nlen = jxr_msg_peek_int16(buf, pos);
		while (len<2 || len-2 < pos-pos0 + nlen)
		{
			// get next block of header
			if ((rv = jxr_recv_and_merge_msg(ac, bt_ResponseHeader, buf+pos, len-(pos-pos0), &buf2)) != APR_SUCCESS)
			{
				return rv;
			}
			buf = buf2;
			len = jxr_msg_get_length(buf,  &pos);
			pos0 = pos;
		}
		nlen = jxr_msg_get_string(buf, &pos, name);
		
		vlen = jxr_msg_peek_int16(buf, pos);
		while (len<2 || len-2 < pos-pos0 + vlen)
		{
			// get next block of header
			if ((rv = jxr_recv_and_merge_msg(ac, bt_ResponseHeader, buf+pos, len-(pos-pos0), &buf2)) != APR_SUCCESS)
			{
				return rv;
			}
			buf = buf2;
			len = jxr_msg_get_length(buf,  &pos);
			pos0 = pos;
		}

		vlen = jxr_msg_get_string((unsigned char*)buf, &pos, value);

		w = name;
		l = value;
        if (vlen == 0)
        {
            // delete it
            apr_table_unset(r->headers_out, w);
			if (!strcasecmp(w,"Content-type"))
				ap_set_content_type(r, NULL);

        }else if (!strcasecmp(w, "Content-type")) {
            char *tmp;

            /* Nuke trailing whitespace */

            char *endp = l + strlen(l) - 1;
            while (endp > l && apr_isspace(*endp)) {
                *endp-- = '\0';
            }

            tmp = apr_pstrdup(r->pool, l);
            ap_content_type_tolower(tmp);
            ap_set_content_type(r, tmp);
        }
        else if (!strcasecmp(w, "Status")) {
		/*
         * If the server returned a specific status, that's what
         * we'll use - otherwise we assume 200 OK.
         */
        
            r->status = atoi(l);
            r->status_line = apr_pstrdup(r->pool, l);
        }
        else if (!strcasecmp(w, "Location")) {
            apr_table_set(r->headers_out, w, l);
        }
        else if (!strcasecmp(w, "Content-Length")) {
            apr_table_set(r->headers_out, w, l);
        }
        else if (!strcasecmp(w, "Content-Range")) {
            apr_table_set(r->headers_out, w, l);
        }
        else if (!strcasecmp(w, "Transfer-Encoding")) {
            apr_table_set(r->headers_out, w, l);
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
            apr_table_add(cookie_table, w, l);
        }
        else {
            apr_table_unset(r->headers_out, w);
            apr_table_unset(r->err_headers_out, w);
            apr_table_add(merge, w, l);
        }
	}


    
	// now merge stuff
	//apr_table_overlap(r->err_headers_out, merge,
    //    APR_OVERLAP_TABLES_MERGE);
    // What should be the right way to handle the merge.  Let's just add them back
    apr_table_do(set_cookie_doo_doo, r->headers_out, merge, NULL);


    if (!apr_is_empty_table(cookie_table)) {
        /* the cookies have already been copied to the cookie_table */
        apr_table_unset(r->err_headers_out, "Set-Cookie");
        r->err_headers_out = apr_table_overlay(r->pool,
            r->err_headers_out, cookie_table);
    }

	return rv;
}

apr_status_t jxr_process_response_body(jaxer_connection *ac, apr_bucket_brigade *bb)
{
	request_rec *r = ac->request;
	apr_status_t rv = APR_SUCCESS;
	apr_size_t pos;
	char type;
	apr_bucket *buckettmp;	
	unsigned char *buf;
	


	//Read Body, build bb
	while (1)
	{
		apr_bucket *bucket;
		apr_size_t datalen;
		
		rv = jxr_receive_message(ac, &buf);
		if ( rv != APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: received document data failed");
			return rv;
		}


		type = jxr_msg_get_type(buf);
		datalen = jxr_msg_get_length(buf,  &pos);

		if (type == bt_EndRequest)
		{
			// Done
			break;
		}else if ( type !=bt_Document)
		{
			// Error
			ap_log_error(APLOG_MARK, APLOG_WARNING, rv, r->server, "mod_jaxer: invalid data type (%c) received, while expecting a body (%d)", type,bt_Document);
			return HTTP_INTERNAL_SERVER_ERROR;
		}

        if (bb != NULL)
        {
            bucket = apr_bucket_pool_create(buf, datalen + sizeof(Jaxer_Header),
                     r->pool, r->connection->bucket_alloc);

		    apr_bucket_split(bucket, sizeof(Jaxer_Header));
		    buckettmp = APR_BUCKET_NEXT(bucket);
		    apr_bucket_delete(bucket);
		    bucket = buckettmp;

		    // Add to bb
		    APR_BRIGADE_INSERT_TAIL(bb, bucket);
        }
	}

    if (bb != NULL)
    {
	    buckettmp = apr_bucket_eos_create(r->connection->bucket_alloc);
	    APR_BRIGADE_INSERT_TAIL(bb, buckettmp);
    }

	return rv;
}


static int jxr_handle_request(request_rec * r,
			   char *req_hdr_msg,
			   char *env_msg,
			   postdata_ctx *post_bb)
{
	apr_pool_t *request_pool = r->main ? r->main->pool : r->pool;
	server_rec *main_server = r->server;
	jaxer_bucket_ctx *ctx;
	apr_status_t rv;
	apr_bucket_brigade *brigade_stdout = 0;
	const char *location;
	unsigned char *buf;
	int got_header = 0;
    int postdata_sent = 0;
	jaxer_connection *ac;
    int should_redirect = 0;
	
	jaxer_worker *worker = get_jaxer_worker(main_server);
	
	ctx = apr_pcalloc(request_pool, sizeof(*ctx));
	if (!ctx) {
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r,
					 "mod_jaxer: apr_calloc bucket_ctx failed in handle_request function");
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	if ((rv = jxr_conn_open(worker, &ctx->ac, r, eRequestType_Handler))!= APR_SUCCESS &&
		(!worker->retry || (rv = jxr_conn_open(worker, &ctx->ac, r, eRequestType_Handler))!= APR_SUCCESS))
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: can't connect to socket");
		ctx->ac->has_error = 1;
		return HTTP_SERVICE_UNAVAILABLE;
	}
	ac = ctx->ac;
	apr_pool_cleanup_register(request_pool, ctx->ac, jaxer_connection_cleanup, apr_pool_cleanup_null);


	// SEND HEADER
	if ((rv = jxr_send_message(ctx->ac, req_hdr_msg)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send HTTP header to jaxer server failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	
	// SEND ENV HEADER
	if ((rv = jxr_send_message(ctx->ac, env_msg)) != APR_SUCCESS) 
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send environment vars to jaxer server failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	
	/* TODO: May need to send an empty doc */
	if ((rv = jxr_send_message_add_header(ac, 0, 0,bt_Document)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send emptty doc to jaxer error");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	if ((rv = jxr_send_message_add_header(ac, 0, 0, bt_EndRequest)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: write end of (doc) request to jaxer server failed");
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
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: check for request message failed");
			ac->has_error = 1;	
			return rv;
		}
        
        msg_type = jxr_msg_get_type(buf);
		switch ( msg_type)
		{
			// char uri[MAX_STRING_LEN];
			// apr_size_t pos, len;
		case bt_RequirePostData:
			if (postdata_sent)
			{
				ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: requesting postdata again");
				ac->has_error = 1;	
				return rv;
			}
			if ((rv = jxr_send_postdata(ac, post_bb, 1)) != APR_SUCCESS)
			{
				ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: error while sending postdata");
				ac->has_error = 1;	
				return rv;
			}
			postdata_sent = 1;
			break;

        case bt_ResponseHeader:
            got_header = 1;
            break;

		default:
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: invalid request (type=%d) received", msg_type);
			ac->has_error = 1;
            return (!APR_SUCCESS);
			break;
		}
	}

	if ((rv = jxr_process_response_headers(ac, buf)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: process response header failed");
		ac->has_error = 1;
		return rv;
	}

    /* Check redirect */
	location = apr_table_get(r->headers_out, "Location");
    if (location && location[0])
    {
        should_redirect = 1;
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: request (status=%d) will be redirected to %s",
            r->status, location);
    }

    /* Create stdout brigade */
    brigade_stdout = apr_brigade_create(request_pool, r->connection->bucket_alloc);
    if (!brigade_stdout) {
	    ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: apr_brigade_create failed in handle_request function");
	    ac->has_error = 1;
	    return HTTP_INTERNAL_SERVER_ERROR;
    }
	
    if ((rv = jxr_process_response_body(ac, should_redirect ? NULL : brigade_stdout)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: process response body failed");
		ac->has_error = 1;
		return rv;
	}

	
	if (should_redirect)
	{
		const static char *buf="<html>Jaxer redirect</html>";
        char s[10];
        apr_bucket* bucket = apr_bucket_pool_create(buf, strlen(buf),
                     r->pool, r->connection->bucket_alloc);
        APR_BRIGADE_INSERT_TAIL(brigade_stdout, bucket);
		bucket = apr_bucket_eos_create(r->connection->bucket_alloc);
	    APR_BRIGADE_INSERT_TAIL(brigade_stdout, bucket);

		apr_table_unset(r->headers_in, "Content-Length");
        
        sprintf(s,"%d", strlen(buf));
        apr_table_setn(r->headers_out, "Content-Length", s);
        
        apr_table_clear(r->headers_out);
        apr_table_clear(r->err_headers_out);
        apr_table_set(r->headers_out, "Location", location);


        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: redirecting request to %s", location);
    }

#if 0
    else if (location && r->status == 200) {
		/* XX Note that if a script wants to produce its own Redirect 
		 * body, it now has to explicitly *say* "Status: 302" 
		 */
        apr_brigade_destroy(brigade_stdout);
		return HTTP_MOVED_TEMPORARILY;
	}
#endif

	/* Now pass to output filter */
	if ((rv = ap_pass_brigade(r->output_filters, brigade_stdout)) != APR_SUCCESS) {
        apr_brigade_destroy(brigade_stdout);
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: ap_pass_brigade failed in handle_request function");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

    apr_brigade_destroy(brigade_stdout);
	return APR_SUCCESS;
}

int jxr_do_handler_request(request_rec * r)
{
	apr_pool_t *request_pool = r->main ? r->main->pool : r->pool;
	server_rec *main_server = r->server;
	apr_status_t rv = APR_SUCCESS;
	int eos_seen = 0;
	
	char *req_hdr_msg;
	char *env_msg;
	postdata_ctx *postdata = 0;
    apr_bucket_brigade *bb;
    apr_bucket *b;

	if ((rv =jxr_build_request_header_msg(r, &req_hdr_msg, eREQUEST_HEADER) ) != APR_SUCCESS)
		return rv;
	if ((rv =jxr_build_request_header_msg(r, &env_msg, eENVIRON_HDR) ) != APR_SUCCESS)
		return rv;


	bb = apr_brigade_create(request_pool, r->connection->bucket_alloc);

	if (!bb	|| (rv = ap_get_brigade(r->input_filters, bb,
								AP_MODE_READBYTES,
								APR_BLOCK_READ,
								HUGE_STRING_LEN)) != APR_SUCCESS)
    {
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
					 "mod_jaxer: can't get data from http client");
		if (bb)
			apr_brigade_destroy(bb);
		return HTTP_INTERNAL_SERVER_ERROR;
	}

    b = APR_BRIGADE_FIRST(bb);
    if (!APR_BUCKET_IS_EOS(b))
    {
        postdata = (postdata_ctx*)apr_pcalloc(r->pool, sizeof(postdata_ctx));
        postdata->pool = r->pool;
        apr_pool_cleanup_register(postdata->pool, postdata, jaxer_postdata_cleanup, apr_pool_cleanup_null);
        while (!eos_seen)
        {
            rv = jxr_get_postdata_from_brigade(r, bb, postdata, &eos_seen);
            if (rv != APR_SUCCESS)
            {
                apr_brigade_destroy(bb);
                return rv;
            }
            apr_brigade_cleanup(bb);
            if(!eos_seen && (rv = ap_get_brigade(r->input_filters, bb,
								AP_MODE_READBYTES,
								APR_BLOCK_READ,
								HUGE_STRING_LEN)) != APR_SUCCESS)
            {
		        ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
					         "mod_jaxer: can't get data from http client");
		        apr_brigade_destroy(bb);
		        return HTTP_INTERNAL_SERVER_ERROR;
	        }

        }
        apr_brigade_destroy(bb);

        if (postdata->err == 0)
        {
            //break the large buckets
            if(postdata->bytes_read==0 && postdata->bb)
            {
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: destroying empty postdata bb.");
                apr_brigade_destroy(postdata->bb);
                postdata->bb = 0;
            }
            if(postdata->bb)
            {
                apr_bucket_brigade *dest = apr_brigade_create(r->pool, r->connection->bucket_alloc);
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: breaking large postdata buckets.");
                
                rv = jxr_append_brigade(r, dest, postdata->bb, 0);
                if (rv != APR_SUCCESS)
                    return rv;
                postdata->bb = dest;
            }
        }
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: Collected postdata: len=%d, has_err=%d, has_bb=%d, has_file=%d",
            postdata->bytes_read, postdata->err, postdata->bb ? 1:0, postdata->tmpfile ? 1:0 );
    }
	/* Bridge the request */
	return jxr_handle_request(r, req_hdr_msg, env_msg, postdata);
}

apr_status_t jxr_init_worker(jaxer_worker *aworker, apr_pool_t *p, int min_size, int max_size, int acquire_conn_timeout)
{
	apr_status_t rv;
	int is_threaded;
    int mpm_threads = 1;
	
	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: initializing jaxerworker");

	rv = apr_pool_create(&aworker->pool, p);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: Failed to create subpool for jaxerworker");
		return rv;
	}

	rv = apr_pool_create(&aworker->res_pool, p);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: Failed to create res pool for jaxerworker");
		return rv;
	}


	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: created pools for worker");

#if APR_HAS_THREADS
	rv = apr_thread_mutex_create (&aworker->mutex, APR_THREAD_MUTEX_DEFAULT, p);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: Failed to create mutex for jaxerworker ");
		return rv;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: mutex created for jaxerworker");

#endif

	 /* Set default connection cache size for multi-threaded MPMs */
	if (ap_mpm_query(AP_MPMQ_IS_THREADED, &is_threaded) == APR_SUCCESS &&
		is_threaded != AP_MPMQ_NOT_SUPPORTED)
	{
        	if (ap_mpm_query(AP_MPMQ_MAX_THREADS, &mpm_threads) != APR_SUCCESS)
			mpm_threads = 1;
	}

	// ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: number of threads=%d for %s", mpm_threads, aworker->name);

	/*
	 * cache_size >= min_connections && cache_size <= mpm_threads
	 * min_connections > mpm_threads/10 
	 */

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: jaxerworker connection pool config: (min=%d max=%d)",
		min_size, max_size);

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

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: using jaxerworker connection pool config: (min=%d max=%d acquire-timeout=%d)",
		aworker->nmin, aworker->nmax, aworker->acquire_timeout);


	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: getting socket address for jaxerworker (host=%s port=%d)",
		aworker->hostname, aworker->port_number);

	rv = apr_sockaddr_info_get(&aworker->remote_sa, aworker->hostname, APR_INET, aworker->port_number, 0, p);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p,
				"mod_jaxer: apr_sockaddr_info_get failed with remote address %s and ip %d: return code=%d", aworker->hostname, aworker->port_number, rv);
		return rv;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: call jxr_conn_setup for worker");

	if ((rv = jxr_conn_setup(aworker)) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p,
				"mod_jaxer: jxr_conn_setup failed for worker");
		return rv;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: initialized worker");

	return rv;
}

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
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: turn on non-blocking failed");
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
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rc, r, "mod_jaxer: socket select failed");
		ac->has_error = 1;
		return rc;
	}
	
	
	/* turn off non-blocking */
	rv = apr_socket_opt_set(sock, APR_SO_NONBLOCK, 0);
	if (rv != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: turn off non-blocking failed");
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
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: receive message failed");
		return rv;
	}
	return rv;
}

#if 0
apr_status_t jxr_process_file_request(jaxer_connection *ac, const char* uri)
{
	/* construct the filename */
	apr_status_t rv;
    request_rec *r = ac->request;
	apr_file_t *fd;
    char * filepath;
	apr_finfo_t *finfo;

    int doc_status = 200; // OK
    
    request_rec* rr;

    /*
     * Make sure the URI is not empty
     */
    if ( !uri || ! *uri)
    {
        doc_status = 400; /* BAD REQUEST */
        ap_log_rerror(APLOG_MARK, APLOG_WARNING, APR_EBADPATH, r,
                     "mod_jaxer: null or empty uri specified while requesting document");
    }
    
    if (doc_status == 200)
        rr = ap_sub_req_lookup_uri(uri, r, NULL);

    if (doc_status == 200 && !rr)
    {
        doc_status = 400;
        ap_log_rerror(APLOG_MARK, APLOG_WARNING, APR_EBADPATH, r,
                     "mod_jaxer: lookup uri returned null request for uri '%s'", uri);
    }
    
    if (doc_status == 200)
    {
        filepath = rr->filename;
        finfo = &rr->finfo;
        // fd = finfo->filehand;
        // ap_set_module_config(rr->request_config, &jaxer_module, r);
        // rv = ap_run_sub_req(rr);
    }

    if (doc_status == 200 && !filepath)
    {
        doc_status = 400;
        ap_log_rerror(APLOG_MARK, APLOG_WARNING, APR_EBADPATH, r,
                     "mod_jaxer: request doc uri (%s) resulted in null filepath", uri);
    }

#if 0    
    if (!filepath)
    {
        ap_log_error(APLOG_MARK, APLOG_WARNING, APR_EBADPATH, r->server,
                     "mod_jaxer: invalid file path %s", filepath);
        return APR_EBADPATH;
    }

#endif

    if (doc_status == 200 && finfo->filetype != APR_REG)
    {
        doc_status = 403; /* Forbidden */

        ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r,
            "mod_jaxer: requested doc (%s) isn't a regular file.", filepath);
    }   

    if (doc_status == 200 &&
        (rv = apr_file_open(&fd, filepath, APR_READ | APR_BINARY | APR_XTHREAD,
                       APR_OS_DEFAULT, r->pool)) != APR_SUCCESS)
    {
       doc_status = 401; /* Unauthorized */
       ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r,
            "mod_jaxer: cannot open requested doc (%s)=>(%s)", uri, filepath);
    }


    /* send status */
    {
        char msg[10];
        apr_size_t pos;
        jxr_msg_init(msg, &pos, BLOCKTYPE_DOC_STATUS);
        jxr_msg_append_int16(msg, &pos, doc_status);
        if ((rv = jxr_send_message(ac, msg)) != APR_SUCCESS)
        {
            ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
                         "mod_jaxer: failed to send doc status to jaxer");
            return rv;
        }
    }

    if (doc_status == 200)
    {
        apr_off_t remain_bytes = finfo->size;
        char buf[MAX_PACKET_SIZE];
	    apr_size_t max_bytes = MAX_PACKET_SIZE - sizeof (Jaxer_Header);
	    apr_size_t nbytes  = max_bytes;
    
	    while (remain_bytes>0)
	    {
		    nbytes = max_bytes;
            if (nbytes > remain_bytes)
                nbytes = (apr_size_t) remain_bytes;
		    if ((rv =  apr_file_read (fd, buf, &nbytes)) != APR_SUCCESS)
		    {
			    ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, 
				    "mod_jaxer: failed to read data from file %s", filepath);
			    return rv;
		    }

		    remain_bytes -= nbytes;

		    /* send the data with a header constructed */
		    if ((rv = jxr_send_message_add_header(ac, buf, (int) nbytes,bt_Document)) != APR_SUCCESS)
		    {
			    ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, 
				    "mod_jaxer: failed to send data from file %s", filepath);
			    return rv;
		    }
	    }
    
	    if ((rv = apr_file_close (fd)) != APR_SUCCESS)
	    {
		    ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, ac->request, "mod_jaxer: failed to close file %s", filepath);
		    return rv;
	    }
    }

    /* at this point we can destroy rr */

    /* do not foget the end req msg */
    if ((rv = jxr_send_message_add_header(ac, 0, 0, bt_EndRequest)) != APR_SUCCESS) 
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send end request for postdata error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	return rv;
}
#endif

apr_status_t jxr_send_postdata(jaxer_connection* ac, postdata_ctx* postdata, int should_add_end_request)
{
	apr_status_t rv = APR_SUCCESS;
	request_rec *r = ac->request;
	server_rec *main_server = r->server;
    
	if(postdata && postdata->bb)
	{
		rv = jxr_send_brigade_add_header(ac, postdata->bb, bt_PostData);
        apr_brigade_destroy(postdata->bb);
        postdata->bb = 0;
        if (rv != APR_SUCCESS) 
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send postdata to jaxer error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}
	}else if (postdata&& postdata->tmpfile)
    {
        const char *filepath;
        
        rv = jxr_send_file_add_header(ac, postdata->tmpfile, bt_PostData);
        rv = apr_file_name_get(&filepath, postdata->tmpfile); 
        apr_file_close(postdata->tmpfile);
        apr_file_remove(filepath, r->pool);
        postdata->tmpfile = 0;
        if (rv != APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send postdata to jaxer error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}

    }else
	{
		// Send empty postdata
		if ((rv = jxr_send_message_add_header(ac, 0, 0, bt_PostData)) != APR_SUCCESS) 
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send postdata to jaxer error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}
	}

    /* Send end of req */

	if (should_add_end_request &&
        (rv = jxr_send_message_add_header(ac, 0, 0, bt_EndRequest)) != APR_SUCCESS) 
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send end request for postdata error");
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
    const char * rh = ap_get_remote_host(r->connection, config, REMOTE_NAME, &is_ip);

    if (ru)
        apr_table_setn(env, "REMOTE_USER", ru);
    // else
    //    apr_table_setn(env, "REMOTE_USER", "");

    if (rh) // && !is_ip)
        apr_table_setn(env, "REMOTE_HOST", rh);

    apr_table_setn(env, "STATUS_CODE", apr_psprintf(r->pool, "%d", r->status));

    ssl_on = jaxer_conn_is_https(r->connection);
    apr_table_setn(env, "HTTPS", apr_pstrdup(r->pool, (ssl_on != 0)? "on" : "off" ));
}

apr_status_t jxr_create_and_open_tmp_file(request_rec* r, apr_file_t **tmpfile)
{
    apr_status_t rv = APR_SUCCESS;

    static int first_time = 1;
    const char *temp_dir;
    char *template;

    ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r,
                     "mod_jaxer: in jxr_create_and_open_tmp_file.");

    rv = apr_temp_dir_get(&temp_dir, r->server->process->pool);
    if (rv != APR_SUCCESS)
    {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r,
                     "mod_jaxer: search for temporary directory failed");
        return rv;
    }
    if (first_time)
    {
        ap_log_error(APLOG_MARK, APLOG_INFO, rv, r->server, "mod_jaxer: large postdata will be saved in tmpdir; %s", temp_dir);
        first_time = 0;
    }else
    {
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: large postdata will be saved in tmpdir; %s", temp_dir);
    }

    apr_filepath_merge(&template, temp_dir,
                       "mod_jaxer.tmp.XXXXXX",
                       APR_FILEPATH_NATIVE, r->pool);
    rv = apr_file_mktemp(tmpfile, template, 0, r->pool);
    ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: tmpfile=%s", template);
    if (rv != APR_SUCCESS)
    {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r,
                     "mod_jaxer: creation of temporary file in directory %s failed",
                     temp_dir);
        return rv;
    }
    ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: tmpfile created/opened");
    return rv;
}

apr_status_t jxr_save_brigade_data_into_file(apr_pool_t* pool, apr_file_t *file, apr_bucket_brigade* bb,
                                             apr_off_t *bytes_read, int *seen_eos)
{
    apr_bucket *b;
    apr_status_t rv = APR_SUCCESS;

    ap_log_perror(APLOG_MARK, APLOG_DEBUG, rv, pool, "mod_jaxer: in jxr_save_brigade_data_into_file");

    if (bytes_read)
        *bytes_read = 0;

    if (seen_eos)
        *seen_eos = 0;

    for (b = APR_BRIGADE_FIRST(bb);
         b != APR_BRIGADE_SENTINEL(bb);
         b = APR_BUCKET_NEXT(b)) 
    {
        const char *data;
        apr_size_t bread, bwritten;

        if (APR_BUCKET_IS_EOS(b))
        {
            if (seen_eos)
                *seen_eos = 1;
            ap_log_perror(APLOG_MARK, APLOG_DEBUG, rv, pool, "mod_jaxer: bucket eos seen.");
            return APR_SUCCESS;
        }
        apr_bucket_read(b, &data, &bread, APR_BLOCK_READ);
        rv = apr_file_write_full(file, data, bread, &bwritten);
        if (rv != APR_SUCCESS)
        {
            const char *filename;

            if (apr_file_name_get(&filename, file) != APR_SUCCESS)
            {
                filename = "(unknown)";
            }
            ap_log_perror(APLOG_MARK, APLOG_ERR, rv, pool,
                         "mod_jaxer: write to temporary file %s failed",
                         filename);
            return rv;
        }
        AP_DEBUG_ASSERT(bread == bwritten);
        ap_log_perror(APLOG_MARK, APLOG_DEBUG, rv, pool, "mod_jaxer: read %d bytes. written %d bytes", bread, bwritten);
        if (bytes_read)
            *bytes_read += bwritten;
    }
    return rv;
}

apr_status_t jxr_get_postdata_from_brigade(request_rec* r, apr_bucket_brigade *bb, postdata_ctx *ctx, int *eos_seen)
{
    // just make a copy

    apr_status_t rv = APR_SUCCESS;
    //apr_pool_t *request_pool = r->main ? r->main->pool : r->pool;
    
    apr_off_t bytes, bytes_read;
    apr_bucket *b;

    jaxer_server_conf *config =
		ap_get_module_config(r->server->module_config, &jaxer_module);
    apr_off_t memory_limit = config->max_postdata_memory;
    
    if(eos_seen)
        *eos_seen = 0;

	apr_brigade_length(bb, 1, &bytes);
    if (memory_limit>0 && (ctx->tmpfile || (ctx->bytes_read + bytes > memory_limit)) )
    {
        if (ctx->tmpfile == NULL)
        {
            rv = jxr_create_and_open_tmp_file(r, &ctx->tmpfile);
            if (rv != APR_SUCCESS)
            {
                return rv;
            }
            //Move the bb data we have saved into file first
            if (ctx->bb)
            {
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: move brigade data into file");
                rv = jxr_save_brigade_data_into_file(r->pool, ctx->tmpfile, ctx->bb, &bytes_read, 0);
                AP_DEBUG_ASSERT(bytes_read == ctx->bytes_read);
                if (rv != APR_SUCCESS)
                {
                    return rv;
                }
                apr_brigade_destroy(ctx->bb);
                ctx->bb = 0;
            }
        }

        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: save new brigade data into file");
        rv = jxr_save_brigade_data_into_file(r->pool, ctx->tmpfile, bb, &bytes_read, eos_seen);
        if (rv != APR_SUCCESS)
        {
            return rv;
        }
        ctx->bytes_read += bytes_read;
    }else
    {

        if (!ctx->bb)
            ctx->bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
        if (!ctx->bb)
        {
            ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r, "mod_jaxer: apr_brigade_create failed.");
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: save new brigade data into mod_jaxer brigade");
	    /* Copy the buckets onto our brigade. */
	    for (b = APR_BRIGADE_FIRST(bb); b != APR_BRIGADE_SENTINEL(bb); b = APR_BUCKET_NEXT(b))
	    {
		    apr_bucket *e = 0;

		    if (APR_BUCKET_IS_EOS(b)) {
			    e = apr_bucket_eos_create(r->connection->bucket_alloc);
			    APR_BRIGADE_INSERT_TAIL(ctx->bb, e);
                *eos_seen = 1;
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: bucket eos seen.");
                ctx->bytes_read += bytes;
			    return APR_SUCCESS;
		    } else if (APR_BUCKET_IS_METADATA(b)) {
			    /* Just ignore metadata. */
		    } else {
			    if ((rv=apr_bucket_copy(b, &e)) == APR_SUCCESS)
				    APR_BRIGADE_INSERT_TAIL(ctx->bb, e);
			    else
                {
                    ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, "mod_jaxer: failed to copy bucket.");
				    ctx->err = 1;
                }
		    }
	    }
        ctx->bytes_read += bytes;
    }
    return APR_SUCCESS;
}


apr_status_t jaxer_postdata_cleanup(void* ctx)
{
    apr_status_t rv = APR_SUCCESS;
    postdata_ctx *postdata = (postdata_ctx*) ctx;

    if (postdata->bb)
    {
        apr_brigade_destroy(postdata->bb);
        postdata->bb = 0;
    }

    if (postdata->tmpfile)
    {
        const char* filename;
        rv = apr_file_name_get(&filename, postdata->tmpfile);
        apr_file_close(postdata->tmpfile);
        if (rv == APR_SUCCESS)
            apr_file_remove(filename, postdata->pool);
        postdata->tmpfile = 0;
    }

    return rv;

}

int jxr_is_debug_request(request_rec * r, char** jaxer_id)
{
#if 1
	apr_table_t *t = r->headers_in;
	const char *DebugHeader = "Jaxer-ID";
	const char *value = apr_table_get(t, DebugHeader);
	if (!value || ! *value)
		return 0;

	*jaxer_id = apr_pstrdup(r->pool, value);
	return 1;
#else
	// TMP FOR MAX TO TEST.  NEED TO CHANGE BACK
	apr_table_t *t = r->subprocess_env;
	const char *DebugHeader = "Jaxer-ID=";
	const char *qs = r->args;
	const char *p = 0;
	char *q;
	if (!qs || ! *qs) return 0;
	p = strstr(qs, DebugHeader);
	if (!p) return 0;
	*jaxer_id = apr_pstrdup(r->pool, p+strlen(DebugHeader));
	q = strchr(*jaxer_id, '&');
	if (q) *q = 0;
	return 1;
#endif
}

int is_apache_dummy_request(request_rec *r)
{
    char *ua = apr_table_get(r->headers_in, "User-Agent");
    if (!ua)
        return 0;
    if (ap_strstr(ua, "internal dummy connection") != 0)
        return 1;
    return 0;
}

