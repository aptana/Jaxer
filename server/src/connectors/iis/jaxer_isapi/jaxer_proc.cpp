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
// validate.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <httpext.h>
#include <httpfilt.h>
#include "stdio.h"
#include <stdlib.h>
#include <wininet.h>
#include <strsafe.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "jaxer_message.h"
#include "jaxer_connection.h"
#include "jaxer_proc.h"
#include "jaxer_config.h"


static CJaxerServer TheServer;
static CJaxerLog TheLog;
static CJCPool ThePool(&GetJaxerServer());

static const char* sJAXER_HANDLES = "/jaxer-server";
static const char* sJAXER_EXT = "jaxer.jaxer";

CJaxerServer& GetJaxerServer()
{
    return TheServer;
}

CJaxerLog& GetJaxerLog()
{
    return TheLog;
}

CJCPool& GetJCPool()
{
    return ThePool;
}

void BuildHeaderMessage(HeaderMap& map, char type, char** msg)
{
    size_t msgLen = 0;
    char *p = 0;
    apr_size_t pos = 0;
    int nFields = 0;
    const char *name;
    const char *value;

    //Determine the space needed
    for (HeaderIterator I = map.begin(); I != map.end(); ++I)
    {
        name = I->first.c_str();
        value = I->second.c_str();
        msgLen += strlen(name) + strlen(value);
        nFields++;
    }

    msgLen += 4 * nFields + 2 + 3;
    
    p = new char[msgLen+1];
    *msg = p;
    // Check fo rNULL;


    jxr_msg_init(p, &pos, type);
    jxr_msg_append_int16(p, &pos, nFields);

    for (HeaderIterator I = map.begin(); I != map.end(); ++I)
    {
        name = I->first.c_str();
        jxr_msg_append_string(p, &pos, name);

        value = I->second.c_str();
        jxr_msg_append_string(p, &pos, value);
    }
    jxr_msg_end(p, &pos);
}

BOOL BuildHTTPHeaderMsg(EXTENSION_CONTROL_BLOCK *pECB, char** sHeaderMsg)
{
    char buf[0x8000];
    DWORD n = 0x8000;
    char *name;
    char* value;
    int ret = (pECB->GetServerVariable)(pECB->ConnID, "ALL_RAW", buf, &n);
    //check ret
    //parse <name: value>[\r\n<name: Value>]

    HeaderMap headers;

    name = strtok (buf, "\n\r");
    while (name != NULL)
    {
        value = strchr(name, ':');
        *value++ = 0;
        while(*value==' ') *value++=0;

        headers[name] = value;
        name = strtok(NULL, "\n\r");
    }

    BuildHeaderMessage(headers, BLOCKTYPE_HTTP_HEADER, sHeaderMsg);
    return TRUE;
}

BOOL BuildHTTPHeaderMsg(HTTP_FILTER_CONTEXT *pECB, char** sHeaderMsg)
{
    char buf[0x8000];
    DWORD n = 0x8000;
    char *name;
    char* value;
    int ret = (pECB->GetServerVariable)(pECB, "ALL_RAW", buf, &n);
    //check ret
    //parse <name: value>[\r\n<name: Value>]

    HeaderMap headers;

    name = strtok (buf, "\n\r");
    while (name != NULL)
    {
        value = strchr(name, ':');
        *value++ = 0;
        while(*value==' ') *value++=0;

        headers[name] = value;
        name = strtok(NULL, "\n\r");
    }

    BuildHeaderMessage(headers, BLOCKTYPE_HTTP_HEADER, sHeaderMsg);
    return TRUE;
}

BOOL RestoreURL(char* sUrl)
{
#if 0
    // anything</jaxer-server>something<jaxer.jaxer> ==></jaxer-server>something
    // anything</jaxer-server>something<jaxer.jaxer>?qs ==></jaxer-server>something?qs
    char *p = strstr(sUrl, sJAXER_HANDLES);
    if (!p)
    {
        return FALSE;
    }
    strcpy(sUrl, p);
#endif

    char* p = strstr(sUrl, "jaxer.jaxer?");
    if (!p)
    {
        p = strstr(sUrl + strlen(sUrl) - strlen("jaxer.jaxer"), "jaxer.jaxer");
        if (!p)
        {
            // Error
            return FALSE;
        }
        *p = 0;
    }else
    {
        strcpy(p, p+strlen("jaxer.jaxer"));
    }
    return TRUE;
}


