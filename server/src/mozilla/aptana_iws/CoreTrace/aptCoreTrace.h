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

#ifndef _aptCoreTrace_H_
#define _aptCoreTrace_H_

#include "nsCOMPtr.h"
#include "../JaxerLog/log.h"

class nsIPrefBranch;
struct PRMonitor;
class LogClient;
    
////////////////////////////////////////////////////////////////////////////////

class aptCoreTrace //: public nsIConsoleMessage
{
public:
	aptCoreTrace();

    nsresult Init(const char* sLock);
    void Terminate();

    // Use these for core
    void Log(eLogLevel eLevel, const char* fmt, ...);

    void LogBytes(eLogLevel eLevel, const char* data, size_t len);
    void LogString(eLogLevel eLevel, const char* str);

    // Use this for Framework, always log
    void LogF(eLogLevel eLevel, const char* str, const char* from = "JS Framework");

    // void LogString(const PRUnichar* message);

    eLogLevel GetCoreLogLevel() { return mCoreLogLevel;}
    // void SetCoreLogLevel(eLogLevel eLevel) { mCoreLogLevel = eLevel;}
    void UpdatePrefSettings(nsIPrefBranch *aPrefBranch, const char* pref = nsnull);

	PRBool GetDumpEnabled() { return (mDumpEnabled && mCanLog); };

	~aptCoreTrace();

private:
    eLogLevel   mCoreLogLevel;
    PRBool      mCanLog;
	PRBool      mDumpEnabled;
    PRInt32     mPid;
    LogClient*  mLC;
    PRMonitor*  mLoggerLock;
    char*       mBuf;
    PRUint32    mBufSize;
};

#endif
