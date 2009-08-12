(function(){
	
	var prefs = CC["@mozilla.org/preferences-service;1"].getService(CI.nsIPrefBranch);
	
	/** 
	 * The string used to identify the user agent of the client
	 * making the request. Setting the userAgentOverride string 
	 * will cause Jaxer to use that string as the userAgent for
	 * Jaxer.Web.get, Jaxer.Sandbox and it will also be reported
	 * as the Browser for JaxaScript libraries being loaded.
	 * 
	 * This can be useful where a Javascript library uses the userAgent
	 * string to provide functionality dynamically, and will cause
	 * the library to use a more compatible code path.
	 * 
	 * @alias Jaxer.userAgentOverride
	 * @property {String}
	 */
	Jaxer.__defineGetter__("userAgentOverride", function()
	{
		try {
			return prefs.getCharPref("general.useragent.override");
		}
		catch (e)
		{
			return null;
		}
	});
	
	Jaxer.__defineSetter__("userAgentOverride", function(str)
	{
		prefs.setCharPref("general.useragent.override", str);
	});
	
	/** 
	 * Reset The string used to identify the user agent of the client
	 * to the original value
	 * 
	 * @alias Jaxer.resetUserAgent
	 * @property {String}
	 */
	Jaxer.resetUserAgent = function resetUserAgent()
	{
		prefs.clearUserPref("general.useragent.override");
	};

})();
