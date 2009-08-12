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

// TODO (jeremyw): close on fork/exec, signal handling

#define INVALID_SOCKET -1
#define VOID void

#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/file.h>

#ifdef SOLARIS
#define _XPG4_2
#include <sys/socket.h>
#undef _XPG4_2
#endif

#include <arpa/inet.h>
#include <sys/wait.h>
#include "jaxerBuildId.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <event.h>
#include <evutil.h>
#include <syslog.h>

#include "logclient.h"

// Include declarations common to Windows and Unix.
#include "decl.h"

//
static LogClient lc;
static LogServerInfo lsi;

#if !defined(LINUX_64)
#include "crashreport.h"
#endif

static void RedeployServer(Server *s);
static void KillServer(Server *s);
static void CreateNewProcess();
static void AssignServer(Server *s, Connection *c);

static unsigned g_processCreationDelay = 0;

class Socket;

/***********************************************************************/

typedef void (*Callback)(int, short, void*);

static void EventLogCB(int severity, const char* msg);
static void TimedEventCallback(int socket, short event, void* vp);
static void SocketEventCallback(int socket, short event, void* vp);
static void SysLog(int level, const char* fmt, ...);

class Socket
{
public:
    virtual ~Socket() { }

    virtual bool OnErr() = 0;
    virtual bool OnRecv() = 0;
    virtual bool OnSend() = 0;

    bool m_valid;
    event m_ev;
    short m_events;
    int m_socket;

    Socket() : m_valid(true), m_events(0), m_socket(-1) { }
};

static int AddSocket(Socket *sock)
{
    int s = sock->m_socket;
    Log(eTRACE, "In AddSocket: Socket=%d socket=%d events=%d", sock, s, sock->m_events);
    if (s < 0)
        return 0;

    event_set(&sock->m_ev, s, sock->m_events|EV_PERSIST, SocketEventCallback, sock);
    return event_add (&sock->m_ev, NULL);
}

static int ModSocket(Socket *sock)
{
    int s = sock->m_socket;
    event *ev = &sock->m_ev;
    short evt = sock->m_events;
    Log(eTRACE, "In ModSocket: Socket=%d socket=%d events=%d", sock, s, evt);
    if (s < 0)
        return 0;
    if( 0 != event_del (ev))
    {
      Log(eWARN, errno, "ModSocket(Socket=%d socket=%d events=%d): event_del failed",
         sock, s, evt);
      return -1;
    }
    if (evt == 0 )
      return 0;
    event_set(ev, s, evt|EV_PERSIST, SocketEventCallback, sock);
    return event_add (ev, NULL);
}

static int DelSocket(event* ev)
{
    return event_del (ev);
}

template<class T>
class Socket_: public Socket
{
public:
    Socket_() : m_onRecv(0), m_onSend(0) { }

    virtual bool OnRecv() { return (static_cast<T *>(this)->*m_onRecv)(); }
    virtual bool OnSend() { return (static_cast<T *>(this)->*m_onSend)(); }

protected:
    typedef bool (T::*Callback)();

    Callback        m_onRecv;
    Callback        m_onSend;
};

class Connection: public Socket_<Connection>
{
public:
    //int             m_socket;
    Server         *m_boundServer;
    unsigned        m_requestCount;
    Connection     *m_next;
    bool            m_pending;
    bool            m_hasError;
	int             m_waitTime; // For debugging, time on waiting list
    enum eMSG_TYPE  m_reqType;

    Connection(int s, bool bLookNewReq = true) : m_boundServer(0), m_requestCount(0),
                        m_next(0), m_pending(false), m_hasError(false),
						m_waitTime(0), m_reqType(eINVALID_MSG)
    {
        m_socket = s;
        Log(eTRACE, "In Connection CTOR socket=%d this=%d", s, this);
		if (bLookNewReq)
		{
			m_hasError = (0 != LookForNewRequest());
		}
    }

    ~Connection()
    {
        Log(eTRACE, "In Connection DTOR socket=%d this=%d", m_socket, this);
        DelSocket(&m_ev);
        close(m_socket);
    }

    int LookForNewRequest()
    {
        m_events = EV_READ;
		m_onRecv = &Connection::RequestArrived;
        int ret = AddSocket(this);
        Log(eTRACE, "Connection::LookForNewRequest: AddSocket socket=%d this=%d ret=%d", m_socket, this, ret);
        return ret;
    }

    virtual bool OnErr()
    {
        // Note that this cannot be called while the connection is bound to
        // a server.
        // delete this;
        return false;
    }

private:
    bool RequestArrived();
};

class Administrator: public Socket_<Administrator>
{
    AdminBufferList *m_toHead;
    AdminBufferList *m_toTail;

public:
    char            m_line[512];
    char           *m_lineEnd;
    char           *m_outputBuf;
    int             m_outputLen;

    Administrator(int s, char* outputbuf=0) 
    {
        m_socket = s;
        m_outputBuf = outputbuf;
        m_outputLen = 0;
        m_lineEnd = m_line;
        m_toHead = new AdminBufferList();
        m_toTail = m_toHead;
        m_onRecv = &Administrator::FinishRecv;
        m_onSend = &Administrator::FinishSend;

        Log(eTRACE, "Administrator:CTOR:socket=%d this=%d", m_socket, this);
        if (m_socket != INVALID_SOCKET)
        {
            m_events = (m_socket==1) ? 0 : EV_READ;
            int ret = AddSocket(this);
            Log(eTRACE, "Administrator:CTOR: AddSocket socket=%d this=%d ret=%d", m_socket, this, ret);
        }
    }

    ~Administrator()
    {
        Log(eTRACE, "Administrator:DTOR: socket=%d this=%d", m_socket, this);
        if (m_socket != INVALID_SOCKET)
        {
            DelSocket(&m_ev);
            if ( m_socket != 1)
                close(m_socket);
        }

        while (m_toHead) {
            AdminBufferList *abl = m_toHead;
            m_toHead = abl->m_next;
            delete abl;
        }
    }

	void Write(const char *p, int len = -1);
	void BeginSendNullSocket();
	void Flush();

#if 0
    void Write(const char *p, int len = -1)
    {
        Log(eTRACE,"Enter Administrator::Write: strlen=%d len=%d", strlen(p), len);

        if (len < 0)
            len = int(strlen(p));
        LogTrace(p, len);

        while (len > 0) {
            int amt = BUFSIZE - m_toTail->m_in;
            if (amt > len)
                amt = len;
            memcpy(m_toTail->m_data + m_toTail->m_in, p, amt);
            m_toTail->m_in += amt;
            len -= amt;
            p += amt;

            if (m_toTail->m_in == BUFSIZE)
            {
                Flush();
            }
        }
    }

    void Flush()
    {
        Log(eDEBUG, "In Administrator:Flush socket=%d this=%d", m_socket, this);
        m_toTail->m_flush = true;

        if (m_toTail == m_toHead)
        {
            BeginSend();
        }

        if (m_toHead == 0)
        {
            m_toHead = m_toTail = new AdminBufferList();
        }else
        {
            m_toTail->m_next = new AdminBufferList();
            m_toTail = m_toTail->m_next;
        }
    }
#endif

