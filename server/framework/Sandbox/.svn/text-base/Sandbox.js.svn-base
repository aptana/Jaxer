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

var log = Log.forModule("Sandbox");

/**
 * @classDescription {Jaxer.Sandbox} The Class describing a Sandbox: 
 * a container that can load a server-side window with its own DOM, 
 * and executes JavaScript server-side, but is more protected than the regular Jaxer 
 * server-side window.
 */

// private variables
const networkUtils = new Jaxer.NetworkUtils();
const jxWindow = Components.Constructor('@aptana.com/utils/jxWindow;1', "jxIWindow");
var openSandboxes = []; // used to close() all created ones at the end of a request

/**
 * The constructor of a Sandbox: 
 * a container that can load a server-side window with its own DOM, 
 * and executes JavaScript server-side, but is more protected than the regular Jaxer 
 * server-side window.
 * 
 * @alias Jaxer.Sandbox
 * @constructor
 * @param {String} [url]
 * 		The url to load. If no url is given, nothing is loaded -- you can later use 
 * 		the load method to load it.
 * @param {String, Object} [data]
 * 		To GET a page from the url, leave this parameter undefined or pass in null.
 * 		To POST to the url, set this parameter to the data to be POSTed.
 * 		It may be a string (usually of the form name1=value&name2=value), 
 * 		or an object whose name->value property pairs will be used to construct such a string.
 * @param {Object, Jaxer.Sandbox.OpenOptions} [options]
 * 		A JavaScript object (hashmap) of name: value property pairs specifying
 * 		how to load this Sandbox.
 * @return {Jaxer.Sandbox}
 * 		Returns an instance of Jaxer.Sandbox
 * @see Jaxer.Sandbox.OpenOptions
 */
function Sandbox(url, data, options)
{
	this._jxWindow = new jxWindow();
	
	url = this.url = normalizeUrl(url, true);
	data = this.data = normalizeData(data);
	options = normalizeAndApplyOptions(this, options);
	
	if (this.url)
	{
		this.open(this.url, data, options);
	}
}

/**
 * Returns the window that's been loaded into this sandbox,
 * or null if a url has not been loaded yet.
 * 
 * @alias Jaxer.Sandbox.prototype.window
 * @property {ChromeWindow}
 */
Sandbox.prototype.__defineGetter__('window', function getWindow()
{
	return this._jxWindow.window;
});

/**
 * Returns the DOM document that's been loaded into this sandbox's window,
 * or null if a url has not been loaded yet.
 * 
 * @alias Jaxer.Sandbox.prototype.document
 * @property {HTMLDocument}
 */
Sandbox.prototype.__defineGetter__('document', function getDocument()
{
	return this._jxWindow.document;
});

/**
 * Opens the sandbox's window and document by creating them (if needed)
 * and then loading a url into them, via a GET or POST request. You usually do not
 * need to explicitly close() this, except to cancel async requests.
 * 
 * @alias Jaxer.Sandbox.prototype.open
 * @param {String} url
 * 		The url to load from
 * @param {String, Object} [data]
 * 		To GET a page from the url, leave this parameter undefined or pass in null.
 * 		To POST to the url, set this parameter to the data to be POSTed.
 * 		It may be a string (usually of the form name1=value&name2=value), 
 * 		or an object whose name->value property pairs will be used to construct such a string.
 * @param {Object, Jaxer.Sandbox.OpenOptions} [options]
 * 		A JavaScript object (hashmap) of name: value property pairs specifying
 * 		how to load this Sandbox.
 * @see Jaxer.Sandbox.prototype.close
 */
Sandbox.prototype.open = function open(url, data, options)
{
	url = this.url = normalizeUrl(url, false);
	data = this.data = normalizeData(data);
	options = normalizeAndApplyOptions(this, options);
	
	log.trace("Opening url '" + this.url + "' with data " + (data == null ? 'null' : ' not null') + " and options " + uneval(this.options));
	
	var referer = (typeof this.options.referer == "string") ? networkUtils.fixupURI(this.options.referer) : null;
	
	var getStream, postStream;
	if (data == null) // GET
	{
		getStream = this.options.headers ? createInputStream(this.options.headers) : null;
		postStream = null;
	}
	else // POST
	{
		getStream = null;
		postStream = createInputStream(this.options.headers, data);
	}

	this._jxWindow.loadURI(url, referer, postStream, getStream);
	
	var index = openSandboxes.indexOf(this);
	if (index < 0)
	{
		openSandboxes.push(this);
	}
}

