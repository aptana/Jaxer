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

/* ***** BEGIN ACKNOWLEDGMENT BLOCK *****
 * 
 * Aptana is grateful to Steve Walker for contributing portions of
 * this program and assigning them to Aptana, Inc.
 * 
 * ***** END ACKNOWLEDGMENT BLOCK ***** */

#define _WIN32_WINNT  0x0500
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <tchar.h>
#include <strsafe.h>
#include "jaxerBuildId.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma warning( disable : 4995)

#include "logclient.h"
#include "JaxerManagerMsg.h"

// Include declarations common to Windows and Unix.
#include "decl.h"

static char* serviceName = "Jaxer Manager";
static char* displayName = "Jaxer Manager";

#define SERVICE_REGISTRY_LOCATION ("SYSTEM\\CurrentControlSet\\Services\\")
#define PARAMS_LOCATION        ("Parameters")
static char szErr[1024] = "";
static char sJaxerImageDir[0x2000];

static LogClient lc;
static LogServerInfo lsi;

#ifdef ENABLE_CRASHREPORTER
#include "CrashReport.h"
#endif

// Handles to the threads in this process.

// listeners: WEB-JAXER-PROTOCOL, WEB-HTTP-PROTOCOL, ADMIN-JAXER-PROTOCOL, ADMIN-HTTP-PROTOCL
HANDLE g_ListenerThreads[4];

// listening for cmds over the admin port (cmd protocol)
//HANDLE g_AdminThread;

// thread processing ALL web/HTTP requests
HANDLE g_ProcessThread;

HANDLE g_MainThread;


// Forward declarations of asynchronous procedure calls.
VOID CALLBACK apc_NewAdministrator(ULONG_PTR);
VOID CALLBACK apc_NewConnection(ULONG_PTR);
VOID CALLBACK apc_NewWebHTTPConnection(ULONG_PTR);
VOID CALLBACK apc_NewAdminHTTPConnection(ULONG_PTR);
VOID CALLBACK apc_CreateProcess(ULONG_PTR);
VOID CALLBACK apc_ProcessCreated(ULONG_PTR);
VOID CALLBACK apc_RetireAdministrator(ULONG_PTR);
VOID CALLBACK apc_DeleteConnection(ULONG_PTR);
VOID CALLBACK apc_DeleteServer(ULONG_PTR);
VOID CALLBACK apc_AdminRecvComplete(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
VOID CALLBACK apc_AdminSendComplete(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
VOID CALLBACK apc_SocketPeekComplete(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
VOID CALLBACK apc_PipeReadComplete(DWORD, DWORD, LPOVERLAPPED);
VOID CALLBACK apc_PushMessageComplete(DWORD, DWORD, LPOVERLAPPED);
VOID CALLBACK apc_ServerWakeupComplete(DWORD, DWORD, LPOVERLAPPED);
VOID CALLBACK apc_Timer(LPVOID, DWORD, DWORD);

static void KillServer(Server *s);
// How long Administrators should be deleted after they are no longer used.
int g_IdleAdminWaitTime = 180;

class EventLogHelper
{
    HANDLE m_hEventLog;
    BOOL m_bAddedToRegistry;
public:
    EventLogHelper()
        :m_hEventLog(NULL)
        ,m_bAddedToRegistry(FALSE)
    {
        const char *sourceName="JaxerManager";
        // Get a handle to the event log.
        m_hEventLog = RegisterEventSource(NULL,  // Use local computer. 
                          sourceName);           // Event source name.
        if (m_hEventLog && !m_bAddedToRegistry)
        {
            AddToRegistry();
            m_bAddedToRegistry = TRUE;
            m_hEventLog = RegisterEventSource(NULL, sourceName);
        }
#if 0
        if (m_hEventLog == ERROR_INVALID_HANDLE)
        {
            // sourceName is Security event log
            m_hEventLog = NULL; 
        }
#endif
    };
 
    ~EventLogHelper()
    {
        if (m_hEventLog != NULL)
            DeregisterEventSource(m_hEventLog);
    }

    void LogError(int id)
    {
        if (m_hEventLog == NULL) return;

        // Simple logging, does not take any args for now.
        DWORD dwEventID = id;
        WORD cInserts = 0;
        LPCSTR szMsg = NULL;

        // Report the event.  do not check for return; there is
        // (almost) nothing we can do.
        ReportEvent(m_hEventLog,  // Event log handle. 
            EVENTLOG_ERROR_TYPE,  // Event type. 
            NULL,                 // Event category.  
            dwEventID,            // Event identifier. 
            NULL,                 // No user security identifier. 
            cInserts,             // Number of substitution strings. 
            0,                    // No data. 
            &szMsg,               // Pointer to strings. 
            NULL);
    };

    void Log(int EventType, int id, TCHAR* arg1)
    {
        if (m_hEventLog == NULL) return;

        // Simple logging, does not take any args for now.
        DWORD dwEventID = id;
        WORD cInserts = 1;
        LPCSTR szMsg = arg1;

        // Report the event.  do not check for return; there is
        // (almost) nothing we can do.
        ReportEvent(m_hEventLog,  // Event log handle. 
            EventType,            // Event type. 
            NULL,                 // Event category.  
            dwEventID,            // Event identifier. 
            NULL,                 // No user security identifier. 
            cInserts,             // Number of substitution strings. 
            0,                    // No data. 
            &szMsg,               // Pointer to strings. 
            NULL);
    };

private:
    BOOL AddToRegistry()
    {
        // Name of the event log.
        char *logName = "Application";
        // Event Source name.
        char *sourceName = "JaxerManager";

        TCHAR exeName[MAX_PATH];
        DWORD ret = GetModuleFileName(NULL, exeName, MAX_PATH);
        ret;

        // This number of categories for the event source.
        DWORD dwCategoryNum = 1;
   
       HKEY hk; 
       DWORD dwData, dwDisp; 
       TCHAR szBuf[MAX_PATH]; 
       size_t cchSize = MAX_PATH;
       
       // Create the event source as a subkey of the log. 
       HRESULT hr = StringCchPrintf(szBuf, cchSize, 
          "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
          logName, sourceName); 
     
       if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szBuf, 
              0, NULL, REG_OPTION_NON_VOLATILE,
              KEY_WRITE, NULL, &hk, &dwDisp))
       {
              return FALSE;
       }

 
       // Set the name of the message file. 
       if (RegSetValueEx(hk,             // subkey handle 
              "EventMessageFile",        // value name 
              0,                         // must be zero 
              REG_EXPAND_SZ,             // value type 
              (LPBYTE) exeName,          // pointer to value data 
              (DWORD) (lstrlen(exeName)+1)*sizeof(TCHAR))) // data size
       {
          RegCloseKey(hk); 
          return FALSE;
       }
 
       // Set the supported event types. 
       dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | 
            EVENTLOG_INFORMATION_TYPE; 
     
       if (RegSetValueEx(hk,      // subkey handle 
               "TypesSupported",  // value name 
               0,                 // must be zero 
               REG_DWORD,         // value type 
               (LPBYTE) &dwData,  // pointer to value data 
               sizeof(DWORD)))    // length of value data 
       {
          RegCloseKey(hk); 
          return FALSE;
       }
 
       // Set the category message file and number of categories.
       if (RegSetValueEx(hk,              // subkey handle 
               "CategoryMessageFile",     // value name 
               0,                         // must be zero 
               REG_EXPAND_SZ,             // value type 
               (LPBYTE) exeName,          // pointer to value data 
               (DWORD) (lstrlen(exeName)+1)*sizeof(TCHAR))) // data size
       {
          RegCloseKey(hk); 
          return FALSE;
       }
 
       if (RegSetValueEx(hk,            // subkey handle 
               "CategoryCount",         // value name 
               0,                       // must be zero 
               REG_DWORD,               // value type 
               (LPBYTE) &dwCategoryNum, // pointer to value data 
               sizeof(DWORD)))          // length of value data 
       {
          RegCloseKey(hk); 
          return FALSE;
       }

       RegCloseKey(hk);

       return TRUE;

    };
};

static EventLogHelper g_elh;

/***********************************************************************/
struct Administrator;

template<class T>
struct ObjectList
{
    T              *m_obj;
    time_t         m_time;
    ObjectList<T>* m_next;

    ObjectList<T>(T* t)
    {
        m_obj = t;
        time(&m_time);
        m_next = 0;
    }

    ~ObjectList<T>()
    {
        delete m_obj;
        m_obj = 0;
    }
};

ObjectList<Administrator> *g_IdleAdmin = 0;

template<class T>
struct Buffer
{
    OVERLAPPED      m_overlapped;   // must be first
    T              *m_owner;
    char            m_data[BUFSIZE];

    Buffer() : m_owner(0)
    {
        memset(&m_overlapped, 0, sizeof(m_overlapped));
    }
};

struct Connection
{
    SOCKET          m_socket;
    Server         *m_boundServer;
    unsigned        m_requestCount;
    bool            m_recving;
    bool            m_pending; // This may be removed.
    Connection     *m_next;
    Buffer<Connection> m_from;
	Buffer<Connection> m_to;
	int             m_waitTime; // For debugging, time on waiting list
	enum eMSG_TYPE  m_reqType;

    Connection(SOCKET s) : m_socket(s), m_boundServer(0), m_requestCount(0),
                           m_recving(false), m_pending(false), m_next(0), m_from(),
						   m_to(), m_waitTime(0), m_reqType(eINVALID_MSG)
    {
        m_from.m_owner = this;
		m_to.m_owner = this;
    };
};

struct Administrator
{
    OVERLAPPED      m_overlapped;   // must be first
    SOCKET          m_socket;
    char            m_line[512];
    char           *m_lineEnd;
    int             m_in;
    int             m_out;
	bool            m_isHTTP;
    AdminBufferList *m_toHead;
    AdminBufferList *m_toTail;
    Buffer<Administrator> m_from;
    char           *m_outputBuf;
    int             m_outputLen;

    Administrator(SOCKET s, char* outputBuf = 0, bool isHTTP = false) : m_socket(s), m_in(0), m_out(0), m_toHead(0),
                              m_toTail(0), m_from(), m_outputBuf(outputBuf), m_outputLen(0), m_isHTTP(isHTTP)
    {
        m_lineEnd = m_line;
        m_toHead = new AdminBufferList();
        m_toTail = m_toHead;
        m_from.m_owner = this;
    }

    ~Administrator()
    {
        while (m_toHead)
        {
            AdminBufferList *abl = m_toHead;
            m_toHead = abl->m_next;
            delete abl;
        }
        closesocket(m_socket);
    }

	void Write(const char *p, int len = -1);
	void BeginSendNullSocket();
	void Flush();

    void BeginSend()
    {
        Log(eTRACE, "Enter Administrator::BeginSend");
        // Special case an Administrator that represents the console.
        if (m_socket == INVALID_SOCKET)
        {
			BeginSendNullSocket();
			return;
#if 0
		    if (m_outputBuf)
            {
                // Send output to m_outputBuf
                strncpy(m_outputBuf+m_outputLen, m_toHead->m_data, m_toHead->m_in);
                m_outputLen += m_toHead->m_in;
                m_outputBuf[m_outputLen] = 0;
                Log(eTRACE, "%s", m_outputBuf + (m_outputLen - m_toHead->m_in));
            }else
            {
			    char *data = (char*) malloc(m_toHead->m_in + 10);
    			
			    if(data == NULL)
			    {
				    Log(eERROR, "Could not allocate memory for logging, sending to stdout");
				    fwrite(m_toHead->m_data, 1, m_toHead->m_in, stdout);
				    fflush(stdout);
			    }
			    else
			    {
				    strncpy(data, m_toHead->m_data, m_toHead->m_in);
				    data[m_toHead->m_in] = '\0';
				    Log(eTRACE, "%s", data);
                    fwrite(data, 1, m_toHead->m_in, stdout);
				    free(data);
			    }
            }

            AdminBufferList *abl = m_toHead;
            m_toHead = abl->m_next;
            if (abl == m_toTail)
            {
                m_toTail = m_toHead;
            }
            delete abl;
            return;
#endif
        }

		WSABUF buffers[1];
        buffers[0].buf = m_toHead->m_data;
        buffers[0].len = m_toHead->m_in;
        DWORD nSent;

        Log(eTRACE, " Begin WSASend: datalen=%d", buffers[0].len);
        
        int rc = WSASend(m_socket, buffers, 1, &nSent, 0, &m_overlapped,
                apc_AdminSendComplete);

        int err = WSAGetLastError();
        if (rc == 0 || err == WSA_IO_PENDING)
        {
            Log(eTRACE, ( rc == 0)? "  WSASend returned OK" : "WSASend IO Pending");
        }else
        {
            Log(eWARN, err, "WSASend returned %d, err=%d", rc, err);
        }
    }

    void BeginRecv()
    {
		DWORD err;
        Log(eTRACE, "Enter Administrator::BeginRecv");

        if (m_socket == INVALID_SOCKET)
        {
            Log(eTRACE, "  Socke is invalid");
            return;
        }
		char abuf[10];
        WSABUF buffers[1];
        buffers[0].len = 3;
        buffers[0].buf = abuf;

        DWORD dwRecv, dwFlags = MSG_PEEK;
        int rc = WSARecv(m_socket, buffers, 1, &dwRecv, &dwFlags, 0, 0);
        
        if ( rc != 0)
        {
			DWORD err = WSAGetLastError();
            Log(eWARN, err, " WSARecv returns rc=%d err=%d",  rc, err);
			//TODO -- close connection
			return;
        }

#if 0
		//Check to see if this is a webrequest
		if (IsHTTPRequest(abuf, dwRecv))
		{
			Connection *c = new Connection(m_socket);
			c->m_recving = true;
			c->m_reqType = eNEW_ADMIN_REQUEST_MSG;
			StartNewRequest(c);

			// Retire this
			m_socket = INVALID_SOCKET;
			QueueUserAPC(apc_RetireAdministrator, g_MainThread, ULONG_PTR(this));
		}else
#endif
		{
			//Write the header back
			//Write("JaxerManager " JAXER_BUILD_ID "\r\n\r\n");
			//Flush();
    
			//WSABUF buffers[1];
			buffers[0].len = BUFSIZE;
			buffers[0].buf = m_from.m_data;
			dwFlags = 0;
			rc = WSARecv(m_socket, buffers, 1, &dwRecv, &dwFlags,
							 &m_from.m_overlapped, apc_AdminRecvComplete);
			err = WSAGetLastError();

			if ( rc == 0 || err == WSA_IO_PENDING)
			{
				Log(eTRACE, ( rc == 0)? "  WSARecv returned OK" : "WSARecv IO Pending");
			}else
			{
				Log(eWARN, err, " WSARecv returns rc=%d err=%d",  rc, err);
			}
		}
    }
};

struct Server
{
    HANDLE          m_process;
    DWORD           m_pid;
    HANDLE          m_pipe;
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
    Buffer<Server>  m_to;
    Buffer<Server>  m_from;
    int             m_respId;
    bool            m_hasMoreResp;
    ServerSettings  m_privateSettings;
	bool            m_markedForDebug;
	char            m_jaxerID[32];

    Server(HANDLE process, DWORD pid, HANDLE pipe) :
        m_process(process), m_pid(pid), m_pipe(pipe),
        m_boundConnection(0), m_next(g_servers), m_nextIdle(0), m_nextDebug(0),
		m_lastSent(0), m_secondsToLive(-1), 
        m_deletePending(false), m_exiting(false),
        m_nRequests(0), m_secsSinceRequestStart(0), m_to(), m_from(),
        m_respId(0), m_hasMoreResp(false),
        m_idleTime(0),
		m_markedForDebug(false)
    {
        m_to.m_owner = this;
        m_from.m_owner = this;
        g_servers = this;
		sprintf(m_jaxerID, "%d-%d", pid, (int)time(NULL));
		m_privateSettings.m_owner = this;
    };

	bool ProcessNeedRespCmdHelper (char* cmd, char* resp);
    bool ProcessNeedRespCmd ()
    {
		bool ret =ProcessNeedRespCmdHelper (m_from.m_data, m_to.m_data);
		if (! ret) return ret;
#if 0
        BOOL ret = TRUE;
        DWORD respLen = 0;
        //DWORD nWritten = 0;
        char buf[128];

        char *cmd = m_from.m_data;
        char *respMsg = m_to.m_data;
        DWORD dwLen = cmd[1] << 8 | cmd[2];
        
        
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
                fprintf(stderr, "opt=%s\n", opt);
                int errCode = 0x7FFF;
                if (opt && *opt)
                    errCode = atoi(opt);
                sprintf(buf, "TEST failed CMD response, Error code should be %d", errCode);
                BuildErrorResponse(respMsg, errCode, buf);
            }else
            {
                int num = 1;
                if (opt && *opt)
                     num = atoi(opt);
                if (num < 1) num = 1;
                m_hasMoreResp = (m_respId < num);
                sprintf(buf, "TEST CMD RESPONSE #%d.  TOTAL RESPONSES SHOULD BE %d", m_respId, num);
                BuildCmdResponse(respMsg, buf, (int)strlen(buf), m_hasMoreResp==TRUE);
            }
        }else if (strcmp("get", opt) == 0)
        {
            Administrator a(INVALID_SOCKET, respMsg+5);
            DoGet(&a, cmd);
            //a.Flush();
            respMsg[0] = eCMD_RESP_MSG;
            respLen = a.m_outputLen + 2;
            respMsg[1] = unsigned((respLen & 0XFF00) >> 8);
            respMsg[2] = unsigned(respLen & 0XFF);
            respMsg[3] = 1; // OK
            respMsg[4] = 0; // no more
        }else if (strcmp("set", opt) == 0)
        {
            Administrator a(INVALID_SOCKET, respMsg+5);
            DoSet(&a, cmd, false);
            a.Flush();
            respMsg[0] = eCMD_RESP_MSG;
            respLen = a.m_outputLen + 2;
            respMsg[1] = unsigned((respLen & 0XFF00) >> 8);
            respMsg[2] = unsigned(respLen & 0XFF);
            respMsg[3] = 1; // OK
            respMsg[4] = 0; // no more
        }else if (strcmp("getme", opt) == 0)
        {
            Administrator a(INVALID_SOCKET, respMsg+5);
            m_privateSettings.DoGet(&a, cmd);
            a.Flush();
            respMsg[0] = eCMD_RESP_MSG;
            respLen = a.m_outputLen + 2;
            respMsg[1] = unsigned((respLen & 0XFF00) >> 8);
            respMsg[2] = unsigned(respLen & 0XFF);
            respMsg[3] = 1; // OK
            respMsg[4] = 0; // no more
        }else if (strcmp("setme", opt) == 0)
        {
            Administrator a(INVALID_SOCKET, respMsg+5);
            m_privateSettings.DoSet(&a, cmd);
            a.Flush();
            respMsg[0] = eCMD_RESP_MSG;
            respLen = a.m_outputLen + 2;
            respMsg[1] = unsigned((respLen & 0XFF00) >> 8);
            respMsg[2] = unsigned(respLen & 0XFF);
            respMsg[3] = 1; // OK
            respMsg[4] = 0; // no more
        }else
        {
            Log(eINFO, "NOT_IMPL: Executing NeedRespCmd %s %s from server %d", opt, cmd, m_pid);
            sprintf(buf, "CMD NOT IMPLEMENTED.");
            BuildCmdResponse(respMsg, buf, (int)strlen(buf), 0);
        }

        ret = SendCmdResponse();
        if (!ret)
        {
            Log(eWARN, "Sending cmd response to server (pid=%d) failed.", m_pid);
            return ret;
        }
#endif

        if (m_hasMoreResp)
            return ProcessNeedRespCmd();

        return true;     
    }

