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
 * @classDescription {Jaxer.Log.Appender} Base class of all Appenders: listeners
 * that know how to append log messages somewhere, e.g. to a file or a database.
 */

/**
 * This is the base class of all Appenders: listeners that know how to append
 * log messages somewhere, e.g. to a file or a database.
 * 
 * @advanced
 * @constructor
 * @alias Jaxer.Log.Appender
 * @param {String} name
 * 		The name to use to identify this appender
 * @param {Jaxer.Log.Level} level
 * 		The level to start this appender at -- messages below this will not be
 * 		appended
 * @return {Jaxer.Log.Appender}
 * 		Returns an instance of Appender.
 */
function Appender(name, level)
{
	this.level = level;
}

/**
 * Append a message associated with the given module to the log
 * 
 * @private
 * @alias Jaxer.Log.Appender.append
 * @param {String} moduleName
 * 		The name of the module to use
 * @param {Jaxer.Log.Level} level
 * 		The level to use for this message
 * @param {String} message
 * 		The message to log
 * @param {Object} [exception]
 * 		An optional exception object to use
 */
Appender.prototype.append = function append(moduleName, level, message, exception)
{
};

/**
 * The level to which this appender is set. Messages below this level will not
 * be logged.
 * 
 * @advanced
 * @alias Jaxer.Log.Appender.prototype.getLevel
 * @return {Jaxer.Log.Level}
 * 		The current level
 */
Appender.prototype.getLevel = function getLevel()
{
	return this.level;
};

/**
 * Sets the level below which this appender will not log messages.
 * 
 * @advanced
 * @alias Jaxer.Log.Appender.prototype.setLevel
 * @param {Jaxer.Log.Level} level
 *		The minimum loggable level. Should be one of Log.TRACE, Log.DEBUG,
 *		Log.INFO, Log.WARN, Log.ERROR, Log.FATAL.
 */
Appender.prototype.setLevel = function setLevel(level)
{
	this.level = level;
};

/**
 * Identifies this appender instance by name
 * 
 * @advanced
 * @alias Jaxer.Log.Appender.prototype.toString
 * @return {String}
 * 		The appender's name
 */
Appender.prototype.toString = function()
{
	return "[" + this.name + "]";
};

Log.Appender = Appender;

})();
