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

if (Jaxer.isOnServer)
{

}
else
{
	// javascript:Jaxer.Log.show()
	// javascript:Jaxer.Log.trace("Hi mom!");void(0)
	Log.show = function show(containerId)
	{
		if (containerId == null && document.body)
		{
			var container = document.createElement("div");
			containerId = "logContainer_" + Math.random().toString();
			container.id = containerId;
			document.body.appendChild(container);
		}
		var inPageAppender = new Log.log4javascript.InPageAppender(document.getElementById(containerId), false);
		Log.logger.addAppender(inPageAppender);
		var logDoc = document.getElementById(containerId).getElementsByTagName("iframe")[0].contentDocument;
		var logWin = document.getElementById(containerId).getElementsByTagName("iframe")[0].contentWindow;
		var moduleFiltersContainer = logDoc.getElementById("levels").cloneNode(false);
		logDoc.getElementById("search").parentNode.insertBefore(moduleFiltersContainer, logDoc.getElementById("search"));

		var html = [];
		var sep = "&nbsp;&nbsp;";
		html.push("Modules: ");
		html.push("<input type='button' onclick='setAllModuleFilters(true)' class='button' title='Set all module filters to fully verbose' value='All on'>" + sep);
		html.push("<input type='button' onclick='setAllModuleFilters(false)' class='button' title='Set all module filters to silent' value='All off'>" + sep);
		var moduleNames = Log.getSortedModuleNames();
		for (var i=0; i<moduleNames.length; i++)
		{
			var moduleName = moduleNames[i];
			html.push("<input type='checkbox' id='filter_" + moduleName + "' onclick='toggleModuleFilter(this)' ");
			if (Log.forModule(moduleName).getLevel() == Log.ALL)
			{
				html.push(" checked ");
			}
			html.push(" title='Toggle the level for this module between fully verbose and silent'>" + moduleName + sep)
		}
		moduleFiltersContainer.innerHTML = html.join('');

		var moduleNames = Log.getSortedModuleNames();
		for (var i=0; i<moduleNames.length; i++)
		{
			Log.forModule(moduleNames[i]).setLevel(Log.OFF);
		}
		logWin.toggleModuleFilter = function(checkbox)
		{
			var moduleName = checkbox.id.substr("filter_".length);
			Log.forModule(moduleName).setLevel(checkbox.checked ? Log.ALL : Log.OFF);
		}
		logWin.setAllModuleFilters = function(enabled)
		{
			var moduleNames = Log.getSortedModuleNames();
			var level = enabled ? Log.ALL : Log.OFF;
			for (var i=0; i<moduleNames.length; i++)
			{
				var moduleName = moduleNames[i];
				var checkbox = logDoc.getElementById("filter_" + moduleName);
				Log.forModule(moduleName).setLevel(level);
				checkbox.checked = enabled;
			}
		}
	}
}