BOOL BuildEnvHeaderMsg(EXTENSION_CONTROL_BLOCK *pECB, char**sEnvMsg)
{
    static char *EnvVars[] =
    {
        "HTTPS", "REQUEST_METHOD",  "REMOTE_USER", "REMOTE_PORT", "REMOTE_HOST",
        "QUERY_STRING", "CONTENT_TYPE", "HTTP_HOST", "SERVER_PROTOCOL",
        0
    };
    
    // STATUS_CODE, DOCUMENT_ROOT,
    int i=0;
    char value[0x8000];
    // char script_filename[MAX_PATH];
    DWORD n;
    int ret;
    
    HeaderMap headers;
    //STATUS_CODE
    sprintf(value, "%d", pECB->dwHttpStatusCode);
    headers["STATUS_CODE"] = value;

    //REQUEST_URL: TODO
    n = 0x8000;
    value[0] = 0;
    ret = (pECB->GetServerVariable)(pECB->ConnID, "HTTP_URL", value, &n);
    if (!RestoreURL(value))
    {
        return FALSE;
    }
    headers["REQUEST_URI"] = value; // "/jaxer-server/callback";

    
    //REMOTE_ADDR
    n = 0x8000;
    value[0] = 0;
    ret = (pECB->GetServerVariable)(pECB->ConnID, "REMOTE_ADDR", value, &n);
    if (n > 0)
    {
        headers["REMOTE_ADDR"] = value;
    }else
    {
        n = 0x8000;
        ret = (pECB->GetServerVariable)(pECB->ConnID, "REMOTE_HOST", value, &n);
        if (n > 0)
        {
            headers["REMOTE_ADDR"] = value;
        }
    }

    //DOCUMENT_ROOT
#if 0
    n = 0x8000;
    value[0] = 0;
    ret = (pECB->GetServerVariable)(pECB, "APPL_PHYSICAL_PATH", value, &n);
    char* p = value;
    while (p && *p)
    {
        if (*p == '\\')
            *p = '/';
        p++;
    }
#endif
    headers["DOCUMENT_ROOT"] = GetJaxerConfig().GetVirtualDirPath();


    //SCRIPT_FILENAME
    strcpy(value, pECB->lpszPathTranslated);
    char * p = value;
    while (p && *p)
    {
        if (*p == '\\')
            *p = '/';
        p++;
    }

    if (!RestoreURL(value))
    {
        return FALSE;
    }
    headers["SCRIPT_FILENAME"] = value;

    //SCRIPT_NAME
    n = 0x8000;
    value[0] = 0;
    (pECB->GetServerVariable)(pECB->ConnID, "SCRIPT_NAME", value, &n);
    if (!RestoreURL(value))
    {
        return FALSE;
    }
    headers["SCRIPT_NAME"] = value;
        
    while (EnvVars[i] != 0)
    {
        n = 0x8000;
        (pECB->GetServerVariable)(pECB->ConnID, EnvVars[i], value, &n);
        headers[EnvVars[i]] = value;
        i++;
    }
    
    BuildHeaderMessage(headers, BLOCKTYPE_ENVIRO_VARS, sEnvMsg);
    return TRUE;
}

BOOL BuildEnvHeaderMsg(HTTP_FILTER_CONTEXT *pECB, char** EnvMsg)
{
    static char *EnvVars[] =
    {
        "HTTPS", "REQUEST_METHOD",  "REMOTE_USER", "REMOTE_PORT", "REMOTE_HOST",
        "QUERY_STRING", "CONTENT_TYPE", "HTTP_HOST", "SERVER_PROTOCOL", 
        "SCRIPT_NAME", 0
    };

    // STATUS_CODE, DOCUMENT_ROOT, SCRIPT_FILENAME
    int i=0;
    char value[0x8000];
    DWORD n;
    int ret;
    
    fctx* ctx = (fctx*) pECB->pFilterContext;
    HeaderMap& headers = ctx->envMap;

    //HTTPS
    n = 0x8000;
    value[0] = 0;
    ret = (pECB->GetServerVariable)(pECB, "HTTPS", value, &n);
    if (stricmp("on", value) == 0)
    {
        // We cannot handle postdata
    }
    

    //REQUEST_URI
    n = 0x8000;
    ret = (pECB->GetServerVariable)(pECB, "HTTP_URL", value, &n);
    headers["REQUEST_URI"] = value;

    //REMOTE_ADDR
    n = 0x8000;
    ret = (pECB->GetServerVariable)(pECB, "REMOTE_ADDR", value, &n);
    if (n > 0)
    {
        headers["REMOTE_ADDR"] = value;
    }else
    {
        n = 0x8000;
        ret = (pECB->GetServerVariable)(pECB, "REMOTE_HOST", value, &n);
        if (n > 0)
        {
            headers["REMOTE_ADDR"] = value;
        }
    }
    

    //DOCUMENT_ROOT

    headers["DOCUMENT_ROOT"] = GetJaxerConfig().GetVirtualDirPath();


    
    while (EnvVars[i] != 0)
    {
        n = 0x8000;
        (pECB->GetServerVariable)(pECB, EnvVars[i], value, &n);
        if (n>0)
        {
            headers[EnvVars[i]] = value;
        }
        i++;
    }
    
    // Add the ones in ctx

    BuildHeaderMessage(headers, BLOCKTYPE_ENVIRO_VARS, EnvMsg);
    return TRUE;
}


