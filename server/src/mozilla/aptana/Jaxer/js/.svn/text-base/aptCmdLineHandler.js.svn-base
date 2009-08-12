/* ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 * 
 *  This program is distributed in the hope that it will be useful, but
 *  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 *  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 *  is prohibited.
 * 
 *  You can redistribute and/or modify this program under the terms of the GPL, 
 *  as published by the Free Software Foundation.  You should
 *  have received a copy of the GNU General Public License, Version 3 along
 *  with this program; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *  
 *  Aptana provides a special exception to allow redistribution of this file
 *  with certain other code and certain additional terms
 *  pursuant to Section 7 of the GPL. You may view the exception and these
 *  terms on the web at http://www.aptana.com/legal/gpl/.
 *  
 *  You may view the GPL, and Aptana's exception and additional terms in the file
 *  titled license-jaxer.html in the main distribution folder of this program.
 *  
 *  Any modifications to this file must keep this entire header intact.
 * 
 * ***** END LICENSE BLOCK ***** */

const CC = Components.classes;
const CI = Components.interfaces;
const CR = Components.results;
const CU = Components.utils;

CU.import("resource://gre/modules/XPCOMUtils.jsm");

const Jaxer = CC['@aptana.com/jaxer/global;1'].getService(CI.aptIJaxerGlobal).getObject();
const Log = CC['@aptana.com/log;1'].getService(CI.aptICoreLog);

function aptCmdLinepHandler() {
}

aptCmdLinepHandler.prototype = {
	
	// properties required for XPCOM registration:
	classDescription: "Jaxer CommandLine Handler",
	classID:          Components.ID("{942E24E8-D453-4a2e-9E48-131EF96546B1}"),
	contractID:       "@aptana.com/jaxer/jaxer-cmdline-handler;1",
	
	_xpcom_categories: [
		{ category: "command-line-handler", entry: "z-jaxer" }
	],

	// nsISupports
	QueryInterface: XPCOMUtils.generateQI([CI.nsICommandLineHandler]),

	// nsICommandLineHandler
	handle : function(cmdLine) {
		if (cmdLine.length > 0) {
			var args = [];
			for(var i = 0; i < cmdLine.length; ++i) {
				args.push(cmdLine.getArgument(i));
			}
			Log.warning("Unhandled command line arguments ("+cmdLine.length+"): "+args.join(", "));
		}
	}

};

function NSGetModule(aCompMgr, aFileSpec) {
  return XPCOMUtils.generateModule([aptCmdLinepHandler]);
}
