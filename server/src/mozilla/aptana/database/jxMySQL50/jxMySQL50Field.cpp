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
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#endif

extern "C" {
#include "my_global.h"
#include "mysql.h"
}

#include "nsCOMPtr.h"
#include "nsString.h"
//#include "nsIXPConnect.h"
#include "jxMySQL50Field.h"
#include "nsIClassInfoImpl.h"


////////////////////////////////////////////////////////////////////////

NS_IMPL_ISUPPORTS1_CI(jxMySQL50Field, jxIMySQL50Field)

jxMySQL50Field::jxMySQL50Field()
{
}

jxMySQL50Field::~jxMySQL50Field()
{
}

/* [noscript] boolean init (in AString name, in AString orgName, in AString table, in AString orgTable, in AString db, in AString catalog, in AString def, in unsigned long length, in unsigned long maxLength, in unsigned long nameLength, in unsigned long orgNameLength, in unsigned long tableLength, in unsigned long orgTableLength, in unsigned long dbLength, in unsigned long catalogLength, in unsigned long defLength, in unsigned long flags, in unsigned long decimals, in unsigned long charsetnr, in unsigned long type); */
NS_IMETHODIMP jxMySQL50Field::Init( const char *name, 
									const char *orgName, 
									const char *table, 
									const char *orgTable, 
									const char *db, 
									const char *catalog, 
									const char *def, 
								   	PRUint32 length, 
								   	PRUint32 maxLength, 
								   	PRUint32 nameLength, 
								   	PRUint32 orgNameLength, 
								   	PRUint32 tableLength, 
								   	PRUint32 orgTableLength, 
								   	PRUint32 dbLength, 
								   	PRUint32 catalogLength, 
								   	PRUint32 defLength, 
								   	PRUint32 flags, 
								   	PRUint32 decimals, 
								   	PRUint32 charsetnr, 
								   	PRUint32 type, 
								   	PRBool *_retval
								   )
{
	mName 			= NS_ConvertUTF8toUTF16(name);
    mOrgName		= NS_ConvertUTF8toUTF16(orgName);
    mTable			= NS_ConvertUTF8toUTF16(table);
    mOrgTable		= NS_ConvertUTF8toUTF16(orgTable);
    mDb				= NS_ConvertUTF8toUTF16(db);
    mCatalog		= NS_ConvertUTF8toUTF16(catalog);
    mDef			= NS_ConvertUTF8toUTF16(def);

    mLength 		= length;
    mMaxLength		= maxLength; 
    mNameLength		= nameLength;
    mOrgNameLength	= orgNameLength;
    mTableLength	= tableLength;
    mOrgTableLength	= orgTableLength;
    mDbLength		= dbLength;
    mCatalogLength	= catalogLength;
    mDefLength		= defLength;
    
	// Rest the flags associated with MySQL internal-use-only bits
    mFlags			= flags & ~(PART_KEY_FLAG|GROUP_FLAG|UNIQUE_FLAG|BINCMP_FLAG);

    mDecimals		= decimals;
    mCharsetnr		= charsetnr;
    mType			= type;

    return NS_OK;
}


/* readonly attribute unsigned long NOT_NULL_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetNOT_NULL_FLAG(PRUint32 *aNOT_NULL_FLAG)
{
	*aNOT_NULL_FLAG = NOT_NULL_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long PRI_KEY_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetPRI_KEY_FLAG(PRUint32 *aPRI_KEY_FLAG)
{
	*aPRI_KEY_FLAG = PRI_KEY_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long UNIQUE_KEY_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetUNIQUE_KEY_FLAG(PRUint32 *aUNIQUE_KEY_FLAG)
{
	*aUNIQUE_KEY_FLAG = UNIQUE_KEY_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long MULTIPLE_KEY_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetMULTIPLE_KEY_FLAG(PRUint32 *aMULTIPLE_KEY_FLAG)
{
	*aMULTIPLE_KEY_FLAG = MULTIPLE_KEY_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long BLOB_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetBLOB_FLAG(PRUint32 *aBLOB_FLAG)
{
	*aBLOB_FLAG = BLOB_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long UNSIGNED_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetUNSIGNED_FLAG(PRUint32 *aUNSIGNED_FLAG)
{
	*aUNSIGNED_FLAG = UNSIGNED_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long ZEROFILL_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetZEROFILL_FLAG(PRUint32 *aZEROFILL_FLAG)
{
	*aZEROFILL_FLAG = ZEROFILL_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long BINARY_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetBINARY_FLAG(PRUint32 *aBINARY_FLAG)
{
	*aBINARY_FLAG = BINARY_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long ENUM_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetENUM_FLAG(PRUint32 *aENUM_FLAG)
{
	*aENUM_FLAG = ENUM_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long AUTO_INCREMENT_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetAUTO_INCREMENT_FLAG(PRUint32 *aAUTO_INCREMENT_FLAG)
{
	*aAUTO_INCREMENT_FLAG = AUTO_INCREMENT_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long TIMESTAMP_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetTIMESTAMP_FLAG(PRUint32 *aTIMESTAMP_FLAG)
{
	*aTIMESTAMP_FLAG = TIMESTAMP_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long SET_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetSET_FLAG(PRUint32 *aSET_FLAG)
{
	*aSET_FLAG = SET_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long NO_DEFAULT_VALUE_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetNO_DEFAULT_VALUE_FLAG(PRUint32 *aNO_DEFAULT_VALUE_FLAG)
{
	*aNO_DEFAULT_VALUE_FLAG = NO_DEFAULT_VALUE_FLAG;
    return NS_OK;
}

/* readonly attribute unsigned long NUM_FLAG; */
NS_IMETHODIMP jxMySQL50Field::GetNUM_FLAG(PRUint32 *aNUM_FLAG)
{
	*aNUM_FLAG = NUM_FLAG;
    return NS_OK;
}




