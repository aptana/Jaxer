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

// This is an example of a Jaxer framework extension.
// It allows automatic servicing of RPC-type requests
// including serializing/deserializing arguments, fetching
// and calling the requested code, etc.
//
// This also uses functionality provided by the more generic
// ServiceDispatcher example extension.

var RPCDispatcher = {};

/**
 * Tries to match the current request's path to a location of a file (not a folder) on disk.
 * Assumes that the last part of the path is the name of the function to call.
 * 
 * @alias Jaxer.Extensions.RPCDispatcher.findFile
 * @advanced
 * @param {String} diskPathRoot
 * 		The path on disk of the folder where the search should begin. 
 * 		Usually this will be Jaxer.request.documentRoot
 * 		but you could use any folder.
 * @param {String} fileExtension
 * 		The extension (including the period) of the file to look for.
 * 		Usually this will be '.js'.
 * @param {Number} [numFirstPathPartToConsiderAsFile]
 * 		An optional number of the first path part to consider as a file, starting from the beginning
 * 		of the path. E.g. if this is 1, we test for a file only starting at pathParts[1] -- all path parts
 * 		below that must be folders. If this is not specified or 0 (the default), then the first path part
 * 		is tested for being a file before it's tested for being a folder. 
 * 		Usually this is specified as one more than the index of the pathPart containing "jaxer-rpc":
 * 		1 + parsedUrl.pathParts.indexOf('jaxer-rpc')
 * @return {String}
 * 		This will return the file://...-type URL of the found file, or null if not found.
 */
RPCDispatcher.findFile = function findFile(diskPathRoot, fileExtension, numFirstPathPartToConsiderAsFile)
{
	return Jaxer.Extensions.ServiceDispatcher ? 
		Jaxer.Extensions.ServiceDispatcher.findFile(diskPathRoot, fileExtension, numFirstPathPartToConsiderAsFile, 1) : // 1 to skip the last 1 part of the path, which is the function name
		null;
}

/**
 * Handles the RPC request: finds and loads the file containing the functions,
 * extracts the arguments (if any) from the requests's data (they must be in the 'params' parameter), 
 * calls the function, and sets the response's body to the serialized result.
 * 
 * @alias Jaxer.Extensions.RPCDispatcher.handleRequest
 * @advanced
 * @param {String} diskPathRoot
 * 		The file://-type URL path on disk where the search for the file to load should begin. 
 * 		Usually this will be Jaxer.request.documentRoot
 * 		but you could use any folder.
 * @param {String} fileExtension
 * 		The extension (including the period) of the file to look for and load.
 * 		Usually this will be '.js'.
 */
RPCDispatcher.handleRequest = function handleRequest(diskPathRoot, fileExtension)
{
	var request = Jaxer.request;
	var response = Jaxer.response;
	var app = request.app;
	var parsedUrl = request.parsedUrl;
	
	var fileToLoad = RPCDispatcher.findFile(diskPathRoot, fileExtension);
	var global = Jaxer.pageWindow;
	var runat = "";
	var useCache = true;
	var forceCacheRefresh = Config.RELOAD_AUTOLOADS_EVERY_PAGE_REQUEST;
	var dontSetRunat = true;
	Includer.load(fileToLoad, global, runat, useCache, forceCacheRefresh, dontSetRunat);
	
	var nameOfFunctionCalled = parsedUrl.leaf;
	var functionCalled = global[nameOfFunctionCalled];
	
	if (typeof functionCalled != 'function') 
	{
		response.exit(400, "Bad Request - Unknown function called");
	}
	
	var params = ('params' in request.data) ? Jaxer.Serialization.fromJSONString(request.data.params) : [];
	if (!(params instanceof Array)) params = [params];
	try 
	{
		var result = functionCalled.apply(null, params);
		response.setContents((result == undefined) ? Jaxer.Serialization.toJSONString(result) : '');
	}
	catch (e)
	{
		Jaxer.Log.error("Error when calling function " + nameOfFunctionCalled + ": \n" + e);
		response.exit(500, "Error when calling function " + nameOfFunctionCalled + " - check the log for more information");
	}
}

RPCDispatcher.onLoad = function onLoad()
{
	// Optional - put any kind of initialization here
	// For example, we could add an entry here to Config.apps
	// instead of putting it in configApps.js
}

RPCDispatcher.onUnload = function onUnload()
{
	// Optional - put any kind of cleanup here
	// For example, if we added an entry to Config.apps in RPCDispatcher.onLoad
	// then we should remove it here
}

Jaxer.loadExtension("RPCDispatcher", RPCDispatcher);

})();
