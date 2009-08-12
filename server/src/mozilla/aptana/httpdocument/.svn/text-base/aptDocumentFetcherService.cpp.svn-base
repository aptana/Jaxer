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
#include "prmem.h"
#include "aptDocumentFetcherService.h"
#include "nsIComponentManager.h"
#include "nsThreadUtils.h"
#include "prlog.h"
#include "nsWebShell.h"
#include "nsIWebProgress.h"
#include "nsIObserver.h"
#include "nsIObserverService.h"
#include "nsIDOMDocument.h"
#include "nsIDOMHTMLDocument.h"
#include "nsIDOMSerializer.h"
#include "nsIDOMElement.h"
#include "nsIScriptGlobalObjectOwner.h"
#include "DocumentFetcher.h"
#include "nsCOMPtr.h"
#include "nsIComponentRegistrar.h"
#include "nsNetUtil.h"
#include "nsIWebNavigation.h"
#include "nsISupportsPrimitives.h"
#include "nsIPrefService.h"
#include "nsIScriptSecurityManager.h"
#include "nsDirectoryServiceDefs.h"
#include "nsIJSRuntimeService.h"
#include "jsapi.h"
#include "nsContentUtils.h"
#include "nsPIDOMWindow.h"
#include "nsIDocument.h"
#include "nsIDocumentEncoder.h"
#include "nsIStorageStream.h"
#include "nsContentCID.h"
#if 1
#include "ssl.h"
#else
// temporary workaround for building order
extern "C" {
	void SSL_ClearSessionCache(void);
}
#endif

#ifdef MOZ_CRASHREPORTER
//#include "nsExceptionHandler.h"
#endif

#include "aptEventTypeManager.h"
#include "aptEventRequestStartData.h"
#include "aptEventHTMLParseCompleteData.h"
#include "aptEventHTMLParseStartData.h"
#include "aptHTTPRequest.h"
#include "aptHTTPResponse.h"
#include "aptEventRequestCompleteData.h"
#include "IFCGXStream.h"
#include "FCGXStream.h"
#include "HTTPStream.h"
#include "nsIFileURL.h"
#include "nsIFileStreams.h"
#include "aptCoreTrace.h"
#include "aptConsoleListener.h"
#include "JaxerDefs.h"


#define _TIMESTATS

static NS_DEFINE_CID(kEventTypeManagerCID, APT_EVENTTYPEMANAGER_CID);
static NS_DEFINE_CID(kCookieServiceCID, NS_COOKIESERVICE_CID);

static char *frameworkFailedDoc=
    "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">"
    "<html><head><title>500 Internal Server Error</title></head><body><h2>Internal Server Error</h2><p>Further information has been logged.</p></body></html>";

static nsIDocShell *gCurrentDocShell = nsnull;
static PRInt32 sMaxBytes = 0;
static nsIURI *sAboutBlankURI = nsnull;
//static PRBool g_IdleLoop = PR_FALSE;

extern aptCoreTrace gJaxerLog;

class AutoCurrentDocShell {
  public:
    AutoCurrentDocShell(nsIDocShell *docShell) {
        NS_ASSERTION(!gCurrentDocShell, "DocShell should be null");

        NS_IF_ADDREF(docShell);
        gCurrentDocShell = docShell;
    }
    ~AutoCurrentDocShell() {
        NS_IF_RELEASE(gCurrentDocShell);
    }
};

DocumentFetcher::DocumentFetcher()
{
    // mFileUploadTmpDir[0] = 0;
    mHasDefaultCallbackDoc = PR_FALSE;
    mFrameworkFailureDoc = frameworkFailedDoc;
    mDeleteFrameworkFailureDoc = PR_FALSE;
    mBypassJaxerIfFrameworkFailToLoad = PR_FALSE;
}

DocumentFetcher::~DocumentFetcher()
{
    if (mProgress)
        mProgress->RemoveProgressListener(static_cast<nsIWebProgressListener*>(this));

    if (mDeleteFrameworkFailureDoc && mFrameworkFailureDoc)
        delete[] mFrameworkFailureDoc;

    nsCOMPtr<nsIWebNavigation> webNav(do_GetInterface(mDocShell));
    webNav->Stop(nsIWebNavigation::STOP_ALL);
    mDocShell = nsnull;
    mCookieManager = nsnull;

    NS_IF_RELEASE(sAboutBlankURI);
    mDefaultCallbackTextStream = nsnull;
    mDefaultCallbackFileStream = nsnull;
    mETM = nsnull;
}

//static const char* kJaxerEnableIdle = "Jaxer.enable_idle";
static const char* kJaxerBypassJaxer = "Jaxer.dev.BypassJaxerIfFrameworkFailToLoad";

// These are not reloaded for now
static const char* kJaxerDefaultcallbackDoc = "Jaxer.dev.LoadDocForCallback";
static const char* kJaxerErrorDoc = "Jaxer.dev.FailedLoadFrameworkErrorDoc";

void DocumentFetcher::UpdatePrefSettings(nsIPrefBranch *aPrefBranch, const char* pref)
{
    gJaxerLog.Log(eTRACE, "In DocumentFetcher::UpdatePrefSettings: pref=%s", pref ? pref : "null");
    if (!pref)
    {
        //aPrefBranch->GetBoolPref(kJaxerEnableIdle, &g_IdleLoop);
        
        aPrefBranch->GetBoolPref(kJaxerBypassJaxer, &mBypassJaxerIfFrameworkFailToLoad);
        
        PRInt32 nType = nsIPrefBranch::PREF_INVALID;
        nsresult rv = aPrefBranch->GetPrefType(kJaxerDefaultcallbackDoc, &nType);
        if (NS_SUCCEEDED(rv) && nsIPrefBranch::PREF_INVALID != nType)
        {
            nsAdoptingCString sCallbackDoc;
            rv = aPrefBranch->GetCharPref(kJaxerDefaultcallbackDoc, getter_Copies(sCallbackDoc));
            if (NS_FAILED(rv))
            {
                gJaxerLog.Log(eWARN, "Failed to get pref %s.  default callback doc not changed", kJaxerDefaultcallbackDoc);
            }else
            {
                rv = SetDefaultCallbackDoc(sCallbackDoc.get());
                if (NS_FAILED(rv))
                {
                    gJaxerLog.Log(eWARN, "Failed to set LoadDocForCallback to %s", sCallbackDoc.get());
                }else
                {
                    gJaxerLog.Log(eDEBUG, "%s will be loaded for each callback.", sCallbackDoc.get());
                }
            }
        }

        nType = nsIPrefBranch::PREF_INVALID;
        rv = aPrefBranch->GetPrefType(kJaxerErrorDoc, &nType);
        if (NS_SUCCEEDED(rv) && nsIPrefBranch::PREF_INVALID != nType)
        {
            nsAdoptingCString sErrorDoc;
            rv = aPrefBranch->GetCharPref(kJaxerErrorDoc, getter_Copies(sErrorDoc));
            if (NS_FAILED(rv))
            {
                gJaxerLog.Log(eWARN, "Failed to get pref %s.  default callback doc not changed", kJaxerErrorDoc);
            }else
            {
                rv = LoadFrameworkFailureErrorDoc(sErrorDoc.get());
                if (NS_FAILED(rv))
                {
                    gJaxerLog.Log(eWARN, "Failed to load FailedLoadFrameworkErrorDoc: %s", sErrorDoc.get());
                }
            }
        }
    //}
    //else if (strcmp(kJaxerEnableIdle, pref) == 0)
    //{
    //    aPrefBranch->GetBoolPref(kJaxerEnableIdle, &g_IdleLoop);
    }else if (strcmp(kJaxerBypassJaxer, pref) == 0)
    {
        aPrefBranch->GetBoolPref(kJaxerBypassJaxer, &mBypassJaxerIfFrameworkFailToLoad);
    }
}

NS_IMPL_ISUPPORTS2(DocumentFetcher, nsIWebProgressListener, nsISupportsWeakReference)


NS_IMETHODIMP
DocumentFetcher::OnStateChange(nsIWebProgress *aWebProgress, 
                               nsIRequest *aRequest, PRUint32 aStateFlags, 
                               nsresult aStatus)
{
    if ((~aStateFlags & (STATE_IS_DOCUMENT | STATE_STOP)) == 0) 
        mDone = PR_TRUE;

    return NS_OK;
}

NS_IMETHODIMP DocumentFetcher::OnProgressChange(nsIWebProgress *aWebProgress, 
                                                nsIRequest *aRequest, 
                                                PRInt32 aCurSelfProgress, PRInt32 aMaxSelfProgress, 
                                                PRInt32 aCurTotalProgress, PRInt32 aMaxTotalProgress)
{
    return NS_OK;
}

NS_IMETHODIMP DocumentFetcher::OnLocationChange(nsIWebProgress *aWebProgress, 
                                                nsIRequest *aRequest, nsIURI *location)
{
    return NS_OK;
}

NS_IMETHODIMP DocumentFetcher::OnStatusChange(nsIWebProgress *aWebProgress, 
                                              nsIRequest *aRequest, nsresult aStatus, 
                                              const PRUnichar *aMessage)
{
    return NS_OK;
}

NS_IMETHODIMP DocumentFetcher::OnSecurityChange(nsIWebProgress *aWebProgress, 
                                                nsIRequest *aRequest, PRUint32 state)
{
    return NS_OK;
}

nsresult
DocumentFetcher::Init()
{
    mDone = PR_FALSE;
    nsresult rv;

    // Get reference to the Aptana EventType manager.
    mETM = do_GetService(kEventTypeManagerCID, &rv);
    if (!mETM)
    {
        gJaxerLog.Log(eERROR, "Get EventTypeManager service failed");
        return NS_ERROR_UNEXPECTED;
    }

    mDefaultCallbackFileStream = do_CreateInstance(NS_LOCALFILEINPUTSTREAM_CONTRACTID);
    if (!mDefaultCallbackFileStream)
    {
        gJaxerLog.Log(eERROR, "Create Default Callback File Stream failed");
        return NS_ERROR_UNEXPECTED;
    }

    mDefaultCallbackTextStream = do_CreateInstance("@mozilla.org/io/string-input-stream;1");
    if (!mDefaultCallbackTextStream)
    {
        gJaxerLog.Log(eERROR, "Create Default Callback Text Stream failed");
        return NS_ERROR_UNEXPECTED;
    }
    
    rv = NS_NewURI(&sAboutBlankURI, "about:blank");
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "Create about:black URI failed");
        return rv;
    }

    mCookieManager = do_GetService(kCookieServiceCID, &rv);
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "Get CookieService failed");
        return rv;
    }

    // Create shell
    mDocShell = do_CreateInstance("@mozilla.org/webshell;1");
    if (!mDocShell)
    {
        gJaxerLog.Log(eERROR, "Create webshell failed.");
        return NS_ERROR_FAILURE;
    }

    nsCOMPtr<nsIPrincipal> principal;
    nsCOMPtr<nsIScriptSecurityManager> secMan = do_GetService(NS_SCRIPTSECURITYMANAGER_CONTRACTID, &rv);
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "Get nsIScriptSecurityManager service faile.");
        return rv;
    }

    rv = secMan->GetSystemPrincipal(getter_AddRefs(principal));
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "GetSystemPrincipal faile.");
        return rv;
    }

    nsCOMPtr<nsPIDOMWindow> piWindow = do_GetInterface(mDocShell);
    piWindow->SetOpenerScriptPrincipal(principal);

    // Attach our progress listener to the shell
    mProgress = do_GetInterface(this->mDocShell);
    mProgress->AddProgressListener(this, nsIWebProgress::NOTIFY_STATE_DOCUMENT);

    return NS_OK;
}


void
DocumentFetcher::ResetIsDone()
{
    mDone = PR_FALSE;
}

static PRInt32 FindToken(const PRUint8 *pSrc, PRInt32 nSrcLen, const PRUint8 *pToken, PRInt32 nTokenLen)
{
    // A simple wrapper to find an arbitrary sequence of
    // bytes from the internal buffer.  If it finds it,
    // return the index, else return -1.
    PRInt32 i;
    if (nTokenLen > nSrcLen)
        return -1;

    for (i = 0; i < (nSrcLen - nTokenLen); i++)
    {
        if (memcmp(pSrc + i, pToken, nTokenLen) == 0)
            return i;
    }

    return -1;
}

