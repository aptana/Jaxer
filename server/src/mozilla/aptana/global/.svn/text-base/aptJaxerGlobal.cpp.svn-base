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
#include "nsISupports.h"
#include "nsIXPConnect.h"
#include "jsapi.h"
#include "jsobj.h"
#include "jsdbgapi.h"
#include "nsIJSRuntimeService.h"
#include "nsIXPCScriptable.h"
#include "nsIScriptGlobalObject.h"
#include "nsServiceManagerUtils.h"
#include "nsIClassInfoImpl.h"
#include "nsIInterfaceRequestorUtils.h"
#include "nsIDOMWindow.h"
#include "nsIDocShell.h"
#include "nsJSUtils.h"
#include "aptJaxerGlobal.h"
#include "aptIDocumentFetcherService.h"
#include "jaxerBuildId.h"

#define DEBUG_SEAL

#ifdef DEBUG_SEAL
#ifdef _WIN32
#define _WINDOWS_
#endif
#include "aptCoreTrace.h"
#ifdef _WIN32
#undef _WINDOWS_
#endif

extern aptCoreTrace gJaxerLog;
#endif

static nsresult
NewJaxerGlobalObject(nsIScriptContext *aContext, nsISupports *aParent, void **aReturn);

aptJaxerGlobal::aptJaxerGlobal()
 : mInitialized(PR_FALSE)
 , mRuntimeService(nsnull)
 , mContext(nsnull)
 , mGlobal(nsnull)
 , mJaxerProto(nsnull)
 , mPrivate(nsnull)
{
	NS_ASSERTION(!sSelf, "aptJaxerGlobal should be a singleton");
	sSelf = this;
}

aptJaxerGlobal::~aptJaxerGlobal()
{
	if (mInitialized) {
		mInitialized = PR_FALSE;
		{
			JSAutoRequest ar(mContext);
			JS_ClearScope(mContext, mGlobal);
			JS_RemoveRoot(mContext, &mGlobal);
			JS_RemoveRoot(mContext, &mPrivate);
			JS_GC(mContext);
		}
		JS_DestroyContext(mContext);
		nsCOMPtr<nsIXPConnect> xpc = do_GetService(nsIXPConnect::GetCID());
		if (xpc) {
			xpc->SyncJSContexts();
		}
	}

	mContext = nsnull;
	mRuntimeService = nsnull;
	sSelf = nsnull;
}

aptJaxerGlobal*
aptJaxerGlobal::sSelf = nsnull;

NS_IMPL_ISUPPORTS1_CI(aptJaxerGlobal, aptIJaxerGlobal)

/* void getObject (); */
NS_IMETHODIMP
aptJaxerGlobal::GetObject()
{
	// This function should only be called from JS.
	nsresult rv;

	nsCOMPtr<nsIXPConnect> xpc = do_GetService(nsIXPConnect::GetCID());
	NS_ENSURE_TRUE(xpc, NS_ERROR_FAILURE);

	nsAXPCNativeCallContext *cc = nsnull;
	rv = xpc->GetCurrentNativeCallContext(&cc);
	NS_ENSURE_SUCCESS(rv, rv);

	JSContext *cx;
	rv = cc->GetJSContext (&cx);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	jsval *retval = nsnull;
	cc->GetRetValPtr(&retval);

	uint32 fileFlags = JS_GetTopScriptFilenameFlags(cx, nsnull);
	if (!(fileFlags & JSFILENAME_SYSTEM)) {
		if (*retval) {
			*retval = JSVAL_VOID;
		}
		return NS_OK;
	}

	JSObject *globalObj = nsnull;
	rv = GetGlobal(&globalObj);

	if (*retval) {
		*retval = OBJECT_TO_JSVAL(globalObj);
	}

    return rv;
}

/* [noscript] readonly attribute JSObjectPtr global; */
NS_IMETHODIMP
aptJaxerGlobal::GetGlobal(JSObject * *aGlobal)
{
	nsresult rv;
	if (!mInitialized) {
		rv = ReallyInit();
		NS_ENSURE_SUCCESS(rv, rv);
	}
	*aGlobal = mJaxerProto;

	return NS_OK;
}

/* [noscript] readonly attribute JSObjectPtr private; */
NS_IMETHODIMP
aptJaxerGlobal::GetPrivate(JSObject * *aPrivate)
{
	nsresult rv;
	if (!mInitialized) {
		rv = ReallyInit();
		NS_ENSURE_SUCCESS(rv, rv);
	}
	*aPrivate = mPrivate;

	return NS_OK;
}

