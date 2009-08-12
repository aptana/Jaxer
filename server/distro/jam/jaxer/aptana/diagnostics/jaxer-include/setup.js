function getBuildNumber()
{
    return Jaxer.buildNumber;
}
getBuildNumber.proxy = true;

function resolvePath(path, referencePath)
{
	return Jaxer.Dir.resolve(path, referencePath);
}
resolvePath.proxy=true;

function getFileContents(aPath)
{
    return Jaxer.File.readLines(aPath, "<BR>") || "";
}
getFileContents.proxy = true;

function doesFileExist(aPath)
{
    return Jaxer.File.exists(aPath);
}
doesFileExist.proxy = true;

function getConfigFileName()
{
	return resolvePath(Jaxer.Config.CONFIG_PATH);
}
getConfigFileName.proxy = true;

function getLocalConfigFileName()
{
	return resolvePath(Jaxer.Dir.combine(Jaxer.Config.LOCAL_CONF_DIR, "config.js"), Jaxer.System.executableFolder);
}
getLocalConfigFileName.proxy = true;

function testDatabase()
{
    var resultSet = Jaxer.DB.frameworkExecute("SELECT * FROM callback_page");
    var numCallbacks = resultSet.rows.length;
    return "Success!";
}
testDatabase.proxy = true;

function findAndShowSuccess(id, message)
{
    var elt = document.getElementById(id);
    if (elt) 
    {
        elt.innerHTML = message || "SUCCESS";
        elt.className = "emphatic";
    }
}
findAndShowSuccess.runat = "both";

function isOnServer()
{
    return (typeof window.Jaxer != "undefined" &&
    	Jaxer != null &&
    	typeof Jaxer.isOnServer != null &&
    	Jaxer.isOnServer);
}
isOnServer.runat = "both";

function showServerSide(id)
{
    var success = isOnServer();
    if (success) 
    {
        findAndShowSuccess(id);
    }
}

function testCallback()
{
    return isOnServer();
}
testCallback.proxy = true;

function showTestCallback(id)
{
    try 
    {
        var success = testCallback();
        if (success) 
        {
            //findAndShowSuccess(id);
            return "Server callback succeeded";
        }
        else 
        {
            throw "Ran on the client, not on the server!";
        }
    } 
    catch (e) 
    {
        alert("Error: " + e);
    }
}
showTestCallback.runat = "client";

function testDatabase()
{
    var resultSet = Jaxer.DB.frameworkExecute("SELECT * FROM callback_page");
    var numCallbacks = resultSet.rows.length;
    return "There are currently " + numCallbacks + " function blocks saved to the database";
}
testDatabase.proxy = true;

function showTestDatabase(id)
{
    try 
    {
        var result = testDatabase();
        findAndShowSuccess(id, result);
    } 
    catch (e) 
    {
        alert("Error: " + e);
    }
}
showTestDatabase.runat = "client";

