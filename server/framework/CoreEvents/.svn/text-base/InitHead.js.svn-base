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

var log = Log.forModule("InitHead");

CoreEvents.clearHandlers(CoreEvents.INIT_HEAD); // Clear this out -- we're the first and only default handler

try
{
	CoreEvents.addHandler(CoreEvents.INIT_HEAD, function onInitHead(evt, doc)
	{
		
		/**
		 * Loads a JavaScript file and evaluates it.
		 * 
		 * @alias Jaxer.load
		 * @param {String} src
		 * 		The URL from which the JavaScript file should be retrieved. If
		 * 		the src is an absolute file://... URL then it is retrieved
		 * 		directly from the file system, otherwise it is retrieved via a
		 * 		web request.
		 * @param {Object} [global]
		 * 		The global (usually a window object) on which to evaluate it. By
		 * 		default, it is the current window object of the page.
		 * @param {String} [runat]
		 * 		The value of the effective runat "attribute" to use when
		 * 		evaluating this code. By default, it uses the same runat
		 * 		attribute as the last evaluated script block.
		 * 
		 */
		Jaxer.load = function load(src, global, runat)
		{
			return Jaxer.Includer.load(src, global || Jaxer.pageWindow, runat); 
		}

		/**
		 * This starts out as null at the start of every request. If you set it to true,
		 * handlers for the onload event registered via addEventListener or
		 * window.onload will be called, regardless of Jaxer.Config.ONLOAD_ENABLED;
		 * if you set it to false, they will not be called, regardless of Jaxer.Config.ONLOAD_ENABLED.
		 * Thus, it is used to override Jaxer.Config.ONLOAD_ENABLED for the current request.
		 * 
		 * @alias Jaxer.onloadEnabled
		 * @property {Boolean}
		 */
		Jaxer.onloadEnabled = null;
		
		// On the server side, we need to tweak what certain basic JavaScript functionalities do:
		Jaxer.Overrides.applyAll(Jaxer.pageWindow);
		
		CallbackManager.initPage(Jaxer.pageWindow);

	});
	
}
catch (ex)
{
	throw new Exception("Could not add handler: " + ex.description, log);
}

Log.trace("*** InitHead.js loaded");

})();
