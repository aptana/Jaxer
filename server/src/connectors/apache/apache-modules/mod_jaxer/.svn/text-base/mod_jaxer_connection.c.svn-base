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
#include "mod_jaxer_connection.h"
#include "http_log.h"
#include "mod_jaxer_protocol.h"
#include "mod_jaxer_message.h"
#include "apr_strings.h"
#include "mod_jaxer_conf.h"
#include "mod_jaxer_proc.h"

#define NCHAR_PER_LINE 16
#define LINE_LEN (NCHAR_PER_LINE*4+2) //3bytes hex, 1byte space, 1byte text, 1 byte end
void jxr_trace(const char* type, const char* data, int len, apr_pool_t *pool)
{
#if 0
    FILE *fp = fopen("c:/ap.log", "a+b");
    char buf[56];
    sprintf(buf, "%s len=%d\n", type, len);
    fwrite(buf, strlen(buf), 1, fp);
    fwrite(data, len, 1, fp);
    strcpy(buf, "\n");
    fwrite(buf, strlen(buf), 1, fp);
    fflush(fp);
    fclose(fp);
#else
    char buf[LINE_LEN];
    int npl;
    int i=0;

    const unsigned char *hex = "0123456789abcdef";
    const unsigned char *p = (const unsigned char*) data;

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
#endif
}

static apr_status_t proc_close_socket(jaxer_connection * ac)
{
	apr_status_t rv = APR_SUCCESS;

	if (ac->has_error)
	{
		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0,ac->request, "mod_jaxer: invalidating connection (%d) due to error", ac->sock);

#ifndef _APACHE20N
		apr_reslist_invalidate(ac->worker->ac_cache, ac);
#endif
		
	} else 
	{
		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0,ac->request, "mod_jaxer: releasing connection (%d) back to pool", ac->sock);

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
    int sent = 0;
    apr_socket_t *sock;
	apr_status_t rv;
	apr_size_t send_len;
	int retry = 0;
	apr_pool_t *p = (ac->request) ? ac->request->pool : ac->worker->pool;

	sock = ac->sock;

    if (get_network_dump(ac->worker->server))
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
    int rdlen = 0;
    apr_socket_t *sock;
	apr_status_t rv;
	apr_size_t read_len;
	int retry = 0;
	apr_pool_t *p = (ac->request) ? ac->request->pool : ac->worker->pool;

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

    if (get_network_dump(ac->worker->server))
    {
        jxr_trace("RECV", b, rdlen, p);
    }
    return rdlen;
}

apr_status_t jxr_receive_message_to_buf(jaxer_connection *ac, unsigned char *buf)
{
    int rc;
	apr_size_t msglen, pos;
	apr_pool_t* p = (ac->request) ? ac->request->pool : ac->worker->pool;

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: receiving a message to buffer (sock=%d)", ac->sock);

	rc = jxr_socket_recvfull(ac, buf, sizeof(Jaxer_Header));
	if(rc < 0)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: read data from socket error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	msglen = jxr_msg_get_length( buf, &pos);

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: receiving messsage body (type=%s len=%d)", g_BlockTypes[buf[0]], msglen);

	rc = jxr_socket_recvfull(ac, buf + sizeof(Jaxer_Header), (int) msglen);
	if(rc < 0)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: read data from socket error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: received a message to buffer (sock=%d)", ac->sock);
	
    return APR_SUCCESS;
} 

