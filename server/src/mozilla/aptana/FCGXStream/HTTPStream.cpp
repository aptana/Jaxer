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
#include "prerror.h"
#include "prprf.h"
#include "HTTPStream.h"
#include "nsILocalFile.h"
#include "nsCRT.h"
#include "nsDirectoryServiceDefs.h"
#include "nsIPrefService.h"
#include "nsThreadUtils.h"
#include "nsIConsoleService.h"
#include "aptCoreTrace.h"
//#include "ConsoleLog.h"
//#include "nsIFileURL.h"
#include "nsIStorageStream.h"
#include "nsContentCID.h"
#include "nsIFileStreams.h"
#include "nsNetCID.h"
#include "plstr.h"
#include "prenv.h"
#include "nsISchemaValidatorRegexp.h"
#include "nsString.h"

//#define kREGEXP_CID "@mozilla.org/xmlextras/schemas/schemavalidatorregexp;1"

#ifdef _WIN32

#include <windows.h>
#include <direct.h>

#define NO_SNPRINTF

#define socklen_t int

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define HANDLE int
#define INVALID_SOCKET -1
#define INVALID_HANDLE_VALUE -1
#define WSAEINTR EINTR
int WSAGetLastError(){ return errno;}
#endif

#include "jaxerProtocolVersion.h"
#include "blockDump.h"
#include "jaxerBuildId.h"

#include "tdate_parse.h"
#include "match.h"

// static HANDLE g_pipe = INVALID_HANDLE_VALUE;
extern aptCoreTrace gJaxerLog;

static const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";

#define SERVER_NAME "AptanaJaxer"
#define SERVER_URL "http://www.aptana.com/"

static PRBool gMimeInited;

// Concurrency Issues:
//
// The only time this code has to run on multiple threads is when Mozilla is
// reading the HTML document via its input pump.  This becomes a problem only
// when post data is requested by a script block, which occurs on the main
// thread.  We must suspend the input pump to keep it out of this code while
// the main thread is actively using the connection.  A lock exists for this
// purpose.

static BlockDumper g_recv("Recv");
static BlockDumper g_send("Send");

// The 2 framework files.
static const char* gsClientFrameworkCompressed = "/jaxer/framework/clientFramework_compressed.js";
static const char* gsClientFramework = "/jaxer/framework/clientFramework.js";

static void LogFatal(const char *msg, PRInt32 msglen = -1)
{
    g_recv.Flush();
    g_send.Flush();
    size_t len = msglen < 0 ? PL_strlen(msg) : msglen;
    gJaxerLog.LogBytes(eFATAL, msg, len);
}

#define LOG_RETURN_IF_ERROR(eLevel, rv, msg) \
if (NS_FAILED(rv)) \
{ \
  gJaxerLog.Log(eLevel, "%s rv=0x%x", msg, rv); \
  return rv; \
}

static void LogFatal()
{
#ifdef _WIN32
    int n = GetLastError();
#else
    int n = errno;
#endif

    char *msg = (char *)PR_Malloc(200);
    PRInt32 msglen = PR_snprintf(msg, 200, "OS error code %d", n);
    LogFatal(msg, msglen);
    PR_Free(msg);
}

ssll::ssll(const char* v, ssll *n)
	:next(n)
{
	value = PL_strdup(v);
	len = strlen(value);
}
ssll::~ssll()
{
	PL_strfree(value);
}

susll::susll(const char* v, susll *n)
	:next(n)
{
	value = NS_ConvertUTF8toUTF16(v);
	len = value.Length();
}
susll::~susll()
{
}

nvsll::nvsll(const char* aname, const char* avalue, nvsll* anext)
: next(anext)
{
	name = PL_strdup(aname);
	nlen = strlen(name);
	value = PL_strdup(avalue);
	vlen = strlen(value);
}

nvsll::~nvsll()
{
	PL_strfree(name);
	PL_strfree(value);
}

////////////////////////////////////////////////////////////////////////////////
//Util functions

static void de_dotdot(char* file)
{
    char* cp;
    char* cp2;
    int l;

    /* Collapse any multiple / sequences. */
    while ( ( cp = strstr( file, "//") ) != (char*) 0 )
	{
		for ( cp2 = cp + 2; *cp2 == '/'; ++cp2 )
			continue;
		strcpy( cp + 1, cp2 );
	}

    /* Remove leading ./ and any /./ sequences. */
    while ( strncmp( file, "./", 2 ) == 0 )
		strcpy( file, file + 2 );
    while ( ( cp = strstr( file, "/./") ) != (char*) 0 )
		strcpy( cp, cp + 2 );

    /* Alternate between removing leading ../ and removing xxx/../ */
    for (;;)
	{
		while ( strncmp( file, "../", 3 ) == 0 )
			strcpy( file, file + 3 );
		cp = strstr( file, "/../" );
		if ( cp == (char*) 0 )
			break;
		for ( cp2 = cp - 1; cp2 >= file && *cp2 != '/'; --cp2 )
			continue;
		strcpy( cp2 + 1, cp + 4 );
	}

    /* Also elide any xxx/.. at the end. */
    while ( ( l = strlen( file ) ) > 3 &&
	    strcmp( ( cp = file + l - 3 ), "/.." ) == 0 )
	{
		for ( cp2 = cp - 1; cp2 >= file && *cp2 != '/'; --cp2 )
			continue;
		if ( cp2 < file )
			break;
		*cp2 = '\0';
	}
}

static PRInt32 FindToken(const char *pSrc, PRInt32 nSrcLen, const char *pToken, PRInt32 nTokenLen)
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

struct mime_entry {
    char* ext;
    size_t ext_len;
    char* val;
    size_t val_len;
    };
static struct mime_entry enc_tab[] = {
#include "mime_encodings.h"
    };
static const int n_enc_tab = sizeof(enc_tab) / sizeof(*enc_tab);
static struct mime_entry typ_tab[] = {
#include "mime_types.h"
    };
static const int n_typ_tab = sizeof(typ_tab) / sizeof(*typ_tab);


/* qsort comparison routine - declared old-style on purpose, for portability. */
static int
ext_compare( const void* a, const void* b )
{
	//struct mime_entry* a, struct mime_entry* b
    return strcmp( ((mime_entry*) a)->ext, ((mime_entry*) b)->ext );
}


static void init_mime()
{
    int i;

    /* Sort the tables so we can do binary search. */
    qsort( enc_tab, n_enc_tab, sizeof(*enc_tab), ext_compare );
    qsort( typ_tab, n_typ_tab, sizeof(*typ_tab), ext_compare );

    /* Fill in the lengths. */
    for ( i = 0; i < n_enc_tab; ++i )
	{
	enc_tab[i].ext_len = strlen( enc_tab[i].ext );
	enc_tab[i].val_len = strlen( enc_tab[i].val );
	}
    for ( i = 0; i < n_typ_tab; ++i )
	{
	typ_tab[i].ext_len = strlen( typ_tab[i].ext );
	typ_tab[i].val_len = strlen( typ_tab[i].val );
	}
}


/* Figure out MIME encodings and type based on the filename.  Multiple
** encodings are separated by commas, and are listed in the order in
** which they were applied to the file.
*/

