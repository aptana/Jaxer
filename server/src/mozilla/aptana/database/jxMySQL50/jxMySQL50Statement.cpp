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
#include "errmsg.h"
}

#include "nsCOMPtr.h"
#include "nsString.h"
#include "nsPrintfCString.h"
#include "jxMySQL50Statement.h"
#include "jxMySQL50.h"
#include "jxMySQL50Field.h"
#include "jxMySQL50ResultSet.h"
#include "nsIClassInfoImpl.h"

#include "jxMySQL50Defs.h"

#define SET_ERROR_RETURN(e) \
    mErrno = e; \
    if (mConnection==nsnull) return JX_MYSQL50_ERROR_NOT_CONNECTED; \
    mConnection->SetErrorNumber(e); return e;

////////////////////////////////////////////////////////////////////////

jxMySQL50Statement::jxMySQL50Statement()
{
	mConnection 				= nsnull;
    mConnected					= PR_FALSE;
	mSTMT 						= nsnull;
    mRowCount					= 0;
	mErrno                      = NS_OK;
    mRES                        = nsnull;
}

jxMySQL50Statement::~jxMySQL50Statement()
{
	if (mSTMT)
	{
        // preserve the mysql error if any
        if(mConnection != nsnull)
        {
            mConnection->SaveMySQLError();
            NS_RELEASE(mConnection);
        }

		Close();
	}
}

jxMySQL50Statement::InBind::InBind()
{
	mCount                              = -1;
	mBIND 								= nsnull;
    mBindArrayType						= nsnull;
  	mBindArrayBufferTYPE_BOOL 			= nsnull;
  	mBindArrayBufferTYPE_DOUBLE 		= nsnull;
  	mBindArrayBufferTYPE_STRING			= nsnull;
  	mBindArrayBufferTYPE_DATE			= nsnull;
  	mBindArrayBufferTYPE_STRING_LEN_IN	= nsnull;
}

jxMySQL50Statement::OutBind::OutBind()
{
  	mBindArrayBufferTYPE_STRING_LEN_OUT	= nsnull;
  	mBindArrayIsNull		 			= nsnull;
}

jxMySQL50Statement::InBind::~InBind()
{
	Free();
}

jxMySQL50Statement::OutBind::~OutBind()
{
	Free_();
}

nsresult jxMySQL50Statement::InBind::Allocate()
{
    PRUint32 	size;

	size = mCount * sizeof(MYSQL_BIND);
	if (! (mBIND = static_cast<MYSQL_BIND*>(nsMemory::Alloc(size))))
    {
		return JX_MYSQL50_ERROR_OUT_OF_MEMORY;
	}

	memset(mBIND, 0, size);

	// Also allocate array to hold the parameter binding types
	size = mCount * sizeof(PRInt32*);
	if (! (mBindArrayType = static_cast<PRInt32*>(nsMemory::Alloc(size))))
    {
		return JX_MYSQL50_ERROR_OUT_OF_MEMORY;
	}
	memset(mBindArrayType, 0, size);

	// Also allocate array to hold the actual content that is of type Integer
	size = mCount * sizeof(PRBool);
	if (! (mBindArrayBufferTYPE_BOOL = static_cast<PRBool*>(nsMemory::Alloc(size))))
    {
		return JX_MYSQL50_ERROR_OUT_OF_MEMORY;
	}
	memset(mBindArrayBufferTYPE_BOOL, 0, size);

	// Also allocate array to hold the actual content that is of type longlong
	size = mCount * sizeof(double);
	if (! (mBindArrayBufferTYPE_DOUBLE = static_cast<double*>(nsMemory::Alloc(size))))
    {
		return JX_MYSQL50_ERROR_OUT_OF_MEMORY;
	}
	memset(mBindArrayBufferTYPE_DOUBLE, 0, size);

	// Also allocate array to hold pointers to the actual content that is of type String
	size = mCount * sizeof(char**);
	if (! (mBindArrayBufferTYPE_STRING = static_cast<char**>(nsMemory::Alloc(size))))
    {
		return JX_MYSQL50_ERROR_OUT_OF_MEMORY;
	}
	memset(mBindArrayBufferTYPE_STRING, 0, size);

	// Also allocate array to hold pointers to the actual content that is of type Date
	size = mCount * sizeof(MYSQL_TIME**);
	if (! (mBindArrayBufferTYPE_DATE = static_cast<MYSQL_TIME**>(nsMemory::Alloc(size))))
    {
		return JX_MYSQL50_ERROR_OUT_OF_MEMORY;
	}
	memset(mBindArrayBufferTYPE_DATE, 0, size);

	// Also allocate array to hold pointers to the lengths of actual content that is of type String
	size = mCount * sizeof(PRInt32*);
	if (! (mBindArrayBufferTYPE_STRING_LEN_IN = static_cast<unsigned long*>(nsMemory::Alloc(size))))
    {
		return JX_MYSQL50_ERROR_OUT_OF_MEMORY;
	}
	memset(mBindArrayBufferTYPE_STRING_LEN_IN, 0, size);

	return NS_OK;
}

nsresult jxMySQL50Statement::OutBind::Allocate()
{
    PRUint32 	size;

	PRInt32 rc = InBind::Allocate();
	if (rc != NS_OK)
    {
		return rc;
    }

	size = mCount * sizeof(PRInt32*);
	if (! (mBindArrayBufferTYPE_STRING_LEN_OUT = static_cast<unsigned long*>(nsMemory::Alloc(size))))
    {
		return JX_MYSQL50_ERROR_OUT_OF_MEMORY;
	}
	memset(mBindArrayBufferTYPE_STRING_LEN_OUT, 0, size);

	// Also allocate array to hold the is_null indicators
	size = mCount * sizeof(PRBool);
	if (! (mBindArrayIsNull = static_cast<PRBool*>(nsMemory::Alloc(size))))
    {
		return JX_MYSQL50_ERROR_OUT_OF_MEMORY;
	}
	memset(mBindArrayIsNull, 0, size);

	return NS_OK;
}