private:
    bool SendCmdResponse()
    {
        unsigned char *respMsg = (unsigned char *)m_to.m_data;
        DWORD respLen = respMsg[1] << 8 | respMsg[2];
        respLen += 3;
        DWORD nWritten = 0;

        BOOL ret = WriteFile(m_pipe, respMsg, respLen, &nWritten, NULL);
        if (!ret || nWritten != respLen)
        {
            // write failed.  Need to kill server
            Log(eINFO, "WriteFile to server %d failed.", m_pid);
            return false;
        }
        return true;
    }
};

/***********************************************************************/

// Winsock does not offer a form of asynchronous accept that is compatible with
// APCs, so do them synchronously in a different thread.  I/O completion ports
// cannot be used as we hand socket handles over to another process; even if
// an I/O is initiated in another process, the completion is queue back here!
// (Microsoft does not make sockets first-class handles.)

//static void AcceptConnections(const char* server, unsigned port, PAPCFUNC apc)
static void AcceptConnections(const ConnectionSetting *cs, PAPCFUNC apc)
{
	const char* server = cs->mInterface;
	unsigned port = cs->mPort;
    Log(eTRACE,"Enter AcceptConnections: server=%s port=%d", server, port);
	
    // Create the listening socket.
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        Log(eERROR, WSAGetLastError(), "Cannot create socket, JaxerManager might already be running.");
        g_elh.LogError(MSG_CANNOT_CREATE_SOCKET);
        exit(1);
    }

	if (inet_addr(server) == INADDR_NONE)
	{
		Log(eWARN, "Cannot determine server (%s) address. Using 127.0.0.1", server);
		server= "127.0.0.1";
	}

    // Bind it to the desired port.
    sockaddr_in service;
	memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(server); //INADDR_ANY;
    service.sin_port = htons(port);

    int rc = bind(s, (SOCKADDR*) &service, sizeof(service));
    if (rc == SOCKET_ERROR) {
		Log(eERROR, WSAGetLastError(), "Cannot bind port %d, JaxerManager might already be running.", port);
        g_elh.LogError(MSG_CANNOT_BIND_PORT);
        exit(1);
    }

    // Start listening for connections.
    rc = listen(s, SOMAXCONN);
    if (rc == SOCKET_ERROR) {
        Log(eERROR, WSAGetLastError(), "Cannot listen on port %d, JaxerManager might already be running.", port);
        g_elh.LogError(MSG_CANNOT_LISTEN_PORT);
        exit(1);
    }

    // Accept incoming connections.
	while (true) {
		SOCKET t = accept(s, 0, 0);
		if (t == INVALID_SOCKET) {
            Log(eERROR, WSAGetLastError(),
                "Cannot accept socket, make sure another JaxerManager is not already running.");
            g_elh.LogError(MSG_CANNOT_ACCEPT_SOCKET);
            exit(1);
        }

		ConnectionWithSetting *cws = new ConnectionWithSetting(cs, t);
		if (!cws)
		{
			Log(eWARN, "Failed to allocate memory for new connection");
			closesocket(t);
			continue;
		}
        // Go inform main thread of new connection.
        //QueueUserAPC(apc, g_MainThread, ULONG_PTR(t));
		QueueUserAPC(apc, g_MainThread, ULONG_PTR(cws));
    }

    return;
}