static const char* figure_mime(const char* name, char* me, size_t me_size )
{
    const char* prev_dot;
    const char* dot;
    const char* ext;
    int me_indexes[100], n_me_indexes;
    size_t ext_len, me_len;
    int i, top, bot, mid;
    int r;
    const char* default_type = "text/plain";
    const char* type;

	if (! gMimeInited)
	{
		init_mime();
		gMimeInited = PR_TRUE;
	}

    /* Peel off encoding extensions until there aren't any more. */
    n_me_indexes = 0;
    for ( prev_dot = &name[strlen(name)]; ; prev_dot = dot )
	{
		for ( dot = prev_dot - 1; dot >= name && *dot != '.'; --dot )
			;
		if ( dot < name )
	    {
			/* No dot found.  No more encoding extensions, and no type
			** extension either.
			*/
			type = default_type;
			goto done;
	    }
		ext = dot + 1;
		ext_len = prev_dot - ext;
		/* Search the encodings table.  Linear search is fine here, there
		** are only a few entries.
		*/
		for ( i = 0; i < n_enc_tab; ++i )
	    {
			if ( ext_len == enc_tab[i].ext_len && PL_strncasecmp( ext, enc_tab[i].ext, ext_len ) == 0 )
			{
				if ( n_me_indexes < sizeof(me_indexes)/sizeof(*me_indexes) )
				{
					me_indexes[n_me_indexes] = i;
					++n_me_indexes;
				}
				goto next;
			}
	    }
		/* No encoding extension found.  Break and look for a type extension. */
		break;

		next: ;
	}

    /* Binary search for a matching type extension. */
    top = n_typ_tab - 1;
    bot = 0;
    while ( top >= bot )
	{
		mid = ( top + bot ) / 2;
		r = PL_strncasecmp( ext, typ_tab[mid].ext, ext_len );
		if ( r < 0 )
			top = mid - 1;
		else if ( r > 0 )
			bot = mid + 1;
		else
		{
			if ( ext_len < typ_tab[mid].ext_len )
				top = mid - 1;
			else if ( ext_len > typ_tab[mid].ext_len )
				bot = mid + 1;
			else
			{
				type = typ_tab[mid].val;
				goto done;
			}
		}
	}
    type = default_type;

    done:

    /* The last thing we do is actually generate the mime-encoding header. */
    me[0] = '\0';
    me_len = 0;
    for ( i = n_me_indexes - 1; i >= 0; --i )
	{
		if ( me_len + enc_tab[me_indexes[i]].val_len + 1 < me_size )
	    {
			if ( me[0] != '\0' )
			{
				strcpy( &me[me_len], "," );
				++me_len;
			}
			strcpy( &me[me_len], enc_tab[me_indexes[i]].val );
			me_len += enc_tab[me_indexes[i]].val_len;
	    }
	}

    return type;
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

#ifdef NO_SNPRINTF
/* Some systems don't have snprintf(), so we make our own that uses
** vsprintf().  This workaround is probably vulnerable to buffer overruns,
** so upgrade your OS!
*/
static int
snprintf( char* str, size_t size, const char* format, ... )
{
    va_list ap;
    int r;

    va_start( ap, format );
    r = vsprintf( str, format, ap );
    va_end( ap );
    return r;
}
#endif /* NO_SNPRINTF */

// Borrowed from jaxerManager, do not escape anything
static char *ScanWord(char *&p)
{
    // Skip white space to find start of new word.
    while (*p && isspace(*p))
        p++;
    char *w = p;

    // In scanning the next word, make sure we never advance p beyond the
    // end of the line.
    if (*p == '"')
	{
        // We have a quote-delimited word.
        char *v = p;
        bool escaped = false;
        while (*++p)
		{
            if (*p == '"')
			{
                p++;
                break;
			} else 
			{
				*v++ = *p;
			}
        }
        *v = 0;
    } else if (*p)
	{
        // We have a space-delimited word.
        while (*p && !isspace(*p))
            p++;
        if (*p)
            *p++ = 0;
    }

    return w;
}

//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// HTTPStream

//PRBool          HTTPStream::mgbWebServerEnabled = PR_FALSE;
PRBool          HTTPStream::mgbRouteEnabled = PR_FALSE;
PRBool          HTTPStream::mgbJaxerRouteAll = PR_FALSE;
char*           HTTPStream::msWebServerConfig = 0;
char*           HTTPStream::msWebDocumentRoot = 0;
size_t          HTTPStream::msWebDocumentRootLen = 0;
char*           HTTPStream::msAdminDocumentRoot = 0;
size_t          HTTPStream::msAdminDocumentRootLen = 0;
JaxerByPass*    HTTPStream::msByPass = 0;
JaxerIndexPage* HTTPStream::msIndexPage = 0;
JaxerHandler*   HTTPStream::msHandler = 0;
JaxerFilter*    HTTPStream::msFilter = 0;
JaxerAlias*     HTTPStream::msAlias = 0;
JaxerRoute*     HTTPStream::msRoute = 0;
	
void HTTPStream::SetWebServerConfigFile(const char* sCfgFile)
{
	//char *val = PR_GetEnv("ENABLE_JAXER_WEBSERVER");
	//mgbWebServerEnabled = (val && *val == '1');
	//if (!mgbWebServerEnabled)
	//	return;

	if (msWebServerConfig)
		PL_strfree(msWebServerConfig);
	msWebServerConfig = PL_strdup(sCfgFile);
	LoadWebServerConfig();
}

void HTTPStream::SetWebDocumentRoot(const char* value)
{
    if (msWebDocumentRoot)
		PL_strfree(msWebDocumentRoot);
	msWebDocumentRoot = PL_strdup(value);
	msWebDocumentRootLen = strlen(msWebDocumentRoot);

	// Remove trailing slash
	char * p = msWebDocumentRoot + msWebDocumentRootLen -1;
	while(p>=msWebDocumentRoot && *p == '/')
	{
		*p-- = 0;
		msWebDocumentRootLen--;
	}

}

void HTTPStream::SetAdminDocumentRoot(const char* value)
{
    if (msAdminDocumentRoot)
		PL_strfree(msAdminDocumentRoot);
	msAdminDocumentRoot = PL_strdup(value);
	msAdminDocumentRootLen = strlen(msAdminDocumentRoot);

	// Remove trailing slash
	char * p = msAdminDocumentRoot + msAdminDocumentRootLen -1;
	while(p>=msAdminDocumentRoot && *p == '/')
	{
		*p-- = 0;
		msAdminDocumentRootLen--;
	}
}

void HTTPStream::AddJaxerByPass(const char* value)
{
    msByPass = new JaxerByPass(value, msByPass);
#if 0
	//Remove trailing "/"
	size_t len = msByPass->len;
	while (len>1 && msByPass->value[len-1] == '/')
	{
		msByPass->value[len-1] = 0;
		msByPass->len--;
		len--;
	}
#endif
}

void HTTPStream::AddJaxerIndexPage(const char* value)
{
    msIndexPage = new JaxerIndexPage(value, msIndexPage);
}

void HTTPStream::AddJaxerRoute(const char* value)
{
	if (mgbJaxerRouteAll)
		return;

    msRoute = new JaxerRoute(value, msRoute);
	//Remove trailing "/"
	size_t len = msRoute->len;
	while (len>1 && msRoute->value[len-1] == '/')
	{
		msRoute->value[len-1] = 0;
		msRoute->len--;
		len--;
	}
	if (msRoute->len == 1 && msRoute->value[0] == '/')
		mgbJaxerRouteAll = PR_TRUE;
}

void HTTPStream::SetEnableJaxerRoute(const char* value)
{
	if (value && PL_strcasecmp(value, "on") == 0)
		mgbRouteEnabled = PR_TRUE;
	else if (!value || PL_strcasecmp(value, "off"))
	{
		gJaxerLog.Log(eWARN, "Invalid value for EnableJaxerRoute.  Valid values are on off.");
	}
}

void HTTPStream::AddJaxerHandler(const char* value)
{
    msHandler = new JaxerHandler(value, msHandler);
#if 0
	//Remove trailing "/"
	size_t len = msHandler->len;
	while (len>1 && msHandler->value[len-1] == '/')
	{
		msHandler->value[len-1] = 0;
		msHandler->len--;
		len--;
	}
#endif
}

void HTTPStream::AddJaxerFilter(const char* value)
{
	// Remove leading "."
	const char *p = value;
	//while(*p == '.') p++;
    msFilter = new JaxerFilter(p, msFilter);
}

void HTTPStream::AddJaxerAlias(const char* url, const char* path)
{
    msAlias = new nvsll(url, path, msAlias);
}

void HTTPStream::PostConfigProcess()
{
    char buf[2000];
	char wkbuf[2000];
	//In case nothing is set or something is missing,
	//set some defaults

	// Use current directory
	char *p;
#ifdef _WIN32
	p = _getcwd(buf, 2000);
	char *q = p;
	while(q && *q)
	{
		if (*q == '\\')
			*q = '/';
		q++;
	}
#else
	p = getcwd(buf, 2000);
#endif
	
	//DocumentRoot must be set
	if (!msWebDocumentRoot)
	{

		if (p)
		{
			SetWebDocumentRoot(p);
		}else
		{
			SetWebDocumentRoot(".");
			gJaxerLog.Log(eINFO, "No document root specified.");
		}
	}

	//Handler, at least /jaxer-callback/
	if (!msHandler)
		AddJaxerHandler("/jaxer-callback");

	if (!msFilter)
	{
		AddJaxerFilter("htm");
		AddJaxerFilter("html");
	}

    // Need to have the 2 aliases
    JaxerAlias *ja = msAlias;
    PRBool bHasCF = PR_FALSE;
    PRBool bHasCFC = PR_FALSE;
    size_t CFClen = strlen(gsClientFrameworkCompressed);
    size_t CFlen = strlen(gsClientFramework);
    while(ja && !bHasCF && !bHasCFC)
    {
        if (!bHasCF && ja->nlen == CFlen && PL_strcasecmp(gsClientFramework, ja->name) == 0)
            bHasCF = PR_TRUE;
        if (!bHasCFC && ja->nlen == CFClen && PL_strcasecmp(gsClientFrameworkCompressed, ja->name) == 0)
            bHasCFC = PR_TRUE;
        ja = ja->next;
    }
    if (!bHasCFC)
    {
		sprintf(wkbuf, "%s/framework/clientFramework_compressed.js", p ? p : ".");
        AddJaxerAlias(gsClientFrameworkCompressed, wkbuf);
    }
    if (!bHasCF)
    {
        sprintf(wkbuf, "%s/framework/clientFramework.js", p ? p : ".");
        AddJaxerAlias(gsClientFramework, wkbuf);
    }

    //Index file
    if (!msIndexPage)
    {
        AddJaxerIndexPage("index.html");
    }

	sprintf(wkbuf, "%s/admin", p ? p : ".");
	SetAdminDocumentRoot(wkbuf);
}

#define MAX_LINE 2000
void HTTPStream::LoadWebServerConfig()
{
	FILE* fp = 0;
	char buf[MAX_LINE + 1];
	char *p, *directive, *arg;
	
	//if (mgbWebServerEnabled)
	{
		fp = fopen(msWebServerConfig, "r");
		// Check error
		if (!fp)
		{
			gJaxerLog.Log(eINFO, "Cannot open config file %s.", msWebServerConfig);
		}
	}
	while(fp && fgets(buf, MAX_LINE, fp))
	{
		p = buf;
		directive = ScanWord(p);

		// Ignore comments
		if (*directive == 0 || *directive == '#')
			continue;

		// Find arg(s)
		arg = ScanWord(p);
		if (!arg || *arg == '#')
		{
			gJaxerLog.Log(eWARN, "Error reading config.  Invalid line: %s", buf);
			continue;
		}

		if (PL_strcasecmp("DocumentRoot", directive) == 0)
		{
			SetWebDocumentRoot(arg);
		}else if (PL_strcasecmp("ByPassJaxer", directive) == 0)
		{
			do
			{
				AddJaxerByPass(arg);
			}while ((arg = ScanWord(p)) && *arg && *arg != '#');
		}else if (PL_strcasecmp("JaxerHandler", directive) == 0)
		{
			do
			{
				AddJaxerHandler(arg);
			}while ((arg = ScanWord(p)) && *arg && *arg != '#');
		}else if (PL_strcasecmp("JaxerFilter", directive) == 0)
		{
			do
			{
				AddJaxerFilter(arg);
			}while ((arg = ScanWord(p)) && *arg && *arg != '#');
		}else if (PL_strcasecmp("IndexPage", directive) == 0)
		{
			do
			{
				AddJaxerIndexPage(arg);
			}while ((arg = ScanWord(p)) && *arg && *arg != '#');
		}else if (PL_strcasecmp("JaxerAlias", directive) == 0)
		{
			char* arg2 = ScanWord(p);
			if(arg2 && *arg2 && *arg2 != '#')
			{
				AddJaxerAlias(arg, arg2);
			}
		}else if (PL_strcasecmp("JaxerRoute", directive) == 0)
		{
			do
			{
				AddJaxerRoute(arg);
			}while ((arg = ScanWord(p)) && *arg && *arg != '#');
		}else if (PL_strcasecmp("EnableJaxerRoute", directive) == 0)
		{
			SetEnableJaxerRoute(arg);
		}else
		{
			gJaxerLog.Log(eWARN, "Error reading config.  Invalid line: %s", buf);
			continue;
		}
	}
    if (fp)
	    fclose(fp);
	PostConfigProcess();
}

HTTPStream::HTTPStream(PRInt32 socket, eMSG_TYPE type)
    : mFD(socket)
	, mRequestType(type)
    , mRecvIn(0)
    , mRecvOut(0)
	, mSendIn(0)
	, mClientPort(0)
	, mnContentLength(0)
    , mContentLeft(0)
	, mHasClientInfo(PR_FALSE)
	, mEORSeen(PR_FALSE)
	, mIsAlias(PR_FALSE)
	, mResponseHeaders(0)
	, mLastModified(0)
	, mIfModifiedSince(0)
	, mIsCallback(PR_FALSE)
	, mIsRoute(PR_FALSE)
	, mShouldCloseConnection(PR_FALSE)
	, mJaxerShouldProcess(PR_TRUE)
	, mProcessFailed(PR_FALSE)
	, mErrorCode(0)
	, mPathInfo(0)
	, mMethodNum(METHOD_UNKNOWN)
{
	int i;
	for(i=0; i<eREQ_END; i++)
	{
		mRequestHeaders[i] = 0;
	}

	sockaddr_in client;
    socklen_t alen = sizeof(client);
	memset(&client, 0, alen);
	if (0 == getpeername(mFD, (sockaddr*)&client, &alen))
	{
		mClientPort = ntohs(client.sin_port);
		strcpy(mClientAddr, inet_ntoa(client.sin_addr));
		mHasClientInfo = PR_TRUE;
	}else
	{
		gJaxerLog.Log(eDEBUG, "cannot obtain client info for socket %d", mFD);
	}
}

HTTPStream::~HTTPStream()
{
	if (mFD != INVALID_SOCKET)
		CloseConnection(false);

	Clear();
}

void HTTPStream::Clear()
{
	for(int i=0; i<eREQ_END; i++)
	{
		if (mRequestHeaders[i])
		{
			PR_Free(mRequestHeaders[i]);
			mRequestHeaders[i] = 0;
		}
	}

	ResponseHeader *p = 0;
	while (mResponseHeaders)
	{
		p = mResponseHeaders;
		mResponseHeaders = p->next;
		delete p;
	}
	mResponseHeaders = 0;

}


nsresult
HTTPStream::IsSocketValid(PRBool *val)
{
    *val = (mFD != INVALID_SOCKET);
     return NS_OK;
}

PRBool HTTPStream::GetProcessFailed()
{
	return mProcessFailed;
}

////////////////////////////////////////////////////////////////////////////////


char* HTTPStream::get_request_line()
{
    //TODO: should remove leading empty lines
	char* p = 0;
	while(mFD != INVALID_SOCKET)
	{
		FillBuffer();
		p = strchr(mRecvBuf+mRecvOut, '\012');
		if (p)
		{
			*p = 0;
			if (p > mRecvBuf+mRecvOut && p[-1] == '\015')
				p[-1] = 0;

			mRecvOut += p+1 -(mRecvBuf+mRecvOut);
			return mRecvBuf;
		}
	}

    return (char*) 0;
}


nsresult HTTPStream::SetRequestHeader(enum eREQUEST_HEADER etype, const char* value, PRBool bCopy)
{
	PRUint32 len = strlen(value);
	if (mRequestHeaders[etype])
		PR_Free(mRequestHeaders[etype]);
	if (bCopy)
	{
		mRequestHeaders[etype] = (char*) PR_Malloc(len + 1);
		memcpy(mRequestHeaders[etype], value, len+1);
	}else
		mRequestHeaders[etype] = (char*)value;

	return NS_OK;
}

const char* HTTPStream::GetRequestHeader(enum eREQUEST_HEADER etype)
{
	return mRequestHeaders[etype];
}

nsresult HTTPStream::LoadDocumentIntoStream()
{
	// construct the input stream from file, or callback string/file
	nsresult rv = NS_OK;

	rv = NS_NewStorageStream(4096, PR_UINT32_MAX, getter_AddRefs(mDocumentStorageStream));
	LOG_RETURN_IF_ERROR(eFATAL, rv, "Create storage stream failed.");
	
#if 0
	nsCOMPtr<nsILocalFile> lf = do_CreateInstance("@mozilla.org/file/local;1", &rv);
    LOG_RETURN_IF_ERROR(eFATAL, rv, "Create local file failed.");
	

	nsCOMPtr<nsIFileInputStream> fis = do_CreateInstance(NS_LOCALFILEINPUTSTREAM_CONTRACTID);
    if (!fis)
    {
        gJaxerLog.Log(eERROR, "Create File Input Stream failed");
        return NS_ERROR_UNEXPECTED;
    }

	rv = fis->Init(NS_ConvertUTF8toUTF16(GetRequestHeader(eREQ_FILE)), -1, -1, 0);
    LOG_RETURN_IF_ERROR(eFATAL, rv, "Init file stream failed.")

	nsCOMPtr<nsIInputStream> inputStream = do_QueryInterface(fis, &rv);
	LOG_RETURN_IF_ERROR(eFATAL, rv, "QI input stream failed.")
#endif

	nsCOMPtr<nsIOutputStream> outputStream;
	rv = mDocumentStorageStream->GetOutputStream(0, getter_AddRefs(outputStream));
	LOG_RETURN_IF_ERROR(eFATAL, rv, "Get output stream from storage failed.");

	PRUint32 nRead, nWrite;
    char buf[4096];
	FILE *fp = fopen (GetRequestHeader(eREQ_FILE), "rb");
    //while ((rv = inputStream->Read(buf, 4096, &nRead)) == NS_OK && nRead > 0)
	while ((nRead = fread(buf, 1, 4096, fp)) > 0)
	{
		rv = outputStream->Write(buf, nRead, &nWrite);
		LOG_RETURN_IF_ERROR(eFATAL, rv, "Write to storage output stream failed.");
		NS_ENSURE_TRUE(nRead == nWrite, NS_ERROR_FAILURE);
    }

	fclose(fp);
	//LOG_RETURN_IF_ERROR(eFATAL, rv, "Read from input stream failed.");
	rv = outputStream->Close();
	LOG_RETURN_IF_ERROR(eFATAL, rv, "Close output stream failed.");

	return NS_OK;
}

int HTTPStream::GetPathinfo(char* file, struct stat* sb)
{
    int r;

    mPathInfo = &file[strlen(file)];
    for (;;)
	{
		do
	    {
			--mPathInfo;
			if ( mPathInfo <= file )
			{
				mPathInfo = (char*) 0;
				return -1;
			}
	    }
		while ( *mPathInfo != '/' );

		*mPathInfo = '\0';
		r = stat(file, sb);
		if ( r >= 0 )
		{
			++mPathInfo;
			return r;
		}
		else
			*mPathInfo = '/';
	}
	return -2;
}

nsresult HTTPStream::SetErrorDoc(int s, char* title, char* text)
{
	mErrorCode = s;
	strcpy(mErrorTitle, title);
	strcpy(mErrorText, text);
	return NS_ERROR_FAILURE;
}

PRBool HTTPStream::DrainRequest()
{
	char* pPostData;
#if 0
	if(!mEORSeen && mRecvIn>mRecvOut && mRecvOut > 0)
	{
		memcpy(mRecvBuf, mRecvBuf+mRecvOut, mRecvIn-mRecvOut);
		mRecvIn -= mRecvOut;
		mRecvOut = 0;
	}
#endif
    while (!mEORSeen && mFD != INVALID_SOCKET)
	{
		FillBuffer();
		if (mEORSeen)
		{
			char *p, *q;
			if((p=strstr( (char*)mRecvBuf, "\015\012\015\012" )) || (q=strstr( (char*)mRecvBuf, "\012\012" )))
			{
				// Mark the start of POST data if any
				if (p)
					pPostData = p+4;
				else
					pPostData = q+2;
				mContentLeft -= mRecvIn - (pPostData - (char*)mRecvBuf);
			}
		}else
		{
			if (mRecvIn>4)
			{
				//memcpy(mRecvBuf, mRecvBuf+mRecvIn-4, 4);
				//mRecvIn=4;
				mRecvOut=mRecvIn-4;
			}
		}
	}
	while(mContentLeft>0 && mFD != INVALID_SOCKET)
	{
		FillRequestContent();
	}
	return (mFD != INVALID_SOCKET);
}

void HTTPStream::SendErrorDoc()
{
	int s = mErrorCode;
	char * title = mErrorTitle;
	char * text = mErrorText;

	PRBool bShouldExit = PR_FALSE;

	if (s==0)
	{
		s = 500;
		title = "Internal Server Error";
		text = "Processing the request failed.";
	}

	gJaxerLog.Log(eDEBUG, "SendErrorDoc: %d title=%s", s, title);

	//First, drain any request stuff
	if (mMethodNum == METHOD_UNKNOWN ||
		((mMethodNum==METHOD_POST || mMethodNum==METHOD_PUT) && mnContentLength == 0))
	{
		bShouldExit = PR_TRUE;
	}

	if (!bShouldExit)
	{
		bShouldExit = ! DrainRequest();
	}
	if (mFD == INVALID_SOCKET)
		CloseConnection(true);

    BeginResponse(s, title);

	char buf[10000];
    size_t buflen;

	// Need to compute the length, so accumulate msg in buffer first.
    buflen = snprintf(buf, sizeof(buf), "\
<HTML>\n\
<HEAD><TITLE>%d %s</TITLE></HEAD>\n\
<BODY BGCOLOR=\"#cc9999\" TEXT=\"#000000\" LINK=\"#2020ff\" VLINK=\"#4040cc\">\n\
<H4>%d %s</H4>\n",
	s, title, s, title );
    
	size_t len = snprintf( buf+buflen, sizeof(buf)-buflen, "%s\n", text );
    buflen += len; 

	if ( GetRequestHeader(eREQ_USER_AGENT) && match( "**MSIE**", GetRequestHeader(eREQ_USER_AGENT) ) )
	{
		int n;
		len = snprintf(buf+buflen, sizeof(buf)-buflen, "<!--\n" );
		buflen += len;
		for ( n = 0; n < 6; ++n )
		{
			len = snprintf( buf+buflen, sizeof(buf)-buflen, "Padding so that MSIE deigns to show this error instead of its own canned one.\n" );
			buflen += len;
		}
		len = snprintf( buf+buflen, sizeof(buf)-buflen, "-->\n" );
		buflen += len;
	}

    len = snprintf( buf+buflen, sizeof(buf)-buflen, "\
<HR>\n\
<ADDRESS><A HREF=\"%s\">%s/%s</A></ADDRESS>\n\
</BODY>\n\
</HTML>\n",
	SERVER_URL, SERVER_NAME, JAXER_BUILD_ID);
	buflen += len;

	char sbuf[10];
	sprintf(sbuf, "%d", buflen);
	SetResponseHeader("Content-Length", sbuf);

	if (!GetRequestHeader(eREQ_PROTOCOL))
		SetRequestHeader(eREQ_PROTOCOL, "HTTP/1.1");
	BeginResponseBody();
	SendResponseFragment(buf, buflen);
	EndResponse();

	if (bShouldExit)
		CloseConnection(true);
}

#if 0
void HTTPStream::make_log_entry( void )
{
    char* ru;
    char url[500];
    char bytes_str[40];
    time_t now;
    struct tm* t;
    const char* cernfmt_nozone = "%d/%b/%Y:%H:%M:%S";
    char date_nozone[100];
    int zone;
    char sign;
    char date[100];

	//TMP
	FILE* logfp = 0;
	char* protocol = 0;
	char* path=0;
	char* req_hostname = 0;
	char* vhost = 0;
	size_t bytes = 0;
	char* host=0;
	char* hostname=0;
	char* remoteuser=0;

    if ( logfp == (FILE*) 0 )
	return;

    /* Fill in some null values. */
    if ( protocol == (char*) 0 )
	protocol = "UNKNOWN";
    if ( path == (char*) 0 )
	path = "";
    if ( req_hostname == (char*) 0 )
	req_hostname = hostname;

    /* Format the user. */
    if ( remoteuser != (char*) 0 )
	ru = remoteuser;
    else
	ru = "-";
    now = time( (time_t*) 0 );
    /* If we're vhosting, prepend the hostname to the url.  This is
    ** a little weird, perhaps writing separate log files for
    ** each vhost would make more sense.
    */
    if ( vhost )
	(void) snprintf( url, sizeof(url), "/%s%s", req_hostname, path );
    else
	(void) snprintf( url, sizeof(url), "%s", path );
    /* Format the bytes. */
    if ( bytes >= 0 )
	(void) snprintf(
	    bytes_str, sizeof(bytes_str), "%lld", (int64_t) bytes );
    else
	(void) strcpy( bytes_str, "-" );
    /* Format the time, forcing a numeric timezone (some log analyzers
    ** are stoooopid about this).
    */
    t = localtime( &now );
    (void) strftime( date_nozone, sizeof(date_nozone), cernfmt_nozone, t );
#ifdef HAVE_TM_GMTOFF
    zone = t->tm_gmtoff / 60L;
#else
    zone = - ( timezone / 60L );
    /* Probably have to add something about daylight time here. */
#endif
    if ( zone >= 0 )
	sign = '+';
    else
	{
	sign = '-';
	zone = -zone;
	}
    zone = ( zone / 60 ) * 100 + zone % 60;
    (void) snprintf( date, sizeof(date), "%s %c%04d", date_nozone, sign, zone );
    /* And write the log entry. */
    //(void) fprintf( logfp,
	//"%.80s - %.80s [%s] \"%.80s %.200s %.80s\" %d %s \"%.200s\" \"%.200s\"\n",
	//ntoa( &client_addr ), ru, date, get_method_str( method ), url,
	//protocol, status, bytes_str, referer, useragent );
    (void) fflush( logfp );
}
#endif

char* HTTPStream::get_method_str( int m )
{
    switch ( m )
	{
	case METHOD_GET: return "GET";
	case METHOD_HEAD: return "HEAD";
	case METHOD_POST: return "POST";
	case METHOD_PUT: return "PUT";
	case METHOD_OPTIONS: return "OPTIONS";
	default: return "UNKNOWN";
	}
}

nsresult HTTPStream::ParseRequestLine(char* line)
{
	gJaxerLog.Log(eDEBUG, "Request:%s", line);

	char* method = ScanWord(line);
	char* path = ScanWord(line);
	char* protocol = ScanWord(line);
    if (!method || !path || !protocol)
	{
		return SetErrorDoc( 400, "Bad Request", "Can't parse request line." );
	}
    
	//1.  Remove duplicated / at beginning
	while ((path[0] == '/') && (path[1] == '/')) {
        ++path ;
    }

	char* query = strchr( path, '?' );
    if ( query == (char*) 0 )
		query = "";
    else
		*query++ = '\0';

	//In-place decode path, not QS!
	rawURlDecode(path, strlen(path));

	//2.  Replace \ with /
#ifdef _WIN32
	{
        char *x;

        for (x = path; (x = strchr(x, '\\')) != NULL; )
            *x = '/';
    }
#endif

	// Simple check
	if ( PL_strcasecmp( method, get_method_str( METHOD_GET ) ) == 0 )
		mMethodNum = METHOD_GET;
    else if ( PL_strcasecmp(method, get_method_str( METHOD_HEAD ) ) == 0 )
		mMethodNum = METHOD_HEAD;
    else if ( PL_strcasecmp(method, get_method_str( METHOD_POST ) ) == 0 )
		mMethodNum = METHOD_POST;
    //else if ( PL_strcasecmp(method, get_method_str( METHOD_PUT ) ) == 0 )
	//	mMethodNum = METHOD_PUT;
    else if ( PL_strcasecmp(method, get_method_str( METHOD_OPTIONS ) ) == 0 )
		mMethodNum = METHOD_OPTIONS;
    else
	{
		gJaxerLog.Log(eWARN, "Bad Request: Unkown method=%s", method);
		return SetErrorDoc(501, "Not Implemented", "That method is not implemented." );
	}

	// Admin Request only supports GET & POST
	if (mRequestType == eNEW_ADMIN_REQUEST_MSG &&
		mMethodNum != METHOD_GET && mMethodNum != METHOD_POST)
	{
		gJaxerLog.Log(eWARN, "Unsupported Admin Request: Unkown method=%s", method);
		return SetErrorDoc(501, "Not Implemented", "That method is not implemented." );
	}

	//Simple check
	if (*path != '/' )
	{
		gJaxerLog.Log(eDEBUG, "path does not start with /: %s", path);

		// Prefix '/' -- method is destroyed!
		*--path = '/';
	}

	//claps path
	de_dotdot(path+1);

	// Put these into the request before they are destroyed.
	//SetRequestHeader(eREQ_METHOD, method);
	SetRequestHeader(eREQ_PATH, path);
	SetRequestHeader(eREQ_PROTOCOL, protocol);
	SetRequestHeader(eREQ_QUERY_STRING, query);

	//Form URI
	size_t pathlen = strlen(path);
	if (*query)
	{
		if (path+pathlen+1 != query)
		{
			// Path has been changed -- reduced
			path[pathlen++] = '?';
			strcpy(path+pathlen, query);
		}else
			path[pathlen] = '?';
	}
	SetRequestHeader(eREQ_URI, path);

	return NS_OK;
}

nsresult HTTPStream::RecvAndParseRequest(aptIHTTPRequest **areq)
{
	nsresult rv = NS_OK;

	// Make sure webserver is enabled
	//if (!mgbWebServerEnabled && mRequestType != eNEW_ADMIN_REQUEST_MSG)
	//	return SetErrorDoc( 500, "Internal Server Error", "No internal webserver support" );;

	//Parse the entire request from the socket, and populate req object
	*areq = nsnull;

	// Free data if any
	Clear();

	// Create the object
	mReq = do_CreateInstance(APT_HTTP_REQUEST_CONTRACTID);
    if (!mReq)
	{
        LogFatal("HTTPStream::RecvAndParseRequest(): Could not create aptIHTTPRequest object.");
		//CloseConnection(true);
		return NS_ERROR_OUT_OF_MEMORY;
	}

	// Read and parse request
	//FillBuffer();

	/* Parse the first line of the request. */
    char* line = get_request_line();
	rv = ParseRequestLine(line);
	NS_ENSURE_SUCCESS(rv, rv);

    /* Parse the rest of the request headers. */
	char *cp;
    while ( ( line = get_request_line() ) != (char*) 0 )
	{
		if ( line[0] == '\0' )
			break;

		cp = strchr(line, ':');
		if (!cp)
		{
			gJaxerLog.Log(eWARN, "Bad Request: unkown head line: %s", line);
			return SetErrorDoc( 400, "Bad Request", "Can't parse request." );
		}
		*cp++ = 0;
		//while (isspace(*cp)) cp++;
		cp += strspn( cp, " \t" );

		if ( PL_strcasecmp( line, "Authorization") == 0 )
		{
			SetRequestHeader(eREQ_AUTHORIZATION, cp);
		}
		else if ( PL_strcasecmp( line, "Content-Length") == 0 )
		{
			mnContentLength = atol( cp );
			mContentLeft = mnContentLength;
			SetRequestHeader(eREQ_CONTENT_LENGTH, cp);
		}
		else if ( PL_strcasecmp( line, "Content-Type") == 0 )
		{
			SetRequestHeader(eREQ_CONTENT_TYPE, cp);
			rv = mReq->SetRawPostDataContentType(NS_ConvertUTF8toUTF16(cp));
            LOG_RETURN_IF_ERROR(eFATAL, rv, "SetRawPostDataContentType failed.")
		}
		else if ( PL_strcasecmp( line, "Cookie") == 0 )
		{
			SetRequestHeader(eREQ_COOKIE, cp);
		}
		else if ( PL_strcasecmp( line, "Host") == 0 )
		{
			if ( strchr( cp, '/' ) != (char*) 0 || cp[0] == '.' )
			{
				gJaxerLog.Log(eWARN, "Bad Request: Bad host: %s", cp);
				return SetErrorDoc( 400, "Bad Request", "Can't parse request - Bad Host" );
			}
			SetRequestHeader(eREQ_HOST, cp);
			mReq->AddEnv(NS_ConvertUTF8toUTF16("HTTP_HOST"), NS_ConvertUTF8toUTF16(cp));
		}
		else if ( PL_strcasecmp( line, "If-Modified-Since") == 0 )
		{
			mIfModifiedSince = tdate_parse( cp );
			SetRequestHeader(eREQ_IF_MODIFIED_SINCE, cp);
		}
		else if ( PL_strcasecmp( line, "Referer") == 0 )
		{
			SetRequestHeader(eREQ_REFERER, cp);
		}
		else if ( PL_strcasecmp( line, "User-Agent") == 0 )
		{
			SetRequestHeader(eREQ_USER_AGENT, cp);
		}else if ( PL_strcasecmp( line, "Connection") == 0 )
		{
			SetRequestHeader(eREQ_CONNECTION, cp);
		}else
		{
			gJaxerLog.Log(eDEBUG, "Request Header: %s:%s", line, cp);
		}

		rv = mReq->AppendHeader(NS_ConvertUTF8toUTF16(line), NS_ConvertUTF8toUTF16(cp));
		NS_ENSURE_SUCCESS(rv, rv);
	}

	if (mMethodNum == METHOD_POST || mMethodNum == METHOD_PUT)
	{
		int extra = mRecvIn - mRecvOut;
		if (extra>0)
		{
			memcpy(mRecvBuf, mRecvBuf+mRecvOut, extra);
			if (mContentLeft <= extra)
				mContentLeft = 0;
			else
				mContentLeft -= extra;
		}else
			extra=0;

		mRecvIn = extra;
		mRecvOut = 0;
	}

	// Process additional file-related stuff after request header has been processed.
	rv = ProcessFilePath();
	NS_ENSURE_SUCCESS(rv, rv);

	PostFileProcess();
	
	if (!mIsCallback && !mIsRoute)
	{
		rv = LoadDocumentIntoStream();
		NS_ENSURE_SUCCESS(rv, rv);
	}

	if (mMethodNum == METHOD_POST || mMethodNum == METHOD_PUT)
	{
		rv = HandlePostData();
		NS_ENSURE_SUCCESS(rv, rv);
	}

	if (mJaxerShouldProcess)
	{
		gJaxerLog.Log(eDEBUG, "Jaxer should process the request.");

		if (mRequestType == eNEW_ADMIN_REQUEST_MSG)
		{
			rv = mReq->SetIsAdmin(PR_TRUE);
			NS_ENSURE_SUCCESS(rv, rv);
		}
	
		rv = mReq->SetQueryString(NS_ConvertUTF8toUTF16(GetRequestHeader(eREQ_QUERY_STRING)));
		NS_ENSURE_SUCCESS(rv, rv);
		rv = mReq->SetMethod(NS_ConvertUTF8toUTF16(get_method_str(mMethodNum)));
		NS_ENSURE_SUCCESS(rv, rv);
		rv = mReq->SetProtocol(NS_ConvertUTF8toUTF16(GetRequestHeader(eREQ_PROTOCOL)));
		NS_ENSURE_SUCCESS(rv, rv);
		rv = mReq->SetUri(NS_ConvertUTF8toUTF16(GetRequestHeader(eREQ_URI)));
		NS_ENSURE_SUCCESS(rv, rv);

		
		rv = mReq->AddEnv(NS_LITERAL_STRING("SCRIPT_NAME"), NS_ConvertUTF8toUTF16(GetRequestHeader(eREQ_PATH)));

		rv = mReq->SetDocumentRoot(NS_ConvertUTF8toUTF16(GetDocumentRoot() ));
		
		//TODO:
		//char filepath[2000];
		//if (mIsAlias)
		//	snprintf(filepath, sizeof(filepath), "%s", GetRequestHeader(eREQ_FILE));
		//else
		//	snprintf(filepath, sizeof(filepath), "%s/%s", GetDocumentRoot(),GetRequestHeader(eREQ_FILE));
		
		gJaxerLog.Log(eDEBUG, "PageFile=%s", GetRequestHeader(eREQ_FILE));
		rv = mReq->SetPageFile(NS_ConvertUTF8toUTF16(GetRequestHeader(eREQ_FILE)));
		NS_ENSURE_SUCCESS(rv, rv);

		// Hardcoded
		rv = mReq->SetStatusCode(200);
		NS_ENSURE_SUCCESS(rv, rv);

		rv = mReq->SetIsHTTPS(PR_FALSE);
		NS_ENSURE_SUCCESS(rv, rv);

		//Client Info
		if (mHasClientInfo)
		{
			rv = mReq->SetRemoteAddr(NS_ConvertUTF8toUTF16(mClientAddr));
			NS_ENSURE_SUCCESS(rv, rv);

            rv = mReq->SetRemotePort(mClientPort);
			NS_ENSURE_SUCCESS(rv, rv);
		}

		//SCRIPT_NAME /foo/bar.html
		//SCRIPT_FILENAME c:/mywebroot/foo/bar.html
	}
	NS_IF_ADDREF(*areq = mReq);
	return rv;
}

const char* HTTPStream::GetDocumentRoot()
{
    if (mRequestType == eNEW_ADMIN_REQUEST_MSG)
		return msAdminDocumentRoot;
	else
		return msWebDocumentRoot;
}

size_t HTTPStream::GetDocumentRootLength()
{
    if (mRequestType == eNEW_ADMIN_REQUEST_MSG)
		return msAdminDocumentRootLen;
	else
		return msWebDocumentRootLen;
}

nsresult
HTTPStream::ProcessPostDataHelper(nsCAutoString &urlEncodedData)
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
        rv = mReq->AddPostStringItem(sName, sValue);
        NS_ENSURE_SUCCESS(rv, rv);

        // move on
        pch = strtok(NULL, "&");
    }
    return rv;
}

