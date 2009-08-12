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

var log = Log.forModule("Includer"); // Only if Log itself is defined at this point of the includes

/**
 * @namespace {Jaxer.Includer}	Namespace object holding functions and members
 * used to get and include HTML and JavaScript from external sources.
 */
var Includer = {};

var INCLUDE_TAG = "jaxer:include";

/**
 * Replaces the element with the contents of its source or path
 * 
 * @private
 * @alias Jaxer.Includer.includeElement
 * @param {DocumentElement} doc
 * 		The document of the element
 * @param {HTMLElement} includeTag
 * 		The HTML element to replace
 * @return {Boolean}
 * 		true if successful, false otherwise
 */
Includer.includeElement = function includeElement(doc, includeTag)
{
	log.trace("Entering with element to include having src = " + includeTag.getAttribute("src") + " and path = " + includeTag.getAttribute("path"));
	// Read in file contents
	var htmlToInclude = "";
	var from;

	var error = null;
	try
	{
		if (includeTag.getAttribute("src")) // url-type path given, either of file type or web type
		{
			from = includeTag.getAttribute("src");
			htmlToInclude = Web.get(from, {cacheBuster: true});
		}
		else
		{
			from = '';
			log.warn("No src attribute was given for this " + includeTag.tagName + " element");
		}
		
		if (from) 
		{
			if (htmlToInclude.length == 0) 
			{
				log.warn("No content was read from included file at " + from + " for this " + includeTag.tagName + " element");
			}
			else 
			{
				log.debug("Successfully included file at " + from + " for this " + includeTag.tagName + " element");
			}
		}
	}
	catch (e)
	{
		error = e;
	}
	
	// Replace includeTag with the parsed HTML
	Util.DOM.insertHTML(htmlToInclude, includeTag, 0);
	
	if (error)
	{
		if (Config.INCLUDE_ERRORS_ARE_RESPONSE_ERRORS) 
		{
			throw new Exception(error);
		}
		else
		{
			log.error(error);
		}
	}

	return (error == null);
}

/**
 * Evaluate the given JavaScript string in the given global context
 * 
 * @advanced
 * @alias Jaxer.Includer.evalOn
 * @param {String} contents
 * 		The string of script code to evaluate
 * @param {Object} [global]
 * 		An optional global context (usually a window object) in which to
 * 		evaluate it
 * @param {String} [effectiveUrl]
 * 		An optional parameter to indicate (e.g. in error messages) the effective
 * 		URL from which this code originates.
 * @return {Object}
 * 		The result of the last JavaScript expression evaluated.
 */
Includer.evalOn = function evalOn(contents, global, effectiveUrl)
{
	if (Jaxer.request)
	{
		log.trace("Evaluating using Jaxer.request.evaluateScript: contents = " + contents.substr(0, 10) + "... with global = " + global + " and effectiveUrl = " + effectiveUrl);
		return Jaxer.request.evaluateScript(contents, global, effectiveUrl)
	}
	else 
	{
		var tempPath = Dir.combine(System.tempFolder, "_script_" + (new Date().getTime()) + Math.random());
		var tempUrl = Dir.pathToUrl(tempPath);
		log.trace("Evaluating using temporary file " + tempUrl + ": " + contents.substr(0, 10) + "...");
		File.write(tempPath, contents);
		try 
		{
			Jaxer.include(tempUrl, global);
		}
		finally 
		{
			File.remove(tempPath);
		}
	}
}

/**
 * Compile the given JavaScript string in the given global context
 * 
 * @advanced
 * @alias Jaxer.Includer.compile
 * @param {String} contents
 * 		The string of script code to compile
 * @param {Object} [global]
 * 		An optional global context (usually a window object) in which to
 * 		compile it
 * @param {String} [effectiveUrl]
 * 		An optional parameter to indicate (e.g. in error messages) the effective
 * 		URL from which this code originates.
 * @return {String}
 * 		The compiled bytecode, as a string.
 */
Includer.compile = function compile(contents, global, effectiveUrl)
{
	if (Jaxer.request)
	{
		log.trace("Compiling using Jaxer.request.compileScript: contents = " + contents.substr(0, 10) + "... with global = " + global + " and effectiveUrl = " + effectiveUrl);
		return Jaxer.request.compileScript(contents, global, effectiveUrl)
	}
	else 
	{
		throw new Exception("Must have a Jaxer.request object before compiling a script")
	}
}

/**
 * Evaluate the given JavaScript bytecode string in the given global context
 * 
 * @advanced
 * @alias Jaxer.Includer.evalCompiledOn
 * @param {String} contents
 * 		The string of script code to evaluate
 * @param {Object} [global]
 * 		An optional global context (usually a window object) in which to
 * 		evaluate it
 * @return {Object}
 * 		The result of the last JavaScript expression evaluated.
 */
Includer.evalCompiledOn = function evalCompiledOn(compiledContents, global)
{
	if (!compiledContents)
	{
		log.debug("Ignoring empty or null compiledContents");
	}
	else if (Jaxer.request)
	{
		log.trace("Evaluating using Jaxer.request.evaluateCompiledScript: compiledContents.length = " + compiledContents.length + "... with global = " + global);
		return Jaxer.request.evaluateCompiledScript(compiledContents, global)
	}
	else 
	{
		throw new Exception("Must have a Jaxer.request object before evaluating a compiled script")
	}
}

