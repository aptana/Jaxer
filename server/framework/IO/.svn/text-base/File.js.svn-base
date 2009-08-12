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
 * @classDescription {Jaxer.File} Utility object for simple filesystem access.
 */

/**
 * Read the contents of a file on local disk. If the file does not exist,
 * returns a null
 * 
 * @alias Jaxer.File.read
 * @param {String} path	
 * 		The full or partial (to be resolved) path to read
 * @return {String|null}
 * 		The contents of the file as a string, or null if the file does not exist
 */
File.read = function read(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (!file.isFile)
	{
		throw path+" is not a file" ;
	}
	
	if (!file.exists)
	{
		return null;
	}
	var data = "";
	try
	{
		file.open('r');
		data = file.read();
	}
	finally
	{
		if (file)
		{
			file.close();
		}
	}
	return data;
};

/**
 * Does the file (or folder) exist on disk?
 * 
 * @alias Jaxer.File.exists
 * @param {String} path	
 * 		The full or partial (to be resolved) path to test
 * @return {Boolean}
 * 		true if exists, false otherwise
 */
File.exists = function exists(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return file.exists;
};

/**
 * Read the contents of a textfile on local disk, return an array of lines. When
 * the optional sep parameter is not provided return a string with the lines
 * concatenated by the provided parameter. If the file does not exist, returns a
 * null
 * 
 * @alias Jaxer.File.readLines
 * @param {String} path
 * 		The full or partial (to be resolved) path to read
 * @param {String} [sep]
 * 		An optional separator to use between lines. If none is specified,
 * 		returns an array of lines.
 * @return {Array|String|null}
 * 		The contents of the file as a string or array of lines, or null if the
 * 		file does not exist
 */
File.readLines = function read(path,sep)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	

	if (!file.exists)
	{
		return null;
	}

	if (!file.isFile)
	{
		throw path+" is not a file" ;
	}

	var data = [];
	try
	{
		file.open('r');
		data = file.readAllLines();
	}
	finally
	{
		if (file)
		{
			file.close();
		}
	}
	return (sep) ? data.join(sep) : data ;
};

/**
 * Writes the provided text to file specified by the path. WARNING -
 * destructive! This will overwrite an existing file so use File.append if you
 * want to add the data to the end of an existing file.
 * 
 * @alias Jaxer.File.write
 * @param {String} path
 * 		The full or partial (to be resolved) path to read
 * @param {String} text
 * 		The text to write to the file
 */
File.write = function write(path, text)
{
	var file = File.getOrCreate(path);
	
	try
	{
		file.open('w');
		for (var i=1; i<arguments.length; i++)
		{
			file.write(String(arguments[i]));
		}
	}
	finally
	{
		if (file)
		{
			file.close();
		}
	}
};

/**
 * Creates a file if required, if the file already exists it will set the last modified timestamp to the current time.
 * @alias Jaxer.File.touch
 * @param {String} path
 * 		The full or partial (to be resolved) path to touched
 */
File.touch = function touch(path)
{
	var file = File.getOrCreate(path);
	
	try
	{
		file.open('w');
	}
	finally
	{
		if (file)
		{
			file.close();
		}
	}
};

/**
 * Truncates a file if the file already exists otherwise it will create an empty file.
 * @alias Jaxer.File.truncate
 * @param {String} path
 * 		The full or partial (to be resolved) path to truncated
 */
File.truncate = function truncate(path)
{
	var file = File.getOrCreate(path);
	file.truncate();
};

/**
 * Returns the size of the file in bytes.
 * @alias Jaxer.File.size
 * @param {String} path
 * 		The full or partial (to be resolved) path to truncated
 * @return {Number}
 * 		The size of the file in bytes
 * 		
 */
File.size = function size(path)
{
	var sPath = Dir.resolve(path);
	var sourceFile = new File(sPath);
	return sourceFile.size;
};

/**
 * Copies the file from sourcePath to destinationPath. 
 * If the destination file exists it will be overwritten.
 * @alias Jaxer.File.copy
 * @param {String} sourcePath
 * 		The full or partial (to be resolved) path of the original file
 * @param {String} destinationPath
 * 		The full or partial (to be resolved) path to the new file
 */
