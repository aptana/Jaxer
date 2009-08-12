/* 
 * This is the default configuration for Jaxer. 
 * It's best to make modifications to the configuration not in this file
 * but rather in the corresponding file in the folder specified by LOCAL_CONF_DIR (below) so your
 * modifications are preserved after you upgrade Jaxer.
 */
(function() {

/**
 * @classDescription {Jaxer.Config} Namespace containing the config properties.
 */

var config = 
{
	/**
	 * A flag to allow a central place to flip the operational behavior of a Jaxer instance from
	 * development to production mode. CAUTION: this should be set to false in a production
	 * environment to prevent sensitive information from getting to the browser!
	 * @alias Jaxer.Config.DEV_MODE
	 * @property {Boolean}
	 * @see Jaxer.Config.DISPLAY_ERRORS 
	 * @see Jaxer.Config.ALERT_CALLBACK_ERRORS 				
	 * @see Jaxer.Config.RELOAD_AUTOLOADS_EVERY_PAGE_REQUEST
	 * @see Jaxer.Config.CACHE_USING_SOURCE_CODE 				
	 */
	DEV_MODE: false,

	/**
	 * A flag that indicates (by convention) that the local config file was loaded during startup.
	 * @alias Jaxer.Config.LOCAL_CONFIG_LOADED
	 * @property {Boolean}
	 * 	
	 */
	LOCAL_CONFIG_LOADED : false,
	
	/**
	 * The prefix for sending callback requests back to Jaxer via the web server, usually "/jaxer-callback". 
	 * It should correspond to how your web server is configured.
	 * @alias Jaxer.Config.CALLBACK_URI
	 * @property {String}
	 * 	
	 */
	CALLBACK_URI: "/jaxer-callback",
	
	/**
	 * Indicates where to find the framework content, usually a mozilla resource url relative to the jaxer executable install folder.
	 * By default it is "resource:///framework"
	 * @alias Jaxer.Config.FRAMEWORK_DIR
	 * @property {String}
	 * 	
	 */
	FRAMEWORK_DIR: "resource:///framework",
	
	/**
	 * The name of the JavaScript file containing the Jaxer framework code. By default it is "serverFramework.js". 
	 * It is located in the folder indicated by Jaxer.Config.FRAMEWORK_DIR
	 * @alias Jaxer.Config.FRAMEWORK_FILE_NAME
	 * @property {String}
	 * 	
	 */
	FRAMEWORK_FILE_NAME: "serverFramework.js",
	
	/**
	 * Additional libraries to load on Jaxer startup. Multiple library names should be separated by
	 * spaces, commas, or semicolons. By default this is "JSLib.js".
	 * @alias Jaxer.Config.LIBRARY_FILE_NAMES
	 * @property {String}
	 * 	
	 */
	LIBRARY_FILE_NAMES: "JSLib.js",
	
	/**
	 * The folder containing JavaScript extension scripts. All JavaScript files in this folder will be loaded on startup after 
	 * the Jaxer framework.
	 * @alias Jaxer.Config.EXTENSIONS_DIR
	 * @property {String}
	 * 	
	 */
	EXTENSIONS_DIR: "resource:///framework/extensions",
	
	/**
	 * A boolean flag to indicate whether to use the Jaxer core logfile for framework and application logging.
	 * By default this is true.
	 * @see Jaxer.Config.LOG_FILE_NAME
	 * @alias Jaxer.Config.USE_CORE_LOG_FILE
	 * @property {Boolean}
	 * 	
	 */
	USE_CORE_LOG_FILE: true,
	
	/**
	 * Contains the name of the log file to use if Jaxer.Config.USE_CORE_LOG_FILE is set to false.
	 * By default this is neither set nor used.
	 * @see Jaxer.Config.USE_CORE_LOG_FILE
	 * @alias Jaxer.Config.LOG_FILE_NAME
	 * @property {String}
	 * 	
	 */
	// LOG_FILE_NAME: "jaxerLog.txt", 

	/**
	 * Contains the path to the folder containing local variants of the various Jaxer config-type files,
	 * which will override settings in the corresponding default files.
	 * The local files will not be overwritten by a Jaxer update. 
	 * Relative paths should be anchored from the Jaxer executable folder. 
	 * @see Jaxer.System.executableFolder
	 * @alias Jaxer.Config.LOCAL_CONF_DIR
	 * @example "../local_jaxer/conf"
	 * @property {String}
	 * 		path to the local conf folder, without a trailing slash
	 * 	
	 */
	LOCAL_CONF_DIR: ('LOCAL_CONF_DIR' in Jaxer.Config) ? Jaxer.Config.LOCAL_CONF_DIR : "../local_jaxer/conf", // It may have been set before this point, if you're in Jaxer Framework dev mode
	
	/**
	 * Contains the path (including filename) to the local folder containing JavaScript extension scripts. 
	 * All JavaScript file in this folder will be loaded on startup after the Jaxer framework.
	 * Relative paths should be anchored from the Jaxer executable folder. 
	 * @see Jaxer.System.executableFolder
	 * @see Jaxer.Config.EXTENSIONS_DIR
	 * @alias Jaxer.Config.LOCAL_EXTENSIONS_DIR
	 * @property {String}
	 * 	
	 */
	LOCAL_EXTENSIONS_DIR: ('LOCAL_EXTENSIONS_DIR' in Jaxer.Config) ? Jaxer.Config.LOCAL_EXTENSIONS_DIR : "../local_jaxer/extensions",
	
	/**
	 * A flag to indicate whether uncaught JavaScript errors are to be handled as response errors,
	 * e.g. using the response error page. By default this is true.
	 * @see Jaxer.Config.RESPONSE_ERROR_PAGE
	 * @see Jaxer.Config.DISPLAY_ERRORS
	 * @alias Jaxer.Config.UNCAUGHT_ERRORS_ARE_RESPONSE_ERRORS
	 * @property {Boolean}
	 * 		
	 */
	UNCAUGHT_ERRORS_ARE_RESPONSE_ERRORS: true,
	
	/**
	 * A flag to indicate whether errors during fetching of a file to be included are to be handled as response errors,
	 * e.g. using the response error page. By default this is true.
	 * @see Jaxer.Config.RESPONSE_ERROR_PAGE
	 * @see Jaxer.Config.DISPLAY_ERRORS
	 * @alias Jaxer.Config.INCLUDE_ERRORS_ARE_RESPONSE_ERRORS
	 * @property {Boolean}
	 * 	
	 */
	INCLUDE_ERRORS_ARE_RESPONSE_ERRORS: true,
	
	/**
	 * Contains the path (including filename) of the html file to be displayed instead of the detailed error message
	 * when a response error occurs, if Jaxer.Config.DISPLAY_ERRORS is set to false (i.e. in production mode).
	 * By default this is "resource:///content/responseError.html", and if it is not set a default HTML string is used.
	 * @see Jaxer.Config.FATAL_ERROR_PAGE
	 * @see Jaxer.Config.DISPLAY_ERRORS
	 * @alias Jaxer.Config.RESPONSE_ERROR_PAGE
	 * @property {String}
	 * 	
	 */
	RESPONSE_ERROR_PAGE: "resource:///content/responseError.html", 
	
	/**
	 * Contains the path (including filename) of the html file to be displayed instead of the detailed error message
	 * when a fatal error occurs, if Jaxer.Config.DISPLAY_ERRORS is set to false (i.e. in production mode).
	 * By default this is "resource:///content/fatalError.html", and if it is not set a default HTML string is used.
	 * @see Jaxer.Config.RESPONSE_ERROR_PAGE
	 * @see Jaxer.Config.DISPLAY_ERRORS
	 * @alias Jaxer.Config.FATAL_ERROR_PAGE
	 * @property {String}
	 * 	
	 */
	FATAL_ERROR_PAGE: "resource:///content/fatalError.html",
	
	/**
	 * Contains the message to display when a callback error occurs.
	 * @see Jaxer.Config.ALERT_CALLBACK_ERRORS
	 * @alias Jaxer.Config.CALLBACK_ERROR_MESSAGE
	 * @property {String}
	 * 	
	 */
	CALLBACK_ERROR_MESSAGE: "Error on server during callback - further information has been logged",
	
	/**
	 * Contains the message to display when a fatal callback error occurs.
	 * @see Jaxer.Config.ALERT_CALLBACK_ERRORS
	 * @alias Jaxer.Config.CALLBACK_FATAL_ERROR_MESSAGE
	 * @property {String} 	
	 */
	CALLBACK_FATAL_ERROR_MESSAGE: "Callback could not be processed due to server error - further information has been logged",
	
	/**
	 * Contains the message to display when a non-callback service error occurs.
	 * @alias Jaxer.Config.SERVICE_ERROR_MESSAGE
	 * @property {String}
	 * 	
	 */
	SERVICE_ERROR_MESSAGE: "Error on server during service call - further information has been logged",
	
	/**
	 * Contains the message to display when a fatal callback error occurs.
	 * @alias Jaxer.Config.SERVICE_FATAL_ERROR_MESSAGE
	 * @property {String} 	
	 */
	SERVICE_FATAL_ERROR_MESSAGE: "Service call could not be processed due to server error - further information has been logged",
	
	/**
	 * A Boolean flag to indicate whether errors should be displayed to the user. 
	 * By default this is set to true, which is fit for development/debug mode only.
	 * CAUTION: In a production environment this should be set to false so that sensitive server content 
	 * is not displayed to the client -- instead users will see a generic error page. 
	 * @see Jaxer.Config.RESPONSE_ERROR_PAGE
	 * @see Jaxer.Config.FATAL_ERROR_PAGE
	 * @alias Jaxer.Config.DISPLAY_ERRORS
	 * @property {Boolean}
	 */
	DISPLAY_ERRORS: true, 
	
	/**
	 * A Boolean flag to indicate whether to popup a window.alert(...) when a callback returns an error.
	 * By default this is set to true; if this is not the desired behavior in production, override
	 * it in your local Jaxer config.
	 * @see Jaxer.Config.CALLBACK_ERROR_MESSAGE
	 * @see Jaxer.Config.CALLBACK_FATAL_ERROR_MESSAGE
	 * @alias Jaxer.Config.ALERT_CALLBACK_ERRORS
	 * @property {Boolean}
	 * 	
	 */
	ALERT_CALLBACK_ERRORS: true, 
	
	/**
	 * The value of the type attribute on all emitted script tags -- namely the ones created
	 * via Jaxer.Util.DOM.createScript, which in particular the Jaxer framework uses to inject
	 * script tags into the browser-bound page. 
	 * Specifying attributes explicitly when calling Jaxer.Util.DOM.createScript will avoid using
	 * this value.
	 * If this is not set or empty, no type attribute will be added by default.
	 * This itself defaults to 'text/javascript'.
	 * @alias Jaxer.Config.DEFAULT_EMITTED_SCRIPT_TYPE
	 * @property {String}
	 * 	
	 */
	DEFAULT_EMITTED_SCRIPT_TYPE: 'text/javascript', 
	
	/**
	 * When true, the Jaxer framework will force reloading from source every script tag marked with
	 * autoload="true" during every request to the page (not during callbacks), rather than using the cached version
	 * which is refreshed only on server restart. This is intended for use while developing, 
	 * as it allows you to edit autoloaded file and see the changes without restarting Jaxer.
	 * This is set to "true" by default and should be overridden to "false" for production.
	 * @alias Jaxer.Config.RELOAD_AUTOLOADS_EVERY_PAGE_REQUEST
	 * @property {Boolean}
	 * 	
	 */
	RELOAD_AUTOLOADS_EVERY_PAGE_REQUEST: true, 
	
	/**
	 * A Boolean flag to indicate where to cache as source or bytecode.
	 * This is set to true by default, so it's easy in development time to read what was cached rather 
	 * than caching the bytecode. Override it to false for better performance.
	 * @alias Jaxer.Config.CACHE_USING_SOURCE_CODE
	 * @property {String}
	 * 	
	 */
	CACHE_USING_SOURCE_CODE: true,
	
	/**
 	 * 
 	 * The URL at which the client should look for the Jaxe client-side framework,
 	 * if EMBEDDED_CLIENT_FRAMEWORK_SRC is undefined or null.
 	 * To embed the client part of the Jaxer framework in the web page, 
	 * specify its location in EMBEDDED_CLIENT_FRAMEWORK_SRC
	 * Alternatively, to have the web server serve its contents, 
	 * <BR>1) comment out EMBEDDED_CLIENT_FRAMEWORK_SRC,
	 * <BR>2) copy the client framework to somewhere the web server can reach it, and
	 * <BR>3) set CLIENT_FRAMEWORK_SRC to the src of the script element that will be inserted into the page
	 * <BR>EMBEDDED_CLIENT_FRAMEWORK_SRC: "resource:///framework/clientFramework_compressed.js"
	 * 
	 * @alias Jaxer.Config.CLIENT_FRAMEWORK_SRC
	 * @property {String}
	 * 
	 */
	 CLIENT_FRAMEWORK_SRC: '/jaxer/framework/clientFramework_compressed.js',  // This will only work after you make the clientFramework file available on the web server
		
	/**
	 * If a URL matches this regular expression, it will be treated as a file URL even when resolved
	 * for Web-type operations such as within script tags, in Jaxer.load(), or in Web.get().
	 * This is useful for accessing files that reside the jaxer-include folder which
	 * are protected from being served out by the web server.
	 * 
	 * @alias Jaxer.Config.REWRITE_TO_FILE_URL_REGEX
	 * @property {RegExp}
	 */
	REWRITE_TO_FILE_URL_REGEX: /\bjaxer-include\//,
	
	/**
	 * To include scripts, access files, and so on, Jaxer may need to fetch content from the web server.
	 * If your web server needs to be reached via a different domain/port than the incoming request,
	 * uncomment and change the following to replace the protocol://domain:port with this value.
	 * For example, setting it to "http://192.168.0.1:8082" will mean that Jaxer will request 
	 * content from your web server using the IP address 192.168.0.1 on port 8082.
	 * This is neither used nor set by default.
	 * @alias Jaxer.Config.REWRITE_RELATIVE_URL
	 * @property {String}
	 * 	
	 */
//	REWRITE_RELATIVE_URL: "http://192.168.0.1:8082", // If your web server is reachable at IP 192.168.0.1 on port 8082
	
	/**
	 * A regular expression used to test the requested url and determine whether or not to apply the Jaxer.Config.REWRITE_RELATIVE_URL
	 * If this is not set, and Jaxer.Config.REWRITE_RELATIVE_URL is set, then all requests to the web server
	 * are rewritten using Jaxer.Config.REWRITE_RELATIVE_URL.
	 * For example, setting it to "^http\\:\\/\\/my.domain.com" will mean that Jaxer will request 
	 * content from your web server using Jaxer.Config.REWRITE_RELATIVE_URL only for URLs starting with
	 * "http://my.domain.com".
	 * This is neither used nor set by default.
	 * @see Jaxer.Config.REWRITE_RELATIVE_URL
	 * @alias Jaxer.Config.REWRITE_RELATIVE_URL_REGEX
	 * @property {String}
	 * 	
	 */
//	REWRITE_RELATIVE_URL_REGEX: "^http\\:\\/\\/my.domain.com", // Optional -- if e.g. you only want to replace URLs that start with this

	/**
	 * To set any of Mozilla preferences, add properties to Jaxer.Config.MOZ_PREFS.
	 * The name of each property should correspond exactly to the Mozilla preference,
	 * and the value should be an integer, boolean, or string.
	 * To see some of the available options, launch Firefox and browse to about:config
	 * @alias Jaxer.Config.MOZ_PREFS
	 * @property {Object}
	 * 	
	 */
	MOZ_PREFS: {},
	
	/**
	 * If this is true, a conventional "X-Powered-By" HTTP header is added to every response processed by Jaxer
	 * indicating that is was processed by Jaxer and giving the Jaxer version. This is set to false by
	 * default for added security.
	 * @alias Jaxer.Config.EXPOSE_JAXER
	 * @property {Boolean}
	 * 	
	 */
	EXPOSE_JAXER: false,
	
	/**
	 * If this is true, any onload event listeners added programatically (via addEventListener
	 * or window.onload) will fire server-side, right after any onserverload handling.
	 * Note that an onload attribute on the body element will not be used server-side
	 * regardless of this setting.
	 * This is set to true by default. You can also override it on a specific request
	 * by setting Jaxer.onloadEnabled.
	 * @alias Jaxer.Config.ONLOAD_ENABLED
	 * @property {Boolean}
	 * @see Jaxer.onloadEnabled
	 * 	
	 */
	ONLOAD_ENABLED: true,
	
	/**
	 * If any config-type parameters were specified on the command line when launching Jaxer, they will
	 * be included as properties of this object.
	 * Note: This is created and populated inside aptEventHandler.js
	 * @alias Jaxer.Config.COMMAND_LINE_PARAMS
	 * @property {Object}
	 * 
	 */
	
	/**
	 * This determines the type returned by Jaxer.request.body, based on the Content-Type 
	 * of the request. The keys are the various mime-type strings, the same as the Content-Type
	 * HTTP header or what Jaxer.request.contentType returns. The mime-type of the request 
	 * must begin with the key for the value to be used. The values are one of the
	 * strings 'string', 'byteArray', or 'stream' (case-insensitively) -- any other value 
	 * is treated as if the key were absent. 
	 * Jaxer.request.body will be a string, a byte array (array of integers),
	 * or a stream, respectively. Note that you can always just access these properties
	 * directly as Jaxer.request.bodyAsString, Jaxer.request.bodyAsByteArray, and 
	 * Jaxer.request.bodyAsStream.
	 * @alias Jaxer.Config.REQUEST_BODY_AS
	 * @property {Object}
	 * @see Jaxer.Request.prototype.body
	 * @see Jaxer.Request.prototype.bodyAsString
	 * @see Jaxer.Request.prototype.bodyAsByteArray
	 * @see Jaxer.Request.prototype.bodyAsStream
	 * 
	 */
	REQUEST_BODY_AS:
	{
		  'application/octet-stream': 'byteArray'
		, 'application/xml': 'string'
		, 'application/x-www-form-urlencoded': 'string'
		, 'multipart/form-data': null 					// The body (and bodyAsString) are not available -- use the data and files properties instead
		, 'text/html': 'string'
		, 'text/plain': 'string'
		, 'text/xml': 'string'
	}
};

/**
 * Contains the parameters for a connection object of one of the supported DB implementations.
 * At the very least, the IMPLEMENTATION property must be defined and be the name of one
 * of the supported implementations in the DB namespace (e.g. "SQLite", "MySQL").
 * The remaining parameters have all the information required to connect to the database.
 * This will be used for the Jaxer framework's internal persistence needs.
 * It defaults to using SQLite.
 * This property is best managed in the local_jaxer/conf/config.js which will not be overwritten by subsequent
 * jaxer updates. Changes in that file will get picked up automatically on restart.
 * 
 * @alias Jaxer.Config.FRAMEWORK_DB
 * @property {Object}
 */
config.FRAMEWORK_DB = {
		// Appropriate for the Aptana Jaxer distribution: 
		// the data folder is a peer of the folder containing the executable
		IMPLEMENTATION: "SQLite",
		PATH: config.LOCAL_CONF_DIR+"/../data/frameworkdata.sqlite",
		CLOSE_AFTER_EXECUTE: false,
		CLOSE_AFTER_REQUEST: true,
		MAX_NUM_TRIES: 100, 		// if locking is detected, retry up to this many times
		MILLIS_BETWEEN_TRIES: 37	// if locking is detected, sleep this many milliseconds between tries
};

for (var prop in config)
{
	Jaxer.Config[prop] = config[prop];
}

})();