static unsigned __stdcall AdminThread(void *cs)
{
	AcceptConnections((const ConnectionSetting*) cs, apc_NewAdministrator);
    return 0;
}

static unsigned __stdcall ListenThread(void *cs)
{
	AcceptConnections((const ConnectionSetting*) cs, apc_NewConnection);
    return 0;
}

/***********************************************************************/

// Process creation is expensive, so do it on a helper thread where it won't
// block other activity.

static unsigned __stdcall ProcessThread(void *)
{
    // Work is all done by APCs.
    while (true)
        SleepEx(100000, TRUE);

    return 0;
}

static void CreateNewProcess()
{
    if (ProcessCreationAllowed()) {
        g_processCreationPending = true;
        QueueUserAPC(apc_CreateProcess, g_ProcessThread, 0);
    }
}

VOID CALLBACK apc_CreateProcess(ULONG_PTR unused)
{
retry:
    BOOL rc;

    // Use a unique pipe name, so that multiple managers can unambiguously
    // connect with their pool of servers.  The name is passed to the servers
    // via an environment variable.
    static char s_pipename[256];
    if (!s_pipename[0]) {
        sprintf(s_pipename, "\\\\.\\pipe\\aptana\\channel-%d-%d",
                getpid(), GetTickCount());
        char buf[512];
        strcpy(buf, "JAXER_PIPENAME=");
        strcat(buf, s_pipename);
        putenv(buf);
    }

    // Begin by creating a new instance of a named pipe.
    HANDLE hServer = CreateNamedPipe(s_pipename,
                                     PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                                     PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
                                     PIPE_UNLIMITED_INSTANCES,
                                     0x4000, 0x4000,
                                     3600 * 1000,
                                     NULL);
    if (hServer == INVALID_HANDLE_VALUE) {
        Log(eERROR, GetLastError(), "Unable to create a named pipe");
        g_elh.LogError(MSG_CANNOT_CREATE_PIPE);
        exit(1);
    }

    // Prime the pipe for the eventual client connection.
    OVERLAPPED ovlp;
    memset(&ovlp, 0, sizeof(ovlp));
    ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    rc = ConnectNamedPipe(hServer, &ovlp);
    if (!rc && GetLastError() != ERROR_IO_PENDING) {
        Log(eERROR, GetLastError(), "Unable to connect the named pipe");
        g_elh.LogError(MSG_CANNOT_CONNECT_PIPE);
        exit(1);
    }

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;

    // cmdline
    std::string sCmdLine = "Jaxer.exe";
    for (iter I = g_Settings.begin(); I != g_Settings.end(); ++I)
    {
		//if (!g_ConnectionSettings[eWEB_HTTP_PROTOCOL].mEnabled && stricmp(I->first.c_str(), "-iwsconfigfile") == 0)
		//	continue;

        sCmdLine.push_back(' ');
        sCmdLine.append(I->first);
        sCmdLine.append(" \"");
        sCmdLine.append(I->second);
        sCmdLine.push_back('"');
    }

    while (true) {
        Log(eDEBUG, "Jaxer cmdline: %s", sCmdLine.c_str());
        Log(eDEBUG, "Jaxer Image Dir: %s", sJaxerImageDir);
        rc = CreateProcess(NULL,
                           (char*) sCmdLine.c_str(),
                           NULL,  /* default process security descriptor */
                           NULL,  /* default thread security descriptor */
                           FALSE, /* don't inherit handles */
                           CREATE_NO_WINDOW | CREATE_BREAKAWAY_FROM_JOB,
                           NULL,  /* inherit environment */
                           sJaxerImageDir,  /* current directory */
                           &si,
                           &pi);
        if (rc)
            break;

        // Process creation failed.  Assume it's a transient condition and try
        // again in a few seconds.
        Log(eWARN, GetLastError(), "Process creation failed");
        Sleep(5000);
    }

    // Apply memory limit to process.
	unsigned g_MaxMemory = g_GeneralSettings[eNAME_JAXER_MAX_MEMORY_MB].miValue;
    if (g_MaxMemory > 0) 
	{
        HANDLE hJob = CreateJobObject(0, 0);
        
		if (hJob != 0) 
		{
            JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };

            jeli.BasicLimitInformation.LimitFlags =
                JOB_OBJECT_LIMIT_PROCESS_MEMORY;

            jeli.ProcessMemoryLimit = g_MaxMemory * 1048576;

            if (SetInformationJobObject(hJob, JobObjectExtendedLimitInformation,
                                        &jeli, sizeof(jeli))) 
			{
                if (AssignProcessToJobObject(hJob, pi.hProcess) == false)
				{
                    Log(eERROR, GetLastError(), "AssignProcessToJobObject failed, could not set memory limits.");				
				}
				else
				{
					//Log(eINFO, "SetInformationJobObject success, set memory limits.");
				}
            }
			else
			{
				Log(eERROR, GetLastError(), "SetInformationJobObject failed, could not set memory limits.");
			}
        }
			
        CloseHandle(hJob);
    }

    // Wait for new process to connect.
    if (WaitForSingleObject(ovlp.hEvent, 60000) == WAIT_TIMEOUT) {
        // Took too long...  give up.
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        CloseHandle(ovlp.hEvent);
        CloseHandle(hServer);
        goto retry;
    }

    // Don't need these anymore.
    CloseHandle(pi.hThread);
    CloseHandle(ovlp.hEvent);

    // Inform main thread that new process is available.
    Server *s = new Server(pi.hProcess, pi.dwProcessId, hServer);
    QueueUserAPC(apc_ProcessCreated, g_MainThread, ULONG_PTR(s));
}

/***********************************************************************/

static HANDLE StartThread(unsigned (__stdcall *start_address)(void *), void* arglist)
{
    uintptr_t h = _beginthreadex(0, 0x10000, start_address, arglist, 0, 0);
    if (h == 0) {
        Log(eERROR, GetLastError(), "Thread creation failed");
        g_elh.LogError(MSG_CANNOT_CREATE_THREAD);
        exit(1);
    }
    return reinterpret_cast<HANDLE>(h);
}

static void OnServerError(Server *s)
{
    // Error occurred during communications.  Let's try to end the server or connection
    KillServer(s);
}

static void OnConnectionError(Connection* c)
{
    if (c->m_socket != INVALID_SOCKET) {
            closesocket(c->m_socket);
            c->m_socket = INVALID_SOCKET;
            QueueUserAPC(apc_DeleteConnection, g_MainThread, ULONG_PTR(c));
        }
}

static void AssignServer(Server *s, Connection *c)
{
	Log(eTRACE,"Enter AssignServer: s=%d pid=%d c=%d DEBUG=%d",
		s, s->m_pid, c, s->m_markedForDebug);
    s->m_privateSettings.ResetOnNewRequest();

    s->m_boundConnection = c;
    s->m_secsSinceRequestStart = 0;
    c->m_boundServer = s;
    CountRequest(s);

    
    WSAPROTOCOL_INFO  WSAProtocolInfo;
    if(WSADuplicateSocket(c->m_socket, s->m_pid, &WSAProtocolInfo) == SOCKET_ERROR)
    {
        Log(eFATAL, WSAGetLastError(), "AssignServer: WSADuplicateSocket failed");
        //Kill server and connection
        OnServerError(s);
        return;
    }

    // Wake up server, informing it that it has a new request.
    s->m_to.m_data[0] =  c->m_reqType; //eNEW_REQUEST_MSG;
    DWORD msglen = sizeof(WSAPROTOCOL_INFO);

	s->m_to.m_data[1] = unsigned((msglen & 0XFF00) >> 8);
    s->m_to.m_data[2] = unsigned(msglen & 0XFF);

    memcpy(&(s->m_to.m_data[3]), &WSAProtocolInfo, msglen);
    
    c->m_requestCount++;
    if (!WriteFileEx(s->m_pipe, s->m_to.m_data, 3 + msglen,
                &s->m_to.m_overlapped, apc_ServerWakeupComplete))
    {
        Log(eWARN, GetLastError(), "AssignServer: WriteFileEx failed");
        //Kill server and connection
        OnServerError(s);
    }
}

VOID CALLBACK apc_ServerWakeupComplete(DWORD dwError, DWORD dwTransfer,
                                       LPOVERLAPPED lpOverlapped)
{
	Server *s = ((Buffer<Server> *)lpOverlapped)->m_owner;
        
	Log(eTRACE,"Enter apc_ServerWakeupComplete: s=%d pid=%d c=%d DEBUG=%d",
		s, s->m_pid, s->m_boundConnection, s->m_markedForDebug);
    if (dwError != 0)
    {
        Log(eWARN, dwError, "apc_ServerWakeupComplete: WriteFileEx failed");

        //Kill server and connection
        OnServerError(s);
    }
}