static apr_status_t jxr_init_debug_request(jaxer_connection* ac, char* jaxer_id)
{
	//TODO: do not used length-fixed buffers
	unsigned char start_req_body[512];
    unsigned char *buf;
	apr_size_t pos;
	apr_size_t nlen, vlen;
    apr_status_t rc;
    char type;
	char name[56];
	char value[256];
    apr_size_t msglen;
	int npairs, i;
    request_rec * r = ac->request;
	
    apr_pool_t* p = r->pool;
	
	if (!r)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, ac->worker->pool, "mod_jaxer: Debug request without a request object");
		return HTTP_SERVICE_UNAVAILABLE;
	}

	jxr_msg_init(start_req_body, &pos,bt_DebugRequest);

	// We have only one "name=value"
	jxr_msg_append_int16(start_req_body, &pos, 1);
	jxr_msg_append_string(start_req_body, &pos, "Jaxer-ID");
	jxr_msg_append_string(start_req_body, &pos, jaxer_id);
	jxr_msg_end(start_req_body, &pos);

	if ((rc = jxr_send_message(ac, (unsigned char*) start_req_body)) != APR_SUCCESS)
    {
        return rc;
    }
    // recv debug request response
    if ((rc = jxr_receive_message(ac, &buf)) != APR_SUCCESS)
    {
        return rc;
    }
    
    msglen = jxr_msg_get_length(buf, &pos);
	
	if (msglen < 3)
	{
		// Invalid message
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: received invalid data length (%d) for a DEBUG_REQUEST block", msglen);
		return HTTP_SERVICE_UNAVAILABLE;
	}
	
    type = jxr_msg_get_type(buf);
    npairs = jxr_msg_get_int16(buf, &pos);

    if (type !=bt_DebugRequest)
	{
		// Invalid message
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: received invalid data type (%c) for a DEBUG_REQUEST block", type);
        return HTTP_SERVICE_UNAVAILABLE;
	}

    if (npairs < 1)
    {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_jaxer: DEBUG_REQUEST does not contain needed data");
		return HTTP_SERVICE_UNAVAILABLE;
	}

	for (i=0; i<npairs; i++)
	{
		name[0] = value[0] = 0;
		nlen = jxr_msg_get_string(buf, &pos, name);
		vlen = jxr_msg_get_string(buf, &pos, value);
		if (nlen == strlen("Accepted") && strcmp(name, "Accepted") == 0)
		{
			if (value[0] == 1)
				return APR_SUCCESS;
			else
			{
				ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: Jaxer cannot handle debug request.");
				return HTTP_SERVICE_UNAVAILABLE;
			}
		}
	}
	
	ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "mod_jaxer: debug request does not contain the required field");
            
	return HTTP_SERVICE_UNAVAILABLE;
}

apr_status_t jxr_send_begin_request_messsage(jaxer_connection *ac, enum eRequestType req_type)
{
	unsigned char start_req_body[26];
    unsigned char buf[512];
	apr_size_t pos;
    apr_status_t rc;
    char type;
    apr_size_t msglen;
	int protocol;
    int jaxer_reply;
	int is_debug_req = 0;
	char *jaxer_id=0;
	
    apr_pool_t* p = (ac->request) ? ac->request->pool : ac->worker->pool;

	if (ac->request && ac->worker->use_proto >= 4  && !ac->worker->retry)
		is_debug_req = jxr_is_debug_request(ac->request, &jaxer_id);
	if (is_debug_req)
	{
		if ((rc = jxr_init_debug_request(ac, jaxer_id)) != APR_SUCCESS)
		{
			return rc;
		}
	}

	// only try once
	if (ac->worker->retry != 0)
		ac->worker->retry = 0;

	jxr_msg_init(start_req_body, &pos,bt_BeginRequest);
	jxr_msg_append_int16(start_req_body, &pos, ac->worker->use_proto);
	jxr_msg_append_byte(start_req_body, &pos, (char) req_type);
	jxr_msg_end(start_req_body, &pos);

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

    if (type !=bt_BeginRequest)
	{
		// Invalid message
		ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, p, "mod_jaxer: received invalid data type (%c) for a BEGIN_REQUEST block", type);
        return HTTP_SERVICE_UNAVAILABLE;
	}

    if (jaxer_reply != 1)
    {
		if (protocol == 3 && ac->worker->use_proto > 3 && ac->worker->use_proto == ac->worker->my_proto)
		{
			// retry
			ac->worker->use_proto = protocol;
			ac->worker->retry = 1;
			ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, p, "mod_jaxer: Jaxer (protocol version=%d) cannot handle the protocol (version=%d).  Down grading mod_jaxer protocol version to %d.", protocol,
            ac->worker->my_proto, protocol);
			return HTTP_SERVICE_UNAVAILABLE;
		}

        ap_log_perror(APLOG_MARK, APLOG_ERR, 0, p, "mod_jaxer: Jaxer (protocol version=%d) cannot handle the protocol (version=%d)", protocol,
            ac->worker->my_proto);
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
            ac->worker->my_proto);
        return HTTP_SERVICE_UNAVAILABLE;
	}

    
    return rc;
}