static PRBool RefillBuf(char* buf, const PRInt32 nBufSize, PRInt32 &nStart, PRInt32 &nEnd, IFCGXStream *fcgx)
{
    // The buf has too little stuff for processing.  Move the end part to beginning,
    // and fill the rest.  Update all parameters.
    // Return true if we (may) still have more data.
    PRBool ret = PR_TRUE;

    PRInt32 nRoom = nBufSize;
    PRInt32 nOffset = 0;
    if (nStart > 0) {
        nOffset = nEnd - nStart;
        for (PRInt32 i = 0; i < nOffset; i++)
            buf[i] = buf[nStart + i];
    }else
        nOffset = nEnd;
    nRoom -= nOffset;

    while (nRoom > 0) {
        PRUint32 nRead;
        nsresult rv = fcgx->Read(buf + nOffset, nRoom, &nRead);
        if (NS_FAILED(rv))
        {
            gJaxerLog.Log(eERROR, "RefillBuf: Read postdata failed.");
            ret = PR_FALSE;
            return ret;
        }

        if (nRead == 0)
        {
            ret = PR_FALSE;
            break;
        }
        nOffset += nRead;
        nRoom -= nRead;
    }

    nEnd = nOffset;
    nStart = 0;

    return ret;
}


nsresult DocumentFetcher::SetDefaultCallbackDoc(const char* uri)
{
    // Convert the string uri into nsIFile.

    nsresult rv = NS_OK;
    PRBool b;

    // Make sure the arg is not null or empty
    if (!uri || !uri[0])
        return NS_ERROR_INVALID_ARG;

    // Create nsIURI interface using teh supplied string uri
    nsCOMPtr<nsIURI> URI;
    rv = NS_NewURI(getter_AddRefs(URI), uri);
    NS_ENSURE_SUCCESS(rv, rv);

    // Get the IFileURL interface
    nsCOMPtr<nsIFileURL> fileURL(do_QueryInterface(URI));
    NS_ENSURE_TRUE(fileURL, NS_ERROR_INVALID_ARG);

    // Get IFile interface
    rv = fileURL->GetFile(getter_AddRefs(mDefaultCallbackDocFile));
    NS_ENSURE_SUCCESS(rv, rv);

    // Make sure the file exists and can be opened
    rv =mDefaultCallbackDocFile->Exists(&b);
    NS_ENSURE_SUCCESS(rv, rv);
    if (!b)
        return NS_ERROR_FILE_NOT_FOUND;


    rv = mDefaultCallbackDocFile->IsDirectory(&b);
    NS_ENSURE_SUCCESS(rv, rv);
    if (b)
        return NS_ERROR_FILE_IS_DIRECTORY;

    rv = mDefaultCallbackDocFile->IsReadable(&b);
    NS_ENSURE_SUCCESS(rv, rv);
    if (!b)
        return NS_ERROR_FILE_ACCESS_DENIED;


    mHasDefaultCallbackDoc = PR_TRUE;

    return NS_OK;
}

nsresult DocumentFetcher::LoadFrameworkFailureErrorDoc(const char* uri)
{
    // Convert the string uri into nsIFile.

    nsresult rv = NS_OK;
    PRBool b;

    // Make sure the arg is not null or empty
    if (!uri || !uri[0])
        return NS_ERROR_INVALID_ARG;

    // Create nsIURI interface using teh supplied string uri
    nsCOMPtr<nsIURI> URI;
    rv = NS_NewURI(getter_AddRefs(URI), uri);
    NS_ENSURE_SUCCESS(rv, rv);

    // Get the IFileURL interface
    nsCOMPtr<nsIFileURL> fileURL(do_QueryInterface(URI));
    NS_ENSURE_TRUE(fileURL, NS_ERROR_INVALID_ARG);

    // Get IFile interface
    nsCOMPtr<nsIFile> aFile;
    rv = fileURL->GetFile(getter_AddRefs(aFile));
    NS_ENSURE_SUCCESS(rv, rv);

    // Make sure the file exists and can be opened
    rv =aFile->Exists(&b);
    NS_ENSURE_SUCCESS(rv, rv);
    if (!b)
        return NS_ERROR_FILE_NOT_FOUND;


    rv = aFile->IsDirectory(&b);
    NS_ENSURE_SUCCESS(rv, rv);
    if (b)
        return NS_ERROR_FILE_IS_DIRECTORY;

    rv = aFile->IsReadable(&b);
    NS_ENSURE_SUCCESS(rv, rv);
    if (!b)
        return NS_ERROR_FILE_ACCESS_DENIED;

    // Get local file interface
    nsCOMPtr<nsILocalFile>  localFile;
    localFile = do_QueryInterface(aFile);
    NS_ENSURE_TRUE(localFile, NS_ERROR_NO_INTERFACE);

    PRInt64 fileSize;
    PRInt32 flen;
    if(NS_FAILED(localFile->GetFileSize(&fileSize)) || !(flen = nsInt64(fileSize)))
    {
        return !NS_OK;
    }

    // We can Open the file for, say writing
    PRFileDesc *fd;
    rv = localFile->OpenNSPRFileDesc(PR_RDONLY, 444, &fd);
    NS_ENSURE_SUCCESS(rv, rv);

    // Read the Doc
    char *p = mFrameworkFailureDoc;
    mFrameworkFailureDoc = new char[flen+1];
    if (!mFrameworkFailureDoc)
    {
        PR_Close(fd);
        mFrameworkFailureDoc = p;
        return !NS_OK;
    }

    if (flen > PR_Read(fd, mFrameworkFailureDoc, flen))
    {
        PR_Close(fd);
        delete[] mFrameworkFailureDoc;
        mFrameworkFailureDoc = p;
        return !NS_OK;
    }

    mFrameworkFailureDoc[flen] = 0;
    mDeleteFrameworkFailureDoc = PR_TRUE;

    return NS_OK;
}


nsresult DocumentFetcher::CreateUniqFile(char* filename, PRFileDesc **ppfd)
{
    nsresult rv;
    /*
    nsCOMPtr<nsILocalFile> tmpfile(do_CreateInstance(NS_LOCAL_FILE_CONTRACTID));
    NS_ENSURE_TRUE(tmpfile, NS_ERROR_FAILURE);


    rv = tmpfile->InitWithNativePath(nsDependentCString(mFileUploadTmpDir));
    NS_ENSURE_SUCCESS(rv, rv);
    */
    
    nsCOMPtr<nsIFile> file;
    NS_GetSpecialDirectory(NS_OS_TEMP_DIR, getter_AddRefs(file));
    nsCOMPtr<nsILocalFile>  tmpfile(do_QueryInterface(file));
    NS_ENSURE_TRUE(tmpfile, NS_ERROR_NO_INTERFACE);

    rv = tmpfile->Append(NS_ConvertASCIItoUTF16("tmp"));
    NS_ENSURE_SUCCESS(rv, rv);

    // Create the temporary file. This may adjust the file's basename.
    rv = tmpfile->CreateUnique(nsIFile::NORMAL_FILE_TYPE, 0644);
    NS_ENSURE_SUCCESS(rv, rv);

    nsAutoString tmpfilename;
    rv = tmpfile->GetPath(tmpfilename);
    NS_ENSURE_SUCCESS(rv, rv);

    strcpy(filename, NS_ConvertUTF16toUTF8(tmpfilename).get());

    // *ppfd = PR_Open(filename, PR_WRONLY | PR_CREATE_FILE, 0664);
    rv = tmpfile->OpenNSPRFileDesc(PR_WRONLY | PR_CREATE_FILE, 0664, ppfd);

    return rv;
}

static PRInt32 htoi(char *s)
{
    PRInt32 value;
    PRInt32 c;

    c = ((unsigned char *)s)[0];
    if (isupper(c))
        c = tolower(c);
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

    c = ((unsigned char *)s)[1];
    if (isupper(c))
        c = tolower(c);
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

    return (value);
}

static PRInt32 rawURlDecode(char* urlEncodedData, PRInt32 len)
{
    // inplace decode the string and return the length of the decoded string.
    char *dest = urlEncodedData;
    char *data = urlEncodedData;

    while (len--)
    {
        if (*data == '+')
        {
            *dest = ' ';
        } else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) 
                 && isxdigit((int) *(data + 2)))
        {
            *dest = (char) htoi(data + 1);
            data += 2;
            len -= 2;
        } else {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = '\0';
    return dest - urlEncodedData;
}

nsresult
DocumentFetcher::ProcessPostData(nsCAutoString &urlEncodedData, aptIHTTPRequest* pReq)
{
    nsresult rv = NS_OK;
    char *data = urlEncodedData.BeginWriting();

    // extract each name=val
    char *pch = strtok(data, "&");
    while (pch != NULL)
    {
        // decode name=value
        rawURlDecode(pch, strlen(pch));

        // extract name & value
        char* value = strchr(pch, '=');
        if (value) *value++ = 0;

        NS_ConvertUTF8toUTF16 sName(pch);
        NS_ConvertUTF8toUTF16 sValue(value);

        // set name/value pair
        rv = pReq->AddPostStringItem(sName, sValue);
        NS_ENSURE_SUCCESS(rv, rv);

        // move on
        pch = strtok(NULL, "&");
    }
    return rv;
}

