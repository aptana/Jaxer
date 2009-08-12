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
#ifndef __JAXER_CONNECTION_H__
#define __JAXER_CONNECTION_H__

#include <winsock2.h>
#include "jaxer_message.h"
#include "jaxer_log.h"

class HelloMsg
{
    char m_msg[256];
    apr_size_t m_pos;
public:
    HelloMsg()
    {
        jxr_msg_init(m_msg, &m_pos, BLOCKTYPE_HELLO);
		jxr_msg_append_int16(m_msg, &m_pos, JAXER_PROTOCOL_VERSION);
		strcpy(m_msg+m_pos, HELLO_STRING);
		m_pos += strlen(HELLO_STRING);
		jxr_msg_end(m_msg, &m_pos);
    }
    const char* Getmsg() const { return m_msg;}
};

class CJaxerServer
{
    char m_host[256];
    int m_port;
public:
    CJaxerServer(const char*host = "127.0.0.1", int port = 4327)
    {
        strcpy(m_host, host);
        m_port = port;
    };


    ~CJaxerServer()
    {
        GetJaxerLog().Log(eTRACE, "~CJaxerServer");
    }

    const char* GetHostName() const { return m_host;};
    int GetPort()const { return m_port;}

    void SetHostName(const char* s) { strcpy(m_host, s);};
    void SetPort(int p) { m_port = p;}
};

class CJaxerConnection
{
    SOCKET m_socket;
    CJaxerServer * m_server;

public:
    CJaxerConnection(CJaxerServer* server)
    {
        m_server = server;
        m_socket = -1;
    }
    ~CJaxerConnection()
    {
        //should not free server
        if ( m_socket != -1)
            closesocket(m_socket);
    }

    BOOL ConnectToJaxer();
    // BOOL SayHelloToJaxer();
    BOOL BeginRequest();
    BOOL SendMsg(const char* msg);
    BOOL SendPostDataFrag(const char *data, size_t len);
    BOOL SendDocumentFrag(const char *data, size_t len);
    BOOL SendEndRequest();
    BOOL RecvMsg(char** msg);
    BOOL RecvMsgToBuffer(char* buf, int len);
    BOOL SendData(const char* buf, int msglen, enum BlockType bType);
    BOOL RecevMsgIfAny(char** msg);
};


class CJCList
{
public:
    CJCList(CJaxerConnection *c=0, CJCList* n=0);
    ~CJCList();
    CJaxerConnection *m_connection;
    CJCList *m_next;
};

class CJCPool
{
public:
    CJCPool(CJaxerServer* s);
    ~CJCPool();
    CJaxerConnection* GetConnection();
    void PutConnection(CJaxerConnection*);
    
private:
    CJaxerConnection* CreateNewConnection();

    CJCList *m_header;
    CJCList *m_tail;
    CJaxerServer* m_server; //Do not own this
    int m_nConnections;
    int m_nMaxConnections;
    int m_nMinConnections;
    CMutex m_lock;
};
#endif // __JAXER_CONNECTION_H__
