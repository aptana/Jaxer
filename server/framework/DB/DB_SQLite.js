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

var log = Log.forModule("DB.SQLite");

/**
 * @namespace {Jaxer.DB.SQLite} Namespace that holds the SQLite implementation
 * of the Jaxer DB API.
 */

var SQLite = {};

var SQLITE_CLASS_ID		= "@mozilla.org/storage/service;1"
var SQLITE_INTERFACE	= Components.interfaces.mozIStorageService;

var VALUE_TYPE_NULL		= 0; 	// Null data type.
var VALUE_TYPE_INTEGER	= 1; 	// INTEGER data type.
var VALUE_TYPE_FLOAT	= 2; 	// FLOAT data type.
var VALUE_TYPE_TEXT		= 3; 	// TEXT data type.
var VALUE_TYPE_BLOB		= 4; 	// BLOB data type.

var SPECIAL_HANDLING_FLAG = "__!jaxerSpecialHandling!__";
var SPECIAL_HANDLING_FLAG_NUMBER = SPECIAL_HANDLING_FLAG + "(number)";
var SPECIAL_HANDLING_FLAG_NUMBER_LENGTH = SPECIAL_HANDLING_FLAG_NUMBER.length;

SQLite.IMPLEMENTATION = "SQLite";

/**
 * Validate the provided connection parameters
 * 
 * @alias Jaxer.DB.SQLite.validateConnectionParameters
 * @param {Object} connectionParams
 * 		A hashmap of parameters needed to connect to the database. The
 * 		properties required of connectionParams are:
 * 		PATH: the path to the file of the database
 */
SQLite.validateConnectionParameters = function validateConnectionParameters(connectionParams)
{
	var errors = [];
	['PATH'].forEach(function(propName)
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
		if (log.getLevel() == Log.TRACE) log.trace('Connection parameters OK :'+ uneval(connectionParams) )
		return; // no action if OK.
	}
}


/**
 * Creates a new database file according to connectionParams.PATH
 * 
 * @alias Jaxer.DB.SQLite.createDB
 * @param {Object} connectionParams
 * 		A hashmap of parameters needed to connect to the database. The
 * 		properties required of connectionParams are:
 * 		PATH: the path to the file of the database
 *  @return {Jaxer.DB.MySQL.Connection}
 *  	Returns an instance of Connection. 
*/
SQLite.createDB = function createDB(connectionParams)
{
	var conn, sql;
	
	conn = new SQLite.Connection(connectionParams);
	log.debug("Creating/verifying connection to database file at " + conn.filepath)

	try
	{
		conn.open();
		conn.close();
		return new SQLite.Connection(connectionParams);
	}
	catch (e)
	{
		if (conn) conn.close();
		throw e;
 	}
}

/**
 * @classDescription {Jaxer.DB.SQLite.Connection} Creates a new connection to
 * the given databaseName (file).
 */

/**
 * Creates a new connection to the given database (file). If the given database does not
 * yet exist, it is created for you when you try to open it. The resulting connection 
 * object is the only way you interact with the database.
 * 
 * @constructor
 * @alias Jaxer.DB.SQLite.Connection
 * @param {Object} connectionParams 
 * 		A hashmap of parameters for connecting to the database. Required
 * 		properties are:
 * 		<ul>
 * 			<li>PATH (the path to the database file).</li>
 *		</ul>
 * 		Optional parameters are:
 * 		<ul>
 * 			<li>CLOSE_AFTER_EXECUTE (whether to close the connection after each call to execute, default is false), and</li>
 * 			<li>CLOSE_AFTER_REQUEST (whether to close the connection after each request, default is true).</li>
 *		</ul>
 *  @return {Jaxer.DB.SQLite.Connection}
 *  	Returns an instance of Connection.
 */
SQLite.Connection = function Connection(connectionParams)
{
	SQLite.validateConnectionParameters(connectionParams);

	this.filepath = Dir.resolve(connectionParams.PATH, System.executableFolder);
	this.file = new Jaxer.File(this.filepath);
	if (!this.file)
	{
		throw new Exception("Could not access database file at path: " + this.filepath);
	}
	this.iFile = this.file.nsIFile;
	this.service = Components.classes[SQLITE_CLASS_ID].getService(SQLITE_INTERFACE);
	this.conn = null;
	this.closeAfterExecute = (typeof connectionParams.CLOSE_AFTER_EXECUTE == 'boolean') ? connectionParams.CLOSE_AFTER_EXECUTE : false;
	this.closeAfterRequest = (typeof connectionParams.CLOSE_AFTER_REQUEST == 'boolean') ? connectionParams.CLOSE_AFTER_REQUEST : true;
	this.maxNumTries = connectionParams.MAX_NUM_TRIES || 100;
	this.millisBetweenTries = connectionParams.MILLIS_BETWEEN_TRIES || 37;
};