void jxMySQL50Statement::InBind::Free()
{
	if (mBIND != nsnull)
	{
		nsMemory::Free(mBIND);
		mBIND = nsnull;
	}

	if (mBindArrayType != nsnull)
	{
		nsMemory::Free(mBindArrayType);
		mBindArrayType = nsnull;
	}

	if (mBindArrayBufferTYPE_BOOL != nsnull)
	{
		nsMemory::Free(mBindArrayBufferTYPE_BOOL);
		mBindArrayBufferTYPE_BOOL = nsnull;
	}

	if (mBindArrayBufferTYPE_DOUBLE != nsnull)
	{
		nsMemory::Free(mBindArrayBufferTYPE_DOUBLE);
		mBindArrayBufferTYPE_DOUBLE = nsnull;
	}

	if (mBindArrayBufferTYPE_STRING != nsnull)
	{
		for (PRInt32 i = 0 ; i < mCount ; i++)
		{
			if (mBindArrayBufferTYPE_STRING[i] != nsnull) { nsMemory::Free(mBindArrayBufferTYPE_STRING[i]); }
		}

		nsMemory::Free(mBindArrayBufferTYPE_STRING);
		mBindArrayBufferTYPE_STRING = nsnull;
	}

	if (mBindArrayBufferTYPE_DATE != nsnull)
	{
		for (PRInt32 i = 0 ; i < mCount ; i++)
		{
			if (mBindArrayBufferTYPE_DATE[i] != nsnull) { nsMemory::Free(mBindArrayBufferTYPE_DATE[i]); }
		}

		nsMemory::Free(mBindArrayBufferTYPE_DATE);
		mBindArrayBufferTYPE_DATE = nsnull;
	}

	if (mBindArrayBufferTYPE_STRING_LEN_IN != nsnull)
	{
		nsMemory::Free(mBindArrayBufferTYPE_STRING_LEN_IN);
		mBindArrayBufferTYPE_STRING_LEN_IN = nsnull;
	}
}

void jxMySQL50Statement::OutBind::Free()
{
	InBind::Free();
	Free_();
}

void jxMySQL50Statement::OutBind::Free_()
{
	if (mBindArrayBufferTYPE_STRING_LEN_OUT != nsnull)
	{
		nsMemory::Free(mBindArrayBufferTYPE_STRING_LEN_OUT);
		mBindArrayBufferTYPE_STRING_LEN_OUT = nsnull;
	}

	if (mBindArrayIsNull != nsnull)
	{
		nsMemory::Free(mBindArrayIsNull);
		mBindArrayIsNull = nsnull;
	}
}


NS_IMPL_ISUPPORTS1_CI(jxMySQL50Statement, jxIMySQL50Statement)

/* [noscript] void setMySQL50 (in jxIMySQL50 aObj); */
NS_IMETHODIMP jxMySQL50Statement::SetMySQL50(jxIMySQL50 *aObj)
{
	mConnection = aObj;
    NS_ADDREF(mConnection);
    return NS_OK;
}