apr_status_t jxr_send_message(jaxer_connection *ac, unsigned char *msg)
{
    int rc;
	apr_size_t msglen, pos;
	apr_pool_t *p = (ac->request) ? ac->request->pool : ac->worker->pool;
	
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
    Jaxer_Header jx_hdr;
	int rc;
	apr_size_t msglen, pos;
	apr_pool_t *p = (ac->request) ? ac->request->pool : ac->worker->pool;
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

    *pmsg = (unsigned char*)apr_palloc(r->pool, msglen + sizeof(Jaxer_Header));
	if (! *pmsg)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: cannot allocate memory.");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	memcpy(*pmsg, (const void*) &jx_hdr, sizeof(Jaxer_Header));

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: receiving messsage body (type=%s len=%d)", g_BlockTypes[(*pmsg)[0]], msglen);

	rc = jxr_socket_recvfull(ac, (*pmsg) + sizeof(Jaxer_Header), (int) msglen);
	if(rc < 0)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rc, p, "mod_jaxer: read data from socket error");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: received a message (sock=%d)", ac->sock);
	
    return APR_SUCCESS;
} 

apr_status_t jxr_connect_socket(jaxer_connection * ac)
{
	apr_status_t rv;
	apr_socket_t *sock = 0;
	jaxer_worker *aworker = ac->worker;
	apr_sockaddr_t *remote_sa = aworker->remote_sa;
	apr_pool_t *p = ac->worker->pool;

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
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
				"mod_jaxer: apr_socket_opt_set failed to set keep alive: return code=%d", rv);
		return rv;
	}
	

	// APR_SO_LINGER?
	rv = apr_socket_opt_set(sock, APR_SO_LINGER, 0);
	if (rv != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
				"mod_jaxer: apr_socket_opt_set failed to set keep alive: return code=%d", rv);
		return rv;
	}

	//TIME OUT
	rv = apr_socket_timeout_set(sock, apr_time_from_sec(3));
    if (rv != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
				"mod_jaxer: apr_socket_opt_set failed to set timeout: return code=%d", rv);
		return rv;
	} 
#endif
	
	// Block first
	rv = apr_socket_opt_set(sock, APR_SO_NONBLOCK, 0);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p,
				"mod_jaxer: apr_socket_opt_set failed to set blocking: return code=%d", rv);
		return rv;
	}

	rv = apr_socket_connect(sock, remote_sa);
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
	apr_status_t rv;
	apr_pool_t *p = (pac->request) ? pac->request->pool : pac->worker->pool;

	rv = jxr_connect_socket(pac);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p, "mod_jaxer: jxr_connect_socket failed");
		pac->inuse = 0;
		return rv;
	}
	return rv;
}


apr_status_t jxr_connect_and_begin_request(jaxer_connection *pac, enum eRequestType req_type)
{
	// Connect socket.  Send BeginRequest
	apr_status_t rv;
	apr_pool_t *p = (pac->request) ? pac->request->pool : pac->worker->pool;

	rv = jxr_connect(pac);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p, "mod_jaxer: jxr_connect failed");
		pac->inuse = 0;
		return rv;
	}

	// Send BeginRequest
	rv = jxr_send_begin_request_messsage(pac, req_type);
	if (rv != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, p, "mod_jaxer: send begin request message failed");
		pac->inuse = 0;
		return rv;
	}
	return rv;
}



apr_status_t jxr_send_brigade(jaxer_connection * ac, apr_bucket_brigade * bb)
{
	apr_bucket *bucket;
	apr_status_t rv;
	
	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, ac->request, "mod_jaxer: sending a brigade (sock=%d)", ac->sock);

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
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, ac->request,
						 "mod_jaxer: can't read request from bucket");
			return rv;
		}

		{
			int type = jxr_msg_get_type(write_buf);
			apr_size_t pos; // not used
			apr_size_t len = jxr_msg_get_length(write_buf, &pos);
			ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, ac->request, "mod_jaxer: sending a brigade (type=%s len=%d)", g_BlockTypes[type], len);
		}

		/* Write the buffer to jaxer server */
		if(0 > jxr_socket_sendfull(ac, write_buf, (int) write_buf_len))
		{
		
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, APR_FROM_OS_ERROR(rv), ac->request,
					"mod_jaxer: can't write to socket");
			return apr_get_os_error();
		}
	}
	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, ac->request, "mod_jaxer: sent a brigade (sock=%d)", ac->sock);

	return APR_SUCCESS;
}

