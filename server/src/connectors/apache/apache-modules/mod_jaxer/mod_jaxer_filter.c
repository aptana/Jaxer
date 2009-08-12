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
#include "http_request.h"
#include "http_config.h"
#include "http_log.h"
#include "mod_jaxer_filter.h"
#include "mod_jaxer_conf.h"
#include "util_script.h"
#include "apr_pools.h"
#include "mod_jaxer_protocol.h"
#include "apr_strings.h"
#include "apr_buckets.h"
#include "mod_jaxer_proc.h"


#include "mod_jaxer_message.h"
#include "mod_jaxer_connection.h"

/*
 * APR does not have select() method, so we have some platform-specific code.
 * So watch out if it does not work on Unix
 */

/* If the status is one of the following then bypass jaxer */
static int skip_jaxer_status[] =
{
    204, /* no content */
     /* 304, Not Modified */
    400, /* Bad Request */
    401, /* Unauthorized */
    402, /* Payment Required */
    403, /* Forbidden */
    404, /* Not Found */
    405, /* Method Not Allowed */
    406, /* Not Acceptable */
    407, /* Proxy Authentication Required */
    408, /* Request Timeout */
    409, /* Conflict */
    410, /* Gone */
    411, /* Length Required */
    412, /* Precondition Failed */
    413, /* Request Entity Too Large */
    414, /* Request-URI Too Long */
    415, /* Unsupported Media Type */
    416, /* Requested Range Not Satisfiable */
    417, /* Expectation Failed */
    500, /* Internal Server Error */
    501, /* Not Implemented */
    502, /* Bad Gateway */
    503, /* Service Unavailable */
    504, /* Gateway Timeout */
    505, /* HTTP Version Not Supported */
    -1 /* terminate */
};

typedef struct out_ctx {
	char *req_hdr_msg;
    char *resp_hdr_msg;
	char *env_msg;
	//apr_bucket_brigade *post_bb;
	apr_bucket_brigade *body_bb;
	Jaxer_Header end_req;
	int eos_seen;
	int postdata_sent;
	//int has_postdata;
    postdata_ctx *postdata;
	jaxer_worker* worker;
	jaxer_connection *ac;
} out_ctx;

// static apr_status_t jxr_send_postdata_if_asking_for(out_ctx* ctx, char** pbuf);
static apr_status_t jxr_send_document_brigade_to_jaxer(jaxer_connection *ac, out_ctx* ctx);

apr_status_t jaxer_in_filter(ap_filter_t *f, apr_bucket_brigade *bb, ap_input_mode_t mode,
		                     apr_read_type_e block, apr_off_t readbytes)
{
	request_rec *r = f->r;
	postdata_ctx *ctx;
	apr_bucket *b;
	apr_status_t rv;
    apr_off_t bytes_read = 0;
    //apr_off_t bytes;
    int eos_seen = 0;

	/* All we need to do is keep a copy of the buckets that pass through us and put
	 * them where our output filter can find them.  The brigade we'll use to hold the
	 * copy is our context.
	 */

    /* Pull data from next filter in line. */
	rv = ap_get_brigade(f->next, bb, mode, block, readbytes);
	if (rv != APR_SUCCESS)
    {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, "mod_jaxer: ap_get_brigade failed to pull data from next filter.");
		return rv;
    }

	if (!f->ctx)
	{
        // If there is no postdata, just remove us.
        b = APR_BRIGADE_FIRST(bb);
        if (APR_BUCKET_IS_EOS(b))
        {
            ap_remove_input_filter(f);
            return APR_SUCCESS;
        }
        
		/* Setup our context. */
		ctx = (postdata_ctx*) apr_pcalloc(r->pool, sizeof(postdata_ctx));
        if (!ctx) 
	    {
		    ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r, "mod_jaxer: can't alloc memory for postdata_ctx");
		    return HTTP_INTERNAL_SERVER_ERROR;
	    }
        ctx->pool = r->pool;
		f->ctx = ctx;
        apr_pool_cleanup_register(r->pool, ctx, jaxer_postdata_cleanup, apr_pool_cleanup_null);
	}
	ctx = f->ctx;

    rv = jxr_get_postdata_from_brigade(r, bb, ctx, &eos_seen);
    if (rv != APR_SUCCESS)
		return rv;
    if (eos_seen)
    {
        if (ctx->err == 0)
        {
            //break the large buckets
            if(ctx->bb && ctx->bytes_read == 0)
            {
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: destroy empty postdata bb.");
                apr_brigade_destroy(ctx->bb);
                ctx->bb = 0;
            }
            if(ctx->bb)
            {
                apr_bucket_brigade *dest = apr_brigade_create(r->pool, r->connection->bucket_alloc);
                if (!dest)
                {
                    ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r, "mod_jaxer: apr_brigade_create failed to create dest bb.");
                    return HTTP_INTERNAL_SERVER_ERROR;
                }
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: break large postdata buckets.");
                rv = jxr_append_brigade(r, dest, ctx->bb, 0);
                if (rv != APR_SUCCESS)
                    return rv;
                ctx->bb = dest;
            }
            apr_table_setn(r->notes, "jaxer-post-data", (char *)ctx);
        }
	    ap_remove_input_filter(f);
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: collected postdata: len=%d, has_err=%d, has_bb=%d, has_file=%d",
            ctx->bytes_read, ctx->err, ctx->bb ? 1:0, ctx->tmpfile ? 1:0);
	    return APR_SUCCESS;
    }

	return APR_SUCCESS;
}