File.copy = function copy(sourcePath, destinationPath)
{
	var sPath = Dir.resolve(sourcePath);
	var sourceFile = new File(sPath);
	
	if (!sourceFile.isFile) 
	{
		throw sPath + " is not a file";
	};
	
	if (!sourceFile.exists) 
	{
		throw sourcePath + " does not exist";		
	};
	var dPath = Dir.resolve(destinationPath);
	var destinationFile = new File(dPath);
	
	if (destinationFile.exists) 
	{
		if (!destinationFile.isFile) 
		{
			throw dPath + " is not a file";
		// TODO 
		// if destination is a folder which doesn't exist then create folder
		// if destination is a folder then create inside folder using original file leafname
		} 
		else
		{
			destinationFile.remove()
		}
	}

	sourceFile.copy(dPath);
	
};

/**
 * Create a uniquely named backup copy of the file referenced by the provided path 
 * @alias Jaxer.File.backup
 * @param {String} sourcePath
 * 		The full or partial (to be resolved) path of the original file
 * @return {String}
 * 		The path to the backup copy of the file
 */
File.backup = function backup(sourcePath)
{
	var sPath = Dir.resolve(sourcePath);
	var sourceFile = new File(sPath);
	
	if (!sourceFile.isFile) 
	{
		throw sPath + " is not a file";
	};
	
	if (!sourceFile.exists) 
	{
		throw sourcePath + " does not exist";		
	};

	var destinationFile = new File(sPath);

	destinationFile.createUnique();
	destinationFile.remove();
	
	sourceFile.copy(destinationFile.path);
	
	return destinationFile.path;
	
};

/**
 * Moves the file from sourcePath to destinationPath, the orginal file is deleted from the file system.  
 * If the destination file exists it will be overwritten.
 * @alias Jaxer.File.move
 * @param {String} sourcePath
 * 		The full or partial (to be resolved) path of the original file
 * @param {String} destinationPath
 * 		The full or partial (to be resolved) path to the new file
 */
File.move = function move(sourcePath,destinationPath)
{
	File.copy(sourcePath,destinationPath);
	File.remove(sourcePath);
};

/**
 * Get/Set the file permissions for the File object
 * 
 * If the optional permissions parameter is provided chmod will set the permissions of the object to those provided.
 * 
 * this may be ignored/misreported by some versions of windows.
 * 
 * on Windows, you can only set the Read/Write bits of a file. And User/Group/Other will have the SAME 
 * settings based on the most-relaxed setting (Read 04, 040, 0400, Write 02, 020, 0200). When a file is created, 
 * by default it has both Read and Write permissions. Also, you cannot set the file permission to WRITE-ONLY, doing 
 * so would set it to read-write
 * 
 * @alias Jaxer.File.chmod
 * @param {String} path
 * 		The full or partial (to be resolved) path of the original file
 * @param {String} permissions
 * 		The file permissions to apply to the file referenced by the provided path, this number is an OCTAL representation of the permissions.
 * 		to indicate a number is in octal format in javascript the first digit must be a 0
 * @return {Number}
 * 		The file permissions from the file referenced by the provided path
 */
File.chmod = function chmod(path,permissions)
{
	var sPath = Dir.resolve(path);
	var sourceFile = new File(sPath);
	
	if (typeof permissions != 'undefined') 
	{
		sourceFile.permissions = permissions;
	}
	return parseInt("0" + sourceFile.permissions , 8) & 07777;
};

/**
 * Extracts the filename for the file referenced by the provided path
 * @alias Jaxer.File.filename
 * @param {String} filename
 * 		The full or partial (to be resolved) of the path
 * @return {String} 
 * 		The filename from the file referenced by the provided path
 */
File.filename = function filename(path)
{
	var sPath = Dir.resolve(path);
	var sourceFile = new File(sPath);
	return sourceFile.leaf;
};

/**
 * Return the file extension for the file referenced by the provided path
 * @alias Jaxer.File.extension
 * @param {String} path
 * 		The full or partial (to be resolved) path of the  file
 * @return {String} 
 * 		The extension of the file referenced by the provided path
 */
File.extension = function extension(path)
{
	var sPath = Dir.resolve(path);
	var sourceFile = new File(sPath);
	return sourceFile.ext;
};

/**
 * Return the dateModified for the file referenced by the provided path
 * @alias Jaxer.File.dateModified
 * @param {String} path
 * 		The full or partial (to be resolved) path of the  file
 * @return {String} 
 * 		The last modified date of file referenced by the provided path
 */
File.dateModified = function dateModified(path)
{
	var sPath = Dir.resolve(path);
	var sourceFile = new File(sPath);
	return sourceFile.dateModified;
};

