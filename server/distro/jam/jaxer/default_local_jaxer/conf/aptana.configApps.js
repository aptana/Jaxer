/* 
 * ***********************************************************************************************
 * 
 * This file defines some apps that are shipped by default with Jaxer. By defining them explicitly
 * in Config.apps, their databases and their data files will be separated from your apps.
 * 
 * ***********************************************************************************************
 */
 
(function() {
 
        /*
         *  	This example sets each of the sample apps to have separate application scopes
         *      The expected URL is in the format http://localhost:8081/aptana/samples/<sample_name>/index.html
         *      The sqlite dbs and any files will be created under the data/samples/<sample_name> folder
         */     
        var JAXER_SAMPLES = 
        {
                /*
                 * 		look for a matching URL of the format 
                 *      http://<DOMAIN>/aptana/samples/<sample_name>/index.html
                 */
                urlTest: function(parsedUrl) 
                { 
                        // if this function returns true this rule is used to apply application level scope.
                        return (parsedUrl.pathParts.length > 2) 
                        && (parsedUrl.pathParts[0] == 'aptana') 
                        && (parsedUrl.pathParts[1] == 'samples'); 
                },
                name: function(parsedUrl) 
                { 
                        return parsedUrl.pathParts[2]; 
                },
                
                path: function(resolvedName, parsedUrl) 
                { 
                        return Dir.combine(Config.DEFAULT_PATH_BASE, 'samples',resolvedName); 
                },
                
                db: function(resolvedName, parsedUrl)
                {
                        switch (Config.DEFAULT_DB.IMPLEMENTATION)
                        {
                                case "SQLite":
                                        // Note the following path will be created automatically if it's also your app's path property:
                                        return Util.extend(Config.DEFAULT_DB, {
                                                PATH: Dir.combine(Config.DEFAULT_PATH_BASE, 'samples', resolvedName, resolvedName+".sqlite")
                                        });
                                case "MySQL":
                                        return Util.extend(Config.DEFAULT_DB, {
                                                NAME: resolvedName
                                        });
                                default:
                                        throw new Exception("Unrecognized database implementation: " + Config.DEFAULT_DB.IMPLEMENTATION);
                        }
                }
        };
        Config.apps.unshift(JAXER_SAMPLES); 
 
        /* 		The diagnostic tests have their own application scope
         * 		The expected URL is in the format
         *      http://<DOMAIN>/aptana/diagnostics/...aptana
         *
         * 		The sqlite dbs and any files will be created under the local_jaxer/data/diagnostics folder
         */
        var JAXER_DIAGNOSTICS = 
        {
                /*
                 * 		look for a matching URL of the format 
                 *      http://<DOMAIN>/aptana/diagnostics/...
                 */
                urlTest: function(parsedUrl) 
                { 
                        return (parsedUrl.pathParts.length > 1) 
                        && (parsedUrl.pathParts[0] == 'aptana') 
                        && (parsedUrl.pathParts[1] == 'diagnostics'); 
                },
        
                name: function(parsedUrl) 
                { 
                        return 'diagnostics'; 
                }, 
                
                path: function(resolvedName, parsedUrl) 
                { 
                        return Dir.combine(Config.DEFAULT_PATH_BASE, 'diagnostics'); 
                },
                
                db: function(resolvedName, parsedUrl)
                {
                        switch (Config.DEFAULT_DB.IMPLEMENTATION)
                        {
                                case "SQLite":
                                        // Note the following path will be created automatically if it's also your app's path property:
                                        return Util.extend(Config.DEFAULT_DB, {
                                                PATH: Dir.combine(Config.DEFAULT_PATH_BASE, 'diagnostics', "diagnostics.sqlite")
                                        });
                                case "MySQL":
                                        return Util.extend(Config.DEFAULT_DB, {
                                                NAME: resolvedName
                                        });
                                default:
                                        throw new Exception("Unrecognized database implementation: " + Config.DEFAULT_DB.IMPLEMENTATION);
                        }
                },
                /*
                 * 		The following properties can be accessed via the Jaxer.request.app.<CONSTANT> properties 
                 * 		during request processing
                 */
        		MAIL_SERVER : 'localhost',
                APPLICATION_LABEL : 'diagnostic tests'
        }       
        Config.apps.unshift(JAXER_DIAGNOSTICS);
 
        /*
	     * Define the interactive shell application context
         * The expected URL is in the format
         *              http://<DOMAIN>/aptana/tools/shell
         *
         * The sqlite dbs and any files will be created under the local_jaxer/data/shell folder
         */
        var JAXER_SHELL = 
        {
                /* look for a matching URL of the format 
                 *      http://<DOMAIN>/aptana/tools/shell/...
                 */
                urlTest: function(parsedUrl) 
                { 
                        return (parsedUrl.pathParts.length > 2) 
                        && (parsedUrl.pathParts[0] == 'aptana') 
                        && (parsedUrl.pathParts[1] == 'tools')
                        && (parsedUrl.pathParts[2] == 'shell'); 
                },
                
                name: function(parsedUrl) 
                { 
                        return 'shell'; 
                }, 
                
                path: function(resolvedName, parsedUrl) 
                { 
                        return Dir.combine(Config.DEFAULT_PATH_BASE, 'tools', 'shell'); 
                },
                
                db: function(resolvedName, parsedUrl)
                {
                        switch (Config.DEFAULT_DB.IMPLEMENTATION)
                        {
                                case "SQLite":
                                        // Note the following path will be created automatically if it's also your app's path property:
                                        return Util.extend(Config.DEFAULT_DB, {
                                                PATH: Dir.combine(Config.DEFAULT_PATH_BASE, 'tools','shell', "shell.sqlite")
                                        });
                                case "MySQL":
                                        return Util.extend(Config.DEFAULT_DB, {
                                                NAME: resolvedName
                                        });
                                default:
                                        throw new Exception("Unrecognized database implementation: " + Config.DEFAULT_DB.IMPLEMENTATION);
                        }
                },
                // Note that the Jaxer shell program allows unrestricted access to server-side resources
				// so understand the security implications before proceeding.
				// For example, you may want to password-protect web server access to this program.
                // To enable access to the Jaxer shell program, set the SHELL_ENABLED function to return true:
                SHELL_ENABLED : function() { return false; }
                // Alternatively, restricting access to your specific IP may provide you with adequate security:
                //SHELL_ENABLED : function() { return (Jaxer.request.remoteAddr == "10.10.1.100"); } // change to your allowed client's IP address
				// Or you could allow only local access, from your server, but note that pages loaded into
				// a Jaxer.Sandbox could make local requests to the Jaxer shell so this may not be very secure:
                //SHELL_ENABLED : function() { return (Jaxer.request.remoteAddr == "127.0.0.1" || Jaxer.request.remoteHost == "localhost"); }
        };
        Config.apps.unshift(JAXER_SHELL); 
 
})();