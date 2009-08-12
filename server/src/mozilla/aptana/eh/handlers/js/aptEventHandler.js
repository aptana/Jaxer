/* ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 * 
 *  This program is distributed in the hope that it will be useful, but
 *  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 *  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 *  is prohibited.
 * 
 *  You can redistribute and/or modify this program under the terms of the GPL, 
 *  as published by the Free Software Foundation.  You should
 *  have received a copy of the GNU General Public License, Version 3 along
 *  with this program; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *  
 *  Aptana provides a special exception to allow redistribution of this file
 *  with certain other code and certain additional terms
 *  pursuant to Section 7 of the GPL. You may view the exception and these
 *  terms on the web at http://www.aptana.com/legal/gpl/.
 *  
 *  You may view the GPL, and Aptana's exception and additional terms in the file
 *  titled license-jaxer.html in the main distribution folder of this program.
 *  
 *  Any modifications to this file must keep this entire header intact.
 * 
 * ***** END LICENSE BLOCK ***** */

const CC = Components.classes;
const CI = Components.interfaces;
const CR = Components.results;
const CU = Components.utils;

CU.import("resource://gre/modules/XPCOMUtils.jsm");

const nsIPrefBranch = CI.nsIPrefBranch;
const nsICommandLine = CI.nsICommandLine;

const aptIEventRequestStart = CI.aptIEventRequestStart;
const aptIEventHTMLParseStart = CI.aptIEventHTMLParseStart;
const aptIEventDocInitialized = CI.aptIEventDocInitialized;
const aptIEventNewHTMLElement = CI.aptIEventNewHTMLElement;
const aptIEventBeforeScriptEvaluated = CI.aptIEventBeforeScriptEvaluated;
const aptIEventAfterScriptEvaluated = CI.aptIEventAfterScriptEvaluated;
const aptIEventLog = CI.aptIEventLog;
const aptIEventHTMLParseComplete = CI.aptIEventHTMLParseComplete;
const aptIEventRequestComplete = CI.aptIEventRequestComplete;

const EVENT_RequestStart = "jaxerEvent.RequestStart";
const EVENT_HTMLParseStart = "jaxerEvent.HTMLParseStart";
const EVENT_DocInitialized = "jaxerEvent.DocInitialized";
const EVENT_NewHTMLElement = "jaxerEvent.NewHTMLElement";
const EVENT_BeforeScriptEvaluated = "jaxerEvent.BeforeScriptEvaluated";
const EVENT_AfterScriptEvaluated = "jaxerEvent.AfterScriptEvaluated";
const EVENT_Log = "jaxerEvent.Log";
const EVENT_HTMLParseComplete = "jaxerEvent.HTMLParseComplete";
const EVENT_RequestComplete = "jaxerEvent.RequestComplete";

const Jaxer = CC['@aptana.com/jaxer/global;1'].getService(CI.aptIJaxerGlobal).getObject();
const Log = CC['@aptana.com/log;1'].getService(CI.aptICoreLog);


function aptEventHandler() {
}