/**
 * Loads the sandbox's window and document from a string by creating them (if needed)
 * and then loading the string into them. You do not need to explicitly close() this.
 * 
 * @alias Jaxer.Sandbox.prototype.setContents
 * @param {String, Node, Document, Object} [contents]
 * 		The contents to load into the window, usually an HTML string. If a DOM
 * 		node or document are used, they are converted to HTML, and if an object
 * 		is passed in its toString() is used.
 * @param {Object, Jaxer.Sandbox.OpenOptions} [options]
 * 		A JavaScript object (hashmap) of name: value property pairs specifying
 * 		how to load this Sandbox.
 * @see Jaxer.Sandbox.prototype.close
 */
Sandbox.prototype.setContents = function setContents(contents, options)
{
	var htmlDocType = contents ? Util.findInGlobalContext("HTMLDocument", contents) : null;
	var htmlEltType = contents ? Util.findInGlobalContext("HTMLElement", contents) : null;

	if (contents == null || contents == undefined)
	{
		contents = '';
	}
	else if ((htmlDocType && (contents instanceof htmlDocType)) ||
			 (htmlEltType && (contents instanceof htmlEltType)) )
	{
		contents = Util.DOM.toHTML(contents);
	}
	else
	{
		contents = String(contents);
	}
	
	url = this.url = normalizeUrl(null, true);
	data = this.data = normalizeData(null);
	options = normalizeAndApplyOptions(this, options);
	
	log.trace("Loading contents '" + contents.substr(0, 50) + "'... with options " + uneval(this.options));
	
	var inputStream = Components.classes["@mozilla.org/io/string-input-stream;1"].createInstance(Components.interfaces.nsIStringInputStream);
	inputStream.data = contents;

	this._jxWindow.loadStream(inputStream, null, null, null);
	
	var index = openSandboxes.indexOf(this);
	if (index < 0)
	{
		openSandboxes.push(this);
	}
}

/**
 * Closes the Sandbox's window, which terminates any async loading and 
 * removes the window and its document. This call is usually optional, since
 * the window and its document are automatically removed at the end of
 * the Jaxer request, and they are safely reused if you open (load) the same
 * Sandbox more than once. It's therefore mostly useful for stopping
 * open async requests.
 * 
 * @alias Jaxer.Sandbox.prototype.close
 */
Sandbox.prototype.close = function close()
{
	var index = openSandboxes.indexOf(this);
	_close(this);
	if (index > -1)
	{
		openSandboxes.splice(index, 1);
	}
}

// This internal implementation does not modify the openSandboxes array
function _close(sb)
{
	sb._jxWindow.close();
}

/**
 * Waits for the Sandbox's window to complete loading, allowing asynchronous
 * processing of events to continue, up to a maximum number of milliseconds
 * (if specified).
 * 
 * @alias Jaxer.Sandbox.prototype.waitForCompletion
 * @param {Number} [maxMillis]
 * 		The number of milliseconds to wait for the sandbox to complete loading
 * 		before timing out and exiting. If this is zero or negative,
 * 		waits indefinitely (though other timeouts may intervene).
 * @return {Boolean}
 * 		Returns true if the Sandbox window loaded successfully (or was never initialized),
 * 		false if the timeout occurred before that.
 */
Sandbox.prototype.waitForCompletion = function waitForCompletion(maxMillis)
{
	var that = this;
	var testLoaded = function testLoaded()
	{
		return (that.readyState == that._jxWindow.UNINITIALIZED) || (that.readyState == that._jxWindow.COMPLETED);
	}
	var didComplete = Thread.waitFor(testLoaded, maxMillis);
	return didComplete;
}

/**
 * Returns the HTTP status of the response to the request to fetch this Sandbox's URL,
 * e.g. 200 (i.e. OK)
 * 
 * @alias Jaxer.Sandbox.prototype.status
 * @property {Number}
 */
Sandbox.prototype.__defineGetter__('status', function getStatus()
{
	var docChannel = this._jxWindow.documentChannel;
	return docChannel ? docChannel.responseStatus : null;
});

/**
 * Returns the HTTP status text of the response to the request to fetch this Sandbox's URL,
 * e.g. 'OK' (for a status of 200)
 * 
 * @alias Jaxer.Sandbox.prototype.statusText
 * @property {String}
 */
Sandbox.prototype.__defineGetter__('statusText', function getStatusText()
{
	var docChannel = this._jxWindow.documentChannel;
	return docChannel ? docChannel.responseStatusText : null;
});

