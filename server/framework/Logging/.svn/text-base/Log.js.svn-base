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
	
/**
 * @namespace {Jaxer.Log}
 * 
 * Log is a static object meant to be shared across the framework and perhaps
 * even the user's code. In a module, use it to create a module-specific logger
 * and then log with it.
 * 
 * @example
 * To log without a module name, use Jaxer.Log.info("my message"), where instead
 * of info you can use any of the six logging levels: trace(...), debug(...),
 * info(...), warn(...), error(...), and fatal(...). The generic logger is set
 * by default to only log messages at or above the info level.
 * 
 * @example
 * To log with a module name, first define a log helper: var log =
 * Jaxer.Log.forModule("myModule"); This will get or create the module logger
 * with this name. Then use log.info(...) or any of the other methods.
 * 
 * The default level of messages to log is set in configLog.js (default and
 * local) and can also be changed in memory:
 * 
 * @see Jaxer.Log.setLevel
 * @see Jaxer.Log.setAllModuleLevels
 */
var Log = 
{
	minLevelForStackTrace: null,
	genericLogger: null
};

/*
 * A hashtable of moduleNames -> moduleLoggers
 */
var modules = {};
var levelNames = ["TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"];
var defaultModuleLevel = null;
var appenders = {};

/**
 * Return a array of implemented Logger levelnames e.g. "TRACE", "DEBUG",
 * "INFO", "WARN", "ERROR", "FATAL"
 * 
 * @advanced
 * @alias Jaxer.Log.getLevelNames
 * @return {String[]}
 */
Log.getLevelNames = function getLevelNames()
{
	return levelNames;
};

/**
 * Lazily creates a logger for the given module name, if needed,
 * and in any case returns it. The referenced Log object knows to create log
 * entries marked as belonging to the specific module.
 * 
 * @alias Jaxer.Log.forModule
 * @param {String} moduleName 
 * 		The name of the module (just an identifier string).
 * @return {Jaxer.Log.ModuleLogger}
 * 		The module-specific logger to use for log entries.
 */
Log.forModule = function forModule(moduleName)
{
	if (!modules[moduleName])
	{
		modules[moduleName] = new Log.ModuleLogger(moduleName, defaultModuleLevel);
	}
	return modules[moduleName];
};

/**
 * Returns a boolean to indicate whether the referenced Log object is wired to
 * do logging for the provided modulename
 * 
 * @alias Jaxer.Log.hasModule
 * @param {String} moduleName
 * 		The name of the module to look up
 * @return {Boolean}
 */
Log.hasModule = function hasModule(moduleName)
{
	return moduleName in modules;
};

/**
 * Gets a sorted array of all the currently-defined modules.
 * 
 * @advanced
 * @alias Jaxer.Log.getSortedModuleNames
 * @return {Array}
 * 		An array of strings, sorted alphabetically, of the names of modules for
 * 		which ModuleLoggers have been defined.
 */
Log.getSortedModuleNames = function getSortedModuleNames()
{
	var names = [];
	for (var name in modules)
	{
		names.push(name);
	}
	return names.sort();
};

/**
 * Set the logging level on ALL modules to level provided for the referenced Log
 * object
 * 
 * @alias Jaxer.Log.setAllModuleLevels
 * @param {Jaxer.Log.Level} level
 * 		The new level to use for all of them
 */
Log.setAllModuleLevels = function setAllModuleLevels(level)
{
	defaultModuleLevel = level;
	for (var name in modules)
	{
		modules[name].setLevel(level);
	}
};

/**
 * Add an appender to the referenced Log object
 * 
 * @advanced
 * @alias Jaxer.Log.addAppender
 * @param {String} name
 * 		The name of the appender
 * @param {Jaxer.Log.Appender} appender
 * 		An instance of the appender. It should be derived from
 * 		Jaxer.Log.Appender.
 */
Log.addAppender = function addAppender(name, appender)
{
	appenders[name] = appender;
};

/**
 * Remove the specified appender from the referenced Log object
 * 
 * @advanced
 * @alias Jaxer.Log.removeAppender
 * @param {String} name
 * 		The name of the appender to use.
 */
Log.removeAppender = function removeAppender(name)
{
	delete appenders[name];
};

/**
 * Get an appender reference from the referenced Log object
 * 
 * @advanced
 * @alias Jaxer.Log.getAppender
 * @param {String} name
 * 		The name of the appender to look for
 * @return {Jaxer.Log.Appender}
 * 		The appender instance
 */
Log.getAppender = function getAppender(name)
{
	return appenders[name];
};

/**
 * The internal logging method which logs to all the current appenders
 * 
 * @private
 * @alias Jaxer.Log._log
 * @param {String} moduleName
 * 		The associated module
 * @param {Jaxer.Log.Level} level
 * 		The level of this message
 * @param {String} message
 * 		The message to log
 * @param {Object} [exception]
 * 		An optional exception to use
 */