nsresult HTTPStream::CreateUniqFile(char* filename, PRFileDesc **ppfd)
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


nsresult
HTTPStream::ProcessMultiPartData(const char* boundary)
{
    nsresult rv = NS_OK;
    // TODO: 
    // 3. file dir, name

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

    char sFullPath[BUFSIZ];
    PRInt32 nSize = 0;
    PRInt32 nItemCount = 0;
    
    gJaxerLog.Log(eTRACE, "Entered ProcessMultiPartData");

	//mRecvIn = mRecvOut = 0;
    rv = FillRequestContent();
	LOG_RETURN_IF_ERROR(eFATAL, rv, "FillRequestContent failed.")
    
    while ((mRecvIn-mRecvOut) > (6+nToken3))
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

        if (mContentLeft>0 && (mRecvIn-mRecvOut) < 100)
		{
            rv = FillRequestContent();
			LOG_RETURN_IF_ERROR(eFATAL, rv, "FillRequestContent failed.")
		}

        if (mRecvOut >= mRecvIn)
        {
            gJaxerLog.Log(eERROR, "ProcessMultiPartData: RefillBuf returned unexpected indexes: start=%d end=%d", mRecvOut, mRecvIn);
            return NS_ERROR_UNEXPECTED;
        }
        // a field must start with name=
        nIndex = FindToken(mRecvBuf+mRecvOut, mRecvIn-mRecvOut, "name=\"", 6);
        if (nIndex == -1) {
            // generate name based on index
            // I do not see we can get here.  If you do not have a name, the field is not sent to the server!
            sprintf(sName, "NoNameField_%d", nItemCount);
        } else {
            mRecvOut += nIndex + 6;
            PRInt32 idx = 0;
            while (mRecvBuf[mRecvOut] != '"') {
                if (mContentLeft>0 && mRecvIn > 0 && mRecvOut >= mRecvIn)
                    FillRequestContent();
                if (mRecvOut>=mRecvIn)
                {
                    gJaxerLog.Log(eERROR, "ProcessMultiPartData: Parsing a name field failed");
                    return NS_ERROR_UNEXPECTED;
                }
                sName[idx++] = mRecvBuf[mRecvOut++];
            }
            sName[idx] = 0;
        }
        
        if (mContentLeft>0 && (mRecvIn-mRecvOut) < 20)
		{
            rv = FillRequestContent();
			LOG_RETURN_IF_ERROR(eFATAL, rv, "FillRequestContent failed.")
		}

        // check to see if it is a file.  This has to be on the same line, so we search only a few bytes
        // otherwise, we may got the next item.
        nIndex = FindToken(mRecvBuf+mRecvOut, (mRecvIn>=mRecvOut+20) ? 20 : mRecvIn-mRecvOut, "filename=\"", 10);
        if (nIndex>0 && nIndex<10) {
            bIsFile = true;
            mRecvOut += nIndex + 10;
            PRInt32 idx = 0;
            while (mRecvBuf[mRecvOut] != '"') {
                if (mContentLeft>0 && mRecvOut >= mRecvIn)
                    FillRequestContent();
                if (mRecvOut >= mRecvIn)
                {
                    gJaxerLog.Log(eERROR, "ProcessMultiPartData: Parsing a filename field failed");
                    return NS_ERROR_UNEXPECTED;
                }
                sFileName[idx++] = mRecvBuf[mRecvOut++];
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
            if (mContentLeft>0 && (mRecvIn-mRecvOut) < 256)
            {
				rv = FillRequestContent();
				LOG_RETURN_IF_ERROR(eFATAL, rv, "FillRequestContent failed.")
			}
            nIndex = FindToken(mRecvBuf+mRecvOut, (mRecvIn >= mRecvOut+30) ? 30 : mRecvIn-mRecvOut, "Content-Type:", 13);
            if (nIndex>0 && nIndex<10) {
                mRecvOut += nIndex + 13;
                PRInt32 idy = 0;
                if (mRecvBuf[mRecvOut] == ' ') mRecvOut++;

                while (mRecvBuf[mRecvOut] != '\r' && mRecvBuf[mRecvOut] != '\n') {
                    if (mContentLeft>0 && mRecvOut >= mRecvIn)
                    {
						rv = FillRequestContent();
						LOG_RETURN_IF_ERROR(eFATAL, rv, "FillRequestContent failed.")
					}
                    if (mRecvOut >= mRecvIn)
                    {
                        gJaxerLog.Log(eERROR, "ProcessMultiPartData: Parsing Content-Type failed");
                        return NS_ERROR_UNEXPECTED;
                    }
                    sContentType[idy++] = mRecvBuf[mRecvOut++];
                }
                sContentType[idy] = 0;
            } else
                strcpy(sContentType, "");
        } else
            bIsFile = false;
    

        // move on to the real data
        if (mContentLeft>0 && (mRecvIn-mRecvOut) < 100)
        {
			rv = FillRequestContent();
			LOG_RETURN_IF_ERROR(eFATAL, rv, "FillRequestContent failed.")
		}

        nIndex = FindToken(mRecvBuf+mRecvOut, mRecvIn-mRecvOut, token2, 4);
        if (nIndex < 0) {
            // something unexpected happened
            rv = NS_ERROR_UNEXPECTED;
            gJaxerLog.Log(eERROR, "ProcessMultiPartData: Looking for token (%s) failed.", token2);
            gJaxerLog.Log(eDEBUG, "ProcessMultiPartData: mRecvOut=%d mRecvIn=%d.", mRecvOut, mRecvIn);
            return rv;
        }
        mRecvOut += nIndex+4;
        nIndex1 = FindToken(mRecvBuf+mRecvOut, mRecvIn-mRecvOut, token3, nToken3);
        while (nIndex1 == -1) {
            if (bIsFile) {
                // save file content from mRecvOut to nBufSize-nToken3
                // fwrite(buf+mRecvOut, nBufSize-mRecvOut-nToken3, 1, fpDisk);
                PRInt32 bytesWritten = PR_Write(fd, mRecvBuf+mRecvOut, mRecvIn-mRecvOut-nToken3);
                if (bytesWritten != (mRecvIn-mRecvOut-nToken3))
                {
                    gJaxerLog.Log(eERROR, "ProcessMultiPartData: PR_Write failed. Writing=%d written=%d", 
                        mRecvIn-mRecvOut-nToken3, bytesWritten);
                    return NS_ERROR_UNEXPECTED;
                }

                nSize += mRecvIn-mRecvOut-nToken3;

            } else {
                // append mRecvOut to nBufSize-nToken3 to Value
				//TODO:: ???? next line
				mRecvBuf[mRecvIn-nToken3]= 0;
				nsDependentCString partValue((const char*)mRecvBuf+mRecvOut, mRecvIn-mRecvOut-nToken3);
				sValue.Append(partValue);
                //memcpy(sValue+nSize, buf+mRecvOut, mRecvIn-mRecvOut-nToken3);
                nSize += mRecvIn-mRecvOut-nToken3;;
            }

            if (mRecvIn > 0) {
                mRecvOut = mRecvIn-nToken3;
                if (mContentLeft>0)
                {
					rv = FillRequestContent();
					LOG_RETURN_IF_ERROR(eFATAL, rv, "FillRequestContent failed.")
				}
            } else {
                // we should never be here
                gJaxerLog.Log(eERROR, "ProcessMultiPartData: Unexpected end of postdata");
                return NS_ERROR_UNEXPECTED;
            }
            
            nIndex1 = FindToken(mRecvBuf+mRecvOut, mRecvIn-mRecvOut, token3, nToken3);
        }
                
        // now we have find the end
        if (bIsFile) {
            // save data from start to nIndex1
            // fwrite(buf+mRecvOut, nIndex1, 1, fpDisk);
            PRInt32 bytesWritten = PR_Write(fd, mRecvBuf+mRecvOut, nIndex1);
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
                rv = mReq->AddPostFileItem(NS_ConvertUTF8toUTF16(sName),
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
            // strncat(sValue, buf+mRecvOut, nIndex1);
			mRecvBuf[mRecvOut+nIndex1] = 0;
			nsDependentCString partValue2((const char*)mRecvBuf+mRecvOut, nIndex1);
			sValue.Append(partValue2);
            //memcpy(sValue+nSize, buf+mRecvOut, nIndex1);
            nSize += nIndex1;
            //sValue[nSize] = 0;
            rv = mReq->AddPostStringItem(NS_ConvertUTF8toUTF16(sName),
                NS_ConvertUTF8toUTF16(sValue));
            if (NS_FAILED(rv))
            {
                gJaxerLog.Log(eERROR, "ProcessMultiPartData: AddPostStringItem failed: name=%s value=%s", sName, sValue.get());
                return rv;
            }
            gJaxerLog.Log(eTRACE, "ProcessMultiPartData: Parsed data: name=%s value=%s", sName, sValue.get());
        }
        
        mRecvOut += nIndex1 + strlen(token3);

        // Need to refill buf
        if (mContentLeft>0 && (mRecvIn - mRecvOut) <= (6+nToken3))
        {
			rv = FillRequestContent();
			LOG_RETURN_IF_ERROR(eFATAL, rv, "FillRequestContent failed.")
		}
    }
    if (mContentLeft>0)
    {
        // We did not read all postdata
        gJaxerLog.Log(eERROR, "ProcessMultiPartData: Not all post data are processed");
        return NS_ERROR_UNEXPECTED;

    }
    gJaxerLog.Log(eTRACE, "ProcessMultiPartData: Done");
    return rv;
}


nsresult HTTPStream::HandlePostData()
{
	nsresult rv = NS_OK;
	// Read post data
	// We should have Content-Length & Content-Type
	gJaxerLog.Log(eTRACE, "Requesting Post data");
	
	// check to see if this is MULTIPART
    const char *mpfd = "multipart/form-data;";
    const char *axwfu = "application/x-www-form-urlencoded";
    const char *contentType = GetRequestHeader(eREQ_CONTENT_TYPE);
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

        rv = ProcessMultiPartData(boundary);
        LOG_RETURN_IF_ERROR(eFATAL, rv, "ProcessMultiPartData failed.")
    } else if (PL_strncasecmp(axwfu, contentType, strlen(axwfu)) == 0)
    {
        gJaxerLog.Log(eTRACE, "This is %s", axwfu);
        nsCAutoString data;
		
		//mRecvIn>0 1st time for left overs
		while (mContentLeft > 0 || mRecvIn>0)
        {
			rv = FillRequestContent();
			LOG_RETURN_IF_ERROR(eFATAL, rv, "Read from input stream failed.")
            data.Append((const char*)mRecvBuf, mRecvIn);
			mRecvIn = mRecvOut = 0;
		}
        
        // Set POST data.
        if (data.Length() > 0) 
		{
            rv = mReq->SetRawPostDataBytes(data.Length(), (PRUint8*)data.BeginReading());
            LOG_RETURN_IF_ERROR(eFATAL, rv, "SetRawPostDataBytes failed.")

            rv = ProcessPostDataHelper(data);
            LOG_RETURN_IF_ERROR(eFATAL, rv, "ProcessPostDataHelper failed.")
        }
    }else
    {
        gJaxerLog.Log(eTRACE, "This is a type we do not know: %s", contentType);
        // allocate memory and save the stuff as bytes
        PRUint32 nRead;
        char buf[4097];

		if (mnContentLength > 4096)
		{
			// Save in file
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

			while(mContentLeft>0 || mRecvIn>0)
            {
				rv = FillRequestContent();
				LOG_RETURN_IF_ERROR(eFATAL, rv, "ReadRequestContent failed.")
				if (fwrite(buf, sizeof(char), mRecvIn, fp) != mRecvIn)
                {
                    fclose(fp);
                    gJaxerLog.Log(eERROR, "fwrite failed.");
                    return NS_ERROR_UNEXPECTED;
                }
				mRecvIn = mRecvOut = 0;
            }
            fclose(fp);
            rv = mReq->SetRawPostDataFile(mnContentLength, aFile);
            LOG_RETURN_IF_ERROR(eERROR, rv, "SetRawPostDataFile failed.")
		}else
		{
			// leave in memory
			//TODO can use mRecvBuf
			char buf[4097];
			nRead = 0;
            while(mContentLeft>0 ||mRecvIn>0)
            {
				rv = FillRequestContent();
				LOG_RETURN_IF_ERROR(eFATAL, rv, "ReadRequestContent failed.")
				memcpy(buf+nRead, mRecvBuf, mRecvIn);
				nRead += mRecvIn;
				mRecvIn = mRecvOut = 0;
            }
			rv = mReq->SetRawPostDataBytes(nRead, (PRUint8*)buf);
            LOG_RETURN_IF_ERROR(eFATAL, rv, "SetRawPostDataBytes failed.")
		}
	}
	return rv;
}