static apr_status_t jaxer_outctx_cleanup(void* ctx)
{
    out_ctx *octx = (out_ctx*) ctx;

    if (octx->body_bb) apr_brigade_destroy(octx->body_bb);

    return jaxer_connection_cleanup(octx->ac);

}

static apr_status_t jaxer_out_filter_init(ap_filter_t *f)
{
	out_ctx *ctx;
	request_rec *r = f->r;
	apr_status_t rv;
    postdata_ctx *ictx = (postdata_ctx*)apr_table_get(r->notes, "jaxer-post-data");
	//apr_bucket_brigade *post_data = ictx->bb;
	apr_off_t post_data_len = 0;
	server_rec *main_server = f->r->server;

	jaxer_worker *w = get_jaxer_worker(main_server);
#pragma message("TODO: make sure w is not NULL")
	
	ctx = apr_pcalloc(r->pool, sizeof(out_ctx));
	if (!ctx) 
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r, "mod_jaxer: can't alloc memory for out_ctx");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

    ctx->postdata = ictx;
	ctx->worker = w;
	f->ctx = ctx;
	apr_pool_cleanup_register(r->pool, ctx, jaxer_outctx_cleanup, apr_pool_cleanup_null);
	
	
	// First, see if we can get a connection, and establish the protocol before we go too far
	if ((rv = jxr_conn_open(ctx->worker, &ctx->ac, r, eRequestType_Filter))!= APR_SUCCESS &&
		(!ctx->worker->retry || (rv = jxr_conn_open(ctx->worker, &ctx->ac, r, eRequestType_Filter))!= APR_SUCCESS))
	{
		// proc_close_socket(r->server, ctx->ac);
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: can't connect to socket");
		return HTTP_SERVICE_UNAVAILABLE;
	}

	
	//END BLOCK
	jxr_init_header(bt_EndRequest, 0, &ctx->end_req);

	ap_add_common_vars(r);
	ap_add_cgi_vars(r);
	apr_table_unset(r->subprocess_env, "GATEWAY_INTERFACE");
	jaxer_add_env_vars(r);
    apr_table_setn(r->subprocess_env, "JAXER_REQ_TYPE", apr_pstrdup(r->pool, "2"));

    // This helps Jaxer to determine the parser it will use.  CONTENT_TYPE has been taken.
    apr_table_setn(r->subprocess_env, "DOC_CONTENT_TYPE", apr_pstrdup(r->pool, r->content_type));
    
    // Postdata is already in ctx->postdata


	// Brigade for DOCUMENT BODY.  The buckets will have the raw body only w/o PACKET HEADER.	
	ctx->body_bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
	if (!ctx->body_bb) 
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r, "mod_jaxer: can't alloc memory for output (body) brigade");
		return HTTP_INTERNAL_SERVER_ERROR;
	}


	if ((rv =jxr_build_request_header_msg(r, &(ctx->req_hdr_msg), eREQUEST_HEADER) ) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r,
						 "mod_jaxer: construct HTTP request header message failed");
		return rv;
	}

    if ((rv =jxr_build_request_header_msg(r, &(ctx->resp_hdr_msg), eRESPONSE_HEADER) ) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r,
						 "mod_jaxer: construct HTTP response header message failed");
		return rv;
	}
	
	if ((rv =jxr_build_request_header_msg(r, &ctx->env_msg, eENVIRON_HDR) ) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r,
						 "mod_jaxer: construct environment variables message failed");
		return rv;
	}

	ctx->eos_seen = 0;
	ctx->postdata_sent = 0;
	return APR_SUCCESS;
}