Log._log = function _log(moduleName, level, message, exception)
{
	for (var appenderName in appenders)
	{
		var appender = appenders[appenderName];
		if (level.isAtLeast(appender.getLevel()))
		{
			appender.append(moduleName, level, message, exception);
		}
	}
};

/**
 * Initialize the logging system
 * 
 * @private
 * @alias Jaxer.Log.init
 * @param {String} defModuleLevelName
 * 		The default name to use when there's no module name specified
 */
Log.init = function init(defModuleLevelName)
{
	for (var i=0; i<levelNames.length; i++)
	{
		var levelName = levelNames[i];
		Log[levelName] = new Log.Level(levelName, i * 1000);
	}
	defaultModuleLevel = Log[defModuleLevelName];

	Log.genericLogger = Log.forModule("framework");

	for (var i=0; i<levelNames.length; i++)
	{
		var methodName = levelNames[i].toLowerCase();
		eval('Log[methodName] = function() { Log.genericLogger["' + methodName + '"].apply(Log.genericLogger, arguments); }');
	}

	Log.minLevelForStackTrace = Log.ERROR;
};

/**
 * Logs a message at the "TRACE" level. The message will only be appended to the
 * log if the level for that module (and that appender) is set at
 * Jaxer.Log.TRACE. This is the most verbose level - instrument your code
 * liberally with trace() calls to be able to pinpoint any issues.
 * 
 * @alias Jaxer.Log.trace
 * @param {String} message
 * 		The message to append to the log. The timestamp, modulename, and
 * 		terminating newline will be added automatically.
 * @param {Error} [exception]
 * 		An optional error or exception to be logged with this message
 * @param {Function} [inFunction]
 * 		An optional indication of which function this message should appear to
 * 		originate from. By default, it's the function that called this logging
 * 		method.
 */

/**
 * Logs a message at the "DEBUG" level. The message will only be appended to the
 * log if the level for that module (and that appender) is set at or below
 * Jaxer.Log.DEBUG. This is the second-most verbose level - instrument your code
 * with debug() calls in those places where debugging is likely to benefit from
 * them.
 * 
 * @alias Jaxer.Log.debug
 * @param {String} message
 * 		The message to append to the log. The timestamp, modulename, and
 * 		terminating newline will be added automatically.
 * @param {Error} [exception]
 * 		An optional error or exception to be logged with this message
 * @param {Function} [inFunction]
 * 		An optional indication of which function this message should appear to
 * 		originate from. By default, it's the function that called this logging
 * 		method.
 */

/**
 * Logs a message at the "INFO" level. The message will only be appended to the
 * log if the level for that module (and that appender) is set at or below
 * Jaxer.Log.INFO. By default, modules are set to show messages at this level,
 * so use info() when you want to show log messages without needing to set the
 * level to more verbose than usual, but don't keep info() messages in your code
 * long term.
 * 
 * @alias Jaxer.Log.info
 * @param {String} message
 * 		The message to append to the log. The timestamp, modulename, and
 * 		terminating newline will be added automatically.
 * @param {Error} [exception]
 * 		An optional error or exception to be logged with this message
 * @param {Function} [inFunction]
 * 		An optional indication of which function this message should appear to
 * 		originate from. By default, it's the function that called this logging
 * 		method.
 */

/**
 * Logs a message at the "WARN" level. The message will only be appended to the
 * log if the level for that module (and that appender) is set at or below
 * Jaxer.Log.WARN. Use this to warn of any unusual or unexpected, but not
 * necessarily erroneous, conditions.
 * 
 * @alias Jaxer.Log.warn
 * @param {String} message
 * 		The message to append to the log. The timestamp, modulename, and
 * 		terminating newline will be added automatically.
 * @param {Error} [exception]
 * 		An optional error or exception to be logged with this message
 * @param {Function} [inFunction]
 * 		An optional indication of which function this message should appear to
 * 		originate from. By default, it's the function that called this logging
 * 		method.
 */

/**
 * Logs a message at the "ERROR" level. The message will only be appended to the
 * log if the level for that module (and that appender) is set at or below
 * Jaxer.Log.ERROR. Use this to log non-fatal but nonetheless real errors.
 * 
 * @alias Jaxer.Log.error
 * @param {String} message
 * 		The message to append to the log. The timestamp, modulename, and
 * 		terminating newline will be added automatically.
 * @param {Error} [exception]
 * 		An optional error or exception to be logged with this message
 * @param {Function} [inFunction]
 * 		An optional indication of which function this message should appear to
 * 		originate from.	By default, it's the function that called this logging
 * 		method.
 */