static void InitiateSocketPeek(Connection *c)
{
    Log(eTRACE,"Enter InitiateSocketPeek c=%d");
    if (c->m_boundServer == 0 && g_ManagerMode == mm_stopping) {
        closesocket(c->m_socket);
        c->m_socket = INVALID_SOCKET;
        QueueUserAPC(apc_DeleteConnection, g_MainThread, ULONG_PTR(c));
        return;
    }
    

    if (c->m_recving)
        return;

    WSABUF buffers[1];
    buffers[0].len = 3;
    buffers[0].buf = c->m_from.m_data;

    DWORD dwRecv, dwFlags = MSG_PEEK;
    int rc = WSARecv(c->m_socket, buffers, 1, &dwRecv, &dwFlags,
                     &c->m_from.m_overlapped, apc_SocketPeekComplete);
    c->m_recving = true;
    int err;
    if (rc == 0)
    {
        Log(eTRACE,"  WSARecv returned ok");
    }else if( (rc == SOCKET_ERROR) && (WSA_IO_PENDING != (err = WSAGetLastError())))
    {
        Log(eWARN, err, "  WSARecv failed: returned %d, err=%d", rc, err);
        //Kill connection
        OnConnectionError(c);
    }
}

static void PushMessage(Server *s)
{
    void *p = 0;
    unsigned len = 0;
    s->m_lastSent->GetBytes(s, p, len);
    if(g_eLogLevel == eTRACE)
    {
        BlockDumper dump;
        dump.LogHeader("PushMessage", len);
        dump.LogContents((char*)p, len);
    }
    if(!WriteFileEx(s->m_pipe, p, len, &s->m_to.m_overlapped,
                apc_PushMessageComplete))
    {
        Log(eWARN, GetLastError(), "PushMessage: WriteFileEx (nbytes=%d) failed", len);
        //Kill server and connection
        OnServerError(s);
    }
}

