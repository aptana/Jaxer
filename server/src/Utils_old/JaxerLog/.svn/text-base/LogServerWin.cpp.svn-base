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

#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <stdio.h>
#include <stdlib.h>


#include <sys/stat.h>
#include <time.h>
#include <process.h>
#include <windows.h> 
#include <tchar.h>
#include <strsafe.h>

#pragma warning(disable : 4995)

#include "LogServerWin.h"
#include "log.h"

#define DEFAULT_BACKUPS 10
#define DEFAULT_FILESIZE 512

static LogServer ls;

LogServer::LogServer()
{
}

LogServer::~LogServer()
{
}

BOOL LogServer::OpenLogForWrite()
{
    m_fdLog = CreateFile(m_sLog,
        FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
										  
    //logme("In InitLog: open log=%s %d", m_sLog, m_fdLog);
    if (m_fdLog == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "LogServer: cannot open log file(%s) for write (err=%d)\n", m_sLog,GetLastError());
        return FALSE;
    }
    return TRUE;
}

BOOL LogServer::Init()
{
    
    if (!IsPipingToExternal())
    {
        if (!OpenLogForWrite())
            return FALSE;

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
    return TRUE;
}

BOOL LogServer::CreateLogPipe()
{
    logme("LogServer::CreateLogPipe: cmd=%s", GetExternalCmd());
    m_fpLog = _popen(GetExternalCmd(), "wb");
    if (m_fpLog == NULL)
    {
        fprintf(stderr, "LogServer: failed to open pipe (%s). err=%d\n", GetExternalCmd(), GetLastError());
        return FALSE;
    }
    return TRUE;

#if 0
        // Create the program
        HANDLE hProgStdinRd, hProgStdinWr;
        SECURITY_ATTRIBUTES saAttr; 
        
        // Set the bInheritHandle flag so pipe handles are inherited.
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        // Create a pipe for the child process's STDIN. 
        if (! CreatePipe(&hProgStdinRd, &hProgStdinWr, &saAttr, 0))
        {
            printf("JaxerLogger: Create Stdin pipe failed.\n");
            return FALSE;
        }

        // Ensure that the write handle to the child process's pipe for STDIN is not inherited. 
        SetHandleInformation( hProgStdinWr, HANDLE_FLAG_INHERIT, 0);

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION) );

        STARTUPINFO si;
        ZeroMemory(&si, sizeof(STARTUPINFO) );
        si.cb = sizeof(STARTUPINFO);
        si.hStdInput = hProgStdinRd;
        si.dwFlags |= STARTF_USESTDHANDLES;

        logme("JaxerLog: pipe cmd=%s", GetExternalCmd());
        BOOL rc = CreateProcess(NULL,
                           (TCHAR*)GetExternalCmd(),
                           NULL,  /* default process security descriptor */
                           NULL,  /* default thread security descriptor */
                           TRUE,  /* inherit handles */
                           CREATE_NO_WINDOW | CREATE_BREAKAWAY_FROM_JOB,
                           NULL,  /* inherit environment */
                           NULL,  /* inherit current directory */
                           &si,
                           &pi);
    
        if (!rc)
        {
            char s[256];
            sprintf(s,"JaxerLogger: Create process (%s) failed. err=%d\n", GetExternalCmd(),
                GetLastError());
            char *t = s;
            logme(s);
            return FALSE;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        m_fdLog = hProgStdinWr;
    }

    return TRUE;
#endif
}

void LogServer::WriteLog(const TCHAR* sSeverity, const TCHAR *msg, int len)
{
    TCHAR buf[256];
    time_t rawtime;
    struct tm* tinfo;
    time( &rawtime);
    tinfo = localtime(&rawtime);
    _stprintf(buf, TEXT("%02d:%02d:%02d %02d/%02d/%04d [%06d] [%s] [JaxerLogger] "),
       tinfo->tm_hour, tinfo->tm_min, tinfo->tm_sec,
       tinfo->tm_mon+1, tinfo->tm_mday, tinfo->tm_year+1900,
       getpid(), sSeverity);

    m_mutex.Lock();
    WriteAll((void*)buf, (int) _tcslen(buf)*sizeof(TCHAR));
    WriteAll((void*)msg, len*sizeof(TCHAR));
    WriteAll((void*)TEXT("\r\n"), 2*sizeof(TCHAR));

    m_mutex.Unlock();
}

BOOL LogServer::WriteAll(void* vp, int n)
{
    m_mutex.Lock();
    BOOL ret = LogServerBase::WriteAll(vp, n);
    m_mutex.Unlock();
    return ret;
}

