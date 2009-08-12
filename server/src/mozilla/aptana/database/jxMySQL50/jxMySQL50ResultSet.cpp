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
#include "nsIXPConnect.h"
#include "jxMySQL50ResultSet.h"
#include "jxMySQL50Field.h"
#include "nsIClassInfoImpl.h"

#include "jxMySQL50Defs.h"

#define SET_ERROR_RETURN(e) \
    if (mConnection==nsnull) return JX_MYSQL50_ERROR_NOT_CONNECTED; \
    mConnection->SetErrorNumber(e); return e;

////////////////////////////////////////////////////////////////////////

jxMySQL50ResultSet::jxMySQL50ResultSet()
{
	mConnection 	= nsnull;
    mConnected		= PR_FALSE;
	mRES 			= nsnull;
    mFieldCount		= 0;
    mRowCount		= 0;
    m_ResultsetType = eNULL;
}

jxMySQL50ResultSet::~jxMySQL50ResultSet()
{
	if (mRES)
	{
        if (mConnection != nsnull)
        {
            if (mConnected)
            {
                mConnection->SaveMySQLError();
            }
            NS_RELEASE(mConnection);
        }
		Close();
	}

}

NS_IMETHODIMP jxMySQL50ResultSet::SetType(PRUint32 type)
{
    m_ResultsetType = (eJX_RESULTSET_TYPE)type;
    return NS_OK;
}

NS_IMETHODIMP jxMySQL50ResultSet::GetType(PRUint32 *type)
{
    *type = m_ResultsetType;
    return NS_OK;
}


NS_IMPL_ISUPPORTS1_CI(jxMySQL50ResultSet, jxIMySQL50ResultSet)

/* [noscript] void setMySQL50 (in jxIMySQL50 aObj); */
NS_IMETHODIMP jxMySQL50ResultSet::SetMySQL50(jxIMySQL50 *aObj)
{
	mConnection = aObj;
    NS_ADDREF(mConnection);

    return NS_OK;
}


/* [noscript] boolean hasRes (); */
NS_IMETHODIMP jxMySQL50ResultSet::HasRes(PRBool *_retval)
{
	if (! mRES)	{ 
		*_retval = PR_FALSE;
	}
	else
	{
		*_retval = PR_TRUE;
	}
	return NS_OK;
}



/* void close (); */
NS_IMETHODIMP jxMySQL50ResultSet::Close()
{
	if (mRES) 
	{ 
		mysql_free_result(mRES);
		mRES = nsnull;
		mConnected = PR_FALSE;
	}

	return NS_OK;
}


/* long currentField (); */
NS_IMETHODIMP jxMySQL50ResultSet::CurrentField(PRInt32 *_retval)
{
	if (! mRES)
    {
        SET_ERROR_RETURN(JX_MYSQL50_NULL_RESULTSET);
    }

	*_retval = (PRInt32)mysql_field_tell(mRES);
    
    return NS_OK;
}




/* void dataSeek (in unsigned long long offset); */
NS_IMETHODIMP jxMySQL50ResultSet::DataSeek(PRInt64 offset, PRBool *_retval)
{
	if (! mRES)	
    {
        SET_ERROR_RETURN (JX_MYSQL50_NULL_RESULTSET);
    }

	if (offset < 0 || offset >= mRowCount) 
	{
		*_retval = PR_FALSE;
		return NS_OK;
	}

	mysql_data_seek(mRES, offset);
	
	*_retval = PR_TRUE;
	return NS_OK;
}



/* AString info (); */
NS_IMETHODIMP jxMySQL50ResultSet::Info(nsAString & _retval)
{
	MYSQL * mysql;
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

	mConnection->GetMysql(&mysql);

	_retval.Assign(NS_ConvertUTF8toUTF16(mysql_info(mysql)));

  	return NS_OK;
}




