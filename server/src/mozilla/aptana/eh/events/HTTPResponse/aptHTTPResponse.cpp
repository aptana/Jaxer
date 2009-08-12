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

#include "nsString.h"
#include "nsILocalFile.h"
#include "aptHTTPResponse.h"

////////////////////////////////////////////////////////////////////////////////

aptHTTPResponse::aptHTTPResponse() 
{
	mStatusCode = 200;
	mStatusPhrase.AppendASCII("OK");
	mShouldUseOriginalContent = PR_FALSE;
	mFrameworkFailed = PR_TRUE;
	mHasContents = PR_FALSE;
	mContentsAsDocument = PR_FALSE;
}

////////////////////////////////////////////////////////////////////////////////

aptHTTPResponse::~aptHTTPResponse()
{
}

////////////////////////////////////////////////////////////////////////////////

NS_IMPL_ISUPPORTS1(aptHTTPResponse, aptIHTTPResponse)


NS_IMETHODIMP 
aptHTTPResponse::GetStatusCode(PRInt32 *aStatusCode)
{
	*aStatusCode = mStatusCode;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPResponse::SetStatusCode(PRInt32 aStatusCode)
{
	mStatusCode = aStatusCode;
    return NS_OK;
}


NS_IMETHODIMP 
aptHTTPResponse::GetStatusPhrase(nsAString &aStatusPhrase)
{
	aStatusPhrase.Assign(mStatusPhrase);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPResponse::SetStatusPhrase(const nsAString &aStatusPhrase)
{
	mStatusPhrase.Assign(aStatusPhrase);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPResponse::GetShouldUseOriginalContent(PRBool *aUseOriginal)
{
    *aUseOriginal = mShouldUseOriginalContent;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPResponse::SetShouldUseOriginalContent(PRBool aUseOriginal)
{
    mShouldUseOriginalContent = aUseOriginal;
    return NS_OK;
}

/* void setContents (in AString contents); */
NS_IMETHODIMP
aptHTTPResponse::SetContents(const nsAString & aContents)
{
	CopyUTF16toUTF8(aContents, mContents);
	mHasContents = PR_TRUE;
	mPipeFile = nsnull;
    return NS_OK;
}

/* void setBinaryContents ([array, size_is (aLength)] in PRUint8 aBytes, in PRUint32 aLength); */
NS_IMETHODIMP
aptHTTPResponse::SetBinaryContents(PRUint8 *aBytes, PRUint32 aLength)
{
	mContents.AssignASCII(reinterpret_cast<char *>(aBytes), aLength);
	mHasContents = PR_TRUE;
	mPipeFile = nsnull;
    return NS_OK;
}

/* void SendFile (in AString aPath); */
NS_IMETHODIMP
aptHTTPResponse::SendFile(const nsAString & aPath)
{
	nsresult rv;
	nsCOMPtr<nsILocalFile> file;
	rv = NS_NewLocalFile(aPath, PR_TRUE, getter_AddRefs(file));
    NS_ENSURE_SUCCESS(rv, rv);

	PRBool exists = PR_FALSE;
	if ( NS_FAILED(file->Exists(&exists)) || !exists) {
		return NS_ERROR_FILE_NOT_FOUND;
	}
	PRBool isfile = PR_FALSE;
	if ( NS_FAILED(file->IsFile(&isfile)) || !isfile) {
		return NS_ERROR_FILE_IS_DIRECTORY;
	}

	mPipeFile = file;
	mHasContents = PR_TRUE;
    return NS_OK;
}

  /* [noscript] readonly attribute boolean hasContents; */
NS_IMETHODIMP
aptHTTPResponse::GetHasContents(PRBool *aHasContents)
{
	*aHasContents = mHasContents;
    return NS_OK;
}

/* [noscript] readonly attribute ACString contents; */
NS_IMETHODIMP
aptHTTPResponse::GetContents(nsACString & aContents)
{
	aContents.Assign(mContents);
    return NS_OK;
}

/* [noscript] readonly attribute nsIFile pipeFile; */
NS_IMETHODIMP
aptHTTPResponse::GetPipeFile(nsIFile * *aPipeFile)
{
	NS_ENSURE_ARG_POINTER(aPipeFile);
	NS_IF_ADDREF(*aPipeFile = mPipeFile);
    return NS_OK;
}

/* int getHeaderCount(); */
NS_IMETHODIMP
aptHTTPResponse::GetHeaderCount(PRInt32 *retv)
{
	*retv = mHeaderNames.Count();
	return NS_OK;
}

/* AString getHeaderName(in long index); */
NS_IMETHODIMP
aptHTTPResponse::GetHeaderName(PRInt32 ord, nsAString &retv)
{
	if (ord >= mHeaderNames.Count() || ord < 0)
		retv.Truncate();
	else
		mHeaderNames.StringAt(ord, retv);

	return NS_OK;
}

/* AString getValueByName(in AString name); */
NS_IMETHODIMP
aptHTTPResponse::GetValueByName(const nsAString &name, nsAString &retv)
{
	PRInt32 cnt = mHeaderNames.Count();
	for (PRInt32 i = 0; i < cnt; i++) {
		if (mHeaderNames[i]->Equals(name)) {
			mHeaderValues.StringAt(i, retv);
			return NS_OK;
		}
	}

	retv.Truncate();
	return NS_OK;
}

/* AString getValueByOrd(in int index); */
NS_IMETHODIMP
aptHTTPResponse::GetValueByOrd(PRInt32 ord, nsAString &retv)
{
	if (ord >= mHeaderNames.Count() || ord < 0)
		retv.Truncate();
	else
		mHeaderValues.StringAt(ord, retv);

	return NS_OK;
}

/* boolean hasHeader(in AString name); */
NS_IMETHODIMP
aptHTTPResponse::HasHeader(const nsAString &name, PRBool *retv)
{
	PRInt32 cnt = mHeaderNames.Count();
	for (PRInt32 i = 0; i < cnt; i++) {
		if (mHeaderNames[i]->Equals(name)) {
			*retv = PR_TRUE;
			return NS_OK;
		}
	}

	*retv = PR_FALSE;
	return NS_OK;
}

/* void addHeader(in AString name, in AString value, in boolean del); */
NS_IMETHODIMP
aptHTTPResponse::AddHeader(const nsAString &name, const nsAString &value, PRBool del)
{
	if (del)
		DeleteHeader(name);

	mHeaderNames.AppendString(name);
	mHeaderValues.AppendString(value);
	return NS_OK;
}

/* void deleteHeader(in AString name); */
NS_IMETHODIMP
aptHTTPResponse::DeleteHeader(const nsAString &name)
{
	PRInt32 cnt = mHeaderNames.Count();
	for (PRInt32 i = 0; i < cnt; i++) {
		if (mHeaderNames[i]->Equals(name)) {
			mHeaderNames.RemoveStringAt(i);
			mHeaderValues.RemoveStringAt(i);
			i--, cnt--;
		}
	}

	return NS_OK;
}

NS_IMETHODIMP 
aptHTTPResponse::GetFrameworkFailed(PRInt32 *aValue)
{
    *aValue = mFrameworkFailed;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPResponse::SetFrameworkFailed(PRInt32 aValue)
{
    mFrameworkFailed = aValue;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPResponse::GetContentsAsDocument(PRInt32 *aValue)
{
    *aValue = mContentsAsDocument;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPResponse::SetContentsAsDocument(PRInt32 aValue)
{
    mContentsAsDocument = aValue;
    return NS_OK;
}