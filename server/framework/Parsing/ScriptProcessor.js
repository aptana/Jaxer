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

var log = Log.forModule("ScriptProcessor"); // Only if Log itself is defined at this point of the includes

/**
 * @classDescription {Jaxer.ScriptProcessor} This class processes script
 * elements to determine which of its functions fall into the various runat
 * categories.
 */

/**
 * This class processes script elements to determine which of its functions fall
 * into the various runat categories. This class collects each function into
 * its appropriate category and provides convenience functions to generate
 * the necessary source code for this block needed for both client-side and
 * server-side use.
 * 
 * @advanced
 * @constructor
 * @alias Jaxer.ScriptProcessor
 * @param {ScriptElement} element
 * 		The underlying script element to be processed by this class
 * @param {Object} jaxerProxies
 * 		If a non-null object, its properties are the function names that should
 * 		be proxied even if not explicitly marked as such. If null, no functions
 * 		should be proxied.
 * @return {Jaxer.ScriptProcessor}
 * 		Returns an instance of ScriptProcessor.
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
	this.srcAttr = null;
	this.localFunctionNames = {};
	this.functionInfos = [];
	this.runatAttr = null;
	this.isBothVariant = false;
	this.hasProxies = false;
}

/**
 * Generates the source code for all functions that will live as callbacks on
 * the server and as client-side code. This effectively generates the original
 * function source for the client as well as a namespaced version of the
 * function which can be used to call the server-side function from the client.
 * 
 * @private
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
		
		for (var i = 0; i < this.functionInfos.length; i++)
		{
			var functionInfo = this.functionInfos[i];
			
			if (functionInfo.removeFromClient)
			{
				// grab the source before and after the assignment
				var start = functionInfo.startingIndex;
				var end = functionInfo.endingIndex;
				var before = source.substring(0, start);
				var after = source.substring(end, source.length);
				
				// create a string of spaces to match the size of the assignment
				var padding = (new Array(end - start + 1)).join(" ");
				
				// recreate the source with the padding applied over the assignment
				source = [before, padding, after].join("");
			}
		}
	}
	else
	{
		source = this.removeRunatAssignments();
	}
	
	if (this.proxyInfos.length > 0 || this.bothProxyInfos.length > 0)
	{
		this.hasProxies = true;
		
		var sources = [source];
		
		if (this.proxyInfos.length > 0) 
		{
			var names = this.proxyInfos.map(function(info) { return info.name; });
			sources.push("eval(Jaxer.Callback.createProxies(['" + names.join("', '") + "']));");
		}
		
		if (this.bothProxyInfos.length > 0)
		{
			var namespacedNames = this.bothProxyInfos.map(function(info) { return info.name; });
			sources.push("eval(Jaxer.Callback.createProxies(['" + namespacedNames.join("', '") + "'], 'Jaxer.Server'));");
		}
		
		source = sources.join("\n");
	}
	
	// collapse to empty string if we only have whitespace
	if (source.match(/^\s+$/))
	{
		source = "";
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
 * @private
 * @alias Jaxer.ScriptProcessor.prototype.getClientSource
 * @return {String}
 * 		The source code of all functions in this underlying script element
 */
ScriptProcessor.prototype.getClientSource = function getClientSource()
{
	var sources = [];
			
	// emit client functions
	this.clientInfos.forEach(function(clientInfo)
	{
		sources.push(clientInfo.createClientSource());
	});
	
	// emit proxies
	if (this.proxyInfos.length > 0)
	{
		this.hasProxies = true;
			
		var names = this.proxyInfos.map(function(info) { return info.name; });
		sources.push("eval(Jaxer.Callback.createProxies(['" + names.join("', '") + "']));");
	}
	
	// emit namespaced proxies
	if (this.bothProxyInfos.length > 0)
	{
		this.hasProxies = true;
		
		var namespacedNames = this.bothProxyInfos.map(function(info) { return info.name; });
		sources.push("eval(Jaxer.Callback.createProxies(['" + namespacedNames.join("', '") + "'], 'Jaxer.Server'));");
	}
	
	return sources.join("\n");
};

/**
 * Determine the value of the underlying script elements runat attribute. If the
 * attribute does not exist, then this method will return the default value of
 * "client"
 * 
 * @private
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
	}
	
	return result;
};

/**
 * Removes any runat attribute, and also the type attribute if present
 * and if the runat attribute is server-only
 * 
 * @private
 * @alias Jaxer.ScriptProcessor.prototype.removeRunatAttribute
 */
