var OK_ICON = "images/accept.png";
var INFO_ICON = "images/info.png";
var WARN_ICON = "images/warn.png";
var ERROR_ICON = "images/error.png";
var FAILURE_ICON = "images/monitor_error.png";

function diagnostics(testName, label, fn){

    var tmp = $('#diagnosticsTemplate').html();
    
    $('#testResults').append(tmp).children(':last').attr({
        'id': testName
    }).addClass('diagnostics');
    
    var currentNode = $('#' + testName);
    currentNode.find('.resultMessage , .more , .moreLink').hide();
    currentNode.find('.runMessage').show().find('SPAN').append(label);
    
    currentNode.find('.moreLink').bind("click", function(){
        $(this).parent().next('.more').toggle();
        var btnLabel = $(this).text();
        $(this).text((btnLabel == "more...") ? "less..." : "more...");
    });
    
    try {
        var diagnosticResults = fn();
        var diagnosis = diagnosticResults.diagnosis;
        var moretext = diagnosticResults.more;
        
        currentNode.find('.resultimage').attr({
            src: OK_ICON,
            title: testName
        });
        currentNode.find('.runMessage').show().find('SPAN').html(diagnosis);
        currentNode.find('.more').html(moretext);
        
        if (moretext.length > 0) {
            currentNode.find('.moreLink').show();
        }
    } 
    catch (err) {
    
        if (!!err.diagnosis) {
            diagnosis = err.diagnosis;
            moretext = err.more || "";
            icon = err.icon || ERROR_ICON;
        }
        else {
            diagnosis = 'An unexpected error occurred in the diagnostic test \'' + testName + '\'';
            moretext = '<B>' + uneval(err) + '</B>';
            icon = FAILURE_ICON;
        }
        
        currentNode.find('.resultimage').attr({
            src: icon,
            title: testName
        });
        currentNode.find('.runMessage').show().find('SPAN').html(diagnosis);
        
        if (moretext.length > 0) {
            currentNode.find('.moreLink').show();
            currentNode.find('.more').html(moretext);
        }
    }
}

function simplePass()
{
    return {
        diagnosis: "this succeeded",
        more: ''
    };
}

function simplePassAndMore()
{
    return {
        diagnosis: "this succeeded with more info provided",
        more: ' Visit <a target="_blank" href="http://aptana.com/jaxer/">Jaxer Central</a> for the latest Jaxer updates and news. '
    };
}

function simpleWarning()
{
    throw {
        diagnosis: "this is an warning",
        more: 'text on a warning but hey it\'s only a warning',
        icon: WARN_ICON
    };
}

function simpleInfo()
{
    throw {
        diagnosis: "this is an info message",
        more: 'this is presented for informational purposes only',
        icon: INFO_ICON
    };
}


function simpleError()
{
    throw {
        diagnosis: "this is an ERROR",
        more: 'this is moretext on an error how cool!'
    };
}

function unhandledError()
{
    //installDir();
}

function diagnosticsServerDetect()
{
    try 
    {
        if (!!Jaxer) 
            return {
                diagnosis: "Jaxer namespace is present",
                more: "build number is " + getBuildNumber()
            }
        else 
            throw {
                diagnosis: "Jaxer namespace not found"
            };
    } 
    catch (err) 
    {
        throw err;
    }
}

function diagnosticsJavascriptVersions()
{
    if (!!_versionB) 
        return {
            diagnosis: "JavaScript Versions - Browser :" + _versionB + " Server " + Jaxer.clientData.JSVersion,
            more: ''
        };
    else 
        throw {
            diagnosis: "JavaScript Version identifier not found"
        };
}

function diagnosticsConfigFileRead()
{
    try 
    {
        var cfgPath = getConfigFileName();;
		
        var result = getFileContents(cfgPath);
        if (result.length == 0) 
        {
            throw {
                'diagnosis': cfgPath + ' - File not found or empty',
                'more': ''
            };
        }
        return {
            'diagnosis': cfgPath + ' - found',
            'more': ''
        }
    } 
    catch (err) 
    {
        throw err;
    }
}

function diagnosticsLocalConfigFileRead()
{
    try 
    {
        var cfgPath = getLocalConfigFileName();
        var result = getFileContents(cfgPath);
        if (result.length == 0) 
        {
            throw {
                diagnosis: cfgPath + ' - File not found or empty',
                more: 'This message is informational only. Most Jaxer packages and Aptana Studio would create a template file here, which you can customize.',
                icon: INFO_ICON
            };
        }
        return {
            'diagnosis': cfgPath + ' - found',
            'more': ''
        }
        
    } 
    catch (err) 
    {
        throw err;
    }
}

function diagnosticsLogFileRead()
{
    try 
    {
        var logfile = getLogFileName();
        
        var exists = doesFileExist(logfile);
        
        if (!exists) 
        {
            throw {
                diagnosis: logfile + ' File not found',
                more: 'This file should have been automatically generated when you first ran Jaxer.',
                icon: WARN_ICON
            };
        }
        else 
        {
            return {
                'diagnosis': logfile + ' - found',
                'more': ''
            }
        }
    } 
    catch (err) 
    {
        throw err;
    }
}

function diagnosticsSSX()
{
    try 
    {
        var result = showTestCallback('callback');
        return {
            'diagnosis': 'Server-side Execution - ' + result,
            'more': ''
        }
    } 
    catch (err) 
    {
        throw {
            diagnosis: 'Server-side JavaScript Execution Failed  ',
            more: uneval(err)
        }
    }
}

//needs fix should be doing a different test
function diagnosticsSSXDeferred()
{
    try 
    {
        var result = showTestCallback('callback');
        return {
            'diagnosis': "Server-side deferred execution Test - " + result,
            'more': ''
        }
    } 
    catch (err) 
    {
        throw {
            diagnosis: "Server-side deferred execution Test failed" + uneval(err),
            more: "If this test failed,it means Jaxer was not able to run the same simple DOM manipulation function as above but this time during the <span class=\"tech-name\">onserverload</span> event.<p>"
        }
    }
}

function diagnosticsCallback()
{
    try 
    {
        var result = showTestCallback('callback');
        return {
            'diagnosis': "Callback Test Passed- " + result,
            'more': ''
        }
    } 
    catch (err) 
    {
        throw {
            diagnosis: "Callback Test Failed- " + err,
            more: "If this test failed, it means Jaxer was not able to call the <span class=\"tech-name\">testCallback()</span> function on the server from the client."
        }
    }
}

function diagnosticsCallbackDB()
{
    try 
    {
        var result = testDatabase();
        return {
            'diagnosis': "Database Callback Test - " + result,
            'more': ''
        }
    } 
    catch (err) 
    {
        throw {
            diagnosis: "Database Callback Test - " + err,
            more: "If this test failed, it means Jaxer was not able to call the <span class=\"tech-name\">testDatabase()</span> function on the server (which accesses the database) from the client."
        }
    }
}
