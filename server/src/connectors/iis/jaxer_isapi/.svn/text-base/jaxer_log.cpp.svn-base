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
#include "jaxer_log.h"
#include <time.h>

// THIS MUST MATCH THE ENUM DEFS
const char *sLogLevel[] =
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

CJaxerLog::CJaxerLog(const char* logfile, int bkupfiles, int maxsizek, eLogLevel eDefault)
:m_bkupfiles(bkupfiles)
,m_maxsizek(maxsizek)
,m_fp(INVALID_HANDLE_VALUE)
,m_eDefault(eDefault)
{
    if(logfile && *logfile)
    {
        strcpy(m_logfile, logfile);
        // m_fp = fopen(m_logfile, "a");
        m_fp = CreateFile(m_logfile, FILE_APPEND_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    }else
    {
        m_logfile[0] = 0;
    }
}

CJaxerLog::~CJaxerLog(void)
{
    if (m_fp != INVALID_HANDLE_VALUE)
        CloseHandle(m_fp);
}

void CJaxerLog::SetNumBackups(int n)
{
    if (n>0 && n<=100)
        m_bkupfiles = n;
}

void CJaxerLog::SetBackUpSizeKB(int n)
{
    if (n<1)
        n = 1;
    m_maxsizek = n * 1024;
}

void CJaxerLog::SetLogLevel(eLogLevel e)
{
    m_eDefault = e;
}

bool CJaxerLog::SetLogFile(const char* logFile)
{
    if (m_fp != INVALID_HANDLE_VALUE)
        CloseHandle(m_fp);
    m_fp = INVALID_HANDLE_VALUE;

    strcpy(m_logfile, logFile);
    // m_fp = fopen(m_logfile, "a");
    m_fp = CreateFile(m_logfile, FILE_APPEND_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    return true;
}


void CJaxerLog::Log(eLogLevel eLevel, char* pszFormat, ...)
{
    if (eLevel < m_eDefault)
        return;

    if (!m_fp)
        return;

    char szBuffer[0x4000];
    char szTimeBuf[0x64];

    va_list arg_ptr;
    va_start(arg_ptr, pszFormat);
    vsprintf(szBuffer, pszFormat, arg_ptr);
    va_end(arg_ptr);

    time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

    sprintf(szTimeBuf,
        "%02d:%02d:%02d %02d/%02d/%04d [%s] ",
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
        timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_year+1900,
        sLogLevel[eLevel]);

    
    OVERLAPPED ovlp;
    memset(&ovlp, 0, sizeof(ovlp));
    LockFileEx(m_fp, LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &ovlp);
    DWORD nBytes;
    SetFilePointer(m_fp, 0, 0, FILE_END);
    WriteFile(m_fp, szTimeBuf, (DWORD)strlen(szTimeBuf), &nBytes, NULL);
    WriteFile(m_fp, szBuffer, (DWORD)strlen(szBuffer), &nBytes, NULL);
    WriteFile(m_fp, "\r\n", 2, &nBytes, NULL);
    UnlockFileEx(m_fp, 0, 1, 0, &ovlp);
}

void CJaxerLog::LogStr(eLogLevel eLevel, const char* pszStr, int len)
{
    if (eLevel < m_eDefault)
        return;

    if (!m_fp)
        return;

    time_t rawtime;
	struct tm * timeinfo;

    char szTimeBuf[0x64];

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

    sprintf(szTimeBuf,
        "%02d:%02d:%02d %02d/%02d/%04d [%s] ",
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
        timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_year+1900,
        sLogLevel[eLevel]);

    
    OVERLAPPED ovlp;
    memset(&ovlp, 0, sizeof(ovlp));
    LockFileEx(m_fp, LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &ovlp);
    DWORD nBytes;
    SetFilePointer(m_fp, 0, 0, FILE_END);
    WriteFile(m_fp, szTimeBuf, (DWORD)strlen(szTimeBuf), &nBytes, NULL);
    WriteFile(m_fp, pszStr, (len==-1)? (DWORD)strlen(pszStr) : len, &nBytes, NULL);
    WriteFile(m_fp, "\r\n", 2, &nBytes, NULL);
    UnlockFileEx(m_fp, 0, 1, 0, &ovlp);
}