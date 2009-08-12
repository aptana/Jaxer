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
#include "mod_jaxer_conf.h"
#include "util_script.h"
#include "apr_pools.h"
#include "mod_jaxer_protocol.h"
#include "apr_strings.h"
#include "apr_buckets.h"
#include "mod_jaxer_proc.h"

#include "mod_jaxer_message.h"
#include "mod_jaxer_connection.h"
#include "compat.h"

typedef struct out_ctx {
	char *hdr_msg;
	char *env_msg;
	Jaxer_Header end_req;
	int postdata_sent;
	int has_postdata;
	jaxer_worker* worker;
	jaxer_connection *ac;
} out_ctx;

static apr_status_t jxr_send_document_brigade_to_jaxer(jaxer_connection *ac, out_ctx* ctx);

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

static out_ctx *jaxer_out_filter_init(request_rec *r)
{
	out_ctx *ctx;
	apr_status_t rv;
	server_rec *main_server = r->server;

	jaxer_worker *w;
	w = get_worker_by_name(main_server, "worker1");
#pragma message("TODO: make sure w is not NULL")
	
	ctx = ap_palloc(r->pool, sizeof(out_ctx));
	if (!ctx) 
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r, "mod_jaxer: can't alloc memory for out_ctx");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	memset(ctx, 0, sizeof(out_ctx));
	ctx->worker = w;
	
	// First, see if we can get a connection, and establish the protocol before we go too far
	if ((rv = jxr_conn_open(w, &ctx->ac, r))!= APR_SUCCESS) 
	{
		// proc_close_socket(r->server, ctx->ac);
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: can't connect to socket");
		return HTTP_SERVICE_UNAVAILABLE;
	}

	//END BLOCK
	jxr_init_header(BLOCKTYPE_ENDREQUEST, 0, &ctx->end_req);

	ap_add_common_vars(r);
	ap_add_cgi_vars(r);
	jaxer_add_env_vars(r);

	if ((rv =jxr_build_request_header_msg(r, &(ctx->hdr_msg), HDR_PRMS) ) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r,
						 "mod_jaxer: construct HTTP header message failed");
		return rv;
	}
	if ((rv =jxr_build_request_header_msg(r, &ctx->env_msg, ENV_PRMS) ) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, apr_get_os_error(), r,
						 "mod_jaxer: construct environment variables message failed");
		return rv;
	}

	return ctx;
}

apr_status_t jaxer_out_filter(request_rec *r)
{
	apr_status_t rv;
	out_ctx *ctx;
	server_rec *main_server = r->server;
	const char *location;
	unsigned char *buf = 0;
	int got_a_message = 0;
	jaxer_connection *ac;
	int done = 0;
    int should_redirect = 0;

    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(r->per_dir_config, &jaxer_module);
    FILE *postData = ap_pfopen(r->pool, config->fnameIn, "r");
		/* Bypass Jaxer if the content isn't HTML or this is a subrequest */
		if (r->main /*|| r->content_type && strcmp(r->content_type, "text/html")*/) {
			return DECLINED;
		}
		
		/* Also bypass if a redirection has been requested. */
		location = ap_table_get(r->headers_out, "Location");
		if (location) {
			return DECLINED;
		}

        /* Check if we should skip jaxer */
            // if we do not have a body, do not bother jaxer
            apr_off_t length = 0;
            int k;
            k=0;
            while(skip_jaxer_status[k] > 0)
            {
                if (r->status == skip_jaxer_status[k])
                {
                    return DECLINED;
                }
                k++;
            }

        /* We should not filter this if uri has no extension and the content_type
           is not configured for us. 
        */
            char *ext = strrchr(r->uri, '.');
            if (!ext && 
                (r->content_type == NULL || 
                 get_worker_name_from_content_type(r->content_type, r) == NULL))
            {
                return DECLINED;
            }

		/* Get setup out of the way */
		ctx = jaxer_out_filter_init(r);

		// SEND HEADER
		if ((rv = jxr_send_message(ctx->ac, ctx->hdr_msg)) != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: send HTTP header to jaxer server failed");
			ctx->ac->has_error = 1;
			return HTTP_INTERNAL_SERVER_ERROR;
		}

		// SEND ENV HEADER
		if ((rv = jxr_send_message(ctx->ac, ctx->env_msg)) != APR_SUCCESS) 
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: send environment vars to jaxer server failed");
			ctx->ac->has_error = 1;
			return HTTP_INTERNAL_SERVER_ERROR;
		}

	ac = ctx->ac;

	/*
	 * Send data to jaxer...
	 */
	if ((rv=jxr_send_document_brigade_to_jaxer(ac, ctx)) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: write docuemnt ot jaxer error");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}

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
		case BLOCKTYPE_REQUEST_POSTDATA:
			if (ctx->postdata_sent)
			{
				ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, r->pool, "mod_jaxer: requesting postdata again");
				ac->has_error = 1;	
				return rv;
			}
			if ((rv = jxr_send_postdata(ctx->ac, r)) != APR_SUCCESS)
			{
				ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, r->pool, "mod_jaxer: error while sending postdata");
				ac->has_error = 1;	
				return rv;
			}
			ctx->postdata_sent = 1;
			break;

