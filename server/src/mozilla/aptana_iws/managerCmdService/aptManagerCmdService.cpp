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

#include "nsThreadUtils.h"
#include "nsIServiceManager.h"
#include "aptManagerCmdService.h"
#include "nsString.h"
#include "aptCoreTrace.h"
#include "aptManagerCmdResponse.h"
#include "aptICmdCallback.h"
#include "aptCmdCallbackEvent.h"
#include "aptManagerEvent.h"
#include "nsComponentManagerUtils.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <errno.h>
#endif


extern aptCoreTrace gJaxerLog;


NS_IMPL_THREADSAFE_ISUPPORTS1(aptManagerCmdService, aptIManagerCmdService)

aptManagerCmdService::aptManagerCmdService()
:mMgrSocket(INVALID_HANDLE_VALUE)
,mInited(PR_FALSE)
,mToMgrLock(nsnull)
,mCmdRespMonitor(nsnull)
,mCmdLock(nsnull)
,mResponse(nsnull)
,mIsAsync(PR_FALSE)
,mCmdInProgress(PR_FALSE)
{

}

aptManagerCmdService::~aptManagerCmdService()
{
  if (mToMgrLock)
      PR_DestroyLock(mToMgrLock);
  if (mCmdLock)
      PR_DestroyLock(mCmdLock);

  if (mCmdRespMonitor)
      PR_DestroyMonitor(mCmdRespMonitor);
}

/* [noscript] void init (in long socket); */
NS_IMETHODIMP aptManagerCmdService::Init(PRInt32 socket)
{
    if (socket == (PRInt32)INVALID_HANDLE_VALUE)
    {
        return NS_ERROR_FAILURE;
    }
    mMgrSocket = (HANDLE)socket;

    mToMgrLock = PR_NewLock();
    mCmdLock = PR_NewLock();
    mCmdRespMonitor = PR_NewMonitor();

    if (!mToMgrLock || !mCmdLock || !mCmdRespMonitor)
    {
        return NS_ERROR_OUT_OF_MEMORY;
    }

    mInited = PR_TRUE;

    return NS_OK;
}

/* void execNoRespCmd (in AUTF8String cmd); */
NS_IMETHODIMP aptManagerCmdService::ExecNoRespCmd(const nsACString & cmd)
{
    gJaxerLog.Log(eTRACE, "ExecNoRespCmd: Entered.");
    if (!mInited)
    {
        gJaxerLog.Log(eERROR, "ExecNoRespCmd: ManagerCmdService not initialized.");
        return NS_ERROR_NOT_INITIALIZED;
    }

    const char *p = 0;
    PRUint32 len = cmd.GetData(&p);
    if (len == 0)
    {
        gJaxerLog.Log(eWARN, "ExecNoRespCmd: null command specified.");
        return NS_OK;
    }

    if (len > CMDBUFSIZE-3)
    {
        // too large
        gJaxerLog.Log(eWARN, "ExecNoRespCmd: Command length (%d) exceeds max limit %d.", len, CMDBUFSIZE-3);
        return NS_ERROR_INVALID_ARG;
    }

    gJaxerLog.Log(eTRACE, "ExecNoRespCmd: Get Lock for writing to manager. cmd:%s", p);
    PR_Lock(mToMgrLock);

    mCmdBuf[0] = eNO_RESP_CMD_MSG;
    mCmdBuf[1] = PRUint8(len >> 8);
    mCmdBuf[2] = PRUint8(len);
    memcpy(mCmdBuf+3, p, len);
    mCmdLen = len + 3;

    nsresult rv = SendCmdToManager();

    PR_Unlock(mToMgrLock);
    gJaxerLog.Log(eTRACE, "ExecNoRespCmd: Released Lock for writing to manager. cmd:%s", p);
    
    return rv;
}
nsresult aptManagerCmdService::BasicCmdCheck(const char* cmd, PRUint32 len)
{
    gJaxerLog.Log(eTRACE, "BasicCmdCheck: Entered.");
    if (!mInited)
    {
        gJaxerLog.Log(eERROR, "BasicCmdCheck: ManagerCmdService not initialized.");
        return NS_ERROR_NOT_INITIALIZED;
    }

    //Make sure no other NeedRespCmd is in progress
    if (mCmdInProgress)
    {
        gJaxerLog.Log(eERROR, "BasicCmdCheck: Another NeedRespCmd is already in progress.");
        return NS_ERROR_UNEXPECTED;
    }

    if (len == 0)
    {
        gJaxerLog.Log(eWARN, "BasicCmdCheck: null command specified.");
        return NS_ERROR_UNEXPECTED;
    }
    if (len > CMDBUFSIZE-3)
    {
        // too large
        gJaxerLog.Log(eWARN, "BasicCmdCheck: Command length (%d) exceeds max limit %d.", len, CMDBUFSIZE-3);
        return NS_ERROR_INVALID_ARG;
    }

    return NS_OK;
}


