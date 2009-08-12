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
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include "LogServerUnix.h"

#ifdef _MAC
    static timeval mac_tv;
#endif

static LogServer ls;
void EventLogCB(int severity, const char* s)
{
    //logme("Server: EventLogCB");
    eLogLevel eLevel = eINFO;
    switch(severity)
    {
        case _EVENT_LOG_DEBUG:
            eLevel = eDEBUG;
            break;
        case _EVENT_LOG_WARN:
            eLevel = eWARN;
            break;
        case _EVENT_LOG_ERR:
            eLevel = eERROR;
            break;
        case _EVENT_LOG_MSG:
            eLevel = eNOTICE;
            break;
    }
    ls.WriteLog(gsLogLevel[eLevel], s, strlen(s));
}

LogServer::LogServer()
:m_fdPipe(-1)
{
}

LogServer::~LogServer()
{
    //logme("In DTOR");
    if (m_fdPipe != -1)
    {
        close(m_fdPipe);
        remove(m_sPipe);
    }
}

bool LogServer::OpenLogForWrite()
{
    m_fdLog = open(m_sLog, O_WRONLY|O_APPEND|O_CREAT|O_NDELAY, 0777);
    if (m_fdLog == -1)
    {
        logme("LogServer: OpenLogForWrite failed: %s: err=%d %s",
          m_sLog, errno, strerror(errno));
        perror("LogServer: Error openning log file");
        return false;
    }
    return true;
}

bool LogServer::Init()
{
    //logme("In InitLog");
    m_fdPipe = open(m_sPipe, O_RDONLY);
    //logme("In InitLog: open pipe=%s %d", m_sPipe, m_fdPipe);
    if (m_fdPipe == -1)
    {
      perror("Error openning pipe");
      return false;
    }

    //logme("In Init: evset");
    event_set(&m_ev, m_fdPipe, EV_READ, PipeReadReadyCallback, this);
    timeval *pt = NULL;
#ifdef _MAC
    mac_tv.tv_sec = 0;
    mac_tv.tv_usec = 1;
    pt = &mac_tv;
#endif

    //logme("In Init: evadd");
    if (event_add(&m_ev, pt))
    {
        perror("In Init: evadd failed");
        return false;
    }

    if (!IsPipingToExternal())
    {
        if (! OpenLogForWrite())
            return false;
        if (UpdateLogFileSize() && m_nCurrentFileSize>=m_nMaxFileSize)
        {
            DoLogFileRotation();
            time(&m_timeLast);
        }
    }else
    {
        //Create Pipe
        return CreateLogPipe();
    }
    return true;
}

bool LogServer::CreateLogPipe()
{
        m_fpLog = popen(m_sProgCmds, "w");
        return (m_fpLog != NULL);
}

#if 0
bool LogServer::CreateLogPipe()
{
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) <0)
    {
        perror("LogServer:CreateLogPipe Failed to create Unix-domain socket pair");
        return false;
    }

    pid_t pid = fork();
    if (pid<0)
    {
        perror("LogServer: Faied to fork");
        return false;
    }
    if (pid>0)
    {
        //we are the parent
        close(sv[0]);
        m_fdLog = sv[1];
    }else
    {
        //we are the child
        close(sv[1]);
        dup2(sv[0], 0);
        close(sv[0]);

        //form argv[]
        char *argv[100];
        char *p = m_sProgCmds;
        int i=0;
        bool bHasQuote = false;
        for(i=0;*p;i++)
        {
            while(isspace(*p)) p++;
            bHasQuote = (*p == '"');
            if (bHasQuote)
            {
                p++;
                argv[i] = p;
                while(*p && *p != '"')
                {
                    if (*p == '\\')
                      p++;
                    p++;
                }
                *p++ = 0;
            }else
            {
                argv[i] = p;
                while(*p && *p != ' ')
                {
                    if (*p == '\\')
                      p++;
                    p++;
                }
                *p++ = 0;
            }
        }
        argv[i] = 0;
        int rc = execv(argv[0], argv);
        printf("execv(%s) failed: err=%d %s\n", argv[0], errno, strerror(errno)); fflush(stdout);
        return false;
    }
    return true;
}
#endif
void LogServer::WriteLog(const char* sSeverity, const char *msg, int len)
{
    char buf[256];
    time_t rawtime;
    struct tm* tinfo;
    time( &rawtime);
    tinfo = localtime(&rawtime);
    sprintf(buf, "%02d:%02d:%02d %02d/%02d/%04d [%06d] [%s] [JaxerLogger] ",
       tinfo->tm_hour, tinfo->tm_min, tinfo->tm_sec,
       tinfo->tm_mon+1, tinfo->tm_mday, tinfo->tm_year+1900);
    WriteAll(buf, strlen(buf));
    WriteAll((void*)msg, len);
    WriteAll((void*)"\n", 1);
}

void LogServer::ProcessInputData()
{
    logme("LogServer: In ProcessInputData:");
    char buf[MAX_BUF_SIZE];
    int numread;
    while((numread = read(m_fdPipe, buf, MAX_BUF_SIZE))>0)
        WriteAll(buf, numread);
    timeval *pt = NULL;
#ifdef _MAC
    mac_tv.tv_sec = 0;
    mac_tv.tv_usec = 1000;
    pt = &mac_tv;
#endif
    event_add(&m_ev, pt);
}

void PipeReadReadyCallback(int fd, short evt, void* vp)
{
    logme("LogServer: in PipeReadReadyCallback");
    LogServer* ls = (LogServer*) vp;
    ls->ProcessInputData();
}

#if 0
void OnTimer(int sock, short evts, void* pv)
{
    event *ev = (event*) pv;
    char buf;
    int ret = recv(sock, &buf, 1, MSG_PEEK|MSG_DONTWAIT);
    if (ret == 0 || (ret==-1 && errno==ECONNRESET))
    {
        logme("msg shutdown...");
        exit(0);
    }
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    event_add(ev, &tv);
}
#endif

void* ReadDataFromStdin(void*)
{
    logme("loggerserver: in ReadDataFromStdin");
    int rc;
    char buf[256];
    while(1)
    {
        rc = read(0, buf, 256);
        if(rc<= 0) // && errno != EINTR)
            break;
    }
    logme("jaxerlogger:MonitorThread: exiting");
    exit(0);
    return 0;
}

int main(int argc, char *argv[])
{
    logme("in LogServer main: argc=%d", argc);
    event_init();
    if (!ls.ParseCmds(argc, argv) || !ls.Init())
    {
        exit(1);
    }

    //logme("Server: event_set_log_callback");
    event_set_log_callback(EventLogCB);

    //Create a different thread to linsten on mgr
    pthread_t monitorThread;
    int rc = pthread_create(&monitorThread, NULL, ReadDataFromStdin, NULL);
    if (rc)
    {
        printf("jaxerlogger: failed to create thread. rc=%d err=[%d]%s\n",
            rc, errno, strerror(errno));
        logme("jaxerlogger: failed to create thread. rc=%d err=[%d]%s",
            rc, errno, strerror(errno));
        exit(1);
    }
#if 0
    event time_event;
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    event_set(&time_event, 0, EV_TIMEOUT, OnTimer, &time_event);
    event_add(&time_event, &tv);
#endif

    event_dispatch();
    return 0;
}


