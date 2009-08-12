/* ***** BEGIN LICENSE BLOCK *****
 * Version: GPL 3
 *
 * This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 * This program is licensed under the GNU General Public license, version 3 (GPL).
 *
 * This program is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 * is prohibited.
 *
 * You can redistribute and/or modify this program under the terms of the GPL, 
 * as published by the Free Software Foundation.  You should
 * have received a copy of the GNU General Public License, Version 3 along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 * Aptana provides a special exception to allow redistribution of this file
 * with certain other code and certain additional terms
 * pursuant to Section 7 of the GPL. You may view the exception and these
 * terms on the web at http://www.aptana.com/legal/gpl/.
 * 
 * You may view the GPL, and Aptana's exception and additional terms in the file
 * titled license-jaxer.html in the main distribution folder of this program.
 * 
 * Any modifications to this file must keep this entire header intact.
 *
 * ***** END LICENSE BLOCK ***** */

(function() {

var log = Log.forModule("DB.ADO");

/**
 * @namespace {Jaxer.DB.ADO} Namespace that holds the ADO implementation of
 * the Jaxer DB API.
 */

var ADO = {};

// Object State
const adStateClosed = 0; 			// closed
const adStateOpen = 1;				// open
const adStateConnecting = 2;		// connecting
const adStateExecuting = 4;			// executing a command
const adStateFetching = 8;			// rows are being retrieved 

// Connection Mode
const adModeRead = 1;				// read-only
const adModeWrite = 2;				// write-only
const adModeReadWrite = 3;			// read/write

// Connection Isolation Level
const adXactReadUncommitted = 256;	// can view uncommitted changes in other transactions  
const adXactReadCommitted = 4096;	// can view changes in other transactions only after they have been committed 
const adXactRepeatableRead = 65536;	// cannot see changes in other transactions, but requerying can retrieve new records
const adXactSerializable = 1048576;	// transactions are conducted in isolation of other transactions

// Data Types
const adSmallInt = 2;				// smallint
const adInteger = 3;				// int
const adSingle = 4;					// real
const adDouble = 5;					// float
const adCurrency = 6;				// money 
const adBoolean = 11;				// bit 
const adDecimal = 14;				// decimal
const adTinyInt = 16;				// tinyint
const adBigInt = 20;				// bigint
const adGUID = 72;					// uniqueidentifier
const adBinary = 128;				// binary
const adChar = 129;					// char 
const adWChar = 130;				// nchar
const adNumeric = 131;				// numeric
const adDBTimeStamp = 135;			// datetime 
const adVarNumeric = 139;			// numeric 
const adVarChar = 200;				// varchar 
const adLongVarChar = 201;			// text 
const adVarWChar = 202;				// nvarchar
const adLongVarWChar = 203;			// ntext 
const adVarBinary = 204;			// varbinary 
const adLongVarBinary = 205;		// image  

// Command Types
const adCmdText = 1;				// a textual definition of a command or stored procedure call
const adCmdTable = 2;				// a table (converted to 'select * from table')
const adCmdStoredProc = 4;			// a stored procedure name 

// Parameter Direction
const adParamInput = 1;				// input parameter (default)
const adParamOutput = 2;			// output parameter
const adParamInputOutput = 3;		// input and output parameter 
const adParamReturnValue = 4;		// return value
var VALUE_TYPE_NULL		= "null"; 		// Null data type.
var VALUE_TYPE_NUMBER	= "double"; 	// Used for ADO integer, floating point, etc. data types.
var VALUE_TYPE_DATETIME	= "datetime"; 	// Datetime data type.
var VALUE_TYPE_DATE		= "date"; 		// Date data type.
var VALUE_TYPE_TIME		= "time"; 		// Time data type.
var VALUE_TYPE_STRING	= "string"; 	// String data type.
var VALUE_TYPE_TEXT		= "text"; 		// Text (CLOB) data type.
var VALUE_TYPE_BLOB		= "blob"; 		// Blob data type.

var BLOB_FLAG = 63; // A way to distinguish binary-lobs (BLOBs) from character-lobs (TEXT) -- see http://dev.mysql.com/doc/refman/5.0/en/c-api-datatypes.html

var RS_HAS_ERROR 	= 0; 	// Not used -- errors are thrown instead
var RS_HAS_RECORDS 	= 1;	// Can call fetch() on it to get the records
var RS_HAS_ROWCOUNT = 2; 	// Can only get the rowcount (e.g. number of rows INSERTed)
var RS_IS_NULL 		= 3;	// Signals the end of a multiple-result-set interaction

var CR_UNKNOWN_ERROR = 2000; // See http://dev.mysql.com/doc/refman/5.0/en/error-messages-client.html

// The following are used to fill in the date portions of a JS Date object 
// being created from a ADO time-type column
ADO.TIME_COLUMN_YEAR	= 1970;
ADO.TIME_COLUMN_MONTH	= 0; // January
ADO.TIME_COLUMN_DAY	= 1;

ADO.IMPLEMENTATION = "ADO";

/**
 * Validate the provided connection parameters
 * 
 * @alias Jaxer.DB.ADO.validateConnectionParameters
 * @param {Object} connectionParams
 * 		A hashmap of required parameters to connect to the database. Required
 * 		properties are HOST (hostname of the server), USER (username for
 * 		authentication), PASS (password for authentication), NAME (database
 * 		name)
 */
ADO.validateConnectionParameters = function validateConnectionParameters(connectionParams)
{
	var errors = [];
	['HOST', 'USER', 'PASS', 'NAME'].forEach(function(propName)
	{
		if (!connectionParams.hasOwnProperty(propName))
		{
			errors.push("Missing '" + propName + "' property");
		}
	});
	if (errors.length > 0) 
	{
		if (log.getLevel() == Log.TRACE) log.trace('Connection parameters incomplete: ' + errors.join("; ") + " params: "+ uneval(connectionParams));
		throw new Exception("Invalid database connection parameters provided: " + errors.join("; "), log);
	} 
	else
	{
		if (log.getLevel() == Log.TRACE) log.trace('Connection parameters are all present (OK):' + uneval(connectionParams) )
		return; // no action if OK.
	}
}

/**
 * Creates a new database named according to connectionParams.NAME, 
 * if one does not already exist.
 * 
 * @alias Jaxer.DB.ADO.createDB
 * @param {Object} connectionParams 
 * 		A hashmap of required parameters to connect to the database. Required
 * 		properties are HOST (hostname of the server), USER (username for
 * 		authentication), PASS (password for authentication), NAME (database
 * 		name), PORT (connection port, default value is "3306"), and
 * 		CLOSE_AFTER_EXECUTE (whether to close the connection after each call to
 * 		execute, default is open).
 *  @return {Jaxer.DB.ADO.Connection}
 *  	Returns and instance of Connection.
 */
ADO.createDB = function createDB(connectionParams)
{
	var dbName = connectionParams.NAME;
	var conn, sql;
	
	var noNameConnectionParams = {};
	for (var p in connectionParams)
	{
		noNameConnectionParams[p] = connectionParams[p];
	}
	noNameConnectionParams.NAME = '';
	
	conn = new ADO.Connection(noNameConnectionParams);

	try
	{
		sql = "CREATE DATABASE IF NOT EXISTS `" + dbName + "`";
		conn.open();
		conn.execute(sql);
		conn.close();
		return new ADO.Connection(connectionParams);
	}
	catch (e)
	{
		if (conn) conn.close();
		throw e;
	}
}

/**
 * @classDescription {Jaxer.DB.ADO.Connection} Creates a new connection to the
 * given databaseName (file).
 */

/**
 * Creates a new connection to the given databaseName. The resulting connection object 
 * is the only way to interact with the database.
 * 
 * @alias Jaxer.DB.ADO.Connection
 * @constructor
 * @param {Object} connectionParams 
 * 		A hashmap of parameters for connecting to the database. Required
 * 		properties are:
 * 		<ul>
 * 			<li>HOST (hostname of the server), </li>
 * 			<li>USER (username for authentication), </li>
 * 			<li>PASS (password for authentication, can be the empty string), and</li>
 * 			<li>NAME (database name, can be the empty string).</li>
 *		</ul>
 * 		Optional parameters are:
 * 		<ul>
 * 			<li>TRUSTED_CONNECTION (if using windows integrated security, USER/PASS not required if provided),</li>
 * 			<li>CLOSE_AFTER_EXECUTE (whether to close the connection after each call to execute, default is false), and</li>
 * 			<li>CLOSE_AFTER_REQUEST (whether to close the connection after each request, default is true).</li>
 *		</ul>
 *  @return {Jaxer.DB.ADO.Connection}
 *  	Returns an instance of Connection.
 */
ADO.Connection = function Connection(connectionParams)
{
	ADO.validateConnectionParameters(connectionParams);

	this.serverName = connectionParams.HOST;
	this.userName = connectionParams.USER;
	this.password = connectionParams.PASS;
	this.databaseName = connectionParams.NAME;

	this.isOpen = false;

	try 
	{
	    this.conn = COMObject('ADODB.Connection');   
	}
	catch (e) 
	{
		throw 'ADODB connection error :'+ e.message;		
	}

	this.closeAfterExecute = (typeof connectionParams.CLOSE_AFTER_EXECUTE == 'boolean') ? connectionParams.CLOSE_AFTER_EXECUTE : false;
	this.closeAfterRequest = (typeof connectionParams.CLOSE_AFTER_REQUEST == 'boolean') ? connectionParams.CLOSE_AFTER_REQUEST : true;
};

/**
 * Is the connection currently open? Recall that even if the answer is no
 * the connection would automatically be opened when needed.
 * 
 * @alias Jaxer.DB.ADO.Connection.prototype.isOpen
 * @property {Boolean}
 */

/**
 * Returns the string identifying the database implementation of this connection.
 * You can compare this e.g. to Jaxer.DB.SQLite.IMPLEMENTATION or
 * Jaxer.DB.ADO.IMPLEMENTATION
 * 
 * @alias Jaxer.DB.ADO.Connection.prototype.implementation
 * @property {String}
 */
ADO.Connection.prototype.__defineGetter__('implementation', function getImplementation()
{
	return ADO.IMPLEMENTATION;
});

/**
 * Returns the string identifying the version of the database to which you are connected.
 * 
 * @alias Jaxer.DB.ADO.Connection.prototype.version
 * @property {String}
 */
ADO.Connection.prototype.__defineGetter__('version', function getVersion()
{
	return this.conn.Version;
});

/**
 * Opens the connection so queries can be executed. This is optional, since if
 * the connection is not open when it's asked to execute some SQL, it will open
 * the connection automatically. Also closing the connection is optional.
 * 
 * @alias Jaxer.DB.ADO.Connection.prototype.open
 */
ADO.Connection.prototype.open = function open()
{
	if (this.isOpen) return;
	if (log.getLevel() == Log.TRACE) 
	{
		var that = this;
		log.trace("Opening database: " + this.databaseName + " with connection parameters: " + 
			['serverName', 'userName', 'password', 'databaseName', 'port', 'socket', 'opt'].map(function(prop)
				{
					return prop + ' = ' + that[prop];
				}).join('; '));
	}
	var errorCode = null, errorMessage = null;
	
	try 
	{
		this.conn.ConnectionString = <>'Provider=SQLOLEDB;Server={this.serverName};Database={this.databaseName};User ID={this.userName};Password={this.password}'</>
	} 
	catch (e) 
	{
		errorCode = CR_UNKNOWN_ERROR;
		errorString = "Unknown error occurred while attempting to connect to ADO server: " + e;
	}
	
	if (errorCode != null)
	{
		var errorMessage = "ADO error " + errorCode + " while opening connection to database; perhaps check your parameters and the availability of the ADO server. Error message: " + errorString + ".";
		var err = new Exception(errorMessage, log);
		err[DB.SQL_ERROR_CODE_PROPERTY] = errorCode;
		err[DB.SQL_ERROR_DESCRIPTION_PROPERTY] = errorString;
		throw err;
	}
	
	this.isOpen = true;
	log.debug("Opened");
};

/**
 * Executes the given sql using the connection. If the SQL includes ?'s (question
 * marks) as parameter placeholders, the values of those parameters
 * should be passed in as extra arguments to this function, either
 * as individual arguments or as a single array.
 * If the last argument is a (non-Array) object, it is used to pass in options.
 * Currently two options are supported: dateHandler and useLocalTimezone.
 * If dateHandler is given, it should be a function that takes
 * the raw ADO date/time/datetime string representation of a returned cell value,
 * and a string describing the declared type,
 * and returns the desired JavaScript representation.
 * If dateHandler is not given, set useLocalTimezone to true to store date/time values using the local timezone.
 * Otherwise, by default they are stored as UTC.
 * 
 * @example
 *		<pre>
 *			rs = conn.execute("SELECT * FROM myTable");
 * 			rs = conn.execute("SELECT * FROM myTable WHERE id=? AND zip=?", myId, myZip);
 * 			rs = conn.execute("SELECT * FROM myTable WHERE id=? AND zip=?", [myId, myZip]);
 * 		</pre>
 * @alias Jaxer.DB.ADO.Connection.prototype.execute
 * @param {String} sql
 * 		The sql statement to be executed as a prepared statement
 * @return {Jaxer.DB.ResultSet, Number, Object[]}
 * 		The results of the query.
 * 		For a SELECT-type query, a Jaxer.DB.ResultSet is returned, with 0 or more rows.
 * 		For an INSERT/UPDATE/DELETE-type query, the number of rows affected is returned. 
 * 		If multiple queries were issued (or a stored procedure was executed) the result will be 
 * 		a corresponding array of Jaxer.DB.ResultSet or Number objects.
 */
ADO.Connection.prototype.execute = function execute(sql)
{

	log.trace("Starting execute " + Array.slice(arguments));
	
	var result;
	
	sql = DB.sqlToString(sql, log);

	this.open(); // In case it hasn't been opened

	var rs = this.conn.Execute(sql);
	
	// todo create a Jaxer.ResultSet
	
	log.trace("Finished executing");
	return [rs.Fields.Item(0).Value];
	
};



/**
 * Closes the connection if it's open. This is optional, and only does something
 * if the connection is open.
 * 
 * @alias Jaxer.DB.ADO.Connection.prototype.close
 */
ADO.Connection.prototype.close = function close()
{
	if (this.isOpen)
	{
		log.trace("Closing connection to database " + this.databaseName);
		if (this.conn && this.conn.State != adStateClosed) 
		{
			this.Close();
		}
		this.isOpen = false;
	}
};

/**
 * Returns the unique ID used for an AUTO_INCREMENT column in the most recent successful
 * INSERT command on the current connection. 
 * If no successful INSERTs have ever occurred on this connection, 0 is returned. 
 * Note that unsuccessful INSERTs do not change this value.
 * This is the same as asking for the lastInsertId or lastInsertRowId properties.
 * See http://dev.mysql.com/doc/refman/5.0/en/getting-unique-id.html for more details.
 * 
 * @alias Jaxer.DB.ADO.Connection.prototype.getLastInsertId
 * @return {Number}
 * 		The id, or 0
 */
ADO.Connection.prototype.getLastInsertId = function getLastInsertId()
{
	if (this.conn && this.isOpen) 
	{
		return this.conn.insertID();
	}
	else
	{
		return 0;
	}
}

ADO.Connection.prototype.getLastId = ADO.Connection.prototype.getLastInsertId;

/**
 * Returns the unique ID used for an AUTO_INCREMENT column in the most recent successful
 * INSERT command on the current connection. 
 * If no successful INSERTs have ever occurred on this connection, 0 is returned. 
 * Note that unsuccessful INSERTs do not change this value.
 * This is a synonym for lastInsertId.
 * See http://dev.mysql.com/doc/refman/5.0/en/getting-unique-id.html for more details.
 * 
 * @alias Jaxer.DB.ADO.Connection.prototype.lastInsertRowId
 * @property {Number}
 */
ADO.Connection.prototype.__defineGetter__('lastInsertRowId', ADO.Connection.prototype.getLastInsertId);

/**
 * Returns the unique ID used for an AUTO_INCREMENT column in the most recent successful
 * INSERT command on the current connection. 
 * If no successful INSERTs have ever occurred on this connection, 0 is returned. 
 * Note that unsuccessful INSERTs do not change this value.
 * This is a synonym for lastInsertRowId.
 * See http://dev.mysql.com/doc/refman/5.0/en/getting-unique-id.html for more details.
 * 
 * @alias Jaxer.DB.ADO.Connection.prototype.lastInsertId
 * @property {Number}
 */
ADO.Connection.prototype.__defineGetter__('lastInsertId', ADO.Connection.prototype.getLastInsertId);

/**
 * Tests the connection by trying to connect and catching and returning any error 
 * encountered. If the connection is successful, returns a null.
 * 
 * @alias Jaxer.DB.ADO.Connection.prototype.test
 * @param {Boolean} keepOpen
 * 		If true, the connection will be kept open (if the test was successful).
 * 		If false, the connection will be left in the same state as before the test:
 * 		if it was open before it will be kept open, otherwise it will be closed.
 * @return {Object}
 * 		If successful, returns null; if unsuccessful, returns the error.
 * 		Usually you can use the error's sqlErrorCode and sqlErrorDescription
 * 		to see what the error was (or just its toString() method).
 */
ADO.Connection.prototype.test = function test(keepOpen)
{
	var wasOpen = this.isOpen;
	var error = null;
	try
	{
		this.open();
	}
	catch (e)
	{
		error = e;
	}
	if (!wasOpen && !keepOpen)
	{
		this.close();
	}
	return error;
}

DB.ADO = ADO;

Jaxer.Log.trace("*** DB_ADO.js loaded");
	
})();
