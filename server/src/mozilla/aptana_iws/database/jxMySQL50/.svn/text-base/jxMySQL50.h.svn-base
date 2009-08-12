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
#ifndef __MYSQL50_H
#define __MYSQL50_H

#include "jxIMySQL50.h"

struct st_mysql;

#define JX_MYSQL50_CID \
{ 0x850de0fa, 0x0597, 0x4aa5, { 0xbf, 0x60, 0x05, 0xfb, 0xeb, 0x66, 0x5e, 0x32 } }

#define JX_MYSQL50_CONTRACTID "@aptana.com/jxMySQL50;1"


class jxMySQL50 : public jxIMySQL50 //, public jxIXPCBridge
{
public:
    jxMySQL50();

    // nsISupports interface
    NS_DECL_ISUPPORTS

    // jxIMySQL50 interface
    NS_DECL_JXIMYSQL50

private:
    ~jxMySQL50();

    st_mysql*   mConnection;
    PRBool      mConnected;
    // JSContext*  mJSContext;
    // JSObject*   mJSScope;
    // JSObject*   mJSObject;

    char*       mHost;
    char*       mUser;
    char*       mPassword;
    char*       mDatabase;
    char*       mSocket;
    PRUint32    mPort;
    PRUint32    mOptions;
    nsresult    mErrno;
    char        mMySQLError[1024];
    int         mMySQLErrno;
};


#ifdef WIN32
#define L64(x) x##i64
typedef __int64 my_longlong;
#else
#define L64(x) x##LL
typedef long long my_longlong;
#endif

#endif // __MYSQL50_H