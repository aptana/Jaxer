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
#include "nsIClassInfoImpl.h"
#include "nsString.h"
#include "nsPrintfCString.h"
#include "jxMySQL50.h"
#include "jxMySQL50ResultSet.h"
#include "jxMySQL50Statement.h"

#include "jxMySQL50Defs.h"

#include "aptICoreLog.h"

////////////////////////////////////////////////////////////////////////

jxMySQL50::jxMySQL50()
{
	mConnection = mysql_init((MYSQL *) nsnull);
	mConnected	= PR_FALSE;
	mHost		= nsnull;
	mUser		= nsnull;
	mPassword 	= nsnull;
	mDatabase	= nsnull;
	mPort		= 0;
    mSocket		= nsnull;
	mOptions	= 0;
    mErrno      = NS_OK;
    mMySQLError[0] = 0;
    mMySQLErrno = 0;
}

jxMySQL50::~jxMySQL50()
{
	// Release any held resources so we don't leak memory
	if (mConnection != nsnull)	{ mysql_close(mConnection);	}

	if (mHost != nsnull) { nsMemory::Free(mHost); }
	if (mUser != nsnull) { nsMemory::Free(mUser); }
	if (mPassword != nsnull) { nsMemory::Free(mPassword); }
	if (mDatabase != nsnull) { nsMemory::Free(mDatabase); }
    if (mSocket != nsnull) { nsMemory::Free(mSocket); }
}


// NS_IMPL_ISUPPORTS2_CI(jxMySQL50, jxIMySQL50, jxIXPCBridge)
NS_IMPL_ISUPPORTS1_CI(jxMySQL50, jxIMySQL50)

// NS_IMPL_ISUPPORTS1(jxMySQL50, jxIMySQL50)


/* [noscript] readonly attribute jxMYSQLPtr mysql; */
NS_IMETHODIMP jxMySQL50::GetMysql(MYSQL * *aMysql)
{
  	if (! aMysql )
    {
        mErrno = JX_MYSQL50_ERROR_NULL_POINTER;
        return mErrno;
    }

	*aMysql = mConnection;
    return NS_OK;
}

/* attribute AString host; */
NS_IMETHODIMP jxMySQL50::GetHost(nsAString & aHost)
{
	aHost.Assign(NS_ConvertUTF8toUTF16(mHost));
  	return NS_OK;
}

NS_IMETHODIMP jxMySQL50::SetHost(const nsAString & aHost)
{
	// process the host arg
	if (aHost.Length() == 0)
	{
		if (mHost != nsnull) { nsMemory::Free(mHost); }
		mHost = nsnull;
	}
	else {
		mHost = ToNewUTF8String(aHost);
	}
	
    return NS_OK;
}

/* attribute AString user; */
NS_IMETHODIMP jxMySQL50::GetUser(nsAString & aUser)
{
	aUser.Assign(NS_ConvertUTF8toUTF16(mUser));
  	return NS_OK;
}

NS_IMETHODIMP jxMySQL50::SetUser(const nsAString & aUser)
{
	// process the user arg
	if (aUser.Length() == 0)
	{
		if (mUser != nsnull) { nsMemory::Free(mUser); }
		mUser = nsnull;
	}
	else {
		mUser = ToNewUTF8String(aUser);
	}
	
    return NS_OK;
}

/* attribute AString password; */
NS_IMETHODIMP jxMySQL50::GetPassword(nsAString & aPassword)
{
	aPassword.Assign(NS_ConvertUTF8toUTF16(mPassword));
  	return NS_OK;
}

NS_IMETHODIMP jxMySQL50::SetPassword(const nsAString & aPassword)
{
	// process the Password arg
	if (aPassword.Length() == 0)
	{
		if (mPassword != nsnull) { nsMemory::Free(mPassword); }
		mPassword = nsnull;
	}
	else {
		mPassword = ToNewUTF8String(aPassword);
	}
	
    return NS_OK;
}

/* attribute AString database; */
NS_IMETHODIMP jxMySQL50::GetDatabase(nsAString & aDatabase)
{
	aDatabase.Assign(NS_ConvertUTF8toUTF16(mDatabase));
  	return NS_OK;
}

NS_IMETHODIMP jxMySQL50::SetDatabase(const nsAString & aDatabase)
{
	// process the Database arg
	if (aDatabase.Length() == 0)
	{
		if (mDatabase != nsnull) { nsMemory::Free(mDatabase); }
		mDatabase = nsnull;
	}
	else {
		mDatabase = ToNewUTF8String(aDatabase);
	}

	if (mysql_select_db(mConnection, mDatabase))
    {
        mErrno = JX_MYSQL50_MYSQL_ERROR;
        return mErrno;
    }
	
    return NS_OK;
}

