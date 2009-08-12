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
#include "stdafx.h"
#include "jaxer_connection.h"
#include "jaxer_proc.h"

static HelloMsg helloMsg;

//Connection pool
CJCList::CJCList(CJaxerConnection *c, CJCList* n)
:m_connection(c)
,m_next(n)
{
}

CJCList::~CJCList()
{
    GetJaxerLog().Log(eTRACE, "~CJCList");
    delete m_connection;
    m_connection = 0;
    // delete m_next;
}


 
CJCPool::CJCPool(CJaxerServer* s)
:m_server(s)
,m_nConnections(0)
{
    m_header = new CJCList();
    m_tail = m_header;
}

CJCPool::~CJCPool()
{
    GetJaxerLog().Log(eDEBUG, "in ~CJCPool: Number of idle connections=%d.", m_nConnections);
    CJCList* p = m_header;
    m_header = 0;
    while(p)
    {
        CJCList* q = p->m_next;
        delete p;
        p = q;
        m_nConnections--;
    }
    //Extra is Header
    GetJaxerLog().Log(eDEBUG, "End of ~CJCPool: Number of idle connections=%d (Should be 0).", m_nConnections+1);
}

CJaxerConnection* CJCPool::CreateNewConnection()
{
    CJaxerConnection* c = new CJaxerConnection(m_server);
    if (!c)
    {
        GetJaxerLog().Log(eERROR, "CreateNewConnection: out of memory");
        return c;
    }

    if (!c->ConnectToJaxer() || !c->BeginRequest())
    {
        GetJaxerLog().Log(eERROR, "CreateNewConnection failed");
        delete c;
        c = 0;
    }
    return c;
}

CJaxerConnection* CJCPool::GetConnection()
{
    m_lock.Lock();
    CJaxerConnection* c = 0;
    if (m_header && m_header->m_next)
    {
       CJCList* l = m_header->m_next;
       m_header->m_next = l->m_next;
       if (m_header->m_next == 0)
       {
           m_tail = m_header;
           if (m_nConnections != 1)
           {
               GetJaxerLog().Log(eWARN, "GetConnection: remaing # of idle connections=%d (expected 1).", m_nConnections);
           }
       }

       c = l->m_connection;
       l->m_connection = 0;
       delete l;
       l = 0;

       m_nConnections--;

       GetJaxerLog().Log(eDEBUG, "GetConnection: remaing # of idle connections=%d.", m_nConnections);

       if(!c->BeginRequest())
       {
           GetJaxerLog().Log(eINFO, "GetConnection: Existing connection cannot be reused; will create new connection.");
           delete c;
           c = 0;
       }
    }
    
    if (!c)
        c = CreateNewConnection();

    m_lock.Unlock();

    return c;
}

void CJCPool::PutConnection(CJaxerConnection* c)
{
    GetJaxerLog().Log(eDEBUG, "PutConnection 0x%0x", c);
    m_lock.Lock();
    CJCList *l = new CJCList(c);
    m_tail->m_next = l;
    m_tail = l;
    m_nConnections++;
    GetJaxerLog().Log(eDEBUG, "PutConnection: remaing # of idle connections=%d.", m_nConnections);
    m_lock.Unlock();
}


BOOL CJaxerConnection::ConnectToJaxer()
{
    DWORD err;
    char *s;
    
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        GetJaxerLog().Log(eERROR, "Cannot create socket error=%d", WSAGetLastError());
        return FALSE;
    }

    const char * host = m_server->GetHostName();
    int port = m_server->GetPort();
    

    // Bind it to the desired port.
    sockaddr_in service;
    service.sin_family = AF_INET;
    unsigned long saddr = inet_addr(host);
    service.sin_addr.s_addr = saddr; // inet_addr(m_server->GetHostName());
    service.sin_port = htons(port); // m_server->GetPort());

    s = inet_ntoa(service.sin_addr);
    if (connect(m_socket, (SOCKADDR *) &service, sizeof(service)) == SOCKET_ERROR)
   {
      err = WSAGetLastError();
      GetJaxerLog().Log(eERROR, "Cannot connect socket error=%d", err);
        
      closesocket(m_socket);
      // WSACleanup();
      return FALSE;
   } 

    return TRUE;
}

