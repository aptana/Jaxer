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
	
// This is a client-side, compressible module -- be sure to end each function declaration with a semicolon

var log = Jaxer.Log.forModule("Callback");
	
// private constants
var EXCEPTION = "exception";
var IS_CLIENT_ERROR = "isClientError";
var CLIENT_ERROR_OPTIONS = "options";
var CLIENT_ERROR_INFO = "info";
var CLIENT_ERROR_WRAPPED = "wrapped";
var PAGE_SIGNATURE = "pageSignature";
var PAGE_NAME = "pageName";
var CALLING_PAGE = "callingPage";
var METHOD_NAME = "methodName";
var NAME = "name";
var MESSAGE = "message";
var PARAMS = "params";
var RETURN_VALUE = "returnValue";
var UID = "uid";

// create placeholder for Callback

/**
 * @namespace {Jaxer.Callback} Callback namespace for remote functions.
 */
var Callback = {};

/**
 * The default HTTP method to use for callback function requests. Initially set
 * to "POST".
 * 
 * @alias Jaxer.Callback.METHOD
 * @property {String}
 */
Callback.METHOD = "POST";

/**
 * The default number of milliseconds to wait before timing out an async
 * callback function request. Initially set to 10 * 1000 (10 seconds).
 * 
 * @alias Jaxer.Callback.TIMEOUT
 * @property {Number}
 */
Callback.TIMEOUT = 10 * 1000;

/**
 * The default polling interval used to see whether the XMLHttpRequest for an
 * async callback function call has returned. Initially set to 11.
 * 
 * @advanced
 * @alias Jaxer.Callback.POLLING_PERIOD
 * @property {Number}
 */
Callback.POLLING_PERIOD = 11;

/**
 * Creates a query string for calling a remote function with the given arguments
 * 
 * @alias Jaxer.Callback.createQuery 
 * @param {String} functionName
 * 		The name of the remote function
 * @param {Object} args
 * 		The arguments of the remote function. This can be a single (primitive)
 * 		object or an array of (primitive) objects
 * @param {Number} [initialNumberToSkip]
 * 		Optionally, how many of the arguments (counting from the beginning) to
 * 		not pass to the remote function
 * @return {String}
 * 		The query string
 */
Callback.createQuery = function createQuery(functionName, args, initialNumberToSkip)
{
	// move Arguments into a real Javascript Array
	var argsArray = [];
	initialNumberToSkip = initialNumberToSkip || 0;
	
	for (var i=initialNumberToSkip; i<args.length; i++)
	{
		argsArray.push(args[i]);
	}
	
	var queryParts = Callback.getQueryParts(functionName, argsArray);
	return Callback.hashToQuery(queryParts);
};

/**
 * Converts a javascript object (hash) into a http query string.
 * 
 * @alias Jaxer.Callback.hashToQuery
 * @param {Object} hash
 * 		Hash of name value pairs to be converted.
 * @return {String}
 * 		The query string
 */
Callback.hashToQuery = function hashToQuery(hash)
{
	var queryStrings = [];
	
	for (var p in hash)
	{
		var name = Callback.formUrlEncode(p);
		var value = ((typeof hash[p] == "undefined") || (hash[p] == null) || (typeof hash[p].toString != "function")) ? "" : Callback.formUrlEncode(hash[p].toString());
		
		queryStrings.push([name, value].join("="));
	}
	
	return queryStrings.join("&");
};

/**
 * URL Encode a query string.
 * 
 * @alias Jaxer.Callback.formUrlEncode
 * @param {String} str
 * 		Query string to be converted.
 * @return {String}
 * 		A URL-encoding string
 */
Callback.formUrlEncode = function formUrlEncode(str)
{
	return encodeURIComponent(str);
};

/**
 * Transforms the raw result data from the XHR call into the expected data
 * format.
 * 
 * @advanced
 * @alias Jaxer.Callback.processReturnValue
 * @param {String} functionName
 * 		The name of the function that was called
 * @param {String} rawResult
 * 		The raw (text) data returned from the XHR call
 * @return {Object}
 * 		The returned data in the format the remote function returned it
 */
