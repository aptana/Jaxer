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

/*
 * The functions below are used in both the client and the server Jaxer frameworks
 */

(function() {

if (!Jaxer.Util) Jaxer.Util = {};

/**
 * @namespace {Jaxer.Util.Cookie} Namespace object holding functions and members
 * for working with client (browser) cookies on the server side.
 */
Jaxer.Util.Cookie = {};

/**
 * Set a cookie name/value pair
 * 
 * @alias Jaxer.Util.Cookie.set
 * @param {String} name
 * 		The name of the cookie to set
 * @param {String} value
 * 		The value of the cookie
 */
Jaxer.Util.Cookie.set = function set(name, value)
{
	var cookieString = encodeURIComponent(name) + "=" + encodeURIComponent(value) + "; path=/";
	if (Jaxer.isOnServer) 
	{
		Jaxer.response.addHeader("Set-Cookie", cookieString, false);
	}
	else
	{
		document.cookie = cookieString;
	}
};

/**
 * Get a cookie key value
 * 
 * @alias Jaxer.Util.Cookie.get
 * @param {String} name
 * 		The name of the cookie to retrieve
 * @return {String}
 * 		Returns the value of the specified cookie or null if the value does not
 * 		exist.
 */
Jaxer.Util.Cookie.get = function get(name)
{
	var value = null;
	var cookies = Jaxer.Util.Cookie.getAll();
	
	if (typeof cookies[name] != "undefined")
	{
		value = cookies[name];
	}
	
	return value;
};

// specials, if given, is a hashmap that gives, for each special index, the property names to
// use for that indexed element's LHS and RHS
function parseHeaderString(str, specials)
{
	var data = {};
	var fragments = str.split(/\s*;\s*/);
	for (var i=0; i<fragments.length; i++)
	{
		var fragment = fragments[i];
		var matched = /^([^\=]+?)\s*\=\s*(.*?)$/.exec(fragment);
		if (matched && matched.length == 3) 
		{
			var lhs, rhs;
			try
			{
				lhs = decodeURIComponent(matched[1]);
			}
			catch (e)
			{
				lhs = matched[1];
				if (Jaxer.isOnServer) Jaxer.Log.debug("Malformed cookie name: name = " + lhs);
			}
			try
			{
				rhs = decodeURIComponent(matched[2]);
			}
			catch (e)
			{
				rhs = matched[2];
				if (Jaxer.isOnServer) Jaxer.Log.debug("Malformed cookie value: name = " + lhs + ", value = " + rhs);
			}
			if (specials && specials[i])
			{
				data[specials[i][0]] = lhs;
				data[specials[i][1]] = rhs;
			}
			else
			{
				data[lhs] = rhs;
			}
		}
	};
	return data;
};

/**
 * Get an object containing all cookie keys and values from the current request. 
 * Each cookie name will become a property on the object and each cookie value 
 * will become that property's value.
 * 
 * @alias Jaxer.Util.Cookie.getAll
 * @return {Object}
 * 		The resulting object of all cookie key/value pairs
 */
Jaxer.Util.Cookie.getAll = function getAll()
{
	var cookieString = Jaxer.isOnServer ? Jaxer.request.headers.Cookie : document.cookie;
	return (typeof cookieString == "string") ?
		parseHeaderString(cookieString) :
		{};
};

/**
 * Parses a given array of an HTTP response's "Set-Cookie" header strings to extract
 * the cookie fields
 * 
 * @alias Jaxer.Util.Cookie.parseSetCookieHeaders
 * @param {Array} setCookieStrings
 * 		An array of the (string) values of the HTTP response's Set-Cookie headers
 * @return {Array}
 * 		An array of objects, one per Set-Cookie header, with properties corresponding to the 
 * 		name, value, expires, path, and domain values in the header
 */
Jaxer.Util.Cookie.parseSetCookieHeaders = function parseSetCookieHeaders(setCookieStrings)
{
	if (typeof setCookieStrings == "string") setCookieStrings = [setCookieStrings];
	var cookies = [];
	for (var i=0; i<setCookieStrings.length; i++)
	{
		cookies.push(parseHeaderString(setCookieStrings[i], {0: ['name', 'value']}));
	};
	return cookies;
};

})();
