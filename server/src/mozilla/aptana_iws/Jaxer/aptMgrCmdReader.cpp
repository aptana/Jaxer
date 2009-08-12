#include "nsIServiceManager.h"
#include "aptMgrCmdReader.h"
#include "aptManagerEvent.h"
//#include "aptExitEvent.h"
#include "aptCoreTrace.h"
#include "jaxerProtocolVersion.h"
#include "blockDump.h"
#include "aptManagerCmdService.h"

#ifndef _WIN32
//#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#endif

extern eJAXER_STATE gJaxerState;
extern aptCoreTrace gJaxerLog;

NS_IMPL_THREADSAFE_ISUPPORTS1(aptMgrCmdReader, nsIRunnable)

aptMgrCmdReader::aptMgrCmdReader(HANDLE num)
: mMgrSocket(num)
 ,mFD(INVALID_SOCKET)
{
#ifdef _WIN32
    memset(&mOverlapped, 0, sizeof(mOverlapped));
    mOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif
}

aptMgrCmdReader::~aptMgrCmdReader()
{
#ifdef _WIN32
    CloseHandle(mOverlapped.hEvent);
#endif
}

NS_IMETHODIMP aptMgrCmdReader::Run()
{
    nsresult rv = NS_OK;
    
    BlockDumper recvDump("Recv");
    nsCOMPtr<aptIManagerCmdService> mcs = do_GetService("@aptana.com/managercmdservice;1", &rv);
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "aptMgrCmdReader: get aptIManagerCmdService failed: rv=0x%x", rv);
        gJaxerState = eEXITING;
    }

    // we are in msg mode, so each read gets one msg
    while (gJaxerState != eEXITING)
    {
        DWORD nRead = 0;
        mFD = INVALID_SOCKET;
        
#ifdef _WIN32
        BOOL rc = TRUE;
        DWORD nAvail = 0;
        ::ReadFile(mMgrSocket, mRecvBuf, CMDBUFSIZE, &nRead, &mOverlapped);
        DWORD wState = WaitForSingleObject(mOverlapped.hEvent,INFINITE);
        //gJaxerLog.Log(eTRACE, "wState=%x  error %d", wState, WAIT_OBJECT_0, GetLastError());

        if (WAIT_OBJECT_0 != wState)
        {
            gJaxerState = eEXITING;
            rv = NS_ERROR_FAILURE;
            gJaxerLog.Log(eFATAL, "WaitForSingleObject failed:%d", GetLastError());
            break;
        }
        
        //  Signalled on the overlapped event handle, check the result
        if (! GetOverlappedResult(mMgrSocket, &mOverlapped, &nRead, TRUE))
        {
            gJaxerState = eEXITING;
            rv = NS_ERROR_FAILURE;
            gJaxerLog.Log(eFATAL, "GetOverlappedResult failed:%d", GetLastError());
            break;
        }

#if 0
        while(nAvail==0 && rc)
        {
            rc = PeekNamedPipe(mMgrSocket, NULL, 0, NULL, &nAvail, NULL);
            if (!rc)
            {
                gJaxerState = eEXITING;
                rv = NS_ERROR_FAILURE;
                gJaxerLog.Log(eFATAL, "PeekNamedPipe failed:%d", GetLastError());
                break;
            }
            if (gJaxerState == eEXITING)
            {
                break;
            }

            SleepEx(1, TRUE);
        }
        if (gJaxerState == eEXITING)
            break;

        rc = ReadFile(mMgrSocket, mRecvBuf, CMDBUFSIZE, &nRead, NULL);
        if (!rc) 
        {
            rv = NS_ERROR_FAILURE;
            gJaxerLog.Log(eFATAL, "ReadFile failed:%d", GetLastError());
            break;
        }
#endif
#else
        iovec iov;
        iov.iov_base = (char*) mRecvBuf;
        iov.iov_len = CMDBUFSIZE;

        char msgbuf[CMSG_SPACE(sizeof(int))];
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = msgbuf;
        msg.msg_controllen = sizeof(msgbuf);

        do {
                nRead = recvmsg(mMgrSocket, &msg, 0);
                gJaxerLog.Log(eTRACE, "recvmsg: nRead=%d errno=%d (EINTR=%d)", nRead, errno, EINTR);
           } while (nRead < 0 && errno == EINTR);
    

        if (nRead < 0) 
        {
            gJaxerLog.Log(eFATAL, "Could not receive message from pipe");
            break;
        } else if (nRead == 0)
        {
            gJaxerLog.Log(eINFO, "Connection to JaxerManager closed.");
            break;
        } else if ((msg.msg_flags & MSG_TRUNC) || (msg.msg_flags & MSG_CTRUNC))
        {
            gJaxerLog.Log(eFATAL, "Control message truncated.");
            break;
        }

        cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg)
        {
            if (cmsg->cmsg_len == CMSG_LEN(sizeof(int)) &&
                  cmsg->cmsg_level == SOL_SOCKET &&
                  cmsg->cmsg_type == SCM_RIGHTS)
            {
                mFD = *(int *)CMSG_DATA(cmsg);
            } else
            {
                // LogFatal("No socket descriptor in control message.");
                mFD = INVALID_SOCKET;
            }
        } else
        {
            mFD = INVALID_SOCKET;
        }

        if (mRecvBuf[0] != eNEW_REQUEST_MSG  && 
			mRecvBuf[0] != eNEW_HTTP_REQUEST_MSG &&
			mRecvBuf[0] != eNEW_ADMIN_REQUEST_MSG &&
			mFD != INVALID_SOCKET)
        {
            close(mFD);
            mFD = INVALID_SOCKET;
        }

        if (mFD == INVALID_SOCKET &&
			(mRecvBuf[0] == eNEW_REQUEST_MSG || 
			 mRecvBuf[0] == eNEW_HTTP_REQUEST_MSG ||
			 mRecvBuf[0] == eNEW_ADMIN_REQUEST_MSG
			)
		   )
        {
          gJaxerLog.Log(eFATAL, "No socket descriptor in control message.");
          mRecvBuf[0] = eEXIT_MSG;
        }
