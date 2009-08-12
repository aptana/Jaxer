function parseBuildNumber(content)
{
	return parseFloat(content.replace(/"|\x0A|\x0D/g,'').replace(/[^0123456789]/g,''));
}

function fetchCurrentBuildNumber(){
	
	var platform = Jaxer.request.OS;
	var platforms = {
			'WINNT' : "win32"
		,	'Darwin': "mac"
		,	'Linux': "ubu32"
		,	'SunOS': "ubu32"
	}
    
	var target = platforms[platform];
	
    var buildVal = parseBuildNumber(Jaxer.buildNumber);
    
    var xhrOptions = new Jaxer.XHR.SendOptions();
    
    xhrOptions.cacheBuster = false;
    xhrOptions.timeout = 0;
    xhrOptions.headers = null;
    xhrOptions.onreadytosend = null;
    xhrOptions.onfinished = null;
    xhrOptions.getTransport = Jaxer.XHR.getTransport;
    
    xhrOptions.method = "GET";
    xhrOptions.async = false;
    xhrOptions.url = "http://update.aptana.com/update/jaxer/"+target+"/version.txt?"+Jaxer.buildNumber;			
	
    xhrOptions.onsuccess = null;
    xhrOptions.onfailure = null;
    
    var result = Jaxer.XHR.send("", xhrOptions);
    
	var latestBuildNumber = result.replace(/[^\.0123456789]/g,'');
	
    var latest = parseBuildNumber(result);                   
	
	// for testing...
	// latest = 1000000000000000360500000;
	
    if (latest > buildVal) {
        return "&nbsp;<a href='http://www.aptana.com/jaxer/download'>Upgrade Available ("+latestBuildNumber+")</a>";
    }
    if (latest == 0) {
        return  false ;
    }
	
	return false; // return empty if we dont recognize the content.
    
}	

window.onserverload = function()
{
	$('jaxer_build').update(Jaxer.buildNumber);
	$('user_agent').update(navigator.userAgent);
}

fetchCurrentBuildNumber.proxy=true;