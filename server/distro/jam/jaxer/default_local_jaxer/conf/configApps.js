/* 
 * ***********************************************************************************************
 * Use this file to override the default configApps settings.
 * 
 * This file is loaded and executed
 * - right after the default configApps.js is read
 * - but before anything else is done, 
 * so you can override anything you need.
 * 
 * Any other files in the same folder (Config.LOCAL_CONF_DIR) whose names end with configApps.js
 * (e.g. my.configApps.js or myApp-configApps.js, but not fooconfigApps.js) will also be
 * loaded and executed automatically, in alphabetic order.
 * 
 * Since this file (and its similarly-named peers) are only loaded at server startup, 
 * you need to restart Jaxer after making changes so your changes take effect.
 *   
 * In this file we define some example database connection objects. These are shown only
 * as a convenience and are not required to define an application.
 * 
 * ***********************************************************************************************
 * 
 * An application is defined by a JavaScript object containing a number of expected functions.
 * 
 * urlTest - This is used to match an application object to the url currently being
 * handled by the request. It can be a function, that's passed the request's parsedUrl and
 * should return true if it matches; it can be a regular expression, which should match against the 
 * request's parsedUrl's pathAndFile; or it can be a string, which should match the parsedUrl's 
 * pathAndFile (if the string contains a ".") or the parsedUrl's first pathPart (if it does not).
 * For each request the server will match the url against its ordered list of applications
 * (the Config.apps array) and will use the first application object whose urlTest matches. 
 * It is therefore important to consider the order that applications are added to the list. 
 * The widest-matching applications should be placed at the end of the list.
 * 
 * name - This should be a string, or a function that returns a string, identifying the application.
 * It should be unique across all applications. 
 * If a function, its sole argument is the request's parsedUrl.
 * 
 * path - This should be a string, or a function that returns a string, specifying the filesystem 
 * location used to resolve relative file and directory paths. 
 * If a function, its sole argument is the request's parsedUrl.
 * 
 * db - This should be a Jaxer database connection parameters object, or a function that returns a 
 * Jaxer database connection parameters object, to be used as the default connection by Jaxer.DB 
 * during requests to this application.
 * If a function, its sole argument is the request's parsedUrl.
 * 
 * handler - For service-type (not page-type) requests where Jaxer is the handler 
 * (and it's not a callback), this determines how the request is handled. 
 * For other requests, do not use a handler property at all.
 * It can be a string or a function:
 * the function takes the app's name and the request's parsedUrl as its two parameters, 
 * and is executed right after the (empty) DOM is created and ready. If it returns a string,
 * or if the handler property is already a string, that string should be the file://...-type URL  
 * location of a JavaScript file to be loaded and executed to handle this request.
 * 
 * You can also add additional optional properties to application objects, as shown in the 
 * sample JAXER_DIAGNOSTICS. The optional properties are available to the application
 * code via the Jaxer.request.app namespace.
 * 
 * Any properties you do not explicitly define will be inherited from the default application specification.
 * 
 * Note: for each of these properties the Jaxer.request.app will have a corresponding property
 * in all-uppercase that gives the fully-resolved value of that property -- that is,
 * the result of evaluating the lowercase property's value (e.g. if it was a function).
 *   
 * ***********************************************************************************************
 * 
 * In this file we use Config.apps.unshift(...) to insert the application definition
 * entries at the top of the list. In this way you can always add more specific application
 * definitions later, at the bottom of this file or in similarly-named files, 
 * and they will appear at the top of the Config.apps list and be tested first.
 * 
 * ***********************************************************************************************
 */
 
(function() {
 
		/* =========================================================================================
		 * 
		 * Setup the default environment
		 * 
		 * ========================================================================================= */ 
		
        /*
         *      For convenience, you can define your connection params once and then reuse them or
         *      extend them for multiple apps. Of course, these variables only have an effect when you
         *      actually use them in the config parameters further down...
         */ 
        var mySQL =
        {
                IMPLEMENTATION: "MySQL"
        ,       HOST: "127.0.0.1"
        ,       PORT: 3306
		,		SOCKET: null  // to use this, set it to '/absolute/path/to/socket/file'
        ,       NAME: "demos"
        ,       USER: "root"
        ,       PASS: ""
        ,       CLOSE_AFTER_EXECUTE: false
        ,       CLOSE_AFTER_REQUEST: false
        };
        
        var sqlite = 
        {
                IMPLEMENTATION: "SQLite"
        ,       PATH: Dir.combine(Config.DEFAULT_PATH_BASE, "appdata.sqlite")
        ,       CLOSE_AFTER_EXECUTE: false
        ,       CLOSE_AFTER_REQUEST: true
        ,       MAX_NUM_TRIES: 100              // if locking is detected, retry up to this many times
        ,       MILLIS_BETWEEN_TRIES: 37        // if locking is detected, sleep this many milliseconds between tries
        }
        
        /*
         *      To override the default app's db assign a well formed db params object, such as one of the previous,
         *      to Config.DEFAULT_DB. 
         *      
         *      Doing this keeps the default app's "smart" db logic intact:
         *      - SQLite: use Config.DEFAULT_DB but override the PATH with a path specific to the app's dynamically-determined name
         *      - MySQL: use Config.DEFAULT_DB but override the db name with the app's dynamically-determined name. 
         *      
         *      While this won't matter much for the default app itself (since its name is constant)
         *      it may come in useful if your apps reuse the default app's db property.
         *      
         *      Uncomment the one you want to use:
         */
//      Config.DEFAULT_DB = mySQL;
//      Config.DEFAULT_DB = sqlite;
 
        /*
        *       Alternatively, you can override the default app's db logic directly.
        *       Uncomment the one you want to use:
        */
//      Config.DEFAULT_APP.db = mySQL;
//      Config.DEFAULT_APP.db = sqlite;
 
 
		/* =========================================================================================
		 * 
		 * Define the applications recognized by this server
		 * 
		 * ========================================================================================= */  
 
        /* 
         *      Here's a common scenario as a sample application definition:
         *      
         *      If the URL has a path after its domain (e.g. www.mydomain.com/myapp/foo.html)
         *      then use it ("myapp" in this case) to define the application context.
         */
        var COMMON_APP = 
        {
                /*
                 * urlTest is defined as a function that returns true or false. It gets the
                 * parsed url object of the current request, which is used to determine whether
                 * the requested url belongs to this application.
                 */
                urlTest: function(parsedUrl) 
                { 
                        return parsedUrl.pathParts.length > 1; 
                },
                /*
                 * name is defined as a function which takes the parsed request url as a parameter and
                 * from that will return a string which is used as the name for the application.
                 * This value is accessible via the Jaxer.request.app.NAME property during request processing
                 * within jaxer.
                 */
                name:    function(parsedUrl) 
                { 
                        return parsedUrl.pathParts[0]; 
                },
                /*
                 * db is a defined as a function that returns a valid database connection object.
                 * This example uses the built in "smart" Config.DEFAULT_APP.db function. 
                 * Note that if you use the default value you can omit this function entirely.
                 * 
                 * Jaxer.DB by default will use the db function to determine which database to use, 
                 * so Jaxer.DB in different application contexts can return a different DB object
                 */
                db:      Config.DEFAULT_APP.db
        }       
        /* 
         * To enable COMMON_APP definition simply uncomment the following line
         */
//      Config.apps.unshift(COMMON_APP);
 
})();