/**
 * Extracts the path of the containing folder for the file referenced by the provided path
 * @alias Jaxer.File.parentPath
 * @param {String} parentPath
 * 		The full or partial (to be resolved) path of the  file
 * @return {String} 
 * 		The path to the parent folder of file referenced by the provided path
 */
File.parentPath = function parentPath(path)
{
	var sPath = Dir.resolve(path);
	var sourceFile = new File(sPath);
	return sourceFile.parentPath;
};

/**
 * Extracts the absolute path of the file referenced by the provided path
 * @alias Jaxer.File.absolutePath
 * @param {String} path
 * 		The full or partial (to be resolved) path of the  file
 * @return {String} 
 * 		The absolute path of the file referenced by the provided path
 */
File.absolutePath = function absolutePath(path)
{
	var sPath = Dir.resolve(path);
	var sourceFile = new File(sPath);
	return sourceFile.path;
};

/**
 * return a crc32 checksum calculated from the file referenced by the provided path
 * @alias Jaxer.File.checksum
 * @param {String} path
 * 		The full or partial (to be resolved) path of the  file
 * @return {String} 
 * 		The checksum of the file referenced by the provided path
 */
File.checksum = function checksum(path)
{
	var sPath = Dir.resolve(path);
	var contents = File.read(sPath);
	var crc = Util.CRC32.getStringCRC(contents);
	
	return crc;
};

/**
 * Add the provided text to the end of an existing file.
 * 
 * @alias Jaxer.File.append
 * @param {String} path
 * 		The full or partial (to be resolved) path to append to
 * @param {String} text
 * 		The text to append
 */
File.append = function append(path, text)
{
	var file = File.getOrCreate(path);
	
	try
	{
		file.open('a');
		for (var i=1; i<arguments.length; i++)
		{
			file.write(arguments[i].toString());
		}
	}
	finally
	{
		if (file)
		{
			file.close();
		}
	}
};

/**
 * Add a line to the end of an existing file.
 * 
 * @alias Jaxer.File.appendLine
 * @param {String} path
 * 		The full or partial (to be resolved) path to append to
 * @param {String} text
 * 		The text to append, as a new line
 */
File.appendLine = function appendLine(path, text)
{
	var args = new Array((arguments.length - 1) * 2);
	for (var i=1; i<arguments.length; i++)
	{
		args[(i - 1) * 2    ] = arguments[i];
		args[(i - 1) * 2 + 1] = "\n";
	}
	args.unshift(path);
	File.append.apply(null, args);
};

/**
 * Get a file object, and if the object doesn't exist then automagically create
 * it.
 * 
 * @alias Jaxer.File.getOrCreate
 * @param {String} path
 * 		The full or partial (to be resolved) path to get or create
 * @return {Jaxer.File}
 * 		The file, possibly newly-created
 */
File.getOrCreate = function getOrCreate(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (!file.exists)
	{
		file.create();
	}

	if (!file.isFile)
	{
		throw path+" is not a file" ;
	}

	return file;
};

/**
 * Delete a file (only if it already exists).
 * 
 * @alias Jaxer.File.remove
 * @param {String} path
 * 		The full or partial (to be resolved) path to delete
 */
File.remove = function remove(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (!file.isFile)
	{
		throw path+" is not a file" ;
	}

	if (file.exists)
	{
		file.remove();
	}
};

/**
 * Returns the MIME type (e.g. "text/html") of the file at the given path.
 * It uses a variety of methods to determine this: 
 * built-in mozilla entries, user-set prefs, the operating system, and category manager.
 * 
 * @alias Jaxer.File.mimeType
 * @param {String} path
 * 		The full or partial (to ne resolved) path of the file
 * @return {String}
 * 		The MIME type of the file, or null if not detected.
 */
File.mimeType = function mimeType(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	var mimeService = Components.classes["@mozilla.org/mime;1"].getService(Components.interfaces.nsIMIMEService);
	try 
	{
		return mimeService.getTypeFromFile(file._nsIFile);
	}
	catch (e)
	{
		if (e.result == NS_ERROR_NOT_AVAILABLE)
		{
			return null;
		}
		else
		{
			throw e;
		}
	}
}

/**
 * Generate an MD2 Hash key for the file. 
 * 
 * @alias Jaxer.File.MD2
 * @see Jaxer.Util.Crypto.File.MD2
 * @param {String} path
 * 		the absolute path to the file 
 * @return {String} 
 * 		A string containing the MD2 hash key for the provided file
 */
