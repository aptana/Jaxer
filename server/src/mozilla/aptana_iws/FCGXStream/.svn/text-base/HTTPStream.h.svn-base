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
#ifndef HTTPStream_h__
#define HTTPStream_h__

#include "nsCOMPtr.h"
#include "nsComponentManagerUtils.h"
#include "nsIInputStream.h"
#include "nsIPrefService.h"
#include "nsIStorageStream.h"
#include "prmon.h"
#include "prio.h"
#include "aptHTTPRequest.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "JaxerDefs.h"

#ifdef _WIN32
#include <Winsock2.h>
#define S_ISDIR(a) (a & _S_IFDIR)
#endif

#define METHOD_UNKNOWN 0
#define METHOD_GET 1
#define METHOD_HEAD 2
#define METHOD_POST 3
#define METHOD_PUT 4
#define METHOD_OPTIONS 5

#ifndef MAXPATHLEN
#define MAXPATHLEN 512
#endif

#ifndef DEFAULTCHARSET
#define DEFAULTCHARSET "iso-8859-1"
#endif /* DEFAULTCHARSET */

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

//#ifndef HAVE_INT64T
//typedef long long int64_t;
//#endif

class nsIFileInputStream;
class nsIStorageStream;
////////////////////////////////////////////////////////////////////////////////

#define FCGXBUFSIZE 8192
//#define JAXER_PROTOCOL_VERSION 3

enum eREQUEST_HEADER
{
	 eREQ_METHOD = 0
	,eREQ_PATH
	//,eREQ_REAL_PATH
	//,eREQ_ALIAS_PATH
	,eREQ_QUERY
	,eREQ_PROTOCOL
	,eREQ_HOST
	,eREQ_AUTHORIZATION
	,eREQ_CONTENT_LENGTH
	,eREQ_CONTENT_TYPE
	,eREQ_FILE_CONTENT_TYPE
	,eREQ_COOKIE
	,eREQ_IF_MODIFIED_SINCE
	,eREQ_REFERER
	,eREQ_USER_AGENT
	,eREQ_CONNECTION
	,eREQ_FILE
	,eREQ_QUERY_STRING
	,eREQ_URI
	,eREQ_END
};

struct ssll
{
	char* value;
	ssll* next;
	size_t len;
	ssll(const char* v, ssll *n);
	~ssll();
};

struct susll
{
	nsAutoString value;
	susll* next;
	size_t len;
	susll(const char* v, susll *n);
	~susll();
};

//NameValueSingleLinkedList
struct nvsll
{
	char* name;
	char *value;
	size_t nlen;
	size_t vlen;
	nvsll *next;
	nvsll(const char* name, const char* value, nvsll* next);
	~nvsll();
};

typedef nvsll ResponseHeader, JaxerAlias;
typedef ssll JaxerIndexPage, JaxerRoute;
typedef susll JaxerByPass, JaxerHandler, JaxerFilter;

class HTTPStream
{
public:
    HTTPStream(PRInt32 socket, eMSG_TYPE type);
    virtual ~HTTPStream();

    // When relavent pref settings have changed, call this function to update prefs
    // It would be better to have its own observer, but for now, just use one.
    //static void UpdatePrefSettings(nsIPrefBranch *aPrefBranch, const char* pref = nsnull);

    // These are the settings we are interested
    //static PRBool g_dumpEnabled;
    
	/////////////////////////
	nsresult IsSocketValid(PRBool *val);
	char* get_request_line();
	nsresult RecvAndParseRequest(aptIHTTPRequest **areq);
	nsresult SetWebSocket(int socket);
	nsresult GetDocumentStream(nsIStorageStream **ss);
	const char* GetRequestHeader(enum eREQUEST_HEADER etype);
	
	nsresult BeginResponse(PRInt32 status=200, const char* sStatus="OK");
    nsresult SetResponseHeader(const char* name, const char* value);
	nsresult BeginResponseBody();
	nsresult SendResponseFragment(const char* buf, size_t len);
	nsresult EndResponse();
	nsresult SendOriginalDoc();

	PRBool ShouldJaxerProcessRequest();
	PRBool IsCallbackRequest();
	PRBool GetProcessFailed();
	void SendErrorDoc();

	static void SetWebServerConfigFile(const char* sCfgFile);

    
private:

	void FillBuffer();
    // Force current block to be completed and sent over the socket.
    nsresult Flush(bool close=false);
	
    // Close connection.  Cannot be called "Close" as that is called by the
    // input pump after HTML has been fully read and parsed.  If fatal, then
    // the connection is in an indeterminate state and we must die and force
    // the manager to also close the connection.
    void CloseConnection(bool fatal);

