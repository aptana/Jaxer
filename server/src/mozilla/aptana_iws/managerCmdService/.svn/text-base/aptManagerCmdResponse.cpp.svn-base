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


#include "aptManagerCmdResponse.h"
#include "nsString.h"
#include "aptCoreTrace.h"

extern aptCoreTrace gJaxerLog;

NS_IMPL_THREADSAFE_ISUPPORTS1(aptManagerCmdResponse, aptIManagerCmdResponse)

aptManagerCmdResponse::aptManagerCmdResponse()
: mFailed(PR_FALSE)
 ,mErrorCode(0)
 ,mRespData(0)
{
}

aptManagerCmdResponse::~aptManagerCmdResponse()
{
    while(mRespData)
    {
        RespList *next = mRespData->m_next;
        delete mRespData;
        mRespData = next;
    }
}

/* readonly attribute AUTF8String data; */
NS_IMETHODIMP aptManagerCmdResponse::GetData(nsACString & aData)
{
    if (mRespData)
    {
        aData.Assign((char*)mRespData->m_data, mRespData->m_len);
        RespList *p = mRespData;
        mRespData = mRespData->m_next;
        delete p;
    }else
    {
        aData.Assign("", 0);
    }
    return NS_OK;
}

/* readonly attribute long dataLength; */
NS_IMETHODIMP aptManagerCmdResponse::GetDataLength(PRInt32 *aDataLength)
{
    *aDataLength = mRespData ? mRespData->m_len : 0;

    return NS_OK;
}

/* readonly attribute boolean hasMore; */
NS_IMETHODIMP aptManagerCmdResponse::GetHasMore(PRBool *aHasMore)
{
    *aHasMore = (mRespData != 0);
    return NS_OK;
}

/* readonly attribute boolean failed; */
NS_IMETHODIMP aptManagerCmdResponse::GetFailed(PRBool *aFailed)
{
    *aFailed = mFailed;
    return NS_OK;
}

/* readonly attribute long errorCode; */
NS_IMETHODIMP aptManagerCmdResponse::GetErrorCode(PRInt32 *aErrorCode)
{
    *aErrorCode = mErrorCode;
    return NS_OK;
}

/* readonly attribute AUTF8String errorText; */
NS_IMETHODIMP aptManagerCmdResponse::GetErrorText(nsACString & aErrorText)
{
    return GetData(aErrorText);
}

nsresult aptManagerCmdResponse::SetErrorText(const PRUint8* errorText, PRUint16 len)
{
    return SetResponseData(errorText, len);
}

nsresult aptManagerCmdResponse::SetResponseData(const PRUint8* data, PRUint16 len)
{
    RespList * aRespElem = new RespList();
    PRUint8 * aDataBuf = new PRUint8[len+1];
    if (!aRespElem || !aDataBuf)
    {
        gJaxerLog.Log(eERROR, "aptManagerCmdResponse::SetResponseData: Failed to allocate memory for response data");
        if (aRespElem)
            delete aRespElem;
        if (aDataBuf)
            delete aDataBuf;

        return NS_ERROR_OUT_OF_MEMORY;
    }

    aRespElem->m_data = aDataBuf;
    aRespElem->m_len = len;
    memcpy(aDataBuf, data, len);
    aDataBuf[len] = 0;
    
    if (!mRespData)
        mRespData = aRespElem;
    else
    {
        RespList *p = mRespData;
        while(p->m_next)
            p = p->m_next;
        p->m_next = aRespElem;
    }

    return NS_OK;
}

nsresult aptManagerCmdResponse::Init(const PRUint8 *resp)
{
    gJaxerLog.Log(eTRACE, "aptManagerCmdResponse::Init: entered");
    if (resp[0] != eCMD_RESP_MSG)
    {
        //error
        gJaxerLog.Log(eERROR, "aptManagerCmdResponse::Init: Invalid data type %d", resp[0]);
        return NS_ERROR_UNEXPECTED;
    }

    PRUint32 len = resp[1] << 8 | resp[2];
    if (len >= CMDBUFSIZE)
    {
        gJaxerLog.Log(eERROR, "aptManagerCmdResponse::Init: Invalid data - size (%d) exceeds max (%d)", len, CMDBUFSIZE);
        
        return NS_ERROR_UNEXPECTED;
    }

    mFailed = (resp[3] == 0);
    if (mFailed && len < 3)
    {
        gJaxerLog.Log(eERROR, "aptManagerCmdResponse::Init: Invalid data - size (%d) too small for a failed-response", len);
        return NS_ERROR_UNEXPECTED;
    }
    if (!mFailed && len <4)
    {
        gJaxerLog.Log(eERROR, "aptManagerCmdResponse::Init: Invalid data - size (%d) too small for a succseeful-response", len);
        return NS_ERROR_UNEXPECTED;
    }

    PRUint16 msglen;

    if (mFailed)
    {
        //mHasMore = PR_FALSE;
        //error code
        mErrorCode = resp[4] << 8 | resp[5];
        if (len>=5)
        {
            msglen = resp[6] << 8 | resp[7];
        }else
            msglen = 0;
        gJaxerLog.Log(eTRACE, "aptManagerCmdResponse::Init: setup a failed msg");
        SetErrorText(resp+8, msglen);
    }else
    {
        msglen = len -2;
        //mHasMore = (resp[4] != 0);
        gJaxerLog.Log(eTRACE, "aptManagerCmdResponse::Init: setup a resp msg.  has more? %d", (resp[4] != 0));
        SetResponseData(resp+5,msglen);
    }

    return NS_OK;
}