VOID CALLBACK apc_PushMessageComplete(DWORD dwError, DWORD dwTransfer,
                                      LPOVERLAPPED lpOverlapped)
{
    Server *s = ((Buffer<Server> *)lpOverlapped)->m_owner;

    // TODO: what if this fails?  Kill server and reassign to another?

    // If the message just pushed told the server to exit, then drop it on the
    // floor and wait for it to either die or time out.
    if (s->m_exiting)
        return;

    // Try to push another message.
    if (Message::Next(s)) {
        PushMessage(s);
        return;
    }

	if (s->m_markedForDebug)
	{
		Connection* oldC = s->m_boundConnection;
		if (oldC)
		{
			oldC->m_boundServer = 0;
			s->m_boundConnection = 0;
			oldC->m_recving = false;
			oldC->m_next = NULL;
		}
		Log(eTRACE,"apc_PushMessageComplete: debug server %d finished serving request %d.", s->m_pid, oldC);

		Connection* ac = g_debugConnections;
		Connection* parent = NULL;
		while(ac)
		{
			if (ac->m_boundServer == s)
			{
				if (parent)
					parent->m_next = ac->m_next;
				else
					g_debugConnections = ac->m_next;

				Log(eTRACE,"apc_PushMessageComplete: let debug server %d server request %d.", s->m_pid, ac);
				AssignServer(s, ac);
				break;
			}
			parent = ac;
			ac = ac->m_next;
		}
		if (!g_debugConnections || !g_debugConnections->m_next)
			g_debugConnectionsTail = g_debugConnections;

		if (oldC)
			InitiateSocketPeek(oldC);

	}else
	{
		// No more messages, so we're idle again.  Assign it to a connection if
		// any are waiting (unless paused).
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

static void KillServer(Server *s)
{
    if (!s->m_deletePending) {
        Connection *c = s->m_boundConnection;
        s->m_deletePending = true;
        CancelIo(s->m_pipe);

        // Also kill the connection as we don't know what state it's in.
        if (c && c->m_socket != INVALID_SOCKET) {
            closesocket(c->m_socket);
            c->m_socket = INVALID_SOCKET;
            QueueUserAPC(apc_DeleteConnection, g_MainThread, ULONG_PTR(c));
        }

        // Remove from records.
        RemoveServer(s);

        QueueUserAPC(apc_DeleteServer, g_MainThread, ULONG_PTR(s));
    }
}

static void SlaughterServer(Server *s, bool isExiting = false)
{
	char pid[10];
	sprintf(pid, "%d", s->m_pid);

	if(!TerminateProcess(s->m_process, 0))
	{
		//Log(eERROR, GetLastError(), "Could not stop server, using TaskKill"); 
	}

    // For now, let's just wait no matter what
    // intptr_t rv = _spawnl(isExiting ? _P_WAIT : _P_NOWAIT , "cmd", "TASKKILL", "/F", "/PID", pid, NULL);
    intptr_t rv = _spawnl(_P_WAIT, "cmd", "TASKKILL", "/F", "/PID", pid, NULL);
    if (isExiting && rv)
    {
        // failed
    }

	KillServer(s);
}

VOID CALLBACK apc_AdminRecvComplete(DWORD dwError, DWORD cbTransferred,
                                    LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
    Administrator *a = ((Buffer<Administrator> *)lpOverlapped)->m_owner;

    if (cbTransferred == 0) {
        Log(eTRACE,"apc_AdminRecvComplete returned nothing.  Deleting admin.");
        // closesocket(a->m_socket);
        QueueUserAPC(apc_RetireAdministrator, g_MainThread, ULONG_PTR(a));
    } else {
        if (g_eLogLevel == eTRACE)
        {
            BlockDumper dump;
            dump.LogHeader("apc_AdminRecvComplete", cbTransferred);
            dump.LogContents(a->m_from.m_data, cbTransferred);
        }
        a->m_in = cbTransferred;
        a->m_out = 0;

        while (a->m_out < a->m_in) {
            char c = a->m_from.m_data[a->m_out++];
            if (c == '\n')
                DoCommand(a);
            else if (c == '\r')
                ;
            else if (a->m_lineEnd < a->m_line + 511)
                *a->m_lineEnd++ = c;
        }

        a->BeginRecv();
    }
}

VOID CALLBACK apc_AdminSendComplete(DWORD dwError, DWORD cbTransferred,
                                    LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
    Log(eTRACE,"Enter apc_AdminSendComplete error=%d transfer=%d flag=%d", dwError, cbTransferred, dwFlags);

    Administrator *a = (Administrator *)lpOverlapped;

    AdminBufferList *abl = a->m_toHead;
    a->m_toHead = abl->m_next;
    if (abl == a->m_toTail)
    {
        a->m_toTail = a->m_toHead;
    }
    delete abl;

    if (a->m_toHead && a->m_toHead->m_flush)
        a->BeginSend();
}

VOID CALLBACK apc_SocketSendDebugComplete(DWORD dwError, DWORD cbTransferred,
                                     LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	Log(eTRACE,"Enter apc_SocketSendDebugComplete");
    Connection *c = ((Buffer<Connection> *)lpOverlapped)->m_owner;
	c->m_next = NULL;
    
    if (dwError != 0)
    {
        Log(eDEBUG, dwError, "  apc_SocketSendDebugComplete failed: request=%d, transferred=%d",
			c, cbTransferred);
        //Kill connection.
		c->m_boundServer = NULL;
        OnConnectionError(c);
    }else
	{
		if (c->m_boundServer)
		{
			if (! c->m_boundServer->m_boundConnection)
			{
				Log(eDEBUG, "  apc_SocketSendDebugComplete: Let jaxer %d server request %d",
					c->m_boundServer->m_pid, c);
				AssignServer(c->m_boundServer, c);
			}else
			{
				Log(eDEBUG, "  apc_SocketSendDebugComplete: Jaxer %d busy, debug request %d put on waiting list",
					c->m_boundServer->m_pid, c);
				c->m_waitTime = DEBUG_CONNECTION_TIMEOUT_SECS;
				if (g_debugConnections == NULL)
					g_debugConnections = g_debugConnectionsTail = c;
				else
				{
					g_debugConnectionsTail->m_next = c;
					g_debugConnectionsTail = c;
				}
			}
		}else
		{
			Log(eDEBUG, dwError, "  apc_SocketSendDebugComplete: closing connection because no debug jaxer found");
			OnConnectionError(c);
		}
	}
}

static void StartNewDebugRequest(Connection* c, char* jaxer_id)
{
	Log(eTRACE,"Enter StartNewDebugRequest");

    c->m_next = 0;
    c->m_pending = false;

    // This is a debug request.  The Jaxer has to be there and idle.
    if (g_ManagerMode == mm_paused)
	{
		Log(eERROR, "StartNewDebugRequest: Manager is in paused state.  Cannot process request.");
		OnConnectionError(c);
		return;
	}

	// prepare msg for connector
    char *buf = c->m_to.m_data;
	DWORD len = strlen("Accepted");
	buf[0] = bt_DebugRequest;
	buf[1] = 0;
	buf[2] = (char)len + 7;

	//Number of headers
	buf[3] = 0;
	buf[4] = 1;

	buf[5] = 0;
	buf[6] = (char) len;
	strcpy(buf+7, "Accepted");
	buf[7+len] = 0;
	buf[8+len] = 1;

	char accept = 0;
    Server *s = g_debugServers;
	Log(eTRACE, "StartNewDebugRequest: Look for the required jaxer id=%s for request %d",
		jaxer_id, c);
		
	while(s)
    {
		if (strcmp(s->m_jaxerID, jaxer_id) == 0)
		{
			//Found the server
			Log(eTRACE, "StartNewDebugRequest: Required jaxer id=%d found", s->m_pid);
			accept = 1;
			c->m_boundServer = s;
			c->m_next = NULL;
			// Is the server busy?
			if (s->m_boundConnection)
			{
				Log(eTRACE, "StartNewDebugRequest: Jaxer id=%d is busy, request %d will be put on waiting list",
					s->m_pid, c);
			}
			break;
		}
		s = s->m_next;
    }

	buf[9+len] = accept;
	
	if (accept==0)
	{
		Log(eWARN, "StartNewDebugRequest: Cannot find the required jaxer id=%s", jaxer_id);
	}

	WSABUF buffers[1];
    buffers[0].buf = buf;
    buffers[0].len = len+10;
    DWORD nSent;

	Log(eTRACE, " StartNewDebugRequest: Begin WSASend: datalen=%d", buffers[0].len);
    
    int rc = WSASend(c->m_socket, buffers, 1, &nSent, 0, &(c->m_to.m_overlapped),
                apc_SocketSendDebugComplete);

    int err = WSAGetLastError();
    if (rc == 0 || err == WSA_IO_PENDING)
    {
        Log(eTRACE, ( rc == 0)? "  WSASend returned OK" : "WSASend IO Pending");
    }else
    {
        Log(eWARN, err, "WSASend returned %d, err=%d", rc, err);
    }
}

VOID CALLBACK apc_SocketRecvDebugComplete(DWORD dwError, DWORD cbTransferred,
                                     LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	//TODO: socket read needs improvements.
	Log(eTRACE,"Enter apc_SocketRecvDebugComplete");
    Connection *c = ((Buffer<Connection> *)lpOverlapped)->m_owner;
    
    if (dwError != 0)
    {
        Log(eDEBUG, dwError, "  apc_SocketRecvDebugComplete failed: transferred=%d", cbTransferred);
        //Kill connection
        OnConnectionError(c);
    }else
	{
		//Parse data
		char* data = c->m_from.m_data;
		data++;
		DWORD tot_len = ((unsigned char)data[0])<<8 | (unsigned char)data[1];
		data += 2;
		if (cbTransferred != (tot_len+3))
		{
			Log(eDEBUG, dwError, "  apc_SocketRecvDebugComplete failed: transferred=%d expected=%d", cbTransferred, tot_len+3);
			OnConnectionError(c);
			return;
		}
		DWORD n = ((unsigned char) data[0])<<8 | (unsigned char)data[1];
		data += 2;
		tot_len -=2;
		for (int i=0; i<n; i++)
		{
			if (tot_len < 4)
			{
				Log(eDEBUG, dwError, "  apc_SocketRecvDebugComplete failed: processing field# %d of %d, remaining data length=%d",
					i+1, n, tot_len);
				OnConnectionError(c);
				return;
			}
			DWORD nlen, vlen;
			char *name, *value;
			nlen = ((unsigned char) data[0])<<8 | (unsigned char)data[1];
			data += 2;
			tot_len -=2;
			name = data;

			data += nlen;
			tot_len -= nlen;

			vlen = data[0]<<8 | data[1];
			data += 2;
			tot_len -=2;
			value = data;

			data += vlen;
			tot_len -= vlen;

			// check for the only name we are so far
			if (nlen == strlen("Jaxer-ID") && strncmp(name, "Jaxer-ID", nlen) == 0)
			{
				// This destories the next name/value.
				*data = 0;
				StartNewDebugRequest(c, value);
				return;
			}
		}
		Log(eDEBUG, dwError, "  apc_SocketRecvDebugComplete failed: cannot find the expected field");
		OnConnectionError(c);
	}
}

VOID CALLBACK apc_SocketPeekComplete(DWORD dwError, DWORD cbTransferred,
                                     LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
    Log(eTRACE,"Enter apc_SocketPeekComplete");
    Connection *c = ((Buffer<Connection> *)lpOverlapped)->m_owner;
    
    if (dwError != 0)
    {
        Log(eDEBUG, dwError, "  apc_SocketPeekComplete failed: transferred=%d", cbTransferred);
        //Kill connection
        OnConnectionError(c);
    }else
    {
		char* data = c->m_from.m_data;
		if(*data == bt_DebugRequest)
		{
#if 0
			if (c->m_reqType == eNEW_ADMIN_REQUEST_MSG)
			{
				Log(eDEBUG, "apc_SocketPeekComplete: Got debug request on admin request/connection");
				//Kill connection
				OnConnectionError(c);
				return;
			}
#endif
			WSABUF buffers[1];
			buffers[0].len = BUFSIZE;
			buffers[0].buf = c->m_from.m_data;

			DWORD dwRecv, dwFlags = 0;
			int rc = WSARecv(c->m_socket, buffers, 1, &dwRecv, &dwFlags,
							 &c->m_from.m_overlapped, apc_SocketRecvDebugComplete);
			DWORD err = WSAGetLastError();

			if ( rc == 0 || err == WSA_IO_PENDING)
			{
				Log(eTRACE, ( rc == 0)? "  WSARecv returned OK" : "WSARecv IO Pending");
			}else
			{
				Log(eWARN, err, " WSARecv returns rc=%d err=%d",  rc, err);
			}
		}else
		{
#if 0
			if (IsHTTPRequest(data, cbTransferred))
			{
				// WEB REQUEST
				if (!g_ConnectionSettings[eWEB_HTTP_PROTOCOL].mEnabled && c->m_reqType != eNEW_ADMIN_REQUEST_MSG)
				{
					Log(eWARN, "apc_SocketPeekComplete: HTTP request without enabling internal webserver");
					//Kill connection
					OnConnectionError(c);
					return;
				}
				if (c->m_reqType != eNEW_ADMIN_REQUEST_MSG)
					c->m_reqType = eNEW_HTTP_REQUEST_MSG;
			}else if (c->m_reqType == eNEW_ADMIN_REQUEST_MSG)
			{
				Log(eDEBUG, "apc_SocketPeekComplete: Got Jaxer (or unkown) request on admin request/connection");
				//Kill connection
				OnConnectionError(c);
				return;
			}else
			{
				//Must be a regular request or error
				//TODO: check error
				c->m_reqType = eNEW_REQUEST_MSG;
			}
#endif
			StartNewRequest(c);
		}
    }
}

VOID CALLBACK apc_NewAdministrator(ULONG_PTR arg)
{
    //SOCKET s = SOCKET(arg);
	ConnectionWithSetting *cs = (ConnectionWithSetting*)arg;
	SOCKET s = cs->m_socket;
    
    Administrator *a = new Administrator(s);
    a->Write("JaxerManager " JAXER_BUILD_ID "\r\n\r\n");
    a->Flush();
    a->BeginRecv();
}

VOID CALLBACK apc_NewConnection(ULONG_PTR arg)
{
	ConnectionWithSetting *cs = (ConnectionWithSetting*)arg;
	SOCKET s = cs->m_socket;
    //SOCKET s = SOCKET(arg);
    Connection *c = new Connection(s);
	c->m_reqType = cs->m_cs->mMsgType;
	delete cs;
	Log(eTRACE, "apc_NewConnection: s=%d c=%d", s, c);
    InitiateSocketPeek(c);
}

static void RedeployServer(Server *s)
{
	Log(eTRACE, "RedeployServer: s=%d pid=%d c=%d, DEBUG=%d", s, s->m_pid, s->m_boundConnection, s->m_markedForDebug);
	// For a debug jaxer, we will not push msgs to avoid another
    if (!s->m_markedForDebug && s->m_boundConnection) {
        Connection *c = s->m_boundConnection;
        s->m_boundConnection->m_boundServer = 0;
        s->m_boundConnection = 0;

        c->m_recving = false;
        InitiateSocketPeek(c);
    }
    
    // Always have a read pending on the server's stdout.  Even when it's idle,
    // the pending read will let us know if the process dies.
    if (!ReadFileEx(s->m_pipe, s->m_from.m_data, BUFSIZE, &s->m_from.m_overlapped,
               apc_PipeReadComplete))
    {
        Log(eWARN, GetLastError(), "RedeployServer: ReadFileEx failed");
        //Kill server and connection
        OnServerError(s);
    }

    // Here's our chance to push accumulated messages to it.
    if (Message::Next(s)) {
		Log(eTRACE, "RedeployServer: pushing msg to server");
        PushMessage(s);
        return;
    }

	if (s->m_markedForDebug)
	{
		Log(eTRACE, "RedeployServer: redeply debug jaxer %d", s->m_pid);
		Connection* oldC = s->m_boundConnection;
		if (oldC)
		{
			oldC->m_boundServer = 0;
			s->m_boundConnection = 0;
			oldC->m_recving = false;
			oldC->m_next = NULL;
		}

		Connection* ac = g_debugConnections;
		Connection* parent = NULL;
		if (ac)
			Log(eTRACE, "RedeployServer: see if any request waiting debug jaxer %d", s->m_pid);
		while(ac)
		{
			if (ac->m_boundServer == s)
			{
				Log(eTRACE, "RedeployServer: Find request %d waiting debug jaxer %d", ac, s->m_pid);
				if (parent)
					parent->m_next = ac->m_next;
				else
					g_debugConnections = ac->m_next;

				Log(eTRACE, "RedeployServer: Let jaxer %d server request %d", s->m_pid, ac);
				AssignServer(s, ac);
				break;
			}
			parent = ac;
			ac = ac->m_next;
		}

		if (!g_debugConnections || !g_debugConnections->m_next)
			g_debugConnectionsTail = g_debugConnections;


		if (oldC)
			InitiateSocketPeek(oldC);

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

VOID CALLBACK apc_ProcessCreated(ULONG_PTR arg)
{
    Server *s = (Server *)arg;
    g_processCreationPending = false;
    g_serverCount++;
    Message::BringUpToDate(s);

#if 0
	//TEST
	if (g_debugServers == NULL)
	{
		g_debugServers = s;
		g_serverCount--;
		s->m_markedForDebug = true;

		//remove from idle server list
		Server *i= g_idleServers;
		Server *p = NULL;
		while(i)
		{
			if  (i == s)
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
		//return;
	}
	// END TEST
#endif

    RedeployServer(s);
}


VOID CALLBACK apc_PipeReadComplete(DWORD dwError, DWORD dwTransfer,
                                   LPOVERLAPPED lpOverlapped)
{
    Log(eTRACE,"Enter apc_PipeReadComplete");

    Server *s = ((Buffer<Server> *)lpOverlapped)->m_owner;
    Connection *c = s->m_boundConnection;
    char *p = (char *)s->m_from.m_data;

	Log(eTRACE,"apc_PipeReadComplete: server=%d pid=%d, c=%d DEBUG=%d", s, s->m_pid, c, s->m_markedForDebug);
    // 0-type [12]-len [3...]-data
    if (dwError != 0 || dwTransfer < 3)
    {
        if (!s->m_exiting)
        {
            if ( dwError > 0)
                    Log(eINFO, "Terminating jaxer (pid=%d) due to pipe read error (result code = %d)", s->m_pid, dwError);
                else
                    Log(eINFO, "Terminating jaxer (pid=%d) due to pipe read error (no bytes transferred)", s->m_pid);
        }
        KillServer(s);
        return;
    }

    p[dwTransfer] = 0;
    switch (*p)
    {
    case eEND_REQUEST_MSG:
        RedeployServer(s);
        break;

    case eNO_RESP_CMD_MSG:
        ProcessNoRespCmd(s, p);
        if (!ReadFileEx(s->m_pipe, s->m_from.m_data, BUFSIZE, &s->m_from.m_overlapped,
               apc_PipeReadComplete))
        {
            Log(eWARN, GetLastError(), "apc_PipeReadComplete: ReadFileEx failed");
            //Kill server and connection
            OnServerError(s);
        }
        break;

    case eNEED_RESP_CMD_MSG:
        s->m_respId = 0;
        if (!s->ProcessNeedRespCmd())
        {
            OnServerError(s);
        }else
        {
            if (!ReadFileEx(s->m_pipe, s->m_from.m_data, BUFSIZE, &s->m_from.m_overlapped,
               apc_PipeReadComplete))
            {
                Log(eWARN, GetLastError(), "apc_PipeReadComplete: ReadFileEx failed");
                //Kill server and connection
                OnServerError(s);
            }
        }
        break;

    default:
        OnServerError(s);
        break;
    }
}

VOID CALLBACK apc_RetireAdministrator(ULONG_PTR arg)
{
    // Keep it for a while then delete it.
    Administrator *a = (Administrator *)arg;
    ObjectList<Administrator> *obj = new ObjectList<Administrator>(a);
    obj->m_next = g_IdleAdmin;
    g_IdleAdmin = obj;
}

VOID CALLBACK apc_DeleteConnection(ULONG_PTR arg)
{
    // Now that all completion callbacks for the closed socket have been
    // executed, the connection object can be deleted.
    Connection *c = (Connection *)arg;
    delete c;
}

VOID CALLBACK apc_DeleteServer(ULONG_PTR arg)
{
    // Now that all completion callbacks for the closed pipe have been
    // executed, the server object can be deleted.  The server process, if
    // still alive and kicking, will die when it sees the pipe has been
    // closed.
    Log(eTRACE, "Enter apc_DeleteServer %d", arg);
    Server *s = (Server *)arg;
    CloseHandle(s->m_pipe);
    CloseHandle(s->m_process);
    delete s;

    // If stopping or reloading, stuff happens once no servers are running.
    if (g_serverCount == 0 && g_debugCount == 0)
        LastServerStopped();
}

VOID CALLBACK apc_Timer(LPVOID, DWORD, DWORD)
{
    DoPeriodic();
}

void DoObjectCleanup()
{
    // Cleanup g_IdleAdmin
    time_t now;
    time (&now);

    ObjectList<Administrator> *p = g_IdleAdmin;
    ObjectList<Administrator> *q = 0;
    while (p)
    {
        if ( (now - p->m_time) >= g_IdleAdminWaitTime)
            break;
        else
        {
            q = p;
            p = p->m_next;
        }
    }

    if (q)
        q->m_next = 0;

    if (p == g_IdleAdmin)
        g_IdleAdmin = 0;

    while (p)
    {
        q = p->m_next;
        delete p;
        p = q;
    }
}

bool CreateLogServer()
{
    if (lsi.GetProcessHandle() != INVALID_HANDLE_VALUE)
    {
        CloseHandle(lsi.GetProcessHandle());
        lsi.SetProcessHandle(INVALID_HANDLE_VALUE);
    }

    if (lsi.GetProcessStdinHandle() != INVALID_HANDLE_VALUE)
    {
        CloseHandle(lsi.GetProcessStdinHandle());
        lsi.SetProcessStdinHandle(INVALID_HANDLE_VALUE);
    }

    if (lsi.GetProcessStderrHandle() != INVALID_HANDLE_VALUE)
    {
        CloseHandle(lsi.GetProcessStderrHandle());
        lsi.SetProcessStderrHandle(INVALID_HANDLE_VALUE);
    }


    //"| /opt/local/sbin/cronolog -a /home/aptana/logs/access/%Y/%U/access_log_%Y%m%d%H" 
    typedef std::map<std::string, std::string>::const_iterator iterator;

    iterator I = g_LogSettings.find("output");
    if (I == g_LogSettings.end())
    {
        fprintf(stderr, "JaxerManager: missing log output (--log:output=<absolute_path>\n");
        return false;
    }


    char sCmdLine[1024];
    const char *name;
    const char *value;
    size_t index = sprintf(sCmdLine, "JaxerLogger.exe --input=%s", lsi.GetFifoName());

    
    for (I = g_LogSettings.begin(); I != g_LogSettings.end(); ++I)
    {
        name = I->first.c_str();
        value = I->second.c_str();
        size_t nvl = strlen(name) + strlen(value) + 6;
        if (nvl + index > 1024)
        {
            fprintf(stderr, "JaxerManager: JaxerLogger commandline is too long.  Internal program error.\n");
            return false;
        }
        strcat(sCmdLine, " \"--");
        strcat(sCmdLine, name);
        strcat(sCmdLine, "=");
        strcat(sCmdLine, value);
        strcat(sCmdLine, "\"");
        index += nvl;   
    }


    //fprintf(stderr,"%s\n", sCmdLine);
    //sprintf(sCmdLine, "JaxerLogger.exe %s \"%s\"", lsi.GetFifoName(), lsi.GetLogFileName());
    //sprintf(sCmdLine, "JaxerLogger.exe --input=%s \"--output=%s\"", lsi.GetFifoName(), lsi.GetLogFileName());
    //sprintf(sCmdLine, "JaxerLogger.exe --input=%s \"--output=%s\"", lsi.GetFifoName(),
    //    "| cronolog.exe -a c:/access_log_%Y%m%d%H");

    HANDLE hLogStdinRd, hLogStdinWr;
    HANDLE hLogStderrRd, hLogStderrWr;
    SECURITY_ATTRIBUTES saAttr; 
    
    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDIN. 
    if (! CreatePipe(&hLogStdinRd, &hLogStdinWr, &saAttr, 0))
    {
        fprintf(stderr, "Create Stdin pipe for log failed.\n");
        return false;
    }

    if (! CreatePipe(&hLogStderrRd, &hLogStderrWr, &saAttr, 0))
    {
        CloseHandle(hLogStdinRd);
        CloseHandle(hLogStdinWr);
        fprintf(stderr, "Create Stderr pipe for log failed.\n");
        return false;
    }

    // Ensure that the write handle to the Log process's pipe for STDIN is not inherited. 
    SetHandleInformation( hLogStdinWr, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation( hLogStderrRd, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION) );

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO) );
    si.cb = sizeof(STARTUPINFO);
    si.hStdInput = hLogStdinRd;
    si.hStdError = hLogStderrWr;
    si.hStdOutput = hLogStderrWr;
    si.dwFlags |= STARTF_USESTDHANDLES;


    BOOL rc = CreateProcess(NULL,
                           sCmdLine,
                           NULL,  /* default process security descriptor */
                           NULL,  /* default thread security descriptor */
                           TRUE,  /* inherit handles */
                           CREATE_NO_WINDOW | CREATE_BREAKAWAY_FROM_JOB,
                           NULL,  /* inherit environment */
                           sJaxerImageDir,  /* current directory */
                           &si,
                           &pi);
    
    
    int i=0;
    while(rc && i<60)
    {
        static char buf[0x1000];
        DWORD nRead=0, nTotal=0, nLeft=0;
        rc = PeekNamedPipe(hLogStderrRd, buf, 0x1000, &nRead, &nTotal, &nLeft);
        if (rc && nRead > 0 && buf[0] == 1)
        {
            logme("Peek(%d): rc=%d nRead=%d buf[0]=%d", i, rc, nRead, buf[0]);
            break;
        }
        if (!rc)
        {
            fprintf(stderr, "PeekNamedPipe failed: error=%d\n", GetLastError());
            logme("PeekNamedPipe failed: error=%d", GetLastError());
            break;
        }

        if(rc && nRead>0)
        {
            buf[nRead] = 0;
            fprintf(stderr, "%s", buf);
            logme("ERR:%s", buf);
            rc = FALSE;
            break;
        }
        i++;
        Sleep(1000);

    }


    if (!rc)
    {
        fprintf(stderr, "Create JaxerLogger process failed.\n");
        CloseHandle(hLogStdinRd);
        CloseHandle(hLogStdinWr);
        CloseHandle(hLogStderrRd);
        CloseHandle(hLogStderrWr);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return false;
    }

    lsi.SetPid(pi.dwProcessId);
    lsi.SetProcessHandle(pi.hProcess);
    lsi.SetProcessStdinHandle(hLogStdinWr);
    lsi.SetProcessStderrHandle(hLogStderrRd);
    //CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}


void InitLogger(const char* sFifoName, const char* sLock)
{
    // We need a dummy file for locking.
    // we use the commandport as a key
    //char sLock[10];
    //sprintf(sLock, "%d", g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort);
    //if( !lc.Init(sFifoName, 100, sLock))
    if( !lc.Init(sFifoName, -1, sLock))
    {
        fprintf(stderr, "Init LogClient failed\n");
        g_elh.LogError(MSG_CANNOT_INIT_LOG);
        exit(1);
    }

    // This is liklely to fail.
    lc.OpenPipeForWrite();
    //logme("after OpenPipeForWrite");
}

int CommonMain(int argc, char **argv)
{
    // If just need the commandline syntex, do not go through the normal process
    if (argc > 1 && strcmp(argv[1], "--help") == 0)
    {
        DisplayUsage();
        return 0;
    }

    // Get CWD
    DWORD nChars = GetModuleFileName(0, sJaxerImageDir, sizeof(sJaxerImageDir)/sizeof(sJaxerImageDir[0]));
    if (nChars == sizeof(sJaxerImageDir)/sizeof(sJaxerImageDir[0]))
    {
        fprintf(stderr, "Internal buffer size too small to hold path.");
        return 1;
    }
    sJaxerImageDir[nChars] = 0;
    char *p = strrchr(sJaxerImageDir, '\\');
    *p = 0;

    // Initialize Winsock.
    WSADATA wsaData;
    if (int rc = WSAStartup(0x0202, &wsaData)) {
        Log(eERROR, GetLastError(), "Socket initialization error (ret=%d), make sure another JaxerManager is not already running.");
        g_elh.LogError(MSG_CANNOT_INIT_SOCKET);
        exit(1);
    }

    // Create a handle to this (main) thread that other threads can use to
    // post APCs to us.
    if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                    GetCurrentProcess(), &g_MainThread,
                    0, FALSE, DUPLICATE_SAME_ACCESS))
    {
        Log(eFATAL, GetLastError(), "DuplicateHandle failed.");
        g_elh.LogError(MSG_CANNOT_DUP_HANDLE);
        exit(1);
    }

    // Setup defaults
    InitSettings();

    // Read the defaut configuration file.
    //FILE *fd = fopen(g_ConfigFile, "r");
    //if (fd)
    //    LoadConfig(fd);

    // Process args.
    ProcessArgs(argc, argv);

    //We need the tmpdir to determine where we should do the dumping.  So this
    //is the earliest point we can do.
    //const char * tmpdir = ".";
    //iter it = g_Settings.find("-tempdir");
    //if (it != g_Settings.end())
    //    tmpdir = it->second.c_str();
	//const char * tmpdir = g_ManagerSettings[NAME_TEMP_DIR].c_str();
	const char* tmpdir = g_Settings["-tempdir"].c_str();
    
    size_t nlen = strlen(tmpdir) + 1;
    wchar_t *wDumpDir = new wchar_t[nlen];
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wDumpDir, nlen, tmpdir, _TRUNCATE);

#ifdef ENABLE_CRASHREPORTER
    google_breakpad::ExceptionHandler ch(
         wDumpDir,                             // Where to dump
         google_breakpad::BeforeDumpCallback,  // FilterCallback
         google_breakpad::AfterDumpCallback,
         NULL,                                 // callback context
         google_breakpad::ExceptionHandler::HANDLER_ALL);

#if 0
,
         MiniDumpNormal,                       // MINIDUMP_TYPE -- minimal for now
         NULL,                                 // Pipename -- used for dumping to external server   
         NULL //&google_breakpad::custom_info         // Additional customer info to include.
         );
#endif
#endif



    // Setup log fifo
    char sPipeName[256], sLock[20];
    sprintf(sPipeName, "\\\\.\\pipe\\.jaxer.%d.pipe", g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort);
    sprintf(sLock, "%d", g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort);
    lsi.SetFifoName(sPipeName);

    lc.SetVerboseOnError();
    InitLogger(sPipeName, sLock);
    lc.SetSilentOnError();

    // Set logfilename in env
    _putenv_s("JAXERLOG_PIPENAME", lsi.GetFifoName());

    lsi.SetLogFileName(g_LogSettings["output"].c_str());
    //Create LogServer
    logme("calling CreateLogServer");
    if(!CreateLogServer())
    {
        g_elh.LogError(MSG_CANNOT_CREATE_LOG_SERVER);
        exit(1);
    }
    //logme("CreateLogServer done");

    if (!lc.OpenPipeForWriteWait(1))
    {
        g_elh.LogError(MSG_CANNOT_COMMUNICATE_LOG_SERVER);
        exit(1);
    }

    lc.SetVerboseOnError();

    Log(eNOTICE, "JaxerManager " JAXER_BUILD_ID " (web %d, command %d)", 
			g_ConnectionSettings[eWEB_JAXER_PROTOCOL].mPort, g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mPort);

    // Create helper threads (and in this order to avoid race conditions).
    g_ProcessThread = StartThread(ProcessThread, NULL);
    //g_ListenThread = StartThread(ListenThread, &g_ConnectionSettings[eWEB_JAXER_PROTOCOL].mPort);
    //g_WebHTTPListenThread = StartThread(ListenThread, &g_WebHTTPPort);
    //g_AdminHTTPListenThread = StartThread(ListenThread, &g_AdminHTTPPort);
	//if (g_ConnectionSettings[eADMIN_JAXER_PROTOCOL].mEnabled)
	//	g_AdminThread = StartThread(AdminThread, &g_ConnectionSettings[eADMIN_JAXER_PROTOCOL]);

	for (int k=0; k<g_nConnectionSettings; k++)
	{
		if (g_ConnectionSettings[k].mEnabled)
		{
			Log(eDEBUG, "Enabling connection: interface=%s port=%d", g_ConnectionSettings[k].mInterface,
				g_ConnectionSettings[k].mPort);
			if (g_ConnectionSettings[k].mType == eADMIN_JAXER_PROTOCOL)
			{
				g_ListenerThreads[k] = StartThread(AdminThread, &g_ConnectionSettings[k]);
			}else
			{
				g_ListenerThreads[k] = StartThread(ListenThread, &g_ConnectionSettings[k]);
			}
		}else
		{
			Log(eDEBUG, "Disabling connection: interface=%s port=%d", g_ConnectionSettings[k].mInterface,
				g_ConnectionSettings[k].mPort);
		}
	}
    
    // Initiate server creation.
	unsigned g_MinServers = g_GeneralSettings[eNAME_MIN_JAXERS].miValue;
    if (g_MinServers > 0) {
        g_processCreationPending = true;
        QueueUserAPC(apc_CreateProcess, g_ProcessThread, 0);
    }

    // Startup a periodic timer.
    HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (hTimer == NULL) {
        Log(eERROR, GetLastError(), "Create waitable timer failure");
        g_elh.LogError(MSG_CANNOT_CREATE_WAITABLE_TIMER);
        exit(1);
    }
    LARGE_INTEGER when;
    when.QuadPart = -1 * 10000000;
    if (!SetWaitableTimer(hTimer, &when, 1000, apc_Timer, 0, FALSE)) {
        Log(eERROR, GetLastError(),"Set waitable timer failure");
        g_elh.LogError(MSG_CANNOT_SET_WAITABLE_TIMER);
        exit(1);
    }

    // Main loop.  Execute asynchronous I/O completion callbacks.
    while (true) {
        SleepEx(100000, TRUE);

		//Must update this.
		g_MinServers = g_GeneralSettings[eNAME_MIN_JAXERS].miValue;

        // If the number of servers is too low, initiate process creation.
        // Note that a server cannot die (or be created) without this loop
        // cycling as a side-effect.
        if (g_serverCount < g_MinServers)
            CreateNewProcess();

        // Check to see if we have anything to cleanup
        DoObjectCleanup();

        // If the log server dies, we need to restart another one
        if (lsi.GetProcessHandle() == INVALID_HANDLE_VALUE ||
            WaitForSingleObject(lsi.GetProcessHandle(), 0) == WAIT_OBJECT_0)
        {
            CreateLogServer();
        }
    }

    return 0;
}

