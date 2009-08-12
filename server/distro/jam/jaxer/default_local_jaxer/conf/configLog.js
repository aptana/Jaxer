/* 
 * Use this file to override the default log settings.
 * Since it only gets read at server startup, you'll need to restart Jaxer.
 * 
 * The Log levels are, in descending order of severity :
 *   Jaxer.Log.TRACE 
 *   Jaxer.Log.DEBUG
 *   Jaxer.Log.INFO 
 *   Jaxer.Log.WARN 
 *   Jaxer.Log.ERROR
 *   Jaxer.Log.FATAL
 *   
 */
(function() {
	
	/* 
	 * If you'd like stack traces for log messages at levels other than the default ERROR, change:
	 */
//	Jaxer.Log.minLevelForStackTrace = Jaxer.Log.ERROR;
	
	/*
	 * If you want to make everything verbose, change INFO -> TRACE in the following
	 * To restore to the default setting, change it back to Jaxer.Log.setAllModuleLevels(Jaxer.Log.INFO);
	 */
//	Jaxer.Log.setAllModuleLevels(Jaxer.Log.INFO);
	
	/* If you want to enable Jaxer logging to the browser console 
	 * (for the Jaxer client framework and your client-side code), uncomment the following.
	 * If the browser does not support a console, this functionality will automatically be disabled.
	 */
//  Jaxer.Log.CLIENT_SIDE_CONSOLE_SUPPORT = true;
	
	/* 
	 * Use the following for when you call Jaxer.Log.<method> directly, without a named module:
	 */
//	Jaxer.Log.genericLogger.setLevel(Jaxer.Log.DEBUG);
	
	/* 
	 * The following are some modules you might want to turn to verbose 
	 * individually
	 */
//	Jaxer.Log.forModule("App").setLevel(Jaxer.Log.TRACE);
	
//	Jaxer.Log.forModule("CacheManager").setLevel(Jaxer.Log.TRACE);
	
//	Jaxer.Log.forModule("Container").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("DBPeristor").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("SessionManager").setLevel(Jaxer.Log.TRACE);

//	Jaxer.Log.forModule("Callback").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("CoreEvents").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("HandlePage").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("HandleCallback").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("HandleService").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("InitHead").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("NewElement").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("ParseComplete").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("ParseStart").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("Request").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("RequestComplete").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("RequestStart").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("Response").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("ServerStart").setLevel(Jaxer.Log.TRACE);

//	Jaxer.Log.forModule("DB.MySQL").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("DB.SQLite").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("DB").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("ResultSet").setLevel(Jaxer.Log.TRACE);

//	Jaxer.Log.forModule("Extend").setLevel(Jaxer.Log.TRACE);

//	Jaxer.Log.forModule("BinarySocket").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("Includer").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("SMPT").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("Socket").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("Web").setLevel(Jaxer.Log.TRACE);

//	Jaxer.Log.forModule("CallbackManager").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("FunctionInfo").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("ScriptInfo").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("ScriptProcessor").setLevel(Jaxer.Log.TRACE);
//	Jaxer.Log.forModule("TextParser").setLevel(Jaxer.Log.TRACE);

//	Jaxer.Log.forModule("Sandbox").setLevel(Jaxer.Log.TRACE);

//	Jaxer.Log.forModule("Process").setLevel(Jaxer.Log.TRACE);

//	Jaxer.Log.forModule("Thread").setLevel(Jaxer.Log.TRACE);
	
	
})();