BOOL SendPostData(EXTENSION_CONTROL_BLOCK *pECB, CJaxerConnection* pc)
{
    char buf[0x8000];
    
    GetJaxerLog().Log(eDEBUG, "Handler:SendPostData:");

    if (stricmp("post", pECB->lpszMethod))
    {
        GetJaxerLog().Log(eDEBUG, "Handler:SendPostData: No post data sending empty block");
        // No post data, just send empty data block
        char pd[1];
        if (!pc->SendPostDataFrag(pd, 0))
        {
            GetJaxerLog().Log(eERROR, "Handler:SendPostData: FAILED sending empty block");
            return FALSE;
        }

    }else
    {
         
        // we have post data.
        // 1.  Send the available data first.
        // 2.  If there is still more, then read them and send off
        
        //first consume the available data and then  read more
        DWORD dwSize = 0x8000;
        DWORD dwTotal = pECB->cbTotalBytes;
        DWORD dwSend = 0;
        DWORD dwAvil = pECB->cbAvailable;
        
        GetJaxerLog().Log(eDEBUG, "Handler:SendPostData: Start sending postdata tot=%d avl=%d", dwTotal, dwAvil);
        while (dwSend < dwAvil)
        {
            dwSize = (dwAvil-dwSend) > 0x8000 ? 0x8000 : (dwAvil-dwSend);
            memcpy(buf, pECB->lpbData + dwSend, dwSize);

            GetJaxerLog().Log(eDEBUG, "Handler:SendPostData: Sending post data len=%d", dwSize);
            GetJaxerLog().LogStr(eTRACE, buf, dwSize);
            if (!pc->SendPostDataFrag(buf, dwSize))
            {
                GetJaxerLog().Log(eERROR, "Failed to send post data frag");
                return FALSE;
            }
            dwSend += dwSize;
        }
        
        dwTotal -= dwAvil;
        GetJaxerLog().Log(eDEBUG, "Handler:SendPostData: More data needs to be read %d", dwTotal);

        while (dwTotal>0)
        {
            // Read in 8k chunks
            dwSize = 0x8000;
            int ret = pECB->ReadClient(pECB->ConnID, buf, &dwSize);

            GetJaxerLog().Log(eDEBUG, "Handler:SendPostData: Read ret=%d size=%d", dwTotal, dwSize);

            GetJaxerLog().Log(eDEBUG, "Handler:SendPostData: Sending Postdata size=%d", dwSize);
            GetJaxerLog().LogStr(eTRACE, buf, dwSize);

            //TODO: Check ret
            if (! pc->SendPostDataFrag(buf, dwSize))
            {
                GetJaxerLog().Log(eERROR, "Failed to send post data fragment");
                return FALSE;
            }

            dwTotal -= dwSize;
            if (dwSize == 0)
            {

                GetJaxerLog().Log(eWARN, "Handler:SendPostData: dwSize=0 ODD");//?
                // break;
            }
        }

        GetJaxerLog().Log(eDEBUG, "Handler:SendPostData: Sending EndReq");
        if (! pc->SendEndRequest())
        {
            GetJaxerLog().Log(eERROR, "Failed to send end req after postdata");
            return FALSE;
        }
    }

    return TRUE;
}

BOOL ProcessResponseHeaderMsg(EXTENSION_CONTROL_BLOCK* pECB, char* msg)
{
    BOOL rv = FALSE;
	
	apr_size_t pos;

	char name[MAX_STRING_LEN];
	char value[MAX_STRING_LEN];
    char headers[0x8000];
    BOOL bKeepAlive = TRUE;

	char *w, *l;
	
	char type = jxr_msg_get_type(msg);
	size_t len = jxr_msg_get_length(msg,  &pos);

	if (type != BLOCKTYPE_HTTP_HEADER)
	{
		// Invalid data
        GetJaxerLog().Log(eDEBUG, "ProcessResponseHeaderMsg:invalid data type (%c) received, while expecting a header (%d)", type, BLOCKTYPE_HTTP_HEADER);
        return rv;
	}


    char Status[100] = "200 OK";
    int nStatus = 200;

    headers[0] = 0;
	int nHeaders = jxr_msg_get_int16(msg, &pos);
	for (int i=0; i<nHeaders; i++)
	{
		// Process one header -- name -- val
		jxr_msg_get_string(msg, &pos, name);
		jxr_msg_get_string(msg, &pos, value);

		w = name;
		l = value;

		
        if (!stricmp(w, "Status"))
        {
		    /*
             * If the server returned a specific status, that's what
             * we'll use - otherwise we assume 200 OK.
             */
        
            strcpy(Status, l);
            nStatus = atoi(Status);
        }else
        {
            char *endp = l + strlen(l) - 1;
            while (endp > l && isspace(*endp))
            {
                *endp-- = '\0';
            }
            strcat (headers, w);
            strcat (headers, ": ");
            strcat (headers, l);
            strcat (headers, "\r\n");
        }

	}
    strcat(headers, "\r\n");

    HSE_SEND_HEADER_EX_INFO HeaderExInfo;  

    HeaderExInfo;
    HeaderExInfo.pszHeader = headers;  
    HeaderExInfo.cchHeader = (DWORD)strlen(headers);  
    HeaderExInfo.pszStatus = Status;  
    HeaderExInfo.cchStatus = (DWORD)strlen(Status);  
    HeaderExInfo.fKeepConn = bKeepAlive; 

    if(! pECB->ServerSupportFunction ( pECB->ConnID,  
        HSE_REQ_SEND_RESPONSE_HEADER_EX, &HeaderExInfo,  
        NULL, NULL ))
    {
        DWORD err = GetLastError();
        GetJaxerLog().Log(eERROR, "ProcessResponseHeaderMsg: ServerSupportFunction failed with error=%d", err);
        return FALSE;
    }

    return TRUE;
}

