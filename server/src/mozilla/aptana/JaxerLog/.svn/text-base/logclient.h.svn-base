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
#ifndef __JAXER_LOGCLIENT_H__
#define __JAXER_LOGCLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <unistd.h>
#include <sys/file.h>
#include <time.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#if OSARCH == SunOS || OSARCH == Darwin
#include <fcntl.h>
#endif

#include <string.h>
#include <signal.h>


//#define _DEBUG_JXLOG
#include "log.h"

#ifndef PIPE_BUF
#define PIPE_BUF 512
#endif

static void 
#ifndef _WIN32
*
#endif
LogThread(void* arg);

struct MessageList
{
    char* msg;
    int len;
    MessageList *next;
    MessageList(char* s=NULL, int l=0):msg(s),len(l), next(0){};
    ~MessageList(){ delete[] msg;};
};

static MessageList NULLMsg;

#ifdef _WIN32
class FileLock
{
    OVERLAPPED m_ovlp;
    HANDLE m_fd;

public:
    FileLock(HANDLE fd)
    :m_fd(fd)
    {    
        memset(&m_ovlp, 0, sizeof(m_ovlp));
        BOOL ret = LockFileEx(m_fd, LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &m_ovlp);

        if (! ret)
        {
            DWORD err = GetLastError();
            //fprintf(stderr, "LockFileEx failed: error=%d\n", err);
        }
    }

    ~FileLock()
    {
        BOOL ret = UnlockFileEx(m_fd, 0, 1, 0, &m_ovlp);
        if (! ret)
        {
            DWORD err = GetLastError();
            //fprintf(stderr, "UnlockFileEx failed: error=%d\n", err);
        }
    };
};

class MutexLock
{
    HANDLE m_mLock;
public:
    MutexLock()
    {
          m_mLock = CreateMutex(NULL, FALSE, NULL);
    };
    ~MutexLock()
    {
          CloseHandle(m_mLock);
    };
    bool Lock()
    {
        DWORD ret = WaitForSingleObject( 
            m_mLock,    // handle to mutex
            INFINITE);  // no time-out interval
        return (ret == WAIT_OBJECT_0);
    };
    bool Unlock()
    {
        BOOL ret = ReleaseMutex(m_mLock);
        return (ret == TRUE);
    }
};
#else

#ifdef SOLARIS
struct flock* file_lock(short type, short whence)
{
    static struct flock ret;
    ret.l_type = type;
    ret.l_start = 0;
    ret.l_whence = whence;
    ret.l_len = 0;
    ret.l_pid = getpid();
    return &ret;
}
#endif /* SOLARIS */

class FileLock
{
    HANDLE m_fd;

public:
    FileLock(HANDLE fd)
    :m_fd(fd)
    {
#ifdef SOLARIS
        int ret = fcntl(m_fd, F_SETLKW, file_lock(F_WRLCK, SEEK_SET));
#else
        int ret = flock(m_fd, LOCK_EX);
#endif //SOLARIS
        if (ret != 0)
        {
            //fprintf(stderr, "fcntl failed: pid=%d ret=%d err=%d %s\n", getpid(), ret, errno, strerror(errno));
        }
    }

    ~FileLock()
    {
#ifdef SOLARIS
        fsync(m_fd);
        fcntl(m_fd, F_SETLK, file_lock(F_UNLCK, SEEK_SET));
#else
        flock(m_fd, LOCK_UN);
#endif //SOLARIS
    }
};

class MutexLock
{
    pthread_mutex_t m_mLock;
public:
    MutexLock()
    {
          pthread_mutex_init(&m_mLock, NULL);
    };
    ~MutexLock()
    {
          pthread_mutex_destroy(&m_mLock);
    };
    bool Lock()
    {
        int ret = pthread_mutex_lock(&m_mLock);
        return (ret == 0);
    };
    bool Unlock()
    {
        int ret = pthread_mutex_unlock(&m_mLock);
        return (ret == 0);
    }
};
#endif //_WIN32

