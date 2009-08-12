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
(function() {

    /* TODO
     * add copious log.trace / log.debug messages
     */
	
	/**
	 * @classDescription {JSLib.Filesystem} File System Object contains methods useful for accessing the basic file and directory objects.
	 */
	
    /**
     * 
     * File System Object. This object is used as a functional wrapper
     * to the file system. It can represent, folders, files, symlinks
     * 
     * @constructor
     * @alias JSLib.Filesystem
     * @param {Object} fsPath
     * 		The path to the filesystem object
     * @return {JSLib.Filesystem} Returns an instance of Filesystem.
     */
    function Filesystem(fsPath)
    {
    
        JSLib.raiseIfTrue((fsPath < 0), "Provided Path Argument is invalid");
        
        // support nsIFile method names

        this.initWithPath = this.initPath;
        this.moveTo = this.move;
        this.copyTo = this.copy;
        
        return (fsPath ? this.initPath(arguments) : JSLib.VOID);
        
    }
    
    Filesystem.prototype = 
    {
        _path: null,
        _nsIFile: null,
        
        /**
         * This method will initialize the file system object with the provided path information (or will attempt to derive the path if an object is provided).
         * An existing File object can be 'repointed' to a new physical file sytem object by invoking this method.
         * 
         * @method
         * @alias JSLib.Filesystem.prototype.initPath
         * @param {Array} a set of arguments
         * @exception {Error} 
         * 		Throws a Exception containing the error code.
         * @return {String} 
         * 		the file extension of the referenced filesystem object
         */
        initPath: function initPath(args)
        {
            // check if the argument is a file:// url
            var fileURL;
            
            function getFileURL(spec)
            {
                fileURL = new JSLib.URL;
                fileURL.spec = spec;
                return fileURL.path;
            }
            
            if (JSLib.typeIsObj(args)) 
            {
                for (var i = 0; i < args.length; i++) 
                {
                    if (args[i].search(/^file:/i) == 0) 
					{
						args[i] = getFileURL(args[i]);
					}
                }
            }
            else 
            {
                if (args.search(/^file:/i) == 0) 
				{
					args = getFileURL(args);
				}
            }
            
            /* 
             * If you are wondering what all this extra cruft is, well
             * this is here so you can reinitialize 'this' with a new path
             */
            var pathToFile = null;
            try 
            {
                if (typeof(args.path) == "string") 
                {
                    JSLib.raiseIfTrue(typeof(args.isDirectory) == "function" && args.isDirectory(), "NS_ERROR_FS_FILE_IS_DIRECTORY");
                    
                    this._nsIFile = new JSLib.File_nsIFile(args.path);
                    pathToFile = this._nsIFile.path;
                }
                else 
                    if (JSLib.typeIsObj(args)) 
                    {
                        this._nsIFile = new JSLib.File_nsIFile(args[0] ? args[0] : this._path);
                        if (args.length > 1) 
                        {
                            for (i = 1; i < args.length; i++) 
                            {
                                this._nsIFile.append(args[i]);
                            }
                        }
                        pathToFile = (args[0] || this._path) ? this._path = this._nsIFile.path : null;
                    }
                    else 
                    {
                        this._nsIFile = new JSLib.File_nsIFile(args ? args : this._path);
                        pathToFile = (this._nsIFile.path ? this._nsIFile.path : null);
                    }
            } 
            catch (e) 
            {
                JSLib.ExceptionHandler(e)
            }
            
            this._path = pathToFile
            return pathToFile;
        }
    };
    
    /**
     * A helper function that will first check to see if the current filesystem instance is correctly initialised and then perform the
     * provided test.
     * 
     * @private
     * @alias JSLib.Filesystem.prototype.check
     * @param {Function}
     * 		A function to be run that returns true/false
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {Boolean} 
     * 		results of running the function which was passed as parameter
     */
    Filesystem.prototype.check = function check(is)
    {
    
        //TODO verify this test is sufficient.
        JSLib.raiseIfTrue(!this._nsIFile && this._path, "File System Object must be Initialized before use");
        
        return (is) ? is() : true;
    };
    
    /**
     * Truncates the file referenced by the filesystem object.
     * 
     * @method
     * @alias JSLib.Filesystem.prototype.resetCache
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {Boolean} 
     * 		true indicates success
     */
    Filesystem.prototype.resetCache = function resetCache()
    {
    
        this.check();
        
        if (this._path) 
        {
            delete this._nsIFile;
            this._nsIFile = new JSLib.File_nsIFile(this._path);
            return true;
        }
        else 
        {
            return false;
        }
    };
    
    /**
     * Evaluates whether the current filesystem object refers to the same file as the one provided as a parameter
     * 
     * @method
     * @alias JSLib.Filesystem.prototype.equals
     * @param {Object} 
     * 		a file system object to be compared
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {Boolean} 
     * 		true if object refers to same filesystem object, false otherwise
     */
    Filesystem.prototype.equals = function equals(aFileObj)
    {
		
		this.check();

        var fo;
        if (typeof(aFileObj.nsIFile) == "object") 
		{
			fo = aFileObj.nsIFile;
		}
		else 
		{
			fo = aFileObj;
		}
        
        return this._nsIFile.equals(fo);
    
    };
    
    /**
     * A boolean indicator of whether the referenced object physically exists on the filesystem.
     * 
     * @alias JSLib.Filesystem.prototype.exists
     * @property {Boolean} 
     * 		true if file exists otherwise false
     */
    Filesystem.prototype.__defineGetter__('exists', function()
    {
        this.check();
        return this._nsIFile.exists();
    });
    
    /**
     * A boolean indicator of whether the referenced object is a folder/directory
     * 
     * @alias JSLib.Filesystem.prototype.isDir
     * @property {Boolean} 
     * 		true if item a folder
     */
    Filesystem.prototype.__defineGetter__('isDir', function()
    {
        this.check();
        return this._nsIFile.isDirectory();
    });

    /**
     * A boolean indicator of whether the referenced object is a folder/directory
     * an alias of as isDir
     * 
     * @alias JSLib.Filesystem.prototype.isFolder
     * @property {Boolean} 
     * 		true if item a folder
     */
    Filesystem.prototype.__defineGetter__('isFolder', function()
    {
        this.check();
        return this.isDir;
    });

    Filesystem.prototype.isDirectory = function isDirectory()
    {
        return this.isDir;
    };

    /**
     * A boolean indicator of whether the referenced object is a file
     * 
     * @alias JSLib.Filesystem.prototype.isFile
     * @property {Boolean} 
     * 		true if item is a file
     */
    Filesystem.prototype.__defineGetter__('isFile', function()
    {
        this.check();
        return this._nsIFile.isFile();
    });
    
    /**
     * A boolean indicator of whether the referenced object is an executable
     * 
     * @alias JSLib.Filesystem.prototype.isExec
     * @property {Boolean} 
     * 		true if item executable
     */
    Filesystem.prototype.__defineGetter__('isExec', function()
    {
        this.check();
		
		if (Jaxer.System.OS == 'Darwin')
		{
			// manually check file permissions on mac as it returns
			// a strange format for permissions
			var p = this.permissions.toString();
			return ( (parseInt(p.substr(-1) & 1) + parseInt(p.substr(-2,1) & 1) + parseInt(p.substr(-3,1) & 1) ) > 0 );
		}
        return this._nsIFile.isExecutable();
    });
	
    Filesystem.prototype.isExecutable = function isExecutable()
    {
        return this.isExec;
    };
    
    /**
     * A boolean indicator of whether the referenced object is a symlink
     * 
     * @alias JSLib.Filesystem.prototype.isSymlink
     * @property {Boolean} 
     * 		true if item symbolic link
     */
    Filesystem.prototype.__defineGetter__('isSymlink', function()
    {
        this.check();
        return this._nsIFile.isSymlink();
    });

    /**
     * A boolean indicator of whether the referenced object is writable
     * 
     * @alias JSLib.Filesystem.prototype.isWritable
     * @property {Boolean} 
     * 		true if item is writable
     */
    Filesystem.prototype.__defineGetter__('isWritable', function()
    {
        this.check();
        return this._nsIFile.isWritable();
    });
    
    /**
     * A boolean indicator of whether the referenced object is readable
     * 
     * @alias JSLib.Filesystem.prototype.isReadable
     * @property {Boolean} 
     * 		true if item is readable
     */
    Filesystem.prototype.__defineGetter__('isReadable', function()
    {
        this.check()
        return this._nsIFile.isReadable();
    });
    
    /**
     * A boolean indicator of whether the referenced object is hidden
     * 
     * @alias JSLib.Filesystem.prototype.isHidden
     * @property {Boolean} 
     * 		true if item is hidden
     */
    Filesystem.prototype.__defineGetter__('isHidden', function()
    {
        this.check()
        return this._nsIFile.isHidden();
    });
    
    /**
     * A boolean indicator of whether the referenced object is special.
     * NOTE: Not implemented on Mac and possibly other systems.
     * 
     * @alias JSLib.Filesystem.prototype.isSpecial
     * @property {Boolean} 
     * 		true if item a special file
     */
    Filesystem.prototype.__defineGetter__('isSpecial', function()
    {
        this.check();
        return this._nsIFile.isSpecial();
    });
    
    /**
     * An indicator of whether the provided permissions value is valid
     * 
     * @private
     * @alias JSLib.Filesystem.prototype.validatePermissions
     * @param {Number} 
     * 		a chmod style permission value
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {Boolean} 
     * 		return value indicates success or fail
     */
    Filesystem.prototype.validatePermissions = function validatePermissions(aNum)
    {
        return (!(typeof(aNum) != 'number' || parseInt(aNum.toString(10).length) < 3));
    };
    
    /**
     * Move the referenced file to a new filesystem location provided as a parameter
     * NOTE: after a move, 'this' will be reinitialized to reference the moved file!
     * 
     * @alias JSLib.Filesystem.prototype.move
     * @param {String} 
     * 		destination path
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     *
     */
    Filesystem.prototype.move = function move(aDest)
    {
    
        JSLib.raiseIfTrue(!aDest, "NS_ERROR_FS_INVALID_ARG");
        
        this.check();
        
        if (typeof(aDest) == "object") 
		{
			if (typeof(aDest.path) != "string") 
			{
				throw new Exception("NS_ERROR_FS_INVALID_ARG");
			}
			else 
			{
				aDest = aDest.path;
			}
		}
        
        var newName = null;
        
        try 
        {
            var f = new JSLib.File_nsIFile(aDest);
            
            JSLib.raiseIfTrue(f.exists() && !f.isDirectory(), "NS_ERROR_FS_FILE_ALREADY_EXISTS "+ f._path);
            JSLib.raiseIfTrue(f.equals(this._nsIFile), "NS_ERROR_FS_FILE_COPY_OR_MOVE_FAILED "+ f._path);
            
            if (!f.exists() && f.parent.exists()) 
			{
				newName = f.leafName;
			}
            
            JSLib.raiseIfTrue(f.equals(this._nsIFile.parent) && !newName, "NS_ERROR_FS_FILE_IS_DIRECTORY "+ f._path);
            
            var dir = f.parent;
            
            if (dir.exists() && dir.isDirectory()) 
            {
                this._nsIFile.moveTo(dir, newName);
                this._path = f.path;
                this.resetCache();
                delete dir;
            }
            else 
            {
                throw new Exception("NS_ERROR_FS_INVALID_ARG");
            }
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Changes the path of the filesystem object by appending the provided leaf value.
     * 
     * @alias JSLib.Filesystem.prototype.append
     * @param {String}  leafname
     * 		The leafname to be appended
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {String} 
     * 		The appended directory and leafname
     */
    Filesystem.prototype.append = function append(aLeaf)
    {
    
        JSLib.raiseIfTrue(!aLeaf, "NS_ERROR_FS_INVALID_ARG");
        
        this.check();
        this._nsIFile.append(aLeaf);
        this._path = this._nsIFile.path;
        
    };
    
    /**
     * This method is used for appending a relative path to the current filesystem object
     * 
     * @alias JSLib.Filesystem.prototype.appendRelativePath
     * @method
     * @param {String}  
     * 		relative path
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     */
    Filesystem.prototype.appendRelativePath = function appendRelativePath(aRelPath)
    {
    
        JSLib.raiseIfTrue(!aRelPath, "NS_ERROR_FS_INVALID_ARG");
        
        this.check()
        this._nsIFile.appendRelativePath(aRelPath);
        this._path = this._nsIFile.path;
        
    };
    
    /**
     * As of Mozilla 1.7, the underlying XPCOM method is only implemented
     * under UNIX builds (except for Mac OSX).
     * This method will fail if the path does not exist.
     * 
     * @alias JSLib.Filesystem.prototype.normalize
     * @exception	{Error} 
     * 		Throws a Exception containing the error code.
     */
    Filesystem.prototype.normalize = function normalize()
    {
    
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FS_FILE_NOT_FOUND "+ this._path);
        
        try 
        {
            this._nsIFile.normalize();
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * Get/Set the file permissions for the File object.
     * 
     * this may be ignored/misreported by some versions of windows.
     * 
     * on Windows, you can only set the Read/Write bits of a file. And User/Group/Other will have the SAME 
     * settings based on the most-relaxed setting (Read 04, 040, 0400, Write 02, 020, 0200). When a file is created, 
     * by default it has both Read and Write permissions. Also, you cannot set the file permission to WRITE-ONLY, doing 
     * so would set it to read-write
     * 
     * @alias JSLib.Filesystem.prototype.permissions
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @property {String} 
     * 		the chmod style permissions of the file
     */
    Filesystem.prototype.__defineGetter__('permissions', function()
    {
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FS_FILE_NOT_FOUND "+ this._path);
        return parseInt(this._nsIFile.permissions.toString(8));
        
    });
	    
    Filesystem.prototype.__defineSetter__('permissions', function(aPermission)
    {
    
        JSLib.raiseIfTrue(!aPermission, "NS_ERROR_FS_INVALID_ARG ");
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FS_FILE_NOT_FOUND "+ this._path);
        JSLib.raiseIfTrue(!this.validatePermissions(aPermission), "NS_ERROR_FS_INVALID_ARG "+ this._path);
        
        this._nsIFile.permissions = aPermission;
    });
	  
    /**
     * The last modified timestamp as reported by the OS.
     * 
     * @alias JSLib.Filesystem.prototype.dateModified
     * @exception	{Error} 
     * 		Throws a Exception containing the error code.
     * @property {Object} 
     * 		a date object representing the last modified timestamp of the file
     */
    Filesystem.prototype.__defineGetter__('dateModified', function()
    {
    
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FS_FILE_NOT_FOUND "+ this._path);
        
        try 
        {
            return (new Date(this._nsIFile.lastModifiedTime));
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    });
        
    /**
     * Returns a clone of the underlying nsIFile object.
     * 
     * @alias JSLib.Filesystem.prototype.nsIFile
     * @param {String} aPermissions
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @property {Object} 
     * 		clone of the contained nsIFile
     */
    Filesystem.prototype.__defineGetter__('nsIFile', function()
    {
        return this._nsIFile.clone();
    });
   
    /**
     * Returns the path of the refererenced filesystem object.
     * 
     * @alias JSLib.Filesystem.prototype.path
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @property {String} 
     * 		path to nsIFile
     */
    Filesystem.prototype.__defineGetter__('path', function()
    {
        return this._nsIFile.path;
    });
    
    /**
     * Return the path of the referenced object as a file URL
     * 
     * 
     * @method
     * @alias JSLib.Filesystem.prototype.URL
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @property {String} 
     * 		the file path as a URL
     */
    Filesystem.prototype.__defineGetter__('URL', function()
    {
        //this.check(function(){
        var ph = JSLib.createInstance("@mozilla.org/network/protocol;1?name=file", "nsIFileProtocolHandler");
        return ph.getURLSpecFromFile(this._nsIFile);
        //});
    });
        
	/**
     * Get/Set the leaf (filename + extension) portion of the file path.
     *   
     * @method
	 * @alias JSLib.Filesystem.prototype.leaf
	 * @property {String}
	 * 		The derived URL spec from the file
	 */
    Filesystem.prototype.__defineGetter__('leaf', function()
    {
    
        this.check();
        try 
        {
            return this._nsIFile.leafName;
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    });
    
    Filesystem.prototype.__defineSetter__('leaf', function(aLeaf)
    {
    
        this.check();
        JSLib.raiseIfTrue(!aLeaf, "NS_ERROR_INVALID_ARG");
        
        try 
        {
            return (this._nsIFile.leafName = aLeaf);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    });
        
	/**
     * Returns the path of the parent filesystem object.
     * if called on the root filesystem object an exception
     * will be thrown. 
	 * @alias JSLib.Filesystem.prototype.leaf
	 * @property {String}
	 * 		The path of the parent file system object
	 */
    Filesystem.prototype.__defineGetter__('parentPath', function()
    {
    
        this.check();
        
        try 
        {
            return (this._nsIFile.parent.path);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    });
            
    JSLib.Filesystem = Filesystem;

})();
