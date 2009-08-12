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

var log = Log.forModule("App");

/**
 * @classDescription {Jaxer.App} Class for the current app's metadata
 */

/**
 * The constructor of an object that describes the current app's metadata
 * 
 * @alias Jaxer.App
 * @constructor
 * @param {Object} props
 * 		The properties to use for initializing this application: urlTest, name, db, path, etc.
 * @return {Jaxer.App}
 * 		Returns an instance of App
 */
function App(props)
{
	
	var defaultApp = Jaxer.Config.DEFAULT_APP || {};
	
	if (!props) throw new Exception("When constructing a new App, you must specify the props argument");

	if (log.getLevel() == Log.TRACE) log.trace("Constructing App from props: " + uneval(props) + " and a default app of: " + uneval(defaultApp));
	
	/**
	 * The object that determines whether this app should be used for the current request.
	 * If a function, it takes the request's parsedUrl as its sole parameter, and returns true if this app should be used.
	 * If a regular expression, it should match against the request's parsedUrl's path if this app should be used.
	 * If a string, it should match the beginning of the request's parsedUrl's path if this app should be used.
	 * 
	 * @alias Jaxer.App.prototype.urlTest
	 * @property {Function, RegExp, String}
	 */
	this.urlTest = props.urlTest || defaultApp.urlTest;
	
	/**
	 * The name to use for the current application. This can be a string or a function:
	 * the function takes the request's parsedUrl as its sole parameter, 
	 * and returns the name.
	 * The name should be unique across all applications (this is checked at server start).
	 * 
	 * @alias Jaxer.App.prototype.name
	 * @property {String, Function}
	 * @see Jaxer.App.prototype.NAME
	 */
	this.name = props.name || defaultApp.name;
	
	/**
	 * The key to use when persisting or restoring values in the scope of the current application, using Jaxer.application. 
	 * This can be a string or a function:
	 * the function takes the request's parsedUrl as its sole parameter, 
	 * and returns the key.
	 * The key should be unique across all applications.
	 * 
	 * The appKey will first default to the application name if not specified, then finally to the default appKey 
	 * in the case of the standard install the default appKey will be localhost:8081
	 * 
	 * @alias Jaxer.App.prototype.appKey
	 * @property {String, Function}
	 * @see Jaxer.application
	 * @see Jaxer.App.prototype.APP_KEY
	 */
	this.appKey = props.appKey || props.name || defaultApp.appKey;
	
	/**
	 * The key to use when persisting or restoring values in the scope of the current page, 
	 * using Jaxer.page or Jaxer.sessionPage. 
	 * This can be a string or a function:
	 * the function takes the request's parsedUrl as its sole parameter, 
	 * and returns the key.
	 * The key should be unique across all pages for any application.
	 * 
	 * @alias Jaxer.App.prototype.pageKey
	 * @property {String, Function}
	 * @see Jaxer.pageKey
	 * @see Jaxer.App.prototype.PAGE_KEY
	 */
	this.pageKey = props.pageKey || defaultApp.pageKey;
	
	/**
	 * The database connection parameters to use by default for the current application. 
	 * This can be an object or a function: 
	 * the function takes the app's name and the request's parsedUrl as its two parameters, 
	 * and returns the database connection parameters to use.
	 * 
	 * @alias Jaxer.App.prototype.db
	 * @property {Object, Function}
	 * @see Jaxer.App.prototype.DB
	 */
	this.db = props.db || defaultApp.db;
	
	/**
	 * The filesystem path to use by default for the current application. 
	 * This can be a string or a function:
	 * the function takes the app's name and the request's parsedUrl as its two parameters, 
	 * and returns the parameters to use.
	 * 
	 * @alias Jaxer.App.prototype.path
	 * @property {String, Function}
	 * @see Jaxer.App.prototype.PATH
	 */
	this.path = props.path || defaultApp.path;
	
	/**
	 * For service-type (not page-type) requests where Jaxer is the handler (and it's not a callback),
	 * this determines how the request is handled. It can be a string or a function:
	 * the function takes the app's name and the request's parsedUrl as its two parameters, 
	 * and is executed right after the (empty) DOM is created and ready. If it returns a string,
	 * or if the handler property is already a string, that string should be the file://...-type URL  
	 * location of a JavaScript file to be loaded and executed to handle this request.
	 * 
	 * @alias Jaxer.App.prototype.handler
	 * @property {String, Function}
	 * @see Jaxer.App.prototype.HANDLER
	 */
	this.handler = props.handler || defaultApp.handler;
	
	this.init();
	
	// You can add properties to an app as long as they don't conflict with existing properties
	for (var extraProp in props)
	{
		if (!(extraProp in this))
		{
			this[extraProp] = props[extraProp];
		}
	}

}

/**
 * Initializes or re-initializes this App object, using any needed values from Jaxer.request (if any)
 * @private
 */
