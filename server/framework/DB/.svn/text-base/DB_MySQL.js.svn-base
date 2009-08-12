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

var log = Log.forModule("DB.MySQL");

/**
 * @namespace {Jaxer.DB.MySQL} Namespace that holds the MySQL implementation of
 * the Jaxer DB API.
 */

var MySQL = {};

var MYSQL50_CLASS_ID		= "@aptana.com/jxMySQL50;1"
var MYSQL50_INTERFACE       = Components.interfaces.jxIMySQL50;

var VALUE_TYPE_NULL		= "null"; 		// Null data type.
var VALUE_TYPE_NUMBER	= "double"; 	// Used for MySQL integer, floating point, etc. data types.
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
// being created from a MySQL time-type column
MySQL.TIME_COLUMN_YEAR	= 1970;
MySQL.TIME_COLUMN_MONTH	= 0; // January
MySQL.TIME_COLUMN_DAY	= 1;

MySQL.IMPLEMENTATION = "MySQL";

/**
 * Validate the provided connection parameters
 * 
 * @alias Jaxer.DB.MySQL.validateConnectionParameters
 * @param {Object} connectionParams
 * 		A hashmap of required parameters to connect to the database. Required
 * 		properties are HOST (hostname of the server), USER (username for
 * 		authentication), PASS (password for authentication), NAME (database
 * 		name)
 */
MySQL.validateConnectionParameters = function validateConnectionParameters(connectionParams)
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
 * @alias Jaxer.DB.MySQL.createDB
 * @param {Object} connectionParams 
 * 		A hashmap of required parameters to connect to the database. Required
 * 		properties are HOST (hostname of the server), USER (username for
 * 		authentication), PASS (password for authentication), NAME (database
 * 		name), PORT (connection port, default value is "3306"), and
 * 		CLOSE_AFTER_EXECUTE (whether to close the connection after each call to
 * 		execute, default is open).
 *  @return {Jaxer.DB.MySQL.Connection}
 *  	Returns and instance of Connection.
 */
MySQL.createDB = function createDB(connectionParams)
{
	var dbName = connectionParams.NAME;
	var conn, sql;
	
	var noNameConnectionParams = {};
	for (var p in connectionParams)
	{
		noNameConnectionParams[p] = connectionParams[p];
	}
	noNameConnectionParams.NAME = '';
	
	conn = new MySQL.Connection(noNameConnectionParams);

	try
	{
		sql = "CREATE DATABASE IF NOT EXISTS `" + dbName + "`";
		conn.open();
		conn.execute(sql);
		conn.close();
		return new MySQL.Connection(connectionParams);
	}
	catch (e)
	{
		if (conn) conn.close();
		throw e;
	}
}

/**
 * @classDescription {Jaxer.DB.MySQL.Connection} Creates a new connection to the
 * given databaseName (file).
 */

/**
 * Creates a new connection to the given databaseName. The resulting connection object 
 * is the only way to interact with the database.
 * 
 * @alias Jaxer.DB.MySQL.Connection
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
 * 			<li>PORT (connection port, default value is 3306),</li>
 * 			<li>SOCKET (absolute path to socket file, default value is null),</li>
 * 			<li>AUTORECONNECT (whether to autoreconnect the connection is the MySQL server drops the connection, default is true), and</li>
 * 			<li>CLOSE_AFTER_EXECUTE (whether to close the connection after each call to execute, default is false), and</li>
 * 			<li>CLOSE_AFTER_REQUEST (whether to close the connection after each request, default is true).</li>
 *		</ul>
 *  @return {Jaxer.DB.MySQL.Connection}
 *  	Returns an instance of Connection.
 */
MySQL.Connection = function Connection(connectionParams)
{
	MySQL.validateConnectionParameters(connectionParams);

	this.serverName = connectionParams.HOST;
	this.userName = connectionParams.USER;
	this.password = connectionParams.PASS;
	this.databaseName = connectionParams.NAME;
	this.socket = connectionParams.SOCKET || null;
	this.port = connectionParams.PORT || 3306;
	this.opt = (1<<16|1<<17); // Enables support for multiple statements and stored procedures
	this.isOpen = false;
	this.conn = Components.classes[MYSQL50_CLASS_ID].createInstance(MYSQL50_INTERFACE);
	
	// force autoreconnect
	this.conn.optionBool(this.conn.OPT_RECONNECT, true);
	
	this.autoReconnect     = (typeof connectionParams.AUTORECONNECT == 'boolean') ? connectionParams.AUTORECONNECT : true;
	this.closeAfterExecute = (typeof connectionParams.CLOSE_AFTER_EXECUTE == 'boolean') ? connectionParams.CLOSE_AFTER_EXECUTE : false;
	this.closeAfterRequest = (typeof connectionParams.CLOSE_AFTER_REQUEST == 'boolean') ? connectionParams.CLOSE_AFTER_REQUEST : true;
};