static JSBool
SealObject(JSContext *cx, JSObject *obj, uintN level, JSString **exclude)
{
	JSBool ok = JS_SealObject(cx, obj, JS_FALSE);
	if (ok && (level < 10)) {
		JSIdArray *ida = JS_Enumerate(cx, obj);
		if (ida) {
			for (jsint i = 0, n = ida->length; i < n; i++) {
#ifdef DEBUG_SEAL				
				PRBool debugProcessed = PR_FALSE;
				static const char * debugLevelStr[] = {
					".", "..", "...", "....", ".....", "......", ".......", "........", ".........", ".........."
				};
#endif
				jsval v;
				JS_IdToValue( cx, ida->vector[i], &v );
				JSString *jstr = JS_ValueToString( cx, v );
				if (exclude) {
					JSString **pstr = exclude;
					while (*pstr) {
						if (!JS_CompareStrings(jstr, *pstr)) {
							break;
						}
						++pstr;
					}
					if (*pstr) {
#ifdef DEBUG_SEAL
						gJaxerLog.Log(eDEBUG, "Exclude Jaxer%s%s", debugLevelStr[level], NS_LossyConvertUTF16toASCII(JS_GetStringChars(jstr)));
#endif
						continue;
					}
				}

				JSProperty *prop = nsnull;
				JSObject *pobj;
				if (OBJ_LOOKUP_PROPERTY(cx, obj, ida->vector[i], &pobj, &prop) && prop) {
					uintN attrs;
					if (OBJ_GET_ATTRIBUTES(cx, obj, ida->vector[i], prop, &attrs) ) {
						OBJ_DROP_PROPERTY(cx, obj, prop);
						if ( (attrs & (JSPROP_GETTER|JSPROP_SETTER)) == 0 ) {
							if (OBJ_GET_PROPERTY(cx, obj, ida->vector[i], &v) && v && JSVAL_IS_OBJECT(v)) {
								pobj = JSVAL_TO_OBJECT(v);
								if (OBJ_IS_NATIVE(pobj) && !JS_ObjectIsFunction(cx, pobj)) {
#ifdef DEBUG_SEAL				
									gJaxerLog.Log(eDEBUG, "Sealing  Jaxer%s%s", debugLevelStr[level], NS_LossyConvertUTF16toASCII(JS_GetStringChars(jstr)));
									debugProcessed = PR_TRUE;
#endif
									ok = SealObject(cx, pobj, level+1, nsnull);
								}
							}
						}
					}
				}
#ifdef DEBUG_SEAL
				if (!debugProcessed) {
					gJaxerLog.Log(eDEBUG, "Skipping Jaxer%s%s", debugLevelStr[level], NS_LossyConvertUTF16toASCII(JS_GetStringChars(jstr)));
				}
#endif
			}
			JS_DestroyIdArray(cx, ida);
		} else {
			ok = JS_FALSE;
		}
	}
	return ok;
}

JS_STATIC_DLL_CALLBACK(JSBool)
JaxerGlobalSeal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    // verify that the current call is from system 
#ifndef DEBUG_SEAL				
	uint32 fileFlags = JS_GetTopScriptFilenameFlags(cx, nsnull);
	if (!(fileFlags & JSFILENAME_SYSTEM)) {
		*rval = JSVAL_VOID;
		return JS_TRUE;
	}
#endif
	JSString **exclude = nsnull;
	if ((argc >= 1) && JSVAL_IS_OBJECT(argv[0])) {
		JSObject *arg0 = JSVAL_TO_OBJECT(argv[0]);
		jsuint length = 0;
		if (JS_IsArrayObject(cx, arg0) &&
			JS_GetArrayLength(cx, arg0, &length)) {
			exclude = (JSString**)JS_malloc(cx, (length+1)*sizeof(JSString*));
			for (jsuint i = 0, j = 0; i < length; ++i) {
				jsval v;
				exclude[i] = nsnull;
				if (JS_GetElement(cx, arg0, i, &v) && JSVAL_IS_STRING(v)) {
					exclude[j++] = JSVAL_TO_STRING(v);
				}
			}
			exclude[length] = nsnull;
		}
	}

	JSBool ok = SealObject(cx, obj, 0, exclude);
	*rval = BOOLEAN_TO_JSVAL(ok);
	return JS_TRUE;
}

JS_STATIC_DLL_CALLBACK(JSBool)
JaxerGlobal_private_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	nsresult rv;
	uint32 fileFlags = JS_GetTopScriptFilenameFlags(cx, nsnull);
	if (!(fileFlags & JSFILENAME_SYSTEM)) {
		*vp = JSVAL_VOID;
		return JS_TRUE;
	}
	nsCOMPtr<aptIJaxerGlobal> jxg = do_GetService(aptIJaxerGlobal::GetCID());
	NS_ENSURE_TRUE(jxg, JS_FALSE);

	JSObject *priv;
	rv = jxg->GetPrivate(&priv);
	NS_ENSURE_SUCCESS(rv, JS_FALSE);

	*vp = OBJECT_TO_JSVAL(priv);
	return JS_TRUE;
}