nsresult jxMySQL50Statement::BindOutput()
{
	PRInt32		ndx;
	PRInt32		col_type;

	// Allocate the BIND array and supporting structures.
	nsresult rv = mOut.Allocate();
    if (NS_FAILED(rv))
    {
        SET_ERROR_RETURN(rv);
    }

	// Set up the BIND structure based on the column types specified by the database metadata.
	for (ndx=0; ndx < mOut.mCount ; ndx++) {
		col_type = (mSTMT->fields) ? mSTMT->fields[ndx].type : MYSQL_TYPE_STRING;
		switch (col_type) {
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_DOUBLE:
			case MYSQL_TYPE_NEWDECIMAL:
			case MYSQL_TYPE_FLOAT:
			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_YEAR:
				mOut.mBindArrayType[ndx] = MYSQL_TYPE_DOUBLE;
				
				/* allocate buffer for double */
				mOut.mBIND[ndx].buffer_type = MYSQL_TYPE_DOUBLE;
				mOut.mBIND[ndx].buffer = &(mOut.mBindArrayBufferTYPE_DOUBLE[ndx]);
				mOut.mBIND[ndx].is_null = (my_bool*)&(mOut.mBindArrayIsNull[ndx]);
				break;

			case MYSQL_TYPE_NULL:
				mOut.mBindArrayType[ndx] = MYSQL_TYPE_NULL;
				/*
				  don't initialize to 0 :
				  1. stmt->result.buf[ofs].buflen
				  2. bind[ofs].buffer
				  3. bind[ofs].buffer_length
				  because memory was allocated with ecalloc
				*/
				mOut.mBIND[ndx].buffer_type = MYSQL_TYPE_NULL;
				mOut.mBIND[ndx].is_null = (my_bool*)&(mOut.mBindArrayIsNull[ndx]);
				break;

			case MYSQL_TYPE_STRING:
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_BIT:
				{
					unsigned long	tmp = 0;
					if (mSTMT->fields[ndx].max_length == 0 && !mysql_stmt_attr_get(mSTMT, STMT_ATTR_UPDATE_MAX_LENGTH, &tmp) && !tmp)
					{
						mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx] = (mSTMT->fields) ? (mSTMT->fields[ndx].length) ? mSTMT->fields[ndx].length + 1: 256: 256;
					} else {
						/*
							the user has called store_result(). if he does not there is no way to determine the
							libmysql does not allow us to allocate 0 bytes for a buffer so we try 1
						*/
						if (!(mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx] = mSTMT->fields[ndx].max_length))
							++mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx];
					}
					mOut.mBindArrayType[ndx] = MYSQL_TYPE_STRING;
					mOut.mBIND[ndx].buffer_type = MYSQL_TYPE_STRING;
		    		mOut.mBindArrayBufferTYPE_STRING[ndx] = static_cast<char*>(nsMemory::Alloc(mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx])) ;
					if (!mOut.mBindArrayBufferTYPE_STRING[ndx])
                    {
                        SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
                    }
                    mOut.mBIND[ndx].buffer = mOut.mBindArrayBufferTYPE_STRING[ndx];
					mOut.mBIND[ndx].is_null = (my_bool*)&(mOut.mBindArrayIsNull[ndx]);
					mOut.mBIND[ndx].buffer_length = mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx];
					mOut.mBIND[ndx].length = &(mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[ndx]);
				}
				break;

			case MYSQL_TYPE_BLOB:
			case MYSQL_TYPE_TINY_BLOB:
			case MYSQL_TYPE_MEDIUM_BLOB:
			case MYSQL_TYPE_LONG_BLOB:
				{
#if 0
					PRUint32	tmp = 0;
					if (mSTMT->fields[ndx].max_length == 0 && !mysql_stmt_attr_get(mSTMT, STMT_ATTR_UPDATE_MAX_LENGTH, &tmp) && !tmp)
					{
						if (mSTMT->fields && mSTMT->fields[ndx].length)
							tmp = mSTMT->fields[ndx].length + 1;

						// tmp will be zero if a length isn't available or if it was the maximum possible of 4GB-1.  In
						// this case, supply a suitable length.

                        // NB. tmp may not always be 4GB-1, so if we do not want support larger than 16MB, we should try
                        // to prevent it.
						if (tmp == 0 || tmp > 16*1024*1024) {
							switch (col_type) {
							case MYSQL_TYPE_BLOB:			tmp =      64*1024;		break;
							case MYSQL_TYPE_TINY_BLOB:		tmp =          256;		break;
							case MYSQL_TYPE_MEDIUM_BLOB:    tmp = 16*1024*1024;		break;
							case MYSQL_TYPE_LONG_BLOB:		tmp = 16*1024*1024;		break;
							}
						}
						mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx] = tmp;
					} else {
						/*
							the user has called store_result(). if he does not there is no way to determine the
							libmysql does not allow us to allocate 0 bytes for a buffer so we try 1
						*/
						if (!(mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx] = mSTMT->fields[ndx].max_length))
							++mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx];
					}
					mOut.mBindArrayType[ndx] = MYSQL_TYPE_BLOB;
					mOut.mBIND[ndx].buffer_type = MYSQL_TYPE_BLOB;
		    		mOut.mBindArrayBufferTYPE_STRING[ndx] = static_cast<char*>(nsMemory::Alloc(mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx])) ;
                    if (!mOut.mBindArrayBufferTYPE_STRING[ndx])
                    {
                        SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
                    }
					mOut.mBIND[ndx].buffer = mOut.mBindArrayBufferTYPE_STRING[ndx];
					mOut.mBIND[ndx].is_null = (my_bool*)&(mOut.mBindArrayIsNull[ndx]);
					mOut.mBIND[ndx].buffer_length = mOut.mBindArrayBufferTYPE_STRING_LEN_IN[ndx];
					mOut.mBIND[ndx].length = (unsigned long*)&(mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[ndx]);
#endif
                    mOut.mBindArrayType[ndx] = MYSQL_TYPE_BLOB;
					mOut.mBIND[ndx].buffer_type = MYSQL_TYPE_BLOB;
		    		mOut.mBindArrayBufferTYPE_STRING[ndx] = 0;
					mOut.mBIND[ndx].buffer = mOut.mBindArrayBufferTYPE_STRING[ndx];
					mOut.mBIND[ndx].is_null = (my_bool*)&(mOut.mBindArrayIsNull[ndx]);
					mOut.mBIND[ndx].buffer_length = 0;
                    mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[ndx] = 0;
					mOut.mBIND[ndx].length = &(mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[ndx]);

				}
				break;

			case MYSQL_TYPE_DATETIME:
			case MYSQL_TYPE_TIME:
			case MYSQL_TYPE_DATE:
			case MYSQL_TYPE_TIMESTAMP:
                if (col_type == MYSQL_TYPE_TIMESTAMP )
				    mOut.mBindArrayType[ndx] = MYSQL_TYPE_DATETIME;
                else
                    mOut.mBindArrayType[ndx] = col_type;

			    mOut.mBindArrayBufferTYPE_DATE[ndx] = static_cast<MYSQL_TIME*>(nsMemory::Alloc(sizeof(MYSQL_TIME)));
                if (!mOut.mBindArrayBufferTYPE_DATE[ndx])
                {
                    SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
                }

				
				/* allocate buffer for DATE */
				mOut.mBIND[ndx].buffer_type = MYSQL_TYPE_DATETIME;
				mOut.mBIND[ndx].buffer = (void*)mOut.mBindArrayBufferTYPE_DATE[ndx];
				mOut.mBIND[ndx].buffer_length = sizeof(MYSQL_TIME);
				mOut.mBIND[ndx].length = (unsigned long*)&(mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[ndx]);
				mOut.mBIND[ndx].is_null = (my_bool*)&(mOut.mBindArrayIsNull[ndx]);
				break;


			default:
				SET_ERROR_RETURN (JX_MYSQL50_ERROR_INVALID_TYPE);
				break;
		}
	}

	PRUint32 rc = mysql_stmt_bind_result(mSTMT, mOut.mBIND);

	if (rc)
    {
        SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
	} else
    {
        if (mRES)
        {
            mysql_free_result(mRES);
            mRES = nsnull;
        }
         
		mRES = mysql_stmt_result_metadata(mSTMT);
        if (mRES)
        {
            if (mysql_stmt_store_result(mSTMT))
            {
#if 0
                int i=0;
                unsigned int en = mysql_stmt_errno(mSTMT);
                const char* es = mysql_stmt_error(mSTMT);
                // Not sure why this would fail.  Will need to be investigated
                // Per DOC, this needs to be called earlier.
                // SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
#endif
            }
        }
		return NS_OK;
	}
}




