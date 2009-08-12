(function() {

Config.apps = [];
Config.appsByName = {};

/**
 * Whether to automatically create folders at the locations specified by each app's (resolved) PATH property,
 * either at server start (if the app's name is a string) or when the app is first requested (if the app's
 * name is a function of the request). This is true by default.
 * @alias Jaxer.Config.AUTO_CREATE_APP_PATHS
 * @property {Boolean}
 */
Config.AUTO_CREATE_APP_PATHS = true;

/**
 * For convenience, a central starting point from which to build paths used as default
 * locations where apps read and write files. To make it easier to use cross-platform, 
 * should include trailing path separator.
 * @alias Jaxer.Config.DEFAULT_PATH_BASE
 * @property {String}
 */
if (!Config.DEFAULT_PATH_BASE) Config.DEFAULT_PATH_BASE = "resource:///../local_jaxer/data"; // Don't override if it's already been set (in platform-specific file, probably)

/**
 * Contains the parameters for a connection object of one of the supported DB implementations.
 * At the very least, the IMPLEMENTATION property must be defined and be the name of one
 * of the supported implementations in the DB namespace (e.g. "SQLite", "MySQL").
 * The remaining parameters have all the information required to connect to the database.
 * This will be used for pages and applications that don't match any of
 * the custom entries in Jaxer.Config.apps, as defined in configApps.js.
 * This property is best managed in the local_jaxer/conf/configApps.js which will not be overwritten by subsequent
 * jaxer updates. Changes in that file will get picked up automatically on restart.
 * 
 * @alias Jaxer.Config.DEFAULT_DB
 * @see Jaxer.DB.MySQL.Connection
 * @see Jaxer.DB.SQLite.Connection
 * @property {Object}
 */
Config.DEFAULT_DB =
{
	// Appropriate for the Aptana Jaxer distribution: 
	// the data folder is a peer of the folder containing the executable
	IMPLEMENTATION: "SQLite",
	PATH: Dir.combine(Config.DEFAULT_PATH_BASE, "appdata.sqlite"),
	CLOSE_AFTER_EXECUTE: false,
	CLOSE_AFTER_REQUEST: true,
	MAX_NUM_TRIES: 100, 		// if locking is detected, retry up to this many times
	MILLIS_BETWEEN_TRIES: 37	// if locking is detected, sleep this many milliseconds between tries
};

function smartDB(resolvedName, parsedUrl)
{
	switch (Config.DEFAULT_DB.IMPLEMENTATION)
	{
		case "SQLite":
			// Note the following path will be created automatically if it's also your app's path property:
			return Util.extend(Config.DEFAULT_DB, {PATH: Dir.combine(Config.DEFAULT_PATH_BASE, resolvedName, "appdata.sqlite")});
		case "MySQL":
			return Util.extend(Config.DEFAULT_DB, {NAME: resolvedName});
		default:
			throw new Exception("Unrecognized database implementation: " + Config.DEFAULT_DB.IMPLEMENTATION);
	}
}

/**
 * Contains the parameters that will be used for pages and applications that don't match any of
 * the custom entries in Jaxer.Config.apps, as defined in configApps.js.
 * This property is best managed, if needed, in the local_jaxer/conf/configApps.js which will not be overwritten by subsequent
 * jaxer updates. Changes in that file will get picked up automatically on restart.
 * Note: even though this is created as a plain object (hashmap) in config.js, it is wrapped
 * up into a Jaxer.App object upon server start
 * 
 * @alias Jaxer.Config.DEFAULT_APP
 * @property {Object}
 */
Config.DEFAULT_APP =
{
	urlTest: /.*/,
	name: "DEFAULT",
	db: smartDB,
	path: function(resolvedName, parsedUrl) { return Dir.combine(Config.DEFAULT_PATH_BASE, resolvedName); },
    handler : function(resolvedName, parsedUrl) {
            return (/\bindex.js$/.test(Jaxer.request.pageFile)) ? 'file://'+Jaxer.request.pageFile : null;
    },
	pageKey: function(parsedUrl) { return parsedUrl.hostAndPort + parsedUrl.path; },
	appKey: function(parsedUrl) { return parsedUrl.hostAndPort; }
};

})();
