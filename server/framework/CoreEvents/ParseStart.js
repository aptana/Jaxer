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

var log = Log.forModule("ParseStart");

CoreEvents.clearHandlers(CoreEvents.PARSE_START); // Clear this out -- we're the first and only default handler

try
{
	
	CoreEvents.addHandler(CoreEvents.PARSE_START, function onParseStart(evt)
	{
		
		/**
		 * Contains information and methods about the current request
		 * 
		 * @alias Jaxer.request
		 * @property {Jaxer.Request}
		 */
		Jaxer.request = frameworkGlobal.request = new Jaxer.Request(evt);

		/**
		 * Contains information and methods about how to respond to the current
		 * request
		 * 
		 * @alias Jaxer.response
		 * @property {Jaxer.Response}
		 */
		Jaxer.response = frameworkGlobal.response = new Jaxer.Response(evt);
		
		var requestErrors = [];
		if (Jaxer.request.constructorErrors.length > 0)
		{
			requestErrors = Jaxer.request.constructorErrors;
		}
		if (Jaxer.request.parsedUrlError)
		{
			requestErrors.push(Jaxer.request.parsedUrlError);
		}
		
		if (requestErrors.length > 0) 
		{
			Jaxer.response.notifyError(new Error(requestErrors.join("; ")));
		}
		else 
		{

			// Determine application from request
			var app;
			var URL_TEST_PROPERTY = "urlTest";
			var parsedUrl = Jaxer.request.parsedUrl;
			if (log.getLevel() == Log.TRACE) log.trace("Matching app using parsedUrl='" + uneval(parsedUrl) + "'");
			for (var iApp=0, appsLen = Config.apps.length; iApp<appsLen; iApp++)
			{
				var candidate = Config.apps[iApp];
				var matched = false;
				if (candidate && candidate[URL_TEST_PROPERTY])
				{
					switch (typeof candidate[URL_TEST_PROPERTY])
					{
						case "function":
							matched = candidate[URL_TEST_PROPERTY](parsedUrl);
							break;
						case "string":
							matched = (parsedUrl.path.indexOf(candidate[URL_TEST_PROPERTY]) == 0);
							break;
						case "object":
							if (candidate[URL_TEST_PROPERTY] instanceof candidate[URL_TEST_PROPERTY].__parent__.RegExp)
							{
								matched = candidate[URL_TEST_PROPERTY].test(parsedUrl.path);
							}
							break;
					}
				}
				if (matched)
				{
					app = candidate;
					break;
				}
			}
			Config.DEFAULT_APP.init();
			app = app ? new App(app) : Config.DEFAULT_APP;
			Jaxer.request.app = app;
			if (log.getLevel() == Log.TRACE) log.trace("Using app '" + app.NAME + "': urlTest = " + app[URL_TEST_PROPERTY]);
			
			// Determine app and page keys from request
			Jaxer.request.appKey = app.APP_KEY;
			if (!Jaxer.request.appKey) throw "Could not calculate an application key from url: " + Jaxer.request.parsedUrl.url;
			Jaxer.request.pageKey = app.PAGE_KEY;
			if (!Jaxer.request.pageKey) throw "Could not calculate a page key from url: " + Jaxer.request.parsedUrl.url;
	
			// Initialize containers for storing/retrieving state
			Container.init(Jaxer.request.appKey, Jaxer.request.pageKey);
			DB.setDefault(app.NAME, app.DB);

		}
		
	});
	
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}


Log.trace("*** ParseStart.js loaded");

})();