void HTTPStream::CheckAlias()
{
	// if path matches alias setting, replace with real path
	//int i;
	const char* path = GetRequestHeader(eREQ_PATH);
	//RequestAlias *ra;
	//for(i=0, ra=&JaxerAlias[0]; ra->uri; ra=&JaxerAlias[++i])
	nvsll *ra = msAlias;
	size_t flen = strlen(path);
	while(ra)
	{
		if (flen == ra->nlen && PL_strcasecmp(path, ra->name) == 0)
		{
			SetRequestHeader(eREQ_PATH, ra->value);
			gJaxerLog.Log(eDEBUG, " ALIAS: %s => %s", ra->name, ra->value);
			mIsAlias = PR_TRUE;
			break;
		}
		ra = ra->next;
	}
}

void HTTPStream::CheckRoute()
{
	// if path matches route setting, prefix with /jaxer-route
	if (!mgbRouteEnabled)
		return;

	const char* path = GetRequestHeader(eREQ_PATH);

	static const char* RoutePrefix = "/jaxer-route";
	static const size_t RPlen = strlen(RoutePrefix);

	char *newpath = 0;
	JaxerRoute *ra = msRoute;
	size_t flen = strlen(path);

	if (mgbJaxerRouteAll)
	{
		newpath = (char*) PR_Malloc(flen + RPlen + 1);
		sprintf(newpath,"%s%s", RoutePrefix, path);
		SetRequestHeader(eREQ_PATH, newpath, PR_FALSE);

		// Change URI as well
		const char* oldURI = GetRequestHeader(eREQ_URI);
		newpath = (char*) PR_Malloc(strlen(oldURI) + RPlen + 1);
		sprintf(newpath,"%s%s", RoutePrefix, oldURI);
		SetRequestHeader(eREQ_URI, newpath, PR_FALSE);

		mIsRoute = PR_TRUE;
		return;
	}

	while(ra)
	{
		if ((flen > ra->len && path[flen] == '/' && PL_strncasecmp(path, ra->value, ra->len) == 0) ||
			(flen == ra->len && PL_strcasecmp(path, ra->value) == 0))
		{
			newpath = (char*) PR_Malloc(flen + RPlen + 1);
			sprintf(newpath,"%s%s", RoutePrefix, path);
			SetRequestHeader(eREQ_PATH, newpath, PR_FALSE);

			// Change URI as well
			const char* oldURI = GetRequestHeader(eREQ_URI);
			newpath = (char*) PR_Malloc(strlen(oldURI) + RPlen + 1);
			sprintf(newpath,"%s%s", RoutePrefix, oldURI);
			SetRequestHeader(eREQ_URI, newpath, PR_FALSE);

			mIsRoute = PR_TRUE;
			return;
		}
		ra = ra->next;
	}
}