/**
 * Is the connection currently open? Recall that even if the answer is no
 * the connection would automatically be opened when needed.
 * 
 * @alias Jaxer.DB.SQLite.Connection.prototype.isOpen
 * @property {Boolean}
 */
SQLite.Connection.prototype.__defineGetter__('isOpen', function getIsOpen()
{
	return Boolean(this.conn && this.conn.connectionReady);
});

/**
 * Returns the string identifying the database implementation of this connection.
 * You can compare this e.g. to Jaxer.DB.SQLite.IMPLEMENTATION or
 * Jaxer.DB.MySQL.IMPLEMENTATION
 * 
 * @alias Jaxer.DB.SQLite.Connection.prototype.implementation
 * @property {String}
 */
SQLite.Connection.prototype.__defineGetter__('implementation', function getImplementation()
{
	return SQLite.IMPLEMENTATION;
});

/**
 * Returns the string identifying the version of the database to which you are connected.
 * 
 * @alias Jaxer.DB.SQLite.Connection.prototype.version
 * @property {String}
 */
SQLite.Connection.prototype.__defineGetter__('version', function getVersion()
{
	return this.execute("SELECT sqlite_version()").singleResult;
});

/**
 * Opens the connection so queries can be executed. This is optional, since if
 * the connection is not open when it's asked to execute some SQL, it will open
 * the connection automatically. Also closing the connection is optional.
 * 
 * @alias Jaxer.DB.SQLite.Connection.prototype.open
 */
