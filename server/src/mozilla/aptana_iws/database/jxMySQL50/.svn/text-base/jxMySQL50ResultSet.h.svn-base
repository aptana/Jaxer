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
#ifndef __JXMYSQL50RESULTSET_H
#define __JXMYSQL50RESULTSET_H


#include "jxIMySQL50.h"
#include "jxIMySQL50ResultSet.h"

struct st_mysql_res;
struct st_mysql_field;

#ifndef MYSQL_ROW
typedef char **MYSQL_ROW;
#endif


#define JX_MYSQL50RESULTSET_CID \
{ 0xe9361eac, 0xaa22, 0x4e24, { 0xbd, 0xd9, 0x59, 0x0f, 0xd1, 0x8d, 0xad, 0xd8 } }

#define JX_MYSQL50RESULTSET_CONTRACTID "@aptana.com/jxMySQL50ResultSet;1"

enum eJX_RESULTSET_TYPE
{
    eRESULTSET = 1,
    eROWCOUNT  = 2,
    eNULL      = 3,
    eERROR     = 4
};


class jxMySQL50ResultSet :  public jxIMySQL50ResultSet
{
public:
    jxMySQL50ResultSet();

    // nsISupports interface
    NS_DECL_ISUPPORTS

    // jxIMySQL50ResultSet interface
    NS_DECL_JXIMYSQL50RESULTSET

private:
    ~jxMySQL50ResultSet();

    jxIMySQL50 *	mConnection;
    PRBool			mConnected;
    st_mysql_res*	mRES;
    PRInt32			mFieldCount;
    PRInt64			mRowCount;

    MYSQL_ROW		mRow;
	unsigned long		*mField_len;
	st_mysql_field		*mFields;
    eJX_RESULTSET_TYPE   m_ResultsetType;
};

#endif // __JXMYSQL50RESULTSET_H