nsresult
DocumentFetcher::ProcessMultiPartData(IFCGXStream *fcgx, const char* boundary, aptIHTTPRequest* pReq)
{
    nsresult rv = NS_OK;
    // TODO: 
    // 3. file dir, name

    static const PRInt32 nMaxBufSize = 1024 * 64;
    PRInt32 nBufSize = nMaxBufSize;
    // PRInt32 nRead = 0;
    char buf[nMaxBufSize + 1];
    // const char* token1 = "\r\n";
    const char* token2 = "\r\n\r\n";
    char token3[256];
    strcpy(token3, "\r\n--");
    strcat(token3, boundary);
    PRInt32 nToken3 = strlen(token3);

    PRInt32 nIndex = -1;
    PRInt32 nIndex1 = -1;
    bool bIsFile = false;
    char sName[BUFSIZ];
    nsCAutoString sValue;

    
    char sFileName[BUFSIZ];
    char sContentType[256];
    PRInt32 nStart = 0;
    PRInt32 nEnd = 0;
    // FILE *fpDisk;
    char sFullPath[BUFSIZ];
    PRInt32 nSize = 0;
    PRInt32 nItemCount = 0;
    PRBool bHasMore = PR_TRUE;
    
    gJaxerLog.Log(eTRACE, "Entered ProcessMultiPartData");

    bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);
    
    
    while ((nEnd-nStart) > (6+nToken3))
    {
        // if we are at the end of the data, then we will have either
        // "--\r\n" or "--\r\n\r\n"
        // so if we have 6 or less chars, do not bother
        
        gJaxerLog.Log(eTRACE, "ProcessMultiPartData: Parsing next item.");

        sName[0] = 0;
        sValue.Truncate();

        sFileName[0] = 0;
        bIsFile = false;
        nSize = 0;
        nItemCount++;
        // nsCOMPtr<nsILocalFile> tmpFile;
        PRFileDesc *fd;

        if (bHasMore && nEnd > 0 && (nEnd-nStart) < 100)
            bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);

        if (nStart >= nEnd)
        {
            gJaxerLog.Log(eERROR, "ProcessMultiPartData: RefillBuf returned unexpected indexes: start=%d end=%d", nStart, nEnd);
            return NS_ERROR_UNEXPECTED;
        }
        // a field must start with name=
        nIndex = FindToken((const PRUint8 *) buf+nStart, nEnd-nStart, (const PRUint8 *)"name=\"", 6);
        if (nIndex == -1) {
            // generate name based on index
            // I do not see we can get here.  If you do not have a name, the field is not sent to the server!
            sprintf(sName, "NoNameField_%d", nItemCount);
        } else {
            nStart += nIndex + 6;
            PRInt32 idx = 0;
            while (buf[nStart] != '"') {
                if (bHasMore && nEnd > 0 && nStart >= nEnd)
                    bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);
                if (nStart>=nEnd)
                {
                    gJaxerLog.Log(eERROR, "ProcessMultiPartData: Parsing a name field failed");
                    return NS_ERROR_UNEXPECTED;
                }
                sName[idx++] = buf[nStart++];
            }
            sName[idx] = 0;
        }
        
        if (bHasMore && nEnd > 0 && (nEnd-nStart) < 20)
            bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);

        // check to see if it is a file.  This has to be on the same line, so we search only a few bytes
        // otherwise, we may got the next item.
        nIndex = FindToken((const PRUint8 *) buf+nStart, (nEnd>=nStart+20) ? 20 : nEnd-nStart, (const PRUint8 *)"filename=\"", 10);
        if (nIndex>0 && nIndex<10) {
            bIsFile = true;
            nStart += nIndex + 10;
            PRInt32 idx = 0;
            while (buf[nStart] != '"') {
                if (bHasMore && nEnd > 0 && nStart >= nEnd)
                    bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);
                if (nStart >= nEnd)
                {
                    gJaxerLog.Log(eERROR, "ProcessMultiPartData: Parsing a filename field failed");
                    return NS_ERROR_UNEXPECTED;
                }
                sFileName[idx++] = buf[nStart++];
            }
            sFileName[idx] = 0;
            // replace '\' by '/'
            PRInt32 k;
            for (k=0; sFileName[k] != 0; k++) {
                if (sFileName[k] == '\\') sFileName[k] = '/';
            }

            rv = CreateUniqFile(sFullPath, &fd);
            if (NS_FAILED(rv))
            {
                gJaxerLog.Log(eERROR, "ProcessMultiPartData: CreateUniqueFile failed: rv=0x%x.", rv);
                return rv;
            }

            // get content-type, again do not search too far
            if (bHasMore && nEnd > 0 && (nEnd-nStart) < 256)
                bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);
            nIndex = FindToken((const PRUint8 *) buf+nStart, (nEnd >= nStart+30) ? 30 : nEnd-nStart, (const PRUint8 *)"Content-Type:", 13);
            if (nIndex>0 && nIndex<10) {
                nStart += nIndex + 13;
                PRInt32 idy = 0;
                if (buf[nStart] == ' ') nStart++;

                while (buf[nStart] != '\r' && buf[nStart] != '\n') {
                    if (bHasMore && nEnd > 0 && nStart >= nEnd)
                        bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);
                    if (nStart >= nEnd)
                    {
                        gJaxerLog.Log(eERROR, "ProcessMultiPartData: Parsing Content-Type failed");
                        return NS_ERROR_UNEXPECTED;
                    }
                    sContentType[idy++] = buf[nStart++];
                }
                sContentType[idy] = 0;
            } else
                strcpy(sContentType, "");
        } else
            bIsFile = false;
    

        // move on to the real data
        if (bHasMore && nEnd > 0 && (nEnd-nStart) < 100)
            bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);

        nIndex = FindToken((const PRUint8 *) buf+nStart, nEnd-nStart, (const PRUint8 *) token2, 4);
        if (nIndex < 0) {
            // something unexpected happened
            rv = NS_ERROR_UNEXPECTED;
            gJaxerLog.Log(eERROR, "ProcessMultiPartData: Looking for token (%s) failed.", token2);
            gJaxerLog.Log(eDEBUG, "ProcessMultiPartData: nStart=%d nEnd=%d.", nStart, nEnd);
            return rv;
        }
        nStart += nIndex+4;
        nIndex1 = FindToken((const PRUint8 *) buf+nStart, nEnd-nStart, (const PRUint8 *) token3, nToken3);
        while (nIndex1 == -1) {
            if (bIsFile) {
                // save file content from nStart to nBufSize-nToken3
                // fwrite(buf+nStart, nBufSize-nStart-nToken3, 1, fpDisk);
                PRInt32 bytesWritten = PR_Write(fd, buf+nStart, nEnd-nStart-nToken3);
                if (bytesWritten != (nEnd-nStart-nToken3))
                {
                    gJaxerLog.Log(eERROR, "ProcessMultiPartData: PR_Write failed. Writing=%d written=%d", 
                        nEnd-nStart-nToken3, bytesWritten);
                    return NS_ERROR_UNEXPECTED;
                }

                nSize += nEnd-nStart-nToken3;

            } else {
                // append nStart to nBufSize-nToken3 to Value
				buf[nStart+nEnd-nStart-nToken3]= 0;
				nsDependentCString partValue(buf+nStart, nEnd-nStart-nToken3);
				sValue.Append(partValue);
                //memcpy(sValue+nSize, buf+nStart, nEnd-nStart-nToken3);
                nSize += nEnd-nStart-nToken3;;
            }

            if (nEnd > 0) {
                nStart = nEnd-nToken3;
                if (bHasMore)
                    bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);
            } else {
                // we should never be here
                gJaxerLog.Log(eERROR, "ProcessMultiPartData: Unexpected end of postdata");
                return NS_ERROR_UNEXPECTED;
            }
            
            nIndex1 = FindToken((const PRUint8 *) buf+nStart, nEnd-nStart, (const PRUint8 *) token3, nToken3);
        }
                
        // now we have find the end
        if (bIsFile) {
            // save data from start to nIndex1
            // fwrite(buf+nStart, nIndex1, 1, fpDisk);
            PRInt32 bytesWritten = PR_Write(fd, buf+nStart, nIndex1);
            if (bytesWritten != nIndex1)
            {
                gJaxerLog.Log(eERROR, "ProcessMultiPartData: PR_Write failed. nIndex1=%d written=%d", 
                        nIndex1, bytesWritten);
                    
                return NS_ERROR_UNEXPECTED;
            }
            nSize += nIndex1;
            // fclose(fpDisk);
            PR_Close(fd);
            if (sFileName[0] == 0 && nSize == 0) {
                remove(sFullPath);
            } else {
                if (sFileName[0] == 0)
                    sprintf(sFileName, "Noname_%d", nItemCount);
                rv = pReq->AddPostFileItem(NS_ConvertUTF8toUTF16(sName),
                    NS_ConvertASCIItoUTF16(sFileName),
                    //NS_ConvertUTF8toUTF16(sFileName),
                    NS_ConvertUTF8toUTF16(sFullPath), 
                    NS_ConvertUTF8toUTF16(sContentType),
                    nSize);
                if (NS_FAILED(rv))
                {
                    gJaxerLog.Log(eERROR, "ProcessMultiPartData: AddPostFileItem failed: name=%s fname=%s", sName, sFileName);
                    return rv;
                }
                gJaxerLog.Log(eTRACE, "ProcessMultiPartData: Parsed file: name=%s filename=%s path=%s contentType=%s size=%d",
                    sName, sFileName, sFullPath, sContentType, nSize);
            }
        } else {
            // append to value
            // strncat(sValue, buf+nStart, nIndex1);
			buf[nStart+nIndex1] = 0;
			nsDependentCString partValue2(buf+nStart, nIndex1);
			sValue.Append(partValue2);
            //memcpy(sValue+nSize, buf+nStart, nIndex1);
            nSize += nIndex1;
            //sValue[nSize] = 0;
            rv = pReq->AddPostStringItem(NS_ConvertUTF8toUTF16(sName),
                NS_ConvertUTF8toUTF16(sValue));
            if (NS_FAILED(rv))
            {
                gJaxerLog.Log(eERROR, "ProcessMultiPartData: AddPostStringItem failed: name=%s value=%s", sName, sValue.get());
                return rv;
            }
            gJaxerLog.Log(eTRACE, "ProcessMultiPartData: Parsed data: name=%s value=%s", sName, sValue.get());
        }
        
        nStart += nIndex1 + strlen(token3);

        // Need to refill buf
        if (bHasMore && (nEnd - nStart) <= (6+nToken3))
        {
            bHasMore = RefillBuf(buf, nBufSize, nStart, nEnd, fcgx);
        }
    }
    if (bHasMore)
    {
        // We did not read all postdata
        gJaxerLog.Log(eERROR, "ProcessMultiPartData: Not all post data are processed");
        return NS_ERROR_UNEXPECTED;

    }
    gJaxerLog.Log(eTRACE, "ProcessMultiPartData: Done");
    return rv;
}

// This is used to construct a dummy page for callback.
static const char *htmlDoc = "<html><head><title>callback</title></head><body></body></html>";

#ifdef MOZ_MEMORY_DEBUG
#ifndef XP_WIN
extern "C" {
extern void malloc_print_stats(void);
}
#endif
#endif

#define LOG_RETURN_IF_ERROR(eLevel, rv, msg) \
if (NS_FAILED(rv)) \
{ \
  gJaxerLog.Log(eLevel, "%s rv=0x%x", msg, rv); \
  return rv; \
}

#define LOG_RETURN(eLevel, rv, msg) \
{ \
    gJaxerLog.Log(eLevel, msg); \
    return rv; \
}

nsresult
DocumentFetcher::ProcessRequest(PRInt32 fd)
{
    nsresult rv;
    gJaxerLog.Log(eTRACE, "Entered ProcessRequest");

#ifdef _TIMESTATS
    int64 startTime;
    if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
        startTime = JS_Now();
    }
#endif /* _TIMESTATS */

    FCGXStream *fcgx = new FCGXStream();
    if (!fcgx)
    {
        //out of memory
        gJaxerLog.Log(eERROR, "Create new FCGXStream failed");
        return NS_ERROR_OUT_OF_MEMORY;
    }
    // NS_IF_ADDREF(fcgx);
    nsCOMPtr<IFCGXStream> dummy(fcgx);

#if 0
    //nsCOMPtr<IFCGXStream> fcgx;
    //rv = APT_NewFCGXStream(getter_AddRefs(fcgx));
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "Create new FCGXStream failed, rv=0x%x", rv);
        return rv;
    }
#endif

    fcgx->SetWebSocket(fd);

    // Reset the completion flag
    mDone = PR_FALSE;

#ifdef MOZ_MEMORY_DEBUG
#ifndef XP_WIN
    malloc_print_stats();