ScriptProcessor.prototype.removeRunatAttribute = function removeRunatAttribute()
{
	if (this.element.hasAttribute(RUNAT_ATTR))
	{
		var runatAttr = this.element.getAttribute(RUNAT_ATTR);
		// remove attribute
		this.element.removeAttribute(RUNAT_ATTR);
		// then see if we need to do more cleanups
		var isBothVariant = runatAttr.match(RUNAT_ANY_BOTH_REGEXP);
		// -If it's not designated by the developer as "both" then any
		// client-bound code injected by Jaxer should not have a type
		// attribute, in case the developer's type attribute was inappropriate
		// for all clients. 
		// - If the developer did designate as "both" and had a type
		// attribute they must have meant that type value to go to the client.
		if (!isBothVariant && runatAttr != RUNAT_CLIENT)
		{
			this.element.removeAttribute(TYPE_ATTR);
		}
		Jaxer.response.noteDomTouched();
	}
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
 * @private
 * @alias Jaxer.ScriptProcessor.prototype.process
 */
ScriptProcessor.prototype.process = function process()
{

	this.srcAttr = this.element.getAttribute(SRC_ATTR);
	this.runatAttr = this.getRunatAttribute();
	this.removeRunatAttribute();

	if (this.runatAttr != RUNAT_CLIENT)
	{

		this.source = (this.srcAttr == null) ? this.element.innerHTML : '';
		var shortContents = (this.source == null) ? "no contents" : "contents='" + this.source.substr(0, 100) + (this.source.length > 100 ? "..." : "") + "'";
		log.debug("Post-processing script block" + 
			" with " + ((this.srcAttr == null) ? "no src" : "src='" + this.srcAttr + "'") +
			" and " + ((this.runatAttr == null) ? "no runat" : "runat='" + this.runatAttr + "'") +
			" and " + shortContents);
					
		// get function infos
		this.functionInfos = TextParser.getFunctionInfos(this.source);
		log.debug("Found " + this.functionInfos.length + " functions to process");
		
		if (SIMPLE_PARSE == false)
		{
			// needed for removal of runat properties
			this.ast = TextParser.ast;
		}
		
		for (var i = 0; i < this.functionInfos.length; i++)
		{
			var functionInfo = this.functionInfos[i];
			var name = functionInfo.name;
			
			// keep track of what we've visited in this script element
			this.localFunctionNames[name] = true;
			
			// get a reference to the function
			var func = Jaxer.pageWindow[name];
			
			// determine the runat and proxy values
			var runatProp = (func.hasOwnProperty(RUNAT_ATTR)) ? func[RUNAT_ATTR] : this.runatAttr;
			var proxyProp = CallbackManager.calculateProxyAttribute(func, this.jaxerProxies, runatProp);
			var effectiveRunat = CallbackManager.calculateEffectiveRunat(runatProp, proxyProp);
			log.trace("Processing function '" + name + "' with runat=" + runatProp + ", proxy=" + proxyProp + ", effectiveRunat=" + effectiveRunat);
		
			// process
			this.processFunctionInfo(functionInfo, effectiveRunat);
		}
		
		// build source for client-bound script element
		this.isBothVariant = this.runatAttr.match(RUNAT_ANY_BOTH_REGEXP);
		if (this.isBothVariant)
		{
			this.source = this.getBothClientSource();
		}
		else
		{
			this.source = this.getClientSource();
		}

	}
};

/**
 * This method applies any needed changes to the DOM and to global functions
 * 
 * @private
 * @alias Jaxer.ScriptProcessor.prototype.apply
 */
ScriptProcessor.prototype.apply = function apply()
{

	if (this.element && (this.runatAttr != RUNAT_CLIENT)) 
	{
		if (this.source) 
		{
			this.element.innerHTML = "\n" + this.source + "\n\t\t";
			Jaxer.response.noteDomTouched();
			log.debug("This script block will be sent to the browser");
		}
		else if (this.srcAttr &&
			this.runatAttr.match(RUNAT_ANY_BOTH_REGEXP)) // create the client-only element in place
		{
			this.element.removeAttribute(RUNAT_ATTR);
			this.element.removeAttribute(AUTOLOAD_ATTR);
			Jaxer.response.noteDomTouched();
		}
		else 
		{
			this.element.parentNode.removeChild(this.element);
			this.element = null;
			Jaxer.response.noteDomTouched();
			log.debug("Removed script block from DOM");
		}
	}
	
	for (var i=0; i<this.functionInfos.length; i++)
	{
		var functionInfo = this.functionInfos[i];
		// remove function from window
		if (functionInfo.removeFromServer)
		{
			var name = functionInfo.name;
			Jaxer.pageWindow[name] = undefined;
		}
	}
	
};

/**
 * NOT CURRENTLY USED
 * This is a convenience function which creates a FunctionInfo from a parser
 * AST node and then calls the processFunctionInfo method.
 * 
 * @private
 * @alias Jaxer.ScriptProcessor.prototype.processFunctionNode
 * @param {Node} functionNode
 * 		A top-level parser AST node representing the parse tree of the
 * 		specified function
 * @param {Function} functionReference
 * 		A reference to the actual Javascript function instance
 * @param {String} runatProperty
 * 		The runat property value active for this function
 */
ScriptProcessor.prototype.processFunctionNode = function processFunctionNode(functionNode, functionReference, runatProperty)
{
	var functionInfo = new FunctionInfo(functionReference, functionNode);
	
	this.processFunctionInfo(functionInfo, runatProperty);
};

/**
 * This method is responsible for classifying each function based on the
 * specified runat and proxy property values. The classifications are later used for
 * emission of client-side functions, proxies, and callback storage.
 * 
 * @private
 * @alias Jaxer.ScriptProcessor.prototype.processFunctionInfo
 * @param {Jaxer.FunctionInfo} functionInfo
 * 		A function info instance representing a Javascript function
 * @param {String} runatProperty
 * 		The runat property value active for this function
 */
ScriptProcessor.prototype.processFunctionInfo = function processFunctionInfo(functionInfo, runatProperty)
{
	switch (runatProperty)
	{
		case RUNAT_CLIENT:
			functionInfo.removeFromServer = true;
			functionInfo.removeFromClient = false;
			this.clientInfos.push(functionInfo);
			break;
			
		case RUNAT_BOTH_NO_CACHE:
			functionInfo.removeFromServer = false;
			functionInfo.removeFromClient = false;
			this.clientInfos.push(functionInfo);
			break;
			
		case RUNAT_BOTH_AND_CACHE:
			functionInfo.removeFromServer = false;
			functionInfo.removeFromClient = false;
			this.clientInfos.push(functionInfo);
			this.cacheInfos.push(functionInfo);
			break;
			
		case RUNAT_BOTH_AND_PROXY:
			functionInfo.removeFromServer = false;
			functionInfo.removeFromClient = false;
			this.cacheInfos.push(functionInfo);
			this.clientInfos.push(functionInfo);
			this.bothProxyInfos.push(functionInfo);
			break;
			
		case RUNAT_SERVER_NO_CACHE:
			functionInfo.removeFromServer = false;
			functionInfo.removeFromClient = true;
			break;
			
		case RUNAT_SERVER_AND_CACHE:
			functionInfo.removeFromServer = false;
			functionInfo.removeFromClient = true;
			this.cacheInfos.push(functionInfo);
			break;
			
		case RUNAT_SERVER_AND_PROXY:
			functionInfo.removeFromServer = false;
			functionInfo.removeFromClient = true;
			this.cacheInfos.push(functionInfo);
			this.proxyInfos.push(functionInfo);
			break;
		
		default:
			log.warn("Unrecognized " + RUNAT_ATTR + " property value: " + runatProperty);
			break;
	}
};

/**
 * This method removes all runat property assignments in the underlying script
 * element's source. This prevents the runat property from being exposed on the
 * client. This method relies on an underlying parser AST node to function
 * properly.
 * 
 * @private
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
 * a parser AST node for this script element's source.
 * 
 * @private
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
	var singleQuotedString = "'(\\\\.|[^\'\\\\\\n\\r])*?\'";
	var doubleQuotedString = "\"(\\\\.|[^\"\\\\\\n\\r])*?\"";
	var runatPart = RUNAT_ATTR + "\\s*=\\s*" + "(" + singleQuotedString + "|" + doubleQuotedString + ")";
	var proxyPart = PROXY_ATTR + "\\s*=[^\r\n;]+";
	var runatPattern = new RegExp("(" + names.join("|") + ")\\.(" + runatPart + "|" + proxyPart + ")(\\s*;)?", "g");
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
