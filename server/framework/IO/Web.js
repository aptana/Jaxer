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

var log = Log.forModule("Web");

/**
 * @namespace {Jaxer.Web} Namespace object holding functions and members used to
 * resolve and fetch web resources. Fetching is done via XMLHttpRequests.
 */
var Web = {};

// private variables
var networkUtils = new Jaxer.NetworkUtils();

// On the server side, XHR should be defined on the frameworkGlobal like the rest of the framework
frameworkGlobal.XHR = Jaxer.XHR;

// The following is documented in XHR.js:
XHR.getTransport = function getTransport()
{
	var xhr;
	if (Jaxer.pageWindow) 
	{
		xhr = new Jaxer.pageWindow.XMLHttpRequest();
	}
	else 
	{
		var xhr = Components.classes["@mozilla.org/xmlextras/xmlhttprequest;1"].createInstance();
		xhr.QueryInterface(Components.interfaces.nsIXMLHttpRequest);
	}
	return xhr;
}

/**
 * Resolves a URL to an absolute URL (one starting with protocol://domain...)
 * To resolve a relative URL (with or without a leading /), use a current
 * document or callback for reference. Uses the base href of the current
 * document, if specified. 
 * If Jaxer.Config.REWRITE_TO_FILE_URL_REGEX exists and matches the url,
 * the url is resolved as a file-type URL: it will use the given referenceUrl if
 * it's already a file-based one, otherwise it will use the Jaxer.request.documentRoot
 * for an absolute url or Jaxer.request.currentFolder for a relative url.
 * If Jaxer.Config.REWRITE_RELATIVE_URL exists (and we're not dealing in file-based URLs),
 * its value is used to rewrite the relative url -- replacing
 * Jaxer.Config.REWRITE_RELATIVE_URL_REGEX pattern, if it exists, else replacing
 * http[s]:// followed by anything but a slash.
 *  
 * @alias Jaxer.Web.resolve
 * @param {String} url 
 * 		The URL to resolve
 * @param {String} [referenceUrl] 
 * 		An optional reference URL to use, overriding the built-in logic
 * @return {String}
 * 		The fully-resolved URL, or the original URL if the input was already an
 * 		absolute URL
 */
