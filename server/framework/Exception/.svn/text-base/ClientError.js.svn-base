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
 * @classDescription {Jaxer.ClientError} An error that can be thrown on the client during a callback
 */

/**
 * An error that can be thrown on the client during a callback.
 * If you want to throw a meaningful (i.e., non-generic) error on the browser
 * from a callback, throw a new Jaxer.ClientError -- just be mindful not to
 * pass sensitive server-side information. On the client, a new Error will
 * be thrown by your proxy, or you can specify you'd like something else
 * to be thrown.
 * 
 * @constructor
 * @alias Jaxer.ClientError
 * @param {String, Object} info
 * 		The descriptive text of the Error to be thrown, or an object whose
 * 		properties will be copied to the Error object to be thrown. But
 * 		if options.wrapped is specified to be false, this argument itself
 * 		is thrown on the client.
 * @param {Object} [options]
 * 		A hashmap of name-value property pairs. Currently one option is supported:
 * 		if there is a 'wrapped' property and
 * 		it is set to false, the info object itself is thrown on the client,
 * 		rather than a new Error.
 */
var ClientError = function ClientError(info, options)
{
	if (typeof info == "undefined") info = "Unspecified server error";
	
	this.info = info;
	this.options = options || {};
};

frameworkGlobal.ClientError = Jaxer.ClientError = ClientError;

Jaxer.Log.trace("*** ClientError.js loaded");

})();