#if 0
BOOL CJaxerConnection::SayHelloToJaxer()
{
    if (! SendMsg(helloMsg.Getmsg()))
	{
        GetJaxerLog().Log(eERROR, "SayHelloToJaxer: faile to send hello");
        return FALSE;
	}

    apr_size_t pos;
    size_t msglen;
    char type;
    char buf[256];
	// Get Response
	if (! RecvMsgToBuffer(buf, 256))
	{
		GetJaxerLog().Log(eERROR, "SayHelloToJaxer: faile to recv hello");
        return FALSE;
	}
	msglen = jxr_msg_get_length(buf, &pos);
	type = jxr_msg_get_type(buf);
	if (msglen < 2)
	{
		// Invalid message
		GetJaxerLog().Log(eERROR, "SayHelloToJaxer: received invalid data length (%d) for a HELLO block", msglen);
		return FALSE;
	}
	if (type != BLOCKTYPE_HELLO)
	{
		// Invalid message
		GetJaxerLog().Log(eERROR, "jaxer_isapi: received invalid data type (%c) for a HELLO block", type);
		return FALSE;
	}
	int protocol = jxr_msg_get_int16(buf, &pos);
	if (protocol != JAXER_PROTOCOL_VERSION)
	{
		// Invalid message
		GetJaxerLog().Log(eERROR, "jaxer_isapi: invalid protocol (%d) for a HELLO block (only supports version %d", protocol,
            JAXER_PROTOCOL_VERSION);
		return FALSE;
	}

	return TRUE;
}
#endif


BOOL CJaxerConnection::SendMsg(const char* msg)
{
    int rc;
	apr_size_t msglen, pos;
	
    msglen = jxr_msg_get_length(msg, &pos);
	rc = send(m_socket, msg, (int) msglen + sizeof(Jaxer_Header), 0);
	if(rc < 0)
	{
		GetJaxerLog().Log(eERROR, "SendMsg: send data over socket error");
		return FALSE;
	}
    return TRUE;
}

BOOL CJaxerConnection::BeginRequest()
{
    static char start_req_body[26];
    static int inited = 0;
    char buf[512];
	apr_size_t pos;
    
    if (!inited)
	{
        char req_type = 0;
    
		jxr_msg_init(start_req_body, &pos, BLOCKTYPE_BEGIN_REQUEST);
		jxr_msg_append_int16(start_req_body, &pos, JAXER_PROTOCOL_VERSION);
		jxr_msg_append_byte(start_req_body, &pos, req_type);
		jxr_msg_end(start_req_body, &pos);
		inited = 1;
	}

	if (!SendMsg(buf))
    {
        GetJaxerLog().Log(eDEBUG, "BeginRequest: SendMsg failed.");
        return FALSE;
    }
    if (!RecvMsgToBuffer(buf, 512))
    {
        GetJaxerLog().Log(eDEBUG, "BeginRequest: RecvMsgToBuffer failed.");
        return FALSE;
    }

    apr_size_t msglen = jxr_msg_get_length(buf, &pos);
	
	if (msglen < 3)
	{
		// Invalid message
		GetJaxerLog().Log(eDEBUG, "BeginRequest: received invalid data length (%d) for a BEGIN_REQUEST block", msglen);
		return FALSE;
	}
    
    char type = jxr_msg_get_type(buf);
    int protocol = jxr_msg_get_int16(buf, &pos);
    int jaxer_reply = jxr_msg_get_byte(buf, &pos);
	if (type != BLOCKTYPE_BEGIN_REQUEST)
	{
		// Invalid message
		GetJaxerLog().Log(eERROR, "jaxer_isapi: received invalid data type (%c) for a BEGIN_REQUEST block", type);
		return FALSE;
	}

    if (jaxer_reply != 1)
    {
        GetJaxerLog().Log(eERROR, "mod_jaxer: Jaxer (protocol version=%d) cannot handle the protocol (version=%d)", protocol,
            JAXER_PROTOCOL_VERSION);
        if (msglen>=5)
        {
            // we have an error code from jaxer
            char sjaxer_error[512];
            int jaxer_error;
            jaxer_error = jxr_msg_get_int16(buf, &pos);
            sjaxer_error[0] = 0;
            if (msglen >7)
                jxr_msg_get_string(buf, &pos, sjaxer_error);
            GetJaxerLog().Log(eERROR, "mod_jaxer: jaxer returned (%d) %s", jaxer_error, sjaxer_error);
        }
            
		return FALSE;
    }

	if (protocol != JAXER_PROTOCOL_VERSION)
	{
		// Invalid message
		GetJaxerLog().Log(eERROR, "mod_jaxer: unsupported protocol (%d) received.  Only supports version %d", protocol,
            JAXER_PROTOCOL_VERSION);
        return FALSE;
	}  

    return TRUE;
}

BOOL CJaxerConnection::SendPostDataFrag(const char *data, size_t len)
{
    return SendData(data, (int) len, BLOCKTYPE_POSTDATA);
}

BOOL CJaxerConnection::SendDocumentFrag(const char *data, size_t len)
{
    return SendData(data, (int) len, BLOCKTYPE_DOCUMENT);
}

BOOL CJaxerConnection::SendEndRequest()
{
    char end[1];
    return SendData(end, 0, BLOCKTYPE_ENDREQUEST);
}