BOOL RecvAndProcessDocument(EXTENSION_CONTROL_BLOCK* pECB, CJaxerConnection* pc)
{
    //Read body from jaxer and plug them into pECB
    char *msg;
    size_t pos;
    BOOL ret = FALSE;

    GetJaxerLog().Log(eDEBUG, "Handler:RecvAndProcessDocument");

    while (pc->RecvMsg(&msg))
    {
        char type = jxr_msg_get_type(msg);
        DWORD len = (DWORD)jxr_msg_get_length(msg,  &pos);
        GetJaxerLog().Log(eDEBUG, "Handler:RecvAndProcessDocument type=%d len=%d", type, len);

	    if (type == BLOCKTYPE_ENDREQUEST)
	    {
            GetJaxerLog().Log(eDEBUG, "Handler:RecvAndProcessDocument EndReq");
            ret = TRUE;
		    break;
	    }else if ( type != BLOCKTYPE_DOCUMENT)
        {
            //invalid type
            GetJaxerLog().Log(eERROR, "RecvAndProcessDocument: got non-doc block %d (doc=%d)", type, BLOCKTYPE_DOCUMENT);
            break;
        }

        GetJaxerLog().Log(eDEBUG, "Handler:RecvAndProcessDocument Write to client %d", len);
        GetJaxerLog().LogStr(eTRACE, msg+sizeof(Jaxer_Header), len);
        if (!pECB->WriteClient (pECB->ConnID, msg+sizeof(Jaxer_Header), &len, HSE_IO_SYNC))
        {
            DWORD err = GetLastError();
            GetJaxerLog().Log(eERROR, "RecvAndProcessDocument: WriteClient failed with error %d", err);
            break;
        }
        delete[] msg;
        msg = 0;
    }

    delete[] msg;
    GetJaxerLog().Log(eDEBUG, "Handler:RecvAndProcessDocument--DONE");

    return ret;
}



BOOL RecvAndProcessDocument(HTTP_FILTER_CONTEXT* pECB)
{
    //Read body from jaxer and plug them into pECB
    char *msg;
    size_t pos;
    BOOL ret = FALSE;
    
    fctx *ctx = (fctx*) pECB->pFilterContext;
    CJaxerConnection *pc = ctx->c;
    
    GetJaxerLog().LogStr(eDEBUG, "Filter:RecvAndProcessDocument");

    while (pc->RecvMsg(&msg))
    {
        char type = jxr_msg_get_type(msg);
        DWORD len = (DWORD)jxr_msg_get_length(msg,  &pos);
        GetJaxerLog().Log(eDEBUG, "Filter:RecvAndProcessDocument type=%d len=%d", type, len);

	    if (type == BLOCKTYPE_ENDREQUEST)
	    {
            GetJaxerLog().LogStr(eDEBUG, "Filter:RecvAndProcessDocument EndReq");
            ret = TRUE;
		    break;
	    }else if ( type != BLOCKTYPE_DOCUMENT)
        {
            //invalid type
            GetJaxerLog().Log(eERROR, "Filter:RecvAndProcessDocument(flt): got non-doc block %d (doc=%d)", type, BLOCKTYPE_DOCUMENT);
            break;
        }

        GetJaxerLog().Log(eDEBUG, "Filter:RecvAndProcessDocument Write to client %d", len);
        GetJaxerLog().LogStr(eTRACE, msg+sizeof(Jaxer_Header), len);
        
        if (!pECB->WriteClient (pECB, msg+sizeof(Jaxer_Header), &len, HSE_IO_SYNC))
        {
            DWORD err = GetLastError();
            GetJaxerLog().Log(eERROR, "RecvAndProcessDocument(flt): WriteClient failed with error %d", err);
            
            break;
        }
        GetJaxerLog().Log(eDEBUG, "Filter:RecvAndProcessDocument Written to client %d", len);
        delete[] msg;
        msg = 0;
    }

    delete[] msg;

    GetJaxerLog().Log(eDEBUG, "Filter:RecvAndProcessDocument--DONE");
    return ret;
}

BOOL ProcessResponseHeaderMsg(HTTP_FILTER_CONTEXT* pECB, char* msg)
{
    BOOL rv = FALSE;
	
	apr_size_t pos;

	char name[MAX_STRING_LEN];
	char value[MAX_STRING_LEN];
    char headers[0x8000];
    BOOL bKeepAlive = TRUE;
    
    char *w, *l;

    fctx* ctx = (fctx*) pECB->pFilterContext;
	
	char type = jxr_msg_get_type(msg);
	size_t len = jxr_msg_get_length(msg,  &pos);

	if (type != BLOCKTYPE_HTTP_HEADER)
	{
		GetJaxerLog().Log(eERROR, "ProcessResponseHeaderMsg(flt): got non-header block %d (hdr=%d)", type, BLOCKTYPE_HTTP_HEADER);
        return rv;
	}


    char Status[100] = "200 OK";
    int nStatus = 200;

    // char *Connection = "Connection:Keep-Alive\r\n";

    headers[0] = 0;
	int nHeaders = jxr_msg_get_int16(msg, &pos);
	for (int i=0; i<nHeaders; i++)
	{
		// Process one header -- name -- val
		jxr_msg_get_string(msg, &pos, name);
		jxr_msg_get_string(msg, &pos, value);

		w = name;
		l = value;

		
        if (!stricmp(w, "Status"))
        {
		    /*
             * If the server returned a specific status, that's what
             * we'll use - otherwise we assume 200 OK.
             */
        
            strcpy(Status, l);
            nStatus = atoi(Status);
        }else
        {
            char *endp = l + strlen(l) - 1;
            while (endp > l && isspace(*endp))
            {
                *endp-- = '\0';
            }
            ctx->RespHeaders[w] = l;
#if 0
            strcat (headers, w);
            strcat (headers, ": ");
            strcat (headers, l);
            strcat (headers, "\r\n");
#endif
        }

	}

    // construct headers
    sprintf(headers, "HTTP/1.1 %s\r\n", Status);
    for (HeaderIterator I = (ctx->RespHeaders).begin(); I != (ctx->RespHeaders).end(); ++I)
    {
        const char* name = I->first.c_str();
        const char* value = I->second.c_str();
        strcat(headers, name);
        strcat(headers, ": ");
        strcat(headers, value);
        strcat(headers, "\r\n");
    }
    strcat(headers, "\r\n");
    DWORD dwLen = (DWORD)strlen(headers);
    if (! pECB->WriteClient(pECB, headers, &dwLen, HSE_IO_SYNC))
    {
        return FALSE;
    }


#if 0
    strcat (headers, Connection);

    strcat(headers, "\r\n");
    DWORD dwReserved = 0;
    if(! pECB->AddResponseHeaders (pECB, headers, dwReserved))
    {
        DWORD err = GetLastError();
        GetJaxerLog().Log(eERROR, "ProcessResponseHeaderMsg(flt): AddResponseHeaders return err %d", err);
        GetJaxerLog().Log(logbuf);
       
        return FALSE;
    }


    HSE_SEND_HEADER_EX_INFO HeaderExInfo;  

    HeaderExInfo;
    HeaderExInfo.pszHeader = headers;  
    HeaderExInfo.cchHeader = strlen(headers );  
    HeaderExInfo.pszStatus = Status;  
    HeaderExInfo.cchStatus = strlen(Status );  
    HeaderExInfo.fKeepConn = bKeepAlive; 

    if(! pECB->ServerSupportFunction ( pECB->ConnID,  
        HSE_REQ_SEND_RESPONSE_HEADER_EX, &HeaderExInfo,  
        NULL, NULL ))
    {
        DWORD err = GetLastError();
        return FALSE;
    }
#endif


    return TRUE;
}


