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
     * @namespace {JSLib.DirUtils} This is a utility class that wraps XPCOM directory/folder utility functions
     */
	
    /**
     * Directory Utility helper object
     * 
     * @constructor
     * @alias JSLib.DirUtils
     * @return {JSLib.DirUtils} Returns an instance of DirUtils.
     * 
     */
    function DirUtils() { }
    
    DirUtils.prototype.useObj = false;
    
    /**
     * Resolve the path for the requested folder.
     * 
     * @private
     * @alias JSLib.DirUtils.prototype.getPath
     * 
     */
    DirUtils.prototype.getPath = function(aAppID)
    {
    
        JSLib.raiseIfTrue(!aAppID, "NS_ERROR_DU_INVALID_ARG");
        
        var rv;
        
        try 
        {
            rv = JSLib.getService(JSLib.DIRUTILS_FILE_DIR_CID, JSLib.DIRUTILS_I_PROPS).get(aAppID, JSLib.DIRUTILS_NSIFILE);
            if (this.useObj) 
            {
                if (rv.isFile()) 
                {
                    rv = new JSLib.File(rv.path);
                }
                else 
                    if (rv.isDirectory()) 
                    {
                        rv = new JSLib.Dir(rv.path);
                    }
            }
            else 
            {
                rv = rv.path;
            }
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        return rv;
    };
        
    /**
     * Return the path to the current folder the process is running from.
     *  
     * @alias JSLib.DirUtils.prototype.current
     * @property {String}
     * 		the path to the current folder the process is running from
     * 
     */
    DirUtils.prototype.__defineGetter__('current', function()
    {
        return this.getPath(JSLib.NS_OS_CURRENT_PROCESS_DIR);
    });
		
    /**
     * Returns the home folder of the current user.
     * 
     * @alias JSLib.DirUtils.prototype.getHome
     * @property {String}
     * 		the home folder of the current user
     */
	DirUtils.prototype.getHome = function () { return this.getPath(JSLib.NS_OS_HOME_DIR); }
    
    /**
     * Returns the deskTop Folder of the current user.
     * 
     * @alias JSLib.DirUtils.prototype.desktopDir
     * @property {String}
     * 		the deskTop Folder of the current user
     */
    DirUtils.prototype.__defineGetter__('desktop', function()
    {
        return this.getPath(JSLib.NS_OS_DESKTOP_DIR);
    });
	
    /**
     * Returns the Jaxer temp Folder.
     * 
     * @alias JSLib.DirUtils.prototype.temp
     * @property {String}
     * 		the jaxer temp 
     */
    DirUtils.prototype.__defineGetter__('temp', function()
    {
        return this.getPath(JSLib.NS_OS_TEMP_DIR);
    });
	   
    JSLib.DirUtils = DirUtils;
    
})();

