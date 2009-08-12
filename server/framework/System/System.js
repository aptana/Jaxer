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

(function()
{
   
/**
 * @namespace {Jaxer.System} Namespace object holding functions and members used
 * to access operating system resources and processes.
 */
var System = {};

var du = new Jaxer.DirUtils();
var fu = new Jaxer.FileUtils();

try
{
   /**
    * The path on disk of the current executable's folder
    * 
	* @alias Jaxer.System.executableFolder
 	* @property {String}
	*/
	System.executableFolder = du.current;
}
catch (e)
{
	coreTraceMethods.WARN("Could not set System.executableFolder: " + e);
	System.executableFolder = null;
}

try
{
   /**
    * The file://... URL of the current executable's folder
    * 
	* @alias Jaxer.System.executableUrl
 	* @property {String}
	*/
	System.executableUrl = fu.pathToURL(du.current);
}
catch (e)
{
	coreTraceMethods.WARN("Could not set System.executableUrl: " + e);
	System.executableUrl = null;
}

try
{
   /**
    * The path on disk of the current user's home folder
    * 
 	* @alias Jaxer.System.homeFolder
 	* @property {String}
	*/
	System.homeFolder = du.home;
}
catch (e)
{
	coreTraceMethods.WARN("Could not set System.homeFolder, this can be caused by an invalid HOME environement variable see Jaxer Known Limitations for more info. (http://www.aptana.com/view/jaxer_known_limitations): " + e);
	System.homeFolder = null;
}

try
{
	/**
 	* The path on disk of the current user's desktop folder
 	* 
	* @alias Jaxer.System.desktopFolder
	* @property {String}
 	*/
	System.desktopFolder = du.desktop;
}
catch (e)
{
	coreTraceMethods.WARN("Could not set System.desktopFolder: " + e);
	System.desktopFolder = null;
}
    

try
{
   /**
    * The path on disk of the system's temp folder
    * 
    * @alias Jaxer.System.tempFolder
    * @property {String}
    */
	System.tempFolder = du.temp;
}
catch (e)
{
	coreTraceMethods.WARN("Could not set System.tempFolder: " + e);
	System.tempFolder = null;
}

try
{
	/**
    * The filesystem separator character (either \ or /)
    * 
    * @alias Jaxer.System.separator
    * @property {String}
    * @return {String}
    * 		fileSystem path separator
    */
	System.separator = (System.executableFolder.indexOf('\\') != -1) ? '\\': '/';
}
catch (e)
{
	coreTraceMethods.WARN("Could not set System.separator: " + e);
	System.separator = null;
}

/**
 * Ask the operating system to attempt to open the file. This simulates "double clicking" the file on your platform. 
 * This routine only works on platforms which support this functionality. This is non-blocking and script execution will
 * continue.
 * 
 * @alias Jaxer.System.launch
 * @method
 * @param {String} path 
 * 		A String containing the path to the file or a Jaxer.File object referencing the filesystem object
 */
System.launch = function launch(path)
{
	if (Jaxer.File.exists(path)) {
	    var cmd = new Jaxer.File(path)._nsIFile; // get the internal XPCOM nsIFile wrapper 
	    cmd.launch();
	}
};

/**
 * A string identifying the current operating system
 * @alias Jaxer.System.OS
 * @property {String}
 */
System.OS = Jaxer['private'].OS;

frameworkGlobal.System = Jaxer.System = System;

Log.trace("*** System.js loaded");

})();
