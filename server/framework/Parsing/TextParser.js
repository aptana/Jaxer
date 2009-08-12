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
 * @classDescription {Range} Represents a block of text within a string.
 */

/**
 * The Range class is used to represent a block of text within a string.
 * 
 * @private
 * @constructor
 * @alias Range
 * @param {Number} start
 * 		The offset to the beginning of the range
 * @param {Number} end
 * 		The offset to the end of the range
 * @return {Range}
 * 		Returns an instance of Range.
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
 * 
 * @private
 * @alias Range.prototype.contains
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
 * 
 * @private
 * @alias Range.prototype.isEmpty
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
 * 
 * @private
 * @alias Range.prototype.isOverlapping
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

var log = Log.forModule("TextParser");

var JS_COMMENT_PATTERN = /(?:\/\*(?:.|[\r\n])+?\*\/)|(?:\/\/[^\r\n]*)/mg;
var NAMED_FUNCTION_PATTERN = /function\s+([\w\$]+)\s*\(([^\)]*)\)/mg;
var WHITESPACE_PATTERN = /^\s+$/;

/**
 * @namespace {Jaxer.TextParser} Namespace object holding functions and members
 * used for simple parsing of source code for reading runat values, proxy
 * values, which functions were defined in which script blocks, etc.
 * @advanced
 */
var TextParser = {};

/**
 * Find all comments in the specified source and return an array of Ranges, one
 * for each comment.
 * 
 * @alias Jaxer.TextParser.getCommentRanges
 * @private
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
 * @private
 * @param {String} source
 * 		The Javascript source code to parse
 * @return {Jaxer.FunctionInfo[]}
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
 * NOT CURRENTLY USED
 * Use a parser to create an AST of the specified source.
 * 
 * @private
 * @alias Jaxer.TextParser.fullParse
 * @param {String} source
 * 		The Javascript source code to parse
 * @return {Node}
 * 		The resulting parser AST of the source code
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
 * fullParse when a parser is not available.
 * 
 * @private
 * @alias Jaxer.TextParser.simpleParse
 * @param {String} source
 * 		The Javascript source code to parse
 * @return {Jaxer.FunctionInfo[]}
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
