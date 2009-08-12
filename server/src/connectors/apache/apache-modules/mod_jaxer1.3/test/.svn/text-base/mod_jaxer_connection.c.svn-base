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
#include "compat.h"
#include "mod_jaxer_connection.h"
#include "mod_jaxer_conf.h"
#include "http_log.h"
#include "mod_jaxer_protocol.h"
#include "mod_jaxer_message.h"
#include "apr_strings.h"
extern int g_jxr_network_trace;

#define APLOG_ZDEBUG  APLOG_NOTICE

#define NCHAR_PER_LINE 16
#define LINE_LEN (NCHAR_PER_LINE*4+2) //3bytes hex, 1byte space, 1byte text, 1 byte end
void jxr_trace(const char* type, const char* data, int len, apr_pool_t *pool)
{
    char buf[LINE_LEN];
    int npl;
    int i=0;

    const char *hex = "0123456789abcdef";
    const char *p = data;

    ap_log_perror(APLOG_MARK, APLOG_ERR, 0, pool, "*** mod_jaxer NetworkDumping [%s] len=%d ***", type, len);
    while(len > 0)
    {
        npl = NCHAR_PER_LINE;
        if (len < npl)
            npl=len;
        memset(buf, ' ', LINE_LEN-1);
        buf[LINE_LEN-1] = '\0';
        for(i=0; i<npl; i++)
        {
            buf[i*3] = hex[p[i] >> 4];
            buf[i*3+1] = hex[p[i] & 0xf];
            if (p[i] >=32 && p[i] <= 127)
                buf[3*NCHAR_PER_LINE+1+i] = p[i]; 
        }

        ap_log_perror(APLOG_MARK, APLOG_ERR, 0, pool, buf);

        len -= npl;
        p += npl;
    }
}

static apr_status_t proc_close_socket(jaxer_connection * ac)
{
	apr_status_t rv = APR_SUCCESS;

	if (ac->has_error)
	{
		compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0,ac->request, "mod_jaxer: invalidating connection (%d) due to error", ac->sock);

		apr_reslist_invalidate(ac->worker->ac_cache, ac);
		
	} else 
	{
		compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0,ac->request, "mod_jaxer: releasing connection (%d) back to pool", ac->sock);

		ac->has_error = 0;
		ac->request = 0;
		apr_reslist_release(ac->worker->ac_cache, ac);
	}

	return rv;
}

apr_status_t jaxer_connection_cleanup(void *theac)
{
	jaxer_connection *ac = (jaxer_connection *) theac;
	if (ac)
        return proc_close_socket(ac);
    else
        return APR_SUCCESS;
}

int jxr_socket_sendfull(jaxer_connection *ac, const unsigned char *b, int len)
{
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(ac->request->per_dir_config, &jaxer_module);
    int sent = 0;
    apr_socket_t *sock;
	apr_status_t rv;
	apr_size_t send_len;
	int retry = 0;
	apr_pool_t *p = (config) ? config->reqPool : ac->worker->pool;

	sock = ac->sock;

    if (g_jxr_network_trace)
    {
        jxr_trace("SEND", b, len, p);
    }
    while (sent < len)
	{
		send_len = len - sent;
		rv = apr_socket_send(sock, (const char*)(b + sent), &send_len);
		if (send_len == len - sent)
		{
			sent += (int) send_len;
			break;
		}
		if (rv != APR_SUCCESS)
		{
            /*
             * Let's hope this traps EWOULDBLOCK too !
             */
            if (APR_STATUS_IS_EAGAIN(rv) && retry<3)
			{
				retry++;
            }else
			{
				ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p, "mod_jaxer: send data over socket error: total len=%d sent=%d", len, sent);
				return -1;
			}
        }
		sent += (int) send_len;
	}
	return sent;
}