apr_status_t jxr_send_file_add_header(jaxer_connection * ac, apr_file_t * file, enum BlockType bType)
{
	//apr_bucket *bucket;
	apr_status_t rv;
	Jaxer_Header jx_hdr;
	request_rec* r = ac->request;
    apr_off_t offset = 0;
	int rc;
    apr_size_t nmax = MAX_PACKET_SIZE-sizeof(Jaxer_Header);
    apr_size_t nbytes = nmax;
    unsigned char buf[MAX_PACKET_SIZE];


	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending a file and add header (type=%s sock=%d)", 
		g_BlockTypes[bType], ac->sock);

    rv = apr_file_seek(file, APR_SET, &offset);
    if (rv != APR_SUCCESS)
    {
        ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
						 "mod_jaxer: can't seek to beginning of file.");
			return rv;
    }

    rv = apr_file_read(file, buf, &nbytes);
    while (rv == APR_SUCCESS && nbytes)
    {
        apr_size_t pos;
        jxr_msg_init((unsigned char*) &jx_hdr, &pos, bType);
		pos += nbytes;
		jxr_msg_end((unsigned char*) &jx_hdr, &pos);

		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending header");

		rc = jxr_socket_sendfull(ac, (unsigned char*)&jx_hdr, (int) sizeof(Jaxer_Header));
		if(rc < 0)
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rc, r, "mod_jaxer: read data from socket error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}

		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending body (len=%d)", nbytes);

		rc = jxr_socket_sendfull(ac, buf, (int) nbytes);
		if(rc < 0)
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rc, r, "mod_jaxer: write data to socket error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}

        //update read-length
        nbytes = nmax;
        rv = apr_file_read(file, buf, &nbytes);
    }
	
	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sent a file and added header (type=%s sock=%d)", 
		g_BlockTypes[bType], ac->sock);

	return APR_SUCCESS;
}



apr_status_t jxr_send_brigade_add_header(jaxer_connection * ac, apr_bucket_brigade * bb, enum BlockType bType)
{
	apr_bucket *bucket;
	apr_status_t rv;
	Jaxer_Header jx_hdr;
	request_rec* r = ac->request;
	int rc;


	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending a brigade and add header (type=%s sock=%d)", 
		g_BlockTypes[bType], ac->sock);

	for (bucket = APR_BRIGADE_FIRST(bb);
		 bucket != APR_BRIGADE_SENTINEL(bb);
		 bucket = APR_BUCKET_NEXT(bucket)) 
	{
		char *write_buf;
		apr_size_t write_buf_len;
		apr_size_t pos;
		
		if (APR_BUCKET_IS_EOS(bucket))
			break;

		if (APR_BUCKET_IS_FLUSH(bucket))
			continue;

		if ((rv = apr_bucket_read(bucket, (const char **)&write_buf, &write_buf_len,
							 APR_BLOCK_READ)) != APR_SUCCESS) {
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r,
						 "mod_jaxer: can't read request from bucket");
			return rv;
		}

		jxr_msg_init((unsigned char*) &jx_hdr, &pos, bType);
		pos += write_buf_len;
		jxr_msg_end((unsigned char*) &jx_hdr, &pos);

		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending header");

		rc = jxr_socket_sendfull(ac, (unsigned char*)&jx_hdr, (int) sizeof(Jaxer_Header));
		if(rc < 0)
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rc, r, "mod_jaxer: read data from socket error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}

		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending body (len=%d)", write_buf_len);

		rc = jxr_socket_sendfull(ac, write_buf, (int) write_buf_len);
		if(rc < 0)
		{
			ap_log_rerror(APLOG_MARK, APLOG_WARNING, rc, r, "mod_jaxer: read data from socket error");
			return HTTP_INTERNAL_SERVER_ERROR;
		}
	}


	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sent a brigade and added header (type=%s sock=%d)", 
		g_BlockTypes[bType], ac->sock);

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

	jxr_init_header(bType, msglen, &jx_hdr);

	vec[0].iov_base = (unsigned char*)&jx_hdr;
	vec[0].iov_len = sizeof(Jaxer_Header);
	vec[1].iov_base = (char*) buf;
	vec[1].iov_len = msglen;
	to_send = msglen + sizeof(Jaxer_Header);

	
	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, "mod_jaxer: sending a message (type=%s len=%d sock=%d)", 
		g_BlockTypes[bType], msglen, ac->sock);

    if(get_network_dump(ac->worker->server))
    {
        jxr_trace("SEND", vec[0].iov_base, (int)vec[0].iov_len, r->pool);
        jxr_trace("SEND", buf, msglen, r->pool);
    }

	if((rv = apr_socket_sendv(sock, vec, 2, &len)) != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: error in sending a messasge");
		return rv;
	}

	if (len < to_send)
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: sending message error: send %d bytes of %d", len, to_send);
		return (!rv);
	}
	return rv;
}

