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
#include "nsIInterfaceRequestorUtils.h"
#include "nsThreadUtils.h"
#include "nsIWebNavigation.h"
#include "nsIWebProgress.h"
#include "nsIDocumentLoader.h"
#include "nsIDOMWindow.h"
#include "nsIDocument.h"
#include "nsIDOMDocument.h"
#include "nsIWeakReference.h"
#include "nsIDocShellLoadInfo.h"
#include "nsIDOMEvent.h"
#include "nsIHttpChannel.h"
#include "jsapi.h"
#include "nsIJSContextStack.h"
#include "nsEventDispatcher.h"
#include "nsServiceManagerUtils.h"
#include "nsIPrivateDOMEvent.h"
#include "nsDOMJSUtils.h"
#include "jxWindow.h"

#define LOAD_STR "load"
#define ERROR_STR "error"

class jxWindowBadCertHandler : public aptIBadCertHandler
	{
	public:
		jxWindowBadCertHandler(jxWindow *target);
		virtual ~jxWindowBadCertHandler();
		
		/* additional members */
		NS_DECL_ISUPPORTS
		NS_DECL_APTIBADCERTHANDLER
		
	private:
		jxWindow* mTarget;
	};

jxWindow::jxWindow():
	mOnLoadListener(nsnull),
	mOnErrorListener(nsnull),
	mOnSSLCertErrorListener(nsnull),
	mOnLocationChangeListener(nsnull),
	mNotificationCallbacks(nsnull),
	mScriptContext(nsnull),
	mState(UNINITIALIZED),
	mRedirect(PR_FALSE),
    mAsync(PR_FALSE),
    mAllowSubframes(PR_FALSE),
    mAllowJavascript(PR_FALSE),
    mAllowMetaRedirects(PR_FALSE),
    mAllowImages(PR_FALSE)
{
}

jxWindow::~jxWindow()
{
	Close();
	mDocShell = nsnull;
}

NS_IMPL_CYCLE_COLLECTION_CLASS(jxWindow)

NS_IMPL_CYCLE_COLLECTION_TRAVERSE_BEGIN(jxWindow)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE_NSCOMPTR(mDocShell)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE_NSCOMPTR(mOnLoadListener)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE_NSCOMPTR(mOnErrorListener)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE_NSCOMPTR(mOnLocationChangeListener)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE_NSCOMPTR(mOnSSLCertErrorListener)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE_NSCOMPTR(mScriptContext)
NS_IMPL_CYCLE_COLLECTION_TRAVERSE_END


NS_IMPL_CYCLE_COLLECTION_UNLINK_BEGIN(jxWindow)
  NS_IMPL_CYCLE_COLLECTION_UNLINK_NSCOMPTR(mDocShell)
  NS_IMPL_CYCLE_COLLECTION_UNLINK_NSCOMPTR(mOnLoadListener)
  NS_IMPL_CYCLE_COLLECTION_UNLINK_NSCOMPTR(mOnErrorListener)
  NS_IMPL_CYCLE_COLLECTION_UNLINK_NSCOMPTR(mOnLocationChangeListener)
  NS_IMPL_CYCLE_COLLECTION_UNLINK_NSCOMPTR(mOnSSLCertErrorListener)
  NS_IMPL_CYCLE_COLLECTION_UNLINK_NSCOMPTR(mScriptContext)
NS_IMPL_CYCLE_COLLECTION_UNLINK_END

// QueryInterface implementation for jxWindow
NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION(jxWindow)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, jxIWindow)
  NS_INTERFACE_MAP_ENTRY(jxIWindow)
  NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
  NS_INTERFACE_MAP_ENTRY(nsIDOMEventTarget)
  NS_INTERFACE_MAP_ENTRY(nsISupportsWeakReference)
  NS_INTERFACE_MAP_ENTRY(nsIInterfaceRequestor)
  NS_INTERFACE_MAP_ENTRY(nsIXPCScriptable)
  NS_IMPL_QUERY_CLASSINFO(jxWindow)
NS_INTERFACE_MAP_END