/* [noscript] void storeResult (); */
NS_IMETHODIMP jxMySQL50ResultSet::StoreResult(MYSQL * aMYSQL)
{
//	MYSQL * mysql;
//	mConnection->GetMysql(&mysql);

    Close();

	mRES = mysql_store_result(aMYSQL);

    if (mRES)  // there are rows
    {
		mConnected = PR_TRUE;

        mFieldCount = mysql_num_fields(mRES);

		mRowCount = mysql_affected_rows(aMYSQL);
        SetType(eRESULTSET);

        // Caller should now retrieve rows, then call close();
    }
    else  // mysql_store_result() returned nothing; should it have?
    {
		mConnected = PR_FALSE;

		mFieldCount = mysql_field_count(aMYSQL);

        if( mFieldCount == 0)
        {
            // query does not return data
            // (it was not a SELECT)
			mRowCount = mysql_affected_rows(aMYSQL);
            SetType(eROWCOUNT);
        }
        else // mysql_store_result() should have returned data
        {
			mRowCount = -1;
			mFieldCount = -1;
            SetType(eERROR);
	
            SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
	        // Caller should now retrieve error info by calling errno() or error()
        }
    }

	
    return NS_OK;
}




/* [noscript] void getMetaData (); */
NS_IMETHODIMP jxMySQL50ResultSet::GetMetaData(MYSQL_STMT * aSTMT)
{
	MYSQL * aMYSQL;
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN(JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

	mConnection->GetMysql(&aMYSQL);

    Close();

	mRES = mysql_stmt_result_metadata(aSTMT);

    if (mRES)  // there are rows
    {
		mConnected = PR_TRUE;

        mFieldCount = mysql_num_fields(mRES);

		mRowCount = mysql_affected_rows(aMYSQL);
        SetType(eRESULTSET);

        // Caller should now retrieve rows, then call close();
    }
    else  // mysql_store_result() returned nothing; should it have?
    {
		mConnected = PR_FALSE;

		mFieldCount = mysql_field_count(aMYSQL);

        if( mFieldCount == 0)
        {
            // query does not return data
            // (it was not a SELECT)
			mRowCount = mysql_affected_rows(aMYSQL);
            SetType(eROWCOUNT);
        }
        else // mysql_store_result() should have returned data
        {
			mRowCount = -1;
			mFieldCount = -1;
            SetType(eERROR);
            SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
	
	        // Caller should now retrieve error info by calling errno() or error()
        }
    }

	
    return NS_OK;
}


/* unsigned long errno (); */
NS_IMETHODIMP jxMySQL50ResultSet::Errno(PRUint32 *aErrno)
{
	PRBool connected;
    nsresult rv;
    if (mConnected == nsnull ||
        NS_FAILED((rv = mConnection->GetConnected(&connected))) ||
        !connected ||
        NS_FAILED((rv = mConnection->Errno(aErrno))))
    {
        *aErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
    }

    return NS_OK;
}


/* AString error (); */
NS_IMETHODIMP jxMySQL50ResultSet::Error(nsAString & aError)
{
    PRBool connected;
    nsresult rv;
    if (mConnected == nsnull ||
        NS_FAILED((rv = mConnection->GetConnected(&connected))) ||
        !connected ||
        NS_FAILED((rv = mConnection->Error(aError))))
    {
        aError.AssignLiteral("Failed to get error or no connection");
    }

    return NS_OK;
}



/* jxIMySQL50Field fetchField (); */
NS_IMETHODIMP jxMySQL50ResultSet::FetchField(jxIMySQL50Field **aReturn)
{
	PRBool	ok;

	if (! mRES)	
    {
        SET_ERROR_RETURN (JX_MYSQL50_NULL_RESULTSET);
    }

	MYSQL_FIELD *field;

	if (!(field = mysql_fetch_field(mRES))) 
	{
   		*aReturn = nsnull;
	    return NS_OK;
	}

	// Spin up a Field object, toss an exception if we can't create
    nsCOMPtr<jxIMySQL50Field> fieldobj(do_CreateInstance(JX_MYSQL50FIELD_CONTRACTID));
    if (!fieldobj)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_CANT_CREATEINSTANCE);
    }

	// Attach ourselves to the ResultSet object so it can make calls back to us if needed
    fieldobj->Init(
    	(field->name ? field->name : ""),
    	(field->org_name ? field->org_name : ""),
    	(field->table ? field->table : ""),
    	(field->org_table ? field->org_table : ""),
    	(field->db ? field->db : ""),
    	(field->catalog ? field->catalog : ""),
    	(field->def ? field->def : ""),
		field->length,
		field->max_length,
		field->name_length,
		field->org_name_length,
		field->table_length,
		field->org_table_length,
		field->db_length,
		field->catalog_length,
		field->def_length,
		field->flags,
		field->decimals,
		field->charsetnr,
		field->type,
		&ok
    );

	// Return the Field object to our caller
   	*aReturn = fieldobj;
    NS_ADDREF(*aReturn);

    return NS_OK;
}