aptEventHandler.prototype = {
	
	// properties required for XPCOM registration:
	classDescription: "Jaxer Event Handler",
	classID:          Components.ID("{A1395C7B-03E0-43e8-8A30-ED73116CE20D}"),
	contractID:       "@aptana.com/jaxer/jaxer-event-handler;1",
	
	_xpcom_categories: [
		{ category: "app-startup", service: true },
		{ category: "command-line-handler", entry: "b-jaxer" },
		{ category: EVENT_RequestStart },
		{ category: EVENT_HTMLParseStart },
		{ category: EVENT_DocInitialized },
		{ category: EVENT_NewHTMLElement },
		{ category: EVENT_BeforeScriptEvaluated },
		{ category: EVENT_AfterScriptEvaluated },
		{ category: EVENT_Log },
		{ category: EVENT_HTMLParseComplete },
		{ category: EVENT_RequestComplete }
	],

	_xpcom_factory: {
		_instance: null,
		createInstance: function (outer, iid) {
			if (outer != null)
				throw CR.NS_ERROR_NO_AGGREGATION;
			if (this._instance == null)
				this._instance = new aptEventHandler();
			return this._instance.QueryInterface(iid);
		}
	},

	// nsISupports
	QueryInterface: XPCOMUtils.generateQI([
						CI.nsIObserver,
						CI.nsICommandLineHandler
	]),
		
	// nsIObserver
	observe: function observe(subject, topic, data) {
		switch(topic) {
		/* Ordered by the frequency of appearance */


		case EVENT_NewHTMLElement:
			// Obtain the aptIEventNewHTMLElement interface for the event
			var nhe = subject.QueryInterface(aptIEventNewHTMLElement);

			// If we successfully obtained the aptIEventNewHTMLElement interface from the incoming object
			if (nhe && Jaxer.frameworkLoaded) {
				Log.trace("aptNewHTMLElement: >>>>>>>>>> start");
				
				if (Jaxer.fatalError) {
					Jaxer.notifyFatal();
					return;
				}
				
				var alreadyInDOM = nhe.afterAddedToDOM; // is false before added to DOM and is true after added to DOM.
				if (!alreadyInDOM) return; // For now, we only need this event after the element has been added to the DOM
				
				// Obtain the DOM Element interface
				var element = nhe.Element;
				if (element) {
					var tagName = element.tagName.toUpperCase();
					Log.trace("aptNewHTMLElement: element is " + tagName);
					nhe.enableBind = true;
					
					// Obtain the DOM interface
					var document = nhe.DOM;
					if (Jaxer.enabled && Jaxer.isHeadCreated && !Jaxer.isHeadInitialized) {
						Jaxer.isHeadInitialized = true;
						Jaxer.CoreEvents.fire(Jaxer.CoreEvents.INIT_HEAD, nhe, document);
					}
					
					switch (tagName) {
					case "SCRIPT":
						if (Jaxer.enabled) {
							nhe.enableScriptEvaluation = Jaxer.CoreEvents.isScriptEvaluateEnabled(element) ? 1 : 0; // Can later be changed by the CoreEvents.NEW_ELEMENT handler
						} else {
							nhe.enableScriptEvaluation = Jaxer.evaluateScriptsIfDisabled;
						}
						break;

					case "HEAD":
						Jaxer.isHeadCreated = true;
						break;
					}
					
					if (Jaxer.enabled) {
						Jaxer.CoreEvents.fire(Jaxer.CoreEvents.NEW_ELEMENT, nhe, document, element);
					}
				}
				Log.trace("aptNewHTMLElement: <<<<<<<<<< end");
			}
			break;


		case EVENT_BeforeScriptEvaluated:
			// Obtain the aptIEventBeforeScriptEvaluated interface for the event
			var bse = subject.QueryInterface(aptIEventBeforeScriptEvaluated);

			// If we successfully obtained the aptIEventBeforeScriptEvaluated interface from the incoming object
			if (bse && (!Jaxer.enabled || Jaxer.frameworkLoaded)) {
				// Obtain the DOM Element interface
				var element = bse.Element;
				if (element) {
					if (Jaxer.enabled) {
						if (Jaxer.fatalError) {
							Jaxer.notifyFatal();
							return;
						}
						// Set the enableScriptEvaluation property to 1 or 0 depending on whether or not
						// you want the contents of this <script> element executed
						bse.enableScriptEvaluation = Jaxer.CoreEvents.isScriptEvaluateEnabled(element) ? 1 : 0;
						if (bse.enableScriptEvaluation) {
							Jaxer.pageWindow.Jaxer.__newFunctions = [];
						}
					} else {
						bse.enableScriptEvaluation = Jaxer.evaluateScriptsIfDisabled;
					}
					Log.trace("aptBeforeScriptEvaluated: setting enableScriptEvaluation = " + bse.enableScriptEvaluation);
				}
			}
			break;


		case EVENT_AfterScriptEvaluated:
			// Obtain the aptIEventAfterScriptEvaluated interface for the event
			var ase = subject.QueryInterface(aptIEventAfterScriptEvaluated);

			// If we successfully obtained the aptIEventAfterScriptEvaluated interface from the incoming object
			if (ase && (!Jaxer.enabled || Jaxer.frameworkLoaded)) {
				// Obtain the DOM Element interface
				var element = ase.Element;
				if (element) {
					/* TODO: send event to framework here */
					delete Jaxer.pageWindow.Jaxer.__newFunctions;
				}
				Log.trace("aptAfterScriptEvaluated");
			}
			break;


		case EVENT_Log:
			// Obtain the aptIEventLog interface for the event
			var le = subject.QueryInterface(aptIEventLog);

			// If we successfully obtained the aptIEventLog interface from the incoming object
			if (le && Jaxer.enabled) {
  				//Log.trace("aptLog: >>>>>>>>>> start");
				// CAUTION! -- For (now) obvious reasons, do NOT put code in here that would cause any logging ;-)

				try {
					if (Jaxer.response && Jaxer.Config && Jaxer.Config.UNCAUGHT_ERRORS_ARE_RESPONSE_ERRORS) {
						var msg = le.message;
						var matches = msg.match(/^\[(JavaScript Error\:[\s\S]*)\]\s*/);
						if (matches) {
							Jaxer.response.notifyError(matches[1], true);
						}
					}
				} 
				catch (e) {
					// Fail silently to avoid re-triggering log
				}
				//Log.trace("aptLog: <<<<<<<<<< end");
			}
			break;


		case EVENT_RequestStart:
			// Obtain the aptIEventRequestStart interface for the event
			var rs = subject.QueryInterface(aptIEventRequestStart);

			var req = rs.Request;
			var method = req.method.toUpperCase();
			if (method == "OPTIONS") {
				handleOPTIONS(req, rs.Response);
			}

			// If we successfully obtained the aptIEventRequestStart interface from the incoming object
			if (rs && Jaxer.enabled) {
				Log.trace("aptRequestStart: >>>>>>>>>> start");
				
				if (Jaxer.fatalError) {
					Jaxer.notifyFatal();
					return;
				}
				
				Jaxer.request = null;
				if (!frameworkLoadStarted) {
					try {
						frameworkLoadStarted = true;
						loadFramework();
						if (Jaxer.fatalError) {
							Jaxer.fatalError =  "Error while loading Jaxer framework: " + Jaxer.fatalError;
						}
						else if (!Jaxer.frameworkLoaded) {
							Jaxer.fatalError = "Jaxer framework load did not reach the end";
						}
					} 
					catch (e) {
						Jaxer.fatalError = "Error loading framework: " + e;
					}
					if (Jaxer.fatalError) {
						Jaxer.fatalError = '' + Jaxer.fatalError.toString() + '; last framework fragment loading: ' + Jaxer.lastLoadedFragment;
						Jaxer.notifyFatal();
						return;
					}
				} else {
					/** TODO: do we still need this?
					var setConfigParams = readSetConfig(rs);
					if (typeof setConfigParams['LOCAL_CONF_DIR'] == 'string') delete setConfigParams['LOCAL_CONF_DIR'].value; // Just to minimize confusion, in case you inquire which local config was loaded
					setSetConfig(setConfigParams);
					*/
				}
				if (!serverStarted) {
					serverStarted = true;
					Log.debug("---------------- Server starting -------------");
					Jaxer.isServerStartedFired = true;
					Jaxer.CoreEvents.fire(Jaxer.CoreEvents.SERVER_START, rs);
				}

				Jaxer.isHeadInitialized = false;
				Jaxer.isHeadCreated = false;
				Jaxer.CoreEvents.fire(Jaxer.CoreEvents.REQUEST_START, rs);

				Log.trace("aptRequestStart: <<<<<<<<<< end");
			}
			break;


		case EVENT_HTMLParseStart:
			// Obtain the aptIEventHTMLParseStart interface for the event
			var htmlps = subject.QueryInterface(aptIEventHTMLParseStart);

			// If we successfully obtained the aptIEventHTMLParseStart interface from the incoming object
			if (htmlps && Jaxer.enabled && Jaxer.frameworkLoaded) {
				Log.trace("aptHTMLParseStart: >>>>>>>>>> start");
				Jaxer.CoreEvents.fire(Jaxer.CoreEvents.PARSE_START, htmlps);
				Log.trace("aptHTMLParseStart: <<<<<<<<<< end");
			}
			break;


		case EVENT_DocInitialized:
			// Obtain the aptIEventDocInitialized interface for the event
			var di = subject.QueryInterface(aptIEventDocInitialized);
			
			// If we successfully obtained the aptIEventDocInitialized interface from the incoming object
			if (di && Jaxer.enabled) {
				Log.trace("aptDocInitialized: >>>>>>>>>> start");
				Log.trace("aptDocInitialized: <<<<<<<<<< end");
			}
			break;


		case EVENT_HTMLParseComplete:
			// Obtain the aptIEventHTMLParseComplete interface for the event
			var htmlpc = subject.QueryInterface(aptIEventHTMLParseComplete);

			// If we successfully obtained the aptIEventHTMLParseComplete interface from the incoming object
			// NOTE: Let fatalErrors and request errors get through so PARSE_COMPLETE can decide whether to show them
			if (htmlpc && Jaxer.enabled && Jaxer.frameworkLoaded) {
				Log.trace("HTMLParseComplete <<<<<< START");
				
				// Fetch the DOM interface from the incoming event data
				var document = htmlpc.DOM;	
				// if we got the DOM interface
				if (document && Jaxer.CoreEvents) {
					Jaxer.CoreEvents.fire(Jaxer.CoreEvents.PARSE_COMPLETE, htmlpc, document);
				}
				Log.trace("HTMLParseComplete <<<<<< END");
			}
			break;


		case EVENT_RequestComplete:
			// Obtain the aptIEventRequestComplete interface for the event
			var rc = subject.QueryInterface(aptIEventRequestComplete);
			
			// If we successfully obtained the aptIEventRequestComplete interface from the incoming object
			if (rc && Jaxer.enabled && Jaxer.frameworkLoaded) {
				Log.trace("aptRequestComplete: >>>>>>>>>> start");
				Jaxer.CoreEvents.fire(Jaxer.CoreEvents.REQUEST_COMPLETE, rc);
				Log.trace("aptRequestComplete <<<<<< end");
			}
			break;


		case "app-startup":
			startup();
			break;
		}
	},

	// nsICommandLineHandler
	handle : function(cmdLine) {
		/* Special parameters handling first */
		var consoleMode = cmdLine.handleFlag('console', true);
		if (consoleMode) {
			Log.trace("Command line argument: console");
		}

		var commandPort = cmdLine.handleFlagWithParam('commandport', true);
		if (commandPort) {
			Log.trace("Command line argument: commandport="+commandPort);
			Jaxer.Config.COMMAND_LINE_PARAMS['commandport'] = +commandPort;
		}

		platformConfPath = cmdLine.handleFlagWithParam('PLATFORM_CONF_PATH', true);
		if (platformConfPath) {
			Log.trace("Command line argument: PLATFORM_CONF_PATH="+platformConfPath);
		}
		localConfDir = cmdLine.handleFlagWithParam('LOCAL_CONF_DIR', true);
		if (localConfDir) {
			Log.trace("Command line argument: LOCAL_CONF_DIR="+localConfDir);
		}

		commandLineParams = {};
		for (var i = 0; i < cmdLine.length; ) {
			var arg = cmdLine.getArgument(i);
			if (arg.charAt(0) == '-') {
				arg = arg.substr(1);
				var value = cmdLine.handleFlagWithParam(arg, true);
				if (value) {
					Log.trace("Command line argument: "+arg+"="+value);
					commandLineParams[arg] = value;
					continue;
				}
			}
			++i;
		}

		var loadOnStart = false;
		try {
			loadOnStart = CC["@mozilla.org/preferences-service;1"].getService(CI.nsIPrefBranch2).getBoolPref("Jaxer.loadFrameworkOnStart");	
		} catch (ignore) {
		}
		if (cmdLine.state == nsICommandLine.STATE_INITIAL_LAUNCH && (loadOnStart || consoleMode))
		{
			Log.info("Start console mode");
			try {
				loadFramework();
				if (Jaxer.fatalError) {
					Jaxer.fatalError =  "Error while loading Jaxer framework: " + Jaxer.fatalError;
				}
				else if (!Jaxer.frameworkLoaded) {
					Jaxer.fatalError = "Jaxer framework load did not reach the end";
				}
			} 
			catch (e) {
				Jaxer.fatalError = "Error loading framework: " + e;
			}
			if (Jaxer.fatalError) {
				Jaxer.fatalError = '' + Jaxer.fatalError.toString() + '; last framework fragment loading: ' + Jaxer.lastLoadedFragment;
				Jaxer.notifyFatal();
				return;
			}
		} else {
			setCommandLineParams(commandLineParams);
		}

	}

};

