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

Log.init("WARN");

if (typeof Config.USE_CORE_LOG_FILE != "undefined" && Config.USE_CORE_LOG_FILE)
{
	var coreAppender = new Log.CoreAppender("CoreAppender", Log.TRACE, coreTraceMethods, coreTraceBefore, coreTraceAfter);
	Log.addAppender(coreAppender.name, coreAppender);
}
else // use file logging
{
	
	var getService = function(aURL, aInterface){
	    try {
	        // determine how 'aInterface' is passed and handle accordingly
	        switch (typeof(aInterface)) {
	            case "object":
	                return Components.classes[aURL].getService(aInterface);
	                break;
	                
	            case "string":
	                return Components.classes[aURL].getService(JSLib.interfaces[aInterface]);
	                break;
	                
	            default:
	                return Components.classes[aURL].getService();
	                break;
	        }
	    } 
	    catch (e) {
	        throw new Exception(e);
	    }
	}

	var logPath;
	if (typeof Config.LOG_PATH == "string")
	{
		logPath = Config.LOG_PATH;
	}
	else
	{
		var logDir = getService("@mozilla.org/file/directory_service;1", "nsIProperties").get("CurProcD", Components.interfaces.nsIFile).path;
		var logFileName = (typeof Config.LOG_FILE_NAME == "string") ? Config.LOG_FILE_NAME : "jaxerFrameworkLog.txt";
		logPath = Dir.combine(logDir, logFileName);
	}
	
	var fileAppender = new Log.FileAppender("FileAppender", Log.TRACE, logPath);
	Log.addAppender(fileAppender.name, fileAppender);
}

Jaxer.include(Config.FRAMEWORK_DIR + "/configLog.js");
if (typeof Config.LOCAL_CONF_DIR == "string") Jaxer.include(Config.LOCAL_CONF_DIR + "/configLog.js");

if (Log.CLIENT_SIDE_CONSOLE_SUPPORT)
{
	var consoleSupport = function()
	{
		if (typeof Jaxer == "undefined") Jaxer = {};
		function hasConsole() { return window.console && typeof window.console.debug == "function" && typeof window.console.info == "function" && typeof window.console.warn == "function" && typeof window.console.error == "function"; };
		function ModuleLogger(moduleName, defaultLevel)
		{
			this.moduleName = moduleName;
			this.level = (typeof defaultLevel == "number") ? defaultLevel : 2;
			var prefix = "[Jaxer.";
			var suffix = "]: ";
			var levels = {trace: [0, "debug"], debug: [1, "debug"], info: [2, "info"], warn: [3, "warn"], error: [4, "error"], fatal: [5, "error"]};
			var ff = function(l) { return function(msg) { if (hasConsole() && this.level<=levels[l][0]) window.console[levels[l][1]]([prefix, this.moduleName, suffix, msg].join(''))}; };
			for (var l in levels)
			{
				this[l] = ff(l);
				this[l.toUpperCase()] = levels[l][0];
			}
			modules[this.moduleName] = this;
		};
		var modules = {};
		Jaxer.Log = new ModuleLogger("");
		Jaxer.Log.forModule = function forModule(moduleName) { return (typeof modules[moduleName] == "undefined") ? new ModuleLogger(moduleName) : modules[moduleName]; };
	};
	if (!Jaxer.beforeClientFramework) Jaxer.beforeClientFramework = [];
	Jaxer.beforeClientFramework.push({contents: consoleSupport.toSource().replace(/\n/g, " ") + "();", src: null});
}