/**
 * Is the connection currently open? Recall that even if the answer is no
 * the connection would automatically be opened when needed.
 * 
 * @alias Jaxer.DB.MySQL.Connection.prototype.isOpen
 * @property {Boolean}
 */

/**
 * Returns the string identifying the database implementation of this connection.
 * You can compare this e.g. to Jaxer.DB.SQLite.IMPLEMENTATION or
 * Jaxer.DB.MySQL.IMPLEMENTATION
 * 
 * @alias Jaxer.DB.MySQL.Connection.prototype.implementation
 * @property {String}
 */
MySQL.Connection.prototype.__defineGetter__('implementation', function getImplementation()
{
	return MySQL.IMPLEMENTATION;
});

/**
 * Returns the string identifying the version of the database to which you are connected.
 * 
 * @alias Jaxer.DB.MySQL.Connection.prototype.version
 * @property {String}
 */
MySQL.Connection.prototype.__defineGetter__('version', function getVersion()
{
	return this.execute("SELECT version()").singleResult;
});

/**
 * Opens the connection so queries can be executed. This is optional, since if
 * the connection is not open when it's asked to execute some SQL, it will open
 * the connection automatically. Also closing the connection is optional.
 * 
 * @alias Jaxer.DB.MySQL.Connection.prototype.open
 */
