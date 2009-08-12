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

    /**
     * @classDescription {JSLib.FileUtils} This is a general class that wraps XPCOM filesystem functionality and from which File and Dir objects are derived.
     */
	
    /**
     * A helper object for filesystem access
     * 
     * @constructor
     * @alias JSLib.FileUtils
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {JSLib.FileUtils} Returns an instance of FileUtils.
     */
    function FileUtils()
    {
        this._dirUtils = new JSLib.DirUtils;
        
        // make compatible w/ nsIFile API
        this.leafName = this.leaf;
        this.fileSize = this.size;
        this.copyTo = this.copy;
    }
    
    FileUtils.prototype._dirUtils = null;
    
    /**
     * Convert a chrome path to a filesystem path
     * 
     * @alias JSLib.FileUtils.prototype.chromeToPath
     * @param {String} a chrome path
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {String} 
     * 		the chrome path as a filesystem path
     */
    FileUtils.prototype.chromeToPath = function chromeToPath(aPath)
    {
    
        JSLib.raiseIfTrue(!aPath || !/^chrome:/.test(aPath), "NS_ERROR_FTL_INVALID_ARG");
        
        var rv;
        
        try 
        {
            var ios = JSLib.getService(JSLib.FILEUTILS_IO_SERVICE_CID, "nsIIOService");
            var uri = ios.newURI(aPath, "UTF-8", null);
            var cr = JSLib.getService(JSLib.FILEUTILS_CHROME_REG_PROGID, "nsIChromeRegistry");
            rv = cr.convertChromeURL(uri);
            
            if (!JSLib.typeIsString(rv)) 
			{
				rv = cr.convertChromeURL(uri).spec;
			}
            
            // preserve the zip entry path "!/browser/content/browser.xul"
            // because urlToPath will flip the "/" on Windows to "\"
            var jarPath = "";
            if (/jar:/.test(rv)) 
            {
                rv = rv.replace(/jar:/, "");
                var split = rv.split("!");
                rv = split[0];
                jarPath = "!" + split[1];
            }
            
            if (/resource:/.test(rv)) //TODO check case specific scan
			{
				rv = rv.replace(/.*resource:/, this._dirUtils.getCurProcDir());
			}
            
            if (/^file:/.test(rv)) //TODO check case specific scan 
			{
				rv = this.urlToPath(rv);
			}
			else 
			{
				rv = this.urlToPath("file://" + rv);
			}
            
            rv += jarPath;
            
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
        return rv;
    };
    
    
    /**
     * Convert a file protocol URL to a filesystem path
     * 
     * @alias JSLib.FileUtils.prototype.urlToPath
     * @param {String} a URL
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {String} 
     * 		the URL as a filesystem path
     */
    FileUtils.prototype.urlToPath = function urlToPath(aPath)
    {
        JSLib.raiseIfTrue(!aPath || !/^file:/.test(aPath), "NS_ERROR_FTL_INVALID_ARG");
        
        try 
        {
            var ph = JSLib.createInstance(JSLib.FILEUTILS_FILEPROTOCOL_CID, "nsIFileProtocolHandler");
            return ph.getFileFromURLSpec(aPath).path;
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Convert a filesystem path to a file protocol URL
     * 
     * @alias JSLib.FileUtils.prototype.pathToURL
     * @param {String} a path
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {String} 
     * 		the filesystem path as a URL
     */
    FileUtils.prototype.pathToURL = function pathToUrl(aPath)
    {
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        try 
        {
            var ph = JSLib.createInstance(JSLib.FILEUTILS_FILEPROTOCOL_CID, "nsIFileProtocolHandler");
            return ph.getURLSpecFromFile(this.nsIFile(aPath));
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * A boolean indicator of whether the referenced object physically exists on the filesystem.
     * 
     * @method
     * @alias JSLib.FileUtils.prototype.exists
     * @exception	{Error} 
     * 		Throws a Exception containing the error code.
     * @return {Boolean} 
     * 		true if file exists otherwise false
     */
   FileUtils.prototype.exists = function exists(aPath)
    {
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        
        try 
        {
            return (new JSLib.FILEUTILS_nsIFile(aPath)).exists();
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Removes the referenced object physically from the filesystem.
     * 
     * @method
     * @alias JSLib.FileUtils.prototype.remove
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     */
    FileUtils.prototype.remove = function remove(aPath)
    {
    
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        JSLib.raiseIfTrue(!this.exists(aPath), "NS_ERROR_FTL_FILE_TARGET_DOES_NOT_EXIST");
        
        try 
        {
            var nsIFile = new JSLib.FILEUTILS_nsIFile(aPath);
            JSLib.raiseIfTrue(nsIFile.isDirectory(), "NS_ERROR_FTL_FILE_IS_DIRECTORY");
            nsIFile.remove(false);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Copies the source path to the destination path
     * @alias JSLib.FileUtils.prototype.copy
     * @param {String}  source path
     * 		The source path
     * @param {String}  destination path
     * 		The destination apth
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     */
    FileUtils.prototype.copy = function copy(aSource, aDest)
    {
    
        JSLib.raiseIfTrue(!aSource || !aDest, "NS_ERROR_FTL_INVALID_ARG");
        JSLib.raiseIfTrue(!this.exists(aSource), "NS_ERROR_FTL_UNEXPECTED");
        
        try 
        {
            var nsIFile = new JSLib.FILEUTILS_nsIFile(aSource);
            var dir = new JSLib.FILEUTILS_nsIFile(aDest);
            var copyName = nsIFile.leafName;
            
            JSLib.raiseIfTrue(nsIFile.isDirectory(), "NS_ERROR_FTL_FILE_IS_DIRECTORY");
            
            if (!this.exists(aDest) || !dir.isDirectory()) 
            {
                copyName = dir.leafName;
                dir = new JSLib.FILEUTILS_nsIFile(dir.path.replace(copyName, ''));
                
                JSLib.raiseIfTrue(!this.exists(dir.path), "NS_ERROR_FTL_FILE_ALREADY_EXISTS");
                JSLib.raiseIfTrue(!dir.isDirectory(), "NS_ERROR_FTL_FILE_INVALID_PATH");
            }
            
            JSLib.raiseIfTrue(this.exists(this.append(dir.path, copyName)), "NS_ERROR_FTL_FILE_ALREADY_EXISTS");
            
            nsIFile.copyTo(dir, copyName);
            
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
 	/**
     * Returns the leaf (filename + extension) portion of the file path
     *  
     * @method
     * @alias JSLib.FileUtils.prototype.leaf
     * @param {String} 
     * 		a filesystem path
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {String} 
     * 		The leaf name
     */
    FileUtils.prototype.leaf = function leaf(aPath)
    {
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        
        try 
        {
            var nsIFile = new JSLib.FILEUTILS_nsIFile(aPath);
            return nsIFile.leafName;
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * Changes the path of the filesystem object by appending the provided leaf value.
     * 
     * @alias JSLib.FileUtils.prototype.append
     * @param {String}  leafname
     * 		The leafname to be appended
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {String} 
     * 		The appended directory and leafname
     */
    FileUtils.prototype.append = function append(aDirPath, aFileName)
    {
    
        JSLib.raiseIfTrue(!aDirPath || !aFileName, "NS_ERROR_FTL_INVALID_ARG");
        JSLib.raiseIfTrue(!this.exists(aDirPath), "NS_ERROR_FTL_FILE_NOT_FOUND");
        
        try 
        {
            var nsIFile = new JSLib.FILEUTILS_nsIFile(aDirPath);
            
            JSLib.raiseIfTrue(nsIFile.exists() && !nsIFile.isDirectory(), "NS_ERROR_FTL_INVALID_ARG");
            
            nsIFile.append(aFileName);
            var rv = nsIFile.path;
            delete nsIFile;
            return rv;
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * This function will validate the numeric permissions value provided, if true
     * the permissions value is valid, if false the value is invalid.
     * 
     * @alias JSLib.FileUtils.prototype.validatePermissions
     * @param {Number} the chmod style permission (0777,0622 etc)
     * @exception	{Error} Throws a Exception containing the error code.
     * @return {Boolean} indicates whether the permissions are valid
     */
    FileUtils.prototype.validatePermissions = function(aNum)
    {
        return (parseInt(aNum.toString(10).length) >= 3)
    };
    
    /**
     * Get the file permissions for the File object
     * 
     * @method
     * @alias JSLib.FileUtils.prototype.permissions
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {String} 
     * 		the chmod style permissions of the file
     */
    FileUtils.prototype.permissions = function permissions(aPath)
    {
    
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        JSLib.raiseIfTrue(!this.exists(aPath), "NS_ERROR_FTL_FILE_NOT_FOUND");
        
        try 
        {
            return (new JSLib.FILEUTILS_nsIFile(aPath)).permissions.toString(8);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * The last modified timestamp as reported by the OS
     * 
     * @method
     * @alias JSLib.FileUtils.prototype.dateModified
     * @exception	{Error} 
     * 		Throws a Exception containing the error code.
     * @return {Object} 
     * 		a date object representing the last modified timestamp of the file
     */
    FileUtils.prototype.dateModified = function dateModified(aPath)
    {
    
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        JSLib.raiseIfTrue(!this.exists(aPath), "NS_ERROR_FTL_FILE_NOT_FOUND");
        
        try 
        {
            return new Date((new JSLib.FILEUTILS_nsIFile(aPath)).lastModifiedTime).toLocaleString();
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Returns the size in bytes as reported by the OS
     * 
     * @alias JSLib.FileUtils.prototype.size
     * @param {String}  source path
     * 		The path to the filesystem object
     * @exception	{Error} 
     * 		Throws a Exception containing the error code.
     * @return {Number} 
     * 		the size of the file in bytes
     */
    FileUtils.prototype.size = function size(aPath)
    {
    
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        JSLib.raiseIfTrue(!this.exists(aPath), "NS_ERROR_FTL_FILE_NOT_FOUND");
        
        try 
        {
            return (new JSLib.FILEUTILS_nsIFile(aPath)).fileSize;
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Returns the extension of the file object
     * 
     * @alias JSLib.FileUtils.prototype.ext
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new File(p);
     *		 alert(f.ext);
     * </pre>
     *	 outputs: dat
     * @param {String} aPath
     * 		The path to the filesystem object 
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {String} 
     * 		the file extension of the referenced filesytem object
     */
    FileUtils.prototype.ext = function ext(aPath)
    {
    
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        JSLib.raiseIfTrue(!this.exists(aPath), "NS_ERROR_FTL_FILE_NOT_FOUND");
        
        try 
        {
            var leafName = (new JSLib.FILEUTILS_nsIFile(aPath)).leafName;
            var dotIndex = leafName.lastIndexOf('.');
            return (dotIndex >= 0) ? leafName.substring(dotIndex + 1) : "";
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
	/**
     * Returns the path of the parent filesystem object
     *  
     * @method
	 * @alias JSLib.FileUtils.prototype.parent
     * @param {String} aPath
     * 		The path to the filesystem object 
	 * @return {String}
	 * 		The path of the parent file system object
	 */
    FileUtils.prototype.parent = function parent(aPath)
    {
    
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        
        try 
        {
            var nsIFile = new JSLib.FILEUTILS_nsIFile(aPath);
            
            JSLib.raiseIfTrue(!nsIFile.exists(), "NS_ERROR_FTL_FILE_NOT_FOUND");
            
            if (nsIFile.isFile()) 
			{
				return nsIFile.parent.path;
			}
			else 
			{
				return (nsIFile.isDirectory()) ? nsIFile.path : null;
			}
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Trys to execute the requested file as a separate
     * *non-blocking* process.
     * Passes the supplied *array* of arguments on the command line if
     * the OS supports it.
     * 
     * @alias JSLib.FileUtils.prototype.run
     * @param {String} aPath
     * 		a filesystem path
     * @param {Array}  aArgs
     * 		a set of arguments passed to the command to be run
     * @exception	{Error} 
     * 		Throws a Exception containing the error code.
     * @return {Number} 
     * 		the exit code of the process
     */
    FileUtils.prototype.run = function run(aPath, aArgs)
    {
    
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        JSLib.raiseIfTrue(!this.exists(aPath), "NS_ERROR_FTL_FILE_NOT_FOUND");
        
        var len = 0;
        if (aArgs) 
		{
			len = aArgs.length;
		}
		else 
		{
			aArgs = null;
		}
        
        try 
        {
            var nsIFile = new JSLib.FILEUTILS_nsIFile(aPath);
            
            // XXX commenting out this check as it fails on OSX 
            // if (!nsIFile.isExecutable()) 
            // throw new Exception("NS_ERROR_FTL_INVALID_ARG");
            
            JSLib.raiseIfTrue(nsIFile.isDirectory(), "NS_ERROR_FTL_FILE_IS_DIRECTORY");
            
            /* 
             * Create and execute the process ...
             *
             * NOTE: The first argument of the process instance's 'run' method
             *			 below specifies the blocking state (false = non-blocking).
             *			 The last argument, in theory, contains the process ID (PID)
             *			 on return if a variable is supplied--not sure how to implement
             *			 this with JavaScript though.
             */
            var theProcess = JSLib.createInstance(JSLib.FILEUTILS_PROCESS_CID, "nsIProcess");
            theProcess.init(nsIFile);
            return theProcess.run(false, aArgs, len);
            
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Creates a file on the filesystem with the default permissions
     * 
     * @alias JSLib.FileUtils.prototype.create
     * @param {String} aPath
     * 		a file system path
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     */
    FileUtils.prototype.create = function create(aPath)
    {
    
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        
        try 
        {
            var f = new JSLib.FILEUTILS_nsIFile(aPath);
            f.create(f.NORMAL_FILE_TYPE, 0644);
            rv = JSLib.OK;
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Returns a boolean indicator that is true for a valid path and false otherwise
     * 
     * @alias JSLib.FileUtils.prototype.isValidPath
     * @param {String}  aPath
     * 		a file system path
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {Boolean} 
     * 		true if valid path was requested
     */
    FileUtils.prototype.isValidPath = function isValidPath(aPath)
    {
    
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        
        try 
        {
            var f = new JSLib.FILEUTILS_nsIFile(aPath);
            return true;
        } 
        catch (e) 
        {
            return false;
        }
    };
    
    /**
     * Returns an nsIFIle object for the specified path
     * 
     * @alias JSLib.FileUtils.prototype.nsIFile
     * @param {String}  aPath
     * 		a file system path
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {Object} 
     * 		nsIFile
     */
    FileUtils.prototype.nsIFile = function nsIFile(aPath)
    {
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FTL_INVALID_ARG");
        
        try 
        {
            return new JSLib.FILEUTILS_nsIFile(aPath);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    JSLib.FileUtils = FileUtils;
   
})();
