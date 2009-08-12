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

var log = Log.forModule("Container");

/**
 * @classDescription {Jaxer.Container} 
 * 		Container object that is used for all types of containers (e.g. session, sessionPage, etc).
 * 
 */

/**
 * This is the contructor for the Container object, used for all types of
 * containers (session, sessionPage, etc).
 * 
 * @advanced
 * @constructor
 * @alias Jaxer.Container
 * @param {String} type
 * 		The type of the container
 * @param {String} key
 * 		The key for the specific instance of the container (e.g. the sessionPage
 * 		container is per page, and the page's key is used here)
 * @param {Object} persistor
 * 		An instance of the persistor (e.g. a DBPersistor) to be used to persist
 * 		this container
 * @param {String} name
 * 		The name of this container
 * @return {Jaxer.Container}
 * 		Returns an instance of Container.
 */
function Container(type, key, persistor, name)
{
	this.type = type;
	this.key = key;
	this.id = this.type + ":" + this.key;
	this.persistor = persistor;
	this.name = name;
	this.isLoaded = false;
	this.names = [];
	this.data = {};
	this.addedData = {}; 	// true ==> newly added
	this.changedData = {};	// defined ==> changed, holds previous value
	this.deletedData = {};	// defined ==> deleted, holds previous value
}

/**
 * Load the container from its store, but only if it has not yet been loaded
 * 
 * @private
 * @alias Jaxer.Container.prototype.loadIfNeeded
 */
Container.prototype.loadIfNeeded = function loadIfNeeded()
{
	if (!this.isLoaded)
	{
		log.debug("Loading container " + this.id + " for first time")
		this.data = this.persistor.loadAll(this.type, this.key);
		for (var p in this.data)
		{
			this.data[p] = Serialization.fromJSONString(this.data[p], {as: Serialization.JAXER_METHOD});
			this.names.push(p);
		}
		this.isLoaded = true;
	}
};

/**
 * Gets the value of the given property
 * 
 * @private
 * @alias Jaxer.Container.prototype.get
 * @param {String} name
 * 		The name of the property whose value we need
 * @return {Object}
 * 		The value
 */
Container.prototype.get = function get(name)
{
	this.loadIfNeeded();
	
	return this.data[name];
}

/**
 * Sets a name-value pair in the current container.
 * 
 * @private
 * @alias Jaxer.Container.prototype.set
 * @param {String} name
 * 		The name of the property to set
 * @param {Object} value
 * 		The value of the property
 */
Container.prototype.set = function set(name, value)
{
	var valueForLogging = String(value);
	valueForLogging = valueForLogging.substr(0, 30) + ((valueForLogging.length > 30) ? "..." : "");
	log.trace("For container " + this.id + ": setting '" + name + "' = " + valueForLogging);
	this.loadIfNeeded();
	var nowExists = this.data.hasOwnProperty(name);
	var newlyAdded = this.addedData.hasOwnProperty(name);
	var wasDeleted = this.deletedData.hasOwnProperty(name);
	var wasChanged = this.changedData.hasOwnProperty(name);
	var previouslyPersisted = (nowExists && !newlyAdded) || wasDeleted;
	delete this.deletedData[name];
	if (previouslyPersisted)
	{
		if (!wasChanged) // This is the first time it's changed since we loaded it
		{
			this.changedData[name] = this.data[name]; // Hold the previous value
		}
	}
	else
	{
		if (!newlyAdded)  // This is the first time it's being set
		{
			this.addedData[name] = true;
		}
	}
	if (!nowExists)
	{
		this.names.push(name);
	}
	this.data[name] = (typeof value == "undefined") ? Container.DEFAULT_VALUE : value;
	valueForLogging = String(this.data[name]);
	valueForLogging = valueForLogging.substr(0, 30) + ((valueForLogging.length > 30) ? "..." : "");
	var changedValueForLogging = String(this.changedData[name]);
	changedValueForLogging = changedValueForLogging.substr(0, 30) + ((changedValueForLogging.length > 30) ? "..." : "");
	log.debug("For container " + this.id + ": this.data['" + name + "'] = " + valueForLogging + "; this.changedData['" + name + "'] = " + changedValueForLogging);
}