class LogClient
{
    char* m_sPipe;
    HANDLE   m_fdPipe;
    MessageList* m_msgHeader;
    MessageList* m_msgTail;
    int  m_nMessages;
    int  m_nMaxMessages;
    bool m_bOK;
    bool m_bInited;
    bool m_bSilentOnError;
    MutexLock m_mLock;
    HANDLE    m_fdLock;
    bool m_bDone;
    bool m_bExit;
    bool m_bFailedWrite;
#ifndef _WIN32
    pthread_t m_thread; //not used
#endif

public:
    LogClient():m_sPipe(NULL)
      ,m_fdPipe(INVALID_HANDLE_VALUE)
      ,m_msgHeader(&NULLMsg)
      ,m_msgTail(&NULLMsg)
      ,m_nMessages(0)
      ,m_nMaxMessages(100)
      ,m_bOK(false)
      ,m_bInited(false)
      ,m_bSilentOnError(true)
      ,m_fdLock(INVALID_HANDLE_VALUE)
      ,m_bDone(false)
      ,m_bExit(false)
      ,m_bFailedWrite(false)
#ifndef _WIN32
      ,m_thread(0)
#endif
      {
      };

    ~LogClient()
    {
      if (m_sPipe)
          delete[] m_sPipe;

      while (m_msgHeader->next)
      {
          MessageList *m = m_msgHeader->next;
          m_msgHeader->next = m->next;
          delete m;
      }
  
      if (m_fdPipe != INVALID_HANDLE_VALUE)
      {
#ifdef _WIN32
          CloseHandle(m_fdPipe);
#else
          close(m_fdPipe);
#endif
      }

      if (m_fdLock != INVALID_HANDLE_VALUE)
      {
#ifdef _WIN32
          CloseHandle(m_fdLock);
#else
          close(m_fdLock);
#endif
      }

    };

    void SetVerboseOnError() { m_bSilentOnError=false;}
    void SetSilentOnError() { m_bSilentOnError=true;}
    bool OpenPipeForWrite()
    {
        logme("In LogClient::OpenPipeForWrite(); pipe=%d", m_fdPipe);
        if (m_fdPipe != INVALID_HANDLE_VALUE)
        {
    #ifdef _WIN32
            CloseHandle(m_fdPipe);
        }

        while (1) 
       { 
            m_fdPipe = CreateFile( 
                m_sPipe,   // pipe name 
                GENERIC_WRITE,  // write access 
                0,              // no sharing 
                NULL,           // default security attributes
                OPEN_EXISTING,  // opens existing pipe 
                0,              // default attributes 
                NULL);          // no template file 

            // Break if the pipe handle is valid. 

            if (m_fdPipe != INVALID_HANDLE_VALUE) 
                break; 

            // Exit if an error other than ERROR_PIPE_BUSY occurs. 

            if (GetLastError() != ERROR_PIPE_BUSY) 
            {
                if (!m_bSilentOnError)
                    printf("Could not open pipe. err=%d\n", GetLastError()); 
                return false;
            }

            // All pipe instances are busy, so wait for 5 seconds. 

            if (!WaitNamedPipe(m_sPipe, 5000)) 
            { 
                if (!m_bSilentOnError)
                    printf("Could not open pipe (wait) err=%d\n", GetLastError()); 
                return false;
            }
        }

        DWORD dwMode = PIPE_READMODE_MESSAGE; 
        if (! SetNamedPipeHandleState( 
          m_fdPipe,    // pipe handle 
          &dwMode,  // new pipe mode 
          NULL,     // don't set maximum bytes 
          NULL)    // don't set maximum time 
          )
       {
           if (!m_bSilentOnError)
               printf("SetNamedPipeHandleState failed. error=%d\n", GetLastError()); 
          return false;
       }

    #else
            close(m_fdPipe);
        }
        m_fdPipe = open(m_sPipe, O_WRONLY|O_NDELAY);
    #endif