/**
 * Returns the loading state of the window in the Sandbox. The possible values
 * are:
 * <ul>
 * 		<li>0 UNINITIALIZED -- the contents have not been set or the url has not been loaded</li>
 * 		<li>1 LOADING -- the contents are being set or the url is being loaded</li>
 * 		<li>2 LOADED -- the contents have been set or the url has been loaded</li>
 * 		<li>3 INTERACTIVE -- all network operations have completed</li>
 * 		<li>4 COMPLETED -- all operations have completed</li>
 * </ul>
 * 
 * @alias Jaxer.Sandbox.prototype.readyState
 * @property {String}
 */
Sandbox.prototype.__defineGetter__('readyState', function getReadyState()
{
	return this._jxWindow.readyState;
});

/**
 * Returns the HTTP method (action) of the request to fetch this Sandbox's URL,
 * e.g. 'GET' ir 'POST'
 * 
 * @alias Jaxer.Sandbox.prototype.method
 * @property {String}
 */
Sandbox.prototype.__defineGetter__('method', function getMethod()
{
	var docChannel = this._jxWindow.documentChannel;
	return docChannel ? docChannel.requestMethod : null;
});

/**
 * Returns the HTTP headers of the request to fetch this Sandbox's URL
 * as properties on the returned object
 * 
 * @alias Jaxer.Sandbox.prototype.requestHeaders
 * @property {Object}
 */
Sandbox.prototype.__defineGetter__('requestHeaders', function getRequestHeaders()
{
	var docChannel = this._jxWindow.documentChannel;
	var headers;
	if (docChannel) 
	{
		headers = {};
		var headerVisitor = 
		{
			visitHeader: function visitHeader(name, value)
			{
				headers[name] = value;
			}
		};
		docChannel.visitRequestHeaders(headerVisitor);
	}
	else
	{
		headers = null;
	}
	return headers;
});

/**
 * Returns the HTTP headers of the response to the request to fetch this Sandbox's URL
 * as properties on the returned object
 * 
 * @alias Jaxer.Sandbox.prototype.responseHeaders
 */
Sandbox.prototype.__defineGetter__('responseHeaders', function getResponseHeaders()
{
	var docChannel = this._jxWindow.documentChannel;
	var headers;
	if (docChannel) 
	{
		headers = {};
		var headerVisitor = 
		{
			visitHeader: function visitHeader(name, value)
			{
				headers[name] = value;
			}
		};
		docChannel.visitResponseHeaders(headerVisitor);
	}
	else
	{
		headers = null;
	}
	return headers;
});

/**
 * Calls close() on all Sandbox windows, and removes them from the openSandboxes array
 * 
 * @private
 * @alias Jaxer.Sandbox.closeAll
 */
Sandbox.closeAll = function closeAll()
{
	openSandboxes.forEach(function closeOne(sb, index)
	{
		log.trace("For Sandbox #" + index + ", sb = " + sb + " and sb._jxWindow = " + sb._jxWindow);
		if (sb && sb._jxWindow) 
		{
			_close(sb);
			log.trace("Auto-closed Sandbox #" + index);
		}
	});
	openSandboxes = [];
}

/**
 * Returns the HTML of the given node in the Sandbox's DOM, 
 * or of the entire Sandbox's document DOM.
 * If the Sandbox has not yet been loaded from a url, 
 * returns an empty string ('').
 * 
 * @alias Jaxer.Sandbox.prototype.toHTML
 * @param {Node, Document} [node]
 * 		The HTML element or document (or DOM node) to be serialized.
 * 		By default the entire Sandbox document is used.
 * @return {String}
 * 		The complete HTML representation of the node or document
 * 		and its contents, or '' if the Sandbox has no document
 */
Sandbox.prototype.toHTML = function toHTML(node)
{
	if (this.document) 
	{
		return Util.DOM.toHTML(node || this.document);
	}
	else
	{
		return '';
	}
}

////// Private functions:

function normalizeUrl(url, allowEmpty)
{
	if ((typeof url != "string") || !url.match(/\w/)) 
	{
		if (!allowEmpty) throw new Exception("No URL specified");
		url = null;
	}
	else 
	{
		url = Web.resolve(url);
	}
	return url;
}

function normalizeData(data)
{
	if (data == null || data == undefined) // will be a GET
	{
		data = null;
	}
	else if (typeof data != "string") 
	{
		if (typeof data == "object") 
		{
			data = Util.Url.hashToQuery(data);
		}
		else
		{
			throw new Exception("The data argument, if specified and not null, must be a string or an object");
		}
	}
	return data;
}