/* void close (); */
NS_IMETHODIMP jxMySQL50Statement::Close()
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }
	if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }	

    mysql_free_result(mRES);

	if(mysql_stmt_close(mSTMT))
    {
        SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
    }

	mSTMT = nsnull;

	mIn.Free();
	mOut.Free();

    return NS_OK;
}


/* unsigned long errno (); */
NS_IMETHODIMP jxMySQL50Statement::Errno(PRUint32 *_retval)
{
	if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

	//*_retval = mysql_stmt_errno(mSTMT);

    if (mErrno == JX_MYSQL50_MYSQL_ERROR)
    {
        *_retval = mysql_stmt_errno(mSTMT);
    }else
        *_retval = mErrno;

  	return NS_OK;
}


/* AString error (); */
NS_IMETHODIMP jxMySQL50Statement::Error(nsAString & aError)
{
	if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (mErrno == JX_MYSQL50_MYSQL_ERROR)
    {
        const char *sError =mysql_stmt_error(mSTMT);
        aError.Assign(NS_ConvertUTF8toUTF16(sError));
        return NS_OK;
    }

    NON_MYSQL_ERROR(mErrno, aError);
    if (aError.Length() == 0)
    {
        const char* me = mysql_stmt_error(mSTMT);
        AppendUTF8toUTF16(nsPrintfCString(0x1000, "Unknown error 0x%0x.  Last MYSQL error is %s", mErrno, (me && *me) ? me : ""), aError);
    }

	//_retval.Assign(NS_ConvertUTF8toUTF16(mysql_stmt_error(mSTMT)));
  	return NS_OK;
}


static PRInt32 lenOfMySQL_TIME = sizeof(MYSQL_TIME);

NS_IMETHODIMP jxMySQL50Statement::Execute(PRUint32 *_retval)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }


	*_retval = 1;

    // Assume the input parameters are all inited.

	// Get access to the array of parameter values and verify its length.

	if (mysql_stmt_bind_param(mSTMT, mIn.mBIND))
	{
        //Error
		*_retval = 1;
        SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
	}

    // Bind output fields if not done already.
	if (!mOut.mBIND && mOut.mCount > 0)
    {
		nsresult rv = BindOutput();
        if (NS_FAILED(rv))
        {
			*_retval = 1;
			SET_ERROR_RETURN (rv);
		}
	}

    if (mysql_stmt_execute(mSTMT)) 
	{
        *_retval = 1;
		SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
	}

	

	*_retval = 0;

    return NS_OK;
}

#if 0
/* jxIMySQL50ResultSet storeResult(); */
NS_IMETHODIMP
jxMySQL50Statement::StoreResult(jxIMySQL50ResultSet **aReturn)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (! aReturn )
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NULL_POINTER);
    }

	// Spin up a ResultSet object, toss an exception if we can't create
    nsCOMPtr<jxIMySQL50ResultSet> res(do_CreateInstance(JX_MYSQL50RESULTSET_CONTRACTID));
    if (!res)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_CANT_CREATEINSTANCE);
    }

	// Attach ourselves to the ResultSet object so it can make calls back to us if needed
    // res->SetMySQL50(mConnection);

    MYSQL *mysql;
    mConnection->GetMysql(&mysql);

    res->SetMySQL50(mConnection);
    nsresult rv = res->StoreResult(mysql);
    if (NS_FAILED(rv))
    {
        SET_ERROR_RETURN (rv);
    }


	// Return the ResultSet object to our caller
   	*aReturn = res;
    NS_ADDREF(*aReturn);

	return NS_OK;
}
#endif


/*
 *
 */
NS_IMETHODIMP
jxMySQL50Statement::BindDatetimeParameter(PRUint32 aParamIndex, PRUint32 aSecond)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mIn.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    

    if (mIn.mBindArrayBufferTYPE_DATE[aParamIndex] == nsnull)
        mIn.mBindArrayBufferTYPE_DATE[aParamIndex] = static_cast<MYSQL_TIME*>(nsMemory::Alloc(sizeof(MYSQL_TIME)));
    if (mIn.mBindArrayBufferTYPE_DATE[aParamIndex] == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
    }

    time_t t = (time_t) aSecond;
    struct tm *tmptm = gmtime(&t);
    // struct tm *tmpltm = localtime(&t);

#if 0
    PRTime t = aSecond * 1000;
    PRExplodedTime prtime;
    PR_ExplodeTime(t, PR_LocalTimeParameters, &prtime);
#endif

    MYSQL_TIME*  mysqlTime = mIn.mBindArrayBufferTYPE_DATE[aParamIndex];
	mysqlTime->year 		= tmptm->tm_year + 1900;
	mysqlTime->month 		= tmptm->tm_mon + 1;
	mysqlTime->day 			= tmptm->tm_mday;
	mysqlTime->hour 		= tmptm->tm_hour;
	mysqlTime->minute 		= tmptm->tm_min;
	mysqlTime->second 		= tmptm->tm_sec;
	mysqlTime->second_part 	= 0;	/* mic's not supported in MySQL yet */
	mysqlTime->neg 			= 0;
	mysqlTime->time_type 	= MYSQL_TIMESTAMP_DATETIME;

	mIn.mBIND[aParamIndex].buffer_type = MYSQL_TYPE_DATETIME;
	mIn.mBIND[aParamIndex].buffer 	 = mIn.mBindArrayBufferTYPE_DATE[aParamIndex];
	mIn.mBIND[aParamIndex].length 	 = 0;
	mIn.mBIND[aParamIndex].length_value 	 = lenOfMySQL_TIME;
	mIn.mBIND[aParamIndex].buffer_length = lenOfMySQL_TIME;

    return NS_OK;
}