//Added the following to support Windows Service

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle = 0;
HANDLE stopServiceEvent = 0;

// Service functions
void WINAPI ServiceControlHandler(DWORD controlCode);
void WINAPI ServiceMain(DWORD argc, char* argv[]);
void InstallService(int, char**);
void UninstallService();
static void OnStartService(int argc, LPCSTR* argv);
void StopService();

static BOOL GetRegistryParameter(HKEY hkey, const char *name, char *value, DWORD size);

typedef struct _thread_data 
{
	int argc;
	char **argv;
}
THREAD_DATA, *PTHREAD_DATA;

DWORD WINAPI ServiceThreadProc(LPVOID lpParam)
{
    PTHREAD_DATA pData;
	pData = (PTHREAD_DATA)lpParam;

    int argc;
    char** argv = 0;

    char  tag[1024];
    HKEY  hk;

    strcpy(tag, SERVICE_REGISTRY_LOCATION);
    strcat(tag, serviceName);
    strcat(tag, "\\");
    strcat(tag, PARAMS_LOCATION);

    if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                     tag,
                                     (DWORD)0,         
                                     KEY_READ,
                                     &hk))
    {
        logme("open key %s failed", tag);
        return -1;
    }

    char huge_buf[0x8000];
    DWORD nsize = 0x8000;
    char name[20];

    if (! GetRegistryParameter(hk, "argc", huge_buf, nsize))
    {
        RegCloseKey(hk);
        return -1;
    }

    DWORD n = atoi(huge_buf);
    argc = n + pData->argc;
    argv = new char*[argc];
    argv[0] = pData->argv[0];
    
    for(DWORD i=1; i<=n; i++)
    {
        sprintf(name, "argv%d", i);
        nsize = 0x8000;
        if (! GetRegistryParameter(hk, name, huge_buf, nsize))
        {
            RegCloseKey(hk);
            for (DWORD j=1; j<i; j++)
                delete[] argv[j];
            delete[] argv;
            return -1;
        }
        argv[i] = new char[strlen(huge_buf)+1];
        strcpy(argv[i], huge_buf);
    }

    for(int j=1; j<pData->argc; j++)
        argv[n+j] = pData->argv[j];

    RegCloseKey(hk);

    g_IsService = true;

    CommonMain(argc, argv);

    for(DWORD i=1; i<=n; i++)
        delete[] argv[i];
    delete[] argv;

    return 0;
}