/* jxIMySQL50Field fetchFieldDirect (in unsigned long fieldNum); */
NS_IMETHODIMP jxMySQL50ResultSet::FetchFieldDirect(PRInt32 fieldNum, jxIMySQL50Field **aReturn)
{
	PRBool	ok;

	if (! mRES)	
    {
        SET_ERROR_RETURN (JX_MYSQL50_NULL_RESULTSET);
    }

	if (fieldNum < 0 || fieldNum >= mFieldCount) {
   		*aReturn = nsnull;
	    return NS_OK;
	}

	MYSQL_FIELD *field;

	if (!(field = mysql_fetch_field_direct(mRES, fieldNum))) 
	{
   		*aReturn = nsnull;
	    return NS_OK;
	}

	// Spin up a Field object, toss an exception if we can't create
    nsCOMPtr<jxIMySQL50Field> fieldobj(do_CreateInstance(JX_MYSQL50FIELD_CONTRACTID));
    if (!fieldobj)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_CANT_CREATEINSTANCE);
    }

	// Attach ourselves to the ResultSet object so it can make calls back to us if needed
    fieldobj->Init(
    	(field->name ? field->name : ""),
    	(field->org_name ? field->org_name : ""),
    	(field->table ? field->table : ""),
    	(field->org_table ? field->org_table : ""),
    	(field->db ? field->db : ""),
    	(field->catalog ? field->catalog : ""),
    	(field->def ? field->def : ""),
		field->length,
		field->max_length,
		field->name_length,
		field->org_name_length,
		field->table_length,
		field->org_table_length,
		field->db_length,
		field->catalog_length,
		field->def_length,
		field->flags,
		field->decimals,
		field->charsetnr,
		field->type,
		&ok
    );

	// Return the Field object to our caller
   	*aReturn = fieldobj;
    NS_ADDREF(*aReturn);

    return NS_OK;
}



/* void getArray (out unsigned long count, [array, size_is (count), retval] out long retv); */
NS_IMETHODIMP jxMySQL50ResultSet::FetchFields(PRUint32 *count, jxIMySQL50Field ***retv)
{
	PRBool		ok;
	PRInt32		i;

	if (! mRES)	
    {
        SET_ERROR_RETURN (JX_MYSQL50_NULL_RESULTSET);
    }

	MYSQL_FIELD	*fields;

	if (!(fields = mysql_fetch_fields(mRES))) 
	{
   		*retv = nsnull;
	    return NS_OK;
	}

    *count = mFieldCount;
    nsCOMPtr<jxIMySQL50Field> fo;
    *retv = (jxIMySQL50Field**)nsMemory::Alloc(*count * sizeof(*fo));
    if (! *retv)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
    }

	for (i = 0; i < mFieldCount; i++) 
	{
		// Spin up a Field object, toss an exception if we can't create
	    nsCOMPtr<jxIMySQL50Field> fieldobj(do_CreateInstance(JX_MYSQL50FIELD_CONTRACTID));
	    if (!fieldobj)
        {
            SET_ERROR_RETURN (JX_MYSQL50_ERROR_CANT_CREATEINSTANCE);
        }

		// Attach ourselves to the ResultSet object so it can make calls back to us if needed
	    fieldobj->Init(
	    	(fields[i].name ? fields[i].name : ""),
	    	(fields[i].org_name ? fields[i].org_name : ""),
	    	(fields[i].table ? fields[i].table : ""),
	    	(fields[i].org_table ? fields[i].org_table : ""),
	    	(fields[i].db ? fields[i].db : ""),
	    	(fields[i].catalog ? fields[i].catalog : ""),
	    	(fields[i].def ? fields[i].def : ""),
			fields[i].length,
			fields[i].max_length,
			fields[i].name_length,
			fields[i].org_name_length,
			fields[i].table_length,
			fields[i].org_table_length,
			fields[i].db_length,
			fields[i].catalog_length,
			fields[i].def_length,
			fields[i].flags,
			fields[i].decimals,
			fields[i].charsetnr,
			fields[i].type,
			&ok
	    );

    	(*retv)[i] = fieldobj;
	    NS_ADDREF((*retv)[i]);
	}

    return NS_OK;
}