/**
 * Logs a message at the "FATAL" level. The message will only be appended to the
 * log if the level for that module (and that appender) is set at or below
 * Jaxer.Log.FATAL. Use this to log the most serious errors.
 * 
 * @alias Jaxer.Log.fatal
 * @param {String} message
 * 		The message to append to the log. The timestamp, modulename, and
 * 		terminating newline will be added automatically.
 * @param {Error} [exception]
 * 		An optional error or exception to be logged with this message
 * @param {Function} [inFunction]
 * 		An optional indication of which function this message should appear to
 * 		originate from. By default, it's the function that called this logging
 * 		method.
 */

/**
 * Set the logging level for the generic logger (the one that's not module-
 * specific)
 * 
 * @alias Jaxer.Log.setLevel
 * @param {Jaxer.Log.Level} level
 * 		The level to use on the generic logger messages below this level will
 * 		not be logged.
 */
Log.setLevel = function(level)
{
	Log.genericLogger.setLevel(level);
};

/**
 * Get the logging level of the generic logger
 * 
 * @alias Jaxer.Log.getLevel
 * @return {Jaxer.Log.Level}
 * 		The level below which non-module-specific messages will not be logged
 */
Log.getLevel = function getLevel()
{
	return Log.genericLogger.getLevel();
};

/**
 * Get the current JavaScript stack trace.
 * 
 * @alias Jaxer.Log.getStackTrace
 * @param {String} [linePrefix]
 * 		An optional prefix (e.g. whitespace for indentation) to prepend to every
 * 		line of the stack trace
 * @param {Number} [framesToSkip]
 * 		An optional number of frames to skip before starting to trace the
 * 		remaining frames
 * @return {String}
 * 		The stack trace as a string consisting of a number of lines, starting at
 * 		the deepest frame
 */
Log.getStackTrace = function getStackTrace(linePrefix, framesToSkip)
{
	if (typeof linePrefix != "string") linePrefix = '';
	if (!Util.Math.isInteger(framesToSkip) || framesToSkip < 0) framesToSkip = 0;
	var stack = Components.stack.caller;
	if (framesToSkip) 
	{
		var leftToSkip = framesToSkip;
		while (leftToSkip > 0 && stack) 
		{
			stack = stack.caller;
			leftToSkip--;
		}
	}
	var iterations = 0;
	var stackTrace = [];
	var MAX_ITERATIONS = 50;
	while (stack && (iterations < MAX_ITERATIONS))
	{
		var filename = stack.filename.replace(/^.*\//, '');
		var name = (stack.name == null) ? '<no name>' : stack.name;
		stackTrace.push(linePrefix + name + " [line " + stack.lineNumber + " of " + filename + " (" + stack.filename + ")]");
		stack = stack.caller;
		iterations++;
	}
	if (iterations >= MAX_ITERATIONS)
	{
		stackTrace.push(linePrefix + "<TRUNCATED>");
	}
	return stackTrace.join("\n");
}

/**
 * Get the current JavaScript stack trace.
 * 
 * @alias Jaxer.Log.getStackTraceAlt
 * @param {String} [linePrefix]
 * 		An optional prefix (e.g. whitespace for indentation) to prepend to every
 * 		line of the stack trace
 * @param {Number} [framesToSkip]
 * 		An optional number of frames to skip before starting to trace the
 * 		remaining frames
 * @return {String}
 * 		The stack trace as a string consisting of a number of lines, starting at
 * 		the deepest frame
 */
Log.getStackTraceAlt = function getStackTraceAlt(linePrefix, framesToSkip)
{
	if (typeof linePrefix != "string") linePrefix = '';
	
	if (!Util.Math.isInteger(framesToSkip) || framesToSkip < 0) framesToSkip = 0;
	
	var stack = Components.stack.caller;
	if (framesToSkip) 
	{
		var leftToSkip = framesToSkip;
		while (leftToSkip > 0 && stack) 
		{
			stack = stack.caller;
			leftToSkip--;
		}
	}
	var iterations = 0;
	var stackTrace = ["\nstack:"];
	var filePaths = {};
	
	var MAX_ITERATIONS = 50;
	while (stack && (iterations < MAX_ITERATIONS))
	{
		var filename = stack.filename.replace(/^.*\//, '');
		var name = (stack.name == null) ? '<no name>' : stack.name;
		
		stackTrace.push(linePrefix + " [" + filename +":" + stack.lineNumber + "] "+ name );
		filePaths[filename] = stack.filename.replace(/.*->\s/, '');
		stack = stack.caller;
		iterations++;
	}
	
	stackTrace.push(["paths:"]);
	for each( p in filePaths)
	{
		stackTrace.push(linePrefix + p );
	}
	
	if (iterations >= MAX_ITERATIONS)
	{
		stackTrace.push(linePrefix + "<TRUNCATED>");
	}
	return stackTrace.join("\n");
}



frameworkGlobal.Log = Jaxer.Log = Log;

})();
