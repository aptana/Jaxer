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

#include "nsComponentManagerUtils.h"
#include "aptEventScriptCompileData.h"

////////////////////////////////////////////////////////////////////////////////

aptEventScriptCompileData::aptEventScriptCompileData() 
{
	mEventBase = do_CreateInstance(APT_EVENT_BASE_CONTRACTID);
}

////////////////////////////////////////////////////////////////////////////////

aptEventScriptCompileData::~aptEventScriptCompileData() 
{
}


////////////////////////////////////////////////////////////////////////////////

NS_IMPL_ISUPPORTS1(aptEventScriptCompileData, aptIEventScriptCompile)


/* attribute ACString scriptURI; */
NS_IMETHODIMP
aptEventScriptCompileData::GetScriptURI(nsACString & aScriptURI)
{
	aScriptURI.Assign(mScriptURI);
    return NS_OK;
}
NS_IMETHODIMP
aptEventScriptCompileData::SetScriptURI(const nsACString & aScriptURI)
{
	mScriptURI.Assign(aScriptURI);
    return NS_OK;
}

/* attribute unsigned long lineNo; */
NS_IMETHODIMP
aptEventScriptCompileData::GetLineNo(PRUint32 *aLineNo)
{
	NS_ENSURE_ARG_POINTER(aLineNo);
	*aLineNo = mLineNo;
    return NS_OK;
}
NS_IMETHODIMP
aptEventScriptCompileData::SetLineNo(PRUint32 aLineNo)
{
	mLineNo = aLineNo;
    return NS_OK;
}

/* attribute AString scriptText; */
NS_IMETHODIMP
aptEventScriptCompileData::GetScriptText(nsAString & aScriptText)
{
	aScriptText.Assign(mScriptText);
    return NS_OK;
}
NS_IMETHODIMP
aptEventScriptCompileData::SetScriptText(const nsAString & aScriptText)
{
	mScriptText.Assign(aScriptText);
    return NS_OK;
}