/* bResult is TRUE if a row is fetched */
NS_IMETHODIMP jxMySQL50ResultSet::FetchRow(PRBool *bResult)
{
	*bResult = PR_FALSE;

	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

	PRBool connected;
	nsresult rv = mConnection->GetConnected(&connected);
    if (NS_FAILED(rv) || !connected)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    } 
	
    if (! mRES)
    {
        SET_ERROR_RETURN (JX_MYSQL50_NULL_RESULTSET);
    }

	// Fetch the next row from the server
    mRow = mysql_fetch_row(mRES);

	*bResult = (mRow != NULL);


    if ( ! *bResult )
        return NS_OK;

    mFields = mysql_fetch_fields(mRES);
	mField_len = mysql_fetch_lengths(mRES);

    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50ResultSet::GetIsNull(PRUint32 aIndex, PRBool *_retval)
{
    if (!mConnection)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (aIndex < 0 || aIndex >= mFieldCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    
    *_retval = (! mRow[aIndex] || mField_len[aIndex] == 0);

    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50ResultSet::GetDouble(PRUint32 aIndex, double *_retval)
{
    if (!mConnection)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (aIndex < 0 || aIndex >= mFieldCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    char buf[30];
    memset(buf, 0, 30);

    memcpy(buf, mRow[aIndex], mField_len[aIndex]);
    
	// fields[i].name

	*_retval = atof(buf);
    
    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50ResultSet::GetDatetimeString(PRUint32 aIndex, nsAString &_retval)
{
    return GetString(aIndex, _retval);
}

NS_IMETHODIMP
jxMySQL50ResultSet::GetDateString(PRUint32 aIndex, nsAString &_retval)
{
    return GetString(aIndex, _retval);
}

NS_IMETHODIMP
jxMySQL50ResultSet::GetTimeString(PRUint32 aIndex, nsAString &_retval)
{
    return GetString(aIndex, _retval);
}

NS_IMETHODIMP
jxMySQL50ResultSet::GetString(PRUint32 aIndex, nsAString &_retval)
{
    if (!mConnection)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (aIndex < 0 || aIndex >= mFieldCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    
    if (mRow[aIndex] && mField_len[aIndex]) {
        //_retval.AssignASCII(mRow[aIndex], mField_len[aIndex]);
        //_retval.Assign((const PRUnichar*) (mRow[aIndex]), mField_len[aIndex]/2);
        _retval = NS_ConvertUTF8toUTF16(mRow[aIndex], mField_len[aIndex]);
    }else {
        // null columns get IsVoid set to distinguish them from empty strings
        _retval.Truncate(0);
        _retval.SetIsVoid(PR_TRUE);
    }

    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50ResultSet::GetUTF8String(PRUint32 aIndex, nsACString &_retval)
{
    if (!mConnection)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (aIndex < 0 || aIndex >= mFieldCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    
    if (mRow[aIndex] && mField_len[aIndex]) {
        _retval.Assign(mRow[aIndex], mField_len[aIndex]);
    }else {
        // null columns get IsVoid set to distinguish them from empty strings
        _retval.Truncate(0);
        _retval.SetIsVoid(PR_TRUE);
    }

    
    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50ResultSet::GetUTF8Text(PRUint32 aIndex, nsACString &_retval)
{
    return GetUTF8String(aIndex, _retval);
}

/* void getBlob(in unsigned long aIndex, out unsigned long aDataSize, [array,size_is(aDataSize)] out octet aData); */
NS_IMETHODIMP
jxMySQL50ResultSet::GetBlob(PRUint32 aIndex, PRUint32* aDataSize, PRUint8 **aData)
{
    if (!mConnection)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (aIndex < 0 || aIndex >= mFieldCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    
    *aDataSize = mField_len[aIndex];
    *aData = static_cast<PRUint8*>(nsMemory::Alloc(*aDataSize));
    memcpy(*aData, mRow[aIndex], *aDataSize);
    
    return NS_OK;
}

/* unsigned long fieldCount (); */
NS_IMETHODIMP jxMySQL50ResultSet::FieldCount(PRUint32 *aCount)
{
	if (!mConnection)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

	PRBool connected;
	nsresult rv;
    if (NS_FAILED((rv= mConnection->GetConnected(&connected))) || !connected)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

	*aCount = mFieldCount;

    return NS_OK;
}


/* unsigned long long fieldSeek (in unsigned long long offset); */
NS_IMETHODIMP jxMySQL50ResultSet::FieldSeek(PRUint64 offset, PRUint64 *_retval)
{
	if (! mRES)
    {
        SET_ERROR_RETURN (JX_MYSQL50_NULL_RESULTSET);
    }

	*_retval = (PRUint64)mysql_field_seek(mRES, (MYSQL_FIELD_OFFSET)offset);
    
    return NS_OK;
}



/* unsigned long rowCount (); */
NS_IMETHODIMP jxMySQL50ResultSet::RowCount(PRInt64 *aCount)
{
	if (!mConnection)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

	PRBool connected;
	nsresult rv;
    if (NS_FAILED((rv= mConnection->GetConnected(&connected))) || !connected)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

	*aCount = mRowCount;

    return NS_OK;
}



/* unsigned long long rowSeek (in unsigned long long offset); */
NS_IMETHODIMP jxMySQL50ResultSet::RowSeek(PRUint64 offset, PRUint64 *_retval)
{
	if (! mRES)
    {
        SET_ERROR_RETURN (JX_MYSQL50_NULL_RESULTSET);
    }

	*_retval = (PRUint64)mysql_row_seek(mRES, (MYSQL_ROW_OFFSET)offset);
    
    return NS_OK;
}


/* long long currentRow (); */
NS_IMETHODIMP jxMySQL50ResultSet::CurrentRow(PRInt64 *_retval)
{
	if (! mRES)
    {
        SET_ERROR_RETURN (JX_MYSQL50_NULL_RESULTSET);
    }

	*_retval = (PRUint64)mysql_row_tell(mRES);
    
    return NS_OK;
}


/* boolean listFields (in AString table, in AString wild); */
NS_IMETHODIMP jxMySQL50ResultSet::ListFields(const nsAString & table, const nsAString & wild, PRBool *_retval)
{
	// Get the table name
	char * strTable = ToNewUTF8String(table);

	// Get the wildcard value name
	char * strWild = ToNewUTF8String(wild);

	// Get our data structure
	MYSQL * mysql;
	mConnection->GetMysql(&mysql);

	// Execute the SQL statement
	mRES = mysql_list_fields(mysql, strTable, strWild);

    nsMemory::Free(strTable);
	nsMemory::Free(strWild);

    if (mRES) {
		*_retval = PR_TRUE;
	}
	else {
		*_retval = PR_FALSE;
        SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
	}

    return NS_OK;
}