/* void BindBooleanParameter (in unsigned long aParamIndex, in boolean aValue); */
NS_IMETHODIMP
jxMySQL50Statement::BindBooleanParameter(PRUint32 aParamIndex, PRBool aValue)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mIn.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    

    mIn.mBindArrayBufferTYPE_BOOL[aParamIndex] = aValue;
	mIn.mBIND[aParamIndex].buffer_type = MYSQL_TYPE_TINY;
	mIn.mBIND[aParamIndex].buffer = &(mIn.mBindArrayBufferTYPE_BOOL[aParamIndex]);
	mIn.mBIND[aParamIndex].length = 0;
    mIn.mBIND[aParamIndex].is_null = 0;

    return NS_OK;
}

/* void bindUTF8TextParameter(in unsigned long aParamIndex, in AUTF8String aValue); */
NS_IMETHODIMP
jxMySQL50Statement::BindUTF8TextParameter(PRUint32 aParamIndex, const nsACString & aValue)
{
    return BindUTF8StringParameter(aParamIndex, aValue);
}

/* void bindUTF8StringParameter (in unsigned long aParamIndex, in AUTF8String aValue); */
NS_IMETHODIMP
jxMySQL50Statement::BindUTF8StringParameter(PRUint32 aParamIndex, const nsACString & aValue)
{
    
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mIn.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    

    char* strp = (char*)(nsPromiseFlatCString(aValue).get());
    PRUint32 nBytes = strlen(strp);

	mIn.mBindArrayBufferTYPE_STRING_LEN_IN[aParamIndex] = nBytes;
    mIn.mBindArrayBufferTYPE_STRING[aParamIndex] = static_cast<char*>(nsMemory::Alloc(nBytes+1));
    if (!mIn.mBindArrayBufferTYPE_STRING[aParamIndex])
    {
		SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
	}
    memcpy(mIn.mBindArrayBufferTYPE_STRING[aParamIndex], strp, nBytes);

	mIn.mBIND[aParamIndex].buffer_type = MYSQL_TYPE_STRING;
	mIn.mBIND[aParamIndex].buffer = mIn.mBindArrayBufferTYPE_STRING[aParamIndex];

	mIn.mBIND[aParamIndex].length 	 = 0;
	mIn.mBIND[aParamIndex].length_value 	 = nBytes;
	mIn.mBIND[aParamIndex].buffer_length = nBytes;
    mIn.mBIND[aParamIndex].is_null = 0;

    return NS_OK;
}

/* void bindStringParameter (in unsigned long aParamIndex, in AString aValue); */
NS_IMETHODIMP
jxMySQL50Statement::BindStringParameter(PRUint32 aParamIndex, const nsAString & aValue)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mIn.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    

    //char *strp = (char*)(nsPromiseFlatString(aValue).get());
    nsCAutoString cstr = NS_ConvertUTF16toUTF8(aValue);
    const char *strp = cstr.get();
    PRUint32 nBytes = strlen(strp);

    mIn.mBindArrayBufferTYPE_STRING_LEN_IN[aParamIndex] = nBytes;

    mIn.mBindArrayBufferTYPE_STRING[aParamIndex] = 
        static_cast<char*>(nsMemory::Alloc(nBytes+1));
    if (!mIn.mBindArrayBufferTYPE_STRING[aParamIndex])
    {
		SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
	}
    memcpy(mIn.mBindArrayBufferTYPE_STRING[aParamIndex], strp, nBytes+1);

	mIn.mBIND[aParamIndex].buffer_type = MYSQL_TYPE_STRING;
	mIn.mBIND[aParamIndex].buffer = mIn.mBindArrayBufferTYPE_STRING[aParamIndex];

	//mIn.mBIND[aParamIndex].length 	 = (unsigned long*)&(mIn.mBindArrayBufferTYPE_STRING_LEN_IN[aParamIndex]);
	mIn.mBIND[aParamIndex].length 	 = 0;
	mIn.mBIND[aParamIndex].length_value 	 = nBytes;
	mIn.mBIND[aParamIndex].buffer_length = nBytes;

    return NS_OK;
}

/* void bindDoubleParameter (in unsigned long aParamIndex, in double aValue); */
NS_IMETHODIMP
jxMySQL50Statement::BindDoubleParameter(PRUint32 aParamIndex, double aValue)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mIn.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    

	mIn.mBindArrayBufferTYPE_DOUBLE[aParamIndex] = aValue;
	mIn.mBIND[aParamIndex].buffer_type = MYSQL_TYPE_DOUBLE;
	mIn.mBIND[aParamIndex].buffer = (char*)&(mIn.mBindArrayBufferTYPE_DOUBLE[aParamIndex]);
	mIn.mBIND[aParamIndex].length = 0;
    mIn.mBIND[aParamIndex].is_null = 0;

    return NS_OK;
}

/* void bindInt32Parameter (in unsigned long aParamIndex, in long aValue); */
NS_IMETHODIMP
jxMySQL50Statement::BindInt32Parameter(PRUint32 aParamIndex, PRInt32 aValue)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mIn.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    

	mIn.mBindArrayBufferTYPE_DOUBLE[aParamIndex] = aValue;
	mIn.mBIND[aParamIndex].buffer_type = MYSQL_TYPE_DOUBLE; // MYSQL_TYPE_LONG;
	mIn.mBIND[aParamIndex].buffer = (char*)&(mIn.mBindArrayBufferTYPE_DOUBLE[aParamIndex]);
	mIn.mBIND[aParamIndex].length = 0;
    mIn.mBIND[aParamIndex].is_null = 0;

    return NS_OK;
}

