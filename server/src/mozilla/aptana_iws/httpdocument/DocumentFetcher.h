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
/* vim: set ts=4 sw=4 et: */
#ifndef DocumentFetcher____h
#define DocumentFetcher____h

//#include "aptCoreTrace.h"

#include "nsICookieManager.h"
#include "JaxerDefs.h"

class aptIHTTPRequest;
class aptEventTypeManager;
class IFCGXStream;
class nsIFileInputStream;
class nsIStringInputStream;
struct PRFileDesc;
class aptConsoleListener;


///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// DocumentFetcher
//***************************************************************************** 

class DocumentFetcher :
     public nsIWebProgressListener,
     public nsSupportsWeakReference
{
public:
  DocumentFetcher();
  ~DocumentFetcher();

  NS_DECL_AND_IMPL_ZEROING_OPERATOR_NEW

  PRBool IsDone() const { return mDone; }
  PRBool HasDefaultCallbackDoc() const { return mHasDefaultCallbackDoc; }

  void ResetIsDone();

  nsresult Init();

  //nsresult FetchFCGX(IFCGXStream *fcgx);
  nsresult ProcessRequest(PRInt32 fd);
  nsresult ProcessHTTPRequest(PRInt32 fd, eMSG_TYPE type);

  // nsresult SetFileUploadTmpDir(const char* documentRoot);
  nsresult SetDefaultCallbackDoc(const char* defaultCBDocURI);
  nsresult LoadFrameworkFailureErrorDoc(const char* uri);

  void UpdatePrefSettings(nsIPrefBranch *aPrefBranch, const char* pref = nsnull);

protected:
  nsCOMPtr<nsIDocShell> mDocShell;
  nsCOMPtr<nsIWebProgress> mProgress;
  nsCOMPtr<aptEventTypeManager> mETM;
  nsCOMPtr<nsIFile> mDefaultCallbackDocFile;
  PRBool mHasDefaultCallbackDoc;
  nsCOMPtr<nsIFileInputStream> mDefaultCallbackFileStream;
  nsCOMPtr<nsIStringInputStream> mDefaultCallbackTextStream;
  nsCOMPtr<nsICookieManager> mCookieManager;
  
  PRBool mDone;
  nsresult ProcessMultiPartData(IFCGXStream *fp, const char* boundary, aptIHTTPRequest* pReq);
  nsresult ProcessPostData(nsCAutoString &urlEncodedData, aptIHTTPRequest* pReq);
  //nsresult OpenNewUniqFileForWriting(char* buf, FILE **pfp);
  
  nsresult CreateUniqFile(char* filename, PRFileDesc **ppfd);


private:
    // char mFileUploadTmpDir[BUFSIZ];
    char *mFrameworkFailureDoc;
    PRBool mDeleteFrameworkFailureDoc;
    PRBool mBypassJaxerIfFrameworkFailToLoad;
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIWEBPROGRESSLISTENER
};


#endif /* DocumentFetcher____h */