BOOL SendPostData(HTTP_FILTER_CONTEXT *pFC)
{
    fctx *ctx = (fctx*) pFC->pFilterContext;

    if (ctx->postdata_sent)
    {
        GetJaxerLog().Log(eERROR, "SendPostData(flt): asking post data after sent");
        return FALSE;
    }

    if (ctx->postData && ctx->postdata_true_len == ctx->postdata_tot_len)
    {
        // Send each post data block
        DataList * pHeader = ctx->postData;
        while(pHeader)
        {
            GetJaxerLog().Log(eDEBUG, "Filter:SendPostData len=%d", pHeader->len);
            GetJaxerLog().LogStr(eTRACE, pHeader->data, pHeader->len);

            if (! ctx->c->SendPostDataFrag(pHeader->data, pHeader->len))
            {
                GetJaxerLog().Log(eERROR, "SendPostData(flt): SendPostDataFrag failed");
                return FALSE;
            }
            pHeader = pHeader->next;
        }
        delete ctx->postData;
        ctx->postData = 0;

        GetJaxerLog().LogStr(eDEBUG, "Filter:SendPostData Sending EndReq");
        // Mark the end of PostData
        if (! ctx->c->SendEndRequest())
        {
            GetJaxerLog().Log(eERROR, "SendPostData(flt): SendEndRequest failed");
                
            return FALSE;
        }

    }else
    {
        GetJaxerLog().Log(eWARN, "Filter:SendPostData Do not have entire data.  PostData dropped: true_len=%d tot_len=%d",
            ctx->postdata_true_len, ctx->postdata_tot_len);
        if (ctx->postData)
        {
            delete ctx->postData;
            ctx->postData = 0;
        }

        // No postdata.  No need for EOR?
        GetJaxerLog().LogStr(eDEBUG, "Filter:SendPostData Sending empty post doc");
        char doc[1];
        if (! ctx->c->SendPostDataFrag(doc, 0))
        {
            GetJaxerLog().Log(eERROR, "SendPostData(flt): Send empty PostDataFrag failed");
            return FALSE;
        }
    }
    ctx->postdata_sent = true;

    return TRUE;
}


BOOL OnEndOfRequest(HTTP_FILTER_CONTEXT* pECB)
{
    fctx *ctx = (fctx*) pECB->pFilterContext;
    char *msg = 0;
    ctx->stage = 2;
    
    BOOL ret = FALSE;

    if (ctx->has_error)
        goto cleanup;

    if (ctx->skip)
    {
        ret = TRUE;
        goto cleanup;
    }

    if (! ctx->c->SendEndRequest())
    {
        GetJaxerLog().Log(eERROR, "OnEndOfRequest(flt): SendEndRequest failed");
        goto cleanup;
    }

    if (! ctx->c->RecvMsg(&msg))
    {
        GetJaxerLog().Log(eERROR, "OnEndOfRequest(flt): RecvMsg (post or hdr) failed");
        
        goto cleanup;
    }

    // This might be a request for postdata.
    // Last chance for postdata
    char msg_type = jxr_msg_get_type(msg);
    if (!ctx->postdata_sent && msg_type == BLOCKTYPE_REQUEST_POSTDATA)
    {
        
        if (!SendPostData(pECB))
        {
            GetJaxerLog().Log(eERROR, "OnEndOfRequest(flt): SendPostData failed");
        
            goto cleanup;
        }
        if (! ctx->c->SendEndRequest())
        {
            GetJaxerLog().Log(eERROR, "OnEndOfRequest(flt): SendEndRequest (after postdata) failed");
            goto cleanup;
        }
        if (! ctx->c->RecvMsg(&msg))
        {
            GetJaxerLog().Log(eERROR, "OnEndOfRequest(flt): RecvMsg (hdr) failed");
            goto cleanup;
        }
    }

    if (!ProcessResponseHeaderMsg(pECB, msg))
    {
        GetJaxerLog().Log(eERROR, "OnEndOfRequest(flt): ProcessResponseHeaderMsg failed");
        goto cleanup;
    }

    if (!RecvAndProcessDocument(pECB))
    {
        GetJaxerLog().Log(eERROR, "OnEndOfRequest(flt): RecvAndProcessDocument failed");
        goto cleanup;
    }

    ret = TRUE;



cleanup:
    ctx->has_error = (!ret);
    delete ctx;
    ctx = 0;
    return ret;
}

