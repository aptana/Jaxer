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

// NOTE! This is a server- and client-side, *compressible* module -- be sure to
// end each assignment of function literals with a semicolon

/*
 * Based on original code from Douglas Crockford's json.js, 2007-04-30
 */

(function(){

/**
 * @namespace {Jaxer.Serialization}
 * 
 * This is the namespace that contains Jaxer serialization methods.
 * 
 * The Jaxer serializer uses the familiar and popular JSON format. However,
 * additional functionality has been provided to allow for serialization of more
 * complex data structures. Specifically, this module supports cyclical data
 * structures, multiple references, and custom typeserializers. Each of these is
 * described below.
 * 
 * Cyclical data structures occur when an object (or array) contains a
 * descendent structure that also references that same object. For example, a
 * DOM node has references to its children and these children also have
 * references to the DOM node (their parentNode). In a traditional JSON
 * environment, if you were to try to serialize this structure, you would end up
 * in an infinite loop or an exception would occur as the serializer traversed
 * the parent node, its child nodes, and then back up to the parent node through
 * the child's parentNode property. Indeed, the serializer couldn't get past the
 * first child in this scenario. The Jaxer serializer bypasses this via the use
 * of marker properties and specially formatted strings referred to as
 * "references".
 * 
 * Multiple references are similar to cyclical data structures in that an object
 * is referenced two or more times. However, this does not necessarily create a
 * cycle. For example, say you have the following code:
 * 
 * 		<pre>var car = {
 * 			color: "blue",
 * 			price: 10000
 * 		};
 * 		var cars = [car, car];</pre>
 * 
 * As you can see, the same car object has been referenced twice in the array.
 * In a traditional JSON serializer, each instance of car would be serialized
 * separately. Unfortunately, that alters the data structure that will be
 * accessed after deserialization in a subtle way. You will end up with two
 * independent car objects which means that changing the price of one will not
 * change the price of the other as would have happened before the
 * serialization/deserialization cycle. In order to restore the same references,
 * Jaxer serializes the car only once and then leaves placeholders to point to
 * that single instance. During deserialization, the placeholders are replaced
 * with actual references to the deserialized object, thus restoring the
 * original data structure as it appeared before serialization.
 * 
 * Some data types cannot be expressed in JSON. For example, the Date type is
 * not listed as a valid type in JSON. So, in order to support this type and
 * potentially many others, the serializer allows the developer to register
 * custom serializers and associated deserializers for a given type. When the
 * serializer sees these types, the custom handlers are used to convert the item
 * to a string. It is then the responsibility of the custom deserializer to
 * restore the string to the original type. For example, Jaxer supports
 * XMLDocuments. The custom serializer creates an XML string which is specially
 * tagged so the deserializer can restore the XML string back to an XMLDocument.
 * 
 * Next, we briefly discuss how Jaxer recognizes cycles, multi-references, and
 * how it represents references and custom serialized objects.
 * 
 * The Jaxer serializer makes an initial pass over the data being serialized.
 * Each object, array, and custom serialization object is tagged with a unique
 * index. (Note that some objects do not allow properties to be added to them.
 * In this situation, the Jaxer serializer maintains an array of these items.
 * This array is searched when new items are encountered and serves the same
 * purpose as the id property). Before adding the index, we first check if we
 * have already indexed the item. If the tag already exists, then we've either
 * exposed a cycle or a multi-reference. At this point, the serializer knows to
 * switch to another JSON format that minimizes the amount of data to be
 * serialized.
 * 
 * References and custom serialization objects each make use of specially
 * formatted strings. To make this a bit clearer, we create an array of two
 * references to the same date object.
 * 
 * 		<pre>var d = new Date();
 * 		var items = [d, d];
 * 		var json = Jaxer.Serialization.toJSONString(items);</pre>
 * 
 * The resulting JSON string will look like the following:
 * 
 * 		<pre>[["~1~","~1~"], "~Date:2007-08-17T11:57:30~"]</pre>
 * 
 * This format always has a top-level array whose first element is the root
 * item that was originally being serialized. In this case, our top-most element
 * was an array. As an aside, the only top-level elements that can generate this
 * format are arrays, objects, and custom serialization objects. The first
 * special format  used for references and is defined with "~#~" where # is a
 * number. The number is the index into the top-level array. The element at that
 * index is the item that needs to be referenced where the reference string
 * lives. In this example, once deserialization has completed, both instances of
 * "~1~" will have been replaced with references to the deserialized date
 * object.
 * 
 * The next custom format, the date, shows how custom serializers emit text. The
 * first item after the ~ but before the : is the name of the type. This is the
 * fully-qualified type as you would have to type it in JavaScript to get to
 * that type's constructor. The string after the : is in a format as generated
 * by the type's custom serializer. The resulting string generated by the custom
 * serializer is in turn serialized as a string, so the deserializer does not
 * need to handle special characters or escape sequences. It is the
 * responsibility of the custom deserializer to consume that text and to return
 * effectively a clone of the original object.
 * 
 * This module also allows a developer to register alternate top-level
 * serialization and deserialization methods. The default method for
 * serialization is 'nativeJSON' which attempts to use the built-in JSON support
 * in the user agent, when available. In cases where 'nativeJSON' is not
 * supported, the 'JSON' mode will be used. The developer can also use 'JSON'
 * along with more options to customize serialization for special values like
 * 'undefined', and 'INFINITY', for example. Finally, there is a 'Jaxer' mode as
 * described above. This mode is used by Jaxer's framework and callback
 * mechanisms and is available to developers that may need this advanced
 * functionality. These serialization methods are specificed in a separate
 * optional parameter to the "toJSONString" and 'fromJSONString" functions. Note
 * that if the developer uses a non-default serialization method, then the
 * developer is also responsible for using this same method for deserialization.
 * Currenty, this implementation cannot detect which method was used for the
 * original serialization step
 * 
 * @see Jaxer.Serialization.toJSONString
 * @see Jaxer.Serialization.fromJSONString
 */

// create Serialization container
var Serialization = {};

// action enumeration
Serialization.SERIALIZE_ACTION = "serialize";
Serialization.THROW_ACTION = "throw";
Serialization.TRUNCATE_ACTION = "truncate";
Serialization.NULLIFY_ACTION = "nullify";
Serialization.RETURN_OBJECT_ACTION = "return object";

// message emitted by the truncate action
var TRUNCATION_MESSAGE = "__truncated__";

// public serialization option properties and default settings
var DEFAULT_MAX_DEPTH = 10;
var MAX_DEPTH_PROPERTY = "maxDepth";
var MAX_DEPTH_ACTION_PROPERTY = "maxDepthAction";
var DATE_SERIALIZATION_ACTION_PROPERTY = "dateSerializationAction";
var SPECIAL_NUMBER_SERIALIZATION_ACTION_PROPERTY = "specialNumberSerializationAction";
var UNDEFINED_SERIALIZATION_ACTION_PROPERTY = "undefinedSerializationAction";
var USE_CUSTOM_SERIALIZERS_PROPERTY = "useCustomSerializers";

// private property names used internally during serialization
var ID_PROPERTY = "$id";
var ITEMS_PROPERTY = "$items";

// our supported serializer method names
Serialization.JAXER_METHOD = "Jaxer";
Serialization.JSON_METHOD = "JSON";
Serialization.NATIVE_JSON_METHOD = "nativeJSON";

var JSON_SYNTAX_ERROR_NAME = "JSONSyntaxError";
var JSON_EVAL_ERROR_NAME = "JSONEvalError";

// default serialization result
var NO_RESULT = "undefined";

// patterns used to query text patterns for special string values
var VALID_TYPE_PATTERN = /^[a-zA-Z_$](?:[-a-zA-Z0-9_$]*)(?:\.[a-zA-Z_$](?:[-a-zA-Z0-9_$]*))*$/;
var DATE_PATTERN = /^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})$/;