apr_status_t jaxer_out_filter(ap_filter_t *f, apr_bucket_brigade *bb)
{
	request_rec *r = f->r;
	apr_status_t rv;
	out_ctx *ctx;
	server_rec *main_server = f->r->server;
	const char *location;
	unsigned char *buf = 0;
	int got_a_message = 0;
	jaxer_connection *ac;
	int done = 0;
    int should_redirect = 0;

	/* Do one-time setup for the current request */
	if (!f->ctx) 
	{
		out_ctx *oc;
		/* Bypass Jaxer if the content isn't HTML or this is a subrequest */
		if (r->main /*|| r->content_type && strcmp(r->content_type, "text/html")*/) {
			ap_remove_output_filter(f);
			return ap_pass_brigade(f->next, bb);
		}
		
		/* Also bypass if a redirection has been requested. */
		location = apr_table_get(r->headers_out, "Location");
		if (location) {
			ap_remove_output_filter(f);
			return ap_pass_brigade(f->next, bb);
		}

        /*
         * Check if we should skip jaxer.
         * Note that some of the checks might be redundant.
        */

        /*
         * Do not filter if we are told not to, or if we are
         * already the handler.
         */
        if (get_is_pass_through(r) != 0 ||
            (r->handler && strcmp(r->handler, "jaxer") == 0))
        {
            ap_remove_output_filter(f);
            return ap_pass_brigade(f->next, bb);
        }

        {
            // if we do not have a body, do not bother jaxer
            apr_off_t length = 0;
            int k;
            rv = apr_brigade_length(bb, 1, &length);
            if (rv != APR_SUCCESS || (r->method_number != M_OPTIONS && length <= 0))
            {
                ap_remove_output_filter(f);
                return ap_pass_brigade(f->next, bb);
            }

            // do not send the folowing status code to jaxer
            k=0;
            while(skip_jaxer_status[k] > 0)
            {
                if (r->status == skip_jaxer_status[k])
                {
                    ap_remove_output_filter(f);
                    return ap_pass_brigade(f->next, bb);
                }
                k++;
            }

            // ignore dummy request
            if (is_apache_dummy_request(r))
            {
                ap_remove_output_filter(f);
                return ap_pass_brigade(f->next, bb);
            }

        }

        /* We will handle this only if this is one of the content types configured.
         * But text/html is always one of them.
         */
        if ((r->content_type == NULL || !should_filter_content_type(r->content_type, r)))
        {
            // if people forgot about setting JaxerFilterContentType,
            // we still do not want block the text/html docs
            
            if (strcmp(r->content_type, "text/html") && strncmp(r->content_type, "text/html;", strlen("text/html;")))
            {
                ap_remove_output_filter(f);
                return ap_pass_brigade(f->next, bb);
            }
        }

        /*
         * We should not pass the content from these handlers to jaxer
         */
        if (r->handler != NULL && should_bypass_handler(r->handler, r))
        {
            ap_remove_output_filter(f);
            return ap_pass_brigade(f->next, bb);
        }


		/* Get setup out of the way */
		rv = jaxer_out_filter_init(f);
		if (rv != APR_SUCCESS) {
			ap_remove_output_filter(f);
			return rv;
		}

		oc = f->ctx;

		// SEND REQUEST HEADER
		if ((rv = jxr_send_message(oc->ac, oc->req_hdr_msg)) != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: send request headers to jaxer server failed");
			oc->ac->has_error = 1;
			return HTTP_INTERNAL_SERVER_ERROR;
		}

        // SEND RESPONSE HEADER
		if ((rv = jxr_send_message(oc->ac, oc->resp_hdr_msg)) != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: send response headers to jaxer server failed");
			oc->ac->has_error = 1;
			return HTTP_INTERNAL_SERVER_ERROR;
		}

		// SEND ENV HEADER
		if ((rv = jxr_send_message(oc->ac, oc->env_msg)) != APR_SUCCESS) 
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: send environment vars to jaxer server failed");
			oc->ac->has_error = 1;
			return HTTP_INTERNAL_SERVER_ERROR;
		}
	}

	ctx = f->ctx;
	ac = ctx->ac;
	


	/* Move incoming stream onto output brigade.  It does NOT have the packet header yet, but has room for that */
    apr_brigade_cleanup(ctx->body_bb);
	rv = jxr_append_brigade(r, ctx->body_bb, bb, &ctx->eos_seen);
	if (rv != APR_SUCCESS)
	{
		ac->has_error = 1;
		ap_remove_output_filter(f);
		return rv;
	}

	/*
	 * Send data to jaxer...
	 */


	if (r->method_number == M_OPTIONS)
	{
		/* Send an empty body toJaxer */
		if ((rv = jxr_send_message_add_header(ac, 0, 0,bt_Document)) != APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send empty doc to jaxer error");
			ac->has_error = 1;
			return HTTP_INTERNAL_SERVER_ERROR;
		}
		if ((rv = jxr_send_message_add_header(ac, 0, 0, bt_EndRequest)) != APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: write end of (doc) request to jaxer server failed");
			ac->has_error = 1;
			return HTTP_INTERNAL_SERVER_ERROR;
		}

	}else if((rv=jxr_send_document_brigade_to_jaxer(ac, ctx)) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: write docuemnt ot jaxer error");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}


	if (!ctx->eos_seen)
		return APR_SUCCESS;
	
	/* Last chance for requesting anything */
	
    got_a_message = 0;

	while (!got_a_message)
	{
		char msg_type;
        
		if ((rv = jxr_receive_message(ac, &buf)) != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: check for request message failed");
			ac->has_error = 1;	
			return rv;
		}
        
        msg_type = jxr_msg_get_type(buf);
		switch ( msg_type)
		{
			// char uri[MAX_STRING_LEN];
			// apr_size_t pos, len;
		case bt_RequirePostData:
			if (ctx->postdata_sent)
			{
				ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, r->pool, "mod_jaxer: requesting postdata again");
				ac->has_error = 1;	
				return rv;
			}
			if ((rv = jxr_send_postdata(ctx->ac, ctx->postdata, 1)) != APR_SUCCESS)
			{
				ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, r->pool, "mod_jaxer: error while sending postdata");
				ac->has_error = 1;	
				return rv;
			}
			ctx->postdata_sent = 1;
			break;

        case bt_ResponseHeader:
            got_a_message = 1;
            break;

		default:
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: invalid request (type=%d) received", msg_type);
			ac->has_error = 1;
            return (!APR_SUCCESS);
			break;
		}
	}

	/* Now we got a message.  This has to be a header msg */
	if ((rv=jxr_process_response_headers(ac, buf)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: parse response header failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}

    /* Check redirect */
	location = apr_table_get(r->headers_out, "Location");

    if (location && location[0])
    {
        should_redirect = 1;
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: request (status=%d) will be redirected to %s",
            r->status, location);
    }

	// The output will reuse body_bb.
	if ((rv=apr_brigade_cleanup(ctx->body_bb)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: empty body_bb failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
    if ((rv=jxr_process_response_body(ac, should_redirect ? NULL : ctx->body_bb)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: parse response body failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	if (should_redirect) 
	{
        const static char *buf="<html>Jaxer redirect</html>";
        char s[10];
		apr_bucket* bucket = apr_bucket_pool_create(buf, strlen(buf),
                     r->pool, r->connection->bucket_alloc);
        APR_BRIGADE_INSERT_TAIL(ctx->body_bb, bucket);
		bucket = apr_bucket_eos_create(r->connection->bucket_alloc);
	    APR_BRIGADE_INSERT_TAIL(ctx->body_bb, bucket);

        apr_table_clear(r->headers_out);
		
        apr_table_clear(r->err_headers_out);
        apr_table_set(r->headers_out, "Location", location);
        apr_table_unset(r->headers_in, "Content-Length");
        
        
        sprintf(s,"%d", strlen(buf));
        apr_table_setn(r->headers_out, "Content-Length", s);

        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: redirecting request to %s", location);
    }

    /* This filter is done once it has served up its content */
    ap_remove_output_filter(f);
	
    rv = ap_pass_brigade(f->next, ctx->body_bb);
    // apr_brigade_cleanup(ctx->body_bb);
    return rv;
}


static apr_status_t jxr_send_document_brigade_to_jaxer(jaxer_connection *ac, out_ctx* ctx)
{
	/*
	 * Setup non-blocking socket, linstening for incoming request.
	 * If requesting post data or requseting file, then send post data or file before continuing.
	 */

	request_rec *r = ac->request;
	server_rec *main_server = r->server;
	
	apr_status_t rv;
	apr_bucket *bucket;
	char *buf = 0;
	apr_bucket_brigade *bbb = ctx->body_bb;

    int done = 0;

	for (bucket = APR_BRIGADE_FIRST(bbb);
		 !done; /* bucket != APR_BRIGADE_SENTINEL(bbb);*/
		 bucket = APR_BUCKET_NEXT(bucket)) 
	{
		char *write_buf;
		apr_size_t write_buf_len;
		
        /* It will be done if we see bucket == APR_BRIGADE_SENTINEL(bbb).
         * At this point, if this is the last brigade, we send end req.
         */
        done = (bucket == APR_BRIGADE_SENTINEL(bbb));
        if (done && !ctx->eos_seen)
        {
            /* we should not proceed */
            break;
        }

		if (APR_BUCKET_IS_EOS(bucket))
			break;

		if (APR_BUCKET_IS_FLUSH(bucket))
			continue;

        

		/* check to see if we have incoming data */
		if ((rv = jxr_get_request_if_any(ctx->ac, &buf)) != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: error in jxr_get_request_if_any");
			return rv;
		}

		/* Let's see what we have here */
		if (buf && buf[0])
		{
			// char uri[MAX_STRING_LEN];
			// apr_size_t pos, len;
				
			/* we got a message */
			char msg_type = jxr_msg_get_type(buf);
			switch (msg_type)
			{
			case bt_RequirePostData:
				if (ctx->postdata_sent)
				{
                    ap_log_perror(APLOG_MARK, APLOG_ERR, 0, r->pool, "mod_jaxer: requesting postdata after it has been sent");
					/* log error */
					return (!APR_SUCCESS);
				}
				if ((rv = jxr_send_postdata(ctx->ac, ctx->postdata, 0)) != APR_SUCCESS)
				{
					ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: error in while sending post data to jaxer");
					return rv;
				}
				ctx->postdata_sent = 1;
				break;

			default:
				ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, r->pool, 
                      "mod_jaxer: invalid request (type=%d) received from jaxer while sending document to it", msg_type);
				return (!APR_SUCCESS);
				break;
			}
		}			

        if (!done)
        {
		    /* Send a message (body) to jaxer */
		    if ((rv = apr_bucket_read(bucket, (const char **)&write_buf, &write_buf_len,
							     APR_BLOCK_READ)) != APR_SUCCESS) {
			    ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool,
						     "mod_jaxer: can't read request from bucket");
			    return rv;
		    }

		    if((rv=jxr_send_message_add_header(ctx->ac, write_buf, (int)write_buf_len,bt_Document)) != APR_SUCCESS)
		    {
			    ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: error in sending a document message");
			    return rv;
		    }
        }else
        {
            /* send end req */
            if ((rv = jxr_send_message(ctx->ac, (unsigned char*) &ctx->end_req)) != APR_SUCCESS)
	        {
		        ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: send end of request to jaxer server failed");
		        ac->has_error = 1;
		        return rv;
	        }
        }

	}
	return APR_SUCCESS;
}