int jxr_socket_recvfull(jaxer_connection *ac, unsigned char *b, int len)
{
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(ac->request->per_dir_config, &jaxer_module);
    int rdlen = 0;
    apr_socket_t *sock;
	apr_status_t rv;
	apr_size_t read_len;
	int retry = 0;
	apr_pool_t *p = (config) ? config->reqPool : ac->worker->pool;

	sock = ac->sock;

    while (rdlen < len)
	{
		read_len = len - rdlen;
		rv = apr_socket_recv(sock, (char *)b + rdlen, &read_len);
		if (rv != APR_SUCCESS)
		{
			if (APR_STATUS_IS_EAGAIN(rv) && retry<3)
			{
				retry++;
            }else
			{
				ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p, "mod_jaxer: receive data over socket error: total len=%d read=%d", len, rdlen);
				return -1;
			}
		}

        rdlen += (int) read_len;
    }

    if (g_jxr_network_trace)
    {
        jxr_trace("RECV", b, rdlen, p);
    }
    return rdlen;
}

apr_status_t jxr_receive_message_to_buf(jaxer_connection *ac, unsigned char *buf)
{
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(ac->request->per_dir_config, &jaxer_module);
    int rc;
	apr_size_t msglen, pos;
	apr_pool_t* p = (config) ? config->reqPool : ac->worker->pool;

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: receiving a message to buffer (sock=%d)", ac->sock);

	rc = jxr_socket_recvfull(ac, buf, sizeof(Jaxer_Header));
	if(rc < 0)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: read data from socket error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	msglen = jxr_msg_get_length( buf, &pos);

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: receiving messsage body (type=%s len=%d)", sBlockType[buf[0]], msglen);

	rc = jxr_socket_recvfull(ac, buf + sizeof(Jaxer_Header), (int) msglen);
	if(rc < 0)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: read data from socket error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: received a message to buffer (sock=%d)", ac->sock);
	
    return APR_SUCCESS;
} 

apr_status_t jxr_send_begin_request_messsage(jaxer_connection *ac, char req_type)
{
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(ac->request->per_dir_config, &jaxer_module);
	static unsigned char start_req_body[26];
    static int inited = 0;
    unsigned char buf[512];
	apr_size_t pos;
    apr_status_t rc;
    char type;
    apr_size_t msglen;
	int protocol;
    int jaxer_reply;
    
	
    apr_pool_t* p = (config) ? config->reqPool : ac->worker->pool;

    if (!inited)
	{
		jxr_msg_init(start_req_body, &pos, BLOCKTYPE_BEGIN_REQUEST);
		jxr_msg_append_int16(start_req_body, &pos, JAXER_PROTOCOL_VERSION);
		jxr_msg_append_byte(start_req_body, &pos, req_type);
		jxr_msg_end(start_req_body, &pos);
		inited = 1;
	}

	if ((rc = jxr_send_message(ac, (unsigned char*) start_req_body)) != APR_SUCCESS)
    {
        return rc;
    }
    // recv begin request response
    if ((rc = jxr_receive_message_to_buf(ac, buf)) != APR_SUCCESS)
    {
        return rc;
    }
    

    msglen = jxr_msg_get_length(buf, &pos);
	
	if (msglen < 3)
	{
		// Invalid message
		ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, p, "mod_jaxer: received invalid data length (%d) for a BEGIN_REQUEST block", msglen);
		return HTTP_SERVICE_UNAVAILABLE;
	}
	
    type = jxr_msg_get_type(buf);
    protocol = jxr_msg_get_int16(buf, &pos);
    jaxer_reply = jxr_msg_get_byte(buf, &pos);

    if (type != BLOCKTYPE_BEGIN_REQUEST)
	{
		// Invalid message
		ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, p, "mod_jaxer: received invalid data type (%c) for a BEGIN_REQUEST block", type);
        return HTTP_SERVICE_UNAVAILABLE;
	}

    if (jaxer_reply != 1)
    {
        ap_log_perror(APLOG_MARK, APLOG_ERR, 0, p, "mod_jaxer: Jaxer (protocol version=%d) cannot handle the protocol (version=%d)", protocol,
            JAXER_PROTOCOL_VERSION);
        if (msglen>=5)
        {
            // we have an error code from jaxer
            unsigned char sjaxer_error[512];
            int jaxer_error;
            jaxer_error = jxr_msg_get_int16(buf, &pos);
            sjaxer_error[0] = 0;
            if (msglen >7)
                jxr_msg_get_string(buf, &pos, sjaxer_error);
            ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, p, "mod_jaxer: jaxer returned (%d) %s", jaxer_error, sjaxer_error);
        }
            
		return HTTP_SERVICE_UNAVAILABLE;
    }

	if (protocol != JAXER_PROTOCOL_VERSION)
	{
		// Invalid message
		ap_log_perror(APLOG_MARK, APLOG_ERR, 0, p, "mod_jaxer: unsupported protocol (%d) received.  Only supports version %d", protocol,
            JAXER_PROTOCOL_VERSION);
        return HTTP_SERVICE_UNAVAILABLE;
	}

    
    return rc;
}

