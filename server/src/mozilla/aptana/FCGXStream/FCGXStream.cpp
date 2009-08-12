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
#include "FCGXStream.h"
#include "nsILocalFile.h"
#include "nsCRT.h"
#include "nsIPrefService.h"
#include "nsThreadUtils.h"
#include "nsIConsoleService.h"
#include "aptCoreTrace.h"
//#include "ConsoleLog.h"

#ifdef _WIN32

#include <windows.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define HANDLE int
#define INVALID_SOCKET -1
#define INVALID_HANDLE_VALUE -1

#endif

#include "jaxerProtocolVersion.h"
#include "blockDump.h"

// static HANDLE g_pipe = INVALID_HANDLE_VALUE;
extern aptCoreTrace gJaxerLog;

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

static void LogFatal(const char *msg, PRInt32 msglen = -1)
{
    g_recv.Flush();
    g_send.Flush();
    size_t len = msglen < 0 ? PL_strlen(msg) : msglen;
    gJaxerLog.LogBytes(eFATAL, msg, len);
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

nsresult
APT_NewFCGXStream(IFCGXStream **aResult)
{
    nsresult rv;
    static NS_DEFINE_CID(kFCGXStreamCID, FCGXSTREAM_CID);
    nsCOMPtr<IFCGXStream> fcgxStream = do_CreateInstance(kFCGXStreamCID, &rv);
	NS_ENSURE_SUCCESS(rv, rv);
	*aResult = fcgxStream;
    NS_IF_ADDREF(*aResult);
    return rv;
}

////////////////////////////////////////////////////////////////////////////////
// FCGXStream

#if 0
static const char *kDumpProtocol = "Jaxer.dev.DumpProtocol";
//static const char *kEnableIdle = "Jaxer.enable_idle";

//PRBool FCGXStream::g_IdleLoop = PR_TRUE;
//PRBool FCGXStream::g_dumpEnabled = PR_FALSE;

void FCGXStream::UpdatePrefSettings(nsIPrefBranch *aPrefBranch, const char* pref)
{
    gJaxerLog.Log(eTRACE, "In FCGXStream::UpdatePrefSettings: pref=%s", pref ? pref : "null");
    gJaxerLog.Log(eTRACE, "Old pref values: g_dumpEnabled=%d", FCGXStream::g_dumpEnabled);
    if (!pref)
    {
        aPrefBranch->GetBoolPref(kDumpProtocol, &FCGXStream::g_dumpEnabled);
        //aPrefBranch->GetBoolPref(kEnableIdle, &FCGXStream::g_IdleLoop);
    }
    else if (strcmp(kDumpProtocol, pref) == 0)
    {
        aPrefBranch->GetBoolPref(kDumpProtocol, &FCGXStream::g_dumpEnabled);
    //}else if (strcmp(kEnableIdle, pref) == 0)
    //{
    //    aPrefBranch->GetBoolPref(kEnableIdle, &FCGXStream::g_IdleLoop);
    }
    gJaxerLog.Log(eTRACE, "new pref values: g_dumpEnabled=%d", FCGXStream::g_dumpEnabled);
}
#endif

FCGXStream::FCGXStream()
    : mFD(INVALID_SOCKET),
      mRecvIn(0),
      mRecvOut(0),
      mRecvCurBlockType(bt_BeginRequest),
      mRecvBytesLeftInBlock(0),
      mSendIn(0),
      mSendHdrOffset(0),
      mDocText(nsnull),
      mDocTextOffset(0),
      mDocTextHeader(nsnull),
      mDocTextTail(nsnull),
      mPostDataAfterEndRequest(PR_FALSE),
      mLock(PR_NewMonitor())
{
}

FCGXStream::~FCGXStream()
{
    CloseConnection(false);
    PR_DestroyMonitor(mLock);
}

NS_IMPL_THREADSAFE_ISUPPORTS3(FCGXStream,
                              nsISeekableStream,
                              nsIInputStream,
                              IFCGXStream)


nsresult
FCGXStream::Close()
{
    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::Seek(PRInt32 whence, PRInt64 offset)
{
    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::Tell(PRInt64 *result)
{
    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::SetEOF()
{
    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::Available(PRUint32* aResult)
{
    *aResult = PR_UINT32_MAX;
    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::Read(char* aBuf, PRUint32 aCount, PRUint32* aResult)
{
    // Mozilla does its HTML reading on a separate thread, so we need to own the
    // lock.  This is the *only* method called by the input pump on a non-main
    // thread.
    PR_EnterMonitor(mLock);

    // Before reading from the stream, check to see if (1) document text has
    // been set aside while waiting for post data, and (2) we have resumed
    // receiving the document.
    if (mDocText && mRecvCurBlockType != bt_PostData) {
        if (mDocText->mLen == 0) {
            // End of document was seen during accumulation.
            *aResult = 0;
            PR_Free(mDocText);
            mDocText = nsnull;
            PR_ExitMonitor(mLock);
            return NS_OK;
        }

        PRUint32 amt = mDocText->mLen - mDocTextOffset;
        if (amt > aCount) amt = aCount;
        memcpy(aBuf, mDocText->mText + mDocTextOffset, amt);
        *aResult = amt;
        mDocTextOffset += amt;

        if (mDocTextOffset == mDocText->mLen) {
            DocText *p = mDocText->mNext;

            // Save the doc for late use
            if (mDocTextTail) {
                mDocTextTail->mNext = mDocText;
                mDocTextTail = mDocTextTail->mNext;
                mDocTextTail->mNext = 0;
            }else {
                mDocTextHeader = mDocTextTail = mDocText;
                mDocText->mNext = 0;
            }

            mDocText = p;
            mDocTextOffset = 0;
        }

        PR_ExitMonitor(mLock);
        return NS_OK;
    }

    const char *p = RecvBytes(*aResult, aCount);
    if (p) {
        memcpy(aBuf, p, *aResult);

        if (mRecvCurBlockType == bt_Document) {
            // Save the data for possible late use
            DocText *dt = (DocText *)PR_Malloc(sizeof(DocText) + *aResult - sizeof(void*));
            dt->mLen = *aResult;
            memcpy(dt->mText, p, *aResult);
            dt->mNext = nsnull;

            if (!mDocTextHeader)
                mDocTextHeader = mDocTextTail = dt;
            else {
                mDocTextTail->mNext = dt;
                mDocTextTail = mDocTextTail->mNext;
            }
        }

    } else
        *aResult = 0;

    PR_ExitMonitor(mLock);
    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::ReadSegments(nsWriteSegmentFun aWriter, void* aClosure,
                         PRUint32 aCount, PRUint32* aResult)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
FCGXStream::IsSocketValid(PRBool *val)
{
    *val = (mFD != INVALID_SOCKET);
     return NS_OK;
}

NS_IMETHODIMP
FCGXStream::IsNonBlocking(PRBool *aNonBlocking)
{
    *aNonBlocking = PR_FALSE;
    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::IsOpen(PRBool *_retval)
{
    *_retval = (mFD != INVALID_SOCKET);
    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::NextRequest(PRInt32 *_retval)
{
    //TODO : return a different error code indicating protocol error
    char error[256];

    if (!RecvNextBlock(PR_FALSE)) {
        g_recv.LogProtocolError(mRecvBuf + mRecvOut, mRecvIn - mRecvOut);
        return NS_ERROR_ABORT;
    }

    if (mRecvCurBlockType != bt_BeginRequest) {
        LogFatal("Expected BeginRequest not received.");
        g_recv.LogProtocolError(mRecvBuf + mRecvOut, mRecvIn - mRecvOut);
        CloseConnection(true);
        return NS_ERROR_ABORT;
    }

    // Verify protocol version.
    PRUint16 version = RecvUint16();
    *_retval = RecvByte();
    PRBool bCanHandleRequest = (*_retval == RT_Handler || *_retval == RT_Filter);
    PRBool bCanHandleProtocol = (version == JAXER_PROTOCOL_VERSION || version == 3);
    if (!bCanHandleProtocol)
    {
        sprintf(error, 
            "Connector protocol version (%d) does not match jaxer's (%d).  You need to update the web connector or Jaxer.",
            version, JAXER_PROTOCOL_VERSION);
        LogFatal(error);
    }
    if (!bCanHandleRequest)
    {
        sprintf(error, 
            "Jaxer does not understand the request type (%d).  You need to update the web connector or Jaxer.",
            *_retval);
        LogFatal(error);
    }

    mPostDataAfterEndRequest = PR_FALSE;

    /*
     * Send back a msg indicating whether we are ok to handle the request
     */
    SendBlockType(bt_BeginRequest);
    //SendUint16(JAXER_PROTOCOL_VERSION); // protocol version
    SendUint16(bCanHandleProtocol? version : JAXER_PROTOCOL_VERSION); // protocol version
    SendByte((bCanHandleRequest && bCanHandleProtocol) ? 1 : 0); //whether we can handle the protocol
    if (!bCanHandleRequest || !bCanHandleProtocol)
    {
        //optionally, we can provide error code (INT32, A short text msg)
        if (!bCanHandleProtocol)
        {
            SendUint16(bre_ProtocolVersionMustBeTheSame);
        }else
        {
            SendUint16(bre_CannotHandleRequestType);
        }
        SendUint16(strlen(error));
        SendBytes(error, strlen(error));
    }

    nsresult rc = Flush(!bCanHandleRequest || !bCanHandleProtocol);
    if ( rc != NS_OK) 
        return rc;

    return (mFD != INVALID_SOCKET && (bCanHandleRequest && bCanHandleProtocol))? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::GetRequestHeaderCount(PRInt32 *_retval)
{
    if (PeekNextBlockType() == bt_RequestHeader) {
        RecvNextBlock(PR_FALSE);
        *_retval = RecvUint16();
    } else {
        *_retval = 0;
    }
    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

nsresult
FCGXStream::GetHeader(nsACString &name, nsACString &value)
{
    RecvString(name);
    RecvString(value);
    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::GetRequestHeader(nsACString &name, nsACString &value)
{
    return GetHeader(name, value);
}

NS_IMETHODIMP
FCGXStream::GetResponseHeaderCount(PRInt32 *_retval)
{
    if (PeekNextBlockType() == bt_ResponseHeader) {
        RecvNextBlock(PR_FALSE);
        *_retval = RecvUint16();
    } else {
        *_retval = 0;
    }
    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::GetResponseHeader(nsACString &name, nsACString &value)
{
    return GetHeader(name, value);
}

NS_IMETHODIMP
FCGXStream::GetEnvVarCount(PRInt32 *_retval)
{
    if (PeekNextBlockType() == bt_Environment) {
        RecvNextBlock(PR_FALSE);
        *_retval = RecvUint16();
    } else {
        *_retval = 0;
    }
    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::GetEnvVar(nsACString &name, nsACString &value)
{
    RecvString(name);
    RecvString(value);
    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::BeginDocumentReceive()
{
    if (!RecvNextBlock(PR_FALSE)) {
        return NS_ERROR_ABORT;
    }

    if (mRecvCurBlockType != bt_Document) {
        LogFatal("Stream not positioned at Document block.");
        g_recv.LogProtocolError(mRecvBuf + mRecvOut, mRecvIn - mRecvOut);
        CloseConnection(true);
        return NS_ERROR_ABORT;
    }

    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::RequestPostData()
{
    // Grab the lock and don't release it until document receive is resumed.
    // This will block Mozilla's input pump while we're fetching the post data
    // on the connection.
    PR_EnterMonitor(mLock);

    if (mRecvCurBlockType != bt_Document && mRecvCurBlockType != bt_EndRequest) {
        LogFatal("Stream not positioned correctly for requesting post data.");
        CloseConnection(true);
        return NS_ERROR_ABORT;
    }

    // Send a RequirePostData block to the web server.
    SendBlockType(bt_RequirePostData);
    Flush(false);
    if (mFD == INVALID_SOCKET)
        return NS_ERROR_ABORT;

    // If we're in the middle of receiving the document, save what's left of the
    // current block and continue receiving and saving document text until a
    // PostData block finally arrives.
    if (mRecvCurBlockType == bt_Document) {
        PRUint32 len = 0;
        while (const char *p = RecvBytes(len)) {
            DocText *dt = (DocText *)PR_Malloc(sizeof(DocText) + len - sizeof(void*));
            dt->mNext = mDocText;
            dt->mLen = len;
            memcpy(dt->mText, p, len);
            mDocText = dt;
        }
        if (mFD == INVALID_SOCKET)
            return NS_ERROR_ABORT;
    }

    // It's possible that by the time we requested post data, the web server
    // already sent EndRequest.  This is OK; the web server was still listening
    // for RequirePostData and will send it anyway, followed by a second
    // EndRequest.
    if (PeekNextBlockType() == bt_EndRequest) {
        RecvNextBlock(PR_FALSE);

        // Mark that Endrequest was seen.
        DocText *dt = (DocText *)PR_Malloc(sizeof(DocText) - sizeof(void*));
        dt->mNext = mDocText;
        dt->mLen = 0;
        mDocText = dt;
        mPostDataAfterEndRequest = PR_TRUE;
    }

    // The DocText blocks were chained in reverse order, so go reverse them into
    // the proper order.
    
    DocText *dt = nsnull;
    while (mDocText) {
        DocText *dt2 = mDocText->mNext;
        mDocText->mNext = dt;
        dt = mDocText;
        mDocText = dt2;
    }
    mDocText = dt;

    // The next block must be a PostData, even if it's empty.
    if (!RecvNextBlock(PR_FALSE))
        return NS_ERROR_ABORT;
    if (mRecvCurBlockType != bt_PostData) {
        LogFatal("Stream not positioned at PostData block.");
        g_recv.LogProtocolError(mRecvBuf + mRecvOut, mRecvIn - mRecvOut);
        CloseConnection(true);
        return NS_ERROR_ABORT;
    }

    // Post data may now be consumed.
    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::ResumeDocumentReceive()
{
    // Make sure all post data has been consumed.
    PRUint32 len = 0;
    while (0 != RecvBytes(len)) { } ; // empty loop body
    if (mFD == INVALID_SOCKET)
        goto errorAbort;

    // Begin next block.  What we expect depends on whether EndRequest was seen
    // prior to receiving the first PostData block.  If it was, then the next
    // block must be another EndRequest; otherwise, the next block may be either
    // Document or EndRequest.  Regardless, the set-aside document text will be
    // read first (pretend the current block is Document so that will happen).
    mRecvCurBlockType = bt_Document;

    // Let the input pump start pumping again.
    PR_ExitMonitor(mLock);
    return NS_OK;

errorAbort:
    PR_ExitMonitor(mLock);
    return NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::GetEndRequest()
{
    if (!RecvNextBlock(PR_FALSE))
        return NS_ERROR_ABORT;

    if (mRecvCurBlockType != bt_EndRequest) {
        LogFatal("Stream not positioned at EndRequest block.");
        CloseConnection(true);
        return NS_ERROR_ABORT;
    }

    return NS_OK;
}

NS_IMETHODIMP
FCGXStream::BeginHTTPResponseHeaders(PRInt32 count)
{
    if (mRecvCurBlockType != bt_EndRequest) {
        LogFatal("Stream not positioned at EndRequest block.");
        CloseConnection(true);
        return NS_ERROR_ABORT;
    }

    SendBlockType(bt_ResponseHeader);
    SendUint16(count);
    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::SendHeader(const nsACString &name, const nsACString &value)
{
    SendString(name);
    SendString(value);
    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::BeginResponse()
{
    SendBlockType(bt_Document);
    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::SendResponseFragment(const char *aString, PRUint32 aLength)
{
    if (mFD != INVALID_SOCKET)
        SendBytes(aString, aLength);
    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::SendOriginalDoc()
{
    while (mDocTextHeader) {
        if (mDocTextHeader->mLen > 0) {
            if (mFD == INVALID_SOCKET)
                return NS_ERROR_ABORT;
            SendBytes(mDocTextHeader->mText, mDocTextHeader->mLen);
        }
        mDocTextHeader = mDocTextHeader->mNext;
    }

    return mFD != INVALID_SOCKET ? NS_OK : NS_ERROR_ABORT;
}

NS_IMETHODIMP
FCGXStream::EndResponse()
{
    // Delete the cached doc.
    DeleteCachedDoc();

    SendBlockType(bt_EndRequest);
    return Flush(true);
}

////////////////////////////////////////////////////////////////////////////////

PRUint8
FCGXStream::RecvByte()
{
    // See if the current block is exhausted.
    while (mRecvBytesLeftInBlock == 0) {
        if (!RecvNextBlock(PR_TRUE))
            return 0;
    }

    // Next, see if we need to receive more bytes from the socket.
    if (mRecvIn == mRecvOut) {
        mRecvIn = 0;
        FillBuffer();
        if (mFD == INVALID_SOCKET)
            return 0;
    }

    mRecvBytesLeftInBlock--;
    g_recv.LogContents(mRecvBuf + mRecvOut, 1);
    return mRecvBuf[mRecvOut++];
}

PRUint16
FCGXStream::RecvUint16()
{
    // Try fast path where both bytes are already in the buffer.
    if (mRecvBytesLeftInBlock >= 2 && mRecvIn >= mRecvOut + 2) {
        g_recv.LogContents(mRecvBuf + mRecvOut, 2);
        PRUint16 val = (mRecvBuf[mRecvOut] << 8) | mRecvBuf[mRecvOut + 1];
        mRecvOut += 2;
        mRecvBytesLeftInBlock -= 2;
        return val;
    }

    // Slower, general case.
    PRUint8 val = RecvByte();
    return (val << 8) | RecvByte();
}

void
FCGXStream::RecvString(nsACString &s)
{
    PRUint16 len = RecvUint16();
    while (len > 0) {
        PRUint32 n = 0;
        const char *p = RecvBytes(n, len);
        if (!p)
            return;
        s.Append(p, n);
        len -= n;
    }
}

void
FCGXStream::RecvBytes(char *buf, PRUint32 len)
{
    while (len > 0) {
        // See if the current block is exhausted.
        while (mRecvBytesLeftInBlock == 0) {
            if (!RecvNextBlock(PR_TRUE))
                return;
        }

        // Next, see if we need to receive more bytes from the socket.
        if (mRecvIn == mRecvOut) {
            mRecvIn = 0;
            FillBuffer();
            if (mFD == INVALID_SOCKET)
                return;
        }

        // Copy some bytes.
        PRUint32 amt = mRecvIn - mRecvOut;
        if (amt > len) amt = len;
        if (amt > mRecvBytesLeftInBlock) amt = mRecvBytesLeftInBlock;
        g_recv.LogContents(mRecvBuf + mRecvOut, amt);
        memcpy(buf, mRecvBuf + mRecvOut, amt);
        buf += amt;
        mRecvBytesLeftInBlock -= amt;
        mRecvOut += amt;
        len -= amt;
    }
}

const char *
FCGXStream::RecvBytes(PRUint32 &len, PRInt32 maxlen)
{
    // See if the current block is exhausted.
    while (mRecvBytesLeftInBlock == 0) {
        if (!RecvNextBlock(PR_TRUE))
            return 0;
    }

    // Next, see if we need to receive more bytes from the socket.
    if (mRecvIn == mRecvOut) {
        mRecvIn = 0;
        FillBuffer();
        if (mFD == INVALID_SOCKET)
            return 0;
    }

    // Return pointer to contiguous bytes and advance past them.
    PRUint32 amt = mRecvIn - mRecvOut;
    if (amt > mRecvBytesLeftInBlock) amt = mRecvBytesLeftInBlock;
    if (amt > PRUint32(maxlen)) amt = maxlen;
    const char *p = (char *)mRecvBuf + mRecvOut;
    g_recv.LogContents(mRecvBuf + mRecvOut, amt);
    mRecvBytesLeftInBlock -= amt;
    mRecvOut += amt;
    len = amt;
    return p;
}

void
FCGXStream::FillBuffer()
{
    if (mFD == INVALID_SOCKET)
        return;

    mRecvOut = 0;
    int amt;
    do {
       amt = recv(mFD, (char*)mRecvBuf + mRecvIn, FCGXBUFSIZE - mRecvIn, 0);
    } while (amt < 0 &&
#ifdef _WIN32
        WSAGetLastError() == WSAEINTR
#else
        errno == EINTR
#endif
        );
    if (amt < 0) {
        LogFatal("FCGXStream::FillBuffer(): Could not read data from socket");
        CloseConnection(true);
    } else if (amt == 0) {
        LogFatal("Unexpected connection close.");
        CloseConnection(true);
    } else {
        mRecvIn += amt;
    }
}

PRUint8
FCGXStream::PeekNextBlockType()
{
    // First make sure we have exhausted the preceding block.
    if (mRecvBytesLeftInBlock > 0) {
        LogFatal("Junk left in block.");
        CloseConnection(true);
        return bt_BadBlockType;
    }

    // A new block header is next.  Make sure we have the entire header.
    while (mRecvIn - mRecvOut < 3) {
        // Move partial header to beginning of buffer.
        mRecvIn -= mRecvOut;
        for (PRInt32 i = 0; i < mRecvIn; i++)
            mRecvBuf[i] = mRecvBuf[mRecvOut + i];

        // Try to get remainder of block header.
        FillBuffer();
        if (mFD == INVALID_SOCKET)
            return bt_BadBlockType;
    }

    // Return its block type.
    return mRecvBuf[mRecvOut];
}

bool
FCGXStream::RecvNextBlock(PRBool forbidBlockTypeChange)
{
    // See if the block type changed with this new block.
    PRUint8 nextbt = PeekNextBlockType();
    if (mFD == INVALID_SOCKET)
        return false;

    if (mRecvCurBlockType != nextbt) {
        if (forbidBlockTypeChange)
            return false;
    }

    // Capture new block info.
    mRecvCurBlockType = mRecvBuf[mRecvOut++];
    mRecvBytesLeftInBlock = (mRecvBuf[mRecvOut] << 8) | mRecvBuf[mRecvOut + 1];
    mRecvOut += 2;
    g_recv.LogHeader(BlockType(mRecvCurBlockType), mRecvBytesLeftInBlock);
    return true;
}

void
FCGXStream::SendByte(PRUint8 byte)
{
    if (mSendIn < mSendHdrOffset + 3) {
        LogFatal("Attempt to SendByte without starting a block.");
        CloseConnection(true);
        return;
    }

    if (mSendIn == FCGXBUFSIZE) {
        SendBlockType(mSendBuf[mSendHdrOffset]);
        if (mFD == INVALID_SOCKET)
            return;
    }

    mSendBuf[mSendIn++] = byte;
    g_send.LogContents(mSendBuf + mSendIn - 1, 1);
}

void FCGXStream::SendUint16(PRUint16 value)
{
    if (mSendIn < mSendHdrOffset + 3) {
        LogFatal("Attempt to SendUint16 without starting a block.");
        CloseConnection(true);
        return;
    }

    // Try fast path where both bytes will fit in the buffer.
    if (mSendIn <= FCGXBUFSIZE - 2) {
        mSendBuf[mSendIn++] = PRUint8(value >> 8);
        mSendBuf[mSendIn++] = PRUint8(value);
        g_send.LogContents(mSendBuf + mSendIn - 2, 2);
        return;
    }

    // Slower, general case.
    SendByte(PRUint8(value >> 8));
    if (mFD != INVALID_SOCKET)
        SendByte(PRUint8(value));
}

void FCGXStream::SendString(const nsACString &s)
{
    const char *p = 0;
    PRUint32 len = s.GetData(&p);
    SendUint16(len);
    if (mFD != INVALID_SOCKET)
        SendBytes(p, len);
}

void FCGXStream::SendBytes(const char *s, PRUint32 len)
{
    if (mSendIn < mSendHdrOffset + 3) {
        LogFatal("Attempt to SendBytes without starting a block.");
        CloseConnection(true);
        return;
    }

    while (len > 0) {
        if (mSendIn == FCGXBUFSIZE) {
            SendBlockType(mSendBuf[mSendHdrOffset]);
            if (mFD == INVALID_SOCKET)
                return;
        }

        PRUint32 amt = FCGXBUFSIZE - mSendIn;
        if (amt > len) amt = len;
        memcpy(mSendBuf + mSendIn, s, amt);
        g_send.LogContents(mSendBuf + mSendIn, amt);
        s += amt;
        mSendIn += amt;
        len -= amt;
    }
}

void FCGXStream::SendBlockType(PRUint8 blockType)
{
    // If there isn't enough space left for another block, flush.
    if (mSendIn > FCGXBUFSIZE * 3/4) {
        Flush(false);
        if (mFD == INVALID_SOCKET)
            return;
    }

    // Complete preceding block header.
    if (mSendIn > mSendHdrOffset) {
        PRUint16 len = mSendIn - mSendHdrOffset - 3;
        mSendBuf[mSendHdrOffset + 1] = PRUint8(len >> 8);
        mSendBuf[mSendHdrOffset + 2] = PRUint8(len);
        mSendHdrOffset = mSendIn;
    }

    // Reserve space for next block header.
    mSendIn = mSendHdrOffset + 3;
    mSendBuf[mSendHdrOffset] = blockType;
    g_send.LogHeader(BlockType(blockType));
}

nsresult FCGXStream::Flush(bool close)
{
    // Complete preceding block header.
    if (mSendIn > mSendHdrOffset) {
        PRUint16 len = mSendIn - mSendHdrOffset - 3;
        mSendBuf[mSendHdrOffset + 1] = PRUint8(len >> 8);
        mSendBuf[mSendHdrOffset + 2] = PRUint8(len);
        mSendHdrOffset = mSendIn;
        g_send.Flush();
    }


    if (mSendIn > 0) {
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
                LogFatal("FCGXStream::Flush(): Could not send data.");
                CloseConnection(true);
                return NS_ERROR_ABORT;
            }
            p += n;
            amt -= n;
        }
        mSendIn = mSendHdrOffset = 0;
    }

    if (close) {
        // Note that this doesn't really close the socket as the manager has
        // a socket handle to it also.
        CloseConnection(false);
    }

    return NS_OK;
}

NS_IMETHODIMP FCGXStream::SetWebSocket(int socket)
{
#ifdef _WIN32
    mFD = (SOCKET) socket;
#else
    mFD = socket;
#endif
    return NS_OK;
}

void FCGXStream::DeleteCachedDoc()
{
    while (mDocTextHeader) {
        DocText *dt = mDocTextHeader->mNext;
        PR_Free(mDocTextHeader);
        mDocTextHeader = dt;
    }
}

void FCGXStream::CloseConnection(bool fatal)
{
    if (mFD != INVALID_SOCKET) {
        g_recv.Flush();
        g_send.Flush();

        mSendIn = mSendHdrOffset = 0;

#ifdef _WIN32       
        closesocket(mFD);
#else
        close(mFD);
#endif
        mFD = INVALID_SOCKET;
        mRecvIn = mRecvOut = 0;

        DeleteCachedDoc();

        while (PR_ExitMonitor(mLock) == PR_SUCCESS)
            ;
    }

    if (fatal)
	{
		gJaxerLog.Terminate();
        exit(1);
	}
}