// custom type handler, serializer, and deserializer hashes
var typeHandlers = {};
var serializers = {};
var deserializers = {};

/*
 * BEGIN: Private functions section
 */

/**
 * Convert an Array to a JSON string
 * 
 * @private
 * @param {Array} ary
 * 		The source Array to be serialized
 * @param {Object} options
 * 		This is the options object passed into the top-level toJSONString
 * 		function.
 * @return {String}
 * 		The resulting JSON string
 */
function ArrayToJSON(ary, options)
{
	// clone options object so as not to alter the original
	options = Jaxer.Util.protectedClone(options);
	
	// decrease recursion depth counter
	options[MAX_DEPTH_PROPERTY]--;
	
	// perform max-depth action if we've reached our maximum recursion depth
	if (options[MAX_DEPTH_PROPERTY] < 0)
	{
		var action = options[MAX_DEPTH_ACTION_PROPERTY] || Serialization.THROW_ACTION;
		
		switch (action)
		{
			case Serialization.TRUNCATE_ACTION:
				return '"' + TRUNCATION_MESSAGE + '"';
				
			case Serialization.THROW_ACTION:
				throw new Error("Maximum recursion depth has been exceeded");
				break;
		}
	}
	
	var result = [];
	var length = ary.length;
	
	// For each value in this array...
	for (var i = 0; i < length; i++)
	{
		var item = ary[i];
		
		// Note that we ignore elements that are not serializeable
		if (isSerializeable(item)) 
		{
			result.push(toCrockfordJSONString(item, options));
		}
	}

	// Join all of the fragments together and return.
	return "[" + result.join(",") + "]";
}

/**
 * Clear the lookup table used to match a type with its constructor. This needs
 * to be performed before serialization and deserialization since the global
 * context changes depending on when in the page life cycle serialization is
 * being performed.
 * 
 * @private
 */
function clearHandlerCache()
{
	for (var name in typeHandlers) 
	{
		typeHandlers[name].constructor = null;
	}
}

/**
 * Convert a date to a our special string format for later deserizliation
 * 
 * @private
 * @param {Date} data
 * 		The source Date to be serialized
 * @return {String}
 * 		The resulting JSON string
 */
function DateToJSON(data)
{
	// Format integers to have at least two digits.
	function pad(n)
	{
		return n < 10 ? '0' + n : n;
	}

	// Ultimately, this method will be equivalent to the date.toISOString
	// method.
	return '"' +
		data.getFullYear() + '-' +
		pad(data.getUTCMonth() + 1) + '-' +
		pad(data.getUTCDate()) + 'T' +
		pad(data.getUTCHours()) + ':' +
		pad(data.getUTCMinutes()) + ':' +
		pad(data.getUTCSeconds()) + '"';
}

/**
 * Traverse the resulting JSON object to perform any post-processing needed
 * to convert references and custom serialization objects to their proper
 * instances.
 * 
 * @private
 * @param {String} property
 * 		The name of the propery to visit
 * @param {Object} obj
 * 		The object whose property will be visited
 * @param {Function} filter
 * 		The function to apply to each element in the data graph
 * @return {Object}
 * 		The resulting filter property value
 */
function walk(property, obj, filter)
{
	if (obj && typeof obj === 'object')
	{
		for (var p in obj)
		{
			if (obj.hasOwnProperty(p))
			{
				obj[p] = walk(p, obj[p], filter);
			}
		}
	}
	
	return filter(property, obj);
}

/**
 * This is a shared function used by both the "jaxer" and the "json"
 * serialization methods. The options object is used to determine if special
 * numbers should be allowed in the source json string
 * 
 * @private
 * @param {Object} json
 * @param {Object} options
 * @return {Object}
 */