apr_status_t jxr_send_message(jaxer_connection *ac, unsigned char *msg)
{
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(ac->request->per_dir_config, &jaxer_module);
    int rc;
	apr_size_t msglen, pos;
	apr_pool_t *p = (config) ? config->reqPool : ac->worker->pool;
	
	msglen = jxr_msg_get_length(msg, &pos);
	rc = jxr_socket_sendfull(ac, msg, (int) msglen + sizeof(Jaxer_Header));
	if(rc < 0)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: send data over socket error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}
    return APR_SUCCESS;
}

apr_status_t jxr_receive_message(jaxer_connection *ac, unsigned char **pmsg)
{
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(ac->request->per_dir_config, &jaxer_module);
    Jaxer_Header jx_hdr;
	int rc;
	apr_size_t msglen, pos;
	apr_pool_t *p = (config) ? config->reqPool : ac->worker->pool;
	request_rec* r = ac->request;

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: receiving a message (sock=%d)", ac->sock);

	if (!r)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, 0, p, "mod_jaxer: connection does not have request object");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: receiving messsage header");

	rc = jxr_socket_recvfull(ac, (unsigned char*)&jx_hdr, sizeof(Jaxer_Header));
	if(rc < 0)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: read data from socket error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	msglen = jxr_msg_get_length( (unsigned char*)&jx_hdr, &pos);

    *pmsg = (unsigned char*)apr_palloc(p, msglen + sizeof(Jaxer_Header));
	if (! *pmsg)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: cannot allocate memory.");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	memcpy(*pmsg, (const void*) &jx_hdr, sizeof(Jaxer_Header));

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: receiving messsage body (type=%s len=%d)", sBlockType[(*pmsg)[0]], msglen);

	rc = jxr_socket_recvfull(ac, (*pmsg) + sizeof(Jaxer_Header), (int) msglen);
	if(rc < 0)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: read data from socket error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: received a message (sock=%d)", ac->sock);
	
    return APR_SUCCESS;
} 

#if 0
apr_status_t jxr_hello_to_jaxer(jaxer_connection *ac)
{
	// This should be done only once per connection

    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(ac->request->per_dir_config, &jaxer_module);
	static unsigned char Hello_Body[256];
	apr_size_t pos;
	static int inited = 0;
	unsigned char buf[256];
	apr_size_t msglen;
	char type;
	int protocol;
	apr_pool_t *p = (config) ? config->reqPool : ac->worker->pool;

	if (!inited)
	{
		jxr_msg_init(Hello_Body, &pos, BLOCKTYPE_HELLO);
		jxr_msg_append_int16(Hello_Body, &pos, JAXER_PROTOCOL_VERSION);
		strcpy((char*)Hello_Body+pos, HELLO_STRING);
		pos += strlen(HELLO_STRING);
		jxr_msg_end(Hello_Body, &pos);

		inited = 1;
	}

	if ( jxr_send_message(ac, (unsigned char*)Hello_Body) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, p, "mod_jaxer: can't send Hello message through socket");
		return HTTP_SERVICE_UNAVAILABLE;
	}
	// Get Response
	if ( jxr_receive_message_to_buf(ac, buf) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, p, "mod_jaxer: can't read Hello message from socket");
		return HTTP_SERVICE_UNAVAILABLE;
	}
	msglen = jxr_msg_get_length(buf, &pos);
	type = jxr_msg_get_type(buf);
	if (msglen < 2)
	{
		// Invalid message
		ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, p, "mod_jaxer: received invalid data length (%d) for a HELLO block", msglen);
		return HTTP_SERVICE_UNAVAILABLE;
	}
	if (type != BLOCKTYPE_HELLO)
	{
		// Invalid message
		ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, p, "mod_jaxer: received invalid data type (%c) for a HELLO block", type);
		return HTTP_SERVICE_UNAVAILABLE;
	}
	protocol = jxr_msg_get_int16(buf, &pos);
	if (protocol != JAXER_PROTOCOL_VERSION)
	{
		// Invalid message
		ap_log_perror(APLOG_MARK, APLOG_ERR, 0, p, "mod_jaxer: unsupported protocol (%d) received.  Only supports version %d", protocol,
            JAXER_PROTOCOL_VERSION);
		return HTTP_SERVICE_UNAVAILABLE;
	}

	return APR_SUCCESS;
}
#endif

