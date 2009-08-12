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

var log = Log.forModule("Thread");

/**
 * @classDescription {Jaxer.Thread} 
 * 		A namespace object used to access threading-related functionality, such as async processing
 */
var Thread = {};

/**
 * The default number of milliseconds to wait before timing out waitFor.
 * Set to 3000 (3 seconds) by default).
 * 
 * @alias Jaxer.Thread.DEFAULT_MAX_MILLIS
 * @property {Number}
 */
Thread.DEFAULT_MAX_MILLIS = 3000;

const currentThread = Components.classes["@mozilla.org/thread-manager;1"].getService().currentThread;

/**
 * The currently executing thread.
 * 
 * @alias Jaxer.Thread.currentThread
 * @property {Object}
 */
Thread.__defineGetter__("currentThread", function() { return currentThread; });

/**
 * Allows asynchronous processing (e.g. async XHR requests, async Sandbox loading, etc.) to happen
 * and waits for them to complete -- completion is indicated by a test function returning true.
 * You can specify a maximum time you want to wait before giving up and continuing.
 * 
 * @alias Jaxer.Thread.waitFor
 * @param {Function} testFunction
 * 		The function to evaluate periodically to see whether we should continue to wait.
 * 		When testFunction returns true, the waitFor function exits.
 * @param {Number} [maxMillis]
 * 		The number of milliseconds to wait for testFunction to return true
 * 		before timing out and exiting. If this is zero or negative,
 * 		waits indefinitely (though other timeouts may intervene)
 * @return {Boolean} 
 * 		true if testFunction returned true, false if the timeout was reached before then
 */
Thread.waitFor = function waitFor(testFunction, maxMillis)
{
	maxMillis = Jaxer.Util.Math.forceInteger(maxMillis, Thread.DEFAULT_MAX_MILLIS);
	
	// Will use pageWindow's setTimeout and clearTimeout
	var win = Jaxer.pageWindow;
	
	// If timeoutCallback is ever called, that means we timed out
	var timedout = false;
	var timeoutCallback = function()
	{
		timedout = true;
	}
	
	log.trace("Starting to waitFor...");
	
	// Set the timeout, then start testing for it and for completion while pumping event loop
	var timeoutHandle = (maxMillis >= 0) ? win.setTimeout(timeoutCallback, maxMillis) : null;
	while (!testFunction() && !timedout) 
	{
		if (log.getLevel() == Log.TRACE) log.trace("...waitFor iteration; currentThread.hasPendingEvents = " + currentThread.hasPendingEvents());
		currentThread.processNextEvent(true);
	}
	if (timeoutHandle) win.clearTimeout(timeoutHandle);

	log.trace("Ending waitFor; timed out? " + timedout);
	
	return !timedout;
}


frameworkGlobal.Thread = Jaxer.Thread = Thread;

Log.trace("*** Thread.js loaded");

})();

