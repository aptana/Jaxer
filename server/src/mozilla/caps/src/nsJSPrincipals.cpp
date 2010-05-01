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
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1999
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#include "nsString.h"
#include "nsIObjectOutputStream.h"
#include "nsIObjectInputStream.h"
#include "nsJSPrincipals.h"
#include "plstr.h"
#include "nsXPIDLString.h"
#include "nsCOMPtr.h"
#include "jsapi.h"
#include "jsxdrapi.h"
#include "nsIJSRuntimeService.h"
#include "nsIServiceManager.h"
#include "nsMemory.h"
#include "nsStringBuffer.h"
#ifdef JAXER
#include "nsScriptSecurityManager.h"
#endif /* JAXER */

JS_STATIC_DLL_CALLBACK(void *)
nsGetPrincipalArray(JSContext *cx, JSPrincipals *prin)
{
    return nsnull;
}

JS_STATIC_DLL_CALLBACK(JSBool)
nsGlobalPrivilegesEnabled(JSContext *cx, JSPrincipals *jsprin)
{
    return JS_TRUE;
}

JS_STATIC_DLL_CALLBACK(JSBool)
nsJSPrincipalsSubsume(JSPrincipals *jsprin, JSPrincipals *other)
{
    nsJSPrincipals *nsjsprin = static_cast<nsJSPrincipals *>(jsprin);
    nsJSPrincipals *nsother  = static_cast<nsJSPrincipals *>(other);

    JSBool result;
    nsresult rv = nsjsprin->nsIPrincipalPtr->Subsumes(nsother->nsIPrincipalPtr,
                                                      &result);
    return NS_SUCCEEDED(rv) && result;
}

JS_STATIC_DLL_CALLBACK(void)
nsDestroyJSPrincipals(JSContext *cx, struct JSPrincipals *jsprin)
{
    nsJSPrincipals *nsjsprin = static_cast<nsJSPrincipals *>(jsprin);

    // We need to destroy the nsIPrincipal. We'll do this by adding
    // to the refcount and calling release

    // Note that we don't want to use NS_IF_RELEASE because it will try
    // to set nsjsprin->nsIPrincipalPtr to nsnull *after* nsjsprin has
    // already been destroyed.
#ifdef NS_BUILD_REFCNT_LOGGING
    // The refcount logging considers AddRef-to-1 to indicate creation,
    // so trick it into thinking it's otherwise, but balance the
    // Release() we do below.
    nsjsprin->refcount++;
    nsjsprin->nsIPrincipalPtr->AddRef();
    nsjsprin->refcount--;
#else
    nsjsprin->refcount++;
#endif
    nsjsprin->nsIPrincipalPtr->Release();
    // The nsIPrincipal that we release owns the JSPrincipal struct,
    // so we don't need to worry about "codebase"
}