NS_IMPL_CYCLE_COLLECTING_ADDREF_AMBIGUOUS(jxWindow, jxIWindow)
NS_IMPL_CYCLE_COLLECTING_RELEASE_AMBIGUOUS(jxWindow, jxIWindow)
NS_IMPL_CI_INTERFACE_GETTER1(jxWindow, jxIWindow)

// The nsIXPCScriptable map declaration that will generate stubs for us...
#define XPC_MAP_CLASSNAME           jxWindow
#define XPC_MAP_QUOTED_CLASSNAME   "jxWindow"
#define XPC_MAP_FLAGS               nsIXPCScriptable::USE_JSSTUB_FOR_ADDPROPERTY |\
                                    nsIXPCScriptable::USE_JSSTUB_FOR_DELPROPERTY |\
                                    nsIXPCScriptable::USE_JSSTUB_FOR_SETPROPERTY
#include "xpc_map_end.h" /* This will #undef the above */

/* void loadURI (in wstring aURI, in nsIURI aReferrer, in nsIInputStream aPostData, in nsIInputStream aHeaders); */
NS_IMETHODIMP
jxWindow::LoadURI(const PRUnichar *aURI, nsIURI *aReferrer, nsIInputStream *aPostData, nsIInputStream *aHeaders)
{
    nsresult rv;
	if (!mDocShell) {
		rv = CreateDocShell();
		NS_ENSURE_SUCCESS(rv, rv);
	} else {
	    nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(mDocShell));
		rv = webNav->Stop(nsIWebNavigation::STOP_ALL);
		NS_ENSURE_SUCCESS(rv, rv);
	}

	InitScriptContext();
    nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(mDocShell));
    rv = webNav->LoadURI(aURI, nsIWebNavigation::LOAD_FLAGS_STOP_CONTENT, aReferrer, aPostData, aHeaders);
    NS_ENSURE_SUCCESS(rv, rv);
	
	if (mAsync) {
		return NS_OK;
	}
	PRBool done;
    return WaitCompleted(&done);
}

/* void loadStream (in nsIInputStream aStream, in nsIURI aURI, in ACString aContentType, in ACString aContentCharset); */
NS_IMETHODIMP
jxWindow::LoadStream(nsIInputStream *aStream, nsIURI *aURI, const nsACString & aContentType, const nsACString & aContentCharset)
{
    nsresult rv;
	if (!mDocShell) {
		rv = CreateDocShell();
		NS_ENSURE_SUCCESS(rv, rv);
	} else {
	    nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(mDocShell));
		rv = webNav->Stop(nsIWebNavigation::STOP_ALL);
		NS_ENSURE_SUCCESS(rv, rv);
	}

	InitScriptContext();
    nsCOMPtr<nsIDocShellLoadInfo> loadInfo;
    rv = mDocShell->CreateLoadInfo(getter_AddRefs(loadInfo));
	NS_ENSURE_SUCCESS(rv, rv);
    
    loadInfo->SetLoadType(nsIDocShellLoadInfo::loadStopContent);

    rv = mDocShell->LoadStream(aStream, aURI, aContentType, aContentCharset, loadInfo);
    NS_ENSURE_SUCCESS(rv, rv);

	if (mAsync) {
		return NS_OK;
	}
	PRBool done;
    return WaitCompleted(&done);
}

/* boolean waitCompleted (); */
NS_IMETHODIMP
jxWindow::WaitCompleted(PRBool *_retval)
{
    NS_ENSURE_ARG_POINTER(_retval);
    nsresult rv;

	if ((mState == UNINITIALIZED) || (mState == COMPLETED)) {
		*_retval = PR_FALSE;
		return NS_OK;
	}

    // Pump events until the document load is completed
    nsIThread *thread = NS_GetCurrentThread();
	while(mState != COMPLETED) {
		PRBool processedEvent;
		rv = thread->ProcessNextEvent(PR_TRUE, &processedEvent);
		NS_ENSURE_SUCCESS(rv, rv);
	}
	*_retval = PR_TRUE;
    return NS_OK;
}

