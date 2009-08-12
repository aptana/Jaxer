/*
 * Menu: Jaxer > Stop Jaxer
 * _Toolbar: Jaxer > Stop Jaxer
 * _Image: icons/window1616.png
 * Key: M2+F12
 * Kudos: Kevin Lindsey
 * License: EPL 1.0
 * DOM: http://download.eclipse.org/technology/dash/update/org.eclipse.eclipsemonkey.lang.javascript
 * DOM: http://localhost/com.aptana.ide.scripting
 */

// includes
include("IDE_Utils.js");

/**
 * main
 */
function main()
{
	var os = getProperty("osgi.os");
	
	if (os == "win32")
	{
		var imageName = "Jaxer.exe";
		var systemRoot = java.lang.System.getenv("SystemRoot");
		var system32 = systemRoot + File.separator + "system32";
		var taskkill = system32 + File.separator + "taskkill";
		
		java.lang.Runtime.getRuntime().exec(taskkill + ' /F /IM ' + imageName);
		
		out.println(imageName + " shut down");
	}
}