        logme("OpenPipeForWrite: fdPipe=%d", m_fdPipe);
        m_bOK = (m_fdPipe != INVALID_HANDLE_VALUE);
        return m_bOK;
    };

    bool OpenPipeForWriteWait(int secs)
    {
        int i=0;
        for(i=0; i<secs; i++)
        {
            if (OpenPipeForWrite())
            {
                m_bInited = true;
                return true;
            }else
#ifdef _WIN32
                Sleep(1000);
#else
                sleep(1);
#endif
        }
        return false;
    }

    bool Init(const char* sPipe, int nMsgQueueSize=100, const char* sLock = "4328")
    {
        logme("In LogClient::Init(%s)", sPipe);
        if (nMsgQueueSize> 100)
           m_nMaxMessages = nMsgQueueSize;

        if (!sPipe) return false;
        m_sPipe = new char[strlen(sPipe)+1];
        if (!m_sPipe)
        {
            logme("In LogClient::new failed");
            return false;
        }
        strcpy(m_sPipe, sPipe);

#ifndef _WIN32

        m_bOK = false;
        int ret_val = mkfifo(m_sPipe, 0666);
        if (ret_val == -1 && errno != EEXIST)
        {
            logme("mkfifo failed. err=%d %s", errno, strerror(errno));
            if (!m_bSilentOnError)
                perror("mkfifo failed");
            return m_bOK;
        }

        //Create thread
        ret_val = pthread_create(&m_thread, NULL, LogThread, this);
        if (ret_val != 0)
        {
            if (!m_bSilentOnError)
                perror("LogClient failed to create thread");
            return m_bOK;
        }

        char sLockFilePath[56];
        sprintf(sLockFilePath, "/tmp/.Jaxer_%s.lock", sLock);
        m_fdLock = open(sLockFilePath, O_WRONLY | O_APPEND | O_CREAT, 0777);
        if (m_fdLock < 0)
        {
            if (!m_bSilentOnError)
                perror("LogClient failed to open file");
            return m_bOK;
        }
        

        m_bOK = true;
        return m_bOK;

#else
        //Create/Open the dummy lock file
        char sLockFilePath[PATH_MAX*2];
        DWORD ret = GetTempPath(PATH_MAX*2,  // length of the buffer
                           sLockFilePath);        // buffer for path
        DWORD ne = (DWORD) strlen(sLock);
        if (ret>0 && (ret+ne+12) < PATH_MAX*2)
        {
            sprintf(sLockFilePath+ret, "Jaxer_%s.lock", sLock);
            m_fdLock = CreateFile(sLockFilePath,
			          GENERIC_WRITE,
					  FILE_SHARE_WRITE | FILE_SHARE_DELETE,
					  NULL,
					  OPEN_ALWAYS,
					  FILE_ATTRIBUTE_HIDDEN,
					  NULL);
        }

        uintptr_t t = _beginthread(LogThread, 0, (void *)this);
        if (t == -1L)
        {
            fprintf(stderr, "LogClient failed to create thread.  Error=%d\n", errno);
            return false;
        }

        m_bInited = true;
        return true;
#endif
    };

    void Terminate()
    {
        m_bExit = true;
        for (int i=0; i<10; i++)
        {
            if (m_bDone)
                break;
#ifdef _WIN32
                Sleep(1000);
#else
                sleep(1);
#endif
        }
    };

    bool IsInited()
    {
        return m_bInited;
    };

    void DoLogWrite()
    {
        logme("Entered DoLogWrite: #msgs=%d", m_nMessages);
        while(true)
        {
            if (!m_bOK)
                OpenPipeForWrite();

            while (m_bOK && m_msgHeader->next)
            {
                //Need Mutex lock
                m_mLock.Lock();

                MessageList *m = m_msgHeader->next;
                m_msgHeader->next = m->next;
                m_nMessages--;
                if (!m_msgHeader->next)
                {
                    if(m != m_msgTail || m_nMessages != 0)
                    {
                        logme("ERROR empty msglist have #msg=%d m=%d tail=%d", m_nMessages, m, m_msgTail);
                    }
                    m_msgTail = m_msgHeader;
                }
                    
                logme("DoLogWrite: #msgs=%d msg=%d", m_nMessages, m);
                m_mLock.Unlock();

                Write(m->msg, m->len) ;
                delete m;
           }
            if (m_bExit)
                break;
#ifdef _WIN32
           Sleep(10);
#else
           timespec ts;
		   ts.tv_sec = 0;
		   ts.tv_nsec = 10 * 1000 * 1000;
           nanosleep(&ts, NULL);
#endif
        }
        m_bDone = true;
    };
        

    bool WriteLog(char* msg, int len)
    {
       logme("in WriteLog: len=%d", len);
       return AppendMessage(msg, len, false);
    };
    bool WriteNewedLog(char* msg, int len)
    {
       logme("in WriteLog: len=%d", len);
       return AppendMessage(msg, len, true);
    };