function NSGetModule(aCompMgr, aFileSpec) {
  return XPCOMUtils.generateModule([aptEventHandler]);
}

function LOG(text) {
	var consoleService = CC["@mozilla.org/consoleservice;1"].getService(CI.nsIConsoleService);
    consoleService.logStringMessage(text+"");
}

/**------------- Jaxer object initialization ----------------**/

const frameworkGlobal = {};

var executableFolderUrl;
var frameworkLoadStarted = false;
var serverStarted = false;
var platformConfPath, localConfDir, commandLineParams;

function startup() {
	Jaxer.enabled = true; // Set this to false to turn off all Jaxer framework handling
	Jaxer.evaluateScriptsIfDisabled = false;
	Jaxer.fatalError = null; // If this is ever set to non-null, not much should be allowed to happen thereafter
	Jaxer.fatalErrorLogged = false;
	Jaxer.fatalErrorPage = '<html><head><title>Server Error</title></head><body>Server error: further information has been logged.</body></html>';
	Jaxer.Config = {};
	Jaxer.Config.CONFIG_PATH = "resource:///framework/config.js"; // This is the only Config value that cannot be initialized and should not be changed in config.js
	Jaxer.frameworkLoaded = false;
	Jaxer.lastLoadedFragment = "(none)";

	Jaxer.beforeClientFramework = [];	// Array of Objects of the form {contents: "...", src: "..."} to insert as script elements before the client framework
	Jaxer.afterClientFramework = [];	// Array of Objects of the form {contents: "...", src: "..."} to insert as script elements after the client framework

	Jaxer.notifyFatal = notifyFatal;
	Jaxer.include = include;

	Jaxer.isOnServer = true;
	Jaxer.isCallback = false;   // This will be set just as soon as we have a Request and can see its uri
	Jaxer.lastScriptRunat = "";

	Jaxer.Config.COMMAND_LINE_PARAMS = {};

	Jaxer.corePreferences = {};
	var prefs = CC["@mozilla.org/preferences-service;1"].getService(CI.nsIPrefService).getBranch("Jaxer.");	
	var prefNames = prefs.getChildList("", {});
	for (var i = 0; i < prefNames.length; ++i)
	{
		var prefName = prefNames[i];
		var value;
		switch (prefs.getPrefType(prefName))
		{
			case nsIPrefBranch.PREF_STRING: // string
				value = prefs.getCharPref(prefName);
				break;
			case nsIPrefBranch.PREF_INT: // int
				value = prefs.getIntPref(prefName);
				break;
			case nsIPrefBranch.PREF_BOOL: // bool
				value = prefs.getBoolPref(prefName);
				break;
		}
		Jaxer.corePreferences[prefNames[i]] = value;
	}

	var executableFolder = CC["@mozilla.org/file/directory_service;1"].getService(CI.nsIProperties).get("CurProcD", CI.nsIFile);
	var ioService = CC["@mozilla.org/network/io-service;1"].getService(CI.nsIIOService);
	var fileHandler = ioService.getProtocolHandler("file").QueryInterface(CI.nsIFileProtocolHandler);
	executableFolderUrl = fileHandler.getURLSpecFromFile(executableFolder);
}


