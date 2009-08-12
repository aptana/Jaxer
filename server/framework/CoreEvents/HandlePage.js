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

var log = Log.forModule("HandlePage");

CoreEvents.clearHandlers(CoreEvents.HANDLE_PAGE); // Clear this out -- we're the first and only default handler

try
{
	CoreEvents.addHandler(CoreEvents.HANDLE_PAGE, function onHandlePage(evt, doc)
	{
		
		// When modifying this, don't forget to think about premature exits from page processing
		// (Jaxer.request.exit(), e.g.) which result in this not being called
		
		if (checkErrors(doc)) 
		{
			// set response headers even though we'll be returning an error of some sort
			Jaxer.response.setNoCacheHeaders();
			Jaxer.response.exposeJaxer();
		}
		else 
		{
			try 
			{
				if (!Jaxer.response.earlyExit) 
				{
				
					log.trace("Processing onserverload with doc = " + doc);
					processOnServerLoad(doc);
					
					// process script blocks
					log.debug("Script processing starting");
					var scriptsInfo = CallbackManager.processCallbacks(doc);
					log.debug("Script processing complete");
					
					processAfterCallbackProcessing(doc);
					
					CallbackManager.cleanup(scriptsInfo);
					
				}

				// persist containers
				Container.persistAll(doc);
				log.trace("Containers persisted");
			} 
			catch (e) 
			{
				Jaxer.response.notifyError(e); // No need to throw again -- we'll handle it in a moment as long as we've done anything in this response
			}
		}
		
		if (Jaxer.response.getDomTouched() || Jaxer.response.getSideEffect()) 
		{
			// set any other response headers on this dynamic, Jaxer-processed contents
			Jaxer.response.setNoCacheHeaders();
			Jaxer.response.exposeJaxer();
		}

		if (!checkErrors(doc) && Jaxer.response.getDomTouched()) 
		{
			try 
			{
				// embed, or emit reference to, the client framework
				// This must come at the end, so Jaxer ends up being defined at the very beginning.
				var head = doc.getElementsByTagName("head")[0];
				if (head && !Jaxer.response.earlyExit)
				{
					var lastInsertedScript;
					if (Jaxer.response.isClientFrameworkOverridden()) 
					{
						var overrideCF = Jaxer.response.getOverriddenClientFramework();
						if (overrideCF.isSrc)
						{
							lastInsertedScript = Util.DOM.insertScriptAtBeginning("", head, { src: overrideCF.value });
						}
						else if (overrideCF.value)
						{
							lastInsertedScript = Util.DOM.insertScriptAtBeginning(overrideCF.value, head);
						}
						else
						{
							lastInsertedScript = null;
						}
					}
					else 
					{
						if (Jaxer.embeddedClientFramework) 
						{
							lastInsertedScript = Util.DOM.insertScriptAtBeginning(Jaxer.embeddedClientFramework, head);
						}
						else 
							if (Config.CLIENT_FRAMEWORK_SRC) 
							{
								var src = Config.CLIENT_FRAMEWORK_SRC.replace(/#.*$/, ''); // strip off fragment, if any
								src += (src.match(/\?/) ? "&" : "?") + "version=" + Jaxer.buildNumber;
								lastInsertedScript = Util.DOM.insertScriptAtBeginning("", head, { src: src });
							}
							else 
							{
								throw new Exception("Could not insert the client part of the Jaxer framework: please check EMBEDDED_CLIENT_FRAMEWORK_SRC and CLIENT_FRAMEWORK_SRC in your Jaxer configuration");
							}
					}
					if (Jaxer.beforeClientFramework)
					{
						for (var iScript=Jaxer.beforeClientFramework.length-1; iScript>=0; iScript--) // iterate in reverse order since we're always inserting at the beginning
						{
							var scriptInfo = Jaxer.beforeClientFramework[iScript];
							var script = Util.DOM.insertScriptAtBeginning(scriptInfo.contents || '', head, scriptInfo.src ? { src: scriptInfo.src } : null);
							lastInsertedScript |= script;
						}
					}
					if (Jaxer.afterClientFramework)
					{
						for (var iScript=0; iScript<Jaxer.afterClientFramework.length; iScript++)
						{
							var scriptInfo = Jaxer.afterClientFramework[iScript];
							if (lastInsertedScript)
							{
								lastInsertedScript = Util.DOM.insertScriptAfter(scriptInfo.contents || '', lastInsertedScript, scriptInfo.src ? { src: scriptInfo.src } : null);
							}
							else
							{
								lastInsertedScript = Util.DOM.insertScriptAtBeginning(scriptInfo.contents || '', head, scriptInfo.src ? { src: scriptInfo.src } : null);
							}
						}
					}
				}
				
				// move all meta tags that specify Content-Type and a charset to the beginning of the head element:
				var metas = Array.slice(doc.getElementsByTagName("meta"));
				metas.forEach(function moveMeta(meta)
				{
					if ((meta.getAttribute("http-equiv") == "Content-Type") &&
						(typeof meta.getAttribute("content") == "string") &&
						(/charset\=/.test(meta.getAttribute("content"))))
					{
						head.insertBefore(meta, head.firstChild);
					}
				});
			} 
			catch (e) 
			{
				Jaxer.response.notifyError(e); // No need to throw again -- we'll handle it in the next line
				checkErrors(doc);
			}
		}
		
		if (Jaxer.response.getDomTouched() || Jaxer.response.getSideEffect())
		{
			try
			{
				Jaxer.DB.closeAllConnections(true);
			}
			catch (e) 
			{
				Jaxer.response.notifyError(e); // No need to throw again -- we'll handle it in the next line
				checkErrors(doc);
			}
		}

		if (Jaxer.response.getDomTouched())
		{
			Jaxer.response.persistHeaders();
		}
		else
		{
			Jaxer.response.useOriginalContents();
		}
		
		// If we made it this far, we'll have some reasonable DOM to serialize and return;
		// If something exited prematurely, at least we won't throw bad data to the browser.
		Jaxer.response.allow(true); 
			
	});
	
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

/**
 * Do the right thing if the developer specified an onserverload handler for
 * this page.
 * 
 * @private
 * @param {DocumentElement} doc
 * 		The document whose onserverload (if any) is to be processed
 */
function processOnServerLoad(doc)
{
	processLoadEvent("serverload", doc, true);
	var pageOnloadEnabled = Jaxer.pageWindow.Jaxer.onloadEnabled;
	if (pageOnloadEnabled == true || 
		(Config.ONLOAD_ENABLED && (pageOnloadEnabled != false)))
	{
		processLoadEvent("load", doc, false);
	}
}

/**
 * Do the right thing if the developer specified an onaftercallbackprocessing handler for
 * this page
 * 
 * @private
 * @param {DocumentElement} doc
 * 		The document whose aftercallbackprocessing (if any) is to be processed
 */
function processAfterCallbackProcessing(doc)
{
	processLoadEvent("aftercallbackprocessing", doc, true);
}

/**
 * Process various load events (common wrapper for onserverload and
 * aftercallbackprocessing and others in the future)
 * 
 * @private
 * @param {String} eventname
 * 		The name of the event to be processed. This is WITHOUT the "on" prefix.
 * @param {DocumentElement} doc
 * 		The document whose onserverload (if any) is to be processed
 * @param {Boolean} removeBodyAttribute
 * 		If true, any body attribute with "on" + eventname will be stripped off.
 */
function processLoadEvent(eventname, doc, removeBodyAttribute)
{
	log.trace("Processing load event '" + eventname + "' with doc = " + doc);
	
	// Note: we remove the attribute from the body tag, if we find it, but we leave it
	// up to Jaxer Core to have registered the value as an event listener
	var oneventname = "on" + eventname;
	if (removeBodyAttribute && doc.body && doc.body.hasAttribute(oneventname))
	{
		doc.body.removeAttribute(oneventname);
		Jaxer.response.noteDomTouched();
	}
	
	var event = Jaxer.pageWindow.document.createEvent("Events");
	event.initEvent(eventname, false, false);
	Jaxer.pageWindow.dispatchEvent(event);
}

/**
 * Check for fatal or response errors, and change the DOM accordingly
 * 
 * @private
 * @return {Boolean}
 * 		true if there were any (so further processing should be stopped), false
 * 		otherwise
 */
function checkErrors(doc)
{
	if (Jaxer.fatalError) // say goodbye and go home
	{
		if (!Jaxer.response.wasErrorShown()) 
		{
			Jaxer.response.notifyErrorShown();
			if (Config.DISPLAY_ERRORS && doc && doc.body) 
			{
				var container = doc.createElement("pre");
				container.setAttribute("style", 'border: 1px solid red; background-color:wheat; font-weight:bold; padding: 4px');
				container.innerHTML = 
					[
						  "Fatal Jaxer error: " + Jaxer.fatalError
						, "(Please see the Jaxer log for more details)"
						, "After fixing the problem, you may need to force the browser to reload this page not from its cache"
					].join("<br/>\n");
				doc.body.insertBefore(container, doc.body.firstChild);
			}
			else
			{
				Jaxer.response.setContents(Jaxer.fatalErrorPage);
			}
		}
		return true;
	}
	
	if (Jaxer.response.hasError()) // say goodbye for this request and go home
	{
		Jaxer.DB.closeAllConnections();
		if (!Jaxer.response.wasErrorShown()) 
		{
			Jaxer.response.notifyErrorShown();
			if (Config.DISPLAY_ERRORS && doc && doc.body) 
			{
				var container = doc.createElement("pre");
				container.setAttribute("style", 'border: 1px solid red; background-color:wheat; font-weight:bold; padding: 4px');
				container.innerHTML = 
					[
						   "Jaxer error processing request: " + Jaxer.response.getError()
						 , "(Please see the Jaxer log for more details)"
						 , "&nbsp;"
						 , "<div style='color: red; font-weight: bold'>CAUTION: Config.DEV_MODE and Config.DISPLAY_ERRORS should be <i>false</i> in production mode</div>"
					].join("<br/>\n");
				doc.body.insertBefore(container, doc.body.firstChild);
			}
			else
			{
				Jaxer.response.setContents(Jaxer.responseErrorPage);
			}
		}
		return true;
	}
	
	return false;
}

Log.trace("*** HandlePage.js loaded");

})();
