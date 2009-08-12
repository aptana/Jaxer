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
#ifndef __LOGSERVERBASE_H__
#define __LOGSERVERBASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include "log.h"

#define DEFAULT_BACKUPS 10
#define DEFAULT_FILESIZE 0x500000

class LogServerBase
{
public:
    LogServerBase()
    :m_fdLog(INVALID_HANDLE_VALUE)
    ,m_fpLog(NULL)
    ,m_sLog(NULL)
    ,m_nBackups(DEFAULT_BACKUPS)
    ,m_nMaxFileSize(DEFAULT_FILESIZE)
    ,m_bPipeToProg(FALSE)
    ,m_sProgCmds(NULL)
    ,m_nCurrentFileSize(-1)
    {
    };

    ~LogServerBase()
    {
        logme("In LogServerBase::DTOR");
        if (m_fdLog != INVALID_HANDLE_VALUE)
            CloseHandle(m_fdLog);

        if (m_fpLog != NULL)
            _pclose(m_fpLog);

        if (m_sLog)
            delete[] m_sLog;
        if (m_sPipe)
            delete[] m_sPipe;
        delete[] m_sProgCmds;
    };

    void DisplayUsage(const TCHAR* argv0)
    {
        printf("Usage:\n"
            "%s --input=pipename --output=<|prog ...>\n"
            "or\n"
            "%s --input=pipename --output=logfile [--filesize=sizeBytes --numbackups=n]\n",
            argv0, argv0);
    };

    BOOL ParseCmds(int argc, TCHAR* argv[])
    {
        // --input=sPipeName
        // --output=|prog ...
        // --output=sLogFile
        // --filesize=m_nMaxFileSize
        // --numbackups=m_nBackups
        //
        int i;
        for(i=1; i<argc; i++)
        {
            TCHAR *p = argv[i];
            logme("LogServer:argv[%d]:>>>%s<<<", i, argv[i]);
            if (*argv[i]++ != '-' || *argv[i]++ != '-')
            {
                fprintf(stderr, "LogServer: invalid argument (%s)\n", p);
                DisplayUsage(argv[0]);
                logme("LogServer: invalid arg");
                return FALSE;
            }
            if (_tcsncmp("output=", argv[i], 7) == 0)
            {
                argv[i] += 7;
                if (*argv[i] == '|')
                {
                    TCHAR *p = argv[i]+1;
                    while(_istspace(*p))
                        p++;
                    if (!SetExternalCmd(p))
                    {
                        logme("LogServer: SetExternalCmd(%s) failed", p);
                        return FALSE;
                    }
                }else
                {
                    if (!SetLogFileName(argv[i]))
                    {
                        logme("LogServer: SetLogFileName(%s) failed", argv[i]);
                        return FALSE;
                    }
                }
            }else if (_tcsncmp("input=", argv[i], 6) == 0)
            {
                argv[i] += 6;
                if (!SetPipeName(argv[i]))
                {
                    logme("LogServer: SetPipeName(%s) failed", argv[i]);
                    return FALSE;
                }
            }else if (_tcsncmp("numbackups=", argv[i], 11) == 0)
            {
                argv[i] += 11;
                int n = _tstoi(argv[i]);
                if (n<2)
                {
                    fprintf(stderr, "LogServer: numbackups (%s) must have a value greater than 1\n", p);
                    logme("LogServer: numbackups wrong value %d", n);
                    return FALSE;
                }
                SetNumBackups(n);
            }else if (_tcsncmp("filesize=", argv[i], 9) == 0)
            {
                argv[i] += 9;
                int m = _tstoi(argv[i]);
                if (m<1)
                {
                    fprintf(stderr, "LogServer: filesize (%s) must have a value greater than 0\n", p);
                    logme("LogServer: filesize wrong value %d", m);
                    return FALSE;
                }
                SetMaxFileSize(m);
            }else
            {
                fprintf(stderr, "invalid argument (%s)\n", p);
                DisplayUsage(argv[0]);
                logme("LogServer: invalid argument --%s\n", argv[i]);
                return FALSE;
            }
        }

        //Make sure we have all we need
        const char *q = GetPipeName();
        if (!q || !*q)
        {
            fprintf(stderr, "LogServer: --input=pipename must be specified\n");
            logme("LogServer: --input=pipename not specified");
            return FALSE;
        }

        const TCHAR* p = GetExternalCmd();
        if (IsPipingToExternal() &&  (!p || !*p))
        {
            fprintf(stderr, "LogServer: --output=| missing cmds\n");
            logme("LogServer: --output=| missing cmds");
            return FALSE;
        }

        p = GetLogFileName();
        if (!IsPipingToExternal() && (!p || !*p))
        {
            fprintf(stderr, "LogServer: --output=logfile must be specified\n");
            logme("LogServer: --output=logfile must be specified");
            return FALSE;
        }

        logme("LogServer: ParseCmds OK");
        return TRUE;
    };


