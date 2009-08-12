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

#ifndef __APT_JAXERGLOBAL_H__
#define __APT_JAXERGLOBAL_H__

#include "aptIJaxerGlobal.h"
#include "jsapi.h"
#include "nsIJSRuntimeService.h"
#include "nsIScriptObjectOwner.h"

class aptJaxerGlobal : public aptIJaxerGlobal 
{
public:
    aptJaxerGlobal();

    NS_DECL_ISUPPORTS
	NS_DECL_APTIJAXERGLOBAL

private:
    ~aptJaxerGlobal();
    nsresult ReallyInit();

protected:
    static aptJaxerGlobal* sSelf;

private:
    nsCOMPtr<nsIJSRuntimeService> mRuntimeService;
    JSRuntime *mRuntime;
    JSContext *mContext;
	
	JSObject *mGlobal;
	JSObject *mJaxerProto;
	JSObject *mPrivate;

	PRBool mInitialized;
};


class aptJaxerGlobalOwner : public nsIScriptObjectOwner 
{
public:
    aptJaxerGlobalOwner();

    NS_DECL_ISUPPORTS
	
	NS_IMETHOD GetScriptObject(nsIScriptContext *aContext, void** aScriptObject);
	NS_IMETHOD SetScriptObject(void* aScriptObject);

private:
    ~aptJaxerGlobalOwner();

private:
	void *mScriptObject;

};

// {54FA19BA-59A5-4522-91DF-04611F99186D}
#define APT_JAXERGLOBALOWNER_CID \
 { 0x54fa19ba, 0x59a5, 0x4522, \
 { 0x91, 0xdf, 0x4, 0x61, 0x1f, 0x99, 0x18, 0x6d } }
#define APT_JAXERGLOBALOWNER_CONTRACTID \
"@aptana.com/jaxer/object;1"

#endif // __APT_JAXERGLOBAL_H__

