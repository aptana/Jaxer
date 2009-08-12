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
 * @namespace {Jaxer.Util.String} Namespace that holds functions and other objects that extend JavaScript's string capabilities.
 */
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
 * Left or right trim the provided string. Optionally, you can specify a list of
 * characters (as a single string) to trim from the source string. By default,
 * whitespace is removed. Also, you can control which side of the string (start
 * or end) is trimmed with the default being both sides.
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
 * @param {String} inString
 * 		The string to look in
 * @param {String} lookFor
 * 		The string to look for
 * @param {Boolean} ignoreCase
 * 		Set to true for case insensitive searches
 * @return {Boolean}
 * 		true if the string starts with the provided string
 */
Util.String.startsWith = function startsWith(inString, lookFor, ignoreCase)
{
	if (ignoreCase)
	{
		inString = inString.toLowerCase();
		lookFor = lookFor.toLowerCase();
	}
	return (inString.indexOf(lookFor) == 0);
};

/**
 * Check to see if a string ends with a string
 * 
 * @alias Jaxer.Util.String.endsWith
 * @param {String} inString
 * 		The string to look in
 * @param {String} lookFor
 * 		The string to look for
 * @param {Boolean} ignoreCase
 * 		Set to true for case insensitive searches
 * @return {Boolean}
 * 		true if the string ends with the provided string
 */
Util.String.endsWith = function endsWith(inString, lookFor, ignoreCase)
{
	if (ignoreCase)
	{
		inString = inString.toLowerCase();
		lookFor = lookFor.toLowerCase();
	}
	return (inString.lastIndexOf(lookFor) == (inString.length - lookFor.length));
};

/**
 * Searches the given lines for the given pattern, and returns the lines that matched.
 * 
 * @alias Jaxer.Util.String.grep
 * @param {String, String[]} stringOrArray
 * 		The string to search through, which will be split up into its lines, 
 * 		or an array of lines (i.e. a string that has already been split)
 * @param {String, RegExp} pattern
 * 		The string pattern to look for, which will be turned into a RegExp,
 * 		or the RegExp to match
 * @return {String[]}
 * 		An array of the lines that matched the pattern
 */
Util.String.grep = function grep(stringOrArray, pattern)
{
	if (typeof stringOrArray == "string")
	{
		stringOrArray = stringOrArray.split("\r?\n");
	}
	if (typeof pattern == "string")
	{
		pattern = new RegExp(pattern);
	}
	return stringOrArray.filter(function(item) { return pattern.test(item); });
}

})();
