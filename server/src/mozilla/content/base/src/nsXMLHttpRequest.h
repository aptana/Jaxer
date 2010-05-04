/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 *  It is derived from Mozilla software and modified by Aptana, Inc.
 *  Aptana, Inc. has elected to use and license the Mozilla software 
 *  under the terms of the GPL, and licenses this file to you under the terms
 *  of the GPL.
 *  
 *  Contributor(s): Aptana, Inc.
 *  The portions modified by Aptana are Copyright (C) 2007-2008 Aptana, Inc.
 *  All Rights Reserved.
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
/* ***** BEGIN ORIGINAL ATTRIBUTION BLOCK *****
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#ifndef nsXMLHttpRequest_h__
#define nsXMLHttpRequest_h__

#include "nsIXMLHttpRequest.h"
#include "nsISupportsUtils.h"
#include "nsString.h"
#include "nsIDOMLoadListener.h"
#include "nsIDOMDocument.h"
#include "nsIURI.h"
#include "nsIHttpChannel.h"
#include "nsIDocument.h"
#include "nsIStreamListener.h"
#include "nsWeakReference.h"
#include "jsapi.h"
#include "nsIScriptContext.h"
#include "nsIChannelEventSink.h"
#include "nsIInterfaceRequestor.h"
#include "nsIHttpHeaderVisitor.h"
#include "nsIProgressEventSink.h"
#include "nsCOMArray.h"
#include "nsJSUtils.h"
#include "nsTArray.h"
#include "nsIJSNativeInitializer.h"
#include "nsIDOMLSProgressEvent.h"
#include "nsClassHashtable.h"
#include "nsHashKeys.h"
#include "prclist.h"
#include "prtime.h"
#ifdef JAXER
#include "aptIBadCertHandler.h"
#endif /* JAXER */

class nsILoadGroup;

class nsAccessControlLRUCache
{
public:
  struct TokenTime
  {
    nsCString token;
    PRTime expirationTime;
  };

  struct CacheEntry : public PRCList
  {
    CacheEntry(nsCString& aKey)
      : mKey(aKey)
    {
      MOZ_COUNT_CTOR(nsAccessControlLRUCache::CacheEntry);
    }
    
    ~CacheEntry()
    {
      MOZ_COUNT_DTOR(nsAccessControlLRUCache::CacheEntry);
    }

    void PurgeExpired(PRTime now);
    PRBool CheckRequest(const nsCString& aMethod,
                        const nsTArray<nsCString>& aCustomHeaders);

    nsCString mKey;
    nsTArray<TokenTime> mMethods;
    nsTArray<TokenTime> mHeaders;
  };

  nsAccessControlLRUCache()
  {
    MOZ_COUNT_CTOR(nsAccessControlLRUCache);
    PR_INIT_CLIST(&mList);
  }

  ~nsAccessControlLRUCache()
  {
    Clear();
    MOZ_COUNT_DTOR(nsAccessControlLRUCache);
  }

  PRBool Initialize()
  {
    return mTable.Init();
  }

  CacheEntry* GetEntry(nsIURI* aURI, nsIPrincipal* aPrincipal,
                       PRBool aWithCredentials, PRBool aCreate);
  void RemoveEntries(nsIURI* aURI, nsIPrincipal* aPrincipal);

  void Clear();

private:
  static PLDHashOperator
    RemoveExpiredEntries(const nsACString& aKey, nsAutoPtr<CacheEntry>& aValue,
                         void* aUserData);

  static PRBool GetCacheKey(nsIURI* aURI, nsIPrincipal* aPrincipal,
                            PRBool aWithCredentials, nsACString& _retval);

  nsClassHashtable<nsCStringHashKey, CacheEntry> mTable;
  PRCList mList;
};

