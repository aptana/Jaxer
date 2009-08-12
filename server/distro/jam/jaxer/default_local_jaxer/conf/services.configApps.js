/* 
 * ***********************************************************************************************
 * 
 * This file defines some simple web service dispatchers that are shipped by default with Jaxer. 
 * 
 * To have Jaxer handle web service requests directly (without the web server needing to
 * hand Jaxer an HTML page to process), the web server must first be configured accordingly.
 * 
 * The Apache shipped with Jaxer is configured to route any requests having jaxer-service
 * or jaxer-rpc directly to Jaxer; similarly for the Jetty web server in Aptana Studio.
 * 
 * Once Jaxer receives the request, entries in Config.apps will define if and how the request is handled.
 * Below are two of them: a generic dispatcher for any jaxer-service request, useful e.g. for
 * defining RESTful services, and a more specialized one for Remote Procedure Calls 
 * that knows to call your JavaScript functions.
 * 
 * ***********************************************************************************************
 */
 
(function() {
 
        /* 
         *      This is a simple example of a universal dispatcher for general web service requests.
         *      
         *      In conjunction with an appropriately-configured web server (e.g. the Apache shipped with Jaxer, 
         *      or Jetty in Aptana Studio), this automatically grabs any service requests.
         *      It defines a service request as any request whose path contains /jaxer-service/ 
         *      (e.g http://www.mydomain.com/myApp/jaxer-service/admin/users/52)
         *      and that has some leading part corresponding to a .js file on disk
         *      (e.g <docroot>/myApp/jaxer-service/admin.js or <docroot>/myApp/jaxer-service/admin/users.js).
         *      That file will be loaded and executed, and it's responsible for handling the HTTP request
         *      and returning an appropriate HTTP response.
         *      
         *      For more information, see the sample extension in jaxer/framework/extensions/serviceDispatcher.js .
         */
		var SERVICE_DISPATCHER = 
		{
			urlTest: function(parsedUrl)
			{
				var indexOfJaxerService = parsedUrl.pathParts.indexOf('jaxer-service');
				if (indexOfJaxerService == -1) return false; 	// Require jaxer-service in the path
				if (!Jaxer.Extensions.ServiceDispatcher) return false;					// Require the ServiceDispatcher extension to be loaded
				var foundFile = Jaxer.Extensions.ServiceDispatcher.findFile(Jaxer.request.documentRoot, '.js', indexOfJaxerService + 1);
				return Boolean(foundFile);												// Require the path to contain a handler file to load
			},
			name: function(parsedUrl)
			{
				var indexOfName = parsedUrl.pathParts.indexOf('jaxer-service') + 1;
				return (parsedUrl.pathParts.length > indexOfName) ? parsedUrl.pathParts[indexOfName] : 'defaultService';
			},
			handler: function(resolvedName, parsedUrl)
			{
				return Jaxer.Extensions.ServiceDispatcher.findFile(Jaxer.request.documentRoot, '.js');
			}
		}
		Config.apps.unshift(SERVICE_DISPATCHER);

        /* 
         *      This is a simple example of a universal dispatcher for RPC (Remote Procedure Call) requests.
         *      
         *      In conjunction with an appropriately-configured web server (e.g. the Apache shipped with Jaxer, 
         *      or Jetty in Aptana Studio), this automatically grabs any RPC requests.
         *      It defines an RPC request as any request whose path contains /jaxer-rpc/ 
         *      (e.g http://www.mydomain.com/myApp/jaxer-rpc/auth/addUser)
         *      and whose leading part (all but the last part) corresponds to a .js file on disk
         *      (e.g <docroot>/myApp/jaxer-rpc/auth.js).
         *      That file will be loaded and executed; then the addUser function in that file will be 
         *      called and its results returned.
         *      
         *      For more information, see the sample extension in jaxer/framework/extensions/rpcDispatcher.js .
         */
		var RPC_DISPATCHER = 
		{
			urlTest: function(parsedUrl)
			{
				var indexOfJaxerRPC = parsedUrl.pathParts.indexOf('jaxer-rpc');
				if (indexOfJaxerRPC == -1) return false; 	// Require jaxer-rpc in the path
				if (!Jaxer.Extensions.RPCDispatcher) return false;					// Require the RestDispatcher extension to be loaded
				var foundFile = Jaxer.Extensions.RPCDispatcher.findFile(Jaxer.request.documentRoot, '.js', indexOfJaxerRPC + 1);
				return Boolean(foundFile);											// Require the path to contain a handler file to load
			},
			name: function(parsedUrl)
			{
				var indexOfName = parsedUrl.pathParts.indexOf('jaxer-rpc') + 1;
				return (parsedUrl.pathParts.length > indexOfName) ? parsedUrl.pathParts[indexOfName] : 'defaultRPCService';
			},
			handler: function(resolvedName, parsedUrl)
			{
				Jaxer.Extensions.RPCDispatcher.handleRequest(Jaxer.request.documentRoot, '.js');
				// don't return anything -- there's no handler file to load, all the handling is already done
			}
		}
		Config.apps.unshift(RPC_DISPATCHER);
 
})();