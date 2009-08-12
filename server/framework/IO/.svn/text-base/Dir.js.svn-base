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

/**
 * @classDescription {Jaxer.FileUtils} This is a general class that wraps XPCOM filesystem functionality and from which File and Dir objects are derived.
 */
var fileUtils = new Jaxer.FileUtils();

/**
 * @classDescription {Jaxer.DirUtils} This is a utility class that wraps XPCOM directory/folder utility functions
 */
var dirUtils = new Jaxer.DirUtils();

/**
 * @classDescription {Jaxer.NetworkUtils} This is a utility class that wraps XPCOM Network utility functions
 */
var networkUtils = new Jaxer.NetworkUtils();

var fileProtocolHandler = Components.classes["@mozilla.org/network/protocol;1?name=file"].createInstance(Components.interfaces.nsIFileProtocolHandler);
// Not used yet: var resourceProtocolHandler = Components.classes["@mozilla.org/network/protocol;1?name=resource"].createInstance(Components.interfaces.nsIResProtocolHandler);

var resourcePattern = /^resource\:\/{2,}/i;
var filePattern = /^file\:\/{2,}/i;
var optFilePattern = /^(file\:\/{2,})?/i;

/**
 * @classDescription {Jaxer.Filesystem} File System Object contains methods useful for accessing the basic file and directory objects.
 */

/**
 * @classDescription {Jaxer.Dir} Utility object for filesystem directory access.
 */

/**
 * Convert a URL in string format to a native filesystem path.
 * The URL must begin with "file:..."
 * 
 * @alias Jaxer.Dir.urlToPath
 * @param {String} url
 * 		The URL to convert
 * @return {String|null}
 * 		The full path, or null if the URL could not be converted
 */
Dir.urlToPath = function urlToPath(url)
{
	var candidate = fileUtils.urlToPath(url);
	return (typeof candidate == "string") ? candidate : null;
}

/**
 * Convert a native filesystem path to a URL format, which will begin
 * with "file:...".
 * 
 * @alias Jaxer.Dir.pathToUrl
 * @param {String} path
 * 		The full path to be converted
 * @return {String|null}
 * 		The URL, or null if the path could not be converted
 */
Dir.pathToUrl = function pathToUrl(path)
{
	var candidate = fileUtils.pathToURL(path);
	return (typeof candidate == "string") ? candidate : null;
}

/**
 * Combines any number of path fragments into a single path, using the current
 * operating system's filesystem path separator. Before joining two fragments
 * with the path separator, it strips any existing path separators on the
 * fragment ends to be joined
 * 
 * @alias Jaxer.Dir.combine
 * @param {String} ... 
 * 		Takes any number of string path fragments
 * @return {String}
 * 		The fragments joined into a path
 */
Dir.combine = function combine()
{
	if (arguments.length == 0) return '';
	if (arguments.length == 1) return arguments[0];
	var isUrl = resourcePattern.test(arguments[0]) || filePattern.test(arguments[0]);
	var sep = isUrl ? '/' : System.separator;
	var stripRight = new RegExp("\\" + sep + "+$");
	var stripLeft  = new RegExp("^\\" + sep + "+");
	var stripBoth  = new RegExp("^\\" + sep + "|\\" + sep + "$", 'g');
	pieces = [];
	pieces.push(arguments[0].replace(stripRight, ''));
	for (var i=1; i<arguments.length-1; i++)
	{
		pieces.push(arguments[i].replace(stripBoth, ''));
	}
	pieces.push(arguments[arguments.length - 1].replace(stripLeft, ''));
	return pieces.join(sep);
}

/**
 * Resolves a path to an absolute path on the filesystem, using as a reference
 * (base) the given path or the current page's path.
 * 
 * @alias Jaxer.Dir.resolve
 * @param {String} pathToResolve
 * 		The path to resolve, e.g. a filename. It can also be a resource pattern
 * 		(e.g. "resource:///...") or a file pattern (e.g. "file:///...")
 * @param {String} [referencePath]
 * 		An optional path to use as a reference. By default, it uses the current
 * 		page's path.
 * @return {String}
 * 		The full path on the filesystem
 */
