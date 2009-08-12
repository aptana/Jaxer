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
 * @classDescription {Jaxer.Log.FileAppender} File-based Log Appender.
 */

/**
 * Create an instance of the FileAppender, a file-based Appender
 * 
 * @advanced
 * @alias Jaxer.Log.FileAppender
 * @constructor
 * @param {String} name 
 * 		The appender name
 * @param {Jaxer.Log.Level} level 
 * 		The logging level to start using with this appender
 * @param {String} logPath 
 * 		The path to the logfile
 * @return {Jaxer.Log.FileAppender}
 * 		Returns an instance of FileAppender.
 */
var FileAppender = function FileAppender(name, level, logPath) 
{
	Log.Appender.call(this, name, level);
	this.logPath = logPath;
};

FileAppender.prototype = new Log.Appender("");
FileAppender.constructor = FileAppender;

/**
 * Set the  path for the filesystem logging target
 * 
 * @advanced
 * @alias Jaxer.Log.FileAppender.prototype.setPath
 * @param {String} newPath 
 * 		The new path to use for this appender. 
 */
FileAppender.prototype.setPath = function(newPath)
{
	this.logPath = newPath;
}

/**
 * Appends the provided message to the referenced appenders log
 * 
 * @advanced
 * @alias Jaxer.Log.FileAppender.prototype.append
 * @param {String} moduleName
 * 		The name of the module to use
 * @param {Jaxer.Log.Level} level
 * 		The level to use for this message
 * @param {String} message
 * 		The message to log
 * @param {Object} [exception]
 * 		An optional exception object to use
*/
FileAppender.prototype.append = function(moduleName, level, message, exception) 
{
	if (this.logPath == null || this.logPath == "") return;
	
	var formattedMessage = Util.DateTime.toPaddedString(new Date()) + " [" + level + "] [" + moduleName + "] " + message + (exception ? ('; ' + exception) : '') + "\n";

	File.append(this.logPath, formattedMessage);

};

Log.FileAppender = FileAppender;

})();