Callback.processReturnValue = function processReturnValue(functionName, rawResult)
{
	log.trace("Received for function " + functionName + ": rawResult = " + rawResult.substr(0, 100) + (rawResult.length > 100 ? '...' : ''));
	try
	{
		var content = Jaxer.Serialization.fromJSONString(rawResult, {as: Jaxer.Serialization.JAXER_METHOD});
	}
	catch (e)
	{
		var desc, showError, likelyServerError;
		if (e.name == Jaxer.Serialization.JSONSyntaxErrorName) 
		{
			desc = "Received a response with an unexpected (non-JSON) syntax";
			showError = false;
			likelyServerError = true;
		}
		else if (e.name == Jaxer.Serialization.JSONEvalErrorName)
		{
			desc = "Error when evaluating the JSON-like response received from the server";
			showError = true;
			likelyServerError = true;
		}
		else
		{
			desc = "Error when processing the JSON response received from the server";
			showError = true;
			likelyServerError = false;
		}
		e.message = 
			desc + " " +
			"while calling server function '" + functionName + "'.\n\n" +
			(showError ? "Error: " + e + "\n\n" : "") +
			"Response:\n" + rawResult.substr(0, 200) + ((rawResult.length > 200) ? "..." : "") +
			(likelyServerError ? "\n\n(Perhaps an error occured on the server?)" : "");
		if (Jaxer.ALERT_CALLBACK_ERRORS)
		{
			alert(e.message);
		}
		throw e;
	}
	
	var result = null;

	if (content !== null && content != undefined)
	{
		if (content.hasOwnProperty(EXCEPTION))
		{
			var eFromServer, eToClient;
			
			if (content[EXCEPTION]) 
			{
				if (content[IS_CLIENT_ERROR]) 
				{
					var clientError = content[EXCEPTION];
					var clientErrorOptions = clientError[CLIENT_ERROR_OPTIONS];
					var clientErrorInfo = clientError[CLIENT_ERROR_INFO];
					if (clientErrorOptions && 
						(CLIENT_ERROR_WRAPPED in clientErrorOptions) && 
							!clientErrorOptions[CLIENT_ERROR_WRAPPED])
					{
						eFromServer = clientErrorInfo;
					}
					else
					{
						var clientErrorMessage = (clientErrorInfo && (clientErrorInfo.message)) ? String(clientErrorInfo.message) : String(clientErrorInfo);
						eFromServer = new Error(clientErrorMessage);
						if (clientErrorInfo && (typeof clientErrorInfo == "object") )
						{
							for (var p in clientErrorInfo) 
							{
								eFromServer[p] = clientErrorInfo[p];
							}
						}
					}
				}
				else 
				{
					eFromServer = content[EXCEPTION];
				}
			}
			else
			{
				eFromServer = "Unspecified server error";
			}
			
			if (eFromServer.hasOwnProperty(NAME))
			{
				var eName = eFromServer[NAME];
				
				try
				{
					eToClient = new window[eName];
								
					for (var p in eFromServer)
					{
						eToClient[p] = eFromServer[p];
					}
				}
				catch(e)
				{
					eToClient = eFromServer;
				}
			}
			else
			{
				eToClient = eFromServer;
			}
			
			if (typeof eToClient.toString != "function")
			{
				eToClient.toString = function toString()
				{
					var name = eToClient.hasOwnProperty(NAME) ? eToClient[NAME] : "server error";
					var message = eToClient.hasOwnProperty(MESSAGE) ? eToClient[MESSAGE] : "(unspecified)";
					return [name, message].join(": ");
				}
			}
			
			if (Jaxer.ALERT_CALLBACK_ERRORS)
			{
				alert("The server function '" + functionName + "' returned an error: " + 
					((typeof eToClient.message == "undefined") ? eToClient.toString() : eToClient.message));
			}
			
			throw eToClient;
		}
		else if (content.hasOwnProperty(RETURN_VALUE))
		{
			result = content[RETURN_VALUE];
		}
		else
		{
			result = undefined;
		}
	}
	
	return result;
};