#if 0
		case BLOCKTYPE_REQUEST_DOC:
			jxr_msg_reset_pos(&pos);
			len = jxr_msg_get_string(buf, &pos, uri);

			if ((rv = jxr_process_file_request(ctx->ac, uri)) != APR_SUCCESS)
			{
				compat_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: error while sending file request");
				ac->has_error = 1;	
				return rv;
			}
			break;
#endif

        case BLOCKTYPE_HTTP_HEADER:
            got_a_message = 1;
            break;

		default:
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: invalid request (type=%d) received", msg_type);
			ac->has_error = 1;
            return (!APR_SUCCESS);
			break;
		}
	}

#if 0
	if (!ctx->postdata_sent)
	{
		if (buf)
			buf[0] = 0;
		if ((rv = jxr_send_postdata_if_asking_for(ctx, &buf)) != APR_SUCCESS)
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send_postdata_if_asking_for error");
			ac->has_error = 1;
			return HTTP_INTERNAL_SERVER_ERROR;
		}

		if (buf && buf[0] && !ctx->postdata_sent)
		{
			/* We read a message from jaxer, but it was not a requeest-for-postdata message */
			got_a_message = 1;
		}
	}

	/* This is the last call to us, send END REQUEST if we did not get any message back from jaxer.
	 * If jaxer has send a HEADER message back, then there is no point of sending an end req.
	 */
	if (!got_a_message && (rv = jxr_send_message(ctx->ac, (unsigned char*) &ctx->end_req)) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send end of request to jaxer server failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	if (!got_a_message)
	{
		/* Get a message */
		if ((rv=jxr_receive_message(ctx->ac, &buf))!= APR_SUCCESS)
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: receive message from jaxer server failed");
			ac->has_error = 1;
			return HTTP_INTERNAL_SERVER_ERROR;
		}
    }

		/* if it is asking for post data, then send postdata with end req */
		if (!ctx->postdata_sent && jxr_msg_get_type(buf) == BLOCKTYPE_REQUEST_POSTDATA)
		{
			/* Send post data */
			if (ctx->has_postdata)
			{
				if ((rv = jxr_send_file_add_header(ctx->ac, postData, BLOCKTYPE_POSTDATA)) != APR_SUCCESS ||
					(rv = jxr_send_message(ctx->ac, (unsigned char*) &ctx->end_req)) != APR_SUCCESS)
				{
					compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send postdata to jaxer error");
					ac->has_error = 1;
					return HTTP_INTERNAL_SERVER_ERROR;
				}
			}else
			{
				if ((rv = jxr_send_message_add_header(ac, 0, 0, BLOCKTYPE_POSTDATA)) != APR_SUCCESS)
				{
					compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: send postdata to jaxer error");
					ac->has_error = 1;
					return HTTP_INTERNAL_SERVER_ERROR;
				}
			}

			ctx->postdata_sent = 1;

			/* get another msg */
			if ((rv=jxr_receive_message(ctx->ac, &buf)) != APR_SUCCESS)
			{
				compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: receive message from jaxer server failed");
				ac->has_error = 1;
				return HTTP_INTERNAL_SERVER_ERROR;
			}
		}