/**
 * Mark a property as changed so it persists
 * 
 * @private
 * @alias Jaxer.Container.prototype.touch
 * @param {String} name
 * 		The name of the property to touch
 */
Container.prototype.touch = function touch(name)
{
	log.trace("For container " + this.id + ": Touching " + name);
	this.loadIfNeeded();
	var nowExists = this.data.hasOwnProperty(name);
	var value = nowExists ? this.data[name] : Container.DEFAULT_VALUE;
	this.set(name, value);
}

/**
 * Does the container have the given property set?
 * 
 * @private
 * @alias Jaxer.Container.prototype.exists
 * @param {String} name
 * 		The name of the property to look for
 * @return {Boolean} true if it exists, false otherwise
 */
Container.prototype.exists = function exists(name)
{
	this.loadIfNeeded();
	return this.data.hasOwnProperty(name);
}

/**
 * Was this property just created, or was it previously persisted?
 * 
 * @private
 * @alias Jaxer.Container.prototype.isPreviouslyPersisted
 * @param {String} name
 * 		The name of the property to query
 * @return {Boolean}
 * 		true if it was already there before, false if it's newly added
 */
Container.prototype.isPreviouslyPersisted = function isPreviouslyPersisted(name)
{
	this.loadIfNeeded();
	var nowExists = this.data.hasOwnProperty(name);
	var newlyAdded = this.addedData.hasOwnProperty(name);
	var wasDeleted = this.deletedData.hasOwnProperty(name);
	return (nowExists && !newlyAdded) || wasDeleted;
}

/**
 * Remove (unset) a property
 * 
 * @private
 * @alias Jaxer.Container.prototype.remove
 * @param {String} name
 * 		The name of the property to remove
 */
Container.prototype.remove = function remove(name)
{
	log.trace("For container " + this.id + ": Removing " + name);
	this.loadIfNeeded();
	if (!this.data.hasOwnProperty(name))
	{
		log.debug("Nothing to remove: " + name);
		return; // Nothing to do
	}
	if (this.isPreviouslyPersisted(name)) // otherwise we'll need this info to remove the name-value from the persistent store
	{
		log.debug("We still need to remember that '" + name + "' was changed because it was previously persisted");
		this.deletedData[name] = this.data[name];
	}
	else
	{
		log.debug("No need to remember that '" + name + "' was changed because it was not previously persisted");
	}
	var iName = this.names.indexOf(name);
	delete this.names[iName];
	delete this.data[name];
	delete this.changedData[name];
}

/**
 * Removes (unsets) all properties on this container
 * 
 * @private
 * @alias Jaxer.Container.prototype.removeAll
 */
Container.prototype.removeAll = function removeAll()
{
	log.trace("For container " + this.id + ": Removing all");
	this.loadIfNeeded();
	for (var iName=0, numNames=this.names.length; iName<numNames; iName++) 
	{
		var name = this.names[iName];
		if (this.isPreviouslyPersisted(name)) // otherwise we'll need this info to remove the name-value from the persistent store
		{
			this.deletedData[name] = this.data[name];
		}
	}
	this.names = [];
	this.data = {};
	this.changedData = {};
}

/**
 * Revert a property to its previously-persisted value
 * 
 * @private
 * @alias Jaxer.Container.prototype.revert
 * @param {String} name
 * 		The name of the property to revert
 */