    virtual bool OnErr()
    {
        // delete this;
        return false;
    }

private:
    void BeginSend()
    {
        Log(eTRACE, "Enter Administrator::BeginSend: socket=%d this=%d", m_socket, this);
        // Special case an Administrator that represents the console.
        if (m_socket == INVALID_SOCKET) {
			BeginSendNullSocket();
			return;
#if 0
            if (m_outputBuf)
            {
                //send output to m_outputBuf
                strncpy(m_outputBuf+m_outputLen, m_toHead->m_data, m_toHead->m_in);
                m_outputLen += m_toHead->m_in;
                m_outputBuf[m_outputLen] = 0;
                Log(eTRACE, "%s", m_outputBuf + (m_outputLen - m_toHead->m_in));
            }else
            {
                char *data = (char *) malloc(m_toHead->m_in + 10);
			
                if(data == NULL)
                {
                    Log(eERROR, errno, "Could not allocate memory for logging, sending to stdout");
                    fwrite(m_toHead->m_data, 1, m_toHead->m_in, stdout);
                    fflush(stdout);
                } else
                {
                    strncpy(data, m_toHead->m_data, m_toHead->m_in);
                    data[m_toHead->m_in] = '\0';
                    Log(eDEBUG, "%s", data);
                    fwrite(data, 1, m_toHead->m_in, stdout);
                    free(data);
                }
            }			
            AdminBufferList *abl = m_toHead;
            m_toHead = abl->m_next;
            if (abl == m_toTail)
                m_toTail = m_toHead;

            delete abl;
            return;
#endif
        }
				
        // Get notified when the socket can take more data.
        m_events = EV_WRITE;
        if (m_socket != 1)
          m_events |= EV_READ;
        if(ModSocket(this))
        {
          Log(eWARN, errno, "Administrator:BeginSend: ModSocket failed");
        }
    }

    bool FinishSend()
    {
        Log(eTRACE, "Enter Administrator::FinishSend: socket=%d this=%d datalen=%d",
            m_socket, this, m_toHead->m_in - m_toHead->m_out);
        
        // Now that it can, give it the data.
        int rc = send(m_socket, m_toHead->m_data + m_toHead->m_out,
                      m_toHead->m_in - m_toHead->m_out, 0);
        if (rc <= 0) {
            // close(m_socket);
            // delete this;
            return false;
        }

        m_toHead->m_out += rc;

        if (m_toHead->m_out == m_toHead->m_in) {
            AdminBufferList *abl = m_toHead;
            m_toHead = abl->m_next;
            if (abl == m_toTail)
                m_toTail = m_toHead;

            delete abl;

            if (!m_toHead || !m_toHead->m_flush) {
                // We are out of data, so cancel notification.
               m_events = (m_socket != INVALID_SOCKET && m_socket != 1) ? EV_READ : 0;
               ModSocket(this);
            }
        }

        return true;
    }

    bool FinishRecv()
    {
        Log(eTRACE, "Administrator:FinishRecv socket=%d this=%d", m_socket, this);
        char buf[512];

		int n;
		if ((n=recv(m_socket, buf, 3, MSG_PEEK)) <= 0)
		{
			Log(eDEBUG, errno, " FinishRecv recv PEEK return %d",  n);
			return false;
        }

#if 0
		//Check to see if this is a webrequest
		if (IsHTTPRequest(buf, n))
		{
			Connection *c = new Connection(m_socket, false);
			c->m_reqType = eNEW_ADMIN_REQUEST_MSG;
			StartNewRequest(c);

			// Retire this
			m_events = 0;
			ModSocket(this);
			m_socket = INVALID_SOCKET;
			return false;
		}
#endif

        int rc = recv(m_socket, buf, 512, 0);
        buf[rc] = 0;

        Log(eTRACE, "In Administrator::FinishRecv datalen(rc)=%d", rc);

        if (rc>0)
        {
            LogTrace(buf, rc);
        }

        if (rc <= 0) {
            // close(m_socket);
            // delete this;
            return false;
        }

        for (int out = 0; out < rc; out++) {
            char c = buf[out];
            if (c == '\n')
            {
                DoCommand(this);
            }else if (c == '\r')
                ;
            else if (m_lineEnd < m_line + 511)
                *m_lineEnd++ = c;
        }

        return true;
    }
};


class Server: public Socket_<Server>
{
public:
    pid_t           m_pid;
    //int             m_socket;
    Connection     *m_boundConnection;
    Server         *m_next;
    Server         *m_nextIdle;
	Server         *m_nextDebug;
    Message        *m_lastSent;
    short           m_secondsToLive;
    bool            m_deletePending;
    bool            m_exiting;
    unsigned        m_nRequests;
    unsigned        m_secsSinceRequestStart;
    unsigned        m_idleTime;

    char            m_inData[CMDBUFSIZE];
    char            m_outData[CMDBUFSIZE];
    bool            m_hasMoreResp;
    int             m_respId;
    ServerSettings  m_privateSettings;
	bool            m_markedForDebug;
	char            m_jaxerID[32];

    Server(pid_t pid, int s) :
        m_pid(pid), m_boundConnection(0), m_next(g_servers),
        m_nextIdle(0), m_nextDebug(0), m_lastSent(0), m_secondsToLive(-1),
        m_deletePending(false), m_exiting(false), m_nRequests(0),
        m_secsSinceRequestStart(0),
        m_hasMoreResp(false), m_respId(0),
        m_idleTime(0), m_markedForDebug(false)
    {
        m_socket = s;
        m_onRecv = &Server::ServerStarted;
        m_events = EV_READ;
        int ret =  AddSocket(this);
        Log(eTRACE, "Server:CTOR AddSocket pid=%d s=%d Svr=%d ret=%d", m_pid, m_socket, this, ret);
        g_servers = this;
		sprintf(m_jaxerID, "%d-%d", pid, time(NULL));
		m_privateSettings.m_owner = this;
    }

    ~Server()
    {
        Log(eTRACE, "Server:DTOR pid=%d s=%d Svr=%d", m_pid, m_socket, this);
        DelSocket(&m_ev);
        close(m_socket);

        // If stopping or reloading, stuff happens once no servers are running.
        if (g_serverCount == 0 && g_debugCount == 0)
            LastServerStopped();
    }

    void BindToConnection(Connection *c)
    {
        Log(eTRACE, "Server:BindToConnection: pid=%d this=%d s=%d c=%d c.s=%d", 
          m_pid, this, m_socket, c, c->m_socket);
        m_boundConnection = c;
        m_secsSinceRequestStart = 0;
        c->m_boundServer = this;
        CountRequest(this);

        // We need to wake up the server.  Wait for notification that we can
        // send stuff to it.
        m_events = EV_READ|EV_WRITE;
        m_onSend = &Server::WakeupServer;
        ModSocket(this);
    }

    virtual bool OnErr()
    {
        // The server has apparently died.
        // KillServer(this);
        return false;
    }