apr_status_t jxr_connect_socket(jaxer_connection * ac)
{
	apr_status_t rv;
	apr_socket_t *sock = 0;
	jaxer_worker *aworker = ac->worker;
	apr_sockaddr_t *remote_sa = aworker->remote_sa;
	apr_pool_t *p = ac->worker->pool;

	ap_log_perror(APLOG_MARK, APLOG_ZDEBUG, 0, p, "in jxr_connect-socket");

	if (ac->sock)
	{
		if ((rv=apr_socket_close(ac->sock)) != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p,
				"mod_jaxer: faied to close socket: return code=%d", rv);
			// return rv;
		}
		ac->sock = 0;
	}

#ifdef _APACHE20
	rv = apr_socket_create_ex(&sock, APR_INET, SOCK_STREAM, APR_PROTO_TCP, p);
#else
	rv = apr_socket_create(&sock, APR_INET, SOCK_STREAM, APR_PROTO_TCP, p);
#endif
	ap_log_perror(APLOG_MARK, APLOG_ZDEBUG, 0, p, "in jxr_connect-socket: apr_socket_create returned %d", rv);

	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p,
				"mod_jaxer: faied to create socket: return code=%d", rv);
		return rv;
	}

	

	/*
	 * The options for the sockets
	 */
#if 0
	// KEEPALIVE
	rv = apr_socket_opt_set(sock, APR_SO_KEEPALIVE, 1);
	if (rv != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
				"mod_jaxer: apr_socket_opt_set failed to set keep alive: return code=%d", rv);
		return rv;
	}
	

	// APR_SO_LINGER?
	rv = apr_socket_opt_set(sock, APR_SO_LINGER, 0);
	if (rv != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
				"mod_jaxer: apr_socket_opt_set failed to set keep alive: return code=%d", rv);
		return rv;
	}

	//TIME OUT
	rv = apr_socket_timeout_set(sock, apr_time_from_sec(3));
    if (rv != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
				"mod_jaxer: apr_socket_opt_set failed to set timeout: return code=%d", rv);
		return rv;
	} 
#endif
	
	// Block first
	rv = apr_socket_opt_set(sock, APR_SO_NONBLOCK, 0);
	ap_log_perror(APLOG_MARK, APLOG_ZDEBUG, 0, p, "apr_socket_opt_set return %d", rv);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p,
				"mod_jaxer: apr_socket_opt_set failed to set blocking: return code=%d", rv);
		return rv;
	}

	rv = apr_socket_connect(sock, remote_sa);
	ap_log_perror(APLOG_MARK, APLOG_ZDEBUG, 0, p, "apr_socket_connect return %d", rv);
	ap_log_perror(APLOG_MARK, APLOG_ZDEBUG, 0, p, "sa: hostname=%s servername=%s port=%d family=%d salen=%d", 
      remote_sa->hostname,
      remote_sa->servname,
      remote_sa->port,
      remote_sa->family,
      remote_sa->salen
);
if (APR_STATUS_IS_EAGAIN(rv))
{
fprintf(stderr, "EAGAIN\n");
}else if (APR_STATUS_IS_EINPROGRESS(rv))
{
fprintf(stderr, "EEINPROGRESS\n");
}

	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p,
				"mod_jaxer: apr_socket_connect failed: return code=%d", rv);
		return rv;
	}

	ac->sock = sock;
	return rv;
}

