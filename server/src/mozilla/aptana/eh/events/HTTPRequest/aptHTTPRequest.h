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

#ifndef _aptHTTPRequest_H_
#define _aptHTTPRequest_H_

#include "nsCOMPtr.h"
#include "aptIHTTPRequest.h"
#include "nsVoidArray.h"
#include "nsString.h"
#include "nsTArray.h"
#include "nsIStorageStream.h"
#include "nsIURL.h"


////////////////////////////////////////////////////////////////////////////////

class aptPostDataItem
{
public:
	//aptPostDataItem(const nsAString name, const nsAString &value, const nsAString &ctype, PRUint32 size);
	aptPostDataItem();
	~aptPostDataItem();
	aptPostDataItem( const aptPostDataItem &other);
	aptPostDataItem& operator=( const aptPostDataItem& other);
	void SetData( const nsAString &name, const nsAString &value);
	
	const nsString& GetName() const;
	const nsString& GetValue() const;
	//const nsString& GetContentType() const;
	///const PRUint32 GetSize() const;
private:
	nsString mName;
	nsString mValue;
	//PRUint8 *mValue;
	//nsString mContentType;
	//PRUint32 mSize;
};

class aptPostFileItem
{
public:
	aptPostFileItem();
	aptPostFileItem( const aptPostFileItem &other);
	aptPostFileItem& operator=( const aptPostFileItem& other);
	void SetData( const nsAString &name, const nsAString &filename, const nsAString &tmpFilename, 
		const nsAString &contentType, PRUint32 size);

	const nsString& GetName() const;
	const nsString& GetOriginalName() const;
	const nsString& GetTmpName() const;
	const nsString& GetContentType() const;
	PRUint32 GetSize() const;
	void DeleteFile();
	~aptPostFileItem();

private:
	nsString mName;
	nsString mOriginalFileName;
	nsString mTmpFileName;
	nsString mContentType;
	PRUint32 mSize;
};

class aptHTTPRequest: public aptIHTTPRequest
{
public:
  aptHTTPRequest();

  // nsISupports interface...
  NS_DECL_ISUPPORTS

  // aptIEventBase interface...
  NS_DECL_APTIHTTPREQUEST

  ~aptHTTPRequest();


  // Add appropriate member  variables below

protected:

	nsString	mMethod;
	nsString	mUri; // /foo/bar.html
	nsString    mUrl; //http://host:port/foo/bar.html
	nsString	mProtocol;

	char*		mRawPostData;
	PRUint32    mRawPostDataLength;
	PRBool      mbRawPostDataInFile;
	nsCOMPtr<nsIFile>  mRawPostDataFile;
	nsString    mRawPostDataContentType;

  	nsStringArray mHeaderNames;
  	nsStringArray mHeaderValues;

    nsStringArray mEnvNames;
    nsStringArray mEnvValues;

    nsString    mRemoteHost;
    nsString    mRemoteUser;
    nsString    mRemoteAddr;
    PRInt32     mRemotePort;
    PRInt32     mStatusCode;

    nsString	mPageFile;
    nsString	mDocumentRoot;
    nsString	mJaxerDir;
    PRBool		mIsJaxerServer;
    PRBool		mIsHTTPS;
	PRBool		mIsAdmin;
    nsString    mQueryString;
    nsString   mLogFileNativePath;
	
	nsTArray<aptPostDataItem> mPostDataItemArray;
	nsTArray<aptPostFileItem> mPostFileItemArray;

	nsCOMPtr<nsIStorageStream>	mDocumentStorageStream;
	nsCOMPtr<nsIURL>			mDocumentURL;
	
};

#endif  