nsresult HTTPStream::ProcessFilePath()
{
	nsresult rv = NS_OK;
	PRBool bIsFile = PR_FALSE;
	
	CheckAlias();

	CheckJaxerHandler();

	char pathbuf[10000];
	char *file;
	const char *p = GetRequestHeader(eREQ_PATH);
	size_t plen = strlen(p);
	PRBool bEndWithSlash = (plen>0 && p[plen-1] == '/');

	gJaxerLog.Log(eDEBUG, "ProcessFilePath: %s", p);

	if (plen == 1)
	{
		p++; //pass the '/', or later this will cause a problem
	}else if ( p[1] == '/' || ( plen>=3 && p[1] == '.' && p[2] == '.' &&
	   ( p[3] == '\0' || p[3] == '/' ) ) )
	{
		//reject:
		//    //*
		//    /..
		//    /../*
    
		gJaxerLog.Log(eWARN, "Bad Request with illegal filename %s", p);
		return SetErrorDoc( 400, "Bad Request", "Illegal filename." );
	}

	if (mIsAlias)
	{
		gJaxerLog.Log(eTRACE, "path matches alias");
		sprintf(pathbuf, "%s", p);
	}else
	{
		gJaxerLog.Log(eTRACE, "path does not match alias");
		sprintf(pathbuf, "%s%s", GetDocumentRoot(), p);
	}

	if (mIsCallback)
	{
		SetRequestHeader(eREQ_FILE, pathbuf);
		return NS_OK;
	}
	
	
	file = pathbuf;
	//if ( vhost )
	//file = virtual_file( file );

	

	// If this is a dir, then find the index page
	struct stat sb;
    int r = stat(file, &sb);
    if ( r < 0 )
		r = GetPathinfo(file, &sb);

	if (r>=0)
	{
		size_t file_len = strlen(file);
	
		if ( ! S_ISDIR( sb.st_mode ) )
		{
			gJaxerLog.Log(eTRACE, "Located disk file (%s) for path %s", file, p);
	
			/* Not a directory. */

			//Ending with a slash, but the last item is not a dir
			if (bEndWithSlash)
			{
				gJaxerLog.Log(eWARN, "Requested file does not exist: %s", mRequestHeaders[eREQ_PATH]);
				return SetErrorDoc(404, "Not Found", "Directory not found." );
			}

			while (file[file_len - 1] == '/' )
			{
				file[file_len - 1] = '\0';
				--file_len;
			}
			mLastModified = sb.st_mtime;
			SetRequestHeader(eREQ_FILE, file);
			if (!mIsAlias)
				SetRequestHeader(eREQ_PATH, pathbuf + GetDocumentRootLength()+1);
			else
				SetRequestHeader(eREQ_PATH, pathbuf);
			return rv;
		}else
		{
			// A directory
			if (mPathInfo && *mPathInfo)
			{
				// Check route
				CheckRoute();
				if (mIsRoute)
					return NS_OK;

				//Specifying a file which does not exist
				gJaxerLog.Log(eWARN, "Requested file does not exist: %s", mRequestHeaders[eREQ_PATH]);
				return SetErrorDoc(404, "Not Found", "File not found." );
			}

			if (!bEndWithSlash)
			{
				// A directory.  But missing the trailing slash
				gJaxerLog.Log(eWARN, "Request points to a directory without ending slash: %s", mRequestHeaders[eREQ_PATH]);
				char location[10000];
				if (GetRequestHeader(eREQ_QUERY_STRING) && GetRequestHeader(eREQ_QUERY_STRING)[0])
					(void) snprintf(
						location, sizeof(location), "%s/?%s", GetRequestHeader(eREQ_PATH),
						GetRequestHeader(eREQ_QUERY_STRING) );
				else
					(void) snprintf(
						location, sizeof(location), "%s/", GetRequestHeader(eREQ_PATH));
				SetResponseHeader("Location", location);
				return SetErrorDoc( 301, "Found", "Directories must end with a slash." );
			}

			if ( file[file_len - 1] != '/')
			{
				file[file_len++] = '/';
				file[file_len++] =  0;
			}

			/* Check for an index file. */
			size_t pathlen = strlen(pathbuf);
			char *p = pathbuf + pathlen;
			//*p = '/';
			JaxerIndexPage *pg = msIndexPage;
			while(pg)
			{
				strcpy(p, pg->value);
				if ( stat(file, &sb ) >= 0 )
				{
					mLastModified = sb.st_mtime;
					SetRequestHeader(eREQ_FILE, file);
					if (!mIsAlias)
					{
						SetRequestHeader(eREQ_PATH, pathbuf + GetDocumentRootLength());
						const char *qs = GetRequestHeader(eREQ_QUERY_STRING);
						if (qs && *qs)
						{
							sprintf(pathbuf, "%s?%s", GetRequestHeader(eREQ_PATH), qs);
							SetRequestHeader(eREQ_URI, pathbuf);
						}else
						{
							SetRequestHeader(eREQ_URI, GetRequestHeader(eREQ_PATH));
						}
					}else
					{
						SetRequestHeader(eREQ_PATH, pathbuf);
						// todo: Add index page to URI
					}

					// Check to see if the new PATH matches the Handler setting
					CheckJaxerHandler();

					return rv;
				}
				pg = pg->next;
			}

			if (!bIsFile)
			{
				gJaxerLog.Log(eWARN, "Rejecting directory request: %s", mRequestHeaders[eREQ_PATH]);
				
				return SetErrorDoc( 404, "Forbidden", "Directory access not permitted." );
			}
		}
	}else // if ( r < 0 )
	{
		// Check route
		CheckRoute();
		if (mIsRoute)
			return NS_OK;

		gJaxerLog.Log(eWARN, "Cannot find requested page: path=%s file=%s", GetRequestHeader(eREQ_PATH),
			file);
		gJaxerLog.Log(eWARN, "FILE: %s", file);
		return SetErrorDoc( 404, "Not Found", "File not found." );
	}
	return rv;
}

