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

Jaxer.dwr = {};

/**
 * This is the path to the DWR servlet. Generally this string will end 'dwr'.
 * <p>This property <strong>must</strong> be set before any calls to
 * <code>Jaxer.dwr.require()</code>.
 * <p>Example usage:
 * <code>Jaxer.dwr.pathToDwrServlet = "http://javaserver.intranet:8080/HOSTEDAPP/dwr";</code>
 */
Jaxer.dwr.pathToDwrServlet = null;

/**
 * Import Java resources into Jaxer.
 * Each resource is the name of a Java class prefixed with details about how it
 * is to be instantiated. In the initial release, only 'new' is supported
 * although it is envisaged that support for other creators like spring/guice
 * will be added shortly. There is a special value of 'util' that will give
 * access to the dwr.util classes
 * <p>Example usage:
 * <code>Jaxer.dwr.require("util", "new/java.io.File");</code>
 * @param {string} varargs list of resources to import
 */
Jaxer.dwr.requireUtil = function()
{
    Jaxer.dwr._requireSingle("util");
};

/**
 * Import Java resources into Jaxer.
 * Each resource is the name of a Java class prefixed with details about how it
 * is to be instantiated. In the initial release, only 'new' is supported
 * although it is envisaged that support for other creators like spring/guice
 * will be added shortly. There is a special value of 'util' that will give
 * access to the dwr.util classes
 * <p>Example usage:
 * <code>Jaxer.dwr.createNew("java.io.File");</code>
 * @param {string} varargs list of resources to import
 */
Jaxer.dwr.configureAccess = function(className)
{
    if (!Jaxer.dwr.pathToDwrServlet) 
    {
        Jaxer.Log.error("Jaxer.dwr.pathToDwrServlet has not been set");
		
        return;
    }
	
    if (!Jaxer.dwr._haveEngine) 
    {
        Jaxer.dwr._requireSingle("engine");
        Jaxer.dwr._haveEngine = true;
    }
	
    Jaxer.dwr._requireSingle("new/" + className);
};

/**
 * Internal function to import a single resource
 * @param {Object} required The short name of the resource to import
 */
Jaxer.dwr._requireSingle = function(required)
{
    var url = Jaxer.dwr.pathToDwrServlet + "/" + required + ".js";
	
    Jaxer.load(url);
};