Container.prototype.revert = function revert(name)
{
	log.trace("For container " + this.id + ": Reverting " + name);
	this.loadIfNeeded();
	var nowExists = this.data.hasOwnProperty(name);
	var newlyAdded = this.addedData.hasOwnProperty(name);
	var wasDeleted = this.deletedData.hasOwnProperty(name);
	var wasChanged = this.changedData.hasOwnProperty(name);
	if (newlyAdded)
	{
		var iName = this.names.indexOf(name);
		delete this.names[iName];
		delete this.data[name];
	}
	else if (wasDeleted)
	{
		this.data[name] = this.deletedData[name];
	}
	else if (wasChanged)
	{
		this.data[name] = this.changedData[name];
	}
	delete this.addedData[name];
	delete this.changedData[name];
	delete this.deletedData[name];
}

/**
 * Persists a container.
 * 
 * @private
 * @alias Jaxer.Container.prototype.persist
 * @param {Object} doc
 * 		The document into which container information may be persisted (currently not used)
 */
Container.prototype.persist = function persist(doc)
{
	log.trace("For container " + this.id + ": Persisting");
	var exposedContainer = exposedContainers[this.name];
	var dataIsSerialized = false;
	var that = this;
	if (exposedContainer) // the exposedContainer has been used, so harvest its data
	{
		dataIsSerialized = true; // we'll end up serializing all the data
		if (Util.isEmptyObject(exposedContainer)) 
		{
			this.removeAll(); // optimization
		}
		else 
		{
			// delete any properties that are no longer in exposedContainer
			var namesToDelete = []
			this.names.forEach(function(name)
			{
				if (!(name in exposedContainer)) 
					namesToDelete.push(name);
			});
			namesToDelete.forEach(function(name)
			{
				that.remove(name);
			});
			// then deal with all data that is in exposedContainer
			var namesToUnchange = [];
			for (var p in exposedContainer)
			{
				if (typeof exposedContainer[p] == "undefined") exposedContainer[p] = Container.DEFAULT_VALUE;
				if (p in this.addedData) // data was newly added in container and was never persisted -- just overwrite it
				{
					this.data[p] = Serialization.toJSONString(exposedContainer[p], { as: Serialization.JAXER_METHOD });
				}
				else if (p in this.changedData) // data was changed in container -- overwrite it unless it was set back to the original
				{
					var original = Serialization.toJSONString(this.changedData[p], { as: Serialization.JAXER_METHOD });
					var latest = Serialization.toJSONString(exposedContainer[p], { as: Serialization.JAXER_METHOD });
					if (original == latest) // set back to original
					{
						namesToUnchange.push(p);
					}
					else // yes, it's changed -- to the latest value, in container
					{
						this.data[p] = latest;
					}
				}
				else if (p in this.data) // data was not changed in container, but maybe was changed in exposedContainer
				{
					var original = Serialization.toJSONString(this.data[p], { as: Serialization.JAXER_METHOD });
					var latest = Serialization.toJSONString(exposedContainer[p], { as: Serialization.JAXER_METHOD });
					if (original != latest) // set back to original
					{
						this.set(p, latest);
					}
				}
				else // add any data that is new
				{
					this.set(p, Serialization.toJSONString(exposedContainer[p], { as: Serialization.JAXER_METHOD }));
				}
			}
			namesToUnchange.forEach(function(name)
			{
				delete that.changedData[name];
			});
		}
	}
	var didSomething = false;
	if ((this.names.length == 0) &&
		!Util.isEmptyObject(this.deletedData)) // optimization to bulk-delete
	{
		this.persistor.removeAll(this.type, this.key);
		didSomething = true;
	}
	else
	{
		if (this.persistor.persistByName)
		{
			for (var name in this.addedData)
			{
				log.debug("For container " + this.id + ": inserting " + name + " into persistent store");
				this.persistor.persist(this.type, this.key, name, dataIsSerialized ? this.data[name] : Serialization.toJSONString(this.data[name], { as: Serialization.JAXER_METHOD }));
				didSomething = true;
			}
			for (var name in this.changedData)
			{
				log.debug("For container " + this.id + ": updating " + name + " in persistent store");
				this.persistor.persist(this.type, this.key, name, dataIsSerialized ? this.data[name] : Serialization.toJSONString(this.data[name], { as: Serialization.JAXER_METHOD }));
				didSomething = true;
			}
			for (var name in this.deletedData)
			{
				log.debug("For container " + this.id + ": removing " + name + " from persistent store");
				this.persistor.remove(this.type, this.key, name);
				didSomething = true;
			}
		}
		else
		{
			log.debug("For container " + this.id + ": persisting all data");
			var serialized;
			if (dataIsSerialized) 
			{
				serialized = this.data;
			}
			else 
			{
				serialized = {};
				for (var name in this.data) 
				{
					serialized[name] = Serialization.toJSONString(this.data[name], { as: Serialization.JAXER_METHOD });
				}
			}
			this.persistor.persist(this.type, this.key, serialized);
			didSomething = true;
		}
	}
	if (didSomething) 
	{
		this.addedData = {};
		this.changedData = {};
		this.deletedData = {};
		if (Jaxer.response)
		{
			Jaxer.response.noteSideEffect();
		}
	}
}

