
// Prepare for this test
(function() {
	
var log = Jaxer.Log.forModule("ServerUnitTesting");

function findServerTests()
{
	
	var serverTests = [];

	// First find all tests implicitly designated as to be proxied by being in a proxy script block
	var potentialImplicitProxies = [];
	var scripts = Array.prototype.slice.call(document.getElementsByTagName("script"));
	var testFunctionPattern = /function\s+(test\w+)/g;
	scripts.forEach(function(script)
	{
		var runatAttr = script.getAttribute(Jaxer.RUNAT_ATTR);
		if (runatAttr == Jaxer.RUNAT_SERVER_AND_PROXY ||
			runatAttr == Jaxer.RUNAT_BOTH_AND_PROXY)
		{
			var code = script.text;
			var matches = testFunctionPattern.exec(code);
			while (matches != null)
			{
				var functionName = matches[1];
				if (typeof window[functionName] == "function")
				{
					potentialImplicitProxies.push(window[functionName]);
				}
				matches = testFunctionPattern.exec(code);
			}
		}
	});
	
	var jaxerProxies = Jaxer.proxies || [];

	// Now find all server-only test functions
	for (var p in window) 
	{
		try
		{
			var candidate = window[p];
			if (typeof candidate == "function" &&
				candidate.name.match(/^test/) &&
				(jaxerProxies.indexOf(name) == -1) &&
				(jaxerProxies.indexOf(candidate) == -1) &&
				((typeof candidate[Jaxer.RUNAT_ATTR] == "undefined" && 
				  typeof candidate[Jaxer.PROXY_ATTR] == "undefined" &&
				  potentialImplicitProxies.indexOf(candidate) == -1) // we don't have an explicit attribute, and the implicit one isn't proxy
				 ||
				 (typeof candidate[Jaxer.RUNAT_ATTR] == "string" &&
				  (candidate[Jaxer.RUNAT_ATTR] != Jaxer.RUNAT_SERVER_AND_PROXY &&
				   candidate[Jaxer.RUNAT_ATTR] != Jaxer.RUNAT_BOTH_AND_PROXY &&
				   candidate[Jaxer.RUNAT_ATTR] != Jaxer.RUNAT_CLIENT))
				 ||
				 (candidate[Jaxer.PROXY_ATTR] == false)))
			{
				serverTests.push(candidate);
			}
		}
		catch (e)
		{
			
		}
	}
	
	return serverTests;
}

window.onserverload = function()
{
	if (!window.runAfterCallbackProcessing)
	{
		beforeClientSend();
	}
}

window.aftercallbackprocessing = function()
{
	if (window.runAfterCallbackProcessing)
	{
		beforeClientSend();
	}
}

function beforeClientSend()
{
	var clientFunctions = [];
	var skip = (typeof SKIP_TESTS == "function") ? SKIP_TESTS() : false;
	if (skip)
	{
		clientFunctions.push("function SKIP_TESTS() { return '" + skip + "'; };");
	}
	else 
	{
		var serverTests = findServerTests();
		if (serverTests.length > 0) 
		{
			serverTests.forEach(function(test)
			{
				var clientTestName = test.name + "_serverOnly";
				try 
				{
					log.debug("Running test: " + clientTestName + "...");
					test();
					log.debug("... test " + clientTestName + " succeeded");
					clientFunctions.push("function " + clientTestName + "() { };"); // No-op represents success
				} 
				catch (e) 
				{
					var msg = (typeof e.message == "string") ? e.message : e.toString();
					var testSkipped = (msg.match(/^\s*SKIPPED\b/));
					if (!testSkipped) log.warn("... test " + clientTestName + " failed");
					clientFunctions.push("function " + clientTestName + "() { throw '" +
						Jaxer.Util.String.escapeForJS(msg).replace(/\r\n/g, " ").replace(/\n/g, " ") +
						"'; };");
				}
			});
		}
	}
	if (clientFunctions.length > 0)
	{
		var head = document.getElementsByTagName("head")[0];
		Jaxer.Util.DOM.insertScriptAtBeginning(clientFunctions.join("\n"), head, {});
	}
}

})();