class nsXHREventTarget : public nsDOMEventTargetHelper,
                         public nsIXMLHttpRequestEventTarget,
                         public nsWrapperCache
{
public:
  virtual ~nsXHREventTarget();
  NS_DECL_ISUPPORTS_INHERITED

  class NS_CYCLE_COLLECTION_INNERCLASS
    : public NS_CYCLE_COLLECTION_CLASSNAME(nsDOMEventTargetHelper)
  {
    NS_IMETHOD RootAndUnlinkJSObjects(void *p);
    NS_DECL_CYCLE_COLLECTION_CLASS_INHERITED_BODY(nsXHREventTarget,
                                                  nsDOMEventTargetHelper)
    NS_IMETHOD_(void) Trace(void *p, TraceCallback cb, void *closure);
  };
  NS_CYCLE_COLLECTION_PARTICIPANT_INSTANCE

  NS_DECL_NSIXMLHTTPREQUESTEVENTTARGET
  NS_FORWARD_NSIDOMEVENTTARGET(nsDOMEventTargetHelper::)
  NS_FORWARD_NSIDOMNSEVENTTARGET(nsDOMEventTargetHelper::)

  void GetParentObject(nsIScriptGlobalObject **aParentObject)
  {
    if (mOwner) {
      CallQueryInterface(mOwner, aParentObject);
    }
    else {
      *aParentObject = nsnull;
    }
  }

  static nsXHREventTarget* FromSupports(nsISupports* aSupports)
  {
    nsPIDOMEventTarget* target =
      static_cast<nsPIDOMEventTarget*>(aSupports);
#ifdef DEBUG
    {
      nsCOMPtr<nsPIDOMEventTarget> target_qi =
        do_QueryInterface(aSupports);

      // If this assertion fires the QI implementation for the object in
      // question doesn't use the nsPIDOMEventTarget pointer as the
      // nsISupports pointer. That must be fixed, or we'll crash...
      NS_ASSERTION(target_qi == target, "Uh, fix QI!");
    }
#endif

    return static_cast<nsXHREventTarget*>(target);
  }

protected:
  nsRefPtr<nsDOMEventListenerWrapper> mOnLoadListener;
  nsRefPtr<nsDOMEventListenerWrapper> mOnErrorListener;
  nsRefPtr<nsDOMEventListenerWrapper> mOnAbortListener;
  nsRefPtr<nsDOMEventListenerWrapper> mOnLoadStartListener;
  nsRefPtr<nsDOMEventListenerWrapper> mOnProgressListener;
};

class nsXMLHttpRequestUpload : public nsXHREventTarget,
                               public nsIXMLHttpRequestUpload
{
public:
  nsXMLHttpRequestUpload(nsPIDOMWindow* aOwner,
                         nsIScriptContext* aScriptContext)
  {
    mOwner = aOwner;
    mScriptContext = aScriptContext;
  }
  virtual ~nsXMLHttpRequestUpload();
  NS_DECL_ISUPPORTS_INHERITED
  NS_FORWARD_NSIXMLHTTPREQUESTEVENTTARGET(nsXHREventTarget::)
  NS_FORWARD_NSIDOMEVENTTARGET(nsXHREventTarget::)
  NS_FORWARD_NSIDOMNSEVENTTARGET(nsXHREventTarget::)
  NS_DECL_NSIXMLHTTPREQUESTUPLOAD

  PRBool HasListeners()
  {
    return mListenerManager && mListenerManager->HasListeners();
  }
};