#endif
#endif

    rv = mCookieManager->RemoveAll();
    LOG_RETURN_IF_ERROR(eFATAL, rv, "Failed to remove cookies.")
    
    SSL_ClearSessionCache();

    //first get the request type.  If the function is successful, the type is fine.
    PRInt32 reqType;
    rv = fcgx->NextRequest(&reqType);
    LOG_RETURN_IF_ERROR(eFATAL, rv, "Failed to get next request.")

    // Create HTTPRequest object.
    nsCOMPtr<aptIHTTPRequest> req;
    req = do_CreateInstance(APT_HTTP_REQUEST_CONTRACTID);
    if (!req)
        LOG_RETURN(eFATAL, NS_ERROR_OUT_OF_MEMORY, "do_CreateInstance failed for aptIHTTPRequest")

    // Create HTTPResponse object.
    nsCOMPtr<aptIHTTPResponse> resp;
    resp = do_CreateInstance(APT_HTTP_RESPONSE_CONTRACTID);
    if (!resp)
        LOG_RETURN(eFATAL, NS_ERROR_OUT_OF_MEMORY, "do_CreateInstance failed for aptIHTTPResponse")

    // Capture the HTTP Request headers.
    PRInt32 n;
    rv = fcgx->GetRequestHeaderCount(&n);
    LOG_RETURN_IF_ERROR(eFATAL, rv, "Get request Header counter failed.")
    
    while (n > 0) {
        nsCAutoString name, value;
        rv = fcgx->GetRequestHeader(name, value);
        LOG_RETURN_IF_ERROR(eFATAL, rv, "Get request Header value failed.")

        rv = req->AppendHeader(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
        LOG_RETURN_IF_ERROR(eFATAL, rv, "Add request Header value failed.")
        n--;
    }

    if (reqType == IFCGXStream::RT_Filter)
    {
        rv = fcgx->GetResponseHeaderCount(&n);
        LOG_RETURN_IF_ERROR(eFATAL, rv, "Get response Header counter failed.")
        
        while (n > 0) {
            nsCAutoString name, value;
            rv = fcgx->GetResponseHeader(name, value);
            LOG_RETURN_IF_ERROR(eFATAL, rv, "Get response Header value failed.")

            rv = resp->AddHeader(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value), PR_TRUE);
            LOG_RETURN_IF_ERROR(eFATAL, rv, "Add response Header value failed.")
            n--;
        }
    }

    // Capture the environment variables.
    rv = fcgx->GetEnvVarCount(&n);
    LOG_RETURN_IF_ERROR(eFATAL, rv, "Get HTTP Env Header counter failed.")

    nsCAutoString envContentType;
    nsCAutoString envDocContentType;
    nsCAutoString envCharset;
    nsCAutoString envQueryString;
    nsCAutoString envRequestMethod;
    nsCAutoString envHost;
    nsCAutoString envScriptName;
    PRBool isHTTPS = PR_FALSE;
    PRBool isHandler = (reqType == IFCGXStream::RT_Handler);

    while (n > 0) {
        nsCAutoString name, value;
        rv = fcgx->GetEnvVar(name, value);
        LOG_RETURN_IF_ERROR(eFATAL, rv, "Get HTTP Env value failed.")

        switch (name[0]) {
            case 'C':
                if (name.Equals("CONTENT_TYPE"))
                {
                    envContentType = value;
                    rv = req->SetRawPostDataContentType(NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                }else {
                    rv = req->AddEnv(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                }
                break;
            case 'D':
                if (name.Equals("DOCUMENT_ROOT")) {
                    rv = req->SetDocumentRoot(NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set document root failed.")
                } else if (name.Equals("DOC_CONTENT_TYPE")) {
                    envDocContentType = value;
                }else {
                    rv = req->AddEnv(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                }
                break;
            case 'H':
                if (name.Equals("HTTP_HOST"))
                {
                    envHost = value;
                    rv = req->AddEnv(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                }else if (name.Equals("HTTPS"))
                {
                    if (value.Equals("on"))
                        req->SetIsHTTPS(isHTTPS = PR_TRUE);
                    else
                        req->SetIsHTTPS(isHTTPS = PR_FALSE);
                }else
                {
                    rv = req->AddEnv(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                }
                break;
#if 0
            case 'J':
                if (name.Equals("JAXER_REQ_TYPE"))
                    isHandler = (value.Equals("1"));
                break;
#endif
            case 'Q':
                if (name.Equals("QUERY_STRING"))
                {
                    envQueryString = value;
                    rv = req->SetQueryString(NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set query string failed.")
                }else
                {
                    rv = req->AddEnv(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                }
                break;
            case 'R':
                if (name.Equals("REQUEST_METHOD"))
                {
                    envRequestMethod = value;
                    req->SetMethod(NS_ConvertUTF8toUTF16(value));
                }else if (name.Equals("REQUEST_URI"))
                {
                    req->SetUri(NS_ConvertUTF8toUTF16(value));
                } else if (name.Equals("REMOTE_ADDR"))
                    req->SetRemoteAddr(NS_ConvertUTF8toUTF16(value));
                else if (name.Equals("REMOTE_PORT"))
                    req->SetRemotePort(atoi(value.get()));
                else if (name.Equals("REMOTE_HOST"))
                    req->SetRemoteHost(NS_ConvertUTF8toUTF16(value));
                else if (name.Equals("REMOTE_USER"))
                    req->SetRemoteUser(NS_ConvertUTF8toUTF16(value));
                else
                {
                    rv = req->AddEnv(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                }

                break;
            case 'S':
                if (name.Equals("SERVER_PROTOCOL"))
                    req->SetProtocol(NS_ConvertUTF8toUTF16(value));
                else if (name.Equals("SCRIPT_FILENAME"))
                    req->SetPageFile(NS_ConvertUTF8toUTF16(value));
                else if (name.Equals("STATUS_CODE"))
                    req->SetStatusCode(atoi(value.get()));
                else if (name.Equals("SCRIPT_NAME"))
                {
                    envScriptName = value;
					gJaxerLog.Log(eDEBUG, "Request path: %s", PromiseFlatCString(value).get());
                    rv = req->AddEnv(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                }else
                {
                    rv = req->AddEnv(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
                    LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                }

                break;

            default:
                rv = req->AddEnv(NS_ConvertUTF8toUTF16(name), NS_ConvertUTF8toUTF16(value));
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Set env var failed.")
                break;
        }
        n--;
    }

    req->SetIsHandler(isHandler);
    gJaxerLog.Log(eDEBUG, "Jaxer is handler for request? (%d)", isHandler);

    PRBool bProcessDocument = (PL_strcasecmp(envRequestMethod.BeginReading(), "options") != 0);

    // Allocate a RequestStart event object
    nsCOMPtr<aptIEventRequestStart> rqs = do_CreateInstance(APT_EVENT_REQUEST_START_CONTRACTID);
    if (!rqs)
        LOG_RETURN(eFATAL, NS_ERROR_OUT_OF_MEMORY, "do_CreateInstance failed for aptIEventRequestStart")

    // Make request/response available to events.  Note: this is setting global variables
    // inside aptEventBaseData and will be available to all future events until reset.
    rqs->SetRequest(req);
    rqs->SetResponse(resp);

    // Set some state for the event.  We got nothing now.  If we need something meanful,
    // then this will have to be fired later.
    rqs->Init();
    rqs->SetDOM(nsnull);
    rqs->SetIsHandler(isHandler);
    rqs->SetProcessDocument(bProcessDocument);

    // Fire the RequestStart event out to all observers
    rv = mETM->FireEvent(rqs, aptEventName_RequestStart);
    LOG_RETURN_IF_ERROR(eFATAL, rv, "RequestStart event returned failure.")

    rqs->GetProcessDocument(&bProcessDocument);

    // At this point, we can process the request. Eg GET/POST/MULTIPART
    gJaxerLog.Log(eDEBUG, "Begin receiving document."); 
    fcgx->BeginDocumentReceive();

    if (PL_strcasecmp(envRequestMethod.BeginReading(), "post") == 0 ||
        PL_strcasecmp(envRequestMethod.BeginReading(), "put") == 0)
    {
        gJaxerLog.Log(eTRACE, "Requesting Post data");
        fcgx->RequestPostData();

        // check to see if this is MULTIPART
        const char *mpfd = "multipart/form-data;";
        const char *axwfu = "application/x-www-form-urlencoded";
        const char *contentType = envContentType.BeginReading();
        gJaxerLog.Log(eTRACE, "contentType=%s", contentType);
        if (PL_strncasecmp(mpfd, contentType, strlen(mpfd)) == 0)
        {
            gJaxerLog.Log(eTRACE, "This is multipart/form-data");
            
            const char*  bd = strstr(contentType, "boundary=");
            if (!bd)
            {
                // we should not be here
                gJaxerLog.Log(eERROR, "Cannot find multipart/form-data boundary: content_type=%s", contentType);
                return NS_ERROR_UNEXPECTED;
            }
            const char* boundary = bd + strlen("boundary=");
            gJaxerLog.Log(eTRACE, "Processing MultiPartData.  Boundary=%s", boundary);

            rv = ProcessMultiPartData(fcgx, boundary, req);
            LOG_RETURN_IF_ERROR(eFATAL, rv, "ProcessMultiPartData failed.")
        } else if (PL_strncasecmp(axwfu, contentType, strlen(axwfu)) == 0)
        {
            gJaxerLog.Log(eTRACE, "This is %s", axwfu);
            nsCAutoString data;
            PRUint32 nRead;
            char buf[4096];
            while ((rv = fcgx->Read(buf, 4096, &nRead)) == NS_OK && nRead > 0)
                data.Append(buf, nRead);
            LOG_RETURN_IF_ERROR(eFATAL, rv, "Read from input stream failed.")

            // Set POST data.
            if (data.Length() > 0) {
                //rv = req->AppendPostData(data.BeginReading());
                rv = req->SetRawPostDataBytes(data.Length(), (PRUint8*)data.BeginReading());
                LOG_RETURN_IF_ERROR(eFATAL, rv, "SetRawPostDataBytes failed.")

                rv = ProcessPostData(data, req);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "ProcessPostData failed.")
            }
        }else
        {
            gJaxerLog.Log(eTRACE, "This is a type we do notknow: %s", contentType);
            // allocate memory and save the stuff as bytes
            PRUint32 nRead;
            char buf[4097];
            PRUint32 nTotal = 0;

            rv = fcgx->Read(buf, 4097, &nRead);
            LOG_RETURN_IF_ERROR(eFATAL, rv, "Read PostData failed.")
            while(nRead>0 && nTotal <4097)
            {
                nTotal += nRead;
                rv = fcgx->Read(buf+nTotal, 4097-nTotal, &nRead);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Read PostData failed.")
            }


            nRead = nTotal;
            nTotal = 0;
            if (nRead> 4096)
            {
                // save data into file
                gJaxerLog.Log(eTRACE, "PostData will be saved in file");
                nsCOMPtr<nsIFile> aFile;
                NS_GetSpecialDirectory(NS_OS_TEMP_DIR, getter_AddRefs(aFile));
                nsCOMPtr<nsILocalFile>  aLocalFile(do_QueryInterface(aFile));
                if (!aLocalFile)
                {
                    gJaxerLog.Log(eFATAL, "Cannot get nsILocalFile interface from nsIFile.");
                    return NS_ERROR_NO_INTERFACE;
                }

                rv = aLocalFile->Append(NS_ConvertASCIItoUTF16("tmp"));
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Append string to file path failed.")
                
                // Create the temporary file. This may adjust the file's basename.
                rv = aLocalFile->CreateUnique(nsIFile::NORMAL_FILE_TYPE, 0644);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Create unique file failed.")

                FILE *fp;

                rv = aLocalFile->OpenANSIFileDesc("w", &fp);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "OpenANSIFileDesc failed.")

                while(nRead>0)
                {
                    if (fwrite(buf, sizeof(char), nRead, fp) != nRead)
                    {
                        fclose(fp);
                        gJaxerLog.Log(eERROR, "fwrite failed.");
                        return NS_ERROR_UNEXPECTED;
                    }
                    nTotal += nRead;
                    rv = fcgx->Read(buf, 4096, &nRead);
                    LOG_RETURN_IF_ERROR(eERROR, rv, "Read PostData failed.")
                }
                fclose(fp);
                rv = req->SetRawPostDataFile(nTotal, aFile);
                LOG_RETURN_IF_ERROR(eERROR, rv, "SetRawPostDataFile failed.")
            }else if (nRead > 0)
            {
                rv = req->SetRawPostDataBytes(nRead, (PRUint8*)buf);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "SetRawPostDataBytes failed.")
            }
        }

        fcgx->ResumeDocumentReceive();
    }

#ifdef _TIMESTATS
    int64 startParseTime;
    if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
        startParseTime = JS_Now();
        int64 ll_delta;
        JSLL_SUB(ll_delta, startParseTime, startTime);
        JSInt32 diff;
        JSLL_L2UI(diff, ll_delta);

        gJaxerLog.Log(eDEBUG, "[STATS] Request fetching time: %ld ms", diff/1000);
    }
#endif /* _TIMESTATS */

    PRBool bUseOriginalDoc = PR_FALSE;
    PRBool bFrameworkFailed = PR_TRUE;
    
    if (bProcessDocument)
    {
        AutoCurrentDocShell ads(mDocShell);

		nsCOMPtr<nsIStorageStream> storageStream;
		rv = NS_NewStorageStream(4096, PR_UINT32_MAX, getter_AddRefs(storageStream));
		LOG_RETURN_IF_ERROR(eFATAL, rv, "Create storage stream failed.");
		
		nsCOMPtr<nsIOutputStream> outputStream;
		rv = storageStream->GetOutputStream(0, getter_AddRefs(outputStream));
		LOG_RETURN_IF_ERROR(eFATAL, rv, "Get output stream from storage failed.");

        // If the incoming request is indeed a callback request
        nsCOMPtr<nsIInputStream> inputStream = fcgx;

        // Since callback will require a page be loaded, the callback flow will be
        // similar to a regular page request (with a few exceptions).
        if (isHandler)
        {
            gJaxerLog.Log(eTRACE, "Jaxer is handler.");
            if (HasDefaultCallbackDoc())
            {
                gJaxerLog.Log(eTRACE, "Has default callback doc");
                rv = mDefaultCallbackFileStream->Init(mDefaultCallbackDocFile, -1, -1, 0);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Init callback file stream failed.")

                inputStream = do_QueryInterface(mDefaultCallbackFileStream, &rv);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "do_QueryInterface for callback file stream failed.")

            }
			else
			{
                gJaxerLog.Log(eTRACE, "Does not have default callback doc");
                // nsCOMPtr<nsIStringInputStream> sis = do_CreateInstance("@mozilla.org/io/string-input-stream;1");
                // NS_ENSURE_TRUE(mDefaultCallbackFileStream, NS_ERROR_FAILURE);

                rv = mDefaultCallbackTextStream->ShareData(htmlDoc, strlen(htmlDoc));
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Init callback text stream failed.")

                inputStream = do_QueryInterface(mDefaultCallbackTextStream, &rv);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "do_QueryInterface for callback text stream failed.")
            }
		}
		
        PRUint32 nRead, nWrite;
        char buf[4096];
        while ((rv = inputStream->Read(buf, 4096, &nRead)) == NS_OK && nRead > 0)
		{
			rv = outputStream->Write(buf, nRead, &nWrite);
			LOG_RETURN_IF_ERROR(eFATAL, rv, "Write to storage output stream failed.");
			NS_ENSURE_TRUE(nRead == nWrite, NS_ERROR_FAILURE);
        }
		LOG_RETURN_IF_ERROR(eFATAL, rv, "Read from FCGX stream failed.");
		rv = outputStream->Close();
		LOG_RETURN_IF_ERROR(eFATAL, rv, "Close output stream failed.");
		
        // Construct original URL
        nsCOMPtr<nsIURL> url = do_CreateInstance(NS_STANDARDURL_CONTRACTID, &rv);
        LOG_RETURN_IF_ERROR(eFATAL, rv, "do_CreateInstance for nsIURL failed.")

        rv = url->SetSpec(NS_LITERAL_CSTRING("http://127.0.0.1/"));
        LOG_RETURN_IF_ERROR(eFATAL, rv, "SetSpec for nsIURL failed.")

        if (isHTTPS) {
            rv = url->SetScheme(NS_LITERAL_CSTRING("https"));
            LOG_RETURN_IF_ERROR(eFATAL, rv, "SetScheme for nsIURL failed.")
        }
        if (envHost.RFindChar(':') != -1) {
            PRInt32 offset = envHost.RFindChar(':');
            nsCAutoString port(envHost);
            envHost.Truncate(offset);
            port.Cut(0, offset);
            rv = url->SetHost(envHost);
            LOG_RETURN_IF_ERROR(eFATAL, rv, "SetHost for nsIURL failed.")
            PRInt32 error = NS_OK;
            PRInt32 nPort = port.ToInteger(&error);
            if (NS_SUCCEEDED(error))
                rv = url->SetPort(nPort);
        } else {
            rv = url->SetHost(envHost);
        }
        NS_ENSURE_SUCCESS(rv, rv);
        rv = url->SetPath(envScriptName);
        NS_ENSURE_SUCCESS(rv, rv);
        rv = url->SetQuery(envQueryString);
        NS_ENSURE_SUCCESS(rv, rv);

		/* XXX:Max commented this
        nsCAutoString sUrl;
        rv = url->GetSpec(sUrl);
        if (rv == NS_OK)
            req->SetUrl(NS_ConvertUTF8toUTF16(sUrl));
		*/
		req->SetDocumentStream(storageStream);
		req->SetDocumentURL(url);

#ifdef MOZ_CRASHREPORTER
        //CrashReporter::AnnotateCrashReport(NS_LITERAL_CSTRING("URL"), sUrl);
#endif

        // Allocate a ParseStart event object
        nsCOMPtr<aptIEventHTMLParseStart> ps = do_CreateInstance(APT_EVENT_HTML_PARSE_START_CONTRACTID);
        if (!ps)
            LOG_RETURN(eFATAL, NS_ERROR_OUT_OF_MEMORY, "do_CreateInstance failed for aptIEventHTMLParseStart")

        // Set some state for the event
        ps->Init();
        ps->SetDOM(nsnull);
        ps->SetIsHandler(isHandler);

        // Fire the HTMLParseStart event out to all observers
        rv = mETM->FireEvent(ps, aptEventName_HTMLParseStart);
        LOG_RETURN_IF_ERROR(eFATAL, rv, "Fire ParseStart event failed.")
        
		rv = storageStream->NewInputStream(0, getter_AddRefs(inputStream));
		LOG_RETURN_IF_ERROR(eFATAL, rv, "Create input stream from storage failed.");

        if (!envDocContentType.Equals("text/html")
            && !envDocContentType.Equals("application/xhtml+xml")
            && !envDocContentType.Equals("application/xml")
            && !envDocContentType.Equals("text/xml")
#ifdef MOZ_SVG      
            && !envDocContentType.Equals("image/svg+xml")
#endif
            )
        {
            envDocContentType.AssignLiteral("text/html");
        }
        nsCOMPtr<nsIJSRuntimeService> rtsvc = do_GetService("@mozilla.org/js/xpc/RuntimeService;1");
        // get the JSRuntime from the runtime svc
        if (!rtsvc) {
          NS_ERROR("failed to get nsJSRuntimeService");
          return NS_ERROR_FAILURE;
        }
        
        JSRuntime *rt = nsnull;
        if (NS_FAILED(rtsvc->GetRuntime(&rt)) || !rt) {
          NS_ERROR("failed to get JSRuntime from nsJSRuntimeService");
          return NS_ERROR_FAILURE;
        }
        if (sMaxBytes == 0) {
            /* 24-26 seems to be the minimum that doesn't lead Jaxer to abnormal exit (win32)
             * may vary on different platforms
             */
            sMaxBytes = nsContentUtils::GetIntPref("Jaxer.JS.memory_limit", 24);
            if (sMaxBytes != 0) {
                sMaxBytes *= 1024L * 1024L;
            } else {
                sMaxBytes = 0xffffffff; /* no limit */
            }
        }
        // Limit JS memory size
        JS_SetGCParameter(rt, JSGC_MAX_BYTES, sMaxBytes);

        // tell the document shell to reset itself in preparation for this new document load
        mDocShell->PrepareForNewContentModel();

#ifdef _TIMESTATS
        int64 startLoadTime;
        if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
            startLoadTime = JS_Now();
        }
#endif /* _TIMESTATS */

        // Load the FCGX into the document
        rv = mDocShell->LoadStream(inputStream,
                url, envDocContentType,
                envCharset, nsnull);
        NS_ENSURE_SUCCESS(rv, rv);

        this->ResetIsDone();

        // Pump events until the document load is completed
        gJaxerLog.Log(eTRACE, "Processing events.");
        nsIThread *thread = NS_GetCurrentThread();
        while (! this->IsDone()) {
            PRBool processedEvent;
            rv = thread->ProcessNextEvent(PR_TRUE, &processedEvent);
        }


        gJaxerLog.Log(eDEBUG, "Document load completed.");

#ifdef _TIMESTATS
        if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
            int64 endTime = JS_Now();
            int64 ll_delta;
            JSLL_SUB(ll_delta, endTime, startLoadTime);
            JSInt32 diff;
            JSLL_L2UI(diff, ll_delta);

            gJaxerLog.Log(eDEBUG, "[STATS] DOM created in: %ld ms", diff/1000);
        }
#endif /* _TIMESTATS */

        // TODO: The following section can be optimized if we do not need a DOM in the ParseComplete event.
        //       In that case, we do not need to create/query interfaces for DOM serialize if the
        //       original doc will be used.
        
        nsCOMPtr<nsIWebNavigation> window(do_GetInterface(mDocShell, &rv));
        LOG_RETURN_IF_ERROR(eFATAL, rv, "Get nsIWebNavigation interface from docshell failed.")
        nsCOMPtr<nsIDOMDocument> dom_doc;
        window->GetDocument(getter_AddRefs(dom_doc));

        // Allocate a HTMLParseComplete event object
        nsCOMPtr<aptIEventHTMLParseComplete> pc = do_CreateInstance(APT_EVENT_HTML_PARSE_COMPLETE_CONTRACTID);
        NS_ENSURE_STATE(pc);
        pc->Init();

        // Set some state for the event
        pc->SetDOM(dom_doc);
        pc->SetIsHandler(isHandler);

        // Fire the HTMLParseComplete event out to all observers
        rv = mETM->FireEvent(pc, aptEventName_HTMLParseComplete);

#ifdef _TIMESTATS
        if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
            int64 endTime = JS_Now();
            int64 ll_delta;
            JSLL_SUB(ll_delta, endTime, startParseTime);
            JSInt32 diff;
            JSLL_L2UI(diff, ll_delta);

            gJaxerLog.Log(eDEBUG, "[STATS] Parse completed in: %ld ms", diff/1000);
        }
#endif /* _TIMESTATS */

        if (NS_FAILED(rv)) {
            gJaxerLog.Log(eERROR, "ParseComplete event returned failure: rv=0x%x", rv);
        }

        resp->GetFrameworkFailed(&bFrameworkFailed);
        gJaxerLog.Log(eDEBUG, "GetFrameworkFailed return failed=%d", bFrameworkFailed);

    } else
    {
        /* Drain document */
        PRUint32 nRead;
        char buf[4096];
        while ((rv = fcgx->Read(buf, 4096, &nRead)) == NS_OK && nRead > 0) {
            /* do nothing */
        }

        bFrameworkFailed = PR_FALSE;
    } /* End of bProcessDocument */

    nsCString strSerializedContent;
    nsCOMPtr<nsIFile> pipeFile;

    if (!bFrameworkFailed) {
        PRBool hasContents = PR_FALSE;
        rv = resp->GetHasContents(&hasContents);
        gJaxerLog.Log(eDEBUG, "GetHasContents return hasContents=%d", hasContents);
        NS_ENSURE_SUCCESS(rv, rv);
        if (!bProcessDocument || (isHandler == PR_TRUE) || hasContents)
        {
            rv = resp->GetPipeFile(getter_AddRefs(pipeFile));
            NS_ENSURE_SUCCESS(rv, rv);
            if (!pipeFile)
            {
                gJaxerLog.Log(eDEBUG, "Calling GetContents to get document content from jaxer.");
                rv = resp->GetContents(strSerializedContent);
                NS_ENSURE_SUCCESS(rv, rv);
            }
        }else {
            rv = resp->GetShouldUseOriginalContent(&bUseOriginalDoc);
            NS_ENSURE_SUCCESS(rv, rv);
            if (!bUseOriginalDoc)
            {
                gJaxerLog.Log(eDEBUG, "Serializing dom to generate content.");

                nsCOMPtr<nsIWebNavigation> window(do_GetInterface(mDocShell, &rv));
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Get nsIWebNavigation interface from docshell failed.")
                nsCOMPtr<nsIDOMDocument> dom_doc;
                window->GetDocument(getter_AddRefs(dom_doc));
                nsCOMPtr<nsIDocument> doc(do_QueryInterface(dom_doc));
                nsAutoString contentType;
                CopyASCIItoUTF16(envDocContentType, contentType);
                if (!doc->IsCaseSensitive()) {
                    // All case-insensitive documents are HTML as far as we're concerned
                    contentType.AssignLiteral("text/html");
                } else {
                    doc->GetContentType(contentType);
                }
#ifdef _TIMESTATS
                int64 startSerializeTime;
                if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
                    startSerializeTime = JS_Now();
                }
#endif /* _TIMESTATS */

                nsCOMPtr<nsIDocumentEncoder> docEncoder = do_CreateInstance(PromiseFlatCString(
                                            nsDependentCString(NS_DOC_ENCODER_CONTRACTID_BASE) +
                                            NS_ConvertUTF16toUTF8(contentType)).get());
                if (!docEncoder && doc->IsCaseSensitive()) {
                    // This could be some type for which we create a synthetic document.  Try again as XML
                    contentType.AssignLiteral("application/xml");
                    docEncoder = do_CreateInstance(NS_DOC_ENCODER_CONTRACTID_BASE "application/xml");
                }
                NS_ENSURE_TRUE(docEncoder, NS_ERROR_FAILURE);
                
                rv = docEncoder->Init(dom_doc, contentType,
                    nsIDocumentEncoder::OutputEncodeHTMLEntities |
                    // Don't do linebreaking that's not present in the source
                    nsIDocumentEncoder::OutputRaw);
                NS_ENSURE_SUCCESS(rv, rv);
                
                nsCAutoString charset(envCharset);
                if (charset.IsEmpty()) {
                    charset = doc->GetDocumentCharacterSet();
                }
                rv = docEncoder->SetCharset(charset);
                NS_ENSURE_SUCCESS(rv, rv);

                nsCOMPtr<nsIStorageStream> storageStream;
                rv = NS_NewStorageStream(4096, PR_UINT32_MAX, getter_AddRefs(storageStream));
                NS_ENSURE_SUCCESS(rv, rv);

                nsCOMPtr<nsIOutputStream> outputStream;
                rv = storageStream->GetOutputStream(0, getter_AddRefs(outputStream));
                NS_ENSURE_SUCCESS(rv, rv);

                rv = docEncoder->EncodeToStream(outputStream);
                NS_ENSURE_SUCCESS(rv, rv);
                outputStream->Close();
                
                PRUint32 dataLength;
                rv = storageStream->GetLength(&dataLength);
                NS_ENSURE_SUCCESS(rv, rv);

                if (!EnsureStringLength(strSerializedContent, dataLength))
                    return NS_ERROR_OUT_OF_MEMORY;

                nsCOMPtr<nsIInputStream> inputStream;
                rv = storageStream->NewInputStream(0, getter_AddRefs(inputStream));
                NS_ENSURE_SUCCESS(rv, rv);

                PRUint32 n;
                rv = inputStream->Read(strSerializedContent.BeginWriting(), dataLength, &n);
                NS_ENSURE_SUCCESS(rv, rv);
                inputStream->Close();

#ifdef _TIMESTATS
                if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
                    int64 endTime = JS_Now();
                    int64 ll_delta;
                    JSLL_SUB(ll_delta, endTime, startSerializeTime);
                    JSInt32 diff;
                    JSLL_L2UI(diff, ll_delta);

                    gJaxerLog.Log(eDEBUG, "[STATS] Response serialized in: %ld ms", diff/1000);
                }
#endif /* _TIMESTATS */
            }
        }
    } else {
        // We will not send the doc back to the browser
        gJaxerLog.Log(eDEBUG, "Framework failed during doc processing.");
        strSerializedContent.AssignASCII(mFrameworkFailureDoc);

        // Add no-cache headers
        resp->AddHeader(NS_LITERAL_STRING("Expires"), NS_LITERAL_STRING("Fri, 23 May 1997 05:00:00 GMT"), PR_TRUE);
        resp->AddHeader(NS_LITERAL_STRING("Cache-Control"), NS_LITERAL_STRING("no-store, no-cache, must-revalidate"), PR_TRUE);
        resp->AddHeader(NS_LITERAL_STRING("Cache-Control"), NS_LITERAL_STRING("post-check=0, pre-check=0"), PR_FALSE);
        resp->AddHeader(NS_LITERAL_STRING("Pragma"), NS_LITERAL_STRING("no-cache"), PR_TRUE);
    }

    // Allocate a RequestComplete event object
    nsCOMPtr<aptIEventRequestComplete> rc = do_CreateInstance(APT_EVENT_REQUEST_COMPLETE_CONTRACTID);

    // Set some state for the event
    rc->Init();
    rc->SetDOM(nsnull);
    rc->SetIsHandler(isHandler);

    // Fire the RequestComplete event out to all observers
    rv = mETM->FireEvent(rc, aptEventName_RequestComplete);
    
    if (NS_FAILED(rv)) {
        gJaxerLog.Log(eERROR, "RequestComplete event returned failure: rv=0x%x", rv);
    }

    // What are the next two lines?
    rc->SetRequest(nsnull);
    rc->SetResponse(nsnull);

    fcgx->GetEndRequest();

    PRInt32 nExtraHeaders = (bUseOriginalDoc || (bFrameworkFailed && mBypassJaxerIfFrameworkFailToLoad)) ? 1 : 2;
    gJaxerLog.Log(eDEBUG, "Extra header count: %d", nExtraHeaders);

    if (nExtraHeaders==2)
    {
        //We will add this, so delete old one
        resp->DeleteHeader(NS_LITERAL_STRING("Content-Length"));
    }

    // Emit response headers.
    resp->GetHeaderCount(&n);
    gJaxerLog.Log(eDEBUG, "Get response header count: %d", n);

    
    fcgx->BeginHTTPResponseHeaders(n + nExtraHeaders);

    if (!bProcessDocument)
    {
        fcgx->SendHeader(NS_LITERAL_CSTRING("Content-Type"), NS_LITERAL_CSTRING(""));
    }

    // Send the status as a response header.
    PRInt32 statusCode;
    nsAutoString statusPhrase;
    if (bFrameworkFailed){
        if (!mBypassJaxerIfFrameworkFailToLoad){
            gJaxerLog.Log(eDEBUG, "Framework failed, but not bypassing Jaxer.");
            statusCode = 500;
            statusPhrase.AssignASCII("Internal Server Framework Error");
        }
    }else{
        resp->GetStatusCode(&statusCode);
        resp->GetStatusPhrase(statusPhrase);
    }

    char buf[20];
    sprintf(buf, "%d ", statusCode);
    NS_ConvertUTF16toUTF8 cStatusPhrase(statusPhrase);
    cStatusPhrase.Insert(buf, 0);
    fcgx->SendHeader(NS_LITERAL_CSTRING("status"), cStatusPhrase);

    // Emit the response headers.
    for (PRInt32 i = 0; i < n; i++) {
        nsAutoString name, value;
        resp->GetHeaderName(i,name);
        resp->GetValueByOrd(i, value);
        fcgx->SendHeader(NS_ConvertUTF16toUTF8(name), NS_ConvertUTF16toUTF8(value));
    }

    if (bUseOriginalDoc || (bFrameworkFailed && mBypassJaxerIfFrameworkFailToLoad)){
        gJaxerLog.Log(eDEBUG, "Using original doc: use_flag=%d frame_failed=%d.", bUseOriginalDoc, bFrameworkFailed);
        fcgx->BeginResponse();
        fcgx->SendOriginalDoc();
        gJaxerLog.Log(eDEBUG, "Original Document sent to web server");
    }else{
        // fcgx->SendHeader(NS_LITERAL_CSTRING("Content-Type"), NS_LITERAL_CSTRING("text/html"));
        PRUint32 contentLength;
        if (pipeFile) {
            PRInt64 fileSize;
            rv = pipeFile->GetFileSize(&fileSize);
            NS_ENSURE_SUCCESS(rv, rv);
            if (fileSize > 16*1024*1024) {
                gJaxerLog.Log(eERROR, "File size exceeds limit: size=%ld", fileSize);
            }
            contentLength = (PRUint32)fileSize;
        } else {
            contentLength = strSerializedContent.Length();
        }
        char buf[20];
        sprintf(buf, "%d", contentLength);
        fcgx->SendHeader(NS_LITERAL_CSTRING("Content-Length"), nsDependentCString(buf));
        
        fcgx->BeginResponse();
        if (pipeFile) {
            gJaxerLog.Log(eDEBUG, "Using Jaxer provided pipe file: length=%d", contentLength);
            nsCOMPtr<nsIInputStream> fileIn;
            rv = NS_NewLocalFileInputStream(getter_AddRefs(fileIn), pipeFile);
            if (NS_FAILED(rv)) {
                gJaxerLog.Log(eERROR, "Open file input stream failed: rv=0x%x", rv);
            }
            PRUint32 count;
            char buf[4096];
            while (PR_TRUE) {
                rv = fileIn->Read(buf, sizeof(buf), &count);
                if (NS_FAILED(rv)) {
                    gJaxerLog.Log(eERROR, "Reading from file failed: rv=0x%x", rv);
                    break;
                }
                if (count == 0) break;
                fcgx->SendResponseFragment(buf, count);
            }
            fileIn->Close();
        } else {
            gJaxerLog.Log(eDEBUG, "Using Jaxer generated doc: length=%d", contentLength);
            fcgx->SendResponseFragment(strSerializedContent.BeginReading(), contentLength);
        }
        gJaxerLog.Log(eDEBUG, "New Document sent to web server");
    }

    fcgx->EndResponse();
    gJaxerLog.Log(eDEBUG, "End Response sent to web server");

