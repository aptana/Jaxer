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
	
var log = Log.forModule("HandleCallback");

// private constants
var METHOD_NAME = "methodName";
var RESULT_AS = "resultAs";
var RESULT_AS_TEXT = "text";
var RESULT_AS_OBJECT = "object";
var RESULT_AS_WRAPPED_OBJECT = "wrappedObject";
var NULL_AS_TEXT = ""; // What to return if the POSTPROCESS is POSTPROCESS_ASTEXT and the result is null or an exception
var NO_TOSTRING_AS_TEXT = ""; // What to return if the POSTPROCESS is POSTPROCESS_ASTEXT and the result has no toString() method
// global constants
var Callback = {};
Callback.PAGE_SIGNATURE = "pageSignature";
Callback.PAGE_NAME = "pageName";
Callback.PARAMETERS = "params";
Callback.PARAMS_AS = "paramsAs";
Callback.PARAMS_AS_TEXT = "text";
Callback.PARAMS_AS_OBJECT = "object";
Callback.PARAMS_AS_ARGUMENTS = "default";
Callback.CALLING_PAGE = "callingPage";

CoreEvents.clearHandlers(CoreEvents.HANDLE_CALLBACK); // Clear this out -- we're the first and only default handler

// add the framework's CALLBACK event handler
CoreEvents.addHandler(CoreEvents.HANDLE_CALLBACK, function onHandleCallback(evt, doc)
{

	log.trace("=== Callback processing initializing");
	
	Jaxer.response.allow(true);
	Jaxer.response.setNoCacheHeaders(); // We always want to avoid caching and get the callback
	Jaxer.response.exposeJaxer();

	// On a callback, the following need to be set from the database, which we do further below
	Jaxer.request.pageFile = "";
	Jaxer.request.documentRoot = "";

	var window = Jaxer.pageWindow;
	
	log.trace("Main callback processing starting");
	var result = null;
	var exception = null;
	
	try
	{
		var info = {};
		var functionName = null;
		var args = [];
		var crc32 = null;
		var pageName = null;
		var resultAs = null;
		
		if (log.getLevel() == Log.TRACE) log.trace("Received Jaxer.request: " + uneval(Jaxer.request));
		
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
		// PARAMS_AS_ARGUMENTS is the default and needs no special handling;
		// PARAMS_AS_OBJECT is handled here
		// PARAMS_AS_TEXT has already been handled in creating the current Request()
		var paramIsQuery = (info.hasOwnProperty(Callback.PARAMS_AS) && info[Callback.PARAMS_AS] == Callback.PARAMS_AS_OBJECT);
		if (paramIsQuery)
		{
			delete info[Callback.PARAMETERS];
			args = [info];
		}
		else if (info.hasOwnProperty(Callback.PARAMETERS))
		{
			if (info[Callback.PARAMETERS].constructor == info[Callback.PARAMETERS].__parent__.Array)
			{
				args = info[Callback.PARAMETERS];
			}
			else
			{
				args = Serialization.fromJSONString(info[Callback.PARAMETERS], {as: Serialization.JAXER_METHOD});
			}
		}
		if (info.hasOwnProperty(Callback.PAGE_SIGNATURE)) // get the signature of the callback contents
		{
			crc32 = info[Callback.PAGE_SIGNATURE];
		}
		if (info.hasOwnProperty(Callback.PAGE_NAME)) // get the name of the page that created the callback contents
		{
			pageName = info[Callback.PAGE_NAME];
		}
		if (info.hasOwnProperty(RESULT_AS))
		{
			resultAs = info[RESULT_AS];
		}
		
		// We might need the above parameters to know how to return the result, even if there's a fatal error
		if (Jaxer.fatalError)
		{
			throw "Fatal Jaxer error: " + Jaxer.fatalError;
		}
		
		// We might need the above parameters to know how to return the result, even if there's a response error
		if (Jaxer.response.hasError())
		{
			throw "Jaxer response error (before callback itself was called): " + Jaxer.response.getError();
		}
		
		// Next, reconstruct environment and call the callback
		if (functionName !== null && args !== null && crc32 && pageName)
		{
			var isPageCached = CallbackManager.isCallbackPageCached(pageName, crc32);
			
			// grab all functions in our callback page that are not being called
			var rs = DB.frameworkExecute("SELECT " + (isPageCached ? "" : "value, ") + "document_root, page_file, access_count FROM callback_page WHERE name=? AND crc32=?",
				[pageName, crc32]);
			
			if (rs.rows.length > 0)
			{
				var callbackRow = rs.rows[0];
				Jaxer.request.documentRoot = callbackRow.document_root;
				Jaxer.request.pageFile = callbackRow.page_file;
				log.debug("Setting [Jaxer.request.documentRoot, Jaxer.request.pageFile] = " + [Jaxer.request.documentRoot, Jaxer.request.pageFile]);
				
				Jaxer._autoload = function _autoload(autoloads)
				{
					log.debug("Restoring autoloads: " + autoloads);
					autoloads.forEach(function autoload(src)
					{
						try
						{
							var useCache = true;
							var forceCacheRefresh = false;
							var dontSetRunat = true;
							Includer.load(src, window, null, useCache, forceCacheRefresh, dontSetRunat);
						}
						catch (e)
						{
							throw new Error("Error during autoloading of src = '" + src + "': " + e);
						}
					});
				}
				
				if (!isPageCached)
				{
					log.debug("Callbacks found in DB but not in cache, restoring to cache using pageName=" + pageName + " and crc32=" + crc32);
					CallbackManager.cacheCallbackPage(pageName, crc32, callbackRow.value);
				}
				var cachedSource = CallbackManager.getCachedCallbackPage(pageName, crc32);
				if (Config.CACHE_USING_SOURCE_CODE)
				{
					if (log.getLevel() == Log.TRACE) log.trace("Evaluating: " + cachedSource);
					Includer.evalOn(cachedSource, window);
				}
				else
				{
					Includer.evalCompiledOn(cachedSource, window);
				}
				
				var accessCount = callbackRow.access_count - 0;
				accessCount++;
				log.trace("Updating access count to " + accessCount);
				
				try
				{
					DB.frameworkExecute("UPDATE callback_page SET access_datetime=?, access_count=? WHERE name=? AND crc32=?",
						[new Date(), accessCount, pageName, crc32]);
				}
				catch(e)
				{
					log.error(e + " [ignoring]");
				}

				if (window.hasOwnProperty("oncallback"))
				{
					try
					{
						log.debug("Executing oncallback");
						window.oncallback();
					}
					catch (e)
					{
						throw new Error("Error during oncallback: " + e);
					}
				}
				
				if (window.hasOwnProperty(functionName))
				{
					var func = window[functionName];
					if (typeof func == "function")
					{
						if (func.hasOwnProperty(PROXY_ATTR) && func[PROXY_ATTR])
						{
							try
							{
								log.debug("About to execute: " + func.name + " with " + args.length + " arguments");
								if (log.getLevel() == Log.TRACE) log.trace("Arguments: " + uneval(args));
								result = func.apply(null, args);
							}
							catch (e)
							{
								throw (e instanceof ClientError) ? e : Exception.toError(e);
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
				throw new Error("Unable to locate callback page for name = " + pageName + " and crc32 = " + crc32);
			}
		}
		else
		{
			throw new Error("Missing function name and/or arguments and/or page name or signature");
		}
	}
	catch(e)
	{
		if (e instanceof ClientError)
		{
			exception = e;
			if (log.getLevel().isAtOrBelow(Log.DEBUG)) log.debug("ClientError being thrown: " + uneval(e));
		}
		else
		{
			exception = new Exception(e); // This will log it 
		}
	}
	// Finally, in any case, package the results
	finally
	{
		
		if ((exception != null) && !Config.DISPLAY_ERRORS && !(exception instanceof ClientError))
		{
			log.debug("An error not of type ClientError occurred during callback, and Config.DISPLAY_ERRORS is false, so returning a generic exception and discarding result: " + result);
			result = null;
			exception = {name: 'Error', message: Jaxer.fatalError ? Config.CALLBACK_FATAL_ERROR_MESSAGE : Config.CALLBACK_ERROR_MESSAGE};
			exception.description = exception.message;
		}

		if (log.getLevel() == Log.TRACE) log.trace("result = " + result);
		
		if (resultAs == RESULT_AS_TEXT)
		{
			Jaxer.response.setContents((result == null) ? NULL_AS_TEXT : ((typeof result.toString == "function") ? result.toString() : NO_TOSTRING_AS_TEXT));
		}
		else if (resultAs == RESULT_AS_OBJECT)
		{
			Jaxer.response.setContents((result == null) ? NULL_AS_TEXT : Serialization.toJSONString(result, { as: Serialization.JAXER_METHOD }));
		}
		else
		{
			var content = {};
			if (exception !== null)
			{
				content.exception = exception;
				content.isClientError = (exception instanceof ClientError);
			}
			else if (result !== undefined)
			{
				content.returnValue = result;
			}
			Jaxer.response.setContents(Serialization.toJSONString(content, { as: Serialization.JAXER_METHOD }));
		}

		if (log.getLevel() == Log.TRACE) log.trace("Jaxer.response.contents = " + Jaxer.response.getContents());
		if (log.getLevel() == Log.DEBUG)
		{
			log.debug("Jaxer.response.contents (first 200 chars) = " + Jaxer.response.getContents().substr(0, 200));
		}
		
		Container.persistAll();
		log.debug("Containers persisted");

		Jaxer.DB.closeAllConnections(true);
		Jaxer.response.persistHeaders();

		log.trace("=== Callback processing complete");
	}
	
});

frameworkGlobal.Callback = Jaxer.Callback = Callback;

Log.trace("*** HandleCallback.js loaded");

})();