/**
 * The default method used to handle errors when calling remote functions
 * asynchronously. It alerts the error message if Jaxer.ALERT_CALLBACK_ERRORS is
 * true, and in any case throws an error
 * 
 * @advanced
 * @alias Jaxer.Callback.onfailureAsync
 * @param {Object} error
 * 		If an error was thrown during the request, it would be here.
 * @param {Object} extra
 * 		Any extra information passed in during the call to Jaxer.XHR.send() to
 * 		help identify the request. Currently, there is one String-valued
 * 		property on this object: functionName. 
 * @param {XMLHttpRequest} xhr
 * 		The XMLHttpRequest object that encountered the error. This might be
 * 		null, if an error was encountered in creating the XMLHttpRequest.
 */
Callback.onfailureAsync = function onfailure(error, extra, xhr)
{
	var message = "Error while contacting server to (asynchronously) call server function '" + extra.functionName + "':\n";
	if (xhr) 
	{
		var status;
		try
		{
			status = xhr.status;
		}
		catch (e) {}
		message += "Received status " + String(xhr.status) + " from the server\n" +
			"Response from server: " + xhr.responseText;
	}
	else if (error)
	{
		message += error;
	}
	if (Jaxer.ALERT_CALLBACK_ERRORS)
	{
		alert(message);
	}
	throw new Error(message);
};

/**
 * The default method used to handle timeouts when calling remote functions
 * asynchronously. It alerts the error message if Jaxer.ALERT_CALLBACK_ERRORS is
 * true, and in any case throws an error
 * 
 * @advanced
 * @alias Jaxer.Callback.ontimeoutAsync
 * @param {Error} error
 * 		The timeout error object encountered, having a "timeout" property with
 * 		its value indicating the timeout (in milliseconds) used in this request.
 * @param {Object} extra
 * 		Any extra information passed in during the call to Jaxer.XHR.send() to
 * 		help identify the request. Currently, there is one String-valued
 * 		property on this object: functionName. 
 * @param {XMLHttpRequest} xhr
 * 		The XMLHttpRequest object that encountered the error.
 */
Callback.ontimeoutAsync = function ontimeout(error, extra, xhr)
{
	var message = "Error while contacting server to (asynchronously) call server function '" + extra.functionName + "':\n";
	message += "Request timed out after " + (error.timeout/1000) + " seconds";
	if (Jaxer.ALERT_CALLBACK_ERRORS)
	{
		alert(message);
	}
	throw new Error(message);
};

/**
 * This method invokes an asynchronous call to a proxied javascript function on
 * the server from the client side javascript. A callback function needs to be
 * provided and is called once the XHR request completes or times out.
 * 
 * @alias Jaxer.Callback.invokeFunctionAsync
 * @param {Object} callback
 * 		If this is a function, this is the function to call upon a successful
 * 		return from the remote invocation. Its arguments are what the remote
 * 		function on the server returned.
 * 		<br><br>
 * 		If this is an array, its elements are as follows (each may be null):
 * 		<ol>
 * 			<li>
 * 				the callback function;
 *			</li>
 * 			<li>
 * 				a function to call on an error, with arguments being the error,
 * 				the "extra" information object that has the functionName as its
 * 				one property, and the XMLHttpRequest object used for the call if
 * 				the call itself encountered an error;
 * 			</li>
 * 			<li>
 * 				the timeout to use, in milliseconds (defaults to
 * 				Jaxer.Callback.TIMEOUT). Use 0 to wait indefinitely.</li>
 * 		</ol> 
 * 		<br><br>
 * 		If this is an object, its "callback", "errorHandler", and timeout
 * 		properties will be used, if any.
 * @param {String} functionName
 * 		The name of the remote function
 * @param {Object} args
 * 		A single argument, or an array of arguments, to be passed to the remote
 * 		function on the server
 */
