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
 * @namespace {Jaxer.Util.Url} Namespace that holds functions and other objects
 * for working with URLs.
 */
Util.Url = {};

/**
 * @classDescription {Jaxer.Util.Url.ParsedUrl} An object describing the parsed
 * pieces of a URL.
 */

function isIpAddress(host)
{
	return /^\d+\.\d+\.\d+\.\d+/.test(host);
}

/**
 * An object describing the parsed pieces of a URL. 
 * This object contains sub properties to allow access to the individual pieces
 * of the URL
 * 
 * @alias Jaxer.Util.Url.ParsedUrl
 * @constructor
 * @return {Jaxer.Util.Url.ParsedUrl}
 * 		Returns an instance of ParsedUrl.
 */
Util.Url.ParsedUrl = function ParsedUrl(url)
{
	
	url = Util.String.trim(url);
	
	// We first use the RegExp from RFC 2396 to parse the basic components
	var pattern = /^(([^\:\/\?#]+)\:)?(\/\/([^\/\?#]*))?([^\?#]*)(\?([^#]*))?(#(.*))?$/;
	//              12                3    4            5        6  7        8 9
	//               scheme                authority    path        query      fragment
	var isMatch = url.match(pattern);
	
	/**
	 * The complete (but whitespace-trimmed) original URL, before parsing
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.url
	 * @property {String}
	 */
	this.url = url;

	/**
	 * The protocol (typically 'http' or 'https'), or '' if not specified.
	 * A synonym for the scheme property.
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.protocol
	 * @property {String}
	 */
	/**
	 * The scheme (typically 'http' or 'https'), or '' if not specified.
	 * A synonym for the protocol property.
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.scheme
	 * @property {String}
	 */
	this.protocol = this.scheme = isMatch ? RegExp.$2 : "";

	/**
	 * The complete authority (e.g. 'user1@www.aptana.com:8081'), or '' if not specified.
	 * Note that the userInfo ('user1') and/or the port ('8081') may not be present.
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.authority
	 * @property {String}
	 */
	this.authority = isMatch ? RegExp.$4 : "";

	/**
	 * The entire path (e.g. '/images/myImage.gif') between the hostAndPort and the query string or fragment (if any), or ''
	 * if none
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.path
	 * @property {String}
	 */
	this.path = isMatch ? Util.String.trim(RegExp.$5) : "";
	
	/**
	 * The query string (e.g. 'name=joe&id=12') after the (first) question mark
	 * (?), or '' if not specified
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.query
	 * @property {String}
	 */
	this.query = (isMatch && RegExp.$7) ? RegExp.$7 : "";

	/**
	 * The fragment string (e.g. 'myBookmark') after the # symbol (e.g.
	 * #myBookmark), or '' if not specified
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.fragment
	 * @property {String}
	 */
	this.fragment = (isMatch && RegExp.$9) ? RegExp.$9 : "";

	// Then we further disassemble these into useful bits:
	
	pattern = /^(([^@]+)@)?([^\:]+)(:(.+))?$/;
	//          12         3       4 5
	isMatch = this.authority.match(pattern);
	
	/**
	 * The userInfo in the URL (e.g. 'user1' in 'user1@www.aptana.com'), or '' if not specified.
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.userInfo
	 * @property {String}
	 */
	this.userInfo = isMatch ? RegExp.$2 : "";
	
	/**
	 * The host (e.g. 'www.aptana.com'), or '' if not specified.
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.host
	 * @property {String}
	 */
	this.host = isMatch ? RegExp.$3 : "";
	
	/**
	 * The port as a string (e.g. '80'), or '' if not specified
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.port
	 * @property {String}
	 */
	this.port = isMatch ? RegExp.$5 : "";
	
	// Further combinations and slices:

	/**
	 * The combined host and port (e.g. 'www.aptana.com:8081'), which might be
	 * just the host if no port was specified.
	 * A synonym for the authority property.
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.hostAndPort
	 * @property {String}
	 */
	this.hostAndPort = this.host + preDress(this.port, ":");

	/**
	 * The protocol, userInfo, host, and port (e.g. 'http://user1@www.aptana.com:8081'), which
	 * might not have a protocol or a port if they were not specified
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.base
	 * @property {String}
	 */
	this.base = postDress(this.protocol, ":") + '//' + this.authority;
	
	var domainParts = isIpAddress(this.host) ? null : this.host.split(".");

	/**
	 * The subdomain (e.g. 'www.playground' in 'www.playground.aptana.com')
	 * before the domain, or '' if none or if the host was an IP address
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.subdomain
	 * @property {String}
	 */
	this.subdomain = domainParts ? domainParts.slice(0, -2).join(".") : '';

	/**
	 * The highest-level non-TLD domain (e.g. 'aptana.com' in
	 * 'www.playground.aptana.com'), or the IP address if the host was an IP address
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.domain
	 * @property {String}
	 */
	this.domain = domainParts ? domainParts.slice(-2).join(".") : this.host;

	/**
	 * A possibly-empty array of strings that compose the path part of the URL 
	 * (e.g. ["images", "small", "logo.png"] in 'http://www.aptana.com/images/small/logo.png?v=12').
	 * Note that if the path ends in a /, the last element in the pathParts array 
	 * (a.k.a. the leaf) will be the empty string.
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.pathParts
	 * @property {Array}
	 */
	var leftTrimmedPath = Util.String.trim(this.path, '/', 'l');
	this.pathParts = (leftTrimmedPath == '') ? [] : leftTrimmedPath.split("/");
	
	/**
	 * The last part of the path, namely everything after the last / and before any query or fragment
	 * (e.g. "logo.png" in 'http://www.aptana.com/images/small/logo.png?v=12').
	 * If the path ends in a trailing /, this is the empty string.
	 * And if the path is completely absent, this is null.
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.leaf
	 * @property {String}
	 */
	this.leaf = (this.pathParts.length > 0) ? this.pathParts[this.pathParts.length - 1] : null;

	/**
	 * An object containing a property for each name=value pair in the query
	 * string of the URL, e.g. 'http://www.aptana.com/staff?name=joe&id=12'
	 * leads to queryParts.name = "joe" and queryParts.id = "12"
	 * 
	 * @alias Jaxer.Util.Url.ParsedUrl.prototype.queryParts
	 * @property {Object}
	 */	
	this.queryParts = Util.Url.queryToHash(this.query);

}

function preDress(str, separator)
{
	return (str == '') ? '' : (separator + str);
}

function postDress(str, separator)
{
	return (str == '') ? '' : (str + separator);
}

/**
 * Parse a string containing a URL into a Jaxer.Util.Url.ParsedUrl object
 * to allow manipulation of the individual URL component parts
 * 
 * @alias Jaxer.Util.Url.parseUrl
 * @param {String} url
 * 		The URL to parse
 * @return {Jaxer.Util.Url.ParsedUrl}
 * 		The parts of a URL broken down into useful pieces
 */
Util.Url.parseUrl = function parseUrl(url)
{
	return new Util.Url.ParsedUrl(url);
};

/**
 * Create a Util.Url.ParsedUrl object from the component pieces provided
 * as parameters to the functions calls.
 * 
 * @alias Jaxer.Util.Url.parseUrlComponents 
 * @param {String} hostAndPort
 * 		The host (and port, if any) containing the given path 
 * @param {String} absolutePath
 * 		The absolute path to a resource on the host
 * @param {String} [protocol]
 * 		The protocol ('http' or 'https'); the default is 'http'
 * @return {Jaxer.Util.Url.ParsedUrl}
 * 		The parts of a URL broken down into useful pieces
 */
Util.Url.parseUrlComponents = function parseUrlComponents(hostAndPort, absolutePath, protocol)
{
	var url = (protocol ? protocol : "http") + "://" + Util.Url.combine(hostAndPort, absolutePath); 
	return Util.Url.parseUrl(url);
};

/**
 * Divides the key/value pairs in a query string and builds an object for these
 * values. The key will become the property name of the object and the value
 * will become the value of that property
 * 
 * @alias Jaxer.Util.Url.queryToHash
 * @param {String} query
 * 		A query string
 * @return {Object}
 */
Util.Url.queryToHash = function queryToHash(query)
{
	var hash = {};
	if (typeof query == "number") query = String(query);
	if (typeof query == "string") query = Jaxer.Util.String.trim(query, " ?");
	if (query) 
	{
		var queryStrings = query.split("&");
		for (var i = 0; i < queryStrings.length; i++) 
		{
			var nameValue = queryStrings[i].split("=");
			hash[Util.Url.formUrlDecode(nameValue[0])] = (nameValue.length > 1) ? Util.Url.formUrlDecode(nameValue[1]) : null;
		}
	}
	return hash;
};

/**
 * Decode a URL by replacing +'s with spaces and all hex values (%xx) with their
 * character value
 * 
 * @alias Jaxer.Util.Url.formUrlDecode
 * @param {String} str
 * 		The source URL to decode
 * @return {String}
 * 		The resulting URL after all hex values have been  converted
 */
Util.Url.formUrlDecode = function formUrlDecode(str)
{
	if (typeof str != "string")
	{
		throw new Exception("formUrlDecode was handed something of type " + (typeof str) + " rather than a string");
	}
	str = str.replace(/\+/g, " "); // For those encoders that use + to represent a space. A true + should have been encoded as %2B
	str = decodeURIComponent(str);
	
	return str;
};

/**
 * Converts an object's properties and property values to a string suitable as
 * a query string. Each property name becomes a key in the query string and each
 * property value becomes the key value. A key and its value are separated by
 * the '=' character. Each key/value pair is separated by '&'. Note that each
 * value is encoded so invalid URL characters are encoded properly.
 * 
 * @alias Jaxer.Util.Url.hashToQuery
 * @param {Object} hash
 * 		The object to convert to a query string
 * @return {String}
 * 		The resulting query string
 */
Util.Url.hashToQuery = function hashToQuery(hash)
{
	var queryStrings = [];
	
	for (var p in hash)
	{
		var name = Util.Url.formUrlEncode(p);
		var value = (hash[p] == null) ? "" : Util.Url.formUrlEncode(hash[p]);
		
		queryStrings.push([name, value].join("="));
	}
	
	return queryStrings.join("&");
};

/**
 * Encode a URL by replacing all special characters with hex values (%xx)
 * 
 * @alias Jaxer.Util.Url.formUrlEncode
 * @param {String} str
 * 		The string to encode
 * @return {String}
 * 		The resulting URL after special characters and spaces have been encoded
 */
Util.Url.formUrlEncode = function formUrlEncode(str)
{
	return encodeURIComponent(str);
};

/**
 * Combines any number of URL fragments into a single URL, using / as the
 * separator. Before joining two fragments with the separator, it strips any
 * existing separators on the fragment ends to be joined
 * 
 * @alias Jaxer.Util.Url.combine
 * @param {String} ...
 * 		Takes any number of string URL fragments
 * @return {String}
 * 		The fragments joined into a URL
 */
Util.Url.combine = function combine()
{
	if (arguments.length == 0) return '';
	if (arguments.length == 1) return arguments[0];
	var pieces = Array.prototype.slice.call(arguments);
	pieces.forEach(function(piece, pieceIndex)
	{
		if (piece == null) pieces[pieceIndex] = '';
	});
	var sep = "/";
	var stripRight = new RegExp("\\" + sep + "+$");
	var stripLeft  = new RegExp("^\\" + sep + "+");
	var stripBoth  = new RegExp("^\\" + sep + "|\\" + sep + "$", 'g');
	pieces[0] = (pieces[0].replace(stripRight, ''));
	pieces[pieces.length - 1] = pieces[pieces.length - 1].replace(stripLeft, '');
	for (var i=1; i<pieces.length-1; i++)
	{
		pieces[i] = pieces[i].replace(stripBoth, '');
	}
	return pieces.join(sep);
}

var filePattern = /^(file|resource|chrome)\:\/{2,}/i;

/**
 * Tests whether the given URL is a reasonable file URL rather than something
 * that's available over the network.The test is pretty simplistic: the URL must
 * start with file://, resource://, or chrome://, or it must contain a backslash
 * (i.e. a Windows filesystem separator)
 * 
 * @alias Jaxer.Util.Url.isFile
 * @param {String} url
 * 		The URL to test
 * @return {Boolean}
 * 		True if a reasonable file URL, false otherwise
 */
Util.Url.isFile = function isFile(url)
{
	if (!url)
	{
		throw new Exception("No url given");
	}
	if (url.match(filePattern)) return true; 	// A recognized file protocol handler
	if (url.match(/\\/)) return true;			// A Windows path
	return false;
}

/**
 * If the given URL is already a file-type URL, it's returned untouched.
 * Otherwise we turn it into a file-type URL by prefixing it with "file://"
 * 
 * @alias Jaxer.Util.Url.ensureFileProtocol
 * @param {String} url
 * 		The URL to apply this to
 * @return {String}
 * 		URL expressed as a file type URL
 */
Util.Url.ensureFileProtocol = function ensureFileProtocol(url)
{
	if (url.match(filePattern)) return url;
	return 'file://' + url;
}

})();
