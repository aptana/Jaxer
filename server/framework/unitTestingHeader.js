Jaxer.load(Jaxer.request.parsedUrl.queryParts.serverOnlyUrl, window, "server");
Jaxer.load(Jaxer.request.parsedUrl.queryParts.bothServerUrl, window, "server");
if (Jaxer.request.parsedUrl.queryParts.libraryUrl) 
{
    Jaxer.load(Jaxer.request.parsedUrl.queryParts.libraryUrl, window, "server");
}
document.write("<script src='" + Jaxer.request.parsedUrl.queryParts.bothClientUrl + "'></" + "script>");

var multipleJaxersAvailable = Jaxer.request.parsedUrl.queryParts.multiJaxer == "true";

function skipIfSingleJaxer()
{
    if (!multipleJaxersAvailable) 
	{
		throw "SKIPPED - This test requires multiple Jaxers to run; " +
			"rerun it with '&multiJaxer=true' in the URL";
	}
}