#endif

	/* Now we got a message.  This has to be a header msg */
	if ((rv=jxr_process_response_headers(r, buf)) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: parse response header failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}

    /* Check redirect */
	location = apr_table_get(r->headers_out, "Location");

    if (location && location[0])
    {
        should_redirect = 1;
        compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: request (status=%d) will be redirected to %s",
            r->status, location);
    }

    if (rv=jxr_process_response_body(ac, r) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: parse response body failed");
		ac->has_error = 1;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	
	

	if (should_redirect) 
	{
		/* This redirect needs to be a GET no matter what the original method was.  */
		r->method = apr_pstrdup(r->pool, "GET");
		r->method_number = M_GET;

		/* We already read the message body (if any), so don't allow 
		 * the redirected request to think it has one. We can ignore 
		 * Transfer-Encoding, since we used REQUEST_CHUNKED_ERROR. 
		 */
		apr_table_unset(r->headers_in, "Content-Length");
        apr_table_clear(r->headers_out);

        compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: redirecting request to %s", location);
		ap_internal_redirect_handler(location, r);
		return APR_SUCCESS;
	}
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
	char *buf = 0;

    int done = 0;
    FILE *responseData;
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(r->per_dir_config, &jaxer_module);

    responseData = ap_pfopen(r->pool, config->fnameOut, "r");
fprintf(stderr, "fnameOut=%s\n", config->fnameOut);
    
    done = 0;
    while (!done) {
		char write_buf[HUGE_STRING_LEN];
		apr_size_t write_buf_len;

        
		
        /* It will be done if we see bucket == APR_BRIGADE_SENTINEL(bbb).
         * At this point, if this is the last brigade, we send end req.
         */
#if 0
        if (done)
        {
            /* we should not proceed */
            break;
        }
#endif

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
			case BLOCKTYPE_REQUEST_POSTDATA:
				if (ctx->postdata_sent)
				{
                    ap_log_perror(APLOG_MARK, APLOG_ERR, 0, r->pool, "mod_jaxer: requesting postdata after it has been sent");
					/* log error */
					return (!APR_SUCCESS);
				}
				if (rv = jxr_send_postdata(ctx->ac, r) != APR_SUCCESS)
				{
					ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: error in while sending post data to jaxer");
					return rv;
				}
				ctx->postdata_sent = 1;
				break;

#if 0
			case BLOCKTYPE_REQUEST_DOC:
				/* get filename -- uri relative to DocumentRoot */
				uri[0] = 0;
				jxr_msg_get_length(buf,  &pos);
				len = jxr_msg_get_string(buf, &pos, uri);

				if ((rv =jxr_process_file_request(ac, uri)) != APR_SUCCESS)
				{
					return rv;
				}

				break;
#endif

			default:
				ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, r->pool, 
                      "mod_jaxer: invalid request (type=%d) received from jaxer while sending document to it", msg_type);
				return (!APR_SUCCESS);
				break;
			}
		}
	
#if 0
		if (!ctx->postdata_sent)
		{
			if (buf)
				buf[0] = 0;
			if((rv = jxr_send_postdata_if_asking_for(ctx, &buf)) != APR_SUCCESS)
			{
				compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: error in send_postdata_if_asking_for");
				return rv;
			}
			if(buf && buf[0] && !ctx->postdata_sent)
			{
				/* We got a message from jaxer, but it was not asking for post data, and yet,
				 * we still have document to send out.
				 */
				compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: invalid data received from jaxer while sending document to it");
				return (!APR_SUCCESS);
			}
		}
#endif

	    /* Send a message (body) to jaxer */
        write_buf_len = fread(write_buf, 1, HUGE_STRING_LEN, responseData);
fprintf(stderr, "GRRR: %d, %d\n", write_buf_len, feof(responseData));
		if (write_buf_len ==  0)
        {
            if (feof(responseData))
            {
                done = 1;
                /* send end req */
                if ((rv = jxr_send_message(ctx->ac, (unsigned char*) &ctx->end_req)) != APR_SUCCESS)
	            {
		            ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: send end of request to jaxer server failed");
		            ac->has_error = 1;
		            return rv;
	            }
           } else
           {
  			    ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool,
			         "mod_jaxer: can't read response data");
		        ac->has_error = 1;
			    return rv;
            }
        } else if((rv=jxr_send_message_add_header(ctx->ac, write_buf, (int)write_buf_len, BLOCKTYPE_DOCUMENT)) != APR_SUCCESS)
		{
			    ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: error in sending a document message");
			    return rv;
		}
#if 0
        {
            /* send end req */
            if ((rv = jxr_send_message(ctx->ac, (unsigned char*) &ctx->end_req)) != APR_SUCCESS)
	        {
		        ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, r->pool, "mod_jaxer: send end of request to jaxer server failed");
		        ac->has_error = 1;
		        return rv;
	        }
        }
#endif

	}
	return APR_SUCCESS;
}

