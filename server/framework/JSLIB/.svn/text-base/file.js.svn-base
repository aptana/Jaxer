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

    /**
     * @classDescription {JSLib.File} 
     * 		This is the class that wraps the XPCOM file handling
     */
	
    /**
     * Creates a new File handle for performing filesystem file operations.
     * 
     * @example
     * <pre>
     *		 var p = '/tmp/foo.dat';
     *		 var f = new Jaxer.File(p);
     * </pre>
     * @constructor
	 * @extends {JSLib.Filesystem}
     * @alias JSLib.File
     * @param {String} aPath 
     *  	an argument of string local file path
     * @exception {Error} 
     *  	Throws a Exception containing the error code upon failure.
     *  @return {JSLib.File} Returns an instance of File.
     */
    function File(aPath)
    {
        JSLib.raiseIfTrue(!aPath, "NS_ERROR_FILE_INVALID_ARG");
        
        // if the argument is a File or nsIFile object	
        return this.initPath((JSLib.typeIsObj(aPath)) ? aPath.path : arguments);   
    }
    
    File.prototype = new JSLib.Filesystem;
    
    // member vars
    File.prototype._mode = null;
    File.prototype._isBinary = false;
    File.prototype._fileChannel = null;
    File.prototype._transport = null;
    File.prototype._URI = null;
    File.prototype._outStream = null;
    File.prototype._inputStream = null;
    File.prototype._lineBuffer = null;
    File.prototype._position = 0;
	
	// Create a private singleton converter to reuse
	var converter = JSLib.createInstance(JSLib.FILE_INTL_SCR_UNI_CONV_CID, JSLib.FILE_INTL_I_SCR_UNI_CONV);
	converter.charset = "UTF-8";
    
    /** 
     * Opens the file for reading or writing. The provided file mode can be one or two characters the 
     * using 'rb','ab','wb' will cause the file to be opened in binary safe mode.
     * 
     * NOTE: This implementation doesn't support file locking so will allow multiple open handles to 
     * the same file.
     * @alias JSLib.File.prototype.open
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 f.open();
     * </pre>
     * @param {String} aMode 
     *  an argument of string 'w', 'a', 'r', 'b'
     * @param {Number} aPermissions 
     *  a number containing the unix style chmod value for the permissions
     * @exception {Error} 
     *  Throws a Exception containing the error code.
     */
    File.prototype.open = function(aMode, aPerms)
    {
        this.check();
        
        
        // close any existing file handles
        this.close();
        
        JSLib.raiseIfTrue(this._mode, "NS_ERROR_FILE_NOT_INITIALIZED_ON_OPEN "+ this._path);
        
        if (!this._URI) 
        {
            if (!this.exists) 
			{
				this.create();
			}
			
            this._URI = JSLib.FILE_IOSERVICE.newFileURI(this._nsIFile);
        }
        
        JSLib.raiseIfTrue(this.exists && this._nsIFile.isDirectory(), "NS_ERROR_FILE_IS_DIRECTORY "+ this._path);

        if (!aMode) 
            aMode = JSLib.FILE_READ_MODE;
        
        this.resetCache();
        
        this._isBinary = false;
        var access;
        while (aMode.length > 0) 
        {
            switch (aMode[0])
            {
                case JSLib.FILE_WRITE_MODE:
                case JSLib.FILE_APPEND_MODE:
                case JSLib.FILE_READ_MODE:
                {
                    access = aMode[0];
                    break;
                }
                case JSLib.FILE_BINARY_MODE:
                {
                    this._isBinary = true;
                    break;
                }
                default:
                    throw new Exception("NS_ERROR_FILE_INVALID_ARG");
            }
            aMode = aMode.substring(1);
        }
        aMode = access;
        
        switch (aMode)
        {
            case JSLib.FILE_WRITE_MODE:
            case JSLib.FILE_APPEND_MODE:
            {
                try 
                {
                    if (!this._fileChannel) 
					{
						this._fileChannel = JSLib.FILE_IOSERVICE.newChannelFromURI(this._URI);
					}
                    
                } 
                catch (e) 
                {
                    JSLib.ExceptionHandler(e)
                }
                
                if (aPerms && this.validatePermissions(aPerms)) 
				{
					this._nsIFile.permissions = aPerms;
				}
                
                if (!aPerms) 
				{
					aPerms = JSLib.FILE_DEFAULT_PERMS;
				}
                
                try 
                {
                    var offSet = 0;
                    this._mode = aMode;
                    // create a filestream
                    var fs = JSLib.createInstance(JSLib.FILE_OUTSTREAM_CID, JSLib.FILE_I_FILE_OUT_STREAM);
                    
                    if (aMode == JSLib.FILE_WRITE_MODE) 
					{
						fs.init(this._nsIFile, JSLib.FILE_NS_TRUNCATE | JSLib.FILE_NS_WRONLY, aPerms, null);
					}
					else 
					{
						fs.init(this._nsIFile, JSLib.FILE_NS_RDWR | JSLib.FILE_NS_APPEND, aPerms, null);
					}
                    
                    this._outStream = fs;
                    if (this._isBinary) 
                    {
                        // wrap a nsIBinaryOutputStream around the actual file
                        var binstream = JSLib.createInstance(JSLib.FILE_BINOUTSTREAM_CID, JSLib.FILE_I_BINARY_OUT_STREAM);
                        binstream.setOutputStream(this._outStream);
                        this._outStream = binstream;
                    }
                } 
                catch (e) 
                {
                    JSLib.ExceptionHandler(e)
                }
                break;
            }
            
            case JSLib.FILE_READ_MODE:
            {
                JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FILE_NOT_FOUND "+ this._path);
                
                this._mode = JSLib.FILE_READ_MODE;
                
                try 
                {
                    this._fileChannel = JSLib.FILE_IOSERVICE.newChannelFromURI(this._URI);
                    this._lineBuffer = new Array();
                    if (this._isBinary) 
                    {
                        // wrap a nsIBinaryInputStream around the nsIInputStream
                        this._inputStream = JSLib.createInstance(JSLib.FILE_BININSTREAM_CID, JSLib.FILE_I_BINARY_IN_STREAM);
                        this._inputStream.setInputStream(this._fileChannel.open());
                    }
                    else 
                    {
                        // wrap a nsIScriptableInputStream around the nsIInputStream
                        this._inputStream = new JSLib.FILE_InputStream();
                        this._inputStream.init(this._fileChannel.open());
                    }
                } 
                catch (e) 
                {
                    JSLib.ExceptionHandler(e)
                }
                
                break;
            }
            
            default:
                throw new Exception("NS_ERROR_FILE_INVALID_ARG "+ this._path);
        }
    }
    
    /**
     * Reads a file (that's already been opened) and returns its contents.
     * Returns a null on failure.
     * 
     * @method
     * @alias JSLib.File.prototype.read
     * @param {Number} [aSize]
     *  	optional: the number of bytes to read from the file. If it is omitted the read will 
     *  	return the entire contents of the file.
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 f.open();
     *		 myContent = f.read();
     *		 f.close();
     * </pre>
     * @exception {Error} 
     *  	Throws a Exception containing the error code.
     * @return {String, Number[]} 
     * 		If the file is text-based, returns the contents as a string; if it's
     * 		binary, returns it as an Array of bytes (numbers between 0 and 255).
     */
    File.prototype.read = function(aSize)
    {
        this.check();
        
        JSLib.raiseIfTrue(!this._inputStream, "NS_ERROR_FILE_NOT_INPUT_STREAM "+ this._path);
        
        if (this._mode != JSLib.FILE_READ_MODE) 
		{
			this.close("NS_ERROR_FILE_NOT_AVAILABLE "+ this._path);
		}
        
        try 
        {
            if (!aSize) 
			{
				aSize = this._nsIFile.fileSize;
			}
            
            var streamData;
            
            if (this._isBinary) 
			{
				streamData = this._inputStream.readByteArray(aSize);
			}
			else 
			{
				streamData = converter.ConvertToUnicode(this._inputStream.read(aSize));
			}
            
	   		this._position = this._position + streamData.length;

            return streamData;
            
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * Reads a single line from an open file, takes no arguments but needs an open read mode filehandle
     * returns string containing the data read on success, null on failure
     * 
     * @method
     * @alias JSLib.File.prototype.readline
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 f.open();
     *		 while(!f.EOF)
     *			 dump("line: "+f.readline()+"\n");
     * </pre>
     * @exception {Error} 
     *  	Throws a Exception containing the error code.
     * @return {String} 
     * 		<string line of foo.dat>
     */
    File.prototype.readline = function()
    {
        this.check();
        
        if (!this._inputStream) 
		{
			this.close("NS_ERROR_FILE_OBJECT_NOT_INITIALIZED_ON_READLINE "+ this._path);
		}
        
        var buf = null;
        var tmp = null;
        
        try 
        {
            if (this._lineBuffer.length < 2) 
            {
                buf = this._inputStream.read(JSLib.FILE_CHUNK);
                this._position = this._position + JSLib.FILE_CHUNK;
                
                
                if (this._position > this._nsIFile.fileSize) 
                {
                    this._position = this._nsIFile.fileSize;
                }
                
                if (buf) 
                {
                    if (this._lineBuffer.length == 1) 
                    {
                        tmp = this._lineBuffer.shift();
                        buf = tmp + buf;
                    }
//                  this._lineBuffer = buf.split(/[\n\r]/);
					this._lineBuffer = buf.split(/(\r\n|\r|\n)/);
                }
            }
            return converter.ConvertToUnicode(this._lineBuffer.shift());
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Returns an array of individual lines read from the file on success, 
     * throws an Exception on failure
     * 
     * @method
     * @alias JSLib.File.prototype.readAllLines
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 var lines = f.readAllLines();
     * </pre>
     * @exception {Error} 
     *  Throws a Exception containing the error code.
     * @return {String} 
     *  <string array of foo.dat>
     */
    File.prototype.readAllLines = function()
    {
        this.check()
        
        try 
        {
            var fis = JSLib.createInstance(JSLib.FILE_INSTREAM_CID, "nsIFileInputStream");
            
            fis.init(this._nsIFile, -1, -1, false);
            
            var lis = JSLib.QI(fis, "nsILineInputStream");
            var line = 
            {
                value: ""
            };
            var more = false;
            var lines = [];
            
            do 
            {
                more = lis.readLine(line);
                lines.push(converter.ConvertToUnicode(line.value));
            }
            while (more);
            
            fis.close();
            return lines;
            
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * Checks 'end of file' status and returns boolean to indicate whether the end of file has been reached.
     * This function takes no arguments but needs an open read mode filehandle.
     * 
     * @alias JSLib.File.prototype.EOF
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 f.open();
     *		 while(!f.EOF)
     *			 dump("line: "+f.readline()+"\n");
     * </pre>
     * @exception {Error} 
     *  	Throws a Exception containing the error code.
     * @property {Boolean} 
     * 		Returns true on eof, false when not at eof
     */
    File.prototype.__defineGetter__('EOF', function()
    {
        this.check();
        
        if (!this._inputStream) 
		{
			this.close("NS_ERROR_FILE_NOT_INITIALIZED_EOF "+ this._path);
		}
        
        return (!((this._lineBuffer.length > 0) || (this._inputStream.available() > 0)))
    });
	    
    /**
     * Write provided data to a file.
     * 
     * @method
     * @alias JSLib.File.prototype.write
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 f.open("w");
     *		 f.write("some data to be written");
     * </pre>
     * @param {String|Array} 
     *  a Buffer to be written to a file, if the file is Binary then the buffer should be an array.
     * @exception {Error} 
     *  Throws a Exception containing the error code.
     */
    File.prototype.write = function(aBuffer)
    {
        this.check()
        
        if (this._mode == JSLib.FILE_READ_MODE) 
            this.close("NS_ERROR_FILE_READ_ONLY "+ this._path);
        
        if (!aBuffer) 
		{
			aBuffer = "";
		}
        
        try 
        {
            if (this._isBinary && aBuffer.constructor == Array) 
			{
				this._outStream.writeByteArray(aBuffer, aBuffer.length);
			}
			else 
			{
				var chunk = converter.ConvertFromUnicode(aBuffer);
				this._outStream.write(chunk, chunk.length);
			}
            
            this._outStream.flush();
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    /**
     * Copy to file to another location.
     * 
     * @method
     * @alias JSLib.File.prototype.copy
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 f.copy("/tmp/foo_copy.dat");
     * </pre>
     * @param {String} aDest 
     *  	the path to copy the file to.
     */
    File.prototype.copy = function(aDest)
    {
        JSLib.raiseIfTrue(!aDest, "NS_ERROR_FILE_INVALID_ARG ");
        
        this.check();
        
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FILE_NOT_FOUND "+ this._path);
 
        try 
        {
            var dest = new JSLib.File_nsIFile(aDest);
            var copyName, dir = null;
            
            JSLib.raiseIfTrue(dest.equals(this._nsIFile), "NS_ERROR_FILE_COPY_OR_MOVE_FAILED "+ dest._path);
            JSLib.raiseIfTrue(dest.exists(), "NS_ERROR_FILE_ALREADY_EXISTS "+ dest._path);
            JSLib.raiseIfTrue(this._nsIFile.isDirectory(), "NS_ERROR_FILE_IS_DIRECTORY "+ dest._path);
            
            if (!dest.exists()) 
            {
                copyName = dest.leafName;
                dir = dest.parent;
                
                JSLib.raiseIfTrue(!dir.exists(), "NS_ERROR_FILE_NOT_FOUND "+ dir._path);
                JSLib.raiseIfTrue(!dir.isDirectory(), "NS_ERROR_FILE_DESTINATION_NOT_DIR "+ dir._path);
                
            }
            
            if (!dir) 
            {
                dir = dest;
                JSLib.raiseIfTrue(dest.equals(this._nsIFile), "NS_ERROR_FILE_COPY_OR_MOVE_FAILED "+ this._path);
                
            }
            this._nsIFile.copyTo(dir, copyName);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Closes an open file stream, takes a single parameter Object which will be thrown as a 
     * Exception after the close and cleanup of the file stream.
     * 
	 * @method
     * @alias JSLib.File.prototype.close
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 f.open();
     *		 f.close();
     * </pre>
     * @param {Object} exception 
     *  	the object to raise as an exception.
     */
    File.prototype.close = function(exception)
    {
    
        if (this._fileChannel) 
		{
			delete this._fileChannel;
		}
    
        if (this._transport) 
		{
			delete this._transport;
		}
        
        if (this._mode) 
		{
			this._mode = null;
		}
        
        if (this._outStream) 
        {
            this._outStream.close();
            delete this._outStream;
        }
        
        if (this._inputStream) 
        {
            this._inputStream.close();
            delete this._inputStream;
        }
        
        if (this._lineBuffer) 
		{
			this._lineBuffer = null;
		}
        
        this._position = 0;
        
        if (this._URI) 
        {
            delete this._URI;
            this._URI = null;
        }
        
        JSLib.raiseIfTrue(!!exception, exception);
    };
    
    /**
     * Creates a new file under the referenced path of the object
     * 
     * @alias JSLib.File.prototype.create
     * @exception {Error} 
     *  	Throws a Exception containing the error code.
     */
    File.prototype.create = function()
    {
        // We can probably implement this so that it can create a 
        // file or dir if a long non-existent mPath is present
        
        this.check();
        JSLib.raiseIfTrue(this.exists, "NS_ERROR_FILE_ALREADY_EXISTS "+ this._path);
 
        try 
        {
            this._nsIFile.create(JSLib.FILE_FILE_TYPE, JSLib.FILE_DEFAULT_PERMS);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Creates a new unique file under the referenced path of the object.
     * 
     * @alias JSLib.File.prototype.createUnique
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     */
    File.prototype.createUnique = function()
    {
        this.check();
        
        try 
        {
            this._nsIFile.createUnique(JSLib.FILE_FILE_TYPE, JSLib.FILE_DEFAULT_PERMS);
            this._path = this._nsIFile.path;
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    };
    
    /**
     * Return a new instance of a JSLib File object referencing the same path
     * 
     * @alias JSLib.File.prototype.clone
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @return {JSLib.File} 
     * 		A File object created from the path of the original file
     */
    File.prototype.clone = function()
    {
        this.check();
        return new File(this._path);
    };
    
    /**
     * Removes the referenced file object from the file system. Throws an exception is the action fails.
     * 
     * @alias JSLib.File.prototype.remove
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     */
    File.prototype.remove = function()
    {
        this.check();
        
        JSLib.raiseIfTrue(!this._path, "NS_ERROR_FILE_INVALID_PATH "+ this._path);
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FILE_NOT_FOUND "+ this._path);
 
        this.close();
        
        try 
        {
	        JSLib.raiseIfTrue(this._nsIFile.isDirectory(), "NS_ERROR_FILE_IS_DIRECTORY "+ this._path);
            this._nsIFile.remove(false);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * Truncates the file. Throws an exception if the action fails.
     * 
     * @alias JSLib.File.prototype.truncate
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     */
    File.prototype.truncate = function()
    {
        this.check();
        
        JSLib.raiseIfTrue(!this._path, "NS_ERROR_FILE_INVALID_PATH "+ this._path);
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FILE_NOT_FOUND "+ this._path);

        JSLib.raiseIfTrue(this._mode in ['r','rb'], "NS_ERROR_FILE_MODE_IS_READ "+ this._path);

        try 
        {
	        JSLib.raiseIfTrue(this._nsIFile.isDirectory(), "NS_ERROR_FILE_IS_DIRECTORY "+ this._path);
            this._nsIFile.remove(false);
            this._nsIFile.create(JSLib.FILE_FILE_TYPE, JSLib.FILE_DEFAULT_PERMS);
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * Returns the current byte position in the referenced file.
     * 
	 * This method is only applicable when using the File.read() method. 
	 * If used with the File.readline() method it will return the internal 
	 * read ahead buffer position, which is unlikely to be what was expected.
 	 * 
     * @alias JSLib.File.prototype.pos
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 f.open();
     *		 while(!f.EOF){
     *			 dump("pos: "+f.pos+"\n");
     *			 dump("line: "+f.readline()+"\n");
     *		 }
     * </pre>
     * @property {Number} 
     * 		the current byte position in the referenced file
     */
    File.prototype.__defineGetter__('pos', function()
    {
        return this._position;
    });
        
    /**
     * Returns the size in bytes of the referenced file system object as reported by the OS.
     * 
     * @alias JSLib.File.prototype.size
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 f.size;
     * </pre>
     *	 outputs: int 16
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @property {Number} 
     * 		The size in bytes of the referenced file system object as reported by the OS
     */
    File.prototype.__defineGetter__('size', function()
    {
        this.check()
        
        JSLib.raiseIfTrue(!this._path, "NS_ERROR_FILE_INVALID_PATH "+ this._path);
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FILE_NOT_FOUND "+ this._path);
        
        this.resetCache();
        
        try 
        {
			JSLib.raiseIfTrue(this.exists && this._nsIFile.isDirectory(), "NS_ERROR_FILE_IS_DIRECTORY "+ this._path);
            return this._nsIFile.fileSize;
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    }); 
        
    /**
     * Returns the extension of the file object
     * 
     * @alias JSLib.File.prototype.ext
     * @example
     * <pre>
     *		 var p='/tmp/foo.dat';
     *		 var f=new Jaxer.File(p);
     *		 alert(f.ext);
     * </pre>
     *	 outputs: dat
     * @exception {Error} 
     * 		Throws a Exception containing the error code.
     * @property {String} 
     * 		the file extension of the referenced filesytem object
     */
    File.prototype.__defineGetter__('ext', function()
    {
        this.check();
        
        JSLib.raiseIfTrue(!this._path, "NS_ERROR_FILE_INVALID_PATH "+ this._path);
        JSLib.raiseIfTrue(!this.exists, "NS_ERROR_FILE_NOT_FOUND "+ this._path);
        
        try 
        {
			JSLib.raiseIfTrue(this.exists && this._nsIFile.isDirectory(), "NS_ERROR_FILE_IS_DIRECTORY "+ this._path);
            var leafName = this._nsIFile.leafName;
            var dotIndex = leafName.lastIndexOf('.');
            return (dotIndex >= 0) ? leafName.substring(dotIndex + 1) : "";
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
    }); 

    JSLib.File = File;

})();