#ifdef _TIMESTATS
    if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
        int64 endTime = JS_Now();
        int64 ll_delta;
        JSLL_SUB(ll_delta, endTime, startTime);
        JSInt32 diff;
        JSLL_L2UI(diff, ll_delta);

        gJaxerLog.Log(eDEBUG, "[STATS] Request processing full time: %ld ms", diff/1000);
        startTime = endTime;
    }
#endif /* _TIMESTATS */

    // Remove any uploaded files that are still in the tmp dir.  Ideally,
    // this should be done in the dtor, but some global object is holding the
    // reference for req.  So we do it here.
    req->RemoveUploadedTmpFiles();
    gJaxerLog.Log(eDEBUG, "Removed tmp files");


    // Load the empty page to force memory cleanup
    mDocShell->PrepareForNewContentModel();
    rv = mDocShell->LoadURI(sAboutBlankURI, nsnull, nsIWebNavigation::LOAD_FLAGS_NONE, PR_TRUE);
    NS_ENSURE_SUCCESS(rv, rv);

    gJaxerLog.Log(eDEBUG, "Exiting FetchFCGX.");
#ifdef _TIMESTATS
    if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
        int64 endTime = JS_Now();
        int64 ll_delta;
        JSLL_SUB(ll_delta, endTime, startTime);
        JSInt32 diff;
        JSLL_L2UI(diff, ll_delta);

        gJaxerLog.Log(eDEBUG, "[STATS] Additional time after response sent: %ld ms", diff/1000);
    }
