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
#include "aptHTTPRequest.h"
#include "nspr.h"
#include "plstr.h"
#include "nsIScriptObjectPrincipal.h"
#include "jsapi.h"
#include "jsstr.h"
#include "jsxdrapi.h"
#include "jsdbgapi.h"
#include "nsIJSContextStack.h"
#include "nsWebShell.h"
#include "nsIPrincipal.h"
#include "nsServiceManagerUtils.h"
#include "nsIXPConnect.h"
#include "nsIScriptSecurityManager.h"
#include "nsIDOMHTMLImageElement.h"
#include "nsImageLoadingContent.h"
#include "imgIContainer.h"
#include "aptIDocumentFetcherService.h"
#include "nsIStringStream.h"
#include "nsIFileStreams.h"
#include "nsILocalFile.h"
#include "nsContentCID.h"

#include "aptEventTypeManager.h"
#include "aptEventScriptCompileData.h"

//#define _TIMESTATS

#ifdef _TIMESTATS
#include "nsPrintfCString.h"
//#include "nsIConsoleService.h"
#include "aptCoreTrace.h"
extern aptCoreTrace gJaxerLog;
#endif /* _TIMESTATS */

NS_DEFINE_CID(kEventTypeManagerCID, APT_EVENTTYPEMANAGER_CID);
static aptEventTypeManager *g_EventTypeManager = nsnull;

static
aptEventTypeManager * GetEventTypeManager()
{
	if (!g_EventTypeManager) {
		nsresult rv;
		nsCOMPtr<aptEventTypeManager> eventTypeManager = do_GetService(kEventTypeManagerCID, &rv);
		g_EventTypeManager = eventTypeManager;
	}
	return g_EventTypeManager;
}


////////////////////////////////////////////////////////////////////////////////
aptPostDataItem::aptPostDataItem() //const nsAString name, const nsAString &value, const nsAString &ctype, PRUint32 size)
{
}
aptPostDataItem::~aptPostDataItem()
{
}
////////////////////////////////////////////////////////////////////////////////
aptPostDataItem::aptPostDataItem( const aptPostDataItem &other)
{
	mName = other.mName;
	mValue = other.mValue;
	/*
	mContentType = other.mContentType;
	mSize = other.mSize;
	mValue = static_cast<PRUint8*>(nsMemory::Alloc(mSize+1));
	memcpy(mValue, other.mValue, mSize);
	mValue[mSize] = 0;
	*/
}

aptPostDataItem& aptPostDataItem::operator=( const aptPostDataItem& other)
{
	if ( this != &other)
	{
		mName = other.mName;
		mValue = other.mValue;
		/*
		mContentType = other.mContentType;
		mSize = other.mSize;
		mValue = static_cast<PRUint8*>(nsMemory::Alloc(mSize+1));
		memcpy(mValue, other.mValue, mSize);
		mValue[mSize] = 0;
		*/
	}
	return *this;
}

void aptPostDataItem::SetData( const nsAString &name, const nsAString &value)
{
	mName = name;
	mValue = value;
	/*
	mContentType = ctype;
	mSize = size;
	mValue = static_cast<PRUint8*>(nsMemory::Alloc(mSize+1));
	memcpy(mValue, value, mSize);
	mValue[mSize] = 0;
	*/
}

////////////////////////////////////////////////////////////////////////////////
const nsString& aptPostDataItem::GetName() const
{
	return mName;
}

////////////////////////////////////////////////////////////////////////////////
const nsString& aptPostDataItem::GetValue() const
{
	return mValue;
}

#if 0
const nsString& aptPostDataItem::GetContentType() const
{
	return mContentType;
}

const PRUint32 aptPostDataItem::GetSize() const
{
	return mSize;
}
#endif

////////////////////////////////////////////////////////////////////////////////
aptPostFileItem::aptPostFileItem()
{
}

////////////////////////////////////////////////////////////////////////////////
aptPostFileItem::~aptPostFileItem()
{
	DeleteFile();
}
////////////////////////////////////////////////////////////////////////////////
aptPostFileItem::aptPostFileItem( const aptPostFileItem &other)
{
	mName = other.mName;
	mOriginalFileName = other.mOriginalFileName;
	mTmpFileName = other.mTmpFileName;
	mContentType = other.mContentType;
	mSize = other.mSize;
}