/* void close (); */
NS_IMETHODIMP
jxWindow::Close()
{
    nsresult rv;
	if (mDocShell) {
		nsCOMPtr<nsIWebProgress> webPogress(do_GetInterface(mDocShell));
		webPogress->RemoveProgressListener(this);

	    nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(mDocShell));
		rv = webNav->Stop(nsIWebNavigation::STOP_ALL);
		NS_ENSURE_SUCCESS(rv, rv);

		nsCOMPtr<nsILoadGroup> loadGroup(do_GetInterface(mDocShell, &rv));
		NS_ENSURE_SUCCESS(rv, rv);
		
		rv = loadGroup->SetNotificationCallbacks(mNotificationCallbacks);
		NS_ENSURE_SUCCESS(rv, rv);
		mNotificationCallbacks = nsnull;
	}
	mDocShell = nsnull;
	mScriptContext = nsnull;
	mOnLoadListener = nsnull;
	mOnErrorListener = nsnull;
	mOnLocationChangeListener = nsnull;
	mOnSSLCertErrorListener = nsnull;

	mState = UNINITIALIZED;
	return NS_OK;
}

/* readonly attribute nsIDOMDocument document; */
NS_IMETHODIMP
jxWindow::GetDocument(nsIDOMDocument * *aDocument)
{
    NS_ENSURE_ARG_POINTER(aDocument);
	*aDocument = nsnull;
	if (mDocShell) {
		nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(mDocShell));
		return webNav->GetDocument(aDocument);
	}
	return NS_OK;
}

/* readonly attribute nsIDOMWindow window; */
NS_IMETHODIMP
jxWindow::GetWindow(nsIDOMWindow * *aWindow)
{
    NS_ENSURE_ARG_POINTER(aWindow);
	*aWindow = nsnull;
	if (mDocShell) {
		nsCOMPtr<nsIDOMWindow> window(do_GetInterface(mDocShell));
		*aWindow = window;
		NS_IF_ADDREF(*aWindow);
	}
	return NS_OK;
}

/* readonly attribute nsIURI currentURI; */
NS_IMETHODIMP
jxWindow::GetCurrentURI(nsIURI * *aCurrentURI)
{
    NS_ENSURE_ARG_POINTER(aCurrentURI);
	*aCurrentURI = nsnull;
	if (!mDocShell) {
		return NS_ERROR_NOT_INITIALIZED;
	}

    nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(mDocShell));
    return webNav->GetCurrentURI(aCurrentURI);
}

/* readonly attribute nsIHttpChannel documentChannel; */
NS_IMETHODIMP
jxWindow::GetDocumentChannel(nsIHttpChannel * *aDocumentChannel)
{
    NS_ENSURE_ARG_POINTER(aDocumentChannel);
	nsresult rv;
	
	*aDocumentChannel = nsnull;
	if (mDocShell) {
		nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(mDocShell));
		nsCOMPtr<nsIDOMDocument> domDocument;
		rv = webNav->GetDocument(getter_AddRefs(domDocument));
		if (NS_SUCCEEDED(rv) && domDocument) {
			nsCOMPtr<nsIDocument> doc(do_QueryInterface(domDocument));
			if (doc) {
				nsIChannel* channel = doc->GetChannel();
				if (channel) {
					return CallQueryInterface(channel, aDocumentChannel);
				}
			}
		}
	}
	return NS_OK;
}

/* attribute boolean async; */
NS_IMETHODIMP
jxWindow::GetAsync(PRBool *aASync)
{
    NS_ENSURE_ARG_POINTER(aASync);

	*aASync = mAsync;
    return NS_OK;
}

NS_IMETHODIMP
jxWindow::SetAsync(PRBool aASync)
{
	if (mState == LOADING) {
		return NS_ERROR_UNEXPECTED;
	}
	mAsync = aASync;
	return NS_OK;
}

/* attribute boolean allowJavascript; */
NS_IMETHODIMP
jxWindow::GetAllowJavascript(PRBool *aAllowJavascript)
{
    NS_ENSURE_ARG_POINTER(aAllowJavascript);

	if (mDocShell) {
		return mDocShell->GetAllowJavascript(aAllowJavascript);
	}
	*aAllowJavascript = mAllowJavascript;
    return NS_OK;
}

