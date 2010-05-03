/* ***** BEGIN LICENSE BLOCK *****
 * Version: GPL 3
 *
 * This program is Copyright (C) 2007 Aptana, Inc. All Rights Reserved
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
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Utilities > Utilities.js
 */
coreTraceMethods.TRACE('Loading fragment: Utilities.js');
(function() {

// create top-level Util container
var Util = {};

/**
 * Remove items from an array that do not pass a given criteria. Each item in
 * the specified array will be passed to the filtering function. If that
 * function returns true, then the item will be appended to the resulting array.
 * If the function returns false, the item is not added to the resulting array.
 * 
 * Note that the specified array is not altered in place. A new array is created
 * as a result of this function call.
 * 
 * @alias Jaxer.Util.filter
 * @param {Array} array
 * 		The source array to be filtered
 * @param {Function} func
 * 		The filtering function to apply to each array item. This filter has two
 * 		parameters. The first parameter is the current item in the array that is
 *		potentially being filtered. The second parameter is the index of the
 *		item potentially being filtered. The index can be used in cases where
 *		the filtering decision needs to be determined based on proximity to
 *		other items in the array
 * @return {Array}
 * 		Returns a new array containing only items that were approved by the
 * 		filtering function.
 */
Util.filter = function(array, func)
{
	var length = array.length;
	var result = [];
	
	for (var i = 0; i < length; i++)
	{
		var item = array[i];
		
		if (func)
		{
			if (func(item, i))
			{
				result.push(item);
			}
		}
		else
		{
			if (item)
			{
				result.push(item);
			}
		}
	}
	
	return result;
};

/**
 * Remove items from an array that do not pass a given criteria. Each item in
 * the specified array will be passed to the filtering function. If that
 * function returns true, then the item will remain in the specified array. If
 * the function returns false, the item is removed from the specified array.
 * 
 * Note that the specified array is altered in place. If you prefer to create a
 * new array, leaving the original in tact, then use Util.filter instead
 * 
 * @alias Jaxer.Util.filterInPlace
 * @param {Array} array
 * 		The source array to be filtered
 * @param {Function} func
 * 		The filtering function to apply to each array item. This filter has two
 * 		parameters. The first parameter is the current item in the array that is
 *		potentially being filtered. The second parameter is the index of the
 *		item potentially being filtered. The index can be used in cases where
 *		the filtering decision needs to be determined based on proximity to
 *		other items in the array
 * @return {Array}
 * 		Returns the filtered array containing only items that were approved by
 * 		the filtering function. Note that this instance will be the same as the
 * 		instance passed into the function. This is provided as a convenience and
 * 		to keep this function signature the same as Util.filter's signature.
 */
Util.filterInPlace = function(array, func)
{
	var length = array.length;
	
	for (var i = 0; i < length; i++)
	{
		var keep = (func) ? func(item, i) : item;
		
		if (keep == false)
		{
			array.splice(i, 1);
			i--;
			length--;
		}
	}
	
	return array;
};

/**
 * Apply a function to each element in an array.
 * 
 * @alias Jaxer.Util.foreach
 * @param {Array} array
 * 		The source array
 * @param {Function} func
 * 		The function to apply to each of the items in the source array. The
 * 		function has two parameters. The first parameter is the current item in
 * 		the array that is potentially being filtered. The second parameter is
 * 		the index of the item potentially being filtered.
 */
Util.foreach = function(array, func)
{
	if (array == null) return;
	
	var length = array.length;
	
	for (var i = 0; i < length; i++)
	{
		func(array[i], i);
	}
};

/**
 * Create a new array by applying the result of a function to each of the items
 * in the array.
 * 
 * @alias Jaxer.Util.map
 * @param {Array} array
 * 		The source array
 * @param {Function} func
 * 		The function to apply to each of the items in the source array. The
 * 		function has two parameters. The first parameter is the current item in
 * 		the array that is being transformed. The second parameter is the index
 * 		of the item being transformed.
 * @return {Array}
 * 		Returns a new array where each item is the result of the specified
 * 		function as it was applied to each of the source array items.
 */
Util.map = function(array, func)
{
	if (array == null) return;
	
	var length = array.length;
	var result = new Array(length);
	
	for (var i = 0; i < length; i++)
	{
		result[i] = func(array[i], i);
	}
	
	return result;
};

/**
 * Replace each item of an array by applying a function and then replacing the
 * original item with the results of that function.
 * 
 * @alias Jaxer.Util.mapInPlace
 * @param {Array} array
 * 		The source array
 * @param {Function} func
 * 		The function to apply to each of the items in the source array. The
 * 		function has two parameters. The first parameter is the current item in
 * 		the array that is being transformed. The second parameter is the index
 * 		of the item being transformed.
 * @return {Array}
 * 		Returns the mapped array containing. Note that this instance will be the
 * 		same the instance passed into the function. This is provided as a
 * 		convenience and to keep this function signature the same as
 * 		Util.map's signature.
 */
Util.mapInPlace = function(array, func)
{
	if (array == null) return;
	
	var length = array.length;
	
	for (var i = 0; i < length; i++)
	{
		array[i] = func(array[i], i);
	}
	
	return array;
};

/**
 * Determine if the specified object contains all properties in a list of
 * property names.
 * 
 * @alias Jaxer.Util.hasProperties
 * @param {Object} object
 * 		The source object
 * @param {String[]} properties
 * 		The list of property names to test on the specified object
 * @return {Boolean}
 * 		Returns true if all properties in the list exist on the specified object
 */
Util.hasProperties = function hasProperties(object, properties)
{
	var result = false;
	
	if (object && properties && properties.constructor === Array)
	{
		result = true;
		
		for (var i = 0; i < properties.length; i++)
		{
			var property = properties[i];
			
			if (object.hasOwnProperty[property] == false)
			{
				result = false;
				break;
			}
		}
	}
	
	return result;
};

/**
 * Get a list of all (or a filtered subset) of the properties of the source object.
 * 
 * @alias Jaxer.Util.getPropertyNames
 * @param {Object} object
 * 		The source object
 * @param {Function} [filter]
 * 		An optional filter function to apply to the property's name and value. 
 * 		filter(name, value) should return something that's equivalent to true if the property is to be included.
 * @param {Boolean} [asHash]
 * 		If true, returns the result as a hash (with all values set to true)
 * @return {Array|Object}
 * 		A list or hash of the property names 
 */
Util.getPropertyNames = function getPropertyNames(object, filter, asHash)
{
	var names = asHash ? {} : [];
	
	if (object)
	{
		for (var p in object)
		{
			try 
			{
				if (!filter ||
				filter(p, object[p])) 
				{
					asHash ? (names[p] = true) : names.push(p);
				}
			} 
			catch (e) 
			{
				// do nothing -- just don't push it
			}
		}
	}
	
	return names;
};

/**
 * Tests whether the given function is native (i.e. for which there is actually no source code)
 * 
 * @alias Jaxer.Util.isNativeFunction
 * @param {Function} func
 * 		The function to test
 * @return {Boolean}
 * 		True if it's a native function, false otherwise 
 */
Util.isNativeFunction = function isNativeFunction(func)
{
	return Util.isNativeFunctionSource(func.toSource());
}

/**
 * Tests whether the given string is the source of a native function (i.e. for which there is actually no source code)
 * 
 * @alias Jaxer.Util.isNativeFunctionSource
 * @param {String} source
 * 		The source string to test
 * @return {Boolean}
 * 		True if it's a native function's source, false otherwise 
 */
Util.isNativeFunctionSource = function isNativeFunctionSource(source)
{
	return Boolean(source.match(/\)\s*\{\s*\[native code\]\s*\}\s*$/));
}

/**
 * Tests whether the given object is a Date object (even if it's from a different global context)
 * @alias Jaxer.Util.isDate
 * @param {Object} obj The object to test
 * @return {Boolean} True if it's a Date (or at least seems to be a Date), false otherwise
 */
Util.isDate = function isDate(obj)
{
	if (typeof obj != "object") return false;
	if (typeof obj.__parent__ == "undefined")
	{
		return (typeof obj.getTime == "function");
	}
	else
	{
		return (obj.constructor == obj.__parent__.Date);
	}
}

/**
 * Create a string representation of all properties in an object. A separator
 * string can be used as a delimited between each property and the user has the
 * object of showing the property values or not.
 * 
 * @private
 * @alias Jaxer.Util.__listProps
 * @param {Object} obj
 * 		The source object
 * @param {String} [separator]
 * 		The separator string to use between properties. If this value is not
 * 		specified or if it is undefined, the string "; " will be used by default
 * @param {Boolean} showContents
 * 		A boolean value indicating whether property values should shown in
 * 		addition to the property names.
 * @return {String} Returns a string representation of the specified object.
 */
Util.__listProps = function __listProps(obj, separator, showContents)
{
	if (obj == null) return "null";
	if (typeof obj == "undefined") return "undefined";
	var props = [];
	if (typeof separator == "undefined" || separator == null) separator = "; ";
	for (var p in obj)
	{
		props.push(p + (showContents ? ": " + obj[p] : "") );
	}
	props = props.sort();
	var type = typeof obj;
	var constr = "";
	if (typeof obj.constructor)
	{
		constr = (typeof obj.constructor.name == "string") ? obj.constructor.name : obj.constructor.toString();
		if (!(obj instanceof obj.constructor))
		{
			constr += " (not an instanceof its own constructor!!)"
		}
	}
	props.unshift("[Type: " + type + ", constructor: " + constr + "]");
	return props.join(separator);
};

frameworkGlobal.Util = Jaxer.Util = Util;

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Utilities > String.js
 */
coreTraceMethods.TRACE('Loading fragment: String.js');
(function() {

// create String container
Util.String = {};

/**
 * Escapes an input string for use with javascript
 * 
 * @alias Jaxer.Util.String.escapeForJS
 * @param {String} raw
 * 		The source string
 * @return {String}
 * 		The escaped string suitable for use in an eval statement
 */
Util.String.escapeForJS = function escapeForJS(raw)
{
	var regex = new RegExp("'|\"|\\\\|</scrip" + "t>", "gi");
	var escaped = raw.replace(regex, function(txt)
	{
		switch (txt)
		{
			case "\\":
				return "\\\\";
				
			case "'":
				return "\\'";
				
			case '"':
				return '\\"';
				
			default: // Must be script end tag
				return "<\\/script>" ;
		}
	});
	
	return escaped;
};

/**
 * Escapes an input string for use with SQL
 * 
 * @alias Jaxer.Util.String.escapeForSQL
 * @param {String} raw
 * 		The source string
 * @return {String}
 * 		The escaped string suitable for use in a SQL query
 */
Util.String.escapeForSQL = function escapeForSQL(raw)
{
	var regex = /['\\\t\r\n]/g;
	var escaped = raw.replace(regex, function(a) {
		switch (a)
		{
			case '\'':
				return "\\'";
				break;
				
			case '\\':
				return "\\\\";
				break;
				
			case '\r':
				return "\\r";
				break;
				
			case '\n':
				return "\\n";
				break;
				
			case '\t':
				return "\\t";
				break;
		}
	});
	
	return escaped;
};

/**
 * Surround the provided string in single quotation marks
 * 
 * @alias Jaxer.Util.String.singleQuote
 * @param {String} text
 * 		The original string
 * @return {String}
 * 		The original string encased in single quotes
 */
Util.String.singleQuote = function(text)
{
	return "'" + text + "'";
};

/**
 * Left or right trim the provided string. Optionally, the user can specify a
 * list of character to trim form the source string. By default, whitespace is
 * removed. Also, the user can control which side of the string (start or end)
 * is trimmed with the default being both sides.
 * 
 * @alias Jaxer.Util.String.trim
 * @param {String} str
 * 		The source string
 * @param {String} [charsToTrim]
 * 		This optional parameter can be used to specify a list of characters to
 * 		remove from the sides of the source string. Any combination of these
 * 		characters will be removed. If this parameter is not specified, then all
 * 		whitespace characters will be removed.
 * @param {String} [leftOrRight]
 * 		This optional parameter can be used to control which side of the string
 * 		is trimmed. A value of "L" will trim the start of the string and all
 * 		other string values will trim the end of the string. If this parameter
 * 		is not specified, then both sides of the string will be trimmed
 * @return {String}
 * 		The resulting trimmed string
 */
Util.String.trim = function trim(str, charsToTrim, leftOrRight)
{
	if (charsToTrim == null || charsToTrim == " ")
	{
		if (leftOrRight)
		{
			var left = leftOrRight.toUpperCase().indexOf("L") == 0;
			
			return left ? str.replace(/^\s+/g,'') : str.replace(/\s+$/g,'');
		}
		else
		{
			return str.replace(/^\s+|\s+$/g,'');
		}
	}
	else // Another alternative to the below would be to (safely) build a regexp
	{
		var left = leftOrRight == null || leftOrRight.toUpperCase().indexOf("L") == 0;
		var right = leftOrRight == null || !left;
		
		for (var i=0; i<charsToTrim.length; i++)
		{
			var charToTrim = charsToTrim[i];
			
			if (left)
			{
				while (str.indexOf(charToTrim) == 0)
				{
					str = str.substr(1);
				}
			}
			if (right)
			{
				var length = str.length;
				
				while (length && (str.lastIndexOf(charToTrim) == length - 1))
				{
					str = str.substring(0, length - 1);
					length = str.length;
				}
			}
		}
		
		return str;
	}
};

/**
 * Convert a string to a CamelCase representation by removing interword spaces
 * and capitalizing the first letter of each word following an underscore
 * 
 * @alias Jaxer.Util.String.upperCaseToCamelCase
 * @param {String} orig
 * 		The orignal string containing underscores between words
 * @return {String}
 * 		The resulting string with underscores removed and the first letter of a
 * 		word capitalized
 */
Util.String.upperCaseToCamelCase = function upperCaseToCamelCase(orig)
{
	return orig.toLowerCase().replace(/_(\w)/g, function(all, c) { return c.toUpperCase(); });
};


/**
 * Check to see if a string starts with another string
 * 
 * @alias Jaxer.Util.String.startsWith
 * @param {String} orig
 * 	The string to look in
 * @param {String} str
 * 	The string to look for
 * @param {String} i Set to 'i' for case insensitive searches
 */
Util.String.startsWith = function startsWith(orig, str, i){
	i = (i) ? 'i' : '';
	var re = new RegExp('^' + str, i);
	return (orig.match(re)) ? true : false;
};

/**
 * Check to see if a string ends with a string
 * 
 * @alias Jaxer.Util.String.endsWith
 * @param {String} orig
 * 	The string to look in
 * @param {String} str
 * 	The string to look for
 * @param {String} i Set to 'i' for case insensitive searches
 */
Util.String.endsWith = function endsWith(orig, str, i){
	i = (i) ? 'gi' : 'g';
	var re = new RegExp(str + '$', i);
	return (orig.match(re)) ? true : false;
};


})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Utilities > DateTime.js
 */
coreTraceMethods.TRACE('Loading fragment: DateTime.js');
(function() {

// create the DateTime container
Util.DateTime = {};

/**
 * Converts a date to a string and pads the month and date values to align
 * all date values in columns.
 * 
 * @alias Jaxer.Util.DateTime.toPaddedString
 * @param {Date} date
 * 		The source date
 * @return {String}
 * 		The source data converted to a string with month and data values padded
 * 		with spaces to align all values
 */
Util.DateTime.toPaddedString = function toPaddedString(date)
{
	var m = date.getMonth();
	var d = date.getDate();
	var sep = "/";
	return date.toTimeString().substr(0, 8) + " " + (m < 10 ? " " : "") + m + sep + (d < 10 ? " " : "") + d + sep + date.getFullYear();
};

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Utilities > Math.js
 */
coreTraceMethods.TRACE('Loading fragment: Math.js');
(function() {

// create Math container
Util.Math = {};

/**
 * Determine whether the specified value is an integer value
 * 
 * @alias Jaxer.Util.Math.isInteger
 * @param {Number} num
 * 		The number to test
 * @return {Boolean}
 * 		Returns true if the number is an integer value
 */
Util.Math.isInteger = function isInteger(num)
{
	return Math.floor(num) == num;
};

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Utilities > Url.js
 */
coreTraceMethods.TRACE('Loading fragment: Url.js');
(function() {

// create URL container
Util.Url = {};

/**
 * Parse a URL into the following components, which are the
 * names of the properties on the resulting object used to access these values:
 * "url", "protocol", "host", "port", "path", "file", "query", "hostAndPost", 
 * "base" (protocol + host + port), "subdomain", "domain", "pathParts", 
 * "pathAndFile", and "queryParts".
 * @alias Jaxer.Util.Url.parseUrl
 * @param {String} url
 * 		The URL to parse
 * @return {Object}
 * 		The parts of a URL broken down as listed above
 */
Util.Url.parseUrl = function parseUrl(url)
{
	
	url = Util.String.trim(url);
	
    var pattern = /^((http|https):\/\/)?([^:\/\s]+)(:(\d+))?([^\.\?#]*?)([^\.\/\?#]+\.[^\?\s]+)?(\?[^#]+)?(#.+)?$/;
	var isMatch = url.match(pattern);
	
	var parsed = 
	{
		url: url,
		protocol: isMatch ? RegExp.$2 : "",
		host: isMatch ? RegExp.$3 : "",
		port: isMatch ? RegExp.$5 : "",
		path: isMatch ? Util.String.trim(RegExp.$6, "/", "r") : "",
		file: isMatch ? RegExp.$7 : "",
		query: (isMatch && RegExp.$8) ? RegExp.$8.substr(1) : "",
		hash: (isMatch && RegExp.$9) ? RegExp.$9.substr(1) : ""
	};
	
	parsed.hostAndPort = parsed.host + (parsed.port == "" ? "" : (":" + parsed.port));
	
	parsed.base = (parsed.protocol == "" ? "" : (parsed.protocol + "://")) + parsed.hostAndPort;
	
	var domainParts = parsed.host.split(".");
	parsed.subdomain = domainParts.slice(0, -2).join(".");
	parsed.domain = domainParts.slice(-2).join(".");
	
	parsed.pathParts = Util.String.trim(parsed.path, "/").split("/");
	
	parsed.pathAndFile = [parsed.path, parsed.file].join((parsed.path != "" && parsed.file != "") ? "/" : "");
	
	parsed.queryParts = Util.Url.queryToHash(parsed.query);
	
    return parsed;
	
};

/**
 * Parse URL fragments into the following components, which are the
 * names of the properties on the resulting object used to access these values:
 * "url", "protocol", "host", "port", "path", "file", "query", "hostAndPost", 
 * "base" (protocol + host + port), "subdomain", "domain", "pathParts", 
 * "pathAndFile", and "queryParts".
 * 
 * @alias Jaxer.Util.Url.parseUrlFragments 
 * @param {String} hostAndPort
 * 		The host (and port, if any) containing the given path 
 * @param {String} absolutePath
 * 		The absolute path to a resource on the host
 * @return {Object}
 * 		The parts of a URL broken down as listed above
 */
Util.Url.parseUrlFragments = function parseUrlFragments(hostAndPort, absolutePath)
{
	
	var pattern, isMatch;
	
	pattern = /^\s*(.*?)(:(\d+))?\s*$/;
	isMatch = hostAndPort.match(pattern);
	host = isMatch ? RegExp.$1 : "";
	port = isMatch ? RegExp.$3 : "";
	absolutePath = Util.String.trim(absolutePath);
	
	var pattern = /^([^\.\?#]*?)([^\.\/\?#]+\.[^\?\s]+)?(\?[^#]+)?(#.+)?$/;
	var isMatch = absolutePath.match(pattern);
	
	var parsed = 
	{
		url: hostAndPort + absolutePath,
		protocol: "", // we don't have this information
		host: host,
		port: port,
		path: isMatch ? Util.String.trim(RegExp.$1, "/", "r") : "",
		file: isMatch ? RegExp.$2 : "",
		query: (isMatch && RegExp.$3) ? RegExp.$3.substr(1) : "",
		hash: (isMatch && RegExp.$4) ? RegExp.$4.substr(1) : ""
	};
	
	parsed.hostAndPort = parsed.host + (parsed.port == "" ? "" : (":" + parsed.port));
	
	// Note this will be the same as hostAndPort until we get the protocol information somehow
	parsed.base = (parsed.protocol == "" ? "" : (parsed.protocol + "://")) + parsed.hostAndPort;
	
	var domainParts = host.split(".");
	parsed.subdomain = domainParts.slice(0, -2).join(".");
	parsed.domain = domainParts.slice(-2).join(".");
	
	parsed.pathParts = Util.String.trim(parsed.path, "/").split("/");
	
	parsed.pathAndFile = [parsed.path, parsed.file].join((parsed.path != "" && parsed.file != "") ? "/" : ""); 
	
	parsed.queryParts = Util.Url.queryToHash(parsed.query);

	return parsed;
	
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
	var queryStrings = query.split("&");
	
	for (var i=0; i<queryStrings.length; i++)
	{
		var nameValue = queryStrings[i].split("=");
		hash[Util.Url.formUrlDecode(nameValue[0])] = (nameValue.length > 1) ? Util.Url.formUrlDecode(nameValue[1]) : null;
	}
	
	return hash;
};

/**
 * Decode a URL by replacing +'s with spaces and all hex values (%xx) with their character value
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
	str = str.replace("+", " "); // For those encoders that use + to represent a space. A true + should have been encoded as %2B
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
 * Combines any number of URL fragments into a single URL, using / as the separator.
 * Before joining two fragments with the separator, it strips any
 * existing separators on the fragment ends to be joined
 * @alias Jaxer.Util.Url.combine
 * @param {String} ... Takes any number of string URL fragments
 * @return {String} The fragments joined into a URL
 */
Util.Url.combine = function combine()
{
	if (arguments.length == 0) return '';
	if (arguments.length == 1) return arguments[0];
	var pieces = Array.prototype.slice.call(arguments);
	var sep = "/";
	var stripRight = new RegExp("\\" + sep + "+$");
	var stripLeft  = new RegExp("^\\" + sep + "+");
	var stripBoth  = new RegExp("^\\" + sep + "|\\" + sep + "$", 'g');
	pieces[0] = pieces[0].replace(stripRight, '');
	pieces[pieces.length - 1] = pieces[pieces.length - 1].replace(stripLeft, '');
	for (var i=1; i<pieces.length-1; i++)
	{
		pieces[i] = pieces[i].replace(stripBoth, '');
	}
	return pieces.join(sep);
}

var filePattern = /^(file|resource|chrome)\:\/\//i;

/**
 * Tests whether the given URL is a reasonable file URL rather than something that's available over the network.
 * The test is pretty simplistic: the URL must start with file://, resource://, or chrome://,
 * or it must contain a backslash (i.e. a Windows filesystem separator)
 * @alias Jaxer.Util.Url.isFile
 * @param {String} url
 * @return {Boolean} true if a reasonable file URL, false otherwise
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
 * Otherwise we turn it into a file-type URL by prefixing it with "file:///"
 * @alias Jaxer.Util.Url.ensureFileProtocol
 * @param {String} url
 * 	The URL to apply this to
 */
Util.Url.ensureFileProtocol = function ensureFileProtocol(url)
{
	if (url.match(filePattern)) return url;
	return 'file:///' + url; // Note absolute path because we don't know any better
}

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Utilities > CRC32.js
 */
coreTraceMethods.TRACE('Loading fragment: CRC32.js');
/**
 * Original code comes from KevLinDev.com at
 * http://www.kevlindev.com/utilities/crc32/crc32.zip
 */

(function(){

var CRC32 = {};

CRC32.VERSION = 1.0;

// CRC polynomial - 0xEDB88320
CRC32.table = [
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
];

/**
 * Create a CRC32 from the characters of a string
 * 
 * @alias Jaxer.CRC32.getStringCRC
 * @param {String} text
 * 		The source string to convert
 * @return {Number}
 * 		The resulting CRC32 of the source string
 */
CRC32.getStringCRC = function(text)
{
	var bytes = [];
	
	for (var i = 0; i < text.length; i++)
	{
		var code = text.charCodeAt(i);
		
		if (code > 0xFF)
		{
			var lo =   code & 0x00FF;
			var hi = ((code & 0xFF00) >> 8) && 0xFF;
			
			bytes.push(lo);
			bytes.push(hi);
		}
		else
		{
			bytes.push(code);
		}
	}
	
	return CRC32.getCRC(bytes, 0, bytes.length);
};

/**
 * Create a CRC32 from an array of bytes. The user may specify the starting
 * offset within the array and the total number of bytes past the offset to
 * include in the resulting CRC32 
 * 
 * @alias Jaxer.CRC32.getCRC
 * @param {Array} data
 * 		An array of byte values
 * @param {Number} [offset]
 * 		The offset within the array where the calculated CRC32 should start
 * @param {Number} [count]
 * 		The number of bytes starting from the offset to include in the resulting
 * 		CRC32.
 * @return {Number}
 * 		The resulting CRC32
 */
CRC32.getCRC = function(data, offset, count)
{
	offset = offset || 0;
	count = count || data.length;
	 
    var crc = 0xFFFFFFFF;
    var k;

    for (var i = 0; i < count; i++)
	{
        k = (crc ^ data[offset+i]) & 0xFF;
        crc = ( (crc >> 8) & 0x00FFFFFF ) ^ CRC32.table[k];
    }

    return ~crc;
};

Util.CRC32 = CRC32;

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Utilities > DOM.js
 */
coreTraceMethods.TRACE('Loading fragment: DOM.js');
(function() {

// create DOM container
Util.DOM = {};

/**
 * Convert an object's properties and property values into a string of
 * attributes suitable for use in an HTMLElement's innerHTML property. Each
 * property is used as the attribute name and each property value becomes the
 * attribute value. Attribute values are surrounding in double-quotes and all
 * property values containing double-quotes will have those characters properly
 * escaped.
 * 
 * @alias Jaxer.Util.DOM.hashToAttributesString
 * @param {Object} hash
 * 		The source object
 * @return {String}
 * 		The resulting string of attribute name/value pairs
 */
Util.DOM.hashToAttributesString = function hashToAttributesString(hash)
{
	return "";
	
	var result = [];
	
	for (var p in hash)
	{
		var value = hash[p].replace(/"/g, '\\"');
		
		result.push(p + "=\"" + value + "\"");
	}
	
	return result.join(" ");
};

/**
 * Convert an array into a string where each item is separated by a newline. If
 * the specfied item is not an Array, then the value itself will be returned.
 * 
 * @private
 * @alias Jaxer.prepareContents
 * @param {Object} contents
 * 		The item to process
 * @return {Object}
 * 		The resulting string value if the "contents" item was an Array;
 * 		otherwise this returns the "contents" item itself
 */
function prepareContents(contents)
{
	return (contents instanceof Array) ? "\n" + contents.join("\n") + "\n" : contents;
}

/**
 * Create a new script element with the specified content and attributes
 * 
 * @alias Jaxer.Util.DOM.createScript
 * @param {Document} doc
 * 		The DocumentElement to use when creating elements for the active
 * 		document.
 * @param {String} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.createScript = function createScript(doc, contents, attributes)
{
	var script = doc.createElement("script");
	
	script.setAttribute("type", "text/javascript");
	if (attributes)
	{
		for (var p in attributes)
		{
			script.setAttribute(p, attributes[p]);
		}
	}
	script.innerHTML = prepareContents(contents);
	
	return script;
};

/**
 * Creates a new script element and adds it before a specified element in the
 * DOM.
 * 
 * @alias Jaxer.Util.DOM.insertScriptBefore
 * @param {Document} doc
 * 		The DocumentElement to use when creating elements for the active
 * 		document.
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element in the DOM before which the new script element will be
 * 		inserted
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.insertScriptBefore = function insertScriptBefore(doc, contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.parentNode.insertBefore(script, elt);
	
	return script;
};

/**
 * Creates a new script element and adds it as the first child of a specified
 * element in the DOM.
 * 
 * @alias Jaxer.Util.DOM.insertScriptAtBeginning
 * @param {Document} doc
 * 		The DocumentElement to use when creating elements for the active
 * 		document.
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element in the DOM where the new script element will be added as the
 * 		element's first child.
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.insertScriptAtBeginning = function insertScriptAtBeginning(doc, contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.insertBefore(script, elt.firstChild);
	elt.insertBefore(doc.createTextNode("\n\t\t"), script);
	
	return script;
};

/**
 * Creates a new script element and add it as the first child of the specified
 * element in the DOM.
 * 
 * @alias Jaxer.Util.DOM.insertScriptAfter
 * @param {Document} doc
 * 		The DocumentElement to use when creating elements for the active
 * 		document.
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element in the DOM after which the new script element will be
 * 		inserted
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.insertScriptAfter = function insertScriptAfter(doc, contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.parentNode.insertBefore(script, elt.nextSibling);
	
	return script;
};

/**
 * Creates a new script element and adds it as the last child of a specified
 * element in the DOM.
 * 
 * @alias Jaxer.Util.DOM.insertScriptAtEnd
 * @param {Document} doc
 * 		The DocumentElement to use when creating elements for the active
 * 		document.
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element in the DOM where the new script element will be added as the
 * 		element's last child.
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.insertScriptAtEnd = function insertScriptAtEnd(doc, contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.appendChild(doc.createTextNode("\n\t\t"));
	elt.appendChild(script);
	
	return script;
};

/**
 * Replace a specified element in the DOM with a new script element.
 * 
 * @alias Jaxer.Util.DOM.replaceWithScript
 * @param {Document} doc
 * 		The DocumentElement to use when creating elements for the active
 * 		document.
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element to replace with the a script element
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.replaceWithScript = function replaceWithScript(doc, contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.parentNode.replaceChild(script, elt);
	
	return script;
};

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Utilities > Cookie.js
 */
coreTraceMethods.TRACE('Loading fragment: Cookie.js');
(function() {

// create Cookie container
/**
 * @namespace {Util.Cookie}
 */
Util.Cookie = {};

/**
 * Set a cookie name/value pair
 * 
 * @alias Jaxer.Util.Cookie.set
 * @param {String} name
 * 		The name of the cookie to set
 * @param {String} value
 * 		The value of the cookie
 */
Util.Cookie.set = function set(name, value)
{
	var cookieString = encodeURIComponent(name) + "=" + encodeURIComponent(value) + "; path=/";
	//var cookieString = name + "=" + encodeURIComponent(value) + "; path=/";
	Jaxer.response.addHeader("Set-Cookie", cookieString, false);
};

/**
 * Get a cookie key value
 * 
 * @alias Jaxer.Util.Cookie.get
 * @param {String} name
 * 		The name of the cookie to retrieve
 * @return {String|null}
 * 		Returns the value of the specified cookie or null if the vaule does not
 * 		exist.
 */
Util.Cookie.get = function get(name)
{
	var value = null;
	var cookies = Util.Cookie.getAll();
	
	if (typeof cookies[name] != "undefined")
	{
		value = cookies[name];
	}
	
	return value;
};

/**
 * Get an object containing all cookie keys and values. Each cookie name will
 * become a property on the object and each cookie value will become that
 * property's value.
 * 
 * @alias Jaxer.Util.Cookie.getAll
 * @return {Object}
 * 		The resulting object of all cookie key/value pairs
 */
Util.Cookie.getAll = function getAll()
{
	var cookies = {};
	
	if (typeof Jaxer.request.headers.Cookie == "string")
	{
		var cookieStrings = Jaxer.request.headers.Cookie.split(";");
		
		for (var i=0; i<cookieStrings.length; i++)
		{
			var cookie = cookieStrings[i].split("=");
			
			if (cookie.length < 2)
			{
				continue;
			}
			
			var name = decodeURIComponent(cookie[0].replace(/^\s+/g,''));
			var value = decodeURIComponent(cookie[1].replace(/^\s+/g,''));
			
			cookies[name] = value;
		}
	}
	
	return cookies;
};

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Utilities > Overrides.js
 */
coreTraceMethods.TRACE('Loading fragment: Overrides.js');
(function(){

// create Overrides container
var Overrides = {};

var overridesToApply = []; // These will be set on the developer's server-side window object
var noOps = ["setTimeout", "setInterval"];

/**
 * Alert in a server-side context will generate an info-level log message
 * @alias Jaxer.Overrides.alert
 * @param {String} message
 */
Overrides.alert = function alert(message)
{
	Jaxer.Log.info(message);
}
overridesToApply.push(Overrides.alert);

/**
 * Prompt in a server-side context will generate an info-level log message
 * @alias Jaxer.Overrides.prompt
 * @param {String} message
 */
Overrides.prompt = function prompt(message)
{
	Jaxer.Log.info("(prompt called for: " + message + ")");
}
overridesToApply.push(Overrides.prompt);

/**
 * Confirm in a server-side context will generate an info-level log message
 * @alias Jaxer.Overrides.confirm
 * @param {String} message
 */
Overrides.confirm = function confirm(message)
{
	Jaxer.Log.info("(confirm called for: " + message + ")");
}
overridesToApply.push(Overrides.prompt);

Overrides.extendDomSetters = function extendDomSetters(global)
{
	if (global.HTMLInputElement) 
	{
		var inputValueSetter = global.HTMLInputElement.prototype.__lookupSetter__('value');
		global.HTMLInputElement.prototype.__defineSetter__('value', function(val)
		{
			inputValueSetter.call(this, val);
			this.setAttribute('value', val);
		});
		var inputCheckedSetter = global.HTMLInputElement.prototype.__lookupSetter__('checked');
		global.HTMLInputElement.prototype.__defineSetter__('checked', function(val)
		{
			inputCheckedSetter.call(this, val);
			if (val) 
			{
				this.setAttribute('checked', null);
			}
			else 
			{
				this.removeAttribute('checked');
			}
		});
	}
	if (global.HTMLTextAreaElement) 
	{
		var textAreaValueSetter = global.HTMLTextAreaElement.prototype.__lookupSetter__('value');
		global.HTMLTextAreaElement.prototype.__defineSetter__('value', function(val)
		{
			textAreaValueSetter.call(this, val);
			this.textContent = val;
		});
	}
	if (global.HTMLSelectElement) 
	{
		var selectSelectedIndexSetter = global.HTMLSelectElement.prototype.__lookupSetter__('selectedIndex');
		global.HTMLSelectElement.prototype.__defineSetter__('selectedIndex', function(index)
		{
			selectSelectedIndexSetter.call(this, index);
			var option = this.options[index];
			if (option) 
			{
				for (var i = 0, len = this.options.length; i < len; i++) 
				{
					if (i == index) 
					{
						this.options[i].setAttribute('selected', null);
					}
					else 
					{
						this.options[i].removeAttribute('selected');
					}
				}
			}
		});
	}
	if (global.HTMLOptionElement) 
	{
		var optionSelectedSetter = global.HTMLOptionElement.prototype.__lookupSetter__('selected');
		global.HTMLOptionElement.prototype.__defineSetter__('selected', function(val)
		{
			optionSelectedSetter.call(this, val);
			if (val) 
			{
				this.setAttribute('selected', null);
			}
			else 
			{
				this.removeAttribute('selected');
			}
		});
	}
}

/**
 * Applies all the overrides on the given global object, including the no-ops "setTimeout" and "setInterval"
 * @alias Jaxer.Overrides.applyAll
 * @param {Object} global
 * 	The global (typically window) object
 */
Overrides.applyAll = function applyAll(global)
{
	Util.foreach(overridesToApply, function(override)
	{
		global[override.name] = override;
	});
	Util.foreach(noOps, function(noOpName)
	{
		global[noOpName] = function() {};
	});
	if (typeof global.document == "object") 
	{
		// Have to literally define and evaluate this function on the target global
		// because of issues with cross-global setters in FF3
		Includer.evalOn(Jaxer.Overrides.extendDomSetters.toSource() + "(document.defaultView)", global);
	}
}

Overrides.applyAll(frameworkGlobal); // These should also be available on the Framework, and also on the window in callbacks

frameworkGlobal.Overrides = Jaxer.Overrides = Overrides;

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Logging > Log.js
 */
coreTraceMethods.TRACE('Loading fragment: Log.js');
(function() {
	
/**
 * Log is a static object meant to be shared across the framework and perhaps even the user's code.
 * In a module, use it to create a module-specific logger and then log with it.
 * 
 * @example
 * To log without a module name, use Jaxer.Log.info("my message"), where instead of info you can use any of the 
 * six logging levels: trace(...), debug(...), info(...), warn(...), error(...), and fatal(...).
 * The generic logger is set by default to only log messages at or above the info level.
 * 
 * @example
 * To log with a module name, first define a log helper: var log = Jaxer.Log.forModule("myModule");
 * This will get or create the module logger with this name.
 * Then use log.info(...) or any of the other methods.
 * 
 * The default level of messages to log is set in configLog.js and can also be changed in memory:
 * see setLevel() and selAllModuleLevels().
 * 
 * @alias Jaxer.Log
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
 * return a array of implemented Logger levelnames
 * e.g. "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
 * @alias Jaxer.Log.getLevelNames
 * @return {String[]}
 */
Log.getLevelNames = function getLevelNames()
{
	return levelNames;
};

/**
 * Lazily creates a logger for the given module name, if needed,
 * and in any case returns it. The referenced Log object knows to create log entries
 * marked as belonging to the specific module.
 * @alias Jaxer.Log.forModule
 * @param {String} moduleName 
 * 	The name of the module (just an identifier string).
 * @return {Log.ModuleLogger} The module-specific logger to use for log entries.
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
 * Returns a boolean to indicate whether the referenced Log object is wired to do logging for the provided modulename
 * @alias Jaxer.Log.hasModule
 * @param {String} moduleName
 * 	The name of the module to look up
 * @return {Boolean}
 */
Log.hasModule = function hasModule(moduleName)
{
	return moduleName in modules;
};

/**
 * Gets a sorted array of all the currently-defined modules.
 * @alias Jaxer.Log.getSortedModuleNames
 * @return {Array} An array of strings, sorted alphabetically, of the names of modules
 * for which ModuleLoggers have been defined.
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
 * set the logging level on ALL modules to level provided for the referenced Log object
 * @alias Jaxer.Log.setAllModuleLevels
 * @param {Jaxer.Log.Level} level
 * 	The new level to use for all of them
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
 * add an appender to the referenced Log object
 * @alias Jaxer.Log.addAppender
 * @param {String} name
 * 	The name of the appender
 * @param {Log.Appender} appender
 * 	An instance of the appender. It should be derived from Jaxer.Log.Appender.
 */
Log.addAppender = function addAppender(name, appender)
{
	appenders[name] = appender;
};

/**
 * remove the specified appender from the referenced Log object
 * @alias Jaxer.Log.removeAppender
 * @param {String} name
 * 	The name of the appender to use.
 */
Log.removeAppender = function removeAppender(name)
{
	delete appenders[name];
};

/**
 * get an appender reference from the referenced Log object
 * @alias Jaxer.Log.getAppender
 * @param {String} name
 * 	The name of the appender to look for
 * @return {Log.Appender} The appender instance
 */
Log.getAppender = function getAppender(name)
{
	return appenders[name];
};

/**
 * The internal logging method which logs to all the current appenders
 * @alias Jaxer.Log._log
 * @private
 * @param {String} moduleName
 * 	The associated module
 * @param {Jaxer.Log.Level} level
 * 	The level of this message
 * @param {String} message
 * 	The message to log
 * @param {Object} [exception]
 * 	An optional exception to use
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
 * @alias Jaxer.Log.init
 * @private
 * @param {String} defModuleLevelName
 * 	The default name to use when there's no module name specified
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
 * Set the logging level for the referenced Log object
 * @alias Jaxer.Log.setLevel
 * @param {Jaxer.Log.Level} level
 * 	The level to use on the generic logger (the one that's not module-specific)
 */
Log.setLevel = function(level)
{
	Log.genericLogger.setLevel(level);
};

/**
 * get the logging level of the generic logger
 * @alias Jaxer.Log.getLevel
 * @return {Jaxer.Log.Level} The level below which non-module-specific messages will not be logged
 */
Log.getLevel = function getLevel()
{
	return Log.genericLogger.getLevel();
};

frameworkGlobal.Log = Jaxer.Log = Log;

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Logging > Level.js
 */
coreTraceMethods.TRACE('Loading fragment: Level.js');
(function(){

/**
 * The constructor of a logging level object. 
 * Messages must exceed a certain severity level before they are logged.
 * @alias Jaxer.Log.Level
 * @constructor
 * @param {String} name
 * 	The name of the level
 * @param {Number} value
 * 	The numeric value to associate with this level. Higher numbers are more severe.
 */
function Level(name, value)
{
	this.name = name.toUpperCase();
	this.value = value;
}

/**
 * The textual representation of a level, namely its name
 * @alias Jaxer.Log.Level.prototype.toString
 * @return {String} The name
 */
Level.prototype.toString = function toString()
{
	return this.name;
}

/**
 * A common comparison operator on Jaxer.Log.Level objects: is the current level at or above the given level?
 * @alias Jaxer.Log.Level.prototype.isAtLeast
 * @return {Boolean} true if matches or exceeds the given level
 */
Level.prototype.isAtLeast = function isAtLeast(otherLevel)
{
	return this.value >= otherLevel.value;
}

/**
 * A common comparison operator on Jaxer.Log.Level objects: is the current level below the given level?
 * @alias Jaxer.Log.Level.prototype.isBelow
 * @return {Boolean} true if falls below the given level
 */
Level.prototype.isBelow = function isBelow(otherLevel)
{
	return this.value < otherLevel.value;
}

Log.Level = Level;

})();


/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Logging > ModuleLogger.js
 */
coreTraceMethods.TRACE('Loading fragment: ModuleLogger.js');
(function(){

/**
 * An object that's created only through the global Log.forModule("...") method for module-specific logging.
 * Usually you create such a logger at the beginning of your module, and then use it throughout your module
 * for logging at various levels. It has its own level, so you can control the verbosity of logging per module.
 * @alias Jaxer.Log.ModuleLogger
 * @constructor
 * @param {String} moduleName
 * 	The name to use for the module
 */
function ModuleLogger(name, level)
{
	this.moduleName = name;
	this.level = level;
}

/**
 * The internal logging function for the module logger, meant to be called from the level-specific methods.
 * It ASSUMES that its caller's caller is the function that's really doing the logging, and uses its name
 * in the log (as well as the module's name).
 * @alias Jaxer.Log.ModuleLogger.prototype._log
 * @param {Jaxer.Log.Level} level
 * 		The level at which to log the message
 * @param {String} message
 * 		The message (the module and caller will be prepended automatically)
 * @param {Exception} [exception]
 * 		Optional: an exception to log
 * @param {Function} [inFunction]
 * 	The function that should be associated with this message, otherwise the caller will be used
 */
ModuleLogger.prototype._log = function(level, message, exception, inFunction)
{
	if (level.isBelow(this.level))
	{
		return;
	}
	inFunction = inFunction || this._log.caller.caller;
	var calledFrom = (typeof inFunction == "function") ? inFunction.name : "";
	message = ["[", this.moduleName, ".", calledFrom, "] ", message].join("");
	if ((typeof inFunction == "function") && level.isAtLeast(Log.minLevelForStackTrace))
	{
		var stackTrace = [];
		var callerFunction = inFunction.caller;
		while (callerFunction)
		{
			stackTrace.push("  --> " + callerFunction.name);
			callerFunction = callerFunction.caller;
		}
		message += "\n" + stackTrace.join("\n");
	}
	Log._log(this.moduleName, level, message, exception);
};

// Create methods for the various levels
var levelNames = Log.getLevelNames();

for (var i = 0; i < levelNames.length; i++)
{
	var levelName = levelNames[i];
	var methodName = levelName.toLowerCase();
	eval('ModuleLogger.prototype[methodName] = function(message, exception, inFunction) { this._log(Log.' + levelName + ', message, exception, inFunction); }');
}

/**
 * Gets the level to which this ModuleLogger is set -- appending messages below this level will do nothing
 * @alias Jaxer.Log.ModuleLogger.prototype.getLevel
 * @return {Jaxer.Log.Level} The current level
 */
ModuleLogger.prototype.getLevel = function getLevel()
{
	return this.level;
};

/**
 * Sets the level below which this moduleLogger will not log messages.
 * @alias Jaxer.Log.ModuleLogger.prototype.getLevel
 * @param {Jaxer.Log.Level} level
 * 	The minimum loggable level. Should be one of Log.TRACE, Log.DEBUG,
 * 	Log.INFO, Log.WARN, Log.ERROR, Log.FATAL.
 */
ModuleLogger.prototype.setLevel = function setLevel(level)
{
	this.level = level;
};

Log.ModuleLogger = ModuleLogger;

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Logging > Appender.js
 */
coreTraceMethods.TRACE('Loading fragment: Appender.js');
(function(){

/**
 * This is the base class of all Appenders: listeners that know how to append log messages somewhere,
 * e.g. to a file or a database.
 * @alias Jaxer.Log.Appender
 * @constructor
 * @param {String} name
 * 	The name to use to identify this appender
 * @param {Jaxer.Log.Level} level
 * 	The level to start this appender at -- messages below this will not be appended
 */
function Appender(name, level)
{
	this.level = level;
}

/**
 * Append a message associated with the given module to the log
 * @alias Jaxer.Log.Appender.append
 * @private
 * @param {String} moduleName
 * 	The name of the module to use
 * @param {Jaxer.Log.Level} level
 * 	The level to use for this message
 * @param {String} message
 * 	The message to log
 * @param {Object} [exception]
 * 	An optional exception object to use
 */
Appender.prototype.append = function append(moduleName, level, message, exception)
{
};

/**
 * The level to which this moduleLogger is set. Messages below this level will not be logged.
 * @alias Jaxer.Log.Appender.getLevel
 * @return {Jaxer.Log.Level} The current level
 */
Appender.prototype.getLevel = function getLevel()
{
	return this.level;
};

/**
 * Sets the level below which this moduleLogger will not log messages.
 * @alias Jaxer.Appender.prototype.setLevel
 * @param {Jaxer.Log.Level} level
 *	The minimum loggable level. Should be one of Log.TRACE, Log.DEBUG,
 *	Log.INFO, Log.WARN, Log.ERROR, Log.FATAL.
 */
Appender.prototype.setLevel = function setLevel(level)
{
	this.level = level;
};

/**
 * Identifies this appender instance by name
 * @alias Jaxer.Appender.prototype.toString
 * @return {String}	The appender's name
 */
Appender.prototype.toString = function()
{
	return "[" + this.name + "]";
};

Log.Appender = Appender;

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Logging > FileAppender.js
 */
coreTraceMethods.TRACE('Loading fragment: FileAppender.js');
(function() {

/**
* Create an instance of the FileAppender, a file-based Appender
* @alias Jaxer.Log.FileAppender
* @constructor
* @param {String} name 
* 	the appender name
* @param {Jaxer.Log.Level} level 
* 	the logging level to start using with this appender
* @param {String} logPath 
* 	the path to the logfile
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
* @alias Jaxer.Log.FileAppender.setPath
* @param {String} newPath 
* 	the new path to use for this appender. 
*/
FileAppender.prototype.setPath = function(newPath)
{
	this.logPath = newPath;
}

/**
* Appends the provided message to the referenced appenders log
* @alias Jaxer.Log.FileAppender.append
 * @param {String} moduleName
 * 	The name of the module to use
 * @param {Jaxer.Log.Level} level
 * 	The level to use for this message
 * @param {String} message
 * 	The message to log
 * @param {Object} [exception]
 * 	An optional exception object to use
*/
FileAppender.prototype.append = function(moduleName, level, message, exception) 
{
	if (this.logPath == null || this.logPath == "") return;
	
	var formattedMessage = Util.DateTime.toPaddedString(new Date()) + " [" + level + "] [" + moduleName + "] " + message + (exception ? ('; ' + exception) : '') + "\n";

	File.append(this.logPath, formattedMessage);

};

Log.FileAppender = FileAppender;

})();


/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Logging > CoreAppender.js
 */
coreTraceMethods.TRACE('Loading fragment: CoreAppender.js');
(function() {

/**
 * Create an instance of the Core appender -- appends log messages to the same file as the Jaxer core
 * @alias Jaxer.Log.CoreAppender
 * @constructor
 * @param {String} name 
 * 	The name to use to identify this appender
 * @param {Jaxer.Log.Level} level
 * 	The level to start this appender at -- messages below this will not be appended
 * @param {coreTraceMethods} coreTraceMethods
 * 	The hashtable of core trace methods that this should invoke
 * @param {coreTraceBefore} coreTraceBefore
 * 	The core function to call before calling any core method -- it will turn on logging regardless of the core's settings
 * @param {coreTraceAfter}  coreTraceAfter
 * 	The core function to call after calling any core method -- it will restore using the core's log settings
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
 * @alias Jaxer.Log.CoreAppender.append
 * @private
 * @param {String} moduleName
 * 	The name of the module to use
 * @param {Jaxer.Log.Level} level
 * 	The level to use for this message
 * @param {String} message
 * 	The message to log
 * @param {Object} [exception]
 * 	An optional exception object to use
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


/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Logging > LogInit.js
 */
coreTraceMethods.TRACE('Loading fragment: LogInit.js');
Log.init("WARN");

if (typeof Config.USE_CORE_LOG_FILE != "undefined" && Config.USE_CORE_LOG_FILE)
{
	var coreAppender = new Log.CoreAppender("CoreAppender", Log.TRACE, coreTraceMethods, coreTraceBefore, coreTraceAfter);
	Log.addAppender(coreAppender.name, coreAppender);
}
else // use file logging
{
	
	getService = function(aURL, aInterface){
	    try {
	        // determine how 'aInterface' is passed and handle accordingly
	        switch (typeof(aInterface)) {
	            case "object":
	                return Components.classes[aURL].getService(aInterface);
	                break;
	                
	            case "string":
	                return Components.classes[aURL].getService(JSLib.interfaces[aInterface]);
	                break;
	                
	            default:
	                return Components.classes[aURL].getService();
	                break;
	        }
	    } 
	    catch (e) {
	        throw new Exception(e);
	    }
	}

	var logPath;
	if (typeof Config.LOG_PATH == "string")
	{
		logPath = Config.LOG_PATH;
	}
	else
	{
		var logDir = getService("@mozilla.org/file/directory_service;1", "nsIProperties").get("CurProcD", Components.interfaces.nsIFile).path;
		//var logDir = (new document.Jaxer.DirUtils()).getCurProcDir();
		//var logDir = "jaxer:/";
		var logFileName = (typeof Config.LOG_FILE_NAME == "string") ? Config.LOG_FILE_NAME : "log.txt";
		logPath = Dir.combine(logDir, logFileName);
		//logPath = logDir + "/" + logFileName;
	}
	
	var fileAppender = new Log.FileAppender("FileAppender", Log.TRACE, logPath);
	Log.addAppender(fileAppender.name, fileAppender);
}

include(Config.FRAMEWORK_DIR + "/configLog.js");

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/IO > jslib_namespace_import.js
 */
coreTraceMethods.TRACE('Loading fragment: jslib_namespace_import.js');
(function(){
	
	/**
	 * import the JSLib namespace objects into Jaxer and the window object
	 */
    frameworkGlobal.Filesystem 		= Jaxer.Filesystem 		= JSLib.Filesystem;
    frameworkGlobal.File 			= Jaxer.File 			= JSLib.File;
    frameworkGlobal.FileUtils 		= Jaxer.FileUtils 		= JSLib.FileUtils ;
    frameworkGlobal.Dir 				= Jaxer.Dir 			= JSLib.Dir;
    frameworkGlobal.DirUtils 		= Jaxer.DirUtils 		= JSLib.DirUtils;
    frameworkGlobal.System 			= Jaxer.System 			= JSLib.System;
    frameworkGlobal.Socket 			= Jaxer.Socket 			= JSLib.Socket;
    frameworkGlobal.SocketListener 	= Jaxer.SocketListener 	= JSLib.SocketListener;
    frameworkGlobal.NetworkUtils 	= Jaxer.NetworkUtils 	= JSLib.NetworkUtils;
	
})();


/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/IO > System.js
 */
coreTraceMethods.TRACE('Loading fragment: System.js');
(function()
{
    
    var du = new Jaxer.DirUtils();
    
    /**
     * The path on disk of the current executable's folder
     * @alias Jaxer.System.executableFolder
     * @private
     */
	try
	{
    	System.executableFolder = du.current;
	}
	catch (e)
	{
		coreTraceMethods.WARN("Could not set System.executableFolder: " + e);
		System.executableFolder = null;
	}
    
    /**
     * The path on disk of the current user's home folder
     * @alias Jaxer.System.homeFolder
     * @private
     */
	try
	{
    	System.homeFolder = du.home;
	}
	catch (e)
	{
		coreTraceMethods.WARN("Could not set System.homeFolder: " + e);
		System.homeFolder = null;
	}
    
    /**
     * The path on disk of the current user's desktop folder
     * @alias Jaxer.DirUtils.prototype.desktopFolder
     * @private
     */
	try
	{
    	System.desktopFolder = du.desktop;
	}
	catch (e)
	{
		coreTraceMethods.WARN("Could not set System.desktopFolder: " + e);
		System.desktopFolder = null;
	}
        
    /**
     * The path on disk of the system's temp folder
     * @alias Jaxer.DirUtils.prototype.tempFolder
     * @private
     */
	try
	{
    	System.tempFolder = du.temp;
 	}
	catch (e)
	{
		coreTraceMethods.WARN("Could not set System.tempFolder: " + e);
		System.tempFolder = null;
	}
   
    /**
     * The filesystem separator character (either \ or /)
     * @alias Jaxer.System.separator
     *
     * @return {String} fileSystem path separator
     */
	try
	{
    	System.separator = (System.executableFolder.indexOf('\\') != -1) ? '\\': '/';
	}
	catch (e)
	{
		coreTraceMethods.WARN("Could not set System.separator: " + e);
		System.separator = null;
	}

    Jaxer.System = System;
    
})();



/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/IO > File.js
 */
coreTraceMethods.TRACE('Loading fragment: File.js');
(function() {

/**
 * Read the contents of a file on local disk. If the file does not exist, returns a null
 * @alias Jaxer.File.read
 * @param {String} path	
 * 	The full or partial (to be resolved) path to read
 * @return {String|null} the contents of the file as a string, or null if the file does not exist
 */
File.read = function read(path)
{
	var fullPath = Jaxer.Dir.resolvePath(path);
	var file = new File(fullPath);
	if (!file.exists())
	{
		return null;
	}
	var data = "";
	try
	{
		file.open('r');
		data = file.read();
	}
	finally
	{
		if (file)
		{
			file.close();
		}
	}
	return data;
};

/**
 * Does the file (or folder) exist on disk?
 * @alias Jaxer.File.exists
 * @param {String} path	
 * 	The full or partial (to be resolved) path to test
 * @return {Boolean} true if exists, false otherwise
 */
File.exists = function exists(path)
{
	var fullPath = Jaxer.Dir.resolvePath(path);
	var file = new File(fullPath);
	return file.exists();
};

/**
 * Read the contents of a textfile on local disk, 
 * return an array of lines. When the optional sep parameter is not provided
 * return a string with the lines concatenated by the provided parameter.
 * If the file does not exist, returns a null
 * @alias Jaxer.File.readLines
 * @param {String} path
 * 	The full or partial (to be resolved) path to read
 * @param {String} [sep]
 * 	An optional separator to use between lines. If none is specified, returns an array of lines.
 * @return {Array|String|null} The contents of the file as a string or array of lines, or null if the file does not exist
 */
File.readLines = function read(path,sep)
{
	var fullPath = Jaxer.Dir.resolvePath(path);
	var file = new File(fullPath);
	if (!file.exists())
	{
		return null;
	}
	var data = [];
	try
	{
		file.open('r');
		data = file.readAllLines();
	}
	finally
	{
		if (file)
		{
			file.close();
		}
	}
	return (sep) ? data.join(sep) : data ;
};

/**
 * Writes the provided text to file specified by the path. WARNING - destructive! This will overwrite an existing file
 * so use File.append if you want to add the data to the end of an existing file.
 * @alias Jaxer.File.write
 * @param {String} path
 * 	The full or partial (to be resolved) path to read
 * @param {String} text
 * 	The text to write to the file
 */
File.write = function write(path, text)
{
	var file = File.getOrCreate(path);
	try
	{
		file.open('w');
		for (var i=1; i<arguments.length; i++)
		{
			file.write(arguments[i].toString());
		}
	}
	finally
	{
		if (file)
		{
			file.close();
		}
	}
};

/**
 * add the provided text to the end of an existing file.
 * @alias Jaxer.File.append
 * @param {String} path
 * 	The full or partial (to be resolved) path to append to
 * @param {String} text
 * 	The text to append
 */
File.append = function append(path, text)
{
	var file = File.getOrCreate(path);
	try
	{
		file.open('a');
		for (var i=1; i<arguments.length; i++)
		{
			file.write(arguments[i].toString());
		}
	}
	finally
	{
		if (file)
		{
			file.close();
		}
	}
};

/**
 * add a line to the end of an existing file.
 * @alias Jaxer.File.appendLine
 * @param {String} path
 * 	The full or partial (to be resolved) path to append to
 * @param {String} text
 * 	The text to append, as a new line
 */
File.appendLine = function appendLine(path, text)
{
	var args = new Array((arguments.length - 1) * 2);
	for (var i=1; i<arguments.length; i++)
	{
		args[(i - 1) * 2    ] = arguments[i];
		args[(i - 1) * 2 + 1] = "\n";
	}
	args.unshift(path);
	File.append.apply(null, args);
};

/**
 * get a file object, and if the object doesn't exist then automagically create it.
 * @alias File.getOrCreate
 * @param {String} path
 * 	The full or partial (to be resolved) path to get or create
 * @return {File} The file, possibly newly-created
 */
File.getOrCreate = function getOrCreate(path)
{
	var fullPath = Jaxer.Dir.resolvePath(path);
	var file = new File(fullPath);
	if (!file.exists())
	{
		file.create();
	}
	return file;
};

/**
 * delete a file (only if it already exists).
 * @alias File.remove
 * @param {String} path
 * 	The full or partial (to be resolved) path to delete
 */
File.remove = function remove(path)
{
	var fullPath = Jaxer.Dir.resolvePath(path);
	var file = new File(fullPath);
	if (file.exists())
	{
		file.remove();
	}
};

Jaxer.File = File;

Log.trace("*** File.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/IO > Dir.js
 */
coreTraceMethods.TRACE('Loading fragment: Dir.js');
(function() {

var fileUtils = new Jaxer.FileUtils();
var dirUtils = new Jaxer.DirUtils();
var networkUtils = new Jaxer.NetworkUtils();
var fileProtocolHandler = Components.classes["@mozilla.org/network/protocol;1?name=file"].createInstance(Components.interfaces.nsIFileProtocolHandler);
// Not used yet: var resourceProtocolHandler = Components.classes["@mozilla.org/network/protocol;1?name=resource"].createInstance(Components.interfaces.nsIResProtocolHandler);

/**
 * convert a URL in string format to a filesystem path.
 * @alias Jaxer.urlToPath
 * @param {String} url
 * 	The URL to convert
 * @return {String|null} the full path, or null if the URL could not be converted
 */
Dir.urlToPath = function urlToPath(url)
{
	var candidate = fileUtils.urlToPath(url);
	return (typeof candidate == "string") ? candidate : null;
}

/**
 * convert a filesystem path to a URL format
 * @alias Jaxer.pathToUrl
 * @param {String} path
 * 	The full path to be converted
 * @return {String|null} the URL, or null if the path could not be converted
 */
Dir.pathToUrl = function pathToUrl(path)
{
	var candidate = fileUtils.pathToURL(path);
	return (typeof candidate == "string") ? candidate : null;
}

/**
 * Combines any number of path fragments into a single path, using the 
 * current operating system's filesystem path separator.
 * Before joining two fragments with the path separator, it strips any
 * existing path separators on the fragment ends to be joined
 * @alias Jaxer.combine
 * @param {String} ... 
 * 	Takes any number of string path fragments
 * @return {String} The fragments joined into a path
 */
Dir.combine = function combine()
{
	if (arguments.length == 0) return '';
	if (arguments.length == 1) return arguments[0];
	var sep = Jaxer.System.separator;
	var stripRight = new RegExp("\\" + sep + "+$");
	var stripLeft  = new RegExp("^\\" + sep + "+");
	var stripBoth  = new RegExp("^\\" + sep + "|\\" + sep + "$", 'g');
	pieces = [];
	pieces.push(arguments[0].replace(stripRight, ''));
	for (var i=1; i<pieces.length-1; i++)
	{
		pieces[i] = pieces[i].replace(stripBoth, '');
	}
	pieces.push(arguments[arguments.length - 1].replace(stripLeft, ''));
	return pieces.join(sep);
}

var resourcePattern = /^resource\:\/\//i;
var filePattern = /^file\:\/\//i;

/**
 * Resolves a path to an absolute path on the filesystem, using as a reference (base) 
 * the given path or the current page's path.
 * @alias Jaxer.Dir.resolvePath
 * @param {String} pathToResolve
 * 	The path to resolve, e.g. a filename. It can also be a resource pattern (e.g. "resource:///...") 
 * 	or a file pattern (e.g. "file:///...")
 * @param {String} [referencePath]
 * 	An optional path to use as a reference.
 * @return {String} The full path on the filesystem
 */
Dir.resolvePath = function resolvePath(pathToResolve, referencePath)
{
	var resolvedPath;
	var urlToResolve = pathToResolve.replace(/\\/g, '/');
	if (urlToResolve.match(resourcePattern))
	{
		resolvedPath = urlToResolve.replace(resourcePattern, System.executableFolder);
		resolvedPath = resolvedPath.replace(/\//g, System.separator);
		return resolvedPath;
	}
	var fullyResolvedUrl;
	if (urlToResolve.match(filePattern)) 
	{
		fullyResolvedUrl = urlToResolve;
	}
	else 
	{
		var referenceUrl;
		if (referencePath) 
		{
			referenceUrl = Dir.pathToUrl(referencePath);
		}
		else if (Jaxer.request && Jaxer.request.pageFile)
		{
			referenceUrl = 'file://' + Jaxer.request.pageFile;
		}
		else
		{
			throw new Exception("Could not resolve path '" + pathToResolve + "' with neither a referencePath nor a request and a request.pageFile");
		}
		var resolvedUrl = Dir.resolveUrl(urlToResolve, referenceUrl);
		var fullyResolvedUrl = Util.Url.ensureFileProtocol(resolvedUrl);
	}
	resolvedPath = fileProtocolHandler.getFileFromURLSpec(fullyResolvedUrl).path;
	return resolvedPath;
}

/**
 * Resolves a URL fragment to a full URL, using as a reference (base) the given URL or the current page's URL.
 * @alias Jaxer.Dir.resolveUrl
 * @param {String} urlToResolve
 * 	The URL to resolve, e.g. a pagename.
 * @param {String} [referenceUrl]
 * 	An optional URL to use as a reference
 * @return {String} The fully-resolved URL
 */
Dir.resolveUrl = function resolveUrl(urlToResolve, referenceUrl)
{
	if (!referenceUrl) 
	{
		referenceUrl = Web.getDefaultReferenceUrl();
	}
	var currentUriObj = networkUtils.fixupURI(referenceUrl);
	var resolvedUrl = currentUriObj.resolve(urlToResolve);
	return resolvedUrl;
}

/**
 * Does the directory exist on disk?
 * @alias Dir.exists
 * @param {String} path
 * 	The full or partial (to be resolved) path to test
 * @return {Boolean} true if exists, false otherwise
 */
Dir.exists = function exists(path)
{
	var fullPath = Dir.resolvePath(path);
	var dir = new Dir(fullPath);
	return dir.exists();
};

Jaxer.Dir = Dir;

Log.trace("*** Dir.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/IO > Includer.js
 */
coreTraceMethods.TRACE('Loading fragment: Includer.js');
(function(){

var log = Log.forModule("Includer"); // Only if Log itself is defined at this point of the includes

var Includer = {};

var INCLUDE_TAG = "jaxer:include";
var INCLUDE_JS_TAG = "jaxer:includeJS";

/**
 * Replaces the element with the contents of its source or path
 * @alias Jaxer.Includer.includeElement
 * @private
 * @param {DocumentElement} doc
 * 	The document of the element
 * @param {HTMLElement} includeTag
 * 	The HTML element to replace
 * @return {Boolean} true if successful, false otherwise
 */
Includer.includeElement = function includeElement(doc, includeTag)
{
	log.trace("Entering with includeTag having src = " + includeTag.getAttribute("src") + " and path = " + includeTag.getAttribute("path"));
	// Read in file contents
	var htmlToInclude = "";
	var from;

	var successful = false;
	try
	{

		if (includeTag.getAttribute("path")) // absolute path on the filesystem was given
		{
			from = includeTag.getAttribute("path");
			htmlToInclude = File.read(from);
		}
		else if (includeTag.getAttribute("src")) // url-type path given, either of file type or web type
		{
			from = includeTag.getAttribute("src");
			requestHeaders = []; // [['X-Jaxer-Passthrough', 'document.body.innerHTML']]
			htmlToInclude = Web.get(from, true, requestHeaders);
		}
		else
		{
			log.warn("No src or path attributes were given for this includeTag");
		}
		
		if (from) 
		{
			if (htmlToInclude.length == 0) 
			{
				log.warn("No content was read from included file at " + from);
			}
			else 
			{
				log.debug("Successfully included file at " + from);
			}
			successful = true;
		}
	}
	catch (e)
	{
		log.error("Error trying to include file '" + path + "': " + e);
	}
	
	if (includeTag.tagName.toLowerCase() == INCLUDE_JS_TAG.toLowerCase())
	{
		var includeTagRunAt = includeTag.getAttribute(RUNAT_ATTR);
		var runat = includeTagRunAt || RUNAT_SERVER_NO_CACHE; 
		htmlToInclude = "<script type='text/javascript' " + RUNAT_ATTR + "='" + runat + "'>\n" + htmlToInclude + "\n</script>\n";
	}
	
	// Replace includeTag with the parsed HTML
	var range = doc.createRange();
	range.setStartBefore(includeTag);
	var parsed = range.createContextualFragment(htmlToInclude);
	includeTag.parentNode.insertBefore(parsed, includeTag);
	includeTag.parentNode.removeChild(includeTag);

	return successful;
}

// TODO: Improve this with Brendan's suggestion

/**
 * Evaluate the given JavaScript string on the given global context
 * @alias Jaxer.Includer.evalOn
 * @param {String} source
 * 	The string of script code to evaluate
 * @param {Object} global
 * 	The global (usually a window object) on which to evaluate it
 */
Includer.evalOn = function evalOn(source, global)
{
	if (!global) throw new Exception("There is no global window on which to evaluate the script");
	if (typeof global.__eval == "function") 
	{
		global.__eval(source);
	}
	else 
	{
		var tempPath = Dir.combine(System.tempFolder, "_script_" + (new Date().getTime()) + Math.random());
		var tempUrl = Dir.pathToUrl(tempPath);
		File.write(tempPath, source);
		try 
		{
			include(tempUrl, global);
		}
		finally 
		{
			File.remove(tempPath);
		}
	}
}

/**
 * Loads and evaluates a JavaScript file on the given global execution object with the given runat attribute.
 * @alias Jaxer.Includer.load
 * @private 
 * @param {String} src
 * 	Where the JavaScript file should be retrieved from
 * @param {Boolean} global
 * 	The global (usually a window object) on which to evaluate it
 */
Includer.load = function load(src, global, runat)
{
	if (!global && load.caller) global = load.caller.__parent__;
	runat = runat || Jaxer.lastScriptRunat;
	var functionsBefore;
	var shouldRunat = function shouldRunat(name, value)
	{
		return (typeof value == "function" && typeof value[RUNAT_ATTR] != "string");
	}
	if (runat && runat.match(RUNAT_ANY_SERVER_REGEXP))
	{
		functionsBefore = Util.getPropertyNames(global, shouldRunat, true);
	}
	if (Util.Url.isFile(src))
	{
		var url = Dir.pathToUrl(Dir.resolvePath(src));
		include(url, global);
	}
	else
	{
		var content = Web.get(src);
		Includer.evalOn(content, global);
	}
	if (functionsBefore) 
	{
		var functionsAfter = Util.getPropertyNames(global, shouldRunat, true);
		for (var p in functionsAfter)
		{
			if (!functionsBefore[p] &&
				!Util.isNativeFunction(global[p]))
			{
				global[p][RUNAT_ATTR] = runat;
			}
		}
	}
	log.debug("Included: " + url);
}

Jaxer.load = Includer.load;

frameworkGlobal.Includer = Jaxer.Includer = Includer;

Log.trace("*** Includer.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/IO > Web.js
 */
coreTraceMethods.TRACE('Loading fragment: Web.js');
(function() {

var log = Log.forModule("Web");

var Web = {};

var networkUtils = new Jaxer.NetworkUtils();
var xhr = Components.classes["@mozilla.org/xmlextras/xmlhttprequest;1"].createInstance(); // This will be a private, synchronous singleton
xhr.QueryInterface(Components.interfaces.nsIXMLHttpRequest);

/**
 * Resolves a URL to an absolute URL (one starting with protocol://domain...)
 * To resolve a relative URL (with or without a leading /), use a current document or callback for reference.
 * Uses the base href of the current document, if specified.
 * If Jaxer.Config.REWRITE_RELATIVE_URL exists, its value is used to rewrite the relative URL. Replaces the 
 *  Jaxer.Config.REWRITE_RELATIVE_URL_REGEX pattern, if it exists. Otherwise, replaces the http or https protocol 
 *  followed by the domain name and port.
 * @alias Jaxer.Web.resolve
 * @param {String} url 
 * 	The URL to resolve
 * @param {String} [referenceUrl] 
 * 	An optional reference URL to use, overriding the built-in logic
 * @return {String} The fully-resolved URL, or the original URL if the input was already an absolute URL
 */
Web.resolve = function resolve(url, referenceUrl)
{
	if (url.match(/^\w+\:/)) // absolute URL with protocol
	{
		return url;
	}
	else // 
	{
		if (!referenceUrl) 
		{
			referenceUrl = Web.getDefaultReferenceUrl();
		}
		var currentUriObj = networkUtils.fixupURI(referenceUrl);
		var resolvedUrl = currentUriObj.resolve(url);
		if (Jaxer.Config.REWRITE_RELATIVE_URL)
		{
			var regex;
			if (Jaxer.Config.REWRITE_RELATIVE_URL_REGEX)
			{
				regex = (typeof Jaxer.Config.REWRITE_RELATIVE_URL_REGEX == "string") ? new RegExp(Jaxer.Config.REWRITE_RELATIVE_URL_REGEX) : Jaxer.Config.REWRITE_RELATIVE_URL_REGEX;
			}
			else
			{
				regex = /^http[s]?\:\/\/[^\/]+/;
			} 
			resolvedUrl = resolvedUrl.replace(regex, Jaxer.Config.REWRITE_RELATIVE_URL);
		}
		return resolvedUrl;
	}
}

/**
 * Returns the URL to be used as a reference for resolving relative URLs if no other reference is given
 * @alias Jaxer.Web.getDefaultReferenceUrl
 * @return {String}	The absolute URL
 */
Web.getDefaultReferenceUrl = function getDefaultReferenceUrl()
{
	if (!Jaxer.pageWindow || !Jaxer.pageWindow.document || !Jaxer.request || !Jaxer.request.parsedUrl) 
	{
		throw new Exception("No default reference URL could be constructed; cannot use relative URLs outside the context of a page or callback");
	}
	var baseElt = Jaxer.pageWindow.document.getElementsByTagName('base')[0];
	if (baseElt && baseElt.href && (baseElt.href != '')) 
	{
		referenceUrl = baseElt.href;
	}
	else 
	{
		referenceUrl = Jaxer.request.parsedUrl.url;
	}
	return referenceUrl;
}

/**
 * A generalized method to synchronously access a web URL via the built-in XMLHttpRequest object.
 * @alias Jaxer.Web.send
 * @param {String} url 
 * 	The url to access
 * @param {String} [method] 
 * 	Usually 'GET' (default) or 'POST'
 * @param {String|Object} [data] 
 * 	Use for POST submissions. This may be a string, or an object whose name->value pairs will be used as the query string
 * @param {Array} [requestHeaders] 
 * 	Optional array of [headerName, headerValue] headers to use as headers on the request.
 *	If the method is POST, a header of ['Content-Type', 'application/x-www-form-urlencoded'] will be added unless
 *	another Content-Type header is specified explicitly.
 * @param {String} [mimeType] 
 * 	If specified, use this to force the response to be handled as a particular content type.
 * @param {Array} [responseHeaders] 
 * 	An optional array of header names to fetch from the response.
 * @return {Object} 
 * 	An object having the properties 'text' (the responseText), 
 * 'xml' (the responseXML as an XmlDocument, if any), 
 * and 'headers' if any responseHeaders were requested.
 * If there was no response, or the response status code was < 200 or >= 400, an Exception is thrown. 
 */
Web.send = function send(url, method, data, requestHeaders, mimeType, responseHeaders)
{
	if (!url) 
	{
		throw new Exception("No URL specified");
	}
	if (typeof method != "string") data = "GET";
	method = method || "GET";
	if ((data != null) && (typeof data != "string")) 
	{
		data = Util.Url.hashToQuery(data);
	}
	if (method == "POST") // Needs the Content-Type set, unless the user overrode it for some reason
	{
		if (!requestHeaders) requestHeaders = [];
		var isContentTypeSet = false;
		for (var i=0; i<requestHeaders.length; i++)
		{
			if (header[0].match(/^\s*content\-type\s*$/i)) 
			{
				isContentTypeSet = true;
				break;
			}
		}
		if (!isContentTypeSet)
		{
			requestHeaders.push(['Content-Type', 'application/x-www-form-urlencoded']);
		}
	}
	log.trace("Parameters are: url: " + url + "; method: " + method + "; data: " + data + "; requestHeaders: " + requestHeaders + "; mimeType: " + mimeType + "; responseHeaders: " + responseHeaders);
	var resolvedUrl = Web.resolve(url);
	log.trace("Resolved URL to: " + resolvedUrl);
	xhr.open(method, resolvedUrl, false); // Synchronous only
	log.trace("Opened xhr");
	if (requestHeaders)
	{
		Util.foreach(requestHeaders, function setHeader(header)
		{
			xhr.setRequestHeader(header[0], header[1]);
		});
	}
	if (mimeType)
	{
		xhr.overrideMimeType(mimeType);
	}
	try
	{
		log.trace("Sending request");
		xhr.send(data);
		log.trace("Request sent");
	}
	catch(e)
	{
		throw new Exception(e);
	}
	log.trace("Received status: " + xhr.status);
	if (xhr.status < 200 || xhr.status >= 400) // All 20x and 30x are considered OK
	{
		throw new Exception("Received status " + xhr.status + " from " + resolvedUrl + ": " + xhr.statusText);
	}
	var result = {text: xhr.responseText, xml: xhr.responseXML};
	log.trace("Received " + (result.text ? result.text.length : 0) + " characters of text and " + (result.xml ? result.xml.length : 0) + " characters of XML");
	if (responseHeaders)
	{
		var headers = {};
		Util.foreach(responseHeaders, function getHeader(header)
		{
			headers[header] = xhr.GetResponseHeader(header);
		});
		result.headers = headers;
		log.trace("Requested headers: " + uneval(headers));
	}
	return result;
}

/**
 * Fetch a document from a URL by resolving it to a local file (if it starts with file://) or by a GET command.
 * @alias Jaxer.Web.get
 * @param {String} url 
 * 	The URL to fetch, which may be a file:// URL if desired. This will first be resolved by Dir.resolvePath() or Web.resolve().
 * @param {Boolean} [insertCacheBuster] 
 * 	If true, inserts a cache-busting random query string into the URL.
 * @param {Array} [requestHeaders] 
 * 	Optional array of [headerName, headerValue] headers to use as headers on the request.
 *	If the method is POST, a header of ['Content-Type', 'application/x-www-form-urlencoded'] will be added unless
 *	another Content-Type header is specified explicitly.
 * @param {String} [mimeType] 
 * 	If specified, use this to force the response to be handled as a particular content type.
 * @return {String} The text of the requested document. An Exception is thrown if not successful.
 */
Web.get = function get(url, insertCacheBuster, requestHeaders, mimeType)
{
	var content;
	if (Util.Url.isFile(url)) 
	{
		try 
		{
			log.trace("Retrieving from file URL: " + url);
			var path = Dir.resolvePath(url);
			log.trace("Resolved to path: " + path);
			content = File.read(path);
			log.trace("Read " + (content ? content.length : 0) + " characters from file");
		} 
		catch (e) 
		{
			throw new Exception("Could not read file from url='" + url + "' (as '" + path + "'): " + e);
		}
	}
	else
	{
		if (url && insertCacheBuster)
		{
			url = [url, (url.match(/\?/) ? '&' : '?'),"_rnd", new Date().getTime(), "=", Math.round(Math.random() * 1000000)].join('');
		}
		content = Web.send(url, "GET", null, requestHeaders, mimeType).text;
	}
	return content;
}

/**
 * POST data to a URL and return the response web page.
 * @alias Jaxer.Web.post
 * @param {String} url 
 * 	The URL of the page to POST to and fetch. This will first be resolved by Web.resolve().
 * @param {String|Object} data 
 * 	The data to submit. 
 *	If a string, it should be a query string in a format (name1=value1&name2=value2) suitable for a Content-Type of 'application/x-www-form-urlencoded'. 
 *	If an object, its enumerable properties will be used to construct the query string.
 * @return {String} The text of the response web page. An Exception is thrown if not successful.
 */
Web.post = function post(url, data)
{
	return Web.send(url, "POST", data).text;
}

frameworkGlobal.Web = Jaxer.Web = Web;

Log.trace("*** Web.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/IO > Socket.js
 */
coreTraceMethods.TRACE('Loading fragment: Socket.js');
/**
 * @author Paul Colton
 */
(function() {

var log = Log.forModule("Socket");

/**
 * The constructor of a network socket object
 * @alias Jaxer.socket
 * @constructor
 */
function Socket()
{
	this._transportService = 
		Components.classes["@mozilla.org/network/socket-transport-service;1"].
			getService(Components.interfaces.nsISocketTransportService);	
}

/**
 * Open the socket for communication
 * @alias Jaxer.Socket.prototype.open
 * @param {String} host
 * 	The host to connect to
 * @param {Number} port
 * 	The port on which to connect
 */
Socket.prototype.open = function(host, port)
{
	
	this._transport = this._transportService.createTransport(null,0,host,port,null);

	if (!this._transport) {
		throw ("Cannot connect to server '" + host + ":" + port);
	}
	
	this._transport.setTimeout(Components.interfaces.nsISocketTransport.TIMEOUT_READ_WRITE, 5);
	
	// Set up output stream
	
	this._ostream = this._transport.openOutputStream(Components.interfaces.nsITransport.OPEN_BLOCKING,0,0);		
	
	this._outcharstream = Components.classes["@mozilla.org/intl/converter-output-stream;1"].
		createInstance(Components.interfaces.nsIConverterOutputStream);
	this._outcharstream.init(this._ostream, "ISO8859-1", 80, 0x0);

	this._outstream = Components.classes["@mozilla.org/scriptableoutputstream;1"].
		createInstance(Components.interfaces.nsIScriptableIOOutputStream);
	this._outstream.initWithStreams(this._ostream, this._outcharstream);

	// Set up input stream
	
	this._istream = this._transport.openInputStream(Components.interfaces.nsITransport.OPEN_BLOCKING,0,0);
		
	this._incharstream = Components.classes["@mozilla.org/intl/converter-input-stream;1"].
		createInstance(Components.interfaces.nsIConverterInputStream);
	this._incharstream.init(this._istream, "ISO8859-1", 80, 0x0);

	this._instream = Components.classes["@mozilla.org/scriptableinputstream;1"].
		createInstance(Components.interfaces.nsIScriptableIOInputStream);
	this._instream.initWithStreams(this._istream, this._incharstream);
};

/**
 * Close the socket
 * @alias Jaxer.Socket.prototype.close
 */
Socket.prototype.close = function() 
{
	try {
		this._instream.close();
		this._outstream.close();
    }
	catch(e) {
		// ignore this exception, we're just trying to close this socket down
	}
	
	this._transport.close(0);
};

/**
 * Write a string to the socket
 * @alias Jaxer.Socket.prototype.writeString
 * @param {String} data
 * 	The text to write
 */
Socket.prototype.writeString = function(data) 
{
	this._outstream.writeString(data);
};

/**
 * Read a single line from the socket
 * @alias Jaxer.Socket.prototype.readLine
 * @return {String}	The text read in
 */
Socket.prototype.readLine = function()
{
	return this._instream.readLine();
};

/**
 * Read characters from the socket into a string
 * @alias Jaxer.Socket.prototype.readString
 * @param {Number} count
 * 	How many characters to read
 * @return {String}	The text read in
 */
Socket.prototype.readString = function(count)
{
	return this._instream.readString(count);
};

/**
 * Is the socket connection available?
 * @alias Jaxer.Socket.prototype.available
 * @return {String}	true if available, false otherwise
 */
Socket.prototype.available = function()
{
	return this._instream.available();
};

/**
 * Flush the socket's output stream
 * @alias Jaxer.Socket.prototype.flush
 */
Socket.prototype.flush = function()
{
	this._outstream.flush();
};

frameworkGlobal.Socket = Jaxer.Socket = Socket;

Log.trace("*** Socket.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/IO > SMTP.js
 */
coreTraceMethods.TRACE('Loading fragment: SMTP.js');
(function(){

var log = Log.forModule("SMTP");

var SMTP = {};

SMTP.DEFAULT_PORT = 25;

/**
 * Sends an email via SMTP
 * @alias Jaxer.SMTP.sendEmail
 * @param {String} mailhost
 * 	The host to connect to
 * @param {Number} mailport
 * 	The port to connect on
 * @param {String} from
 * 	The address this message is coming from
 * @param {String} to
 * 	The address this message is going to
 * @param {String} subject
 * 	The subject of the message
 * @param {String} msg
 * 	The body of the message
 */
SMTP.sendEmail = function sendEmail(mailhost, mailport, from, to, subject, msg)
{
	var s = new Jaxer.Socket();
	s.open(mailhost, mailport);
	
	// Read in initial welcome string
	res = s.readLine();
	
	if (Jaxer.Util.String.startsWith(res, "220") == false) 
	{
		throw new Exception("SMTP: introduction resulted in: " + res);
	}
	
	s.writeString("HELO " + "localhost\r\n");
	s.flush();
	res = s.readLine();
	
	if (Jaxer.Util.String.startsWith(res, "250") == false) 
	{
		throw new Exception("SMTP: expected 250 return code when sending HELO, received: " + res);
	}
	
	s.writeString("MAIL FROM: " + from + "\r\n");
	s.flush();
	res = s.readLine();
	
	if (Jaxer.Util.String.startsWith(res, "250") == false) 
	{
		throw new Exception("SMTP: expected 250 return code when sending MAIL FROM, received: " + res);
	}
	
	if (to.constructor == to.__parent__.Array) 
	{
		for (var i = 0; i < to.length; i++) 
		{
			s.writeString("RCPT TO: " + to[i] + "\r\n");
			s.flush();
			res = s.readLine();
			
			if (Jaxer.Util.String.startsWith(res, "250") == false) 
			{
				throw new Exception("SMTP: expected 250 return code when sending RCPT TO, received: " + res);
			}
		}
	}
	else 
	{
		s.writeString("RCPT TO: " + to + "\r\n");
		s.flush();
		res = s.readLine();
		
		if (Jaxer.Util.String.startsWith(res, "250") == false) 
		{
			throw new Exception("SMTP: expected 250 return code when sending RCPT TO, received: " + res);
		}
	}
	
	s.writeString("DATA\r\n");
	s.flush();
	res = s.readLine();
	
	if (Jaxer.Util.String.startsWith(res, "354") == false) 
	{
		throw new Exception("SMTP: expected 354 return code when sending DATA, received: " + res);
	}
	
	s.writeString("Subject:" + subject + "\r\n");
	s.writeString("\r\n");
	
	var msgLines = msg.split('\n');
	for (var i = 0; i < msgLines.length; i++) 
	{
		var line = msgLines[i];
		if (Jaxer.Util.String.startsWith(line, ".")) 
		{
			line = "." + line;
		}
		s.writeString(line + "\n");
	}
	
	s.writeString("\r\n.\r\n");
	s.flush();
	res = s.readLine();
	
	if (Jaxer.Util.String.startsWith(res, "250") == false) 
	{
		throw new Exception("SMTP: expected 250 return code when closing conversation, received: " + res);
	}
	
	s.writeString("QUIT\r\n");
	s.flush();
	s.close();
}

/**
 * Sends an email message object via SMTP
 * @alias Jaxer.SMTP.sendMessage
 * @param {String} mailhost
 * 	The host to connect to
 * @param {Number} mailport
 * 	The port to connect on
 * @param {MailMessage} mailMessage
 * 	The Jaxer.SMTP.MailMessage object to send
 */
SMTP.sendMessage = function sendMessage(mailhost, mailport, mailMessage)
{
	SMTP.sendEmail(mailhost, mailport, mailMessage._from, mailMessage._recipients, mailMessage._subject, mailMessage._body);
}

/**
 * A structure holding email message information
 * @alias Jaxer.MailMessage
 * @constructor
 */
function MailMessage()
{
	this._date = new Date();
	this._from = "";
	this._recipients = [];
	this._subject = "";
	this._body = "";
}

/**
 * Adds a recipient to the message
 * @alias Jaxer.MailMessage.prototype.addRecipient
 * @param {String} recipient
 * 	The email address
 */
MailMessage.prototype.addRecipient = function addRecipient(recipient)
{
	this._recipients.push(recipient);
};

/**
 * Sets the "From" address on the message
 * @alias Jaxer.MailMessage.prototype.setFrom
 * @param {String} from
 * 	The email address from which this message is coming
 */
MailMessage.prototype.setFrom = function setFrom(from)
{
	this._from = from;
};

/**
 * Sets the "Subject" of the message
 * @alias Jaxer.MailMessage.prototype.setSubject
 * @param {String} subject
 * 	The subject text
 */
MailMessage.prototype.setSubject = function setSubject(subject)
{
	this._subject = subject;
};

/** 
 * Sets the timestamp on the message
 * @alias Jaxer.MailMessage.prototype.setDate
 * @param {Object} date
 * 	The date it's sent
 */
MailMessage.prototype.setDate = function setDate(date)
{
	this._date = date;
};

/**
 * Sets the body (contents) of the message
 * @alias Jaxer.MailMessage.prototype.setBody
 * @param {String} body
 * 	The text of the message
 */
MailMessage.prototype.setBody = function setBody(body)
{
	this._body = body;
};

SMTP.MailMessage = MailMessage;
frameworkGlobal.SMTP = Jaxer.SMTP = SMTP;

Log.trace("*** SMTP.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Exception > Exception.js
 */
coreTraceMethods.TRACE('Loading fragment: Exception.js');
(function() {

/**
 * The exception used by the Jaxer framework. At runtime this is available from the window object
 * @alias Jaxer.Exception
 * @constructor
 * @param {String, Error} info
 * 	The descriptive text of the Exception to be thrown, or an Error-derived object
 * @param {Object} [logger]
 * 	An optional reference to an instance of the framework logger to be used. A default setting is used if this parameter is omitted
 * @return {Exception} an instance of Exception
 */
var Exception = function Exception(info, logger)
{
	this.name = "Exception";
	var infoIsError = (info instanceof Error);
	this.innerErrorName = infoIsError ? info.name : null;
	var error;
	if (infoIsError)
	{
		error = info;
	}
	else
	{
		error = new Error((info == null || info == "" || (typeof info.toString != "function")) ? "(unknown error)" : info.toString());
		if (info != null && typeof info == "object") // try to extract more information from info
		{
			for (var p in info)
			{
				error[p] = info[p];
			}
		}
	}
	var details = [];
	for (var p in error)
	{
		var val = error[p];
		var type = typeof val;
		if (val != null &&
			type != "undefined" &&
			type != "function" &&
			typeof val.toString == "function")
		{
			this[p] = error[p];
			var sep = (p.indexOf("\n") > -1) ? "\n" : " ";
			details.push(p + ":" + sep + val.toString());
		}
	}
	this.details = details.join("\n");
	this.description = this.message;
	logger = logger || Log.genericLogger;
	logger.error(this.details, null, this.constructor.caller);
};

Exception.prototype = new Error();
Exception.prototype.constructor = Exception;

/**
 * Provides a string representation of the Exception description.
 * @alias Jaxer.Exception.prototype.toString
 * @method
 * @return {String}	A description of the exception
 */
Exception.prototype.toString = function toString()
{
	return "Exception: " + this.description;
};

// Static utility functions
/**
 * Returns a JavaScript Error (or Error-derived) object based on the given object
 * @alias Jaxer.Exception.toError
 * @method
 * @param {Object} obj
 * 		If this is already derived from an Error, it will just be returned.
 * 		Otherwise it will be stringified and used as the description of the error.
 * @return {Error} The Error-derived representation
 */
Exception.toError = function toError(obj)
{
	if (typeof obj == "undefined") 
	{
		return new Error("undefined");
	}
	else if (obj instanceof Error || obj instanceof Exception) 
	{
		return obj;
	}
	else if (typeof obj.toString == "function") 
	{
		return new Error(obj.toString());
	}
	else 
	{
		return new Error("Cannot convert error object to text message");
	}
};

var detailProperties = ["message", "innerErrorName", "fileName", "lineNumber", "stack"];

/**
 * Get verbose details on the error
 * @alias Jaxer.Exception.toDetails
 * @param {Object} obj
 * 	The error object on which details are desired. An attempt is made to convert it into an Error-derived object before details are retrieved.
 * @return {String} The detailed description
 */
Exception.toDetails = function toDetails(obj)
{
	var error = Exception.toError(obj);
	var details = [];
	Util.foreach(detailProperties, function(prop)
	{
		if (typeof error[prop] != "undefined")
		{
			var val = error[prop];
			if (val != null && val != "")
			{
				details.push([prop + ": ", val].join(prop == "stack" ? "\n" : ""));
			}
		}
	});
	return details.join("\n");
}

frameworkGlobal.Exception = Jaxer.Exception = Exception;

Jaxer.Log.trace("*** Exception.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/DB > DB.js
 */
coreTraceMethods.TRACE('Loading fragment: DB.js');
(function(){

var log = Log.forModule("DB"); // Only if Log itself is defined at this point of the includes

/**
 * @namespace {Jaxer.DB}
 * @private
 * 
 */

var defaultNames = ["FRAMEWORK", "DEVELOPER"];
var DB =
{
	defaultImplementations: {},
	defaultConnections: {}
};
for (var i = 0; i < defaultNames.length; i++)
{
	var name = defaultNames[i];
	DB[name] = name;
	DB.defaultImplementations[name] = null;
	DB.defaultConnections[name] = null;
}

/**
 * Returns a boolean value to indicate whether the referenced database is the framwork DB or the developer DB
 * 
 * @example
 * 
 *	var name = isFramework ? DB.FRAMEWORK : DB.DEVELOPER;
 * @private  
 * @alias Jaxer.DB.isSpecified
 * @param {Boolean} isFramework 
 * 	indicates which DB (true : framework , false : developer)
 * @return {Boolean} Whether the given default database's implementation and connection paramaters have been specified
 */
DB.isSpecified = function isSpecified(isFramework)
{
	var props = isFramework ? 
		["DB_FRAMEWORK_IMPLEMENTATION", "DB_FRAMEWORK_CONNECTION_PARAMS"] :
		["DB_IMPLEMENTATION", "DB_CONNECTION_PARAMS"];
	return Util.hasProperties(Config, props);
};

/**
 * The default implementation of execute if there is no connection: just throws an exception
 * @private
 * @alias Jaxer.DB.executeWithoutConnection
 */
DB.executeWithoutConnection = function executeWithoutConnection()
{
	throw new Exception("Attempted to execute SQL query without having a valid default connection", log);
};

DB.execute = DB.frameworkExecute = DB.executeWithoutConnection; // The default value, before initialization

/**
 * The default implementation of a connection if there is no implementation defined: just throws an exception
 * @private
 * @alias Jaxer.DB.connectionWithoutImplementation
 */
DB.connectionWithoutImplementation = function connectionWithoutImplementation()
{
	throw new Exception("Attempted to create a new Connection without having a valid default DB implementation", log);
};

DB.Connection = DB.FrameworkConnection = DB.connectionWithoutImplementation; // The default value, before initialization

/**
 * Initializes the default database implementations for the framework and optionally for the developer 
 * @private  
 * @alias DB.init
 * @param {Boolean} isFramework
 * @param {Object} impl
 * @param {Object} conn
 */
DB.init = function init(isFramework, impl, conn)
{
	var name = isFramework ? DB.FRAMEWORK : DB.DEVELOPER;
	
	this.defaultImplementations[name] = impl;
	this.defaultConnections[name] = conn;
	
	var Conn = isFramework ? "FrameworkConnection" : "Connection";
	var exec = isFramework ? "frameworkExecute" : "execute";
	
	if (this.defaultImplementations[name])
	{
		this[Conn] = this.defaultImplementations[name].Connection; // This is the constructor for the current implementation's Connection
	}
	else
	{
		this[Conn] = DB.connectionWithoutImplementation;
	}

	if (this.defaultConnections[name])
	{
		this[exec] = function() { return this.defaultConnections[name].execute.apply(this.defaultConnections[name], arguments)};
	}
	else
	{
		this[exec] = DB.executeWithoutConnection;
	}
};

frameworkGlobal.DB = Jaxer.DB = DB;

Log.trace("*** DB.js loaded");  // Only if Log itself is defined at this point of the includes

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/DB > ResultSet.js
 */
coreTraceMethods.TRACE('Loading fragment: ResultSet.js');
(function(){

var log = Log.forModule("ResultSet"); // Only if Log itself is defined at this point of the includes

/**
 * Returned value of a SQL query, containing the named rows and columns of the result and 
 * to be used as the return value of execute().
 * Each row has a special property, $values, that is an array ordered according to the columns array.
 * The rowsAsArray array is similar to the rows array but each row in it is an array itself: 
 * it is the corresponding $values array. 
 * 
 * To see if there is any data, check whether hasData is true or whether rows.length or rowsAsArrays.length exceed 0.
 * To get a single result (the first column of the first row) grab singleResult.
 * 
 * @example
 * If the third column is named "account" you can get to a given cell value either through
 * resultSet.rows[10].account or through
 * resultSet.rows[10].$values[2] or through
 * resultSet.rowsAsArrays[10][2]
 * 
 * @alias Jaxer.ResultSet
 * @constructor
 */
var ResultSet = function ResultSet()
{
	this.rows = [];
	this.rowsAsArrays = [];
	this.columns = [];
	this.singleResult = null;
	this.hasData = false;
};

/**
 * Returns the index of the column with the given name
 * @alias Jaxer.ResultSet.prototype.indexOfColumn
 * @param {String} columnName the textual name of the database column 
 * @return {Integer} The 0-based index in the columns array (and in each row in the rows array)
 */
ResultSet.prototype.indexOfColumn = function indexOfColumn(columnName)
{
	return this.columns.indexOf(columnName);
};

/**
 * Returns an array of objects, one per row, that only have properties corresponding
 * to the given columns.
 * @alias Jaxer.ResultSet.prototype.extractColumns
 * @param {Array} columns The names of the columns that should be made available for each row.
 * @return {Array} An array of simple objects, each with the requested properties.
 */
ResultSet.prototype.extractColumns = function extractColumns(columns)
{
	var result = [];
	if (columns == "*") columns = this.columns;
	for (var i=0; i<this.rows.length; i++)
	{
		var obj = {};
		for (var ip=0; ip<columns.length; ip++)
		{
			var column = columns[ip];
			obj[column] = this.rows[i][column];
		}
		result.push(obj);
	}
	return result;
};

/**
 * Returns a string representation of the resultset object
 * @alias Jaxer.ResultSet.prototype.toString
 * @return {String} a string representation of the resultset object
 */
ResultSet.prototype.toString = function toString()
{
	if (!this.hasData) return "(empty)";
	var columnSeparator = " | ";
	var lineSeparator = "\n";
	var lines = [];
	lines.push("Columns: " + this.columns.join(columnSeparator));
	for (var i=0; i<this.rowsAsArrays.length; i++)
	{
		lines.push("Row " + i + ": " + this.rowsAsArrays[0].join(columnSeparator));
	}
	return lines.join(lineSeparator);
};

/**
 * Returns a HTML table snippet containing the resultset items
 * @alias Jaxer.ResultSet.prototype.toHTML
 * @param {Object} tableAttributes
 * 	A hashmap which will be turned into attribute-value pairs on the table tag
 * @param {Object} headingAttributes
 * 	A hashmap which will be turned into attribute-value pairs on the thead tag
 * @param {Object} bodyAttributes
 * 	A hashmap which will be turned into attribute-value pairs on the tbody tag
 * @return {String} HTML table snippet containing the resultset items
 */
ResultSet.prototype.toHTML = function toHTML(tableAttributes, headingAttributes, bodyAttributes)
{
	var lines = [];
	lines.push("<table " + Util.DOM.hashToAttributesString(tableAttributes) + ">");
	lines.push("\t<thead " + Util.DOM.hashToAttributesString(headingAttributes) + ">");
	lines.push("\t\t<tr>");
	for (var i=0; i<this.columns.length; i++)
	{
		lines.push("\t\t\t<td>" + this.columns[i] + "</td>");
	}
	lines.push("\t\t</tr>");
	lines.push("\t</thead>");
	if (this.hasData)
	{
		lines.push("\t<tbody " + Util.DOM.hashToAttributesString(bodyAttributes) + ">");
		for (var i=0; i<this.rows.length; i++)
		{
			var values = this.rows[i].$values;
			lines.push("\t\t<tr>");
			for (var j=0; j<values.length; j++)
			{
				lines.push("\t\t\t<td>" + values[j] + "</td>");
			}
			lines.push("\t\t</tr>");
		}
		lines.push("\t</tbody>");
	}
	lines.push("</table>");
	return lines.join("\n");
};

DB.ResultSet = ResultSet;

Log.trace("*** ResultSet.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/DB > DB_MySQL.js
 */
coreTraceMethods.TRACE('Loading fragment: DB_MySQL.js');
(function() {

var log = Log.forModule("DB.MySQL");

/**
 * @namespace {Jaxer.MySQL} This is the namespace that holds the MySQL implementation of the Jaxer DB API
 */
var MySQL = {};

var MYSQL50_CLASS_ID		= "@aptana.com/jxMySQL50;1"
var MYSQL50_INTERFACE       = Components.interfaces.jxIMySQL50;

var VALUE_TYPE_NULL		= "null"; 		// Null data type.
var VALUE_TYPE_NUMBER	= "double"; 	// Used for MySQL integer, floating point, etc. data types.
var VALUE_TYPE_DATETIME	= "datetime"; 	// Datetime data type.
var VALUE_TYPE_DATE		= "date"; 		// Date data type.
var VALUE_TYPE_TIME		= "time"; 		// Time data type.
var VALUE_TYPE_TEXT		= "string"; 	// Text data type.
var VALUE_TYPE_BLOB		= "blob"; 		// Blob data type.

// The following are used to fill in the date portions of a JS Date object 
// being created from a MySQL time-type column
MySQL.TIME_COLUMN_YEAR	= 1970;
MySQL.TIME_COLUMN_MONTH	= 0; // January
MySQL.TIME_COLUMN_DAY	= 1;

/**
 * Creates a new database named according to connectionParams.NAME
 * @alias Jaxer.MySQL.createDB
 * @param {Object} connectionParams A hashmap of parameters needed to connect to the database.
 * The properties required of connectionParams are:
 *   HOST: the hostname of the server
 *   USER: the username for authentication
 *   PASS: the password for authentication
 *   NAME: the name of the database to create
 *   PORT: the port on which to connect (defaults to 3306)
 */
MySQL.createDB = function createDB(connectionParams)
{
	var dbName = connectionParams.NAME;
	var conn, sql;
	
	var noNameConnectionParams = {};
	for (var p in connectionParams)
	{
		noNameConnectionParams[p] = connectionParams[p];
	}
	noNameConnectionParams.NAME = '';
	
	conn = new MySQL.Connection(noNameConnectionParams);

	try
	{
		sql = "CREATE DATABASE IF NOT EXISTS `" + dbName + "`";
		conn.open();
		conn.execute(sql);
		conn.close();
	}
	finally
	{
		if (conn) conn.close();
	}
}

/**
 * Creates a new connection to the given databaseName (file).
 * Can be any string value, including an empty string.
 * Creates the specified database if it does not yet exist.
 * The resulting connection object is the only way to interact with the database.
 * @alias Jaxer.MySQL.Connection
 * @constructor
 * @param {Object} connectionParams 
 * 	A hashmap of required parameters to connect to the database. Required properties are HOST (hostname of the server),
 *  USER (username for authentication), PASS (password for authentication), NAME (database name), 
 *  PORT (connection port, default value is "3306"), and CLOSE_AFTER_EXECUTE (whether to close the connection after
 *  each call to execute, default is open).
 */
MySQL.Connection = function Connection(connectionParams)
{
	this.serverName = connectionParams.HOST;
	this.userName = connectionParams.USER;
	this.password = connectionParams.PASS;
	this.databaseName = connectionParams.NAME;
	this.port = connectionParams.PORT || 3306;
	this.isOpen = false;
	this.conn = Components.classes[MYSQL50_CLASS_ID].getService(MYSQL50_INTERFACE);
	this.closeAfterExecute = connectionParams.CLOSE_AFTER_EXECUTE;
};

/**
 * Opens the connection so queries can be executed.
 * This is optional, since if the connection is not open when it's asked to
 * execute some SQL, it will open the connection automatically.
 * Also closing the connection is optional.
 * @alias Jaxer.MySQL.Connection.prototype.open
 */
MySQL.Connection.prototype.open = function open()
{
	if (this.isOpen) return;
	log.debug("Opening database: " + this.databaseName);
	var success = this.conn.connect(this.serverName, this.userName, this.password, this.databaseName, this.port, 0);
	if (success)
	{
		this.conn.autocommit(1);
		success = !this.conn.error();
	}
	if (!success)
	{
		throw new Exception("Could not connect to MySQL database!", log);
	}
	this.isOpen = true;
	log.debug("Opened");
};

/**
 * Executes the given sql using the connection.
 * If the second argument is used, it should be an array of parameters
 * to be substituted sequentially for question marks ("?") in the sql statement,
 * which functions as a prepared statement.
 * @alias Jaxer.MySQL.Connection.prototype.execute
 * @param {String} sql the sql statement to be executed as a prepared statement
 * @param {Array} params the sequential parameters passed to the prepared statement for execution
 * @return {DB.ResultSet} The results of the query, which may or may not contain any data
 */
MySQL.Connection.prototype.execute = function execute(sql, params)
{
	log.trace("Starting execute");
	this.open(); // In case it hasn't been opened
	var rs, stmt, resultCode;
	try
	{
		var useStatement = (params && params.length);
		log.trace((useStatement ? "Using a prepared statement" : "Not using a prepared statement") + " to process: " + sql);
		if (useStatement)
		{
			try
			{
				stmt = this.conn.prepare(sql);
			}
			catch (e)
			{
				throw new Exception("Could not create prepared statement: " + e + "; SQL: " + sql, log);
			}
			if (!stmt)
			{
				throw new Exception("Could not prepare statement: " + sql, log);
			}
			var hasParams = params && params.length > 0;
			var stmtParams = stmt.paramCount();
			if ((!hasParams && (stmtParams > 0             )) ||
				( hasParams && (stmtParams != params.length)))
			{
				throw new Exception("Prepared statement " + sql + " has " + stmtParams + " parameters while " + params.length + " parameter values were given", log);
			}
			if (hasParams) // bind them
			{
				log.trace("Preparing statement: " + sql + "\nwith params: " + params);
				Util.foreach(params, function(param, index)
				{
					switch (typeof param)
					{
						case "number":
							if (isFinite(param))
							{
								stmt.bindDoubleParameter(index, param);
							}
							else
							{
								log.warn("Parameter " + index + " is not a finite number - using NULL instead. SQL: " + sql, log);
								stmt.bindNullParameter(index);
							}
							break;
						case "boolean":
							stmt.bindDoubleParameter(index, (parameter ? 1 : 0));
							break;
						case "string":
							stmt.bindUTF8StringParameter(index, param);
							break;
						case "object": // can only be used for Date binding
							if (param == null)
							{
								stmt.bindNullParameter(index);
							}
							else if (Util.isDate(param))
							{
								stmt.bindDatetimeParameter(index, Math.round(param.getTime()/1000)); // seconds since midnight 1/1/1970.
							}
							else
							{
								log.warn("Parameter " + index + " is not a Date - using NULL instead. SQL: " + sql, log);
								stmt.bindNullParameter(index);
							}
							break;
						case "undefined":
							stmt.bindNullParameter(index);
							break;
						default:
							log.warn("Parameter " + index + " is of an unsupported type (" + (typeof param) + " - using NULL instead. SQL: " + sql, log);
							stmt.bindStringParameter(index, param.toString());
							break;
					}
				});
			}
		}
		
		// Now execute the statement and return any values as a resultset
		// TODO: fix it such that even if there are no rows, columns could be meaningful
		var resultSet = new DB.ResultSet();
		
		var colTypes = [];
		var iRow = 0;
		if (useStatement) 
		{
			log.trace("Executing prepared statement");
			resultCode = stmt.execute();
			if (resultCode != Components.results.NS_OK)
			{
				throw new Exception("MySQL error " + resultCode + " while executing prepared statement. SQL: " + sql, log);
			}
			var numCols = stmt.fieldCount();
			for (var iCol = 0; iCol < numCols; iCol++) 
			{
				log.trace("Column " + iCol + ": " + stmt.getColumnName(iCol));
				resultSet.columns[iCol] = stmt.getColumnName(iCol);
				colTypes[iCol] = 
				{
					type: stmt.getColumnDecltype(iCol)
				};
			}
			log.trace("Fetching prepared statement return values to populate resultSet");
			// TODO: The following is a temporary workaround and should be cleaned up.
			var rm = stmt.resultMetadata();
			var hasResultSet = rm && (typeof rm.rowCount == "function") && (rm.rowCount() < 0);
			// End of workaround
			if (hasResultSet) 
			{
				while (stmt.fetch()) 
				{
					var row = {};
					var $values = [];
					for (var iCol = 0; iCol < numCols; iCol++) 
					{
						var colName = resultSet.columns[iCol];
						var value = getAsTypePrepared(stmt, iCol, colTypes[iCol]);
						log.trace("Row " + iRow + ", column " + colName + ": " + value);
						$values[iCol] = value;
						row[colName] = value;
					}
					row.$values = $values;
					resultSet.rows.push(row);
					resultSet.rowsAsArrays.push($values);
					iRow++;
				}
			}
		}
		else
		{
			log.trace("Executing non-prepared statement");
			try 
			{
				rs = this.conn.query(sql);
			} 
			catch (e) 
			{
				throw new Exception("MySQL error while executing non-prepared statement. Error: " + e + "; SQL: " + sql, log);
			}
			if (rs) 
			{
				try 
				{
					var numCols = rs.fieldCount();
					for (var iCol = 0; iCol < numCols; iCol++) 
					{
						var field = rs.fetchFieldDirect(iCol);
						log.trace("Column " + iCol + ": " + field.name());
						resultSet.columns[iCol] = field.name();
						colTypes[iCol] = 
						{
							type: field.typeName()
						};
					}
					log.trace("Fetching non-prepared-statement return values to populate resultSet");
					while (rs.fetchRow()) 
					{
						var row = {};
						var $values = [];
						for (var iCol = 0; iCol < numCols; iCol++) 
						{
							var colName = resultSet.columns[iCol];
							var value = getAsTypeQuery(rs, iCol, colTypes[iCol]);
							log.trace("Row " + iRow + ", column " + colName + ": " + value);
							$values[iCol] = value;
							row[colName] = value;
						}
						row.$values = $values;
						resultSet.rows.push(row);
						resultSet.rowsAsArrays.push($values);
						iRow++;
					}
				}
				finally 
				{
					if (rs)
					{
						rs.close();
					}
				}
			}
		}
		
		if (resultSet.rows.length > 0)
		{
			resultSet.hasData = true;
			resultSet.singleResult = resultSet.rowsAsArrays[0][0];
		}
		
		log.trace("Populated resultSet with " + resultSet.rows.length + " rows of " + resultSet.columns.length + " columns");
		
	}
	finally
	{
		try
		{
			if (stmt) 
			{
				log.trace("Closing statement");
				stmt.close(); // Important to always do this to free up resource
			}
			if (this.closeAfterExecute) 
			{
				log.trace("Closing the connection");
				this.close();
			}
		}
		catch (e) // log but do not throw again
		{
			log.error("Error trying to close the statement: " + e);
		}
	}
	
	log.trace("Finished executing");
	return resultSet;
};

/**
 * Retrieves the specified value from the prepared statement (at its current cursor location), casting to the given type
 * @param {Object} stmt
 * 	the statement object
 * @param {Object} iCol
 * 	The index of the column whose value is to be fetched
 * @param {Object} iType
 * 	The type object containing information on how to return this value
 * @return {String}	The value as the requested type
 */
function getAsTypePrepared(stmt, iCol, iType)
{
	if (stmt.getIsNull(iCol)) return null;
	var mysqlType = iType.type;
	var value;
	switch (mysqlType)
	{
		case VALUE_TYPE_NULL:
			value = null;
			break;
		case VALUE_TYPE_NUMBER:
			value = stmt.getDouble(iCol);
			break;
		case VALUE_TYPE_DATETIME:
			value = new Date(stmt.getDatetime(iCol) * 1000); // seconds
			break;
		case VALUE_TYPE_DATE:
			var dateValues = stmt.getDateString(Index).split(/\-/);	// "MM-DD-YYYY"
			value = new Date(dateValues[2], dateValues[1], dateValues[0], 0);
			break;
		case VALUE_TYPE_TIME:
			var timeValues = stmt.getTimeString(Index).split(/\:/);	// "HH:MM:SS"
			value = new Date(MySQL.TIME_COLUMN_YEAR, MySQL.TIME_COLUMN_MONTH, MySQL.TIME_COLUMN_DAY, timeValues[0], timeValues[1], timeValues[2], 0);
			break;
		case VALUE_TYPE_TEXT:
			value = stmt.getUTF8String(iCol);
			break;
		case VALUE_TYPE_BLOB:
			value = stmt.getUTF8String(iCol); // TODO: revisit blobs
			break;
		default:
			throw new Exception("MySQL statement returned an unknown data type: " + mysqlType, log);
	}
	return value;
}

/**
 * Retrieves the specified value from the MySQL resultset (at its current cursor location), casting to the given type
 * @param {Object} rs
 * 	the MySQL resultset
 * @param {Object} iCol
 * 	The index of the column whose value is to be fetched
 * @param {Object} iType
 * 	The type object containing information on how to return this value
 * @return {String}	The value as the requested type
 */
function getAsTypeQuery(rs, iCol, iType)
{
	if (rs.getIsNull(iCol)) return null;
	var mysqlType = iType.type;
	var value;
	switch (mysqlType)
	{
		case VALUE_TYPE_NULL:
			value = null;
			break;
		case VALUE_TYPE_NUMBER:
			value = rs.getDouble(iCol);
			break;
		case VALUE_TYPE_DATETIME:
			var dtValues = rs.getDatetimeString(iCol).split(/[\-\:\s]/);	// "YYYY-MM-DD HH:MM:SS"
			value = new Date(dtValues[0], parseInt(dtValues[1]) - 1, dtValues[2], dtValues[3], dtValues[4], dtValues[5], 0);
			break;
		case VALUE_TYPE_DATE:
			var dateValues = rs.getDateString(iCol).split(/\-/);	// "MM-DD-YYYY"
			value = new Date(dateValues[2], dateValues[1], dateValues[0], 0);
			break;
		case VALUE_TYPE_TIME:
			var timeValues = rs.getTimeString(iCol).split(/\:/);	// "HH:MM:SS"
			value = new Date(MySQL.TIME_COLUMN_YEAR, MySQL.TIME_COLUMN_MONTH, MySQL.TIME_COLUMN_DAY, timeValues[0], timeValues[1], timeValues[2], 0);
			break;
		case VALUE_TYPE_TEXT:
			value = rs.getString(iCol);
			break;
		case VALUE_TYPE_BLOB:
			value = rs.getString(iCol); // TODO: revisit blobs
			break;
		default:
			throw new Exception("MySQL query returned an unknown data type: " + mysqlType, log);
	}
	return value;
}

/**
 * Closes the connection if it's open. This is optional, and only does something if the connection is open.
 * @alias MySQL.Connection.prototype.close
 */
MySQL.Connection.prototype.close = function close()
{
	if (this.isOpen)
	{
		this.conn.close();
		this.isOpen = false;
	}
};

DB.MySQL = MySQL;

Jaxer.Log.trace("*** DB_MySQL.js loaded");
	
})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/DB > DB_SQLite.js
 */
coreTraceMethods.TRACE('Loading fragment: DB_SQLite.js');
(function() {

var log = Log.forModule("DB.SQLite");

/**
 * @namespace {Jaxer.SQLite} This is the namespace that holds the SQLite implementation of the Jaxer DB API
 */
var SQLite = {};

var SQLITE_CLASS_ID		= "@mozilla.org/storage/service;1"
var SQLITE_INTERFACE	= Components.interfaces.mozIStorageService;

var VALUE_TYPE_NULL		= 0; 	// Null data type.
var VALUE_TYPE_INTEGER	= 1; 	// INTEGER data type.
var VALUE_TYPE_FLOAT	= 2; 	// FLOAT data type.
var VALUE_TYPE_TEXT		= 3; 	// TEXT data type.
var VALUE_TYPE_BLOB		= 4; 	// BLOB data type.

var SPECIAL_HANDLING_FLAG = "__!jaxerSpecialHandling!__";
var SPECIAL_HANDLING_FLAG_NUMBER = SPECIAL_HANDLING_FLAG + "(number)";
var SPECIAL_HANDLING_FLAG_NUMBER_LENGTH = SPECIAL_HANDLING_FLAG_NUMBER.length;

/**
 * Creates a new database file according to connectionParams.PATH
 * @alias Jaxer.SQLite.createDB
 * @param {Object} connectionParams A hashmap of parameters needed to connect to the database.
 * The properties required of connectionParams are:
 *   PATH: the path to the file of the database
 */
SQLite.createDB = function createDB(connectionParams)
{
	var conn, sql;
	
	conn = new SQLite.Connection(connectionParams);
	log.debug("Creating/verifying connection to database file at " + conn.filepath)

	try
	{
		conn.open();
	}
	finally
	{
		if (conn) conn.close();
	}
}

/**
 * Creates a new connection to the given databaseName (file).
 * This can be any string you want, including the empty string.
 * If the given database does not yet exist, it is created for you.
 * The resulting connection object is the only way you interact with the database.
 * @alias Jaxer.SQLite.Connection
 * @constructor
 * @param {Object} connectionParams 
 * 	A hashmap of parameters needed to connect to the database. Required properties are PATH (database path) and
 *  CLOSE_AFTER_EXECUTE (whether to close the connection after each call to execute, default is open).
 */
SQLite.Connection = function Connection(connectionParams)
{
	this.filepath = Dir.resolvePath(connectionParams.PATH, System.executableFolder);
	this.file = new Jaxer.File(this.filepath);
	this.iFile = this.file.nsIFile;
	this.service = Components.classes[SQLITE_CLASS_ID].getService(SQLITE_INTERFACE);
	this.conn = null;
	this.closeAfterExecute = connectionParams.CLOSE_AFTER_EXECUTE;
};

/**
 * Opens the connection so queries can be executed.
 * This is optional, since if the connection is not open when it's asked to
 * execute some SQL, it will open the connection automatically.
 * Also closing the connection is optional.
 * @alias Jaxer.SQLite.Connection.prototype.open
 */
SQLite.Connection.prototype.open = function open()
{
	if ((this.conn == null) || (!this.conn.connectionReady))
	{
		log.debug("Opening connection to database: " + this.filepath);
		try 
		{
			this.conn = this.service.openDatabase(this.iFile);
		} 
		catch (e) 
		{
			throw new Exception(e, log);
		}
		log.debug("Opened");
	}
};

/**
 * Executes the given sql using the connection.
 * If the second argument is used, it should be an array of parameters
 * to be substituted sequentially for question marks ("?") in the sql statement,
 * which functions as a prepared statement.
 * @alias Jaxer.SQLite.Connection.prototype.execute
 * @param {String} sql the sql statement to be executed as a prepared statement
 * @param {Array} params the sequential parameters passed to the prepared statement for execution
 * @return {DB.ResultSet} The results of the query, which may or may not contain any data
 */
SQLite.Connection.prototype.execute = function execute(sql, params)
{
	log.trace("Starting execute");
	this.open(); // In case it hasn't been opened
//	if (this.conn.transactionInProgress) // TODO: is this necessary?
//	{
//		log.trace("Committing previous transaction");
//		this.conn.commitTransaction();
//	}
	var stmt;
	try
	{
		sql = sql.replace(/INTEGER PRIMARY KEY AUTO_INCREMENT/gi, "INTEGER PRIMARY KEY AUTOINCREMENT");
		log.trace("About to create a statement from SQL: " + sql);
		try
		{
			stmt = this.conn.createStatement(sql);
		}
		catch (e)
		{
			throw new Exception("Could not create prepared statement: " + e + "; SQL: " + sql, log);
		}
		if (!stmt)
		{
			throw new Exception("Could not prepare statement: " + sql, log);
		}
		var hasParams = params && params.length > 0;
		if ((!hasParams && (stmt.parameterCount > 0             )) ||
			( hasParams && (stmt.parameterCount != params.length)))
		{
			throw new Exception("Prepared statement " + sql + " has " + stmt.parameterCount + " parameters while there were " + params.length + " parameter values given", log);
		}
		if (hasParams) // bind them
		{
			log.trace("Preparing statement: " + sql + "\nwith params: " + params);
			Util.foreach(params, function(param, index)
			{
				switch (typeof param)
				{
					case "number":
						if (isFinite(param))
						{
							stmt.bindDoubleParameter(index, param);
						}
						else
						{
							stmt.bindStringParameter(index, SPECIAL_HANDLING_FLAG_NUMBER + param.toString());
						}
						break;
					case "boolean":
						stmt.bindInt32Parameter(index, (parameter ? 1 : 0));
						break;
					case "string":
						stmt.bindStringParameter(index, param);
						break;
					case "object":
						if (param == null)
						{
							stmt.bindNullParameter(index);
						}
						else if (Util.isDate(param))
						{
							stmt.bindInt64Parameter(index, param.getTime()); // milliseconds since midnight 1/1/1970.
						}
						else
						{
							stmt.bindStringParameter(index, param.toString());
						}
						break;
					case "undefined":
						stmt.bindNullParameter(index);
						break;
					default:
						stmt.bindStringParameter(index, param.toString());
						break;
				}
			});
		}
		
		// Now execute the statement and return any values as a resultset
		// TODO: fix it such that even if there are no rows, columns could be meaningful
		var resultSet = new DB.ResultSet();
		var columnsSet = false;
		var iRow = 0;
		var colTypes = [];
		while (stmt.executeStep())
		{
			log.trace("Populating resultSet");
			// The first time only, get the names of the columns in the returned value
			if (!columnsSet)
			{
				var numCols = stmt.columnCount;
				for (var iCol=0; iCol<numCols; iCol++)
				{
					log.trace("Column " + iCol + ": " + stmt.getColumnName(iCol));
					resultSet.columns[iCol] = stmt.getColumnName(iCol);
					colTypes[iCol] = 
					{
						declared: stmt.getColumnDecltype(iCol),
						sqlite: stmt.getTypeOfIndex(iCol)
					};
				}
				columnsSet = true;
			}
			var row = {};
			var $values = [];
			for (var iCol=0; iCol<numCols; iCol++)
			{
				var colName = resultSet.columns[iCol];
				var value = getAsType(stmt, iCol, colTypes[iCol]);
				log.trace("Row " + iRow + ", column " + colName + ": " + value);
				$values[iCol] = value;
				row[colName] = value;
			}
			row.$values = $values;
			resultSet.rows.push(row);
			resultSet.rowsAsArrays.push($values);
			iRow++;
		}
		
		if (resultSet.rows.length > 0)
		{
			resultSet.hasData = true;
			resultSet.singleResult = resultSet.rowsAsArrays[0][0];
		}
		
		log.trace("Populated resultSet with " + resultSet.rows.length + " rows of " + resultSet.columns.length + " columns");
		
	}
	catch(e)
	{
		throw new Exception(e);
	}
	finally
	{
		try
		{
			if (stmt) 
			{
				log.trace("Resetting statement");
				stmt.reset(); // Important to always do this to prevent locks! http://developer.mozilla.org/en/docs/Storage#Resetting_a_statement
				stmt.finalize();
			}
//			if (this.conn.transactionInProgress) // TODO: is this necessary?
//			{
//				log.trace("Committing transaction");
//				this.conn.commitTransaction();
//			}
			if (this.closeAfterExecute) 
			{
				log.trace("Closing the connection");
				this.close();
			}
		}
		catch (e) // log but do not throw again
		{
			log.error("Error trying to close the statement: " + e);
		}
	}
	
	log.trace("Finished executing");
	return resultSet;
};

/**
 * Retrieves the specified value from the prepared statement (at its current cursor location), casting to the given type
 * @param {Object} stmt
 * 	the statement object
 * @param {Object} iCol
 * 	The index of the column whose value is to be fetched
 * @param {Object} iType
 * 	The type object containing information on how to return this value
 * @return {String}	The value as the requested type
 */
function getAsType(stmt, iCol, iType)
{
	if (stmt.getIsNull(iCol)) return null;
	var declaredType = iType.declared;
	var sqliteType = iType.sqlite;
	if (sqliteType == VALUE_TYPE_NULL) return null;
	var value;
	switch (sqliteType)
	{
		case VALUE_TYPE_INTEGER:
			value = stmt.getInt64(iCol);
			break;
		case VALUE_TYPE_FLOAT:
			value = stmt.getDouble(iCol);
			break;
		case VALUE_TYPE_TEXT:
			value = stmt.getString(iCol);
			break;
		case VALUE_TYPE_BLOB:
			value = stmt.getString(iCol); // TODO: revisit blobs
			break;
		default:
			throw new Exception("SQLite statement returned an unknown data type: " + sqliteType, log);
	}
	if (declaredType.match(/bool/i))
	{
		value = Boolean(value);
	}
	if ((declaredType.match(/date/i) || declaredType.match(/time/i)) && (sqliteType == VALUE_TYPE_INTEGER))
	{
		value = new Date(value);
	}
	if ((sqliteType == VALUE_TYPE_TEXT) && (value.indexOf(SPECIAL_HANDLING_FLAG) == 0))
	{
		if (value.indexOf(SPECIAL_HANDLING_FLAG_NUMBER) == 0)
		{
			value = Number(value.substr(SPECIAL_HANDLING_FLAG_NUMBER_LENGTH));
		}
	}
	return value;
}

/**
 * Closes the connection if it's open. This is optional, and only does something if the connection is open.
 * @alias Jaxer.SQLite.Connection.prototype.close
 */
SQLite.Connection.prototype.close = function close()
{
	if (this.conn && this.conn.connectionReady) 
	{
		this.conn.close();
	}
};

DB.SQLite = SQLite;

Jaxer.Log.trace("*** DB_SQLite.js loaded");
	
})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Container > Container.js
 */
coreTraceMethods.TRACE('Loading fragment: Container.js');
(function(){

var log = Log.forModule("Container");

/**
 * @namespace {Jaxer.Container}
 */

/**
 * This is the contructor for the Container object, used for all types of containers (session, sessionPage, etc.)
 * @alias Jaxer.Container
 * @constructor
 * @param {String} type
 * 	The type of the container
 * @param {String} key
 * 	The key for the specific instance of the container (e.g. the sessionPage container is per page, and the page's key is used here)
 * @param {Object} persistor
 * 	An instance of the persistor (e.g. a DBPersistor) to be used to persist this container
 */
function Container(type, key, persistor)
{
	this.type = type;
	this.key = key;
	this.id = this.type + ":" + this.key;
	this.persistor = persistor;
	this.isLoaded = false;
	this.names = [];
	this.data = {};
	this.addedData = {}; 	// true ==> newly added
	this.changedData = {};	// defined ==> changed, holds previous value
	this.deletedData = {};	// defined ==> deleted, holds previous value
}

/**
 * Load the container from its store, but only if it has not yet been loaded
 * @alias Jaxer.Container.prototype.loadIfNeeded
 * @private
 */
Container.prototype.loadIfNeeded = function loadIfNeeded()
{
	if (!this.isLoaded)
	{
		log.debug("Loading container " + this.id + " for first time")
		this.data = this.persistor.loadAll(this.type, this.key);
		for (var p in this.data)
		{
			this.names.push(p);
		}
		this.isLoaded = true;
	}
};

/**
 * Gets the value of the given property
 * @alias Jaxer.Container.prototype.get
 * @param {String} name
 * 	The name of the property whose value we need
 * @return {Object}	The value
 */
Container.prototype.get = function get(name)
{
	this.loadIfNeeded();
	
	return this.data[name];
}

/**
 * Sets a name-value pair in the current container.
 * @alias Jaxer.Container.prototype.set
 * @param {String} name
 * 	The name of the property to set
 * @param {Object} value
 * 	The value of the property
 */
Container.prototype.set = function set(name, value)
{
	log.trace("For container " + this.id + ": setting '" + name + "' = '" + value + "'");
	this.loadIfNeeded();
	var nowExists = this.data.hasOwnProperty(name);
	var newlyAdded = this.addedData.hasOwnProperty(name);
	var wasDeleted = this.deletedData.hasOwnProperty(name);
	var wasChanged = this.changedData.hasOwnProperty(name);
	var previouslyPersisted = (nowExists && !newlyAdded) || wasDeleted;
	delete this.deletedData[name];
	if (previouslyPersisted)
	{
		if (!wasChanged) // This is the first time it's changed since we loaded it
		{
			this.changedData[name] = this.data[name]; // Hold the previous value
		}
	}
	else
	{
		if (!newlyAdded)  // This is the first time it's being set
		{
			this.addedData[name] = true;
		}
	}
	if (!nowExists)
	{
		this.names.push(name);
	}
	this.data[name] = (typeof value == "undefined") ? Container.DEFAULT_VALUE : value;
	log.debug("For container " + this.id + ": this.data['" + name + "'] = " + this.data[name] + "; this.changedData['" + name + "'] = " + this.changedData[name]);
}

/**
 * Mark a property as changed so it persists
 * @alias Jaxer.Container.prototype.touch
 * @param {String} name
 * 	The name of the property to touch
 */
Container.prototype.touch = function touch(name)
{
	log.trace("For container " + this.id + ": Touching " + name);
	this.loadIfNeeded();
	var nowExists = this.data.hasOwnProperty(name);
	var value = nowExists ? this.data[name] : Container.DEFAULT_VALUE;
	this.set(name, value);
}

/**
 * Does the container have the given property set?
 * @alias Jaxer.Container.prototype.exists
 * @param {String} name
 * 	The name of the property to look for
 * @return {Boolean} true if it exists, false otherwise
 */
Container.prototype.exists = function exists(name)
{
	this.loadIfNeeded();
	return this.data.hasOwnProperty(name);
}

/**
 * Was this property just created, or was it previously persisted?
 * @alias Jaxer.Container.prototype.isPreviouslyPersisted
 * @param {String} name
 * 	The name of the property to query
 * @return {Boolean} true if it was already there before, false if it's newly added
 */
Container.prototype.isPreviouslyPersisted = function isPreviouslyPersisted(name)
{
	this.loadIfNeeded();
	var nowExists = this.data.hasOwnProperty(name);
	var newlyAdded = this.addedData.hasOwnProperty(name);
	var wasDeleted = this.deletedData.hasOwnProperty(name);
	return (nowExists && !newlyAdded) || wasDeleted;
}

/**
 * Remove (unset) a property
 * @alias Jaxer.Container.prototype.remove
 * @param {String} name
 * 	The name of the property to remove
 */
Container.prototype.remove = function remove(name)
{
	log.trace("For container " + this.id + ": Removing " + name);
	this.loadIfNeeded();
	if (!this.data.hasOwnProperty(name))
	{
		log.debug("Nothing to remove: " + name);
		return; // Nothing to do
	}
	if (this.isPreviouslyPersisted(name)) // otherwise we'll need this info to remove the name-value from the persistent store
	{
		log.debug("No need to remember that '" + name + "' was changed because it was not previously persisted");
		this.deletedData[name] = this.data[name];
	}
	else
	{
		log.debug("We still need to remember that '" + name + "' was changed because it was previously persisted");
	}
	var iName = this.names.indexOf(name);
	delete this.names[iName];
	delete this.data[name];
	delete this.changedData[name];
}

/**
 * Revert a property to its previously-persisted value
 * @alias Jaxer.Container.prototype.revert
 * @param {String} name
 * 	The name of the property to revert
 */
Container.prototype.revert = function revert(name)
{
	log.trace("For container " + this.id + ": Reverting " + name);
	this.loadIfNeeded();
	var nowExists = this.data.hasOwnProperty(name);
	var newlyAdded = this.addedData.hasOwnProperty(name);
	var wasDeleted = this.deletedData.hasOwnProperty(name);
	var wasChanged = this.changedData.hasOwnProperty(name);
	if (newlyAdded)
	{
		var iName = this.names.indexOf(name);
		delete this.names[iName];
		delete this.data[name];
	}
	else if (wasDeleted)
	{
		this.data[name] = this.deletedData[name];
	}
	else if (wasChanged)
	{
		this.data[name] = this.changedData[name];
	}
	delete this.addedData[name];
	delete this.changedData[name];
	delete this.deletedData[name];
}

/**
 * Persists a container.
 * @alias Jaxer.Container.prototype.persist
 */
Container.prototype.persist = function persist(doc)
{
	log.trace("For container " + this.id + ": Persisting");
	var didSomething = false;
	if (this.persistor.persistByName)
	{
		for (var name in this.addedData)
		{
			log.debug("For container " + this.id + ": inserting " + name + " into persistent store");
			this.persistor.persist(this.type, this.key, name, this.data[name]);
			didSomething = true;
		}
		for (var name in this.changedData)
		{
			log.debug("For container " + this.id + ": updating " + name + " in persistent store");
			this.persistor.persist(this.type, this.key, name, this.data[name]);
			didSomething = true;
		}
		for (var name in this.deletedData)
		{
			log.debug("For container " + this.id + ": removing " + name + " from persistent store");
			this.persistor.remove(this.type, this.key, name);
			didSomething = true;
		}
	}
	else
	{
		log.debug("For container " + this.id + ": persisting all data");
		this.persistor.persist(this.type, this.key, this.data);
		didSomething = true;
	}
	if (didSomething) 
	{
		this.addedData = {};
		this.changedData = {};
		this.deletedData = {};
		if (Jaxer.response)
		{
			Jaxer.response.noteSideEffect();
		}
	}
}

// Static methods

var containerTypes = ["APPLICATION", "PAGE", "SESSION", "SESSION_PAGE"];
var jaxerContainerNames = [];

for (var i = 0; i < containerTypes.length; i++)
{
	var containerType = containerTypes[i];
	
	Container[containerType] = containerType;
	containerName = Util.String.upperCaseToCamelCase(containerType);
	jaxerContainerNames.push(containerName);
}

var containers = {};

/**
 * Initialize the Containers subsystem.
 * @alias  Jaxer.Container.init
 * @method
 */
Container.init = function init()
{
	var dbPersistor = new Container.DBPersistor();
	var keys = {};
	var appAndPage = Route.getAppAndPage();
	keys.application = appAndPage.app;
	keys.page = appAndPage.page;
	keys.session = SessionManager.keyFromRequest(keys.application);
	keys.sessionPage = keys.session + "$$" + keys.page;
	for (var i=0; i<containerTypes.length; i++)
	{
		var containerType = containerTypes[i];
		var containerName = jaxerContainerNames[i];
		var container = new Container(containerType, keys[containerName], dbPersistor);
		Jaxer[containerName] = containers[container.type] = container;
		log.debug("Created " + container.type + " container as Jaxer." + containerName + " with key " + container.key);
	}
	Jaxer.clientData = {};
};

/**
 * Persists all container data to the store (as needed).
 * @alias Jaxer.Container.persistAll
 * @method
 * @param {Object} doc
 * 	The current document, if any, into which the clientData container's data will be inserted. 
 * 	Not applicable for callbacks.
 */
Container.persistAll = function persistAll(doc)
{
	// Persist session key to client
	SessionManager.keyToResponse(containers[Container.APPLICATION].key, containers[Container.SESSION].key);
	// Persist all containers to the DB and then clear them to make sure nobody has access to their data
	for (var i=0; i<containerTypes.length; i++)
	{
		var containerType = containerTypes[i];
		var containerName = jaxerContainerNames[i];
		containers[containerType].persist();
		delete containers[containerType];
		delete Jaxer[containerName];
	}
	// Persist clientData to client and then clear it
	var hasClientData = false;
	for (var p in Jaxer.clientData)
	{
		hasClientData = true;
		break;
	}
	if (hasClientData && doc)
	{
		Jaxer.response.noteSideEffect();
		Jaxer.response.noteDomTouched();
		var dataString = Serialization.toJSONString(Jaxer.clientData);
		var js = "Jaxer.clientData = Jaxer.Serialization.fromJSONString('" + Util.String.escapeForJS(dataString) + "');";
		var head = doc.getElementsByTagName("head")[0];
		Util.DOM.insertScriptAtBeginning(doc, js, head, null);
	}
	delete Jaxer.clientData;
};

Container.DEFAULT_VALUE = true; // The default value for name-value pairs

frameworkGlobal.Container = Jaxer.Container = Container;

Log.trace("*** Container.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Container > DBPersistor.js
 */
coreTraceMethods.TRACE('Loading fragment: DBPersistor.js');
(function(){

var log = Log.forModule("DBPersistor"); // Only if Log itself is defined at this point of the includes

/**
 * A database-based persistor for Jaxer Container objects (session, sessionPage, etc.)
 * @alias Jaxer.DBPersistor
 * @constructor
 */
function DBPersistor()
{
	this.persistByName = true;
}

/**
 * Retrieve a given container type's and name's property, by name
 * @alias Jaxer.DBPersistor.prototype.load
 * @param {String} type
 * 	The type of the container
 * @param {String} key
 * 	The key for the specific instance of the container (e.g. the sessionPage container is per page, and the page's key is used here)
 * @param {String} name
 * 	The name of the property to query for
 * @return {Object}	The value of the requested property, or the empty object if there is none
 */
DBPersistor.prototype.load = function load(type, key, name)
{
	log.debug("Loading: " + [type, key, name]);
	var rs = DB.frameworkExecute("SELECT value FROM containers WHERE `type` = ? AND `key` = ? AND `name` = ?",
		[type, key, name]);
	if (rs.rows.length == 0)
	{
		log.debug("No values found");
		return {};
	}
	else
	{
		var value = rs.rows[0].value;
		log.debug("Found: " + value);
		return Serialization.fromJSONString(value);
	}
};

/**
 * Load all the name-value properties at once for the given container type and key
 * @alias Jaxer.DBPersistor.prototype.loadAll
 * @param {String} type
 * 	The type of the container
 * @param {String} key
 * 	The key for the specific instance of the container (e.g. the sessionPage container is per page, and the page's key is used here)
 * @return {Object}	A hashmap of name-value pairs
 */
DBPersistor.prototype.loadAll = function loadAll(type, key)
{
	log.debug("Loading all: " + [type, key]);
	var rs = DB.frameworkExecute("SELECT name, value FROM containers WHERE `type` = ? AND `key` = ?",
		[type, key]);
	var results = {};
	var iReturned = 0;
	for (var i=0; i<rs.rows.length; i++)
	{
		var row = rs.rows[i];
		results[row.name] = Serialization.fromJSONString(row.value);
		iReturned++;
	}
	log.debug("Returning " + iReturned + " results");
	return results;
};

/**
 * Persist a particular property (by name) for the given container type and key
 * @alias Jaxer.DBPersistor.prototype.persist
 * @param {String} type
 * 	The type of the container
 * @param {String} key
 * 	The key for the specific instance of the container (e.g. the sessionPage container is per page, and the page's key is used here)
 * @param {String} name
 * 	The name of the property to persist
 * @param {Object} data
 * 	The value to persist for this property
 */
DBPersistor.prototype.persist = function persist(type, key, name, data)
{
	log.debug("Persisting " + [type, key, name, data]);
	var dataString = Serialization.toJSONString(data);
	var now = new Date();
	// In MySQL we could use the following statement:
	//DB.frameworkExecute("INSERT INTO containers (" + fields + ") VALUES (" + placeholders + ")" +
	//	" ON DUPLICATE KEY UPDATE `value` = VALUES(`value`), `modification_datetime` = VALUES(`modification_datetime`)",
	//	values);
	// But to be DB-independent, we need to first see if it exists and update, or else insert
	// TODO: This is NOT race-condition-safe
	var rs = DB.frameworkExecute("SELECT id FROM containers WHERE `type`=? AND `key`=? AND `name`=?", [type, key, name]);
	if (rs.rows.length > 0)
	{
		DB.frameworkExecute("UPDATE containers SET `value`=?, `modification_datetime`=? WHERE id=?", [dataString, now, rs.rows[0].id]);
	}
	else
	{
		var fields = "`" + ['type', 'key', 'name', 'value', 'creation_datetime', 'modification_datetime'].join("`,`") + "`";
		var values = [type, key, name, dataString, now, now];
		var placeholders = Util.map(values, function(field) { return "?"; });
		DB.frameworkExecute("INSERT INTO containers (" + fields + ") VALUES (" + placeholders + ")", values);
	}
};

/**
 * Completely remove the given property from the database
 * @alias Jaxer.DBPersistor.prototype.remove
 * @param {String} type
 * 	The type of the container
 * @param {String} key
 * 	The key for the specific instance of the container (e.g. the sessionPage container is per page, and the page's key is used here)
 * @param {String} name
 * 	The name of the property to remove
 */
DBPersistor.prototype.remove = function remove(type, key, name)
{
	log.debug("Deleting " + [type, key, name]);
	DB.frameworkExecute("DELETE FROM containers WHERE `type` = ? AND `key` = ? AND `name` = ?",
		[type, key, name]);
};

/**
 * Creates the database schema needed to persist containers
 * @alias Jaxer.DBPersistor.createSchema
 */
DBPersistor.createSchema = function createSchema() // static
{
	var impl, conn, sql;
	
	impl = DB[Config.DB_FRAMEWORK_IMPLEMENTATION];
	conn = new impl.Connection(Config.DB_FRAMEWORK_CONNECTION_PARAMS);

	try
	{
		sql = "CREATE TABLE IF NOT EXISTS containers (" +
			" `id` INTEGER PRIMARY KEY AUTO_INCREMENT," +
			" `type` VARCHAR(32) NOT NULL," +
			" `key` VARCHAR(255) NOT NULL," +
			" `name` VARCHAR(255) NOT NULL," +
			" `value` LONGBLOB," +
			" `creation_datetime` DATETIME NOT NULL," +
			" `modification_datetime` DATETIME NOT NULL" +
			")";

		conn.open();
		conn.execute(sql);
		Log.debug("Successfully created schema: " + sql);
		conn.close();
	}
	catch (e)
	{
		log.error("Error creating schema: " + sql + ": " + e + " [continuing anyway]");
	}
	finally
	{
		if (conn) conn.close();
	}
};

Container.DBPersistor = DBPersistor;

Log.trace("*** DBPersistor.js loaded");  // Only if Log itself is defined at this point of the includes

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Container > SessionManager.js
 */
coreTraceMethods.TRACE('Loading fragment: SessionManager.js');
(function(){

var log = Log.forModule("SessionManager"); // Only if Log itself is defined at this point of the includes

var SessionManager = 
{
	COOKIE_PREFIX: "JaxerSessionId$$"
};

/**
 * Get session key from the client via cookie, or create one as needed
 * @alias Jaxer.SessionManager.keyFromRequest
 * @param {String} appKey
 * 	The key that uniquely identifies the current application
 * @return {String}	A session key that can be used to track the current session (new or existing)
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
 * @alias Jaxer.SessionManager.keyToResponse
 * @param {String} appKey
 * 	The key that uniquely identifies the current application
 * @param {String} key
 * 	The session key to use
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

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Container > Route.js
 */
coreTraceMethods.TRACE('Loading fragment: Route.js');
(function() {
	
var log = Log.forModule("Route");

var Route = {};

/**
 * Get app and page keys from Request.
 * @alias Jaxer.Route.getAppAndPage
 * @return {Object}	A hashmap containing the app and page matched for the current request from the routes table (in configRoutes.js)
 */
Route.getAppAndPage = function getAppAndPage()
{
	log.trace("Getting app and page keys from request...")
	var suffix = " before the app and page can be determined";
	var request = Jaxer.request;
	var noRequest = ((typeof request == "undefined") || (request == null));
	if (noRequest) throw new Exception("Need Jaxer.request" + suffix, log);
	var url = request.parsedUrl;
	if (url == null) throw new Exception("Could not parse url to determine routes: " + request.parsedUrlError);
	var noRoutes = (typeof Config.routes == "undefined");
	if (noRoutes) throw new Exception("Need Config.routes" + suffix, log);
	var noRoutesArray = noRoutes || (Config.routes.constructor != Config.routes.__parent__.Array);
	if (noRoutesArray) throw new Exception("Need Config.routes to be an array" + suffix, log);

	log.trace("Parsed url: " + Util.__listProps(url, "; ", true));
	
	var appAndPage = null;
	for (var i=0; i<Config.routes.length; i++)
	{
		appAndPage = Config.routes[i](url);
		Log.trace("For url = " + url.host + " and i = " + i + ", appAndPage = " + appAndPage);
		if (appAndPage != null) break;
	}
	if (appAndPage == null)
	{
		throw new Exception("No app and page match found for: " + url.url, log);
	}
	appAndPage.app = appAndPage[0];
	appAndPage.page = appAndPage[1];
	return appAndPage;
};

include(Config.FRAMEWORK_DIR + "/configRoutes.js");

frameworkGlobal.Route = Jaxer.Route = Route;

Log.trace("*** Route.js loaded");
		
})();


/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > Request.js
 */
coreTraceMethods.TRACE('Loading fragment: Request.js');
(function() {

var log = Log.forModule("Request");

/**
 * An instance of this object has the lifecycle of the current request and contains information about it.
 * @alias Jaxer.Request
 * @constructor
 * @param {Object} evt
 * 	The core event whose data is used to initialize this Request object instance
 */
var Request = function Request(evt)
{

	var _request = evt.Request;
	
	for (var p in _request)
	{
		if (typeof(_request[p]) == "string")
		{
			this[p] = _request[p];
		}
	}
	
	this.headers = {};

	var headerCount = _request.GetHeaderCount();
	
	for (var i = 0; i<headerCount; i++)
	{
		var name = _request.GetHeaderName(i);
		var value = _request.GetValueByOrd(i);

		this.headers[name] = value;
	}
	
	var hasReferer = (typeof this.headers.Referer == "string");
	this.referer = hasReferer ? Util.Url.parseUrl(this.headers.Referer) : null;
	var hasUri = (typeof this.uri == "string");
	var hasHost = (typeof this.headers.Host == "string");
	this.current = (hasUri && hasHost) ? Util.Url.parseUrlFragments(this.headers.Host, this.uri) : null;
	
	if (Jaxer.isCallback)
	{
		this.parsedUrl = this.referer;
		
		if (this.parsedUrl)
		{
			this.parsedUrlError = "";
		}
		else
		{
			var headers = [];
			
			for (var headerName in this)
			{
				headers.push(headerName + "=" + this[headerName]);
			}
			
			headers = headers.sort();
			
			this.parsedUrlError = "This is a callback but there was no evt.Request.headers.Referer:\n" + headers.join("; ");
		}
	}
	else
	{
		this.parsedUrl = this.current;
		
		if (this.parsedUrl)
		{
			this.parsedUrlError = "";
		}
		else
		{
			this.parsedUrlError = "This is not a callback and there was no evt.Request.uri and/or no evt.Request.headers.Host";
		}
	}
	
	this.currentFolder = this.pageFile.replace(/[\/\\][^\/\\]*$/, '');
	
	var data = {};
	var files = [];
	var paramIsPost = false;
	
	// First fill info with data from GET and/or POST request data
	if (this.current)
	{
		for (var p in this.current.queryParts)
		{
			data[p] = this.current.queryParts[p];
		}
		if (data.hasOwnProperty(Callback.PARAMS_AS) && data[Callback.PARAMS_AS] == Callback.PARAMS_AS_POST_TEXT)
		{
			paramIsPost = true;
		}
	}
	
	if (this.method == "POST")
	{
		if (paramIsPost)
		{
			data[Callback.PARAMETERS] = [this.postData];
		}
		else
		{
			var postCount = _request.GetDataItemCount();
			
			for (var i = 0; i < postCount; i++) 
			{
				var name = _request.GetDataItemName(i);
				var value = _request.GetDataItemValue(i);
				
				data[name] = value;
			}
			
			var fileCount = _request.GetFileCount();
			for (var i=0; i<fileCount; i++) 
			{
				files.push(new FileInfo(request, i));
			}
			
		}
	}
	
	this.data = data;
	this.files = files;
	this.paramIsPost = paramIsPost;

}

/**
 * Create a string representation of all request header key/value pairs
 * @internal
 * @alias Jaxer.Request.prototype.listHeaders
 * @param {String} [separator]
 * 		This optional parameter can be used to specify the string to use between
 * 		request header entries. If this value is not specified, then the string
 * 		"; " will be used
 * @return {String}
 * 		A string of all header key/value pairs
 */
Request.prototype.listHeaders = function listHeaders(separator)
{
	if (typeof separator == "undefined")
	{
		separator = "; ";
	}
	
	var headers = [];
	
	for (var headerName in this.headers)
	{
		headers.push(headerName + "=" + this.headers[headerName]);
	}
	
	headers = headers.sort();
	
	return headers.join(separator);
};

/**
 * An internal container for information about uploaded files
 * @param {Object} req
 * 	The core's request object
 * @param {Object} i
 * 	An index into the list of uploaded files
 */
var FileInfo = function FileInfo(req, i)
{
	this.fileName = req.GetFileName(i);
	this.originalFileName = req.GetOriginalFileName(i);
	this.tempFileName = req.GetTmpFileName(i);
	this.contentType = req.GetFileContentType(i);
	this.fileSize = req.GetFileSize(i);
	this.index = i;
}

/**
 * Save the uploaded file to the given path -- otherwise it will be automatically purged after this request
 * @alias Jaxer.FileInfo.prototype.save
 * @param {String} newFileName
 * 	The name (or full path) of the file to which the uploaded file should be saved.
 */
FileInfo.prototype.save = function save(newFileName)
{
	var file = new Jaxer.File(this.tempFileName);
	var newFile = new Jaxer.File(newFileName);
	if (newFile.exists())
	{
		newFile.remove();
	}
	file.copy(newFileName);
}

frameworkGlobal.Request = Jaxer.Request = Request;

Jaxer.Log.trace("*** Request.js loaded");

})();


/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > Response.js
 */
coreTraceMethods.TRACE('Loading fragment: Response.js');
(function() {

var log = Log.forModule("Response");

/**
 * An instance of this object has the lifecycle of the current response and contains information about it.
 * @alias Jaxer.Response
 * @constructor
 * @param {Object} evt
 * 	The core event whose data is used to initialize this Response object instance and to output to the client
 */
var Response = function Response(evt)
{

	var _response = evt.Response;
	var hasDomBeenTouched = false;
	var hasSideEffect = false;
	var error = null;
	var errorLogged = false;
	
	this.allow = function allow(isAllowed)
	{
		_response.frameworkFailed = !Boolean(isAllowed);
	}
	
	this.noteDomTouched = function noteDomTouched()
	{
		hasDomBeenTouched = true;
	}
	
	this.noteSideEffect = function noteSideEffect()
	{
		hasSideEffect = true;
	}
	
	this.getDomTouched = function getDomTouched()
	{
		return hasDomBeenTouched;
	}
	
	this.getSideEffect = function getSideEffect()
	{
		return hasSideEffect;
	}
	
	this.addHeader = function addHeader(name, value, replaceExisting)
	{
		_response.addHeader(name, value, replaceExisting);
	}
	
	this.setContents = function setContents(contents)
	{
		_response.contents = contents;
	}
	
	this.useOriginalContents = function useOriginalContents()
	{
		log.trace("Jaxer did not end up modifying this page or having any (known) side-effects - using original response in lieu of a Jaxer-generated one")
		_response.shouldUseOriginalContent = true;
	}
	
	this.notifyError = function notifyError(newError)
	{
		if (!error) 
		{
			error = newError || "Unspecified error";
		}
		if (!errorLogged)
		{
			errorLogged = true;
			log.error("Error processing this request: " + error);
		}
	}
	
	this.hasError = function hasError()
	{
		return error ? true : false;
	}
	
	this.getError = function getError()
	{
		return error;
	}

	this.setNoCacheHeaders = function setNoCacheHeaders()
	{
		log.trace("Setting no-cache headers");
		// Set enough headers to prevent caching: we assume the served page is considered dynamic content.
		var noCacheHeaders = 
		[
			["Expires", "Fri, 23 May 1997 05:00:00 GMT", true],
			["Cache-Control", "no-store, no-cache, must-revalidate", true],
			["Cache-Control", "post-check=0, pre-check=0", false],
			["Pragma", "no-cache", true]
		];
		for (var i=0; i<noCacheHeaders.length; i++)
		{
			Jaxer.response.addHeader(noCacheHeaders[i][0], noCacheHeaders[i][1], noCacheHeaders[i][2]);
		}
	}
	
	this.exposeJaxer = function exposeJaxer()
	{
		// Expose the Jaxer processing (in addition to any previous processors)
		if (Jaxer.Config.EXPOSE_JAXER) 
		{
			Jaxer.response.addHeader("X-Powered-By", "Jaxer/" + Jaxer.buildNumber + " (Aptana)", false);
		}
	}
	
	this.getHeader = function getHeader(nameOrIndex)
	{
		if (typeof nameOrIndex == "number")
		{
			return _response.getValueByOrd(nameOrIndex);
		}
		else
		{
			return _response.getValueByName(nameOrIndex.toString());
		}
	}
	
	this.getHeaderName = function getHeaderName(index)
	{
		return _response.getHeaderName(index);
	}
	
	this.getHeaderCount = function getHeadersCount()
	{
		return _response.getHeaderCount();
	}
	
}

frameworkGlobal.Response = Jaxer.Response = Response;

Jaxer.Log.trace("*** Response.js loaded");

})();


/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > CoreEvents.js
 */
coreTraceMethods.TRACE('Loading fragment: CoreEvents.js');
(function() {

var log = Log.forModule("CoreEvents");

// private variables
var CoreEvents = {};
var handlers = {};

var eventNames = ["SERVER_START", "REQUEST_START", "PARSE_START", "NEW_ELEMENT", "INIT_HEAD", "PARSE_COMPLETE", "REQUEST_COMPLETE", "CALLBACK"];
for (var i=0; i<eventNames.length; i++)
{
	var eventName = eventNames[i];
	CoreEvents[eventName] = eventName;
	log.trace("Defining CoreEvents." + eventName);
	handlers[eventName] = [];
}

/**
 * getEventNames returns a (copy of the) array of framework event names to which you can register handlers
 * @alias Jaxer.CoreEvents.getEventNames
 * @return {Array} an array of handler names (modifying it has no impact on CoreEvents)
 */
CoreEvents.getEventNames = function getEventNames()
{
	return eventNames.concat(); // returns a copy so the original is not externally modifiable
}

/**
 * bind a handler to the specified eventType.
 * @alias Jaxer.CoreEvents.addHandler
 * @param {String} eventType
 * 	one of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 * @param {Function} handlerFunction
 * 	a function reference invoked when the event specified by eventType is triggered
 * @param {Function} [testFunction]
 * 	an optional function to be evaluated when the event is about to be fired; 
 * 	it'll only be fired if the function evaluates to true.
 * 	The one argument passed to this function is the request (same as Jaxer.request)
 * @param {Object} [handlerFunctionObject]
 * 	optional object on which to call the handlerFunction (it becomes the value of 'this' within the function). 
 */
CoreEvents.addHandler = function addHandler(eventType, handlerFunction, testFunction, handlerFunctionObject)
{
	if (handlers.hasOwnProperty(eventType) == false) 
	{
		throw new Exception("Attempted to add a handler to an eventType for which there are no handler queues: " + eventType, log);
	}

	var handler = { handler: handlerFunction };
	handler.test = (testFunction ? testFunction : null);
	handler.object = (handlerFunctionObject ? handlerFunctionObject : null);
	
	log.trace("Adding a handler to " + eventType + ": " + handler.handler.toString().substr(0, 60) + "..."); // This may be verbose!
	handlers[eventType].push(handler);
	log.trace("Finished adding a handler");
};

/**
 * remove an existing handler on an event type
 * @alias Jaxer.CoreEvents.removeHandler
 * @param {String} eventType
 * 	one of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 * @param {Function} handlerFunction
 * 	the handler to remove
 */
CoreEvents.removeHandler = function removeHandler(eventType, handlerFunction)
{
	if (handlers.hasOwnProperty(eventType) == false) 
	{
		throw new Exception("Attempted to add a handler to an eventType for which there are no handler queues: " + eventType, log);
	}
	log.trace("Removing a handler for " + eventType + ": " + handlerFunction.toString().substr(0, 60) + "..."); // This may be verbose!
	var removed = false;
	for (var i=handlers.length-1; i>=0; i--)
	{
		var handler = handlers[i].handler;
		if (handler == handlerFunction)
		{
			handlers.splice(i, 1); // remove the handler
			removed = true;
			break;
		}
	}
	if (removed)
	{
		log.warn("Did not find the handler to remove");
	}
	else
	{
		log.trace("Finished removing a handler");
	}
}

/**
 * returns an array of handlers for the specified eventType.
 * @alias Jaxer.CoreEvents.getHandlers
 * @param {String} eventType
 * 	one of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 * @return {Array} an array of handlers
 */
CoreEvents.getHandlers = function getHandlers(eventType)
{
	if (handlers.hasOwnProperty(eventType) == false)
	{
		throw new Exception("Invalid event queue specified: " + eventType, log);
	}
	
	return handlers[eventType];
};

/**
 * removes the handlers for the specified eventType. 
 * @alias Jaxer.CoreEvents.clearHandlers
 * @param {String} eventType
 * 	one of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 */
CoreEvents.clearHandlers = function clearHandlers(eventType)
{
	var handlersForType = CoreEvents.getHandlers(eventType);
	
	handlersForType = [];
};

/**
 * triggers the handlers for the specified eventType.
 * @alias Jaxer.CoreEvents.fire
 * @param {String} eventType
 * 	one of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 */
CoreEvents.fire = function fire(eventType)
{
	try 
	{
		if (Jaxer.fatalError &&
			eventType != CoreEvents.PARSE_START &&
			eventType != CoreEvents.PARSE_COMPLETE) // only these special events can deal with fatal errors meaningfully - TODO: what about callbacks?
		{
			Jaxer.notifyFatal("Aborting " + eventType + " due to Jaxer.fatalError: " + Jaxer.fatalError);
			return;
		}
		
		var evt = arguments[1];
		
		if (eventType == CoreEvents.REQUEST_START) 
		{
			Jaxer.isCallback = false;
			Jaxer.pageWindow = null;
			Jaxer.lastScriptRunat = "";
		}
		
		if (eventType == CoreEvents.PARSE_START) 
		{
			Jaxer.isCallback = evt.Request.isCallback;
			log.trace("During CoreEvents.PARSE_START, evt.Request.uri=" + evt.Request.uri + " and Jaxer.isCallback=" + Jaxer.isCallback);
		}
		
		log.trace("eventType: " + eventType);
		
		if ((eventType == CoreEvents.PARSE_COMPLETE) && Jaxer.isCallback) 
		{
			eventType = CoreEvents.CALLBACK;
		}
		
		log.trace("At this time, Jaxer.isCallback=" + Jaxer.isCallback + " and eventType=" + eventType);
		
		if (handlers.hasOwnProperty(eventType) == false) 
		{
			throw new Exception("Attempted to fire an event for which there are no handler queues: " + eventType, log);
		}
		
		var handlersForType = CoreEvents.getHandlers(eventType);
		
		log.trace("Firing an event for " + eventType);
		
		var args = new Array(arguments.length - 1);
		for (var i = 1; i < arguments.length; i++) 
		{
			args[i - 1] = arguments[i];
		}
		
		for (var i = 0; i < handlersForType.length; i++) 
		{
			var handler = handlersForType[i].handler;
			var test = handlersForType[i].test;
			var object = handlersForType[i].object;
			
			if (!test || test(Jaxer.request)) 
			{
				log.trace("Calling event handler: " + handler.toString().substr(0, 60) + "..."); // This may be verbose!
				handler.apply(object, args);
			}
		}
	
	} 
	catch (e) 
	{
		var message = "Error during framework event " + eventType + ": " + e;
		if (eventType == CoreEvents.SERVER_START || (!Jaxer.request)) 
		{
			Jaxer.notifyFatal(message);
		}
		else 
		{
			Jaxer.request.notifyError(message);
		}
	}
		
};

/**
 * Should the given script element be evaluated (server-side) rather than passed to the client without being evaluated
 * @alias Jaxer.CoreEvents.isScriptEvaluateEnabled
 * @param {Object} scriptElement
 * 	The DOM script element
 * @return {Boolean} true if it should be evaluated, false if it should not
 */
CoreEvents.isScriptEvaluateEnabled = function isScriptEvaluateEnabled(scriptElement)
{
	var runat = scriptElement.getAttribute(RUNAT_ATTR);
	return (
		!Jaxer.passthrough &&
		scriptElement.hasAttribute(RUNAT_ATTR) && 
		scriptElement.getAttribute(RUNAT_ATTR).match(RUNAT_ANY_SERVER_REGEXP));
}

frameworkGlobal.CoreEvents = Jaxer.CoreEvents = CoreEvents;

Jaxer.Log.trace("*** CoreEvents.js loaded");
	
})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > ServerStart.js
 */
coreTraceMethods.TRACE('Loading fragment: ServerStart.js');
(function() {

var log = Log.forModule("ServerStart");
	
CoreEvents.clearHandlers(CoreEvents.SERVER_START); // Clear this out -- we're the first and only default handler

try
{
	CoreEvents.addHandler(CoreEvents.SERVER_START, function onServerStart(evt)
	{

		if (!DB.isSpecified(true))
		{
			var error = new Exception("The database connection parameters have not been specified", log);
			Jaxer.fatalError = error;
			throw error;
		}

		try
		{
			
			log.trace("About to create framework database");
			createDatabase(true);
			if (DB.isSpecified(false))
			{
				log.trace("About to create developer database");
				createDatabase(false);
			}
					
			log.trace("About to create callbacks schema");
			createCallbacksSchema();
			
			log.trace("About to create container schema");
			Container.DBPersistor.createSchema();
			
			log.trace("About to create callbackPages cache");
			Jaxer.callbackPages = {};
			Jaxer.callbackPageCRCs = {};
			Jaxer.callbackPagesByCRC = {};
			Jaxer.callbackGlobal = Components.utils.import("resource:///framework/callbackGlobal.js", null);
			
			if (Config.EMBEDDED_CLIENT_FRAMEWORK_SRC) // embed the client framework in the page
			{
				log.trace("Reading embedded client framework source from: " + Config.EMBEDDED_CLIENT_FRAMEWORK_SRC);
				Jaxer.embeddedClientFramework = File.read(Config.EMBEDDED_CLIENT_FRAMEWORK_SRC);; 
				log.trace("Embedded client framework is " + Jaxer.embeddedClientFramework.length + " characters long");
			}
			else
			{
				Jaxer.embeddedClientFramework = null; // will need to get the client framework from the web server
			}
			
			Jaxer.loadAllExtensions();
		
		}
		catch (e)
		{
			var error = new Exception(e, log);
			Jaxer.fatalError = error;
			throw error;
		}
		
	});
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

/**
 * Creates the framework or developer database according to the Config settings
 * @param {Boolean} isFramework
 * 	If true, create the framework database, else create the developer database
 */
function createDatabase(isFramework)
{
	var impl = isFramework ? DB[Config.DB_FRAMEWORK_IMPLEMENTATION] : DB[Config.DB_IMPLEMENTATION];
	var connectionParams = isFramework ? Config.DB_FRAMEWORK_CONNECTION_PARAMS : Config.DB_CONNECTION_PARAMS;
	impl.createDB(connectionParams);
}

/**
 * Creates the framework's database schema needed by the framework
 */
function createCallbacksSchema()
{
	var impl, conn, sql, rs;
	
	impl = DB[Config.DB_FRAMEWORK_IMPLEMENTATION];
	conn = new impl.Connection(Config.DB_FRAMEWORK_CONNECTION_PARAMS);

	try
	{
		sql = "CREATE TABLE IF NOT EXISTS callback_page (" +
			" id INTEGER PRIMARY KEY AUTO_INCREMENT," +
			" crc32 INT(11) DEFAULT NULL," +
			" name VARCHAR(255) DEFAULT NULL," +
			" document_root VARCHAR(255) DEFAULT NULL," +
			" page_file VARCHAR(255) DEFAULT NULL," +
			" value LONGTEXT," +
			" creation_datetime DATETIME DEFAULT NULL," +
			" access_datetime DATETIME DEFAULT NULL," +
			" access_count INT(11) DEFAULT 0" +
			")";

		conn.open();
		conn.execute(sql);
		Log.debug("Successfully executed SQL: " + sql);
		conn.close();
		
	}
	catch (e)
	{
		log.error("Error executing SQL: " + sql + ": " + e + " [continuing anyway]");
	}
	finally
	{
		if (conn) conn.close();
	}
	
}

Log.trace("*** ServerStart.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > RequestStart.js
 */
coreTraceMethods.TRACE('Loading fragment: RequestStart.js');
(function() {

var log = Log.forModule("RequestStart");

CoreEvents.clearHandlers(CoreEvents.REQUEST_START); // Clear this out -- we're the first and only default handler

try
{
	
	CoreEvents.addHandler(CoreEvents.REQUEST_START, function onRequestStart(evt)
	{
		// Not much to do by default -- note we don't yet have a Request object
	});
	
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

Log.trace("*** RequestStart.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > InitHead.js
 */
coreTraceMethods.TRACE('Loading fragment: InitHead.js');
(function() {

var log = Log.forModule("InitHead");

CoreEvents.clearHandlers(CoreEvents.INIT_HEAD); // Clear this out -- we're the first and only default handler

try
{
	CoreEvents.addHandler(CoreEvents.INIT_HEAD, function onInitHead(evt, doc)
	{
		
		// TODO: Try to move this earlier, perhaps to RequestStart where we load the framework
		// Sanity check
		if (!DB.isSpecified(true))
		{
			var error = new Exception("The database connection parameters have not been specified", log);
			Jaxer.fatalError = error;
			throw error;
		}

		// Use default implementations and connections throughout
		log.trace("Initializing the default database connections");
		var impl, conn;
		
		impl = DB[Config.DB_FRAMEWORK_IMPLEMENTATION];
		conn = new impl.Connection(Config.DB_FRAMEWORK_CONNECTION_PARAMS);
		DB.init(true, impl, conn);
		
		if (DB.isSpecified(false))
		{
			impl = DB[Config.DB_IMPLEMENTATION];
			conn = new impl.Connection(Config.DB_CONNECTION_PARAMS);
			DB.init(false, impl, conn);
		}
	
		// Initialize containers for storing/retrieving state
		Container.init();
		
		Jaxer.pageWindow = doc.defaultView; // Thanks Brendan!
		var defaultScript = "Jaxer.__eval = function __eval(s) { return eval(s); };\n" +
			"Jaxer.load = function load(src, global, runat) { Jaxer.Includer.load(src, global || window, runat); }\n" +
			"Jaxer.Overrides.applyAll(window);";
		Jaxer.Includer.evalOn(defaultScript, Jaxer.pageWindow); // TODO: we should not need __eval

	});
	
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

Log.trace("*** InitHead.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > NewElement.js
 */
coreTraceMethods.TRACE('Loading fragment: NewElement.js');
(function() {

var log = Log.forModule("NewElement");

CoreEvents.clearHandlers(CoreEvents.NEW_ELEMENT); // Clear this out -- we're the first and only default handler

try
{
	CoreEvents.addHandler(CoreEvents.NEW_ELEMENT, function onNewElement(evt, doc, elt)
	{
		log.trace("Received element: " + elt.tagName);
		switch (elt.tagName)
		{
			case "SCRIPT":
				if (CoreEvents.isScriptEvaluateEnabled(elt))
				{
					Jaxer.response.noteDomTouched();
					Jaxer.response.noteSideEffect();
				}
				Jaxer.lastScriptRunat = elt.getAttribute(RUNAT_ATTR);
				if (elt.hasAttribute(SRC_ATTR) &&
					elt.hasAttribute(RUNAT_ATTR) &&
					elt.getAttribute(RUNAT_ATTR).match(RUNAT_ANY_SERVER_REGEXP)) 
				{
					if (Config.INLINE_EXTERNAL_SCRIPTS)
					{
						inlineScript(elt);
					}
					else 
					{
						log.warn("Could not include script " + elt.getAttribute(SRC_ATTR) + " because Config.INLINE_EXTERNAL_SCRIPTS is false");
					}
				}
				break;
			case "JAXER:INCLUDE":
			case "JAXER:INCLUDEJS":
				Jaxer.response.noteDomTouched();
				Jaxer.response.noteSideEffect();
				Includer.includeElement(doc, elt);
				break;
		}
	});
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

/**
 * Get and include the given script element's source, and turn the script element into an inline one
 * @param {ScriptElement} script
 * 	The script element to be inlined
 */
function inlineScript(script)
{
	var src = script.getAttribute(SRC_ATTR);
	var content = Web.get(src);
	content = content.replace(/\r\n/g, "\n").replace(/\r/g, "\n");
	script.removeAttribute(SRC_ATTR);
	script.innerHTML = content;
}

Log.trace("*** NewElement.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > ParseComplete.js
 */
coreTraceMethods.TRACE('Loading fragment: ParseComplete.js');
(function() {

var log = Log.forModule("ParseComplete");

CoreEvents.clearHandlers(CoreEvents.PARSE_COMPLETE); // Clear this out -- we're the first and only default handler

try
{
	CoreEvents.addHandler(CoreEvents.PARSE_COMPLETE, function onParseComplete(evt, doc)
	{
		
		Jaxer.response.allow(true);
		
		if (Jaxer.fatalError) // say goodbye and go home
		{
			Jaxer.notifyFatal("Aborting PARSE_COMPLETE due to Jaxer.fatalError: " + Jaxer.fatal);
			var container = doc.createElement("div");
			container.setAttribute("style", 'border: 1px solid red; background-color:wheat; font-weight:bold; padding: 4px');
			container.innerHTML = 
				[
				 "Fatal Jaxer error: " + Jaxer.fatalError,
				 "(Please see the Jaxer log for more details)",
				 "After fixing the problem, you may need to force the browser to reload this page not from its cache"
				].join("<br>\n");
			doc.body.insertBefore(container, doc.body.firstChild);
			return;
		}
		
		if (Jaxer.response.hasError()) // say goodbye for this request and go home
		{
			Jaxer.request.notifyError("Aborting PARSE_COMPLETE due to error processing this request: " + Jaxer.response.getError());
			var container = doc.createElement("div");
			container.setAttribute("style", 'border: 1px solid red; background-color:wheat; font-weight:bold; padding: 4px');
			container.innerHTML = 
				[
				 "Jaxer error processing request: " + Jaxer.response.getError(),
				 "(Please see the Jaxer log for more details)"
				].join("<br>\n");
			doc.body.insertBefore(container, doc.body.firstChild);
			return;
		}
		
		processOnServerLoad(doc);

		// process script blocks
		try
		{
			log.debug("Script processing starting");
			CallbackManager.processCallbacks(doc);
			log.debug("Script processing complete");
		}
		catch (e)
		{
			log.error("Error during CallbackManager.processCallbacks: \n" + Exception.toDetails(e));
		}
		
		// persist containers
		Container.persistAll(doc);
		log.trace("Containers persisted");
		
		if (Jaxer.response.getDomTouched() || Jaxer.response.getSideEffect()) 
		{
			// embed, or emit reference to, the client framework
			// This must come at the end, so Jaxer ends up being defined at the very beginning.
			var head = doc.getElementsByTagName("head")[0];
			if (Jaxer.embeddedClientFramework)
			{
				Util.DOM.insertScriptAtBeginning(doc, Jaxer.embeddedClientFramework, head);
			}
			else if (Config.CLIENT_FRAMEWORK_SRC) 
			{
				var src = Config.CLIENT_FRAMEWORK_SRC;
				src += ((src.indexOf("?") > -1) ? "&" : "?") + "version=" + Jaxer.buildNumber;
				Util.DOM.insertScriptAtBeginning(doc, "", head, 
				{
					src: src
				});
			}
			else 
			{
				log.error("Could not insert the client part of the Jaxer framework: please check EMBEDDED_CLIENT_FRAMEWORK_SRC and CLIENT_FRAMEWORK_SRC in your Jaxer configuration");
			}
			
			// set any other response headers on this dynamic, Jaxer-processed contents
			Jaxer.response.setNoCacheHeaders();
			Jaxer.response.exposeJaxer();
		}
		else
		{
			Jaxer.response.useOriginalContents();
		}
	});
	
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

/**
 * Do the right thing if the developer specified an onserverload handler for this page
 * @param {DocumentElement} doc
 * 	The document whose onserverload (if any) is to be processed
 */
function processOnServerLoad(doc)
{
	var onServerLoad;
	
	if (typeof Jaxer.pageWindow.onserverload != "undefined")
	{
		onServerLoad = Jaxer.pageWindow.onserverload;
	}
	else if (doc.body.hasAttribute("onserverload"))
	{
		onServerLoad = doc.body.getAttribute("onserverload");
		doc.body.removeAttribute("onserverload");
		Jaxer.response.noteDomTouched();
	}
	else
	{
		return;
	}
	
	try
	{
		if (typeof onServerLoad == "function")
		{
			Jaxer.response.noteSideEffect();
			onServerLoad();
		}
		else if (onServerLoad != null && typeof onServerLoad.toString == "function")
		{
			Jaxer.response.noteSideEffect();
			Jaxer.Includer.evalOn(onServerLoad.toString(), Jaxer.pageWindow);
		}
		else
		{
			log.warn("onserverload is defined but it is neither a function nor can it be converted to one, ignoring... ")
		}
	}
	catch (e)
	{
		log.error("Error during onserverload: " + e);
	}
}

Log.trace("*** ParseComplete.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > ParseStart.js
 */
coreTraceMethods.TRACE('Loading fragment: ParseStart.js');
(function() {

var log = Log.forModule("ParseStart");

CoreEvents.clearHandlers(CoreEvents.PARSE_START); // Clear this out -- we're the first and only default handler

try
{
	
	CoreEvents.addHandler(CoreEvents.PARSE_START, function onParseStart(evt)
	{
		
		Jaxer.request = frameworkGlobal.request = new Jaxer.Request(evt);
		Jaxer.response = frameworkGlobal.response = new Jaxer.Response(evt);
		log.trace("[Jaxer.request.documentRoot, Jaxer.request.pageFile] = " + [Jaxer.request.documentRoot, Jaxer.request.pageFile]);
		
		// The following can be set by the developer to list all the server functions that
		// should be proxied. It's equivalent to setting .proxy = true on them.
		// If a string is specified, it should be the function's name.
		// To enforce no proxies, the developer should set this to null (not just to []).
		Jaxer.proxies = [];
		
	});
	
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

Log.trace("*** ParseStart.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > Callback.js
 */
coreTraceMethods.TRACE('Loading fragment: Callback.js');
(function() {
	
var log = Log.forModule("Callback");

// private constants
var ID = "id";
var METHOD_NAME = "methodName";
var RESULT_AS = "resultAs";
var RESULT_AS_TEXT = "text";
var RESULT_AS_OBJECT = "object";
var RESULT_AS_WRAPPED_OBJECT = "wrappedObject";
var NULL_AS_TEXT = ""; // What to return if the POSTPROCESS is POSTPROCESS_ASTEXT and the result is null or an exception
// global constants
var Callback = {};
Callback.PARAMETERS = "params";
Callback.PARAMS_AS = "paramsAs";
Callback.PARAMS_AS_POST_TEXT = "postText";
Callback.PARAMS_AS_QUERY = "query";
Callback.PARAMS_AS_ARRAY = "array";

CoreEvents.clearHandlers(CoreEvents.CALLBACK); // Clear this out -- we're the first and only default handler

// add the framework's CALLBACK event handler
CoreEvents.addHandler(CoreEvents.CALLBACK, function onCallback(evt, doc)
{
	log.trace("Callback processing initializing");
	
	// On a callback, the following need to be set from the database, which we do further below
	Jaxer.request.pageFile = "";
	Jaxer.request.documentRoot = "";
	
	log.trace("Main callback processing starting");
	var result = null;
	var exception = null;
	
	try
	{
		var info = {};
		var functionName = null;
		var args = [];
		var crc32 = null;
		var resultAs = null;
		var paramIsPost = false;
		
		// First fill info with data from GET and/or POST request data
		var requestData = Jaxer.request.data;
		for (var p in requestData)
		{
			info[p] = Jaxer.request.data[p];
		}
		
		// Now extract data from info to specific fields: functionName, args, etc.
		if (info.hasOwnProperty(METHOD_NAME))
		{
			functionName = info[METHOD_NAME];
		}
		var paramIsQuery = (info.hasOwnProperty(Callback.PARAMS_AS) && info[Callback.PARAMS_AS] == Callback.PARAMS_AS_QUERY);
		if (paramIsQuery)
		{
			delete info[Callback.PARAMETERS];
			args = [info];
		}
		else if (info.hasOwnProperty(Callback.PARAMETERS))
		{
			if (info[Callback.PARAMETERS].constructor == Array)
			{
				args = info[Callback.PARAMETERS];
			}
			else
			{
				args = Serialization.fromJSONString(info[Callback.PARAMETERS]);
			}
		}
		if (info.hasOwnProperty(ID)) // get page key
		{
			crc32 = info[ID];
		}
		if (info.hasOwnProperty(RESULT_AS))
		{
			resultAs = info[RESULT_AS];
		}
		
		// Next, reconstruct environment and call the callback
		if (functionName !== null && args !== null && crc32 !== null)
		{
			var isPageCached = Jaxer.callbackPagesByCRC.hasOwnProperty(crc32);
			
			// grab all functions in our callback page that are not being called
			var rs = DB.frameworkExecute("SELECT " + (isPageCached ? "" : "value, ") + "name, document_root, page_file, access_count FROM callback_page WHERE crc32 = ?",
				[crc32]);
			
			if (rs.rows.length > 0)
			{
				var callbackRow = rs.rows[0];
				Jaxer.request.documentRoot = callbackRow.document_root;
				Jaxer.request.pageFile = callbackRow.page_file;
				var pageKey = CallbackManager.pageNameToKey(callbackRow.name);
				log.trace("[Jaxer.request.documentRoot, Jaxer.request.pageFile] = " + [Jaxer.request.documentRoot, Jaxer.request.pageFile]);
				
				if (!isPageCached)
				{
					log.debug("Callbacks found in DB but not in cache, restoring to cache using pageKey=" + pageKey + " and crc32=" + crc32);
					CallbackManager.cacheCallbackPage(pageKey, crc32, callbackRow.value);
				}
				var callbackPage = Jaxer.callbackPages[Jaxer.callbackPagesByCRC[crc32]];
				Jaxer.callbackGlobal.window = Jaxer.pageWindow;
				Jaxer.callbackGlobal.document = Jaxer.pageWindow.document;
				for (var fn in callbackPage)
				{
					if ((typeof fn == "string") && (typeof callbackPage[fn] == "function"))
					{
						Jaxer.pageWindow[fn] = callbackPage[fn];
					}
				}
				
				var accessCount = callbackRow.access_count - 0;
				accessCount++;
				log.debug("Updating access count to " + accessCount);
				
				try
				{
					DB.frameworkExecute("UPDATE callback_page SET access_datetime=?, access_count=? WHERE crc32=?",
						[new Date(), accessCount, crc32]);
				}
				catch(e)
				{
					log.error(e + " [ignoring]");
				}

				if (callbackPage.hasOwnProperty("oncallback"))
				{
					try
					{
						callbackPage.oncallback();
					}
					catch (e)
					{
						throw new Error("Error during oncallback: " + e);
					}
				}
				
				if (callbackPage.hasOwnProperty(functionName))
				{
					var func = callbackPage[functionName];
					if (typeof func == "function")
					{
						if (func.hasOwnProperty(PROXY_ATTR) && (func[PROXY_ATTR] == true))
						{
							try
							{
								log.debug("About to execute: " + func.name);
								result = func.apply(null, args);
							}
							catch (e)
							{
								throw Exception.toError(e);
							}
						}
						else
						{
							throw new Error("Function '" + functionName + "' was not marked as callable");
						}
					}
					else
					{
						throw new Error("Property '" + functionName + "' is not of type 'function' but rather '" + (typeof func) + "'");
					}
				}
				else
				{
					throw new Error("Could not locate function: " + functionName);
				}
			}
			else
			{
				throw new Error("Unable to locate callback page for id = " + crc32);
			}
		}
		else
		{
			throw new Error("Missing function name and/or arguments and/or page id");
		}
	}
	catch(e)
	{
		log.error("CALLBACK Event Error: " + e);
		exception = e;
	}
	// Finally, in any case, package the results
	finally
	{
		log.trace("result = " + result);
		
		if (resultAs == RESULT_AS_TEXT)
		{
			Jaxer.response.setContents((result == null) ? NULL_AS_TEXT : result.toString());
		}
		else if (resultAs == RESULT_AS_OBJECT)
		{
			Jaxer.response.setContents((result == null) ? NULL_AS_TEXT : Serialization.toJSONString(result));
		}
		else
		{
			var content = {};
			if (exception !== null)
			{
				content.exception = exception;
			}
			else if (result !== undefined)
			{
				content.returnValue = result;
			}
			Jaxer.response.setContents(Serialization.toJSONString(content));
		}

		log.trace("Jaxer.response.contents = " + Jaxer.response.contents);
		
		Container.persistAll();
		log.trace("Containers persisted");

		Jaxer.response.allow(true);
		Jaxer.response.setNoCacheHeaders();
		Jaxer.response.exposeJaxer();

	}
	
	log.trace("Callback processing complete");
});

frameworkGlobal.Callback = Jaxer.Callback = Callback;

Log.trace("*** Callback.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/CoreEvents > RequestComplete.js
 */
coreTraceMethods.TRACE('Loading fragment: RequestComplete.js');
(function() {

var log = Log.forModule("RequestComplete");

CoreEvents.clearHandlers(CoreEvents.REQUEST_COMPLETE); // Clear this out -- we're the first and only default handler

try
{

	CoreEvents.addHandler(CoreEvents.REQUEST_COMPLETE, function onRequestComplete(evt)
	{
		// Nothing to do by default
	});
	
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

Log.trace("*** RequestComplete.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Extend > Extend.js
 */
coreTraceMethods.TRACE('Loading fragment: Extend.js');
(function() {

var log = Log.forModule("Extend");

Jaxer.Extensions = {}; // This will hold all the extensions

/**
 * Loads a framework extension (already defined in memory).
 * If the namespaceObject defines an onLoad method, it will be called (without arguments)
 * @alias Jaxer.loadExtension
 * @param {String} name
 * 	The (unique) name to use for this extension
 * @param {Object} namespaceObject
 * 	The object holding all the members of this extension
 */
Jaxer.loadExtension = function loadExtension(name, namespaceObject)
{
	if (!namespaceObject)
	{
		throw new Exception("No namespaceObject specified for extension '" + name + "' -- extension not loaded");
	}
	Jaxer.unloadExtension(name, true); // Unload any previous version, if any
	log.trace("Adding extension '" + name + "'");
	Jaxer.Extensions[name] = namespaceObject;
	if (typeof namespaceObject.onLoad == "function")
	{
		log.trace("Loading extension '" + name + "'...");
		namespaceObject.onLoad();
		log.trace("Loaded extension '" + name + "'");
	}
}

/**
 * Unloads a previously-loaded framework extension.
 * If its namespaceObject defines an onUnload method, it will be called (without arguments)
 * @alias Jaxer.unloadExtension
 * @param {String} name
 * 	The name used to load this extension
 * @param {Boolean} noWarnings
 * 	Whether to warn if the extension was not loaded when this was called, or its namespaceObject was not there.
 */
Jaxer.unloadExtension = function unloadExtension(name, noWarnings)
{
	var namespaceObject = Jaxer.Extensions[name];
	if (!namespaceObject)
	{
		if (!noWarnings) log.warn("No namespaceObject found for extension '" + name + "' -- extension not unloaded");
		return;
	}
	if (typeof namespaceObject.onUnload == "function") 
	{
		log.trace("Unloading extension '" + name + "'...");
		namespaceObject.onUnload();
		log.trace("Unloaded extension '" + name + "'");
	}
	delete Jaxer.Extensions[name];
	log.trace("Removed extension '" + name + "'");
}

function getDirContents(configPath)
{
	if (!configPath) return [];
	var extensionsPath = Dir.resolvePath(configPath, System.executableFolder);
	log.trace("Looking for extensions in path: " + extensionsPath);
	var dir = new Dir(extensionsPath);
	var dirContents;
	if (dir.exists())
	{
		dirContents = dir.readDir();
		log.trace("Found " + dirContents.length + " objects in path");
	}
	else
	{
		dirContents = [];
	}
	return dirContents;
}

/**
 * Loads all extensions from the folder defined in Config.EXTENSIONS_DIR.
 * @alias Jaxer.loadAllExtensions
 */
Jaxer.loadAllExtensions = function loadAllExtensions()
{
	log.trace("Loading all extensions...");
	var dirContents = getDirContents(Config.EXTENSIONS_DIR);
	dirContents = dirContents.concat(getDirContents(Config.LOCAL_EXTENSIONS_DIR));
	var numIncluded = 0;
	Util.foreach(dirContents, function loadFile(file)
	{
		if (file.isFile() && (file.URL.match(/\.js$/)))
		{
			log.trace("Included extension: " + file.path + " (from URL: " + file.URL + ")");
			numIncluded++;
			include(file.URL);
		}
	});
	log.debug("Included " + numIncluded + " extensions");
}

Log.trace("*** Extend.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Serialization > Serialization.js
 */
coreTraceMethods.TRACE('Loading fragment: Serialization.js');
/**
 * Original code from Douglas Crockford's json.js, 2007-04-30
 */

(function(){

// create Serialization container
var Serialization = {};

/**
 * Reconstruct a Javascript data structure from a JSON string. Note that we have
 * extended JSON to support object references and to support dates. Object
 * references allow multiple places within the JSON data structure to point to
 * the same object as opposed to clones of those objects. Date support uses a
 * special string format to store a given date in GMT
 * 
 * @alias Serialization.fromJSONString
 * @param {String} json
 * 		A string in the JSON format
 * @return {Object}
 * 		The resulting object graph after converting the JSON string to the
 * 		equivalent Javascript data structure
 */
Serialization.fromJSONString = function fromJSONString(json)
{
	var REFERENCE_PATTERN = /^~(\d+)~$/;
	var DATE_PATTERN = /^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})$/;
	var references = [];
	var result;

	/**
	 * A reference constitutes an object and a property on the object. This
	 * class is used to specify a specific property on an object for later
	 * setting of that value.
	 *
	 * @private
	 * @alias Serialization.fromJSONString.Reference
	 * @param {Object} object
	 * 		The source object of this reference
	 * @param {String} property
	 * 		the property on the object representing this reference value
	 * @param {Number} index
	 * 		The reference ID that uniquely identifies this reference 
	 */
	function Reference(object, property, index)
	{
		this.object = object;
		this.property = property;
		this.index = index;
	}
	
	/**
	 * Walks the list of nodes passed in the method and sets all properties
	 * on this instance's underlying object to the values in the node list
	 *
	 * @private
	 * @method
	 * @alias Serialization.fromJSONString.Reference.prototype.setValue
	 * @param {Array} nodes
	 * 		A list of all nodes in the data graph. This array is used to
	 * 		extract the value of this reference via this reference's unique id.
	 */
	Reference.prototype.setValue = function(nodes)
	{
		var result = false;
		
		if (0 <= this.index && this.index < nodes.length)
		{
			this.object[this.property] = nodes[this.index];
			result = true;
		}
		
		return result;
	};
	
	/**
	 * This post-processing steps replaces all reference strings with the actual
	 * object reference to which they refer.
	 * 
	 * @private
	 * @alias Serialization.fromJSONString.postProcess
	 * @param {Array} input
	 * 		The source array created by the first step of eval'ing the JSON
	 * 		source string.
	 * @return {Object}
	 * 		The resulting object created by dereferencing all reference values
	 * 		and rewiring of the object graph
	 */
	function postProcess(input)
	{
		var result = input;
		
		if (input.length > 0)
		{
			var valid = true;
			
			for (var i = 0; i < input.length; i++)
			{
				var item = input[i];
				
				if (item === null || item === undefined)
				{
					valid = false;
					break;
				}
				
				var type = item.constructor;
				var itemGlobal = (typeof item.__parent__ == "undefined") ? window : item.__parent__;
				
				if (type !== itemGlobal.Array && type !== itemGlobal.Object)
				{
					valid = false;
					break;
				}
				
				// add any references
				switch (type)
				{
					case itemGlobal.Array:
						postProcessArray(item);
						break;
						
					case itemGlobal.Object:
						postProcessObject(item);
						break;
				}
			}
			
			if (valid)
			{
				if (references.length > 0)
				{
					result = input[0];
					
					for (var i = 0; i < references.length; i++)
					{
						var success = references[i].setValue(input);
						
						if (success == false)
						{
							result = input;
							break;
						}
					}
				}
			}
		}
		
		return result;
	}
	
	/**
	 * This post-processing step replaces all object references that are members
	 * of the specified array with actual references to the object to which they
	 * refer
	 * 
	 * @private
	 * @alias Serialization.fromJSONString.postProcessArray
	 * @param {Array} ary
	 * 		The source array to process
	 * @return {Boolean}
	 * 		Returns true if the specified array was a valid reference array
	 */
	function postProcessArray(ary)
	{
		var result = true;
		
		for (var i = 0; i < ary.length; i++)
		{
			if (postProcessMember(ary, i) == false)
			{
				result = false;
				break;
			}
		}
		
		return result;
	}
	
	/**
	 * This post-processing step replaces all object references that are members
	 * of the specified object with actual references to the object to which
	 * they refer
	 * 
	 * @private
	 * @alias Serialization.fromJSONString.postProcessObject
	 * @param {Object} obj
	 * 		The source object to process
	 * @param {Array} references
	 * 		An array of reference instances
	 * @return {Boolean}
	 * 		Returns true if the specified object was a valid reference object
	 */
	function postProcessObject(obj, references)
	{
		var result = true;
		
		for (var p in obj)
		{
			if (postProcessMember(obj, p) == false)
			{
				result = false;
				break;
			}
		}
		
		return result;
	}
	
	/**
	 * This post-processing steps replaces all reference strings with the actual
	 * object reference to which they refer. Also, special date string formats
	 * are replaced with actual Date objects.
	 * 
	 * @private
	 * @alias Serialization.fromJSONString.postProcessMember
	 * @param {Object} obj
	 * 		The object to post-process
	 * @param {String|Number} property
	 * 		The name or index of the object to process.
	 * @return {Boolean}
	 * 		Returns true if the obj[property] value is a valid reference object
	 */
	function postProcessMember(obj, property)
	{
		var item = obj[property];
		var result = true;
		
		if (item !== null && item !== undefined)
		{
			var type = item.constructor;
			var itemGlobal = (typeof item.__parent__ == "undefined") ? window : item.__parent__;
			
			switch (type)
			{
				case itemGlobal.Array:
					// we only allow empty arrays
					if (item.length > 0)
					{
						result = false;
					}
					break;

				case itemGlobal.Object:
					// we only allow empty objects
					for (var p in item)
					{
						result = false;
						break;
					}
					break;
											
				case itemGlobal.String:
					var match = item.match(REFERENCE_PATTERN);
					
					if (match !== null)
					{
						var index = match[1] - 0;
						var ref = new Reference(obj, property, index);
						
						references.push(ref);
					}
					else
					{
						match = item.match(DATE_PATTERN);
						
						if (match !== null)
						{
							obj[property] = new Date(Date.UTC(match[1], match[2] - 1, match[3], match[4], match[5], match[6]));
						}
					}
					break;
			}
		}
		
		return result;
	}
	
	/**
	 * For JSON strings that do not contain references, we make a
	 * post-processing step to replace all special date strings with actual
	 * Date instances.
	 * 
	 * @private
	 * @alias Serialization.fromJSONString.filter
	 * @param {String} property
	 * 		The property name to filter
	 * @param {Object} value
	 * 		The value of the property being filtered
	 */
	function filter(property, value)
	{
		var result = value;
		
		if (typeof value == "string")
		{
			var match = value.match(DATE_PATTERN);
			
			if (match)
			{
				result = new Date(Date.UTC(match[1], match[2] - 1, match[3], match[4], match[5], match[6]));
			}
		}
		
		return result;
	}
	
	/**
	 * Traverse the resulting JSON object to perform any post-processing needed
	 * to convert references and date strings to their proper instances.
	 * 
	 * @private
	 * @alias Serialization.fromJSONString.walk
	 * @param {String} property
	 * 		The name of the propery to visit
	 * @param {Object} obj
	 * 		The object whose property will be visited
	 * @return {Object}
	 * 		The resulting filter property value
	 */
	function walk(property, obj)
	{
		if (obj && typeof obj === 'object')
		{
			for (var p in obj)
			{
				if (obj.hasOwnProperty(p))
				{
					obj[p] = walk(p, obj[p]);
				}
			}
		}
		
		return filter(property, obj);
	}
	
	// Run the text against a regular expression to look for non-JSON
	// characters. We are especially concerned with '()' and 'new' because they
	// can cause invocation, and '=' because it can cause mutation. But just to
	// be safe, we will reject all unexpected characters.

	if (/^("(\\.|[^"\\\n\r])*?"|[,:{}\[\]0-9.\-+EINaefilnr-uy \n\r\t])+?$/.test(json))
	{
		// We use the eval function to compile the text into a JavaScript
		// structure. The '{' operator is subject to a syntactic ambiguity in
		// JavaScript: it can begin a block or an object literal. We wrap the
		// text in parens to eliminate the ambiguity.

		try
		{
			result = eval('(' + json + ')');
		}
		catch (e)
		{
			throw new SyntaxError("parseJSON: exception '" + e + "' when evaluating: " + json);
		}
	}
	else
	{
		throw new SyntaxError("parseJSON: unexpected characters in: " + json);
	}

	// expand references
	
	if (result)
	{
		var itemGlobal = (typeof result.__parent__ == "undefined") ? window : result.__parent__;
		
		if (result.constructor === itemGlobal.Array)
		{
			result = postProcess(result);
		}
		else
		{
			// expand special strings (just date right now)
			result = walk('', result);
		}
	}
	
	return result;
};

/**
 * Convert the specified object into a JSON representation. Note that we have
 * modified JSON to support object references (cycles) and to convert Dates into
 * a special format that will be recognized by our code during deserialization.
 * 
 * @alias Serialization.toJSONString
 * @param {Object} data
 * 		The source object to convert to a JSON string
 * @return {String}
 * 		The resulting JSON string which can be reversed back into the source
 * 		object via Serialization.fromJSONString
 */
Serialization.toJSONString = function toJSONString(data)
{
	var ID_PROPERTY = "$id";
	
	/**
	 * Convert an Array to a JSON string
	 * 
	 * @private
	 * @alias Serialization.toJSONString.ArrayToJSON
	 * @param {Array} ary
	 * 		The source Array to be serialized
	 * @return {String}
	 * 		The resulting JSON string
	 */
	function ArrayToJSON(ary)
	{
		var result = [];
		var length = ary.length;
		
		// For each value in this array...
		for (var i = 0; i < length; i += 1)
		{
			result.push(Serialization.toJSONString(ary[i]));
		}
	
		// Join all of the fragments together and return.
		return "[" + result.join(",") + "]";
	}
	
	/**
	 * Convert a date to a our special string format for later deserizliation
	 * 
	 * @private
	 * @alias Serialization.toJSONString.DateToJSON
	 * @param {Date} data
	 * 		The source Date to be serialized
	 * @return {String}
	 * 		The resulting JSON string
	 */
	function DateToJSON(data)
	{
		// Format integers to have at least two digits.
		function pad(n)
		{
			return n < 10 ? '0' + n : n;
		}
	
		// Ultimately, this method will be equivalent to the date.toISOString
		// method.
		return '"' +
			data.getFullYear() + '-' +
			pad(data.getUTCMonth()) + '-' +
			pad(data.getUTCDate()) + 'T' +
			pad(data.getUTCHours()) + ':' +
			pad(data.getUTCMinutes()) + ':' +
			pad(data.getUTCSeconds()) + '"';
	}
	
	/**
	 * Convert an object to a JSON string
	 * 
	 * @private
	 * @alias Serialization.toJSONString.ObjectToJSON
	 * @param {Object} data
	 * 		The source object to be serialized
	 * @return {String}
	 * 		The resulting JSON string
	 */
	function ObjectToJSON(data)
	{
		var result = [];
	
		// Iterate through all of the keys in the object, ignoring the proto
		// chain.
		for (var k in data)
		{
			var p = '"' + k + '":';
			var v = data[k];
			
			result.push(p + Serialization.toJSONString(v));
		}
	
		// Join all of the fragments together and return.
		return "{" + result.join(',') + "}";
	}
	
	/**
	 * Convert a string to a JSON string
	 * 
	 * @private
	 * @alias Serialization.toJSONString.StringToJSON
	 * @param {Object} data
	 * 		The source string to be serialized
	 * @return {String}
	 * 		The resulting JSON string
	 */
	function StringToJSON(data)
	{
		// m is a table of character substitutions.
		var characterMap = {
			'\b': '\\b',
			'\t': '\\t',
			'\n': '\\n',
			'\f': '\\f',
			'\r': '\\r',
			'"' : '\\"',
			'\\': '\\\\'
		};
		
		// If the string contains no control characters, no quote characters,
		// and no backslash characters, then we can simply slap some quotes
		// around it. Otherwise we must also replace the offending characters
		// with safe sequences.

		if (/["\\\x00-\x1f]/.test(data))
		{
			return '"' + data.replace(
				/([\x00-\x1f\\"])/g,
				function (a, b)
				{
					var c = characterMap[b];
					
					if (c)
					{
						return c;
					}
					
					c = b.charCodeAt();
					
					return '\\u00' + Math.floor(c / 16).toString(16) + (c % 16).toString(16);
				}
			) + '"';
		}
		
		return '"' + data + '"';
	}
	
	/**
	 * Walk the object graph and tag all items in the graph. Note that cycles
	 * are detected in this process and all special properties used for this
	 * discovery process are later removed.
	 * 
	 * @private
	 * @alias Serialization.toJSONString.tagReferences
	 * @param {Object} obj
	 * 		The top-most object in the data graph.
	 * @return {Boolean}
	 * 		Return true if this specifed object contains references; otherwise,
	 * 		return false. This value can be used to decide if this object needs
	 * 		to be represented as standard JSON or in our extended format.
	 */
	function tagReferences(obj)
	{
		var index = 0;
		var result = false;
		var queue = [obj];
		
		while (queue.length > 0)
		{
			var item = queue.shift();
			
			if (item !== null && item !== undefined)
			{
				if (item.hasOwnProperty(ID_PROPERTY) == false)
				{
					var type = item.constructor;
					var itemGlobal = (typeof item.__parent__ == "undefined") ? window : item.__parent__;
					
					switch (type)
					{
						case itemGlobal.Array:
							if (item.length > 0)
							{
								item[ID_PROPERTY] = index;
								items[index] = item;
								index++;
								
								for (var i = 0; i < item.length; i++)
								{
									queue.push(item[i]);
								}
							}
							break;
							
						case itemGlobal.Object:
							for (var p in item)
							{
								// we only get here if we have properties
								item[ID_PROPERTY] = index;
								items[index] = item;
								index++;
								
								for (var p in item)
								{
									queue.push(item[p]);
								}
								
								// exit since we only needed to know if we had
								// properties
								break;
							}
							break;
							
						default:
							break;
					}
				}
				else
				{
					result = true;
				}
			}
		}
		
		return result;
	}
	
	/**
	 * Remove all temporary properties used in cycle discovery
	 * 
	 * @private
	 * @alias Serialization.toJSONString.untagReferences
	 */
	function untagReferences()
	{
		for (var i = 0; i < items.length; i++)
		{
			delete items[i][ID_PROPERTY];
		}
	}
	
	/**
	 * Convert the specified object into a JSON string
	 * 
	 * @private
	 * @alias Serialization.toJSONString.JSONify
	 * @param {Object} data
	 * 		The Javascript value to be serialized
	 * @return {String}
	 * 		The resulting JSON string
	 */
	function JSONify(data)
	{
		var result;
		var ctor = data.constructor;
		var dataGlobal = (typeof data.__parent__ == "undefined") ? window : data.__parent__;
			
		switch (ctor)
		{
			case dataGlobal.Array:
				result = ArrayToJSON(data);
				break;
			
			case dataGlobal.Boolean:
			case dataGlobal.Number:
				result = String(data);
				break;
				
			case dataGlobal.Date:
				result = DateToJSON(data);
				break;
				
			case dataGlobal.Object:
				result = ObjectToJSON(data);
				break;
				
			case dataGlobal.String:
				result = StringToJSON(data);
				break;
				
			default:
				// handle custom objects
				result = ObjectToJSON(data);
				break;
		}
		
		return result;
	}
	
	var result = "null";
	var items = [];
	
	if (data !== null && data !== undefined)
	{
		if (tagReferences(data) == false)
		{
			untagReferences();
			
			result = JSONify(data);
		}
		else
		{
			var references = [];
			
			for (var i = 0; i < items.length; i++)
			{
				var item = items[i];
				var type = item.constructor;
				var itemGlobal = (typeof item.__parent__ == "undefined") ? window : item.__parent__;
				
				switch (type)
				{
					case itemGlobal.Array:
						var parts = [];
						
						for (var j = 0; j < item.length; j++)
						{
							var elem = item[j];
							
							if (elem !== null && elem !== undefined)
							{
								if (elem.hasOwnProperty(ID_PROPERTY))
								{
									parts.push('"~' + elem[ID_PROPERTY] + '~"');
								}
								else
								{
									parts.push(JSONify(elem));
								}
							}
							else
							{
								parts.push("null");
							}
						}
						
						references.push("[" + parts.join(",") + "]");
						break;
						
					case itemGlobal.Object:
						var parts = [];
						
						for (var p in item)
						{
							if (p != ID_PROPERTY)
							{
								var elem = item[p];
								var k = '"' + p + '":';
								
								if (elem !== null && elem !== undefined)
								{
									if (elem.hasOwnProperty(ID_PROPERTY))
									{
										parts.push(k + '"~' + elem[ID_PROPERTY] + '~"');
									}
									else
									{
										parts.push(k + JSONify(elem));
									}
								}
								else
								{
									parts.push(k + "null");
								}
							}
						}
						
						references.push("{" + parts.join(",") + "}");
						break;
						
					default:
						break;
				}
			}
			
			result = "[" + references.join(",") + "]";
		}
	}

	return result;
};

Jaxer.Serialization = Serialization;

if (Jaxer.isOnServer)
{
	frameworkGlobal.Serialization = Jaxer.Serialization;
}

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Parsing > FunctionInfo.js
 */
coreTraceMethods.TRACE('Loading fragment: FunctionInfo.js');
(function(){

var log = Log.forModule("FunctionInfo");

// private constants
var CALLBACK_NS = "Jaxer.Callback";
var SERVER_NS = "Jaxer.Server";
var INVOKE_NAME = "invokeFunction";
var INVOKE_ASYNC_NAME = "invokeFunctionAsync";
var ASYNC_SUFFIX = "Async";

/**
 * FunctionInfo
 * 
 * This class encapsulates function information needed for generating
 * client-side proxies and for server-side storage of functions used in
 * callbacks.
 * 
 * @constructor
 * @alias Jaxer.FunctionInfo
 * @param {Function|String} functionReference
 * 		If there are 3 arguments passed to this constructor, this parameter is
 * 		the name of the function this info represents. Otherwise, this is a
 * 		reference to the actual Javascript function instance this info
 * 		represents.
 * @param {Node|String[]} [functionNode]
 * 		If there are 3 arguments passed to this constructor, this parameter is
 * 		an array of parameter names for the function this info represents.
 * 		Otherwise, this is an optional parameter that is the root node of an AST
 * 		representing the function this info represents.
 * @param {String} [source]
 * 		If there are 3 arguments passed to this constructor, this parameter is
 * 		the actual source code of the function this info represents.
 */
function FunctionInfo(functionReference, functionNode)
{
	if (arguments.length == 3)
	{
		this.name = arguments[0];
		this.parameterNames = arguments[1];
		this.source = arguments[2];
		this.prototypeProperties = [];
	}
	else
	{
		if (functionNode === undefined)
		{
			var source = functionReference.toString();
			var ast = parse(source, "<none>", 1);
			
			functionNode = ast.funDecls[0];
		}
		
		this.name = functionNode.name;
		this.parameterNames = functionNode.params;
		this.source = functionReference.toString();
		this.prototypeProperties = [];
		
		if (functionReference.hasOwnProperty("prototype"))
		{
			var proto = functionReference.prototype;
			var properties = [];
			
			for (var p in proto)
			{
				// make sure this is a local property only and a function
				if (proto.hasOwnProperty(p) && typeof(proto[p]) == "function")
				{
					properties.push(p);
				}
			}
			
			var self = this;
			
			Util.foreach(properties.sort(), function(p)
			{
				var ref = proto[p];
				var source = p + " = " + ref.toString();
				var ast = parse(source, "<none>", 1);
				var node = ast[0].expression[1];
				
				node.name = p;
				
				self.prototypeProperties.push(new FunctionInfo(ref, node));
			});
		}
	}
}

/**
 * Create a string representation of the underlying function to be used
 * client-side as a callback to this function on the server
 * 
 * @method
 * @alias Jaxer.FunctionInfo.prototype.createClientCallback
 * @return {String}
 * 		Returns a string representation of this function info's underlying
 * 		Javascript function in a form suitable for invocation on the client
 * 		but with execution being done on the server
 */
FunctionInfo.prototype.createClientCallback = function()
{
	var buffer = [];
	
	// create synchronous callback
	buffer.push("function ");
	buffer.push(this.name);
	buffer.push("() {return " + CALLBACK_NS + "." + INVOKE_NAME + ".call(null, \"");
	buffer.push(this.name);
	buffer.push("\", arguments);}");
	
	buffer.push("\n");
	
	// create asynchronous callback
	buffer.push("function ");
	buffer.push(this.name);
	buffer.push(ASYNC_SUFFIX);
	buffer.push("(callback) {return " + CALLBACK_NS + "." + INVOKE_ASYNC_NAME + ".call(null, callback, \"");
	buffer.push(this.name);
	buffer.push("\", arguments);}");
	
	return buffer.join("");
};

/**
 * Is the underlying function a native one (for which no actual source is available)?
 * 
 * @method
 * @alias Jaxer.FunctionInfo.prototype.isNative
 * @return {Boolean}
 */
FunctionInfo.prototype.isNative = function isNative()
{
	return Util.isNativeFunctionSource(this.source);
}

/**
 * Create a string representation of the underlying function to be used
 * client-side as the source of this function.
 * 
 * @method
 * @alias Jaxer.FunctionInfo.prototype.createClientSource
 * @return {String}
 * 		Returns a string representation of this function info's underlying
 * 		Javascript function in a form needed to execute the function on the
 * 		client
 */
FunctionInfo.prototype.createClientSource = function()
{
	var buffer = [];
	var parentName = this.name;
	
	buffer.push(this.source);
	
	Util.foreach(this.prototypeProperties, function(f)
	{
		buffer.push([parentName, ".prototype.", f.name, " = ", f.createClientSource(), ";"].join(""));
	});
	
	return buffer.join("\n");
};

/**
 * Create a string representation of the underlying function to be used
 * client-side as a callback to this function on the server. This differs from
 * createClientCallback in that this version namespaces the function. This is
 * needed when a client-side function exists both as a client-side function and
 * as a server-side callback. The namespace allows the developer to distinguish
 * between which version of the function that needs to be invoked.
 * 
 * @method
 * @alias Jaxer.FunctionInfo.prototype.createNamespacedClientCallback
 * @return {String}
 * 		Returns a string representation of this function info's underlying
 * 		Javascript function in a form suitable for invocation on the client
 * 		but with execution being done on the server
 */
FunctionInfo.prototype.createNamespacedClientCallback = function()
{
	var buffer = [];
	
	// create synchronous callback
	buffer.push(SERVER_NS + ".");
	buffer.push(this.name);
	buffer.push(" = function ");
	buffer.push(this.name);
	buffer.push("() {return " + CALLBACK_NS + "." + INVOKE_NAME + ".call(null, \"");
	buffer.push(this.name);
	buffer.push("\", arguments);}");
	
	buffer.push("\n");
	
	// create synchronous callback
	buffer.push(SERVER_NS + ".");
	buffer.push(this.name);
	buffer.push(ASYNC_SUFFIX);
	buffer.push(" = function ");
	buffer.push(this.name);
	buffer.push(ASYNC_SUFFIX);
	buffer.push("(callback) {return " + CALLBACK_NS + "." + INVOKE_ASYNC_NAME + ".call(null, callback, \"");
	buffer.push(this.name);
	buffer.push("\", arguments);}");
	
	return buffer.join("");
};

/**
 * Create a string representation of the underlying function to be used
 * during a callback. This source will be stored in the callback database and
 * will be used to reconstitute the function during a callback. This is used
 * for cached and proxied functions.
 * 
 * @method
 * @alias Jaxer.FunctionInfo.prototype.createServerFunction
 * @param {String} [namespace]
 * 		If specified, the function is specified as a property on the given namespace object.
 * 		Otherwise, the function is specified as a global property
 * @param {Boolean} [proxied]
 * 		If specified and true, the function will have a property called "proxy" and set to true.
 * @return {String}
 * 		Returns a string representation of this function info's underlying
 * 		Javascript function in a form needed to execute the function on the
 * 		server.
 */
FunctionInfo.prototype.createServerFunction = function(namespace, proxied, noLocalScope)
{
	var sources = [];
	
	var assignments = [];
	assignments.push(namespace + "." + this.name);
	if (!noLocalScope)
	{
		assignments.push("this." + this.name);
	}
	assignments.push(this.source);
	
	sources.push(assignments.join(" = ") + ";"); // main assignments of function body to properties
	if (proxied)
	{
		sources.push(namespace + "." + this.name + ".proxy = true;")
	}
	
	// add prototype methods
	Util.foreach(this.prototypeProperties, function(f)
	{
		sources.push(namespace + "." + this.name + ".prototype." + f.name + " = " + f.source + ";");
	});
	
	return sources.join("\n");
};

frameworkGlobal.FunctionInfo = Jaxer.FunctionInfo = FunctionInfo;

Log.trace("*** FunctionInfo.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Parsing > Range.js
 */
coreTraceMethods.TRACE('Loading fragment: Range.js');
(function(){

var log = Log.forModule("Range");

/**
 * The Range class is used to represent a block of text within a string.
 * @constructor
 * @alias Jaxer.Range
 * @param {Number} start
 * 	The offset to the beginning of the range
 * @param {Number} end
 * 	The offset to the end of the range
 */
function Range(start, end)
{
	this.start = start;
	this.end = end;
}

/**
 * Determine if the specified value is contained by this range. Note that the
 * value must be contained by the half-open interval [start,end). This means
 * that a value equal to the ending offset of this range is not considered to
 * be contained by the range. However, an offset equal to the start of this
 * range is within the ragne.
 * @method
 * @alias Jaxer.Range.prototype.contains
 * @param {Number} value
 * 		The value to test
 * @return {Boolean}
 * 		Returns true if the value is contained within this range
 */
Range.prototype.contains = function(value)
{
	return (this.start <= value && value < this.end);
};

/**
 * Determine if this range contains any content. This method will return true
 * if the start of this rang eis greater than or equal to the end of this range.
 * @method
 * @alias Jaxer.Range.prototype.isEmpty
 * @return {Boolean}
 * 		Returns true if this range is empty
 */
Range.prototype.isEmpty = function()
{
	return (this.start >= this.end);
};

/**
 * Determines if two ranges are overlapping. Note this uses the contains method
 * to determine the overlap so the same rules about half-open intervals
 * mentioned in that method's description also apply here.
 * @method
 * @alias Jaxer.Range.prototype.isOverlapping
 * @param {Range} range
 * 		The range to test
 * @return {Boolean}
 * 		Returns true if the ranges overlap
 */
Range.prototype.isOverlapping = function(range)
{
	var result;
	
	if (this.isEmpty() || range.isEmpty())
	{
		result = false;
	}
	else
	{
		result = (
			this.contains(range.start)   ||
			this.contains(range.end - 1) ||
			range.contains(this.start)   ||
			range.contains(this.end - 1)		
		);
	}
	
	return result;
};

frameworkGlobal.Range = Jaxer.Range = Range;

Log.trace("*** Range.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Parsing > TextParser.js
 */
coreTraceMethods.TRACE('Loading fragment: TextParser.js');
(function(){

var log = Log.forModule("TextParser");

var JS_COMMENT_PATTERN = /(?:\/\*(?:.|[\r\n])+?\*\/)|(?:\/\/[^\r\n]*)/mg;
var NAMED_FUNCTION_PATTERN = /function\s+([\w\$]+)\s*\(([^\)]*)\)/mg;
var WHITESPACE_PATTERN = /^\s+$/;

// create TextParser container
var TextParser = {};

/**
 * Find all comments in the specified source and return an array of Ranges, one
 * for each comment.
 * 
 * @alias Jaxer.TextParser.getCommentRanges
 * @param {String} source
 * 		The Javascript source code to parse
 * @return {Range[]}
 * 		A list of range objects representing the range of all multi-line
 * 		comments in the source code
 */
TextParser.getCommentRanges = function(source)
{
	var commentRanges = [];
	var commentMatcher = JS_COMMENT_PATTERN.exec(source);
	
	while (commentMatcher != null)
	{
		var start = commentMatcher.index;
		var end = JS_COMMENT_PATTERN.lastIndex;
		var range = new Range(start, end);
		
		commentRanges.push(range);
		
		commentMatcher = JS_COMMENT_PATTERN.exec(source);
	}
	
	return commentRanges;
};

/**
 * Find all uncommented function definitions in the specified source and return
 * an array of FunctionInfos, one for each function found.
 * 
 * @alias Jaxer.TextParser.getFunctionInfos
 * @param {String} source
 * 		The Javascript source code to parse
 * @return {FunctionInfo[]}
 * 		A list of function info instances, one for each Javascript function
 * 		found in the source code
 */
TextParser.getFunctionInfos = function(source)
{
	var result;
	
	if (SIMPLE_PARSE)
	{
		result = simpleParse(source);
	}
	else
	{
		result = fullParse(source);
	}
	
	return result;
};

/**
 * Use Narcissus to create an AST of the specified source.
 * 
 * @private
 * @alias Jaxer.TextParser.fullParse
 * @param {String} source
 * 		The Javascript source code to parse
 * @return {Node}
 * 		The resulting Narcissus AST of the source code
 */
function fullParse(source)
{
	var ast;
	var result = [];
		
	try
	{
		ast = parse(source);
	}
	catch (e)
	{
		// create empty AST so we can continue processing
		ast = parse("");
	}
	
	// temp
	TextParser.ast = ast;
	
	var functionNodes = ast.funDecls;
	
	for (var i = 0; i < functionNodes.length; i++)
	{
		var functionNode = functionNodes[i];
		var name = functionNode.name;
		
		if (Jaxer.pageWindow.hasOwnProperty(name))
		{
			if (typeof Jaxer.pageWindow[name] == "function")
			{
				var func = Jaxer.pageWindow[name];
				
				result.push(new FunctionInfo(func, functionNode));
			}
			else
			{
				log.debug("Global property is no longer a function: " + name);
			}
		}
		else
		{
			log.debug("Function no longer exists: " + name)
		}
	}
	
	return result;
}

/**
 * Create a list of function info instances, one for each function in the source
 * code. Note that special checks are done so as not to include functions
 * contained within multi-line comments. This function is used in place of
 * fullParse when Narcissus is not available.
 * 
 * @private
 * @alias Jaxer.TextParser.simpleParse
 * @param {String} source
 * 		The Javascript source code to parse
 * @return {FunctionInfo[]}
 * 		A list of function info instances, one for each Javascript function
 * 		found in the source code
 */
function simpleParse(source)
{
	// find comment ranges
	var commentRanges = TextParser.getCommentRanges(source);
	
	// find named functions not within comments
	var result = [];
	var functionMatcher = NAMED_FUNCTION_PATTERN.exec(source);
	var rangeIndex = 0;
	
	while (functionMatcher != null)
	{
		var range = new Range(functionMatcher.index, NAMED_FUNCTION_PATTERN.lastIndex);
		var insideComment = false;
		
		// NOTE: We advance until we find we're in a comment or until we've passed the end
		// of the current function. This prevents us from looping through all comments for
		// each function we find. This should result in better scaling with large files and
		// files containing a lot of comments.
		for (; rangeIndex < commentRanges.length; rangeIndex++)
		{
			var commentRange = commentRanges[rangeIndex];
			
			if (range.isOverlapping(commentRange))
			{
				insideComment = true;
				break;
			}
			else if (commentRange.start >= range.end)
			{
				break;
			}
		}
		
		if (insideComment == false)
		{
			var name = functionMatcher[1];
			var paramText = functionMatcher[2];
			var params;
			
			// process parameters
			if (paramText && WHITESPACE_PATTERN.test(paramText) == false)
			{
				params = functionMatcher[2].split(/\s*,\s*/);
			}
			else
			{
				params = [];
			}
			
			// process function body
			var startingIndex = functionMatcher.index;
			var index = startingIndex;
			var braceCount = 0;
			
			// advance to '{'
			while (index < source.length)
			{
				if (source[index] == '{')
				{
					break;
				}
				else
				{
					index++;
				}
			}
			
			// find end of function
			while (index < source.length)
			{
				var c = source[index];
				
				if (source[index] == '{')
				{
					braceCount++;
				}
				else if (source[index] == '}')
				{
					braceCount--;
					
					if (braceCount == 0)
					{
						// include close brace
						index++;
						
						// and exit loop
						break;
					}
				}
				
				index++;
			}
			
			// save function info if the function exists in global
			if (Jaxer.pageWindow.hasOwnProperty(name))
			{
				if (typeof Jaxer.pageWindow[name] == "function")
				{
					var functionSource = Jaxer.pageWindow[name].toString();
					var info = new FunctionInfo(name, params, functionSource);
					
					info.startingIndex = startingIndex;
					info.endingIndex = index;
					
					result.push(info);
				}
			}
			
			// advance over function body
			NAMED_FUNCTION_PATTERN.lastIndex = index;
		}
		
		functionMatcher = NAMED_FUNCTION_PATTERN.exec(source);
	}
	
	return result;
}

frameworkGlobal.TextParser = Jaxer.TextParser = TextParser;

Log.trace("*** TextParser.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Parsing > RunatConstants.js
 */
coreTraceMethods.TRACE('Loading fragment: RunatConstants.js');
(function(){

var consts =
{
	RUNAT_ATTR: "runat",
	SRC_ATTR: "src",
	PROXY_ATTR: "proxy",
	
	RUNAT_CLIENT: "client",
	RUNAT_SERVER_NO_CACHE: "server-nocache",
	RUNAT_SERVER_AND_CACHE: "server",
	RUNAT_SERVER_AND_PROXY: "server-proxy",
	RUNAT_BOTH_NO_CACHE: "both-nocache",
	RUNAT_BOTH_AND_CACHE: "both",
	RUNAT_BOTH_AND_PROXY: "both-proxy",
	
	RUNAT_ANY_SERVER_REGEXP: /^\s*(both|server)/i,
	RUNAT_ANY_BOTH_REGEXP: /^\s*both/i,
	
	// NOTE: This flag is temporary and should no longer be needed
	// once we have a fast JS parser in place
	SIMPLE_PARSE: true
};

for (var p in consts)
{
	frameworkGlobal[p] = Jaxer[p] = consts[p];
}

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Parsing > ScriptInfo.js
 */
coreTraceMethods.TRACE('Loading fragment: ScriptInfo.js');
(function(){

var log = Log.forModule("ScriptInfo"); // Only if Log itself is defined at this point of the includes

/**
 * This is a container object used during runat attribute and property
 * processing. This allows all runat-related state to be passed around in one
 * simple container object. At the same time, this formalizes what is being
 * passed around which is preferable to using a simple anonymous object.
 * 
 * @constructor
 * @alias Jaxer.ScriptInfo
 */
function ScriptInfo()
{
	this.functionNames = {};
	this.cacheInfos = [];
	this.proxyInfos = [];
	this.hasProxies = false;
	if (Jaxer.proxies)
	{
		var proxies = {};
		Util.foreach(Jaxer.proxies, function(proxy)
		{
			if (typeof proxy == "function" && proxy.name)
			{
				proxies[proxy.name] = true;
			}
			else if (typeof proxy == "string")
			{
				proxies[proxy] = true;
			}
			else
			{
				log.warn("The following Jaxer.proxies element was neither a named function nor a string: " + proxy);
			}
		});
		this.jaxerProxies = proxies;
	}
	else
	{
		this.jaxerProxies = null;
	}
}

frameworkGlobal.ScriptInfo = Jaxer.ScriptInfo = ScriptInfo;

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Parsing > ScriptProcessor.js
 */
coreTraceMethods.TRACE('Loading fragment: ScriptProcessor.js');
(function(){

var log = Log.forModule("ScriptProcessor"); // Only if Log itself is defined at this point of the includes

/**
 * This class processes script elements to determine which of its functions fall
 * into the various runat categories. This class collects each function into
 * its appropriate category and provides convenience functions to generate
 * the necessary source code for this block needed for both client-side and
 * server-side use.
 * 
 * @constructor
 * @alias Jaxer.ScriptProcessor
 * @param {ScriptElement} element
 * 		The underlying script element to be processed by this class
 * @param {object} jaxerProxies
 * 		If a non-null object, its properties are the function names that should be proxied
 * 		even if not explicitly marked as such.
 * 		If null, no functions should be proxied.
 */
function ScriptProcessor(element, jaxerProxies)
{
	this.element = element;
	this.jaxerProxies = jaxerProxies;
	
	this.clientInfos = [];
	this.proxyInfos = [];
	this.bothProxyInfos = [];
	this.cacheInfos = [];
	
	this.ast = null;
	this.source = "";
	this.localFunctionNames = {};
	this.remove = false;
	this.hasProxies = false;
}

/**
 * Generates the source code for all functions that will live as callbacks on
 * the server and as client-side code. This effectively generates the original
 * function source for the client as well as a namespaced version of the
 * function which can be used to call the server-side function from the client.
 * 
 * @method
 * @alias Jaxer.ScriptProcessor.prototype.getBothClientSource
 * @return {String}
 * 		The source code of all functions in this underlying script element
 */
ScriptProcessor.prototype.getBothClientSource = function getBothClientSource()
{
	var source;
	
	if (SIMPLE_PARSE)
	{
		source = this.removeRunatAssignmentsSimple();
	}
	else
	{
		source = this.removeRunatAssignments();
	}
	
	if (this.proxyInfos.length > 0 || this.bothProxyInfos.length > 0)
	{
		this.hasProxies = true;
		
		var sources = [source];
		
		// emit proxies
		Util.foreach(this.proxyInfos, function(info)
		{
			sources.push(info.createNamespacedClientCallback());
		});
		
		// emit proxies
		Util.foreach(this.bothProxyInfos, function(info)
		{
			sources.push(info.createNamespacedClientCallback());
		});
		
		source = sources.join("\n");
	}
	
	return source;
};

/**
 * Generates the source code for all functions that will execute on the client
 * only. This comes into play, for example, when a server script element has
 * tagged one of its containing functions for client-side execution. Other cases
 * include dynamic generation of functions that did not actually exist in the
 * script element's source text.
 * 
 * @method
 * @alias Jaxer.ScriptProcessor.prototype.getClientSource
 * @return {String}
 * 		The source code of all functions in this underlying script element
 */
ScriptProcessor.prototype.getClientSource = function getClientSource()
{
	var sources = [];
			
	// emit client functions
	Util.foreach(this.clientInfos, function(clientInfo)
	{
		sources.push(clientInfo.createClientSource());
	});
	
	// emit proxies
	if (this.proxyInfos.length > 0)
	{
		this.hasProxies = true;
			
		Util.foreach(this.proxyInfos, function(info)
		{
			sources.push(info.createClientCallback());
		});
	}
	
	// emit namespaced proxies
	if (this.bothProxyInfos.length > 0)
	{
		this.hasProxies = true;
		
		Util.foreach(this.bothProxyInfos, function(info)
		{
			sources.push(info.createNamespacedClientCallback());
		});
	}
	
	return sources.join("\n");
};

/**
 * Determine the value of the underlying script elements runat attribute. If the
 * attribute does not exist, then this method will return the default value of
 * "client"
 * 
 * @method
 * @alias Jaxer.ScriptProcessor.prototype.getRunatAttribute
 * @return {String}
 * 		The runat attribute value 
 */
ScriptProcessor.prototype.getRunatAttribute = function getRunatAttribute()
{
	var result = RUNAT_CLIENT;
	
	if (this.element.hasAttribute(RUNAT_ATTR))
	{
		// get attribute value
		result = this.element.getAttribute(RUNAT_ATTR);
		
		// remove attribute
		this.element.removeAttribute(RUNAT_ATTR);
	}
	
	return result;
};

/**
 * Process the content of all functions inside this instance's underlying script
 * element. The script element's runat attribute is applied to all function
 * instances that exist in global. FunctionInfo's are created for all of those
 * functions. Any changes to the role of a function in this script element are
 * updated. For example, if a script element is tagged to run on the server, but
 * upon execution, if one of the functions in that element is tagged to run
 * client-side, then the content of the script element served to the client will
 * contain only that one client-side function. If that function has not been
 * tagged for client-side execution, the entire script element would have been
 * removed since the functions were intended to run on the server only.
 * 
 * @method
 * @alias Jaxer.ScriptProcessor.prototype.process
 */
ScriptProcessor.prototype.process = function process()
{
	var runatAttr = this.getRunatAttribute();

	if (runatAttr != RUNAT_CLIENT)
	{
		// get source
		if (this.element.hasAttribute(SRC_ATTR))
		{
			// grab file contents here
			this.source = "";
		}
		else
		{
			this.source = this.element.innerHTML;
		}
		
		// get function infos
		var functionInfos = TextParser.getFunctionInfos(this.source);
		
		if (SIMPLE_PARSE == false)
		{
			// needed for removal of runat properties
			this.ast = TextParser.ast;
		}
		
		for (var i = 0; i < functionInfos.length; i++)
		{
			var functionInfo = functionInfos[i];
			var name = functionInfo.name;
			
			// keep track of what we've visited in this script element
			this.localFunctionNames[name] = true;
			
			// get a reference to the function
			var func = Jaxer.pageWindow[name];
			
			// determine the runat and proxy values
			var runatProp = (func.hasOwnProperty(RUNAT_ATTR)) ? func[RUNAT_ATTR] : runatAttr;
			var proxyProp = CallbackManager.calculateProxyAttribute(func, this.jaxerProxies);
			log.debug("Processing function '" + name + "' with runat = " + runatProp + " and proxy = " + proxyProp);
		
			// process
			this.processFunctionInfo(functionInfo, runatProp, proxyProp);
		}
		
		// build source for client-bound script element
		var isBothVariant = runatAttr.match(RUNAT_ANY_BOTH_REGEXP);
		var source;
		
		if (isBothVariant)
		{
			source = this.getBothClientSource();
		}
		else
		{
			source = this.getClientSource();
		}
		
		// set new script element source or remove it if there is none
		if (source != "")
		{
			if (this.element.hasAttribute(SRC_ATTR))
			{
				this.element.removeAttribute(SRC_ATTR);
			}
			this.element.innerHTML = "\n" + source + "\n\t\t";
		}
		else
		{
			if (isBothVariant == false || this.element.hasAttribute(SRC_ATTR) == false)
			{
				// tag script to be removed during post-processing
				this.remove = true;
			}
		}
	}
};

/**
 * NOT CURRENTLY USED
 * This is a convenience function which creates a FunctionInfo from a Narcissus
 * AST node and then calls the processFunctionInfo method.
 * 
 * @method
 * @alias Jaxer.ScriptProcessor.prototype.processFunctionNode
 * @param {Node} functionNode
 * 		A top-level Narcissus AST node representing the parse tree of the
 * 		specified function
 * @param {Function} functionReference
 * 		A reference to the actual Javascript function instance
 * @param {String} runatProperty
 * 		The runat property value active for this function
 */
ScriptProcessor.prototype.processFunctionNode = function processFunctionNode(functionNode, functionReference, runatProperty)
{
	var functionInfo = new FunctionInfo(functionReference, functionNode);
	
	this.processFunctionInfo(functionInfo, runatProperty, null);
};

/**
 * This method is responsible for classifying each function based on the
 * specified runat and proxy property values. The classifications are later used for
 * emission of client-side functions, proxies, and callback storage.
 * 
 * @method
 * @alias Jaxer.ScriptProcessor.prototype.processFunctionInfo
 * @param {FunctionInfo} functionInfo
 * 		A function info instance representing a Javascript function
 * @param {String} runatProperty
 * 		The runat property value active for this function
 * @param {String} proxyProperty
 * 		The proxy property value active for this function, or null if there isn't one
 */
ScriptProcessor.prototype.processFunctionInfo = function processFunctionInfo(functionInfo, runatProperty, proxyProperty)
{
	switch (runatProperty)
	{
		case RUNAT_CLIENT:
			this.clientInfos.push(functionInfo);
			break;
			
		case RUNAT_BOTH_NO_CACHE:
			this.clientInfos.push(functionInfo);
			if (proxyProperty == true) // force both cache and proxy
			{
				this.cacheInfos.push(functionInfo);
				this.bothProxyInfos.push(functionInfo);
			}
			break;
			
		case RUNAT_BOTH_AND_CACHE:
			this.clientInfos.push(functionInfo);
			this.cacheInfos.push(functionInfo);
			if (proxyProperty == true) // force proxy
			{
				this.bothProxyInfos.push(functionInfo);
			}
			break;
			
		case RUNAT_BOTH_AND_PROXY:
			this.cacheInfos.push(functionInfo);
			this.clientInfos.push(functionInfo);
			if (proxyProperty != false)
			{
				this.bothProxyInfos.push(functionInfo);
			}
			break;
			
		case RUNAT_SERVER_NO_CACHE:
			if (proxyProperty == true) // force proxy
			{
				this.cacheInfos.push(functionInfo);
				this.proxyInfos.push(functionInfo);
			}
			break;
			
		case RUNAT_SERVER_AND_CACHE:
			this.cacheInfos.push(functionInfo);
			if (proxyProperty == true) // force proxy
			{
				this.proxyInfos.push(functionInfo);
			}
			break;
			
		case RUNAT_SERVER_AND_PROXY:
			this.cacheInfos.push(functionInfo);
			if (proxyProperty != false)
			{
				this.proxyInfos.push(functionInfo);
			}
			break;
		
		default:
			log.debug("Unrecognized " + RUNAT_ATTR + " property value: " + runatProperty);
			break;
	}
};

/**
 * This method removes the underlying script element from the DOM.
 * 
 * @method
 * @alias Jaxer.ScriptProcessor.prototype.removeElement
 */
ScriptProcessor.prototype.removeElement = function removeElement()
{
	if (this.remove && this.element)
	{
		this.element.parentNode.removeChild(this.element);
		this.element = null;
	}
};

/**
 * This method removes all runat property assignments in the underlying script
 * element's source. This prevents the runat property from being exposed on the
 * client. This method relies on an underlying Narcissus AST node to function
 * properly.
 * 
 * @method
 * @alias Jaxer.ScriptProcessor.prototype.removeRunatAssignments
 * @return {String}
 * 		The original source code with all runat assignments whited out
 */
ScriptProcessor.prototype.removeRunatAssignments = function removeRunatAssignments()
{
	var source = this.source;
	var localFunctionNames = this.localFunctionNames;
	
	// get all runat properties
	var assignments = this.ast.select(
		"/semicolon/assign[1]",
		function(node)
		{
			var lhs = node[0];
			var result = false;
			
			if (lhs.type == DOT)
			{
				var rhs = lhs[1];
				var lhs = lhs[0];
				
				result = (lhs.type == IDENTIFIER && rhs.value == RUNAT_ATTR && localFunctionNames.hasOwnProperty(lhs.value));
			}
			
			return result;
		}
	);
	
	// blot out each statement
	for (var i = 0; i < assignments.length; i++)
	{
		var node = assignments[i];
		var start = node.start;
		var end = node.end;
		
		// Following semicolons are not included in the node range. Advance
		// until we hit a semicolon, or something other than a tab or space
		while (end < source.length)
		{
			var c = source[end];
			
			if (c == ' ' || c == '\t')
			{
				end++;
			}
			else
			{
				if (c == ';')
				{
					end++;
				}
				
				break;
			}
		}
		
		// grab the source before and after the assignment
		var before = source.substring(0, start);
		var after = source.substring(end, source.length);
		
		// create a string of spaces to match the size of the assignment
		var padding = (new Array(end - start + 1)).join(" ");
		
		// recreate the source with the padding applied over the assignment
		source = [before, padding, after].join("");
	}
	
	return source;
};

/**
 * This method removes all runat property assignments in the underlying script
 * element's source. This prevents the runat property from being exposed on the
 * client. This method is used to remove these assignments when we do not have
 * a Narcissus AST node for this script element's source.
 * 
 * @method
 * @alias Jaxer.ScriptProcessor.prototype.removeRunatAssignmentsSimple
 * @return {String}
 * 		The original source code with all runat assignements whited out
 */
ScriptProcessor.prototype.removeRunatAssignmentsSimple = function removeRunatAssignmentsSimple()
{
	// start with the original source
	var source = this.source;
	var localFunctionNames = this.localFunctionNames;
	if (localFunctionNames.length == 0)
	{
		return source; // There's nothing to remove
	}
	
	// build list of names
	var names = [];
	
	for (var name in localFunctionNames)
	{
		names.push(name);
	}
	
	names.sort().reverse();
	
	// build runat property regex (note names cannot be empty by this point)
	var runatPattern = new RegExp("(" + names.join("|") + ")\\." + RUNAT_ATTR + "\\s*=\\s*\"(\\\\.|[^\"\\\\\\n\\r])*?\"(\\s*;)?", "g");
	// replace
	var match = runatPattern.exec(source);
	
	while (match !== null)
	{
		var start = match.index;
		var end = runatPattern.lastIndex;
		
		// grab the source before and after the assignment
		var before = source.substring(0, start);
		var after = source.substring(end, source.length);
		
		// create a string of spaces to match the size of the assignment
		var padding = (new Array(end - start + 1)).join(" ");
		
		// recreate the source with the padding applied over the assignment
		source = [before, padding, after].join("");
		
		match = runatPattern.exec(source);
	}
	
	return source;
};

frameworkGlobal.ScriptProcessor = Jaxer.ScriptProcessor = ScriptProcessor;

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework/Parsing > CallbackManager.js
 */
coreTraceMethods.TRACE('Loading fragment: CallbackManager.js');
(function(){

var log = Log.forModule("CallbackManager"); // Only if Log itself is defined at this point of the includes

// private variables
var document;

// create callback manager container
var CallbackManager = {};

/**
 * Process all functions for possible caching, proxying, and callback
 * invocation.
 * @alias Jaxer.CallbackManager.processCallbacks
 * @param {Document} doc
 * 		The current document's DocumentElement. This is used to create elements
 * 		in the active DOM as needed.
 */
CallbackManager.processCallbacks = function processCallbacks(doc)
{
	// save reference to document
	document = doc;
	
	var scriptsInfo = new ScriptInfo();
	
	// process all script elements in the document
	processScriptElements(scriptsInfo);
	
	// processs all globally defined functions
	processGlobalFunctions(scriptsInfo);
	
	// cache server-side functions for future callbacks
	cacheServerFunctions(scriptsInfo);
};

/**
 * Once all functions have been processed, we need to make sure all cached and
 * proxied functions are added to the database so we can reconstitute those
 * functions during callbacks.
 * 
 * @private
 * @alias Jaxer.CallbackManager.cacheServerFunctions
 * @param {ScriptInfo} scriptsInfo
 * 		The active scriptInfo being used to manage state in this callback
 * 		manager session
 */
function cacheServerFunctions(scriptsInfo)
{
	log.trace("Going to cache " + scriptsInfo.cacheInfos.length + " functions...");
	if (scriptsInfo.cacheInfos.length > 0)
	{
		
		Jaxer.response.noteSideEffect();
		
		// build source
		var sources = [];
		
		// get page key
		var pageName = Route.getAppAndPage().page;
		var pageKey = CallbackManager.pageNameToKey(pageName);
		var namespace = "Jaxer.callbackPages['" + pageKey + "']";

		// Build a hashmap to quickly identify which cached functions should be client-callable
		var callables = {};
		Util.foreach(scriptsInfo.proxyInfos, function(info)
		{
			callables[info.name] = true;
		});
		
		// accumulate functions to cache on the server
		Util.foreach(scriptsInfo.cacheInfos, function(info)
		{
			var isCallable = callables.hasOwnProperty(info.name);
			if (info.isNative())
			{
				if (isCallable)
				{
					throw new Exception("Cannot cache and proxy a native function: " + uneval(info));
				}
				else
				{
					log.warn("Cannot cache a native function -- skipping: " + uneval(info));
				}
			}
			else
			{
				sources.push(info.createServerFunction(namespace, isCallable));
			}
		});
		
		// append oncallback function, if it exists
		if (typeof Jaxer["oncallback"] == "function")
		{
			var onCallbackInfo = new FunctionInfo(Jaxer["oncallback"]);
			if (onCallbackInfo.isNative()) // We cannot just issue a warning and ignore, because it'll need to be on the Jaxer namespace when we come back
			{
				throw new Exception("Cannot cache oncallback because it is a native function: " + uneval(onCallbackInfo));
			}
			sources.push(onCallbackInfo.createServerFunction(namespace, false, true));
		}
		
		var source = sources.join("\n");
				
		// calculate crc32
		var crc32 = Util.CRC32.getStringCRC(source);
		
		// determine if we have a page like this already in the db
		var rs = DB.frameworkExecute(
			"SELECT COUNT(*) FROM callback_page WHERE name = ? AND crc32 = ?",
			[pageName, crc32]
		);
		var countExisting = rs.singleResult;
		
		// if we don't, then add it to the db
		if (countExisting == 0)
		{
			log.debug("Storing this page's callbacks in the DB with name='" + pageName + "' and crc32=" + crc32);
			
			// build field list
			var fields = [
				'crc32',
				'name',
				'value',
				'document_root',
				'page_file',
				'creation_datetime',
				'access_datetime',
				'access_count'
			];
			
			// build value list
			var values = [
				crc32,
				pageName,
				source,
				Jaxer.request.documentRoot,
				Jaxer.request.pageFile,
				new Date(),
				null,
				0
			];

			var placeholders = Util.map(values, function(field) { return "?"; });
			
			// build query
			query = "INSERT INTO callback_page(" + fields.join(",") + ") VALUES(" + placeholders + ")";
			
			// commit source to db
			DB.frameworkExecute(query, values);
		}
		
		// determine whether we've cached this page already, else cache it
		if (!Jaxer.callbackPages.hasOwnProperty(pageKey) ||
			Jaxer.callbackPageCRCs[pageKey] != crc32)
		{
			log.debug("Caching this page's callbacks under pageKey=" + pageKey + " and crc32=" + crc32);
			CallbackManager.cacheCallbackPage(pageKey, crc32, source);
		}
		
		if (scriptsInfo.hasProxies)
		{
			Jaxer.response.noteDomTouched();
			// Pass callback-related parameters to the client
			var head = document.getElementsByTagName("head")[0];
			Util.DOM.insertScriptAtBeginning(
				document, 
				[
					"Jaxer.Callback.id = " + crc32,
				 	"Jaxer.CALLBACK_URI = '" + Jaxer.Config.CALLBACK_URI + "'",
					"Jaxer.ALERT_CALLBACK_ERRORS = " + Boolean(Jaxer.Config.ALERT_CALLBACK_ERRORS)
				].join("; ") + ";",
				head,
				null
			);
		}
	}
}

/**
 * Generates a key out of the page name
 * @alias Jaxer.CallbackManager.pageNameToKey
 * @param {String} pageName
 * 	The name (URL) of the page
 * @return {String} the text-only key we will use
 */
CallbackManager.pageNameToKey = function pageNameToKey(pageName)
{
	return "page_" + Util.CRC32.getStringCRC(pageName).toString().replace(/\D/g, "_");
}

/**
 * Cache the callback page in memory for faster execution on callback
 * @alias Jaxer.CallbackManager.cacheCallbackPage
 * @param {String} pageKey
 * 	The key by which to store/retrieve the code page
 * @param {String} crc32
 * 	Used to see whether the code has changed
 * @param {String} source
 * 	The JavaScript source code (function definitions) to execute
 */
CallbackManager.cacheCallbackPage = function cacheCallbackPage(pageKey, crc32, source)
{
	Jaxer.callbackPages[pageKey] = {};
	Jaxer.callbackGlobal.__eval("(function(){\n" + source + "\n})();");
	Jaxer.callbackPageCRCs[pageKey] = crc32;
	Jaxer.callbackPagesByCRC[crc32] = pageKey;
}

/**
 * Visit all functions hanging off of global and process each one that has a
 * runat property defined. This approach allows server-executed code to modify
 * the role a function plays regardless of its containing script elements runat
 * attribute value. This also allows functions to be generated on the fly and
 * for functions to change roles based on queries performed in the server-side
 * code.
 * 
 * @private
 * @alias Jaxer.CallbackManager.processGlobalFunctions
 * @param {ScriptInfo} scriptsInfo
 * 		The active scriptInfo being used to manage state in this callback
 * 		manager session
 */
function processGlobalFunctions(scriptsInfo)
{
	var processor = new ScriptProcessor(null, scriptsInfo.jaxerProxies);
	var handledInScripts = scriptsInfo.functionNames;
	
	// only process functions not handled in script elements
	function shouldHandle(name, value)
	{
		return !handledInScripts.hasOwnProperty(name) &&
			typeof value == "function" && 
			value.hasOwnProperty(RUNAT_ATTR);
	}
	
	var globalFunctionNames = Util.getPropertyNames(Jaxer.pageWindow, shouldHandle);
	Util.foreach(globalFunctionNames, function processFunction(name)
	{
		var candidate = Jaxer.pageWindow[name];
		var source = candidate.toString(); // get function source
		var info = null;
		
		if (SIMPLE_PARSE) 
		{
			log.trace("source before: " + source);
			if (source.indexOf("function (") == 0) 
			{
				source = source.replace(/^function \(/, "function " + name + "(");
			}
			else
			{
				source = source.replace(/^function \w+\(/, "function " + name + "(");
			}
			log.trace("source after: " + source);
			
			info = new FunctionInfo(name, [], source);
		}
		else 
		{
			// parse source into a function info
			var infos = TextParser.getFunctionInfos(source);
			
			if (infos.length > 0) 
			{
				info = infos[0];
			}
		}
		
		if (info) 
		{
			// get runat and proxy attributes
			var runat = candidate[RUNAT_ATTR];
			var proxy = CallbackManager.calculateProxyAttribute(candidate, scriptsInfo.jaxerProxies);
			log.debug("Processing function '" + name + "' with runat = " + runat + " and proxy = " + proxy);
			
			// process content
			processor.processFunctionInfo(info, runat, proxy);
		}
		else 
		{
			log.error("Error parsing '" + name + "': " + source);
		}
	})
	
	// NOTE: This has to be called before you can rely on hasProxies
	var source = processor.getClientSource();
	
	if (source != "")
	{
		// add client script element
		Util.DOM.insertScriptAtEnd(
			document,
			"\n" + source + "\n\t\t",
			document.getElementsByTagName("head")[0],
			null,
			true
		);
	}
	
	// see if we had proxies
	if (processor.hasProxies)
	{
		scriptsInfo.hasProxies = true;
	}
	
	// accumulate cache and proxy infos
	scriptsInfo.cacheInfos = scriptsInfo.cacheInfos.concat(processor.cacheInfos);
	scriptsInfo.proxyInfos = scriptsInfo.proxyInfos.concat(processor.proxyInfos, processor.bothProxyInfos);
}

/**
 * Process each script element in this page. Each function in each script
 * element is visited. All processing is delegated to the
 * ScriptProcessor.process method.
 * 
 * @private
 * @alias Jaxer.CallbackManager.processScriptElements
 * @param {ScriptInfo} scriptInfo
 * 		The active scriptInfo being used to manage state in this callback
 * 		manager session
 */
function processScriptElements(scriptsInfo)
{
	try
	{
		// process each script element
		var processors = Util.map(
			document.getElementsByTagName("script"),
			function (element) {
				var processor = new ScriptProcessor(element, scriptsInfo.jaxerProxies);
				processor.process();
				return processor;
			}
		);
		
		// perform any post-processing
		Util.foreach(
			processors,
			function (processor) {
				// see if we had proxies
				if (processor.hasProxies)
				{
					scriptsInfo.hasProxies = true;
				}
				
				// accumulate cacheInfos, proxyInfos
				scriptsInfo.cacheInfos = scriptsInfo.cacheInfos.concat(processor.cacheInfos);
				scriptsInfo.proxyInfos = scriptsInfo.proxyInfos.concat(processor.proxyInfos, processor.bothProxyInfos);
				
				// add top-level function names to accumulated list of names
				for (var p in processor.localFunctionNames)
				{
					scriptsInfo.functionNames[p] = processor.localFunctionNames[p];
				}
				
				// remove script element from document, if needed
				processor.removeElement();
			}
		);
	}
	catch(e)
	{
		log.error("Error: " + e);
	}
}

/**
 * Calculates the effective proxy attribute of a function based on settings on the function
 * and on global settings.
 * 
 * @private
 * @alias Jaxer.CallbackManager.calculateProxyAttribute
 * @param {Function} func
 * 		The function whose proxy attribute is to be calculated
 * @param {Object} jaxerProxies
 * 		An object containing names (as its properties) of functions to implicitly proxy.
 * 		If null, no functions will be proxied.
 */
CallbackManager.calculateProxyAttribute = function calculateProxyAttribute(func, jaxerProxies)
{
	if (jaxerProxies == null)
	{
		return false;
	}
	else if (jaxerProxies.hasOwnProperty(func.name))
	{
		return true;
	}
	else if (func.hasOwnProperty(PROXY_ATTR) &&
	 		 typeof func[PROXY_ATTR] == "boolean")
	{
		return func[PROXY_ATTR];
	}
	else
	{
		return null;
	}
}

frameworkGlobal.CallbackManager = Jaxer.CallbackManager = CallbackManager;

Log.trace("*** CallbackManager.js loaded");

})();

/*
 * fragment : /Users/kevin/Documents/Workspaces/jaxer/js_framework > Utilities/Both.js
 */
coreTraceMethods.TRACE('Loading fragment: Utilities/Both.js');
/**
 * Used to set events on DOM elements such that they "do the right thing" both client-side and server-side.
 * On the client, this acts as expected, setting a property with the name eventName (e.g. onclick) on the DOM element.
 * On the server, the eventName attribute is set on the DOM element so it can be serialized with the DOM before sending to the client.
 * If the handler is a (server side) function with a name, the attribute's value is handler.name + "()"
 * On the server, 
 * @alias Jaxer.setEvent
 * @param {Object} domElement
 * 	The element on which to set the event
 * @param {String} eventName
 * 	The name of the event to set
 * @param {Object} handler
 * 	The handler function, or the body (as a string)
 */
Jaxer.setEvent = function setEvent(domElement, eventName, handler)
{
	if (Jaxer.isOnServer)
	{
		var attribute;
		if (typeof handler == "function")
		{
			if (handler.name == "")
			{
				attribute = "(" + handler.toSource() + ")()";
			}
			else
			{
				attribute = handler.name + "()";
			}
		}
		else // handler should be a string (the handler function's body)
		{
			attribute = handler;
		}
		domElement.setAttribute(eventName, attribute);
	}
	else
	{
		var func;
		if (typeof handler == "function")
		{
			func = handler;
		}
		else // handler should be a string (the handler function's body)
		{
			func = new Function(handler);
		}
		domElement[eventName] = func;
	}
};

/**
 * Sets the title of the document and works on either the server or the client. 
 * @alias Jaxer.setTitle
 * @param {String} title
 * 	The text of the title
 */
Jaxer.setTitle = function setTitle(title)
{
	if (Jaxer.isOnServer)
	{
		var doc = Jaxer.pageDocument;
		var titleElement = doc.getElementsByTagName("title")[0];
		if (!titleElement)
		{
			var head = doc.getElementsByTagNames("head")[0];
			if (head)
			{
				titleElement = doc.createElement("title");
				head.appendChild(titleElement);
			}
		}
		if (titleElement)
		{
			titleElement.firstChild.data = title;
		}
	}
	else
	{
		document.title = title;
	}
};