/* void execNeedRespCmd (in AUTF8String cmd, in aptICmdCompleteCallback cb, in boolean bAsync); */
NS_IMETHODIMP aptManagerCmdService::ExecNeedRespCmd(const nsACString & cmd, aptICmdCompleteCallback *cb, PRBool bAsync)
{
    nsresult rv;

    gJaxerLog.Log(eTRACE, "ExecNeedRespCmd: Entered.");

    const char *p = 0;
    PRUint32 len = cmd.GetData(&p);

    rv = BasicCmdCheck(p, len);
    if (NS_FAILED(rv))
    {
        return rv;
    }

    if (!cb)
    {
        gJaxerLog.Log(eERROR, "ExecNeedRespCmd: callback function cannot be null.");
        return NS_ERROR_UNEXPECTED;
    }

    gJaxerLog.Log(eTRACE, "ExecNeedRespCmd: Get CMD Lock. cmd:%s", p);
    PR_Lock(mCmdLock);
    if (mCmdInProgress)
    {
        gJaxerLog.Log(eERROR, "ExecNeedRespCmd: Another NeedRespCmd is already in progress.");
        PR_Unlock(mCmdLock);
        return NS_ERROR_UNEXPECTED;
    }

    mIsAsync = bAsync;
    mCallback = cb;

    // Get and remember the calling thread
    rv = NS_GetCurrentThread(getter_AddRefs(mCallingThread));
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eWARN, "ExecNeedRespCmd: failed to get current thread: rv=0x%x.", rv);
        PR_Unlock(mCmdLock);
        return rv;
    }

    mResponse = new aptManagerCmdResponse();
    if (!mResponse)
    {
        gJaxerLog.Log(eERROR, "ExecNeedRespCmd: Failed to allocate memory for response object");
        PR_Unlock(mCmdLock);
        return NS_ERROR_OUT_OF_MEMORY;
    }
    
    mCmdInProgress = PR_TRUE;
    
    PR_Unlock(mCmdLock);

    //Make sure we do not fight with others for the network
    gJaxerLog.Log(eTRACE, "ExecNeedRespCmd: Get Lock for writing to manager.");
    PR_Lock(mToMgrLock);

    
    mCmdBuf[0] = eNEED_RESP_CMD_MSG;
    mCmdBuf[1] = PRUint8(len >> 8);
    mCmdBuf[2] = PRUint8(len);
    memcpy(mCmdBuf+3, p, len);
    mCmdLen = len + 3;

    if (!mIsAsync)
    {
        // wait for response.  Need this before sending the cmd.
        PR_EnterMonitor(mCmdRespMonitor);
        gJaxerLog.Log(eTRACE, "ExecNeedRespCmd: Enter monitor for cmd response");
    }
    rv = SendCmdToManager();

    // Now other NoRespCmds can be sent to manager
    PR_Unlock(mToMgrLock);
    gJaxerLog.Log(eTRACE, "ExecNeedRespCmd: Released Lock for writing to manager. cmd:%s", p);

    if (NS_FAILED(rv))
    {
        mCmdInProgress = PR_FALSE;
        PR_ExitMonitor(mCmdRespMonitor);
        gJaxerLog.Log(eERROR, "ExecNeedRespCmd: Send cmd to manager failed: rv=0x%x.", rv);
        return rv;
    }
    
    if (!mIsAsync)
    {
        // Now we are waiting for response
        gJaxerLog.Log(eTRACE, "ExecNeedRespCmd: Waiting for cmd response");
        PR_Wait(mCmdRespMonitor, PR_INTERVAL_NO_TIMEOUT);

        gJaxerLog.Log(eTRACE, "ExecNeedRespCmd: Got cmd response");
        
        rv = mCallback->HandleResponse(mResponse);
        PR_ExitMonitor(mCmdRespMonitor);

        mCmdInProgress = PR_FALSE;
    }
    
    return NS_OK;
}