apr_status_t jxr_connect(jaxer_connection *pac)
{
	// Connect socket.
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(pac->request->per_dir_config, &jaxer_module);
	apr_status_t rv;

	rv = jxr_connect_socket(pac);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, NULL, "mod_jaxer: jxr_connect_socket failed");
		pac->inuse = 0;
		return rv;
	}

#if 0
	// Say Hello
	rv = jxr_hello_to_jaxer(pac);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p, "mod_jaxer: jxr_hello_to_jaxer failed");
		pac->inuse = 0;
		return rv;
	}
#endif

	return rv;
}


apr_status_t jxr_connect_and_begin_request(jaxer_connection *pac)
{
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(pac->request->per_dir_config, &jaxer_module);
	// Connect socket.  Send BeginRequest
	apr_status_t rv;
	apr_pool_t *p = (config) ? config->reqPool : pac->worker->pool;

	rv = jxr_connect(pac);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p, "mod_jaxer: jxr_connect failed");
		pac->inuse = 0;
		return rv;
	}

	// Send BeginRequest
	rv = jxr_send_begin_request_messsage(pac, 0);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p, "mod_jaxer: send begin request message failed");
		pac->inuse = 0;
		return rv;
	}
	return rv;
}



#ifndef APACHE1_3
apr_status_t jxr_send_brigade(jaxer_connection * ac, apr_bucket_brigade * bb)
{
	apr_bucket *bucket;
	apr_status_t rv;
	
	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, ac->request, "mod_jaxer: sending a brigade (sock=%d)", ac->sock);

	for (bucket = APR_BRIGADE_FIRST(bb);
		 bucket != APR_BRIGADE_SENTINEL(bb);
		 bucket = APR_BUCKET_NEXT(bucket))
	{
		char *write_buf;
		apr_size_t write_buf_len;
		
		if (APR_BUCKET_IS_EOS(bucket))
			break;

		if (APR_BUCKET_IS_FLUSH(bucket))
			continue;

		if ((rv =
			 apr_bucket_read(bucket, (const char **)&write_buf, &write_buf_len,
							 APR_BLOCK_READ)) != APR_SUCCESS) {
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, ac->request,
						 "mod_jaxer: can't read request from bucket");
			return rv;
		}

		{
			int type = jxr_msg_get_type(write_buf);
			apr_size_t pos; // not used
			apr_size_t len = jxr_msg_get_length(write_buf, &pos);
			compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, ac->request, "mod_jaxer: sending a brigade (type=%s len=%d)", sBlockType[type], len);
		}

		/* Write the buffer to jaxer server */
		if(0 > jxr_socket_sendfull(ac, write_buf, (int) write_buf_len))
		{
		
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, APR_FROM_OS_ERROR(rv), ac->request,
					"mod_jaxer: can't write to socket");
			return apr_get_os_error();
		}
	}
	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, ac->request, "mod_jaxer: sent a brigade (sock=%d)", ac->sock);

	return APR_SUCCESS;
}
#endif