/* void bindInt64Parameter (in unsigned long aParamIndex, in long long aValue); */
NS_IMETHODIMP
jxMySQL50Statement::BindInt64Parameter(PRUint32 aParamIndex, PRInt64 aValue)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mIn.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

	mIn.mBindArrayBufferTYPE_DOUBLE[aParamIndex] = aValue;
	mIn.mBIND[aParamIndex].buffer_type = MYSQL_TYPE_DOUBLE; // MYSQL_TYPE_LONGLONG;
	mIn.mBIND[aParamIndex].buffer = (char*) &(mIn.mBindArrayBufferTYPE_DOUBLE[aParamIndex]);
	mIn.mBIND[aParamIndex].length = 0;
    mIn.mBIND[aParamIndex].is_null = 0;

    return NS_OK;
}

/* void bindNullParameter (in unsigned long aParamIndex); */
NS_IMETHODIMP
jxMySQL50Statement::BindNullParameter(PRUint32 aParamIndex)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mIn.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

	mIn.mBindArrayBufferTYPE_BOOL[aParamIndex] = 1;
	mIn.mBIND[aParamIndex].buffer_type = MYSQL_TYPE_NULL;
	mIn.mBIND[aParamIndex].buffer = (char*)&(mIn.mBindArrayBufferTYPE_BOOL[aParamIndex]);
	mIn.mBIND[aParamIndex].length = 0;

    // This may not work
    mIn.mBIND[aParamIndex].is_null = (my_bool*) &(mIn.mBindArrayBufferTYPE_BOOL[aParamIndex]);

    return NS_OK;
}

/* void bindBlobParameter (in unsigned long aParamIndex, [array, const, size_is (aValueSize)] in octet aValue, in unsigned long aValueSize); */
NS_IMETHODIMP
jxMySQL50Statement::BindBlobParameter(PRUint32 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mIn.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    mIn.mBindArrayBufferTYPE_STRING_LEN_IN[aParamIndex] = aValueSize;

    mIn.mBindArrayBufferTYPE_STRING[aParamIndex] = 
        static_cast<char*>(nsMemory::Alloc(aValueSize));
    if (!mIn.mBindArrayBufferTYPE_STRING[aParamIndex])
    {
		SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
	}

    memcpy(mIn.mBindArrayBufferTYPE_STRING[aParamIndex], (void*)aValue, aValueSize);

    mIn.mBindArrayBufferTYPE_STRING_LEN_IN[aParamIndex] = aValueSize;
	mIn.mBIND[aParamIndex].buffer_type = MYSQL_TYPE_BLOB;
	mIn.mBIND[aParamIndex].buffer = mIn.mBindArrayBufferTYPE_STRING[aParamIndex];

	mIn.mBIND[aParamIndex].length 	 = 0;
	mIn.mBIND[aParamIndex].length_value 	 = mIn.mBindArrayBufferTYPE_STRING_LEN_IN[aParamIndex];
	mIn.mBIND[aParamIndex].buffer_length = mIn.mBindArrayBufferTYPE_STRING_LEN_IN[aParamIndex];

    return NS_OK;
}


/* [noscript] long init (in AString query); */
NS_IMETHODIMP jxMySQL50Statement::Init(const nsAString & query, PRInt32 *_retval)
{
    nsresult rv = NS_OK;

	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT != nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_ALREADY_INITIALIZED);
    }

	MYSQL * mysql;
	mConnection->GetMysql(&mysql);

	// Create a new statement handle
	if ((mSTMT = mysql_stmt_init(mysql))) 
	{
		// Get the query string
		char * stmt = ToNewUTF8String(query);

		// Prepare the statement
        int ret = mysql_stmt_prepare(mSTMT, stmt, query.Length());
        nsMemory::Free(stmt);
			
		if (ret) 
		{
			// Statement prep failed
#if 0
            char err[512];
            sprintf(err, " %s", mysql_stmt_error(mSTMT));
#endif
            *_retval = ret;
            SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
		}
		else 
		{
			mIn.mCount = mysql_stmt_param_count(mSTMT);
			mOut.mCount = mysql_stmt_field_count(mSTMT);

			// Statement prep succeeded
            *_retval = 0;

            // Allocated the BIND array and supporting structures
	        if (mIn.mBIND == nsnull && mIn.mCount > 0) { 
		        rv = mIn.Allocate();
		        if (rv != NS_OK)
                {
                    *_retval = -1;
                    SET_ERROR_RETURN (rv);
                }
            }	    
		}
	}
	else {
		// Statement prep failed
		*_retval = -1;
        SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
	}

    return NS_OK;
}




/* long paramCount (); */
NS_IMETHODIMP jxMySQL50Statement::ParamCount(PRInt32 *_retval)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }
	*_retval = mIn.mCount;

    return NS_OK;
}



/* AString SQLState (); */
NS_IMETHODIMP jxMySQL50Statement::SQLState(nsAString & _retval)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

	_retval.Assign(NS_ConvertUTF8toUTF16(mysql_stmt_sqlstate(mSTMT)));
  	return NS_OK;
}



/* boolean dataSeek (in long long offset); */
NS_IMETHODIMP jxMySQL50Statement::DataSeek(PRInt64 offset, PRBool *_retval)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

	if (offset < 0 || offset >= mRowCount) 
	{
		*_retval = PR_FALSE;
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
	}

	mysql_stmt_data_seek(mSTMT, offset);
	
	*_retval = PR_TRUE;
	return NS_OK;
}


/* long long rowCount (); */
NS_IMETHODIMP jxMySQL50Statement::RowCount(PRInt64 *aCount)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

	*aCount = mysql_stmt_num_rows(mSTMT);

    return NS_OK;
}


/* unsigned long fieldCount (); */
NS_IMETHODIMP jxMySQL50Statement::FieldCount(PRUint32 *aCount)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

	*aCount = mOut.mCount;

    return NS_OK;
}


