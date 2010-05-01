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
 * The Original Code is the IDispatch implementation for XPConnect.
 *
 * The Initial Developer of the Original Code is
 * David Bradley.
 * Portions created by the Initial Developer are Copyright (C) 2002
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

/** \file XPCDispInterface.cpp
 * XPCDispInterface implementation
 * This file contains the implementation of the XPCDispInterface class
 */

#include "xpcprivate.h"

/**
 * Is this function reflectable
 * This function determines if we should reflect a particular method of an
 * interface
 */
inline
PRBool IsReflectable(FUNCDESC * pFuncDesc)
{
    return (pFuncDesc->wFuncFlags&FUNCFLAG_FRESTRICTED) == 0 &&
           pFuncDesc->funckind == FUNC_DISPATCH || 
           pFuncDesc->funckind == FUNC_PUREVIRTUAL ||
           pFuncDesc->funckind == FUNC_VIRTUAL;
}

XPCDispInterface::Allocator::Allocator(JSContext * cx, ITypeInfo * pTypeInfo) :
    mMemIDs(nsnull), mCount(0), mIDispatchMembers(0), mCX(cx), 
    mTypeInfo(pTypeInfo)
{
    TYPEATTR * attr;
    HRESULT hr = pTypeInfo->GetTypeAttr(&attr);
    if(SUCCEEDED(hr))
    {
        mIDispatchMembers = attr->cFuncs;
        mMemIDs = new DISPID[mIDispatchMembers];
        pTypeInfo->ReleaseTypeAttr(attr);
        // Bail if we couldn't create the buffer
        if(!mMemIDs)
            return;
    }
    for(UINT iMethod = 0; iMethod < mIDispatchMembers; iMethod++ )
    {
        FUNCDESC* pFuncDesc;
        if(SUCCEEDED(pTypeInfo->GetFuncDesc(iMethod, &pFuncDesc)))
        {
            // Only add the function to our list if it is at least at nesting level
            // 2 (i.e. defined in an interface derived from IDispatch).
            if(IsReflectable(pFuncDesc))
                Add(pFuncDesc->memid);
            pTypeInfo->ReleaseFuncDesc(pFuncDesc);
        }
    }
}

void XPCDispInterface::Allocator::Add(DISPID memID)
{
    NS_ASSERTION(Valid(), "Add should never be called if out of memory");
    // Start from the end and work backwards, the last item is the most
    // likely to match
    PRUint32 index = mCount;
    while(index > 0)
    {
        if(mMemIDs[--index] == memID)
            return;
    };
    NS_ASSERTION(Count() < mIDispatchMembers, "mCount should always be less "
                                             "than the IDispatch member count "
                                             "here");
    mMemIDs[mCount++] = memID;
    return;
}

inline
PRUint32 XPCDispInterface::Allocator::Count() const 
{
    return mCount;
}

XPCDispInterface*
XPCDispInterface::NewInstance(JSContext* cx, nsISupports * pIface)
{
    CComQIPtr<IDispatch> pDispatch(reinterpret_cast<IUnknown*>(pIface));

    if(pDispatch)
    {
        unsigned int count;
        HRESULT hr = pDispatch->GetTypeInfoCount(&count);
        if(SUCCEEDED(hr) && count > 0)
        {
            CComPtr<ITypeInfo> pTypeInfo;
            hr = pDispatch->GetTypeInfo(0,LOCALE_SYSTEM_DEFAULT, &pTypeInfo);
            if(SUCCEEDED(hr))
            {
                Allocator allocator(cx, pTypeInfo);
                return allocator.Allocate();
            }
        }
    }
    return nsnull;
}

/**
 * Sets a members type based on COM's INVOKEKIND
 */
static
void ConvertInvokeKind(INVOKEKIND invokeKind, XPCDispInterface::Member & member)
{
    switch (invokeKind)
    {
        case INVOKE_FUNC:
        {
            member.SetFunction();
        }
        break;
        case INVOKE_PROPERTYGET:
        {
            member.MakeGetter();
        }
        break;
        case INVOKE_PROPERTYPUT:
        {
            member.MakeSetter();
        }
        break;
        // TODO: Handle putref
        default:
        {
            NS_ERROR("Invalid invoke kind found in COM type info");
        }
        break;
    }
}

static
PRBool InitializeMember(JSContext * cx, ITypeInfo * pTypeInfo,
                        FUNCDESC * pFuncDesc, 
                        XPCDispInterface::Member * pInfo)
{
    pInfo->SetMemID(pFuncDesc->memid);
    BSTR name;
    UINT nameCount;
    if(FAILED(pTypeInfo->GetNames(
        pFuncDesc->memid,
        &name,
        1,
        &nameCount)))
        return PR_FALSE;
    if(nameCount != 1)
        return PR_FALSE;
    JSString* str = JS_InternUCStringN(cx,
                                       reinterpret_cast<const jschar *>(name),
                                       ::SysStringLen(name));
    ::SysFreeString(name);
    if(!str)
        return PR_FALSE;
    // Initialize
    pInfo = new (pInfo) XPCDispInterface::Member;
    if(!pInfo)
        return PR_FALSE;
    pInfo->SetName(STRING_TO_JSVAL(str));
    pInfo->ResetType();
    ConvertInvokeKind(pFuncDesc->invkind, *pInfo);
    pInfo->SetTypeInfo(pFuncDesc->memid, pTypeInfo, pFuncDesc);
    return PR_TRUE;
}