JS_STATIC_DLL_CALLBACK(JSBool)
JaxerGlobal_pageWindow_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    nsCOMPtr<aptIDocumentFetcherService> dfs(do_GetService("@aptana.com/httpdocumentfetcher;1"));
	NS_ENSURE_TRUE(dfs, JS_FALSE);

	nsCOMPtr<nsIDocShell> docShell;
	dfs->GetDocShell(getter_AddRefs(docShell));
	NS_ENSURE_TRUE(docShell, JS_FALSE);
	nsCOMPtr<nsIDOMWindow> window(do_GetInterface(docShell));
	NS_ENSURE_TRUE(window, JS_FALSE);

	nsCOMPtr<nsIScriptGlobalObject> sgo(do_GetInterface(docShell));
	NS_ENSURE_TRUE(sgo, JS_FALSE);

	nsCOMPtr<nsIXPConnect> xpc = do_GetService(nsIXPConnect::GetCID());
	NS_ENSURE_TRUE(xpc, JS_FALSE);
	
	JSAutoRequest ar(cx);
	nsCOMPtr<nsIXPConnectJSObjectHolder> wrapper;
	xpc->WrapNative(cx, sgo->GetGlobalJSObject(), window,
					NS_GET_IID(nsIDOMWindow),
					getter_AddRefs(wrapper));
	JSObject* wrapper_jsobj = nsnull;
	wrapper->GetJSObject(&wrapper_jsobj);
	NS_ASSERTION(wrapper_jsobj, "could not get jsobject of wrapped native");
	*vp = OBJECT_TO_JSVAL(wrapper_jsobj);
	return JS_TRUE;
}

enum jaxer_tinyid {
	JAXER_PRIVATE = -1,
	JAXER_PAGEWINDOW = -2,
	JAXER_BUILDNUMBER = -3
};

JS_STATIC_DLL_CALLBACK(JSBool)
JaxerGlobal_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (!JSVAL_IS_INT(id))
		return JS_TRUE;
	
	switch (JSVAL_TO_INT(id)) {
	case JAXER_BUILDNUMBER:
		*vp = nsJSUtils::ConvertStringToJSVal(NS_LITERAL_STRING(JAXER_BUILD_ID), cx);
		break;
	}
	return JS_TRUE;
}

