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

var log = Log.forModule("Request");

/**
 * @classDescription {Jaxer.Request} An instance of this object has 
 * the lifecycle of the current request and contains information about it.
 */

/**
 * An instance of this object has the lifecycle of the current request and
 * contains information about it.
 * 
 * This constructor is typically never invoked by the user. During the processing
 * of the page Jaxer will create an instance of this object named Jaxer.request, 
 * this instance should be used to introspect the request details.
 * 
 * @constructor
 * @alias Jaxer.Request
 * @param {Object} evt
 * 		The core event whose data is used to initialize this Request object
 * 		instance
 * @return {Jaxer.Request}
 * 		Returns an instance of Request.
 */
var Request = function Request(evt)
{
	
	this.constructorErrors = [];

	var _request = evt.Request; 
	this._request = _request; // For internal (Jaxer framework) use only -- APIs may change
	
	for (var p in _request)
	{
		if ((typeof _request[p] == "string") || 
			(typeof _request[p] == "boolean"))
		{
			this[p] = _request[p];
		}
	}
	
	/**
	 * The location on disk of the top folder from which all web pages are served
	 * by the web server, as an absolute URL (without the preceding file://).
	 * This is usually only meaningful if the web server is on the same
	 * filesystem as Jaxer.
	 * @alias Jaxer.Request.prototype.documentRoot
	 * @property {String}
	 */
	
	/**
	 * The location on disk of the current page's file, served
	 * by the web server, as an absolute URL (without the preceding file://).
	 * This is usually only meaningful if the web server is on the same
	 * filesystem as Jaxer.
	 * @alias Jaxer.Request.prototype.pageFile
	 * @property {String}
	 */
	
	/**
	 * The type of HTTP request this is: usually "GET" or "POST".
	 * Guaranteed to be all upper-case, and the same as
	 * the action property.
	 * @alias Jaxer.Request.prototype.method
	 * @property {String}
	 * @alias Jaxer.Request.prototype.action
	 * @see Jaxer.Request.ACTIONS
	 */
	/**
	 * The type of HTTP request this is: usually "GET" or "POST".
	 * Guaranteed to be all upper-case, and the same as
	 * the method property.
	 * @alias Jaxer.Request.prototype.action
	 * @property {String}
	 * @see Jaxer.Request.ACTIONS
	 */
	if (typeof this.method == "string")
	{
		this.method = this.method.toUpperCase();
		this.action = this.method;
	}
	
	/**
	 * Whether the current page is being requested and served over 
	 * the HTTPS protocol.
	 * @alias Jaxer.Request.prototype.isHTTPS
	 * @property {Boolean}
	 */
	
	/**
	 * True if Jaxer is the main "handler" for the current request,
	 * rather than is filtering a page served by a different handler.
	 * @alias Jaxer.Request.prototype.isHandler
	 * @property {Boolean}
	 */
	
	/**
	 * The protocol declared in the HTTP request, e.g. "HTTP/1.1".
	 * @alias Jaxer.Request.prototype.protocol
	 * @property {String}
	 */
	
	/**
	 * The query part of the current request's URL, after the "?".
	 * This is also available as Jaxer.request.parsedUrl.query
	 * and is parsed into the JavaScript object Jaxer.request.parsedUrl.queryParts.
	 * @alias Jaxer.Request.prototype.queryString
	 * @property {String}
	 */
	
	/**
	 * The value of the Referer HTTP header for this request, which should indicate
	 * the complete URL of the page that made this request. If this is a callback,
	 * the referer is taken from the "callingPage" parameter of the request, only
	 * using the Referer header if for some reason "callingPage" is not available.
	 * @alias Jaxer.Request.prototype.referer
	 * @property {String}
	 */
	
	/**
	 * The Internet Protocol (IP) address of the client (browser) that sent the request.
	 * @alias Jaxer.Request.prototype.remoteAddr
	 * @property {String}
	 */
	
	/**
	 * The hostname of the client (browser) that sent the request,
	 * or the IP address of the client if the hostname cannot be determined.
	 * @alias Jaxer.Request.prototype.remoteHost
	 * @property {String}
	 */
	
	/**
	 * If the browser making the request submitted HTTP authentication credentials,
	 * this is the username submitted. Otherwise it is the empty string.
	 * @alias Jaxer.Request.prototype.remoteUser
	 * @property {String}
	 */
	
	/**
	 * The URL (and URI) of the current request.
	 * @alias Jaxer.Request.prototype.uri
	 * @property {String}
	 */
	
	/**
	 * A collection of the HTTP headers received from the web server for this request,
	 * as properties on this simple JavaScript object. 
	 * If multiple headers had the same name, their values are given as an array.
	 * 
	 * @alias Jaxer.Request.prototype.headers
	 * @property {Object}
	 */
	this.headers = {}; // This will be a MultiHash

	try
	{
		var headerCount = _request.GetHeaderCount();
		for (var i=0; i<headerCount; i++)
		{
			var name = _request.GetHeaderName(i);
			var value = _request.GetValueByOrd(i);
			Jaxer.Util.MultiHash.add(this.headers, name, value);
		}
	}
	catch (e)
	{
		this.constructorErrors.push("Error reading header properties: " + e);
	}
	
	if (log.getLevel() == Log.TRACE) log.trace("Constructing request from: " + uneval(this));
	
	/**
	 * A collection of the environment variables received from the web server for this request,
	 * as properties on this simple JavaScript object.
	 * If multiple variables had the same name, their values are given as an array.
	 * 
	 * @alias Jaxer.Request.prototype.env
	 * @property {Object}
	 */
	this.env = {}; // This will be a MultiHash

	try
	{
		var envCount = _request.GetEnvCount();
		for (var i = 0; i<envCount; i++)
		{
			var name = _request.GetEnvName(i);
			var value = _request.GetEnvValueByOrd(i);
			Jaxer.Util.MultiHash.add(this.env, name, value);
		}
	}
	catch (e)
	{
		this.constructorErrors.push("Error reading env variables: " + e);
	}
	
	/**
	 * Holds the parsed URL information of the current page, which on a callback
	 * is different from the original page.
	 * 
	 * @alias Jaxer.Request.prototype.current
	 * @property {Jaxer.Util.Url.ParsedUrl}
	 * @see Jaxer.Request.parsedUrl
	 */
	if ((typeof this.headers.Host == "string") && (typeof this.uri == "string"))
	{
		var uriIsAbsolutePath = (this.uri.charAt(0) == "/");
		if (uriIsAbsolutePath)
		{
			try
			{
				this.current = Util.Url.parseUrlComponents(this.headers.Host, this.uri, _request.isHTTPS ? 'https' : 'http');
			}
			catch (e)
			{
				this.constructorErrors.push("Error parsing the URL components, Host='" + this.headers.Host + "' and uri='" + this.uri + "': " + e);
			}
		}
		else
		{
			try
			{
				this.current = Util.Url.parseUrl(this.uri);
			}
			catch (e)
			{
				this.constructorErrors.push("Error parsing the URL, uri='" + this.uri + "': " + e);
			}
		}
	}
	if (!this.current) this.current = null;

	/**
	 * The folder (directory) on disk holding the file (pageFile) being served
	 * in this request. This is '' if there is no pageFile information, e.g. if
	 * the web server is on a different filesystem than Jaxer.
	 * 
	 * @alias Jaxer.Request.prototype.currentFolder
	 * @property {String}
	 */
	try
	{
		this.currentFolder = this.pageFile.replace(/[\/\\][^\/\\]*$/, '');
	}
	catch (e)
	{
		this.currentFolder = '';
		this.constructorErrors.push("Error constructing currentFolder property from pageFile=" + this.pageFile);
	}
	
	/**
	 * The body of this request (usually the payload of a POST request) as a string,
	 * which may be empty. For example, a GET request always has an empty body. 
	 * NOTE that when the contentType is 'multipart/form-data' this will
	 * always be '', indicating that the body of the request has been
	 * pre-processed by Jaxer and is instead available via the files and data
	 * properties.
	 * 
	 * @alias Jaxer.Request.prototype.bodyAsString
	 * @property {String}
	 */
	var _bodyAsString;
	this.__defineGetter__("bodyAsString", function() 
	{
		if (_bodyAsString == undefined)
		{
			if (_request.GetRawPostDataLength() > 0) 
			{
				_bodyAsString = _request.GetRawPostDataAsString();
			}
			else
			{
				_bodyAsString = '';
			}
		}
		return _bodyAsString; 
	});
	
	/**
	 * The Content-Type (i.e. mime-type) of the body of this request, which describes
	 * the body of this request. Usually this will be 'application/x-www-form-urlencoded'
	 * or 'multipart/form-data'.
	 * 
	 * @alias Jaxer.Request.prototype.contentType
	 * @property {String}
	 */
	this.__defineGetter__("contentType", function()
	{
		return this._request.GetRawPostDataContentType();
	});
	
	/** 
	 * The string used to identify the user agent of the client
	 * making the request.
	 * 
	 * @alias Jaxer.Request.prototype.userAgent
	 * @property {String}
	 */
	this.__defineGetter__("userAgent", function()
	{
		return this.headers['User-Agent'] || 'Unknown';
	});
	
	/**
	 * The Content-Length in bytes of the body of this request.
	 * NOTE that when the contentType is 'multipart/form-data' this will
	 * always return 0, indicating that the body of the request has been
	 * pre-processed by Jaxer and is instead available via the files and data
	 * properties.
	 * 
	 * @alias Jaxer.Request.prototype.contentLength
	 * @property {Number}
	 */
	this.__defineGetter__("contentLength", function()
	{
		return this._request.GetRawPostDataLength();
	});
	
	var _body;
	/**
	 * The body of the HTTP request, which usually contains the data during
	 * a POST request, and is often of type 'application/x-www-form-urlencoded'
	 * (i.e. "name1=value1&name2=value2&..."). The type of data returned by this 
	 * property depends on the Content-Type of the request and on Config.REQUEST_BODY_AS.
	 * @alias Jaxer.Request.prototype.body
	 * @property {Object}
	 */
	this.__defineGetter__("body", function() 
	{
		if (_body == undefined) 
		{
			_body = null;
			if (Config.REQUEST_BODY_AS && (typeof Config.REQUEST_BODY_AS == "object")) 
			{
				var contentType = this.contentType.toLowerCase();
				var values = Config.REQUEST_BODY_AS;
				for (var mimeTypePrefix in values) 
				{
					var value = values[mimeTypePrefix];
					if (typeof value != 'string') continue;
					if (contentType.indexOf(mimeTypePrefix.toLowerCase()) == 0) 
					{
						var matched;
						switch (value.toLowerCase())
						{
							case 'string':
								_body = this.bodyAsString;
								matched = true;
								break;
							case 'bytearray':
								_body = this.bodyAsByteArray;
								matched = true;
								break;
							case 'stream':
								_body = this.bodyAsStream;
								matched = true;
								break;
						}
						if (matched) break;
					}
				}
			}
		}
		return _body; 
	});
	
	/**
	 * The body of this request (usually the payload of a POST request) as a byte array,
	 * which may be empty. For example, a GET request always has an empty body. 
	 * 
	 * @alias Jaxer.Request.prototype.bodyAsByteArray
	 * @property {Number[]}
	 */
	var _bodyAsByteArray;
	this.__defineGetter__("bodyAsByteArray", function() 
	{
		if (_bodyAsByteArray == undefined) 
		{
			if (_request.GetRawPostDataLength() > 0) 
			{
				var baval = {};
				_request.GetRawPostDataAsByteArray({}, baval);
				_bodyAsByteArray = baval.value;
			}
			else
			{
				_bodyAsByteArray = [];
			}
		}
		return _bodyAsByteArray; 
	});
	
	/**
	 * The body of this request (usually the payload of a POST request) as a stream,
	 * which may be empty. For example, a GET request always has an empty body. 
	 * 
	 * @alias Jaxer.Request.prototype.bodyAsStream
	 * @property {Object}
	 */
	var _bodyAsStream;
	this.__defineGetter__("bodyAsStream", function() 
	{
		if (_bodyAsString == undefined)
		{
			if (_request.GetRawPostDataLength() > 0) 
			{
				_bodyAsStream = _request.GetRawPostDataAsInputStream();
			}
			else
			{
				_bodyAsStream = '';
			}
		}
		return _bodyAsStream; 
	});
	
	var data = {};
	var files = [];
	var paramIsBody = false;
	
	// First fill data from query string, which may also change how we will interpret the body (if any)
	if (this.current)
	{
		for (var p in this.current.queryParts)
		{
			data[p] = this.current.queryParts[p];
		}
		if (data.hasOwnProperty(Callback.PARAMS_AS) && data[Callback.PARAMS_AS] == Callback.PARAMS_AS_TEXT)
		{
			paramIsBody = true;
		}
	}
	
	// Now fill data from the request's body
	if (paramIsBody)
	{
		data[Callback.PARAMETERS] = [this.body];
	}
	else
	{
		try
		{
			for (var i = 0, dataItemCount = _request.GetDataItemCount(); i < dataItemCount; i++) 
			{
				var name = _request.GetDataItemName(i);
				var value = _request.GetDataItemValue(i);
				
				data[name] = value;
			}
		}
		catch (e)
		{
			this.constructorErrors.push("Error reading data items from Core: " + e);
		}
		try
		{
			for (var i=0, fileCount = _request.GetFileCount(); i<fileCount; i++) 
			{
				files.push(new FileInfo(_request, i));
			}
		}
		catch (e)
		{
			this.constructorErrors.push("Error reading file upload items from Core: " + e);
		}
	}
	
	/**
	 * An object holding the name=value pairs of the current request's body
	 * (assumed to be of type application/x-www-form-urlencoded) as properties
	 * 
	 * @alias Jaxer.Request.prototype.data
	 * @property {Object}
	 */
	this.data = data;

	/**
	 * An array of Jaxer.Request.FileInfo objects describing any uploaded files.
	 * You must call save(newFileName) on each of these if you'd like to save
	 * them, otherwise they will be purged at the end of the request.
	 * 
	 * @alias Jaxer.Request.prototype.files
	 * @property {Array}
	 * @see Jaxer.FileInfo.prototype.save
	 */
	this.files = files;

	/**
	 * True if the entire body of the request is to be considered as the
	 * single data parameter of this request.
	 * 
	 * @alias Jaxer.Request.prototype.paramIsBody
	 * @property {Boolean}
	 */
	this.paramIsBody = paramIsBody;
	
	/**
	 * The Jaxer.App object constructed by searching in configApps.js for an object that matches 
	 * the current request's parsedUrl and using it to set application-specific settings 
	 * (such as the database connection to use) during this request
	 * 
	 * @alias Jaxer.Request.prototype.app
	 * @property {Jaxer.App}
	 */
	this.app = null;

	/**
	 * A string used to identify what application the current request is asking for
	 * 
	 * @advanced
	 * @alias Jaxer.Request.prototype.appKey
	 * @property {String}
	 */
	this.appKey = null;

	/**
	 * A string used to identify what page in the application the current request is asking for
	 * 
	 * @advanced
	 * @alias Jaxer.Request.prototype.pageKey
	 * @property {String}
	 */
	this.pageKey = null;

	/**
	 * True if Jaxer is handling the current request as a callback (RPC).
	 * This requires Jaxer to be the handler of the request and the
	 * path of the request to start with Config.CALLBACK_URI (which must
	 * not be blank)
	 * @alias Jaxer.Request.prototype.isCallback
	 * @property {Boolean}
	 */
	this.isCallback = this.isHandler && 
		(this.current && 
		 Config.CALLBACK_URI &&
		 (this.current.path.indexOf(Config.CALLBACK_URI) == 0)
		);

	/**
	 * The "Referer", if any, of the request: the URL of the page that submitted
	 * the request
	 * 
	 * @alias Jaxer.Request.prototype.referer
	 * @property {String}
	 */
	var effectiveReferer = data[Callback.CALLING_PAGE] || this.headers.Referer;
	if (typeof effectiveReferer == "string")
	{
		try
		{
			this.referer = Util.Url.parseUrl(effectiveReferer);
		}
		catch (e)
		{
			this.constructorErrors.push("Error parsing the effective Referer URL '" + effectiveReferer + "': " + e);
		}
	}
	if (!this.referer) this.referer = null;
	
	/**
	 * Holds the parsed URL information of the true page: this is the current
	 * URL for a regularly-served page, but for a callback this is the URL of
	 * the original page (now the Referer) that requested the callback.
	 * 
	 * @alias Jaxer.Request.prototype.parsedUrl
	 * @property {Jaxer.Util.Url.ParsedUrl}
	 */
	if (this.isCallback) // callback page flow - we should have a referer
	{
		this.parsedUrl = this.referer;
		if (this.parsedUrl)
		{
			this.parsedUrlError = "";
		}
		else
		{
			var headerNames = [];
			
			for (var headerName in this.headers)
			{
				headerNames.push(headerName + "=" + this.headers[headerName]);
			}
			
			headerNames = headerNames.sort();

			this.parsedUrlError = "This is a callback but there was no '" + Callback.CALLING_PAGE + "' value in the callback request nor " +
				"a Referer in evt.Request.headers:\n" + headerNames.join("; ");
		}
	}
	else // normal (page) or service flow - we should have this.current
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
	
	/**
	 * Low-level method to evaluate a string of JavaScript source code in a
	 * given global context and with a certain effectiveUrl as its "file".
	 * 
	 * @alias Jaxer.Request.prototype.evaluateScript
	 * @param {String} contents
	 * 		The string of script code to evaluate
	 * @param {Object} [global]
	 * 		An optional global context (usually a window object) in which to
	 * 		evaluate it
	 * @param {String} [effectiveUrl]
	 * 		An optional parameter to indicate (e.g. in error messages) the
	 * 		effective URL from which this code originates.
	 * @return {Object}
	 * 		The result of the evaluation, if any
	 * @see Jaxer.Includer.evalOn
	 */
	this.evaluateScript = function evaluateScript(contents, global, effectiveUrl)
	{
		if (typeof contents != "string")
		{
			try
			{
				contents = String(contents);
			}
			catch (e)
			{
				throw new Exception("Non-string contents passed to Jaxer.Request.evaluateScript()");
			}
		}
		if (effectiveUrl == undefined)
		{
			effectiveUrl = "javascript:" + contents.substr(0, 60).replace(/\n/g, '\\n') + (contents.length > 60 ? '...' : ''); 
		}
		return _request.ExecuteJavascript(contents, effectiveUrl, global);
	}
	
	/**
	 * Low-level method to compile a string of JavaScript source code in a
	 * given global context and with a certain effectiveUrl as its "file".
	 * 
	 * @alias Jaxer.Request.prototype.compileScript
	 * @param {String} contents
	 * 		The string of script code to evaluate
	 * @param {Object} [global]
	 * 		An optional global context (usually a window object) in which to
	 * 		evaluate it
	 * @param {String} [effectiveUrl]
	 * 		An optional parameter to indicate (e.g. in error messages) the
	 * 		effective URL from which this code originates.
	 * @return {String}
	 * 		The compiled code
	 * @see Jaxer.Includer.compile
	 */
	this.compileScript = function compileScript(contents, global, effectiveUrl)
	{
		if (typeof contents != "string")
		{
			try
			{
				contents = String(contents);
			}
			catch (e)
			{
				throw new Exception("Non-string contents passed to Jaxer.Request.compileScript()");
			}
		}
		if (effectiveUrl == undefined)
		{
			effectiveUrl = "javascript:" + contents.substr(0, 60).replace(/\n/g, '\\n') + (contents.length > 60 ? '...' : ''); 
		}
		return _request.CompileScript(contents, effectiveUrl, global);
	}
	
	/**
	 * Low-level method to evaluate a string of compiled JavaScript code in a
	 * given global context.
	 * 
	 * @alias Jaxer.Request.prototype.evaluateCompiledScript
	 * @param {String} compiledContents
	 * 		The bytecode string of script code to evaluate
	 * @param {Object} [global]
	 * 		An optional global context (usually a window object) in which to
	 * 		evaluate it
	 * @return {Object}
	 * 		The result of the evaluation, if any
	 * @see Jaxer.Includer.evalCompiledOn
	 */
	this.evaluateCompiledScript = function evaluateCompiledScript(compiledContents, global)
	{
		if (typeof compiledContents != "string")
		{
			try
			{
				compiledContents = String(compiledContents);
			}
			catch (e)
			{
				throw new Exception("Non-string contents passed to Jaxer.Request.evaluateCompiledScript()", log);
			}
		}
		return _request.RunScript(compiledContents, global);
	}
	
}

