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
#ifndef __JXMYSQL50FIELD_H
#define __JXMYSQL50FIELD_H

#include "jxIMySQL50.h"
#include "jxIMySQL50Field.h"
#include "nsString.h"

#define JX_MYSQL50FIELD_CID \
{ 0x13d6acde, 0x6b59, 0x4eb8, { 0x82, 0x1e, 0x22, 0x2f, 0x9a, 0x44, 0x12, 0xd1 } }


#define JX_MYSQL50FIELD_CONTRACTID "@aptana.com/jxMySQL50Field;1"

class jxMySQL50Field : public jxIMySQL50Field
{
public:
    jxMySQL50Field();

    // nsISupports interface
    NS_DECL_ISUPPORTS

    // jxMySQL50Field interface
    NS_DECL_JXIMYSQL50FIELD

private:
    ~jxMySQL50Field();

    nsString 	mName;
    nsString 	mOrgName;
    nsString 	mTable;
    nsString 	mOrgTable;
    nsString 	mDb;
    nsString 	mCatalog;
    nsString 	mDef;

    PRInt32 	mLength;
    PRInt32 	mMaxLength;
    PRInt32 	mNameLength;
    PRInt32 	mOrgNameLength;
    PRInt32 	mTableLength;
    PRInt32 	mOrgTableLength;
    PRInt32 	mDbLength;
    PRInt32 	mCatalogLength;
    PRInt32 	mDefLength;
    PRInt32 	mFlags;
    PRInt32 	mDecimals;
    PRInt32 	mCharsetnr;
    PRInt32 	mType;
};

#endif // __JXMYSQL50FIELD_H
