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
 * @classDescription {Jaxer.Log.CoreAppender} Appends log messages to the same
 * file as the Jaxer core.
 */

/**
 * Create an instance of the Core appender -- appends log messages to the same
 * file as the Jaxer core
 * 
 * @advanced
 * @alias Jaxer.Log.CoreAppender
 * @constructor
 * @param {String} name 
 * 		The name to use to identify this appender
 * @param {Jaxer.Log.Level} level
 * 		The level to start this appender at -- messages below this will not be
 * 		appended
 * @param {Object} coreTraceMethods
 * 		The hashtable of core trace methods that this should invoke
 * @param {Function} coreTraceBefore
 * 		The core function to call before calling any core method -- it will turn
 * 		on logging regardless of the core's settings
 * @param {Function}  coreTraceAfter
 * 		The core function to call after calling any core method -- it will
 * 		restore using the core's log settings
 * @return {Jaxer.Log.CoreAppender}
 * 		Returns an instance of CoreAppender.
 */
var CoreAppender = function CoreAppender(name, level, coreTraceMethods, coreTraceBefore, coreTraceAfter)
{
	Log.Appender.call(this, name, level);
	this.coreTraceMethods = coreTraceMethods;
	this.coreTraceBefore = coreTraceBefore;
	this.coreTraceAfter = coreTraceAfter;
};

CoreAppender.prototype = new Log.Appender("");
CoreAppender.constructor = CoreAppender;

/**
 * Append a message associated with the given module to the log
 * 
 * @private
 * @alias Jaxer.Log.CoreAppender.prototype.append
 * @param {String} moduleName
 * 		The name of the module to use
 * @param {Jaxer.Log.Level} level
 * 		The level to use for this message
 * @param {String} message
 * 		The message to log
 * @param {Object} [exception]
 * 		An optional exception object to use
 */
CoreAppender.prototype.append = function(moduleName, level, message, exception) 
{
	
	try
	{
		if (coreTraceBefore) coreTraceBefore();
		this.coreTraceMethods[level.name](message + (exception ? ('; ' + exception) : ''));
	}
	finally
	{
		if (coreTraceAfter) coreTraceAfter();
	}

};

Log.CoreAppender = CoreAppender;

})();

