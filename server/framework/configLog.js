/* 
 * The following set the default log settings.
 * To override them, it's best to use a separate configLog.js (usually in a local_jaxer/conf folder)
 * so your changes do not get overwritten by a new Jaxer upgrade.
 * This file is only read at server startup.
 * 
 * The Log levels are:
 *   Log.TRACE, Log.DEBUG, Log.INFO, Log.WARN, Log.ERROR, Log.FATAL
 *   
 */
(function() {
	
	Log.minLevelForStackTrace = Log.ERROR;
	
	Log.setAllModuleLevels(Log.INFO);
	
	Log.CLIENT_SIDE_CONSOLE_SUPPORT = false;
			
})();
