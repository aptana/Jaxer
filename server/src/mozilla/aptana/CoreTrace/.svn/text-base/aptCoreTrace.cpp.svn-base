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

#include "aptCoreTrace.h"
#include "nsIPrefService.h"
#include "nsServiceManagerUtils.h"
#include "nsIConsoleService.h"
#include "prprf.h"
#include "nsString.h"
#include "nspr.h"
#include "../JaxerLog/logclient.h"


////////////////////////////////////////////////////////////////////////////////

// This class/interface is not intended for QI/CI, only for regular server code to use.
aptCoreTrace::aptCoreTrace()
: mBuf(nsnull)
, mLoggerLock(nsnull)
, mCoreLogLevel(eINFO)
, mCanLog(PR_FALSE)
, mPid(0)
, mLC(nsnull)
{
    mCoreLogLevel = eINFO;
    mCanLog = PR_FALSE;
}

aptCoreTrace::~aptCoreTrace()
{
    if (mBuf) {
        PR_Free(mBuf);
        mBuf = nsnull;
    }
    if (mLoggerLock)
    {
        PR_DestroyMonitor(mLoggerLock);
    }
    if(mLC)
      delete mLC;
}

static const char *kJaxerCoreLogLevel = "Jaxer.Core.LogLevel";
static const char *kDumpProtocol = "Jaxer.dev.DumpProtocol";

void aptCoreTrace::UpdatePrefSettings(nsIPrefBranch *aPrefBranch, const char* pref)
{  
    if (!pref) {
		aPrefBranch->GetBoolPref(kDumpProtocol, &mDumpEnabled);
        nsAdoptingCString sLevel;
        nsresult rv = aPrefBranch->GetCharPref(kJaxerCoreLogLevel, getter_Copies(sLevel));
        if (NS_SUCCEEDED(rv)) {
            if (sLevel.Equals("TRACE"))
                mCoreLogLevel = eTRACE;
            else if (sLevel.Equals("DEBUG"))
                mCoreLogLevel = eDEBUG;
            else if (sLevel.Equals("INFO"))
                mCoreLogLevel = eINFO;
            else if (sLevel.Equals("WARN"))
                mCoreLogLevel = eWARN;
            else if (sLevel.Equals("ERROR"))
                mCoreLogLevel = eERROR;
            else if (sLevel.Equals("FATAL"))
                mCoreLogLevel = eFATAL;
            else
                Log(eWARN, "Invalid value (%s) for preference %s.  LogLevel not changed.", sLevel.get(), kJaxerCoreLogLevel);
        }else
            Log(eWARN, "Get pref value for %s failed (rv=0x%x)", kJaxerCoreLogLevel, rv);

    }else if (strcmp(pref, kJaxerCoreLogLevel) == 0) {
        nsAdoptingCString sLevel;
        nsresult rv = aPrefBranch->GetCharPref(kJaxerCoreLogLevel, getter_Copies(sLevel));
        if (NS_SUCCEEDED(rv)) {
            if (sLevel.Equals("TRACE"))
                mCoreLogLevel = eTRACE;
            else if (sLevel.Equals("DEBUG"))
                mCoreLogLevel = eDEBUG;
            else if (sLevel.Equals("INFO"))
                mCoreLogLevel = eINFO;
            else if (sLevel.Equals("WARN"))
                mCoreLogLevel = eWARN;
            else if (sLevel.Equals("ERROR"))
                mCoreLogLevel = eERROR;
            else if (sLevel.Equals("FATAL"))
                mCoreLogLevel = eFATAL;
            else
                Log(eWARN, "Invalid value (%s) for preference %s.  LogLevel not changed.", sLevel.get(), kJaxerCoreLogLevel);
        }else
            Log(eWARN, "Get pref value for %s failed (rv=0x%x)", kJaxerCoreLogLevel, rv);

    }else if (strcmp(pref, kDumpProtocol) == 0) {
        aPrefBranch->GetBoolPref(kDumpProtocol, &mDumpEnabled);
    }
}

nsresult aptCoreTrace::Init(const char* sLock)
{
    if (mCanLog) return NS_OK;
    
    mLC = new LogClient();
    if (!mLC)
      return NS_ERROR_OUT_OF_MEMORY;

    const char* logfifo = getenv("JAXERLOG_PIPENAME");
    mLC->Init(logfifo, -1, sLock);
    mLC->OpenPipeForWrite();
#ifdef _WIN32
    mPid = GetCurrentProcessId();
#else
    mPid = getpid();
#endif
    
    mLoggerLock = PR_NewMonitor();

    mBuf = (char *)PR_Malloc(0x8000);
    mBufSize = 0x8000;

    mCanLog = PR_TRUE;

    nsCOMPtr<nsIPrefBranch> prefBranch(do_GetService(NS_PREFSERVICE_CONTRACTID));

    if (prefBranch)
        UpdatePrefSettings(prefBranch, nsnull);

    return NS_OK;
}

