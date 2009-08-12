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

#ifndef __APT_MANAGER_CMD_RESPONSE_H__
#define __APT_MANAGER_CMD_RESPONSE_H__

#include <prlock.h>
#include <prmon.h>
#include "aptIManagerCmdResponse.h"

#ifdef _WIN32
#include <Winsock2.h>
#endif

#include "JaxerDefs.h"

// {D76A84D2-0F6F-4544-9F3E-4F323509255F}
#define APT_MANAGERCMDRESPONSE_CID \
{ 0xd76a84d2, 0xf6f, 0x4544, { 0x9f, 0x3e, 0x4f, 0x32, 0x35, 0x9, 0x25, 0x5f } }

#define APT_MANAGERCMDRESPONSE_CONTRACTID \
"@aptana.com/managercmdresponse;1"

struct RespList
{
    RespList         *m_next;
    int              m_len;
    PRUint8          *m_data;

    RespList() : m_next(0), m_len(0), m_data(0)
    {
    }
    ~RespList()
    {
        if (m_data)
            delete[] m_data;  
    }
};

class aptManagerCmdResponse : public aptIManagerCmdResponse
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_APTIMANAGERCMDRESPONSE

  aptManagerCmdResponse();
  nsresult SetErrorText(const PRUint8* errorText, PRUint16 len);
  nsresult SetResponseData(const PRUint8* data, PRUint16 len);
  void SetFailed(PRBool failed) { mFailed = failed; }
  
  PRBool hasMore() { return (mRespData != 0);}

  // This is a little misleading.  It unpackages a message and
  // add it to the mRespData list.
  nsresult Init(const PRUint8* p);

private:
  ~aptManagerCmdResponse();

protected:
    // main response data.  failed response msg has only one element.
    RespList *mRespData;

    PRBool   mFailed;
    PRUint16 mErrorCode;
};


#endif // __APT_MANAGER_CMD_RESPONSE_H__