//Refill buffer and null-terminate string
void HTTPStream::FillBuffer()
{
    if (mFD == INVALID_SOCKET)
        return;

	//Move Data to beginning of buffer
	if (mRecvOut>0)
	{
		memcpy(mRecvBuf, mRecvBuf+mRecvOut, mRecvIn-mRecvOut);
		mRecvIn -= mRecvOut;
		mRecvOut = 0;
		mRecvBuf[mRecvIn] = 0;
	}
	if (mEORSeen)
		return;

	//Just read, assume no more request appending before we send data back
	mRecvOut = 0;
    int amt;
    do {
       amt = recv(mFD, mRecvBuf + mRecvIn, FCGXBUFSIZE - mRecvIn, 0);
	}while(amt < 0 && WSAGetLastError() == WSAEINTR);
	
	if (amt < 0) {
        LogFatal("HTTPStream::FillBuffer(): Could not read data from socket");
        CloseConnection(true);
    } else if (amt == 0) {
        LogFatal("Unexpected connection close.");
        CloseConnection(true);
	}else {
		g_recv.LogContents((PRUint8*)mRecvBuf + mRecvIn, amt);
		// See if we got the end of request
		mRecvIn += amt;
		mRecvBuf[mRecvIn] = 0;
		char *p, *q;
		if((p=strstr(mRecvBuf, "\015\012\015\012" )) || (q=strstr( mRecvBuf, "\012\012" )))
		{
#if 0
			// Mark the start of POST data if any
			if (p)
				mPostData = p+4;
			else
				mPostData = q+2;
#endif
			mEORSeen = PR_TRUE;
		}
	}
}