function notifyFatal(error)
{
	if (!Jaxer.fatalError) // This is the first notification
	{
		Jaxer.fatalError = error || "Unspecified error";
		if (Jaxer.DB && Jaxer.DB.closeAllConnections)
		{
			Jaxer.DB.closeAllConnections();
		}
	}
	if (!Jaxer.fatalErrorLogged)
	{
		Jaxer.fatalErrorLogged = true;
		Log.fatal("Fatal error - Jaxer will need to be restarted after the error is corrected: " + Jaxer.fatalError);
	}
}


function include(url, global) 
{
	Log.trace("Including: " + url);
	if (!url) return;
	var rv, resolvedUrl;
	if (url.match(/file\:\/\//i)) 
	{
		resolvedUrl = url;
	}
	else
	{
		resolvedUrl = url.replace(/resource\:\/\/(\/)?/i, ''); // The default for a URL is relative to the current executable anyway
		resolvedUrl = executableFolderUrl + resolvedUrl.replace(/^\/+/, '');
	}
	if (!global) 
	{
		global = frameworkGlobal;
	}
	
	try 
	{
		CC["@mozilla.org/moz/jssubscript-loader;1"].getService(CI.mozIJSSubScriptLoader).loadSubScript(resolvedUrl, global);
		rv = CR.NS_OK;
	}
	catch (e) {
		rv = - CR.NS_ERROR_INVALID_ARG;
	}
	return rv;
}

function setCommandLineParams(params)
{
	for (var name in params)
	{
		var value = params[name];
		Jaxer.Config.COMMAND_LINE_PARAMS[name] = value;

		var typeDescription;
		if (value == "null") 
		{
			value = null;
			typeDescription = 'null';
		} 
		else 
		{
			var numValue = +value; // will be a valid number or NaN
			if (!isNaN(numValue)) 
			{
				value = numValue;
				typeDescription = "number";
			} 
			else 
			{
				typeDescription = "string";
			}
		}

		var propertyNames = name.split(/\./);
		var objToSetOn = Jaxer.Config;
		for (var j = 0; j < propertyNames.length - 1; ++j)
		{
			var propertyName = propertyNames[j];
			if (typeof objToSetOn[propertyName] == "undefined")
			{
				objToSetOn[propertyName] = {};
			}
			objToSetOn = objToSetOn[propertyName];
		}
		try
		{
			Log.trace("Setting Jaxer.Config." + name + " = " + value + " (" + typeDescription + ")");
			objToSetOn[propertyNames[propertyNames.length - 1]] = value;
		}
		catch (e)
		{
			Log.warning("Error setting Jaxer.Config." + name + " = " + value + " (" + typeDescription + "): " + e + " (ignoring)");
		}
	}
}

function loadFramework()
{
	frameworkGlobal.Config = Jaxer.Config;

	frameworkGlobal.coreTraceMethods = 
	{
		FATAL: function FATAL(text) {Log.fatal(text)},
		ERROR: function ERROR(text) {Log.error(text)},
		WARN:  function WARN(text) {Log.warning(text)},
		INFO:  function INFO(text) {Log.info(text)},
		DEBUG: function DEBUG(text) {Log.debug(text)},
		TRACE: function TRACE(text) {Log.trace(text)}
	};
	frameworkGlobal.coreTraceBefore = function coreTraceBefore() { Log.forceTrace = true; }
	frameworkGlobal.coreTraceAfter = function coreTraceAfter() { Log.forceTrace = false; }

	Jaxer.include(Jaxer.Config.CONFIG_PATH);

	for (var key in commandLineParams) {
		Jaxer.Config.COMMAND_LINE_PARAMS[key] = commandLineParams[key];
	}
	if (platformConfPath)
	{
		Log.trace("Including Jaxer.Config.PLATFORM_CONF_PATH.value = " + platformConfPath + " (string)");
		Jaxer.include(platformConfPath);
	}
	if (localConfDir)
	{
		Log.trace("Setting Jaxer.Config.LOCAL_CONF_DIR = " + localConfDir + " (string)");
		Jaxer.Config.LOCAL_CONF_DIR = localConfDir;
	}
	Log.trace("Including Jaxer.Config.LOCAL_CONF_DIR + '/config.js' = " + Jaxer.Config.LOCAL_CONF_DIR + "/config.js")
	Jaxer.include(Jaxer.Config.LOCAL_CONF_DIR + "/config.js");  // This is optional, for overrides
	setCommandLineParams(commandLineParams);
	Log.trace("Before loading framework - Jaxer.Config.FRAMEWORK_DIR = " + Jaxer.Config.FRAMEWORK_DIR);
	Log.trace("Before loading framework - Jaxer.Config.LOCAL_CONF_DIR = " + Jaxer.Config.LOCAL_CONF_DIR);
	
	if (Jaxer.Config.MOZ_PREFS)
	{
		var prefs = CC["@mozilla.org/preferences-service;1"].getService(CI.nsIPrefBranch);
		for (var pref in Jaxer.Config.MOZ_PREFS)
		{
			var value = Jaxer.Config.MOZ_PREFS[pref];
			if (value == undefined || value == null)
			{
				prefs.clearUserPref(pref);
			}
			else
			{
				switch (typeof value)
				{
					case "number":
						prefs.setIntPref(pref, value);
						break;
					case "boolean":
						prefs.setBoolPref(pref, value);
						break;
					default:
						prefs.setCharPref(pref, String(value));
				}
			}
		}
	}
	
	var frameworkPath = Jaxer.Config.FRAMEWORK_DIR + "/" + Jaxer.Config.FRAMEWORK_FILE_NAME;
	var libraryPaths = [];
	if (Jaxer.Config.LIBRARY_FILE_NAMES) 
	{
		var libraries = Jaxer.Config.LIBRARY_FILE_NAMES.split(/[\s\,\;]+/);
		for (var i = 0; i < libraries.length; i++) 
		{
			if (libraries[i]) 
			{
				libraryPaths.push(Jaxer.Config.FRAMEWORK_DIR + "/" + libraries[i]);
			}
		}
	}
	
	// Libraries should not depend on serverFramework, but serverFramework is likely to depend on the libraries
	// Any libraries that depend on serverFramework should instead be included as extensions
	for (var i=0; i<libraryPaths.length; i++)
	{
		Jaxer.include(libraryPaths[i]);
	}
	Jaxer.include(frameworkPath);
	if (Jaxer.Config.ONFRAMEWORKLOAD_PATH)
		Jaxer.include(Jaxer.Config.ONFRAMEWORKLOAD_PATH);
		
}

function handleOPTIONS(req, resp)
{
	try {
		var headerCount = req.GetHeaderCount();
		for (var i = 0; i < headerCount; ++i) {
			var name = req.GetHeaderName(i);
			if (name == "Compliance") {
				var value = req.GetValueByOrd(i);
				if (value.match(/^server=jaxer$/)) {
					resp.addHeader(name, value, false);
				}
			}
		}
	} catch (e) {
		Log.warning("Error handling OPTIONS request: " + e);
	}
}
