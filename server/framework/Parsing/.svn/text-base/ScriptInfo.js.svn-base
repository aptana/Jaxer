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

(function(){

var log = Log.forModule("ScriptInfo"); // Only if Log itself is defined at this point of the includes

/**
 * @classDescription {Jaxer.ScriptInfo} Container object used during runat
 * attribute and property processing.
 */

/**
 * This is a container object used during runat attribute and property
 * processing. This allows all runat-related state to be passed around in one
 * simple container object. At the same time, this formalizes what is being
 * passed around which is preferable to using a simple anonymous object.
 * 
 * @advanced
 * @constructor
 * @alias Jaxer.ScriptInfo
 * @return {Jaxer.ScriptInfo}
 * 		Returns an instance of ScriptInfo.
 */
function ScriptInfo()
{
	this.functionNames = {};
	this.cacheInfos = [];
	this.proxyInfos = [];
	this.hasProxies = false;
	if (Jaxer.proxies)
	{
		var proxies = {};
		Jaxer.proxies.forEach(function(proxy)
		{
			if (typeof proxy == "function" && proxy.name)
			{
				proxies[proxy.name] = true;
			}
			else if (typeof proxy == "string")
			{
				proxies[proxy] = true;
			}
			else
			{
				log.warn("The following Jaxer.proxies element was neither a named function nor a string: " + proxy);
			}
		});
		this.jaxerProxies = proxies;
	}
	else
	{
		this.jaxerProxies = null;
	}
}

frameworkGlobal.ScriptInfo = Jaxer.ScriptInfo = ScriptInfo;

})();
