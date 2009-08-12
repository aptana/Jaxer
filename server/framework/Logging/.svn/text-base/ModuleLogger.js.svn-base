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

/**
 * @classDescription {Jaxer.Log.ModuleLogger} Object created by the 
 * global Log.forModule("...") method for module-specific logging.
 */

/**
 * An object that's created only through the global Log.forModule("...") method
 * for module-specific logging. Usually you create such a logger at the
 * beginning of your module, and then use it throughout your module for logging
 * at various levels. It has its own level, so you can control the verbosity of
 * logging per module.
 * 
 * @constructor
 * @alias Jaxer.Log.ModuleLogger
 * @param {String} moduleName
 * 		The name to use for the module
 * @param {Jaxer.Log.Level}
 * 		The log level for the module
 * @return {Jaxer.Log.ModuleLogger}
 * 		Returns an instance of ModuleLogger.
 */
function ModuleLogger(name, level)
{
	this.moduleName = name;
	this.level = level;
}

/**
 * The internal logging function for the module logger, meant to be called from
 * the level-specific methods. It ASSUMES that its caller's caller is the
 * function that's really doing the logging, and uses its name in the log (as
 * well as the module's name).
 * 
 * @private
 * @alias Jaxer.Log.ModuleLogger.prototype._log
 * @param {Jaxer.Log.Level} level
 * 		The level at which to log the message
 * @param {String} message
 * 		The message (the module and caller will be prepended automatically)
 * @param {Jaxer.Exception} [exception]
 * 		Optional: an exception to log
 */
ModuleLogger.prototype._log = function _log(level, message, exception, inFunction)
{
	if (level.isBelow(this.level))
	{
		return;
	}
	if (!inFunction) inFunction = this._log.caller.caller;
	var calledFrom = (typeof inFunction == "function") ? inFunction.name : "";
	message = "[" + this.moduleName + "." + calledFrom + "] " + message;
	if ((typeof inFunction == "function") && level.isAtLeast(Log.minLevelForStackTrace))
	{
		message += Log.getStackTraceAlt("  -> ", 1)+"\n";
	}
	Log._log(this.moduleName, level, message, exception);
};

// Create methods for the various levels
var levelNames = Log.getLevelNames();

for (var i = 0; i < levelNames.length; i++)
{
	var levelName = levelNames[i];
	var methodName = levelName.toLowerCase();
	eval('ModuleLogger.prototype["' + methodName + '"] = function ' + methodName + '(message, exception, inFunction) { this._log(Log.' + levelName + ', message, exception, inFunction); }');
}

/**
 * Logs a message at the "TRACE" level for this module. The message will only be
 * appended to the log if the level for this module (and that appender) is set
 * at Jaxer.Log.TRACE. This is the most verbose level - instrument your code
 * liberally with trace() calls to be able to pinpoint any issues.
 * 
 * @alias Jaxer.Log.ModuleLogger.prototype.trace
 * @param {String} message
 * 		The message to append to the log. The timestamp, modulename, and
 * 		terminating newline will be added automatically.
 * @param {Error} [exception]
 * 		An optional error or exception to be logged with this message
 * @param {Function} [inFunction]
 *		An optional indication of which function this message should appear to
 		originate from. By default, it's the function that called this logging
 		method.
 */

/**
 * Logs a message at the "DEBUG" level for this module. The message will only be
 * appended to the log if the level for this module (and that appender) is set
 * at or below Jaxer.Log.DEBUG. This is the second-most verbose level -
 * instrument your code with debug() calls in those places where debugging is
 * likely to benefit from them.
 * 
 * @alias Jaxer.Log.ModuleLogger.prototype.debug
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
 * Logs a message at the "INFO" level for this module. The message will only be
 * appended to the log if the level for this module (and that appender) is set
 * at or below Jaxer.Log.INFO. By default, modules are set to show messages at
 * this level, so use info() when you want to show log messages without needing
 * to set the level to more verbose than usual, but don't keep info() messages
 * in your code long term.
 * 
 * @alias Jaxer.Log.ModuleLogger.prototype.info
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
 * Logs a message at the "WARN" level for this module. The message will only be
 * appended to the log if the level for this module (and that appender) is set 
 * at or below Jaxer.Log.WARN. Use this to warn of any unusual or unexpected,
 * but not necessarily erroneous, conditions.
 * 
 * @alias Jaxer.Log.ModuleLogger.prototype.warn
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
 * Logs a message at the "ERROR" level for this module. The message will only be
 * appended to the log if the level for this module (and that appender) is set
 * at or below Jaxer.Log.ERROR. Use this to log non-fatal but nonetheless real
 * errors.
 * 
 * @alias Jaxer.Log.ModuleLogger.prototype.error
 * @param {String} message
 * 		Tthe message to append to the log. The timestamp, modulename, and
 * 		terminating newline will be added automatically.
 * @param {Error} [exception]
 * 		An optional error or exception to be logged with this message
 * @param {Function} [inFunction]
 * 		An optional indication of which function this message should appear to
 * 		originate from. By default, it's the function that called this logging
 * 		method.
 */

/**
 * Logs a message at the "FATAL" level for this module. The message will only be
 * appended to the log if the level for this module (and that appender) is set
 * at or below Jaxer.Log.FATAL. Use this to log the most serious errors.
 * 
 * @alias Jaxer.Log.ModuleLogger.prototype.fatal
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
 * Gets the level to which this ModuleLogger is set -- appending messages below
 * this level will do nothing
 * 
 * @alias Jaxer.Log.ModuleLogger.prototype.getLevel
 * @return {Jaxer.Log.Level}
 * 		The current level
 */
ModuleLogger.prototype.getLevel = function getLevel()
{
	return this.level;
};

/**
 * Sets the level below which this moduleLogger will not log messages.
 * 
 * @alias Jaxer.Log.ModuleLogger.prototype.setLevel
 * @param {Jaxer.Log.Level} level
 * 		The minimum loggable level. Should be one of Jaxer.Log.TRACE, Jaxer.Log.DEBUG,
 * 		Jaxer.Log.INFO, Jaxer.Log.WARN, Jaxer.Log.ERROR, Jaxer.Log.FATAL.
 */
ModuleLogger.prototype.setLevel = function setLevel(level)
{
	this.level = level;
};

Log.ModuleLogger = ModuleLogger;

})();