#endif /* _TIMESTATS */
    return rv;
}


nsresult
DocumentFetcher::ProcessHTTPRequest(PRInt32 fd, eMSG_TYPE type)
{
	// Type is a flag to distinguish different request types.
	// Currently, it is
	//  9 (eNEW_HTTP_REQUEST_MSG) -- regular
	// 10 (eNEW_ADMIN_REQUEST_MSG) -- admin HTTP request

    nsresult rv;
    gJaxerLog.Log(eTRACE, "Entered ProcessRequest");

#ifdef _TIMESTATS
    int64 startTime;
    if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
        startTime = JS_Now();
    }
#endif /* _TIMESTATS */

    HTTPStream fcgx(fd, type);
    
    // Reset the completion flag
    mDone = PR_FALSE;

#ifdef MOZ_MEMORY_DEBUG
#ifndef XP_WIN
    malloc_print_stats();
#endif
#endif

    rv = mCookieManager->RemoveAll();
    LOG_RETURN_IF_ERROR(eFATAL, rv, "Failed to remove cookies.")
    
    SSL_ClearSessionCache();

    // Create HTTPRequest object.
    nsCOMPtr<aptIHTTPRequest> req;
	// Process request, save all state, etc
    rv = fcgx.RecvAndParseRequest(getter_AddRefs(req));


	if (rv != NS_OK || fcgx.GetProcessFailed() == PR_TRUE)
	{
		gJaxerLog.Log(eDEBUG, "Parsing request failed");
		fcgx.SendErrorDoc();
		return NS_OK;
	}
    
	if (!fcgx.ShouldJaxerProcessRequest())
	{
		rv = fcgx.BeginResponse(200, "OK");
		LOG_RETURN_IF_ERROR(eFATAL, rv, "BeginResponse failed.")
		rv = fcgx.SendOriginalDoc();
		LOG_RETURN_IF_ERROR(eFATAL, rv, "SendOriginalDoc failed.")
		rv = fcgx.EndResponse();
		LOG_RETURN_IF_ERROR(eFATAL, rv, "EndResponse failed.")
		gJaxerLog.Log(eDEBUG, "Bypassing Jaxer: %s", fcgx.GetRequestHeader(eREQ_PATH));
		return NS_OK;
	}

    // Create HTTPResponse object.
    nsCOMPtr<aptIHTTPResponse> resp;
    resp = do_CreateInstance(APT_HTTP_RESPONSE_CONTRACTID);
    if (!resp)
        LOG_RETURN(eFATAL, NS_ERROR_OUT_OF_MEMORY, "do_CreateInstance failed for aptIHTTPResponse")

 
	PRBool isHandler = fcgx.IsCallbackRequest();
    req->SetIsHandler(isHandler);
    gJaxerLog.Log(eDEBUG, "Jaxer is handler for request? (%d)", isHandler);

    PRBool bProcessDocument = PR_TRUE; //(PL_strcasecmp(envRequestMethod.BeginReading(), "options") != 0);

    // Allocate a RequestStart event object
    nsCOMPtr<aptIEventRequestStart> rqs = do_CreateInstance(APT_EVENT_REQUEST_START_CONTRACTID);
    if (!rqs)
        LOG_RETURN(eFATAL, NS_ERROR_OUT_OF_MEMORY, "do_CreateInstance failed for aptIEventRequestStart")

    // Make request/response available to events.  Note: this is setting global variables
    // inside aptEventBaseData and will be available to all future events until reset.
    rqs->SetRequest(req);
    rqs->SetResponse(resp);

    // Set some state for the event.  We got nothing now.  If we need something meanful,
    // then this will have to be fired later.
    rqs->Init();
    rqs->SetDOM(nsnull);
    rqs->SetIsHandler(isHandler);
    rqs->SetProcessDocument(bProcessDocument);

    // Fire the RequestStart event out to all observers
    rv = mETM->FireEvent(rqs, aptEventName_RequestStart);
    LOG_RETURN_IF_ERROR(eFATAL, rv, "RequestStart event returned failure.")

    rqs->GetProcessDocument(&bProcessDocument);

    
#ifdef _TIMESTATS
    int64 startParseTime;
    if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
        startParseTime = JS_Now();
        int64 ll_delta;
        JSLL_SUB(ll_delta, startParseTime, startTime);
        JSInt32 diff;
        JSLL_L2UI(diff, ll_delta);

        gJaxerLog.Log(eDEBUG, "[STATS] Request fetching time: %ld ms", diff/1000);
    }
