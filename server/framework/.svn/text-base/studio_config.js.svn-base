/* 
 * Use this file to override the default config.js settings with platform-specific
 * settings, in this case for Jaxer embedded in the Aptana Studio platform.
 * It will only be loaded if its path is specified in the command line to
 * JaxerManager as the value of the PLATFORM_CONF_PATH argument.
 * This file is read right after the default config.js is read
 * but before anything else is done, so you can override anything you need.
 * Since these only gets read at server startup, you'll need to restart Jaxer
 * after modifying this file for those modifications to have an effect.
 * Note that you have full read access to not only the PLATFORM_CONF_PATH's value
 * but also to the command line args via Config.COMMAND_LINE_PARAMS.
 */

(function() {

	// E.g.: WORKSPACE_DATA_PATH=file:///Users/<username>/Documents/Aptana%20Studio/.metadata/.plugins/com.aptana.ide.framework.jaxer/jaxer_studio_data
	if ('WORKSPACE_DATA_PATH' in Config.COMMAND_LINE_PARAMS) 
	{
		var wsDataPath = Config.COMMAND_LINE_PARAMS.WORKSPACE_DATA_PATH;
		
		if ('PATH' in Config.FRAMEWORK_DB) 
			Config.FRAMEWORK_DB.PATH = wsDataPath + '/local_jaxer/data/frameworkdata.sqlite';
			
		Config.DEFAULT_PATH_BASE    = wsDataPath + '/local_jaxer/data';
		Config.LOCAL_CONF_DIR 		= wsDataPath + '/local_jaxer/conf';
		Config.LOCAL_EXTENSIONS_DIR = wsDataPath + '/local_jaxer/extensions';
		
	}
	
	// Studio will embed the (compressed) framework packaged with its version of Jaxer
	Config.EMBEDDED_CLIENT_FRAMEWORK_SRC = 'resource:///framework/clientFramework_compressed.js';
	
	// Override the default config.js to make development easier within Studio.
	// This can in turn be overridden with local_jaxer/conf/config.js
	Config.DEV_MODE = true;

})();
