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

#include "stdio.h"
#include <stdlib.h>
#include <wininet.h>
#include <strsafe.h>
#include <map>
#include <string>
#include "jaxer_message.h"
#include "jaxer_connection.h"
#include "jaxer_proc.h"
#include "jaxer_config.h"
#include "../../../mozilla/aptana/jaxerBuildId.h"

#pragma comment(lib, "ws2_32.lib")

static CJaxer_config gJaxerConfig;
CJaxer_config& GetJaxerConfig() { return gJaxerConfig;}

static char ini_file_name[MAX_PATH];

BOOL APIENTRY DllMain(HINSTANCE hInst, ULONG  ulReason, LPVOID lpReserved)
{
    BOOL fReturn = TRUE;
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[MAX_PATH];
    char file_name[MAX_PATH];

    UNREFERENCED_PARAMETER(lpReserved);

    switch (ulReason) {
    case DLL_PROCESS_ATTACH:

        // Initialize Winsock.
        WSADATA wsaData;
        if (int rc = WSAStartup(0x0202, &wsaData))
        {
            return FALSE;
        }

        if (GetModuleFileName(hInst, file_name, sizeof(file_name)))
        {
            _splitpath(file_name, drive, dir, fname, NULL);
            _makepath(ini_file_name, drive, dir, fname, ".conf");
            if ( strncmp(ini_file_name, "\\\\?\\", 4) == 0)
			{
				int i;
				for ( i=4; i<= (int)strlen(ini_file_name); i++)
				{
					ini_file_name[i-4] = ini_file_name[i];
				}
			}
            gJaxerConfig.SetConfigFilename(ini_file_name);
            gJaxerConfig.LoadConfig();
            GetJaxerLog().SetLogFile(gJaxerConfig.GetLogFileName());
            GetJaxerLog().SetBackUpSizeKB(gJaxerConfig.GetLogFileSizeKB());
            GetJaxerLog().SetLogLevel(gJaxerConfig.GetLogLevel());
            GetJaxerLog().SetNumBackups(gJaxerConfig.GetLogFileBackups());

            GetJaxerServer().SetHostName(gJaxerConfig.GetJaxerServerIP());
            GetJaxerServer().SetPort(gJaxerConfig.GetJaxerServerPort());

            GetJaxerLog().Log(eINFO, "jaxer_isapi(" JAXER_BUILD_ID "): loaded config from %s",ini_file_name);

        }
        else {
            fReturn = FALSE;
        }
    break;
    case DLL_PROCESS_DETACH:
#if 0
        __try {
            TerminateFilter(HSE_TERM_MUST_UNLOAD);
        }
        __except(1) {
        }
#endif
        break;

    default:
        break;
    }

    return fReturn;
}


BOOL WINAPI GetFilterVersion(HTTP_FILTER_VERSION *pVer)
{
	BOOL	bSuccess = TRUE;
    GetJaxerLog().Log(eDEBUG, "Entered GetFilterVersion" JAXER_BUILD_ID);
#if 0
	try
	{
		gbl_MemPool.Set(gbl_PoolName);
	}
	catch(SMemException &)
	{
		bSuccess = FALSE;
	}
#endif

    pVer->dwFlags &= ~SF_NOTIFY_ORDER_MASK;
    pVer->dwFlags |= SF_NOTIFY_ORDER_HIGH;

	pVer->dwFlags |= SF_NOTIFY_READ_RAW_DATA | SF_NOTIFY_PREPROC_HEADERS | SF_NOTIFY_URL_MAP |
        SF_NOTIFY_SEND_RESPONSE | SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST;

    strncpy(pVer->lpszFilterDesc, "Jaxer Global fillter for IIS 5 and 5.1 - " JAXER_BUILD_ID, HSE_MAX_EXT_DLL_NAME_LEN);

    return bSuccess;
}


DWORD WINAPI HttpFilterProc(HTTP_FILTER_CONTEXT *pFC, DWORD NotiType, VOID *pVN)
{

    DWORD ret = SF_STATUS_REQ_NEXT_NOTIFICATION;

    switch(NotiType)
    {
    case SF_NOTIFY_READ_RAW_DATA:
        if (! OnReadRawData(pFC, (PHTTP_FILTER_RAW_DATA)pVN))
        {
            ret = SF_STATUS_REQ_ERROR;
        }
        break;

    case SF_NOTIFY_PREPROC_HEADERS:
        if (!OnPreprocHeaders(pFC, (PHTTP_FILTER_PREPROC_HEADERS)pVN))
        {
            ret = SF_STATUS_REQ_ERROR;
        }
        break;

    case SF_NOTIFY_URL_MAP:
        if (!OnNotifyUrlMap(pFC, (PHTTP_FILTER_URL_MAP)pVN))
        {
            ret = SF_STATUS_REQ_ERROR;
        }
        break;

    case SF_NOTIFY_SEND_RESPONSE:
        if (!OnSendResponse(pFC, (PHTTP_FILTER_SEND_RESPONSE) pVN))
        {
            ret = SF_STATUS_REQ_ERROR;
        }
        break;

    case SF_NOTIFY_SEND_RAW_DATA:
        if (!OnSendRawdata(pFC, (PHTTP_FILTER_RAW_DATA)pVN))
        {
            ((fctx*)pFC->pFilterContext)->has_error = true; 
            ret = SF_STATUS_REQ_ERROR;
        }
        break;

    case SF_NOTIFY_END_OF_REQUEST:
        if ( OnEndOfRequest(pFC))
        {
            GetJaxerLog().Log(eDEBUG, "Successfully processed the request");
            ret = SF_STATUS_REQ_FINISHED_KEEP_CONN;
        }else
        {
            GetJaxerLog().Log(eERROR, "Failed to process the request");
            ret = SF_STATUS_REQ_ERROR;
        }
        break;
    }


	return ret;
}

BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
	pVer->dwExtensionVersion = HSE_VERSION;
	strncpy(pVer->lpszExtensionDesc, "Jaxer ISAPI Extension for IIS 5 and 5.1 - " JAXER_BUILD_ID, HSE_MAX_EXT_DLL_NAME_LEN);
    GetJaxerLog().Log(eINFO, "called GetExtensionVersion");

	return TRUE;
}



DWORD WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB)
{
    DWORD rc = HSE_STATUS_ERROR;
    
     
    GetJaxerLog().Log(eINFO, "Entered HttpExtensionProc, path=%s", pECB->lpszPathInfo);

    // We should not handle it if it is bypass
    char* q = strstr(pECB->lpszPathInfo, "/jaxer-include");
    if (q && (q[strlen("/jaxer-include")] == '/' || q[strlen("/jaxer-include")] == '?' ||
        q[strlen("/jaxer-include")] == '\0'))
    {
        GetJaxerLog().Log(eDEBUG, "jaxer-include will not be handled by jaxer");
        return FALSE;
    }

    CJCPool& jcp = GetJCPool();
    CJaxerConnection *c = jcp.GetConnection();
    if (!c)
    {
        GetJaxerLog().Log(eERROR, "Failed to get a connection to Jaxer");
        return rc;
    }

    char *pHTTPHeaderMsg;
    BuildHTTPHeaderMsg(pECB, &pHTTPHeaderMsg);
    
    if (!c->SendMsg(pHTTPHeaderMsg))
    {
        delete[] pHTTPHeaderMsg;
        delete c;
        GetJaxerLog().Log(eERROR, "Failed to send header msg");
        return rc;
    }
    delete[] pHTTPHeaderMsg;

    char *pEnvMsg;
    BuildEnvHeaderMsg(pECB, &pEnvMsg);
    if (!c->SendMsg(pEnvMsg))
    {
        delete[] pEnvMsg;
        delete c;
        GetJaxerLog().Log(eERROR, "Failed to send env msg");
        return rc;
    }
    delete[] pEnvMsg;

    // Send empty doc
    char doc[1];
    if (! c->SendDocumentFrag(doc, 0))
    {
        delete c;
        GetJaxerLog().Log(eERROR, "Failed to send empty doc");
        return rc;
    }

    if (! c->SendEndRequest())
    {
        delete c;
        GetJaxerLog().Log(eERROR, "Failed to send end req");
        return rc;
    }

    char *msg;
    if (! c->RecvMsg(&msg))
    {
        delete c;
        GetJaxerLog().Log(eERROR, "Failed to receive msg (request post data or the header)");
        return rc;
    }
    char msg_type = jxr_msg_get_type(msg);
    if (msg_type == BLOCKTYPE_REQUEST_POSTDATA)
    {
        delete[] msg;
        if (! SendPostData(pECB, c))
        {
            delete c;
            GetJaxerLog().Log(eERROR, "Failed to send post data");
            return rc;
        }
        if (! c->RecvMsg(&msg))
        {
            delete c;
            GetJaxerLog().Log(eERROR, "Failed to recv msg after sending post data");
            return rc;
        }
        msg_type = jxr_msg_get_type(msg);
    }

    if (! ProcessResponseHeaderMsg(pECB, msg))
    {
        delete[] msg;
        jcp.PutConnection(c);
        GetJaxerLog().Log(eERROR, "Failed to process response header msgr");
        return rc;
    }
    delete[] msg;


    if (! RecvAndProcessDocument(pECB, c))
    {
        delete c;
        GetJaxerLog().Log(eERROR, "Failed to recv and process doc");
        return rc;
    }

    jcp.PutConnection(c);

    GetJaxerLog().Log(eDEBUG, "Exit HttpExtensionProc, path=%s", pECB->lpszPathInfo);
    return HSE_STATUS_SUCCESS;
}