NS_IMETHODIMP
jxWindow::SetAllowJavascript(PRBool aAllowJavascript)
{
	mAllowJavascript = aAllowJavascript;
	if (mDocShell) {
		return mDocShell->SetAllowJavascript(aAllowJavascript);
	}
	return NS_OK;
}

/* attribute boolean allowMetaRedirects; */
NS_IMETHODIMP
jxWindow::GetAllowMetaRedirects(PRBool *aAllowMetaRedirects)
{
    NS_ENSURE_ARG_POINTER(aAllowMetaRedirects);

	if (mDocShell) {
		return mDocShell->GetAllowMetaRedirects(aAllowMetaRedirects);
	}
	*aAllowMetaRedirects = mAllowMetaRedirects;
    return NS_OK;
}

NS_IMETHODIMP
jxWindow::SetAllowMetaRedirects(PRBool aAllowMetaRedirects)
{
	mAllowMetaRedirects = aAllowMetaRedirects;
	if (mDocShell) {
		return mDocShell->SetAllowMetaRedirects(aAllowMetaRedirects);
	}
	return NS_OK;
}

/* attribute boolean allowSubframes; */
NS_IMETHODIMP
jxWindow::GetAllowSubframes(PRBool *aAllowSubframes)
{
    NS_ENSURE_ARG_POINTER(aAllowSubframes);

	if (mDocShell) {
		return mDocShell->GetAllowSubframes(aAllowSubframes);
	}
	*aAllowSubframes = mAllowSubframes;
	return NS_OK;
}

NS_IMETHODIMP
jxWindow::SetAllowSubframes(PRBool aAllowSubframes)
{
	mAllowSubframes = aAllowSubframes;
	if (mDocShell) {
		return mDocShell->SetAllowSubframes(aAllowSubframes);
	}
	return NS_OK;
}

/* attribute boolean allowImages; */
NS_IMETHODIMP
jxWindow::GetAllowImages(PRBool *aAllowImages)
{
    NS_ENSURE_ARG_POINTER(aAllowImages);

	if (mDocShell) {
		return mDocShell->GetAllowImages(aAllowImages);
	}
	*aAllowImages = mAllowImages;
	return NS_OK;
}

NS_IMETHODIMP
jxWindow::SetAllowImages(PRBool aAllowImages)
{
	mAllowImages = aAllowImages;
	if (mDocShell) {
		return mDocShell->SetAllowImages(aAllowImages);
	}
	return NS_OK;
}

/* attribute long readyState; */
NS_IMETHODIMP
jxWindow::GetReadyState(PRInt32 *aReadyState)
{
    NS_ENSURE_ARG_POINTER(aReadyState);

	*aReadyState = mState;
	return NS_OK;
}

/* attribute nsIDOMEventListener onload; */
NS_IMETHODIMP
jxWindow::GetOnload(nsIDOMEventListener * *aOnLoad)
{
  NS_ENSURE_ARG_POINTER(aOnLoad);

  NS_IF_ADDREF(*aOnLoad = mOnLoadListener);

  return NS_OK;
}

NS_IMETHODIMP
jxWindow::SetOnload(nsIDOMEventListener * aOnLoad)
{
  mOnLoadListener = aOnLoad;
  return NS_OK;
}

/* attribute nsIDOMEventListener onerror; */
NS_IMETHODIMP
jxWindow::GetOnerror(nsIDOMEventListener * *aOnerror)
{
  NS_ENSURE_ARG_POINTER(aOnerror);

  NS_IF_ADDREF(*aOnerror = mOnErrorListener);

  return NS_OK;
}

NS_IMETHODIMP
jxWindow::SetOnerror(nsIDOMEventListener * aOnerror)
{
  mOnErrorListener = aOnerror;
  return NS_OK;
}