/**
 * Create a string representation of all request header key/value pairs
 * 
 * @private
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
 * @classDescription {Jaxer.Request.FileInfo} Container for information about
 * uploaded files.
 */

/**
 * A container for information about uploaded files. The constructor is not
 * meant to be called directly.
 * 
 * @advanced
 * @constructor
 * @alias Jaxer.Request.FileInfo
 * @param {Object} req
 * 		The Jaxer Core's request object
 * @param {Number} i
 * 		An index into the list of uploaded files
 * @return {Jaxer.Request.FileInfo}
 * 		Returns an instance of FileInfo.
 */
var FileInfo = function FileInfo(req, i)
{
	this.elementName = req.GetFileName(i);
	this.fileName = req.GetOriginalFileName(i);
	this.originalFileName = req.GetOriginalFileName(i);
	this.tempFileName = req.GetTmpFileName(i);
	this.contentType = req.GetFileContentType(i);
	this.fileSize = req.GetFileSize(i);
	this.index = i;
}

/**
 * The name of the field as specified in the HTML form
 * 
 * @alias Jaxer.Request.FileInfo.prototype.elementName
 * @property {String}
 */

/**
 * The original name of the file that was actually uploaded
 * from the user's filesystem
 * 
 * @alias Jaxer.Request.FileInfo.prototype.fileName
 * @property {String}
 */