    void PushMessage()
    {
        Log(eTRACE, "Server::PushMessage pid=%d this=%d s=%d live=%d", m_pid, this, m_socket, m_secondsToLive);
        // Wait for notification that we can send stuff to it.
        m_events = EV_READ|EV_WRITE;
        m_onSend = &Server::DoPush;
        ModSocket(this);
    }
    bool ProcessNeedRespCmdHelper (char* cmd, char* resp);
bool ProcessNeedRespCmd ()
{
    //cmd is already in m_inData; and null terminated
    bool ret = ProcessNeedRespCmdHelper(m_inData, m_outData);
	if (!ret) return ret;
#if 0
    unsigned char *p = (unsigned char*)m_inData;
    char *cmd = m_inData;
    int respLen = 0;
    int dwLen = p[1] << 8 | p[2];
    char buf[128];

    //Move to the real cmd
    cmd += 3;
    if (m_respId == 0)
    {
        Log(eDEBUG, "Executing NeedRespCmd %s from server %d", cmd, m_pid);
    }else
    {
        //The original has been destoried due to ScanWord.
        for (int i=0; i<dwLen-1; i++)
            if (cmd[i] == 0)
                cmd[i] = ' ';
    }

    m_respId++;
    m_hasMoreResp = false;

    //fprintf(stderr, "cmd=%s\n", cmd);
    char *opt = ScanWord(cmd);
    if (strcmp("test", opt) == 0)
    {
        opt = ScanWord(cmd);
        if (strcmp("failure", opt) == 0)
        {
            opt = ScanWord(cmd);
            //fprintf(stderr, "opt=%s\n", opt);
            int errCode = 0x7FFF;
            if (opt && *opt)
                errCode = atoi(opt);
            sprintf(buf, "TEST failed CMD response, Error code should be %d", errCode);
            BuildErrorResponse(m_outData, errCode, buf);
        }else
        {
            int num = 1;
            if (opt && *opt)
                 num = atoi(opt);
            if (num < 1) num = 1;
            m_hasMoreResp = (m_respId < num);
            sprintf(buf, "TEST CMD RESPONSE #%d.  TOTAL RESPONSES SHOULD BE %d", m_respId, num);
            BuildCmdResponse(m_outData, buf, (int)strlen(buf), m_hasMoreResp);
        }
    }else if (strcmp("get", opt) == 0)
    {
        Administrator a(INVALID_SOCKET, m_outData+5);
        DoGet(&a, cmd);
        //a.Flush();
        m_outData[0] = eCMD_RESP_MSG;
        respLen = a.m_outputLen + 2;
        m_outData[1] = unsigned((respLen & 0XFF00) >> 8);
        m_outData[2] = unsigned(respLen & 0XFF);
        m_outData[3] = 1; // OK
        m_outData[4] = 0; // no more
    }else if (strcmp("set", opt) == 0)
    {
        Administrator a(INVALID_SOCKET, m_outData+5);
        DoSet(&a, cmd, false);
        a.Flush();
        m_outData[0] = eCMD_RESP_MSG;
        respLen = a.m_outputLen + 2;
        m_outData[1] = unsigned((respLen & 0XFF00) >> 8);
        m_outData[2] = unsigned(respLen & 0XFF);
        m_outData[3] = 1; // OK
        m_outData[4] = 0; // no more
    }else if (strcmp("getme", opt) == 0)
    {
        Administrator a(INVALID_SOCKET, m_outData+5);
        m_privateSettings.DoGet(&a, cmd);
        a.Flush();
        m_outData[0] = eCMD_RESP_MSG;
        respLen = a.m_outputLen + 2;
        m_outData[1] = unsigned((respLen & 0XFF00) >> 8);
        m_outData[2] = unsigned(respLen & 0XFF);
        m_outData[3] = 1; // OK
        m_outData[4] = 0; // no more
    }else if (strcmp("setme", opt) == 0)
    {
        Administrator a(INVALID_SOCKET, m_outData+5);
        m_privateSettings.DoSet(&a, cmd);
        a.Flush();
        m_outData[0] = eCMD_RESP_MSG;
        respLen = a.m_outputLen + 2;
        m_outData[1] = unsigned((respLen & 0XFF00) >> 8);
        m_outData[2] = unsigned(respLen & 0XFF);
        m_outData[3] = 1; // OK
        m_outData[4] = 0; // no more
    }else
    {
        Log(eINFO, "NOT_IMPL: Executing NeedRespCmd %s %s from server %d", opt, cmd, m_pid);
        sprintf(buf, "CMD NOT IMPLEMENTED.");
        BuildCmdResponse(m_outData, buf, (int)strlen(buf), 0);
    }

    ret = SendCmdResponse();
    if (!ret)
    {
        Log(eWARN, "Sending cmd response to server (pid=%d) failed.", m_pid);
        return ret;
    }
#endif

    if (!m_hasMoreResp)
    {
        m_onSend = 0;
        m_events = EV_READ;
        m_respId = 0;
    }
    ModSocket(this);

    return true;     
}

private:
    bool SendCmdResponse()
    {
        //data is in m_outData.
        unsigned char* p = (unsigned char*)m_outData;
        int len = p[1] << 8 | p[2];
        len += 3; //Header
        Log(eTRACE, "Server::SendCmdResponse (len=%d) pid=%d this=%d s=%d", len, m_pid, this, m_socket);

        iovec iov;
        iov.iov_base = m_outData;
        iov.iov_len = len;

        if(g_eLogLevel == eTRACE)
        {
            BlockDumper dump;
            dump.LogHeader("CmdResponse", len);
            dump.LogContents(m_outData, len);
        }

        char msgbuf[CMSG_SPACE(sizeof(int))];
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        // Fake a control msg, so jaxer will receive
        // individula msgs, one at a time.
        // But jaxer should not play with the descriptor.

        msg.msg_control = msgbuf;
        msg.msg_controllen = sizeof(msgbuf);

        cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));

        *(int *)CMSG_DATA(cmsg) = 0;
        msg.msg_controllen = cmsg->cmsg_len;

        if (g_eLogLevel == eTRACE)
        {
            BlockDumper dump;
            dump.LogHeader("SendResp", len);
            dump.LogContents((char*)m_outData, len);

        }
        if (sendmsg(m_socket, &msg, 0)<0)
        {
            Log(eERROR, errno, "SendCmdResponse sendmsg failed");
            return false;
        }

        return true;
    }

    bool WakeupServer()
    { 
        Log(eTRACE, "Enter Server::WakeupServer pid=%d this=%d s=%d Reqtype=%d", m_pid, this, m_socket, m_boundConnection->m_reqType);
        // Wake up server, informing it that it has a new request.  Pass the socket
        // to the server so it can talk to the web server directly.
        m_outData[0] = m_boundConnection->m_reqType; //eNEW_REQUEST_MSG;
        m_outData[1] = 0;
        m_outData[2] = 0;
        m_boundConnection->m_requestCount++;

        iovec iov;
        //JZ iov.iov_base = &m_outData;
        iov.iov_base = m_outData;
        iov.iov_len = 3;

        char msgbuf[CMSG_SPACE(sizeof(int))];
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = msgbuf;
        msg.msg_controllen = sizeof(msgbuf);

        cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));

        *(int *)CMSG_DATA(cmsg) = m_boundConnection->m_socket;
        msg.msg_controllen = cmsg->cmsg_len;

        // No longer need send notifications.
        m_onSend = 0;
        m_events = EV_READ;
        if (0 != ModSocket(this))
        {
            Log(eWARN, errno, "Server:WakeupServer: ModSocket turn off send event failed");
            return false;
        }
        
        if (sendmsg(m_socket, &msg, 0)<0)
        {
            Log(eERROR, errno, "WakeupServer sendmsg failed");
            return false;
        }

        return true;
    }

    bool DoPush()
    {
        Log(eTRACE, "Server::DoPush pid=%d this=%d s=%d live=%d", m_pid, this, m_socket, m_secondsToLive);
        void *p = 0;
        unsigned len = 0;
        m_lastSent->GetBytes(this, p, len);

        iovec iov;
        iov.iov_base = (char*)p;
        iov.iov_len = len;

        char msgbuf[CMSG_SPACE(sizeof(int))];
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        // Fake a control msg, so jaxer will receive
        // individula msgs, one at a time.
        // But jaxer should not play with the descriptor.

        msg.msg_control = msgbuf;
        msg.msg_controllen = sizeof(msgbuf);

        cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));

        *(int *)CMSG_DATA(cmsg) = 0;
        msg.msg_controllen = cmsg->cmsg_len;

        if (g_eLogLevel == eTRACE)
        {
            BlockDumper dump;
            dump.LogHeader("SendMsg", len);
            dump.LogContents((char*)p, len);

        }
        if (sendmsg(m_socket, &msg, 0)<0)
        {
            Log(eERROR, errno, "DoPush sendmsg failed");
            return false;
        }

        // If the message just pushed told the server to exit, then drop it on
        // the floor and wait for it to either die or time out.
        if (m_exiting) {
            m_onSend = 0;
            m_events = EV_READ;
            ModSocket(this);
            return true;
        }

        if (!Message::Next(this)) {
            if (m_markedForDebug)
            {
                Connection* oldC = m_boundConnection;
                if (oldC)
                {
                    oldC->m_boundServer = 0;
                    m_boundConnection = 0;
                    //oldC->m_recving = false;
                    oldC->m_next = 0;
                }
                Log(eTRACE, "Server: DoPush: debug server %d finsihed request %d.",
                    m_pid, oldC);

                Connection* ac = g_debugConnections;
                Connection* parent = 0;
                while(ac)
                {
                    if (ac->m_boundServer == this)
                    {
                        if (parent)
                            parent->m_next = ac->m_next;
                        else
                            g_debugConnections = ac->m_next;

                        Log(eTRACE, "Server: DoPush: let server %d server request %d.", m_pid, ac);

                        m_boundConnection = ac;
                        break;
                    }
                    parent = ac;
                    ac = ac->m_next;
                }
                if (!g_debugConnections || !g_debugConnections->m_next)
                    g_debugConnectionsTail = g_debugConnections;

                if (oldC)
                    oldC->LookForNewRequest();
                if (m_boundConnection);
                {
                    AssignServer(this, m_boundConnection);
                    return true;
                }
            }else
            {

                // No more messages, so we're idle again.  Assign it to a connection
                // if any are waiting (unless paused).
                if (g_connFirst && g_ManagerMode != mm_paused) {
                    Connection *c = g_connFirst;
                    g_connFirst = g_connFirst->m_next;
                    g_waitingConnections--;
                    if (g_connFirst == 0)
                        g_connLast = 0;
                    //m_boundConnection = c;
                    //c->m_boundServer = this;
                    //m_onSend = &Server::WakeupServer;
                    AssignServer(this, c);
        
                    return true;
                }

                // Otherwise, add it to the idle list.
                AddIdleServer(this);
            }

            // No longer need send notifications.
            m_onSend = 0;
            m_events = EV_READ;
            ModSocket(this);
            return true;
        }

       //ModSocket(m_socket, this, true, true);
        return true;
    }

    bool ServerStarted()
    {
        Log(eTRACE, "Enter Server::ServerStarted pid=%d this=%d s=%d", m_pid, this, m_socket);

        // Read a single byte informing us if the server started successfully.
        char byte;
        int rc = recv(m_socket, &byte, 1, 0);
        if (rc == 1 && byte == 1) {
            // It did.
            m_onRecv = &Server::RecvServerMsg;
            // m_events |= EV_READ;
            g_processCreationPending = false;
            g_serverCount++;

            Log(eTRACE, "ServerStarted: pid=%d total#=%d", m_pid, g_serverCount);

            Message::BringUpToDate(this);

            #if 0 //TEST
            if (!g_debugServers)
            {
                g_debugServers = this;
                g_serverCount--;
				g_debugCount++;
                m_markedForDebug = true;
                Server *i = g_idleServers;
                Server *p = 0;
                while(i)
                {
                    if (i==this)
                    {
                        if (p)
                            p->m_nextIdle = i->m_nextIdle;
                        else
                            g_idleServers = i->m_nextIdle;
                        g_idleCount--;
                        break;
                    }
                    p = i;
                    i = i->m_nextIdle;
                }
            }
            #endif //END TEST

            RedeployServer(this);
            return true;
        } else {
            if (rc>0)
                Log(eERROR, "Jaxer creation failed: recved %d bytes. byte1=%d", rc, byte);
            else if (rc == 0)
                Log(eERROR, "Jaxer creation failed: recved zero bytes");
            else
                Log(eERROR, errno, "Jaxer creation failed!");


            g_processCreationDelay = 5;

            // The server will be killed.  But we need to correct the count.
            g_serverCount++;
            return false;
        }
    }

    bool RecvServerMsg()
    {
        Log(eTRACE, "Enter RecvServerMsg: pid=%d this=%d s=%d live=%d", m_pid, this, m_socket, m_secondsToLive);

        int rc = recv(m_socket, m_inData, 3, 0);
        if (rc < 3)
        {
            if (!m_exiting)
            {
                Log(eINFO, "Read data from jaxer (pid=%d) failed (rc=%d). Server will be killed", m_pid, rc);
            }
            return false;
        }

        int msglen = (unsigned char) m_inData[1] << 8 | (unsigned char) m_inData[2];
        if (msglen > CMDBUFSIZE - 4)
        {
            if (!m_exiting)
            {
                Log(eINFO, "Cmd too large (len=%d) from jaxer (pid=%d). Server will be killed", msglen, m_pid);
            }
            return false;
        }

        int nread = 0;
        while(nread<msglen)
        {
            rc = recv(m_socket, m_inData+3+nread, msglen-nread, 0);
            if (rc > 0)
                nread += rc;
            else
            {
                if (!m_exiting)
                {
                    Log(eINFO, "Recv message from jaxer (pid=%d) failed. Server will be killed", m_pid);
                }
                return false;
            }
        }

        rc = msglen + 3;

        if(g_eLogLevel == eTRACE)
        {
            BlockDumper dump;
            dump.LogHeader("recv (ret)", rc);
            dump.LogContents(m_inData, rc);
        }

        m_inData[rc] = 0;
        switch(m_inData[0])
        {
        case eEND_REQUEST_MSG:
            Log(eTRACE, "RecvServerMsg return to pool pid=%d this=%d s=%d live=%d",
               m_pid, this, m_socket, m_secondsToLive);
            RedeployServer(this);
            return true;
            break;

        case eNO_RESP_CMD_MSG:
            Log(eTRACE, "recived NO_RESP_CMD %s from server (pid=%d)", m_inData, m_pid);
            ProcessNoRespCmd(this, m_inData);
            ModSocket(this);
            return true;
            break;

        case eNEED_RESP_CMD_MSG:
            m_respId = 0;
            Log(eTRACE, "recived NEED_RESP_CMD %s from server (pid=%d)", m_inData, m_pid);
            m_events = EV_WRITE;
            m_onSend = &Server::ProcessNeedRespCmd;
            ModSocket(this);
            return true;
            break;

        default:
            // Server appears to have died.
            Log(eTRACE, "RecvServerMsg: (rc=%d) bad server will be killed pid=%d", rc, m_pid);
            return false;
            break;
       }
       Log(eERROR, "RecvServerMsg: unexpected (pid=%d)", m_pid);
       return false;
    }
};