    virtual BOOL OpenLogForWrite() = 0;

    virtual BOOL WriteAll(void* vp, int n)
    {
        const char* p = (const char*) vp;
        BOOL bRet = TRUE;

        if (IsPipingToExternal())
        {
            if (m_fpLog == NULL)
                return FALSE;

            int nw = 0;
            while (nw < n)
            {

                size_t m;
                m = fwrite(p+nw, 1, n-nw, m_fpLog);
                if (m>0)
                {
                    logme("Write n=%d m=%d nw=%d", n, m, nw);
                    nw += m;
                }else
                {
                    logme("WriteAll n=%d failed: err=%d", n, 
#ifdef _WIN32
                        GetLastError());
#else
                        errno);
#endif
                    bRet = FALSE;
                    break;
                }
            }
            fflush(m_fpLog);
        }else
        {

            if (m_fdLog==INVALID_HANDLE_VALUE && !OpenLogForWrite())
                return FALSE;

            int nw = 0;
            while (nw < n)
            {

                DWORD m = -1;
        #ifdef _WIN32
                SetFilePointer(m_fdLog, 0, 0, FILE_END);
                bRet = WriteFile(m_fdLog, p+nw, n-nw, &m, NULL);
                if (bRet)
                {
                    nw += m;
                }else
                {
                    logme("WriteAll n=%d failed: err=%d", n, GetLastError());
                    bRet = FALSE;
                    break;
                }
        #else
                m = write(m_fdLog, p+nw, n-nw);
                if (m > 0)
                {
                    nw += m;
                }else
                {
                    logme("WriteAll n=%d failed: err=%d (%s)", n, errno, strerror(errno));
                    bRet = FALSE;
                    break;
                }
        #endif
            }

            //logme("WriteAll: n=%d written=%d", n, nw);

            time_t now;
            time(&now);
            if (m_nCurrentFileSize<0)
            {
                if(now - m_timeLast > 3600)
                {
                    UpdateLogFileSize();
                    time(&m_timeLast);
                }
            }else
            {
                m_nCurrentFileSize += nw;
            }
            
            if (m_nCurrentFileSize > m_nMaxFileSize)
            {
                if (DoLogFileRotation())
                    m_nCurrentFileSize = 0;
                else
                {
                    m_nCurrentFileSize = -1;
                    time(&m_timeLast);
                }
            }
        }