Callback.invokeFunctionAsync = function invokeFunctionAsync(callback, functionName, args)
{
	var message = Callback.createQuery(functionName, args, 1); // skip encoding the callback itself
	log.trace("Invoking function " + functionName + " asynchronously with arguments encoded as: " + message);
	
	var extra = {functionName: functionName}; // this will be passed back to error handling methods
	
	var callbackFunction, errorHandler, timeout;
	if (typeof callback == "function")
	{
		callbackFunction = callback;
	}
	else if (typeof callback == "object")
	{
		if (typeof callback.length == "number") // assume it's array-like
		{
			callbackFunction = (callback.length > 0 && typeof callback[0] == "function") ? callback[0] : undefined;
			if (callback.length > 1 && typeof callback[1] == "function") errorHandler = callback[1];
			if (callback.length > 2 && typeof callback[2] == "number") timeout = callback[2];
		}
		else
		{
			callbackFunction = (typeof callback["callback"] == "function") ? callback["callback"] : undefined;
			if (typeof callback["errorHandler"] == "function") errorHandler = callback["errorHandler"];
			if (typeof callback["timeout"] == "number") timeout = callback["timeout"];
		}
	}
	
	var onsuccess = function onsuccess(rawResult, extra)
	{
		try
		{
			var processedResult = Callback.processReturnValue(functionName, rawResult);
			if (callbackFunction)
			{
				callbackFunction(processedResult);
			}
		}
		catch (e) // to do something meaningful with async exceptions, you'll need an errorHandler
		{
			if (errorHandler)
			{
				errorHandler(e, extra);
			}
		}
	};

	var options = 
	{
		url: Jaxer.CALLBACK_URI,
		cacheBuster: false,
		method: Callback.METHOD,
		async: true,
		onsuccess: onsuccess,
		onfailure: errorHandler || Callback.onfailureAsync,
		timeout: (typeof timeout == "number") ? timeout : Callback.TIMEOUT,
		ontimeout: errorHandler || Callback.ontimeoutAsync,
		headers: null,
		onreadytosend: null,
		onfinished: null,
		contentType: "application/x-www-form-urlencoded",
		testSuccess: Jaxer.XHR.testSuccess,
		as: "text",
		pollingPeriod: Callback.POLLING_PERIOD
	};

	var pollId = Jaxer.XHR.send(message, options, extra);
	
	return pollId;
	
};

/**
 * This method invokes a synchronous call to a proxied JavaScript function on
 * the server from the client side javascript.
 * 
 * @alias Jaxer.Callback.invokeFunction
 * @param {String} functionName
 * 		The name of the remote function to call on the server
 * @param {Object} args
 * 		A single argument, or an array of arguments, to be passed to the remote
 * 		function on the server
 * @return {Object}
 * 		The value returned by the remote function on the server
 */
Callback.invokeFunction = function invokeFunction(functionName, args)
{
	var message = Callback.createQuery(functionName, args);
	log.trace("Invoking function " + functionName + " synchronously with arguments encoded as: " + message);
	
	var extra = {functionName: functionName}; // this will be passed back to error handling methods
	
	var options = 
	{
		url: Jaxer.CALLBACK_URI,
		cacheBuster: false,
		method: Callback.METHOD,
		async: false,
		onsuccess: null, // we'll use the return value of XHR.send() and any errors it throws
		onfailure: null,
		timeout: Callback.TIMEOUT,
		ontimeout: null,
		headers: null,
		onreadytosend: null,
		onfinished: null,
		contentType: "application/x-www-form-urlencoded",
		testSuccess: Jaxer.XHR.testSuccess,
		as: "text",
		pollingPeriod: Callback.POLLING_PERIOD
	};
	
	try
	{
		var response = Jaxer.XHR.send(message, options, extra);
	}
	catch (e)
	{
		if (Jaxer.ALERT_CALLBACK_ERRORS)
		{
			alert("Error while contacting server to call server function '" + functionName + "': " + e);
		}
		throw e;
	}
	
	return Callback.processReturnValue(functionName, response);
};