/**
 * Loads and evaluates a JavaScript file on the given global execution object
 * with the given runat attribute.
 * 
 * @alias Jaxer.Includer.load
 * @param {String} src
 * 		The URL from which the JavaScript file should be retrieved. If the src
 * 		is an absolute file://... URL then it is retrieved directly from the
 * 		file system, otherwise it is retrieved via a web request.
 * @param {Object} [global]
 * 		The global (usually a window object) on which to evaluate it. By
 * 		default, it is the same global as the one in which the calling function
 * 		is executing.
 * @param {String} [runat]
 * 		The value of the effective runat "attribute" to use when evaluating this
 * 		code. By default, it uses the same runat attribute as the last evaluated
 * 		script block.
 * @param {Boolean} [useCache]
 * 		If true, the file is loaded from a cached compiled version if available,
 * 		and if not available the file's contents are fetched, compiled and cached.
 * 		By default this is false.
 * @param {Boolean} [forceCacheRefresh]
 * 		If true, force loading from src even if found in cache. The loaded contents
 * 		will then be cached.
 * 		By default this is false.
 * @param {Boolean} [dontSetRunat]
 * 		If true, any functions created in this script block will not have a runat
 * 		property set on them, not even the default runat of the last script block.
 * 		By default this is false.
 * @return {Object}
 * 		The result of the last JavaScript expression evaluated, if any.
 */
Includer.load = function load(src, global, runat, useCache, forceCacheRefresh, dontSetRunat)
{
	var result;
	var oldNewFunctions = Jaxer.pageWindow.Jaxer.__newFunctions;
	var useNewFunctions = false;
	
	src = Web.resolve(src);
	
	try
	{
		
		if (!global) global = Util.getGlobalContext(load.caller);
		runat = runat || Jaxer.lastScriptRunat;
		
		if (!dontSetRunat) 
		{
			if (runat && runat.match(RUNAT_ANY_SERVER_REGEXP)) 
			{
				useNewFunctions = true;
			}
		}
		
		var cachedContents, isInCache;
		if (useCache) 
		{
			if (forceCacheRefresh) 
			{
				isInCache = false;
			}
			else 
			{
				cachedContents = CacheManager.autoloadScripts[src];
				isInCache = (cachedContents != undefined);
				log.trace("Is '" + src + "' in cache? " + isInCache);
			}
		}
		
		var contents;
		if (!useCache || !isInCache) 
		{
			if (Util.Url.isFile(src)) 
			{
				contents = File.read(src);
			}
			else 
			{
				contents = Web.get(src);
			}
			log.trace("Retrieved " + contents.length + " characters from '" + src + "'");
		}
		
		if (useCache && !isInCache)
		{
			if (Config.CACHE_USING_SOURCE_CODE)
			{
				log.trace("Caching as plain text '" + src + "'");
				cachedContents = contents;
			}
			else
			{
				log.trace("Compiling and caching '" + src + "'");
				cachedContents = Includer.compile(contents, global, src);
			}
			CacheManager.autoloadScripts[src] = cachedContents;
		}
		
		if (useNewFunctions) // turn this on right before evaluation
		{
			Jaxer.pageWindow.Jaxer.__newFunctions = [];
		}
		
		if (useCache) 
		{
			if (Config.CACHE_USING_SOURCE_CODE)
			{
				log.trace("Evaluating '" + src + "' from source code");
				result = Includer.evalOn(cachedContents, global, src);
			}
			else
			{
				log.trace("Evaluating '" + src + "' from compiled code");
				result = Includer.evalCompiledOn(cachedContents, global, src);
			}
		}
		else 
		{
			log.trace("Evaluating '" + src + "'");
			result = Includer.evalOn(contents, global, src);
		}
		
		if (Jaxer.pageWindow.Jaxer.__newFunctions) 
		{
			log.trace("Jaxer.pageWindow.Jaxer.__newFunctions contains " + (Jaxer.pageWindow.Jaxer.__newFunctions.length) + " items");
			Jaxer.pageWindow.Jaxer.__newFunctions.forEach(function setRunat(name)
			{
				if (name && (typeof name == "string"))
				{
					var func = global[name];
					if (func && (func != null) && (func.toSource) && (typeof func == "function") && // Need to do some extraordinary checking because XDR compilation can leave function "stubs"
						!Util.isNativeFunction(func) &&
						(typeof func[RUNAT_ATTR] != "string"))
					{
						func[RUNAT_ATTR] = runat;
						log.trace("Set runat='" + runat + "' on function named '" + func.name + "'");
					}
				}
			});
		}
		
		log.debug("Included: " + src);

	}
	finally // clean up Jaxer.pageWindow.Jaxer.__newFunctions whether or not there was an error
	{
		if (useNewFunctions) 
		{
			Jaxer.pageWindow.Jaxer.__newFunctions = oldNewFunctions;
		}
	}
	
	return result;
	
}

Jaxer.load = Includer.load;

frameworkGlobal.Includer = Jaxer.Includer = Includer;

Log.trace("*** Includer.js loaded");

})();