Web.resolve = function resolve(url, referenceUrl)
{
	// absolute URL with protocol are unchanged
	if (/^\w+\:\/\//.test(url)) 
	{
		if (/^resource/.test(url)) 
		{
			resolvedUrl = url.replace(/^resource\:\/\//, System.executableUrl);
		}
		else 
		{
			resolvedUrl = url;
		}
	}
	else
	{
		if (Config.REWRITE_TO_FILE_URL_REGEX && 					// If we're supposed to rewrite some URLs to be file-based...
			Config.REWRITE_TO_FILE_URL_REGEX.test(url) &&			// ... and this is such a URL...
		    (!referenceUrl || !Util.Url.isFileUrl(referenceUrl)))	// ... and we're not given a file-based reference URL
		{															// ... then use the document root (for absolute URLs) or current folder (for relative ones)
			referenceUrl = "file://" + (/^\//.test(url) ? Jaxer.request.documentRoot : Jaxer.request.currentFolder) + "/";
		}															// as the file-based reference URL for resolving
		else if (!referenceUrl) 									// otherwise get a web-based URL as the reference, if needed
		{
			referenceUrl = Web.getDefaultReferenceUrl();
		}
		var currentUriObj = networkUtils.fixupURI(referenceUrl);
		var resolvedUrl = currentUriObj.resolve(url);
		if (Jaxer.Config.REWRITE_RELATIVE_URL)
		{
			var regex;
			if (Config.REWRITE_RELATIVE_URL_REGEX)
			{
				regex = (typeof Jaxer.Config.REWRITE_RELATIVE_URL_REGEX == "string") ? new RegExp(Jaxer.Config.REWRITE_RELATIVE_URL_REGEX) : Jaxer.Config.REWRITE_RELATIVE_URL_REGEX;
			}
			else
			{
				regex = /^http[s]?\:\/\/[^\/]+/;
			} 
			resolvedUrl = resolvedUrl.replace(regex, Jaxer.Config.REWRITE_RELATIVE_URL);
		}
	}
	return resolvedUrl;
}

/**
 * Returns the URL to be used as a reference for resolving relative URLs if no
 * other reference is given
 * 
 * @advanced
 * @alias Jaxer.Web.getDefaultReferenceUrl
 * @return {String}
 * 		The absolute URL
 */
Web.getDefaultReferenceUrl = function getDefaultReferenceUrl()
{
	if (!Jaxer.pageWindow || !Jaxer.pageWindow.document || !Jaxer.request || !Jaxer.request.parsedUrl) 
	{
		throw new Exception("No default reference URL could be constructed; cannot use relative URLs outside the context of a page or callback");
	}
	var baseElt = Jaxer.pageWindow.document.getElementsByTagName('base')[0];
	if (baseElt && baseElt.href && (baseElt.href != '')) 
	{
		referenceUrl = baseElt.href;
	}
	else 
	{
		referenceUrl = Jaxer.request.parsedUrl.url;
	}
	return referenceUrl;
}

/**
 * A generalized method to access a web URL via the built-in XMLHttpRequest object.
 * 
 * @alias Jaxer.Web.send
 * @param {String} url 
 * 		The url to access
 * @param {String} [method] 
 * 		Usually 'GET' (default) or 'POST'
 * @param {String|Object} [data] 
 * 		Use for POST submissions, or for GET requests if the url does not
 * 		already contain the data. This may be a string (usually of the form
 * 		name1=value&name2=value), or an object whose name->value property pairs
 * 		will be used to construct such a string.
 * @param {Jaxer.XHR.SendOptions|Object} [options] 
 * 		Options for finer control of how the request is made.
 * @return {Object}
 * 		The text of the requested document, or the XML DOM if the response was
 * 		an XML document. Set the "as" property in the options argument to "text"
 * 		or "xml" or "e4x" to force what is returned. An Exception is thrown if not
 * 		successful.
 */
Web.send = function send(url, method, data, options)
{
	if ((typeof url != "string") || !url.match(/\w/)) 
	{
		throw new Exception("No URL specified");
	}
	options = options || {};
	if ((data != null) && (data != undefined) && (typeof data != "string")) 
	{
		data = Util.Url.hashToQuery(data);
	}
	log.trace("Parameters are: url: " + url + "; method: " + method + "; data: " + data + "; options: " + uneval(options));
	var resolvedUrl = Web.resolve(url);
	log.trace("Resolved URL to: " + resolvedUrl);
	options.url = resolvedUrl;
	if (typeof method != "undefined") options.method = method;
	var response = XHR.send(data, options);
	log.trace("Received response of type '" + (typeof response) + 
		((typeof response == "string") ? 
			"' and length " + response.length + ": " + response.substr(0, 100) + (response.length > 100 ? "..." : "") :
			"'"));
	return response;
}

/**
 * Fetch a document from a URL by resolving it to a local file
 * (if it starts with file://) or by a GET command.
 * 
 * @alias Jaxer.Web.get
 * @param {String} url 
 * 		The URL to fetch, which may be a file:// URL if desired. This will first
 * 		be resolved by Dir.resolve() or Web.resolve().
 * @param {Jaxer.XHR.SendOptions|Object} options 
 * 		Options for finer control of how the request is made.
 * @return {Object}
 * 		The text of the requested document, or the XML DOM if the response was
 * 		an XML document. Set the "as" property in the options argument to "text"
 * 		or "xml" to force what is returned. An Exception is thrown if not
 * 		successful.
 */
Web.get = function get(url, options)
{
	var content;
	var url = Web.resolve(url); // Do this explicitly in case it's turned into a file-type URL
	if (Util.Url.isFile(url)) 
	{
		try 
		{
			log.trace("Retrieving from file URL: " + url);
			var path = Dir.resolve(url);
			log.trace("Resolved to path: " + path);
			content = File.read(path);
			log.trace("Read " + (content ? content.length : 0) + " characters from file");
		} 
		catch (e) 
		{
			throw new Exception("Could not read file from url='" + url + "' (as '" + path + "'): " + e);
		}
	}
	else
	{
		log.trace("Retrieving from web URL: " + url);
		content = Web.send(url, Web.ACTIONS.GET, null, options);
	}
	return content;
}

/**
 * POST data to a URL and return the response web page.
 * 
 * @alias Jaxer.Web.post
 * @param {String} url 
 * 		The URL of the page to POST to and fetch. This will first be resolved by
 * 		Web.resolve().
 * @param {String|Object} data 
 * 		The data to submit. If a string, it should be a query string in a format
 * 		(name1=value1&name2=value2) suitable for a Content-Type of
 * 		'application/x-www-form-urlencoded'. If an object, its enumerable
 * 		properties will be used to construct the query string.
 * @param {Jaxer.XHR.SendOptions|Object} [options] 
 * 		Options for finer control of how the request is made.
 * @return {Object}
 * 		The text of the response, or the XML DOM if the response was
 * 		an XML document. Set the "as" property in the options argument to "text"
 * 		or "xml" to force what is returned. An Exception is thrown if not
 * 		successful.
 */
Web.post = function post(url, data, options)
{
	return Web.send(url, Web.ACTIONS.POST, data, options);
}

/**
 * Send an OPTIONS-type HTTP request to a URL and return the response.
 * 
 * @alias Jaxer.Web.options
 * @param {String} url 
 * 		The URL of the page to send the request. This will first be resolved by
 * 		Web.resolve().
 * @param {String|Object} data 
 * 		The data to submit. If a string, it should be a query string in a format
 * 		(name1=value1&name2=value2) suitable for a Content-Type of
 * 		'application/x-www-form-urlencoded'. If an object, its enumerable
 * 		properties will be used to construct the query string.
 * @param {Jaxer.XHR.SendOptions|Object} [options] 
 * 		Options for finer control of how the request is made.
 * @return {Object}
 * 		The text of the response, or the XML DOM if the response was
 * 		an XML document. Set the "as" property in the options argument to "text"
 * 		or "xml" to force what is returned. An Exception is thrown if not
 * 		successful.
 */
Web.options = function options(url, data, options)
{
	return Web.send(url, Web.ACTIONS.OPTIONS, data, options);
}

/**
 * Send a HEAD-type HTTP request to a URL and return the response.
 * 
 * @alias Jaxer.Web.head
 * @param {String} url 
 * 		The URL of the page to send the request. This will first be resolved by
 * 		Web.resolve().
 * @param {Jaxer.XHR.SendOptions|Object} [options] 
 * 		Options for finer control of how the request is made.
 * @return {Object}
 * 		The text of the response, or the XML DOM if the response was
 * 		an XML document. Set the "as" property in the options argument to "text"
 * 		or "xml" to force what is returned. An Exception is thrown if not
 * 		successful.
 */
Web.head = function head(url, options)
{
	return Web.send(url, Web.ACTIONS.HEAD, null, options);
}

/**
 * Send a PUT-type HTTP request to a URL and return the response.
 * 
 * @alias Jaxer.Web.put
 * @param {String} url 
 * 		The URL of the page to send the request. This will first be resolved by
 * 		Web.resolve().
 * @param {String|Object} data 
 * 		The data to submit. If a string, it should be a query string in a format
 * 		(name1=value1&name2=value2) suitable for a Content-Type of
 * 		'application/x-www-form-urlencoded'. If an object, its enumerable
 * 		properties will be used to construct the query string.
 * @param {Jaxer.XHR.SendOptions|Object} [options] 
 * 		Options for finer control of how the request is made.
 * @return {Object}
 * 		The text of the response, or the XML DOM if the response was
 * 		an XML document. Set the "as" property in the options argument to "text"
 * 		or "xml" to force what is returned. An Exception is thrown if not
 * 		successful.
 */
Web.put = function put(url, data, options)
{
	return Web.send(url, Web.ACTIONS.PUT, data, options);
}

/**
 * Send a DELETE-type HTTP request to a URL and return the response.
 * 
 * @alias Jaxer.Web.del
 * @param {String} url 
 * 		The URL of the page to send the request. This will first be resolved by
 * 		Web.resolve().
 * @param {Jaxer.XHR.SendOptions|Object} [options] 
 * 		Options for finer control of how the request is made.
 * @return {Object}
 * 		The text of the response, or the XML DOM if the response was
 * 		an XML document. Set the "as" property in the options argument to "text"
 * 		or "xml" to force what is returned. An Exception is thrown if not
 * 		successful.
 */
Web.del = function del(url, options)
{
	return Web.send(url, Web.ACTIONS.DELETE, null, options);
}

/**
 * GET or POST data to a URL, loading the result into the window of
 * a new Jaxer.Sandbox object.
 * 
 * @alias Jaxer.Web.open
 * @param {String} url 
 * 		The URL of the page to GET or POST to. This will first be resolved by
 * 		Web.resolve().
 * @param {String|Object} [data]
 * 		The data to submit, if any. If this parameter is omitted or is null,
 * 		a GET is used; otherwise a POST is used. 
 * 		If this is a string, it should usually be a query string in a format
 * 		(name1=value1&name2=value2) suitable for a Content-Type of
 * 		'application/x-www-form-urlencoded'. If an object, its enumerable
 * 		properties will be used to construct the query string.
 * @param {Jaxer.Sandbox.OpenOptions|Object} options 
 * 		Options for finer control of how the request is made.
 * @return {Jaxer.Sandbox}
 * 		The Sandbox, which will usually have a window and document in it 
 * 		(if the request was successful and returned an HTML page)
 * @see Jaxer.Sandbox
 */
Web.open = function open(url, data, options)
{
	return new Jaxer.Sandbox(url, data, options);
}


/**
 * An enumeration of constants for various standard HTTP methods or actions 
 * 
 * @alias Jaxer.Web.ACTIONS
 * @property {Object}
 */
Web.ACTIONS = {};

/**
 * A constant describing the standard HTTP GET method (or action) 
 * 
 * @alias Jaxer.Web.ACTIONS.GET
 * @property {String}
 */
Web.ACTIONS.__defineGetter__("GET", function() { return "GET"; });
/**
 * A constant describing the standard HTTP POST method (or action) 
 * 
 * @alias Jaxer.Web.ACTIONS.POST
 * @property {String}
 */
Web.ACTIONS.__defineGetter__("POST", function() { return "POST"; });
/**
 * A constant describing the standard HTTP PUT method (or action) 
 * 
 * @alias Jaxer.Web.ACTIONS.PUT
 * @property {String}
 */
Web.ACTIONS.__defineGetter__("PUT", function() { return "PUT"; });
/**
 * A constant describing the standard HTTP DELETE method (or action) 
 * 
 * @alias Jaxer.Web.ACTIONS.DELETE
 * @property {String}
 */
Web.ACTIONS.__defineGetter__("DELETE", function() { return "DELETE"; });
/**
 * A constant describing the standard HTTP HEAD method (or action) 
 * 
 * @alias Jaxer.Web.ACTIONS.HEAD
 * @property {String}
 */
Web.ACTIONS.__defineGetter__("HEAD", function() { return "HEAD"; });
/**
 * A constant describing the standard HTTP OPTIONS method (or action) 
 * 
 * @alias Jaxer.Web.ACTIONS.OPTIONS
 * @property {String}
 */
Web.ACTIONS.__defineGetter__("OPTIONS", function() { return "OPTIONS"; });


frameworkGlobal.Web = Jaxer.Web = Web;

Log.trace("*** Web.js loaded");

})();