function normalizeAndApplyOptions(sb, options)
{
	// first make sure the sandbox has a set of default options
	if ((!sb.options) || (typeof sb.options != 'object'))
	{
		sb.options = Jaxer.Util.clone(Sandbox.defaults);
	}
	
	options = options || {};
	
	// iterate over the options that should always be there, and normalize each
	for (var p in Sandbox.defaults)
	{
		// if the option is missing, add the default
		if (!(p in sb.options))
		{
			sb.options[p] = Sandbox.defaults[p];
		}
		// if the option is being overridden by the supplied options, use the overridden value
		if (p in options)
		{
			sb.options[p] = options[p];
		}
	}
	
	// Now apply options on the jxWindow:
	if (typeof sb.options.onload == "function")
	{
		sb._jxWindow.onload = sb.options.onload;
	}
	if (typeof sb.options.onlocationchange == "function")
	{
		sb._jxWindow.onlocationchange = function onlocationchange(nsIURI, isHTTPRedirect)
		{
			return sb.options.onlocationchange.call(sb._jxWindow, nsIURI.spec, isHTTPRedirect);
		}
	}
	if (typeof sb.options.onsslcerterror == "function")
	{
		sb._jxWindow.onsslcerterror = function onsslcerterror(socketInfo, sslStatus, targetSite)
		{
			var cert = sslStatus.serverCert; /* nsIX509Cert */
			var certInfo = new Util.Certificate.CertInfo(socketInfo, sslStatus, targetSite);
			return sb.options.onsslcerterror.call(certInfo, cert, socketInfo, sslStatus, targetSite);
		}
	}
	sb._jxWindow.async = Boolean(sb.options.async);
	sb._jxWindow.allowJavascript = Boolean(sb.options.allowJavaScript); // slight capitalization discrepancy
	sb._jxWindow.allowMetaRedirects = Boolean(sb.options.allowMetaRedirects);
	sb._jxWindow.allowSubframes = Boolean(sb.options.allowSubFrames); // slight capitalization discrepancy
	// Not yet implemented in Core:
	sb._jxWindow.allowImages = false; // Boolean(sb.options.allowImages); // slight capitalization discrepancy
	
	return this.options;
}

function createInputStream(headers, body)
{
	var mimeStream = Components.classes["@mozilla.org/network/mime-input-stream;1"].createInstance(Components.interfaces.nsIMIMEInputStream);

	if (!headers || typeof headers != "object")
	{
		headers = {};
	}
	
	if (typeof body == "string")
	{
		if (!'Content-Type' in headers)
		{
			headers['Content-Type'] = "application/x-www-form-urlencoded";
		}
		delete headers['Content-Length'];
		mimeStream.addContentLength = true;
		var inputStream = Components.classes["@mozilla.org/io/string-input-stream;1"].createInstance(Components.interfaces.nsIStringInputStream);
		inputStream.data = body;
		mimeStream.setData(inputStream);
	}
	
	for (var headerName in headers)
	{
		mimeStream.addHeader(headerName, String(headers[headerName]));
	}
	
	return mimeStream;
}

/**
 * @classDescription {Jaxer.Sandbox.OpenOptions} Options used to define the behavior
 * of Jaxer.Sandbox opening.
 */

/**
 * Options used to define the behavior of Jaxer.Sandbox loading. Create a new Jaxer.Sandbox loading()
 * to get the default options, then modify its properties as needed before
 * passing it to new Jaxer.Sandbox() or to load() on an instance of Jaxer.Sandbox.
 * 
 * @constructor
 * @alias Jaxer.Sandbox.OpenOptions
 * @return {Jaxer.Sandbox.OpenOptions}
 * 		Returns an instance of OpenOptions.
 */
