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

#include "nsServiceManagerUtils.h"
#include "nsISupportsUtils.h"

#include "PrefObserver.h"
#include "aptCoreTrace.h"
#include "FCGXStream.h"
#include "HTTPStream.h"
#include "aptDocumentFetcherService.h"

extern aptCoreTrace gJaxerLog;

const char *kJaxerAll = "Jaxer.";
//const char *kJaxerCore = "Jaxer.Core.";
//const char *kJaxerDev = "Jaxer.dev.";

PrefObserver *PrefObserver::sPrefObserver = nsnull;

PrefObserver::PrefObserver()
{
    nsresult rv = NS_OK;
    mPrefBranch = do_GetService(NS_PREFSERVICE_CONTRACTID, &rv);
    NS_ASSERTION(mPrefBranch, "where is the pref service?");
}

PrefObserver::~PrefObserver()
{
}

NS_IMPL_ADDREF(PrefObserver)
NS_IMPL_RELEASE(PrefObserver)

NS_INTERFACE_MAP_BEGIN(PrefObserver)
    NS_INTERFACE_MAP_ENTRY(nsIObserver)
    NS_INTERFACE_MAP_ENTRY(nsISupportsWeakReference)
NS_INTERFACE_MAP_END

/* void observe (in nsISupports aSubject, in string aTopic, in wstring aData); */
NS_IMETHODIMP PrefObserver::Observe(nsISupports *aSubject, const char *aTopic, const PRUnichar *aData)
{
    if (strcmp(NS_PREFBRANCH_PREFCHANGE_TOPIC_ID, aTopic) != 0)
    {
        return NS_OK;
    }

    nsresult rv;
    nsCOMPtr<nsIPrefBranch> prefBranch = do_QueryInterface(aSubject, &rv);
    if (NS_FAILED(rv))
        return rv;

    NS_ConvertUTF16toUTF8 pref(aData);
    gJaxerLog.Log(eDEBUG, "PrefObserver::Observe: pref: %s", pref.get());
    if (strncmp(kJaxerAll, pref.get(), strlen(kJaxerAll)) == 0)
    {
        Sync(prefBranch, pref.get());
    }

    return NS_OK;
}

nsresult
PrefObserver::Subscribe()
{
    NS_ENSURE_TRUE(mPrefBranch, NS_ERROR_FAILURE);

    mPrefBranch->AddObserver(kJaxerAll, this, PR_TRUE);
    //mPrefBranch->AddObserver(kJaxerDev, this, PR_TRUE);
    
    Sync(mPrefBranch, nsnull);

    return NS_OK;
}

nsresult
PrefObserver::Unsubscribe()
{
    NS_ENSURE_TRUE(mPrefBranch, NS_ERROR_FAILURE);

    mPrefBranch->RemoveObserver(kJaxerAll, this);
    //mPrefBranch->RemoveObserver(kJaxerDev, this);
    
    return NS_OK;
}

void PrefObserver::Sync(nsIPrefBranch *aPrefBranch, const char* pref)
{
    NS_ASSERTION(aPrefBranch, "no pref branch");
    if (!aPrefBranch)
    {
        return;
    }

    //FCGXStream::UpdatePrefSettings(aPrefBranch, pref);
    gJaxerLog.UpdatePrefSettings(aPrefBranch, pref);

    nsresult rv;
    nsCOMPtr<aptIDocumentFetcherService> dfs = do_GetService("@aptana.com/httpdocumentfetcher;1", &rv);
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eWARN, "Cannot get aptIDocumentFetcherService. Some prefs may not be updated.");
    }else
    {
        rv = dfs->Init();
        if (NS_FAILED(rv))
        {
            gJaxerLog.Log(eWARN, "Cannot get initialize aptIDocumentFetcherService. Some prefs may not be updated.");
        }else
            dfs->UpdatePrefSettings(aPrefBranch, pref);
    }


#if 0
    PRInt32 nLevel = (PRInt32) eINFO;
    eLogLevel eLevel = eINFO;
        
    aPrefBranch->GetIntPref("Jaxer.Core.LogLevel", &nLevel);
    gJaxerLog.Log(eDEBUG, "PrefObserver::Sync: got Jaxer.Core.LogLevel=%d", nLevel);
    if (nLevel >= (PRInt32) eTRACE && nLevel <= (PRInt32) eFATAL)
        eLevel = (eLogLevel) nLevel;

    gJaxerLog.Log(eDEBUG, "PrefObserver::Sync: Setting LogLevel to %d", (PRInt32) eLevel);
    gJaxerLog.SetCoreLogLevel(eLevel);
#endif
}

///////////////////////////////////////////////////////////////////////////////
nsresult PrefObserver::InitPrefObserver()
{
    if (!PrefObserver::sPrefObserver)
    {
        PrefObserver::sPrefObserver = new PrefObserver();
        if (!PrefObserver::sPrefObserver)
        {
            return NS_ERROR_FAILURE;
        }
        PrefObserver::sPrefObserver->AddRef();
        PrefObserver::sPrefObserver->Subscribe();
    }
    return NS_OK;
}

void PrefObserver::ReleasePrefObserver()
{
    if (PrefObserver::sPrefObserver)
    {
        PrefObserver::sPrefObserver->Unsubscribe();
        PrefObserver::sPrefObserver->Release();
        PrefObserver::sPrefObserver = nsnull;
    }
}