DWORD WINAPI LogThread(LPVOID lpvParam) 
{ 
   char msg[MAX_BUF_SIZE];  
   DWORD cbBytesRead; 
   
// The thread's parameter is a handle to a pipe instance. 
   HANDLE hPipe = (HANDLE) lpvParam;
   
   logme("in LogThread hPipe=%d tid=%d\n", hPipe, GetCurrentThreadId());
   
   while (1) 
   { 
       //printf("[%d-%d]: About to read...\n", tid, hPipe);fflush(stdout);
   // Read client requests from the pipe. 
      BOOL fSuccess = ReadFile( 
         hPipe,        // handle to pipe 
         msg,          // buffer to receive data 
         MAX_BUF_SIZE, // size of buffer 
         &cbBytesRead, // number of bytes read 
         NULL);        // not overlapped I/O 

      if (! fSuccess)
      {
          printf("LogThread: ReadFile failed\n");fflush(stdout);
          break; 
      }
      
      if (cbBytesRead != 0)
      {
          //msg[cbBytesRead] = 0;
          //printf("[%d-%d]WA:%s\n", tid, hPipe, msg);fflush(stdout);
          fSuccess = ls.WriteAll(msg, cbBytesRead);
          if (! fSuccess)
          {
              printf("LogThread: WriteAll failed\n");
              break; 
          }
      }
  } 
 
 
   logme("LogThread retruning; hPipe=%d tid=%d\n", hPipe, GetCurrentThreadId());
   DisconnectNamedPipe(hPipe); 
   CloseHandle(hPipe);
   
   return 1;
}

DWORD WINAPI MonitorThread(LPVOID) 
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE)
    {
        logme("MonitorThread:Failed to get stdin handle\n");
        fprintf(stderr, "Failed to get stdin handle\n");
        ExitProcess(1);
    }

    CHAR chBuf[256]; 
    DWORD dwRead; 
   
    for (;;) 
   { 

    // Read from standard input. 
      BOOL fSuccess = ReadFile(hStdin, chBuf, 256, &dwRead, NULL); 
      if (! fSuccess || dwRead == 0)
      {
          logme("fSuccess=%d dwRead=%d err=%d", fSuccess, dwRead, GetLastError());
          fprintf(stderr, "Terminating due to manager exit\n");
          ExitProcess(1);
      }
   }
    return 1;
}

int _tmain(int argc, TCHAR *argv[])
{
    //Sleep(20 * 1000);
    logme("in LogServer main: argc=%d", argc);

    if(!ls.ParseCmds(argc, argv) || !ls.Init())
    {
        exit(1);
    }

    //Create a thread to linsten on manager
    DWORD dwThreadId;
            
    HANDLE hMonitorThread = CreateThread( 
        NULL,              // no security attribute 
        0,                 // default stack size 
        MonitorThread,     // thread proc
        NULL,              // thread parameter 
        0,                 // not suspended 
        &dwThreadId);      // returns thread ID 
    
    logme("***[%d-%d]CreateThread\n", dwThreadId, hMonitorThread);
    if (hMonitorThread == NULL) 
    {
        fprintf(stderr, "CreateMonitor Thread failed\n"); 
        return 0;
    }else
    {
        //printf("[%d-%d]closing thread\n", dwThreadId, hPipe);fflush(stdout);
        CloseHandle(hMonitorThread); 
    }
    const char One = 1;
    fwrite(&One, 1, 1, stderr);
    fflush(stderr);

    for (;;) 
    { 
        //printf("***calling CreateNamedPipe***\n");

        HANDLE hPipe = CreateNamedPipe( 
          ls.GetPipeName(),         // pipe name 
          PIPE_ACCESS_INBOUND,      // read access 
          PIPE_TYPE_MESSAGE |       // message type pipe 
          PIPE_READMODE_MESSAGE |   // message-read mode
          PIPE_WAIT,                // blocking mode 
          PIPE_UNLIMITED_INSTANCES, // max. instances  
          MAX_BUF_SIZE,                  // output buffer size 
          MAX_BUF_SIZE,                  // input buffer size 
          0,                        // client time-out 
          NULL);                    // default security attribute 
        
        if (hPipe == INVALID_HANDLE_VALUE) 
        {
            printf("CreatePipe failed\n"); 
            return 0;
        }
 
        logme("***[%d]CreateNamedPipe***\n", hPipe);

        // Wait for the client to connect; if it succeeds, 
        // the function returns a nonzero value. If the function
        // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
        
        BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ? 
         TRUE : (GetLastError() == ERROR_IO_PENDING); //ERROR_PIPE_CONNECTED); 
        
        //printf("***[%d]ConnectNamedPipe return %d***\n", hPipe, fConnected);
        if (fConnected) 
        {
            DWORD dwThreadId;
            
            // Create a thread for this client.
            HANDLE hThread = CreateThread( 
                NULL,              // no security attribute 
                0,                 // default stack size 
                LogThread,    // thread proc
                (LPVOID) hPipe,    // thread parameter 
                0,                 // not suspended 
                &dwThreadId);      // returns thread ID 
            
            logme("***[%d-%d-%d]CreateThread\n", dwThreadId, hPipe, hThread);
            if (hThread == NULL) 
            {
                CloseHandle(hPipe);
                printf("CreateThread failed"); 
                return 0;
            }else
            {
                //printf("[%d-%d]closing thread\n", dwThreadId, hPipe);fflush(stdout);
                CloseHandle(hThread); 
            }
        }else 
        {
            // The client could not connect, so close the pipe. 
            logme("[%d]Closing pipe\n", hPipe);
            CloseHandle(hPipe); 
        }
    } 

    return 1; 
} 
 
