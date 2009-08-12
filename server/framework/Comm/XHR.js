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

// This is a server- and client-side, compressible module -- be sure to end each function declaration with a semicolon

var log = Jaxer.Log.forModule("XHR");

function getWindow()
{
	if (Jaxer.isOnServer)
	{
		return Jaxer.pageWindow || this.__parent__;
	}
	else
	{
		return window;
	}
};

/**
 * @namespace {Jaxer.XHR} Namespace to hold the Jaxer client-side cross-browser
 * wrapper around XMLHttpRequest.
 */
var XHR = {};

/**
 * The value of the "reason" property that indicates a timeout has occurred.
 * This property is set on the Error object that's thrown by XHR.send() during
 * synchronous requests that don't use the onsuccess function but rather just
 * return a response or throw an Error.
 * 
 * @advanced
 * @alias Jaxer.XHR.REASON_TIMEOUT
 * @property {String}
 */
XHR.REASON_TIMEOUT = "timeout";

/**
 * The value of the "reason" property that indicates a communication failure has
 * occurred. This property is set on the Error object that's thrown by
 * XHR.send() during synchronous requests that don't use the onsuccess function
 * but rather just return a response or throw an Error.
 * 
 * @advanced
 * @alias Jaxer.XHR.REASON_FAILURE
 * @property {String}
 */
XHR.REASON_FAILURE = "failure";

XHR.asyncRequests = {}; // Holds identifier codes for the pending async requests so you can call XHR.cancel() on them

/**
 * The default client-side function used to handle any errors that occur during
 * XMLHttpRequest processing by throwing an error describing them
 * 
 * @advanced
 * @alias Jaxer.XHR.onfailure
 * @param {Object} error
 * 		An error object describing the error, if one was thrown. Otherwise this
 * 		is null.
 * @param {Object} extra
 * 		Any extra information passed into Jaxer.XHR.send(), e.g. to make error
 * 		messages more informative.
 * @param {XMLHttpRequest} xhr
 * 		The XMLHttpRequest object that contains the information received from
 * 		the server,	e.g. in xhr.status and xhr.responseText. It may be null if
 * 		an error was encountered creating it.
 */
XHR.onfailure = function onfailure(error, extra, xhr)
{
	if (xhr) 
	{
		var status;
		try
		{
			status = xhr.status;
		}
		catch (e) {}
		throw new Error("XMLHttpRequest: Received status " + String(xhr.status) + " from the server\n" +
			"Response from server: " + xhr.responseText);
	}
	else if (error)
	{
		throw error;
	}
};

/**
 * The default client-side function used to handle any timeout errors that occur
 * during XMLHttpRequest processing by throwing an error describing them
 * 
 * @advanced
 * @alias Jaxer.XHR.ontimeout
 * @param {Error} timeout
 * 		The timeout error object encountered, having a "timeout" property with
 * 		its value indicating the timeout (in milliseconds) used in this request.
 * @param {Object} extra
 * 		Any extra information passed into Jaxer.XHR.send(), e.g. to make error
 * 		messages more informative.
 * @param {XMLHttpRequest} xhr
 * 		The XMLHttpRequest object that contains the information received from
 * 		the server, e.g. in xhr.status and xhr.responseText. It may be null if
 * 		an error was encountered creating it.
 */
XHR.ontimeout = function ontimeout(error, extra, xhr)
{
	throw new Error("XMLHttpRequest: Request timed out after " + (error.timeout/1000) + " seconds");
};

/**
 * Returns an XMLHttpRequest object by calling the platform-specific API for it.
 * On the server side of Jaxer, the XPCOM version of XMLHttpRequest is used.
 * 
 * @advanced
 * @alias Jaxer.XHR.getTransport
 * @return {XMLHttpRequest}
 */