File.MD2 = function MD2(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.MD2(file);
};

/**
 * Generate an MD5 Hash key for the file. 
 * 
 * @alias Jaxer.File.MD5
 * @see Jaxer.Util.Crypto.File.MD5
 * @param {String} path
 * 		the absolute path to the file 
 * @return {String} 
 * 		A string containing the MD5 hash key for the provided file
 */
File.MD5 = function MD5(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.MD5(file);
};

/**
 * Generate an SHA1 Hash key for the file. 
 * 
 * @alias Jaxer.File.SHA1
 * @see Jaxer.Util.Crypto.File.SHA1
 * @param {String} path
 * 		the absolute path to the file 
 * @return {String} 
 * 		A string containing the SHA1 hash key for the provided file
 */
File.SHA1 = function SHA1(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.SHA1(file);
};

/**
 * Generate an SHA256 Hash key for the file. 
 * 
 * @alias Jaxer.File.SHA256
 * @see Jaxer.Util.Crypto.File.SHA256
 * @param {String} path
 * 		the absolute path to the file 
 * @return {String} 
 * 		A string containing the SHA256 hash key for the provided file
 */
File.SHA256 = function SHA256(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.SHA256(file);
};

/**
 * Generate an SHA384 Hash key for the file. 
 * 
 * @alias Jaxer.File.SHA384
 * @see Jaxer.Util.Crypto.File.SHA384
 * @param {String} path
 * 		the absolute path to the file 
 * @return {String} 
 * 		A string containing the SHA384 hash key for the provided file
 */
File.SHA384 = function SHA384(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.SHA384(file);
};

/**
 * Generate an SHA512 Hash key for the file. 
 * 
 * @alias Jaxer.File.SHA512
 * @see Jaxer.Util.Crypto.File.SHA512
 * @param {String} path
 * 		the absolute path to the file 
 * @return {String} 
 * 		A string containing the SHA512 hash key for the provided file
 */
File.SHA512 = function SHA512(path)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.SHA512(file);
};

/**
 * Verify an MD2 Hash key for the file. 
 * 
 * @alias Jaxer.File.MD2
 * @see Jaxer.Util.Crypto.File.MD2
 * @param {String} path
 * 		the absolute path to the file 
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */
File.MD2 = function MD2(path,hash)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.verifyMD2(file);
};

/**
 * Verify an MD5 Hash key for the file. 
 * 
 * @alias Jaxer.File.MD5
 * @see Jaxer.Util.Crypto.File.MD5
 * @param {String} path
 * 		the absolute path to the file 
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */
File.MD5 = function MD5(path,hash)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.verifyMD5(file);
};

/**
 * Verify an SHA1 Hash key for the file. 
 * 
 * @alias Jaxer.File.SHA1
 * @see Jaxer.Util.Crypto.File.SHA1
 * @param {String} path
 * 		the absolute path to the file 
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */
File.SHA1 = function SHA1(path,hash)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.verifySHA1(file);
};

/**
 * Verify an SHA256 Hash key for the file. 
 * 
 * @alias Jaxer.File.SHA256
 * @see Jaxer.Util.Crypto.File.SHA256
 * @param {String} path
 * 		the absolute path to the file 
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */
File.SHA256 = function SHA256(path,hash)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.verifySHA256(file);
};

/**
 * Verify an SHA384 Hash key for the file. 
 * 
 * @alias Jaxer.File.SHA384
 * @see Jaxer.Util.Crypto.File.SHA384
 * @param {String} path
 * 		the absolute path to the file 
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */
File.SHA384 = function SHA384(path,hash)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.verifySHA384(file);
};

/**
 * Verify an SHA512 Hash key for the file. 
 * 
 * @alias Jaxer.File.SHA512
 * @see Jaxer.Util.Crypto.File.SHA512
 * @param {String} path
 * 		the absolute path to the file 
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */
File.SHA512 = function SHA512(path,hash)
{
	var fullPath = Dir.resolve(path);
	var file = new File(fullPath);
	
	if (file.exists && !file.isFile)
	{
		throw path+" is not a file" ;
	}

	return Jaxer.Util.Crypto.File.verifySHA512(file);
};

Jaxer.File = File;

Log.trace("*** File.js loaded");

})();