class nsXMLHttpRequest : public nsXHREventTarget,
                         public nsIXMLHttpRequest,
                         public nsIJSXMLHttpRequest,
                         public nsIDOMLoadListener,
                         public nsIStreamListener,
                         public nsIChannelEventSink,
                         public nsIProgressEventSink,
                         public nsIInterfaceRequestor,
                         public nsSupportsWeakReference,
                         public nsIJSNativeInitializer,
                         public nsITimerCallback
{
public:
  nsXMLHttpRequest();
  virtual ~nsXMLHttpRequest();

  NS_DECL_ISUPPORTS_INHERITED

  // nsIXMLHttpRequest
  NS_DECL_NSIXMLHTTPREQUEST

  // nsIJSXMLHttpRequest
  NS_IMETHOD GetOnuploadprogress(nsIDOMEventListener** aOnuploadprogress);
  NS_IMETHOD SetOnuploadprogress(nsIDOMEventListener* aOnuploadprogress);

  NS_FORWARD_NSIXMLHTTPREQUESTEVENTTARGET(nsXHREventTarget::)

  // nsIDOMEventListener
  NS_DECL_NSIDOMEVENTLISTENER

  // nsIDOMLoadListener
  NS_IMETHOD Load(nsIDOMEvent* aEvent);
  NS_IMETHOD BeforeUnload(nsIDOMEvent* aEvent);
  NS_IMETHOD Unload(nsIDOMEvent* aEvent);
  NS_IMETHOD Abort(nsIDOMEvent* aEvent);
  NS_IMETHOD Error(nsIDOMEvent* aEvent);

  // nsIStreamListener
  NS_DECL_NSISTREAMLISTENER

  // nsIRequestObserver
  NS_DECL_NSIREQUESTOBSERVER

  // nsIChannelEventSink
  NS_DECL_NSICHANNELEVENTSINK

  // nsIProgressEventSink
  NS_DECL_NSIPROGRESSEVENTSINK

  // nsIInterfaceRequestor
  NS_DECL_NSIINTERFACEREQUESTOR

  // nsITimerCallback
  NS_DECL_NSITIMERCALLBACK

  // nsIJSNativeInitializer
  NS_IMETHOD Initialize(nsISupports* aOwner, JSContext* cx, JSObject* obj,
                       PRUint32 argc, jsval* argv);

  NS_FORWARD_NSIDOMEVENTTARGET(nsXHREventTarget::)
  NS_FORWARD_NSIDOMNSEVENTTARGET(nsXHREventTarget::)

  // This creates a trusted readystatechange event, which is not cancelable and
  // doesn't bubble.
  static nsresult CreateReadystatechangeEvent(nsIDOMEvent** aDOMEvent);
  // For backwards compatibility aPosition should contain the headers for upload
  // and aTotalSize is LL_MAXUINT when unknown. Both those values are
  // used by nsXMLHttpProgressEvent. Normal progress event should not use
  // headers in aLoaded and aTotal is 0 when unknown.
  void DispatchProgressEvent(nsPIDOMEventTarget* aTarget,
                             const nsAString& aType,
                             // Whether to use nsXMLHttpProgressEvent,
                             // which implements LS Progress Event.
                             PRBool aUseLSEventWrapper,
                             PRBool aLengthComputable,
                             // For Progress Events
                             PRUint64 aLoaded, PRUint64 aTotal,
                             // For LS Progress Events
                             PRUint64 aPosition, PRUint64 aTotalSize);
  void DispatchProgressEvent(nsPIDOMEventTarget* aTarget,
                             const nsAString& aType,
                             PRBool aLengthComputable,
                             PRUint64 aLoaded, PRUint64 aTotal)
  {
    DispatchProgressEvent(aTarget, aType, PR_FALSE,
                          aLengthComputable, aLoaded, aTotal,
                          aLoaded, aLengthComputable ? aTotal : LL_MAXUINT);
  }

  // This is called by the factory constructor.
  nsresult Init();

  void SetRequestObserver(nsIRequestObserver* aObserver);

  NS_DECL_CYCLE_COLLECTION_CLASS_INHERITED(nsXMLHttpRequest,
                                           nsXHREventTarget)

  static PRBool EnsureACCache()
  {
    if (sAccessControlCache)
      return PR_TRUE;

    nsAutoPtr<nsAccessControlLRUCache> newCache(new nsAccessControlLRUCache());
    NS_ENSURE_TRUE(newCache, PR_FALSE);

    if (newCache->Initialize()) {
      sAccessControlCache = newCache.forget();
      return PR_TRUE;
    }

    return PR_FALSE;
  }

  static void ShutdownACCache()
  {
    delete sAccessControlCache;
    sAccessControlCache = nsnull;
  }

  PRBool AllowUploadProgress();

  static nsAccessControlLRUCache* sAccessControlCache;

protected:
  friend class nsMultipartProxyListener;

  nsresult DetectCharset(nsACString& aCharset);
  nsresult ConvertBodyToText(nsAString& aOutBuffer);
  static NS_METHOD StreamReaderFunc(nsIInputStream* in,
                void* closure,
                const char* fromRawSegment,
                PRUint32 toOffset,
                PRUint32 count,
                PRUint32 *writeCount);
  // Change the state of the object with this. The broadcast argument
  // determines if the onreadystatechange listener should be called.
  nsresult ChangeState(PRUint32 aState, PRBool aBroadcast = PR_TRUE);
  nsresult RequestCompleted();
  nsresult GetLoadGroup(nsILoadGroup **aLoadGroup);
  nsIURI *GetBaseURI();

  nsresult RemoveAddEventListener(const nsAString& aType,
                                  nsRefPtr<nsDOMEventListenerWrapper>& aCurrent,
                                  nsIDOMEventListener* aNew);

  nsresult GetInnerEventListener(nsRefPtr<nsDOMEventListenerWrapper>& aWrapper,
                                 nsIDOMEventListener** aListener);

  already_AddRefed<nsIHttpChannel> GetCurrentHttpChannel();

  /**
   * Check if aChannel is ok for a cross-site request by making sure no
   * inappropriate headers are set, and no username/password is set.
   *
   * Also updates the XML_HTTP_REQUEST_USE_XSITE_AC bit.
   */
  nsresult CheckChannelForCrossSiteRequest(nsIChannel* aChannel);

  void StartProgressEventTimer();

  nsCOMPtr<nsISupports> mContext;
  nsCOMPtr<nsIPrincipal> mPrincipal;
  nsCOMPtr<nsIChannel> mChannel;
  // mReadRequest is different from mChannel for multipart requests
  nsCOMPtr<nsIRequest> mReadRequest;
  nsCOMPtr<nsIDOMDocument> mResponseXML;
  nsCOMPtr<nsIChannel> mACGetChannel;
  nsTArray<nsCString> mACUnsafeHeaders;

  nsRefPtr<nsDOMEventListenerWrapper> mOnUploadProgressListener;
  nsRefPtr<nsDOMEventListenerWrapper> mOnReadystatechangeListener;

  nsCOMPtr<nsIStreamListener> mXMLParserStreamListener;

  // used to implement getAllResponseHeaders()
  class nsHeaderVisitor : public nsIHttpHeaderVisitor {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIHTTPHEADERVISITOR
    nsHeaderVisitor() { }
    virtual ~nsHeaderVisitor() {}
    const nsACString &Headers() { return mHeaders; }
  private:
    nsCString mHeaders;
  };

  nsCString mResponseBody;

  nsCString mOverrideMimeType;

  /**
   * The notification callbacks the channel had when Send() was
   * called.  We want to forward things here as needed.
   */
  nsCOMPtr<nsIInterfaceRequestor> mNotificationCallbacks;
  /**
   * Sink interfaces that we implement that mNotificationCallbacks may
   * want to also be notified for.  These are inited lazily if we're
   * asked for the relevant interface.
   */
  nsCOMPtr<nsIChannelEventSink> mChannelEventSink;
  nsCOMPtr<nsIProgressEventSink> mProgressEventSink;

  nsIRequestObserver* mRequestObserver;

  nsCOMPtr<nsIURI> mBaseURI;

  PRUint32 mState;

#ifdef JAXER
  nsCOMPtr<aptIBadCertHandler> mOnSSLCertErrorListener;

  nsresult mStatus;
  
  nsresult HandleCertProblem(nsIInterfaceRequestor *socketInfo, nsISSLStatus *status, const nsACString & targetSite, PRBool *_retval);
  friend class BadCertHandler;

  // Try to get the charset from the 1st 2K of doc
  nsresult DetectCharsetFromDoc(nsACString& aCharset);
#endif /* JAXER */

  nsRefPtr<nsXMLHttpRequestUpload> mUpload;
  PRUint64 mUploadTransferred;
  PRUint64 mUploadTotal;
  PRPackedBool mUploadComplete;
  PRUint64 mUploadProgress; // For legacy
  PRUint64 mUploadProgressMax; // For legacy

  PRPackedBool mErrorLoad;

  PRPackedBool mTimerIsActive;
  PRPackedBool mProgressEventWasDelayed;
  PRPackedBool mLoadLengthComputable;
  PRUint64 mLoadTotal; // 0 if not known.
  nsCOMPtr<nsITimer> mProgressNotifier;

  PRPackedBool mFirstStartRequestSeen;
};

