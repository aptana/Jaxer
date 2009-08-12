/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The original developers of this code are the contributors to the 
 * JSLib project at http://jslib.mozdev.org.
 * Portions created by the original developers are Copyright (C) 2000-2007.
 * All Rights Reserved.
 *
 * This code is derived from a subset of the JSLib code and has been
 * modified by Aptana, Inc. in 2008. The portions modified by Aptana, Inc.
 * are Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved.
 *
 * This file is distributed by Aptana, Inc. as part of its Jaxer program.
 * A copy of the MPL, and related compliance information in a file titled 
 * license-jslib.html, are provided in the main distribution folder of this program.
 *
 * ***** END LICENSE BLOCK ***** */
(function(){  
	
    // TODO FIX - permissions seems to be ignored on folder objects
    
    /**
     * @classDescription {JSLib.Dir} This is the class that wraps the XPCOM directory/folder handling
     */
    
	/**
     * Creates a new Directory handle for performing filesystem directory operations.
     * 
     * @constructor
	 * @extends {JSLib.Filesystem}
     * @alias JSLib.Dir
     * @param {String} aPath 
     *  	a java style path object where double backslash is used as a folder hierarchy delimiter
     * @exception {Error} 
     * 		Throws an exception containing the error code.
     * @return {JSLib.Dir} 
     * 		a Dir object reference initialized to the provided path.
     */
    function Dir(aPath)
    {
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_DIR_XPC_NOT_ENOUGH_ARGS");
        return this.initPath((JSLib.typeIsObj(aPath) ? aPath.path : arguments));
    }
    
    Dir.prototype = new JSLib.Filesystem;
	Dir.prototype._nsIFile = null;
        
    /**
     * Creates a new folder under the referenced path of the directory object
     * 
	 * The format of the permissions is a unix style numeric chmod i.e. 0777 or 444
	 * 
	 * on Windows, you can only set the Read/Write bits of a file. And User/Group/Other will have the SAME 
     * settings based on the most-relaxed setting (Read 04, 040, 0400, Write 02, 020, 0200). When a file is created, 
     * by default it has both Read and Write permissions. Also, you cannot set the file permission to WRITE-ONLY, doing 
     * so would set it to read-write
	 *		
     * @method
     * @alias JSLib.Dir.prototype.create
     * @param {String} aPermissions
     * 		The permissions used to create the filesystem object.
     * 
     * @exception {Error} 
     * 		Throws a Exception containing the error code if filesytem object is unable to be created.
     */
    Dir.prototype.create = function(aPermissions)
    {
        JSLib.raiseIfTrue(this.exists, "NS_ERROR_DIR_FILE_ALREADY_EXISTS : " + this._path);
        
        var checkedPerms;
        
        if (JSLib.typeIsNum(aPermissions)) 
        {
            checkedPerms = this.validatePermissions(aPermissions);
            JSLib.raiseIfTrue(!checkedPerms, "NS_ERROR_DIR_INVALID_ARG "+ this._path);
            checkedPerms = aPermissions;
        }
        else 
        {
            var p = this._nsIFile.parent;
            while (p && !p.exists()) 
			{
				p = p.parent;
			}
            
            checkedPerms = p.permissions;
        }
        
        if (!checkedPerms) 
		{
			checkedPerms = JSLib.DIR_DEFAULT_PERMS;
		}
		
        try 
        {
            this._nsIFile.create(JSLib.DIR_DIRECTORY, checkedPerms);
         } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e);
        }
        
    };
	
	/**
	 * Creates a hierarchy of folders as needed to contain the current folder's path.
	 * The format of the permissions is the same as for the create method.
	 * 
     * @method
     * @alias JSLib.Dir.prototype.createHierarchy
     * @param {String} aPermissions
     * 		The permissions used to create al the filesystem objects
     * 
     * @exception {Error} 
     * 		Throws a Exception containing the error code if any filesytem object is unable to be created.
	 */
	Dir.prototype.createHierarchy = function(aPermissions)
	{
		var current = this._nsIFile;
		var toCreate = [];
		// First walk up until we find something that exists, accumulating directories to create
		while (current && !current.exists())
		{
			try
			{
				toCreate.unshift(new Dir(current.path));
				current = current.parent;
			}
			catch (e)
			{
				JSLib.ExceptionHandler(e);
			}
		}
		// Then walk back down, creating as we go along
		toCreate.forEach(function(dir)
		{
			dir.create(aPermissions);
		});
	}
    
    /**
     * Create a new unique folder under the referenced path of the directory object
     * 
	 * The format of the permissions is a unix style numeric chmod i.e. 0777 or 444
	 * 
	 * on Windows, you can only set the Read/Write bits of a file. And User/Group/Other will have the SAME 
     * settings based on the most-relaxed setting (Read 04, 040, 0400, Write 02, 020, 0200). When a file is created, 
     * by default it has both Read and Write permissions. Also, you cannot set the file permission to WRITE-ONLY, doing 
     * so would set it to read-write

     * @method
     * @alias JSLib.Dir.prototype.createUnique
     * @param {String} aPermissions
     * 		The permissions used to create the filesystem object, this may be ignored by some versions of windows.
     * @exception {Error} 
     * 		Throws an Exception containing the error code.
     */
    Dir.prototype.createUnique = function(aPermissions)
    {
        this.check();
        
        var checkedPerms;
        if (JSLib.typeIsNum(aPermissions)) 
        {
            checkedPerms = this.validatePermissions(aPermissions);
            
            if (!checkedPerms) 
			{
				throw new Exception("NS_ERROR_DIR_INVALID_ARG"+ this._path);
			}
			
            checkedPerms = aPermissions;
        }
        else 
        {
            var p = this._nsIFile.parent;
            while (p && !p.exists()) 
			{
				p = p.parent;
			}
            checkedPerms = p.permissions;
        }
        
        if (!checkedPerms) 
		{
			checkedPerms = JSLib.DIR_DEFAULT_PERMS;
		}
        
        try 
        {
            this._nsIFile.createUnique(JSLib.DIR_DIRECTORY, checkedPerms);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * Read the contents of a directory
     * 
     * @method
     * @alias JSLib.Dir.prototype.readDir
     * @exception {Error} 
     * 		Throws an Exception containing the error code.
     * @return {Jaxer.Filesystem[]} 
     * 		an Array of Filesystem Objects, with no sort order explicitly set.
     */
    Dir.prototype.readDir = function()
    {
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_DIR_FILE_TARGET_DOES_NOT_EXIST "+ this._path);
        
        try 
        {
            JSLib.raiseIfTrue(!this.isDir, "NS_ERROR_DIR_FILE_NOT_DIRECTORY "+ this._path);
            
            var files = this._nsIFile.directoryEntries;
            var listings = new Array();
            var file;
            
            while (files.hasMoreElements()) 
            {
                file = files.getNext().QueryInterface(Components.interfaces.nsILocalFile);
				
                if (file.isFile()) 
				{
					listings.push(new JSLib.Filesystem(file.path));
				}
				
                if (file.isDirectory()) 
				{
					listings.push(new JSLib.Dir(file.path));
				}
            }
            
            return listings;
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * Clone a directory object
     * 
     * @method
     * @alias JSLib.Dir.prototype.clone
     * @exception {Error} 
     * 		Throws an Exception containing the error code.
     * @return {JSLib.Dir} 
     * 		an new Dir Object cloned from the original
     */
    Dir.prototype.clone = function()
    {
        this.check();
        return new JSLib.Dir(this._path);
    };
    
    /**
     * Tests whether a file Object exists in the Physical directory referenced by the Dir object
     * 
     * @method
     * @alias JSLib.Dir.prototype.contains
     * @param {Object} aFileObj
     * 		the aFileObj parameter may be either a JSLib file object or a string containing the name of the object.
     * @exception {Error} 
     * 		Throws an Exception containing the error code.
     * @return {Boolean} 
     * 		True/False indicates whether the file was found;
     */
    Dir.prototype.contains = function(aFileObj)
    {
        JSLib.raiseIfTrue(!aFileObj, "NS_ERROR_DIR_INVALID_ARG "+ this._path);
        
        this.check();
        
        try 
        {
            var fo = (typeof(aFileObj.nsIFile) == "object") ? aFileObj.nsIFile : fo = aFileObj;
            return this._nsIFile.contains(fo, true);
        } 
        catch (e) 
        {
            return false;
        }
    };
    
    /**
     * Removes specified folder from the file system
     * 
     * @method
     * @alias JSLib.Dir.prototype.remove
     * @param {Boolean} aRecursive
     * 		True/False value to indicate whether the removal includes subfolders.
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     */
    Dir.prototype.remove = function(aRecursive)
    {
        if (typeof(aRecursive) != 'boolean') 
		{
			aRecursive = false;
		}
        
        JSLib.raiseIfTrue(!this._path,    "NS_ERROR_DIR_INVALID_ARG "+ this._path);
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_DIR_FILE_TARGET_DOES_NOT_EXIST "+ this._path);
        JSLib.raiseIfTrue(!this.isDir,  "NS_ERROR_DIR_FILE_NOT_DIRECTORY "+ this._path);
        
        try 
        {
            this._nsIFile.remove(aRecursive);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    JSLib.Dir = Dir;

})();
