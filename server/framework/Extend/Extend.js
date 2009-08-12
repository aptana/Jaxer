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

var log = Log.forModule("Extend");

/**
 * @namespace {Jaxer.Extensions} A container for all the loaded extensions to
 * the Jaxer framework.
 */
Jaxer.Extensions = {}; // This will hold all the extensions

/**
 * Loads a framework extension (already defined in memory). If the
 * namespaceObject defines an onLoad method, it will be called (without
 * arguments)
 * 
 * @alias Jaxer.loadExtension
 * @param {String} name
 * 		The (unique) name to use for this extension
 * @param {Object} namespaceObject
 * 		The object holding all the members of this extension
 */
Jaxer.loadExtension = function loadExtension(name, namespaceObject)
{
	if (!namespaceObject)
	{
		throw new Exception("No namespaceObject specified for extension '" + name + "' -- extension not loaded");
	}
	Jaxer.unloadExtension(name, true); // Unload any previous version, if any
	log.trace("Adding extension '" + name + "'");
	Jaxer.Extensions[name] = namespaceObject;
	if (typeof namespaceObject.onLoad == "function")
	{
		log.trace("Loading extension '" + name + "'...");
		namespaceObject.onLoad();
		log.trace("Loaded extension '" + name + "'");
	}
}

/**
 * Unloads a previously-loaded framework extension. If its namespaceObject
 * defines an onUnload method, it will be called (without arguments)
 * 
 * @alias Jaxer.unloadExtension
 * @param {String} name
 * 		The name used to load this extension
 * @param {Boolean} noWarnings
 * 		Whether to warn if the extension was not loaded when this was called, or
 * 		its namespaceObject was not there.
 */
Jaxer.unloadExtension = function unloadExtension(name, noWarnings)
{
	var namespaceObject = Jaxer.Extensions[name];
	if (!namespaceObject)
	{
		if (!noWarnings) log.warn("No namespaceObject found for extension '" + name + "' -- extension not unloaded");
		return;
	}
	if (typeof namespaceObject.onUnload == "function") 
	{
		log.trace("Unloading extension '" + name + "'...");
		namespaceObject.onUnload();
		log.trace("Unloaded extension '" + name + "'");
	}
	delete Jaxer.Extensions[name];
	log.trace("Removed extension '" + name + "'");
}

/**
 * @private
 * @param {Object} configPath
 */
function getDirContents(configPath)
{
	if (!configPath) return [];
	var extensionsPath = Dir.resolve(configPath, System.executableFolder);
	log.trace("Looking for extensions in path: " + extensionsPath);
	var dir = new Dir(extensionsPath);
	var dirContents;
	if (dir.exists)
	{
		dirContents = dir.readDir();
		log.trace("Found " + dirContents.length + " objects in path");
	}
	else
	{
		dirContents = [];
	}
	return dirContents;
}

/**
 * Loads all extensions from the folder defined in Config.EXTENSIONS_DIR.
 * 
 * @alias Jaxer.loadAllExtensions
 */
Jaxer.loadAllExtensions = function loadAllExtensions()
{
	log.trace("Loading all extensions...");
	var dirContents = getDirContents(Config.EXTENSIONS_DIR);
	dirContents = dirContents.concat(getDirContents(Config.LOCAL_EXTENSIONS_DIR));
	var numIncluded = 0;
	dirContents.forEach(function loadFile(file)
	{
		if (file.isFile && (file.URL.match(/\.js$/)))
		{
			log.trace("Included extension: " + file.path + " (from URL: " + file.URL + ")");
			numIncluded++;
			Jaxer.include(file.URL);
		}
	});
	log.debug("Included " + numIncluded + " extensions");
}

Log.trace("*** Extend.js loaded");

})();