/* long long insertId (); */
NS_IMETHODIMP jxMySQL50Statement::InsertId(PRInt64 *aID)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

	*aID = mysql_stmt_insert_id(mSTMT);

    return NS_OK;
}


/* jxIMySQL50ResultSet resultMetadata (); */
NS_IMETHODIMP jxMySQL50Statement::ResultMetadata(jxIMySQL50ResultSet **aReturn)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

	// If we didn't get a valid place to return the ResultSet object pointer, 
	// then cause an exception to be returned to JavaScript
  	if (! aReturn )
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NULL_POINTER);
    }

	// Spin up a ResultSet object, toss an exception if we can't create
    nsCOMPtr<jxIMySQL50ResultSet> res(do_CreateInstance(JX_MYSQL50RESULTSET_CONTRACTID));
    if (!res)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_CANT_CREATEINSTANCE);
    }

	// Attach ourselves to the ResultSet object so it can make calls back to us if needed
    res->SetMySQL50(mConnection);

	// Capture the resultSet metadata
    nsresult rv = res->GetMetaData(mSTMT);
    if (NS_FAILED(rv))
    {
        SET_ERROR_RETURN (rv);
    }


	// Return the ResultSet object to our caller
   	*aReturn = res;
    NS_ADDREF(*aReturn);

	return NS_OK;
}


/* bool fetch (); */
NS_IMETHODIMP jxMySQL50Statement::Fetch(PRBool *_retval)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

	PRInt32	ret = mysql_stmt_fetch(mSTMT);
	*_retval = PR_FALSE;

	if (!ret || ret == MYSQL_DATA_TRUNCATED) {
		*_retval = PR_TRUE;
	} else if (ret == MYSQL_NO_DATA) {
	   	*_retval = PR_FALSE;
	} else 
    {
		// const char* err = mysql_stmt_error(mSTMT);
		PRUint32 eno = mysql_stmt_errno(mSTMT);
		if ( eno == 2050 || eno == 2051 || eno == 2052 || eno == 2053)
        {
		  // No result set
		  return NS_OK;
        }

		SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
	}

	return NS_OK;

}


NS_IMETHODIMP
jxMySQL50Statement::GetInt32(PRUint32 aIndex, PRInt32 *_retval)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    if (mOut.mBindArrayIsNull[aIndex])
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_BIND_ARRAY_IS_NULL);
    }

    if (mOut.mBindArrayType[aIndex] != MYSQL_TYPE_DOUBLE)
    {
        SET_ERROR_RETURN (JX_MYSQL50_CANNOT_CONVERT_DATA);
    }

    double *dd = (double*)(&(mOut.mBindArrayBufferTYPE_DOUBLE[aIndex]));
	*_retval = (PRInt32) *dd;
    
    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Statement::GetInt64(PRUint32 aIndex, PRInt64 *_retval)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    double *dd = (double*)(&(mOut.mBindArrayBufferTYPE_DOUBLE[aIndex]));
    PRInt64 n = (PRInt64) *dd;
	*_retval = n;
    
    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Statement::GetDouble(PRUint32 aIndex, double *_retval)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    double *dd = (double*)(&(mOut.mBindArrayBufferTYPE_DOUBLE[aIndex]));
	*_retval = *dd;
    
    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Statement::GetUTF8String(PRUint32 aIndex, nsACString &_retval)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    
    if (mOut.mBindArrayIsNull[aIndex]) {
        // null columns get IsVoid set to distinguish them from empty strings
        _retval.Truncate(0);
        _retval.SetIsVoid(PR_TRUE);
    } else {
        PRUint32 slen = mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[aIndex];
        const char* cstr = mOut.mBindArrayBufferTYPE_STRING[aIndex];
        _retval.Assign(cstr, slen);
    }

    
    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Statement::GetString(PRUint32 aIndex, nsAString & _retval)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    
    if (mOut.mBindArrayIsNull[aIndex]) {
        // null columns get IsVoid set to distinguish them from empty strings
        _retval.Truncate(0);
        _retval.SetIsVoid(PR_TRUE);
    } else {
        // int slen = sqlite3_column_bytes16 (mSqliteStatement, aIndex);
        // const PRUnichar *wstr = (const PRUnichar *) sqlite3_column_text16 (mSqliteStatement, aIndex);
        PRUint32 slen = mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[aIndex];
        // const PRUnichar* wstr =  (const PRUnichar *) mOut.mBindArrayBufferTYPE_STRING[aIndex];
        //_retval.AssignASCII(mOut.mBindArrayBufferTYPE_STRING[aIndex], slen);
        _retval = NS_ConvertUTF8toUTF16(mOut.mBindArrayBufferTYPE_STRING[aIndex], slen);
    }

    
    return NS_OK;
}