static JSClass jaxerClass = {
	"Jaxer", 0,
	JS_PropertyStub, JS_PropertyStub, JaxerGlobal_getProperty, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSFunctionSpec jaxer_static_methods[] = {
  { "seal",		JaxerGlobalSeal,	1,0,0 },
  { nsnull,		nsnull,				0,0,0 }
};


static JSPropertySpec jaxer_static_props[] = {
    { "private",	JAXER_PRIVATE,	JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_SHARED,
				JaxerGlobal_private_getter,	JaxerGlobal_private_getter },
    { "pageWindow",	JAXER_PAGEWINDOW,	JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_SHARED,
				JaxerGlobal_pageWindow_getter,	JaxerGlobal_pageWindow_getter },
    { "buildNumber",	JAXER_BUILDNUMBER,	JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_ENUMERATE,
				nsnull,	nsnull },
    { nsnull,	0,	0,
				nsnull,	nsnull}
};

nsresult
aptJaxerGlobal::ReallyInit()
{
	nsresult rv;
	
	mRuntimeService = do_GetService("@mozilla.org/js/xpc/RuntimeService;1");
	NS_ENSURE_TRUE(mRuntimeService, NS_ERROR_FAILURE);

	rv = mRuntimeService->GetRuntime(&mRuntime);
	NS_ENSURE_SUCCESS(rv, rv);

	nsCOMPtr<nsIXPConnect> xpc = do_GetService(nsIXPConnect::GetCID());
	NS_ENSURE_TRUE(xpc, NS_ERROR_FAILURE);

	// Create our compilation context.
	mContext = JS_NewContext(mRuntime, 256);
	NS_ENSURE_TRUE(mContext, NS_ERROR_OUT_OF_MEMORY);

	uint32 options = JS_GetOptions(mContext);
	JS_SetOptions(mContext, options | JSOPTION_XML);

	// Always use the latest js version
	JS_SetVersion(mContext, JSVERSION_LATEST);
	
	nsCOMPtr<nsIXPCScriptable> backstagePass;
	rv = mRuntimeService->GetBackstagePass(getter_AddRefs(backstagePass));
	NS_ENSURE_SUCCESS(rv, rv);

	// Make sure InitClassesWithNewWrappedGlobal() installs the
	// backstage pass as the global in our compilation context.
	JS_SetGlobalObject(mContext, nsnull);

	nsCOMPtr<nsIXPConnectJSObjectHolder> holder;
	rv = xpc->InitClassesWithNewWrappedGlobal(mContext, backstagePass,
					NS_GET_IID(nsISupports),
					nsIXPConnect::FLAG_SYSTEM_GLOBAL_OBJECT,
					getter_AddRefs(holder));
    NS_ENSURE_SUCCESS(rv, rv);

    rv = holder->GetJSObject(&mGlobal);
    NS_ENSURE_SUCCESS(rv, rv);	
	NS_ENSURE_TRUE(mGlobal, NS_ERROR_FAILURE);

    JSAutoRequest ar(mContext);
    mJaxerProto = JS_InitClass(mContext, mGlobal, nsnull, &jaxerClass, 0, nsnull,
							nsnull, nsnull, jaxer_static_props, jaxer_static_methods);
	NS_ENSURE_TRUE(mJaxerProto, NS_ERROR_FAILURE);
	JS_SetParent(mContext, mJaxerProto, nsnull);

	JS_AddNamedRoot(mContext, &mGlobal, "JaxerGlobal");

	mPrivate = JS_NewObjectWithGivenProto(mContext, nsnull, nsnull, nsnull);
	NS_ENSURE_TRUE(mPrivate, NS_ERROR_FAILURE);
	JS_SetParent(mContext, mPrivate, nsnull);
	JS_AddNamedRoot(mContext, &mPrivate, "JaxerPrivate");

	// Jaxer.private.OS = <OS_TARGET>
	JSBool ok = JS_DefineProperty(mContext, mPrivate, "OS",
					nsJSUtils::ConvertStringToJSVal(NS_LITERAL_STRING(OS_TARGET), mContext),
					nsnull, nsnull,
					JSPROP_READONLY | JSPROP_PERMANENT);
	NS_ENSURE_TRUE(ok, NS_ERROR_FAILURE);

	mInitialized = PR_TRUE;
	return NS_OK;
}


aptJaxerGlobalOwner::aptJaxerGlobalOwner()
 : mScriptObject(nsnull)
{
}

aptJaxerGlobalOwner::~aptJaxerGlobalOwner()
{
}

NS_IMPL_THREADSAFE_ISUPPORTS1(aptJaxerGlobalOwner, nsIScriptObjectOwner)

NS_IMETHODIMP
aptJaxerGlobalOwner::GetScriptObject(nsIScriptContext *aContext, void** aScriptObject)
{
	NS_PRECONDITION(nsnull != aScriptObject, "null arg");
	nsresult rv = NS_OK;

	if (nsnull == mScriptObject)
	{
		rv = NewJaxerGlobalObject(aContext,
								aContext->GetGlobalObject(),
								&mScriptObject);
	}

	*aScriptObject = mScriptObject;
	return rv;
}

NS_IMETHODIMP
aptJaxerGlobalOwner::SetScriptObject(void *aScriptObject)
{
	mScriptObject = aScriptObject;
	return NS_OK;
}

static nsresult
NewJaxerGlobalObject(nsIScriptContext *aContext, nsISupports *aParent, void **aReturn)
{
	NS_PRECONDITION(nsnull != aContext && nsnull != aReturn,
		"null argument to NewJaxerGlobalObject");

	nsresult rv;
	JSObject *proto;
	JSObject *parent = nsnull;
	JSContext *cx = (JSContext *)aContext->GetNativeContext();
	
	nsCOMPtr<nsIScriptObjectOwner> owner(do_QueryInterface(aParent));
	
	if (owner) {
		rv = owner->GetScriptObject(aContext, (void **)&parent);
		NS_ENSURE_SUCCESS(rv, rv);
	} else {
		nsCOMPtr<nsIScriptGlobalObject> sgo(do_QueryInterface(aParent));
		NS_ENSURE_TRUE(sgo, NS_ERROR_FAILURE);
		parent = sgo->GetGlobalJSObject();
	}
	
	nsCOMPtr<aptIJaxerGlobal> jxg = do_GetService(aptIJaxerGlobal::GetCID());
	NS_ENSURE_TRUE(jxg, NS_ERROR_FAILURE);

	rv = jxg->GetGlobal(&proto);
	NS_ENSURE_SUCCESS(rv, rv);

    JSAutoRequest ar(cx);
	JSObject *obj = JS_NewObject(cx, nsnull, proto, parent);
	NS_ENSURE_TRUE(obj, NS_ERROR_FAILURE);

	*aReturn = obj;
	return NS_OK;
}