JS_STATIC_DLL_CALLBACK(JSBool)
nsTranscodeJSPrincipals(JSXDRState *xdr, JSPrincipals **jsprinp)
{
    nsresult rv;

#ifdef JAXER
    if (!xdr->userdata) {
		nsCOMPtr<nsIPrincipal> principal;
		nsScriptSecurityManager::GetScriptSecurityManager()->GetSystemPrincipal(getter_AddRefs(principal));
		principal->GetJSPrincipals(xdr->cx, jsprinp);
		return JS_TRUE;
}
#endif /* JAXER */
    if (xdr->mode == JSXDR_ENCODE) {
        nsIObjectOutputStream *stream =
            reinterpret_cast<nsIObjectOutputStream*>(xdr->userdata);

        // Flush xdr'ed data to the underlying object output stream.
        uint32 size;
        char *data = (char*) ::JS_XDRMemGetData(xdr, &size);

        rv = stream->Write32(size);
        if (NS_SUCCEEDED(rv)) {
            rv = stream->WriteBytes(data, size);
            if (NS_SUCCEEDED(rv)) {
                ::JS_XDRMemResetData(xdr);

                // Require that GetJSPrincipals has been called already by the
                // code that compiled the script that owns the principals.
                nsJSPrincipals *nsjsprin =
                    static_cast<nsJSPrincipals*>(*jsprinp);

                rv = stream->WriteObject(nsjsprin->nsIPrincipalPtr, PR_TRUE);
            }
        }
    } else {
        NS_ASSERTION(JS_XDRMemDataLeft(xdr) == 0, "XDR out of sync?!");
        nsIObjectInputStream *stream =
            reinterpret_cast<nsIObjectInputStream*>(xdr->userdata);

        nsCOMPtr<nsIPrincipal> prin;
        rv = stream->ReadObject(PR_TRUE, getter_AddRefs(prin));
        if (NS_SUCCEEDED(rv)) {
            PRUint32 size;
            rv = stream->Read32(&size);
            if (NS_SUCCEEDED(rv)) {
                char *data = nsnull;
                if (size != 0)
                    rv = stream->ReadBytes(size, &data);
                if (NS_SUCCEEDED(rv)) {
                    char *olddata;
                    uint32 oldsize;

                    // Any decode-mode JSXDRState whose userdata points to an
                    // nsIObjectInputStream instance must use nsMemory to Alloc
                    // and Free its data buffer.  Swap the new buffer we just
                    // read for the old, exhausted data.
                    olddata = (char*) ::JS_XDRMemGetData(xdr, &oldsize);
                    nsMemory::Free(olddata);
                    ::JS_XDRMemSetData(xdr, data, size);

                    prin->GetJSPrincipals(xdr->cx, jsprinp);
                }
            }
        }
    }

    if (NS_FAILED(rv)) {
        ::JS_ReportError(xdr->cx, "can't %scode principals (failure code %x)",
                         (xdr->mode == JSXDR_ENCODE) ? "en" : "de",
                         (unsigned int) rv);
        return JS_FALSE;
    }
    return JS_TRUE;
}

nsresult
nsJSPrincipals::Startup()
{
    static const char rtsvc_id[] = "@mozilla.org/js/xpc/RuntimeService;1";
    nsCOMPtr<nsIJSRuntimeService> rtsvc(do_GetService(rtsvc_id));
    if (!rtsvc)
        return NS_ERROR_FAILURE;

    JSRuntime *rt;
    rtsvc->GetRuntime(&rt);
    NS_ASSERTION(rt != nsnull, "no JSRuntime?!");

    JSPrincipalsTranscoder oldpx;
    oldpx = ::JS_SetPrincipalsTranscoder(rt, nsTranscodeJSPrincipals);
    NS_ASSERTION(oldpx == nsnull, "oops, JS_SetPrincipalsTranscoder wars!");

    return NS_OK;
}

nsJSPrincipals::nsJSPrincipals()
{
    codebase = nsnull;
    getPrincipalArray = nsGetPrincipalArray;
    globalPrivilegesEnabled = nsGlobalPrivilegesEnabled;
    refcount = 0;
    destroy = nsDestroyJSPrincipals;
    subsume = nsJSPrincipalsSubsume;
    nsIPrincipalPtr = nsnull;
}

nsresult
nsJSPrincipals::Init(nsIPrincipal *aPrincipal, const nsCString& aCodebase)
{
    if (nsIPrincipalPtr) {
        NS_ERROR("Init called twice!");
        return NS_ERROR_UNEXPECTED;
    }

    nsIPrincipalPtr = aPrincipal;
    nsStringBuffer* buf = nsStringBuffer::FromString(aCodebase);
    char* data;
    if (buf) {
        buf->AddRef();
        data = static_cast<char*>(buf->Data());
    } else {
        PRUint32 len = aCodebase.Length();
        buf = nsStringBuffer::Alloc(len + 1); // addrefs
        if (!buf) {
            return NS_ERROR_OUT_OF_MEMORY;
        }
        data = static_cast<char*>(buf->Data());
        memcpy(data, aCodebase.get(), len);
        data[len] = '\0';
    }
    
    codebase = data;

    return NS_OK;
}

nsJSPrincipals::~nsJSPrincipals()
{
    if (codebase) {
        nsStringBuffer::FromData(codebase)->Release();
    }
}
