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

var log = Log.forModule("Response");

/**
 * @classDescription {Jaxer.Response} Current response and its associated
 * information.
 */

/**
 * An instance of this object has the lifecycle of the current response and
 * contains information about it.
 * 
 * This constructor is typically never invoked by the user. During the processing
 * of the page Jaxer will create an instance of this object named Jaxer.response, 
 * this instance should be used to introspect and manage the response details.
 * 
 * @constructor
 * @alias Jaxer.Response
 * @param {Object} evt
 * 		The Jaxer Core event whose data is used to initialize this Response
 * 		object instance and to output to the client
 * @return {Jaxer.Response}
 * 		Returns an instance of Response.
 */
var Response = function Response(evt)
{

	var _response = evt.Response;
	this._response = _response; // For internal (Jaxer framework) use only -- APIs may change
	
	var _earlyExit = false;
	
	var hasDomBeenTouched = false;
	var hasSideEffect = false;
	var error = null;
	var errorLogged = false;
	var errorShown = false;
	var clientFrameworkOverridden = false;
	var overriddenClientFrameworkValue = undefined;
	var overriddenClientFrameworkIsSrc = false;
	var willSerializeDOM = true;
	
	// If a fatal error has already occurred, the DOM coming out of Jaxer
	// will surely be different than what went in
	if (Jaxer.fatalError) hasDomBeenTouched = true;
	
	this.allow = function allow(isAllowed)
	{
		_response.frameworkFailed = !Boolean(isAllowed);
	}
	
	/**
	 * This will be true when the document processing has been exited before
	 * reaching the end of the document, via the exit() method or another method
	 * such as redirect() which uses exit() internally. When that happens,
	 * some post-processing steps such as callback proxying will be omitted. 
	 * 
	 * @advanced
	 * @alias Jaxer.Response.prototype.earlyExit
	 * @property {Boolean}
	 */
	this.__defineGetter__("earlyExit", function() 
	{ 
		return this._earlyExit; 
	});
	
	/**
	 * Remembers that the DOM has been altered in some way, which ensures that
	 * the final response will be the Jaxer-produced one rather than the
	 * original document Jaxer received.
	 * 
	 * @advanced
	 * @alias Jaxer.Response.prototype.noteDomTouched
	 */
	this.noteDomTouched = function noteDomTouched()
	{
		hasDomBeenTouched = true;
	}
	
	/**
	 * Remembers that processing this page has had some side effect, which
	 * ensures that the final response will be treated as having been processed
	 * by Jaxer, whether or not its contents have been modified.
	 * 
	 * @advanced
	 * @alias Jaxer.Response.prototype.noteSideEffect
	 */
	this.noteSideEffect = function noteSideEffect()
	{
		hasSideEffect = true;
	}
	
	/**
	 * Has the DOM possibly been modified in any way?
	 * 
	 * @advanced
	 * @alias Jaxer.Response.prototype.getDomTouched
	 * @return {Boolean} 
	 * 		true if it has (or might have), false if it could not have been
	 */
	this.getDomTouched = function getDomTouched()
	{
		return hasDomBeenTouched;
	}
	
	/**
	 * Has there been any possible side effect (e.g. database accessed, etc.)?
	 * 
	 * @advanced
	 * @alias Jaxer.Response.prototype.getSideEffect
	 * @return {Boolean}
	 * 		true if there has been (or might have been), false if there could
	 * 		not have been
	 */
	this.getSideEffect = function getSideEffect()
	{
		return hasSideEffect;
	}
	
	/**
	 * A collection of the HTTP response headers that will be returned to the browser,
	 * as properties on this simple JavaScript object. 
	 * If multiple headers have the same name, their values are given as an array.
	 * NOTE: some headers are set by the web server after Jaxer is done, and they
	 * will not appear here.
	 * 
	 * @alias Jaxer.Response.prototype.headers
	 * @property {Object}
	 */
	this.headers = {}; // This will be a MultiHash

	var headerCount = _response.getHeaderCount();
	for (var i=0; i<headerCount; i++)
	{
		var name = _response.getHeaderName(i);
		var value = _response.getValueByOrd(i);
		Jaxer.Util.MultiHash.add(this.headers, name, value);
	}
	
	var originalHeaders = Jaxer.Util.clone(this.headers, true);
	
	/**
	 * Adds an HTTP header to the response. If an existing header with this name exists, 
	 * this value will be appended -- if you don't want that, just set
	 * Jaxer.response.headers[name] = value
	 * 
	 * @alias Jaxer.Response.prototype.addHeader
	 * @param {String} name
	 * 		The name of the HTTP header
	 * @param {String} value
	 * 		The value to use
	 */
	this.addHeader = function addHeader(name, value)
	{
		Jaxer.Util.MultiHash.add(this.headers, name, value);
	}
	
	/**
	 * Removes an HTTP header with a specific value from the response.
	 * If there is no such name-value pair, it does nothing.
	 * If an existing header with this name exists and has multiple values, 
	 * this value will be removed -- if you don't want that, just delete
	 * Jaxer.response.headers[name]
	 * 
	 * @alias Jaxer.Response.prototype.removeHeader
	 * @param {String} name
	 * 		The name of the HTTP header
	 * @param {String} value
	 * 		The value to remove
	 */
	this.removeHeader = function removeHeader(name, value)
	{
		Jaxer.Util.MultiHash.remove(this.headers, name, value);
	}
	
	/**
	 * Actually sets the name-value pairs on the underlying response object.
	 * This should be called automatically at the end of the procesing
	 * 
	 * @private
	 * @alias Jaxer.Response.prototype.persistHeaders
	 */
	this.persistHeaders = function persistHeaders()
	{
		var diff = Jaxer.Util.MultiHash.diff(this.headers, originalHeaders);
		var name;
		for (name in diff.deleted)
		{
			setHeaderValues(this, name, '');
		}
		for (name in diff.added)
		{
			setHeaderValues(this, name, String(diff.added[name]));
		}
		for (name in diff.changed)
		{
			setHeaderValues(this, name, String(diff.changed[name]));
		}
	}
	
	// A utility function
	function setHeaderValues(self, name, values)
	{
		self._response.deleteHeader(name);
		if (values || (values == 0))
		{
			if (!values.__parent__ || (values.constructor != values.__parent__.Array))
			{
				values = [values];
			}
			values.forEach(function(value)
			{
				self._response.addHeader(name, value, false);
			});
		}
	}
	
	/**
	 * Sets the contents (body) of the response. Once the response's contents
	 * are set this way, the DOM is no longer serialized into the response
	 * contents at the end of page processing.
	 * 
	 * @alias Jaxer.Response.prototype.setContents
	 * @param {String, XMLDocument, XML, Object} contents
	 * 		The contents to use for the body of the response. 
	 * 		If this is an XMLDocument or an E4X XML object, its string representation
	 * 		will be used, and a Content-Type header of "application/xml" will be returned;
	 * 		if this is another type of object, its JSON representation
	 * 		will be used, and a Content-Type header of "application/json" will be returned;
	 * 		otherwise the contents will be the String() of the passed-in value with no
	 * 		Content-Type impact.
	 * 		Note if a Content-Type header was already specified in this response, that will not
	 * 		be changed in any case.
	 * @param {Number} [statusCode]
	 * 		The integer value of the HTTP status code to return. 
	 * 		If 0 or null or not specified, it is not set.
	 * @param {String} [reasonPhrase]
	 * 		The string description of the HTTP status code to return.
	 * 		If the statusCode is 0 or null or not specified, the reasonPhrase is not set.
	 * 		Otherwise it defaults to the standard reason phrase for the statusCode
	 * 		("OK" for 200)
	 */
	this.setContents = function setContents(contents, statusCode, reasonPhrase)
	{
		willSerializeDOM = false;
		var htmlDocType = contents ? Util.findInGlobalContext("HTMLDocument", contents) : null;
		var htmlEltType = contents ? Util.findInGlobalContext("HTMLElement", contents) : null;
		
		var setContentType = false;
		if (contents == null || contents == undefined)
		{
			contents = '';
		}
		else if (typeof contents == 'xml') // e4x
		{
			contents = contents.toXMLString();
			setContentType = 'application/xml';
		}
		else if (contents.constructor == Util.findInGlobalContext("XMLDocument", contents))
		{
			var serializer = Util.findInGlobalContext("XMLSerializer", contents);
			contents = serializer.serializeToString(contents);
			setContentType = 'application/xml';
		}
		else if ((htmlDocType && (contents instanceof htmlDocType)) ||
				 (htmlEltType && (contents instanceof htmlEltType)) )
		{
			contents = Util.DOM.toHTML(contents);
			setContentType = 'text/html';
		}
		else if (typeof contents == "object")
		{
			contents = Serialization.toJSONString(contents, { as: Serialization.JAXER_METHOD });
			setContentType = 'application/json';
		}
		else
		{
			contents = String(contents);
		}
		
		if (statusCode)
		{
			this.setStatus(statusCode, reasonPhrase);
		}
		
		if ((typeof setContentType == "string") && !('Content-Type' in this.headers))
		{
			this.headers['Content-Type'] = setContentType;
		}
		
		_response.setContents(contents);
	}
	
	/**
	 * Sets the contents (body) of the response to the given binary (byte array) data. 
	 * Once the response's contents are set this way, the DOM is no longer serialized 
	 * into the response contents at the end of page processing.
	 * 
	 * @alias Jaxer.Response.prototype.setContentsBinary
	 * @param {Number[]} contents
	 * 		The contents to use for the body of the response. 
	 * 		It should be an array of numbers between 0 and 255, to be treated
	 * 		as a byte array.
	 * 		The data should be appropriate for the Content-Type header used in this response.
	 * @param {String} [contentType]
	 * 		The Content-Type to be used for this response. If this is not specified or
	 * 		is null, a Content-Type should be set separately.
	 * @param {Number} [statusCode]
	 * 		The integer value of the HTTP status code to return. 
	 * 		If 0 or null or not specified, it is not set.
	 * @param {String} [reasonPhrase]
	 * 		The string description of the HTTP status code to return.
	 * 		If the statusCode is 0 or null or not specified, the reasonPhrase is not set.
	 * 		Otherwise it defaults to the standard reason phrase for the statusCode
	 * 		("OK" for 200)
	 * @example
	 * 		<pre>
	 * 			// If the image is in your current folder rather than the default file location:
	 * 			var path = Jaxer.Dir.resolve("image.png", Jaxer.request.currentFolder);
	 * 			var file = new Jaxer.File(path);
	 * 			file.open('rb');
	 * 			var contents = file.read();
	 * 			file.close();
	 * 			Jaxer.response.setContentsBinary(contents, "image/png");
	 * 		</pre>
	 */
	this.setContentsBinary = function setContentsBinary(contents, contentType, statusCode, reasonPhrase)
	{
		willSerializeDOM = false;
		
		var setContentType = false;
		if (contents == null || contents == undefined)
		{
			contents = [];
		}
		else if (!Util.isArray(contents))
		{
			throw new Exception("For setContentsBinary, the contents must be a (byte) Array")
		}
		
		if (typeof contentType == "string")
		{
			setContentType = contentType;
		}
		
		if (statusCode)
		{
			this.setStatus(statusCode, reasonPhrase);
		}
		
		if ((typeof setContentType == "string"))
		{
			this.headers['Content-Type'] = setContentType;
		}
		
		_response.setBinaryContents(contents, contents.length);
	}
	
	/**
	 * Sets the contents (body) of the response to the given file. 
	 * The file's contents will be streamed directly back to the web server,
	 * chunk by chunk via the socket to the web server,
	 * without going through the Jaxer JavaScript framework.
	 * Once the response's contents are set this way, the DOM is no longer serialized 
	 * into the response contents at the end of page processing.
	 * 
	 * @alias Jaxer.Response.prototype.setContentsFile
	 * @param {String} path
	 * 		The path to the file on disk.
	 * 		The file should be appropriate for the Content-Type header used in this response.
	 * @param {String} [contentType]
	 * 		The Content-Type to be used for this response. If this is not specified or
	 * 		is null, it will be set from Jaxer.File.mimeType(path), unless that too returns a null.
	 * 		If so, then a Content-Type should be set separately.
	 * @param {Number} [statusCode]
	 * 		The integer value of the HTTP status code to return. 
	 * 		If 0 or null or not specified, it is not set.
	 * @param {String} [reasonPhrase]
	 * 		The string description of the HTTP status code to return.
	 * 		If the statusCode is 0 or null or not specified, the reasonPhrase is not set.
	 * 		Otherwise it defaults to the standard reason phrase for the statusCode
	 * 		("OK" for 200)
	 * @example
	 * 		<pre>
	 * 			// If the image is in your current folder rather than the default file location:
	 * 			var path = Jaxer.Dir.resolve("image.png", Jaxer.request.currentFolder);
	 * 			Jaxer.response.setContentsFile(path, "image/png");
	 * 		</pre>
	 */
	this.setContentsFile = function setContentsFile(path, contentType, statusCode, reasonPhrase)
	{
		willSerializeDOM = false;

		var setContentType = false;
		
		if (typeof path != "string")
		{
			throw new Exception("For setContentsFile, the path must be a string")
		}
		path = Dir.resolve(path);
		if (!File.exists(path))
		{
			throw new Exception("No file found at path: " + path);
		}
		
		if (typeof contentType != "string")
		{
			contentType = File.mimeType(path);
		}
		if (typeof contentType == "string")
		{
			setContentType = contentType;
		}
		
		if (statusCode)
		{
			this.setStatus(statusCode, reasonPhrase);
		}
		
		if ((typeof setContentType == "string"))
		{
			this.headers['Content-Type'] = setContentType;
		}
		
		_response.sendFile(path);
	}

	/**
	 * Gets the contents (body) of the response.
	 * For regular page requests, this will return null unless setContents has
	 * been called, in which case it will return the value set by the 
	 * last call to setContents.
	 * This should only be used for non-binary return types, i.e. it should
	 * not be used after calling setContentsBinary or setContentsFile.
	 * For callbacks, this should only be called at the
	 * very end of callback processing, when the contents have been explicitly
	 * set by the Jaxer framework using the setContents method.
	 * 
	 * @advanced
	 * @alias Jaxer.Response.prototype.getContents
	 * @return {String}
	 * 		The explicitly-set contents at this time, or null if no explicit
	 * 		contents have been set (so the DOM will be serialized if
	 * 		this is a regular page request).
	 */
	this.getContents = function getContents()
	{
		return (willSerializeDOM ? null : _response.contents);
	}
	
	/**
	 * Sets the HTTP status code and reason phrase for the response.
	 * By default 
	 * 
	 * @alias Jaxer.Response.prototype.setStatus
	 * @param {Number} [statusCode]
	 * 		The integer value of the HTTP status code.
	 * 		Defaults to 200
	 * @param {String} [reasonPhrase]
	 * 		The string description of the HTTP status code.
	 * 		Defaults to the standard reason phrase for the statusCode
	 * 		("OK" for 200)
	 */
	this.setStatus = function setStatus(statusCode, reasonPhrase)
	{
		statusCode = Util.Math.forceInteger(statusCode, 200); // 200: OK
		this._response.statusCode = statusCode;
		
		// HTTP Reason Phrase:
		if (typeof reasonPhrase != "string") 
		{
			reasonPhrase = this.statusCodeToReasonPhrase(statusCode);
		}
		this._response.statusPhrase = reasonPhrase;
	}
	
	/**
	 * Gets the integer value of the HTTP status code for the request
	 * 
	 * @alias Jaxer.Response.prototype.statusCode
	 * @property {Number}
	 */
	this.__defineGetter__("statusCode", function() 
	{ 
		return this._response.statusCode; 
	});
	
	/**
	 * Gets the string description of the HTTP status code for the request
	 * 
	 * @alias Jaxer.Response.prototype.reasonPhrase
	 * @property {String}
	 */
	this.__defineGetter__("reasonPhrase", function() 
	{ 
		return this._response.statusPhrase; 
	});
	
	/**
	 * Once this is called, the contents of the response will be the same as the
	 * contents that Jaxer received in this request, i.e. all DOM changes will
	 * be discarded. This is ONLY effective for regular requests, not callbacks.
	 * 
	 * @advanced
	 * @alias Jaxer.Response.prototype.useOriginalContents
	 */
	this.useOriginalContents = function useOriginalContents()
	{
		log.trace("Jaxer did not end up modifying this page or having any (known) side-effects - using original response in lieu of a Jaxer-generated one")
		_response.shouldUseOriginalContent = true;
	}
	
	/**
	 * Stops processing the current request's HTML page and JavaScript, and returns this
	 * response to the browser via the web server.
	 * 
	 * @alias Jaxer.Response.prototype.exit
	 * @param {Number} [statusCode]
	 * 		The HTTP status code to return to the browser: by default it is 200.
	 * @param {String, XMLDocument, XML} [contents]
	 * 		The contents to use for the body of the response. If this is null
	 * 		or undefined, and setContents has not been called on this response,
	 * 		the current DOM will be serialized and returned as the response contents.
	 * 		If this is an XMLDocument or an E4X XML object, its string representation
	 * 		will be used, and a Content-Type header of "application/xml" will be returned
	 * 		(unless a Content-Type header was already specified in this response)
	 * @param {String} [reasonPhrase]
	 * 		The text description (e.g. "OK") of the status code. For all the standard status codes
	 * 		this is optional, as the standard description for the status code will
	 * 		be used by default.
	 * @see Jaxer.Response.prototype.setContents
	 * @see Jaxer.Response.prototype.addHeader
	 * @see Jaxer.Response.prototype.redirect
	 * @see Jaxer.Response.prototype.statusCodeToReasonPhrase
	 */
	this.exit = function exit(statusCode, contents, reasonPhrase)
	{
		// response contents (body of response):
		if (contents != null && contents != undefined) 
		{
			this.setContents(contents);
		}
		
		// HTTP Status Code:
		statusCode = Util.Math.forceInteger(statusCode, 200); // 200: OK
		this.setStatus(statusCode, reasonPhrase);
		
		// Indicate the premature exit:
		_earlyExit = true;
		
		// Now stop DOM processing and JS processing
		Jaxer.request._request.StopRequest();
		Jaxer.request._request.Exit();
	}
	
	/**
	 * Stops processing the current request's HTML page and JavaScript, and returns
	 * a redirect-type HTTP response to the browser. No contents (response body)
	 * are returned to the browser, and the DOM is not serialized.
	 * 
	 * @alias Jaxer.Response.prototype.redirect
	 * @param {String} url
	 * 		The URL to which the browser should redirect the request. This is mandatory
	 * 		unless the Location header has already been set for this response.
	 * @param {Number} [statusCode]
	 * 		The HTTP status code to return to the browser: by default it is 307 (temporary redirect).
	 * @param {String} [reasonPhrase]
	 * 		The text description of the status code. For all the standard status codes
	 * 		this is optional, as the standard description for the status code will
	 * 		be used by default.
	 * @see Jaxer.Response.prototype.setContents
	 * @see Jaxer.Response.prototype.addHeader
	 * @see Jaxer.Response.prototype.exit
	 * @see Jaxer.Response.prototype.statusCodeToReasonPhrase
	 */
	this.redirect = function redirect(url, statusCode, reasonPhrase)
	{
		if (typeof url == "string")
		{
			this.addHeader("Location", url, true);
		}
		else
		{
			url = this.getHeader("Location");
			if (!url) 
			{
				throw new Exception("For a redirect, the Location header must have previously been set, or a string URL parameter must be specified");
			}
		}
		
		statusCode = Util.Math.forceInteger(statusCode, 307); // 307: Temporary Redirect

		log.trace("Redirecting to '" + url + "' with status code " + statusCode);
		this.exit(statusCode, '', reasonPhrase);
	}
	
	/**
	 * Returns the standard W3C reason phrase for the given standard status code.
	 * For example, passing in 200 returns "OK", and passing in 404 returns "Not Found".
	 * 
	 * @alias Jaxer.Response.prototype.statusCodeToReasonPhrase
	 * @param {Number} statusCode
	 * 		The status code, as an integer (or a string that can be parsed to an integer).
	 * 		If this is not a recognized status code, "Unknown Status Code" is returned.
	 */
	this.statusCodeToReasonPhrase = function statusCodeToReasonPhrase(statusCode)
	{
		statusCode = Util.Math.forceInteger(statusCode, -1);
		switch (statusCode)
		{
			case 100: return "Continue";
			case 101: return "Switching Protocols";
			case 200: return "OK";
			case 201: return "Created";
			case 202: return "Accepted";
			case 203: return "Non-Authoritative Information";
			case 204: return "No Content";
			case 205: return "Reset Content";
			case 206: return "Partial Content";
			case 300: return "Multiple Choices";
			case 301: return "Moved Permanently";
			case 302: return "Found";
			case 303: return "See Other";
			case 304: return "Not Modified";
			case 305: return "Use Proxy";
			case 307: return "Temporary Redirect";
			case 400: return "Bad Request";
			case 401: return "Unauthorized";
			case 402: return "Payment Required";
			case 403: return "Forbidden";
			case 404: return "Not Found";
			case 405: return "Method Not Allowed";
			case 406: return "Not Acceptable";
			case 407: return "Proxy Authentication Required";
			case 408: return "Request Time-out";
			case 409: return "Conflict";
			case 410: return "Gone";
			case 411: return "Length Required";
			case 412: return "Precondition Failed";
			case 413: return "Request Entity Too Large";
			case 414: return "Request-URI Too Large";
			case 415: return "Unsupported Media Type";
			case 416: return "Requested range not satisfiable";
			case 417: return "Expectation Failed";
			case 500: return "Internal Server Error";
			case 501: return "Not Implemented";
			case 502: return "Bad Gateway";
			case 503: return "Service Unavailable";
			case 504: return "Gateway Time-out";
			case 505: return "HTTP Version not supported";
			default: return "Unknown Status Code";
		}
	}
	
	/**
	 * Notifies the framework that an error has occurred during the handling of
	 * this request and generation of this response. How this is handled is then
	 * determined by the Jaxer.Config settings.
	 * 
	 * @alias Jaxer.Response.prototype.notifyError
	 * @param {Object} newError
	 * 		The error describing what happened. If the framework has already
	 * 		been notified of an error during the current request, this newError
	 * 		is not used.
	 * @param {Boolean} [avoidLogging]
	 * 		If this evaluates to true, an error message will not be logged
	 * 		during this call. Note that if an error message has been logged
	 * 		already in this request, another message will not be logged in any
	 * 		case.
	 */
	this.notifyError = function notifyError(newError, avoidLogging)
	{
		if (!error) 
		{
			error = newError || "Unspecified error";
		}
		if (!errorLogged && !avoidLogging)
		{
			errorLogged = true;
			log.error("Error processing this request: " + error);
		}
	}
	
	/**
	 * Has the framework been notified of an error during the handling of this
	 * request and generation of this response?
	 * 
	 * @alias Jaxer.Response.prototype.hasError
	 * @return {Boolean}
	 * 		True if an error was submitted.
	 */
	this.hasError = function hasError()
	{
		return error ? true : false;
	}
	
	/**
	 * Gets the error, if any, of which the framework has been notified during
	 * the handling of this request and generation of this response.
	 * 
	 * @alias Jaxer.Response.prototype.getError
	 */
	this.getError = function getError()
	{
		return error;
	}
	
	/**
	 * Notifies the framework that an error has already been reported in the output page
	 * so it need not be reported in the output page again.
	 * @alias Jaxer.Response.prototype.notifyErrorShown
	 */
	this.notifyErrorShown = function notifyErrorShown()
	{
		errorShown = true;
	}
	
	/**
	 * True if an error has already been reported in the output page
	 * so it need not be reported in the output page again.
	 * @alias Jaxer.Response.prototype.wasErrorShown
	 * @return {Boolean} True if notifyErrorShown() was called in this response, false otherwise.
	 */
	this.wasErrorShown = function wasErrorShown()
	{
		return errorShown;
	}

	/**
	 * Sets headers on this response to mark it as a dynamic one and avoid its
	 * being cached. This will always be called by the framework during callback
	 * processing. For regular (non-callback) requests, this will only be called
	 * by the framework if the DOM has been changed or a side-effect has (or
	 * could have) occurred during the processing of this request and the
	 * generation of the response.
	 * 
	 * @alias Jaxer.Response.prototype.setNoCacheHeaders
	 */
	this.setNoCacheHeaders = function setNoCacheHeaders()
	{
		log.trace("Setting no-cache headers");
		Jaxer.response.headers["Expires"] = "Fri, 23 May 1997 05:00:00 GMT";
		Jaxer.response.headers["Cache-Control"] = "no-cache, must-revalidate";
		Jaxer.response.addHeader("Cache-Control", "post-check=0, pre-check=0");
		Jaxer.response.headers["Pragma"] = "no-cache";
		delete Jaxer.response.headers["Last-Modified"];
		delete Jaxer.response.headers["ETag"];
	}
	
	/**
	 * If Jaxer.Config.EXPOSE_JAXER, this sets an "X-Powered-By" header on the
	 * response indicating that Jaxer processing has occurred and noting the
	 * build number.
	 * 
	 * @alias Jaxer.Response.prototype.exposeJaxer
	 */
	this.exposeJaxer = function exposeJaxer()
	{
		// Expose the Jaxer processing (in addition to any previous processors)
		if (Jaxer.Config.EXPOSE_JAXER) 
		{
			Jaxer.response.addHeader("X-Powered-By", "Jaxer/" + Jaxer.buildNumber + " (Aptana)");
		}
	}
	
	/**
	 * Overrides the automatic injection of the client framework into the
	 * response page. If neither src nor contents are given, then NO client
	 * framework is injected into the page. Make sure you do not rely
	 * (implicitly or explicitly) on the presence of the default Jaxer client
	 * framework if you call this method!
	 * 
	 * @alias Jaxer.Response.prototype.setClientFramework
	 * @param {String} [src]
	 * 		The url of the alternate client framework to use, if any. If this
	 * 		src argument is given, any contents argument will not be used.
	 * @param {String} [contents]
	 * 		The contents of the script block to use as an alternate client
	 * 		framework. If a src argument is specified, the contents argument
	 * 		will not be used.
	 */
	this.setClientFramework = function setClientFramework(src, contents)
	{
		clientFrameworkOverridden = true;
		if (src)
		{
			overriddenClientFrameworkIsSrc = true;
			overriddenClientFrameworkValue = src;
		}
		else
		{
			overriddenClientFrameworkIsSrc = false;
			overriddenClientFrameworkValue = contents; // May well be undefined
		}
	}
	
	/**
	 * Restores the use of the default client framework (using the Jaxer.Config
	 * settings), undoing any previous calls to setClientFramework().
	 * 
	 * @alias Jaxer.Response.prototype.resetClientFramework
	 */
	this.resetClientFramework = function resetClientFramework()
	{
		clientFrameworkOverridden = false;
		overriddenClientFrameworkIsSrc = false;
		overriddenClientFrameworkValue = undefined;
	}
	
	/**
	 * Checks whether the client framework has been overridden for this
	 * response, i.e. whether setClientFramework has been called.
	 * 
	 * @alias Jaxer.Response.prototype.isClientFrameworkOverridden
	 * @return {Boolean}
	 * 		true if the client framework is overridden, false otherwise
	 */
	this.isClientFrameworkOverridden = function isClientFrameworkOverridden()
	{
		return clientFrameworkOverridden;
	}
	
	/**
	 * Gets the parameters of the alternate client framework that will be
	 * injected into this response, if the default client framework has been
	 * overridden using setClientFramework().
	 * 
	 * @alias Jaxer.Response.prototype.getOverriddenClientFramework
	 * @return {Object}
	 * 		A JavaScript object with properties isSrc (Boolean indicating
	 * 		whether the value is the src or the contents of the script block to
	 * 		be injected) and value (the src or the contents).
	 */
	this.getOverriddenClientFramework = function getOverriddenClientFramework()
	{
		return clientFrameworkOverridden ? 
			{ isSrc: overriddenClientFrameworkIsSrc, value: overriddenClientFrameworkValue } : 
			null;
	}
	
}

frameworkGlobal.Response = Jaxer.Response = Response;

Jaxer.Log.trace("*** Response.js loaded");

})();
