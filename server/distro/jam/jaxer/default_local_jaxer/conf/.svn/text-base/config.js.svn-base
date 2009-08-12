/* 
 * Use this file to override the default configuration settings.
 * This file is read right after the default config.js is read
 * but before anything else is done, so you can override anything you need.
 * Since these only gets read at server startup, you'll need to restart Jaxer
 * after modifying this file for those modifications to have an effect.
 */

(function(){

	Config.LOCAL_CONFIG_LOADED = true; // flag to indicate the local config was loaded.

	// Config.DEV_MODE is used as a group switch for configuration settings generally preferred during 
	// development only. You may still set configuration items individually, if required.
	// CAUTION: this should be set to false in a production environment to prevent sensitive information 
	// from getting to the browser!
	//Config.DEV_MODE = true; 	// default setting is production mode (false); set to true for development/debugging
									  	
	// CAUTION: In a production environment this should be set to false so that sensitive server content 
	// is not displayed to the client -- instead users will see a generic error page.
	Config.DISPLAY_ERRORS 						=  Config.DEV_MODE ; // some error messages may be displayed within the served page or in callbacks
	Config.ALERT_CALLBACK_ERRORS 				=  Config.DEV_MODE ; // popup a window.alert(...) when a callback returns an error
	Config.RELOAD_AUTOLOADS_EVERY_PAGE_REQUEST 	=  Config.DEV_MODE ; // minimize script fetching and optimize performance
	Config.CACHE_USING_SOURCE_CODE 				=  Config.DEV_MODE ; // optimize performance by caching bytecode

	// Use the following to override some of the default error-handling settings: 
//	Config.UNCAUGHT_ERRORS_ARE_RESPONSE_ERRORS = true;
//	Config.INCLUDE_ERRORS_ARE_RESPONSE_ERRORS = true;
//	Config.RESPONSE_ERROR_PAGE = "resource:///content/responseError.html"; 
//	Config.FATAL_ERROR_PAGE = "resource:///content/fatalError.html";
//	Config.CALLBACK_ERROR_MESSAGE = "Error on server during callback - further information has been logged";
//	Config.CALLBACK_FATAL_ERROR_MESSAGE = "Callback could not be processed due to server error - further information has been logged";
//	Config.SERVICE_ERROR_MESSAGE = "Error on server during service call - further information has been logged";
//	Config.SERVICE_FATAL_ERROR_MESSAGE = "Service call could not be processed due to server error - further information has been logged";

	// If you'd like by default to NOT process programatically-added onload listeners server-side,
	// set Config.ONLOAD_ENABLED to false. For request-by-request control, see Jaxer.onloadEnabled.
//	Config.ONLOAD_ENABLED = false;

	// To include scripts and access files and so on, Jaxer may need to get that content from the web server.
	// If your web server needs to be reached via a different domain/port than the incoming request,
	// uncomment and change the following to replace the protocol://domain:port with this value
//	Config.REWRITE_RELATIVE_URL = "http://192.168.0.1:8082"; // If your web server is reachable at IP 192.168.0.1 on port 8082
//	Config.REWRITE_RELATIVE_URL_REGEX = "^http\\:\\/\\/my.domain.com"; // Optional -- if e.g. you only want to replace URLs that start with this

	// To set any of Mozilla preferences, add properties to Config.MOZ_PREFS.
	// The name of each property should correspond exactly to the Mozilla preference,
	// and the value should be an integer, boolean, or string.
	// To see some of the available options, launch Firefox and browse to about:config
	// For example, here's how to override Jaxer Core's default logging level.
	// Valid levels are: TRACE, DEBUG, INFO, WARN, ERROR, FATAL.
//	Config.MOZ_PREFS["Jaxer.Core.LogLevel"] = "TRACE";
	// You can also turn on logging of the protocol communications between Jaxer and the web server -- 
	// resulting in a very verbose log!
//	Config.MOZ_PREFS["Jaxer.dev.DumpProtocol"] = true;
	// As another example, the following configures the proxy settings to be used 
	// for HTTP requests, say when Jaxer.Web.get(...) is used. 
	// See http://developer.mozilla.org/en/docs/Mozilla_Networking_Preferences#Proxy
//	Config.MOZ_PREFS["network.proxy.type"] = 1; // manual
//	Config.MOZ_PREFS["network.proxy.http"] = "127.0.0.1";
//	Config.MOZ_PREFS["network.proxy.http_port"] = 8888;

	// To have Jaxer use MySQL rather than SQLite for its own needs,
	// use settings similar to the following:
//	Config.FRAMEWORK_DB =
//	{
//		IMPLEMENTATION: "MySQL"
//	,	HOST: "127.0.0.1"
//	,	PORT: 3306
//	,	SOCKET: null  // to use this, set it to '/absolute/path/to/socket/file'
//	,	NAME: "jaxer"
//	,	USER: "root"
//	,	PASS: ""
//	,	CLOSE_AFTER_EXECUTE: false
//	,	CLOSE_AFTER_REQUEST: false
//	};

	// To embed the client part of the Jaxer framework in the web page, 
	// specify its location in EMBEDDED_CLIENT_FRAMEWORK_SRC
	// Alternatively, to have the web server serve its contents, 
	// 1) comment out EMBEDDED_CLIENT_FRAMEWORK_SRC,
	// 2) copy the client framework to somewhere the web server can reach it, and
	// 3) use CLIENT_FRAMEWORK_SRC to specify the src of the <SCRIPT> element that will be inserted into the page
//	Config.EMBEDDED_CLIENT_FRAMEWORK_SRC = "resource:///framework/clientFramework_compressed.js";
//	// Config.CLIENT_FRAMEWORK_SRC = "/aptana/clientFramework.js";
	
	// To add another Content-Type (mime-type) that should be recognized when you ask
	// for Jaxer.request.body and returned to you as a string, byteArray, or stream,
	// modify Config.REQUEST_BODY_AS, for example via:
// 	Config.REQUEST_BODY_AS['application/x-whatever'] = 'string';
// 	Config.REQUEST_BODY_AS['application/x-binary'] = 'byteArray';
// 	Config.REQUEST_BODY_AS['application/x-something'] = 'stream';

	// If you are modifying the Jaxer framework, you may want to use the modified copy, so you
	// can quickly test out your modifications. Here's how (replace <...> with the right values:
//	var depotDir = "file:///C:/Documents and Settings/<username>/My Documents/Aptana Studio/<framework_project>";
//	var depotConfigPath = depotDir + "/config.js";
//	// Get the latest config settings from the local depot copy, not the install directory
//	Jaxer.include(depotConfigPath);
//	// But the depot's config will point back to the install directory, so override that explicitly
//	Config.FRAMEWORK_DIR = depotDir;

})();