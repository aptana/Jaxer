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
#ifndef __JAXER_PROC_H__
#define __JAXER_PROC_H__

#include <map>
#include <string>
#include <httpext.h>
#include <httpfilt.h>
#include "jaxer_log.h"

#define MAX_STRING_LEN 0x4000

typedef std::map<std::string, std::string> HeaderMap;
typedef std::map<std::string, std::string>::const_iterator HeaderIterator;

CJaxerServer& GetJaxerServer();
CJCPool& GetJCPool();

struct DataList
{
    char* data;
    int len;
    DataList* next;
    DataList(char* s, int l)
    {
        data = s;
        len = l;
        next = 0;
    }
    ~DataList()
    {
        GetJaxerLog().Log(eTRACE, "~DataList");
        delete[] data;
        data = 0;
        if (next)
        {
            GetJaxerLog().Log(eTRACE, "next ~DataList");
            delete next;
            next = 0;
        }
    }
};


struct fctx
{
    int stage;
    DataList *postData;
    DataList *postData_end;
    bool postdata_sent;
    int postdata_tot_len;
    int postdata_true_len;
    CJaxerConnection *c;
    int doc_len;
    bool has_error;
    // bool done;
    bool skip;
    bool skip2; //hack.  Not sure if we should skip
    int status;
    HeaderMap envMap;
    HeaderMap RespHeaders;
    fctx()
    {
        stage = 0;
        postData = 0;
        postData_end = 0;
        postdata_sent = false;
        postdata_true_len = 0;
        postdata_tot_len = 0;
        doc_len = -1;
        c = 0;
        has_error = false;
        // done = 0;
        skip = false;
        skip2 = true;
        status = 200;
    }
    ~fctx()
    {
        GetJaxerLog().Log(eTRACE, "~fctx");
        delete postData;
        postData = 0;
        if (has_error)
        {
            GetJaxerLog().Log(eTRACE, "~fctx delete c");
            delete c;
            c = 0;
        }else if (c!=0)
            GetJCPool().PutConnection(c);
    }
};

void BuildHeaderMessage(HeaderMap& map, char type, char** msg);
BOOL BuildHTTPHeaderMsg(EXTENSION_CONTROL_BLOCK *pECB, char** sHeaderMsg);
BOOL BuildHTTPHeaderMsg(HTTP_FILTER_CONTEXT *pECB, char** sHeaderMsg);
BOOL BuildEnvHeaderMsg(EXTENSION_CONTROL_BLOCK *pECB, char**sEnvMsg);
BOOL BuildEnvHeaderMsg(HTTP_FILTER_CONTEXT *pECB, char** EnvMsg);
BOOL SendPostData(EXTENSION_CONTROL_BLOCK *pECB, CJaxerConnection* pc);
BOOL ProcessResponseHeaderMsg(EXTENSION_CONTROL_BLOCK* pECB, char* msg);
BOOL RecvAndProcessDocument(EXTENSION_CONTROL_BLOCK* pECB, CJaxerConnection* pc);
BOOL RecvAndProcessDocument(HTTP_FILTER_CONTEXT* pECB);
BOOL ProcessResponseHeaderMsg(HTTP_FILTER_CONTEXT* pECB, char* msg);
BOOL SendPostData(HTTP_FILTER_CONTEXT *pFC);
BOOL OnEndOfRequest(HTTP_FILTER_CONTEXT* pECB);
BOOL OnSendRawdata(HTTP_FILTER_CONTEXT *pFC, PHTTP_FILTER_RAW_DATA pRawData);
BOOL OnSendResponse(HTTP_FILTER_CONTEXT* pFC, PHTTP_FILTER_SEND_RESPONSE pVN);
BOOL OnNotifyUrlMap(HTTP_FILTER_CONTEXT* pFC, PHTTP_FILTER_URL_MAP pUMData);
BOOL HandledByJaxer(PHTTP_FILTER_URL_MAP pUMData);
BOOL OnPreprocHeaders(HTTP_FILTER_CONTEXT* pFC, PHTTP_FILTER_PREPROC_HEADERS pVN);
BOOL OnReadRawData(HTTP_FILTER_CONTEXT* pFC, PHTTP_FILTER_RAW_DATA pVN);



#endif // __JAXER_PROC_H__

