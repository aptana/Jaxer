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

(function(){

var log = Log.forModule("DB"); // Only if Log itself is defined at this point of the includes

/**
 * @namespace {Jaxer.DB} The namespace that holds functions and other objects
 * for working with a database.
 */
var DB =
{
	defaultConnections: {},
	SQL_ERROR_CODE_PROPERTY: 'sqlErrorCode',
	SQL_ERROR_DESCRIPTION_PROPERTY: 'sqlErrorDescription',
	FRAMEWORK: "FRAMEWORK_" + Math.random() + (new Date()).getTime()
};

/**
 * Returns a boolean value to indicate whether a connection exists (regardless
 * of whether it's open) with the given key
 * 
 * @private  
 * @alias Jaxer.DB.isSpecified
 * @param {String} name 
 * 		The name of the connection, usually the name of the application
 * @return {Boolean}
 * 		True of the connection has been defined
 */
DB.isSpecified = function isSpecified(name)
{
	return Boolean(defaultConnections[name]);
};

/**
 * The default implementation of execute if there is no connection: just throws
 * an exception
 * 
 * @private
 * @alias Jaxer.DB.executeWithoutConnection
 */
DB.executeWithoutConnection = function executeWithoutConnection()
{
	throw new Exception("Attempted to execute SQL query without having a valid default connection", log);
};

DB.execute = DB.frameworkExecute = DB.executeWithoutConnection; // The default value, before initialization

/**
 * The default implementation of a connection if there is no implementation
 * defined: just throws an exception
 * 
 * @private
 * @alias Jaxer.DB.connectionWithoutImplementation
 */
DB.connectionWithoutImplementation = function connectionWithoutImplementation()
{
	throw new Exception("Attempted to create a new Connection without having a valid default DB implementation", log);
};

/**
 * Initializes the database subsystem, including the database used by the framework itself
 * and any databases that will be needed by the apps in configApps.js whose names are known
 * in advance (i.e. they're not determined dynamically by the request's URL).
 * 
 * @private
 * @alias Jaxer.DB.init
 */
DB.init = function init()
{
	DB.initDefault(DB.FRAMEWORK, Config.FRAMEWORK_DB);
	DB.setDefault(DB.FRAMEWORK); // This sets the default actions for the framework connection
	
	for (var appName in Config.appsByName)
	{
		var app = Config.appsByName[appName];
		if (app.DB)
		{
			DB.initDefault(appName, app.DB);
		}
	}

}

/**
 * Initializes a default database connection, which entails creating it (not necessarily opening it)
 * and adding it to the Jaxer.DB.defaultConnections hashmap
 * 
 * @advanced
 * @alias Jaxer.DB.initDefault
 * @param {String} name
 * 		The (resolved) name of the app for which this connection should be used
 * @param {Object} params
 * 		The parameters to use for the database connection. In particular this object
 * 		must have an IMPLEMENTATION property equal to "MySQL", "SQLite", or other 
 * 		supported databases. 
 */
DB.initDefault = function initDefault(name, params)
{
	var isFramework = (name == DB.FRAMEWORK);
	var impl = DB[params.IMPLEMENTATION];
	if (!impl)
	{
		var message = "The IMPLEMENTATION property " + params.IMPLEMENTATION + " of the database params was not set to a reasonable value: ";
		Jaxer.Log.error(message + "params = " + uneval(params));
		throw message + "see the log file for more information";
	}
	log.trace("Creating database (if necessary) for " + (isFramework ? "Jaxer framework" : "app: " + name));
	impl.createDB(params);

	var conn = new impl.Connection(params);
	DB.defaultConnections[name] = conn;
	return conn;
};

/**
 * Sets the given named connection as the default one from now on (though this
 * is by default set anew for each request). If the given name is not already
 * a recognized (i.e. initialized) connection, it will first be initialized.
 * 
 * @advanced
 * @alias Jaxer.DB.setDefault
 * @param {String} name
 * 		The (resolved) name of the app for which this connection should be used
 * @param {Object} [params]
 * 		The parameters to use for the database connection if the name is not
 * 		already an initialized connection. In particular this object
 * 		must have an IMPLEMENTATION property equal to "MySQL", "SQLite", or other 
 * 		supported databases. 
 */
DB.setDefault = function setDefault(name, params)
{
	var isFramework = (name == DB.FRAMEWORK);
	log.trace("Setting default connection for " + (isFramework ? "Jaxer framework" : "app: " + name));

	var connectionName = isFramework ? "frameworkConnection" : "connection";
	var constructorName = isFramework ? "FrameworkConnection" : "Connection";
	var exec = isFramework ? "frameworkExecute" : "execute";
	var mapExec = isFramework ? "mapFrameworkExecute" : "mapExecute";
	var lastId = isFramework ? "frameworkLastInsertId" : "lastInsertId";
	var lastRowId = isFramework ? "frameworkLastInsertRowId" : "lastInsertRowId";

	var conn = DB.defaultConnections[name];
	if (!conn && params) // initialize a new default connection
	{
		conn = DB.initDefault(name, params);
	}
	
	if (conn)
	{
		DB[connectionName] = conn;
		DB[constructorName] = DB[conn.implementation].Connection; // This is the constructor for the current implementation's Connection
		DB[exec] = function() { return DB.defaultConnections[name].execute.apply(DB.defaultConnections[name], arguments)};
		DB[mapExec] = function() { return DB.defaultConnections[name].mapExecute.apply(DB.defaultConnections[name], arguments)};
		DB.__defineGetter__(lastId, function() { return this.defaultConnections[name].getLastId(); });
		DB.__defineGetter__(lastRowId, function() { return this.defaultConnections[name].getLastId(); });
	}
	else
	{
		DB[connectionName] = DB.connectionWithoutImplementation;
		DB[constructorName] = null;
		DB[exec] = DB.executeWithoutConnection;
		DB[mapExec] = DB.executeWithoutConnection;
		DB.__defineGetter__(lastId, function() { throw new Exception("Attempted to access " + lastId + " without having a valid default DB connection", log); });
		DB.__defineGetter__(lastRowId, function() { throw new Exception("Attempted to access " + lastRowId + " without having a valid default DB connection", log); });
	}
}

/**
 * A hashmap that holds references to the currently-recognized default 
 * database connections, according to configApps.js, config.js, and the
 * page requests encountered so far. It's keyed off the (resolved) name
 * of the current app.
 * 
 * @advanced
 * @alias Jaxer.DB.defaultConnections
 * @property {Object}
 */

/**
 * Holds a reference to the constructor of the current default database connection used
 * for database interactions, e.g. by Jaxer.DB.execute. This is determined
 * by the settings in configApps.js and in config.js. It can be different
 * for different requests. Its type is Jaxer.DB.MySQL.Connection or
 * Jaxer.DB.SQLite.Connection, etc.
 * 
 * @advanced
 * @alias Jaxer.DB.Connection
 * @property {Object}
 */

/**
 * Holds a reference to the constructor of the current default connection used
 * for the Jaxer framework's internal database interactions, e.g. by 
 * Jaxer.DB.frameworkExecute. This is determined by the settings in config.js. 
 * Its type is Jaxer.DB.MySQL.Connection or Jaxer.DB.SQLite.Connection, etc.
 * 
 * @advanced
 * @alias Jaxer.DB.FrameworkConnection
 * @property {Object}
 */

/**
 * Holds a reference to the current default connection that will be used
 * for database interactions, e.g. by Jaxer.DB.execute. This is determined
 * by the settings in configApps.js and in config.js. It can be different
 * for different requests. Its type is Jaxer.DB.MySQL.Connection or
 * Jaxer.DB.SQLite.Connection, etc.
 * 
 * @alias Jaxer.DB.connection
 * @property {Object}
 * @see Jaxer.DB.Connection
 */

/**
 * Holds a reference to the current default connection that will be used
 * for the Jaxer framework's internal database interactions, e.g. by 
 * Jaxer.DB.frameworkExecute. This is determined by the settings in config.js. 
 * Its type is Jaxer.DB.MySQL.Connection or Jaxer.DB.SQLite.Connection, etc.
 * 
 * @alias Jaxer.DB.frameworkConnection
 * @property {Object}
 * @see Jaxer.DB.FrameworkConnection
 */

/**
 * Executes the given SQL query string on the current default database 
 * (as defined in configApps.js). If the SQL includes ?'s (question
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
 *			rs = Jaxer.DB.execute("SELECT * FROM myTable");
 * 			rs = Jaxer.DB.execute("SELECT * FROM myTable WHERE id=? AND zip=?", myId, myZip);
 * 			rs = Jaxer.DB.execute("SELECT * FROM myTable WHERE id=? AND zip=?", [myId, myZip]);
 * 		</pre>
 * @alias Jaxer.DB.execute
 * @param {String} sql
 * 		The SQL to execute.
 * @return {Jaxer.DB.ResultSet, Number, Object[]}
 * 		The results of the query.
 * 		For a SELECT-type query, a Jaxer.DB.ResultSet is returned, with 0 or more rows.
 * 		For an INSERT/UPDATE/DELETE-type query, the number of rows affected is returned. 
 * 		On MySQL only: if multiple queries were issued (or a stored procedure was executed)  
 * 		the result will be a corresponding array of Jaxer.DB.ResultSet or Number objects.
 * @see Jaxer.DB.ResultSet
 */

/**
 * Prepares the given SQL query string on the current default database 
 * (as defined in configApps.js) and then iteratively executes it
 * over the given array of parameters.
 * 
 * @example
 *		<pre>
 * 			[rsA, rsB] = Jaxer.DB.mapExecute("SELECT * FROM myTable WHERE id=?", [idA, idB]);
 * 			[rsA, rsB] = Jaxer.DB.mapExecute("SELECT * FROM myTable WHERE id=? AND zip=?", [ [idA, zipA], [idB, zipB] ]);
 * 		</pre>
 * @alias Jaxer.DB.mapExecute
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
 * @see Jaxer.DB.ResultSet
 */

/**
 * Executes the given SQL query string on the default framework database 
 * (as defined in Jaxer.Config.DB_FRAMEWORK). If the SQL includes ?'s (question
 * marks) as parameter placeholders, the values of those parameters
 * should be passed in as extra arguments to this function, either
 * as individual arguments or as a single array.
 * 
 * @example
 *		<pre>
 *			rs = Jaxer.DB.frameworkExecute("SELECT * FROM containers");
 * 			rs = Jaxer.DB.frameworkExecute("SELECT * FROM containers WHERE id=? AND name=?", myId, myName);
 * 			rs = Jaxer.DB.frameworkExecute("SELECT * FROM containers WHERE id=? AND name=?", [myId, myName]);
 * 		</pre>
 * @advanced
 * @alias Jaxer.DB.frameworkExecute
 * @param {String} sql
 * 		The SQL to execute.
 * @return {Jaxer.DB.ResultSet}
 * 		The results of the query (which may be an empty resultset)
 * @see Jaxer.DB.ResultSet
 */

/**
 * Prepares the given SQL query string on the default framework database 
 * (as defined in Jaxer.Config.DB_FRAMEWORK) and then iteratively executes it
 * over the given array of parameters.
 * 
 * @example
 *		<pre>
 * 			[rsA, rsB] = Jaxer.DB.mapFrameworkExecute("SELECT * FROM containers WHERE id=?", [idA, idB]);
 * 			[rsA, rsB] = Jaxer.DB.mapFrameworkExecute("SELECT * FROM containers WHERE id=? AND name=?", [ [idA, nameA], [idB, nameB] ]);
 * 		</pre>
 * @alias Jaxer.DB.mapFrameworkExecute
 * @param {String} sql
 * 		The SQL to execute, using ?'s (question marks) as parameter placeholders
 * @param {Array} arrayOfParameters
 * 		An array of parameters to use for each execution. Each element of the array
 * 		may itself be a single value or an array of values (corresponding to
 * 		the ?'s in the SQL)
 * @param {Object} [options]
 * 		An optional hashmap of options. Currently one option is supported: flatten.
 * 		If its value is true, the returned result will be a single ResultSet
 * 		with its rows being the concatenated rows of each query.
 * @return {Object}
 * 		A corresponding array of Jaxer.DB.ResultSet's for each query, 
 * 		or a single Jaxer.DB.ResultSet if the 'flatten' option is true.
 * @see Jaxer.DB.ResultSet
 */

/**
 * When you INSERT a row that has an AUTO_INCREMENT-type column in the default developer database, the value of that 
 * column in this row is automatically set by the database. To know what it was set to (an integer), 
 * retrieve the lastInsertId right after you execute the INSERT statement. Under other circumstances, 
 * the behavior of lastInsertId depends on the database implementation; for example, SQLite always 
 * returns the rowid of the last successfully inserted row, whether or not it had an 
 * INTEGER PRIMARY KEY AUTO_INCREMENT column.
 * 
 * @alias Jaxer.DB.lastInsertId
 * @property {Number}
 */

/**
 * When you INSERT a row that has an AUTO_INCREMENT-type column in the default developer database, the value of that 
 * column in this row is automatically set by the database. To know what it was set to (an integer), 
 * retrieve the lastInsertRowId right after you execute the INSERT statement. Under other circumstances, 
 * the behavior of lastInsertRowId depends on the database implementation; for example, SQLite always 
 * returns the rowid of the last successfully inserted row, whether or not it had an 
 * INTEGER PRIMARY KEY AUTO_INCREMENT column.
 * 
 * @alias Jaxer.DB.lastInsertRowId
 * @property {Number}
 */

/**
 * When you INSERT a row that has an AUTO_INCREMENT-type column in the default framework database, the value of that 
 * column in this row is automatically set by the database. To know what it was set to (an integer), 
 * retrieve the lastInsertId right after you execute the INSERT statement. Under other circumstances, 
 * the behavior of lastInsertId depends on the database implementation; for example, SQLite always 
 * returns the rowid of the last successfully inserted row, whether or not it had an 
 * INTEGER PRIMARY KEY AUTO_INCREMENT column.
 * 
 * @advanced
 * @alias Jaxer.DB.lastInsertId
 * @property {Number}
 */

/**
 * When you INSERT a row that has an AUTO_INCREMENT-type column in the default framework database, the value of that 
 * column in this row is automatically set by the database. To know what it was set to (an integer), 
 * retrieve the lastInsertRowId right after you execute the INSERT statement. Under other circumstances, 
 * the behavior of lastInsertRowId depends on the database implementation; for example, SQLite always 
 * returns the rowid of the last successfully inserted row, whether or not it had an 
 * INTEGER PRIMARY KEY AUTO_INCREMENT column.
 * 
 * @advanced
 * @alias Jaxer.DB.lastInsertRowId
 * @property {Number}
 */

/**
 * Closes all the default DB connections that are open. This is called by the
 * framework when there is a fatal or response error, or after every request if
 * CLOSE_AFTER_REQUEST is set to true in the connection's configuration
 * parameters
 * 
 * @advanced
 * @alias Jaxer.DB.closeAllConnections
 * @param {Boolean} onlyIfCloseAfterRequest
 * 		Set this to true if you only want to close connections whose
 * 		CLOSE_AFTER_REQUEST parameter is true
 */
DB.closeAllConnections = function closeAllConnections(onlyIfCloseAfterRequest)
{
	for (var connName in this.defaultConnections) 
	{
		var conn = this.defaultConnections[connName];
		if (conn && 
			(typeof conn.close == "function") &&
			(!onlyIfCloseAfterRequest || conn.closeAfterRequest)) 
		{
			conn.close();
		}
	}
}

/**
 * Attempts to convert the given sql to a string, if needed, and then trim it.
 * If the conversion fails or the string is empty, throws a Jaxer.Exception.
 * 
 * @advanced
 * @alias Jaxer.DB.sqlToString
 * @param {Object} sql
 * 		The sql to convert to a string, if needed
 * @param {Jaxer.Log.ModuleLogger} [log]
 * 		The logger into which errors should be logged. Defaults to the DB
 * 		module's logger.
 * @return {String}
 * 		The string representation of the sql.
 */
DB.sqlToString = function sqlToString(sql, logger)
{
	var e = null;
	logger |= log;
	
	if (typeof sql == 'xml')
	{
		sql = String(sql);
	}
	else if ((typeof sql != "string") && 
		(typeof sql != "undefined") && 
		(sql != null) &&
		(typeof sql.toString == "function"))
	{
		sql = sql.toString();
	}
	
	if (typeof sql == "string") 
	{
		sql = Jaxer.Util.String.trim(sql);
		if (sql == '')
		{
			e = new Exception("The 'sql' parameter must not be an empty string", logger);
		}
	}
	else
	{
		e = new Exception("The 'sql' parameter must be of type 'string' (not '" + typeof sql + "')", logger);
	}
	
	if (e)
	{
		e[DB.SQL_ERROR_CODE_PROPERTY] = -1;
		e[DB.SQL_ERROR_DESCRIPTION_PROPERTY] = e.message;
		throw e;
	}
	
	return sql;
}

/**
 * Inspects the caller's arguments "array" to determine whether it has been 
 * given prepared-statements parameters and if so whether they are already
 * array-like (versus not, in which case they'll need to be wrapped in an
 * Array).
 * 
 * @private
 * @alias Jaxer.DB.getParamsFromArgs
 * @param {Arguments} args
 * 		The arguments "array" of the caller (within a function, use: arguments)
 * @param {Number} paramsIndex
 * 		The index within args at which the params argument is expected to appear
 * 		when a params argument is present
 * @return {Array}
 * 		An array with two members: 
 * 		the first is an array of the params, which may be empty;
 * 		the second is an options object, which may be empty
 */
DB.getParamsFromArgs = function getParamsFromArgs(args, paramsIndex)
{
	var params, opts;
	
	var numArgs = args.length - paramsIndex;
	if (numArgs > 0) 
	{
		// First, if the last parameter is a non-Array object, it'll always be interpreted as the options
		var lastParam = args[args.length - 1];
		var lastParamIsArray, lastParamIsDate, firstParamIsArray; // it might be that the first param is the last param
		if (lastParam && (typeof lastParam == "object"))
		{
			lastParamIsArray = Util.isArray(lastParam);
			lastParamIsDate = Util.isDate(lastParam);
			if ((args.length - 1) == paramsIndex)
			{
				firstParamIsArray = lastParamIsArray;
			}
			if (!lastParamIsArray && !lastParamIsDate)
			{
				opts = lastParam;
				numArgs--;
			}
		}
	}
	
	// If there are still args, let's see if we're given an array vs. we need to turn them into an array
	if (numArgs > 0)
	{
		var firstParam = args[paramsIndex];
		if (firstParamIsArray == undefined) // we have not already calculated it
		{
			firstParamIsArray = Util.isArray(firstParam);
		}
		if (firstParamIsArray)
		{
			params = firstParam;
		}
		else
		{
			params = [];
			for (var iArg=paramsIndex; iArg<paramsIndex+numArgs; iArg++)
			{
				params.push(args[iArg]);
			}
		}
	}
	else
	{
		params = [];
	}
	
	opts = opts || {};
	
	return [params, opts];
}

frameworkGlobal.DB = Jaxer.DB = DB;

Log.trace("*** DB.js loaded");  // Only if Log itself is defined at this point of the includes

})();