#endif /* _TIMESTATS */

    PRBool bUseOriginalDoc = PR_FALSE;
    PRBool bFrameworkFailed = PR_TRUE;
    
	nsCAutoString envHost (fcgx.GetRequestHeader(eREQ_HOST));
	nsCAutoString envScriptName(fcgx.GetRequestHeader(eREQ_PATH));
	nsCAutoString envQueryString(fcgx.GetRequestHeader(eREQ_QUERY_STRING));
	nsCAutoString envDocContentType(fcgx.GetRequestHeader(eREQ_CONTENT_TYPE));
	nsCAutoString envCharset;
    if (bProcessDocument)
    {
        AutoCurrentDocShell ads(mDocShell);

		nsCOMPtr<nsIStorageStream> storageStream;
		nsCOMPtr<nsIInputStream> inputStream;
		
        //rv = fcgx.GetDocumentStream(getter_AddRefs(inputStream));

        // Since callback will require a page be loaded, the callback flow will be
        // similar to a regular page request (with a few exceptions).
        if (isHandler)
        {
			gJaxerLog.Log(eTRACE, "Jaxer is handler.");
            
			rv = NS_NewStorageStream(4096, PR_UINT32_MAX, getter_AddRefs(storageStream));
			LOG_RETURN_IF_ERROR(eFATAL, rv, "Create storage stream failed.");
		
			// If the incoming request is indeed a callback request
			
			nsCOMPtr<nsIOutputStream> outputStream;
			rv = storageStream->GetOutputStream(0, getter_AddRefs(outputStream));
			LOG_RETURN_IF_ERROR(eFATAL, rv, "Get output stream from storage failed.");

            if (HasDefaultCallbackDoc())
            {
                gJaxerLog.Log(eTRACE, "Has default callback doc");
                rv = mDefaultCallbackFileStream->Init(mDefaultCallbackDocFile, -1, -1, 0);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Init callback file stream failed.")

                inputStream = do_QueryInterface(mDefaultCallbackFileStream, &rv);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "do_QueryInterface for callback file stream failed.")

            }
			else
			{
                gJaxerLog.Log(eTRACE, "Does not have default callback doc");
                // nsCOMPtr<nsIStringInputStream> sis = do_CreateInstance("@mozilla.org/io/string-input-stream;1");
                // NS_ENSURE_TRUE(mDefaultCallbackFileStream, NS_ERROR_FAILURE);

                rv = mDefaultCallbackTextStream->ShareData(htmlDoc, strlen(htmlDoc));
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Init callback text stream failed.")

                inputStream = do_QueryInterface(mDefaultCallbackTextStream, &rv);
                LOG_RETURN_IF_ERROR(eFATAL, rv, "do_QueryInterface for callback text stream failed.")
            }
			PRUint32 nRead, nWrite;
			char buf[4096];
			while ((rv = inputStream->Read(buf, 4096, &nRead)) == NS_OK && nRead > 0)
			{
				rv = outputStream->Write(buf, nRead, &nWrite);
				LOG_RETURN_IF_ERROR(eFATAL, rv, "Write to storage output stream failed.");
				NS_ENSURE_TRUE(nRead == nWrite, NS_ERROR_FAILURE);
			}
			LOG_RETURN_IF_ERROR(eFATAL, rv, "Read from FCGX stream failed.");
			rv = outputStream->Close();
			LOG_RETURN_IF_ERROR(eFATAL, rv, "Close output stream failed.");
		}else
		{
			rv = fcgx.GetDocumentStream(getter_AddRefs(storageStream));
			LOG_RETURN_IF_ERROR(eFATAL, rv, "GetDocumentStream failed.");
		}
		
 
        // Construct original URL
        nsCOMPtr<nsIURL> url = do_CreateInstance(NS_STANDARDURL_CONTRACTID, &rv);
        LOG_RETURN_IF_ERROR(eFATAL, rv, "do_CreateInstance for nsIURL failed.")

        rv = url->SetSpec(NS_LITERAL_CSTRING("http://127.0.0.1/"));
        LOG_RETURN_IF_ERROR(eFATAL, rv, "SetSpec for nsIURL failed.")

		PRBool isHTTPS = PR_FALSE;
        if (isHTTPS) {
            rv = url->SetScheme(NS_LITERAL_CSTRING("https"));
            LOG_RETURN_IF_ERROR(eFATAL, rv, "SetScheme for nsIURL failed.")
        }

		
        if (envHost.RFindChar(':') != -1) {
            PRInt32 offset = envHost.RFindChar(':');
            nsCAutoString port(envHost);
            envHost.Truncate(offset);
            port.Cut(0, offset);
            rv = url->SetHost(envHost);
            LOG_RETURN_IF_ERROR(eFATAL, rv, "SetHost for nsIURL failed.")
            PRInt32 error = NS_OK;
            PRInt32 nPort = port.ToInteger(&error);
            if (NS_SUCCEEDED(error))
                rv = url->SetPort(nPort);
        } else {
            rv = url->SetHost(envHost);
        }
        NS_ENSURE_SUCCESS(rv, rv);
        rv = url->SetPath(envScriptName);
        NS_ENSURE_SUCCESS(rv, rv);
        rv = url->SetQuery(envQueryString);
        NS_ENSURE_SUCCESS(rv, rv);

		req->SetDocumentStream(storageStream);
		req->SetDocumentURL(url);

#ifdef MOZ_CRASHREPORTER
        //CrashReporter::AnnotateCrashReport(NS_LITERAL_CSTRING("URL"), sUrl);
#endif

        // Allocate a ParseStart event object
        nsCOMPtr<aptIEventHTMLParseStart> ps = do_CreateInstance(APT_EVENT_HTML_PARSE_START_CONTRACTID);
        if (!ps)
            LOG_RETURN(eFATAL, NS_ERROR_OUT_OF_MEMORY, "do_CreateInstance failed for aptIEventHTMLParseStart")

        // Set some state for the event
        ps->Init();
        ps->SetDOM(nsnull);
        ps->SetIsHandler(isHandler);

        // Fire the HTMLParseStart event out to all observers
        rv = mETM->FireEvent(ps, aptEventName_HTMLParseStart);
        LOG_RETURN_IF_ERROR(eFATAL, rv, "Fire ParseStart event failed.")
        
		rv = storageStream->NewInputStream(0, getter_AddRefs(inputStream));
		LOG_RETURN_IF_ERROR(eFATAL, rv, "Create input stream from storage failed.");

        if (!envDocContentType.Equals("text/html")
            && !envDocContentType.Equals("application/xhtml+xml")
            && !envDocContentType.Equals("application/xml")
            && !envDocContentType.Equals("text/xml")
#ifdef MOZ_SVG      
            && !envDocContentType.Equals("image/svg+xml")
#endif
            )
        {
            envDocContentType.AssignLiteral("text/html");
        }
        nsCOMPtr<nsIJSRuntimeService> rtsvc = do_GetService("@mozilla.org/js/xpc/RuntimeService;1");
        // get the JSRuntime from the runtime svc
        if (!rtsvc) {
          NS_ERROR("failed to get nsJSRuntimeService");
          return NS_ERROR_FAILURE;
        }
        
        JSRuntime *rt = nsnull;
        if (NS_FAILED(rtsvc->GetRuntime(&rt)) || !rt) {
          NS_ERROR("failed to get JSRuntime from nsJSRuntimeService");
          return NS_ERROR_FAILURE;
        }
        if (sMaxBytes == 0) {
            /* 24-26 seems to be the minimum that doesn't lead Jaxer to abnormal exit (win32)
             * may vary on different platforms
             */
            sMaxBytes = nsContentUtils::GetIntPref("Jaxer.JS.memory_limit", 24);
            if (sMaxBytes != 0) {
                sMaxBytes *= 1024L * 1024L;
            } else {
                sMaxBytes = 0xffffffff; /* no limit */
            }
        }
        // Limit JS memory size
        JS_SetGCParameter(rt, JSGC_MAX_BYTES, sMaxBytes);

        // tell the document shell to reset itself in preparation for this new document load
        mDocShell->PrepareForNewContentModel();

#ifdef _TIMESTATS
        int64 startLoadTime;
        if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
            startLoadTime = JS_Now();
        }
#endif /* _TIMESTATS */

        // Load the FCGX into the document
        rv = mDocShell->LoadStream(inputStream,
                url, envDocContentType,
                envCharset, nsnull);
        NS_ENSURE_SUCCESS(rv, rv);

        this->ResetIsDone();

        // Pump events until the document load is completed
        gJaxerLog.Log(eTRACE, "Processing events.");
        nsIThread *thread = NS_GetCurrentThread();
        while (! this->IsDone()) {
            PRBool processedEvent;
            rv = thread->ProcessNextEvent(PR_TRUE, &processedEvent);
        }


        gJaxerLog.Log(eDEBUG, "Document load completed.");

#ifdef _TIMESTATS
        if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
            int64 endTime = JS_Now();
            int64 ll_delta;
            JSLL_SUB(ll_delta, endTime, startLoadTime);
            JSInt32 diff;
            JSLL_L2UI(diff, ll_delta);

            gJaxerLog.Log(eDEBUG, "[STATS] DOM created in: %ld ms", diff/1000);
        }
#endif /* _TIMESTATS */

        // TODO: The following section can be optimized if we do not need a DOM in the ParseComplete event.
        //       In that case, we do not need to create/query interfaces for DOM serialize if the
        //       original doc will be used.
        
        nsCOMPtr<nsIWebNavigation> window(do_GetInterface(mDocShell, &rv));
        LOG_RETURN_IF_ERROR(eFATAL, rv, "Get nsIWebNavigation interface from docshell failed.")
        nsCOMPtr<nsIDOMDocument> dom_doc;
        window->GetDocument(getter_AddRefs(dom_doc));

        // Allocate a HTMLParseComplete event object
        nsCOMPtr<aptIEventHTMLParseComplete> pc = do_CreateInstance(APT_EVENT_HTML_PARSE_COMPLETE_CONTRACTID);
        NS_ENSURE_STATE(pc);
        pc->Init();

        // Set some state for the event
        pc->SetDOM(dom_doc);
        pc->SetIsHandler(isHandler);

        // Fire the HTMLParseComplete event out to all observers
        rv = mETM->FireEvent(pc, aptEventName_HTMLParseComplete);

#ifdef _TIMESTATS
        if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
            int64 endTime = JS_Now();
            int64 ll_delta;
            JSLL_SUB(ll_delta, endTime, startParseTime);
            JSInt32 diff;
            JSLL_L2UI(diff, ll_delta);

            gJaxerLog.Log(eDEBUG, "[STATS] Parse completed in: %ld ms", diff/1000);
        }
