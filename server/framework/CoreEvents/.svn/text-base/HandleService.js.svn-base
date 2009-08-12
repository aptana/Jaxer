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
	
var log = Log.forModule("HandleService");

CoreEvents.clearHandlers(CoreEvents.HANDLE_SERVICE); // Clear this out -- we're the first and only default handler

// add the framework's HANDLE_SERVICE event handler
CoreEvents.addHandler(CoreEvents.HANDLE_SERVICE, function onHandleService(evt, doc)
{

	log.trace("=== Service processing initializing");
	if (log.getLevel() == Log.TRACE) log.trace("Received Jaxer.request: " + uneval(Jaxer.request));	
	
	Jaxer.response.allow(true);
	var exception = null;
	
	try
	{

		// We might need the above parameters to know how to return the result, even if there's a fatal error
		if (Jaxer.fatalError)
		{
			throw "Fatal Jaxer error: " + Jaxer.fatalError;
		}
		
		// We might need the above parameters to know how to return the result, even if there's a response error
		if (Jaxer.response.hasError())
		{
			throw "Jaxer response error (before callback itself was called): " + Jaxer.response.getError();
		}
		
		// Jaxer.request.app should be defined at this point, with a HANDLER property that points to the JS file to load (if any)
		var src = Jaxer.request.app.callHandler();
		if (typeof src == 'string') 
		{
			src = Util.Url.ensureFileProtocol(src);
			var global = Jaxer.pageWindow;
			var runat = "";
			var useCache = true;
			var forceCacheRefresh = Config.RELOAD_AUTOLOADS_EVERY_PAGE_REQUEST;
			var dontSetRunat = true;
			Includer.load(src, global, runat, useCache, forceCacheRefresh, dontSetRunat);
		}
		
	}
	catch(e)
	{
		exception = new Exception(e); // This will log it 
	}
	// Finally, in any case, package the results
	finally
	{
		
		if (exception)
		{
			if (!Config.DISPLAY_ERRORS) 
			{
				log.debug("An error occurred during service handling, and Config.DISPLAY_ERRORS is false, so returning a generic error message");
				exception = 
				{
					name: 'Error',
					message: Jaxer.fatalError ? Config.SERVICE_FATAL_ERROR_MESSAGE : Config.SERVICE_ERROR_MESSAGE
				};
				exception.description = exception.message;
			}
			var content = {};
			content.exception = exception;
			Jaxer.response.setContents(Serialization.toJSONString(content, { as: Serialization.JAXER_METHOD }));
			Jaxer.response.setStatus(500); // 500: Internal server error
		}

		if (log.getLevel() == Log.TRACE) log.trace("Jaxer.response.contents = " + Jaxer.response.getContents());
		if (log.getLevel() == Log.DEBUG)
		{
			log.debug("Jaxer.response.contents (first 200 chars) = " + Jaxer.response.getContents().substr(0, 200));
		}
		
		Container.persistAll();
		log.debug("Containers persisted");

		Jaxer.DB.closeAllConnections(true);
		Jaxer.response.persistHeaders();

		log.trace("=== Service processing complete");
	}
	
});

Log.trace("*** HandleService.js loaded");

})();
