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
// It allows automatic handling of any kind of service-type requests
// in which Jaxer is the direct handler of the request, e.g. RESTful
// services. It essentially figures out whether a JavaScript file
// exists on disk to handle the request, and if so loads and executes it.
// The code in that file is responsible for the rest. 

var ServiceDispatcher = {};

var verifiedPaths = {}; // Holds paths that have already been matched to files on disk

/**
 * Tries to match the current request's path to a location of a file (not a folder) on disk.
 * 
 * @alias Jaxer.Extensions.ServiceDispatcher.findFile
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
 * 		Usually this is specified as one more than the index of the pathPart containing "jaxer-service":
 * 		1 + parsedUrl.pathParts.indexOf('jaxer-service')
 * @param {Number} [numPathPartsToIgnoreAtEnd]
 * 		An optional number of path parts to not consider, starting from the end
 * 		of the path. E.g., if this is 1, then the last part of the path is not considered
 * 		when attempting the matching. The default is 0.
 * @return {String}
 * 		This will return the file://...-type URL of the found file, or null if not found.
 */
ServiceDispatcher.findFile = function findFile(diskPathRoot, fileExtension, numFirstPathPartToConsiderAsFile, numPathPartsToIgnoreAtEnd)
{
	if (!numFirstPathPartToConsiderAsFile) numFirstPathPartToConsiderAsFile = 0;
	if (!numPathPartsToIgnoreAtEnd) numPathPartsToIgnoreAtEnd = 0;
	var requestPath = Jaxer.request.parsedUrl.path;
	var diskPath;
	
	var useCache = !Config.RELOAD_AUTOLOADS_EVERY_PAGE_REQUEST;
	if (useCache)
	{
		for (var pathPrefix in verifiedPaths)
		{
			if (requestPath.indexOf(pathPrefix) == 0) // matched -- we've verified this path already
			{
				diskPath = verifiedPaths[pathPrefix];
				break;
			}
		}
	}
	
	if (!diskPath) // try to find it on disk
	{
		var currentPath = '/';
		var currentFolder = Dir.resolve(diskPathRoot);
		var pathParts = Jaxer.request.parsedUrl.pathParts;
		for (var partIndex=0, partsLength=pathParts.length-numPathPartsToIgnoreAtEnd; partIndex<partsLength; partIndex++)
		{
			var pathPart = pathParts[partIndex];
			if (pathPart == '..')
			{
				break; // You cannot walk up the folder hierarchy, for security reasons
			}
			currentPath = Util.Url.combine(currentPath, pathPart);
			currentFolder = Dir.combine(currentFolder, pathPart);
			if ((partIndex >= numFirstPathPartToConsiderAsFile) && Jaxer.File.exists(currentFolder + fileExtension)) // found path and file -- we're done
			{
				diskPath = currentFolder + fileExtension;
				verifiedPaths[currentPath] = diskPath;
				break;
			}
			else
			{
				var fsObj = new File(currentFolder);
				if (!fsObj.exists || fsObj.isFile) // there is no such folder -- stop and give up
				{
					break;
				}
			}
		}
	}
	
	return diskPath ? Jaxer.Dir.pathToUrl(diskPath) : null;
}

ServiceDispatcher.onLoad = function onLoad()
{
	// Optional - put any kind of initialization here
	// For example, we could add an entry here to Config.apps
	// instead of putting it in configApps.js
}

ServiceDispatcher.onUnload = function onUnload()
{
	// Optional - put any kind of cleanup here
	// For example, if we added an entry to Config.apps in ServiceDispatcher.onLoad
	// then we should remove it here
}

Jaxer.loadExtension("ServiceDispatcher", ServiceDispatcher);

})();