static apr_status_t jxr_conn_close(void *data)
{
    apr_status_t rv = APR_SUCCESS;
    jaxer_connection *ac= (jaxer_connection*)data;
    if (ac->sock)
	{
		if((rv = apr_socket_close(ac->sock)) != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, (ac->request) ? ac->request->pool : ac->pool,
				"mod_jaxer: apr_socket_close failed: return code=%d", rv);
		}

		ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, (ac->request) ? ac->request->pool : ac->pool,
            "mod_jaxer: jxr_conn_close: connection (%d) closed", ac->sock);

		ac->sock = 0;
		ac->has_error = 0;
		ac->request = 0;
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
    apr_pool_t *rec_pool;
	apr_status_t rv = APR_SUCCESS;

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: jxr_conn_construct: creating a new connection");

    rv = apr_pool_create(&rec_pool, p);
    if (rv != APR_SUCCESS) {
        ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p,
                     "mod_jaxer: jxr_conn_construct: Failed to create memory pool");
        return rv;
    }

	ac = apr_pcalloc(rec_pool, sizeof(jaxer_connection));
	if (!ac)
	{
		rv = APR_ENOMEM;
        apr_pool_destroy(rec_pool);
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: allocate memory for jaxer_connection failed");
		return rv;
	}

	// Initial setup for connection (non-zero values)
	ac->worker = aw;
	ac->start_time = apr_time_now();
    ac->pool = rec_pool;
    apr_pool_cleanup_register(ac->pool, ac, jxr_conn_close,
                              apr_pool_cleanup_null);

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: jxr_conn_construct: trying to connect & talk to jaxer");

	if ((rv = jxr_connect(ac)) != APR_SUCCESS)
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, p, "mod_jaxer: jxr_connect failed");
        apr_pool_destroy(ac->pool);
		return rv;
	}

	*pac = ac;

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, p, "mod_jaxer: jxr_conn_construct: a new connection (%d) was created successfully", ac->sock);

	return rv;
}

static apr_status_t jxr_conn_destruct(void *conn, void *params, apr_pool_t *p)
{
	apr_status_t rv = APR_SUCCESS;
	jaxer_connection *ac = conn;
    jaxer_worker* aw = params;

	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, (ac->request) ? ac->request->pool : p,
        "mod_jaxer: jxr_conn_destruct: closing connection (%d)", ac->sock);


	if (ac->sock)
	{
		if((rv = apr_socket_close(ac->sock)) != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_WARNING, rv, (ac->request) ? ac->request->pool : p,
				"mod_jaxer: apr_socket_close failed: return code=%d", rv);
		}

		ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, (ac->request) ? ac->request->pool : p,
            "mod_jaxer: jxr_conn_destruct: connection (%d) closed", ac->sock);

		ac->sock = 0;
		ac->has_error = 0;
		ac->request = 0;
	}
    if (aw->destroyed)
    {
        apr_pool_destroy(ac->pool);
    }

	return rv;
}