Sandbox.OpenOptions = function OpenOptions()
{

	/**
	 * Set to true for asynchronous, false for synchronous (default). 
	 * To use it asynchronously, see also Jaxer.Thread.waitFor.
	 *
	 * @alias Jaxer.Sandbox.OpenOptions.prototype.async
	 * @property {Boolean}
	 * @see Jaxer.Thread.waitFor
	 */	
	this.async = false;
	
	/**
	 * Whether to allow (server-side) execution of JavaScript, if any, within
	 * the loaded page. By default, this is true. This applies to JavaScript in the page
	 * inside this Sandbox instance, not the JavaScript of the loading page.
	 * Note that the JavaScript inside this Sandbox instance in any case does
	 * not have access to the Jaxer framework, and is running without the
	 * usual Jaxer server-side privileges.
	 *
	 * @alias Jaxer.Sandbox.OpenOptions.prototype.allowJavaScript
	 * @property {Boolean}
	 */
	this.allowJavaScript = true;
	
	/**
	 * Whether to allow refresh-based redirects within the loaded page.
	 * By default, this is true;
	 *
	 * @alias Jaxer.Sandbox.OpenOptions.prototype.allowMetaRedirects
	 * @property {Boolean}
	 */
	this.allowMetaRedirects = true;
	
	/**
	 * Whether to allow the loaded page to itself load frames or iframes.
	 * By default, this is true.
	 *
	 * @alias Jaxer.Sandbox.OpenOptions.prototype.allowSubFrames
	 * @property {Boolean}
	 */
	this.allowSubFrames = true;
	
	// Not yet implemented in core:
//	this.allowImages = false;
	
	/**
	 * The value of this, if any, will be used as the Sandbox's
	 * onload handler. The onload handler will be called after the page is loaded,
	 * and after the page's own onload is invoked, if any. 
	 * It's particularly useful when the page is loaded asynchronously, in which case it
	 * may be used in conjunction with the Sandbox's waitForCompletion method or
	 * with Jaxer.Thread.waitFor.
	 *
	 * @alias Jaxer.Sandbox.OpenOptions.prototype.onload
	 * @property {Function}
	 * @see Jaxer.Sandbox.prototype.waitForCompletion
	 * @see Jaxer.Thread.waitFor
	 */
	this.onload = null;
	
	/**
	 * Set this to an event handler function you want called if the document in the Sandbox redirects (changes its location).
	 * This will happen whether the redirect is due to an HTTP status code or header  (e.g. the web server
	 * returned a 307 Temporary Redirect and a Location header), or it is in the HTML either as a meta element
	 * with a HTTP-Equiv attribute or a JavaScript action (e.g. window.location = ...). 
	 * <br><br>
	 * The function will be called with two arguments: 
	 * <ol>
	 * <li>the new url to which the location would be changed, </li>
	 * <li>and a boolean flag that will be true if it's an HTTP status+location redirect 
	 * and false if it's a location change due to HTML or JavaScript. </li>
	 * </ol>
	 * <br><br>
	 * If the function returns a true, the redirection is allowed; if it returns a false the location is not changed.
	 * 
	 * @alias Jaxer.Sandbox.OpenOptions.prototype.onlocationchange
	 * @property {Function}
	 */
	this.onlocationchange = null;
	
	/**
	 * Set to a custom callback function to call if an SSL request fails
	 * due to a certificate error. 
	 * <br><br>
	 * The function will be called with the following arguments: 
	 * <ol>
	 * <li>the certInfo (an object containing properties describing the certificate and its status), </li>
	 * <li>the cert (certificate) itself (an object containing properties describing the certificate and its status), </li>
	 * <li>the XHR's socketInfo</li>
	 * <li>the XHR's sslStatus</li>
	 * <li>the XHR's targetSite</li>
	 * </ol>
	 * <br><br>
	 * It should return true to ignore the error,
	 * or false to abort the request. This is only available server-side.
	 * If (and only if) this is set to a function that returns true, 
	 * information about the failure can be retrieved from the extended 
	 * response's certInfo property.
	 * 
	 * @alias Jaxer.Sandbox.OpenOptions.prototype.onsslcerterror
	 * @property {Function}
	 */
	this.onsslcerterror = null;
	
	/**
	 * Add name-value pairs to this object to set headers on the http request.
	 * Both the name and the value should be strings.
	 * For POST requests, a 'Content-Type' header with value "application/x-www-form-urlencoded"
	 * will be added automatically unless you specify a different one, and a
	 * 'Content-Length' header with the appropriate value will also be set.
	 *
	 * @alias Jaxer.Sandbox.OpenOptions.prototype.headers
	 * @property {Object}
	 */
	this.headers = {};
}

/**
 * The default OpenOptions which new calls to new Jaxer.Sandbox() or Jaxer.Web.load() will use, 
 * unless overridden by the options argument.
 * 
 * @alias Jaxer.Sandbox.defaults
 * @property {Jaxer.Sandbox.OpenOptions}
 */
Sandbox.defaults = new Sandbox.OpenOptions();

frameworkGlobal.Sandbox = Jaxer.Sandbox = Sandbox;

Log.trace("*** Sandbox.js loaded");

})();
