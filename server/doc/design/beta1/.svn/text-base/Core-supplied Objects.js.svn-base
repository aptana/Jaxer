/**
 * @projectDescription Spec-by-example of the high-level data objects supplied by the core
 * 
 * @author Uri Sarid
 */

var Request =									// Read-Only
{
	method: "POST",								// never null: usually "GET" or "POST"
	uri: "/pages/page1/html?foo=bar&user=1",  	// never null
	httpVersion: "HTTP/1.1",					// never null; should we just have 1.1?
	headers: 									// never null
	{
		Accept: "text/*, text/html",
		"Cache-Control": "no-cache",
		// etc.
	},
	POSTData: 									// null if not a POST request
	{
		fieldName1: "value1",
		fieldName2: "value2",
		// etc.
	}
}

var Response =									// Read-Write
{
	statusCode: "200",							// never null
	statusPhrase: "OK",							// never null
	httpVersion: "HTTP/1.1",					// never null; should we just have 1.1?
	headers:									// never null
	{
		Date: "Mon, 23 May 2005 22:38:34 GMT",
		"Content-Length": "438",
		// etc.
	},
	contents: "<html>...</html>"				// The contents received by Jaxer, or the contents to send back (?)
}