BOOL SendDocFragToJaxer(HTTP_FILTER_CONTEXT *pFC, const char *pData, DWORD MsgLen)
{
    DWORD nSend = 0;
    char *msg;
    fctx* ctx = (fctx*) pFC->pFilterContext;
    
    while (nSend < MsgLen)
    {
        // Need to check if it is asking for postdata
        if (!ctx->postdata_sent)
        {
            if (! ctx->c->RecevMsgIfAny(&msg))
            {
                GetJaxerLog().Log(eERROR, "OnSendRawdata: RecevMsgIfAny(post) failed");
                return FALSE;
            }
            if (msg && msg[0])
            {
                // This has to be a request for postdata msg
                char msg_type = jxr_msg_get_type(msg);
		        if (msg_type == BLOCKTYPE_REQUEST_POSTDATA)
                {
                    if (!SendPostData(pFC))
                    {
                        GetJaxerLog().Log(eERROR, "OnSendRawdata: SendPostData failed");
                        return FALSE;
                    }
                }else
                {
                    // error
                    GetJaxerLog().Log(eERROR, "OnSendRawdata: expecting post msg (%d) but got %d", BLOCKTYPE_REQUEST_POSTDATA, msg_type);
                    return FALSE;
                }

            }
        }

        DWORD n = (MsgLen - nSend) > 0XFFF0 ? 0XFFF0 : (MsgLen - nSend); //leave some space for header

        GetJaxerLog().Log(eDEBUG, "Filter:OnSendRawdata: Sending Doc len=%d", n);
        GetJaxerLog().LogStr(eTRACE, pData+nSend, n);

        if (! ctx->c->SendDocumentFrag(pData+nSend, n))
        {
            GetJaxerLog().Log(eERROR, "OnSendRawdata: SendDocumentFrag (%d) failed", n);
            return FALSE;
        }
        nSend += n;  
    }
    return TRUE;
}

BOOL OnSendRawdata(HTTP_FILTER_CONTEXT *pFC, PHTTP_FILTER_RAW_DATA pRawData)
{
    char *msg;
    fctx* ctx = (fctx*) pFC->pFilterContext;
    if (!ctx)
    {
        GetJaxerLog().Log(eDEBUG, "OnSendRawdata: ctx is null");
        return FALSE;
    }
    if (ctx->skip)
        return TRUE;

    if (ctx->stage == 0)
    {
        /* It is possible that we may not want to filter this request based on conten type.
         * So let's check that first before sending anything to jaxer
         */

        // The 1st block is header.  Let's keep this
        char * pData = (char*) pRawData->pvInData;
        DWORD dwLen = pRawData->cbInData;
        char * pEnd = strstr(pData, "\r\n\r\n");

        // I assume all headers are here.  Otherwise, this will not work.
        if (!pEnd)
        {
            GetJaxerLog().Log(eERROR, "Header is incomplete.  This ISAPI assumes all headers are in one block.");
            return FALSE;
        }
        pEnd += 2;

        char *pStt = pData;
        // Get the first line
        char *name = pStt;
        char *end = strstr(name, "\r\n");
        *end = 0;
        // name = HTTP/1.1 200 OK
        char *value = strchr(name, ' ');
        *value++ = 0;
        int nStatusCode = atoi(value);
        // Make sure it is OK
        pStt = end + 2;
        
        while (pStt < pEnd)
        {
            name = pStt;
            end = strstr(name, "\r\n");
            *end = 0;
            value = strchr(name, ' ');
            value[-1] = 0;
            *value++ = 0;
            pStt = end + 2;
            ctx->RespHeaders[name] = value;
        }
        pEnd += 2;

        /* Now we have parsed all headers.*/
        if (ctx->skip2 == false)
        {
            GetJaxerLog().Log(eDEBUG, "Check to see if we should filter based on content_type.");
            // Request does not have ext.  check ct
            HeaderIterator iter = (ctx->RespHeaders).find("Content-Type");
            if (iter != (ctx->RespHeaders).end())
            {
                const char* ct = iter->second.c_str();
                if (ct && *ct)
                {
                    if(!GetJaxerConfig().ShouldFilterCT(ct))
                    {
                        ctx->skip = ctx->skip2 = true;
                        GetJaxerLog().Log(eDEBUG, "Bypass Jaxer filtering based on content_type filtering.");
                        return TRUE;
                    }else
                    {
                        GetJaxerLog().Log(eDEBUG, "Sending to Jaxer based on content_type filtering.");
                    }
                }else
                {
                    GetJaxerLog().Log(eINFO, "Response has null or empty Content-Type, will not send to jaxer.");
                    ctx->skip = ctx->skip2 = true;
                    return TRUE;
                }

            }else
            {
                GetJaxerLog().Log(eINFO, "Response does not have Content-Type, will not send to jaxer.");
                ctx->skip = ctx->skip2 = true;
                return TRUE;
            }
        }


        CJCPool& jcp = GetJCPool();
        ctx->c = jcp.GetConnection();
        if (!ctx->c)
        {
            GetJaxerLog().Log(eERROR, "OnSendRawdata: failed to obtain a connection to Jaxer");
            return FALSE;
        }

        // Build and send header & env
        
        if (! BuildHTTPHeaderMsg (pFC, &msg))
        {
            ctx->has_error = true;
            GetJaxerLog().Log(eERROR, "OnSendRawdata: BuildHTTPHeaderMsg failed");
            return FALSE;
        }
        if (! ctx->c->SendMsg(msg))
        {
            GetJaxerLog().Log(eERROR, "OnSendRawdata: send msg (hdr)");
            delete[] msg;
            return FALSE;
        }
        delete[] msg;

        if (! BuildEnvHeaderMsg(pFC, &msg))
        {
            GetJaxerLog().Log(eERROR, "OnSendRawdata: BuildEnvHeaderMsg failed");
            return FALSE;
        }
        if (! ctx->c->SendMsg(msg))
        {
            GetJaxerLog().Log(eERROR, "OnSendRawdata: send msg (env)");
            delete[] msg;
            return FALSE;
        }
        delete[] msg;

        
        if (dwLen > (DWORD)(pEnd - pData))
        {
            // we have data following header
            //TODO send it to Jaxer
            if (! SendDocFragToJaxer(pFC, pEnd, dwLen - (pEnd - pData)))
            {
                return FALSE;
            }
        }
        
        ctx->stage = 1;
    }else if (ctx->stage == 1)
    {

        // Now the document is on its way to be sent to jaxer
        char * pData = (char*) pRawData->pvInData;
        DWORD MsgLen = pRawData->cbInData;
        if (! SendDocFragToJaxer(pFC, pData, MsgLen))
        {
            return FALSE;
        }
        // We cannot use this to determine the end of the data
        // ctx->doc_len -= MsgLen;
    }

    if (ctx->stage != 2)
    {
        // Now erase the data
        pRawData->cbInData = 0;
        pRawData->cbInBuffer = 0;
        pRawData->cbInData = 0;
    }


    return TRUE;
}

