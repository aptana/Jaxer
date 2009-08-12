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
     * @classDescription {JSLib.NetworkUtils} This is a utility class that wraps XPCOM Network utility functions
     */
	
    /**
     * Network Utility helper object
     * 
     * @constructor
     * @alias JSLib.NetworkUtils
     * @return {JSLib.NetworkUtils}
     */
    function NetworkUtils()
    {
    }
    
    NetworkUtils.prototype = 
    {
        _CallBack: null,
        _ValidateURI: null,
    }
    
    NetworkUtils.prototype._callback = function()
    {
        JSLib.raiseIfTrue(JSLib.typeIsFunc(this.callback), "callback is not a function ...");
        this.callback();
    };
    
    NetworkUtils.prototype.__defineGetter__('callback', function()
    {
        return this._CallBack;
    });
    
    NetworkUtils.prototype.__defineGetter__('callback', function(aVal)
    {
        this._CallBack = aVal;
    });
    
    /**
     * Invokes the XPCOM nsIURIFixup method and return a 'fixed' URI
     * 
     * @alias JSLib.NetworkUtils.prototype.fixupURI
     * @param {Object} aURI
     * 		The URI to process
     * @return {String} 
     * 		The fixed URI as a string
     */
	NetworkUtils.prototype.fixupURI = function(aURI)
    {
    
        JSLib.raiseIfTrue(!aURI, "NS_ERROR_XPC_NOT_ENOUGH_ARGS");
        try 
        {
            var fixupURI = JSLib.createInstance(JSLib.NETUTILS_URIFIXUP, "nsIURIFixup");
            
            // FIXUP_FLAG_NONE = 0
            var uri = fixupURI.createFixupURI(aURI, 0);
            
            if (uri) 
			{
				return uri;
			}
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        return null;
        
    };
    
    /**
     * Validates the provided URI using XPCOM and returns JSLib.Ok if successful
     * 
     * @alias JSLib.NetworkUtils.prototype.validateURI
     * @param {Object} aURI
     * 		The	URI to validate
     */
	NetworkUtils.prototype.validateURI = function(aURI)
    {
        JSLib.raiseIfTrue(!aURI, "NS_ERROR_XPC_NOT_ENOUGH_ARGS");
        
        var uri = aURI;
        
        if (JSLib.instanceOf(aURI, "nsIURI")) 
		{
			uri = aURI.spec;
		}
        
        this._ValidateURI = uri;
        
        try 
        {
            uri = this.fixupURI(uri);
            
            var checker = JSLib.createInstance(JSLib.NETUTILS_URICHECK, "nsIURIChecker");
            checker.init(uri);
            checker.loadFlags = Components.interfaces.nsIRequest.LOAD_BYPASS_CACHE;
            
            checker.asyncCheck(this, null);
            return JSLib.OK;
            
        } 
        catch (e) 
        {
            JSLib.ExceptionHandler(e)
        }
        
    };
    
    /**
     * Returns a QueryInterface for the provided XPCOM Interface ID
     * 
     * @alias JSLib.NetworkUtils.prototype.QueryInterface
     * @param {Object} iid
     * 		the requested XPCOM Interface ID
     */
	NetworkUtils.prototype.QueryInterface = function(iid)
    {
        JSLib.raiseIfTrue(!iid.equals(Components.interfaces.nsIRequestObserver) && !iid.equals(Components.interfaces.nsISupports) && !iid.equals(Components.interfaces.nsIInterfaceRequestor), Components.results.NS_ERROR_NO_INTERFACE);
        return this;
    };
    
    /**
     * A stub function to be overridden 
     * 
     * @alias JSLib.NetworkUtils.prototype.onStartRequest
     * @param {Object} aRequest
     * 	 	request object
     * @param {Object} aContext
     * 		the contextual object for this request
     */
	NetworkUtils.prototype.onStartRequest = function(aRequest, aContext)
    {
    };
    
    /**
     * Invokes the callback function for completed requests
     * 
     * @alias JSLib.NetworkUtils.prototype.onStopRequest
     * @param {Object} aRequest
     * 		the request object
     * @param {Object} aContext
     * 		the current context of the request context
     * @param {Object} aStatus
     * 		the current status of the request object
     */
	NetworkUtils.prototype.onStopRequest = function(aRequest, aContext, aStatus)
    {
        var cb = this.callback;
        if (aStatus == 0) 
        {
            if (JSLib.typeIsFunc(this.callback)) 
			{
				setTimeout(cb, 1, true);
			}
        }
        else 
        {
            if (JSLib.typeIsFunc(this.callback)) 
			{
				setTimeout(cb, 1, false);
			}
        }
        aRequest.cancel(Components.results.NS_ERROR_ABORT);
    };
    
    JSLib.NetworkUtils = NetworkUtils;
    
})();