function evalJSONString(json, options)
{
	var result = NO_RESULT;
	var simpleValuePattern =
		(options[SPECIAL_NUMBER_SERIALIZATION_ACTION_PROPERTY] === Serialization.SERIALIZE_ACTION)
			?	/"[^"\\\n\r]*"|true|false|null|undefined|NaN|[-+]?Infinity|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g
			:	/"[^"\\\n\r]*"|true|false|null|undefined|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g;
	
	// Run the text against a regular expression to look for non-JSON
	// characters. We are especially concerned with '()' and 'new' because they
	// can cause invocation, and '=' because it can cause mutation. But just to
	// be safe, we will reject all unexpected characters.

	// if (/^("(\\.|[^"\\\n\r])*?"|[,:{}\[\]0-9.\-+EINaefilnr-uy \n\r\t])+?$/.test(json))
	
	// We split the second stage into 4 regexp operations in order to work around
	// crippling inefficiencies in IE's and Safari's regexp engines. First we
	// replace all backslash pairs with '@' (a non-JSON character). Second, we
	// replace all simple value tokens with ']' characters. Third, we delete all
	// open brackets that follow a colon or comma or that begin the text. Finally,
	// we look to see that the remaining characters are only whitespace or ']' or
	// ',' or ':' or '{' or '}'. If that is so, then the text is safe for eval.
    if (
		/^[\],:{}\s]*$/.
			test
			(
				json.replace(/\\["\\\/bfnrtu]/g, '@').
					replace(simpleValuePattern, ']').
					replace(/(?:^|:|,)(?:\s*\[)+/g, '')
			)
	)
	{
		// We use the eval function to compile the text into a JavaScript
		// structure. The '{' operator is subject to a syntactic ambiguity in
		// JavaScript: it can begin a block or an object literal. We wrap the
		// text in parens to eliminate the ambiguity.

		try
		{
			result = eval('(' + json + ')');
		}
		catch (e)
		{
			var err = new Error("parseJSON: exception '" + e + "' when evaluating: " + json);
			err.name = JSON_EVAL_ERROR_NAME;
			throw err;
		}
	}
	else
	{
		var err = new Error("parseJSON: unexpected characters in: " + json);
		err.name = JSON_SYNTAX_ERROR_NAME;
		throw err;
	}
	
	return result;
}

/**
 * Find the type handler name for the specified object's type. If no handler
 * exists, then this function will return null.
 * 
 * @private
 * @param {Object} item
 * 		The item for which a custom serialization handler name is being queried.
 * @return {String}
 * 		If the item's type is not registered with a custom serialization
 * 		handler, then this function will return null; otherwise, the fully-
 * 		qualified type name will be returned. This type name is also the name
 * 		of the handler.
 */
function findHandlerName(item)
{
	var result = null;
	
	for (var name in typeHandlers)
	{
		if (typeHandlers[name].canSerialize(item))
		{
			result = name;
			break;
		}
	}
	
	return result;
}

/**
 * fromCrockfordJSONString
 * 
 * @private
 * @param {String} json
 * @param {Object} options
 * @return {Object}
 */
function fromCrockfordJSONString(json, options)
{
	/**
	 * For JSON strings that do not contain references, we make a
	 * post-processing step to replace all custom serialization string with
	 * their deserialized instances.
	 * 
	 * @private
	 * @param {String} property
	 * 		The property name to filter
	 * @param {Object} value
	 * 		The value of the property being filtered
	 */
	function filter(property, value)
	{
		var result = value;
		
		if (typeof value === "string")
		{
			var match;
			
			if (match = value.match(DATE_PATTERN))
			{
				var win = getWindow();
				
				result = new win.Date(Date.UTC(match[1], match[2] - 1, match[3], match[4], match[5], match[6]));
			}
		}
		
		return result;
	}
	
	var result = evalJSONString(json, options);
	
	if (result)
	{
		result = walk('', result, filter);
	}
	
	return result;
}

/**
 * fromJaxerJSONString
 * 
 * @private
 * @param {String} json
 * @param {Object} options
 * @return {Object}
 */
function fromJaxerJSONString(json, options)
{
	var REFERENCE_PATTERN = /^~(\d+)~$/;
	var REFERENCE_STRING = /('~\d+~'|"~\d+~")/;
	var CUSTOM_SERIALIZATION_PATTERN = /^~([a-zA-Z_$](?:[-a-zA-Z0-9_$]*)(?:\.[a-zA-Z_$](?:[-a-zA-Z0-9_$]*))*):([\s\S]+)~$/; // Use \s\S to match newlines too
	
	/**
	 * A reference constitutes an object and a property on the object. This
	 * class is used to specify a specific property on an object for later
	 * setting of that value.
	 *
	 * @private
	 * @constructor
	 * @param {Object} object
	 * 		The source object of this reference
	 * @param {String} property
	 * 		the property on the object representing this reference value
	 * @param {Number} index
	 * 		The reference ID that uniquely identifies this reference 
	 */
	function Reference(object, property, index)
	{
		this.object = object;
		this.property = property;
		this.index = index;
	}
	
	/**
	 * Walks the list of nodes passed in the method and sets all properties
	 * on this instance's underlying object to the values in the node list
	 *
	 * @private
	 * @param {Array} nodes
	 * 		A list of all nodes in the data graph. This array is used to
	 * 		extract the value of this reference via this reference's unique id.
	 */
	Reference.prototype.setValue = function(nodes)
	{
		var result = false;
		
		if (0 <= this.index && this.index < nodes.length)
		{
			this.object[this.property] = nodes[this.index];
			result = true;
		}
		
		return result;
	};
	
	/**
	 * This post-processing step replaces all reference strings with the actual
	 * object reference to which they refer.
	 * 
	 * @private
	 * @param {Array} input
	 * 		The source array created by the first step of eval'ing the JSON
	 * 		source string.
	 * @return {Object}
	 * 		The resulting object created by dereferencing all reference values
	 * 		and rewiring of the object graph
	 */
	function postProcess(input)
	{
		var result = input;
		
		if (input.length > 0)
		{
			var valid = true;
			
			inputLoop:
			for (var i = 0; i < input.length; i++)
			{
				var item = input[i];
				
				if (item === null || item === undefined)
				{
					valid = false;
					break;
				}
				
				var type = item.constructor;
				var itemGlobal = getWindow(item);
				
				// add any references
				switch (type)
				{
					case itemGlobal.Array:
						postProcessArray(item);
						break;
						
					case itemGlobal.Object:
						postProcessObject(item);
						break;

                    case itemGlobal.String:
                        postProcessMember(input, i);
                        break;
						
					default:
						valid = false;
						break inputLoop;
				}
			}

			if (valid)
			{
				if (references.length > 0)
				{
					result = input[0];
					
					for (var i = 0; i < references.length; i++)
					{
						var success = references[i].setValue(input);
						
						if (success === false)
						{
							result = input;
							break;
						}
					}
				}
			}
		}
		
		return result;
	}
	
	/**
	 * This post-processing step replaces all object references that are members
	 * of the specified array with actual references to the object to which they
	 * refer
	 * 
	 * @private
	 * @param {Array} ary
	 * 		The source array to process
	 * @return {Boolean}
	 * 		Returns true if the specified array was a valid reference array
	 */
	function postProcessArray(ary)
	{
		var result = true;
		
		for (var i = 0; i < ary.length; i++)
		{
			if (postProcessMember(ary, i) === false)
			{
				result = false;
				break;
			}
		}
		
		return result;
	}
	
	/**
	 * This post-processing step replaces all object references that are members
	 * of the specified object with actual references to the object to which
	 * they refer
	 * 
	 * @private
	 * @param {Object} obj
	 * 		The source object to process
	 * @param {Array} references
	 * 		An array of reference instances
	 * @return {Boolean}
	 * 		Returns true if the specified object was a valid reference object
	 */
	function postProcessObject(obj, references)
	{
		var result = true;
		
		for (var p in obj)
		{
			if (postProcessMember(obj, p) === false)
			{
				result = false;
				break;
			}
		}
		
		return result;
	}
	
	/**
	 * This post-processing steps replaces all reference strings with the actual
	 * object reference to which they refer. Also, custom deserializers are
	 * invoked for any matching custom serializer strings that are encountered.
	 * 
	 * @private
	 * @param {Object} obj
	 * 		The object to post-process
	 * @param {String|Number} property
	 * 		The name or index of the object to process.
	 * @return {Boolean}
	 * 		Returns true if the obj[property] value is a valid reference object
	 */
	function postProcessMember(obj, property)
	{
		var item = obj[property];
		var result = true;
		
		if (item !== null && item !== undefined)
		{
			var type = item.constructor;
			var itemGlobal = getWindow(item);
			
			switch (type)
			{
				case itemGlobal.Array:
					// we only allow empty arrays
					if (item.length > 0)
					{
						result = false;
					}
					break;

				case itemGlobal.Object:
					// we only allow empty objects
					for (var p in item)
					{
						result = false;
						break;
					}
					break;
											
				case itemGlobal.String:
					var match;
					
					if (match = item.match(REFERENCE_PATTERN))
					{
						var index = match[1] - 0;
						var ref = new Reference(obj, property, index);
						
						references.push(ref);
					}
					else if (match = item.match(CUSTOM_SERIALIZATION_PATTERN))
					{
						var name = match[1];
						var serializedString = match[2];
						var handler = typeHandlers[name];
						
						if (handler && handler.canDeserialize && handler.canDeserialize(serializedString))
						{
							obj[property] = handler.deserializer(serializedString);
						}
					}
					break;
			}
		}
		
		return result;
	}
	
	/**
	 * For JSON strings that do not contain references, we make a
	 * post-processing step to replace all custom serialization string with
	 * their deserialized instances.
	 * 
	 * @private
	 * @param {String} property
	 * 		The property name to filter
	 * @param {Object} value
	 * 		The value of the property being filtered
	 */
	function filter(property, value)
	{
		var result = value;
		
		if (typeof value === "string")
		{
			var match;
			
			if (match = value.match(CUSTOM_SERIALIZATION_PATTERN))
			{
				var name = match[1];
				var serializedString = match[2];
				var handler = typeHandlers[name];
				
				if (handler && handler.canDeserialize && handler.canDeserialize(serializedString))
				{
					result = handler.deserializer(serializedString);
				}
			}
		}
		
		return result;
	}
	
	var result = evalJSONString(json, options);
	var references = [];
	
	if (result)
	{
		var itemGlobal = getWindow(result);
		
		// if the result is an array, it may be in our format to support
		// cycles and multi-references
		if (result.constructor === itemGlobal.Array)
		{
			// It is an array, so next test for reference strings
			if (REFERENCE_STRING.test(json))
			{
				// found one, so process references
				result = postProcess(result);
			}
			else
			{
				// no references, so process custom serialization strings only
				result = walk('', result, filter);
			}
		}
		else
		{
			// not a "references" structure, so process custom serialization
			// strings only
			result = walk('', result, filter);
		}
	}
	
	return result;
}

/**
 * Find the window object that created the specified object. This returns the
 * correct global context when performing comparisons against the object's
 * constructor.
 * 
 * @private
 * @param {Object} object
 * 		The object from which determine the global context
 * @return {Object}
 * 		Returns the object's owning window
 */
function getWindow(object)
{
	var globalContext;

	var hasParent =
			object !== null
		&&	typeof(object) !== "undefined"
		&&	object.__parent__ !== null
		&&	typeof(object.__parent__) !== "undefined";
	
	if (hasParent)
	{
		var current = object;
		var parent = object.__parent__;
		while (parent && parent !== current)
		{
			current = parent;
			parent = parent.__parent__;
		}
		if (current != object)
		{
			globalContext = current;
		}
	}
	
	if (!globalContext) 
	{
		if (Jaxer.isOnServer) 
		{
			globalContext = Jaxer.pageWindow || Jaxer.frameworkGlobal;
		}
		else 
		{
			globalContext = window;
		}
	}
	
	return globalContext;
}

/**
 * Checks whether the given argument is JSON-serializable (i.e. JSON-
 * representible) or not (e.g. functions are not).
 * 
 * @private
 * @param {Object} obj
 * 		The object to test, which can be of any type or even undefined
 * @return {Boolean}
 * 		true if representable in JSON, false otherwise.
 */
function isSerializeable(obj)
{
	var result = false;
	
	if (obj === null || obj === undefined) 
	{
		result = true;
	}
	else 
	{
		switch (typeof obj)
		{
			case "string":
			case "number":
			case "boolean":
			case "object": // also includes Dates and Arrays
				result = true;
				break;
				
			case "function": // only RegExp "functions" are serializable
				result = (obj.constructor === getWindow(obj).RegExp);
				break;
		}
	}
	
	return result;
}

/**
 * Convert an object to a JSON string
 * 
 * @private
 * @param {Object} data
 * 		The source object to be serialized
 * @param {Object} options
 * 		This is the options object passed into the top-level toJSONString
 * 		function.
 * @return {String}
 * 		The resulting JSON string
 */
function ObjectToJSON(data, options)
{
	// clone options object so as not to alter the original
	options = Jaxer.Util.protectedClone(options);
	
	// decrease recursion depth counter
	options[MAX_DEPTH_PROPERTY]--;
	
	// perform max-depth action if we've reached our maximum recursion depth
	if (options[MAX_DEPTH_PROPERTY] < 0)
	{
		var action = options[MAX_DEPTH_ACTION_PROPERTY] || Serialization.THROW_ACTION;
		
		switch (action)
		{
			case Serialization.TRUNCATE_ACTION:
				return '"' + TRUNCATION_MESSAGE + '"';
				
			case Serialization.THROW_ACTION:
				throw new Error("Maximum recursion depth has been exceeded");
				break;
		}
	}
	
	var result = [];

	// Iterate through all of the keys in the object, ignoring the proto chain.
	for (var k in data)
	{
		var p = '"' + k + '":';
		var v = data[k];
		
		// Note that we ignore elements that are not serializeable
		if (isSerializeable(v)) 
		{
			result.push(p + toCrockfordJSONString(v, options));
		}
	}

	// Join all of the fragments together and return.
	return "{" + result.join(',') + "}";
}

/**
 * Convert a string to a JSON string
 * 
 * @private
 * @param {Object} data
 * 		The source string to be serialized
 * @param {Object} options
 * 		This is the options object passed into the top-level toJSONString
 * 		function.
 * @return {String}
 * 		The resulting JSON string
 */
function StringToJSON(data, options)
{
	// m is a table of character substitutions.
	var characterMap = {
		'\b': '\\b',
		'\t': '\\t',
		'\n': '\\n',
		'\f': '\\f',
		'\r': '\\r',
		'"' : '\\"',
		'\\': '\\\\'
	};
	
	// If the string contains no control characters, no quote characters,
	// and no backslash characters, then we can simply slap some quotes
	// around it. Otherwise we must also replace the offending characters
	// with safe sequences.

	if (/["\\\x00-\x1f]/.test(data))
	{
		return '"' + data.replace(
			/([\x00-\x1f\\"])/g,
			function (a, b)
			{
				var c = characterMap[b];
				
				if (c)
				{
					return c;
				}
				
				c = b.charCodeAt();
				
				return '\\u00' + Math.floor(c / 16).toString(16) + (c % 16).toString(16);
			}
		) + '"';
	}
	
	return '"' + data + '"';
}

/**
 * Convert the specified object into a JSON string
 * 
 * @private
 * @param {Object} data
 * 		The Javascript value to be serialized
 * @param {Object} options
 * 		The options object
 * @return {String}
 * 		The resulting JSON string
 */
function toCrockfordJSONString(data, options)
{
	var result = NO_RESULT;
	
	if (isSerializeable(data)) 
	{
		if (data === null) 
		{
			result = "null";
		}
		else if (data === undefined) 
		{
			var action = options[UNDEFINED_SERIALIZATION_ACTION_PROPERTY] || Serialization.THROW_ACTION;
			
			switch (action)
			{
				case Serialization.SERIALIZE_ACTION:
					result = "undefined";
					break;
					
				case Serialization.NULLIFY_ACTION:
					result = "null";
					break;
					
				case Serialization.THROW_ACTION:
				default:
					throw new Error("Serialization of 'undefined' is not supported unless the undefinedSerializationAction option is set to 'serialize'");
			}
		}
		else 
		{
			var ctor = data.constructor;
			var dataGlobal = getWindow(data);
			
			switch (ctor)
			{
				case dataGlobal.Array:
					result = ArrayToJSON(data, options);
					break;
					
				case dataGlobal.Boolean:
					result = String(data, options);
					break;
					
				case dataGlobal.Number:
					if (isFinite(data) === false) 
					{
						var action = options[SPECIAL_NUMBER_SERIALIZATION_ACTION_PROPERTY] || Serialization.THROW_ACTION;
						
						switch (action)
						{
							case Serialization.SERIALIZE_ACTION:
								result = String(data, options);
								break;
								
							case Serialization.NULLIFY_ACTION:
								result = "null";
								break;
									
							case Serialization.THROW_ACTION:
							default:
								throw new Error("Serialization of special numbers is not supported unless the specialNumberSerializationAction option is set to 'serialize'");
						}
					}
					else 
					{
						result = String(data, options);
					}
					break;
					
				case dataGlobal.Object:
					result = ObjectToJSON(data, options);
					break;
					
				case dataGlobal.String:
					result = StringToJSON(data, options);
					break;
					
				case dataGlobal.Function:
					// should not get here because we've checked for being
					// serializable, but just in case
					break;
					
				default: // custom built-ins
					if (options[USE_CUSTOM_SERIALIZERS_PROPERTY]) 
					{
						var typeName = findHandlerName(data);
					
						if (typeName !== null) 
						{
							result = StringToJSON("~" + typeName + ":" + typeHandlers[typeName].serializer(data) + "~");
						}
						else 
						{
							// isSerializeable said we could serialize this object,
							// so treat it as a generic object
							result = ObjectToJSON(data, options);
						}
					}
					else if (ctor === dataGlobal.Date) 
					{
						var action = options[DATE_SERIALIZATION_ACTION_PROPERTY] || Serialization.THROW_ACTION;
						
						switch (action)
						{
							case Serialization.SERIALIZE_ACTION:
								result = DateToJSON(data);
								break;
								
							case Serialization.NULLIFY_ACTION:
								result = "null";
								break;
								
							case Serialization.RETURN_OBJECT_ACTION:
								result = "{}";
								break;
								
							case Serialization.THROW_ACTION:
							default:
								throw new Error("Serialization of Dates is not supported unless the dateSerializationAction option is set to 'serialize'");
						}
					}
					else 
					{
						// isSerializeable said we could serialize this object,
						// so treat it as a generic object
						result = ObjectToJSON(data, options);
					}
					break;
			}
		}
	}
	
	return result;
}

/**
 * toJaxerJSONString
 * 
 * @private
 * @param {Object} data
 * @param {Object} options
 */
function toJaxerJSONString(data, options)
{
	var result = NO_RESULT;
	var wrappedItems = [];
	
	/**
	 * A wrapped object is used to hold objects that are not expandable. We
	 * need to be able to add an id property to each object to find cycles and
	 * mult-references in the data graph. If that object doesn't allow new
	 * properties to be added to it (typically XPCOM wrapper objects), then we
	 * can use an instance of WrappedObject in its place. This object will
	 * serve only as a container for an object and its id. This later will be
	 * expanded back into the serialization stream as the underlying object so
	 * this will never appear in the final JSON string
	 * 
	 * @private
	 * @constructor
	 * @param {Object} id
	 * @param {Object} object
	 */
	function WrappedObject(id, object)
	{
		// set id
		this[ID_PROPERTY] = id;
		
		// save reference so we can test if this is exactly equivalent to other
		// references to this object
		this.object = object;
		
		// add to wrapped item list
		wrappedItems.push(this);
	}
	
	/**
	 * Since wrapped objects can't have properties added to them, we need to
	 * check the wrappedItems array to see if it exists there. This is
	 * equivalent to checking if the id property has been defined on an object
	 * that couldn't have that property added to it
	 * 
	 * @private
	 * @param {Object} object
	 * @return {Boolean}
	 */
	function isWrappedItem(object)
	{
		var length = wrappedItems.length;
		var result = false;
		
		for (var i = 0; i < length; i++)
		{
			var wrappedItem = wrappedItems[i];
			
			if (wrappedItem.object === object)
			{
				result = true;
				break;
			}
		}
		
		return result;
	}
	
	/**
	 * This function will return either a WrappedItem instance or the object
	 * passed into the function. If the object has been wrapped, then its
	 * wrapper is returned; othewise, we return the object itself
	 * 
	 * @private
	 * @param {Object} object
	 * @return {Object}
	 */
	function getWrappedItem(object)
	{
		var length = wrappedItems.length;
		var result = object;
		
		for (var i = 0; i < length; i++)
		{
			var wrappedItem = wrappedItems[i];
			
			if (wrappedItem.object === object)
			{
				result = wrappedItem;
				break;
			}
		}
		
		return result;
	}
	
	/**
	 * Walk the object graph and tag all items in the graph. Note that cycles
	 * and multi-references are detected in this process and all special
	 * properties used for this discovery process are later removed.
	 * 
	 * @private
	 * @return {Boolean}
	 * 		Return true if this specifed object contains references; otherwise,
	 * 		return false. This value can be used to decide if this object needs
	 * 		to be represented as standard JSON or in our extended format.
	 */
	function tagReferences()
	{
		var result = false;
		
		var index = 0;
		var queue = [data];
		
		while (queue.length > 0)
		{
			var item = queue.shift();
			
			if (item !== null && item !== undefined)
			{
				if (!item.hasOwnProperty || (item.hasOwnProperty(ID_PROPERTY) === false && isWrappedItem(item) === false))
				{
					// NOTE: In some browsers, such as Safari 3 and possibly
					// Firefox 2, RegExp's yield "function" here, and there are
					// other such examples
					if (typeof(item) === "object" || typeof(item) === "function") 
					{
						var type = item.constructor;
						var itemGlobal = getWindow(item);
						
						if (type === itemGlobal.Array) 
						{
							if (item.length > 0) 
							{
								item[ID_PROPERTY] = index;
								options[ITEMS_PROPERTY][index] = item;
								index++;
								
								for (var i = 0; i < item.length; i++) 
								{
									// We only need to process elements that
									// are serializeable since non-serializeable
									// elements will be skipped later
									if (isSerializeable(item[i])) 
									{
										queue.push(item[i]);
									}
								}
							}
						}
						else 
						{
							var handlerName = findHandlerName(item);
							
							if (type === itemGlobal.Object || handlerName !== null) 
							{
								try 
								{
									item[ID_PROPERTY] = index;
									options[ITEMS_PROPERTY][index] = item;
								} 
								catch (e) 
								{
									// Some objects, like XPCOM objects, don't
									// allow properties to be added to them, so,
									// we wrap these objects in WrappedObjects
									// for later special processing
									options[ITEMS_PROPERTY][index] = new WrappedObject(index, item);
								}
								
								index++;
							}
							
							// only process child properties for objects that
							// don't have custom serialization
							if (handlerName === null) 
							{
								for (var p in item) 
								{
									// no need to process id properties that were just added
									if (p !== ID_PROPERTY) 
									{
										try 
										{
											// We only need to process elements that
											// are serializeable since non-serializeable
											// elements will be skipped later
											if (isSerializeable(item[p])) 
											{
												queue.push(item[p]);
											}
										} 
										catch (e) 
										{
											Jaxer.Log.debug("During serialization, could not access property " + p + " so it will be ignored");
										}
									}
								}
							}
						}
					}
				}
				else
				{
					// found multiple references to the same object or array
					result = true;
				}
			}
		}
		
		return result;
	}
	
	/**
	 * Convert the specified items into a JSON string emitting special
	 * string values for references
	 * 
	 * @private
	 * @return {String}
	 * 		The resulting JSON string
	 */
	function toJSONWithReferences()
	{
		var items = options[ITEMS_PROPERTY];
		var references = [];
			
		for (var i = 0; i < items.length; i++)
		{
			var item = items[i];
			
			// grab stand-in object if this is a WrappedObject
			if (item.constructor === WrappedObject)
			{
				item = item.object;
			}
			
			var type = item.constructor;
			var itemGlobal = getWindow(item);

			switch (type)
			{
				case itemGlobal.Array:
					var parts = [];
					
					for (var j = 0; j < item.length; j++)
					{
						var elem = getWrappedItem(item[j]);
						
						if (elem !== undefined && elem !== null && elem.hasOwnProperty && elem.hasOwnProperty(ID_PROPERTY))
						{
							parts.push('"~' + elem[ID_PROPERTY] + '~"');
						}
						else
						{
							parts.push(toCrockfordJSONString(elem, options));
						}
					}
					
					references.push("[" + parts.join(",") + "]");
					break;
					
				case itemGlobal.Object:
					var parts = [];
					
					for (var p in item)
					{
						if (p !== ID_PROPERTY)
						{
							var elem = getWrappedItem(item[p]);
							var k = '"' + p + '":';
							
							if (elem !== undefined && elem !== null && elem.hasOwnProperty && elem.hasOwnProperty(ID_PROPERTY))
							{
								parts.push(k + '"~' + elem[ID_PROPERTY] + '~"');
							}
							else
							{
								parts.push(k + toCrockfordJSONString(elem, options));
							}
						}
					}
					
					references.push("{" + parts.join(",") + "}");
					break;
					
				default:
					var typeHandler = findHandlerName(item);
					
					if (typeHandler !== null)
					{
						references.push(toCrockfordJSONString(item, options));
					}
					else
					{
						// log and/or throw exception?
					}
					break;
			}
		}
		
		return "[" + references.join(",") + "]";
	}
	
	/**
	 * Remove id properties used to detect cycles and multi-references
	 */
	function untagReferences()
	{
		var items = options[ITEMS_PROPERTY];
		
		for (var i = 0; i < items.length; i++) 
		{
			var item = items[i];
			
			// only non-wrapped objects were able to have the id property added to them
			if (item.constructor !== WrappedObject) 
			{
				delete item[ID_PROPERTY];
			}
		}
	}
	
	// start of function body
	
	if (data !== undefined)
	{
		if (tagReferences() === false)
		{
			// we didn't find any cycles or multi-references, so remove the
			// properties we used to identify those structures
			untagReferences();
			
			// and then serialize the data as standard JSON
			result = toCrockfordJSONString(data, options);
		}
		else
		{
			// we did find a cycle or multi-reference, so emit our semantically
			// special JSON structure
			result = toJSONWithReferences();
			
			// we're done with our object tags, so remove those
			untagReferences();
		}
	}

	return result;
}

/*
 * BEGIN: Public Serialization functions section
 */

/**
 * Add a top-level JSON serializer
 * 
 * @alias Jaxer.Serialization.addDeserializer
 * @param {String} name
 * 		The unique name of the deserializer. This name can be specified in the
 * 		options object provided to the fromJSONString function. That will select
 * 		this deserializer as the top-level function to deserialize the specified
 * 		object. Note that case is not significant
 * @param {Function} deserializer
 * 		The function used to deserialized the JSON string created by the
 * 		associated serializer.
 * @param {Function} [beforeDeserialization]
 * 		An optional function that will be called before the top-level
 * 		deserialization process begins. This function should take a single
 * 		parameter which will be the options object provided to the
 * 		fromJSONString function. Note that the options object will be an
 * 		inherited clone of the object sent to fromJSONString. This allows this
 * 		function to initialize any data structures needed by the deserializer
 * 		without altering the original options object passed into fromJSONString
 * @param {Function} [afterDeserialization]
 * 		An optional function that will be called after the top-level
 * 		deserialization process ends. This function should take a single
 * 		parameter which will be the options object provided to the
 * 		fromJSONString function. Note that the options object will be an
 * 		inherited clone of the object sent to the fromJSONString.
 */
Serialization.addDeserializer = function(name, deserializer, beforeDeserialization, afterDeserialization)
{
	if (typeof(name) === "string" && typeof(deserializer) === "function")
	{
		name = name.toLocaleLowerCase();
		
		// Only allow "jaxer" to be registered once
		if (name !== Serialization.JAXER_METHOD || deserializers.hasOwnProperty(Serialization.JAXER_METHOD) === false)
		{
			var handler = {
				deserializer: deserializer,
				beforeDeserialization: (typeof(beforeDeserialization) === "function") ? beforeDeserialization : function() {},
				afterDeserialization: (typeof(afterDeserialization) === "function") ? afterDeserialization : function() {}
			};
			
			deserializers[name] = handler;
		}
	}
};

/**
 * Add a top-level JSON serializer
 * 
 * @alias Jaxer.Serialization.addSerializer
 * @param {String} name
 * 		The unique name of the serializer. This name can be specified in the
 * 		options object provided to the toJSONString function. That will select
 * 		this serializer as the top-level function to serialize the specified
 * 		object. Note that case is not significant
 * @param {Function} serializer
 * 		The function used to serialize data. This function should take two
 * 		arguments: the actual data to serialize and an options object
 * @param {Function} [beforeSerialization]
 * 		An optional function that will be called before the top-level
 * 		serialization process begins. This function should take a single
 * 		parameter which will be the options object provided to the toJSONString
 * 		function. Note that the options object will be an inherited clone of
 * 		the object sent to the toJSONString. This allows this function to
 * 		initialize any data structures needed by the serializer without altering
 * 		the original options object passed into toJSONString
 * @param {Function} [afterSerialization]
 * 		An optional function that will be called after the top-level
 * 		serialization process ends. This function should take a single
 * 		parameter which will be the options object provided to the toJSONString
 * 		function. Note that the options object will be an inherited clone of
 * 		the object sent to the toJSONString.
 */
Serialization.addSerializer = function(name, serializer, beforeSerialization, afterSerialization)
{
	if (typeof(name) === "string" && typeof(serializer) === "function")
	{
		name = name.toLocaleLowerCase();
		
		// Only allow "jaxer" to be registered once
		if (name !== Serialization.JAXER_METHOD || serializers.hasOwnProperty(Serialization.JAXER_METHOD) === false)
		{
			var handler = {
				serializer: serializer,
				deserializer: (typeof(deserializer) === "function") ? deserializer : function() {},
				beforeSerialization: (typeof(beforeSerialization) === "function") ? beforeSerialization : function() {},
				afterSerialization: (typeof(afterSerialization) === "function") ? afterSerialization : function() {}
			};
			
			serializers[name] = handler;
		}
	}
};

/**
 * Add handlers for custom serialization/deserialization
 * 
 * @alias Jaxer.Serialization.addTypeHandler
 * @param {String} name
 * 		The fully-qualified name of the type. This should reflect the full,
 * 		potentially dotted, notation you would need to use to access this type's
 * 		constructor from the global context.
 * @param {Function} serializer
 * 		A function that takes an instance of the type it serializes and that
 * 		returns a string representation of the type suitable as input into
 * 		the deserializer
 * @param {Function} deserializer
 * 		A function that takes a string produced by the custom serializer and
 * 		that returns a new instance of the custom supported type.
 * @param {Function} [canSerialize]
 * 		An optional function that takes an object instance and returns a
 * 		boolean. This function should return true if it the current handler is
 * 		able to serialize the object passed to it.
 * @param {Function} [canDeserialize]
 * 		An optional function that takes an object instance and returns a
 * 		boolean. This function should return true if it the current handler is
 * 		able to deserialize the string passed to it.
 */
Serialization.addTypeHandler = function(name, serializer, deserializer, canSerialize, canDeserialize)
{
	if
	(
			typeof(name) === "string"
		&&	VALID_TYPE_PATTERN.test(name)
		&&	typeof(serializer) === "function"
		&&	typeof(deserializer) === "function"
	)
	{
		// add handlers
		var handler = {
			constructor: null,
			serializer: serializer,
			deserializer: deserializer
		};
		
		// set serialization test function
		if (typeof(canSerialize) === "function")
		{
			handler.canSerialize = canSerialize;
		}
		else
		{
			handler.canSerialize = function(item)
			{
//				var candidate = handler.constructor;
				var result = false;
				
//				// We have to do lazy loading of constructors so we have references
//				// from the correct global
//				if (candidate === null) 
//				{
					// look up constructor
					var parts = name.split(/\./);
					
					// start at global
					var candidate = getWindow(item);
					
					// and traverse each segment of the type
					for (var i = 0; i < parts.length; i++) 
					{
						var part = parts[i];
						
						if (candidate && ((candidate.hasOwnProperty && candidate.hasOwnProperty(part)) || (part in candidate)))
						{
							candidate = candidate[part];
						}
						else 
						{
							candidate = null;
							break;
						}
					}
					
//					if (candidate !== null) 
//					{
//						handler.constructor = candidate;
//					}
//					else 
//					{
//						handler.constructor = undefined;
//					}
//				}
				
				if (candidate !== null) 
				{
					result = candidate === item.constructor;
				}
				
				return result;
			}
		}
		
		// set deserialization test function
		if (typeof(canDeserialize) === "function")
		{
			handler.canDeserialize = canDeserialize;
		}
		else
		{
			handler.canDeserialize = function(str)
			{
				return true; // By default we should be able to handle any string we've serialized
			}
		}
		
		typeHandlers[name] = handler;
	}
};

/**
 * Reconstructs a Javascript data structure from a JSON string. Note that the
 * serialization mode ('Jaxer', 'JSON', or 'nativeJSON') can be specified in
 * the "options" parameter with the 'as' property. This will default to
 * 'nativeJSON' when either no options are passed in or if the 'as' property is
 * not defined. See Jaxer.Serialization.toJSONString for more details.
 * 
 * @alias Jaxer.Serialization.fromJSONString
 * @param {String} json
 * 		A string in the JSON format
 * @param {Object} options
 * 		The options objecct which can be used to control deserialization
 * @return {Object}
 * 		The resulting object graph after converting the JSON string to the
 * 		equivalent Javascript data structure
 * @see Jaxer.Serialization.toJSONString
 */
Serialization.fromJSONString = function fromJSONString(json, options)
{
	var result = NO_RESULT;
	
	// setup default values
	if (options && typeof(options) === "object")
	{
		// prevent changes to the original options object
		var clone = Jaxer.Util.protectedClone(options);
		
		// add properties to our clone when the original options object does
		// not contain them
		
		if (options.hasOwnProperty("as") === false)
		{
			clone.as = Serialization.NATIVE_JSON_METHOD;
		}
		
		// make sure we use the clone from here on
		options = clone;
	}
	else
	{
		// create a new object with default values
		options = {
			as: Serialization.NATIVE_JSON_METHOD
		};
	}
	
	// get the name of the serializer we're supposed to use	
	var deserializerName = options.as.toLocaleLowerCase();
	
	// grab appropriate serializer
	if (serializers.hasOwnProperty(deserializerName))
	{
		// grab handler functions for this serialization method
		var handler = deserializers[deserializerName];
		
		// allow serializer to perform any initialization it needs
		handler.beforeDeserialization(options);
		
		// deserialize
		result = handler.deserializer(json, options);
		
		// allow serializer to perform any cleanup it needs
		handler.afterDeserialization(options);
	}
	else
	{
		throw new Error("Unknown deserialization method: '" + options.as + "'");
	}
	
	return result;
};

/**
 * Remove support for the custom JSON serializer
 * 
 * @alias Jaxer.Serialization.removeSerializer
 * @param {String} name
 * 		The name of the serializer to remove. Note that case is not significant
 * @return {Boolean}
 * 		Returns true if the serializer was successfully removed. Note that this
 * 		function will return false if you attempt to remove a handler that is
 * 		not already registered or if it is a built-in serializer like "jaxer".
 */
Serialization.removeSerializer = function(name)
{
	var result = false;
	
	if (typeof(name) === "string")
	{
		name = name.toLocaleLowerCase();
		
		if (name !== Serialization.JAXER_METHOD) 
		{
			result = delete serializers[name];
		}
	}
	
	return result;
};

/**
 * Remove support for custom serialization/deserialization for the specified
 * type
 * 
 * @alias Jaxer.Serialization.removeTypeHandler
 * @param {String} name
 * 		The fully qualified name of the type to remove
 * @return {Boolean}
 * 		Returns true if the handler was successfully removed. Note that this
 * 		function will return false if you attempt to remove a handler that is
 * 		not already registered.
 */
Serialization.removeTypeHandler = function(name)
{
	var result = false;
	
	if (typeof(name) === "string") 
	{
		result = delete typeHandlers[name];
	}
	
	return result;
};

/**
 * Convert the specified object into a JSON representation. Note that we have
 * modified JSON to support object references (cycles) and to convert Dates into
 * a special format that will be recognized by our code during deserialization.
 * 
 * This function includes an optional second parameter which can be used to
 * control how the data is serialized. If the options parameter defines an 'as'
 * property, that will be used to select the serialization format. Currently,
 * the values 'Jaxer', 'JSON', and 'nativeJSON' are supported. 'Jaxer' includes
 * support for cycles, multi-refs, and custom type serializers. 'JSON' and
 * 'nativeJSON' follow the serialization format and semantics as defined by
 * Douglas Crockford on the json.org website.
 * 
 * When specifying the 'Jaxer' serializer, additional options are available. The
 * "useCustomSerializers" has a boolean value which defaults to true. When this
 * property is true, any type serializers that have been registered via
 * addTypeHandler will be used in the serialization process. When this value is
 * false, items needing custom serialization will be ignored as they would be in
 * the "JSON" format. The "undefinedSerializationAction" property determines how
 * the 'undefined' value is handled. The action defaults to 'serialize', but
 * 'throw' is also supported which will throw an exception when trying to
 * serialize 'undefined'.
 * 
 * When specifying the 'JSON' serializer, additional options are available. The
 * 'maxDepth' property, which defaults to 10, is used to prevent deep recursion.
 * If the recursion level is encountered, the 'maxDepthAction' property
 * determines the serializer's action. 'truncate' will emit a "__truncated__"
 * string in place of the object that would cause the recursion level to be
 * exceeded. 'throw' will throw an exception. The 'dateSerializationAction'
 * property is used to determine how dates are processed. A value of 'serialize'
 * will convert the date to a specially formatted string as described in the
 * json.org example code. A value of 'throw' will throw an exception when a date
 * is encountered. Finally, a value of "return object" will return an empty
 * object in place of the Date itself. The 'undefinedSerializationAction'
 * property is used to determine how 'undefined' is processed. A value of
 * 'serialize' will convert the value to 'undefined'. 'throw' will throw an
 * exception and 'nullify' will return 'null'. The
 * 'specialNumberSerializationAction' property is used to determine how
 * Infinity, -Infinity, and NaN are processed. A value of 'serialize' will
 * convert the value to their text representation which is the same as the
 * identifier used to represent them. 'throw' will throw an exception and
 * 'nullify' will return null.
 * 
 * When specifying the 'nativeJSON' serializer, the built-in native support for
 * JSON serialization will be used, when available. This serialization does not
 * support any custom options. In the case where 'nativeJSON' is specified but
 * is not available, this mode will fallback to the 'JSON' mode with options
 * specified in that mode to match the behavior of the native JSON
 * implementation as specificed in the ECMAScript 3.1 specification.
 * 
 * Note that other serializers can be registered with Jaxer. Most likely those
 * serializers will define their own set of options. You will need to refer to
 * the implementors documentation to determine those properties, their values,
 * and their associated semantics.
 * 
 * @alias Jaxer.Serialization.toJSONString
 * @param {Object} data
 * 		The source object to convert to a JSON string
 * @param {Object} [options]
 * 		An optional object used to specify configuration info to the selected
 * 		serializer
 * @return {String}
 * 		The resulting JSON string which can be reversed back into the source
 * 		object via Serialization.fromJSONString
 */
Serialization.toJSONString = function(data, options)
{
	var result = NO_RESULT;
	
	// setup default values
	if (options && typeof(options) === "object")
	{
		// prevent changes to the original options object
		var clone = Jaxer.Util.protectedClone(options);
		
		// add properties to our clone when the original options object does
		// not contain them
		
		if (options.hasOwnProperty("as") === false)
		{
			clone.as = Serialization.NATIVE_JSON_METHOD;
		}
		
		// make sure we use the clone from here on
		options = clone;
	}
	else
	{
		// create a new object with default values
		options = {
			as: Serialization.NATIVE_JSON_METHOD
		};
	}
	
	// get the name of the serializer we're supposed to use	
	var serializerName = options.as.toLocaleLowerCase();
	
	// grab appropriate serializer
	if (serializers.hasOwnProperty(serializerName))
	{
		// grab handler functions for this serialization method
		var handler = serializers[serializerName];
		
		// allow serializer to perform any initialization it needs
		handler.beforeSerialization(options);
		
		// serialize
		result = handler.serializer(data, options);
		
		// allow serializer to perform any cleanup it needs
		handler.afterSerialization(options);
	}
	else
	{
		throw new Error("Unknown serialization method: '" + options.as + "'");
	}
	
	return result;
};

/*
 * BEGIN: Serializer registration section
 */

// register Jaxer-style serialization which supports cycles, multi-references
// and customer serialization by type
(function() {
	// common options handling
	function initOptions(options)
	{
		// make sure the following values are defined
		var defaults = {};
		
		// NOTE: These properties are not used by the "jaxer" serializer proper,
		// but in cases where there are no cycles or multi-refs, we simply call
		// the "json" serializer which does use these properties.
		defaults[MAX_DEPTH_PROPERTY] = DEFAULT_MAX_DEPTH;
		defaults[MAX_DEPTH_ACTION_PROPERTY] = Serialization.TRUNCATE_ACTION;
		
		defaults[USE_CUSTOM_SERIALIZERS_PROPERTY] = true;
		defaults[UNDEFINED_SERIALIZATION_ACTION_PROPERTY] = Serialization.SERIALIZE_ACTION;
		defaults[SPECIAL_NUMBER_SERIALIZATION_ACTION_PROPERTY] = Serialization.SERIALIZE_ACTION;
		
		Jaxer.Util.safeSetValues(options, defaults);
		
		// clear custom serializer cache
		clearHandlerCache();
	}
	
	Serialization.addSerializer(
		Serialization.JAXER_METHOD,
		toJaxerJSONString,
		function beforeJaxerSerialization(options)
		{
			// do common options setup
			initOptions(options);
			
			// make sure "items" array exists
			options[ITEMS_PROPERTY] = [];
		},
		function afterJaxerSerialization(options)
		{
			// release any references we might have in the "items" array
			delete options[ITEMS_PROPERTY];
		}
	);
	
	Serialization.addDeserializer(
		Serialization.JAXER_METHOD,
		fromJaxerJSONString,
		initOptions
	);
})();

// register Crockford-style serialization
(function() {
	function initOptions(options)
	{
		// make sure the following values are defined
		var defaults = {};
		
		defaults[MAX_DEPTH_PROPERTY] = DEFAULT_MAX_DEPTH;
		defaults[MAX_DEPTH_ACTION_PROPERTY] = Serialization.THROW_ACTION;
		defaults[DATE_SERIALIZATION_ACTION_PROPERTY] = Serialization.SERIALIZE_ACTION;
		defaults[UNDEFINED_SERIALIZATION_ACTION_PROPERTY] = Serialization.NULLIFY_ACTION;
		defaults[SPECIAL_NUMBER_SERIALIZATION_ACTION_PROPERTY] = Serialization.NULLIFY_ACTION;
		
		Jaxer.Util.safeSetValues(options, defaults);
		
		// make sure we don't use our custom serializers in "Crockford" mode
		options[USE_CUSTOM_SERIALIZERS_PROPERTY] = false;
	}
	
	Serialization.addSerializer(
		Serialization.JSON_METHOD,
		toCrockfordJSONString,
		initOptions
	);
	
	Serialization.addDeserializer(
		Serialization.JSON_METHOD,
		fromCrockfordJSONString,
		initOptions
	);
})();

// register native JSON serialization
(function() {
	// These defaults cause the JS-based serialization to match the native
	// implementation. These are used when the native code is not available
	var defaults = {};
	
	defaults[MAX_DEPTH_PROPERTY] = DEFAULT_MAX_DEPTH;
	defaults[MAX_DEPTH_ACTION_PROPERTY] = Serialization.THROW_ACTION;
	defaults[DATE_SERIALIZATION_ACTION_PROPERTY] = Serialization.RETURN_OBJECT_ACTION;
	defaults[UNDEFINED_SERIALIZATION_ACTION_PROPERTY] = Serialization.NULLIFY_ACTION;
	defaults[SPECIAL_NUMBER_SERIALIZATION_ACTION_PROPERTY] = Serialization.NULLIFY_ACTION;
	
	function hasNativeJSON()
	{
		var window = getWindow();
		var result = false;
				
		if (window && "JSON" in window)
		{
			if ("stringify" in window.JSON)
			{
				result = true;
			}
		}
		
		return result;
	}
	
	Serialization.addSerializer(
		Serialization.NATIVE_JSON_METHOD,
		function(data, options)
		{
			if (hasNativeJSON())
			{
				return JSON.stringify(data);
			}
			else
			{
				return toCrockfordJSONString(data, defaults);
			}
		}
	);
	
	Serialization.addDeserializer(
		Serialization.NATIVE_JSON_METHOD,
		function(json, options)
		{
			if (hasNativeJSON())
			{
				return JSON.parse(json);
			}
			else
			{
				return fromCrockfordJSONString(json, defaults);
			}
		}
	);
})();

/*
 * BEGIN: Custom serialization handlers section
 */

// register Date serializer/deserializer
Serialization.addTypeHandler(
	"Date",
	function serializeDate(date)
	{
		// Format integers to have at least two digits.
		function pad(n)
		{
			return n < 10 ? '0' + n : n;
		}
	
		return date.getUTCFullYear() + '-' +
			pad(date.getUTCMonth() + 1) + '-' +
			pad(date.getUTCDate()) + 'T' +
			pad(date.getUTCHours()) + ':' +
			pad(date.getUTCMinutes()) + ':' +
			pad(date.getUTCSeconds());
	},
	function deserializeDate(serializedDate)
	{
		var match = serializedDate.match(DATE_PATTERN);
		var result = null;
						
		if (match !== null)
		{
			var win = getWindow();
			result = new win.Date(Date.UTC(match[1], match[2] - 1, match[3], match[4], match[5], match[6]));
		}
		
		return result;
	}
);

// register RegExp serializer/deserializer
Serialization.addTypeHandler(
	"RegExp",
	function serializeRegExp(regex)
	{
		return regex.toString();
	},
	function deserializeRegExp(serializedRegex)
	{
		// NOTE: Some browsers add custom regex flags. Since we can't know what
		// all of those might be at any given point in time, we allow any
		// character to serve as a flag as opposed to simply [img]
		var match = serializedRegex.match(/^\/(.+)\/([a-zA-Z]+)?$/);
		var result = serializedRegex;
		
		if (match !== null)
		{
			var win = getWindow();
			result = new win.RegExp(match[1], match[2]);
		}
		
		return result;
	}
);

// NOTE: If this is set to false, we won't bother trying to use the type handler
// client-side
var tryXMLDocument = true;

// register XMLDocument serializer/deserializer
Serialization.addTypeHandler(
	"XMLDocument",
	function serializeXMLDocument(doc)
	{
		var win = getWindow();
		var result = null;
		
		if (win.XMLSerializer) 
		{
			var serializer = new win.XMLSerializer();
			result = serializer.serializeToString(doc);
		}
		else
		{
			result = doc.xml;
		}
		
		return result;
	},
	function deserializeXMLDocument(xml)
	{
		var win = getWindow();
		var result = null;
		
		if (win.DOMParser) 
		{
			var parser = new win.DOMParser();
			result = parser.parseFromString(xml, "application/xml");
		}
		else if (win.ActiveXObject) 
		{
			try 
			{
				var doc = new win.ActiveXObject("Microsoft.XMLDOM");
				doc.async = false;
				doc.loadXML(xml);
				result = doc;
			} 
			catch (e) 
			{
				if (!Jaxer.isOnServer) tryXMLDocument = false;
			}
		}
		
		return result;
	},
	function canSerializeXMLDocument(data)
	{
		if (!Jaxer.isOnServer && !tryXMLDocument) return false;
		var win = getWindow(data);
		if (data && win.XMLSerializer && data.constructor == win.XMLDocument) return true;
		if (data && win.ActiveXObject && (typeof data.constructor == "undefined") && (typeof data.xml == "string")) return true;
		if (!Jaxer.isOnServer) tryXMLDocument = false;
		return false;
	},
	function canDeserializeXMLDocument(str)
	{
		if (!Jaxer.isOnServer && !tryXMLDocument) return false;
		var win = getWindow();
		if (win.DOMParser) return true;
		if (win.ActiveXObject) return true; // There's no way to know more without trying: if it turns out we can't actually deserialize, we'll find out the first time we try to do so. 
		if (!Jaxer.isOnServer) tryXMLDocument = false;
		return false;
	}
);

/*
 * END: Custom serialization handlers section
 */

// expose Serialization in Jaxer namespace
Jaxer.Serialization = Serialization;

if (Jaxer.isOnServer)
{
	frameworkGlobal.Serialization = Jaxer.Serialization;
}

})();