nsresult HTTPStream::FillRequestContent()
{
    if (mFD == INVALID_SOCKET)
        return NS_ERROR_FAILURE;

	if (mRecvIn > mRecvOut && mRecvOut>0)
		memcpy(mRecvBuf, mRecvBuf+mRecvOut, mRecvIn-mRecvOut);
	mRecvIn -= mRecvOut;
	mRecvOut = 0;

	if (mContentLeft<=0)
		return NS_OK;

	// The caller know how much to read
	PRUint32 nRead = FCGXBUFSIZE - mRecvIn;
	PRUint32 n;
	if (nRead > mContentLeft) nRead = mContentLeft;
	do
	{
		n = recv(mFD, (char*)mRecvBuf+mRecvIn, nRead, 0);
	}while(n < 0 && WSAGetLastError() == WSAEINTR);
	if (n<=0)
	{
		LogFatal("Unexpected connection close.");
		CloseConnection(true);
		return NS_ERROR_FAILURE;
	}

	g_recv.LogContents((PRUint8*)mRecvBuf + mRecvIn, n);

	mRecvIn += n;
	mContentLeft -= n;
	
	return NS_OK;
}

nsresult HTTPStream::Flush(bool close)
{
    if (mSendIn > 0) {
		g_send.Flush();

        int n;
        char *p = (char *)mSendBuf;
        int amt = mSendIn;
        while (amt > 0) {
            do {
                n = send(mFD, p, amt, 0);
            } while (n < 0 && 
#ifdef _WIN32
                WSAGetLastError() == WSAEINTR
#else
                errno == EINTR
#endif
                );
            if (n <= 0) {
                LogFatal("HTTPStream::Flush(): Could not send data.");
                CloseConnection(true);
                return NS_ERROR_ABORT;
            }
			g_send.LogContents((PRUint8*)p, n);
            p += n;
            amt -= n;
        }
        mSendIn = 0;
    }

    if (close) {
        // Note that this doesn't really close the socket as the manager has
        // a socket handle to it also.
        CloseConnection(false);
    }

    return NS_OK;
}

nsresult HTTPStream::SetWebSocket(int socket)
{
#ifdef _WIN32
    mFD = (SOCKET) socket;
#else
    mFD = socket;
#endif
    return NS_OK;
}

void HTTPStream::CloseConnection(bool fatal)
{
    if (mFD != INVALID_SOCKET) {
        g_recv.Flush();
        g_send.Flush();

        mSendIn = 0;

#ifdef _WIN32       
        closesocket(mFD);
#else
        close(mFD);
#endif
        mFD = INVALID_SOCKET;
        mRecvIn = mRecvOut = 0;
    }

    if (fatal)
	{
		gJaxerLog.Terminate();
        exit(1);
	}
}

nsresult HTTPStream::BeginResponse(PRInt32 status, const char* sStatus)
{
	// Setup default headers
	char buf[512];
	time_t now = time( (time_t*) 0 );
    strftime(buf, sizeof(buf), rfc1123_fmt, gmtime(&now) );
	SetResponseHeader("Date", buf);

	sprintf(buf, "%s/%s", SERVER_NAME, JAXER_BUILD_ID);
	SetResponseHeader("Server", buf);

	//SetResponseHeader("Content-Type", "text/html");

	if ( status < 100 || status >= 400 )
	{
		SetResponseHeader("Cache-Control", "no-cache,no-store");
		SetResponseHeader("Content-Type", "text/html");
	}else
	{
		const char* req_ct = GetRequestHeader(eREQ_FILE_CONTENT_TYPE);
		if (req_ct && *req_ct)
			SetResponseHeader("Content-Type", req_ct);
	}

	snprintf( buf, sizeof(buf), "%d %s", status, sStatus);
	mRespStatus_n = status;
	strcpy(mRespStatus_s, buf);

	return NS_OK;
}