apr_status_t jxr_send_file_add_header(jaxer_connection * ac, char *fname, enum BlockType bType)
{
	apr_status_t rv;
	Jaxer_Header jx_hdr;
	request_rec* r = ac->request;
	int rc;
        int n;
        FILE *f;
        char buf[HUGE_STRING_LEN];

	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending a brigade and add header (type=%s sock=%d)", 
		sBlockType[bType], ac->sock);

	f = ap_pfopen(r->pool, fname, "r");
	if (f == NULL) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, r, "Could not open file %s",
                      fname);
        }
        while (1) {
            apr_size_t pos;
            while ((n = fread(buf, sizeof(char), HUGE_STRING_LEN, f)) < 1
                   && ferror(f) && errno == EINTR && !r->connection->aborted)
                continue;

            if (n < 1) {
                break;
            }

		jxr_msg_init((unsigned char*) &jx_hdr, &pos, bType);
		pos += n;
		jxr_msg_end((unsigned char*) &jx_hdr, &pos);

		compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending header");

		rc = jxr_socket_sendfull(ac, (unsigned char*)&jx_hdr, (int) sizeof(Jaxer_Header));
		if(rc < 0)
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rc, r, "mod_jaxer: read data from socket error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}

		compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending body (len=%d)", n);

		rc = jxr_socket_sendfull(ac, buf, (int)n );
		if(rc < 0)
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rc, r, "mod_jaxer: read data from socket error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}
	}


	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sent a brigade and added header (type=%s sock=%d)", 
		sBlockType[bType], ac->sock);

	return APR_SUCCESS;
}

apr_status_t jxr_send_message_add_header(jaxer_connection *ac, const char* buf, int msglen, enum BlockType bType)
{
	apr_size_t len = 0;
	apr_size_t to_send = 0;
	apr_status_t rv = APR_SUCCESS;
	struct iovec vec[2];
	Jaxer_Header jx_hdr;
	request_rec *r = ac->request;
	apr_socket_t* sock = ac->sock;
    jaxer_dir_conf *config = (jaxer_dir_conf *)ap_get_module_config(r->per_dir_config, &jaxer_module);

	jxr_init_header(bType, msglen, &jx_hdr);

	vec[0].iov_base = (unsigned char*)&jx_hdr;
	vec[0].iov_len = sizeof(Jaxer_Header);
	vec[1].iov_base = (char*) buf;
	vec[1].iov_len = msglen;
	to_send = msglen + sizeof(Jaxer_Header);

	
	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending a message (type=%s len=%d sock=%d)", 
		sBlockType[bType], msglen, ac->sock);

    if(g_jxr_network_trace!=0)
    {
        jxr_trace("SEND", vec[0].iov_base, vec[0].iov_len, config->reqPool);
        jxr_trace("SEND", buf, msglen, config->reqPool);
    }

	if((rv = apr_socket_sendv(sock, vec, 2, &len)) != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: error in sending a messasge");
		return rv;
	}

	if (len < to_send)
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: sending message error: send %d bytes of %d", len, to_send);
		return (!rv);
	}
	return rv;
}


/*
 * USe apr_reslist to manage connections
 */

static apr_status_t jxr_conn_construct(void** pac, void *params, apr_pool_t *p)
{
	jaxer_worker *aw = (jaxer_worker*) params;
	jaxer_connection *ac=0;
	apr_status_t rv = APR_SUCCESS;

	ap_log_perror(APLOG_MARK, APLOG_ZDEBUG, 0, p, "mod_jaxer: jxr_conn_construct: creating a new connection for worker %s", aw->name);

	ac = apr_pcalloc(p, sizeof(jaxer_connection));
	if (!ac)
	{
		rv = APR_ENOMEM;
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: allocate memory for jaxer_connection failed");
		return rv;
	}

	// Initial setup for connection (non-zero values)
	ac->worker = aw;
	ac->start_time = apr_time_now();

	ap_log_perror(APLOG_MARK, APLOG_ZDEBUG, 0, p, "mod_jaxer: jxr_conn_construct: trying to connect & talk to jaxer");

	if ((rv = jxr_connect(ac)) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: jxr_connect failed");
		return rv;
	}

	*pac = ac;

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: jxr_conn_construct: a new connection (%d) for worker %s was created successfully", ac->sock, aw->name);

	return rv;
}

