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

var log = Log.forModule("CallbackManager");

// private variables
var document;
var runatFunctions = {};
var autoloads = [];

/**
 * @namespace {Jaxer.CallbackManager}
 * A namespace object holding functions and members for preparing the callback
 * data at the end of page processing that will allow the page to call back its
 * server-side functions.
 * @advanced
 */
var CallbackManager = {};

/**
 * Process all functions for possible caching, proxying, and callback
 * invocation.
 * 
 * @advanced
 * @alias Jaxer.CallbackManager.processCallbacks
 * @param {Document} doc
 * 		The current document's DocumentElement. This is used to create elements
 * 		in the active DOM as needed.
 */
CallbackManager.processCallbacks = function processCallbacks(doc)
{
	// save reference to complete document
	document = doc;
	
	var scriptsInfo = new ScriptInfo();
	
	// process all script elements in the document
	processScriptElements(scriptsInfo);
	
	// processs all globally defined functions
	processGlobalFunctions(scriptsInfo);
	
	// cache server-side functions for future callbacks
	cacheServerFunctions(scriptsInfo);

	return scriptsInfo;
};

/**
 * Once all functions have been processed, we need to make sure all cached and
 * proxied functions are added to the database so we can reconstitute those
 * functions during callbacks.
 * 
 * @private
 * @alias Jaxer.CallbackManager.cacheServerFunctions
 * @param {Jaxer.ScriptInfo} scriptsInfo
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
		var pageName = Jaxer.request.pageKey;
		var namespace = null;

		// Build a hashmap to quickly identify which cached functions should be client-callable
		var callables = {};
		scriptsInfo.proxyInfos.forEach(function(info)
		{
			callables[info.name] = true;
		});
		
		// accumulate functions to cache on the server
		scriptsInfo.cacheInfos.forEach(function(info)
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
		var oncallback = Jaxer.pageWindow.oncallback;
		if (typeof oncallback == "function")
		{
			var onCallbackInfo;
			var onCallbackName = "oncallback";
			if (SIMPLE_PARSE)
			{
				var onCallbackSource = oncallback.toString();
				if (onCallbackSource.indexOf("function (") == 0) 
				{
					onCallbackSource = onCallbackSource.replace(/^function \(/, "function " + onCallbackName + "(");
				}
				else
				{
					onCallbackSource = onCallbackSource.replace(/^function \w+\(/, "function " + onCallbackName + "(");
				}
				onCallbackInfo = new FunctionInfo(onCallbackName, [], onCallbackSource);
			}
			else
			{
				onCallbackInfo = new FunctionInfo(oncallback)
			}
			if (onCallbackInfo.isNative()) 
			{
				log.warn("Cannot cache oncallback because it is a native function: " + uneval(onCallbackInfo));
			}
			else 
			{
				sources.push(onCallbackInfo.createServerFunction(namespace, false, true));
			}
		}
		
		// append all autoloads for this page
		var strAutoloads = Serialization.toJSONString(autoloads, { as: Serialization.JAXER_METHOD });
		log.trace("Saving autoloads for this page: " + strAutoloads);
		sources.unshift("Jaxer._autoload(" + strAutoloads + ");");
		sources.unshift("/* callback.js */");
		
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

			var placeholders = values.map(function(field) { return "?"; });
			
			// build query
			query = "INSERT INTO callback_page(" + fields.join(",") + ") VALUES(" + placeholders + ")";
			
			// commit source to db
			DB.frameworkExecute(query, values);
		}
		
		// determine whether we've cached this page already, else cache it
		if (!CallbackManager.isCallbackPageCached(pageName, crc32))
		{
			CallbackManager.cacheCallbackPage(pageName, crc32, source);
		}
		
		if (scriptsInfo.hasProxies)
		{
			Jaxer.response.noteDomTouched();
			// Pass callback-related parameters to the client
			var head = document.getElementsByTagName("head")[0];
			Util.DOM.insertScriptAtBeginning(
				[
					"Jaxer.Callback." + Callback.PAGE_SIGNATURE + " = " + crc32,
					"Jaxer.Callback." + Callback.PAGE_NAME + " = '" + pageName + "'",
					"Jaxer.Callback." + Callback.CALLING_PAGE + " = '" + Util.String.escapeForJS(Jaxer.request.parsedUrl.url) + "'",
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
 * Generate the unique key by which callbacks are stored/cached
 * 
 * @advanced
 * @alias Jaxer.CallbackManager.getCallbackKey
 * @param {String} pageName
 * 		The name identifying the page that was served
 * @param {String} crc32
 * 		The crc32 signature of the callback contents,
 * 		used to see whether the code was changed
 * @return {String}
 * 		The key uniquely identifying the callback page
 */
CallbackManager.getCallbackKey = function getCallbackKey(pageName, crc32)
{
	return crc32 + "::" + pageName;
}

/**
 * Cache the callback page in memory for faster execution on callback
 * 
 * @private
 * @alias Jaxer.CallbackManager.cacheCallbackPage
 * @param {String} pageName
 * 		The name identifying the page that was served
 * @param {String} crc32
 * 		The crc32 signature of the callback contents,
 * 		used to see whether the code was changed
 * @param {String} source
 * 		The JavaScript source code (function definitions) to execute
 */
CallbackManager.cacheCallbackPage = function cacheCallbackPage(pageName, crc32, source)
{
	var key = CallbackManager.getCallbackKey(pageName, crc32);
	log.debug("Caching this page under key = " + key);
	CacheManager.callbackPages[key] = Config.CACHE_USING_SOURCE_CODE ? source : Includer.compile(source);
}

/**
 * Retrieve the callback page cached in memory
 * 
 * @private
 * @alias Jaxer.CallbackManager.getCachedCallbackPage
 * @param {String} pageName
 * 		The name identifying the page that was served
 * @param {String} crc32
 * 		The crc32 signature of the callback contents,
 * 		used to see whether the code was changed
 * @return {String} source
 * 		The (compiled) cached callback page
 */
CallbackManager.getCachedCallbackPage = function getCachedCallbackPage(pageName, crc32)
{
	var key = CallbackManager.getCallbackKey(pageName, crc32);
	log.debug("Retrieving cached page using key = " + key);
	return CacheManager.callbackPages[key];
}

/**
 * Has the callback page been cached in memory?
 * 
 * @private
 * @alias Jaxer.CallbackManager.isCallbackPageCached
 * @param {String} pageName
 * 		The name identifying the page that was served
 * @param {String} crc32
 * 		The crc32 signature of the callback contents,
 * 		used to see whether the code was changed
 * @return {Boolean}
 * 		true if cached (in memory on this instance of Jaxer), false otherwise
 */
CallbackManager.isCallbackPageCached = function isCallbackPageCached(pageName, crc32)
{
	var key = CallbackManager.getCallbackKey(pageName, crc32);
	return CacheManager.callbackPages.hasOwnProperty(key);
}

/**
 * Initialize any functionality that will later be needed to setup callbacks
 * for this page. This should be called before any developer code could run.
 * Right now it just sets up a registry of any functions that have their runat
 * property set, so we can track them and make sure we process them at the end 
 * of the page, rather than searching the global context to find them.
 * 
 * @private
 * @alias Jaxer.CallbackManager.initPage
 * @param {Object} global
 * 		The global (window) context on which to instrument the Function prototype
 * 		such that setting a runat property registers the function.
 */
CallbackManager.initPage = function initPage(global)
{
	
	/**
	 * Adding functions (or their names) to this array is equivalent to
	 * setting their proxy property to true. To enforce no proxies,
	 * overriding any proxy property or runat="server-proxy", set
	 * Jaxer.proxies = null.
	 * 
	 * @alias Jaxer.proxies
	 * @property {Function[]}
	 */
	Jaxer.proxies = [];
	
	runatFunctions = {};
	Util.setSafeSetter(global.Function.prototype, RUNAT_ATTR, function setRunat(func, value)
	{
		func[RUNAT_ATTR] = value; 
		var name = func.name;
		if (name && 
			(name != 'anonymous') && 
			(!runatFunctions.hasOwnProperty(name))) // We will only process non-anonymous global functions
		{
			runatFunctions[name] = func;
		}
	});
	
	autoloads = [];
}

/**
 * Add a script location ot the autoloads of the current page
 * 
 * @private
 * @alias Jaxer.CallbackManager.processGlobalFunctions
 * @param {String} src
 * 		The fully-resolved src (location) of the autoloaded script
 */
CallbackManager.addAutoload = function addAutoload(src)
{
	autoloads.push(src);
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
 * @param {Jaxer.ScriptInfo} scriptsInfo
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
	
	for (var name in runatFunctions)
	{
		var candidate = runatFunctions[name];
		log.trace("For runatFunctions member " + name + ": handledInScripts.hasOwnProperty=" + handledInScripts.hasOwnProperty(name) + "; typeof candidate: " + typeof candidate);
		if (handledInScripts.hasOwnProperty(name) || 
			!candidate || 
			(typeof candidate != "function"))
		{
			continue;
		}
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
			var runatProp = candidate[RUNAT_ATTR];
			var proxyProp = CallbackManager.calculateProxyAttribute(candidate, scriptsInfo.jaxerProxies, runatProp);
			var effectiveRunat = CallbackManager.calculateEffectiveRunat(runatProp, proxyProp);
			log.debug("Processing global function '" + name + "' with runat=" + runatProp + ", proxy=" + proxyProp + ", effectiveRunat=" + effectiveRunat);
			
			// process content
			processor.processFunctionInfo(info, effectiveRunat);
		}
		else 
		{
			log.error("Error parsing '" + name + "': " + source);
		}
	}
	
	// NOTE: This has to be called before you can rely on hasProxies
	var source = processor.getClientSource();
	
	if (source != "")
	{
		// add client script element
		Util.DOM.insertScriptAtEnd(
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
 * @param {Jaxer.ScriptInfo} scriptInfo
 * 		The active scriptInfo being used to manage state in this callback
 * 		manager session
 */
function processScriptElements(scriptsInfo)
{
	try
	{
		// TODO: why do we need to use documentElement to see all the scripts??
		var scripts = Array.slice(document.documentElement.getElementsByTagName("script"));
		log.debug("Found " + scripts.length + " script blocks to conditionally post-process");
		// process each script element
		var processors = scripts.map(
			function processElement(element) 
			{
				var processor = new ScriptProcessor(element, scriptsInfo.jaxerProxies);
				processor.process();
				return processor;
			}
		);
		
		// perform any post-processing
		processors.forEach(
			function postProcess(processor) 
			{
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
				
				// apply any needed changes to the DOM and the window object
				processor.apply();
			}
		);
	}
	catch(e)
	{
		log.error("Error: " + e);
	}
}

/**
 * Calculates the effective proxy attribute of a function based on settings on
 * the function and on global settings.
 * 
 * @private
 * @alias Jaxer.CallbackManager.calculateProxyAttribute
 * @param {Function} func
 * 		The function whose proxy attribute is to be calculated
 * @param {Object} jaxerProxies
 * 		An object containing names (as its properties) of functions to
 * 		implicitly proxy. If null, no functions will be proxied.
 * @param {String} runat
 * 		The runat property value from which to extract the proxy attribute, if
 * 		the function does not have its own runat property.
 */
CallbackManager.calculateProxyAttribute = function calculateProxyAttribute(func, jaxerProxies, runat)
{
	if (jaxerProxies == null)
	{
		return false;
	}
	else if (jaxerProxies.hasOwnProperty(func.name))
	{
		return true;
	}
	else if (func.hasOwnProperty(PROXY_ATTR))
	{
		return Boolean(func[PROXY_ATTR]);
	}
	else if (!runat)
	{
		return false;
	}
	else
	{
		switch(runat)
		{
			case RUNAT_CLIENT:
			case RUNAT_BOTH_NO_CACHE:
			case RUNAT_BOTH_AND_CACHE:
			case RUNAT_SERVER_NO_CACHE:
			case RUNAT_SERVER_AND_CACHE:
				return false;
				
			case RUNAT_BOTH_AND_PROXY:
			case RUNAT_SERVER_AND_PROXY:
				return true;
			
			default:
				var src = func.toSource();
				if (src.length > 60) src = src.substr(0, 60) + "...";
				log.error("Unrecognized " + RUNAT_ATTR + " property value '" + runat + "' for function: " + src);
				return false;
		}
	}
}

// private lookup table mapping runat values + proxy values to effective runat value
var PROPERTY_VALUE_MAP = {};
PROPERTY_VALUE_MAP[RUNAT_CLIENT + "-false"] = RUNAT_CLIENT;
PROPERTY_VALUE_MAP[RUNAT_CLIENT + "-true"] = RUNAT_CLIENT;
PROPERTY_VALUE_MAP[RUNAT_SERVER_AND_CACHE + "-false"] = RUNAT_SERVER_AND_CACHE;
PROPERTY_VALUE_MAP[RUNAT_SERVER_AND_CACHE + "-true"] = RUNAT_SERVER_AND_PROXY;
PROPERTY_VALUE_MAP[RUNAT_SERVER_AND_PROXY + "-false"] = RUNAT_SERVER_AND_CACHE;
PROPERTY_VALUE_MAP[RUNAT_SERVER_AND_PROXY + "-true"] = RUNAT_SERVER_AND_PROXY;
PROPERTY_VALUE_MAP[RUNAT_SERVER_NO_CACHE + "-false"] = RUNAT_SERVER_NO_CACHE;
PROPERTY_VALUE_MAP[RUNAT_SERVER_NO_CACHE + "-true"] = RUNAT_SERVER_AND_PROXY;
PROPERTY_VALUE_MAP[RUNAT_BOTH_AND_CACHE + "-false"] = RUNAT_BOTH_AND_CACHE;
PROPERTY_VALUE_MAP[RUNAT_BOTH_AND_CACHE + "-true"] = RUNAT_BOTH_AND_PROXY;
PROPERTY_VALUE_MAP[RUNAT_BOTH_AND_PROXY + "-false"] = RUNAT_BOTH_AND_CACHE;
PROPERTY_VALUE_MAP[RUNAT_BOTH_AND_PROXY + "-true"] = RUNAT_BOTH_AND_PROXY;
PROPERTY_VALUE_MAP[RUNAT_BOTH_NO_CACHE + "-false"] = RUNAT_BOTH_NO_CACHE;
PROPERTY_VALUE_MAP[RUNAT_BOTH_NO_CACHE + "-true"] = RUNAT_BOTH_AND_PROXY;

CallbackManager.calculateEffectiveRunat = function calculateEffectiveRunat(runatProp, proxyProp)
{
	return PROPERTY_VALUE_MAP[runatProp + "-" + proxyProp];
}

CallbackManager.cleanup = function cleanup(scriptsInfo)
{
	if (scriptsInfo) 
	{
		for (var name in scriptsInfo.functionNames) 
		{
			Jaxer.pageWindow[name] = undefined;
		}
	}
	
	runatFunctions = null;
	Jaxer.proxies = null;

}

frameworkGlobal.CallbackManager = Jaxer.CallbackManager = CallbackManager;

Log.trace("*** CallbackManager.js loaded");

})();