static Server *g_toDelete = 0;

/***********************************************************************/

class Listener: public Socket_<Listener>
{
public:
    //void Bind(const char* server, unsigned port, void (*cb)(int)) 
    void Bind(const ConnectionSetting* cs, void (*cb)(ConnectionWithSetting*)) 
    {
        Log(eTRACE, "Listener:server=%s Bind port=%d L=%d", cs->mInterface, cs->mPort, this);
        m_cs = cs;
        
        m_onRecv = &Listener::Accept;
        m_callback = cb;

        // Create the listening socket.
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket < 0) {
            Log(eFATAL, errno, "Cannot create socket, another JaxerManager could be running.");
            SysLog(LOG_ERR, "Cannot create socket, another JaxerManager could be running: %m.");
            exit(1);
        }

        // Allow reuse of port so that the manager can be restarted faster.
        int opt = 1;
        setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

		if (inet_addr(cs->mInterface) == (unsigned long)-1)
		{
			Log(eWARN, "Cannot determine server (%s) address. Using 127.0.0.1", cs->mInterface);
            free(cs->mInterface);
			cs->mInterface = strdup("127.0.0.1");
		}
        // Bind it to the desired port.
        sockaddr_in service;
        memset(&service, 0, sizeof(service));
        service.sin_family = AF_INET;
        service.sin_addr.s_addr = inet_addr(cs->mInterface);
        service.sin_port = htons(cs->mPort);