/* attribute unsigned long port; */
NS_IMETHODIMP jxMySQL50::GetPort(PRUint32 *aPort)
{
	*aPort = mPort;
  	return NS_OK;
}

NS_IMETHODIMP jxMySQL50::SetPort(PRUint32 aPort)
{
	mPort = aPort;
  	return NS_OK;
}

/* attribute AString socket; */
NS_IMETHODIMP jxMySQL50::GetSocket(nsAString & aSocket)
{
	aSocket.Assign(NS_ConvertUTF8toUTF16(mSocket));
  	return NS_OK;
}

NS_IMETHODIMP jxMySQL50::SetSocket(const nsAString & aSocket)
{
	// process the socket arg
	if (aSocket.Length() == 0)
	{
		if (mSocket != nsnull) { nsMemory::Free(mSocket); }
		mSocket = nsnull;
	}
	else {
		mSocket = ToNewUTF8String(aSocket);
	}
	
    return NS_OK;
}

/* attribute AString characterSet; */
NS_IMETHODIMP jxMySQL50::GetCharacterSet(nsAString & aCharacterSet)
{
	aCharacterSet.Assign(NS_ConvertUTF8toUTF16(mysql_character_set_name(mConnection)));
  	return NS_OK;
}

#if 0
NS_IMETHODIMP jxMySQL50::SetCharacterSet(const nsAString & aCharacterSet)
{
	if (! mConnected)
    {
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	char * str = ToNewUTF8String(aCharacterSet);

	if (mysql_set_character_set(mConnection, str))
    {
        mErrno = JX_MYSQL50_MYSQL_ERROR;
        nsMemory::Free(str);
        return mErrno;
    }


	nsMemory::Free(str);

    return NS_OK;
}
#endif



/* attribute unsigned long options; */
NS_IMETHODIMP jxMySQL50::GetOptions(PRUint32 *aOptions)
{
	*aOptions = mOptions;
  	return NS_OK;
}


/* [noscript] readonly attribute boolean connected; */
NS_IMETHODIMP jxMySQL50::GetConnected(PRBool *aConnected)
{
	if (! mConnected) 
	{	
		*aConnected = PR_FALSE; 
	} 
	else {
		*aConnected = PR_TRUE; 
	}

    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_COMPRESS; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_COMPRESS(PRUint32 *aCLIENT_COMPRESS)
{
	*aCLIENT_COMPRESS = CLIENT_COMPRESS;
    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_FOUND_ROWS; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_FOUND_ROWS(PRUint32 *aCLIENT_FOUND_ROWS)
{
	*aCLIENT_FOUND_ROWS = CLIENT_FOUND_ROWS;
    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_IGNORE_SPACE; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_IGNORE_SPACE(PRUint32 *aCLIENT_IGNORE_SPACE)
{
	*aCLIENT_IGNORE_SPACE = CLIENT_IGNORE_SPACE;
    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_INTERACTIVE; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_INTERACTIVE(PRUint32 *aCLIENT_INTERACTIVE)
{
	*aCLIENT_INTERACTIVE = CLIENT_INTERACTIVE;
    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_LOCAL_FILES; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_LOCAL_FILES(PRUint32 *aCLIENT_LOCAL_FILES)
{
	*aCLIENT_LOCAL_FILES = CLIENT_LOCAL_FILES;
    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_MULTI_RESULTS; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_MULTI_RESULTS(PRUint32 *aCLIENT_MULTI_RESULTS)
{
	*aCLIENT_MULTI_RESULTS = CLIENT_MULTI_RESULTS;
    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_MULTI_STATEMENTS; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_MULTI_STATEMENTS(PRUint32 *aCLIENT_MULTI_STATEMENTS)
{
	*aCLIENT_MULTI_STATEMENTS = CLIENT_MULTI_STATEMENTS;
    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_NO_SCHEMA; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_NO_SCHEMA(PRUint32 *aCLIENT_NO_SCHEMA)
{
	*aCLIENT_NO_SCHEMA = CLIENT_NO_SCHEMA;
    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_ODBC; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_ODBC(PRUint32 *aCLIENT_ODBC)
{
	*aCLIENT_ODBC = CLIENT_ODBC;
    return NS_OK;
}

/* readonly attribute unsigned long CLIENT_SSL; */
NS_IMETHODIMP jxMySQL50::GetCLIENT_SSL(PRUint32 *aCLIENT_SSL)
{
	*aCLIENT_SSL = CLIENT_SSL;
    return NS_OK;
}



/* readonly attribute unsigned long OPT_CONNECT_TIMEOUT; */
NS_IMETHODIMP jxMySQL50::GetOPT_CONNECT_TIMEOUT(PRUint32 *aOPT_CONNECT_TIMEOUT)
{
	*aOPT_CONNECT_TIMEOUT = MYSQL_OPT_CONNECT_TIMEOUT;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_COMPRESS; */
NS_IMETHODIMP jxMySQL50::GetOPT_COMPRESS(PRUint32 *aOPT_COMPRESS)
{
	*aOPT_COMPRESS = MYSQL_OPT_COMPRESS;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_NAMED_PIPE; */
NS_IMETHODIMP jxMySQL50::GetOPT_NAMED_PIPE(PRUint32 *aOPT_NAMED_PIPE)
{
	*aOPT_NAMED_PIPE = MYSQL_OPT_NAMED_PIPE;
    return NS_OK;
}

/* readonly attribute unsigned long INIT_COMMAND; */
NS_IMETHODIMP jxMySQL50::GetINIT_COMMAND(PRUint32 *aINIT_COMMAND)
{
	*aINIT_COMMAND = MYSQL_INIT_COMMAND;
    return NS_OK;
}

/* readonly attribute unsigned long READ_DEFAULT_FILE; */
NS_IMETHODIMP jxMySQL50::GetREAD_DEFAULT_FILE(PRUint32 *aREAD_DEFAULT_FILE)
{
	*aREAD_DEFAULT_FILE = MYSQL_READ_DEFAULT_FILE;
    return NS_OK;
}

/* readonly attribute unsigned long READ_DEFAULT_GROUP; */
NS_IMETHODIMP jxMySQL50::GetREAD_DEFAULT_GROUP(PRUint32 *aREAD_DEFAULT_GROUP)
{
	*aREAD_DEFAULT_GROUP = MYSQL_READ_DEFAULT_GROUP;
    return NS_OK;
}

/* readonly attribute unsigned long SET_CHARSET_DIR; */
NS_IMETHODIMP jxMySQL50::GetSET_CHARSET_DIR(PRUint32 *aSET_CHARSET_DIR)
{
	*aSET_CHARSET_DIR = MYSQL_SET_CHARSET_DIR;
    return NS_OK;
}

/* readonly attribute unsigned long SET_CHARSET_NAME; */
NS_IMETHODIMP jxMySQL50::GetSET_CHARSET_NAME(PRUint32 *aSET_CHARSET_NAME)
{
	*aSET_CHARSET_NAME = MYSQL_SET_CHARSET_NAME;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_LOCAL_INFILE; */
NS_IMETHODIMP jxMySQL50::GetOPT_LOCAL_INFILE(PRUint32 *aOPT_LOCAL_INFILE)
{
	*aOPT_LOCAL_INFILE = MYSQL_OPT_LOCAL_INFILE;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_PROTOCOL; */
NS_IMETHODIMP jxMySQL50::GetOPT_PROTOCOL(PRUint32 *aOPT_PROTOCOL)
{
	*aOPT_PROTOCOL = MYSQL_OPT_PROTOCOL;
    return NS_OK;
}

/* readonly attribute unsigned long SHARED_MEMORY_BASE_NAME; */
NS_IMETHODIMP jxMySQL50::GetSHARED_MEMORY_BASE_NAME(PRUint32 *aSHARED_MEMORY_BASE_NAME)
{
	*aSHARED_MEMORY_BASE_NAME = MYSQL_SHARED_MEMORY_BASE_NAME;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_READ_TIMEOUT; */
NS_IMETHODIMP jxMySQL50::GetOPT_READ_TIMEOUT(PRUint32 *aOPT_READ_TIMEOUT)
{
	*aOPT_READ_TIMEOUT = MYSQL_OPT_READ_TIMEOUT;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_WRITE_TIMEOUT; */
NS_IMETHODIMP jxMySQL50::GetOPT_WRITE_TIMEOUT(PRUint32 *aOPT_WRITE_TIMEOUT)
{
	*aOPT_WRITE_TIMEOUT = MYSQL_OPT_WRITE_TIMEOUT;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_USE_RESULT; */
NS_IMETHODIMP jxMySQL50::GetOPT_USE_RESULT(PRUint32 *aOPT_USE_RESULT)
{
	*aOPT_USE_RESULT = MYSQL_OPT_USE_RESULT;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_USE_REMOTE_CONNECTION; */
NS_IMETHODIMP jxMySQL50::GetOPT_USE_REMOTE_CONNECTION(PRUint32 *aOPT_USE_REMOTE_CONNECTION)
{
	*aOPT_USE_REMOTE_CONNECTION = MYSQL_OPT_USE_REMOTE_CONNECTION;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_USE_EMBEDDED_CONNECTION; */
NS_IMETHODIMP jxMySQL50::GetOPT_USE_EMBEDDED_CONNECTION(PRUint32 *aOPT_USE_EMBEDDED_CONNECTION)
{
	*aOPT_USE_EMBEDDED_CONNECTION = MYSQL_OPT_USE_EMBEDDED_CONNECTION;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_GUESS_CONNECTION; */
NS_IMETHODIMP jxMySQL50::GetOPT_GUESS_CONNECTION(PRUint32 *aOPT_GUESS_CONNECTION)
{
	*aOPT_GUESS_CONNECTION = MYSQL_OPT_GUESS_CONNECTION;
    return NS_OK;
}

/* readonly attribute unsigned long SET_CLIENT_IP; */
NS_IMETHODIMP jxMySQL50::GetSET_CLIENT_IP(PRUint32 *aSET_CLIENT_IP)
{
	*aSET_CLIENT_IP = MYSQL_SET_CLIENT_IP;
    return NS_OK;
}

/* readonly attribute unsigned long SECURE_AUTH; */
NS_IMETHODIMP jxMySQL50::GetSECURE_AUTH(PRUint32 *aSECURE_AUTH)
{
	*aSECURE_AUTH = MYSQL_SECURE_AUTH;
    return NS_OK;
}

/* readonly attribute unsigned long REPORT_DATA_TRUNCATION; */
NS_IMETHODIMP jxMySQL50::GetREPORT_DATA_TRUNCATION(PRUint32 *aREPORT_DATA_TRUNCATION)
{
	*aREPORT_DATA_TRUNCATION = MYSQL_REPORT_DATA_TRUNCATION;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_RECONNECT; */
NS_IMETHODIMP jxMySQL50::GetOPT_RECONNECT(PRUint32 *aOPT_RECONNECT)
{
	*aOPT_RECONNECT = MYSQL_OPT_RECONNECT;
    return NS_OK;
}

/* readonly attribute unsigned long OPT_SSL_VERIFY_SERVER_CERT; */
NS_IMETHODIMP jxMySQL50::GetOPT_SSL_VERIFY_SERVER_CERT(PRUint32 *aOPT_SSL_VERIFY_SERVER_CERT)
{
	*aOPT_SSL_VERIFY_SERVER_CERT = MYSQL_OPT_SSL_VERIFY_SERVER_CERT;
    return NS_OK;
}




/* void autocommit (in boolean mode); */
NS_IMETHODIMP jxMySQL50::Autocommit(PRBool mode, PRBool *_retval)
{
	// If we get a non-zero return value
	if ( mysql_autocommit(mConnection, mode))
	{
		// error
    	*_retval = PR_FALSE;
        mErrno = JX_MYSQL50_MYSQL_ERROR;
		return mErrno;
	}
	// Succeeded so return TRUE
   	*_retval = PR_TRUE;
	return NS_OK;
}




/* boolean changeUser (in AString user, in AString password, in AString dbname); */
NS_IMETHODIMP jxMySQL50::ChangeUser(const nsAString & argUser, const nsAString & argPassword, const nsAString & argDatabase, PRBool *_retval)
{
	if (! mConnected)
    {
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	// process the user arg
	if (argUser.Length() == 0 || argPassword.Length() == 0)
	{
		mErrno = JX_MYSQL50_ERROR_NULL_POINTER;
        return mErrno;
	}

	mUser = ToNewUTF8String(argUser);

	mPassword = ToNewUTF8String(argPassword);

	// process the database arg
	if (argDatabase.Length() != 0)
	{
		if (mDatabase != nsnull) { nsMemory::Free(mDatabase); }

		mDatabase = ToNewUTF8String(argDatabase);
	}
	else 
	{
		if (mDatabase != nsnull) { nsMemory::Free(mDatabase); }

		mDatabase = nsnull;
	}

	// If the change failed
  	if (mysql_change_user(mConnection, mUser, mPassword, mDatabase))
	{
		// Return FALSE
    	*_retval = PR_FALSE;
        mErrno = JX_MYSQL50_MYSQL_ERROR;
		return mErrno;
	}

   	*_retval = PR_TRUE;

	mConnected = PR_TRUE;

	return NS_OK;
}



/* AString clientInfo (); */
NS_IMETHODIMP jxMySQL50::ClientInfo(nsAString & _retval)
{
	_retval.Assign(NS_ConvertUTF8toUTF16(mysql_get_client_info()));
  	return NS_OK;
}

/* unsigned long clientVersion (); */
NS_IMETHODIMP jxMySQL50::ClientVersion(PRUint32 *_retval)
{
	*_retval = mysql_get_client_version();
  	return NS_OK;
}




/* void close (); */
NS_IMETHODIMP jxMySQL50::Close()
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	mysql_close(mConnection);

	mConnected = PR_FALSE;

	mConnection = mysql_init((MYSQL *) nsnull);

	return NS_OK;
}


/* boolean commit (); */
NS_IMETHODIMP jxMySQL50::Commit(PRBool *_retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }


	if (mysql_commit(mConnection))
	{
		// Return FALSE
    	*_retval = PR_FALSE;
        mErrno = JX_MYSQL50_MYSQL_ERROR;
		return mErrno;
	}

   	*_retval = PR_TRUE;

	return NS_OK;
}



/* AString info (); */
NS_IMETHODIMP jxMySQL50::Info(nsAString & _retval)
{
	_retval.Assign(NS_ConvertUTF8toUTF16(mysql_info(mConnection)));
  	return NS_OK;
}



/* unsigned long long insertID (); */
NS_IMETHODIMP jxMySQL50::InsertID(PRUint64 *_retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	*_retval = mysql_insert_id(mConnection);

  	return NS_OK;
}





/* boolean rollback (); */
NS_IMETHODIMP jxMySQL50::Rollback(PRBool *_retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }
	if (mysql_rollback(mConnection))
	{
		// Return FALSE
    	*_retval = PR_FALSE;
		mErrno = JX_MYSQL50_MYSQL_ERROR;
        return mErrno;
	}

   	*_retval = PR_TRUE;

	return NS_OK;
}


/* AString serverInfo (); */
NS_IMETHODIMP jxMySQL50::ServerInfo(nsAString & _retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	_retval.Assign(NS_ConvertUTF8toUTF16(mysql_get_server_info(mConnection)));
  	return NS_OK;
}


/* AString SQLState (); */
NS_IMETHODIMP jxMySQL50::SQLState(nsAString & _retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }
	_retval.Assign(NS_ConvertUTF8toUTF16(mysql_sqlstate(mConnection)));
  	return NS_OK;
}



/* unsigned long serverVersion (); */
NS_IMETHODIMP jxMySQL50::ServerVersion(PRUint32 *_retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }
	*_retval = mysql_get_server_version(mConnection);
  	return NS_OK;
}



/* AString serverInfo (); */
NS_IMETHODIMP jxMySQL50::HostInfo(nsAString & _retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }
	_retval.Assign(NS_ConvertUTF8toUTF16(mysql_get_host_info(mConnection)));
  	return NS_OK;
}



/* AString escape (in AString input); */
NS_IMETHODIMP jxMySQL50::Escape(const nsAString & input, nsAString & _retval)
{
	char		*newstr;
	PRInt32		escapestr_len, newstr_len;

	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	char * str = ToNewUTF8String(input);
	escapestr_len = strlen(str);

    newstr = static_cast<char*>(nsMemory::Alloc((escapestr_len*2) + 1));
    if (!newstr)
    {
        mErrno = JX_MYSQL50_ERROR_OUT_OF_MEMORY;
        return mErrno;
    }

	newstr_len = mysql_real_escape_string(mConnection, newstr, str, escapestr_len);

	_retval.Assign(NS_ConvertUTF8toUTF16(newstr));

	nsMemory::Free(str);
	nsMemory::Free(newstr);

  	return NS_OK;
}



/* long nextResult (); */
NS_IMETHODIMP jxMySQL50::NextResult(jxIMySQL50ResultSet **aReturn)
{
	// If we didn't get a valid place to return the ResultSet object pointer, 
	// then cause an exception to be returned to JavaScript
  	if (! aReturn )
    {
        mErrno = JX_MYSQL50_ERROR_NULL_POINTER;
        return mErrno;
    }

	// If we're not connected to a db server at the moment,
	// then cause an exception to be returned to JavaScript
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	// Spin up a ResultSet object, toss an exception if we can't create
    nsCOMPtr<jxIMySQL50ResultSet> res(do_CreateInstance(JX_MYSQL50RESULTSET_CONTRACTID));
    if (!res)
    {
        mErrno = JX_MYSQL50_ERROR_CANT_CREATEINSTANCE;
        return mErrno;
    }

	// Attach ourselves to the ResultSet object so it can make calls back to us if needed
    res->SetMySQL50(this);

	// Return the ResultSet object to our caller
   	*aReturn = res;
    NS_ADDREF(*aReturn);

	// Tell server to bump to next result set
	int status = mysql_next_result(mConnection);
    if (status == 0)
    {
        // Do some work to gether up results.
        // error already set for this if failed.
        return res->StoreResult(mConnection);
    }else if (status == -1)
    {
        // no more results
        res->SetType(eNULL);
    }else
    {
        //error
        res->SetType(eERROR);
        mErrno = JX_MYSQL50_MYSQL_ERROR;
        return mErrno;
    }

    return NS_OK;
}



enum 
{
	JX_SERVER_GONE_ERROR = CR_SERVER_GONE_ERROR
};


/* long ping (); */
NS_IMETHODIMP jxMySQL50::Ping(PRInt32 *_retval)
{
	if (! mConnected) {	
		*_retval = JX_SERVER_GONE_ERROR;
	  	return NS_OK;
	}

	*_retval = mysql_ping(mConnection);
  	return NS_OK;
}



/* unsigned long protocolVersion (); */
NS_IMETHODIMP jxMySQL50::ProtocolVersion(PRUint32 *_retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	*_retval = mysql_get_proto_info(mConnection);

  	return NS_OK;
}



/* unsigned long warningCount (); */
NS_IMETHODIMP jxMySQL50::WarningCount(PRUint32 *_retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	*_retval = mysql_warning_count(mConnection);

  	return NS_OK;
}



/* AString stat (); */
NS_IMETHODIMP jxMySQL50::Stat(nsAString & _retval)
{
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }
	_retval.Assign(NS_ConvertUTF8toUTF16(mysql_stat(mConnection)));
  	return NS_OK;
}


/* boolean threadSafe (); */
NS_IMETHODIMP jxMySQL50::ThreadSafe(PRBool *_retval)
{
	*_retval = mysql_thread_safe();
  	return NS_OK;
}

 /*[noscript] SetErrno(in unsigned long err) */
NS_IMETHODIMP jxMySQL50::SetErrorNumber(PRUint32 err)
{
    mErrno = err;
    return NS_OK;
}

 /*[noscript] void saveMySQLError() */
NS_IMETHODIMP jxMySQL50::SaveMySQLError()
{
    if (mErrno == JX_MYSQL50_MYSQL_ERROR)
    {
        mMySQLError[0] = 0;
        mMySQLErrno = mysql_errno(mConnection);
        const char *p = mysql_error(mConnection);
        size_t n = strlen(p);
        if (n<1023)
            strcpy(mMySQLError, p);
        else
        {
            strncpy(mMySQLError, p, 1023);
            mMySQLError[1023] = 0;
        }
        
    }
    return NS_OK;
}

/* unsigned long errno (); */
NS_IMETHODIMP jxMySQL50::Errno(PRUint32 *_retval)
{
    if (mErrno == JX_MYSQL50_MYSQL_ERROR)
    {
        *_retval = mysql_errno(mConnection);
        if (*_retval == 0)
            *_retval = mMySQLErrno;
    }else
        *_retval = mErrno;

  	return NS_OK;
}


/* AString error (); */
NS_IMETHODIMP jxMySQL50::Error(nsAString & aError)
{
    aError.Truncate();
    if (mErrno == JX_MYSQL50_MYSQL_ERROR)
    {
        if (mysql_errno(mConnection) == 0)
            aError.Assign(NS_ConvertUTF8toUTF16(mMySQLError));
        else
            aError.Assign(NS_ConvertUTF8toUTF16(mysql_error(mConnection)));
        return NS_OK;
    }

    NON_MYSQL_ERROR(mErrno, aError);
    if (aError.Length() == 0)
    {
        const char* me = mysql_error(mConnection);
        AppendUTF8toUTF16(nsPrintfCString(0x1000, "Unknown error %lX.  Last MYSQL error is %s", mErrno, (me && *me) ? me : ""), aError);
    }


#if 0

    switch (mErrno)
    {
    case JX_MYSQL50_ERROR_NOT_CONNECTED:
        aError.AssignLiteral("Not connected to MYSQL DB");
        break;
    case JX_MYSQL50_ERROR_CANT_CREATEINSTANCE:
        aError.AssignLiteral("Create object failed");
        break;
    case JX_MYSQL50_ERROR_STMT_ALREADY_INITIALIZED:
        aError.AssignLiteral("Statement already initialized");
        break;

    case JX_MYSQL50_ERROR_STMT_NULL:
        aError.AssignLiteral("Statement is null");
        break;

    case JX_MYSQL50_ERROR_UNEXPECTED_ERROR:
        aError.AssignLiteral("Unexpected error");
        break;

    case JX_MYSQL50_NULL_RESULTSET:
        aError.AssignLiteral("Resultset is null");
        break;

    case JX_MYSQL50_ERROR_ILLEGAL_VALUE:
        aError.AssignLiteral("Illegal value (eg too small or toolarge) specified");
        break;

    case JX_MYSQL50_ERROR_INVALID_TYPE:
        aError.AssignLiteral("Invalid type");
        break;
    case JX_MYSQL50_ERROR_BIND_ARRAY_IS_NULL:
        aError.AssignLiteral("Bind array is null");
        break;
    case JX_MYSQL50_CANNOT_CONVERT_DATA:
        aError.AssignLiteral("Cannot convert data");
        break;
    case JX_MYSQL50_ERROR_NULL_POINTER:
        aError.AssignLiteral("The point is null");
        break;
    case JX_MYSQL50_ERROR_OUT_OF_MEMORY:
        aError.AssignLiteral("Out of memory");
        break;

    default:
        char unkError[0x1000];
        sprintf(unkError, "Unkown error 0x%0x.  Last MYSQL error is %s", mErrno, mysql_error(mConnection));
        aError.AssignLiteral(unkError);
        break;
    }
#endif

  	return NS_OK;
}

/* long option (in unsigned long option); */
NS_IMETHODIMP jxMySQL50::Option(PRUint32 option, PRInt32 *_retval)
{
	PRInt32 ret = mysql_options(mConnection, (mysql_option)option, 0);

	*_retval = ret;
    if (ret)
    {
        mErrno = JX_MYSQL50_MYSQL_ERROR;
        return mErrno;
    }

  	return NS_OK;
}

/* long optionString (in unsigned long option, in AString arg); */
NS_IMETHODIMP jxMySQL50::OptionString(PRUint32 option, const nsAString & arg, PRInt32 *_retval)
{
	char * str = ToNewUTF8String(arg);

	PRInt32 ret = mysql_options(mConnection, (mysql_option)option, str);

	nsMemory::Free(str);

	*_retval = ret;
    if (ret)
    {
        mErrno = JX_MYSQL50_MYSQL_ERROR;
        return mErrno;
    }

  	return NS_OK;
}

/* long optionInt (in unsigned long option, in unsigned long arg); */
NS_IMETHODIMP jxMySQL50::OptionInt(PRUint32 option, PRUint32 arg, PRInt32 *_retval)
{
	PRInt32 ret = mysql_options(mConnection, (mysql_option)option, (char*) &arg);

	*_retval = ret;
    if (ret)
    {
        mErrno = JX_MYSQL50_MYSQL_ERROR;
        return mErrno;
    }

  	return NS_OK;
}

/* long optionBool (in unsigned long option, in boolean arg); */
NS_IMETHODIMP jxMySQL50::OptionBool(PRUint32 option, PRBool arg, PRInt32 *_retval)
{
	PRInt32 ret = mysql_options(mConnection, (mysql_option)option, (char*) &arg);

	*_retval = ret;
    if (ret)
    {
        mErrno = JX_MYSQL50_MYSQL_ERROR;
        return mErrno;
    }

  	return NS_OK;
}



/* boolean connect (in AString host, in AString user, in AString password, in AString database, in unsigned long port, in AString socket, in unsigned long options); */
NS_IMETHODIMP jxMySQL50::Connect(const nsAString & argHost, const nsAString & argUser, const nsAString & argPassword,
                                 const nsAString & argDatabase, PRUint32 argPort, const nsAString & argSocket, PRUint32 argOptions, PRBool *_retval)
{
	// process the host arg
	if (argHost.Length() != 0)
	{
		if (mHost != nsnull) { nsMemory::Free(mHost); }

		mHost = ToNewUTF8String(argHost);
	}

	// process the user arg
	if (argUser.Length() != 0)
	{
		if (mUser != nsnull) { nsMemory::Free(mUser); }

		mUser = ToNewUTF8String(argUser);
	}

	// process the password arg
	if (argPassword.Length() != 0)
	{
		if (mPassword != nsnull) { nsMemory::Free(mPassword); }

		mPassword = ToNewUTF8String(argPassword);
	}

	// process the database arg
	if (argDatabase.Length() != 0)
	{
		if (mDatabase != nsnull) { nsMemory::Free(mDatabase); }

		mDatabase = ToNewUTF8String(argDatabase);
	}

	// process the port arg
	if (argPort != 0)
	{
		mPort = argPort;
	}

    // process the socket arg
	if (argSocket.Length() != 0)
	{
		if (mSocket != nsnull) { nsMemory::Free(mSocket); }

		mSocket = ToNewUTF8String(argSocket);
	}

	// process the options arg
	if (argOptions != 0)
	{
		mOptions = argOptions;
	}

	// If the connect failed
  	if (! mysql_real_connect(mConnection, mHost, mUser, mPassword, mDatabase, mPort, mSocket, mOptions))
	{
		// Return FALSE
    	*_retval = PR_FALSE;
        mErrno = JX_MYSQL50_MYSQL_ERROR;

		nsCOMPtr<aptICoreLog> log(do_CreateInstance(APT_CORELOG_CONTRACTID));
		if (log)
		{
			nsCAutoString sError(mysql_error(mConnection));
			nsCAutoString sMe("jxMySQL50");
			log->Log(eERROR, sMe, sError);
		}
		return mErrno;
	}

    if (mysql_set_character_set(mConnection, "utf8")) 
    {
        // Return FALSE
    	*_retval = PR_FALSE;
        mErrno = JX_MYSQL50_MYSQL_ERROR;
		return mErrno;
    }

   	*_retval = PR_TRUE;

	mConnected = PR_TRUE;

	return NS_OK;
}

/* boolean query (in AString statement); */
NS_IMETHODIMP jxMySQL50::Query(const nsAString & statement, jxIMySQL50ResultSet **aReturn)
{
	// If we didn't get a valid place to return the ResultSet object pointer, 
	// then cause an exception to be returned to JavaScript
  	//NS_ENSURE_ARG_POINTER(aReturn);
    if (! aReturn )
    {
        mErrno = JX_MYSQL50_ERROR_NULL_POINTER;
        return mErrno;
    }

	// If we're not connected to a db server at the moment,
	// then cause an exception to be returned to JavaScript
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	// Spin up a ResultSet object, toss an exception if we can't create
    nsCOMPtr<jxIMySQL50ResultSet> res(do_CreateInstance(JX_MYSQL50RESULTSET_CONTRACTID));
    if(!res)
    {
        mErrno = JX_MYSQL50_ERROR_CANT_CREATEINSTANCE;
        return mErrno;
    }

	// Attach ourselves to the ResultSet object so it can make calls back to us if needed
    res->SetMySQL50(this);

	// Get the length of the query string
	char * stmt = ToNewUTF8String(statement);
	int	length;
  	length= strlen(stmt);

	// Execute the SQL statement
  	int ret = mysql_real_query(mConnection, stmt, length);

    nsMemory::Free(stmt);

    if (ret)
    {
        // error
        *aReturn = 0;
        mErrno = JX_MYSQL50_MYSQL_ERROR;
        return mErrno;
    }

	// Return the ResultSet object to our caller
   	*aReturn = res;
    NS_ADDREF(*aReturn);

	
	// Do some work to gether up results.
    // error already set if failed.
    return res->StoreResult(mConnection);
    
#if 0
	// If the ResultSet actually isn't there
	PRBool hasRes = PR_FALSE;
	res->HasRes(&hasRes);
	if (!hasRes)
	{
		// Release our containing object, and return NULL
    	NS_RELEASE(*aReturn);
   		*aReturn = nsnull;
	}
#endif

	// return NS_OK;
}


/* jxIMySQL50ResultSet listFields (in AString table, in AString wild); */
NS_IMETHODIMP jxMySQL50::ListFields(const nsAString & table, const nsAString & wild, jxIMySQL50ResultSet **aReturn)
{
	if (! aReturn )
    {
        mErrno = JX_MYSQL50_ERROR_NULL_POINTER;
        return mErrno;
    }

	// If we're not connected to a db server at the moment,
	// then cause an exception to be returned to JavaScript
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	// Spin up a ResultSet object, toss an exception if we can't create
    nsCOMPtr<jxIMySQL50ResultSet> res(do_CreateInstance(JX_MYSQL50RESULTSET_CONTRACTID));
    if(!res)
    {
        mErrno = JX_MYSQL50_ERROR_CANT_CREATEINSTANCE;
        return mErrno;
    }

	// Attach ourselves to the ResultSet object so it can make calls back to us if needed
    res->SetMySQL50(this);

	// Call listFields() 
	PRBool b;
	nsresult rv = res->ListFields(table, wild, &b);

	// If the call failed
	if (NS_FAILED(rv))
	{
	   	*aReturn = nsnull;
        mErrno = rv;
		return mErrno;
	}

	// Return the ResultSet object to our caller
   	*aReturn = res;
    NS_ADDREF(*aReturn);
	
	return NS_OK;
}


/* jxIMySQL50Statement prepare (in AString queryStr); */
NS_IMETHODIMP jxMySQL50::Prepare(const nsAString & queryStr, jxIMySQL50Statement **aReturn)
{
	if (! aReturn )
    {
        mErrno = JX_MYSQL50_ERROR_NULL_POINTER;
        return mErrno;
    }

	// If we're not connected to a db server at the moment,
	// then cause an exception to be returned to JavaScript
	if (! mConnected)
    {	
        mErrno = JX_MYSQL50_ERROR_NOT_CONNECTED;
        return mErrno;
    }

	// Spin up a Statement object, toss an exception if we can't create
    nsCOMPtr<jxIMySQL50Statement> stmt(do_CreateInstance(JX_MYSQL50STATEMENT_CONTRACTID));
    if (!stmt)
    {
        mErrno = JX_MYSQL50_ERROR_CANT_CREATEINSTANCE;
        return mErrno;
    }

	// Attach ourselves to the Statement object so it can make calls back to us if needed
    stmt->SetMySQL50(this);

	// Init the Statement
	PRInt32 rc;
	nsresult rv = stmt->Init(queryStr, &rc);

    
    if (NS_FAILED(rv))
	{
        // init failed
	   	*aReturn = nsnull;
        mErrno = rv;
#if 0
        char err[512];
        sprintf(err, " %s", mysql_error(mConnection));
#endif
        return mErrno;
	}

    // Return the Statement object to our caller
   	*aReturn = stmt;
    NS_ADDREF(*aReturn);

	return NS_OK;
}