void aptCoreTrace::Terminate()
{
    mLC->Terminate();
}

void aptCoreTrace::Log(eLogLevel eLevel, const char* fmt, ...)
{
    static const int nEOL = 2;
    if (!mCanLog) return;

    if (eLevel < mCoreLogLevel) return;
    
	PRTime curTime = PR_Now();
	PRExplodedTime localTime;
	PR_ExplodeTime(curTime, PR_LocalTimeParameters, &localTime);

	PR_EnterMonitor(mLoggerLock);

    PRUint32 nWritten = PR_snprintf(mBuf, mBufSize-nEOL,
			"%02d:%02d:%02d %02d/%02d/%04d [%6d] [%s] ", // XXX i18n!
			localTime.tm_hour, localTime.tm_min, localTime.tm_sec,
			localTime.tm_month+1, localTime.tm_mday, localTime.tm_year,
			mPid, gsLogLevel[eLevel]);


    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    PRUint32 nExtra = PR_vsnprintf(mBuf+nWritten, mBufSize-nWritten-nEOL, fmt, arg_ptr);
    va_end(arg_ptr);

    if (nExtra == (PRUint32) (-1))
    {
        //failed?
        PR_ExitMonitor(mLoggerLock);
        return;
    }


    nWritten += nExtra;

#ifdef _WIN32
    mBuf[nWritten++] = '\r';
#endif
    mBuf[nWritten++] = '\n';
    
    mLC->WriteLog(mBuf, nWritten);
    PR_ExitMonitor(mLoggerLock);
}

void aptCoreTrace::LogBytes(eLogLevel eLevel, const char* data, size_t len)
{
    // Extra spaces for Date, PID, EOL etc
#ifdef _WIN32
    static const int nEXTRA = 34;
#else
    static const int nEXTRA = 33;
#endif
    if (!mCanLog) return;

    if (eLevel < mCoreLogLevel) return;
    
	PRTime curTime = PR_Now();
	PRExplodedTime localTime;
	PR_ExplodeTime(curTime, PR_LocalTimeParameters, &localTime);
    int nTotal = len + nEXTRA + strlen(gsLogLevel[eLevel]);

    char *buf = new char[nTotal];
    if (!buf) return;

    PRUint32 nWritten = PR_snprintf(buf, nTotal,
			"%02d:%02d:%02d %02d/%02d/%04d [%6d] [%s] ", // XXX i18n!
			localTime.tm_hour, localTime.tm_min, localTime.tm_sec,
			localTime.tm_month+1, localTime.tm_mday, localTime.tm_year,
			mPid, gsLogLevel[eLevel]);


    memcpy(buf+nWritten, data, len);

    nWritten += len;

#ifdef _WIN32
    buf[nWritten++] = '\r';
#endif
    buf[nWritten++] = '\n';
    
#ifdef _DEBUG
    if (nWritten != nTotal)
    {
        char tmp[50];
        sprintf(tmp, "nW=%d nT=%d\n", nWritten, nTotal);
        mLC->WriteLog(tmp, strlen(tmp));
    }
#endif

    mLC->WriteNewedLog(buf, nWritten);
}

void aptCoreTrace::LogString(eLogLevel eLevel, const char* str)
{
    LogBytes(eLevel, str, strlen(str));
}


//Framework logging - always log
void aptCoreTrace::LogF(eLogLevel eLevel, const char* str, const char* from)
{
    if (!mCanLog) return;
    size_t nlen = strlen(str) + 100;
    char *msg = new char[nlen];

    if (!msg)
    {
        // no memory
        return;
    }

    PRTime curTime = PR_Now();
	PRExplodedTime localTime;
	PR_ExplodeTime(curTime, PR_LocalTimeParameters, &localTime);

	PRUint32 nWritten = PR_snprintf(msg, nlen,
			"%02d:%02d:%02d %02d/%02d/%04d [%6d] [%s] [%s] %s%s", // XXX i18n!
			localTime.tm_hour, localTime.tm_min, localTime.tm_sec,
			localTime.tm_month+1, localTime.tm_mday, localTime.tm_year,
			mPid, gsLogLevel[eLevel], from, str,
#ifdef _WIN32
   
            "\r\n"
#else
            "\n"
#endif
            );
   
    mLC->WriteNewedLog(msg, nWritten);
}