#endif
        mRecvIn = PRUint16(nRead);

        if (gJaxerLog.GetDumpEnabled()) 
        {
            gJaxerLog.Log(eINFO, "Message from manager:");
            recvDump.LogContents(mRecvBuf, mRecvIn, true);
            recvDump.Flush();
        }
    
        
        // 0-type [12]-len, [...]-msg data
        PRUint8 msgType = mRecvBuf[0];
        PRUint16 msgLen = (mRecvBuf[1] << 8) | mRecvBuf[2];
        if (mRecvIn<3 || mRecvIn != msgLen + 3)
        {
            //error - msg has to have at least 3 bytes
            gJaxerLog.Log(eFATAL, "Invalid message received from manager: RecvLen=%d, msglen=%d, msgType=%d.",
                mRecvIn, msgLen, msgType);
            //Exit
            msgType = eEXIT_MSG;

        }
        
        switch (msgType) 
        {
            case eNEW_REQUEST_MSG:
			case eNEW_HTTP_REQUEST_MSG:
			case eNEW_ADMIN_REQUEST_MSG:
                gJaxerLog.Log(eTRACE, "Received START_REQ_MSG.");
#ifdef _WIN32
                //Extract the socket
                WSAPROTOCOL_INFO sockInfo;
                if (msgLen != (PRUint16) sizeof(sockInfo))
                {
                    gJaxerLog.Log(eFATAL, "Wrong data received for socket info: Expected length=%d, received length=%d",
                        sizeof(sockInfo), mRecvIn);
                    return NS_ERROR_FAILURE;
                }
                memcpy(&sockInfo, mRecvBuf+3, msgLen);
                mFD = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sockInfo, 0, 0);
#endif
                // New request
                {
                    nsCOMPtr<aptManagerEvent> ReqEvent = new aptManagerEvent((eMSG_TYPE)msgType);
                    ReqEvent->SetWebSocket(mFD);
                    ReqEvent->SetMgrSocket(mMgrSocket);
                    // We need this be async
                    rv = NS_DispatchToMainThread(ReqEvent, NS_DISPATCH_NORMAL);
                }
                break;

            case eSET_PREF_MSG:
                gJaxerLog.Log(eTRACE, "Received SET_PREF_MSG.");
                // Pref request
                {
                    nsCOMPtr<aptManagerEvent> aEvent = new aptManagerEvent(eSET_PREF_MSG);
                    rv = aEvent->SetEventData(mRecvBuf + 1, mRecvIn-1);
                    // We need this be async
                    rv = NS_DispatchToMainThread(aEvent, NS_DISPATCH_NORMAL);
                }
                
                break;
                
            case eCMD_RESP_MSG:
                gJaxerLog.Log(eTRACE, "Received eCMD_RESP_MSG.");
                rv = mcs->SetCmdRespData((const char*)mRecvBuf);
                if (NS_FAILED(rv))
                {
                    gJaxerLog.Log(eFATAL, "Set cmd response data failed: rv=0x%x.", rv);
                    // exit
                    nsCOMPtr<aptManagerEvent> aEvent = new aptManagerEvent(eEXIT_MSG);
                    NS_DispatchToMainThread(aEvent, NS_DISPATCH_NORMAL);
                    return rv;
                }
                break;

            case eEXIT_MSG:
            default:
                if (msgType == eEXIT_MSG)
                {
                    gJaxerLog.Log(eINFO, "Received exit message from manager.  Exiting...");
                }else
                {
                    gJaxerLog.Log(eFATAL, "Unknown message (%d) from manager.", mRecvBuf[0]);
                }
                gJaxerState = eEXITING;

                break;
        }
    }

    if (mcs)
        mcs->NotifyPossibleCmdFailure();

    // exit
    nsCOMPtr<aptManagerEvent> aEvent = new aptManagerEvent(eEXIT_MSG);                   
    rv = NS_DispatchToMainThread(aEvent, NS_DISPATCH_NORMAL);
            
    gJaxerLog.Log(eTRACE, "MgrCmdReader thread returning with code %d.", rv);

    return rv;
}