/**
 * The original name of the file that was actually uploaded
 * from the user's filesystem
 * 
 * @alias Jaxer.Request.FileInfo.prototype.originalFileName
 * @property {String}
 */

/**
 * The name under which the file was temporarily saved
 * 
 * @alias Jaxer.Request.FileInfo.prototype.tempFileName
 * @advanced
 * @property {String}
 */

/**
 * The content type of this file, as reported by the web server
 * 
 * @alias Jaxer.Request.FileInfo.prototype.contentType
 * @property {String}
 */

/**
 * The size of the file contents, in bytes
 * 
 * @alias Jaxer.Request.FileInfo.prototype.fileSize
 * @property {Number}
 */

/**
 * An index into the list of uploaded files
 * 
 * @alias Jaxer.Request.FileInfo.prototype.index
 * @property {Number}
 */

/**
 * Save the uploaded file to the given path -- otherwise it will be
 * automatically purged after this request
 * 
 * @alias Jaxer.Request.FileInfo.prototype.save
 * @param {String} newFileName
 * 		The name (or full path) of the file to which the uploaded file should be
 * 		saved.
 */
FileInfo.prototype.save = function save(newFileName)
{
	var file = new Jaxer.File(this.tempFileName);
	var newFile = new Jaxer.File(newFileName);
	if (newFile.exists)
	{
		newFile.remove();
	}
	file.copy(newFileName);
}