BOOL OnSendResponse(HTTP_FILTER_CONTEXT* pFC, PHTTP_FILTER_SEND_RESPONSE pVN)
{
    fctx* ctx = (fctx*) pFC->pFilterContext;

    ctx->status = pVN->HttpStatus;
    if (ctx->status ==204 || ctx->status >= 300)
    {
        ctx->skip = ctx->skip2 = true;
        GetJaxerLog().Log(eERROR, "OnSendResponse: skip %d", ctx->status);
        return TRUE;
    }

    if (ctx->skip)
    {
        return TRUE;
    }

    GetJaxerLog().Log(eDEBUG, "Filter will handle this request (status=%d)", ctx->status);

    return TRUE;
}

BOOL OnNotifyUrlMap(HTTP_FILTER_CONTEXT* pFC, PHTTP_FILTER_URL_MAP pUMData)
{
        fctx* ctx = (fctx*) pFC->pFilterContext;
        if (!ctx || (ctx->skip && ctx->skip2))
            return TRUE;

        char value[MAX_PATH];
        
        strcpy(value, pUMData->pszPhysicalPath);

#if 0
        if (ctx->skip)
        {
            // We may want to skip this, but have to check if it is dir
            GetJaxerLog().Log(eDEBUG, "Checking to see if we should handle \"%s\"", value);
            struct _stat buf;
            if( 0 == _stat(value, &buf))
            {
                if (buf.st_mode & _S_IFDIR)
                {
                    ctx->skip = ctx->skip2 = false;
                    GetJaxerLog().Log(eDEBUG, "Filter DIR \"%s\"", value);
                }
            }else
            {
                DWORD err = GetLastError();
                if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
                {
                    ctx->skip = ctx->skip2 = false;
                    GetJaxerLog().Log(eDEBUG, "Filter dir \"%s\"", value);
                }else
                {
                    GetJaxerLog().Log(eWARN, "Skipping request: Stat failed on \"%s\". error=%d", value, err);
                }
            }
        }

        if (ctx->skip)
            return TRUE;

#endif


        char *sfn = value;
        while (*sfn)
        {
            if (*sfn == '\\') *sfn = '/';
            sfn++;
        }

        ctx->envMap["SCRIPT_FILENAME"] = value;

        GetJaxerLog().Log(eDEBUG, "Request: %s", pUMData->pszURL);


        // If it is POST, find the post data length
        DWORD n = MAX_PATH;
        value[0] = 0;
        if ((pFC->GetServerVariable)(pFC, "REQUEST_METHOD", value, &n) &&
            stricmp("POST", value) == 0)
        {
            value[0] = 0;
            n = MAX_PATH;
            if ((pFC->GetServerVariable)(pFC, "CONTENT_LENGTH", value, &n))
            {
                ctx->postdata_tot_len = atoi(value);
            }
        }


        return TRUE;
        
}