int main(int argc, char** argv)
{
    logme("main: argc=%d argv0=%s", argc, argv[0]);
    for(int i=1; i<argc;i++)
        logme("argv[%d]=%s", i, argv[i]);

    if (argc > 1)
    {
        if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "-install") == 0)
	    {
		    InstallService(argc-2, argv+2);
		    return 0;
	    }else if (strcmp(argv[1], "-u") == 0 || strcmp(argv[1], "-uninstall") == 0)
	    {
		    UninstallService();
		    return 0;
	    }else if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-start") == 0)
	    {
            argv[1] = argv[0];
		    OnStartService(argc-1, (const char**) argv+1);
		    return 0;
	    }else if (strcmp(argv[1], "-stop") == 0)
	    {
		    StopService();
		    return 0;
	    }else if ( argc == 2 && strcmp(argv[1], displayName) == 0)
        {
	        // Run
	        SERVICE_TABLE_ENTRY serviceTable[] = 
	        {
		        {serviceName, ServiceMain},
		        {0, 0}
	        };

            StartServiceCtrlDispatcher(serviceTable);

	        return 0;
        }
    }

    // regular
    return CommonMain(argc, argv);
}

void WINAPI ServiceMain(DWORD argc, char* argv[])
{
    logme("In ServiceMain argc=%d argv[0]=%s", argc, argv[0]);
	// Initialise service status
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, ServiceControlHandler);

	if (serviceStatusHandle)
	{
		// Service is running
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		// Save the command args for passing to the thread
		PTHREAD_DATA pData = (PTHREAD_DATA)	HeapAlloc(
			GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(THREAD_DATA));

		if (pData!= NULL)
		{
			pData->argc = argc;
			pData->argv = argv;

			DWORD dwThreadId;
			HANDLE serviceThread = CreateThread( 
				NULL,               // default security attributes
				0,                  // use default stack size  
				ServiceThreadProc,  // thread function 
				pData,              // argument to thread function 
				0,                  // use default creation flags 
				&dwThreadId);	    // returns the thread identifier 
		 
			if (serviceThread != NULL)
			{
				// Create wait event
				stopServiceEvent = CreateEvent(0, FALSE, FALSE, 0);

				// Wait for it
				WaitForSingleObject(stopServiceEvent, INFINITE);
			}
		}

		// Service Stopped
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);
	}
}

static void StopAllServers();
static void onStopService(Administrator* a)
{
    Log(eINFO, "Terminating all jaxers per stop request.");
    StopAllServers();
    g_ManagerMode = mm_stopping;
    if (a->m_socket != INVALID_SOCKET)
        a->Write("*** ok\r\n");
    if (g_serverCount == 0 && g_debugCount == 0)
    {
        a->Flush();
        lc.Terminate();
        g_ManagerMode = mm_stopped;
    }

    while (g_ManagerMode != mm_stopped)
        SleepEx(1, TRUE);

	// Stop the service
	SetEvent(stopServiceEvent);
}