nsresult aptManagerCmdService::SendCmdToManager()
{
    gJaxerLog.Log(eTRACE, "SendCmdToManager: enter");
#ifdef _WIN32
    // TO DO: need to make sure all data are written
    DWORD nWritten = 0;
    BOOL rc = WriteFile(mMgrSocket, mCmdBuf, mCmdLen, &nWritten, 0);
    if (!rc || nWritten != mCmdLen)
    {
        gJaxerLog.Log(eFATAL, "aptManagerCmdService: Could not send data to manager:%d.  Jaxer needs to exit.", GetLastError());
        aptManagerEvent* aEvent = new aptManagerEvent(eEXIT_MSG);
        NS_DispatchToMainThread(aEvent, NS_DISPATCH_NORMAL);
        return NS_ERROR_ABORT;
    }
#else
    int nSent = 0;
    while(nSent < mCmdLen)
    {
        int n;
        do
        {
            n = send(mMgrSocket, mCmdBuf+nSent, mCmdLen-nSent, 0);
        }while (n<0 && errno == EINTR);
    
        if (n<=0)
        {
            gJaxerLog.Log(eFATAL, "aptManagerCmdService: Could not send data to manager:ret=%d, errno=%d.  Jaxer needs to exit.", n, errno);
            aptManagerEvent* aEvent = new aptManagerEvent(eEXIT_MSG);
            NS_DispatchToMainThread(aEvent, NS_DISPATCH_NORMAL);
            return NS_ERROR_ABORT;
        }else
        {
            nSent += n;
        }
    }
    
#endif

    return NS_OK;
}

/* [noscript] void setCmdRespData (in string resp); */
NS_IMETHODIMP aptManagerCmdService::SetCmdRespData(const char *aResp)
{
    nsresult rv;
    const unsigned char* resp = (const unsigned char*) aResp;
    gJaxerLog.Log(eTRACE, "SetCmdRespData: Entered.");
    if (!mInited)
    {
        gJaxerLog.Log(eERROR, "SetCmdRespData: ManagerCmdService not initialized.");
        return NS_ERROR_NOT_INITIALIZED;
    }

    if (!mCmdInProgress)
    {
        gJaxerLog.Log(eERROR, "SetCmdRespData: No NeedrespCmd in progress.");
        return NS_ERROR_UNEXPECTED;
    }

    PRUint32 len = resp[1] << 8 | resp[2];
    PRBool ok = resp[3];
    PRBool more = resp[4];
    int errCode = resp[4] << 8 | resp[5];
    int errLen = resp[6] << 8 | resp[7];

    int offset = (ok) ? 5 : 8;
    int datalen = (ok) ? (len -2) : errLen;

    rv = mResponse->Init((PRUint8*)aResp);
    if (NS_FAILED(rv) || !ok || !more)
    {    
        if (mIsAsync)
        {
            // Construct an event and dispatch it to the calling thread
            aptCallbackEvent *aEvent = new aptCallbackEvent(mResponse, mCallback);
            if (!aEvent)
            {
                gJaxerLog.Log(eERROR, "SetCmdRespData: failed to create a callback event.");
                return NS_ERROR_OUT_OF_MEMORY;
            }
            mCmdInProgress = PR_FALSE;
            mCallingThread->Dispatch(aEvent, NS_DISPATCH_NORMAL);
        }else
        {
            PR_EnterMonitor(mCmdRespMonitor);        
            PR_Notify(mCmdRespMonitor);
            PR_ExitMonitor(mCmdRespMonitor);
        }
        
    }
   
    return NS_OK;
}

/* [noscript] void execEndReqCmd (); */
NS_IMETHODIMP aptManagerCmdService::ExecEndReqCmd()
{
    if (!mInited)
    {
        gJaxerLog.Log(eERROR, "ExecEndReqCmd: ManagerCmdService not initialized.");
        return NS_ERROR_NOT_INITIALIZED;
    }

    gJaxerLog.Log(eTRACE, "ExecEndReqCmd: Sending End_Req cmd to manager.");
    PR_Lock(mToMgrLock);

    mCmdBuf[0] = eEND_REQUEST_MSG;
    mCmdBuf[1] = 0;
    mCmdBuf[2] = 0;
    mCmdLen = 3;

    nsresult rv = SendCmdToManager();
    
    PR_Unlock(mToMgrLock);
    
    return rv;
}

/* [noscript] void notifyPossibleCmdFailure (); */
NS_IMETHODIMP aptManagerCmdService::NotifyPossibleCmdFailure()
{
    gJaxerLog.Log(eTRACE, "NotifyPossibleCmdfailure: Entered.");
    if (!mInited)
    {
        return NS_OK;
    }

    if (!mCmdInProgress)
    {
        return NS_OK;
    }

    char buf[5];
    buf[0] = buf[1] = buf[2] = 0;
    SetCmdRespData(buf);
    
    return NS_OK;
}

