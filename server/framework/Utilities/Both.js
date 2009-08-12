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

/*
 * The functions below are used in both the client and the server Jaxer frameworks,
 * to offer common APIs for some frequently-needed tasks that need to be implemented
 * differently on client and server.
 */

if (!Jaxer.Util) Jaxer.Util = {};

/**
 * Used to set events on DOM elements such that they "do the right thing" both
 * client-side and server-side. On the client, this acts as expected, setting a
 * property with the name eventName (e.g. onclick) on the DOM element. On the
 * server, the eventName attribute is set on the DOM element so it can be
 * serialized with the DOM before sending to the client. If the handler is a
 * (server side) function with a name, the attribute's value is handler.name +
 * "()". 
 * 
 * @alias Jaxer.setEvent
 * @param {Object} domElement
 * 		The element on which to set the event
 * @param {String} eventName
 * 		The name of the event to set
 * @param {Object} handler
 * 		The handler function, or the body (as a string)
 */
Jaxer.setEvent = function setEvent(domElement, eventName, handler)
{
	if (Jaxer.isOnServer)
	{
		var attribute;
		if (typeof handler == "function")
		{
			if (handler.name == "")
			{
				attribute = "(" + handler.toSource() + ")()";
			}
			else
			{
				attribute = handler.name + "()";
			}
		}
		else // handler should be a string (the handler function's body)
		{
			attribute = handler;
		}
		domElement.setAttribute(eventName, attribute);
	}
	else
	{
		var func;
		if (typeof handler == "function")
		{
			func = handler;
		}
		else // handler should be a string (the handler function's body)
		{
			func = new Function(handler);
		}
		domElement[eventName] = func;
	}
};

/**
 * Sets the title of the document and works on either the server or the client.
 * 
 * @alias Jaxer.setTitle
 * @param {String} title
 * 		The text of the title
 */
Jaxer.setTitle = function setTitle(title)
{
	if (Jaxer.isOnServer)
	{
		if (!Jaxer.pageWindow)
		{
			throw new Exception("Jaxer.pageWindow is not available for some reason");
		}
		var doc = Jaxer.pageWindow.document;
		if (!doc)
		{
			throw new Exception("Jaxer.pageWindow.document is not available for some reason");
		}
		var titleElement = doc.getElementsByTagName("title")[0];
		if (!titleElement)
		{
			var head = doc.getElementsByTagNames("head")[0];
			if (head)
			{
				titleElement = doc.createElement("title");
				head.appendChild(titleElement);
			}
		}
		if (titleElement)
		{
			titleElement.firstChild.data = title;
		}
	}
	else
	{
		document.title = title;
	}
};

/**
 * Returns an array whose elements consist of the elements of all the arrays or
 * array-like objects passed in as arguments. If any of the arguments is null
 * or undefined (i.e. is equivalent to false) it is skipped.
 * 
 * @alias Jaxer.Util.concatArrays
 * @param {Object} [...]
 * 		Any number of arrays or array-like objects (e.g. a function's arguments meta-array).
 * 		Note that, unlike Array.concat, the arguments here need to be arrays or array-like
 * 		objects that have a length property and an indexer (i.e. obj[i] is defined)
 * @return {Array}
 * 		The concatenated array
 */
Jaxer.Util.concatArrays = function concatArrays()
{
	var all = [];
	for (var iArg = 0, lenArgs = arguments.length; iArg < lenArgs; iArg++) 
	{
		var arr = arguments[iArg];
		if (arr) 
		{
			for (iArr=0, lenArr=arr.length; iArr<lenArr; iArr++) 
			{
				all.push(arr[iArr]);
			}
		}
	}
	return all;
};

/**
 * Creates a new object whose private prototype (the one used when looking up
 * property values) will be set to the object passed into this function. This
 * allows for the resulting clone object to add new properties and to redefine
 * property values without affecting the master object. If access to the master
 * object is required, the cloned object contains a '$parent' property which
 * can be used for that purpose.
 * 
 * @param {Object} master
 * @return {Object}
 * 		Returns a new object that effectively inherits all properties of the
 * 		passed in object via JavaScript's prototype inheritance mechanism.
 */
Jaxer.Util.protectedClone = function(master)
{
	// anonymous object creator
	var f = function() {};
	
	// attach original object to prototype
	f.prototype = master;
	
	// create a new object whose [proto] now points to the original object
	var result = new f();
	
	// make a local reference to the master in cases where we need to unroll
	// the prototype chain
	result.$parent = master;
	
	// return result;
	return result;
};

/**
 * If a property on "values" is not defined on the target object, then add
 * that property and associated value to the target object. Note that the target
 * object's [proto] chain is included in the search for each property.
 * 
 * @param {Object} targetObject
 * @param {Object} values
 */
Jaxer.Util.safeSetValues = function(targetObject, values)
{
	for (var p in values)
	{
		if (!targetObject[p])
		{
			targetObject[p] = values[p];
		}
	}
};