#endif /* _TIMESTATS */

        if (NS_FAILED(rv)) {
            gJaxerLog.Log(eERROR, "ParseComplete event returned failure: rv=0x%x", rv);
        }

        resp->GetFrameworkFailed(&bFrameworkFailed);
        gJaxerLog.Log(eDEBUG, "GetFrameworkFailed return failed=%d", bFrameworkFailed);

    } else
    {
#if 0 //JZ
        /* Drain document */
        PRUint32 nRead;
        char buf[4096];

        while ((rv = fcgx->Read(buf, 4096, &nRead)) == NS_OK && nRead > 0) {
            /* do nothing */
        }
#endif

        bFrameworkFailed = PR_FALSE;
    } /* End of bProcessDocument */

    nsCString strSerializedContent;
    nsCOMPtr<nsIFile> pipeFile;

    if (!bFrameworkFailed) {
        PRBool hasContents = PR_FALSE;
        rv = resp->GetHasContents(&hasContents);
        gJaxerLog.Log(eDEBUG, "GetHasContents return hasContents=%d", hasContents);
        NS_ENSURE_SUCCESS(rv, rv);
        if (!bProcessDocument || (isHandler == PR_TRUE) || hasContents)
        {
            rv = resp->GetPipeFile(getter_AddRefs(pipeFile));
            NS_ENSURE_SUCCESS(rv, rv);
            if (!pipeFile)
            {
                gJaxerLog.Log(eDEBUG, "Calling GetContents to get document content from jaxer.");
                rv = resp->GetContents(strSerializedContent);
                NS_ENSURE_SUCCESS(rv, rv);
            }
        }else {
            rv = resp->GetShouldUseOriginalContent(&bUseOriginalDoc);
            NS_ENSURE_SUCCESS(rv, rv);
            if (!bUseOriginalDoc)
            {
                gJaxerLog.Log(eDEBUG, "Serializing dom to generate content.");

                nsCOMPtr<nsIWebNavigation> window(do_GetInterface(mDocShell, &rv));
                LOG_RETURN_IF_ERROR(eFATAL, rv, "Get nsIWebNavigation interface from docshell failed.")
                nsCOMPtr<nsIDOMDocument> dom_doc;
                window->GetDocument(getter_AddRefs(dom_doc));
                nsCOMPtr<nsIDocument> doc(do_QueryInterface(dom_doc));
                nsAutoString contentType;
                CopyASCIItoUTF16(envDocContentType, contentType);
                if (!doc->IsCaseSensitive()) {
                    // All case-insensitive documents are HTML as far as we're concerned
                    contentType.AssignLiteral("text/html");
                } else {
                    doc->GetContentType(contentType);
                }
#ifdef _TIMESTATS
                int64 startSerializeTime;
                if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
                    startSerializeTime = JS_Now();
                }
#endif /* _TIMESTATS */

                nsCOMPtr<nsIDocumentEncoder> docEncoder = do_CreateInstance(PromiseFlatCString(
                                            nsDependentCString(NS_DOC_ENCODER_CONTRACTID_BASE) +
                                            NS_ConvertUTF16toUTF8(contentType)).get());
                if (!docEncoder && doc->IsCaseSensitive()) {
                    // This could be some type for which we create a synthetic document.  Try again as XML
                    contentType.AssignLiteral("application/xml");
                    docEncoder = do_CreateInstance(NS_DOC_ENCODER_CONTRACTID_BASE "application/xml");
                }
                NS_ENSURE_TRUE(docEncoder, NS_ERROR_FAILURE);
                
                rv = docEncoder->Init(dom_doc, contentType,
                    nsIDocumentEncoder::OutputEncodeHTMLEntities |
                    // Don't do linebreaking that's not present in the source
                    nsIDocumentEncoder::OutputRaw);
                NS_ENSURE_SUCCESS(rv, rv);
                
                nsCAutoString charset(envCharset);
                if (charset.IsEmpty()) {
                    charset = doc->GetDocumentCharacterSet();
                }
                rv = docEncoder->SetCharset(charset);
                NS_ENSURE_SUCCESS(rv, rv);

                nsCOMPtr<nsIStorageStream> storageStream;
                rv = NS_NewStorageStream(4096, PR_UINT32_MAX, getter_AddRefs(storageStream));
                NS_ENSURE_SUCCESS(rv, rv);

                nsCOMPtr<nsIOutputStream> outputStream;
                rv = storageStream->GetOutputStream(0, getter_AddRefs(outputStream));
                NS_ENSURE_SUCCESS(rv, rv);

                rv = docEncoder->EncodeToStream(outputStream);
                NS_ENSURE_SUCCESS(rv, rv);
                outputStream->Close();
                
                PRUint32 dataLength;
                rv = storageStream->GetLength(&dataLength);
                NS_ENSURE_SUCCESS(rv, rv);

                if (!EnsureStringLength(strSerializedContent, dataLength))
                    return NS_ERROR_OUT_OF_MEMORY;

                nsCOMPtr<nsIInputStream> inputStream;
                rv = storageStream->NewInputStream(0, getter_AddRefs(inputStream));
                NS_ENSURE_SUCCESS(rv, rv);

                PRUint32 n;
                rv = inputStream->Read(strSerializedContent.BeginWriting(), dataLength, &n);
                NS_ENSURE_SUCCESS(rv, rv);
                inputStream->Close();

#ifdef _TIMESTATS
                if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
                    int64 endTime = JS_Now();
                    int64 ll_delta;
                    JSLL_SUB(ll_delta, endTime, startSerializeTime);
                    JSInt32 diff;
                    JSLL_L2UI(diff, ll_delta);

                    gJaxerLog.Log(eDEBUG, "[STATS] Response serialized in: %ld ms", diff/1000);
                }
#endif /* _TIMESTATS */
            }
        }
    } else {
        // We will not send the doc back to the browser
        gJaxerLog.Log(eDEBUG, "Framework failed during doc processing.");
        strSerializedContent.AssignASCII(mFrameworkFailureDoc);

        // Add no-cache headers
        resp->AddHeader(NS_LITERAL_STRING("Expires"), NS_LITERAL_STRING("Fri, 23 May 1997 05:00:00 GMT"), PR_TRUE);
        resp->AddHeader(NS_LITERAL_STRING("Cache-Control"), NS_LITERAL_STRING("no-store, no-cache, must-revalidate"), PR_TRUE);
        resp->AddHeader(NS_LITERAL_STRING("Cache-Control"), NS_LITERAL_STRING("post-check=0, pre-check=0"), PR_FALSE);
        resp->AddHeader(NS_LITERAL_STRING("Pragma"), NS_LITERAL_STRING("no-cache"), PR_TRUE);
    }

    // Allocate a RequestComplete event object
    nsCOMPtr<aptIEventRequestComplete> rc = do_CreateInstance(APT_EVENT_REQUEST_COMPLETE_CONTRACTID);

    // Set some state for the event
    rc->Init();
    rc->SetDOM(nsnull);
    rc->SetIsHandler(isHandler);

    // Fire the RequestComplete event out to all observers
    rv = mETM->FireEvent(rc, aptEventName_RequestComplete);
    
    if (NS_FAILED(rv)) {
        gJaxerLog.Log(eERROR, "RequestComplete event returned failure: rv=0x%x", rv);
    }

    // What are the next two lines?
    rc->SetRequest(nsnull);
    rc->SetResponse(nsnull);

    //fcgx->GetEndRequest();

    PRInt32 nExtraHeaders = (bUseOriginalDoc || (bFrameworkFailed && mBypassJaxerIfFrameworkFailToLoad)) ? 1 : 2;
    gJaxerLog.Log(eDEBUG, "Extra header count: %d", nExtraHeaders);

    if (nExtraHeaders==2)
    {
        //We will add this, so delete old one
        resp->DeleteHeader(NS_LITERAL_STRING("Content-Length"));
    }

    // Emit response headers.
	PRInt32 n; //JZ
    resp->GetHeaderCount(&n);
    gJaxerLog.Log(eDEBUG, "Get response header count: %d", n);

    //fcgx->BeginHTTPResponseHeaders(n + nExtraHeaders);
	// Send the status as a response header.
    PRInt32 statusCode;
    nsAutoString statusPhrase;
    if (bFrameworkFailed){
        if (!mBypassJaxerIfFrameworkFailToLoad){
            gJaxerLog.Log(eDEBUG, "Framework failed, but not bypassing Jaxer.");
            statusCode = 500;
            statusPhrase.AssignASCII("Internal Server Framework Error");
        }
    }else{
        resp->GetStatusCode(&statusCode);
        resp->GetStatusPhrase(statusPhrase);
    }

    //char buf[20];
    //sprintf(buf, "%d ", statusCode);
    //NS_ConvertUTF16toUTF8 cStatusPhrase(statusPhrase);
    //cStatusPhrase.Insert(buf, 0);
	rv = fcgx.BeginResponse(statusCode, NS_ConvertUTF16toUTF8(statusPhrase).get());

#if 0
	if (!bProcessDocument)
    {
        fcgx->SendHeader(NS_LITERAL_CSTRING("Content-Type"), NS_LITERAL_CSTRING(""));
    }
#endif
    
    // Emit the response headers.
    for (PRInt32 i = 0; i < n; i++) {
        nsAutoString name, value;
        resp->GetHeaderName(i,name);
        resp->GetValueByOrd(i, value);
        fcgx.SetResponseHeader(NS_ConvertUTF16toUTF8(name).BeginReading(), NS_ConvertUTF16toUTF8(value).BeginReading());
    }

    if (bUseOriginalDoc || (bFrameworkFailed && mBypassJaxerIfFrameworkFailToLoad)){
        gJaxerLog.Log(eDEBUG, "Using original doc: use_flag=%d frame_failed=%d.", bUseOriginalDoc, bFrameworkFailed);
        //fcgx.BeginResponse();
        fcgx.SendOriginalDoc();
        gJaxerLog.Log(eDEBUG, "Original Document sent to web server");
    }else{
        // fcgx->SendHeader(NS_LITERAL_CSTRING("Content-Type"), NS_LITERAL_CSTRING("text/html"));
        PRUint32 contentLength;
        if (pipeFile) {
            PRInt64 fileSize;
            rv = pipeFile->GetFileSize(&fileSize);
            NS_ENSURE_SUCCESS(rv, rv);
            if (fileSize > 16*1024*1024) {
                gJaxerLog.Log(eERROR, "File size exceeds limit: size=%ld", fileSize);
            }
            contentLength = (PRUint32)fileSize;
        } else {
            contentLength = strSerializedContent.Length();
        }
        char buf[20];
        sprintf(buf, "%d", contentLength);
        fcgx.SetResponseHeader("Content-Length", buf);
        
        fcgx.BeginResponseBody();
        if (pipeFile) {
            gJaxerLog.Log(eDEBUG, "Using Jaxer provided pipe file: length=%d", contentLength);
            nsCOMPtr<nsIInputStream> fileIn;
            rv = NS_NewLocalFileInputStream(getter_AddRefs(fileIn), pipeFile);
            if (NS_FAILED(rv)) {
                gJaxerLog.Log(eERROR, "Open file input stream failed: rv=0x%x", rv);
            }
            PRUint32 count;
            char buf[4096];
            while (PR_TRUE) {
                rv = fileIn->Read(buf, sizeof(buf), &count);
                if (NS_FAILED(rv)) {
                    gJaxerLog.Log(eERROR, "Reading from file failed: rv=0x%x", rv);
                    break;
                }
                if (count == 0) break;
                fcgx.SendResponseFragment(buf, count);
            }
            fileIn->Close();
        } else {
            gJaxerLog.Log(eDEBUG, "Using Jaxer generated doc: length=%d", contentLength);
            fcgx.SendResponseFragment(strSerializedContent.BeginReading(), contentLength);
        }
        gJaxerLog.Log(eDEBUG, "New Document sent to web server");
    }

    fcgx.EndResponse();

    gJaxerLog.Log(eDEBUG, "End Response sent to web server");

#ifdef _TIMESTATS
    if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
        int64 endTime = JS_Now();
        int64 ll_delta;
        JSLL_SUB(ll_delta, endTime, startTime);
        JSInt32 diff;
        JSLL_L2UI(diff, ll_delta);

        gJaxerLog.Log(eDEBUG, "[STATS] Request processing full time: %ld ms", diff/1000);
        startTime = endTime;
    }
#endif /* _TIMESTATS */

    // Remove any uploaded files that are still in the tmp dir.  Ideally,
    // this should be done in the dtor, but some global object is holding the
    // reference for req.  So we do it here.
    req->RemoveUploadedTmpFiles();
    gJaxerLog.Log(eDEBUG, "Removed tmp files");


    // Load the empty page to force memory cleanup
    mDocShell->PrepareForNewContentModel();
    rv = mDocShell->LoadURI(sAboutBlankURI, nsnull, nsIWebNavigation::LOAD_FLAGS_NONE, PR_TRUE);
    NS_ENSURE_SUCCESS(rv, rv);

    gJaxerLog.Log(eDEBUG, "Exiting FetchFCGX.");
#ifdef _TIMESTATS
    if (gJaxerLog.GetCoreLogLevel() == eDEBUG) {
        int64 endTime = JS_Now();
        int64 ll_delta;
        JSLL_SUB(ll_delta, endTime, startTime);
        JSInt32 diff;
        JSLL_L2UI(diff, ll_delta);

        gJaxerLog.Log(eDEBUG, "[STATS] Additional time after response sent: %ld ms", diff/1000);
    }
#endif /* _TIMESTATS */
    return rv;
}


aptDocumentFetcherServiceImpl::aptDocumentFetcherServiceImpl()
{
  mDocumentFetcher = nsnull;
  //Init();
}

aptDocumentFetcherServiceImpl::~aptDocumentFetcherServiceImpl()
{
    NS_IF_RELEASE(mDocumentFetcher);
}

nsresult
aptDocumentFetcherServiceImpl::Init()
{
    if (mDocumentFetcher == nsnull)
    {
      mDocumentFetcher = new DocumentFetcher();
      if (mDocumentFetcher == nsnull)
          return NS_ERROR_OUT_OF_MEMORY;

      NS_ADDREF(mDocumentFetcher);

      return mDocumentFetcher->Init();
    }
    return NS_OK;
}

/* nsISupports interface implementation... */
NS_IMPL_ISUPPORTS1(aptDocumentFetcherServiceImpl, aptIDocumentFetcherService)

/* aptIDocumentFetcherService interface implementation... */

/* [noscript] void processRequest (in long fd); */
NS_IMETHODIMP aptDocumentFetcherServiceImpl::ProcessRequest(PRInt32 fd)
{
    nsresult rv = mDocumentFetcher->ProcessRequest(fd);
    return rv;
}

/* [noscript] void processRequest (in long fd, in long type); */
NS_IMETHODIMP aptDocumentFetcherServiceImpl::ProcessHTTPRequest(PRInt32 fd, PRInt32 type)
{
    nsresult rv = mDocumentFetcher->ProcessHTTPRequest(fd, (eMSG_TYPE)type);
    return rv;
}

/* [noscript] readonly attribute nsIDocShell docShell; */
NS_IMETHODIMP 
aptDocumentFetcherServiceImpl::GetDocShell(nsIDocShell * *aDocShell)
{
    NS_ENSURE_ARG_POINTER(aDocShell);
    *aDocShell = gCurrentDocShell;
    NS_IF_ADDREF(*aDocShell);
    return NS_OK;
}

/* [noscript] void UpdatePrefSettings(in nsIPrefBranch aPrefBranch, in string pref); */
NS_IMETHODIMP 
aptDocumentFetcherServiceImpl::UpdatePrefSettings(nsIPrefBranch *aPrefBranch, const char* pref)
{
    mDocumentFetcher->UpdatePrefSettings(aPrefBranch, pref);
    return NS_OK;
}

/* void exit (); */
NS_IMETHODIMP
aptDocumentFetcherServiceImpl::Exit()
{
	extern eJAXER_STATE gJaxerState;
	gJaxerState = eEXITING;
    return NS_OK;
}