/* attribute jxILocationChangeListener onlocationchange; */
NS_IMETHODIMP
jxWindow::GetOnlocationchange(jxILocationChangeListener * *aOnlocationchange)
{
  NS_ENSURE_ARG_POINTER(aOnlocationchange);

  NS_IF_ADDREF(*aOnlocationchange = mOnLocationChangeListener);

  return NS_OK;
}
NS_IMETHODIMP
jxWindow::SetOnlocationchange(jxILocationChangeListener * aOnlocationchange)
{
  mOnLocationChangeListener = aOnlocationchange;
  return NS_OK;
}

/* attribute aptIBadCertHandler onsslcerterror; */
NS_IMETHODIMP
jxWindow::GetOnsslcerterror(aptIBadCertHandler * *aOnsslcerterror)
{
  NS_ENSURE_ARG_POINTER(aOnsslcerterror);

  NS_IF_ADDREF(*aOnsslcerterror = mOnSSLCertErrorListener);

  return NS_OK;
}

NS_IMETHODIMP
jxWindow::SetOnsslcerterror(aptIBadCertHandler * aOnsslcerterror)
{
  mOnSSLCertErrorListener = aOnsslcerterror;
  return NS_OK;
}

////////////////////////////////////////////////////
// nsIWebProgressListener
//

/* void onStateChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in unsigned long aStateFlags, in nsresult aStatus); */
NS_IMETHODIMP
jxWindow::OnStateChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 aStateFlags, nsresult aStatus)
{
	if ((~aStateFlags & (STATE_IS_DOCUMENT | STATE_IS_REQUEST | STATE_IS_WINDOW | STATE_IS_NETWORK | STATE_START)) == 0) {
		PRInt32 prevState = mState;
		mState = LOADING;
		if (prevState == COMPLETED) {
			LocationChanged(aRequest);
		}
	} else if ((~aStateFlags & (STATE_IS_REQUEST | STATE_REDIRECTING)) == 0) {
		mRedirect = PR_TRUE;
	} else if (mRedirect
		&& ((~aStateFlags & (STATE_IS_REQUEST | STATE_START)) == 0)) {
		LocationChanged(aRequest);
	} else if ((~aStateFlags & (STATE_IS_REQUEST | STATE_STOP)) == 0) {
		mState = LOADED;
	} else if ((~aStateFlags & (STATE_IS_DOCUMENT | STATE_STOP)) == 0) {
		mState = INTERACTIVE;
	} else if ( (mState == INTERACTIVE)
		&& ((~aStateFlags & (STATE_IS_NETWORK | STATE_IS_WINDOW | STATE_STOP)) == 0)) {
		RequestCompleted();
	}
    return NS_OK;
}

/* void onProgressChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in long aCurSelfProgress, in long aMaxSelfProgress, in long aCurTotalProgress, in long aMaxTotalProgress); */
NS_IMETHODIMP
jxWindow::OnProgressChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRInt32 aCurSelfProgress, PRInt32 aMaxSelfProgress, PRInt32 aCurTotalProgress, PRInt32 aMaxTotalProgress)
{
    return NS_OK;
}

/* void onLocationChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsIURI aLocation); */
NS_IMETHODIMP
jxWindow::OnLocationChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsIURI *aLocation)
{
    return NS_OK;
}

/* void onStatusChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsresult aStatus, in wstring aMessage); */
NS_IMETHODIMP
jxWindow::OnStatusChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsresult aStatus, const PRUnichar *aMessage)
{
    return NS_OK;
}

/* void onSecurityChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in unsigned long aState); */
NS_IMETHODIMP
jxWindow::OnSecurityChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 aState)
{
    return NS_OK;
}

////////////////////////////////////////////////////
// nsIDOMEventTarget
//

/* void addEventListener (in string type, in nsIDOMEventListener listener); */
NS_IMETHODIMP
jxWindow::AddEventListener(const nsAString& type,
                                   nsIDOMEventListener *listener,
                                   PRBool useCapture)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

/* void removeEventListener (in string type, in nsIDOMEventListener listener); */
NS_IMETHODIMP
jxWindow::RemoveEventListener(const nsAString & type,
                                      nsIDOMEventListener *listener,
                                      PRBool useCapture)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean dispatchEvent (in nsIDOMEvent evt); */