NS_IMETHODIMP
jxMySQL50Statement::GetDatetimeString(PRUint32 aIndex, nsAString &result)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    
    if (mOut.mBindArrayIsNull[aIndex]) {
        // null columns get IsVoid set to distinguish them from empty strings
        result.Truncate(0);
        result.SetIsVoid(PR_TRUE);
    } else {
        MYSQL_TIME*  mysqlTime = mOut.mBindArrayBufferTYPE_DATE[aIndex];
        char buf[30];
        sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", mysqlTime->year, mysqlTime->month,
            mysqlTime->day, mysqlTime->hour, mysqlTime->minute,
            mysqlTime->second);
        result.AssignASCII(buf);
    }

    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Statement::GetDateString(PRUint32 aIndex, nsAString &result)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    
    if (mOut.mBindArrayIsNull[aIndex]) {
        // null columns get IsVoid set to distinguish them from empty strings
        result.Truncate(0);
        result.SetIsVoid(PR_TRUE);
    } else {
        MYSQL_TIME*  mysqlTime = mOut.mBindArrayBufferTYPE_DATE[aIndex];
        char buf[30];
        sprintf(buf, "%04d-%02d-%02d", mysqlTime->year, mysqlTime->month,
            mysqlTime->day);
        result.AssignASCII(buf);
    }

    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Statement::GetTimeString(PRUint32 aIndex, nsAString &result)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    
    if (mOut.mBindArrayIsNull[aIndex]) {
        // null columns get IsVoid set to distinguish them from empty strings
        result.Truncate(0);
        result.SetIsVoid(PR_TRUE);
    } else {
        MYSQL_TIME*  mysqlTime = mOut.mBindArrayBufferTYPE_DATE[aIndex];
        char buf[30];
        sprintf(buf, "%02d:%02d:%02d", mysqlTime->hour, mysqlTime->minute,
            mysqlTime->second);
        result.AssignASCII(buf);
    }

    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Statement::GetBlob(PRUint32 aIndex, PRUint32 *aDataSize, PRUint8 **aData)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    
    int blobsize = mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[aIndex];

    if (blobsize > 0)
    {
        void *blob = static_cast<void*>(nsMemory::Alloc(blobsize));
        if (blob == NULL)
        {
            SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
        }
        my_bool is_null;
        my_bool error;
        unsigned long length;
    
        MYSQL_BIND bind;
        memset(&bind, 0, sizeof(bind));
        bind.buffer_type = MYSQL_TYPE_BLOB;
        bind.buffer= blob;
        bind.buffer_length= blobsize;
        bind.is_null= &is_null;
        bind.length= &length;
        bind.error= &error;

        if (mysql_stmt_fetch_column(mSTMT, &bind, aIndex, 0))
        {
            SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
        }


        *aData = (PRUint8*) blob;
        *aDataSize = blobsize;
    }else
    {
        *aData = 0;
        *aDataSize = 0;
    }
    return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Statement::GetUTF8Text(PRUint32 aIndex, nsACString &aData)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    
    if (mOut.mBindArrayIsNull[aIndex]) {
        // null columns get IsVoid set to distinguish them from empty strings
        aData.Truncate(0);
        aData.SetIsVoid(PR_TRUE);
    }else
    {
        int nsize = mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[aIndex];

        char *data = static_cast<char*>(nsMemory::Alloc(nsize+1));
        //extra for the null
        if (data == NULL)
        {
            SET_ERROR_RETURN (JX_MYSQL50_ERROR_OUT_OF_MEMORY);
        }

        my_bool is_null;
        my_bool error;
        unsigned long length;
    
        MYSQL_BIND bind;
        memset(&bind, 0, sizeof(bind));

        bind.buffer= (void*)data;
        bind.buffer_length= nsize;
        bind.buffer_type = MYSQL_TYPE_STRING;
        bind.is_null= &is_null;
        bind.length= &length;
        bind.error= &error;

        if (mysql_stmt_fetch_column(mSTMT, &bind, aIndex, 0))
        {
            SET_ERROR_RETURN (JX_MYSQL50_MYSQL_ERROR);
        }

        data[nsize] = 0;

        aData = nsDependentCString(data, nsize);
    }

    return NS_OK;
}
/*
S_IMETHODIMP
jxMySQL50Statement::GetUTF8String(PRUint32 aIndex, nsACString &_retval)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    
    if (mOut.mBindArrayIsNull[aIndex]) {
        // null columns get IsVoid set to distinguish them from empty strings
        _retval.Truncate(0);
        _retval.SetIsVoid(PR_TRUE);
    } else {
        PRUint32 slen = mOut.mBindArrayBufferTYPE_STRING_LEN_OUT[aIndex];
        const char* cstr = mOut.mBindArrayBufferTYPE_STRING[aIndex];
        _retval.Assign(cstr, slen);
    }

    
    return NS_OK;
}

*/

NS_IMETHODIMP
jxMySQL50Statement::GetIsNull(PRUint32 aIndex, PRBool *_retval)
{
    if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aIndex < 0 || aIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }
    
    *_retval = mOut.mBindArrayIsNull[aIndex];

    return NS_OK;
}


/* AString getColumnDecltype(in unsigned long aParamIndex); */
NS_IMETHODIMP
jxMySQL50Statement::GetColumnDecltype(PRUint32 aParamIndex, nsACString& aDeclType)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    char *declType = 0;
    switch (mOut.mBindArrayType[aParamIndex]) {
    case MYSQL_TYPE_STRING:
        declType = "string";
        break;

    case MYSQL_TYPE_BLOB:
        declType = "blob";
        break;

    case MYSQL_TYPE_DOUBLE:
        declType = "double";
        break;

    case MYSQL_TYPE_DATETIME:
        declType = "datetime";
        break;

    case MYSQL_TYPE_DATE:
        declType = "date";
        break;

    case MYSQL_TYPE_TIME:
        declType = "time";
        break;

    case MYSQL_TYPE_NULL:
        declType = "null";

    default:
        declType = "unknown";
        break;
    }
    
    aDeclType.Assign(declType);
    
	return NS_OK;
}

NS_IMETHODIMP
jxMySQL50Statement::GetColumnName(PRUint32 aParamIndex, nsACString& aName)
{
	if (mConnection == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_NOT_CONNECTED);
    }

    if (mSTMT == nsnull)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_STMT_NULL);
    }

    if (aParamIndex < 0 || aParamIndex >= mOut.mCount)
    {
        SET_ERROR_RETURN (JX_MYSQL50_ERROR_ILLEGAL_VALUE);
    }

    if (mSTMT->fields[aParamIndex].name)
        aName.Assign(mSTMT->fields[aParamIndex].name, mSTMT->fields[aParamIndex].name_length);
    else
        aName.Assign("");
	return NS_OK;
}