static apr_status_t jxr_aw_destroy(void *data)
{
    jaxer_worker* aw = data;

    aw->destroyed = 1;

    return APR_SUCCESS;
}

apr_status_t jxr_conn_setup(jaxer_worker* aw)
{
	apr_status_t rv = APR_SUCCESS;


	ap_log_perror(APLOG_MARK, APLOG_DEBUG, 0, aw->pool, "mod_jaxer: jxr_conn_setup: creating connection pool (min=%d keep=%d max=%d)",
			aw->nmin, aw->nkeep, aw->nmax);

	rv = apr_reslist_create(&aw->ac_cache, aw->nmin, aw->nkeep, aw->nmax, apr_time_from_sec(aw->exptime),
		jxr_conn_construct, jxr_conn_destruct, aw, aw->res_pool);

	if (rv == APR_SUCCESS)
	{
#ifndef _APACHE20N
		apr_reslist_timeout_set(aw->ac_cache, apr_time_from_sec(aw->acquire_timeout));
#endif

		apr_pool_cleanup_register(aw->res_pool, aw->ac_cache, (void*)jxr_aw_destroy, apr_pool_cleanup_null);

		ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, aw->res_pool, "mod_jaxer: connection pool (min=%d keep=%d max=%d) created",
			aw->nmin, aw->nkeep, aw->nmax);
		
	}else
	{
		ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, aw->res_pool, "mod_jaxer: failed to initialize connection reslist");
		// apr_pool_destroy(aw->pool);
		// aw->pool = 0;
	}

	

	return rv;

}

/*
 * This function gets a connection back either by connecting it, or reuse one.
 */
apr_status_t jxr_conn_open(jaxer_worker* aw, jaxer_connection **pac, request_rec *r, enum eRequestType req_type)
{
	apr_status_t rv = APR_SUCCESS;
	
	*pac = 0;

	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: entered jxr_conn_open");

	if (!aw->ac_cache)
	{
		// Create the resource first.  Grab the mutex
#if APR_HAS_THREADS
		rv = apr_thread_mutex_lock(aw->mutex);
		if (rv != APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_CRIT, rv, r, "mod_jaxer: Failed to acquire thread mutex for jaxerworker");
			return rv;
		}
#endif

		// Make sure it is indeed uncreated
		if (!aw->ac_cache)
		{
			ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: creating the connection pool for worker");
			rv = jxr_conn_setup(aw);
		}

		// Before we do anything else, release the mutex.
#if APR_HAS_THREADS
		if (apr_thread_mutex_unlock(aw->mutex) != APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_CRIT, 0, r, "mod_jaxer: Failed to release thread mutex for jaxerworker");
		}
#endif

		if (rv != APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_CRIT, rv, r, "mod_jaxer: jxr_conn_setup failed for jaxerworker");
			return rv;
		}
		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: created the connection pool for worker");

	}

	// At this point, we have a cache
	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: acquiring a connection for worker");

	rv = apr_reslist_acquire(aw->ac_cache, (void **)pac);
	if (rv != APR_SUCCESS)
	{
		ap_log_rerror(APLOG_MARK, APLOG_CRIT, rv, r, "mod_jaxer: Failed to acquire connection from pool for jaxerworker");
		return rv;
	}

	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: acquired a connection (%d)", (*pac)->sock);

	// Setup additional parameters
	(*pac)->request = r;
	(*pac)->has_error = 0;
	(*pac)->last_active_time = apr_time_now();
	
	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: checking connection (%d)", (*pac)->sock);

	// Make sure it is connected
	if((rv = jxr_send_begin_request_messsage(*pac, req_type)) != APR_SUCCESS &&
			((rv = jxr_connect_and_begin_request(*pac, req_type)) != APR_SUCCESS))
	{
		ap_log_rerror(APLOG_MARK, APLOG_WARNING, rv, r, "mod_jaxer: reuse connection or reconnect failed");
#ifndef _APACHE20N
		apr_reslist_invalidate(aw->ac_cache, *pac);
#endif
		*pac = 0;

		return rv;
	}

	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, "mod_jaxer: jxr_conn_open: acquiredconnection (%d) successfully", (*pac)->sock);

	return rv;

}