        int rc = bind(m_socket, (sockaddr*) &service, sizeof(service));
        if (rc < 0) {
            Log(eFATAL, errno, "Cannot bind port, another JaxerManager could be running");
            SysLog(LOG_ERR, "Cannot bind port, another JaxerManager could be running");
            exit(1);
        }

        // Start listening for connections.
        rc = listen(m_socket, SOMAXCONN);
        if (rc < 0) {
            Log(eFATAL, errno, "listen: Cannot listen on port, another JaxerManager could be running");
            SysLog(LOG_ERR, "listen: Cannot listen on port, another JaxerManager could be running");
            exit(1);
        }

        // Put in non-blocking to prevent obscure blockages.
        int flags = fcntl(m_socket, F_GETFL);
        if (!(flags & O_NONBLOCK))
        {
            rc = fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
        }

        // Register for notification of incoming connections.
        m_events = EV_READ;
        if ( 0 != AddSocket(this))
        {
            Log(eFATAL, errno, "Listener: notification reg failed");
            SysLog(LOG_ERR, "Listener: notification reg failed: %m");
            exit(1);
        }
    }

    virtual bool OnErr()
    {
        return false;
    }


protected:
    bool Accept()
    {
        Log(eTRACE, "in Listener:Accept s=%d this=%d", m_socket, this);
        int s = accept(m_socket, 0, 0);
        Log(eTRACE, "    Accepted s=%d", s);
        if (s < 0) {
            Log(eERROR, errno, "Cannot accept socket, another JaxerManager could be running");
            //exit(1);
        }

        // Make sure socket is blocking.
        int flags = fcntl(s, F_GETFL);
        if (flags & O_NONBLOCK)
        {
            int rc = fcntl(s, F_SETFL, flags & ~O_NONBLOCK);
            rc;
        }

        ConnectionWithSetting *cws = new ConnectionWithSetting(m_cs, s);

        (*m_callback)(cws);
        return true;
    }

    void          (*m_callback)(ConnectionWithSetting*);

    const ConnectionSetting *m_cs;
};

bool Connection::RequestArrived()
    {
        Log(eTRACE, "In Connection::Request Arrived this=%d socket=%d", this, m_socket);
        if (g_ManagerMode == mm_stopping) {
            return false;
        }

        // Ignore the connection while the server is handling it.  An event
        // must be passed even though it is ignored due to a Linux kernel bug
        // in versions prior to 2.6.9.
        m_events = 0;
        DelSocket(&m_ev);

        Log(eTRACE, "  call recv s=%d this=%d", m_socket, this);
        // Make sure the other side didn't close the connection.
        char buf[8192];
		int n;
		int nPeek = (m_reqType == eNEW_REQUEST_MSG || g_debugCount <= 0) ? 1 : 8192;
        if ((n=recv(m_socket, buf, nPeek, MSG_PEEK)) <= 0) {
            // This really should be an error msg.
            Log(eTRACE, "Connection::RequestArrived: recv peek msg failed.", errno);
            // delete this;
            return false;
        }

		if (m_reqType == eNEW_REQUEST_MSG)
		{
			if (buf[0] == bt_DebugRequest)
			{
				// v4.  Need to see if this is a debug request
				Log(eTRACE, "Connection::RequestArrived: DebugRequest(%d).", buf[0]);

				// get the msg
				char mbuf[512]; // this is a small msg
				char *jaxer_id = 0;
				int nr = recv(m_socket, mbuf, 512, 0);
				if (nr <= 0)
				{
					Log(eERROR, "Connection::RequestArrived: recv debug msg failed.", errno);
					return false;
				}
				char *p = mbuf+1;
				int len = ((unsigned char) p[0]) << 8 | (unsigned char) p[1];
				Log(eTRACE, "Connection::RequestArrived: debug msg len=%d.", len);
				p += 2;
				if (len != nr-3)
				{
					Log(eERROR, "Connection::RequestArrived: recv debug msg length mismatch: %d %d.",
						len+3, nr);
					return false;
				}
				int nheaders = ((unsigned char) p[0]) << 8 | (unsigned char) p[1];
				Log(eTRACE, "Connection::RequestArrived: debug msg headesr=%d.", nheaders);
				len -= 2;
				p += 2;
				bool got_id = false;
				for (int k=0; k<nheaders; k++)
				{
					if (len < 4)
					{
						Log(eERROR, "Connection::RequestArrived: recv debug msg failed: processing field# %d of %d, len=%d",
						k+1, nheaders, len);
						return false;
					}
					int nlen, vlen;
					char *name, *value;
					nlen = ((unsigned char) p[0]) << 8 | (unsigned char) p[1];
					p += 2;
					len -= 2;
					name = p;

					p += nlen;
					len -= nlen;

					vlen = ((unsigned char) p[0]) << 8 | (unsigned char) p[1];
					*p = 0;
					p += 2;
					len -= 2;
					value = p;

					p += vlen;
					len -= vlen;

					Log(eTRACE, "Connection::RequestArrived: debug msg name=%s len=%d vlen=%d.",
						name, nlen, vlen);
					// check for the only name we care so far
					if (nlen == strlen("Jaxer-ID") && strncmp(name, "Jaxer-ID", nlen) == 0)
					{
						*p = 0;
						jaxer_id = value;
						got_id = true;
						break;
					}
				}

				if (!got_id)
				{
					Log(eERROR, "recv debug msg failed: cannot find the Jaxer-ID field");
					return false;
				}
				// find the jaxer
				char accept = 0;
				Server *s = g_debugServers;
				Log(eTRACE, "Look for the required jaxer (id=%s) for request %d", jaxer_id, this);
				while(s)
				{
					if (strcmp(s->m_jaxerID, jaxer_id) == 0)
					{
						//Found the server
						Log(eTRACE, "Required debug jaxer id=%d found", s->m_pid);
						accept = 1;
						m_boundServer = s;
						m_next = NULL;
						// Is the server busy?
						if (s->m_boundConnection)
						{
							Log(eTRACE, "Jaxer id=%d is busy, debug request will be put on waiting list",
								s->m_pid, this);
						}
						break;
					}
					s = s->m_next;
				}


				// Inform connector
				char buf2[56];
				memset(buf2, 0, 56);
				char sl = (char) strlen("Accepted");
				buf2[0] = bt_DebugRequest;
				buf2[2] = sl + 7;
				buf2[4] = 1;
				buf2[6] = sl;
				strcpy(buf2+7, "Accepted");
				buf2[8+sl] = 1;
				buf2[9+sl] = accept;

				int ns = send(m_socket, buf2, sl+10, 0);
				if (ns != sl+10)
				{
					Log(eWARN, "Failed to send DebugRequest msg: len=%d send=%d error=%d",
						sl+10, ns, errno);
						return false;
				}
				if (!accept)
				{
					Log(eWARN, "Cannot find the required jaxer id=%s", jaxer_id);
					return false;
				}

				// ok, let jaxer do its work
				if (!m_boundServer->m_boundConnection)
				{
					Log(eDEBUG, "Let jaxer %d server request %d", m_boundServer->m_pid, this);
					AssignServer(m_boundServer, this);
				}else
				{
					Log(eDEBUG, "Jaxer %d busy, request %d put on waiting list",
						m_boundServer->m_pid, this);
					m_waitTime = DEBUG_CONNECTION_TIMEOUT_SECS;
					if (g_debugConnections)
					{
						g_debugConnectionsTail->m_next = this;
						g_debugConnectionsTail = this;
					}else
					{
						g_debugConnectionsTail = g_debugConnections = this;
					}
				}
				return true;
			}else
			{
				Log(eTRACE, "  peek(%d) ok. calling StartNewRequest socket=%d this=%d", buf, m_socket, this);
				StartNewRequest(this);
				return true;
			}
		}else
		{
			// This is a http request
			char *jaxer_id = get_jaxer_id_from_http_req(buf, n);
			if (! jaxer_id)
			{
				StartNewRequest(this);
			}else
			{
				bool bHandled = find_server_and_handle_connection(this, jaxer_id, g_debugServers);
				if ( !bHandled)
				{
					//Cannot find the Jaxer
					Log(eWARN, "  Connection::RequestArrived: cannot find the requested debug jaxer %s. Let other Jaxer serving the request %d", jaxer_id, this);
					StartNewRequest(this);
				}
			}
		}
		return true; //catch all
    }

