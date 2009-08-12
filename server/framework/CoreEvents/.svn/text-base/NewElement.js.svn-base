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

var log = Log.forModule("NewElement");

CoreEvents.clearHandlers(CoreEvents.NEW_ELEMENT); // Clear this out -- we're the first and only default handler

var tempLoads = {};

Jaxer.__tempLoad = {};

Jaxer.__tempLoad.set = function set(elt, src, global, runat, useCache, forceRefreshCache)
{
	var id = 'tempLoad_' + Math.random() + (new Date()).getTime();
	var contents = "Jaxer.__tempLoad.load('" + id + "')";
	elt.innerHTML = contents;

	var tempLoad = 
	{
		params: [src, global, runat, useCache, forceRefreshCache, false],
		elt: elt,
		src: src
	};
	tempLoads[id] = tempLoad;
	
	return tempLoad;
}

Jaxer.__tempLoad.load = function load(id)
{
	var tempLoad = tempLoads[id];
	var elt = tempLoad.elt;
	if (elt) 
	{
		elt.innerHTML = '';
	}
	log.trace("Loading src'ed script with [src, global, useCache, forceRefreshCache] = " + tempLoad.params);
	Includer.load.apply(null, tempLoad.params);
}

Jaxer.__tempLoad.clearAll = function clearAll()
{
	tempLoads = {};
}

try
{
	CoreEvents.addHandler(CoreEvents.NEW_ELEMENT, function onNewElement(evt, doc, elt)
	{
		switch (elt.tagName.toUpperCase())
		{
			case "SCRIPT":
				var src = elt.getAttribute(SRC_ATTR);
				var runat = elt.getAttribute(RUNAT_ATTR);
				var autoload = elt.hasAttribute(AUTOLOAD_ATTR) &&
					!elt.getAttribute(AUTOLOAD_ATTR).match(AUTOLOAD_ATTR_FALSE_REGEXP);
				log.trace("Received element: " + elt.tagName + 
					" with " + ((src == null) ? "no src" : "src='" + src + "'") +
					" and " + ((runat == null) ? "no runat" : "runat='" + runat + "'"));
				if (CoreEvents.isScriptEvaluateEnabled(elt) ||
					CoreEvents.isScriptLoadEnabled(elt)) // will Core be evaluating this script server-side?
				{                                        // if so, assume there could be a side-effect or DOM change
					Jaxer.response.noteDomTouched();
					Jaxer.response.noteSideEffect();
				}
				Jaxer.lastScriptRunat = runat;
				if (CoreEvents.isScriptLoadEnabled(elt)) 
				{
					
					if (runat.match(RUNAT_ANY_BOTH_REGEXP)) // create the client-only element in place
					{
						// The functions to be created should be tagged as server-only; the client ones will be created
						// directly on the client when the src file is loaded by the client directly.
						runat = runat.replace(RUNAT_BOTH_BASE, RUNAT_SERVER_BASE); 
					}

					if (autoload)
					{
						if (!src) 
						{
							log.warn("Could not autoload script without a valid " + SRC_ATTR + " attribute");
							autoload = false;
						}
						else if (runat.match(RUNAT_NO_CACHE_REGEXP)) 
						{
							log.warn("Could not autoload script " + src + " because its " + RUNAT_ATTR + " attribute indicates not to cache it");
							autoload = false;
						}
						else
						{
							src = Web.resolve(src);
						}
					}
					var useCache = autoload;
					var forceRefreshCache = autoload && Jaxer.Config.RELOAD_AUTOLOADS_EVERY_PAGE_REQUEST;
					
					Jaxer.__tempLoad.set(elt, src, Jaxer.pageWindow, runat, useCache, forceRefreshCache);
					
					if (autoload)
					{
						CallbackManager.addAutoload(src);
					}
				}
				break;
			case "JAXER:INCLUDE":
				var src = elt.getAttribute(SRC_ATTR);
				var path = elt.getAttribute("path");
				log.trace("Received element: " + elt.tagName + 
					" with " + ((src == null) ? "no src" : "src='" + src + "'") +
					" and " + ((path == null) ? "no path" : "path='" + path + "'"));
				Jaxer.response.noteDomTouched();
				Jaxer.response.noteSideEffect();
				Includer.includeElement(doc, elt);
				break;
		}
	});
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

Log.trace("*** NewElement.js loaded");

})();