    // The socket connection.
#ifdef _WIN32
    SOCKET       mFD;
#else
    int          mFD;
#endif

	eMSG_TYPE    mRequestType;

	// Information about the incoming connection
	PRUint16     mClientPort;
	char         mClientAddr[64];
	PRBool       mHasClientInfo;

	// Whether Jaxer failed to parse the request
	PRBool       mProcessFailed;
	char         mErrorTitle[64];
	char         mErrorText[256];
	PRUint16     mErrorCode;

	nsCOMPtr<nsIStorageStream> mDocumentStorageStream;
    
	// Receive stuff.
    PRUint16     mRecvIn;  //Available
    PRUint16     mRecvOut; //Has processed
    
    // Send stuff.
    PRUint16     mSendIn;
    
    ///////////////////////////////USED/////////////////////////////
	// Numeric Value represent the Request type.
	PRUint8      mMethodNum;

	// Content length of the request
	PRUint32     mnContentLength;
	// Used to determine if all content has been read in.
	PRUint32     mContentLeft;

	// Whether we have seen the end of the request HEADER
	PRBool       mEORSeen;
	PRBool       mIsAlias;

	int          mRespStatus_n;
	char         mRespStatus_s[256];
	// A set of known request headers
	char        *mRequestHeaders[eREQ_END];

	ResponseHeader *mResponseHeaders;

	// The Last Time the disk file was modified.
	// Used only if original doc should be used.
	time_t       mLastModified;

	time_t       mIfModifiedSince;
	char        *mPathInfo;

	PRBool       mJaxerShouldProcess;
	PRBool       mIsCallback;
	PRBool       mIsRoute;

    // If things are so out of control, the connection should be
	// closed to avoid further confusions.
	PRBool       mShouldCloseConnection;

	nsCOMPtr<aptIHTTPRequest> mReq;
	
	// Keep the big stuff at the end.
    char      mRecvBuf[FCGXBUFSIZE+1];
    char      mSendBuf[FCGXBUFSIZE+1];
	//char         mWorkBuf[FCGXBUFSIZE+1];

	nsresult ProcessFilePath();
	nsresult HandlePostData();
	nsresult ProcessPostDataHelper(nsCAutoString &urlEncodedData);
	nsresult ProcessMultiPartData(const char* boundary);
	nsresult FillRequestContent();
	nsresult CreateUniqFile(char* filename, PRFileDesc **ppfd);
	nsresult LoadDocumentIntoStream();
	nsresult SetErrorDoc(int s, char* title, char* text);
	PRBool DrainRequest();
	nsresult FindMatch(const susll* pattern_list, const char* value, PRBool* bMatch);

	const char* GetDocumentRoot();
	size_t GetDocumentRootLength();

	nsresult SetRequestHeader(enum eREQUEST_HEADER etype, const char* value, PRBool bCopy=PR_TRUE);
	void Clear();
	int GetPathinfo(char* file, struct stat* sb);
	
	nsresult DoFile(char* filepath);
	//void make_log_entry();
	char* get_method_str( int m );
	void PostFileProcess();
	void CleanPath();
	void CheckAlias();
	void CheckRoute();
	void CheckJaxerHandler();
	void FigureContentTypeFromFileExt();
	nsresult ParseRequestLine(char* line);

	//static PRBool mgbWebServerEnabled;
	static char* msWebServerConfig;
	static char* msWebDocumentRoot;
	static size_t msWebDocumentRootLen;
	static JaxerByPass* msByPass;
	static JaxerIndexPage* msIndexPage;
	static JaxerHandler* msHandler;
	static JaxerFilter* msFilter;
	static JaxerRoute* msRoute;
	static JaxerAlias* msAlias;
	static PRBool mgbRouteEnabled;
	static PRBool mgbJaxerRouteAll;

	static char* msAdminDocumentRoot;
	static size_t msAdminDocumentRootLen;

	static void LoadWebServerConfig();
	static void SetWebDocumentRoot(const char* value);
	static void SetAdminDocumentRoot(const char* value);
	static void AddJaxerByPass(const char* value);
	static void AddJaxerIndexPage(const char* value);
	static void AddJaxerHandler(const char* value);
	static void AddJaxerFilter(const char* value);
	static void AddJaxerRoute(const char* value);
	static void AddJaxerAlias(const char* url, const char* path);
	static void SetEnableJaxerRoute(const char* value);
	static void PostConfigProcess();
};

#endif // HTTPStream_h__