/**
 * Returns the URL that can be used as a GET request to call a JavaScript
 * function on the server. 
 * <br><br>
 * The server listens for two special properties: "resultAs" and "paramsAs". 
 * <br><br>
 * If present, resultAs specifies how the result of functionToCall is to be
 * returned to the client. Valid values for resultAs are "text", "object", and
 * "wrappedObject" (default), which return the result of the callback as a
 * single string, JSON object literal, or JSON object literal with metadata,
 * respectively. 
 * <br><br>
 * If present, "paramsAs" specifies how the request is to be translated into
 * arguments for the functionToCall. Valid values for "paramsAs" are "text",
 * "object", and "default", which hands the GET or POST data to functionToCall
 * as a single string, a single hash (object literal) of name-value pairs, or as
 * regular JavaScript arguments with values extracted from paramsToPass,
 * respectively.
 * 
 * @alias Jaxer.Callback.getUrl
 * @param {Object}	functionToCall 
 * 		Name of the function (or the function itself) to call server-side
 * @param {Object}	paramsToPass 
 * 		An array of parameters (or the single parameter) to pass to the function
 * @param {String, Object} ...
 * 		Optional parameter(s) to append to the end of the URL as part of the
 * 		query string. Strings will be appended to the end of the URL separated
 * 		by a "&". Hashes will be appended as &name1=value&name2=value2...
 * @return {String}
 * 		The URL that can be called (via a GET) to invoke the function
 */
Callback.getUrl = function getUrl()
{
	var queryParts = Callback.getQueryParts.apply(this, arguments);
	return (Callback.getBaseUrl() + "?" + Callback.hashToQuery(queryParts));
};

/**
 * Returns the URL for use in callbacks, without any parameters
 * 
 * @advanced
 * @alias Jaxer.Callback.getBaseUrl
 * @return {String}
 * 		The URL to GET or POST to
 */
Callback.getBaseUrl = function getBaseUrl()
{
	return Jaxer.CALLBACK_URI;
};

/**
 * Returns a hash of the "form-like" name-value pairs needed to call a
 * JavaScript function on the server. These can be submitted to the server as a
 * GET request (but see Callback.getUrl which wraps this in a Url for you) or as
 * a POST request, and usually via an XMLHttpRequest mechanism.
 * <br><br>
 * The server listens for two special name-value pairs: "resultAs" and
 * "paramsAs". 
 * <br><br>
 * If present, resultAs specifies how the result of functionToCall is to be
 * returned to the client. Valid values for resultAs are "text", "object", and
 * "wrappedObject" (default), which return the result of the callback as a
 * single string, a JSON object literal, or a JSON object literal with metadata,
 * respectively. 
 * <br><br>
 * If present, "paramsAs" specifies how the request is to be translated into
 * arguments for the functionToCall. Valid values for "paramsAs" are "text",
 * "object", and "default", which hands the GET or POST data to functionToCall
 * as a single string, a single hash (object literal) of name-value pairs, or as
 * regular JavaScript arguments with values extracted from paramsToPass,
 * respectively.
 * 
 * @alias Jaxer.Callback.getQueryParts
 * @param {Object} functionToCall
 * 		Name of the function (or the function itself) to be called server-side
 * @param {Object} paramsToPass
 * 		An array of parameters (or the single parameter) to pass to the function
 * @param {String, Object} ...
 * 		Optional parameter(s) to append to the end of the URL as part of the
 * 		query string. String arguments should be "name=value" pairs joined by
 * 		"&" characters. If arguments are a hash, their properties are added to
 * 		the hash.
 * @return {Object}
 * 		The hash of the information needed to invoke the function
 */
Callback.getQueryParts = function getQueryParts(functionToCall, paramsToPass)
{
	var parts = {};
	// First normalize all the input arguments
	var functionName = (typeof functionToCall == "function") ? functionToCall.name : functionToCall; // Allows passing in a function or its name
	if (paramsToPass == null) paramsToPass = [];
	if (paramsToPass.constructor != Array) // Allows passing in a single parameter without wrapping it in an array
	{
		paramsToPass = [paramsToPass];
	}
	var serializedParams = [];
	for (var i=0; i<paramsToPass.length; i++)
	{
		var param = paramsToPass[i];
		var serializedParam = Jaxer.Serialization.toJSONString(param, { as: Jaxer.Serialization.JAXER_METHOD });
		if (serializedParam == undefined && param != undefined)
		{
			throw new Error("When calling function " + functionName + ", parameter #" + i + " cannot be sent because it is not serializable: " + param);
		}
		serializedParams.push(serializedParam);
	}
	// Any remaining arguments should be strings or hashes of options to be appended to the url
	for (var i=2; i<arguments.length; i++)
	{
		var arg = arguments[i];
		if (typeof arg == "string")
		{
			var argParts = arg.split("&");
			for (var j=0; j<argParts.length; j++)
			{
				var argPart = argParts[j].split("=");
				parts[argPart[0]] = (argPart.length > 1) ? argPart[1] : null;
			}
		}
		else
		{
			for (var p in arg)
			{
				parts[p] = arg[p];
			}
		}
	}
	parts[PAGE_SIGNATURE] = Jaxer.Callback[PAGE_SIGNATURE];
	parts[PAGE_NAME] = Jaxer.Callback[PAGE_NAME];
	parts[CALLING_PAGE] = Jaxer.Callback[CALLING_PAGE];
	parts[METHOD_NAME] = functionName;
	parts[PARAMS] = "[" + serializedParams.join(",") + "]";
	parts[UID] = "" + new Date().getTime() + "_" + Math.round(Math.random() * 1000000);
	
	return parts;
};