/**
 * An enumeration of constants for various standard HTTP methods or actions 
 * 
 * @alias Jaxer.Request.ACTIONS
 * @property {Object}
 */
Request.ACTIONS = Web.ACTIONS;

/**
 * A constant describing the standard HTTP GET method (or action) 
 * 
 * @alias Jaxer.Request.ACTIONS.GET
 * @property {String}
 */
/**
 * A constant describing the standard HTTP POST method (or action) 
 * 
 * @alias Jaxer.Request.ACTIONS.POST
 * @property {String}
 */
/**
 * A constant describing the standard HTTP PUT method (or action) 
 * 
 * @alias Jaxer.Request.ACTIONS.PUT
 * @property {String}
 */
/**
 * A constant describing the standard HTTP DELETE method (or action) 
 * 
 * @alias Jaxer.Request.ACTIONS.DELETE
 * @property {String}
 */
/**
 * A constant describing the standard HTTP HEAD method (or action) 
 * 
 * @alias Jaxer.Request.ACTIONS.HEAD
 * @property {String}
 */
/**
 * A constant describing the standard HTTP OPTIONS method (or action) 
 * 
 * @alias Jaxer.Request.ACTIONS.OPTIONS
 * @property {String}
 */

frameworkGlobal.Request = Jaxer.Request = Request;

Jaxer.Log.trace("*** Request.js loaded");

})();

