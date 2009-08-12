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
(function()
{

    /**
     * @namespace {JSLib} 
     * 		This is the namespace that holds XPCOM functions and variables
     */
    
	
	//noformat
	
	JSLib = {}; // Should be global
	
	JSLib.OK							= 0;
	JSLib.VOID 							= void(null)
	
	JSLib.DIR_DIRECTORY               	= 0x01;  // 1
	JSLib.DIR_DEFAULT_PERMS           	= 0755;
	
	JSLib.DIRUTILS_FILE_DIR_CID 		= "@mozilla.org/file/directory_service;1";
	JSLib.DIRUTILS_I_PROPS	  			= "nsIProperties";
	
	JSLib.FILE_LOCAL 					= "@mozilla.org/file/local;1";

	JSLib.NETWORK_URL 					= "@mozilla.org/network/standard-url;1";
	JSLib.URL_COMP 						= "nsIURL";
	
	JSLib.FILE_IOSERVICE_CID			= "@mozilla.org/network/io-service;1";
	JSLib.FILEUTILS_IO_SERVICE_CID 		= '@mozilla.org/network/io-service;1'; //dupe
	
	JSLib.FILE_I_STREAM_CID	 			= "@mozilla.org/scriptableinputstream;1";
	JSLib.SOCKET_I_STREAM_CID 			= "@mozilla.org/scriptableinputstream;1" //dupe
	
	JSLib.FILE_INSTREAM_CID				= "@mozilla.org/network/file-input-stream;1";	
	JSLib.FILE_OUTSTREAM_CID			= "@mozilla.org/network/file-output-stream;1";
	JSLib.FILE_BINOUTSTREAM_CID 		= "@mozilla.org/binaryoutputstream;1";
	
	JSLib.FILE_BININSTREAM_CID			= "@mozilla.org/binaryinputstream;1";
	JSLib.SOCKET_BININSTREAM_CID 		= "@mozilla.org/binaryinputstream;1" //dupe
	
	JSLib.FILEUTILS_PROCESS_CID			= "@mozilla.org/process/util;1";
	JSLib.FILEUTILS_FILEPROTOCOL_CID	= "@mozilla.org/network/protocol;1?name=file"
	
	JSLib.FILEUTILS_CHROME_REG_PROGID 	= '@mozilla.org/chrome/chrome-registry;1';
	
	JSLib.NETUTILS_URIFIXUP 			= "@mozilla.org/docshell/urifixup;1";
	JSLib.NETUTILS_URICHECK 			= "@mozilla.org/network/urichecker;1";
	
	JSLib.SOCKET_TRANSPORT_CID 			= "@mozilla.org/network/socket-transport-service;1";
	JSLib.SOCKET_INPUT_STREAM_PUMP 		= "@mozilla.org/network/input-stream-pump;1" 
	
	JSLib.ZIP_CID 						= "@mozilla.org/libjar/zip-reader;1";
	JSLib.ZIP_I_ZIP_READER 				= "nsIZipReader";
	
	JSLib.File_nsIFile 					= new Components.Constructor(JSLib.FILE_LOCAL, "nsILocalFile", "initWithPath");
	JSLib.URL 							= new Components.Constructor(JSLib.NETWORK_URL, JSLib.URL_COMP);
	JSLib.FILEUTILS_nsIFile				= new Components.Constructor(JSLib.FILE_LOCAL, "nsILocalFile", "initWithPath");
	
	JSLib.FILE_I_SCRIPTABLE_IN_STREAM	= "nsIScriptableInputStream";
	JSLib.FILE_I_BINARY_OUT_STREAM		= "nsIBinaryOutputStream";
	JSLib.FILE_I_BINARY_IN_STREAM		= "nsIBinaryInputStream";
	
	JSLib.FILE_INTL_SCR_UNI_CONV_CID	= "@mozilla.org/intl/scriptableunicodeconverter";
	JSLib.FILE_INTL_I_SCR_UNI_CONV		= "nsIScriptableUnicodeConverter";
	
	JSLib.FILE_READ_MODE	 			= "r";
	JSLib.FILE_WRITE_MODE				= "w";
	JSLib.FILE_APPEND_MODE	 			= "a";
	JSLib.FILE_BINARY_MODE	 			= "b";
	
	JSLib.FILE_FILE_TYPE	 			= 0x00;	// 0
	JSLib.FILE_CHUNK		 			= 1024;	// buffer for readline => set to 1k
	JSLib.FILE_DEFAULT_PERMS 			= 0644;
	

	JSLib.DIRUTILS_NSIFILE	  			= Components.interfaces.nsIFile;
	JSLib.FILE_I_IOSERVICE				= Components.interfaces.nsIIOService;
	JSLib.FILE_I_FILE_OUT_STREAM		= Components.interfaces.nsIFileOutputStream;
	
	/* 
	 * Useful OS System Dirs
	 * /usr/src/mozilla/dist/bin
	 */
	JSLib.NS_OS_CURRENT_PROCESS_DIR 	= "CurProcD"; 
	JSLib.NS_OS_DESKTOP_DIR 			= "Desk";
	JSLib.NS_OS_HOME_DIR				= "Home"; 
	JSLib.NS_OS_TEMP_DIR 				= "TmpD"; 
	
	/*
	 * Possible values for the ioFlags parameter 
	 * From: 
	 * http://lxr.mozilla.org/seamonkey/source/nsprpub/pr/include/prio.h#601
	 */
	JSLib.FILE_NS_RDONLY				= 0x01;
	JSLib.FILE_NS_WRONLY				= 0x02;
	JSLib.FILE_NS_RDWR					= 0x04;
	JSLib.FILE_NS_CREATE_FILE			= 0x08;
	JSLib.FILE_NS_APPEND				= 0x10;
	JSLib.FILE_NS_TRUNCATE				= 0x20;
	JSLib.FILE_NS_SYNC					= 0x40;
	JSLib.FILE_NS_EXCL					= 0x80;
	
	//format
    
    /**
     * typeIsObj
     * 
     * A boolean indicator for whether the param is an object. 
     * 
     * @private
     * @param {Object} aType
     * @return {Boolean} 
     * 		true if object otherwise false
     */
	JSLib.typeIsObj = function(aType)
    {
        return (aType && typeof(aType) == "object");
    };
    
	/**
	 * typeIsNum
	 * 
     * @private
	 * @param {Object} aType
	 * 	the item to test
	 * @return {Boolean} 
	 * 		true if number otherwise false
	 */
    JSLib.typeIsNum = function(aType)
    {
        return (aType && typeof(aType) == "number");
    };
    
	/**
	 * typeIsStr
	 * 
     * @private
	 * @param {Object} aType
	 * 	the item to test
	 * @return {Boolean} 
	 * 		true if String otherwise false
	 */
    JSLib.typeIsStr = function(aType)
    {
        return (aType && typeof(aType) == "string");
    };
    
	/**
	 * typeIsFunc
	 * 
     * @private
	 * @param {Object} aType
	 * 	the item to test
	 * @return {Boolean} 
	 * 		true if function otherwise false
	 */
    JSLib.typeIsFunc = function(aType)
    {
        return (aType && typeof(aType) == "function");
    };
    
	/**
	 * Tests whether the object is an instance of the XPCOM interface provided
	 * 
     * @private
	 * @param {Object} aObj
	 * 		the object to be tested
	 * @param {Object} aInterface
	 * 		the interface name as a string
	 * @return {Boolean} 
	 * 		true if the object is an instance of the requested type, false otherwise
	 */
    JSLib.instanceOf = function(aObj, aInterface)
    {
        return (aObj instanceof Components.interfaces[aInterface]);
    };
    
	/**
	 * Wrapper for the XPCOM function createinstance
	 * 
     * @private
	 * @param {Object} aURL
	 * 	the URL 
	 * @param {Object} aInterface
	 * 	the named interface to instantiate
	 * @return {Object} 
	 * 	an instance of the XPCOM object
	 */
    JSLib.createInstance = function(aURL, aInterface)
    {
        try 
        {
            return Components.classes[aURL].createInstance(Components.interfaces[aInterface]);
        } 
        catch (e) 
        {
            throw new Exception(e);
        }
    };
    
    /**
     * Wrapper for the XPCOM function getService
     * @private
     * @param {Object} aURL
     * 		the URL
     * @param {Object} aInterface
     * 		the named interface to instantiate
     * @return {Object}
     * 		an instance of the XPCOM object
     */
	JSLib.getService = function(aURL, aInterface)
    {
        try 
        {
            // determine how 'aInterface' is passed and handle accordingly
            switch (typeof(aInterface))
            {
                case "object":
                    return Components.classes[aURL].getService(aInterface);
                    break;
                    
                case "string":
                    return Components.classes[aURL].getService(Components.interfaces[aInterface]);
                    break;
                    
                default:
                    return Components.classes[aURL].getService();
                    break;
            }
        } 
        catch (e) 
        {
            throw new Exception(e);
        }
    };
    
    /**
     * Wrapper for the XPCOM function QueryInterface
     * 
     * @private
     * @param {Object} aURL
     * @param {Object} aInterface
     * 		the named interface to instantiate
     * @return {Object}
     * 		an instance of the XPCOM objects query interface
     */
	JSLib.QI = function(aObj, aInterface)
    {
        try 
        {
            return aObj.QueryInterface(Components.interfaces[aInterface]);
        } 
        catch (e) 
        {
            throw new Exception(e);
        }
    }
    
    try 
    {
        JSLib.FILE_InputStream = new Components.Constructor(JSLib.FILE_I_STREAM_CID, JSLib.FILE_I_SCRIPTABLE_IN_STREAM);
        JSLib.FILE_IOSERVICE = JSLib.getService(JSLib.FILE_IOSERVICE_CID, JSLib.FILE_I_IOSERVICE);
    } 
    catch (e) 
    {
        throw new Exception(e);
    };
    
    /**
     * Init the base exception handler
     * @private
     * @param {Object} e
     * 		The exception to throw
     */
	JSLib.ExceptionHandler = function ExceptionHandler(e) { 
		throw "Exception thrown from JSLib: " + e; 
	}; 
	
    /**
     * Register an exceptionHandler to deal with any JSLib exceptions
     * @private
     * @param {Function} fn
     * 		The function to process any exceptions thrown
     */
	JSLib.setExceptionHandler = function(fn){
		JSLib.ExceptionHandler = fn;
	}
	
	/**
     * Exception handler that ships with core jslib
     * @private
     */
    JSLib.defaultExceptionHandler = function(e)
    {
		var errMsg = "\n-----======[ jsLib ERROR ]=====-----\n";
		
        if (JSLib.typeIsStr(e)) 
		{
			throw e;
		}
        
        if (/^TypeError/.test(e)) 
		{
			throw e;
		}
        
        if (JSLib.typeIsObj(e)) 
		{
			var m, n, r, l, ln, fn = "";
			try 
			{
				rv = -e.result;
				m = e.message;
				fn = e.filename;
				l = e.location;
				ln = l.lineNumber;
			} 
			catch (e) 
			{
			}
			errMsg += "FileName:          " + fn + "\n" ;
			errMsg += "Result:            " + rv + "\n" ;
			errMsg += "Message:           " + m + "\n" ;
			errMsg += "LineNumber:        " +ln +"\n";
		} else {
			errMsg += uneval(e);
		}
              
        throw errMsg;

    }

	JSLib.ExceptionHandler = JSLib.defaultExceptionHandler;
	
    /**
     * @private
     * Throws the provided exception via the currently registered exception handler.
     * @param {Boolean} assertion
     * 		a true/false condition to indicate whether the assertion is true
     * @param {Object} message
     * 		the object to throw as an exception
     */
    JSLib.raiseIfTrue = function(assertion, message)
    {
        if (assertion) 
        {
            JSLib.ExceptionHandler(message);
        }
    }
        
})();


