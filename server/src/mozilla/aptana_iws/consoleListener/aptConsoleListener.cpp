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
#include "nsString.h"
#include "aptConsoleListener.h"
#include "nsIConsoleService.h"
#include "aptIEventLog.h"
#include "aptEventTypeManager.h"
#include "aptEventNames.h"
#include "nsServiceManagerUtils.h"
#include "nsIOutputStream.h"
#include "nsIScriptError.h"
#include "prio.h"
#include "nsNetUtil.h"
#include "nsReadableUtils.h"
#include "nsIFileURL.h"
#include "nsISupportsPrimitives.h"
#include "nspr.h"
#include "prlog.h"
#include "prthread.h"
#include "aptCoreTrace.h"

#ifdef _WIN32

//#include <windows.h>
//#undef GetMessage

//static DWORD g_pid = 0;

#else

#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <errno.h>

// static int g_fd = -1;
//static int g_pid = 0;

#endif

#if 0
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
#endif
////////////////////////////////////////////////////////////////////////

//static NS_DEFINE_IID(kISupportsIID, NS_ISUPPORTS_IID);
//static NS_DEFINE_IID(kIObserverIID, NS_IOBSERVER_IID);
//static NS_DEFINE_IID(knsIConsoleListenerIID, NS_ICONSOLELISTENER_IID);

static PRThread *g_mainThread;

extern aptCoreTrace gJaxerLog;

aptConsoleListener::aptConsoleListener()
{
	g_mainThread = PR_GetCurrentThread();
}

aptConsoleListener::~aptConsoleListener()
{
}

#if 0
NS_IMPL_ADDREF(aptConsoleListener)
NS_IMPL_RELEASE(aptConsoleListener)

NS_INTERFACE_MAP_BEGIN(aptConsoleListener)
    NS_INTERFACE_MAP_ENTRY(nsIConsoleListener)
NS_INTERFACE_MAP_END
#else
NS_IMPL_THREADSAFE_ISUPPORTS1(aptConsoleListener, nsIConsoleListener)
#endif

NS_IMETHODIMP
aptConsoleListener::Observe(nsIConsoleMessage *aMessage)
{
	nsAdoptingString traceMessage;
	nsresult rv;
    
	// Get message text.
	aMessage->GetMessageMoz(getter_Copies(traceMessage));
	nsCOMPtr<nsIScriptError> scriptError(do_QueryInterface(aMessage));
	if ((PR_GetCurrentThread() == g_mainThread)
		&& scriptError) {
		PRUint32 flags;
		rv = scriptError->GetFlags(&flags);
		if (NS_SUCCEEDED(rv) && !(flags & nsIScriptError::warningFlag)) {
			nsCOMPtr<aptIEventLog> el = do_CreateInstance(APT_EVENT_LOG_CONTRACTID);
			nsCOMPtr<aptIEventTypeManager> etm = do_GetService(APT_EVENTTYPEMANAGER_CONTRACTID, &rv);
			el->Init();
			el->SetMessage(traceMessage);
			etm->FireEvent(el, aptEventName_Log);
		}
	}

    gJaxerLog.Log(eERROR, "%s", NS_ConvertUTF16toUTF8(traceMessage).get());

	return NS_OK;
}

nsresult
aptConsoleListener::Register()
{
	// nsresult rv;
	nsCOMPtr<nsIConsoleService> cs(do_GetService(NS_CONSOLESERVICE_CONTRACTID));
    AddRef();
    cs->RegisterListener(this);

	return NS_OK;
}

nsresult
aptConsoleListener::Unregister()
{
  nsCOMPtr<nsIConsoleService> cs(do_GetService(NS_CONSOLESERVICE_CONTRACTID));
  cs->UnregisterListener(this);
  //Release();
  return NS_OK;
}

