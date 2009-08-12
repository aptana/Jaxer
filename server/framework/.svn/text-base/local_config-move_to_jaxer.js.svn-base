(function(){
	
	var depotDir = "file:///Users/sarid/Documents/Aptana Studio/js_framework";
	var depotConfigPath = depotDir + "/config.js";
	// Get the latest config settings from the local depot copy, not the install directory
	Jaxer.include(depotConfigPath);
	// But the depot's config will point back to the install directory, so override that explicitly
	Config.FRAMEWORK_DIR = depotDir;
	// Now override anything you want:
	Config.EMBEDDED_CLIENT_FRAMEWORK_SRC = depotDir + "/clientFramework_compressed.js";
})();

