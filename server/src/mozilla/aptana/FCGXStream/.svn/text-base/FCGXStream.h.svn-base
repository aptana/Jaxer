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
#ifndef FCGXStream_h__
#define FCGXStream_h__

#include "nsCOMPtr.h"
#include "nsComponentManagerUtils.h"
#include "nsIInputStream.h"
#include "nsISeekableStream.h"
#include "nsIPrefService.h"
#include "IFCGXStream.h"
#include "prmon.h"

#ifdef _WIN32
#include <Winsock2.h>
#endif

#define FCGXSTREAM_CLASSNAME \
    "FCGXStream"
#define FCGXSTREAM_CONTRACTID \
    "@aptana.com/network/fcgx-stream;1"
#define FCGXSTREAM_CID \
{ /* e380b63b-a505-4ac0-a558-d85eac0977f6 */         \
    0xe380b63b,                                      \
    0xa505,                                          \
    0x4ac0,                                          \
    {0xa5, 0x58, 0xd8, 0x5e, 0xac, 0x9, 0x77, 0xf6 } \
}

////////////////////////////////////////////////////////////////////////////////

#define FCGXBUFSIZE 8192
//#define JAXER_PROTOCOL_VERSION 3

class FCGXStream : public nsISeekableStream,
                   public IFCGXStream
{
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSISEEKABLESTREAM
    NS_DECL_NSIINPUTSTREAM
    NS_DECL_IFCGXSTREAM

    FCGXStream();
    virtual ~FCGXStream();

    //static nsresult Startup(int argc, char **argv);
    //static void Cleanup();

    // When relavent pref settings have changed, call this function to update prefs
    // It would be better to have its own observer, but for now, just use one.
    static void UpdatePrefSettings(nsIPrefBranch *aPrefBranch, const char* pref = nsnull);

    // These are the settings we are interested
    //static PRBool g_IdleLoop;
    //static PRBool g_dumpEnabled;
    
private:
    // Receive various stuff from a stream.
    PRUint8 RecvByte();
    PRUint16 RecvUint16();
    void RecvString(nsACString &s);

    // Receive multiple bytes from a stream.  Exactly len bytes are read into
    // a buffer supplied by the caller.
    void RecvBytes(char *buf, PRUint32 len);

    // Receive multiple bytes from a stream.  The maximum number of available
    // contiguous bytes is returned.  Caller must not free the returned pointer
    // and must be finished with it before the next receive operation is
    // performed.  A null pointer is returned if end-of-stream is reached (a
    // block of a different type is received).
    const char *RecvBytes(PRUint32 &len, PRInt32 maxlen = -1);

    void FillBuffer();
    PRUint8 PeekNextBlockType();
    bool RecvNextBlock(PRBool forbidBlockTypeChange);

    // Send various stuff to a stream.
    void SendByte(PRUint8 byte);
    void SendUint16(PRUint16 value);
    void SendString(const nsACString &s);
    void SendBytes(const char *s, PRUint32 len);

    // Start sending a new block of the specified type.  Any previous block
    // is completed.  If the block fills up, a new block of the same type will
    // be started automatically.
    void SendBlockType(PRUint8 blockType);

    // Force current block to be completed and sent over the socket.
    nsresult Flush(bool close);

    nsresult GetHeader(nsACString &name, nsACString &value);

    // Delete the cached document.
    void DeleteCachedDoc();

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

    // PRBool       mDoHelloExchange;

    // Receive stuff.
    PRUint16     mRecvIn;
    PRUint16     mRecvOut;
    PRUint8      mRecvCurBlockType;
    PRUint16     mRecvBytesLeftInBlock;

    // Send stuff.
    PRUint16     mSendIn;
    PRUint16     mSendHdrOffset;

    // Keep track of document text received waiting for a PostData block to
    // arrive.
    struct DocText
    {
        DocText *mNext;
        PRUint32 mLen;
        char     mText[sizeof(void*)];
    };

    DocText     *mDocText;
    PRUint32     mDocTextOffset;
    DocText     *mDocTextHeader;
    DocText     *mDocTextTail;

    PRBool       mPostDataAfterEndRequest;

    // Concurrency control.  Mozilla reads HTML on a separate thread.
    PRMonitor   *mLock;

    // Keep the big stuff at the end.
    PRUint8      mRecvBuf[FCGXBUFSIZE];
    PRUint8      mSendBuf[FCGXBUFSIZE];
};

nsresult APT_NewFCGXStream(IFCGXStream **aResult);

#endif // FCGXStream_h__