NS_IMETHODIMP
jxWindow::DispatchEvent(nsIDOMEvent *evt, PRBool *_retval)
{
	// Ignored
	return NS_OK;
}


/////////////////////////////////////////////////////
// nsIInterfaceRequestor
//
NS_IMETHODIMP
jxWindow::GetInterface(const nsIID & aIID, void **aResult)
{
	if (aIID.Equals(NS_GET_IID(aptIBadCertHandler)) && mOnSSLCertErrorListener) {
		*aResult = new jxWindowBadCertHandler(this);
		NS_IF_ADDREF(static_cast<aptIBadCertHandler*>(*aResult));
		return NS_OK;
	}
	if (mNotificationCallbacks) {
		return mNotificationCallbacks->GetInterface(aIID, aResult);
	}
	return QueryInterface(aIID, aResult);
}


////////////////////////////////////////////////////
// 
//
nsresult
jxWindow::CreateDocShell()
{
    nsresult rv;

	// Create shell
	mDocShell = do_CreateInstance("@mozilla.org/webshell;1");
	NS_ENSURE_TRUE(mDocShell, NS_ERROR_FAILURE);
	
	rv = mDocShell->SetAllowJavascript(mAllowJavascript);
	NS_ENSURE_SUCCESS(rv, rv);
	rv = mDocShell->SetAllowMetaRedirects(mAllowMetaRedirects);
	NS_ENSURE_SUCCESS(rv, rv);
	rv = mDocShell->SetAllowSubframes(mAllowSubframes);
	NS_ENSURE_SUCCESS(rv, rv);
	rv = mDocShell->SetAllowImages(mAllowImages);
	NS_ENSURE_SUCCESS(rv, rv);

	nsCOMPtr<nsIWebProgress> webPogress(do_GetInterface(mDocShell));
	webPogress->AddProgressListener(this, nsIWebProgress::NOTIFY_STATE_DOCUMENT
										| nsIWebProgress::NOTIFY_STATE_NETWORK
										| nsIWebProgress::NOTIFY_STATE_REQUEST
										| nsIWebProgress::NOTIFY_STATE_WINDOW);

	nsCOMPtr<nsILoadGroup> loadGroup(do_GetInterface(mDocShell, &rv));
	NS_ENSURE_SUCCESS(rv, rv);
	
	rv = loadGroup->GetNotificationCallbacks(getter_AddRefs(mNotificationCallbacks));
	NS_ENSURE_SUCCESS(rv, rv);
	
	rv = loadGroup->SetNotificationCallbacks(static_cast<nsIInterfaceRequestor *>(this));
	NS_ENSURE_SUCCESS(rv, rv);
	
	return NS_OK;
}

void
jxWindow::InitScriptContext()
{
	nsCOMPtr<nsIJSContextStack> stack = do_GetService("@mozilla.org/js/xpc/ContextStack;1");
	if (!stack) {
		return;
	}
	
	JSContext *cx;
	if (NS_FAILED(stack->Peek(&cx)) || !cx) {
		return;
	}
	mScriptContext = GetScriptContextFromJSContext(cx);
	return;
}

nsresult
jxWindow::RequestCompleted()
{	
	mState = COMPLETED;

	if (mOnLoadListener) {
		nsCOMPtr<nsIDOMEvent> domevent;
		nsresult rv = CreateEvent(NS_LITERAL_STRING(LOAD_STR), getter_AddRefs(domevent));
		if (NS_SUCCEEDED(rv)) {		
			nsCOMPtr<nsIJSContextStack> stack;
			JSContext *cx = nsnull;
			
			if (mScriptContext) {
				stack = do_GetService("@mozilla.org/js/xpc/ContextStack;1");
				
				if (stack) {
					cx = (JSContext *)mScriptContext->GetNativeContext();
					if (cx) {
						stack->Push(cx);
					}
				}
			}
			
			mOnLoadListener->HandleEvent(domevent);
			if (cx) {
				stack->Pop(&cx);
			}
		}
	}
	return NS_OK;
}

