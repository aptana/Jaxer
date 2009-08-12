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

(function() {

/**
 * @namespace {Jaxer.Util} A namespace to hold a miscellany of generic utility
 * functions and other objects. In particular, it also holds sub-namespaces
 * for more specific operations.
 */
var Util = {};

/**
 * Remove items from an array that do not pass a given criteria. Each item in
 * the specified array will be passed to the filtering function. If that
 * function returns true, then the item will remain in the specified array. If
 * the function returns false, the item is removed from the specified array.
 * 
 * Note that the specified array is altered in place. If you prefer to create a
 * new array, leaving the original intact, then use the native Array's 
 * filter method instead.
 * 
 * @alias Jaxer.Util.filterInPlace
 * @param {Array} array
 * 		The source array to be filtered
 * @param {Function} func
 * 		The filtering function to apply to each array item. This filter has two
 * 		parameters. The first parameter is the current item in the array that is
 *		potentially being filtered. The second parameter is the index of the
 *		item potentially being filtered. The index can be used in cases where
 *		the filtering decision needs to be determined based on proximity to
 *		other items in the array
 * @return {Array}
 * 		Returns the filtered array containing only items that were approved by
 * 		the filtering function. Note that this instance will be the same as the
 * 		instance passed into the function. This is provided as a convenience and
 * 		to keep this function signature the same as Util.filter's signature.
 */
Util.filterInPlace = function filterInPlace(array, func)
{
	var length = array.length;
	
	for (var i = 0; i < length; i++)
	{
		var keep = (func) ? func(item, i) : item;
		
		if (keep == false)
		{
			array.splice(i, 1);
			i--;
			length--;
		}
	}
	
	return array;
};

/**
 * Replace each item of an array by applying a function and then replacing the
 * original item with the results of that function.
 * 
 * @alias Jaxer.Util.mapInPlace
 * @param {Array} array
 * 		The source array
 * @param {Function} func
 * 		The function to apply to each of the items in the source array. The
 * 		function has two parameters. The first parameter is the current item in
 * 		the array that is being transformed. The second parameter is the index
 * 		of the item being transformed.
 * @return {Array}
 * 		Returns the mapped array. Note that this instance will be the same as
 * 		the instance passed into the function. This is provided as a convenience
 * 		and to keep this function's signature the same as the signature of the
 * 		native Array's map mathod.
 */
Util.mapInPlace = function mapInPlace(array, func)
{
	if (array == null) return;
	
	var length = array.length;
	
	for (var i = 0; i < length; i++)
	{
		array[i] = func(array[i], i);
	}
	
	return array;
};

/**
 * Determine if the specified object contains all properties in a list of
 * property names.
 * 
 * @alias Jaxer.Util.hasProperties
 * @param {Object} object
 * 		The source object
 * @param {String[]} properties
 * 		The list of property names to test on the specified object
 * @return {Boolean}
 * 		Returns true if all properties in the list exist on the specified object
 */
Util.hasProperties = function hasProperties(object, properties)
{
	var result = false;
	
	if (object && properties && properties.constructor === Array)
	{
		result = true;
		
		for (var i = 0; i < properties.length; i++)
		{
			var property = properties[i];
			
			if (object.hasOwnProperty[property] == false)
			{
				result = false;
				break;
			}
		}
	}
	
	return result;
};

/**
 * Get all property names or filtered subset of names from an object.
 * 
 * @alias Jaxer.Util.getPropertyNames
 * @param {Object} object
 * 		The source object
 * @param {Function} [filter]
 * 		An optional filter function to apply to the property's name and value. 
 * 		filter(name, value) should return something that's equivalent to true if
 * 		the property is to be included.
 * @param {Boolean} [asHash]
 * 		If true, returns the result as a hash (with all values set to true)
 * @return {Object}
 * 		A list or hash of the property names depending on the value provided to
 * 		the asHash parameter
 */
Util.getPropertyNames = function getPropertyNames(object, filter, asHash)
{
	var names = asHash ? {} : [];
	
	if (object)
	{
		for (var p in object)
		{
			try 
			{
				if (!filter || filter(p, object[p])) 
				{
					asHash ? (names[p] = true) : names.push(p);
				}
			} 
			catch (e) 
			{
				// do nothing -- just don't push it
			}
		}
	}
	
	return names;
};

/**
 * Tests whether the given function is native (i.e. for which there is actually
 * no source code)
 * 
 * @alias Jaxer.Util.isNativeFunction
 * @param {Function} func
 * 		The function to test
 * @return {Boolean}
 * 		True if it's a native function, false otherwise 
 */
Util.isNativeFunction = function isNativeFunction(func)
{
	return Util.isNativeFunctionSource(func.toSource());
}

/**
 * Tests whether the given string is the source of a native function (i.e. for
 * which there is actually no source code)
 * 
 * @alias Jaxer.Util.isNativeFunctionSource
 * @param {String} source
 * 		The source string to test
 * @return {Boolean}
 * 		True if it's a native function's source, false otherwise 
 */
Util.isNativeFunctionSource = function isNativeFunctionSource(source)
{
	return Boolean(source.match(/\)\s*\{\s*\[native code\]\s*\}\s*$/));
}

/**
 * Tests whether the given object is a Date object (even if it's from a
 * different global context)
 * 
 * @alias Jaxer.Util.isDate
 * @param {Object} obj
 * 		The object to test
 * @return {Boolean}
 * 		True if it's a Date (or at least seems to be a Date), false otherwise
 */
Util.isDate = function isDate(obj)
{
	if (!obj || (typeof obj != "object")) 
		return false;
	
	var global = Util.getGlobalContext(obj);
	if (global && global.Date)
		return (obj.constructor == global.Date);
		
	if (obj.constructor)
		return (obj.constructor.name == "Date");
	
	return false;
}

/**
 * Tests whether the given object is an Array object (even if it's from a
 * different global context). This returns false on Array-like objects
 * that are not in fact arrays, such as the arguments object inside a function.
 * 
 * @alias Jaxer.Util.isArray
 * @param {Object} obj
 * 		The object to test
 * @return {Boolean}
 * 		True if it's an Array (or at least seems to be an Array), false otherwise
 */
Util.isArray = function isArray(obj)
{
	if (!obj || (typeof obj != "object")) 
		return false;
	
	var global = Util.getGlobalContext(obj);
	if (global && global.Array)
		return (obj.constructor == global.Array);
		
	if (obj.constructor)
		return (obj.constructor.name == "Array");
	
	return false;
}

/**
 * Does nothing for the given number of milliseconds
 * 
 * @alias Jaxer.Util.sleep
 * @param {Number} milliseconds
 * 		The number of milliseconds to pause.
 */
Util.sleep = function sleep(milliseconds)
{
	var thread = Components.classes['@mozilla.org/thread-manager;1'].getService().currentThread;
	
	thread.sleep(milliseconds);
}

Util.setSafeSetter = function setSafeSetter(proto, propName, setter)
{
	proto.__defineSetter__(propName, function _setSafeSetter(value)
	{
		// Need to backup this setter and delete it off the prototype
		// before calling setter which may change the actual property, 
		// so the property is actually set and we also avoid recursion
		var myself = proto.__lookupSetter__(propName);
		delete proto[propName];
		setter(this, value);
		proto.__defineSetter__(propName, myself);
	});
}

/**
 * Tests whether the given object is devoid of any (enumerable) properties.
 * 
 * @alias Jaxer.Util.isEmptyObject
 * @param {Object} obj
 * 		The object to test
 * @return {Boolean}
 * 		false if there is (at least) one enumerable property, true otherwise
 */
Util.isEmptyObject = function isEmptyObject(obj)
{
	for (var prop in obj)
	{
		return false;
	}
	return true;
}

/**
 * Clones an object (actually any argument) and returns the clone. If obj is of type
 * "object", then the clone is created from the same constructor (but without any arguments).
 * For a deep clone, every (enumerable) property is itself cloned; otherwise, every
 * (enumerable) property is simply copied (by value or reference).
 * 
 * @alias Jaxer.Util.clone
 * @param {Object} obj
 * 		The object to clone. If it's not of type object, its value is simply copied and returned.
 * 		It is not altered.
 * @param {Boolean} [deep]
 * 		Whether to make a deep clone or a shallow one (just copy properties); by default, false.
 * @param {Number} [maxDepth]
 * 		An optional maximum cloning depth. By default it's 10. This prevents infinite loops.
 * @return {Object}
 * 		The new, cloned object.
 */
Util.clone = function clone(obj, deep, maxDepth)
{
	if (typeof maxDepth != "number") maxDepth = 10;
	var clone;
	if (typeof obj != "object")
	{
		clone = obj;
	}
	else
	{
		clone = new obj.constructor();
		if (maxDepth) 
		{
			for (var p in obj) 
			{
				clone[p] = deep ? Util.clone(obj[p], deep, maxDepth-1) : obj[p];
			}
		}
	}
	return clone;
}

/**
 * Extends an object by (shallow) cloning it and then copying all (enumerable) properties
 * from the extensions object to the new cloned object.
 * 
 * @alias Jaxer.Util.extend
 * @param {Object} obj
 * 		The object to use as a base and extend. It is not altered.
 * @param {Object} extensions
 * 		The object to use as extensions -- usually this is a simple
 * 		hashmap of properties and their values.
 * @return {Object}
 * 		The extended clone.
 */
Util.extend = function extend(obj, extensions)
{
	var extended = Util.clone(obj, false);
	for (var p in extensions)
	{
		extended[p] = extensions[p];
	}
	return extended;
}

/**
 * Find the global context ('window', in the browser) to which the given object
 * is ultimately parented. If no object is given, returns the developer page's window
 * object, and if that does not exist for some reason, returns the framework's
 * global context.
 * 
 * @alias Jaxer.Util.getGlobalContext
 * @advanced
 * @param {Object} [obj]
 * 		The object whose global context is to be found
 * @return {Object}
 * 		The global context ('window', in the browser)
 */
Util.getGlobalContext = function getGlobalContext(obj)
{
	var globalContext;
	// If an obj was given, try to find its global context
	if (obj != null && obj != undefined)
	{
		try
		{
			var current = obj;
			var parent = obj.__parent__;
			while (parent && parent !== current)
			{
				current = parent;
				parent = parent.__parent__;
			}
			if (current != obj)
			{
				globalContext = current;
			}
		}
		catch (e)
		{
			// Nothing to do -- just return the no-object global
		}
	}
	
	if (!globalContext) // if we haven't found one yet...
	{
		if (Jaxer.pageWindow) // prefer the user's global context
		{
			globalContext = Jaxer.pageWindow;
		}
		else
		{
			globalContext = Jaxer.frameworkGlobal;
		}
	}
	
	return globalContext;
}

/**
 * Finds the named object within the global context ('window', in the browser) to which the second argument
 * is ultimately parented. If no second argument is given, finds the named object in the developer page's window
 * object, and if that does not exist for some reason, finds it in the Jaxer framework's
 * global context.
 * 
 * @alias Jaxer.Util.findInGlobalContext
 * @param {String} objectName
 * 		The name of the object to find. If this name contains periods (".") then it's split into subnames
 * 		and each is used to search in the former. So "a.b.c" means get the value of the property "a" in the global context,
 * 		and in it get the value of the property "b", and in it get the value of the property "c".
 * 		If any of these is not defined it returns undefined.
 * @param {Object} [objForGlobalContext]
 * 		The object whose global context is the one to use for finding the named object, which defaults 
 * 		to the developer page's window object (or else the Jaxer framework's global)
 * @return {Object}
 * 		The found object, or undefined if it cannot be found
 */
Util.findInGlobalContext = function findInGlobalContext(objectName, objForGlobalContext)
{
	var globalContext = Util.getGlobalContext(objForGlobalContext);
	var names = objectName.split(/\./);
	var obj = globalContext;
	names.forEach(function(name)
	{
		if (!obj || (typeof obj != "object")) return undefined;
		obj = obj[name];
	})
	return obj;
}

/**
 * Creates a new instance of an object given the name of a constructor and the object
 * in which to find the constructor. Any parameters beyond these two will be passed
 * to the constructor, getting around an apparent limitation of JavaScript: the inability
 * to use "call" or "apply" on a constructor.
 * @param {String} constructorName
 * 		The name of the function to use as the constructor of the object
 * @param {Object} [objForGlobalContext]
 * 		If specified, this should be the "window"
 */
Util.createObject = function createObject(constructorName, objForGlobalContext)
{
	var constructor = Util.findInGlobalContext(constructorName, objForGlobalContext);
	if (!constructor)
	{
		throw new Exception("Could not find a constructor named '" + constructorName + "'");
	}
	var args = Array.slice(arguments, 2);
	var internalArgNames = ['arg'+i for (i in args)];
	var argNames = ["o"].concat(internalArgNames);
	var c = Function.constructor.apply(null,[argNames.join(","),
			"var constr = Jaxer.Util.findInGlobalContext('" + constructorName + "', o);\n" +
			"return new constr(" + internalArgNames.join(",") + ");"]
			);
	return c.apply(null, [objForGlobalContext].concat(args))
}

/**
 * Create a string representation of all properties in an object. A separator
 * string can be used as a delimited between each property and the user has the
 * object of showing the property values or not.
 * 
 * @private
 * @alias Jaxer.Util.__listProps
 * @param {Object} obj
 * 		The source object
 * @param {String} [separator]
 * 		The separator string to use between properties. If this value is not
 * 		specified or if it is undefined, the string "; " will be used by default
 * @param {Boolean} showContents
 * 		A boolean value indicating whether property values should shown in
 * 		addition to the property names.
 * @return {String} Returns a string representation of the specified object.
 */
Util.__listProps = function __listProps(obj, separator, showContents)
{
	if (obj == null) return "null";
	
	if (typeof obj == "undefined") return "undefined";
	
	var props = [];
	
	if (typeof separator == "undefined" || separator == null) separator = "; ";
	
	for (var p in obj)
	{
		props.push(p + (showContents ? ": " + obj[p] : "") );
	}
	
	props = props.sort();
	
	var type = typeof obj;
	var constr = "";
	
	if (typeof obj.constructor != "undefined")
	{
		constr = (typeof obj.constructor.name == "string") ? obj.constructor.name : obj.constructor.toString();
		
		if (!(obj instanceof obj.constructor))
		{
			constr += " (not an instanceof its own constructor!!)"
		}
	}
	
	props.unshift("[Type: " + type + ", constructor: " + constr + "]");
	
	return props.join(separator);
};

frameworkGlobal.Util = Jaxer.Util = Util;

})();
