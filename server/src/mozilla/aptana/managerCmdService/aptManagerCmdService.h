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

#ifndef __APT_MANAGER_CMD_SERVICE_H__
#define __APT_MANAGER_CMD_SERVICE_H__

#include <prlock.h>
#include <prmon.h>
#include "nsCOMPtr.h"
#include "aptIManagerCmdService.h"

#ifdef _WIN32
#include <Winsock2.h>
#endif

#include "JaxerDefs.h"

class aptManagerCmdResponse;

// {5AEC0455-2662-4841-9769-62DA08A4FD8E}
#define APT_MANAGERCMDSERVICE_CID \
{ 0x5aec0455, 0x2662, 0x4841, { 0x97, 0x69, 0x62, 0xda, 0x8, 0xa4, 0xfd, 0x8e } }

#define APT_MANAGERCMDSERVICE_CONTRACTID \
"@aptana.com/managercmdservice;1"


class aptManagerCmdService : public aptIManagerCmdService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_APTIMANAGERCMDSERVICE

  aptManagerCmdService();

private:
  ~aptManagerCmdService();

protected:
    nsresult SendCmdToManager();
    nsresult BasicCmdCheck(const char* cmd, PRUint32 len);

    PRBool mInited;

    // lock for write to the pipe/socket to manager
    PRLock *mToMgrLock; 
    
    // lock for changing mCmdInProgress from PR_FALSE to PR_TRUE.
    // only one NeedRespCmd can be issued.
    PRLock *mCmdLock;

    /*
     * For Sync NeedRespCmd, use this to notify the end of the response data.
     * The calling thread waits for this being notified.
     */
    PRMonitor *mCmdRespMonitor;

    /*
     * The socket/handle for communicating to manager.
     */
    HANDLE  mMgrSocket;

    /*
     * Static buf for constructing and sending cmd to manager.
     * max length is limited.  This is protected by mToMgrLock.
     */
    PRUint8 mCmdBuf[CMDBUFSIZE];
    PRUint16 mCmdLen;
    
    /*
     * Whether a NeedRespCmd is already in progress.  Only one is allowed.
     */
    PRBool             mCmdInProgress;
    
    /*
     * Is the current NeedRespCmd Async?
     */
    PRBool             mIsAsync;

    /*
     * the thread that issued the NeedRespCmd
     */
    nsCOMPtr<nsIThread> mCallingThread;
    
    /*
     * cmd Response received
     */
    nsCOMPtr<aptManagerCmdResponse> mResponse;

    /*
     * callback function when responses are ready
     */
    nsCOMPtr<aptICmdCompleteCallback> mCallback;
    
};

#endif // __APT_MANAGER_CMD_SERVICE_H__