        return bRet;
    };

    int GetNumBackups() { return m_nBackups;};
    void SetNumBackups(int n) { if (n>1) m_nBackups = n;};

    int GetMaxFileSize() { return m_nMaxFileSize;};
    void SetMaxFileSize(int n) { if (n>0) m_nMaxFileSize = n;};

    int IsPipingToExternal() { return m_bPipeToProg;};

    BOOL SetPipeName(TCHAR* s)
    {
        delete[] m_sPipe;
        m_sPipe = new char[_tcslen(s) + 1];
        if (!m_sPipe)
        {
            fprintf(stderr, "LogServer: out of memory when allocating memory for SetPipeName\n");
            return FALSE;
        }
        for(size_t i=0; i<=_tcslen(s); i++)
            m_sPipe[i] = s[i];
        return TRUE;
    };

    BOOL SetLogFileName(TCHAR* s)
    {
        delete[] m_sLog;
        m_sLog = new TCHAR[_tcslen(s) + 1];
        if (!m_sLog)
        {
            fprintf(stderr, "LogServer: out of memory when allocating memory for SetLogFileName\n");
            return FALSE;
        }
        _tcscpy(m_sLog, s);
        return TRUE;
    };

    BOOL UpdateLogFileSize()
    {
        BOOL ret = TRUE;
    #ifdef _WIN32
        BY_HANDLE_FILE_INFORMATION FileInformation;
        ret = GetFileInformationByHandle(m_fdLog, &FileInformation);
        if (!ret)
        {
            m_nCurrentFileSize = -1;
            printf("LogServer: cannot get fileinfo.  Logfile rotation will delay\n");
            logme("LogServer: cannot get fileinfo. (err=%d)  Logfile rotation will delay\n", GetLastError());
        }else
        {
            m_nCurrentFileSize = FileInformation.nFileSizeLow;
        }
    #else
        m_nCurrentFileSize = lseek(m_fdLog, 0, SEEK_END);
        ret = (m_nCurrentFileSize != -1);
    #endif
        time(&m_timeLast); 
        return ret;
    };


    BOOL SetExternalCmd(TCHAR* cmd)
    {
        delete[] m_sProgCmds;
        m_sProgCmds = new TCHAR[_tcslen(cmd) + 1];
        if (!m_sProgCmds)
        {
            fprintf(stderr, "LogServer: out of memory when allocating memory for ExternalCmd\n");
            return FALSE;
        }
        _tcscpy(m_sProgCmds, cmd);
        m_bPipeToProg = true;
        return TRUE;
    };

    const TCHAR* GetExternalCmd() {return m_sProgCmds;};

    const TCHAR* GetLogFileName() { return m_sLog;};

    const char* GetPipeName() { return m_sPipe;};
    BOOL DoLogFileRotation()
    {
#ifdef _WIN32
        if (!CloseHandle(m_fdLog))
#else
        if (close(m_fdLog))
#endif
        {
            char *p = "LogServer: CloseHandle failed -- cannot perform logfile rotation\r\n";
    #ifdef _WIN32
            DWORD nw;
            WriteFile(m_fdLog, p, strlen(p), &nw, NULL);
    #else
            write(m_fdLog, p, strlen(p));
    #endif
            return FALSE;
        }

        char *sLogfilePath = m_sLog;

        char sBackupLogFile[PATH_MAX+10];
        time_t oldest;
        unsigned nId = 0;
        size_t n1=0;

        char *pdot = strrchr(sLogfilePath, '.');
        if (pdot)
            n1 = pdot-sLogfilePath;
        
        for (int i=1; i<=m_nBackups; i++)
        {
            if (pdot)
            {
                strncpy(sBackupLogFile, sLogfilePath, n1);
                sprintf(sBackupLogFile+n1, "%d%s", i, pdot);
            }else
            {
                sprintf(sBackupLogFile, "%s.%d", sLogfilePath, i);
            }

    #ifdef _WIN32
            struct _stat attrib;
            if (_stat(sBackupLogFile, &attrib) != 0)
    #else
            struct stat attrib;
            if (stat(sBackupLogFile, &attrib) != 0)
    #endif
            {
                nId = i;
                break;
            }
            if (nId == 0 || attrib.st_mtime < oldest)
            {
                oldest = attrib.st_mtime;
                nId = i;
            }
        }

        if (pdot)
        {
            strncpy(sBackupLogFile, sLogfilePath, n1);
            sprintf(sBackupLogFile+n1, "%d%s", nId, pdot);
        }else
        {
            sprintf(sBackupLogFile, "%s.%d", sLogfilePath, nId);
        }
        
        remove(sBackupLogFile);
        int ret = rename(sLogfilePath, sBackupLogFile);

        if( !OpenLogForWrite())
        {
            return FALSE;
        }

        if (ret != 0)
        {
            #ifdef _WIN32
            char *p = "Could not rename ";
            DWORD m;

            WriteFile(m_fdLog, p, strlen(p), &m, NULL);
            WriteFile(m_fdLog, sLogfilePath, strlen(sLogfilePath), &m, NULL);
            p = " to ";
            WriteFile(m_fdLog, p, strlen(p), &m, NULL);
            WriteFile(m_fdLog, sBackupLogFile, strlen(sBackupLogFile), &m, NULL);
            p = ", log rotation failed\r\n";
            WriteFile(m_fdLog, p, strlen(p), &m, NULL);
            #else
            char *p = "Could not rename ";

            write(m_fdLog, p, strlen(p));
            write(m_fdLog, sLogfilePath, strlen(sLogfilePath));
            p = " to ";
            write(m_fdLog, p, strlen(p));
            write(m_fdLog, sBackupLogFile, strlen(sBackupLogFile));
            p = ", log rotation failed\n";
            write(m_fdLog, p, strlen(p));
            #endif
            return FALSE;
        }
        return TRUE;
    };

public:
    HANDLE m_fdLog;
    FILE*  m_fpLog;
    TCHAR* m_sLog;
    char * m_sPipe;
    time_t m_timeLast;

    int    m_nBackups;
    int    m_nMaxFileSize;
    BOOL   m_bPipeToProg;
    TCHAR* m_sProgCmds;
    int    m_nCurrentFileSize;
};
  
#endif // __LOGSERVERBASE_H__