//static Listener g_administratorListener;
static Listener g_connectionListener[4]; //g_nConnectionSettings;

static void NewAdministrator(ConnectionWithSetting *cws)
{
    Log(eTRACE, "in NewAdministrator s=%d", cws->m_socket);
    Administrator *a = new Administrator(cws->m_socket);
    delete cws;
    Log(eTRACE, "  created Admin %d s=%d", a, a->m_socket);
    a->Write("JaxerManager " JAXER_BUILD_ID "\n\n");
    a->Flush();
}

static void NewConnection(ConnectionWithSetting *cws)
{
    Log(eTRACE, "In NewConnection s=%d", cws->m_socket);
    Connection* c = new Connection(cws->m_socket);
    c->m_reqType = cws->m_cs->mMsgType;
    delete cws;
    Log(eTRACE, "NewConnection: created new connection c=%d err=%d", c, c->m_hasError);
    if (c && c->m_hasError)
    {
        Log(eERROR, "NewConnection: failed to create new connection.");
        delete c;
    }
}

/***********************************************************************/

static void CreateNewProcess()
{
    if (!ProcessCreationAllowed())
        return;

    g_processCreationPending = true;

    // We will communicate with the spawned Jaxer via a Unix-domain socket.
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        Log(eFATAL, errno, "Cannot create Unix-domain socket pair.");
        SysLog(LOG_ERR, "Cannot create Unix-domain socket pair: %m");
        exit(1);
    }

    // Now fork, inheriting the above sockets.
    pid_t pid = fork();
    if (pid < 0) {
        Log(eFATAL, errno, "Cannot fork.");
        SysLog(LOG_ERR, "Cannot fork: %m.");
        exit(1);
    } else if (pid > 0) {
        // We are the original, parent process.  Close the child end of the
        // socket pair and set up a Server object.
        close(sv[1]);
        new Server(pid, sv[0]);
    } else {
        // We are the child.  Close the parent end of the socket pair, and
        // renumber our end to become stdin.  Alse set JAXER_PIPENAME so that
        // the server knows we started it (but is otherwise unused on Unix).
        close(sv[0]);
        dup2(sv[1], 0);
        close(sv[1]);
        putenv("JAXER_PIPENAME=-");
        if (g_ConnectionSettings[eWEB_HTTP_PROTOCOL].mEnabled)
            putenv("ENABLE_JAXER_WEBSERVER=1");

        //Do not bring down the log server even if we fail here
        lsi.SetPid(-1);

#if defined (__linux__) || defined (__sun__)
        // Impose memory limit.
        unsigned g_MaxMemory = g_GeneralSettings[eNAME_JAXER_MAX_MEMORY_MB].miValue;
        if (g_MaxMemory > 0) {
            struct rlimit r;
            getrlimit(RLIMIT_AS, &r);
            r.rlim_cur = g_MaxMemory * 1048576;
            if (setrlimit(RLIMIT_AS, &r) < 0)
            {
                Log(eERROR, "Can't impose memory limit");
            }
            else
           {
              Log(eDEBUG, "Set the memory limits to %d.", g_MaxMemory);
           }
        }
#endif

        int n = g_Settings.size();
        n++; //argv[0], & null
        n *= 2; // name, value

        char **argv = new char*[n];
        if (!argv)
        {
            Log(eERROR, "Failed to allocate (n=%d).", n);
            exit(1);
        }

        int i=0;
#ifdef __APPLE__
        argv[i++] = "Jaxer";
#else
        argv[i++] = "jaxer";
#endif        
        for (iter I = g_Settings.begin(); I != g_Settings.end(); ++I)
        {
			//if (!g_ConnectionSettings[eWEB_HTTP_PROTOCOL].mEnabled && stricmp(I->first.c_str(), "-iwsconfigfile") == 0)
			//	continue;
            argv[i++] = (char*) I->first.c_str();
            argv[i++] = (char*) I->second.c_str();
        }
        argv[i] = 0;
            
        execv(argv[0], argv);

        Log(eFATAL, errno, "execl failed");
        SysLog(LOG_ERR, "execl failed: %m");
        // Inform parent we couldn't exec.
        char byte = 0;
        write(0, &byte, 1);
        exit(1);
    }
}

/***********************************************************************/

static void AssignServer(Server *s, Connection *c)
{
    s->m_privateSettings.ResetOnNewRequest();
    s->BindToConnection(c);
}

static void PushMessage(Server *s)
{
    s->PushMessage();
}

static void KillServer(Server *s)
{
    if (s->m_deletePending)
        return;

    // Remove from idle list if necessary.
    RemoveServer(s);

    s->m_next = g_toDelete;
    s->m_deletePending = true;
    g_toDelete = s;
}

static void SlaughterServer(Server *s, bool isExiting = false)
{
    kill(s->m_pid, SIGKILL);
    KillServer(s);
}

