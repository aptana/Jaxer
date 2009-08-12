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

var log = Log.forModule("ServerStart");
	
CoreEvents.clearHandlers(CoreEvents.SERVER_START); // Clear this out -- we're the first and only default handler

try
{
	CoreEvents.addHandler(CoreEvents.SERVER_START, function onServerStart(evt)
	{

		// Note that error handling configurations should be done first, so they can handle any errors in the remaining code:

		/**
		 * If Jaxer.Config.RESPONSE_ERROR_PAGE has been set, it's
		 * assumed to be the path to a file containing the error page
		 * to return when an error has occurred during the response.
		 * This file is read once at server startup and kept in memory,
		 * to be returned as required. 
		 * If Config.RESPONSE_ERROR_PAGE is not set, a default HTML
		 * string is used.
		 * You can change the behavior
		 * of the framework when it encounters errors serving requests
		 * using Jaxer.Config.DISPLAY_ERRORS.
		 * 
		 * @alias Jaxer.responseErrorPage
		 * @property {String}
		 */
		if (Config.RESPONSE_ERROR_PAGE) // used for errors encountered while processing a specific request to generate a response
		{
			Jaxer.responseErrorPage = File.read(Config.RESPONSE_ERROR_PAGE);
		}
		if (Jaxer.responseErrorPage == null)
		{
			Jaxer.responseErrorPage = '<html><head><title>Error</title></head><body><h2>Error processing your request</h2> This server is configured to not display errors on the browser. Further information may have been logged on the server.</body></html>';
		}
		
		/**
		 * If Jaxer.Config.FATAL_ERROR_PAGE has been set, it's
		 * assumed to be the path to a file containing the error page
		 * to return when a fatal error has occurred.
		 * This file is read once at server startup and kept in memory,
		 * to be returned as required. 
		 * If Config.FATAL_ERROR_PAGE is not set, a default HTML
		 * string is used.
		 * You can change the behavior
		 * of the framework when it encounters fatal errors
		 * using Jaxer.Config.DISPLAY_ERRORS.
		 * 
		 * @alias Jaxer.responseErrorPage
		 * @property {String}
		 */
		if (Config.FATAL_ERROR_PAGE) // used for fatal errors that stop the server from effectively processing any more requests
		{
			try
			{
				Jaxer.fatalErrorPage = File.read(Config.FATAL_ERROR_PAGE);
			}
			catch (e)
			{
				Jaxer.fatalErrorPage = null;
			}
		}
		if (Jaxer.fatalErrorPage == null)
		{
			Jaxer.fatalErrorPage = '<html><head><title>Server Error</title></head><body><h2>Server Error</h2> This server is configured to not display errors on the browser. Further information may have been logged on the server.</body></html>';
		}

		if (!Config.FRAMEWORK_DB)
		{
			var error = new Exception("The database connection parameters for the Jaxer framework have not been specified", log);
			Jaxer.notifyFatal(error);
			throw error;
		}

		try
		{
			loadConfigApps(Config.FRAMEWORK_DIR);
			if (typeof Config.LOCAL_CONF_DIR == "string") 
			{
				loadConfigApps(Config.LOCAL_CONF_DIR);
			}
			if (!(Config.DEFAULT_APP instanceof App)) Config.DEFAULT_APP = new App(Config.DEFAULT_APP);
			// Sanity check on the apps:
			if (!Config.apps || !(Config.apps instanceof Config.apps.__parent__.Array)) throw "Config.apps is not defined or is not an Array; please check all configApps.js files";
			Config.apps.forEach(function (app, index)
			{
				if (!app) throw "The app at position " + index + " of Config.apps is invalid; please check all configApps.js files";
				switch (typeof app.name)
				{
					case "string":
						var appName = String(app.name);
						if (appName == Config.DEFAULT_APP.name) throw "Cannot use application name of '" + appName + "' in Config.apps (it's reserved for the default); please check all configApps.js files";
						if (Config.appsByName[appName]) throw "Duplicate name '" + appName + "' found in Config.apps; please check all configApps.js files";
						Config.appsByName[appName] = new App(app);
						break;
					case "function": // The app will be determined dynamically when requests come in
						break;
					default:
						throw "The app at position " + index + " of Config.apps has no name or name function; please check all configApps.js files";
				}
			});
			
			DB.init();
					
			log.trace("About to create callbacks schema");
			createCallbacksSchema();
			
			log.trace("About to create container schema");
			Container.DBPersistor.createSchema();
			
			/**
			 * If Jaxer.Config.EMBEDDED_CLIENT_FRAMEWORK_SRC has been set, it's
			 * assumed to be the path to the embeddable (and usually compressed)
			 * version of the Jaxer client-side framework. This file is read
			 * once at server startup and kept in memory, to be embedded in
			 * pages that may require it. You can change this behavior
			 * using Jaxer.Response.prototype.setClientFramework.
			 * 
			 * @alias Jaxer.embeddedClientFramework
			 * @property {String}
			 */
			if (Config.EMBEDDED_CLIENT_FRAMEWORK_SRC) // embed the client framework in the page
			{
				log.trace("Reading embedded client framework source from: " + Config.EMBEDDED_CLIENT_FRAMEWORK_SRC);
				Jaxer.embeddedClientFramework = File.read(Config.EMBEDDED_CLIENT_FRAMEWORK_SRC);; 
				log.trace("Embedded client framework is " + Jaxer.embeddedClientFramework.length + " characters long");
			}
			else
			{
				Jaxer.embeddedClientFramework = null; // will need to get the client framework from the web server
			}
						
			Jaxer.loadAllExtensions();
		
		}
		catch (e)
		{
			var error = new Exception(e, log);
			Jaxer.notifyFatal(error);
			throw error;
		}
		
	});
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

/**
 * Creates the database schema needed by the framework.
 * 
 * @private
 */
function createCallbacksSchema()
{
	sql = "CREATE TABLE IF NOT EXISTS callback_page (" +
		" id INTEGER PRIMARY KEY AUTO_INCREMENT," +
		" crc32 INT(11) DEFAULT NULL," +
		" name VARCHAR(255) DEFAULT NULL," +
		" document_root VARCHAR(255) DEFAULT NULL," +
		" page_file VARCHAR(255) DEFAULT NULL," +
		" value LONGTEXT," + 
		" creation_datetime DATETIME DEFAULT NULL," +
		" access_datetime DATETIME DEFAULT NULL," +
		" access_count INT(11) DEFAULT 0" +
		")";
	DB.frameworkExecute(sql);
}

/**
 * Loads all files named like configApps.js found in the given folder 
 * 
 * @param {String} folderSpec
 * 		The folder to use, either as an absolute URL or relative to the executable folder
 * 
 * @private
 */
function loadConfigApps(folderSpec)
{
	var defaultRootUrl = Dir.pathToUrl(System.executableFolder);
	var absConfUrl = Web.resolve(folderSpec, defaultRootUrl);
	var confPathOnDisk = Dir.urlToPath(absConfUrl);
	var confFiles = Dir.grep(confPathOnDisk, 
	{
		pattern: '\\bconfigApps.js$',
		recursive: false
	});
	confFiles.sort(function(f1, f2)
	{
		if (f1 < f2) 
			return -1;
		if (f2 < f1) 
			return 1;
		return 0;
	}); // will load files in alphabetic order
	confFiles.forEach(function loadConfigApp(confFile)
	{
		var confFileUrl = Jaxer.Dir.pathToUrl(confFile.path);
		Jaxer.include(confFileUrl);
	});
	
}

Log.trace("*** ServerStart.js loaded");

})();