// Static methods

var containerTypes = ["APPLICATION", "PAGE", "SESSION", "SESSION_PAGE"];
var jaxerContainerNames = [];

for (var i = 0; i < containerTypes.length; i++)
{
	var containerType = containerTypes[i];
	
	Container[containerType] = containerType;
	containerName = Util.String.upperCaseToCamelCase(containerType);
	jaxerContainerNames.push(containerName);
}

var containers = {}; // these are instances of Container;
var exposedContainers = {}; // these are plain objects exposed to the developer off the Jaxer namespace

/**
 * Initialize the Containers subsystem for the current request
 * 
 * @private
 * @alias  Jaxer.Container.init
 * @param {String} appKey
 * 		The string identifying the application associated with the current request
 * @param {String} pageKey
 * 		The string identifying the page associated with the current request
 */
Container.init = function init(appKey, pageKey)
{
	var dbPersistor = new Container.DBPersistor();
	var keys = {};
	keys.application = appKey;
	keys.page = pageKey;
	keys.session = SessionManager.keyFromRequest(keys.application);
	keys.sessionPage = keys.session + "$$" + keys.page;
	for (var i=0; i<containerTypes.length; i++)
	{
		var containerType = containerTypes[i];
		var containerName = jaxerContainerNames[i];
		var container = new Container(containerType, keys[containerName], dbPersistor, containerName);
		containers[container.type] = container;
		exposedContainers[container.name] = null;
		Jaxer.__defineGetter__(containerName, createGetter(container.type, container.name));
		Jaxer.__defineSetter__(containerName, createSetter(container.type, container.name));
		log.debug("Created " + container.type + " container as Jaxer." + container.name + " with key " + container.key);
	}
	Jaxer.clientData = {};
};

function createGetter(containerType, containerName)
{
	var func = function getContainer()
	{
		log.trace("Getting exposedContainer: " + containerName);
		var exposedContainer = exposedContainers[containerName];
		if (!exposedContainer) 
		{
			exposedContainer = {};
			var _container = containers[containerType];
			_container.loadIfNeeded();
			log.trace("Populating Jaxer." + containerName + " with properties: " + _container.names);
			_container.names.forEach(function(name)
			{
				exposedContainer[name] = _container.get(name);
			});
			exposedContainers[containerName] = exposedContainer;
		}
		return exposedContainer;
	};
	return func;
}

function createSetter(containerType, containerName)
{
	var func = function setContainer(value)
	{
		log.trace("Setting exposedContainer: " + containerName);
		if (typeof value == "object")
		{
			var exposedContainer = {};
			for (var p in value)
			{
				exposedContainer[p] = value[p];
			}
			exposedContainers[containerName] = exposedContainer;
		}
		else
		{
			throw "You can only set Jaxer." + containerName + " to a value of type 'object'";
		}
	};
	return func;
}

