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

var log = Log.forModule("SessionManager"); // Only if Log itself is defined at this point of the includes

/**
 * @namespace {Jaxer.SessionManager} A namespace object holding functions and
 * members used to manage a user session across multiple requests.
 */
var SessionManager = 
{
	COOKIE_PREFIX: "JaxerSessionId$$"
};

/**
 * Get session key from the client via cookie, or create one as needed
 * 
 * @advanced
 * @alias Jaxer.SessionManager.keyFromRequest
 * @param {String} appKey
 * 		The key that uniquely identifies the current application
 * @return {String}
 * 		A session key that can be used to track the current session (new or
 * 		existing)
 */
SessionManager.keyFromRequest = function keyFromRequest(appKey)
{
	var cookieName = SessionManager.COOKIE_PREFIX + appKey;
	var key = Util.Cookie.get(cookieName);
	log.debug("Getting session key from the client via cookie " + cookieName + ": " + key);
	if (key == null)
	{
		key = "" + (new Date().getTime()) + Math.random().toString().replace(/[^\d]/g, "");
		log.debug("Creating a new session key: " + key);
	}
	return key;
};


/**
 * Set session key on the response to the client via a cookie
 * 
 * @advanced
 * @alias Jaxer.SessionManager.keyToResponse
 * @param {String} appKey
 * 		The key that uniquely identifies the current application
 * @param {String} key
 * 		The session key to use
 */
SessionManager.keyToResponse = function keyToResponse(appKey, key)
{
	var cookieName = SessionManager.COOKIE_PREFIX + appKey;
	log.debug("Saving session key to the client via cookie " + cookieName + ": " + key);
	Util.Cookie.set(SessionManager.COOKIE_PREFIX + appKey, key);
}

frameworkGlobal.SessionManager = Jaxer.SessionManager = SessionManager;

Log.trace("*** SessionManager.js loaded");

})();