static
XPCDispInterface::Member * FindExistingMember(XPCDispInterface::Member * first,
                                              XPCDispInterface::Member * last,
                                              MEMBERID memberID)
{
    // Iterate backward since the last one in is the most likely match
    XPCDispInterface::Member * cur = last;
    if (cur != first)
    {
        do 
        {
            --cur;
            if(cur->GetMemID() == memberID)
                return cur;
        } while(cur != first);
    } 
    // no existing property, return the new one
    return last;
}

PRBool XPCDispInterface::InspectIDispatch(JSContext * cx, ITypeInfo * pTypeInfo, PRUint32 members)
{
    HRESULT hResult;

    XPCDispInterface::Member * pInfo = mMembers;
    mMemberCount = 0;
    for(PRUint32 index = 0; index < members; index++ )
    {
        FUNCDESC* pFuncDesc;
        hResult = pTypeInfo->GetFuncDesc(index, &pFuncDesc );
        if(FAILED(hResult))
            continue;
        if(IsReflectable(pFuncDesc))
        {
            switch(pFuncDesc->invkind)
            {
                case INVOKE_PROPERTYPUT:
                case INVOKE_PROPERTYPUTREF:
                case INVOKE_PROPERTYGET:
                {
                    XPCDispInterface::Member * pExisting = FindExistingMember(mMembers, pInfo, pFuncDesc->memid);
                    if(pExisting == pInfo)
                    {
                        if(InitializeMember(cx, pTypeInfo, pFuncDesc, pInfo))
                        {
                            ++pInfo;
                            ++mMemberCount;
                        }
                    }
                    else
                    {
                        ConvertInvokeKind(pFuncDesc->invkind, *pExisting);
                    }
                    if(pFuncDesc->invkind == INVOKE_PROPERTYGET)
                    {
                        pExisting->SetGetterFuncDesc(pFuncDesc);
                    }
                }
                break;
                case INVOKE_FUNC:
                {
                    if(InitializeMember(cx, pTypeInfo, pFuncDesc, pInfo))
                    {
                        ++pInfo;
                        ++mMemberCount;
                    }
                }
                break;
                default:
                    pTypeInfo->ReleaseFuncDesc(pFuncDesc);
                break;
            }
        }
        else
        {
            pTypeInfo->ReleaseFuncDesc(pFuncDesc);
        }
    }
    return PR_TRUE;
}

/**
 * Compares a PRUnichar and a JS string ignoring case
 * @param ccx an XPConnect call context
 * @param lhr the PRUnichar string to be compared
 * @param lhsLength the length of the PRUnichar string
 * @param rhs the JS value that is the other string to compare
 * @return true if the strings are equal
 */
inline
PRBool CaseInsensitiveCompare(XPCCallContext& ccx, const PRUnichar* lhs, size_t lhsLength, jsval rhs)
{
    if(lhsLength == 0)
        return PR_FALSE;
    size_t rhsLength;
    PRUnichar* rhsString = xpc_JSString2PRUnichar(ccx, rhs, &rhsLength);
    return rhsString && 
        lhsLength == rhsLength &&
        _wcsnicmp(lhs, rhsString, lhsLength) == 0;
}

const XPCDispInterface::Member* XPCDispInterface::FindMemberCI(XPCCallContext& ccx, jsval name) const
{
    size_t nameLength;
    PRUnichar* sName = xpc_JSString2PRUnichar(ccx, name, &nameLength);
    if(!sName)
        return nsnull;
    // Iterate backwards over the members array (more efficient)
    const Member* member = mMembers + mMemberCount;
    while(member > mMembers)
    {
        --member;
        if(CaseInsensitiveCompare(ccx, sName, nameLength, member->GetName()))
        {
            return member;
        }
    }
    return nsnull;
}

JSBool XPCDispInterface::Member::GetValue(XPCCallContext& ccx,
                                          XPCNativeInterface * iface, 
                                          jsval * retval) const
{
    // This is a method or attribute - we'll be needing a function object

    // We need to use the safe context for this thread because we don't want
    // to parent the new (and cached forever!) function object to the current
    // JSContext's global object. That would be bad!
    if((mType & RESOLVED) == 0)
    {
        JSContext* cx = ccx.GetSafeJSContext();
        if(!cx)
            return JS_FALSE;

        intN argc;
        intN flags;
        JSNative callback;
        // Is this a function or a parameterized getter/setter
        if(IsFunction() || IsParameterizedProperty())
        {
            argc = GetParamCount();
            flags = 0;
            callback = XPC_IDispatch_CallMethod;
        }
        else
        {
            if(IsSetter())
            {
                flags = JSFUN_GETTER | JSFUN_SETTER;
            }
            else
            {
                flags = JSFUN_GETTER;
            }
            argc = 0;
            callback = XPC_IDispatch_GetterSetter;
        }

#ifdef JAXER
        JSAutoRequest ar(cx);
#endif
        JSFunction *fun = JS_NewFunction(cx, callback, argc, flags, nsnull,
                                         JS_GetStringBytes(JSVAL_TO_STRING(mName)));
        if(!fun)
            return JS_FALSE;

        JSObject* funobj = JS_GetFunctionObject(fun);
        if(!funobj)
            return JS_FALSE;

        // Store ourselves and our native interface within the JSObject
        if(!JS_SetReservedSlot(ccx, funobj, 0, PRIVATE_TO_JSVAL(this)))
            return JS_FALSE;

        if(!JS_SetReservedSlot(ccx, funobj, 1, PRIVATE_TO_JSVAL(iface)))
            return JS_FALSE;

        {   // scoped lock
            XPCAutoLock lock(ccx.GetRuntime()->GetMapLock());
            const_cast<Member*>(this)->mVal = OBJECT_TO_JSVAL(funobj);
            const_cast<Member*>(this)->mType |= RESOLVED;
        }
    }
    *retval = mVal;
    return JS_TRUE;
}