BOOL RequestShouldBeHandledByJaxer(char* sUrl)
{
    char *p = sUrl;
    while(*p)
        tolower(*p++);

#define STRING2(x) #x
#define STRING(x) STRING2(x)

#pragma message(__FILE__ "[" STRING(__LINE__) "]: The following section is commented out because jaxer issued hardcoded /jaxer-server")
#if 0 
    // We should not handle it if it is not within our root
    if(!GetJaxerConfig().IsURLMine(sUrl))
        return FALSE;
#endif

    char *q = strstr(sUrl, sJAXER_HANDLES);
    if (q )
    {
        char c = q[strlen(sJAXER_HANDLES)];
        if (c == '/' || c == '?' || c == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL ShouldWeFilterTheRequest(char* sUrl, fctx* ctx)
{
    char *p = sUrl;
    char c;
    while(*p)
        tolower(*p++);

    // We should not handle it if it is not within our root
    if(!GetJaxerConfig().IsURLMine(sUrl))
        return FALSE;


    // If we handle this, we should not filter it.
    char *q = strstr(sUrl, sJAXER_HANDLES);
    if (q )
    {
        c = q[strlen(sJAXER_HANDLES)];
        if (c == '/' || c == '?' || c == 0)
        {
            return FALSE;
        }
    }

    // We should not handle it if it is bypass
    q = strstr(sUrl, "/jaxer-include");
    if (q && (q[strlen("/jaxer-include")] == '/' || q[strlen("/jaxer-include")] == '?' ||
        q[strlen("/jaxer-include")] == '\0'))
    {
        return FALSE;
    }

    
    q = strchr(sUrl, '?');
    if (q && *q)
    {
        *q = 0;
    }

    //Find the part past the last slash
    q = strrchr(sUrl, '/');
    if (q)
    {
        q++;
    }

    p = strrchr(q, '.');
    if (p)
    {
        return GetJaxerConfig().ShouldFilterExt(p);
    }else
    {
        //This is based on content type.  We have to figure that out later.
        ctx->skip2 = false;
    }
    return FALSE;
}

// const char *sVirtualRoot = "/jaxer";
BOOL OnPreprocHeaders(HTTP_FILTER_CONTEXT* pFC, PHTTP_FILTER_PREPROC_HEADERS pVN)
{
    fctx* ctx =  new fctx;
    pFC->pFilterContext = ctx;
    

    
    //This is the place we check to see if we should be the handler.
    char sUrl[0x8000];
    DWORD len = 0x8000;

    if (!pVN->GetHeader(pFC, "url", sUrl, &len))
    {
        DWORD err = GetLastError();
        GetJaxerLog().Log(eERROR, "Failed to get URL header %d", err);
        return FALSE;
    }
    GetJaxerLog().Log(eDEBUG, "Filter:OnPreprocHeaders: %s", sUrl);


    // This changes URL
    if (RequestShouldBeHandledByJaxer(sUrl))
    {
        GetJaxerLog().LogStr(eDEBUG, "Request will be handled by Jaxer");
        ctx->skip = true;

        // append sJAXER_EXT to URL
        char *q = strchr(sUrl, '?');
        char sQuery[MAX_PATH];
        sQuery[0] = 0;
        if (q)
        {
            strcpy(sQuery, q);
            *q = 0;
        }

        strcat(sUrl, sJAXER_EXT);
        strcat(sUrl, sQuery);

        // prefix virtual root
        strcpy(sQuery, sUrl);
        strcpy(sUrl, GetJaxerConfig().GetVirtualDirName());
        strcat(sUrl, sQuery);

        GetJaxerLog().Log(eDEBUG, "URL changed to %s", sUrl);
        if (!pVN->SetHeader(pFC, "url", sUrl))
        {
            GetJaxerLog().LogStr(eERROR, "Failed to get URL header");
            return FALSE;
        }
    }else if (! ShouldWeFilterTheRequest(sUrl, ctx))
    {
        ctx->skip = true;
        if (ctx->skip2==true)
        {
            GetJaxerLog().Log(eDEBUG, "Filter:OnPreprocHeaders: Request (%s) will NOT be handled by filter based on extension",
                sUrl);
        }else
        {
            GetJaxerLog().Log(eDEBUG, "Filter:OnPreprocHeaders: Request (%s) does not have extension.  Not sure if Jaxer will handle for now",
                sUrl);
        }
    }else
    {
        GetJaxerLog().Log(eDEBUG, "Filter:OnPreprocHeaders: Request (%s) will be handled by filter based on extension.",
            sUrl);
    }
    return TRUE;
}

BOOL OnReadRawData(HTTP_FILTER_CONTEXT* pFC, PHTTP_FILTER_RAW_DATA pVN)
{
    fctx* ctx = (fctx*) pFC->pFilterContext;
    if (!ctx)
    {
        return TRUE;
    }

    if (ctx->skip)
    {
        return TRUE;
    }

    if (pVN->cbInData > 0)
    {
        char * s = new char[pVN->cbInData];
        memcpy(s, pVN->pvInData, pVN->cbInData);
        GetJaxerLog().Log(eDEBUG, "Filter: OnReadRawData: length=%d", pVN->cbInData);
        GetJaxerLog().LogStr(eTRACE, s, pVN->cbInData);
        DataList *postData = new DataList(s, pVN->cbInData);
        if (!postData)
        {
            GetJaxerLog().LogStr(eERROR, "OnReadRawData: out of memory");
            return FALSE;
        }
        if (ctx->postData == 0)
        {
            ctx->postData = postData;
            ctx->postData_end = postData;
        }else
        {
            ctx->postData_end->next = postData;
            ctx->postData_end = ctx->postData_end->next;
        }
        ctx->postdata_true_len += pVN->cbInData;
    }

    return TRUE;
}

