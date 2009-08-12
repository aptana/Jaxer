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
#ifndef __JXMYSQL50STATEMENT_H
#define __JXMYSQL50STATEMENT_H

#include "jxIMySQL50.h"
#include "jxIMySQL50Statement.h"

struct st_mysql_bind;
struct st_mysql_stmt;
struct st_mysql_time;

#define JX_MYSQL50STATEMENT_CID \
{ 0x849a85e9, 0xaec1, 0x4fe6, { 0x9f, 0x3d, 0xad, 0x50, 0xd2, 0xc, 0xd1, 0x93 } }


#define JX_MYSQL50STATEMENT_CONTRACTID "@aptana.com/jxMySQL50Statement;1"


class jxMySQL50Statement :  public jxIMySQL50Statement
{
public:
    jxMySQL50Statement();

    // nsISupports interface
    NS_DECL_ISUPPORTS

    // jxIMySQL50Statement interface
    NS_DECL_JXIMYSQL50STATEMENT


private:
    ~jxMySQL50Statement();

	nsresult BindOutput();

    jxIMySQL50 *	mConnection;
    PRBool			mConnected;
    st_mysql_stmt* 	mSTMT;
    PRInt64			mRowCount;
    MYSQL_RES*      mRES;

	struct InBind
	{
		PRInt32			mCount;

		st_mysql_bind*	mBIND;

		PRInt32 * 		mBindArrayType;

		PRBool * 		mBindArrayBufferTYPE_BOOL;
		double * 		mBindArrayBufferTYPE_DOUBLE;
		char * *		mBindArrayBufferTYPE_STRING;
		st_mysql_time**	mBindArrayBufferTYPE_DATE;
		unsigned long * mBindArrayBufferTYPE_STRING_LEN_IN;

		InBind();
		~InBind();

		nsresult Allocate();
		void Free();
	};

	struct OutBind: public InBind
	{
		unsigned long* 	mBindArrayBufferTYPE_STRING_LEN_OUT;
		PRBool *		mBindArrayIsNull;

		OutBind();
		~OutBind();

		nsresult Allocate();
		void Free();
		void Free_();
	};

	InBind			mIn;
	OutBind			mOut;
    nsresult        mErrno;
};

#endif // __JXMYSQL50STATEMENT_H