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

#include "aptEventBaseData.h"
#include "nsComponentManagerUtils.h"
#include "nsServiceManagerUtils.h"
#include "nsString.h"
#include "nsDirectoryService.h"
#include "nsDirectoryServiceDefs.h"
#include "nsIDOMDocument.h"
#include "aptIHTTPRequest.h"
#include "aptIHTTPResponse.h"

// Yes, this is an abomination, but there isn't any other way to always get these to
// JS code.
static aptIHTTPRequest *g_Request = 0;
static aptIHTTPResponse *g_Response = 0;

////////////////////////////////////////////////////////////////////////////////

aptEventBaseData::aptEventBaseData() 
{
	mDOM = nsnull; 
	mRequest = nsnull; 
	mResponse = nsnull; 
	mPageFile = nsnull;
	mDocumentRoot = nsnull;
	mRequestIsJaxerServer = false;
}

////////////////////////////////////////////////////////////////////////////////

aptEventBaseData::~aptEventBaseData()
{
	if (nsnull != mRequest)
	{
  		NS_RELEASE(mRequest);
		mRequest = nsnull; 
	}

	if (nsnull != mResponse)
	{
  		NS_RELEASE(mResponse);
		mResponse = nsnull; 
	}

	if (nsnull != mDOM)
	{
  		NS_RELEASE(mDOM);
		mDOM = nsnull; 
	}

	if (nsnull != mPageFile)
	{
  		nsMemory::Free(mPageFile);
		mPageFile = nsnull; 
	}
	if (nsnull != mDocumentRoot)
	{
  		nsMemory::Free(mDocumentRoot);
		mDocumentRoot = nsnull; 
	}

}

////////////////////////////////////////////////////////////////////////////////

NS_IMPL_ISUPPORTS1(aptEventBaseData, aptIEventBase)


NS_IMETHODIMP 
aptEventBaseData::Init()
{
	nsCOMPtr<nsIFile> jaxerDir;
    NS_GetSpecialDirectory(NS_OS_CURRENT_PROCESS_DIR, getter_AddRefs(jaxerDir));
  	nsCAutoString path;
  	nsresult rv = jaxerDir->GetNativePath(path);
	mJaxerDir = ToNewCString(path);

	NS_IF_RELEASE(mRequest);
	NS_IF_ADDREF(mRequest = g_Request);

	NS_IF_RELEASE(mResponse);
	NS_IF_ADDREF(mResponse = g_Response);

    return NS_OK;
}


NS_IMETHODIMP 
aptEventBaseData::GetDOM(nsIDOMDocument * *aDOM)
{
	if (nsnull == mDOM)
	{
	    *aDOM = nsnull;
	}
	else
	{
	    NS_ADDREF(*aDOM = mDOM);
	}

    return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::SetDOM(nsIDOMDocument * aDOM)
{
	// if we don't have the DOM yet
	if (nsnull == mDOM)
	{
		if (nsnull != aDOM)
		{
			NS_ADDREF(mDOM = aDOM);
		}
	}

    return NS_OK;
}


NS_IMETHODIMP 
aptEventBaseData::GetPageFile(nsAString & aPageFile)
{
	aPageFile.Assign(NS_ConvertUTF8toUTF16(mPageFile));

    return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::SetPageFile(const nsAString & aPageFile)
{
	if (mPageFile != nsnull) 
	{ 
		nsMemory::Free(mPageFile); 
		mPageFile = nsnull;
	}
	// process the PageFile arg
	if (aPageFile.Length())
	{
		mPageFile = ToNewUTF8String(aPageFile);
	}

    return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::GetDocumentRoot(nsAString & aDocumentRoot)
{
	aDocumentRoot.Assign(NS_ConvertUTF8toUTF16(mDocumentRoot));

    return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::SetDocumentRoot(const nsAString & aDocumentRoot)
{
	if (mDocumentRoot != nsnull) 
	{ 
		nsMemory::Free(mDocumentRoot); 
		mDocumentRoot = nsnull;
	}
	// process the DocumentRoot arg
	if (aDocumentRoot.Length())
	{
		mDocumentRoot = ToNewUTF8String(aDocumentRoot);
	}

    return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::GetRequest(aptIHTTPRequest * *aRequest)
{
	NS_IF_ADDREF(*aRequest = mRequest);
    return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::SetRequest(aptIHTTPRequest *aRequest)
{
	NS_IF_RELEASE(g_Request);
	NS_IF_ADDREF(g_Request = aRequest);
	return NS_OK;
}


NS_IMETHODIMP 
aptEventBaseData::GetResponse(aptIHTTPResponse * *aResponse)
{
	NS_IF_ADDREF(*aResponse = mResponse);
    return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::SetResponse(aptIHTTPResponse *aResponse)
{
	NS_IF_RELEASE(g_Response);
	NS_IF_ADDREF(g_Response = aResponse);
	return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::GetJaxerDir(nsAString & aJaxerDir)
{
	aJaxerDir.Assign(NS_ConvertUTF8toUTF16(mJaxerDir));

    return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::GetIsHandler(PRBool *bIsJaxerServer)
{
	*bIsJaxerServer = mRequestIsJaxerServer;
    return NS_OK;
}

NS_IMETHODIMP 
aptEventBaseData::SetIsHandler(PRBool bIsJaxerServer)
{
	mRequestIsJaxerServer = bIsJaxerServer;
	return NS_OK;
}