/**
 * A persistent session-like container that can store and persist name-value 
 * pairs in the context of an entire application. Whether a given page belongs
 * to a given application is determined by configApps.js, usually from the
 * page's URL. All pages belonging to an application, and their callbacks, have
 * read/write access to this application container, regardless of user session,
 * and to no other application-level container.
 * 
 * @alias Jaxer.application
 * @private
 * @property {Object}
 */

/**
 * A persistent session-like container that can store and persist name-value
 * pairs in the context of one page. What constitutes a page is defined by
 * configApps.js, usually from the page's URL (i.e. it determines which URLs
 * constitute unique pages). A page and its callbacks have read/write access to
 * this page container regardless of user session, and to no other page-level
 * container.
 * 
 * @alias Jaxer.page
 * @private
 * @property {Object}
 */

/**
 * A persistent session container that can store and persist name-value pairs in
 * the context of a user/browser session across all pages of an application. 
 * What constitutes an application is defined by configApps.js, usually from
 * the request's URL. A page and its callbacks have read/write access to this
 * page container regardless of user session, and to no other page-level
 * container.
 * 
 * @alias Jaxer.session
 * @private
 * @property {Object}
 */

/**
 * A persistent session-like container that can store and persist name-value
 * pairs in the context of a user/browser session on a given page. What
 * constitutes a page is defined by configApps.js, usually from the request's
 * URL. A page and its callbacks have read/write access to this sessionPage
 * container as long as the session continues, and to no other sessionPage
 * containers.
 * 
 * @alias Jaxer.sessionPage
 * @private
 * @property {Object}
 */

/**
 * A JavaScript Object that can be used to communicate data from the server at
 * the end of server-side page processing to the client. When Jaxer starts to
 * process a page server-side, Jaxer.clientData is an empty object: {}. If you
 * set any properties on this object, the entire object will be JSON-serialized
 * at the end of server-side processing, and will be automatically de-serialized
 * when it gets to the client, so you can access your data as Jaxer.clientData
 * in the browser. Note that if there is no data, Jaxer.clientData will not be
 * created at all on the client.
 * 
 * @alias Jaxer.clientData
 * @private
 * @property {Object}
 */

/**
 * Persists all container data to the store (as needed).
 * 
 * @private
 * @alias Jaxer.Container.persistAll
 * @param {Object} doc
 * 		The current document, if any, into which the clientData container's data
 * 		will be inserted. Not applicable for callbacks.
 */
Container.persistAll = function persistAll(doc)
{
	// Persist session key to client
	SessionManager.keyToResponse(containers[Container.APPLICATION].key, containers[Container.SESSION].key);
	// Persist all containers to the DB and then clear them to make sure nobody has access to their data
	for (var i=0; i<containerTypes.length; i++)
	{
		var containerType = containerTypes[i];
		var containerName = jaxerContainerNames[i];
		containers[containerType].persist();
		delete containers[containerType];
		delete exposedContainers[containerName];
		delete Jaxer[containerName];
	}
	// Persist clientData to client and then clear it
	var hasClientData = !Util.isEmptyObject(Jaxer.clientData);
	if (hasClientData && doc)
	{
		Jaxer.response.noteSideEffect();
		Jaxer.response.noteDomTouched();
		var dataString = Serialization.toJSONString(Jaxer.clientData, { as: Serialization.JAXER_METHOD });
		var js = "Jaxer.clientData = Jaxer.Serialization.fromJSONString('" + Util.String.escapeForJS(dataString) + "', {as: Jaxer.Serialization.JAXER_METHOD});";
		var head = doc.getElementsByTagName("head")[0];
		Util.DOM.insertScriptAtBeginning(js, head, null);
	}
	delete Jaxer.clientData;
};

Container.DEFAULT_VALUE = true; // The default value for name-value pairs

frameworkGlobal.Container = Jaxer.Container = Container;

Log.trace("*** Container.js loaded");

})();