nsresult
jxWindow::LocationChanged(nsIRequest *aRequest)
{
	nsresult rv;
	PRBool isRedirect = mRedirect;
	mRedirect = PR_FALSE;

	nsCOMPtr<nsIChannel> channel(do_QueryInterface(aRequest));
	NS_ENSURE_TRUE(channel, NS_ERROR_FAILURE);
	nsCOMPtr<nsIURI> aLocation;
	rv = channel->GetURI(getter_AddRefs(aLocation));
	NS_ENSURE_SUCCESS(rv, rv);
	NS_ENSURE_TRUE(aLocation, NS_ERROR_FAILURE);

	if (mOnLocationChangeListener) {		
		nsCOMPtr<nsIJSContextStack> stack;
		JSContext *cx = nsnull;

		if (mScriptContext) {
			stack = do_GetService("@mozilla.org/js/xpc/ContextStack;1");

			if (stack) {
				cx = (JSContext *)mScriptContext->GetNativeContext();
				if (cx) {
					stack->Push(cx);
				}
			}
		}

		PRBool retval = PR_TRUE;
		rv = mOnLocationChangeListener->OnLocationChange(aLocation, isRedirect, &retval);

		if (cx) {
			stack->Pop(&cx);
		}

		if (NS_SUCCEEDED(rv) && !retval) {
			nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(mDocShell));
			rv = webNav->Stop(nsIWebNavigation::STOP_ALL);
		}
	}

	return rv;
}

nsresult
jxWindow::CreateEvent(const nsAString& aType, nsIDOMEvent** aDOMEvent)
{
  nsresult rv = nsEventDispatcher::CreateEvent(nsnull, nsnull,
                                               NS_LITERAL_STRING("Events"),
                                               aDOMEvent);
  if (NS_FAILED(rv)) {
    return rv;
  }

  nsCOMPtr<nsIPrivateDOMEvent> privevent(do_QueryInterface(*aDOMEvent));
  if (!privevent) {
    NS_IF_RELEASE(*aDOMEvent);
    return NS_ERROR_FAILURE;
  }

  if (!aType.IsEmpty()) {
    (*aDOMEvent)->InitEvent(aType, PR_FALSE, PR_FALSE);
  }
  
  privevent->SetTarget(this);
  privevent->SetCurrentTarget(this);
  privevent->SetOriginalTarget(this);

  // We assume anyone who managed to call CreateEvent is trusted
  privevent->SetTrusted(PR_TRUE);

  return NS_OK;
}

NS_IMPL_THREADSAFE_ISUPPORTS1(jxWindowBadCertHandler, aptIBadCertHandler)

jxWindowBadCertHandler::jxWindowBadCertHandler(jxWindow *target)
  : mTarget(target)
{
}

jxWindowBadCertHandler::~jxWindowBadCertHandler()
{
}

/* boolean handleCertProblem (in nsIInterfaceRequestor socketInfo, in nsISSLStatus status, in AUTF8String targetSite); */
NS_IMETHODIMP
jxWindowBadCertHandler::HandleCertProblem(nsIInterfaceRequestor *socketInfo, nsISSLStatus *status, const nsACString & targetSite, PRBool *_retval)
{
  return mTarget->HandleCertProblem(socketInfo, status, targetSite, _retval);
}

nsresult
jxWindow::HandleCertProblem(nsIInterfaceRequestor *socketInfo, nsISSLStatus *status, const nsACString & targetSite, PRBool *_retval)
{
  NS_ENSURE_ARG_POINTER(_retval);
  *_retval = PR_FALSE;

  nsresult rv;
  if (!mOnSSLCertErrorListener) {
	  return NS_OK;
  }
  nsCOMPtr<nsIJSContextStack> stack;
  JSContext *cx = nsnull;

  if (mScriptContext) {
    stack = do_GetService("@mozilla.org/js/xpc/ContextStack;1");

    if (stack) {
      cx = (JSContext *)mScriptContext->GetNativeContext();

      if (cx) {
        stack->Push(cx);
      }
    }
  }

  rv = mOnSSLCertErrorListener->HandleCertProblem(socketInfo, status, targetSite, _retval);

  if (cx) {
    stack->Pop(&cx);
  }

  return rv;
}