MySQL.Connection.prototype.open = function open()
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
		// force autoreconnect
		if (this.autoReconnect)
		{ 
			this.conn.optionString(this.conn.OPT_RECONNECT, "1");
		}

		this.conn.connect(this.serverName, this.userName, this.password, this.databaseName, this.port, this.socket, this.opt);
		try 
		{
			this.conn.autocommit(1);
		} 
		catch (e) 
		{
			errorCode = this.conn.errno();
			errorString = this.conn.error();
		}
	} 
	catch (e) 
	{
		errorCode = CR_UNKNOWN_ERROR;
		errorString = "Unknown error occurred while attempting to connect to MySQL server: " + e;
	}
	
	if (errorCode != null)
	{
		var errorMessage = "MySQL error " + errorCode + " while opening connection to database; perhaps check your parameters and the availability of the MySQL server. Error message: " + errorString + ".";
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
 * the raw MySQL date/time/datetime string representation of a returned cell value,
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
 * @alias Jaxer.DB.MySQL.Connection.prototype.execute
 * @param {String} sql
 * 		The sql statement to be executed as a prepared statement
 * @return {Jaxer.DB.ResultSet, Number, Object[]}
 * 		The results of the query.
 * 		For a SELECT-type query, a Jaxer.DB.ResultSet is returned, with 0 or more rows.
 * 		For an INSERT/UPDATE/DELETE-type query, the number of rows affected is returned. 
 * 		If multiple queries were issued (or a stored procedure was executed) the result will be 
 * 		a corresponding array of Jaxer.DB.ResultSet or Number objects.
 */
MySQL.Connection.prototype.execute = function execute(sql)
{

	log.trace("Starting execute " + Array.slice(arguments));
	
	var result;
	
	sql = DB.sqlToString(sql, log);

	this.open(); // In case it hasn't been opened

	try
	{
		// determine what params we've been given, if any
		var params, options;
		[params, options] = DB.getParamsFromArgs(arguments, 1);
		
		var useStatement = params && params.length;
		log.trace((useStatement ? "Using a prepared statement" : "Not using a prepared statement") + " to process: " + sql);
		if (useStatement)
		{
			log.trace("Will use a prepared statement");
			result = executeWithStatement(this, sql, [params], options)[0];
		}
		else
		{
			result = executeWithoutStatement(this, sql, options);
		}
		
		if (result.constructor == DB.ResultSet) 
		{
			log.trace("Populated resultSet with " + result.rows.length + " rows of " + result.columns.length + " columns");
		}
		else if (result.constructor == Array)
		{
			log.trace("Populated " + result.length + " results");
		}
		
	}
	finally
	{
		try
		{
			if (this.closeAfterExecute) 
			{
				log.trace("Closing the connection");
				this.close();
			}
		}
		catch (e) // log but do not throw again
		{
			log.error("Error trying to close the statement: " + e);
		}
	}
	
	log.trace("Finished executing");
	return result;
	
};

/**
 * Prepares the given SQL query string on the current default database 
 * (as defined in configApps.js) and then iteratively executes it
 * over the given array of parameters.
 * 
 * @example
 *		<pre>
 * 			[rsA, rsB] = conn.mapExecute("SELECT * FROM myTable WHERE id=?", [idA, idB]);
 * 			[rsA, rsB] = conn.mapExecute("SELECT * FROM myTable WHERE id=? AND zip=?", [ [idA, zipA], [idB, zipB] ]);
 * 		</pre>
 * @alias Jaxer.DB.MySQL.Connection.prototype.mapExecute
 * @param {String} sql
 * 		The SQL to execute, using ?'s (question marks) as parameter placeholders
 * @param {Array} arrayOfParameters
 * 		An array of parameters to use for each execution. Each element of the array
 * 		may itself be a single value or an array of values (corresponding to
 * 		the ?'s in the SQL)
 * @param {Object} [options]
 * 		An optional hashmap of options. Currently three options are supported: flatten, dateHandler, and useLocalTimezone.
 * 		If flatten is true, the returned result will be a single ResultSet
 * 		with its rows being the concatenated rows of each query.
 *		If dateHandler is given, it should be a function that takes
 * 		the raw MySQL date/time/datetime string representation of a returned cell value,
 * 		and a string describing the declared type,
 * 		and returns the desired JavaScript representation.
 * 		If dateHandler is not given, set useLocalTimezone to true to store date/time values using the local timezone.
 * 		Otherwise, by default they are stored as UTC.
 * @return {Object[]}
 * 		A corresponding array of Jaxer.DB.ResultSets or Numbers for each query, 
 * 		or a combined Jaxer.DB.ResultSet or Number if the 'flatten' option is true.
 * 		For SELECT-type queries one or more Jaxer.DB.ResultSets are returned;
 * 		for INSERT/UPDATE/DELETE-type queries the number of affected rows is returned.
 * @see Jaxer.DB.ResultSet
 */
MySQL.Connection.prototype.mapExecute = function mapExecute(sql, arrayOfParams, options)
{

	log.trace("Starting execute");
	
	var result;
	
	sql = DB.sqlToString(sql, log);

	this.open(); // In case it hasn't been opened

	try
	{
		result = executeWithStatement(this, sql, arrayOfParams, options);
	}
	finally
	{
		try
		{
			if (this.closeAfterExecute) 
			{
				log.trace("Closing the connection");
				this.close();
			}
		}
		catch (e) // log but do not throw again
		{
			log.error("Error trying to close the connection: " + e);
		}
	}
	
	log.trace("Finished executing");
	return result;
	
}

function executeWithStatement(conn, sql, arrayOfParams, options)
{

	var stmt;
	var flatten = options && options.flatten;
	var results = flatten ? null : [];
	var result = null;
	try // will close (release) the statement even if there are any errors
	{

		// First prepare the statement
		try
		{
			stmt = conn.conn.prepare(sql);
		}
		catch (e)
		{
			var errorCode = conn.conn.errno();
			var errorString = conn.conn.error();
			var errorMessage = "MySQL error " + errorCode + " while preparing statement. Error message: " + errorString + ". SQL: " + sql;
			var err = new Exception(errorMessage, log);
			err[DB.SQL_ERROR_CODE_PROPERTY] = errorCode;
			err[DB.SQL_ERROR_DESCRIPTION_PROPERTY] = errorString;
			throw err;
		}
		if (!stmt)
		{
			throw new Exception("Could not prepare statement. SQL: " + sql, log);
		}
		
		arrayOfParams.forEach(function executeOneWithStatement(params)
		{
			params = DB.getParamsFromArgs([params], 0)[0];
			var stmtParams = stmt.paramCount();
			if (params.length != stmtParams)
			{
				throw new Exception("Prepared statement " + sql + " has " + stmtParams + " parameters while " + params.length + " parameter values were given", log);
			}
			
			result = bindAndExecute(stmt, sql, params, result, options);
			if (!flatten) 
			{
				results.push(result);
				result = null;
			}
		});
		
		if (flatten)
		{
			results = result;
		}
	
	}
	finally
	{
		if (stmt) 
		{
			log.trace("Closing statement");
			try 
			{
				stmt.close(); // Important to always do this to free up resource
			} 
			catch (e) // log but do not throw again
			{
				log.error("Error trying to close the statement: " + e);
			}
		}
	}

	return results;

}

// Get the params array ready for logging
function truncateArrayForDisplay(arr)
{
	var maxLen = 4;
	var maxMember = 4;
	var strBuf = [];
	var truncated;
	arr.forEach(function(param)
	{
		truncated = false;
		if (typeof param == 'string')
		{
			truncated = (param.length < maxMember);
			if (truncated) 
			{
				param = param.substr(0, maxMember) + '...';
			}
		}
		else if (
			param != null && 
			typeof param == 'object' && 
			param.constructor == param.__parent__.Array &&
			param.length > maxMember)
		{
			truncated = (param.length > maxMember);
			if (truncated) 
			{
				param = param.slice(0, maxMember);
				param.push('...')
			}
			param = '[' + param.join(',') + ']';
		}
		strBuf.push(String(param));
	});
	return '[' + strBuf.join(',') + ']';
}

// If you pass in a resultSet or a number, the results of this execution will be appended to it.
// If you pass in null, a new resultSet or number will be created.
// In either case, the resultSet or number is returned.
// We don't currently do anything in here with the options hashmap -- it's reserved for future use
function bindAndExecute(stmt, sql, params, result, options)
{
	
	// TODO: fix it such that even if there are no rows to return, the returned columns could be meaningful
	// First bind the parameters
	if (log.getLevel() == Log.TRACE) log.trace("Binding statement '" + sql + "' with params array: " + truncateArrayForDisplay(params));
	var boundAs = [];
	params.forEach(function(param, index)
	{
		switch (typeof param)
		{
			case "number":
				if (isFinite(param)) 
				{
					stmt.bindDoubleParameter(index, param);
					boundAs.push('double');
				}
				else 
				{
					log.warn("Parameter " + index + " is not a finite number - using NULL instead. SQL: " + sql, log);
					stmt.bindNullParameter(index);
					boundAs.push('null');
				}
				break;
			case "boolean":
				stmt.bindDoubleParameter(index, (param ? 1 : 0));
				boundAs.push('double');
				break;
			case "string":
				stmt.bindUTF8StringParameter(index, param);
				boundAs.push('utf8String')
				break;
			case "object": // can only be used for Blob (from integer array) or Date binding
				if (param == null) 
				{
					stmt.bindNullParameter(index);
					boundAs.push('null')
				}
				else if (Util.isDate(param)) 
				{
					var offset = (options && options.useLocalTimezone) ? param.getTimezoneOffset() * 60 : 0;
					stmt.bindDatetimeParameter(index, Math.round(param.getTime() / 1000) - offset); // seconds since midnight 1/1/1970.
					boundAs.push('datetime')
				}
				else if (Util.isArray(param)) 
				{
					stmt.bindBlobParameter(index, param, param.length);
					boundAs.push('blob');
				}
				else 
				{
					log.warn("Parameter " + index + " is not a Date or a (byte) Array - using NULL instead. SQL: " + sql, log);
					stmt.bindNullParameter(index);
					boundAs.push('null')
				}
				break;
			case "undefined":
				stmt.bindNullParameter(index);
				boundAs.push('null')
				break;
			default:
				log.warn("Parameter " + index + " is of an unsupported type (" + (typeof param) + " - using NULL instead. SQL: " + sql, log);
				stmt.bindNullParameter(index);
				boundAs.push('null')
				break;
		}
	});
	log.trace("Parameters were bound as: " + boundAs.join('; '));
	
	// Then execute the statement	
	log.trace("Executing prepared statement");
	try
	{
		stmt.execute();
	}
	catch (e)
	{
		var errorCode = stmt.errno();
		var errorString = stmt.error();
		var errorMessage = "MySQL error " + errorCode + " while executing prepared statement. Error message: " + errorString + ". SQL: " + sql;
		var err = new Exception(errorMessage, log);
		err[DB.SQL_ERROR_CODE_PROPERTY] = errorCode;
		err[DB.SQL_ERROR_DESCRIPTION_PROPERTY] = errorString;
		throw err;
	}
	
	// And finally extract any results
	var appendToResult = (result != null); // If you pass in null, a new resultSet will be created
	var rm = stmt.resultMetadata();
	
	switch (rm.type)
	{

		case RS_HAS_RECORDS:
		
			log.trace("Fetching prepared statement return values to populate resultSet");
			
			if (!appendToResult) result = new DB.ResultSet();
			
			var numCols = stmt.fieldCount();
			var colTypes = [];
			for (var iCol = 0; iCol < numCols; iCol++) 
			{
				if (!appendToResult) 
				{
					log.trace("Column " + iCol + ": " + stmt.getColumnName(iCol));
					result.columns[iCol] = stmt.getColumnName(iCol);
				}
				var declType = stmt.getColumnDecltype(iCol);
				if (declType == VALUE_TYPE_BLOB) // May have to adjust it for TEXT-type BLOB
				{
					var field = rm.fetchFieldDirect(iCol);
					if (field.charsetnr() != BLOB_FLAG)
					{
						declType = VALUE_TYPE_TEXT;
					}
				}
				colTypes[iCol] = 
				{
					type: declType
				};
			}
			
			while (stmt.fetch()) 
			{
				var row = {};
				var colName, value, retrievedAs;
				var retrievedTypes = [];
				for (var iCol = 0; iCol < numCols; iCol++) 
				{
					colName = result.columns[iCol];
					[value, retrievedAs] = getAsTypePrepared(stmt, iCol, colTypes[iCol], options);
					retrievedTypes.push(retrievedAs);
					row[colName] = value;
				}
				log.trace("Retrieved row with the following column types: " + retrievedTypes.join("; "));
				result.addRow(row);
			}
			
			break;
			
		case RS_HAS_ROWCOUNT:
		
			log.trace("Fetching prepared statement's number of affected rows to set (or add to) total");

			if (!appendToResult) result = 0;
			
			result += rm.rowCount();
			
			break;
		
	}
	
	return result;

}

function executeWithoutStatement(conn, sql, options)
{
	// TODO: fix it such that even if there are no rows to return, the returned columns could be meaningful
	log.trace("Executing non-prepared statement");
	
	// First execute the query
	try 
	{
		var rs = conn.conn.query(sql);
	} 
	catch (e) 
	{
		var errorCode = conn.conn.errno();
		var errorString = conn.conn.error();
		var errorMessage = "MySQL error " + errorCode + " while executing non-prepared statement. Error message: " + errorString + ". SQL: " + sql;
		var err = new Exception(errorMessage, log);
		err[DB.SQL_ERROR_CODE_PROPERTY] = errorCode;
		err[DB.SQL_ERROR_DESCRIPTION_PROPERTY] = errorString;
		throw err;
	}
	
	var results = [];
	try
	{
		while (rs.type != RS_IS_NULL)
		{
			switch (rs.type)
			{
				case RS_HAS_RECORDS:
					var resultSet = new DB.ResultSet();
					var colTypes = [];
					var numCols = rs.fieldCount();
					for (var iCol = 0; iCol < numCols; iCol++) 
					{
						var field = rs.fetchFieldDirect(iCol);
						log.trace("Column " + iCol + ": " + field.name());
						resultSet.columns[iCol] = field.name();
						var declType = field.typeName();
						if (declType == VALUE_TYPE_BLOB) // May have to adjust it for TEXT-type BLOB
						{
							if (field.charsetnr() != BLOB_FLAG)
							{
								declType = VALUE_TYPE_TEXT;
							}
						}
						colTypes[iCol] = 
						{
							type: declType
						};
					}
					log.trace("Fetching non-prepared-statement return values to populate resultSet");
					while (rs.fetchRow()) 
					{
						var row = {};
						var colName, value, retrievedAs;
						var retrievedTypes = [];
						for (var iCol = 0; iCol < numCols; iCol++) 
						{
							colName = resultSet.columns[iCol];
							[value, retrievedAs] = getAsTypeQuery(rs, iCol, colTypes[iCol], options);
							retrievedTypes.push(retrievedAs);
							row[colName] = value;
						}
						if (log.getLevel() == Log.TRACE) log.trace("Retrieved row with the following column types: " + retrievedTypes.join("; "));
						resultSet.addRow(row);
					}
					results.push(resultSet);
					break;
				case RS_HAS_ROWCOUNT:
					results.push(rs.rowCount());
					break;
			}
			try
			{
				rs = conn.conn.nextResult();
			}
			catch (e)
			{
				var errorCode = conn.conn.errno();
				var errorString = conn.conn.error();
				var errorMessage = "MySQL error " + errorCode + " while executing query #" + results.length + ". Error message: " + errorString + ". SQL: " + sql;
				var err = new Exception(errorMessage, log);
				err[DB.SQL_ERROR_CODE_PROPERTY] = errorCode;
				err[DB.SQL_ERROR_DESCRIPTION_PROPERTY] = errorString;
				throw err;
			}
		}
	}
	finally 
	{
		if (rs)
		{
			rs.close();
		}
	}
	
	switch (results.length)
	{
		case 0:
			return null;
		case 1:
			return results[0];
		default:
			return results;
	}

}

/**
 * Retrieves the specified value from the prepared statement (at its current
 * cursor location), casting to the given type.
 * 
 * @private
 * @param {Object} stmt
 * 		The statement object
 * @param {Object} iCol
 * 		The index of the column whose value is to be fetched
 * @param {Object} iType
 * 		The type object containing information on how to return this value
 * @param {Object} options
 * 		An optional hashmap of options. Currently two options are supported: dateHandler, and useLocalTimezone.
 *		If dateHandler is given, it should be a function that takes
 * 		the raw MySQL date/time/datetime string representation of a returned cell value,
 * 		and a string describing the declared type,
 * 		and returns the desired JavaScript representation.
 * 		If dateHandler is not given, set useLocalTimezone to true to store date/time values using the local timezone.
 * 		Otherwise, by default they are stored as UTC.
 * @return {Array}
 * 		The value as the requested type, and the description of what type was used to retrieve it
 */
function getAsTypePrepared(stmt, iCol, iType, options)
{
	var value, retrievedAs;
	if (stmt.getIsNull(iCol)) 
	{
		value = null;
		retrievedAs = 'null';
	}
	else 
	{
		var mysqlType = iType.type;
		pageDate = Jaxer.pageWindow ? Jaxer.pageWindow.Date : Date;
		switch (mysqlType)
		{
			case VALUE_TYPE_NULL:
				value = null;
				retrievedAs = 'null';
				break;
			case VALUE_TYPE_NUMBER:
				value = stmt.getDouble(iCol);
				retrievedAs = 'double';
				break;
			case VALUE_TYPE_DATETIME:
				if (options && (typeof options.dateHandler == "function")) 
				{
					value = options.dateHandler(stmt.getDatetimeString(iCol), mysqlType);
				}
				else 
				{
					var dtValues = stmt.getDatetimeString(iCol).split(/[\-\:\s]0?/); // getDatetimeString: "YYYY-MM-DD HH:MM:SS" where 12>=MM>=1, 31>=DD>=1, 23>=HH>=0, 59>=MM>=0, 59>=SS>=0
					value = (options && options.useLocalTimezone) ?
						new pageDate(         dtValues[0], parseInt(dtValues[1]) - 1, dtValues[2], dtValues[3], dtValues[4], dtValues[5], 0) :
						new pageDate(Date.UTC(dtValues[0], parseInt(dtValues[1]) - 1, dtValues[2], dtValues[3], dtValues[4], dtValues[5], 0));
				}
				retrievedAs = 'datetime';
				break;
			case VALUE_TYPE_DATE:
				if (options && (typeof options.dateHandler == "function")) 
				{
					value = options.dateHandler(stmt.getDateString(iCol), mysqlType);
				}
				else 
				{
					var dateValues = stmt.getDateString(iCol).split(/\-0?/); // getDateString: "YYYY-MM-DD" where 12>=MM>=1, 31>=DD>=1
					value = (options && options.useLocalTimezone) ?
						new pageDate(         dateValues[0], parseInt(dateValues[1]) - 1, dateValues[2], 0) :
						new pageDate(Date.UTC(dateValues[0], parseInt(dateValues[1]) - 1, dateValues[2], 0));
				}
				retrievedAs = 'date';
				break;
			case VALUE_TYPE_TIME:
				if (options && (typeof options.dateHandler == "function")) 
				{
					value = options.dateHandler(stmt.getTimeString(iCol), mysqlType);
				}
				else 
				{
					var timeValues = stmt.getTimeString(iCol).split(/\:0?/); // getTimeString: "HH:MM:SS" where 23>=HH>=0, 59>=MM>=0, 59>=SS>=0
					value = (options && options.useLocalTimezone) ?
						new pageDate(         MySQL.TIME_COLUMN_YEAR, MySQL.TIME_COLUMN_MONTH, MySQL.TIME_COLUMN_DAY, timeValues[0], timeValues[1], timeValues[2], 0) :
						new pageDate(Date.UTC(MySQL.TIME_COLUMN_YEAR, MySQL.TIME_COLUMN_MONTH, MySQL.TIME_COLUMN_DAY, timeValues[0], timeValues[1], timeValues[2], 0));
				}
				retrievedAs = 'time';
				break;
			case VALUE_TYPE_STRING:
				value = stmt.getUTF8String(iCol);
				retrievedAs = 'string';
				break;
			case VALUE_TYPE_TEXT:
				value = stmt.getUTF8Text(iCol);
				retrievedAs = 'text';
				break;
			case VALUE_TYPE_BLOB:
				var blobLengthWrapper = {};
				var blobValueWrapper = {};
				stmt.getBlob(iCol, blobLengthWrapper, blobValueWrapper);
				value = blobValueWrapper.value;
				retrievedAs = 'blob';
				break;
			default:
				throw new Exception("MySQL statement returned an unknown data type: " + mysqlType, log);
		}
	}
	return [value, retrievedAs];
}

/**
 * Retrieves the specified value from the MySQL resultset (at its current cursor
 * location), casting to the given type.
 * 
 * @private
 * @param {Object} rs
 * 		The MySQL resultset
 * @param {Object} iCol
 * 		The index of the column whose value is to be fetched
 * @param {Object} iType
 * 		The type object containing information on how to return this value
 * @param {Object} options
 * 		An optional hashmap of options. Currently two options are supported: dateHandler, and useLocalTimezone.
 *		If dateHandler is given, it should be a function that takes
 * 		the raw MySQL date/time/datetime string representation of a returned cell value,
 * 		and a string describing the declared type,
 * 		and returns the desired JavaScript representation.
 * 		If dateHandler is not given, set useLocalTimezone to true to store date/time values using the local timezone.
 * 		Otherwise, by default they are stored as UTC.
 * @return {Array}
 * 		The value as the requested type, and the description of what type was used to retrieve it
 */
function getAsTypeQuery(rs, iCol, iType, options)
{
	var value, retrievedAs;
	if (rs.getIsNull(iCol)) 
	{
		value = null;
		retrievedAs = 'null';
	}
	else 
	{
		var mysqlType = iType.type;
		pageDate = Jaxer.pageWindow ? Jaxer.pageWindow.Date : Date;
		switch (mysqlType)
		{
			case VALUE_TYPE_NULL:
				value = null;
				retrievedAs = 'null';
				break;
			case VALUE_TYPE_NUMBER:
				value = rs.getDouble(iCol);
				retrievedAs = 'double';
				break;
			case VALUE_TYPE_DATETIME:
				if (options && (typeof options.dateHandler == "function")) 
				{
					value = options.dateHandler(rs.getDatetimeString(iCol), mysqlType);
				}
				else 
				{
					var dtValues = rs.getDatetimeString(iCol).split(/[\-\:\s]0?/); // getDatetimeString: "YYYY-MM-DD HH:MM:SS" where 12>=MM>=1, 31>=DD>=1, 23>=HH>=0, 59>=MM>=0, 59>=SS>=0
					value = (options && options.useLocalTimezone) ?
						new pageDate(         dtValues[0], parseInt(dtValues[1]) - 1, dtValues[2], dtValues[3], dtValues[4], dtValues[5], 0) :
						new pageDate(Date.UTC(dtValues[0], parseInt(dtValues[1]) - 1, dtValues[2], dtValues[3], dtValues[4], dtValues[5], 0));
				}
				retrievedAs = 'datetime';
				break;
			case VALUE_TYPE_DATE:
				if (options && (typeof options.dateHandler == "function")) 
				{
					value = options.dateHandler(rs.getDateString(iCol), mysqlType);
				}
				else 
				{
					var dateValues = rs.getDateString(iCol).split(/\-0?/); // getDateString: "YYYY-MM-DD" where 12>=MM>=1, 31>=DD>=1
					value = (options && options.useLocalTimezone) ?
						new pageDate(         dateValues[0], parseInt(dateValues[1]) - 1, dateValues[2], 0) :
						new pageDate(Date.UTC(dateValues[0], parseInt(dateValues[1]) - 1, dateValues[2], 0));
				}
				retrievedAs = 'date';
				break;
			case VALUE_TYPE_TIME:
				if (options && (typeof options.dateHandler == "function")) 
				{
					value = options.dateHandler(rs.getTimeString(iCol), mysqlType);
				}
				else 
				{
					var timeValues = rs.getTimeString(iCol).split(/\:0?/); // getTimeString "HH:MM:SS" where 23>=HH>=0, 59>=MM>=0, 59>=SS>=0
					value = (options && options.useLocalTimezone) ?
						new pageDate(         MySQL.TIME_COLUMN_YEAR, MySQL.TIME_COLUMN_MONTH, MySQL.TIME_COLUMN_DAY, timeValues[0], timeValues[1], timeValues[2], 0) :
						new pageDate(Date.UTC(MySQL.TIME_COLUMN_YEAR, MySQL.TIME_COLUMN_MONTH, MySQL.TIME_COLUMN_DAY, timeValues[0], timeValues[1], timeValues[2], 0));
				}
				retrievedAs = 'time';
				break;
			case VALUE_TYPE_STRING:
				value = rs.getUTF8String(iCol);
				retrievedAs = 'string';
				break;
			case VALUE_TYPE_TEXT:
				value = rs.getUTF8Text(iCol);
				retrievedAs = 'text';
				break;
			case VALUE_TYPE_BLOB:
				var blobLengthWrapper = {};
				var blobValueWrapper = {};
				rs.getBlob(iCol, blobLengthWrapper, blobValueWrapper);
				value = blobValueWrapper.value;
				retrievedAs = 'blob';
				break;
			default:
				throw new Exception("MySQL query returned an unknown data type: " + mysqlType, log);
		}
	}
	return [value, retrievedAs];
}

/**
 * Closes the connection if it's open. This is optional, and only does something
 * if the connection is open.
 * 
 * @alias Jaxer.DB.MySQL.Connection.prototype.close
 */
MySQL.Connection.prototype.close = function close()
{
	if (this.isOpen)
	{
		log.trace("Closing connection to database " + this.databaseName);
		this.conn.close();
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
 * @alias Jaxer.DB.MySQL.Connection.prototype.getLastInsertId
 * @return {Number}
 * 		The id, or 0
 */
MySQL.Connection.prototype.getLastInsertId = function getLastInsertId()
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

MySQL.Connection.prototype.getLastId = MySQL.Connection.prototype.getLastInsertId;

/**
 * Returns the unique ID used for an AUTO_INCREMENT column in the most recent successful
 * INSERT command on the current connection. 
 * If no successful INSERTs have ever occurred on this connection, 0 is returned. 
 * Note that unsuccessful INSERTs do not change this value.
 * This is a synonym for lastInsertId.
 * See http://dev.mysql.com/doc/refman/5.0/en/getting-unique-id.html for more details.
 * 
 * @alias Jaxer.DB.MySQL.Connection.prototype.lastInsertRowId
 * @property {Number}
 */
MySQL.Connection.prototype.__defineGetter__('lastInsertRowId', MySQL.Connection.prototype.getLastInsertId);

/**
 * Returns the unique ID used for an AUTO_INCREMENT column in the most recent successful
 * INSERT command on the current connection. 
 * If no successful INSERTs have ever occurred on this connection, 0 is returned. 
 * Note that unsuccessful INSERTs do not change this value.
 * This is a synonym for lastInsertRowId.
 * See http://dev.mysql.com/doc/refman/5.0/en/getting-unique-id.html for more details.
 * 
 * @alias Jaxer.DB.MySQL.Connection.prototype.lastInsertId
 * @property {Number}
 */
MySQL.Connection.prototype.__defineGetter__('lastInsertId', MySQL.Connection.prototype.getLastInsertId);

/**
 * Tests the connection by trying to connect and catching and returning any error 
 * encountered. If the connection is successful, returns a null.
 * 
 * @alias Jaxer.DB.MySQL.Connection.prototype.test
 * @param {Boolean} keepOpen
 * 		If true, the connection will be kept open (if the test was successful).
 * 		If false, the connection will be left in the same state as before the test:
 * 		if it was open before it will be kept open, otherwise it will be closed.
 * @return {Object}
 * 		If successful, returns null; if unsuccessful, returns the error.
 * 		Usually you can use the error's sqlErrorCode and sqlErrorDescription
 * 		to see what the error was (or just its toString() method).
 */
MySQL.Connection.prototype.test = function test(keepOpen)
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

DB.MySQL = MySQL;

Jaxer.Log.trace("*** DB_MySQL.js loaded");
	
})();