void WINAPI ServiceControlHandler(DWORD controlCode)
{
    Administrator a(INVALID_SOCKET);
    switch (controlCode)
	{
		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
	
			onStopService(&a);
			return;

		default:
			break;
	}

	SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

BOOL GetRegistryParameter(HKEY hkey, const char *name, char *value, DWORD size)
{   
    DWORD type = 0;
    LONG  lrc = RegQueryValueEx(hkey,     
                          name,      
                          (LPDWORD)0,
                          &type,    
                          (LPBYTE)value,
                          &size); 
    if(ERROR_SUCCESS != lrc)
        return FALSE;        
    
    value[size] = '\0';

    return TRUE;     
}

char *GetLastErrorText( char *lpszBuf, DWORD dwSize )
{
    DWORD dwRet;
    char *lpszTemp = NULL;

    dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
                          NULL,
                          GetLastError(),
                          LANG_NEUTRAL,
                          (char *)&lpszTemp,
                          0,
                          NULL);

    // supplied buffer is not long enough
    if(!dwRet || ((long)dwSize < (long)dwRet+14))
    {
        lpszBuf[0] = '\0';
    } else
    {
        lpszTemp[lstrlen(lpszTemp)-2] = '\0';  //remove cr and newline character
        sprintf(lpszBuf, "%s (0x%x)", lpszTemp, GetLastError());
    }

    if(lpszTemp)
    {
        LocalFree((HLOCAL) lpszTemp );
    }

    return lpszBuf;
}

static BOOL CreateRegistryKey(const char *tag, HKEY *key)
{
    LONG  lrc = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                               tag,
                               0,
                               NULL,
                               REG_OPTION_NON_VOLATILE,
                               KEY_WRITE,
                               NULL,
                               key,
                               NULL);
    return (ERROR_SUCCESS == lrc);
}

static int SetRegistryParameter(HKEY hkey, const char *name, char *value)
{       
    return  (ERROR_SUCCESS == RegSetValueEx(hkey, 
                        name,            
                        0,              
                        REG_SZ,  
                        (const BYTE*)value, 
                        (DWORD)strlen(value)));
}

typedef WINADVAPI BOOL (WINAPI * pfnChangeServiceConfig)
                       (SC_HANDLE hService, DWORD dwInfoLevel, LPVOID lpInfo);

static BOOL UpdateRegistrySettings(SC_HANDLE schService, int argc, char** argv)
{
    char  tag[0x1000];
    HKEY  hk;
    
    HMODULE hAdvApi32;
    char *szDescription = "Manages Jaxers and their communications with the webserver.  It starts a configured number of Jaxers and a JaxerLogger.";
    pfnChangeServiceConfig fnChangeServiceConfig;
            
    if((hAdvApi32 = GetModuleHandle("advapi32.dll"))
       && ((fnChangeServiceConfig = (pfnChangeServiceConfig)
            GetProcAddress(hAdvApi32, "ChangeServiceConfig2A"))))
    {
        (void) fnChangeServiceConfig(schService, // Service Handle
                                       1,        // SERVICE_CONFIG_DESCRIPTION
                                       &szDescription);
    }

    
    strcpy(tag, SERVICE_REGISTRY_LOCATION);
    strcat(tag, serviceName);
    strcat(tag, "\\");
    strcat(tag, PARAMS_LOCATION);

    if (!CreateRegistryKey(tag, &hk))
    {
        printf("Update registry Error: Can not create key %s - %s\n",
            tag, GetLastErrorText(szErr, sizeof(szErr)));
        return FALSE;
    }

    char value[20], name[20];
    sprintf(value, "%d", argc);
    if (! SetRegistryParameter(hk, "argc", value))
    {
        printf("Update registry Error: Can not create parameter %s - %s\n",
            argc, GetLastErrorText(szErr, sizeof(szErr)));
        RegCloseKey(hk);
        return FALSE;
    }
    
    for(int i=0; i<argc; i++)
    {
        sprintf(name, "argv%d", i+1);
        if(! SetRegistryParameter(hk, name, argv[i]))
        {
            printf("Update registry Error: Can not create parameter %s - %s\n",
                name, GetLastErrorText(szErr, sizeof(szErr))); 
            RegCloseKey(hk);
            return FALSE;
        }

    }

    RegCloseKey(hk);
    
    return TRUE;
}


void InstallService(int argc, char** argv)
{
	SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);

	if (serviceControlManager)
	{
		char path[0x2000 + 2];
        path[0] = '"';
        DWORD nlen=0;

		if ((nlen=GetModuleFileName(0, path+1, 2000)) > 0)
		{
            strcat(path, "\" \"");
            strcat(path, displayName);
            strcat(path, "\"");

			SC_HANDLE service = CreateService(
							serviceControlManager,
							serviceName, displayName,
							SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
							SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, path,
							0, 0, 0, 0, 0 );

			if(service)
            {
                if(UpdateRegistrySettings(service, argc, argv))
                {
                    printf("Service %s installed.\n", serviceName);
                    g_elh.Log(EVENTLOG_INFORMATION_TYPE, MSG_SERVICE_INSTALLED, serviceName);
                } else
                {
                    fprintf(stderr, "CreateService failed setting the registry\n");
                    g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_INSTALL_SERVICE, "Failed setting the registry");
                    DeleteService(service);
                }
                CloseServiceHandle(service);
            } else
            {
                fprintf(stderr, "CreateService failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
                g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_INSTALL_SERVICE, GetLastErrorText(szErr, sizeof(szErr)));
            }
		}

		CloseServiceHandle(serviceControlManager);
	}
}

void UninstallService()
{
	SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);

	if (serviceControlManager)
	{
		SC_HANDLE service = OpenService(serviceControlManager,
			serviceName, SERVICE_QUERY_STATUS | DELETE);

		if (service)
		{
			SERVICE_STATUS serviceStatus;

			if (QueryServiceStatus(service, &serviceStatus))
			{
				if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
                {
					DeleteService(service);
                    g_elh.Log(EVENTLOG_INFORMATION_TYPE, MSG_SERVICE_UNINSTALLED, serviceName);
                    printf("Service %s uninstalled\n", serviceName);
                }else
                {
                    fprintf(stderr, "Service not uninstalled because it is not in stopped state\n");
                    g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_UNINSTALL_SERVICE, "Service is not in stopped state");
                }
			}else
            {
                fprintf(stderr, "Failed to uninstall service due to query service status failure: %s\n",
                    GetLastErrorText(szErr, sizeof(szErr)));
                g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_UNINSTALL_SERVICE, "Query service status failed");
            }

			CloseServiceHandle(service);
		}else
        {
            fprintf(stderr, "Failed to uninstall service %s due to open service failure: %s\n",
                    serviceName, GetLastErrorText(szErr, sizeof(szErr)));
            g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_UNINSTALL_SERVICE, "Open service failed");
        }

		CloseServiceHandle(serviceControlManager);
	}else
    {
        fprintf(stderr, "Failed to uninstall service %s due to open SCM failure: %s\n",
                    serviceName, GetLastErrorText(szErr, sizeof(szErr)));
        g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_UNINSTALL_SERVICE, "Open SCM failed");
    }
}

void OnStartService(int argc, LPCSTR* argv)
{
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;

    logme("on OnStartService argc=%d argv0=%s", argc, argv[0]);
    schSCManager = OpenSCManager(NULL,     // machine (NULL == local)
                                 NULL,     // database (NULL == default)
                                 SC_MANAGER_ALL_ACCESS);   // access required                       

    if(schSCManager)
    {
        schService = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS);
 
       if(schService)
       {
            // try to start the service
            if(StartService(schService, argc, argv))
            {
                printf("Starting %s.", serviceName);
                Sleep(1000);
                SERVICE_STATUS ssStatus;

                while(QueryServiceStatus(schService, &ssStatus ))
                {
                    if(ssStatus.dwCurrentState == SERVICE_START_PENDING)
                    {
                        printf(".");
                        Sleep(1000);
                    } else
                    {
                        break;
                    }
                }

                if(ssStatus.dwCurrentState == SERVICE_RUNNING)
                {
                    printf("\n%s started.\n", serviceName);
                    g_elh.Log(EVENTLOG_INFORMATION_TYPE, MSG_SERVICE_STARTED, serviceName);
                } else
                {
                    printf("\n%s failed to start.\n", serviceName);
                    g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_START_SERVICE, "Service not running");
                }
            }
            else
            {
                printf("StartService failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
                g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_START_SERVICE, GetLastErrorText(szErr, sizeof(szErr)));
            }

            CloseServiceHandle(schService);
        } else 
       {
            printf("OpenService failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
            g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_START_SERVICE, "Cannot open service.");
        }

        CloseServiceHandle(schSCManager);
    } else
    {
        printf("OpenSCManager failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
        g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_START_SERVICE, "Open SCM failed.");
    }
}

void StopService()
{
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;

    schSCManager = OpenSCManager(NULL,     // machine (NULL == local)
                                 NULL,     // database (NULL == default)
                                 SC_MANAGER_ALL_ACCESS);   // access required                       

    if(schSCManager)
    {
        schService = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS);

        if(schService)
        {
            SERVICE_STATUS ssStatus;
            // try to stop the service
            if(ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus))
            {
                printf("Stopping %s.", serviceName);
                Sleep(1000);

                while(QueryServiceStatus(schService, &ssStatus))
                {
                    if(ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
                    {
                        printf(".");
                        Sleep(1000);
                    } else
                    {
                        break;
                    }
                }

                if(ssStatus.dwCurrentState == SERVICE_STOPPED)
                {
                    printf("\n%s stopped.\n", serviceName);
                    g_elh.Log(EVENTLOG_INFORMATION_TYPE, MSG_SERVICE_STOPPED, serviceName);
                } else 
                {
                    printf("\n%s failed to stop.\n", serviceName);
                    g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_STOP_SERVICE, "Not stopped");
                }
            }
            else
            {
                printf("StopService failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
                g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_STOP_SERVICE, GetLastErrorText(szErr, sizeof(szErr)));
            }

            CloseServiceHandle(schService);
        } else 
        {
            printf("OpenService failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
            g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_STOP_SERVICE, "Open service failed.");
        }

        CloseServiceHandle(schSCManager);
    } else
    {
        printf("OpenSCManager failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
        g_elh.Log(EVENTLOG_ERROR_TYPE, MSG_CANNOT_STOP_SERVICE, "Open SCM failed.");
    }
}

// Include implementation common to Windows and Unix.
#include "impl.h"


#if 0
VOID CALLBACK apc_NewAdministratorNew(ULONG_PTR arg)
{
    SOCKET s = SOCKET(arg);
    Administrator *a = new Administrator(s);
    a->BeginRecv();
}

    void Write(const char *p, int len = -1)
    {
        Log(eTRACE, "Enter Administrator::Write strlen=%d len=%d", strlen(p), len);
        
        if (len < 0)
            len = int(strlen(p));
        LogTrace(p, len);

        while (len > 0) {
            int amt = min(len, BUFSIZE - m_toTail->m_in);
            memcpy(m_toTail->m_data + m_toTail->m_in, p, amt);
            m_toTail->m_in += amt;
            len -= amt;
            p += amt;

            if (m_toTail->m_in == BUFSIZE)
                Flush();
        }
    }

    void Flush()
    {
        m_toTail->m_flush = true;
        if (m_toTail == m_toHead)
            BeginSend();
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