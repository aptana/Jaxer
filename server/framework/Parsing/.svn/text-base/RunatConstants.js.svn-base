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
	
var RUNAT_SEPARATOR = "-";
var RUNAT_SERVER_BASE = "server";
var RUNAT_BOTH_BASE = "both";

var consts =
{
	RUNAT_ATTR: "runat",
	SRC_ATTR: "src",
	PROXY_ATTR: "proxy",
	TYPE_ATTR: "type",
	AUTOLOAD_ATTR: "autoload",
	
	RUNAT_CLIENT: "client",
	RUNAT_SEPARATOR: RUNAT_SEPARATOR,
	RUNAT_SERVER_BASE: RUNAT_SERVER_BASE,
	RUNAT_SERVER_NO_CACHE: RUNAT_SERVER_BASE + RUNAT_SEPARATOR + "nocache",
	RUNAT_SERVER_AND_CACHE: RUNAT_SERVER_BASE,
	RUNAT_SERVER_AND_PROXY: RUNAT_SERVER_BASE + RUNAT_SEPARATOR + "proxy",
	RUNAT_BOTH_BASE: RUNAT_BOTH_BASE,
	RUNAT_BOTH_NO_CACHE: RUNAT_BOTH_BASE + RUNAT_SEPARATOR + "nocache",
	RUNAT_BOTH_AND_CACHE: RUNAT_BOTH_BASE,
	RUNAT_BOTH_AND_PROXY: RUNAT_BOTH_BASE + RUNAT_SEPARATOR + "proxy",
	
	RUNAT_ANY_SERVER_REGEXP: /^\s*(both|server)/i,
	RUNAT_ANY_BOTH_REGEXP: /^\s*both/i,
	RUNAT_NO_CACHE_REGEXP: /^\s*nocache/i,
	AUTOLOAD_ATTR_FALSE_REGEXP: /^false$/i,
	
	// NOTE: This flag is temporary and should no longer be needed
	// once we access a fast JS parser
	SIMPLE_PARSE: true
};

for (var p in consts)
{
	frameworkGlobal[p] = Jaxer[p] = consts[p];
}

})();