static apr_status_t jxr_conn_destruct(void *conn, void *params, apr_pool_t *p)
{
	apr_status_t rv = APR_SUCCESS;
	jaxer_connection *ac = conn;

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, (ac->request) ? ac->request->pool : p,
        "mod_jaxer: jxr_conn_destruct: closing connection (%d) for worker %s", ac->sock, ac->worker->name);


	if (ac->sock)
	{
		if((rv = apr_socket_close(ac->sock)) != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, (ac->request) ? ac->request->pool : p,
				"mod_jaxer: apr_socket_close failed: return code=%d", rv);
		}

		ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, (ac->request) ? ac->request->pool : p,
            "mod_jaxer: jxr_conn_destruct: connection (%d) for worker %s closed", ac->sock, ac->worker->name);

		ac->sock = 0;
		ac->has_error = 0;
		ac->request = 0;
	}

	return rv;
}

apr_status_t jxr_conn_setup(jaxer_worker* aw)
{
	apr_status_t rv = APR_SUCCESS;


	ap_log_perror(APLOG_MARK, APLOG_ZDEBUG, 0, aw->pool, "mod_jaxer: jxr_conn_setup: creating connection pool (min=%d keep=%d max=%d) for worker %s",
			aw->nmin, aw->nkeep, aw->nmax, aw->name);

	rv = apr_reslist_create(&aw->ac_cache, aw->nmin, aw->nkeep, aw->nmax, apr_time_from_sec(aw->exptime),
		jxr_conn_construct, jxr_conn_destruct, aw, aw->res_pool);

	if (rv == APR_SUCCESS)
	{
		apr_reslist_timeout_set(aw->ac_cache, apr_time_from_sec(aw->acquire_timeout));

		apr_pool_cleanup_register(aw->res_pool, aw->ac_cache, (void*)apr_reslist_destroy, apr_pool_cleanup_null);

		ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, aw->res_pool, "mod_jaxer: connection pool (min=%d keep=%d max=%d) created for worker %s",
			aw->nmin, aw->nkeep, aw->nmax, aw->name);
		
	}else
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, aw->res_pool, "mod_jaxer: failed to initialize connection reslist for worker %s", aw->name);
		// apr_pool_destroy(aw->pool);
		// aw->pool = 0;
	}

	

	return rv;

}

/*
 * This function gets a connection back either by connecting it, or reuse one.
 */
apr_status_t jxr_conn_open(jaxer_worker* aw, jaxer_connection **pac, request_rec *r)
{
	apr_status_t rv = APR_SUCCESS;
	
	*pac = 0;

	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: entered jxr_conn_open for worker %s", aw->name);

	if (!aw->ac_cache)
	{
		// Create the resource first.  Grab the mutex
#if APR_HAS_THREADS
		rv = apr_thread_mutex_lock(aw->mutex);
		if (rv != APR_SUCCESS)
		{
			compat_log_rerror(APLOG_MARK, APLOG_CRIT, rv, r, "mod_jaxer: Failed to acquire thread mutex for jaxerworker %s", aw->name);
			return rv;
		}
#endif

		// Make sure it is indeed uncreated
		if (!aw->ac_cache)
		{
			compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: creating the connection pool for worker %s", aw->name);
			rv = jxr_conn_setup(aw);
		}

		// Before we do anything else, release the mutex.
#if APR_HAS_THREADS
		if (apr_thread_mutex_unlock(aw->mutex) != APR_SUCCESS)
		{
			compat_log_rerror(APLOG_MARK, APLOG_CRIT, 0, r, "mod_jaxer: Failed to release thread mutex for jaxerworker %s", aw->name);
		}
#endif

		if (rv != APR_SUCCESS)
		{
			compat_log_rerror(APLOG_MARK, APLOG_CRIT, rv, r, "mod_jaxer: jxr_conn_setup failed for jaxerworker %s", aw->name);
			return rv;
		}
		compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: created the connection pool for worker %s", aw->name);

	}

	// At this point, we have a cache
	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: acquiring a connection for worker %s", aw->name);

	rv = apr_reslist_acquire(aw->ac_cache, (void **)pac);
	if (rv != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_CRIT, rv, r, "mod_jaxer: Failed to acquire connection from pool for jaxerworker %s", aw->name);
		return rv;
	}

	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: acquired a connection (%d) for worker %s", (*pac)->sock, aw->name);

	// Setup additional parameters
	(*pac)->request = r;
	(*pac)->has_error = 0;
	(*pac)->last_active_time = apr_time_now();
	
	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: checking connection (%d) for worker %s", (*pac)->sock, aw->name);

	// Make sure it is connected
	if((rv = jxr_send_begin_request_messsage(*pac, 0)) != APR_SUCCESS &&
			((rv = jxr_connect_and_begin_request(*pac)) != APR_SUCCESS))
	{
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: reuse connection or reconnect failed");
		apr_reslist_invalidate(aw->ac_cache, *pac);
		*pac = 0;

		return rv;
	}

	compat_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: acquiredconnection (%d) successfully for worker %s", (*pac)->sock, aw->name);

	return rv;

}