////////////////////////////////////////////////////////////////////////////////
aptPostFileItem& aptPostFileItem::operator=( const aptPostFileItem& other)
{
	if ( this != &other)
	{
		mName = other.mName;
		mOriginalFileName = other.mOriginalFileName;
		mTmpFileName = other.mTmpFileName;
		mContentType = other.mContentType;
		mSize = other.mSize;
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
void aptPostFileItem::SetData( const nsAString &name, const nsAString &filename, const nsAString &tmpFilename, 
	const nsAString &contentType, PRUint32 size)
{
	mName = name;
	mOriginalFileName = filename;
	mTmpFileName = tmpFilename;
	mContentType = contentType;
	mSize = size;
}

////////////////////////////////////////////////////////////////////////////////
const nsString& aptPostFileItem::GetName() const
{
	return mName;
}

////////////////////////////////////////////////////////////////////////////////
const nsString& aptPostFileItem::GetOriginalName() const
{
	return mOriginalFileName;
}

////////////////////////////////////////////////////////////////////////////////
const nsString& aptPostFileItem::GetTmpName() const
{
	return mTmpFileName;
}

////////////////////////////////////////////////////////////////////////////////
const nsString& aptPostFileItem::GetContentType() const
{
	return mContentType;
}


////////////////////////////////////////////////////////////////////////////////
PRUint32 aptPostFileItem::GetSize() const
{
	return mSize;
}

////////////////////////////////////////////////////////////////////////////////
void aptPostFileItem::DeleteFile()
{
	//delete the file if it exists.
	NS_ConvertUTF16toUTF8 tmpfn(mTmpFileName);
	const char* srcFile = tmpfn.get();
	
	remove(srcFile);
}

////////////////////////////////////////////////////////////////////////////////

aptHTTPRequest::aptHTTPRequest()
{
    mRemotePort = 0;
    mIsJaxerServer = PR_FALSE;
    mIsHTTPS = PR_FALSE;
	mIsAdmin = PR_FALSE;
    mStatusCode = -1;
	mRawPostData = nsnull;
	mRawPostDataLength = 0;
	mbRawPostDataInFile = PR_FALSE;
}

////////////////////////////////////////////////////////////////////////////////

aptHTTPRequest::~aptHTTPRequest() 
{
	if (mRawPostData)
		nsMemory::Free(mRawPostData);
}

////////////////////////////////////////////////////////////////////////////////

NS_IMPL_ISUPPORTS1(aptHTTPRequest, aptIHTTPRequest)

NS_IMETHODIMP 
aptHTTPRequest::GetRemoteHost(nsAString &aReturn)
{
    aReturn.Assign(mRemoteHost);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetRemoteHost(const nsAString &aValue)
{
    mRemoteHost.Assign(aValue);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetRemoteAddr(nsAString &aReturn)
{
    aReturn.Assign(mRemoteAddr);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetRemoteAddr(const nsAString &aValue)
{
    mRemoteAddr.Assign(aValue);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetRemoteUser(nsAString &aReturn)
{
    aReturn.Assign(mRemoteUser);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetRemoteUser(const nsAString &aValue)
{
    mRemoteUser.Assign(aValue);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetRemotePort(PRInt32* aReturn)
{
    *aReturn = mRemotePort;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetRemotePort(PRInt32 aValue)
{
    mRemotePort = aValue;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetStatusCode(PRInt32* aReturn)
{
    *aReturn = mStatusCode;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetStatusCode(PRInt32 aValue)
{
    mStatusCode = aValue;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetPageFile(nsAString & aPageFile)
{
    aPageFile.Assign(mPageFile);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetPageFile(const nsAString & aPageFile)
{
    mPageFile.Assign(aPageFile);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetDocumentRoot(nsAString & aDocumentRoot)
{
    aDocumentRoot.Assign(mDocumentRoot);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetDocumentRoot(const nsAString & aDocumentRoot)
{
    mDocumentRoot.Assign(aDocumentRoot);
	return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetIsJaxerServer(PRBool *bIsJaxerServer)
{
    *bIsJaxerServer = mIsJaxerServer;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetIsHandler(PRBool *bIsJaxerServer)
{
    *bIsJaxerServer = mIsJaxerServer;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetIsHandler(PRBool bIsJaxerServer)
{
    mIsJaxerServer = bIsJaxerServer;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetIsHTTPS(PRBool *bIsHTTPS)
{
    *bIsHTTPS = mIsHTTPS;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetIsHTTPS(PRBool bIsHTTPS)
{
    mIsHTTPS = bIsHTTPS;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetIsAdmin(PRBool *bIsAdmin)
{
    *bIsAdmin = mIsAdmin;
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetIsAdmin(PRBool bIsAdmin)
{
    mIsAdmin = bIsAdmin;
    return NS_OK;
}

NS_IMETHODIMP
aptHTTPRequest::GetOS(nsACString& aResult)
{
    aResult.AssignLiteral(OS_TARGET);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetQueryString(nsAString &aMethod)
{
    aMethod.Assign(mQueryString);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetQueryString(const nsAString &aMethod)
{
    mQueryString.Assign(aMethod);
    return NS_OK;
}


NS_IMETHODIMP 
aptHTTPRequest::GetMethod(nsAString &aMethod)
{
	aMethod.Assign(mMethod);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetMethod(const nsAString &aMethod)
{
	mMethod.Assign(aMethod);
    return NS_OK;
}


NS_IMETHODIMP 
aptHTTPRequest::GetUri(nsAString &aUri)
{
	aUri.Assign(mUri);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetUri(const nsAString &aUri)
{
	mUri.Assign(aUri);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::GetProtocol(nsAString &aProtocol)
{
	aProtocol.Assign(mProtocol);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetProtocol(const nsAString &aProtocol)
{
	mProtocol.Assign(aProtocol);
    return NS_OK;
}

#if 0
NS_IMETHODIMP 
aptHTTPRequest::GetPostData(nsAString &aPostData)
{
	aPostData.Assign(mPostData);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetPostData(const nsAString &aPostData)
{
	mPostData.Assign(aPostData);
    return NS_OK;
}
#endif

NS_IMETHODIMP 
aptHTTPRequest::GetLogFileNativePath(nsAString &aPath)
{
	aPath.Assign(mLogFileNativePath);
    return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::SetLogFileNativePath(const nsAString &aPath)
{
	mLogFileNativePath.Assign(aPath);
    return NS_OK;
}

#if 0
/* [noscript] void AppendPostData(in string aData); */
NS_IMETHODIMP 
aptHTTPRequest::AppendPostData(const char *aData)
{
	mPostData.AppendASCII(aData);
    return NS_OK;
}
#endif

/* int GetHeaderCount(); */
NS_IMETHODIMP
aptHTTPRequest::GetHeaderCount(PRUint32 *retv)
{
	*retv = mHeaderNames.Count();
	return NS_OK;
}

/* AString GetHeaderName(in long index); */
NS_IMETHODIMP
aptHTTPRequest::GetHeaderName(PRUint32 ord, nsAString &retv)
{
	if (ord >= mHeaderNames.Count())
		retv.Truncate();
	else
		mHeaderNames.StringAt(ord, retv);

	return NS_OK;
}

/* AString GetValueByName(in AString name); */
NS_IMETHODIMP
aptHTTPRequest::GetValueByName(const nsAString &name, nsAString &retv)
{
	PRUint32 cnt = mHeaderNames.Count();
	for (PRUint32 i = 0; i < cnt; i++) {
		if (mHeaderNames[i]->Equals(name)) {
			mHeaderValues.StringAt(i, retv);
			return NS_OK;
		}
	}

	retv.Truncate();
	return NS_OK;
}

/* AString GetValueByOrd(in int index); */
NS_IMETHODIMP
aptHTTPRequest::GetValueByOrd(PRUint32 ord, nsAString &retv)
{
	if (ord >= mHeaderNames.Count())
		retv.Truncate();
	else
		mHeaderValues.StringAt(ord, retv);

	return NS_OK;
}

/* boolean HasHeader(in AString name); */
NS_IMETHODIMP
aptHTTPRequest::HasHeader(const nsAString &name, PRBool *retv)
{
	PRUint32 cnt = mHeaderNames.Count();
	for (PRUint32 i = 0; i < cnt; i++) {
		if (mHeaderNames[i]->Equals(name)) {
			*retv = PR_TRUE;
			return NS_OK;
		}
	}

	*retv = PR_FALSE;
	return NS_OK;
}

/* [noscript] void AppendHeader(in AString name, in AString value); */
NS_IMETHODIMP
aptHTTPRequest::AppendHeader(const nsAString &name, const nsAString &value)
{
	mHeaderNames.AppendString(name);
	mHeaderValues.AppendString(value);
	return NS_OK;
}

/* int GetEnvCount(); */
NS_IMETHODIMP
aptHTTPRequest::GetEnvCount(PRUint32 *retv)
{
	*retv = mEnvNames.Count();
	return NS_OK;
}

/* AString GetEnvName(in long index); */
NS_IMETHODIMP
aptHTTPRequest::GetEnvName(PRUint32 ord, nsAString &retv)
{
	if (ord >= mEnvNames.Count())
		retv.Truncate();
	else
		mEnvNames.StringAt(ord, retv);

	return NS_OK;
}

/* AString GetEnvValueByName(in AString name); */
NS_IMETHODIMP
aptHTTPRequest::GetEnvValueByName(const nsAString &name, nsAString &retv)
{
	PRUint32 cnt = mEnvNames.Count();
	for (PRUint32 i = 0; i < cnt; i++) {
		if (mEnvNames[i]->Equals(name)) {
			mEnvValues.StringAt(i, retv);
			return NS_OK;
		}
	}

	retv.Truncate();
	return NS_OK;
}

/* AString GetEnvValueByOrd(in int index); */
NS_IMETHODIMP
aptHTTPRequest::GetEnvValueByOrd(PRUint32 ord, nsAString &retv)
{
	if (ord >= mEnvNames.Count())
		retv.Truncate();
	else
		mEnvValues.StringAt(ord, retv);

	return NS_OK;
}

/* boolean HasEnv(in AString name); */
NS_IMETHODIMP
aptHTTPRequest::HasEnv(const nsAString &name, PRBool *retv)
{
	PRUint32 cnt = mEnvNames.Count();
	for (PRUint32 i = 0; i < cnt; i++) {
		if (mEnvNames[i]->Equals(name)) {
			*retv = PR_TRUE;
			return NS_OK;
		}
	}

	*retv = PR_FALSE;
	return NS_OK;
}

/* [noscript] void AddEnv(in AString name, in AString value); */
NS_IMETHODIMP
aptHTTPRequest::AddEnv(const nsAString &name, const nsAString &value)
{
	mEnvNames.AppendString(name);
	mEnvValues.AppendString(value);
	return NS_OK;
}

/* boolean HasPostData(); */
NS_IMETHODIMP
aptHTTPRequest::HasPostData(PRBool *retv)
{
	PRUint32 cnt = mPostDataItemArray.Length();
	*retv = ( cnt > 0) ? PR_TRUE : PR_FALSE;

	return NS_OK;
}

/* boolean HasFiles(); */
NS_IMETHODIMP
aptHTTPRequest::HasFiles(PRBool *retv)
{
	PRUint32 cnt = mPostFileItemArray.Length();
	*retv = ( cnt > 0) ? PR_TRUE : PR_FALSE;

	return NS_OK;
}


/* long GetDataItemCount(); */
NS_IMETHODIMP
aptHTTPRequest::GetDataItemCount(PRUint32 *retv)
{
	*retv = mPostDataItemArray.Length();
	return NS_OK;
}

/* long GetFileCount(); */
NS_IMETHODIMP
aptHTTPRequest::GetFileCount(PRUint32 *retv)
{
	*retv = mPostFileItemArray.Length();
	return NS_OK;
}

/* AString GetDataItemName( in unsigned long index);*/
NS_IMETHODIMP
aptHTTPRequest::GetDataItemName(PRUint32 ord, nsAString &retv)
{
	if (ord >= mPostDataItemArray.Length())
		retv.Truncate();
	else
		retv = mPostDataItemArray[ord].GetName();

	return NS_OK;
}

/* AString GetDataItemValue( in unsigned long index);*/
NS_IMETHODIMP
aptHTTPRequest::GetDataItemValue(PRUint32 ord, nsAString &retv)
{
	if (ord >= mPostDataItemArray.Length())
		retv.Truncate();
	else
		retv = mPostDataItemArray[ord].GetValue();

	return NS_OK;
}

/* [noscript] void SetRawPostDataBytes (in unsigned long size, [array, size_is (size)] in octet value); */
NS_IMETHODIMP aptHTTPRequest::SetRawPostDataBytes(PRUint32 size, PRUint8 *value)
{
	if (mRawPostData)
	{
		nsMemory::Free(mRawPostData);
		mRawPostData = nsnull;
	}
	mRawPostData = static_cast<char*> (nsMemory::Alloc(size+1));
	if ( !mRawPostData)
		return NS_ERROR_OUT_OF_MEMORY;

	memcpy(mRawPostData, (char*)value, size);

	mRawPostDataLength = size;
	mRawPostData[mRawPostDataLength] = 0;

	mbRawPostDataInFile = PR_FALSE;
    return NS_OK;
}

/* [noscript] void SetRawPostDataFile (in unsigned long size, in nsIFile aFile); */
NS_IMETHODIMP aptHTTPRequest::SetRawPostDataFile(PRUint32 size, nsIFile *aFile)
{
    mRawPostDataLength = size;

	mbRawPostDataInFile = PR_TRUE;
	mRawPostDataFile = aFile;

    return NS_OK;
}

/* [noscript] void SetRawPostDataContentType (in AString ctype); */
NS_IMETHODIMP aptHTTPRequest::SetRawPostDataContentType(const nsAString & ctype)
{
	mRawPostDataContentType = ctype;
    return NS_OK;
}


/* AString GetRawPostDataAsString (); */
NS_IMETHODIMP aptHTTPRequest::GetRawPostDataAsString(nsAString & _retval)
{
	nsresult rv = NS_OK;
	_retval.Truncate();
	if (mRawPostDataLength == 0)
		return NS_OK;

	if (mbRawPostDataInFile)
	{
		FILE *fp;
		nsCOMPtr<nsILocalFile>  aLocalFile(do_QueryInterface(mRawPostDataFile));
		NS_ENSURE_TRUE(aLocalFile, NS_ERROR_NO_INTERFACE);
		rv = aLocalFile->OpenANSIFileDesc("r", &fp);
		NS_ENSURE_SUCCESS(rv, rv);

		nsCAutoString data;
        PRUint32 nRead;
        char buf[4096];
		while ((nRead =fread(buf, sizeof(char), 4096, fp))>0)
		{
			data.Append(buf, nRead);
		}
		fclose(fp);
		if (data.Length() <= 0)
			return NS_ERROR_UNEXPECTED;

		CopyUTF8toUTF16(data, _retval);
	}else
	{
		CopyUTF8toUTF16(mRawPostData, _retval);
	}

	return NS_OK;
}

/* AString GetRawPostDataContentType (); */
NS_IMETHODIMP aptHTTPRequest::GetRawPostDataContentType(nsAString & _retval)
{
    _retval = mRawPostDataContentType;

	return NS_OK;
}

/* unsigned long GetRawPostDataLength (); */
NS_IMETHODIMP aptHTTPRequest::GetRawPostDataLength(PRUint32 *_retval)
{
    *_retval = mRawPostDataLength;

	return NS_OK;
}

/* void GetRawPostDataAsByteArray (out unsigned long aValueSize, [array, size_is (aValueSize)] out octet aValue); */
NS_IMETHODIMP aptHTTPRequest::GetRawPostDataAsByteArray(PRUint32 *aValueSize, PRUint8 **aValue)
{
	nsresult rv = NS_OK;
	*aValue = nsnull;
	*aValueSize = 0;

	if (mRawPostDataLength == 0)
		return NS_OK;

	*aValue = static_cast<PRUint8*>(nsMemory::Alloc(mRawPostDataLength+1));
	if (! *aValue)
		return NS_ERROR_OUT_OF_MEMORY;

	if (mbRawPostDataInFile)
	{
		FILE *fp;
		nsCOMPtr<nsILocalFile>  aLocalFile(do_QueryInterface(mRawPostDataFile));
		NS_ENSURE_TRUE(aLocalFile, NS_ERROR_NO_INTERFACE);
		rv = aLocalFile->OpenANSIFileDesc("r", &fp);
		NS_ENSURE_SUCCESS(rv, rv);

		PRUint32 nRead;
		PRUint32 nLeft = mRawPostDataLength;
        while(nLeft)
		{
			nRead =fread(*aValue + mRawPostDataLength - nLeft, sizeof(char), nLeft, fp);
			nLeft -= nRead;
			if (nRead == 0)
				break;
		}
		fclose(fp);
	}else
	{
		memcpy(*aValue, mRawPostData, mRawPostDataLength);
	}
	*aValueSize = mRawPostDataLength;
	return NS_OK;
}

/* nsIInputStream GetRawPostDataAsInputStream (); */
NS_IMETHODIMP aptHTTPRequest::GetRawPostDataAsInputStream(nsIInputStream **_retval)
{
	nsresult rv = NS_OK;
	*_retval = nsnull;
	if (mRawPostDataLength == 0)
		return NS_OK;

	if (mbRawPostDataInFile)
	{
		nsCOMPtr<nsIFileInputStream> fis = do_CreateInstance("@mozilla.org/network/file-input-stream;1");
		if (!fis)
		{
			//gJaxerLog.Log(eERROR, "Create File Stream failed");
			return NS_ERROR_UNEXPECTED;
		}

		rv = fis->Init(mRawPostDataFile, -1, -1, 0);
		if (NS_FAILED(rv))
		{
			return rv;
		}
        NS_IF_ADDREF(*_retval = fis); //do_QueryInterface(fis, &rv);
	}else
	{
		nsCOMPtr<nsIStringInputStream> sis = do_CreateInstance("@mozilla.org/io/string-input-stream;1");
		if (!sis)
		{
			//gJaxerLog.Log(eERROR, "Create Text Stream failed");
			return NS_ERROR_OUT_OF_MEMORY;
		}

		rv = sis->ShareData(mRawPostData, mRawPostDataLength);
		if (NS_FAILED(rv))
		{
			return rv;
		}
		NS_IF_ADDREF(*_retval = sis); //do_QueryInterface(sis);
	}
	return (*_retval) ? NS_OK : NS_ERROR_FAILURE;
}


/* AString GetFileName( in unsigned long index); */
NS_IMETHODIMP
aptHTTPRequest::GetFileName(PRUint32 ord, nsAString &retv)
{
	if (ord >= mPostFileItemArray.Length() )
		retv.Truncate();
	else
		retv = mPostFileItemArray[ord].GetName();

	return NS_OK;
}

/* AString GetTmpFileName( in unsigned long index); */
NS_IMETHODIMP
aptHTTPRequest::GetTmpFileName(PRUint32 ord, nsAString &retv)
{
	if (ord >= mPostFileItemArray.Length() )
		retv.Truncate();
	else
		retv = mPostFileItemArray[ord].GetTmpName();

	return NS_OK;
}

/* AString GetOriginalFileName( in unsigned long index); */
NS_IMETHODIMP
aptHTTPRequest::GetOriginalFileName(PRUint32 ord, nsAString &retv)
{
	if (ord >= mPostFileItemArray.Length())
		retv.Truncate();
	else
		retv = mPostFileItemArray[ord].GetOriginalName();

	return NS_OK;
}


/* AString GetFileContentType( in unsigned long index); */
NS_IMETHODIMP
aptHTTPRequest::GetFileContentType(PRUint32 ord, nsAString &retv)
{
	if (ord >= mPostFileItemArray.Length())
		retv.Truncate();
	else
		retv = mPostFileItemArray[ord].GetContentType();

	return NS_OK;
}

/* long GetFileSize( in unsigned long index); */
NS_IMETHODIMP
aptHTTPRequest::GetFileSize(PRUint32 ord, PRUint32 *retv)
{
	if (ord >= mPostFileItemArray.Length())
		*retv = 0;
	else
		*retv = mPostFileItemArray[ord].GetSize();

	return NS_OK;
}


/* [noscript] void AddPostStringItem( int AString name, in AString value); */
NS_IMETHODIMP
aptHTTPRequest::AddPostStringItem(const nsAString &name, const nsAString& value)
{
	aptPostDataItem* newItem = mPostDataItemArray.AppendElement();
	newItem->SetData( name, value);
	return NS_OK;
}

#if 0
/* [noscript] void AddPostDataItem (in AString name, [array, size_is (size)] in octet value, in AString ctype, in unsigned long size); */
NS_IMETHODIMP aptHTTPRequest::AddPostDataItem(const nsAString & name, PRUint8 *value, const nsAString & ctype, PRUint32 size)
{
    aptPostDataItem* newItem = mPostDataItemArray.AppendElement();
	newItem->SetData( name, value, ctype, size);
	return NS_OK;
}
#endif

/* [noscript] void AddPostFileItem( int AString name, in AString fileName, in AString tmpFileName, in AString contentType, in in long size); */
NS_IMETHODIMP
aptHTTPRequest::AddPostFileItem(const nsAString &name, const nsAString &filename, const nsAString &tmpFilename, 
	const nsAString &contentType, PRUint32 size)
{
	aptPostFileItem* newItem = mPostFileItemArray.AppendElement();
	newItem->SetData( name, filename, tmpFilename, contentType, size);
	return NS_OK;
}

/* boolean SaveFile( in unsigned long index, in AString destPath); */
NS_IMETHODIMP
aptHTTPRequest::SaveFile(PRUint32 ord, const nsAString& destPath, PRBool *retv)
{
	PRUint32 cnt = mPostFileItemArray.Length();
	*retv = PR_FALSE;
	if (ord < cnt)
	{
		//try rename first

		nsString srcFileName = mPostFileItemArray[ord].GetTmpName();
		// const char* srcFile = NS_ConvertUTF16toUTF8(srcFileName).get();
		NS_ConvertUTF16toUTF8 dest(destPath);
		NS_ConvertUTF16toUTF8 src(srcFileName);
		const char * destFile = dest.get();
		const char* srcFile = src.get();
		
		PRInt32 ret = rename( srcFile, destFile);
		if ( ret != 0)
		{
			//rename failed, try copy
			char buffer[BUFSIZ] = { '\0' };
			FILE *fpin = fopen( srcFile, "rb");
			FILE *fpout = fopen ( destFile, "wb");
			size_t len = 0;
			if ( fpin != NULL && fpout != NULL)
			{
				while( (len = fread( buffer, 1, BUFSIZ, fpin)) > 0 )
				{
					size_t len2 = fwrite( buffer, 1, len, fpout );
					len2;
				}
				fclose(fpin);
				fclose(fpout) ;
			    remove( srcFile);
				*retv = PR_TRUE;
			}
		}

	}

	return NS_OK;
}

/* [noscript] void RemoveUploadedTmpFiles(); */
NS_IMETHODIMP
aptHTTPRequest::RemoveUploadedTmpFiles()
{
	//delete any tmp files and clear the array
	PRUint32 cnt = mPostFileItemArray.Length();
	PRUint32 i;
	for (i=0; i<cnt; i++)
	{
		mPostFileItemArray[i].DeleteFile();
	}
	mPostFileItemArray.Clear();

	// rawpostdata
	if (mRawPostDataLength>0)
	{
		if (mbRawPostDataInFile)
		{
			//delete file
			mRawPostDataFile->Remove(PR_FALSE);
		}else
		{
			if (mRawPostData)
			{
				nsMemory::Free(mRawPostData);
				mRawPostData=nsnull;
			}
		}
	}
	return NS_OK;
}

static size_t
jsstrlen(const jschar *s)
{
    const jschar *t;

    for (t = s; *t != 0; t++)
        continue;
    return (size_t)(t - s);
}

/* void ExecuteJavascript(in wstring script [, in string url [, in JSObject *target_obj]]); */
NS_IMETHODIMP
aptHTTPRequest::ExecuteJavascript(const PRUnichar * /*script*/ /* [, const char *url] */ /* [, JSObject *target_obj] */)
{
    nsresult rv;

	jschar *scriptText;
	char *url = nsnull;
	JSObject *target_obj = nsnull;
	jsval *rval;
	JSContext *cx;

#ifdef _TIMESTATS
	int64 startTime = JS_Now();
#endif /* _TIMESTATS */

	nsCOMPtr<nsIPrincipal> principal;

	/* get JS things from the CallContext */
	nsCOMPtr<nsIXPConnect> xpc = do_GetService(nsIXPConnect::GetCID());
	NS_ENSURE_TRUE(xpc, NS_ERROR_FAILURE);

	nsAXPCNativeCallContext* cc = nsnull;
	rv = xpc->GetCurrentNativeCallContext(&cc);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	PRUint32 argc;
	rv = cc->GetArgc (&argc);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	jsval *argv;
	rv = cc->GetArgvPtr (&argv);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	rv = cc->GetRetValPtr (&rval);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	{
		rv = cc->GetJSContext (&cx);
		NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

		JSAutoRequest ar(cx);

		JSBool ok = JS_ConvertArguments (cx, argc, argv, "W / s o", &scriptText, &url, &target_obj);
		if (!ok)
		{
			cc->SetExceptionWasThrown (JS_TRUE);
			/* let the exception raised by JS_ConvertArguments show through */
			return NS_OK;
		}

		if (!url)
			url = "<JaxerEval>";

		if (!target_obj)
		{
			/* if the user didn't provide an object to eval onto, use current DocShell */
			nsCOMPtr<aptIDocumentFetcherService> dfs(do_GetService("@aptana.com/httpdocumentfetcher;1"));
			NS_ENSURE_TRUE(dfs, NS_ERROR_FAILURE);

			nsCOMPtr<nsIDocShell> docShell;
			dfs->GetDocShell(getter_AddRefs(docShell));
			NS_ENSURE_TRUE(docShell, NS_ERROR_FAILURE);

			nsCOMPtr<nsIScriptGlobalObject> sgo(do_GetInterface(docShell));
			NS_ENSURE_TRUE(sgo, NS_ERROR_FAILURE);

			
			nsIScriptContext *scx = sgo->GetContext();
			NS_ENSURE_TRUE(scx, NS_ERROR_FAILURE);
			cx = (JSContext *)scx->GetNativeContext();
			NS_ENSURE_TRUE(cx, NS_ERROR_FAILURE);
			target_obj = sgo->GetGlobalJSObject(); // or ::JS_GetGlobalObject(cx);
			NS_ENSURE_TRUE(target_obj, NS_ERROR_FAILURE);

		}

		nsCOMPtr<nsIScriptSecurityManager> secman = 
			do_GetService(NS_SCRIPTSECURITYMANAGER_CONTRACTID);
		NS_ENSURE_TRUE(secman, NS_ERROR_FAILURE);

		rv = secman->GetSystemPrincipal(getter_AddRefs(principal));
		NS_ENSURE_SUCCESS(rv, rv);
		NS_ENSURE_TRUE(principal, NS_ERROR_FAILURE);

		// Innerize the target_obj so that we compile the loaded script in the
		// correct (inner) scope.
		JSClass *target_class = JS_GET_CLASS(cx, target_obj);
		if (target_class->flags & JSCLASS_IS_EXTENDED)
		{
			JSExtendedClass *extended = (JSExtendedClass*)target_class;
			if (extended->innerObject)
			{
				target_obj = extended->innerObject(cx, target_obj);
				NS_ENSURE_TRUE(target_obj, NS_ERROR_FAILURE);
			}
		}
	}

	// Allocate a ScriptCompile event object.
	nsCOMPtr<aptIEventScriptCompile> sc = do_CreateInstance(APT_EVENT_SCRIPT_COMPILE_CONTRACTID);
	sc->Init();
	sc->SetScriptURI(nsDependentCString(url));
	sc->SetLineNo(1);
	sc->SetScriptText(nsDependentString(scriptText));
	// Fire the ScriptCompile event out to all observers.
	GetEventTypeManager()->FireEvent(sc, aptEventName_ScriptCompile);
	
	nsCAutoString scriptURI;
	sc->GetScriptURI(scriptURI);
	
	PRUint32 lineNo;
	sc->GetLineNo(&lineNo);
	
	JSAutoRequest ar(cx);
    nsCOMPtr<nsIJSContextStack> contextStack = do_GetService("@mozilla.org/js/xpc/ContextStack;1");
    JS_ClearPendingException(cx);
	
	JSPrincipals* jsPrincipals;
    rv = principal->GetJSPrincipals(cx, &jsPrincipals);
    NS_ENSURE_SUCCESS(rv, rv);
	
    rv = contextStack->Push(cx);
    NS_ENSURE_SUCCESS(rv, rv);
	
    uint32 options = JS_GetOptions(cx);
    JS_SetOptions(cx, options | JSOPTION_DONT_REPORT_UNCAUGHT);

    JSBool ok = JS_EvaluateUCScriptForPrincipals(cx, target_obj, jsPrincipals, scriptText, jsstrlen(scriptText), scriptURI.get(), lineNo, rval);

    JS_SetOptions(cx, options);

    cc->SetExceptionWasThrown (!ok);
    cc->SetReturnValueWasSet (ok);

    JSContext *oldcx;
    rv = contextStack->Pop(&oldcx);
    NS_ENSURE_SUCCESS(rv, rv);
	
    rv = JSPRINCIPALS_DROP(cx, jsPrincipals);
    NS_ENSURE_SUCCESS(rv, rv);

#ifdef _TIMESTATS
	int64 endTime = JS_Now();
	int64 ll_delta;
	JSLL_SUB(ll_delta, endTime, startTime);
	JSInt32 diff;
	JSLL_L2UI(diff, ll_delta);

    gJaxerLog.Log(eDEBUG, "Eval run time: %ld for %s", diff, url);

#if 0
    nsCOMPtr<nsIConsoleService> mConsoleService = do_GetService(NS_CONSOLESERVICE_CONTRACTID);
    nsCOMPtr<aptICoreTrace> mCoreTrace = do_CreateInstance(APT_CORETRACE_CONTRACTID);
    nsString aMsg(NS_LITERAL_STRING("Eval run time: "));
	AppendUTF8toUTF16(nsPrintfCString("%ld", diff), aMsg);
	aMsg.Append(NS_LITERAL_STRING(" for: "));
	AppendASCIItoUTF16(url, aMsg);
	APT_CORETRACE_DEBUG(mConsoleService, mCoreTrace, aMsg);
#endif
#endif /* _TIMESTATS */

	return rv;
}

/* void CompileScript (in wstring script [, in string url [, in JSObject *target_obj]]); */
NS_IMETHODIMP
aptHTTPRequest::CompileScript(const PRUnichar * /*script*/ /* [, const char *url] */ /* [, JSObject *target_obj] */)
{
    nsresult rv;

	jschar *scriptText;
	char *url = nsnull;
	JSObject *target_obj = nsnull;
	jsval *rval;
	JSString *str;
	JSContext *cx;
    uint32 len;
	void *buf;

	nsCOMPtr<nsIPrincipal> principal;

	/* get JS things from the CallContext */
	nsCOMPtr<nsIXPConnect> xpc = do_GetService(nsIXPConnect::GetCID());
	NS_ENSURE_TRUE(xpc, NS_ERROR_FAILURE);

	// nsCOMPtr<nsIXPCNativeCallContext> cc;
	nsAXPCNativeCallContext* cc = nsnull;
	
	rv = xpc->GetCurrentNativeCallContext(&cc);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	PRUint32 argc;
	rv = cc->GetArgc (&argc);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	jsval *argv;
	rv = cc->GetArgvPtr (&argv);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	rv = cc->GetRetValPtr (&rval);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	{
		rv = cc->GetJSContext (&cx);
		NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

		JSAutoRequest ar(cx);

		JSBool ok = JS_ConvertArguments (cx, argc, argv, "W / s o", &scriptText, &url, &target_obj);
		if (!ok)
		{
			cc->SetExceptionWasThrown (JS_TRUE);
			/* let the exception raised by JS_ConvertArguments show through */
			return NS_OK;
		}

		if (!url)
			url = "<JaxerEval>";

		if (!target_obj)
		{
			/* if the user didn't provide an object to eval onto, use current DocShell */
			nsCOMPtr<aptIDocumentFetcherService> dfs(do_GetService("@aptana.com/httpdocumentfetcher;1"));
			NS_ENSURE_TRUE(dfs, NS_ERROR_FAILURE);

			nsCOMPtr<nsIDocShell> docShell;
			dfs->GetDocShell(getter_AddRefs(docShell));
			NS_ENSURE_TRUE(docShell, NS_ERROR_FAILURE);

			nsCOMPtr<nsIScriptGlobalObject> sgo(do_GetInterface(docShell));
			NS_ENSURE_TRUE(sgo, NS_ERROR_FAILURE);
		
			nsIScriptContext *scx = sgo->GetContext();
			NS_ENSURE_TRUE(scx, NS_ERROR_FAILURE);
			cx = (JSContext *)scx->GetNativeContext();
			NS_ENSURE_TRUE(cx, NS_ERROR_FAILURE);
			target_obj = sgo->GetGlobalJSObject(); // or ::JS_GetGlobalObject(cx);
			NS_ENSURE_TRUE(target_obj, NS_ERROR_FAILURE);

		}

		nsCOMPtr<nsIScriptSecurityManager> secman = 
			do_GetService(NS_SCRIPTSECURITYMANAGER_CONTRACTID);
		NS_ENSURE_TRUE(secman, NS_ERROR_FAILURE);

		rv = secman->GetSystemPrincipal(getter_AddRefs(principal));
		NS_ENSURE_SUCCESS(rv, rv);
		NS_ENSURE_TRUE(principal, NS_ERROR_FAILURE);

		// Innerize the target_obj so that we compile the loaded script in the
		// correct (inner) scope.
		JSClass *target_class = JS_GET_CLASS(cx, target_obj);
		if (target_class->flags & JSCLASS_IS_EXTENDED)
		{
			JSExtendedClass *extended = (JSExtendedClass*)target_class;
			if (extended->innerObject)
			{
				target_obj = extended->innerObject(cx, target_obj);
				NS_ENSURE_TRUE(target_obj, NS_ERROR_FAILURE);
			}
		}
	}

	// Allocate a ScriptCompile event object.
	nsCOMPtr<aptIEventScriptCompile> sc = do_CreateInstance(APT_EVENT_SCRIPT_COMPILE_CONTRACTID);
	sc->Init();
	sc->SetScriptURI(nsDependentCString(url));
	sc->SetLineNo(1);
	sc->SetScriptText(nsDependentString(scriptText));
	// Fire the ScriptCompile event out to all observers.
	GetEventTypeManager()->FireEvent(sc, aptEventName_ScriptCompile);
	
	nsCAutoString scriptURI;
	sc->GetScriptURI(scriptURI);	

	PRUint32 lineNo;
	sc->GetLineNo(&lineNo);

	JSAutoRequest ar(cx);
    nsCOMPtr<nsIJSContextStack> contextStack = do_GetService("@mozilla.org/js/xpc/ContextStack;1");
    JS_ClearPendingException(cx);
	
	JSPrincipals* jsPrincipals;
    rv = principal->GetJSPrincipals(cx, &jsPrincipals);
    NS_ENSURE_SUCCESS(rv, rv);
	
    rv = contextStack->Push(cx);
    NS_ENSURE_SUCCESS(rv, rv);
	
    uint32 options = JS_GetOptions(cx);
    JS_SetOptions(cx, options | JSOPTION_DONT_REPORT_UNCAUGHT);

    JSScript *script = JS_CompileUCScriptForPrincipals(cx, target_obj, jsPrincipals, scriptText, jsstrlen(scriptText), scriptURI.get(), lineNo);
	NS_ENSURE_TRUE(script, NS_ERROR_FAILURE);

    JS_SetOptions(cx, options);

    /* create new XDR */
    JSXDRState *xdr = JS_XDRNewMem(cx, JSXDR_ENCODE);
    NS_ENSURE_TRUE(xdr, NS_ERROR_OUT_OF_MEMORY);

    /* write  */
	JSBool ok = JS_XDRScript(xdr, &script);

    JS_DestroyScript(cx, script);

    JSContext *oldcx;
    rv = contextStack->Pop(&oldcx);
    if (NS_FAILED(rv)) {
        goto out;
	}
	
    rv = JSPRINCIPALS_DROP(cx, jsPrincipals);
    if (NS_FAILED(rv)) {
        goto out;
	}

    if (!ok) {
		rv = NS_ERROR_FAILURE;
        goto out;
	}

    buf = JS_XDRMemGetData(xdr, &len);
    if (!buf) {
        rv = NS_ERROR_FAILURE;
        goto out;
    }

    JS_ASSERT((jsword)buf % sizeof(jschar) == 0);
    len /= sizeof(jschar);
    str = JS_NewUCStringCopyN(cx, (jschar *)buf, len);
    if (!str) {
        rv = NS_ERROR_OUT_OF_MEMORY;
        goto out;
    }

    *rval = STRING_TO_JSVAL(str);
     rv = NS_OK;

out:
    JS_XDRDestroy(xdr);
	return rv;
}

/* void RunScript (in wstring data [, in JSObject *target_obj]); */
NS_IMETHODIMP
aptHTTPRequest::RunScript(const PRUnichar * /*data*/ /* [, JSObject *target_obj] */)
{
    nsresult rv;
	JSString *data;
	JSObject *target_obj = nsnull;
	jsval *rval;
	JSContext *cx;
	
#ifdef _TIMESTATS
	int64 startTime = JS_Now();
#endif /* _TIMESTATS */

	/* get JS things from the CallContext */
	nsCOMPtr<nsIXPConnect> xpc = do_GetService(nsIXPConnect::GetCID());
	NS_ENSURE_TRUE(xpc, NS_ERROR_FAILURE);

	//nsCOMPtr<nsIXPCNativeCallContext> cc;
	nsAXPCNativeCallContext* cc = nsnull;
	rv = xpc->GetCurrentNativeCallContext(&cc);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	PRUint32 argc;
	rv = cc->GetArgc (&argc);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	jsval *argv;
	rv = cc->GetArgvPtr (&argv);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	rv = cc->GetRetValPtr (&rval);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	{
		rv = cc->GetJSContext (&cx);
		NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

		JSAutoRequest ar(cx);

		JSBool ok = JS_ConvertArguments (cx, argc, argv, "S / o", &data, &target_obj);
		if (!ok)
		{
			cc->SetExceptionWasThrown (JS_TRUE);
			/* let the exception raised by JS_ConvertArguments show through */
			return NS_OK;
		}

		if (!target_obj)
		{
			/* if the user didn't provide an object to eval onto, use current DocShell */
			nsCOMPtr<aptIDocumentFetcherService> dfs(do_GetService("@aptana.com/httpdocumentfetcher;1"));
			NS_ENSURE_TRUE(dfs, NS_ERROR_FAILURE);

			nsCOMPtr<nsIDocShell> docShell;
			dfs->GetDocShell(getter_AddRefs(docShell));
			NS_ENSURE_TRUE(docShell, NS_ERROR_FAILURE);

			nsCOMPtr<nsIScriptGlobalObject> sgo(do_GetInterface(docShell));
			NS_ENSURE_TRUE(sgo, NS_ERROR_FAILURE);

			
			nsIScriptContext *scx = sgo->GetContext();
			NS_ENSURE_TRUE(scx, NS_ERROR_FAILURE);
			cx = (JSContext *)scx->GetNativeContext();
			NS_ENSURE_TRUE(cx, NS_ERROR_FAILURE);
			target_obj = sgo->GetGlobalJSObject(); // or ::JS_GetGlobalObject(cx);
			NS_ENSURE_TRUE(target_obj, NS_ERROR_FAILURE);

		}

		// Innerize the target_obj so that we compile the loaded script in the
		// correct (inner) scope.
		JSClass *target_class = JS_GET_CLASS(cx, target_obj);
		if (target_class->flags & JSCLASS_IS_EXTENDED)
		{
			JSExtendedClass *extended = (JSExtendedClass*)target_class;
			if (extended->innerObject)
			{
				target_obj = extended->innerObject(cx, target_obj);
				NS_ENSURE_TRUE(target_obj, NS_ERROR_FAILURE);
			}
		}
	}

	/* create new XDR */
    JSXDRState *xdr = JS_XDRNewMem(cx, JSXDR_DECODE);
    NS_ENSURE_TRUE(xdr, NS_ERROR_OUT_OF_MEMORY);

    uint32 len = JS_GetStringLength(data);
    PRUnichar *buf = JS_GetStringChars(data);

    len *= sizeof(jschar);
    JS_XDRMemSetData(xdr, buf, len);

	JSAutoRequest ar(cx);

    /* read  */
    JSScript *script;
	JSBool ok = JS_XDRScript(xdr, &script);
	
    /*
     * We reset the buffer to be NULL so that it doesn't free the chars
     * memory owned by data.
     */
    JS_XDRMemSetData(xdr, NULL, 0);

	JS_XDRDestroy(xdr);
    NS_ENSURE_TRUE(ok, NS_ERROR_FAILURE);

    nsCOMPtr<nsIJSContextStack> contextStack = do_GetService("@mozilla.org/js/xpc/ContextStack;1");
    JS_ClearPendingException(cx);
		
    rv = contextStack->Push(cx);
    NS_ENSURE_SUCCESS(rv, rv);
	
    uint32 options = JS_GetOptions(cx);
    JS_SetOptions(cx, options | JSOPTION_DONT_REPORT_UNCAUGHT);

#ifdef _TIMESTATS
	int64 execTimeStart = JS_Now();
#endif /* _TIMESTATS */
    ok = JS_ExecuteScript(cx, target_obj, script, rval);
#ifdef _TIMESTATS
	int64 execTimeEnd = JS_Now();
#endif /* _TIMESTATS */

    JS_SetOptions(cx, options);

    cc->SetExceptionWasThrown (!ok);
    cc->SetReturnValueWasSet (ok);

    JSContext *oldcx;
    rv = contextStack->Pop(&oldcx);
    NS_ENSURE_SUCCESS(rv, rv);

#ifdef _TIMESTATS
	int64 endTime = JS_Now();
	int64 ll_delta, ll_delta2;
	JSLL_SUB(ll_delta, endTime, startTime);
	JSInt32 diff, diff2;
	JSLL_L2UI(diff, ll_delta);

    JSLL_SUB(ll_delta2, execTimeEnd, execTimeStart);
	JSLL_L2UI(diff2, ll_delta2);
    gJaxerLog(eDEBUG, "Bytecode run time: %ld/%ld for %s", diff, diff2, JS_GetScriptFilename(cx, script));

#if 0
    nsCOMPtr<nsIConsoleService> mConsoleService = do_GetService(NS_CONSOLESERVICE_CONTRACTID);
    nsCOMPtr<aptICoreTrace> mCoreTrace = do_CreateInstance(APT_CORETRACE_CONTRACTID);
    nsString aMsg(NS_LITERAL_STRING("Bytecode run time: "));
	AppendUTF8toUTF16(nsPrintfCString("%ld", diff), aMsg);
	aMsg.Append(NS_LITERAL_STRING("/"));
	JSLL_SUB(ll_delta, execTimeEnd, execTimeStart);
	JSLL_L2UI(diff, ll_delta);
	AppendUTF8toUTF16(nsPrintfCString("%ld", diff), aMsg);
	aMsg.Append(NS_LITERAL_STRING(" for: "));
	AppendASCIItoUTF16(JS_GetScriptFilename(cx, script), aMsg);
	APT_CORETRACE_DEBUG(mConsoleService, mCoreTrace, aMsg);
#endif
#endif /* _TIMESTATS */

	JS_DestroyScript(cx, script);

	return rv;
}

/*
 *  Cancel the request.
 */
NS_IMETHODIMP 
aptHTTPRequest::StopRequest()
{
    nsCOMPtr<aptIDocumentFetcherService> dfs(do_GetService("@aptana.com/httpdocumentfetcher;1"));
	NS_ENSURE_TRUE(dfs, NS_ERROR_FAILURE);

	nsCOMPtr<nsIDocShell> docShell;
	dfs->GetDocShell(getter_AddRefs(docShell));
	NS_ENSURE_TRUE(docShell, NS_ERROR_FAILURE);

    nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(docShell));
    NS_ENSURE_TRUE(webNav, NS_ERROR_FAILURE);

    return webNav->Stop(nsIWebNavigation::STOP_ALL);
}

/*
 *  Exit - terminate JS execution
 */
NS_IMETHODIMP 
aptHTTPRequest::Exit()
{
    nsresult rv;

	nsCOMPtr<nsIXPConnect> xpc = do_GetService(nsIXPConnect::GetCID());
	NS_ENSURE_TRUE(xpc, NS_ERROR_FAILURE);

	nsAXPCNativeCallContext* cc = nsnull;
	rv = xpc->GetCurrentNativeCallContext(&cc);
	NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

	cc->SetExceptionWasThrown(JS_TRUE);

	return NS_OK;
}

NS_IMETHODIMP 
aptHTTPRequest::LoadImage(nsIDOMHTMLImageElement *image)
{
    nsresult rv;

    nsCOMPtr<imgIContainer> imgContainer;

    nsCOMPtr<nsIImageLoadingContent> imageLoader = do_QueryInterface(image);
    if (imageLoader) {
        nsCOMPtr<imgIRequest> imgRequest;
        rv = imageLoader->GetRequest(nsIImageLoadingContent::CURRENT_REQUEST,
                                     getter_AddRefs(imgRequest));
        NS_ENSURE_SUCCESS(rv, rv);

		if (!imgRequest) {
	        rv = imageLoader->ForceReload();
            NS_ENSURE_SUCCESS(rv, rv);
			rv = imageLoader->GetRequest(nsIImageLoadingContent::CURRENT_REQUEST,
										 getter_AddRefs(imgRequest));
			NS_ENSURE_SUCCESS(rv, rv);

			PRUint32 status;
			imgRequest->GetImageStatus(&status);
			nsIThread *thread = NS_GetCurrentThread();
			while ((status & imgIRequest::STATUS_ERROR) == 0 && (status & imgIRequest::STATUS_LOAD_COMPLETE) == 0) {
			  if (!NS_ProcessNextEvent(thread)) {
				  return NS_ERROR_UNEXPECTED;
			  }
			  imgRequest->GetImageStatus(&status);
			}
        }

		if (!imgRequest)
            // XXX ERRMSG we need to report an error to developers here! (bug 329026)
            return NS_ERROR_NOT_AVAILABLE;

        PRUint32 status;
        imgRequest->GetImageStatus(&status);
        if ((status & imgIRequest::STATUS_LOAD_COMPLETE) == 0)
            return NS_ERROR_NOT_AVAILABLE;
		return NS_OK;
	}
    
	return NS_ERROR_NOT_AVAILABLE;
}

/* [noscript] void setDocumentStream (in nsIStorageStream stream); */                                                                                          
NS_IMETHODIMP aptHTTPRequest::SetDocumentStream(nsIStorageStream *stream)                                                                                           
{
	mDocumentStorageStream = stream;
    return NS_OK;                                                                                                                           
}                                                                                                                                                              

/* [noscript] void setDocumentURL (in nsIURL url); */                                                                                                                  
NS_IMETHODIMP aptHTTPRequest::SetDocumentURL(nsIURL *url)                                                                                                                   
{
	mDocumentURL = url;
    return NS_OK;                                                                                                                           
}                                                                                                                                                              

/* nsIInputStream getDocumentInputStream (); */                                                                                                                
NS_IMETHODIMP aptHTTPRequest::GetDocumentInputStream(nsIInputStream **_retval)                                                                                      
{
	if (mDocumentStorageStream) {
		return mDocumentStorageStream->NewInputStream(0, _retval);
	}
    return NS_ERROR_FAILURE;                                                                                                                           
}                                                                                                                                                              

/* readonly attribute nsIURL documentURL; */                                                                                                                   
NS_IMETHODIMP aptHTTPRequest::GetDocumentURL(nsIURL * *aDocumentURL)                                                                                                
{
	NS_ENSURE_ARG_POINTER(aDocumentURL);
	*aDocumentURL = mDocumentURL;
	NS_IF_ADDREF(*aDocumentURL);
    return NS_OK;                                                                                                                           
}                                                                                                                                                              