private:

#ifdef _WIN32

    bool Write(const char* msg, int nlen)
    {
        logme("LogClient::Write[%d]", nlen);

        int nw = 0;
        int nTry = 1 + nlen /1000;
        static char* sError = "\r\nFailed to write a message\r\n";
        FileLock aLock(m_fdLock);

        while (nw < nlen)
        {
            // in msg mode
            DWORD nwrite = ((nlen-nw) > MAX_BUF_SIZE) ? MAX_BUF_SIZE : (nlen-nw);
            DWORD n = -1;
            BOOL ret = WriteFile(m_fdPipe, msg+nw, nwrite, &n, NULL);
            if (!ret && !m_bSilentOnError)
                fprintf(stderr, "Writemsg failed err=%d\n", GetLastError());
            logme("Write %d, errno=%d error=%s", n, errno, strerror(errno));
            if (n>0)
            {
                nw += n;
            }else if(!ret)
            {
                if (nTry-- >0)
                    continue;
                
                m_bOK = false;
                
                WriteFile(m_fdPipe, sError, (DWORD)strlen(sError), &n, NULL);
                
                return false;
            }
        }
        return true;
    };
    
#else

    bool Write(const char* msg, int nlen)
    {
        logme("LogClient::Write[%d]", nlen);

        int nw = 0;
        int nTry = 1 + nlen/512;
        static char* sError = "\nFailed to write a message\n";
        FileLock aLock(m_fdPipe);

        while (nw < nlen)
        {
            int nwrite = nlen-nw;
            int n = write(m_fdPipe, msg + nw, nwrite);
            logme("Write %d, errno=%d error=%s", n, errno, strerror(errno));
            if (n>0)
            {
                nw += n;
            }else if (n <= 0)
            {
                if (nTry-- > 0)
                {
                    sleep(1);
                    continue;
                }
                if (!m_bFailedWrite)
				{
                    fprintf(stderr, "Failed to write a log message: message length=%d written=%d error=%d %s\n",
                        nlen, nw, errno, strerror(errno));
                
					m_bFailedWrite = true;
				}

                write(m_fdPipe, sError, strlen(sError));

                m_bOK = false;
                
            }
        }
        return m_bOK;
    };
#endif

    bool AppendMessage(char* msg, int len, bool bFree = false)
    {
        // bFree == true : we should free msg after use; the caller has called new
        // but will not free.
        logme("In AppendMessage: len=%d #msgs=%d max=%d", len, m_nMessages, m_nMaxMessages);
        //if (m_nMessages >= m_nMaxMessages) return false;
        
        char *s;
        if (bFree)
        {
            s = msg;
        }else
        {
            s = new char[len];
            if (!s)
            {
                logme("In AppendMessage: new failed. n=%d");
                return false;
            }
            memcpy(s, msg, len);
        }
        MessageList *m = new MessageList(s, len);
        if (!m)
        {
            logme("In AppendMessage: new MessageList failed");
            delete[] s;
            return false;
        }
        
        //Need mutex
        m_mLock.Lock();
        m_msgTail->next = m;
        m_msgTail = m;
        m_nMessages++;
        m_mLock.Unlock();

        logme("AppendMessage: nmsg=%d", m_nMessages);
        return true;
    };
};

#ifdef _WIN32
void LogThread(void* arg)
{
    LogClient *lc = (LogClient*) arg;
    lc->DoLogWrite();

    _endthread();
}
#else
void * LogThread(void* arg)
{
    LogClient *lc = (LogClient*) arg;
    lc->DoLogWrite();
    return 0;
}

#endif //_WIN32

#endif //__JAXER_LOGCLIENT_H__