XHR.getTransport = function getTransport()
{
	var xhr, e, win;
	win = getWindow();
	try
	{
		// On IE use the most common/standard ActiveX call to minimize version bugs/weirdnesses
		xhr = win.ActiveXObject ? new win.ActiveXObject("Microsoft.XMLHTTP") : new win.XMLHttpRequest();
	}
	catch (e) {} // May be needed on older versions of IE to prevent "automation error" notifications
	if (!xhr)
	{
		throw new Error("Could not create XMLHttpRequest" + (e ? '\n' + e : ''));
	}
	return xhr;
};

/**
 * The default function used to test whether the XMLHttpRequest got a successful
 * response or not, in particular using xhr.status, location.protocol and some
 * browser sniffing.
 * 
 * @advanced
 * @alias Jaxer.XHR.testSuccess
 * @param {XMLHttpRequest} xhr
 * 		The XMLHttpRequest object that got the response
 * @return {Boolean}
 * 		true if successful, false otherwise
 */
XHR.testSuccess = function testSuccess(xhr)
{
	var success = false;
	var win = getWindow();
	try
	{
		success =
			(!xhr.status && win.location && win.location.protocol == "file:") ||
			(xhr.status >= 200 && xhr.status < 300) || 
			(xhr.status == 304) ||
			(win.navigator && win.navigator.userAgent.match(/webkit/) && xhr.status == undefined);
	}
	catch(e) { }
	return success;
};

/**
 * The generic function used to send requests via XMLHttpRequest objects. Each
 * request gets its own XMLHttpRequest object, and async requests hold onto that
 * object until they're finished or timed out or canceled. On the server side of
 * Jaxer, only synchronous requests are supported.
 * <br><br>
 * For async requests, this returns a key that can be used to abort the request
 * via Jaxer.XHR.cancel().
 * <br><br>
 * For synchronous requests, returns the response of the server or throws an
 * exception if an error occurred, unless an onsuccess function was specified in
 * the options, in which case it passes the response to that function and also
 * handles any errors through the onfailure function if specified in the
 * options.
 * <br><br>
 * In any case, the response can be a text string or an XML DOM. To force one or
 * the other, set the "as" property on the options argument, e.g. if as="text"
 * it will definitely be a text string, if as="xml" it will definitely be an
 * XML DOM, and if as="e4x" it will be an E4X DOM (if E4X is supported -- 
 * which is always the case server-side but may not be client-side).
 * 
 * @alias Jaxer.XHR.send
 * @param {String} message
 * 		The message to send, usually as a query string
 * 		("name1=value&name2=value2...")
 * @param {Jaxer.XHR.SendOptions|Object} [options]
 * 		A JavaScript object (hashmap) of name: value property pairs specifying
 * 		how to handle this request.
 * @param {Object} extra
 * 		Any extra information that might be useful e.g. in the error handlers on
 * 		this request. This object is simply passed on to them if/when they're
 * 		called. E.g. Jaxer.Callback uses this information to pass the name of
 * 		the function being called remotely, so error messages can be more
 * 		informative.
 * @return {Object}
 * 		For async requests, a key to the XHR object; for synchronous requests
 * 		(with no onsuccess handler in the options), a text string or an XML DOM,
 * 		or an object containing detailed information about the response (if
 * 		the options specified extendedResponse=true)
 */