/* AString name (); */
NS_IMETHODIMP jxMySQL50Field::Name(nsAString & _retval)
{
	_retval.Assign(mName);
  	return NS_OK;
}

/* AString orgName (); */
NS_IMETHODIMP jxMySQL50Field::OrgName(nsAString & _retval)
{
	_retval.Assign(mOrgName);
  	return NS_OK;
}

/* AString table (); */
NS_IMETHODIMP jxMySQL50Field::Table(nsAString & _retval)
{
	_retval.Assign(mTable);
  	return NS_OK;
}

/* AString orgTable (); */
NS_IMETHODIMP jxMySQL50Field::OrgTable(nsAString & _retval)
{
	_retval.Assign(mOrgTable);
  	return NS_OK;
}

/* AString db (); */
NS_IMETHODIMP jxMySQL50Field::Db(nsAString & _retval)
{
	_retval.Assign(mDb);
  	return NS_OK;
}

/* AString catalog (); */
NS_IMETHODIMP jxMySQL50Field::Catalog(nsAString & _retval)
{
	_retval.Assign(mCatalog);
  	return NS_OK;
}

/* AString def (); */
NS_IMETHODIMP jxMySQL50Field::Def(nsAString & _retval)
{
	_retval.Assign(mDef);
  	return NS_OK;
}

/* unsigned long length (); */
NS_IMETHODIMP jxMySQL50Field::Length(PRUint32 *_retval)
{
	*_retval = mLength;
  	return NS_OK;
}

/* unsigned long maxLength (); */
NS_IMETHODIMP jxMySQL50Field::MaxLength(PRUint32 *_retval)
{
	*_retval = mMaxLength;
  	return NS_OK;
}

/* unsigned long nameLength (); */
NS_IMETHODIMP jxMySQL50Field::NameLength(PRUint32 *_retval)
{
	*_retval = mNameLength;
  	return NS_OK;
}

/* unsigned long orgNameLength (); */
NS_IMETHODIMP jxMySQL50Field::OrgNameLength(PRUint32 *_retval)
{
	*_retval = mOrgNameLength;
  	return NS_OK;
}

/* unsigned long tableLength (); */
NS_IMETHODIMP jxMySQL50Field::TableLength(PRUint32 *_retval)
{
	*_retval = mTableLength;
  	return NS_OK;
}

/* unsigned long orgTableLength (); */
NS_IMETHODIMP jxMySQL50Field::OrgTableLength(PRUint32 *_retval)
{
	*_retval = mOrgTableLength;
  	return NS_OK;
}

/* unsigned long dbLength (); */
NS_IMETHODIMP jxMySQL50Field::DbLength(PRUint32 *_retval)
{
	*_retval = mDbLength;
  	return NS_OK;
}

/* unsigned long catalogLength (); */
NS_IMETHODIMP jxMySQL50Field::CatalogLength(PRUint32 *_retval)
{
	*_retval = mCatalogLength;
  	return NS_OK;
}

/* unsigned long defLength (); */
NS_IMETHODIMP jxMySQL50Field::DefLength(PRUint32 *_retval)
{
	*_retval = mDefLength;
  	return NS_OK;
}

/* unsigned long flags (); */
NS_IMETHODIMP jxMySQL50Field::Flags(PRUint32 *_retval)
{
	*_retval = mFlags;
  	return NS_OK;
}

/* unsigned long decimals (); */
NS_IMETHODIMP jxMySQL50Field::Decimals(PRUint32 *_retval)
{
	*_retval = mDecimals;
  	return NS_OK;
}

/* unsigned long charsetnr (); */
NS_IMETHODIMP jxMySQL50Field::Charsetnr(PRUint32 *_retval)
{
	*_retval = mCharsetnr;
  	return NS_OK;
}

/* unsigned long type (); */
NS_IMETHODIMP jxMySQL50Field::Type(PRUint32 *_retval)
{
	*_retval = mType;
  	return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Field::TypeName(nsAString& aDeclType)
{
	char *declType = 0;
    switch (mType) {
    case MYSQL_TYPE_STRING:
    case MYSQL_TYPE_VARCHAR:
    case MYSQL_TYPE_ENUM:
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_BIT:
        declType = "string";
        break;

    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
    case MYSQL_TYPE_BLOB:
        declType = "blob";
        break;

    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_TINY:
    case MYSQL_TYPE_SHORT:
    case MYSQL_TYPE_LONG:
    case MYSQL_TYPE_FLOAT:
    case MYSQL_TYPE_DOUBLE:
    case MYSQL_TYPE_LONGLONG:
    case MYSQL_TYPE_INT24:
    case MYSQL_TYPE_NEWDECIMAL:
    case MYSQL_TYPE_YEAR:
        declType = "double";
        break;

    case MYSQL_TYPE_DATETIME:
    case MYSQL_TYPE_TIMESTAMP:
        declType = "datetime";
        break;

    case MYSQL_TYPE_DATE:
    case MYSQL_TYPE_NEWDATE:
        declType = "date";
        break;

    case MYSQL_TYPE_TIME:
        declType = "time";
        break;

    case MYSQL_TYPE_NULL:
        declType = "null";

    case MYSQL_TYPE_SET:
    case MYSQL_TYPE_GEOMETRY:
    default:
        declType = "unknown";
        break;
    }
    
    aDeclType.AssignASCII(declType);
    
	return NS_OK;
}


