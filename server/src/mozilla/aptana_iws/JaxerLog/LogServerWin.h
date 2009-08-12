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
#ifndef __LOGSERVER_H__
#define __LOGSERVER_H__

#include "LogServer.h"

class CMutex
{
    CRITICAL_SECTION m_cs;

public:
    CMutex() {InitializeCriticalSection(&m_cs);}
    ~CMutex() {DeleteCriticalSection(&m_cs);}
    void Lock() {EnterCriticalSection(&m_cs);}
    void Unlock() {LeaveCriticalSection(&m_cs);}
};

class LogServer : public LogServerBase
{
public:
    LogServer();
    ~LogServer();

    //void DisplayUsage(const TCHAR* argv0);
    //BOOL ParseCmds(int argc, TCHAR* argv[]);

    BOOL Init();
    void WriteLog(const TCHAR* severity, const TCHAR* msg, int len);
    BOOL WriteAll(void* p, int len);
    //int GetNumBackups() { return m_nBackups;};
    //void SetNumBackups(int n) { if (n>1) m_nBackups = n;};

    //int GetMaxFileSize() { return m_nMaxFileSize;};
    //void SetMaxFileSize(int n) { if (n>0) m_nMaxFileSize = n;};

    //int IsPipingToExternal() { return m_bPipeToProg;};
#if 0
    BOOL SetExternalCmd(TCHAR* cmd)
    {
        delete[] m_sProgCmds;
        m_sProgCmds = new TCHAR[_tcslen(cmd) + 1];
        if (!m_sProgCmds) return FALSE;
        _tcscpy(m_sProgCmds, cmd);
        m_bPipeToProg = true;
        return TRUE;
    };

    const TCHAR* GetExternalCmd() {return m_sProgCmds;};

    BOOL SetLogFileName(TCHAR* s);
    const CHAR* GetLogFileName() { return m_sLog;};

    BOOL SetPipeName(TCHAR* s);
    const char* GetPipeName() { return m_sPipe;};
    BOOL DoLogFileRotation();
    BOOL UpdateLogFileSize();
#endif
    BOOL OpenLogForWrite();
    BOOL CreateLogPipe();


private:
    
    //HANDLE m_fdLog;
    //TCHAR* m_sLog;
    //char*  m_sPipe;
    CMutex m_mutex;
    //time_t m_timeLast;
    //BOOL   m_bHasRotationError;

    //int    m_nBackups;
    //int    m_nMaxFileSize;
    //BOOL   m_bPipeToProg;
    //TCHAR* m_sProgCmds;
    //int    m_nCurrentFileSize;
};
  
#endif // __LOGSERVER_H__