XHR.send = function send(message, options, extra)
{
	
	var win = getWindow();
	var xmlSerializer = (typeof win.XMLSerializer == "function") ? new win.XMLSerializer() : null;
	var e4xConstructor = (typeof win.XML == "function") ? win.XML : null;

	if (typeof message != "string")
	{
		if ((message == null) || (message == undefined) || (typeof message.toString != "function"))
		{
			message = '';
		}
		else
		{
			message = message.toString();
		}
	}
	options = options || {};
	var as = options.as || XHR.defaults.as || '';
	as = as.toLowerCase();
	var url = options.url || XHR.defaults.url || Jaxer.CALLBACK_URI;
	url = url.replace(/#.*$/, ''); // strip off any fragment
	var cacheBuster = (typeof options.cacheBuster == "undefined") ? XHR.defaults.cacheBuster : options.cacheBuster;
	if (cacheBuster) url += (url.match(/\?/) ? "&" : "?") + "_rnd" + ((new Date()).getTime()) + "=" + Math.random();
	var method = String(options.method || XHR.defaults.method || "GET").toUpperCase();
	if ((method == "GET") && (message != ''))
	{
		url += (url.match(/\?/) ? "&" : "?") + message;
		message = ''; // prevent submitting this twice in IE
	}
	var async = (typeof options.async == "undefined") ? XHR.defaults.async : options.async;
	var username = options.username || XHR.defaults.username || null;
	var password = options.password || XHR.defaults.password || null;
	var onsuccess = options.onsuccess || XHR.defaults.onsuccess;
	var onfailure = options.onfailure || XHR.defaults.onfailure;
	var onsslcerterror = options.onsslcerterror || XHR.defaults.onsslcerterror;
	var timeout = options.timeout || XHR.defaults.timeout || 0;
	var ontimeout = timeout ? (options.ontimeout || XHR.defaults.ontimeout) : null;
	var headers = options.headers || XHR.defaults.headers;
	var overrideMimeType = as ? ((as == 'xml' || as == 'e4x') ? 'application/xml' : 'text/plain') : options.overrideMimeType || XHR.defaults.overrideMimeType || null;
	var onreadytosend = options.onreadytosend || XHR.defaults.onreadytosend;
	var onfinished = options.onfinished || XHR.defaults.onfinished;
	var contentType = options.contentType || XHR.defaults.contentType;
	var testSuccess = options.testSuccess || XHR.defaults.testSuccess;
	if (typeof testSuccess != "function") testSuccess = XHR.testSuccess;
	var responseType = as ? ((as == 'xml' || as == 'e4x') ? as : 'text') : options.responseType || XHR.defaults.responseType || 'text';
	var extendedResponse = options.extendedResponse || XHR.defaults.extendedResponse;
	var pollingPeriod = options.pollingPeriod || XHR.defaults.pollingPeriod || 11;
	var getTransport = options.getTransport || XHR.defaults.getTransport;
	if (typeof getTransport != "function") getTransport = XHR.getTransport;

	var useOnFunctions = (typeof onsuccess == "function"); // otherwise, return a value or throw an error
	var error = null;

	var xhr = getTransport();
	
	// Open the transport:
	try
	{
		xhr.open(method, url, async, username, password);
	}
	catch (e)
	{
		error = new Error("xhr.open error: " + e + "\n\n" + "typeof xhr: " + (typeof xhr) + "\n\nparams: " + [method, url, async]);
		if (useOnFunctions) 
		{
			if (typeof onfailure == "function") onfailure(error, extra, xhr);
			return;
		}
		else 
		{
			throw error;
		}
	}
	
	// Get ready to send:
	if (headers && (typeof headers == "object")) // array or actual object
	{
		if (typeof headers.length == "number") // assume it's an array
		{
			for (var iHeader = 0, lenHeaders = headers.length; iHeader < lenHeaders; iHeader++) 
			{
				xhr.setRequestHeader(headers[iHeader][0], headers[iHeader][1]);
			}
		}
		else // assume it's an object of name => value pairs
		{
			for (var headerName in headers)
			{
				xhr.setRequestHeader(headerName, headers[headerName]);
			}
		}
	}
	else 
	{
		if (contentType != '') xhr.setRequestHeader("Content-Type", contentType);
		xhr.setRequestHeader("X-Requested-With", "XMLHttpRequest");
	}
	if (overrideMimeType && (typeof xhr.overrideMimeType == "function"))
	{
		xhr.overrideMimeType(overrideMimeType);
	}
	if (typeof onreadytosend == "function") onreadytosend(xhr, extra);
	
	// Get ready to receive: (modeled after jQuery)
	var finished = false;
	var pollId, asyncKey;
	var finish = function finish(cancelCode)
	{
		var response;
		if (!finished && xhr && ((xhr.readyState == 4) || (cancelCode == "timedout") || (cancelCode == "canceled")))
		{
			// prevent firing more than once
			finished = true; 

			// stop polling
			if (pollId)
			{
				asyncKey = 'id_' + pollId;
				win.clearInterval(pollId);
				delete XHR.asyncRequests[asyncKey];
			}
			
			var err = null;
			var msgIdentifier = (async ? "Asynchronous" : "Synchronous") + " request " +
				(asyncKey ? asyncKey + " " : "") + "to url " + url;
			
			if (cancelCode == "timedout")
			{
				log.trace(msgIdentifier + " timed out");
				err = new Error(msgIdentifier + " timed out");
				err.reason = XHR.REASON_TIMEOUT;
				err.timeout = timeout;
				if (useOnFunctions && (typeof ontimeout == "function")) 
				{
					ontimeout(err, extra, xhr);
				}
			}
			else if (cancelCode == "canceled")
			{
				log.trace(msgIdentifier + " canceled");
			}
			else
			{
				if (testSuccess(xhr)) // succeeded
				{
					var useXml;
					switch (responseType)
					{
						case "xml":
						case "e4x":
							useXml = true;
							break;
						case "auto":
							var autoType = "text";
							try 
							{
								autoType = xhr.getResponseHeader("content-type");
							} 
							catch (e) {}
							useXml = Boolean(autoType.match(/xml/i));
							break;
						default: // "text" and anything else
							useXml = false;
					}
					response = useXml ? xhr.responseXML : xhr.responseText;
					if (responseType == 'e4x' && xmlSerializer && e4xConstructor) 
					{
						var xmlRootString = xmlSerializer.serializeToString(response.documentElement);
						response = new e4xConstructor(xmlRootString);
					}
					var responseAsText = useXml ? xhr.responseText : response;
					log.trace(msgIdentifier + " received " + (useXml ? "XML" : "text") + " response: " +
						responseAsText.substr(0, 100) + (responseAsText.length > 100 ? '...' : ''));
					if (extendedResponse)
					{
						var responseHeadersString = xhr.getAllResponseHeaders();
						var responseHeaders = {};
						if (responseHeadersString)
						{
							responseHeadersString.split(/[\r\n]+/).forEach(function (headerString)
							{
								var matchedHeaders = /^([^\:]+)\: (.*)$/.exec(headerString);
								if (matchedHeaders && matchedHeaders.length == 3) 
								{
									var headerName = matchedHeaders[1];
									var headerValue = matchedHeaders[2];
									if (headerName in responseHeaders)
									{
										if (typeof responseHeaders[headerName] == "string")
										{
											responseHeaders[headerName] = [responseHeaders[headerName], headerValue];
										}
										else
										{
											responseHeaders[headerName].push(headerValue);
										}
									}
									else
									{
										responseHeaders[headerName] = headerValue;
									}
								}
							});
						}
						var setCookieHeaders = responseHeaders["Set-Cookie"];
						var setCookieStrings = setCookieHeaders ? (typeof setCookieHeaders == "string" ? [setCookieHeaders] : setCookieHeaders) : [];
						response = new XHR.ResponseData({
							  response: response
							, text: xhr.responseText
							, xml: xhr.responseXML
							, xhr: xhr
							, extra: extra
							, headers: responseHeaders
							, status: xhr.status
							, statusText: xhr.statusText
							, cookies: Jaxer.Util.Cookie.parseSetCookieHeaders(setCookieStrings)
							, certInfo: certInfo
						});
					}
					if (useOnFunctions) 
					{
						onsuccess(response, extra);
					}
				}
				else  // failed
				{
					log.trace(msgIdentifier + " failed");
					err = new Error(msgIdentifier + " failed");
					err.reason = XHR.REASON_FAILURE;
					err.status = xhr.status;
					if (useOnFunctions && (typeof onfailure == "function")) 
					{
						onfailure(err, extra, xhr);
					}
				}
			}
			
			// finish and clean up
			if (typeof onfinished == "function") onfinished(xhr, cancelCode, extra);
			pollId = asyncKey = xhr = url = undefined;
			
			if (!useOnFunctions && err) 
			{
				throw err;
			}
		}
		
		if (!useOnFunctions) return response;
	};

	// For async requests, look for received data, and timeout if requested and needed
	if (async)
	{
		// Poll for receiving data, instead of subscribing to onreadystatechange - modeled after jQuery
		pollId = win.setInterval(finish, pollingPeriod);
		asyncKey = 'id_' + pollId;
		XHR.asyncRequests[asyncKey] = {url: url, message: message, timeout: timeout, timestamp: new Date(), finish: finish};
		
		if (timeout)
		{
			win.setTimeout(function xhrTimeout()
				{
					if (!xhr) return; 					// we've already finished
					xhr.abort();						// otherwise, abort
					if (!finished) finish("timedout");	// and finish with a timeout if we haven't already finished
				}, timeout);
		}
	}

	// Handle SSL certificate errors -- server-side only
	var certInfo = null;
	if (Jaxer.isOnServer)
	{
		xhr.onsslcerterror = function(socketInfo, sslStatus, targetSite)
		{
			var cert = sslStatus.serverCert; /* nsIX509Cert */
			certInfo = new Util.Certificate.CertInfo(socketInfo, sslStatus, targetSite);
			var ignore = false; // by default, we abort on SSL errors
			if (typeof onsslcerterror == "function")
			{
				ignore = onsslcerterror(certInfo, cert, socketInfo, sslStatus, targetSite);
			}
			return ignore; // true - ignore, false - abort
		}
	}
	
	// Now actually send the request
	log.trace("Sending " + (async ? "asynchronous " : "synchronous ") + method +
		" request to url " + url + " with " + (message == '' ? "no data" : "data: " + message));
	xhr.send((message == '') ? null : message);
	log.trace("Sent");
	
	if (async) // For async requests, return an id by which requests may be aborted or prematurely timed out
	{
		log.trace("Response will be received asynchronously with key: " + asyncKey);
		return asyncKey;
	}
	else // And for synchronous requests, force the receiving. Timeout is not possible.
	{
		var response = finish(false);
		if ((typeof(response) == "object") && ("documentElement" in response))
		{
			var docElement = response.documentElement;
			if (docElement && docElement.nodeName == "parsererror")
			{
				error = new Error("Error reading returned XML: " + docElement.firstChild.data + "\nXHR params: " + [method, url, async]);
				if (useOnFunctions) 
				{
					if (typeof onfailure == "function") onfailure(error, extra, xhr);
				}
				else 
				{
					throw error;
				}
			}
		}
		if (!useOnFunctions)
		{
			return response;
		}
	}
	
};

/**
 * Cancels the pending async XMLHttpRequest if its response has not yet been
 * received and if it has not yet timed out.
 * 
 * @alias Jaxer.XHR.cancel
 * @param {Number} asyncKey
 * 		The key that Jaxer.XHR.send() returned when the request was created.
 * @return {Boolean}
 * 		true if the request was found and canceled, false if it was not found 
 * 		(i.e. was not in the pending queue)
 */
XHR.cancel = function cancel(asyncKey)
{
	if (typeof XHR.asyncRequests[asyncKey] != "undefined")
	{
		log.trace("Canceling request " + asyncKey);
		XHR.asyncRequests[asyncKey].finish("canceled");
		return true;
	}
	else
	{
		return false; // nothing to cancel
	}
};

/**
 * @classDescription {Jaxer.XHR.ResponseData} A hashmap containing detaild information
 * about the response from an XHR.send.
 */

/**
 * A hashmap containing detailed information about the response from an XHR.send.
 * This is returned as the response of XHR.send when the SendOptions specify
 * extendedResponse=true.
 * 
 * @constructor
 * @alias Jaxer.XHR.ResponseData
 * @return {Jaxer.XHR.ResponseData}
 * 		Returns an instance of ResponseData.
 */
XHR.ResponseData = function ResponseData(values)
{
	/**
	 * The responseText string or responseXML XMLDocument of the response,
	 * depending on the SendOptions and the returned content type
	 *
	 * @alias Jaxer.XHR.ResponseData.prototype.response
	 * @property {Object}
	 */
	this.response = values.response;
	
	/**
	 * The responseText of the response, or null if none
	 *
	 * @alias Jaxer.XHR.ResponseData.prototype.text
	 * @property {String}
	 */
	this.text = values.text;
	
	/**
	 * The responseXML of the response, or null if none
	 *
	 * @alias Jaxer.XHR.ResponseData.prototype.xml
	 * @property {XMLDocument}
	 */
	this.xml = values.xml;
	
	/**
	 * The XMLHttpRequest object used in the request-response
	 *
	 * @alias Jaxer.XHR.ResponseData.prototype.xhr
	 * @property {XMLHttpRequest}
	 */
	this.xhr = values.xhr;
	
	/**
	 * Information about the SSL certificate used in this request-response.
	 * This is only available server-side.
	 * NOTE: currently this is only available when an SSL certificate error
	 * was encountered, and the onsslcerterror function was set and
	 * returned true.
	 *
	 * @alias Jaxer.XHR.ResponseData.prototype.certInfo
	 * @property {Jaxer.Util.Certificate.CertInfo}
	 * @see Jaxer.XHR.SendOptions.prototype.onsslcerterror
	 */
	this.certInfo = values.certInfo;
	
	/**
	 * The value of the "extra" parameter, if any, passed into XHR.send.
	 *
	 * @alias Jaxer.XHR.ResponseData.prototype.extra
	 * @property {Object}
	 */
	this.extra = values.extra;
	
	/**
	 * A hashmap containing properties corresponding to the names of the
	 * response headers. For each property, if the header name was present
	 * multiple times in the response, the value of the property is an
	 * array of the corresponding header values; otherwise, the value
	 * of the property is the value of the header.
	 *
	 * @alias Jaxer.XHR.ResponseData.prototype.headers
	 * @property {Object}
	 */
	this.headers = values.headers;
	
	/**
	 * The HTTP status code of the response (e.g. 200)
	 *
	 * @alias Jaxer.XHR.ResponseData.prototype.status
	 * @property {Number}
	 */
	this.status = values.status;
	
	/**
	 * The HTTP status text of the response (e.g. "OK")
	 *
	 * @alias Jaxer.XHR.ResponseData.prototype.statusText
	 * @property {String}
	 */
	this.statusText = values.statusText;
	
	/**
	 * An array of cooky directives indicated in the response via the
	 * "Set-Cookie" header. Each cookie is represented by
	 * an object with properties corresponding to its 
 	 * name, value, expires, path, and domain.
	 *
	 * @see Jaxer.Util.Cookie.parseSetCookieHeaders
	 * @alias Jaxer.XHR.ResponseData.prototype.cookies
	 * @property {Array}
	 */
	this.cookies = values.cookies;
	
};

/**
 * @classDescription {Jaxer.XHR.SendOptions} Options used to define the behavior
 * of Jaxer.XHR.send.
 */

/**
 * Options used to define the behavior of Jaxer.XHR.send. Create a new Jaxer.XHR.SendOptions()
 * to get the default options, then modify its properties as needed before
 * passing it to Jaxer.XHR.send.
 * 
 * @constructor
 * @alias Jaxer.XHR.SendOptions
 * @return {Jaxer.XHR.SendOptions}
 * 		Returns an instance of SendOptions.
 */
XHR.SendOptions = function SendOptions()
{
	/**
	 * The URL to which the XMLHttpRequest is to be sent. On the client side,
	 * defaults to Jaxer.CALLBACK_URI which is used to handle function callbacks
	 * from client-side proxies to their server-side counterparts.
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.url
	 * @property {String}
	 */
	this.url = Jaxer.CALLBACK_URI;
	
	/**
	 * If true (default, client-side), a random name and value query pair will be appended to
	 * the URL on each call
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.cacheBuster
	 * @property {Boolean}
	 */
	this.cacheBuster = Jaxer.isOnServer ? false : true;
	
	/**
	 * Should be "GET" (default, server-side) or "POST" (default, client-side)
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.method
	 * @property {String}
	 */
	this.method = Jaxer.isOnServer ? "GET" : "POST";
	
	/**
	 * Set to true for asynchronous, false for synchronous. By default it's
	 * true client-side and false server-side. To use it server-side, see also
	 * Jaxer.Thread.waitFor.
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.async
	 * @property {Boolean}
	 * @see Jaxer.Thread.waitFor
	 */
	this.async = Jaxer.isOnServer ? false : true;
	
	/**
	 * If the target URL requires authentication, specify this username, 
	 * otherwise leave this as null.
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.username
	 * @property {String}
	 */
	this.username = null;
	
	/**
	 * If the target URL requires authentication, specify this password, 
	 * otherwise leave this as null.
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.password
	 * @property {String}
	 */
	this.password = null;
	
	/**
	 * Set to a function to call if successful. Its arguments are the response
	 * received back from the server, and any "extra" information passed in when
	 * calling send(). For synchronous calls, you can optionally set onsuccess
	 * to null to have XHR.send() return a value directly (and throw errors on
	 * failure/timeout).
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.onsuccess
	 * @property {Function}
	 */
	this.onsuccess = null;
	
	/**
	 * Set to a custom callback function to call if unsuccessful (by default set
	 * to Jaxer.XHR.onfailure client-side). Its arguments are the error
	 * encountered, the "extra" information from the caller, and the XHR
	 * instance.
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.onfailure
	 * @property {Function}
	 */
	this.onfailure = Jaxer.isOnServer ? null : XHR.onfailure;
	
	/**
	 * Set to a custom callback function to call if an SSL request fails
	 * due to a certificate error. Its arguments are the certInfo (an object
	 * containing properties describing the certificate and its status), the
	 * cert, and the XHR's socketInfo,
	 * sslStatus, and targetSite. It should return true to ignore the error,
	 * or false to abort the request. This is only available server-side.
	 * If (and only if) this is set to a function that returns true, 
	 * information about the failure can be retrieved from the extended 
	 * response's certInfo property.
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.onsslcerterror
	 * @property {Function}
	 * @see Jaxer.XHR.ResponseData.prototype.certInfo
	 * @see Jaxer.Util.Certificate.CertInfo
	 */
	this.onsslcerterror = null;
	
	/**
	 * For async (client-side) requests, set to number of milliseconds before
	 * timing out, or 0 to wait indefinitely
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.timeout
	 * @property {Number}
	 * @see Jaxer.XHR.defaults.timeout
	 * @see Jaxer.XHR.SendOptions.DEFAULT_TIMEOUT
	 */
	this.timeout = XHR.SendOptions.DEFAULT_TIMEOUT;
	
	/**
	 * Set to a custom timeout function to call if timeout is used and the async
	 * request has timed out. Its arguments are the timeout error encountered, 
	 * the "extra" information from the caller, and the XHR instance.
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.ontimeout
	 * @property {Function}
	 */
	this.ontimeout = null;
	
	/**
	 * Set to null to use default headers; set to an array of [name, value]
	 * arrays to use custom headers instead, or to an object containing
	 * properties to use as the headers
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.headers
	 * @property {Object|Array}
	 */
	this.headers = null;
	
	/**
	 * Set to "text" to force interpreting the response as text regardless of
	 * mimetype. Set to "xml" to force interpreting the response as XML
	 * regardless of mimetype and returning the XML as an XML (DOM) object via
	 * XMLHttpRequest.responseXML. Set to null to not force anything - see
	 * overrideMimeType and responseType for finer control.
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.as
	 * @property {String}
	 */
	this.as = null;
	
	/**
	 * Set to null to use whatever mimetype the server sends in the response;
	 * set to a mimetype string (e.g. "text/plain") to force the response to be
	 * interpreted using the given mimetype
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.overrideMimeType
	 * @property {String}
	 */
	this.overrideMimeType = null;
	
	/**
	 * Set to a custom function to call just before sending (e.g. to set custom
	 * headers, mimetype, keep reference to xhr object, etc.)
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.onreadytosend
	 * @property {Function}
	 */
	this.onreadytosend = null;
	
	/**
	 * Set to a custom function to call when done receiving (or timed out),
	 * usually to abort()
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.onfinished
	 * @property {Function}
	 */
	this.onfinished = null;
	
	/**
	 * The content type of the request being sent (by default
	 * "application/x-www-form-urlencoded")
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.contentType
	 * @property {String}
	 */
	this.contentType = "application/x-www-form-urlencoded";
	
	/**
	 * If this is set to true, the response returned directly
	 * or passed to an onsuccess handler will contain detailed
	 * information about the response, in the form of a
	 * Jaxer.XHR.ResponseData object.
	 * 
	 * @see Jaxer.XHR.ResponseData
	 * @alias Jaxer.XHR.SendOptions.extendedResponse
	 * @property {Boolean}
	 */
	this.extendedResponse = false;
	
	/**
	 * Set to a custom function that receives the XMLHttpRequest (after
	 * readyState == 4) and tests whether it succeeded (by default
	 * Jaxer.XHR.testSuccess)
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.testSuccess
	 * @property {Function}
	 */
	this.testSuccess = XHR.testSuccess;
	
	/**
	 * Set to "text" (default) to use the responseText, to "xml" to use the
	 * responseXML, or "auto" to use the response's content-type to choose
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.responseText
	 * @property {String}
	 */
	this.responseType = "text";
	
	/**
	 * For async requests, the number of milliseconds between
	 * polling for onreadystatechange, by default 11
	 *
	 * @advanced
	 * @alias Jaxer.XHR.SendOptions.prototype.pollingPeriod
	 * @property {Number}
	 */
	this.pollingPeriod = 11;
	
	/**
	 * The function to use to create the XMLHttpRequest, by default
	 * XHR.getTransport
	 *
	 * @alias Jaxer.XHR.SendOptions.prototype.getTransport
	 * @property {Function}
	 */
	this.getTransport = XHR.getTransport;
};

/**
 * The default value to use when creating new Jaxer.XHR.SendOptions() objects,
 * in milliseconds. It's only used for async requests.
 * It defaults to 30 seconds (30000) on the server, and 0 on the client.
 * 0 means no timeout.
 * 
 * NOTE: To set the the default timeout to use when creating new XHR()s
 * (i.e. XMLHttpRequests), set Jaxer.XHR.defaults.timeout instead.
 * 
 * @alias Jaxer.SendOptions.DEFAULT_TIMEOUT
 * @property {Number}
 */
XHR.SendOptions.DEFAULT_TIMEOUT = Jaxer.isOnServer ? 30000 : 0;

/**
 * The default SendOptions which new calls to Jaxer.XHR.send(message, options,
 * extra) will use, unless overridden by the options argument. This is slightly
 * different for client-side and server-side requests (e.g. server-side requests
 * are by default synchronous).
 * 
 * @alias Jaxer.XHR.defaults
 * @property {Jaxer.XHR.SendOptions}
 */
XHR.defaults = new XHR.SendOptions();

/**
 * The default value to use when creating new Jaxer.XHR (XMLHttpRequest) requests,
 * in milliseconds. This also applies to anything that uses XHRs, such as Jaxer.Web.get.
 * It's only used for async requests.
 * It defaults to 30 seconds (30000) on the server, and 0 on the client.
 * 0 means no timeout.
 * 
 * NOTE: To set the the default timeout to use when creating new XHR.SendOptions()
 * objects, set Jaxer.XHR.SendOptions.timeout instead.
 * 
 * @alias Jaxer.defaults.timeout
 * @property {Number}
 */

Jaxer.XHR = XHR;

})();
