(function() {
	
var config = // for future use
{
};

if (typeof window.Jaxer == "undefined") { window.Jaxer = {}; }

/**
 * True on the server side, false on the client (browser).
 * 
 * @alias Jaxer.isOnServer
 * @property {Boolean}
 */
Jaxer.isOnServer = false;

/**
 * Holds the proxy functions for calling server functions that were designated
 * with runat="both-proxy" (or equivalent), to prevent function name collisions.
 * So e.g. if a function getName() is defined with runat="both-proxy", in the
 * browser you can call getName() to use the client-side function or
 * Jaxer.Server.getName() to use the server-side function. Jaxer.Server holds
 * both the synchronous and asynchronous versions of the proxies (e.g.
 * Jaxer.Server.getName() and Jaxer.Server.getName.async()).
 * 
 * @alias Jaxer.Server
 */
if (typeof Jaxer.Config == "undefined") { Jaxer.Config = {}; }
if (typeof Jaxer.Server == "undefined") { Jaxer.Server = {}; }

for (var prop in config)
{
	Jaxer.Config[prop] = config[prop];
}

if (typeof Jaxer.Log == "undefined")
{
	function ModuleLogger()
	{
		this.trace = this.debug = this.info = this.warn = this.error = this.fatal = function() {};
	}
	var genericModuleLogger = new ModuleLogger();
	Jaxer.Log = genericModuleLogger;
	Jaxer.Log.forModule = function() { return genericModuleLogger; }
}

})();