BOOL CJaxerConnection::SendData(const char* buf, int msglen, enum BlockType bType)
{
    DWORD err;
	// DWORD len = 0;
	// DWORD to_send = 0;
	// WSABUF vec[2];
	Jaxer_Header jx_hdr;
    // int rv;
	
	jxr_init_header(bType, msglen, &jx_hdr);

#if 0
	vec[0].buf = (char*)&jx_hdr;
	vec[0].len = sizeof(Jaxer_Header);
	vec[1].buf = (char*) buf;
	vec[1].len = msglen;
	to_send = msglen + sizeof(Jaxer_Header);

	
	rv = WSASend(m_socket, vec, 2, &len, 0, NULL, NULL);
    if (rv != 0)
	{
        err = WSAGetLastError();
		return FALSE;
	}

	if (len < to_send)
	{
		return FALSE;
	}
#endif
    if (send(m_socket, (const char*) &jx_hdr,  sizeof(Jaxer_Header), 0) == SOCKET_ERROR)
    {
        err = WSAGetLastError();
        GetJaxerLog().Log(eERROR, "SendData: send over socket (hdr) failed, err=%d", err);
		return FALSE;
    }

    if (send(m_socket, buf,  msglen, 0) == SOCKET_ERROR)
    {
        err = WSAGetLastError();
        GetJaxerLog().Log(eERROR, "SendData: send over socket (msg, len=%d) failed, err=%d", msglen, err);
		
		return FALSE;
    }


	return TRUE;
}

BOOL CJaxerConnection::RecvMsgToBuffer(char* buf, int len)
{
    int rv = recv(m_socket, buf, len, 0);
    return (rv!=0 && rv != SOCKET_ERROR);
}

BOOL CJaxerConnection::RecvMsg(char** msg)
{
    // Caller frees msg
    Jaxer_Header jx_hdr;

    apr_size_t pos;
    int rv = recv(m_socket, (char*)&jx_hdr, sizeof(jx_hdr), 0);
    if (rv != sizeof(jx_hdr))
    {
        DWORD err = WSAGetLastError();
        GetJaxerLog().Log(eERROR, "RecvMsg: recv hdr failed: rv = %d error = %d", rv, err);
                
        return FALSE;
    }
    apr_size_t msglen = jxr_msg_get_length((const char*)&jx_hdr, &pos);

    *msg = new char[msglen+sizeof(jx_hdr)];
    // TODO: Check msg for error
    memcpy(*msg, &jx_hdr, sizeof(jx_hdr));
    if (msglen > 0)
    {
        int recved = 0;
        while (recved < (int)msglen)
        {
            rv = recv(m_socket, (*msg)+sizeof(jx_hdr)+recved, (int)msglen-recved, 0);
            if (rv == 0 || rv == SOCKET_ERROR)
            {
                delete[] *msg;
                *msg = 0;
                GetJaxerLog().Log(eERROR, "RecvMsg: recv msg (%d) failed: rv = %d, err=%d", msglen, rv, WSAGetLastError());
            
                return FALSE;
            }
            recved += rv;
        }
    }
    return TRUE;
}

BOOL CJaxerConnection::RecevMsgIfAny(char** msg)
{
    // Return FALSE if failure,
    // Otherwise *msg will not be null
    // if there is msg
    
	int rc;
    *msg = 0;

	 u_long iMode = 1; //Non Blocking
    if (ioctlsocket(m_socket, FIONBIO, &iMode))
    {
        DWORD err = WSAGetLastError();
        GetJaxerLog().Log(eERROR, "RecevMsgIfAny: ioctlsocket turn on non-block failed: err = %d", err);
        
        return FALSE;
    }

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
    fd_set fdread;
	FD_ZERO(&fdread);
	FD_SET(m_socket, &fdread);
			
	rc = select(1 + (int)m_socket, &fdread, NULL, NULL, &tv);
	if (rc < 0)
	{
		GetJaxerLog().Log(eERROR, "RecevMsgIfAny: socket select failed: err = %d", WSAGetLastError());
        return FALSE;
	}
	
	
	/* turn off non-blocking */
    iMode = 0;
	if (ioctlsocket(m_socket, FIONBIO, &iMode))
    {
        DWORD err = WSAGetLastError();
        GetJaxerLog().Log(eERROR, "RecevMsgIfAny: ioctlsocket turn off non-block failed: err = %d", err);
        
        return FALSE;
    }


	/* return if nothing */
	if (rc == 0)
		return TRUE;


	/* 
	 * Since we have only one socket, we know we have data for reading if we are here.
	 * read a message
	 */
	if (! RecvMsg(msg))
	{
		GetJaxerLog().Log(eERROR, "RecevMsgIfAny: RecvMsg (hdr or post) failed");
        return FALSE;
	}
	return TRUE;
}