nsresult HTTPStream::SetResponseHeader(const char* name, const char* value)
{
	ResponseHeader *p = mResponseHeaders;
	size_t nlen = strlen(name);
	while(p)
	{
		if (nlen==p->nlen && PL_strcasecmp(p->name, name) == 0)
		{
			PL_strfree(p->value);
			p->value = PL_strdup(value);
			p->vlen = strlen(p->value);
			return NS_OK;
		}
		p = p->next;
	}
	mResponseHeaders = new ResponseHeader(name, value, mResponseHeaders);
	return NS_OK;
}

nsresult HTTPStream::BeginResponseBody()
{
	// All headers are set.
	mSendIn = sprintf((char*)mSendBuf, "%s %s\r\n", GetRequestHeader(eREQ_PROTOCOL), mRespStatus_s);

	ResponseHeader *p = mResponseHeaders;
	while(p)
	{
		if (mSendIn + p->nlen + p->vlen > FCGXBUFSIZE - 10)
			Flush();
		size_t n = sprintf((char*)mSendBuf+mSendIn, "%s: %s\r\n", p->name, p->value);
		mSendIn += n;
		p = p->next;
	}
	mSendBuf[mSendIn++] = '\r';
	mSendBuf[mSendIn++] = '\n';

	return NS_OK;
}

nsresult HTTPStream::SendResponseFragment(const char* buf, size_t len)
{
	if (mSendIn + len > FCGXBUFSIZE)
		Flush();
	
	if (mMethodNum == METHOD_OPTIONS || mMethodNum == METHOD_HEAD)
		return NS_OK;

	// Assuming len < FCGXBUFSIZE
	size_t sent = 0;
	size_t send = 0;
	while (sent < len)
	{
		send = FCGXBUFSIZE - mSendIn;
		if (send > (len - sent))
			send = len - sent;
		
		memcpy(mSendBuf+mSendIn, buf+sent, send);
		mSendIn += send;
		sent += send;
		Flush();
	}

	return NS_OK;
}

nsresult HTTPStream::EndResponse()
{
	Flush(true);
	return NS_OK;
}

nsresult HTTPStream::SendOriginalDoc()
{
	nsresult rv;
	gJaxerLog.Log(eDEBUG, "In HTTPStream::SendOriginalDoc");
	if (mMethodNum != METHOD_OPTIONS && mMethodNum != METHOD_HEAD)
	{
		if (mIfModifiedSince >= mLastModified)
		{
			// The document is not modified.  Send back 304
			gJaxerLog.Log(eDEBUG, " Doc not modified, sending back 304.");
			rv = BeginResponse(304, "Not Modified");
			rv = BeginResponseBody();
			return NS_OK;
		}
	}
	PRUint32 dataLength;
	char buf[4096];
    rv = mDocumentStorageStream->GetLength(&dataLength);
    NS_ENSURE_SUCCESS(rv, rv);
	sprintf(buf, "%d", dataLength);
	rv = SetResponseHeader("Content-Length", buf);

	strftime(buf, sizeof(buf), rfc1123_fmt, gmtime( &mLastModified ) );
	rv = SetResponseHeader("Last-Modified", buf);

	rv = BeginResponseBody();
	NS_ENSURE_SUCCESS(rv, rv);

	nsCOMPtr<nsIInputStream> inputStream;
    rv = mDocumentStorageStream->NewInputStream(0, getter_AddRefs(inputStream));
    NS_ENSURE_SUCCESS(rv, rv);
	PRUint32 n;
	while ((rv = inputStream->Read(buf, 4096, &n)) == NS_OK && n > 0)
	{
		rv = SendResponseFragment(buf, n);
        NS_ENSURE_SUCCESS(rv, rv);
	}
    inputStream->Close();

    return NS_OK;
}

void HTTPStream::FigureContentTypeFromFileExt()
{
	//const char* p = GetRequestHeader(eREQ_FILE_CONTENT_TYPE);
	//if (p && *p)
	//	return;

	const char* filename = GetRequestHeader(eREQ_FILE);

	//Content-Type based on ext.
	char encBuf[256];
	const char *mime_type = figure_mime(filename, encBuf, sizeof(encBuf));
	SetRequestHeader(eREQ_FILE_CONTENT_TYPE, mime_type);
}

void HTTPStream::CheckJaxerHandler()
{
	// is this a callback?
	mIsCallback = PR_FALSE;
	const char* p = GetRequestHeader(eREQ_PATH);
	//nsAutoString value = NS_ConvertUTF8toUTF16 (p);
		
	
	FindMatch(msHandler, p, &mIsCallback);
#if 0
	PRBool isValid;

	nsCOMPtr<nsISchemaValidatorRegexp> regexp = do_GetService(NS_SCHEMAVALIDATORREGEXP_CONTRACTID);
	JaxerHandler *jcf = msHandler;
	while(jcf)
	{
		rv = regexp->RunRegexp(value, jcf->value, "g", &isValid);
		if (isValid)
		{
			mIsCallback = PR_TRUE;
			gJaxerLog.Log(eDEBUG, "Request is callback");
			break;
		}
		jcf = jcf->next;
	}
#endif
}

nsresult HTTPStream::FindMatch(const susll* pattern_list, const char* value, PRBool* bMatch)
{
	nsresult rv = NS_OK;
	*bMatch = PR_FALSE;
	nsAutoString aValue = NS_ConvertUTF8toUTF16(value);

	nsCOMPtr<nsISchemaValidatorRegexp> regexp = do_GetService(NS_SCHEMAVALIDATORREGEXP_CONTRACTID);
	
	PRBool isValid;
	const susll *pt = pattern_list;
	while(pt)
	{
		rv = regexp->RunRegexp(aValue, pt->value, "", &isValid);
		if (isValid)
		{
			*bMatch = PR_TRUE;
			return rv;
			break;
		}
		pt = pt->next;
	}

	return rv;
}

void HTTPStream::PostFileProcess()
{
	// Figure out
	// *.  content-type
	// *.  If Page needs to go through Jaxer
	// *.  If so, is this a callback

	nsresult rv;
	if (mIsRoute)
	{
		mJaxerShouldProcess = PR_TRUE;
		return;
	}

	const char* filename = GetRequestHeader(eREQ_FILE);

	FigureContentTypeFromFileExt();

	// go through jaxer?
	const char* p;
	mJaxerShouldProcess = PR_TRUE;
	PRBool bMatch;
	
	//1.  Bypass?
	JaxerByPass* jbp = msByPass;
	//JaxerCallbackFilter * jbp;

	// Should not this be URL?
	p = GetRequestHeader(eREQ_PATH);
	rv = FindMatch(msByPass, p, &bMatch);
	mJaxerShouldProcess = !bMatch;

#if 0
	size_t flen = strlen(p);

	nsAutoString aPath = NS_ConvertUTF8toUTF16(p);

	nsCOMPtr<nsISchemaValidatorRegexp> regexp = do_GetService(NS_SCHEMAVALIDATORREGEXP_CONTRACTID);
	
	PRBool isValid;
	while(jbp)
	{
		rv = regexp->RunRegexp(aPath, jbp->value, "g", &isValid);
		if (isValid)
		{
			mJaxerShouldProcess = PR_FALSE;
			gJaxerLog.Log(eDEBUG, "Page should bypass Jaxer");
			break;
		}
		jbp = jbp->next;
	}
#endif

	//2.  Check Filter
	if (mJaxerShouldProcess && !mIsCallback)
	{
		mJaxerShouldProcess = PR_FALSE;
		//JaxerFilter * ext = msFilter;
		rv = FindMatch(msFilter, p, &mJaxerShouldProcess);
#if 0
		nsAutoString aExt = NS_ConvertUTF8toUTF16(p);
		while(ext)
		{
			rv = regexp->RunRegexp(aExt, ext->value, "g", &isValid);
			if (isValid)
			{
				mJaxerShouldProcess = PR_TRUE;
				gJaxerLog.Log(eDEBUG, "Jaxer should process page based on ext");
				break;
			}
			ext = ext->next;
		}
#endif
	}
}

PRBool HTTPStream::IsCallbackRequest()
{
	return mIsCallback | mIsRoute;
}

PRBool HTTPStream::ShouldJaxerProcessRequest()
{
	return mJaxerShouldProcess;
}

nsresult HTTPStream::GetDocumentStream(nsIStorageStream **ss)
{
	NS_IF_ADDREF(*ss = mDocumentStorageStream);
	return NS_OK;
}

#if 0 //OLD STUFF
void
HTTPStream::FillBuffer()
{
    if (mFD == INVALID_SOCKET)
        return;

	// CATCH: We can only read upto the end of the request.
	// So peek first, and read as needed to avoid over-read.
    mRecvOut = 0;
    int pk_amt, rd_amt;
    do {
       pk_amt = recv(mFD, (char*)mRecvBuf + mRecvIn, FCGXBUFSIZE - mRecvIn, MSG_PEEK);
	}while(pk_amt < 0 && WSAGetLastError() == WSAEINTR);
	
	if (pk_amt < 0) {
        LogFatal("HTTPStream::FillBuffer(): Could not read data from socket");
        CloseConnection(true);
    } else if (pk_amt == 0) {
        LogFatal("Unexpected connection close.");
        CloseConnection(true);
	}else {
		// See if we got the end o frequest
		mRecvBuf[mRecvIn+pk_amt] = 0;
		char *p, *q;
		rd_amt = pk_amt;
		if ((p=strstr( (char*)mRecvBuf, "\015\012\015\012" )) || (q=strstr( (char*)mRecvBuf, "\012\012" )))
		{
			// Make sure we do not read more...
			if (p)
				rd_amt = (p - (char*)mRecvBuf)+4 - mRecvIn;
			else
				rd_amt = (q - (char*)mRecvBuf)+2 - mRecvIn;
			mEORSeen = PR_TRUE;
		}
		int nrd = recv(mFD, (char*)mRecvBuf + mRecvIn, rd_amt, 0);
	   if (rd_amt != nrd)
	   {
		   LogFatal("HTTPStream::FillBuffer(): Could not read data from socket");
		   CloseConnection(true);
	   }
	   mRecvIn += nrd;
	}
}


#endif
