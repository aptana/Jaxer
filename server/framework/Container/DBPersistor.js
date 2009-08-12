/* ***** BEGIN LICENSE BLOCK *****
 * Version: GPL 3
 *
 * This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 * This program is licensed under the GNU General Public license, version 3 (GPL).
 *
 * This program is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 * is prohibited.
 *
 * You can redistribute and/or modify this program under the terms of the GPL, 
 * as published by the Free Software Foundation.  You should
 * have received a copy of the GNU General Public License, Version 3 along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 * Aptana provides a special exception to allow redistribution of this file
 * with certain other code and certain additional terms
 * pursuant to Section 7 of the GPL. You may view the exception and these
 * terms on the web at http://www.aptana.com/legal/gpl/.
 * 
 * You may view the GPL, and Aptana's exception and additional terms in the file
 * titled license-jaxer.html in the main distribution folder of this program.
 * 
 * Any modifications to this file must keep this entire header intact.
 *
 * ***** END LICENSE BLOCK ***** */

(function(){

var log = Log.forModule("DBPersistor"); // Only if Log itself is defined at this point of the includes

/**
 * @classDescription {Jaxer.DBPersistor} A database-based persistor for Jaxer
 * Container objects (session, sessionPage, etc.)
 */

/**
 * A database-based persistor for Jaxer Container objects (session, sessionPage,
 * etc.)
 * 
 * @advanced
 * @constructor
 * @alias Jaxer.DBPersistor
 * @return {Jaxer.DBPersistor}
 * 		Returns an instance of DBPersistor.
 */
function DBPersistor()
{
	this.persistByName = true;
}

/**
 * Retrieve a given container type's and name's property, by name
 * 
 * @advanced
 * @alias Jaxer.DBPersistor.prototype.load
 * @param {String} type
 * 		The type of the container
 * @param {String} key
 * 		The key for the specific instance of the container (e.g. the sessionPage 
 * 		ontainer is per page, and the page's key is used here)
 * @param {String} name
 * 		The name of the property to query for
 * @return {String}
 * 		The value of the (usually serialized) requested property, 
 * 		or the empty string if there is none
 */
DBPersistor.prototype.load = function load(type, key, name)
{
	log.debug("Loading: " + [type, key, name]);
	var rs = DB.frameworkExecute("SELECT value FROM containers WHERE `type` = ? AND `key` = ? AND `name` = ?",
		[type, key, name]);
	if (rs.rows.length == 0)
	{
		log.debug("No values found");
		return {};
	}
	else
	{
		var value = rs.rows[0].value;
		log.debug("Found: " + value);
		return value;
	}
};

/**
 * Load all the name-value properties at once for the given container type and
 * key
 * 
 * @advanced
 * @alias Jaxer.DBPersistor.prototype.loadAll
 * @param {String} type
 * 		The type of the container
 * @param {String} key
 * 		The key for the specific instance of the container (e.g. the sessionPage
 * 		container is per page, and the page's key is used here)
 * @return {Object}
 * 		A hashmap of name-value pairs; the values are usually serialized
 * 		and need to be deserialized via Serialization.fromJSONString
 */
DBPersistor.prototype.loadAll = function loadAll(type, key)
{
	log.debug("Loading all: " + [type, key]);
	var rs = DB.frameworkExecute("SELECT name, value FROM containers WHERE `type` = ? AND `key` = ?",
		[type, key]);
	var results = {};
	var iReturned = 0;
	for (var i=0; i<rs.rows.length; i++)
	{
		var row = rs.rows[i];
		results[row.name] = row.value;
		iReturned++;
	}
	log.debug("Returning " + iReturned + " results");
	return results;
};

/**
 * Persist a particular property (by name) for the given container type and key
 * 
 * @advanced
 * @alias Jaxer.DBPersistor.prototype.persist
 * @param {String} type
 * 		The type of the container
 * @param {String} key
 * 		The key for the specific instance of the container (e.g. the sessionPage
 * 		container is per page, and the page's key is used here)
 * @param {String} name
 * 		The name of the property to persist
 * @param {String} data
 * 		The value to persist for this property, which should have already
 * 		been serialized (using Serialization.toJSONString)
 */
DBPersistor.prototype.persist = function persist(type, key, name, data)
{
	log.debug("Persisting " + [type, key, name, data]);
	var now = new Date();
	// In MySQL we could use the following statement:
	//DB.frameworkExecute("INSERT INTO containers (" + fields + ") VALUES (" + placeholders + ")" +
	//	" ON DUPLICATE KEY UPDATE `value` = VALUES(`value`), `modification_datetime` = VALUES(`modification_datetime`)",
	//	values);
	// But to be DB-independent, we need to first see if it exists and update, or else insert
	// TODO: This is NOT race-condition-safe
	var rs = DB.frameworkExecute("SELECT id FROM containers WHERE `type`=? AND `key`=? AND `name`=?", [type, key, name]);
	if (rs.rows.length > 0)
	{
		DB.frameworkExecute("UPDATE containers SET `value`=?, `modification_datetime`=? WHERE id=?", [data, now, rs.rows[0].id]);
	}
	else
	{
		var fields = "`" + ['type', 'key', 'name', 'value', 'creation_datetime', 'modification_datetime'].join("`,`") + "`";
		var values = [type, key, name, data, now, now];
		var placeholders = values.map(function(field) { return "?"; });
		DB.frameworkExecute("INSERT INTO containers (" + fields + ") VALUES (" + placeholders + ")", values);
	}
};

/**
 * Completely remove the given property from the database
 * 
 * @advanced
 * @alias Jaxer.DBPersistor.prototype.remove
 * @param {String} type
 * 		The type of the container
 * @param {String} key
 * 		The key for the specific instance of the container (e.g. the sessionPage
 * 		container is per page, and the page's key is used here)
 * @param {String} name
 * 		The name of the property to remove
 */
DBPersistor.prototype.remove = function remove(type, key, name)
{
	log.debug("Deleting " + [type, key, name]);
	DB.frameworkExecute("DELETE FROM containers WHERE `type` = ? AND `key` = ? AND `name` = ?",
		[type, key, name]);
};

/**
 * Completely remove all the container's properties from the database
 * 
 * @advanced
 * @alias Jaxer.DBPersistor.prototype.removeAll
 * @param {String} type
 * 		The type of the container
 * @param {String} key
 * 		The key for the specific instance of the container (e.g. the sessionPage
 * 		container is per page, and the page's key is used here)
 */
DBPersistor.prototype.removeAll = function removeAll(type, key)
{
	log.debug("Removing all from " + [type, key]);
	DB.frameworkExecute("DELETE FROM containers WHERE `type` = ? AND `key` = ?",
		[type, key]);
};

/**
 * Creates the database schema needed to persist containers
 * 
 * @advanced
 * @alias Jaxer.DBPersistor.createSchema
 */
DBPersistor.createSchema = function createSchema() // static
{
	sql = "CREATE TABLE IF NOT EXISTS containers (" +
		" `id` INTEGER PRIMARY KEY AUTO_INCREMENT," +
		" `type` VARCHAR(32) NOT NULL," +
		" `key` VARCHAR(255) NOT NULL," +
		" `name` VARCHAR(255) NOT NULL," +
		" `value` LONGTEXT," + 
		" `creation_datetime` DATETIME NOT NULL," +
		" `modification_datetime` DATETIME NOT NULL" +
		")";
	DB.frameworkExecute(sql);
};

Container.DBPersistor = DBPersistor;

Log.trace("*** DBPersistor.js loaded");  // Only if Log itself is defined at this point of the includes

})();