#if 0
void jxr_conn_release(jaxer_worker* aw, jaxer_connection* ac)
{
	apr_reslist_release(aw->ac_cache, ac);
}


apr_status_t jxr_get_or_create_connection(jaxer_worker *aworker, jaxer_connection **ac, request_rec* r)
{
	apr_status_t rv = APR_SUCCESS;
	int i;

	jaxer_connection *pac = 0;
	int tries = 0;
	int got_connection = 0;

	*ac = 0;

#if APR_HAS_THREADS
	rv = apr_thread_mutex_lock(aworker->mutex);
	if (rv != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_CRIT, rv, r, "Failed to acquire thread mutex for jaxerworker %s", aworker->name);
		return rv;
	}
#endif

#if 0 //TODO
	for(i=0; i<aworker->cache_size; i++)
	{
		pac = &aworker->ac_cache[i];
		if (pac->inuse == 0)
		{
			pac->inuse = 1;
			break;
		}
	}
#endif

	if (i >= aworker->cache_size)
	{
		// Try to close a connection that has been there for too long
		apr_time_t cutoff_time = apr_time_now() - aworker->force_close_timeout;
#if 0 // TODO
		for(i=0; i<aworker->cache_size; i++)
		{
			pac = &aworker->ac_cache[i];
			if (pac->last_active_time < cutoff_time)
			{
				// close this connection
				rv = apr_socket_close(pac->sock);
				if (rv != APR_SUCCESS)
				{
					// If it fails, log a warning
					compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "Failed to close a connection for jaxerworker %s", aworker->name);
				}
				pac->sock = 0;
				pac->inuse = 1;
				pac->start_time = 0;
				break;
			}
		}
#endif

	}

#if APR_HAS_THREADS
	rv = apr_thread_mutex_unlock(aworker->mutex);
	if (rv != APR_SUCCESS)
	{
		compat_log_rerror(APLOG_MARK, APLOG_CRIT, rv, r, "Failed to release thread mutex for jaxerworker %s", aworker->name);
		return rv;
	}
#endif
	
	if (i >= aworker->cache_size)
	{
		// we run out of connections
		compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "no free connections for jaxerworker %s", aworker->name);
		return APR_ENOSOCKET;
	}

	// Now we have a slot.  If the connection is still valid, then just reuse it, otherwise, connect it
	// How to determine if the socket connection is still valid? ask?

	pac->request = r;
		
	if (!pac->sock)
	{
		rv = jxr_connect_and_begin_request(pac);
		if (rv != APR_SUCCESS)
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: establish a new connection ready for sending request failed");
			return rv;
		}
	}else
	{
		// We are reusing an existing socket connection

		// At this point, we should determine if the connection is still alive by sending the BeginRequest msg.
		// If it is not connected, we should reconnect it
		// Send BeginRequest
		if((rv = jxr_send_begin_request_messsage(pac, 0)) != APR_SUCCESS &&
			((rv = jxr_connect_and_begin_request(pac)) != APR_SUCCESS))
		{
			compat_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: reuse connection or reconnect failed");
			pac->inuse = 0;
			return rv;
		}
	}


	// Setup time
	if (pac->start_time == 0)
		pac->start_time = apr_time_now();

	pac->last_active_time = apr_time_now();

	*ac = pac;

	return rv;
}
#endif