static void RedeployServer(Server *s)
{
    Log(eTRACE, "In RedeployServer: pid=%d this=%d s=%d live=%d", s->m_pid, s, s->m_socket, s->m_secondsToLive);
    if (!s->m_markedForDebug && s->m_boundConnection) {
        Connection *c = s->m_boundConnection;
        s->m_boundConnection->m_boundServer = 0;
        s->m_boundConnection = 0;
        Log(eTRACE, " look for nr");
        c->LookForNewRequest();
    }

    // Here's our chance to push accumulated messages to it.
    if (Message::Next(s)) {
        Log(eTRACE, "Calling PushMessage...");
        s->PushMessage();
        return;
    }

    if (s->m_markedForDebug)
    {
        Connection* oldC = s->m_boundConnection;
        if (oldC)
        {
            oldC->m_boundServer = 0;
            s->m_boundConnection = 0;
            //oldC->m_recving = false;
            oldC->m_next = 0;
            Log(eTRACE, "Server: DoPush: debug server %d finished request %d.",
                s->m_pid, oldC);
        }

        Connection* ac = g_debugConnections;
        Connection* parent = 0;
        if (ac)
            Log(eTRACE, "Server: DoPush: there are witing debug requests.");

        while(ac)
        {
            if (ac->m_boundServer == s)
            {
                if (parent)
                    parent->m_next = ac->m_next;
                else
                    g_debugConnections = ac->m_next;

                Log(eTRACE, "Server: DoPush: let server %d server request %d.", s->m_pid, ac);

                s->m_boundConnection = ac;
                break;
            }
            parent = ac;
            ac = ac->m_next;
        }
        if (!g_debugConnections || !g_debugConnections->m_next)
            g_debugConnectionsTail = g_debugConnections;

        if (oldC)
            oldC->LookForNewRequest();

        if (s->m_boundConnection)
        {
            AssignServer(s, s->m_boundConnection);
            return;
        }
    }else
    {
        // Assign it to a connection if any are waiting (unless paused).
        if (g_connFirst && g_ManagerMode != mm_paused) {
            Connection *c = g_connFirst;
            g_connFirst = g_connFirst->m_next;
            g_waitingConnections--;
            if (g_connFirst == 0)
                g_connLast = 0;
            AssignServer(s, c);
            return;
        }

        // Otherwise, add it to the idle list.
        AddIdleServer(s);
    }
}

static void DoPeriodicLinux()
{
    Log(eTRACE,"In DoPeriodicLinux");
    if (g_processCreationDelay > 0) {
        if (--g_processCreationDelay == 0)
            g_processCreationPending = false;
    }
    DoPeriodic();
}


bool CreateLogServer()
{
    if (lsi.GetProcessStdinHandle() != INVALID_HANDLE_VALUE)
    {
        close(lsi.GetProcessStdinHandle());
        lsi.SetProcessStdinHandle(INVALID_HANDLE_VALUE);
    }

    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) <0)
    {
        perror("CreateLogServer: Failed to create Unix-domain socket pair");
        return false;
    }

    pid_t pid = fork();
    if (pid<0)
    {
        perror("CreateLogServer: Faied to fork");
        return false;
    }
    if (pid>0)
    {
        //we are the parent
        close(sv[0]);
        lsi.SetProcessStdinHandle(sv[1]);
        lsi.SetPid(pid);
    }else
    {
        //we are the child
        close(sv[1]);
        dup2(sv[0], 0);
        close(sv[0]);

        char *argv[100];
        char argv1[100];
        std::string sargs[100];
        int i=0;
#ifdef __APPLE__
        argv[i++] = "JaxerLogger";
#else
        argv[i++] = "jaxerlogger";
#endif        
        sprintf(argv1, "--input=%s", lsi.GetFifoName());
        argv[i++] = argv1;
        for (iter I = g_LogSettings.begin(); I != g_LogSettings.end(); ++I, ++i)
        {
            sargs[i] = "--";
            sargs[i] += I->first;
            sargs[i] += "=";
            sargs[i] += I->second;
            argv[i] = (char*) sargs[i].c_str();
        }
        argv[i] = 0;
            
        //int rc = execl("jaxerlogger", "jaxerlogger", lsi.GetFifoName(), lsi.GetLogFileName(), (char*)NULL);
        execv(argv[0], argv);
        printf("execv(%s) failed: err=%d %s\n", argv[0], errno, strerror(errno)); fflush(stdout);
        SysLog(LOG_ERR, "execv(%s) failed: %m", argv[0]);
        exit(1);
    }
    return true;
}

void InitLogger(const char* sFifoName, const char* sLock)
{
    if( !lc.Init(sFifoName, -1, sLock))
    {
        SysLog(LOG_ERR, "Init LogClient failed: %m.");
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN);
    // This is liklely to fail.
    lc.OpenPipeForWrite();
    //logme("after OpenPipeForWrite");
}

int main(int argc, char **argv)
{
    // If just need the commandline syntex, do not go through the normal process
    if (argc > 1 && strcmp(argv[1], "--help") == 0)
    {
        DisplayUsage();
        return 0;
    }

    // Setup defaults for passing to Jaxer
    InitSettings();

    // Read the deafult configuration file.
    //FILE *fd = fopen(g_ConfigFile, "r");
    //if (fd)
    //{
    //    LoadConfig(fd);
    //}

    // Process args.
    ProcessArgs(argc, argv);
	
    //We need the tmpdir to determine where we should do the dumping.  So this
    //is the earliest point we can do.
    //const char * dumpDir = ".";
    //iter it = g_Settings.find("-tempdir");
    //if (it != g_Settings.end())
    //    dumpDir = it->second.c_str();
	//const char * dumpDir = g_ManagerSettings[NAME_TEMP_DIR].c_str();
	const char* dumpDir = g_Settings["-tempdir"].c_str();
    
#if !defined(LINUX_64)
    //LINUX
    google_breakpad::ExceptionHandler eh(
         dumpDir,                              // Where to dump
         google_breakpad::BeforeDumpCallback,  // FilterCallback
         google_breakpad::AfterDumpCallback,
         NULL,                                 // callback context
         true                                  // always write a dump when exception occurs
         );
#endif

    // Setup log fifo
    char sFifoName[56], sLock[20];
    sprintf(sFifoName, "/tmp/.jaxer.%d.fifo", g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort);
    sprintf(sLock, "%d", g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort);
    lsi.SetFifoName(sFifoName);
    lc.SetSilentOnError();
    //lc.SetVerboseOnError();
    InitLogger(sFifoName, sLock);

    // Do not write log msg if no pidfile is specified.
    char* pidFilename = g_GeneralSettings[eNAME_PID_FILE].msValue;
    if (pidFilename != NULL && !WritePidFile())
    {
        Log(eERROR, "Failed to write pid in file - %s", 
            (pidFilename==NULL) ? "No pid-file specified" : pidFilename);
    }

#ifdef __linux__
    // Setup LD_LIBRARY_PATH so that Jaxer runs.
    if (const char *p = getenv("LD_LIBRARY_PATH")) {
        char *q = new char[strlen(p) + 20];
        strcpy(q, "LD_LIBRARY_PATH=.:");
        strcat(q, p);
        putenv(q);
    } else {
        putenv("LD_LIBRARY_PATH=.");
    }
    // Setup MOZILLA_FIVE_HOME for Jaxer
    char* cwd = get_current_dir_name();
    setenv("MOZILLA_FIVE_HOME", cwd, true);
    free(cwd);
#endif

    lsi.SetLogFileName(g_LogSettings["output"].c_str());
    //Create LogServer
    //logme("calling CreateLogServer");
    if(!CreateLogServer())
    {
        SysLog(LOG_CRIT, "Failed to create log server: %m");
        exit(1);
    }
    //logme("CreateLogServer done");

    char logpipe[256];
    sprintf(logpipe, "JAXERLOG_PIPENAME=%s",lsi.GetFifoName() );
    putenv(logpipe);

    if (!lc.OpenPipeForWriteWait(2))
    {
        SysLog(LOG_CRIT, "Failed to communicate with logger: %m");
        exit(1);
    }
    lc.SetVerboseOnError();

    Log(eNOTICE, "JaxerManager " JAXER_BUILD_ID " (web %d, command %d)", g_ConnectionSettings[eWEB_JAXER_PROTOCOL].mPort, g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort);

    Log(eTRACE, "event_init");
    // Initalize the event library
    event_init();

    event_set_log_callback (EventLogCB);

    Log(eTRACE, "after event_init");
    // Create an event for period job
    struct event timeout;
    struct timeval tv;
    evtimer_set(&timeout, TimedEventCallback, &timeout);
    evutil_timerclear(&tv);
    tv.tv_sec = 1;
    event_add(&timeout, &tv);

    // Start listeners for new connections.
    //const char *adminserver = g_AdminInterface;
	const char *adminserver = g_GeneralSettings[eNAME_ADMIN_JAXER_INTERFACE].msValue;
	//iter I = g_ManagerSettings.find(NAME_ADMIN_JAXER_INTERFACE);
	//if (I != g_ManagerSettings.end())
	//	adminserver = I->second.c_str();

    //g_administratorListener.Bind(adminserver, g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort, NewAdministrator);
    #if 0
    if (g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mEnabled)
    {
	    Log(eDEBUG, "Bind admin listener: interface=%s port=%d", g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mInterface,
            , g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort);
        g_administratorListener.Bind(&g_ConnectionSettings[eADMIN_JAXER_PROTOCOL], NewAdministrator);
    }else
    {
	    Log(eDEBUG, "admin listener disabled: interface=%s port=%d", g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mInterface,
            , g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort);
    }
    #endif

	//const char *webserver = g_GeneralSettings[eNAME_WEB_JAXER_INTERFACE].msValue; //g_WebInterface;
	//I = g_ManagerSettings.find(NAME_WEB_JAXER_INTERFACE);
	//if (I != g_ManagerSettings.end())
	//	webserver = I->second.c_str();

    for (int k=0; k<g_nConnectionSettings; k++)
    {
        if (g_ConnectionSettings[k].mEnabled)
        {
            if (g_ConnectionSettings[k].mType != eADMIN_JAXER_PROTOCOL)
            {
                Log(eDEBUG, "Bind listener interface=%s port=%d", g_ConnectionSettings[k].mInterface, g_ConnectionSettings[k].mPort);
                g_connectionListener[k].Bind(&g_ConnectionSettings[k], NewConnection);
            }else
            {
                Log(eDEBUG, "Bind admin interface=%s port=%d", g_ConnectionSettings[k].mInterface, g_ConnectionSettings[k].mPort);
                g_connectionListener[k].Bind(&g_ConnectionSettings[k], NewAdministrator);
            }
        }else
        {
            Log(eDEBUG, "Disabled listener: interface=%s port=%d", g_ConnectionSettings[k].mInterface, g_ConnectionSettings[k].mPort);
        }
    //g_connectionListener.Bind(webserver, g_ConnectionSettings[eWEB_JAXER_PROTOCOL].mPort, NewConnection);
    }

    unsigned g_MinServers = g_GeneralSettings[eNAME_MIN_JAXERS].miValue;
    Log(eTRACE, "create server min=%d", g_MinServers);
    // Initiate server creation.
    if (g_MinServers > 0)
    {
        Log(eTRACE, "about to Create New process");
        CreateNewProcess();
    }
	
    Log(eTRACE, "event_dispatch");
    event_dispatch();

    lsi.Terminate();
    return 0;
}