// helper class to expose a progress DOM Event

class nsXMLHttpProgressEvent : public nsIDOMProgressEvent,
                               public nsIDOMLSProgressEvent,
                               public nsIDOMNSEvent,
                               public nsIPrivateDOMEvent
{
public:
  nsXMLHttpProgressEvent(nsIDOMProgressEvent* aInner,
                         PRUint64 aCurrentProgress,
                         PRUint64 aMaxProgress);
  virtual ~nsXMLHttpProgressEvent();

  NS_DECL_CYCLE_COLLECTING_ISUPPORTS
  NS_DECL_CYCLE_COLLECTION_CLASS_AMBIGUOUS(nsXMLHttpProgressEvent, nsIDOMNSEvent)
  NS_FORWARD_NSIDOMEVENT(mInner->)
  NS_FORWARD_NSIDOMNSEVENT(mInner->)
  NS_FORWARD_NSIDOMPROGRESSEVENT(mInner->)
  NS_DECL_NSIDOMLSPROGRESSEVENT
  // nsPrivateDOMEvent
  NS_IMETHOD DuplicatePrivateData()
  {
    return mInner->DuplicatePrivateData();
  }
  NS_IMETHOD SetTarget(nsIDOMEventTarget* aTarget)
  {
    return mInner->SetTarget(aTarget);
  }
  NS_IMETHOD_(PRBool) IsDispatchStopped()
  {
    return mInner->IsDispatchStopped();
  }
  NS_IMETHOD_(nsEvent*) GetInternalNSEvent()
  {
    return mInner->GetInternalNSEvent();
  }
  NS_IMETHOD SetTrusted(PRBool aTrusted)
  {
    return mInner->SetTrusted(aTrusted);
  }

protected:
  // Use nsDOMProgressEvent so that we can forward
  // most of the method calls easily.
  nsRefPtr<nsDOMProgressEvent> mInner;
  PRUint64 mCurProgress;
  PRUint64 mMaxProgress;
};

#endif