App.prototype.init = function init()
{
	
	var parsedUrl = (Jaxer.request ? Jaxer.request.parsedUrl : null);
	
	/**
	 * The fully-evaluated name to use for the current application. 
	 * If this app's "name" property was a function, it will have been evaluated to a string using the current request.
	 * 
	 * @alias Jaxer.App.prototype.NAME
	 * @property {String}
	 * @see Jaxer.App.prototype.name
	 */
	var _NAME = extractFunctionOrString(this.name, [parsedUrl]);
	this.__defineGetter__("NAME", function() { return _NAME; });
	
	log.trace("Initializing app '" + _NAME + "' (urlTest: " + this.urlTest + ")");
	
	/**
	 * The fully-evaluated key to use when persisting or restoring values in the scope of the current application, using Jaxer.application. 
	 * If this app's "appKey" property was a function, it will have been evaluated to a string using the current request.
	 * 
	 * @alias Jaxer.App.prototype.APP_KEY
	 * @property {String}
	 * @see Jaxer.App.prototype.appKey
	 */
	var _APP_KEY = parsedUrl ? extractFunctionOrString(this.appKey, [parsedUrl]) : null;
	this.__defineGetter__("APP_KEY", function() { return _APP_KEY; });
	log.trace("APP_KEY for app '" + this.NAME + "': " + _APP_KEY);

	/**
	 * The fully-evaluated key to use when persisting or restoring values in the scope of the current page, using Jaxer.page or Jaxer.sessionPage. 
	 * If this app's "pageKey" property was a function, it will have been evaluated to a string using the current request.
	 * 
	 * @alias Jaxer.App.prototype.PAGE_KEY
	 * @property {String}
	 * @see Jaxer.App.prototype.pageKey
	 */
	var _PAGE_KEY = parsedUrl ? extractFunctionOrString(this.pageKey, [parsedUrl]) : null;
	this.__defineGetter__("PAGE_KEY", function() { return _PAGE_KEY; });
	log.trace("PAGE_KEY for app '" + this.NAME + "': " + _PAGE_KEY);

	/**
	 * The fully-evaluated database parameters to use by default for the current application. 
	 * If this app's "db" property was a function, it will have been evaluated to an object using the current request.
	 * 
	 * @alias Jaxer.App.prototype.DB
	 * @property {Object}
	 * @see Jaxer.App.prototype.DB
	 */
	var _DB = extractFunctionOrObject(this.db, [_NAME, parsedUrl]);
	this.__defineGetter__("DB", function() { return _DB; });

	/**
	 * The fully-evaluated filesystem path to use by default for the current application. 
	 * If this app's "path" property was a function, it will have been evaluated to a string using the current request.
	 * 
	 * @alias Jaxer.App.prototype.PATH
	 * @property {String}
	 * @see Jaxer.App.prototype.path
	 */
	var _PATH = extractFunctionOrString(this.path, [_NAME, parsedUrl]);
	this.__defineGetter__("PATH", function() { return _PATH; });
	log.trace("PATH for app '" + this.NAME + "': " + _PATH);
	if (_PATH) this.initPath();

}

function extractFunctionOrString(obj, args)
{
	switch (typeof obj)
	{
		case "function":
			return String(obj.apply(null, args));
		case "string":
			return obj;
		default:
			return null;
	}
}

function extractFunctionOrObject(obj, args)
{
	switch (typeof obj)
	{
		case "function":
			return obj.apply(null, args);
		case "object":
			return obj;
		default:
			return null;
	}
}

/**
 * Initializes (creates if necessary) the path for this app, if any
 * 
 * @alias Jaxer.App.prototype.initPath
 */
App.prototype.initPath = function initPath()
{
	if (this.PATH)
	{
		var path = Dir.resolve(this.PATH);
		var dir = new Dir(path);
		if (dir.exists)
		{
			if (!dir.isDir)
			{
				throw new Exception("The path '" + path + "' for app '" + this.NAME + "' exists but is not a folder!");
			}
		}
		else if (Config.AUTO_CREATE_APP_PATHS)
		{
			log.trace("Creating or verifying hierarchy for app '" + this.NAME + "': " + path);
			dir.createHierarchy();
		}
	}
}

/**
 * Calls the handler function for this app, if there is a handler and it's a function.
 * In any case it also sets the HANDLER property for this app.
 * 
 * @alias Jaxer.App.prototype.callHandler
 */
App.prototype.callHandler = function callHandler()
{
	/**
	 * The fully-evaluated file://...-type URL location of a JavaScript file to be loaded and executed to handle this request
	 * If this app's "handler" property was a function, it will have been evaluated to a string using the current request.
	 * If this app's "handler" function did not return a string, this property will be set to null,
	 * and no JavaScript file will be loaded.
	 * 
	 * @alias Jaxer.App.prototype.HANDLER
	 * @property {String}
	 * @see Jaxer.App.prototype.handler
	 */
	var _HANDLER;
	var parsedUrl = (Jaxer.request ? Jaxer.request.parsedUrl : null);
	switch (typeof this.handler)
	{
		case "function":
			_HANDLER = this.handler(this.NAME, parsedUrl);
			if (typeof _HANDLER != 'string') _HANDLER = null;
			break;
		case "string":
			_HANDLER = this.handler;
			break;
		default:
			_HANDLER = null;
	}
	this.__defineGetter__("HANDLER", function() { return _HANDLER; });
	log.trace("HANDLER for app '" + this.NAME + "': " + _HANDLER);
	return _HANDLER;
}

/**
 * Initialize the static apps: those whose names are known in advance
 * @private
 */
App.initStatic = function initStatic()
{
	for (var appName in Config.appsByName)
	{
		var app = Config.appsByName[appName];
		app.initPath();
	}
}

frameworkGlobal.App = Jaxer.App = App;

Log.trace("*** App.js loaded");

})();
