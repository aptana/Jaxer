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

#include "nsCOMPtr.h"
#include "nsIClassInfoImpl.h"
#include "nsComponentManagerUtils.h"
#include "nsString.h"
#include "nsSupportsArray.h"
#include "aptCoreLog.h"
#include "aptCoreTrace.h"
#include "../JaxerLog/log.h"

extern aptCoreTrace gJaxerLog;

static void LogHelper(PRBool bForce, eLogLevel eLevel, const nsACString& message, const char *from = "JS Framework")
{
	if (bForce || (eLevel >= gJaxerLog.GetCoreLogLevel())) {
		gJaxerLog.LogF(eLevel, PromiseFlatCString(message).get(), from);
	}
}

aptCoreLog::aptCoreLog()
: mForceTrace(PR_FALSE),
  mListening(PR_FALSE),
  mLock(nsnull)
{
	mLock = PR_NewLock();
}

aptCoreLog::~aptCoreLog()
{
	if (mLock)
		PR_DestroyLock(mLock);
}

NS_IMPL_ISUPPORTS1_CI(aptCoreLog, aptICoreLog)

/* attribute boolean forceTrace; */
NS_IMETHODIMP aptCoreLog::GetForceTrace(PRBool *aForceTrace)
{
	NS_ENSURE_ARG_POINTER(aForceTrace);
	*aForceTrace = mForceTrace;
    return NS_OK;
}
NS_IMETHODIMP aptCoreLog::SetForceTrace(PRBool aForceTrace)
{
	mForceTrace = aForceTrace;
    return NS_OK;
}

/* void log (in long type, in ACString from, in ACString message); */
NS_IMETHODIMP aptCoreLog::Log(PRInt32 type, const nsACString & from, const nsACString & message)
{
	NotifyListeners(type, message);
	LogHelper(mForceTrace, (eLogLevel)type, message, PromiseFlatCString(from).get());
    return NS_OK;
}

/* void fatal (in AString message); */
NS_IMETHODIMP aptCoreLog::Fatal(const nsACString & message)
{
	NotifyListeners(eFATAL, message);
	LogHelper(mForceTrace, (eLogLevel)eFATAL, message);
    return NS_OK;
}

/* void error (in AString message); */
NS_IMETHODIMP aptCoreLog::Error(const nsACString & message)
{
	NotifyListeners(eERROR, message);
	LogHelper(mForceTrace, (eLogLevel)eERROR, message);
    return NS_OK;
}

/* void warning (in AString message); */
NS_IMETHODIMP aptCoreLog::Warning(const nsACString & message)
{
	NotifyListeners(eWARN, message);
	LogHelper(mForceTrace, (eLogLevel)eWARN, message);
    return NS_OK;
}

/* void info (in AString message); */
NS_IMETHODIMP aptCoreLog::Info(const nsACString & message)
{
	NotifyListeners(eINFO, message);
	LogHelper(mForceTrace, (eLogLevel)eINFO, message);
    return NS_OK;
}

/* void debug (in AString message); */
NS_IMETHODIMP aptCoreLog::Debug(const nsACString & message)
{
	NotifyListeners(eDEBUG, message);
	LogHelper(mForceTrace, (eLogLevel)eDEBUG, message);
    return NS_OK;
}

/* void trace (in AString message); */
NS_IMETHODIMP aptCoreLog::Trace(const nsACString & message)
{
	NotifyListeners(eTRACE, message);
	LogHelper(mForceTrace, (eLogLevel)eTRACE, message);
	return NS_OK;
}

/* void logString (in AString message); */
NS_IMETHODIMP aptCoreLog::LogString(const nsACString & message)
{
	NotifyListeners(eNOTICE, message);
	LogHelper(mForceTrace, (eLogLevel)eNOTICE, message);
    return NS_OK;
}

/* void registerListener (in aptICoreLogListener listener); */                                                                                          
NS_IMETHODIMP
aptCoreLog::RegisterListener(aptICoreLogListener *listener)                                                                                
{
	nsAutoLock lock(mLock);
	nsISupportsKey key(listener);
	
	if (!mListeners.Exists(&key))
		mListeners.Put(&key, listener);
    return NS_OK;
}                                                                                                                                                       

/* void unregisterListener (in aptICoreLogListener listener); */                                                                                        
NS_IMETHODIMP
aptCoreLog::UnregisterListener(aptICoreLogListener *listener)                                                                              
{
	nsAutoLock lock(mLock);
	
    nsISupportsKey key(listener);
    mListeners.Remove(&key);
    return NS_OK;
}

static PRBool PR_CALLBACK snapshot_enum_func(nsHashKey *key, void *data, void* closure)
{
    nsISupportsArray *array = (nsISupportsArray *)closure;
	
    // Copy each element into the temporary nsSupportsArray...
    array->AppendElement((nsISupports*)data);
    return PR_TRUE;
}

void
aptCoreLog::NotifyListeners(PRInt32 type, const nsACString& message)
{
	nsSupportsArray listenersSnapshot;
	{
		nsAutoLock lock(mLock);
		mListeners.Enumerate(snapshot_enum_func, &listenersSnapshot);
	}
	
	nsresult rv;
	PRUint32 snapshotCount;
	nsCOMPtr<aptICoreLogListener> listener;
    rv = listenersSnapshot.Count(&snapshotCount);
	NS_ENSURE_SUCCESS(rv, );
	NS_ENSURE_TRUE(snapshotCount, );
	
	{
        nsAutoLock lock(mLock);
        NS_ENSURE_TRUE(!mListening, );
        mListening = PR_TRUE;
    }

	for (PRUint32 i = 0; i < snapshotCount; i++) {
        rv = listenersSnapshot.GetElementAt(i, getter_AddRefs(listener));
        if (NS_FAILED(rv)) {
            break;
        }
        listener->Observe(type, message);
    }
    
    {
        nsAutoLock lock(mLock);
        mListening = PR_FALSE;
    }
}