SQLite.Connection.prototype.open = function open()
{
	if (!this.isOpen)
	{
		log.debug("Opening connection to database: " + this.filepath);
		var numTries = 0;
		var err = null;
		var timer = (new Date()).getTime();
		var errorCode = 0;
		var errorString = '';
		while (numTries < this.maxNumTries) 
		{
			try 
			{
				err = null;
				this.conn = this.service.openDatabase(this.iFile);
				break;
			} 
			catch (e) 
			{
				if (this.conn && this.conn.conectionReady) 
				{
					errorString = this.conn.lastErrorString;
					errorCode = this.conn.lastError;
				}
				else
				{
					if (!this.file.exists) 
					{
						errorString = "Database file not found at " + this.filepath;
						errorCode = -1;
					}
					else 
					{
						errorString = "Error initializing connection to database file " + this.filepath + ": " + e.message;
						errorCode = e.result;
					}
				}
				err = "Error opening database: " + errorString + "; \nmozStorage exception: " + e;
				if ((errorCode != 5) && (errorCode != 6) && (errorCode != Components.results.NS_ERROR_FILE_IS_LOCKED)) // not due to a locking issue -- so don't bother retrying
				{
					break; 
				}
				numTries++;
				Util.sleep(this.millisBetweenTries);
			}
		}
		var msgStats = 'Tried ' + (numTries + 1) + ' time(s) in ' + ((new Date()).getTime() - timer) + ' ms to open connection to SQLite database file ' + this.filepath;
		if (err)
		{
			log.debug(msgStats);
			err = new Exception(err);
			if (errorCode) err[DB.SQL_ERROR_CODE_PROPERTY] = errorCode;
			if (errorString) err[DB.SQL_ERROR_DESCRIPTION_PROPERTY] = errorString;
			throw err;
		}
		else
		{
			log.trace(msgStats)
		}
		log.debug("Opened");
	}
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
 * @alias Jaxer.DB.SQLite.Connection.prototype.execute
 * @param {String} sql
 * 		The sql statement to be executed as a prepared statement
 * @return {Jaxer.DB.ResultSet, Number, Object[]}
 * 		The results of the query.
 * 		For a SELECT-type query, a Jaxer.DB.ResultSet is returned, with 0 or more rows.
 * 		For an INSERT/UPDATE/DELETE-type query, the number of rows affected is returned. 
 */
SQLite.Connection.prototype.execute = function execute(sql, params)
{
	log.trace("Starting execute");
	
	sql = massageSql(sql);
	
	this.open(); // In case it hasn't been opened
	try
	{
		// determine what params we've been given, if any
		var params, options;
		[params, options] = DB.getParamsFromArgs(arguments, 1);
		var result = executeWithStatement(this, sql, [params], options)[0];
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
 * @alias Jaxer.DB.SQLite.Connection.prototype.mapExecute
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
 * @return {Object}
 * 		A corresponding array of Jaxer.DB.ResultSets or Numbers for each query, 
 * 		or a combined Jaxer.DB.ResultSet or Number if the 'flatten' option is true.
 * 		For SELECT-type queries one or more Jaxer.DB.ResultSets are returned;
 * 		for INSERT/UPDATE/DELETE-type queries the number of affected rows is returned.
 * @see Jaxer.DB.ResultSet
 */
SQLite.Connection.prototype.mapExecute = function mapExecute(sql, arrayOfParams, options)
{
	log.trace("Starting execute");
	var result;
	
	sql = massageSql(sql);

	this.open(); // In case it hasn't been opened
	var transactionBegun = false;
	try
	{
		if (!this.conn.transactionInProgress)
		{
			transactionBegun = true;
			this.conn.beginTransactionAs(this.conn.TRANSACTION_DEFERRED);
			log.trace("Beginning a new transaction");
		}
		result = executeWithStatement(this, sql, arrayOfParams, options);
	}
	finally
	{
		try
		{
			if (transactionBegun)
			{
				log.trace("Committing the transaction");
				this.conn.commitTransaction();
			}
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

// Since SQLite uses a slightly different syntax for the following very specific and common case,
// rewrite the more common syntax to adhere to SQLite's syntax
function massageSql(sql)
{
	return DB.sqlToString(sql, log).replace(/INTEGER PRIMARY KEY AUTO_INCREMENT/gi, "INTEGER PRIMARY KEY AUTOINCREMENT");
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
			stmt = conn.conn.createStatement(sql);
		}
		catch (e)
		{
			e = new Exception("Error preparing statement: [" + conn.conn.lastError + "] " + conn.conn.lastErrorString + "; \nSQL: " + sql + "; \nmozStorage exception: " + e, log);
			e[DB.SQL_ERROR_CODE_PROPERTY] = conn.conn.lastError;
			e[DB.SQL_ERROR_DESCRIPTION_PROPERTY] = conn.conn.lastErrorString;
			throw e;
		}
		if (!stmt)
		{
			throw new Exception("Could not prepare statement: " + sql, log);
		}
		
		arrayOfParams.forEach(function executeOneWithStatement(params)
		{
			params = DB.getParamsFromArgs([params], 0)[0];
			var stmtParams = stmt.parameterCount;
			if (params.length != stmtParams)
			{
				throw new Exception("Prepared statement " + sql + " has " + stmtParams + " parameters while " + params.length + " parameter values were given", log);
			}
			
			result = bindAndExecute(conn, stmt, sql, params, result, options);
			if (!flatten) 
			{
				results.push(result);
				result = null;
			}
			
			stmt.reset();
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
				log.trace("Resetting and finalizing statement");
				stmt.reset(); // Important to always do this to prevent locks! http://developer.mozilla.org/en/docs/Storage#Resetting_a_statement
				stmt.finalize();
			} 
			catch (e) // log but do not throw again
			{
				log.error("Error trying to reset and finalize the statement: " + e);
			}
		}
	}

	return results;

}

// If you pass in a resultSet or number, the results of this execution will be appended to it.
// If you pass in null, a new resultSet or number will be created.
// In either case, the resultSet or number is returned.
// We don't currently do anything in here with the options hashmap -- it's reserved for future use
function bindAndExecute(conn, stmt, sql, params, result, options)
{

	// First bind the parameters
	if (params && params.length) // bind them
	{
		log.trace("Binding statement " + sql + "\nwith params: " + params);
		params.forEach(function(param, index)
		{
			switch (typeof param)
			{
				case "number":
					if (isFinite(param))
					{
						stmt.bindDoubleParameter(index, param);
					}
					else
					{
						stmt.bindStringParameter(index, SPECIAL_HANDLING_FLAG_NUMBER + param.toString());
					}
					break;
				case "boolean":
					stmt.bindInt32Parameter(index, (param ? 1 : 0));
					break;
				case "string":
					stmt.bindStringParameter(index, param);
					break;
				case "object":
					if (param == null)
					{
						stmt.bindNullParameter(index);
					}
					else if (Util.isDate(param))
					{
						var offset = (options && options.useLocalTimezone) ? param.getTimezoneOffset() * 60 * 1000 : 0;
						stmt.bindInt64Parameter(index, param.getTime() - offset); // milliseconds since midnight 1/1/1970.
					}
					else
					{
						log.warn("Parameter " + index + " is not a Date (the only parameter of JavaScript type 'object' that can be persisted) - using NULL instead. SQL: " + sql, log);
						stmt.bindNullParameter(index);
					}
					break;
				case "undefined":
					stmt.bindNullParameter(index);
					break;
				default:
					log.warn("Parameter " + index + " is of an unsupported type (" + (typeof param) + " - using NULL instead. SQL: " + sql, log);
					stmt.bindNullParameter(index);
					break;
			}
		});
	}
		
	// Now execute the statement and return any values as a resultset or number
	// TODO: fix it such that even if there are no rows, meaningful columns could be returned
	var appendToResult = (result != null); // If you pass in null, a new resultSet or number will be created
	var columnsSet = appendToResult;
	var iRow = 0;
	var execute = true;
	while (execute)
	{
		var numTries = 0;
		var err = null;
		var timer = (new Date()).getTime();
		// We'll try multiple times if we get an error because of the database or a table being locked
		var errorCode = 0;
		var errorString = '';
		while (numTries < conn.maxNumTries) 
		{
			try 
			{
				err = null;
				execute = stmt.executeStep();
				break;
			} 
			catch (e) 
			{
				errorString = conn.conn.lastErrorString;
				errorCode = conn.conn.lastError;
				err = "Error executing statement: [" + errorCode + "] " + errorString + "; \nSQL: " + sql + "; \nmozStorage exception: " + e;
				if ((errorCode != 5) && (errorCode != 6)) // not due to a locking issue -- so don't bother retrying
				{
					break; 
				}
				Util.sleep(conn.millisBetweenTries);
			}
			numTries++;
		}
		var msgStats = 'Tried ' + (numTries + 1) + ' time(s) in ' + ((new Date()).getTime() - timer) + ' ms to execute statement on SQLite database file ' + conn.filepath;
		if (err)
		{
			if (numTries > 0) log.warn(msgStats);
			err = new Exception(err);
			if (errorCode) err[DB.SQL_ERROR_CODE_PROPERTY] = errorCode;
			if (errorString) err[DB.SQL_ERROR_DESCRIPTION_PROPERTY] = errorString;
			throw err;
		}
		else
		{
			log.trace(msgStats)
		}
		
		var returnResultSet = (/^\W*SELECT\b/i).test(sql); // TODO: This will need to be more robust when stored procedures are supported
		
		if (returnResultSet) // There is resultSet to be had
		{
			if (!appendToResult) 
			{
				result = new DB.ResultSet();
				appendToResult = true;
			}
			if (execute) 
			{
				var numCols = stmt.columnCount;
				// The first time only, get the names of the columns in the returned value
				if (!columnsSet) 
				{
					log.trace("Populating resultSet");
					for (var iCol = 0; iCol < numCols; iCol++) 
					{
						log.trace("Column " + iCol + ": " + stmt.getColumnName(iCol));
						result.columns[iCol] = stmt.getColumnName(iCol);
					}
					columnsSet = true;
				}
				var row = {};
				for (var iCol = 0; iCol < numCols; iCol++) 
				{
					var colType = 
					{
						declared: stmt.getColumnDecltype(iCol),
						sqlite: stmt.getTypeOfIndex(iCol)
					};
					var colName = result.columns[iCol];
					var value = getAsType(stmt, iCol, colType, options);
					row[colName] = value;
				}
				result.addRow(row);
				iRow++;
			}
		}
		else // There's just the number f affected rows to return
		{
			if (!appendToResult) 
			{
				result = 0;
				appendToResult = true;
			}
			result += stmt.changedRowCount;
		}
	}
	
	return result;
	
}

/**
 * Retrieves the specified value from the prepared statement (at its current
 * cursor location), casting to the given type.
 * 
 * @private
 * @alias Jaxer.DB.SQLite.getAsType
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
 * @return {String}
 * 		The value as the requested type
 */
function getAsType(stmt, iCol, iType, options)
{
	if (stmt.getIsNull(iCol)) return null;
	var declaredType = iType.declared;
	var sqliteType = iType.sqlite;
	if (sqliteType == VALUE_TYPE_NULL) return null;
	var value;
	switch (sqliteType)
	{
		case VALUE_TYPE_INTEGER:
			value = stmt.getInt64(iCol);
			break;
		case VALUE_TYPE_FLOAT:
			value = stmt.getDouble(iCol);
			break;
		case VALUE_TYPE_TEXT:
			value = stmt.getString(iCol);
			break;
		case VALUE_TYPE_BLOB:
			value = stmt.getString(iCol); // TODO: revisit blobs
			break;
		default:
			throw new Exception("SQLite statement returned an unknown data type: " + sqliteType, log);
	}
	if (declaredType.match(/bool/i))
	{
		value = Boolean(value);
	}
	else if ((declaredType.match(/date/i) || declaredType.match(/time/i)) && (sqliteType == VALUE_TYPE_INTEGER))
	{
		var offset = (options && options.useLocalTimezone) ? param.getTimezoneOffset() * 60 * 1000 : 0;
		value = (options && (typeof options.dateHandler == "function")) ? options.dateHandler(value, declaredType) : new Date(value + offset);
	}
	else if ((sqliteType == VALUE_TYPE_TEXT) && (value.indexOf(SPECIAL_HANDLING_FLAG) == 0))
	{
		if (value.indexOf(SPECIAL_HANDLING_FLAG_NUMBER) == 0)
		{
			value = Number(value.substr(SPECIAL_HANDLING_FLAG_NUMBER_LENGTH));
		}
	}
	return value;
}

/**
 * Closes the connection if it's open. This is optional, and only does something
 * if the connection is open.
 * 
 * @alias Jaxer.DB.SQLite.Connection.prototype.close
 */
SQLite.Connection.prototype.close = function close()
{
	if (this.isOpen) 
	{
		log.trace("Closing the connection");
		this.conn.close();
		this.conn = null;
	}
}

/**
 * Returns the unique "rowid" of the most recent successful INSERT command
 * on the current connection. If the table has a column of type INTEGER PRIMARY KEY,
 * this is used as the rowid. If no successful INSERTs have ever occurred on this
 * connection, 0 is returned. Note that unsuccessful INSERTs do not change this value.
 * This is the same as asking for the lastInsertRowId or lastInsertId properties.
 * See http://www.sqlite.org/c3ref/last_insert_rowid.html for more details.
 * 
 * @alias Jaxer.DB.SQLite.Connection.prototype.getLastInsertRowId
 * @return {Number}
 * 		The rowid, or 0
 */
SQLite.Connection.prototype.getLastInsertRowId = function getLastInsertRowId()
{
	if (this.isOpen) 
	{
		return this.conn.lastInsertRowID;
	}
	else
	{
		return 0;
	}
}

SQLite.Connection.prototype.getLastId = SQLite.Connection.prototype.getLastInsertRowId;

/**
 * Returns the unique "rowid" of the most recent successful INSERT command
 * on the current connection. If the table has a column of type INTEGER PRIMARY KEY,
 * this is used as the rowid. If no successful INSERTs have ever occurred on this
 * connection, 0 is returned. Note that unsuccessful INSERTs do not change this value.
 * This is a synonym for lastInsertId.
 * See http://www.sqlite.org/c3ref/last_insert_rowid.html for more details.
 * 
 * @alias Jaxer.DB.SQLite.Connection.prototype.lastInsertRowId
 * @property {Number}
 */
SQLite.Connection.prototype.__defineGetter__('lastInsertRowId', SQLite.Connection.prototype.getLastInsertRowId);

/**
 * Returns the unique "rowid" of the most recent successful INSERT command
 * on the current connection. If the table has a column of type INTEGER PRIMARY KEY,
 * this is used as the rowid. If no successful INSERTs have ever occurred on this
 * connection, 0 is returned. Note that unsuccessful INSERTs do not change this value.
 * This is a synonym for lastInsertRowId.
 * See http://www.sqlite.org/c3ref/last_insert_rowid.html for more details.
 * 
 * @alias Jaxer.DB.SQLite.Connection.prototype.lastInsertId
 * @property {Number}
 */
SQLite.Connection.prototype.__defineGetter__('lastInsertId', SQLite.Connection.prototype.getLastInsertRowId);

/**
 * Tests the connection by trying to connect and catching and returning any error 
 * encountered. If the connection is successful, returns a null.
 * 
 * @alias Jaxer.DB.SQLite.Connection.prototype.test
 * @param {Boolean} keepOpen
 * 		If true, the connection will be kept open (if the test was successful).
 * 		If false, the connection will be left in the same state as before the test:
 * 		if it was open before it will be kept open, otherwise it will be closed.
 * @return {Object}
 * 		If successful, returns null; if unsuccessful, returns the error.
 * 		Usually you can use the error's sqlErrorCode and sqlErrorDescription
 * 		to see what the error was (or just its toString() method).
 */
SQLite.Connection.prototype.test = function test(keepOpen)
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

DB.SQLite = SQLite;

Jaxer.Log.trace("*** DB_SQLite.js loaded");
	
})();