Dir.resolve = function resolve(pathToResolve, referencePath)
{
	var absoluteUrl;
	var urlToResolve = pathToResolve.replace(/\\/g, '/');
	
	if (filePattern.test(urlToResolve) ||
		/^(\w\:)?\//.test(urlToResolve)) 
	{
		absoluteUrl = urlToResolve;
	}
	else if (resourcePattern.test(urlToResolve))
	{
		absoluteUrl = Util.Url.combine(Dir.pathToUrl(System.executableFolder), urlToResolve.replace(resourcePattern, ''));
	}
	else
	{
		if (!referencePath)
		{
			if (Jaxer.request && Jaxer.request.app && (typeof Jaxer.request.app.PATH == "string")) 
			{
				referencePath = Jaxer.request.app.PATH;
			}
			else
			{
				throw new Exception("Could not resolve path '" + pathToResolve + "' because there was neither a referencePath nor a current application with a (default) PATH");
			}
		}
		if (filePattern.test(referencePath))
		{
			referenceUrl = referencePath;
		}
		else if (resourcePattern.test(referencePath))
		{
			referenceUrl = Util.Url.combine(Dir.pathToUrl(System.executableFolder), referencePath.replace(resourcePattern, ''));
		}
		else
		{
			referenceUrl = Dir.pathToUrl(referencePath);
		}
		absoluteUrl = Util.Url.combine(referenceUrl, urlToResolve);
	}
	
	var fullyResolvedUrl = absoluteUrl.replace(optFilePattern, "file:///"); // This forces it to be "truly" absolute
	var resolvedPath = fileProtocolHandler.getFileFromURLSpec(fullyResolvedUrl).path;
	return resolvedPath;
}

/**
 * Does the directory exist on disk?
 * 
 * @alias Jaxer.Dir.exists
 * @param {String} path
 * 		The full or partial (to be resolved) path to test
 * @return {Boolean}
 * 		true if exists, false otherwise
 */
Dir.exists = function exists(path)
{
	var fullPath = Dir.resolve(path);
	var dir = new Dir(fullPath);
	
	if (dir.exists && !dir.isDir)
	{
		throw path+" is not a folder" ;
	}

	return dir.exists;
};

/**
 * Creates a new folder (directory) at the specified path and returns it
 * 
 * The format of the permissions is a unix style numeric chmod i.e. 0777 or 444
 * 
 * on Windows, you can only set the Read/Write bits of a file. And User/Group/Other will have the SAME 
 * settings based on the most-relaxed setting (Read 04, 040, 0400, Write 02, 020, 0200). When a file is created, 
 * by default it has both Read and Write permissions. Also, you cannot set the file permission to WRITE-ONLY, doing 
 * so would set it to read-write
 *		
 * @method
 * @alias Jaxer.Dir.create
 * @param {String} path
 * 		The path of the new folder
 * @param {String} aPermissions
 * 		The permissions used to create the filesystem object.
 * @return {Jaxer.Dir}
 * 		The new directory object
 * @exception {Error} 
 * 		Throws a Exception containing the error code if filesytem object is unable to be created.
 */
Dir.create = function create(path, aPermissions)
{
	var fullPath = Dir.resolve(path);
	var dir = new Dir(fullPath);
	dir.create();
	return dir;
}

/**
 * Creates a hierarchy of folders as needed to contain the current folder's path.
 * The format of the permissions is the same as for the create method.
 * 
 * @method
 * @alias Jaxer.Dir.createHierarchy
 * @param {String} path
 * 		The path of the new folder
 * @param {String} aPermissions
 * 		The permissions used to create all the filesystem objects
 * @return {Jaxer.Dir}
 * 		The new directory object
 * @exception {Error} 
 * 		Throws a Exception containing the error code if filesytem object is unable to be created.
 */
Dir.createHierarchy = function createHierarchy(path, aPermissions)
{
	var fullPath = Dir.resolve(path);
	var dir = new Dir(fullPath);
	dir.createHierarchy();
	return dir;
}

/**
 * Scan a folder tree from the provided path and find files
 * that match the provided regular expression pattern.
 * 
 * The available options properties are
 * <ul>
 * 	<li>pattern  : a string containing a regular expression e.g. "^.*\.js$"</li>
 * 	<li>flags    : the flags to use with the regular expression, e.g. "i" to ignore case</li>
 * 	<li>recursive: true/false indication of whether to search sub folders for the match</li>
 * </ul>
 *
 * @alias Jaxer.Dir.grep
 * @param {String} path
 *   The starting path for the search. This must be a folder.
 * @param {Object} [options]
 *   Optional An Associative Array of optional parameters
 * @exception {Jaxer.Exception}
 *   A Jaxer.Exception object is thrown when the path is not a valid folder
 * @return {Jaxer.File[]}
 *   An Array of Jaxer.File objects that matched the provided pattern
 */
Dir.grep = function grep(path, options)
{
    /*
     * Validate the provided path
     */
    if (path.length == 0) 
	{
		throw new Exception("missing path");
    }           
    try 
	{
        var dir = new Dir(path);
        if (dir.exists && dir.isFile) 
		{
			throw new Exception("path is not a Folder : " + path);
        }
    } 
    catch (e) 
	{
		throw new Exception("Invalid path " + path);
    }
    /*
     * Marshall optional parameters
     */
    var pattern 	= (options) ? (options.pattern 		|| undefined) 	: undefined;
    var recursive 	= (options) ? (options.recursive 	|| undefined) 	: undefined;
    var flags 		= (options) ? (options.flags 		|| '') 			: '';
    
	var regexp;
	
    if (typeof pattern != 'string') 
	{
		pattern = undefined;	
    } 
	else
	{
        regexp = new RegExp(pattern, flags);
	}
    /*
     * Setup the search variables
     */
    var directoryList = new Array(dir);
    var matchedFiles = new Array();
    
    /*
     * Stack based search of the folder structure
     */
    while (directoryList.length > 0) 
	{
        var directoryContents = directoryList.shift().readDir();               
        for (var index = 0; index < directoryContents.length; index++) 
		{
            var fileSystemObject = directoryContents[index];   
            /*
             * Check files for a match and push onto the result stack
             */
            if (fileSystemObject.isFile) 
			{
                if ((typeof pattern == 'undefined') || regexp.test(fileSystemObject.leaf)) 
				{
                    matchedFiles.push(new Jaxer.File(fileSystemObject));
                }
            }
            /*
             * Add folders to the directory list if we are searching sub folders
             */
            if (recursive && fileSystemObject.isDir) 
			{
                directoryList.push(fileSystemObject);
            }
        }
    }
	
    /*
     * return the matched files as an array of Jaxer.File objects
     */
    return matchedFiles;
}
/**
 * Scan a folder tree from the provided path and find files
 * that match the provided regular expression pattern and
 * run the provided function against each match
 *
 * @alias Jaxer.Dir.map
 * @param {String} path
 *   The starting path for the search. This must be a folder.
 * @param {Object} [options]
 *   Optional An Associative Array of optional parameters
 * @param {Function(Jaxer.File):Object} [fn]
 *   Optional The function to run. It will be invoked for each the matched 
 *   Jaxer.File object, with the Jaxer.File object being passed as its 
 *   parameter
 * @exception {Jaxer.Exception}
 *   A Jaxer.Exception object is thrown when the path is not a valid folder
 * @return {Object[]}
 *   An Array of objects created by invoking the provided function on each
 *   file which matched the provided pattern
 */
Dir.map = function map(path, options, fn)
{
    fn = fn || function(file){ return file };
    return Dir.grep(path, options).map(function(file) { return fn(file) });
}

Jaxer.Dir = Dir;

Log.trace("*** Dir.js loaded");

})();