static void SocketEventCallback(int socket, short event, void* vp) 
{
    bool ret = true;
    Log(eTRACE, "SocketEventCallback: s=%d e=%d vp=%d", socket, event, vp);
    Socket *s = (Socket*) vp;
    if (event & EV_READ)
    {
      Log(eTRACE, "    CALL OnRecv");
      ret = s->OnRecv();
    }
    if (!ret)
    {
        Log(eDEBUG, "SocketEventCallback: OnRecv failed. s=%d e=%d vp=%d", socket, event, vp);
    }
    if (ret && event & EV_WRITE)
    {
      Log(eTRACE, "    CALL OnSend");
      ret = s->OnSend();
      if (!ret)
      {
          Log(eDEBUG, "SocketEventCallback: OnSend failed. s=%d e=%d vp=%d", socket, event, vp);
      }
    }
    if (!ret)
    {
      event_del(&s->m_ev);
      Server *ps = dynamic_cast<Server*>(s);
      if (ps)
      {
        if (!ps->m_exiting)
            Log(eINFO, "SocketEventCallback: Killing Jaxer due to failed event handling.");
		else
			Log(eDEBUG, "SocketEventCallback: Jaxer terminated as instructed.  Cleaning up.");

        KillServer(ps);
      }else
      {
        Log(eDEBUG, "SocketEventCallback: Deleting socket object due to failed event handling.");
        delete s;
      }
    }
}

static void OnConnectionError(Connection* c)
{
    event_del(&c->m_ev);
    delete c;
}

static void TimedEventCallback(int socket, short evt, void* vp)
{
        event *ev = (event*)vp;

        // Figure out how long we should wait for the next event to fire
        timeval start_time;
        gettimeofday(&start_time, NULL);

        DoPeriodicLinux();

        // To avoid processing events on deleted objects above, we deferred
        // deletion until now.
        while (g_toDelete) {
            Server *s = g_toDelete;
            g_toDelete = s->m_next;

            // Also kill the connection as we don't know what state it's in.
            if (s->m_boundConnection)
                delete s->m_boundConnection;

            Log(eTRACE, "Delete server: pid=%p", s->m_pid);

            delete s;
        }

        // If the number of servers is too low, initiate process creation.
        // Note that a server cannot die without this loop cycling as a
        // side-effect from the closure of the associated socket.
        unsigned g_MinServers = g_GeneralSettings[eNAME_MIN_JAXERS].miValue;
        if (g_serverCount < g_MinServers)
        {
            Log(eTRACE,"About to call CreateNewProcess due to low server count");
            CreateNewProcess();
        }

        // Go clean up zombie processes.
        pid_t cpid;
        while ((cpid =waitpid(-1, NULL, WNOHANG)) > 0)
        {
            if(cpid == lsi.GetPid() && !lsi.IsExiting())
            {
                Log(eERROR, "Jaxer Logger terminated. Restarting it");
                lsi.SetPid(-1);
                //close(g_sockLogger);
                //g_sockLogger = -1;
                CreateLogServer();
            }
        }

        struct timeval tv;
        evutil_timerclear(&tv);

        timeval end_time;

        gettimeofday(&end_time, NULL);
        if (end_time.tv_sec == start_time.tv_sec)
        {
            tv.tv_usec = 1000000 - (end_time.tv_usec - start_time.tv_usec);
        }else if (end_time.tv_sec == start_time.tv_sec+1 && end_time.tv_usec < start_time.tv_usec)
        {
            tv.tv_usec = start_time.tv_usec - end_time.tv_usec;
        }else
        {
          //more than a second has elapsed
          tv.tv_usec = 1;
        }

        event_add(ev, &tv);
}

void EventLogCB(int severity, const char* msg)
{
    eLogLevel eLevel = eINFO;
    switch (severity)
    {
        case _EVENT_LOG_DEBUG:
            eLevel = eDEBUG;
        break;
        case _EVENT_LOG_MSG:
            eLevel = eNOTICE;
        break;
        case _EVENT_LOG_WARN:
            eLevel = eWARN;
        break;
        case _EVENT_LOG_ERR:
            eLevel = eERROR;
        break;
    }
    Log (eLevel, "libevent:%s", msg);
}

// Include implementation common to Windows and Unix.
#include "impl.h"


void SysLog(int level, const char* fmt, ...)
{
  static int logOpened = false;
  if (! logOpened)
  {
    openlog("jaxermanager", LOG_CONS | LOG_PID, LOG_USER);
    logOpened = true;
  }

  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  vsyslog(level, fmt, arg_ptr);
  va_end(arg_ptr);
}