/**
 * A short convenience function to call a remote function, synchronously or
 * asynchronously based on whether or not you specify a callback function as the
 * third argument.
 * 
 * @alias Jaxer.Callback.remote
 * @param {String} functionName
 * 		The name of the remote function to call
 * @param {Object} args
 * 		A single argument, or an array of arguments, to pass to the remote
 * 		function
 * @param {Object} [callback]
 * 		If this is not specified, the call will be synchronous.
 * 		<br>
 * 		If this is specified, the call will be asynchronous.
 * 		<br><br>
 * 		If this is a function, this is the function to call upon a successful
 * 		return from the remote invocation. Its arguments are what the remote
 * 		function on the server returned.
 * 		<br><br>
 * 		If this is an array, its elements are as follows (each may be null):
 * 		<ol>
 * 			<li>
 * 				the callback function;
 * 			</li>
 * 			<li>
 * 				a function to call on an error, with arguments being the error,
 * 				the "extra" information object that has the functionName as its
 * 				one property, and the XMLHttpRequest object used for the call if
 * 				the call itself encountered an error;
 * 			</li>
 * 			<li>
 * 				the timeout to use, in milliseconds (defaults to
 * 				Jaxer.Callback.TIMEOUT). Use 0 to wait indefinitely.
 * 			</li>
 * 		</ol> 
 * 		<br><br>
 * 		If this is an object, its "callback", "errorHandler", and timeout
 * 		properties will be used, if any.
 * @return {Object}
 * 		If synchronous, the value returned by the remote function; if
 * 		asynchronous, an id by which the remote call can be canceled via
 * 		Jaxer.XHR.cancel()
 */
Jaxer.remote = function remote(functionName, args, callback)
{
	if (arguments.length == 3)
	{
		return Callback.invokeFunctionAsync(callback, functionName, args);
	}
	else
	{
		return Callback.invokeFunction(functionName, args);
	}
};

/**
 * Dynamically (at run time) creates proxies on the client from an array of names and an optional
 * namespace on which the function names will become properties.
 * 
 * @private
 * @alias Jaxer.Callback.createProxies
 * @param {String[]} proxyNames
 * 		The names of the functions to create proxies for
 * @param {Object} [namespace]
 * 		An optional namespace object. If given, the proxy foo will be accessible as <namespace>.foo;
 * 		if omitted, the procy foo will be accessible off the global ("window") as simply foo.
 */
Callback.createProxies = function createProxies(proxyNames, namespace)
{
	var js = [];
	for (var iName=0, lenNames = proxyNames.length; iName<lenNames; iName++)
	{
		var name = proxyNames[iName];
		var prop = namespace ? (namespace + "." + name) : name;
		js.push(prop + " = function " + name + "() { return Jaxer.remote('" + name + "', arguments); }");
		js.push(prop + ".async = function " + name + "_async(callback) { return Jaxer.remote('" + name + "', arguments, callback); }");
		js.push(prop + ".getUrl = function " + name + "_getUrl() { return Jaxer.Callback.getUrl.apply(this, Jaxer.Util.concatArrays(['" + name + "'], arguments)); }");
	}
	return js.join("\n");
};

Jaxer.Callback = Callback;

})();
