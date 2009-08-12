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

/**
 * @namespace {Jaxer.Overrides} Namespace used for overriding some of the
 * built-in JavaScript and JavaScript-environment (user-agent) functions that
 * may not make sense or need to behave differently on the server.
 */
var Overrides = {};

var overridesToApply = []; // These will be set on the developer's server-side window object
var noOps = []; // e.g. if you want to disable setTimeout and setInterval, use: ["setTimeout", "setInterval"];

/**
 * Alert in a server-side context will generate an info-level log message
 * 
 * @alias Jaxer.Overrides.alert
 * @param {String} message
 */
Overrides.alert = function alert(message)
{
	Jaxer.Log.info(message);
}
overridesToApply.push(Overrides.alert);

/**
 * Prompt in a server-side context will generate an info-level log message
 * 
 * @alias Jaxer.Overrides.prompt
 * @param {String} message
 */
Overrides.prompt = function prompt(message)
{
	Jaxer.Log.info("(prompt called for: " + message + ")");
}
overridesToApply.push(Overrides.prompt);

// The following could be used to make all server-side XHRs into synchronous ones.
// Otherwise, async ones require pumping the event loop to get the onreadystatechange to fire in time
// But note that we may then have to stop circular references to the underlying XMLHttpRequest object
//Overrides.XMLHttpRequest = function XMLHttpRequest()
//{
//	this.xhr = Jaxer.XHR.getTransport();
//	this.status = this.xhr.status;
//	this.statusText = this.xhr.statusText;
//	this.readyState = this.xhr.readyState;
//	this.responseText = this.xhr.responseText;
//	this.responseXML = this.xhr.responseXML;
//	this.onreadystatechange = null;
//}
//Overrides.XMLHttpRequest.prototype.open = function open()
//{ 
//	var args = Array.prototype.slice.call(arguments); // Turn into an array for safer manipulation
//	if (args.length < 1) throw new Error("No method argument specified");
//	if (args.length < 2) throw new Error("No url argument specified");
//	args[2] = false; // server-side requests are always synchronous
//	this.xhr.open.apply(this.xhr, args);
//}
//Overrides.XMLHttpRequest.prototype.setRequestHeader = function setRequestHeader() { this.xhr.setRequestHeader.apply(this.xhr, arguments); }
//Overrides.XMLHttpRequest.prototype.overrideMimeType = function overrideMimeType() { this.xhr.overrideMimeType.apply(this.xhr, arguments); }
//Overrides.XMLHttpRequest.prototype.getResponseHeader = function getResponseHeader() { this.xhr.getResponseHeader.apply(this.xhr, arguments); }
//Overrides.XMLHttpRequest.prototype.getAllResponseHeaders = function getAllResponseHeaders() { this.xhr.getAllResponseHeaders.apply(this.xhr, arguments); }
//Overrides.XMLHttpRequest.prototype.send = function send()
//{ 
//	this.xhr.send.apply(this.xhr, arguments);
//	this.status = this.xhr.status;
//	this.statusText = this.xhr.statusText;
//	this.readyState = this.xhr.readyState;
//	this.responseText = this.xhr.responseText;
//	this.responseXML = this.xhr.responseXML;
//	if (typeof this.onreadystatechange == "function")
//	{
//		this.onreadystatechange();
//	}
//}
//overridesToApply.push(Overrides.XMLHttpRequest);

/**
 * Confirm in a server-side context will generate an info-level log message
 * 
 * @alias Jaxer.Overrides.confirm
 * @param {String} message
 */
Overrides.confirm = function confirm(message)
{
	Jaxer.Log.info("(confirm called for: " + message + ")");
}
overridesToApply.push(Overrides.prompt);

/**
 * Alters the built-in setter methods for various DOM FORM element prototypes to
 * alter the DOM as well as set the value of the associated in-memory property.
 * E.g., normally when you set the value of an input element, the "value"
 * attribute of the element in the DOM isn't altered. After running the function
 * below, the "value" attribute on any input element will stay in sync with its
 * in-memory value, so it will get serialized with the rest of the DOM when
 * we're ready to send the DOM to the browser.
 * 
 * @alias Jaxer.Overrides.extendDomSetters
 * @advanced
 * @param {Object} global
 * 		The global object (usually a window object) whose prototype setters are
 * 		to be overridden.
 */
Overrides.extendDomSetters = function extendDomSetters(global)
{
	if (global.HTMLInputElement) 
	{
		var inputValueSetter = global.HTMLInputElement.prototype.__lookupSetter__('value');
		global.HTMLInputElement.prototype.__defineSetter__('value', function(val)
		{
			inputValueSetter.call(this, val);
			this.setAttribute('value', val);
		});
		var inputCheckedSetter = global.HTMLInputElement.prototype.__lookupSetter__('checked');
		global.HTMLInputElement.prototype.__defineSetter__('checked', function(val)
		{
			inputCheckedSetter.call(this, val);
			if (val) 
			{
				this.setAttribute('checked', null);
			}
			else 
			{
				this.removeAttribute('checked');
			}
		});
	}
	if (global.HTMLTextAreaElement) 
	{
		var textAreaValueSetter = global.HTMLTextAreaElement.prototype.__lookupSetter__('value');
		global.HTMLTextAreaElement.prototype.__defineSetter__('value', function(val)
		{
			textAreaValueSetter.call(this, val);
			this.textContent = val;
		});
	}
	if (global.HTMLSelectElement) 
	{
		var selectSelectedIndexSetter = global.HTMLSelectElement.prototype.__lookupSetter__('selectedIndex');
		global.HTMLSelectElement.prototype.__defineSetter__('selectedIndex', function(index)
		{
			selectSelectedIndexSetter.call(this, index);
			var option = this.options[index];
			if (option) 
			{
				for (var i = 0, len = this.options.length; i < len; i++) 
				{
					if (i == index) 
					{
						this.options[i].setAttribute('selected', null);
					}
					else 
					{
						this.options[i].removeAttribute('selected');
					}
				}
			}
		});
	}
	if (global.HTMLOptionElement) 
	{
		var optionSelectedSetter = global.HTMLOptionElement.prototype.__lookupSetter__('selected');
		global.HTMLOptionElement.prototype.__defineSetter__('selected', function(val)
		{
			optionSelectedSetter.call(this, val);
			if (val) 
			{
				this.setAttribute('selected', null);
			}
			else 
			{
				this.removeAttribute('selected');
			}
		});
	}
}

/**
 * @namespace {Jaxer.Overrides.JSON} An object that exposes native functions 
 * for constructing and parsing JSON data strings.
 * A synonym for window.JSON.
 */

/**
 * Returns a JSON string representation of its argument. More options are likely
 * to be available in the future. 
 * A synonym for Jaxer.Overrides.JSON.stringify, JSON.encode, and JSON.stringify
 * 
 * @alias Jaxer.Overrides.JSON.encode
 * @param {Object} value
 * 		The value whose JSON string representation is to be returned
 * @return {String}
 * 		The JSON string representation of the value
 * @see Jaxer.Overrides.JSON.decode
 * @see Jaxer.Overrides.JSON.parse
 * @see Jaxer.Serialization.toJSONString
 */

/**
 * Returns a JSON string representation of its argument. More options are likely
 * to be available in the future. 
 * A synonym for Jaxer.Overrides.JSON.encode, JSON.encode, and JSON.stringify
 * 
 * @alias Jaxer.Overrides.JSON.stringify
 * @param {Object} value
 * 		The value whose JSON string representation is to be returned
 * @return {String}
 * 		The JSON string representation of the value
 * @see Jaxer.Overrides.JSON.decode
 * @see Jaxer.Overrides.JSON.parse
 * @see Jaxer.Serialization.toJSONString
 */

/**
 * Returns the JavaScript value represented by the given JSON string. More options are likely
 * to be available in the future. 
 * A synonym for Jaxer.Overrides.JSON.parse, JSON.decode, and JSON.parse
 * 
 * @alias Jaxer.Overrides.JSON.decode
 * @param {String} text
 * 		The JSON string describing the value to be created
 * @return {Object}
 * 		The value value represented by the JSON string
 * @see Jaxer.Overrides.JSON.encode
 * @see Jaxer.Overrides.JSON.stringify
 * @see Jaxer.Serialization.fromJSONString
 */

/**
 * Returns the JavaScript value represented by the given JSON string. More options are likely
 * to be available in the future. 
 * A synonym for Jaxer.Overrides.JSON.decode, JSON.parse, and JSON.decode
 * 
 * @alias Jaxer.Overrides.JSON.parse
 * @param {String} text
 * 		The JSON string describing the value to be created
 * @return {Object}
 * 		The value value represented by the JSON string
 * @see Jaxer.Overrides.JSON.encode
 * @see Jaxer.Overrides.JSON.stringify
 * @see Jaxer.Serialization.fromJSONString
 */

/**
 * @namespace {JSON} An object that exposes native functions 
 * for constructing and parsing JSON data strings.
 * A synonym for Jaxer.Overrides.JSON.
 */

/**
 * Returns a JSON string representation of its argument. More options are likely
 * to be available in the future. 
 * A synonym for Jaxer.Overrides.JSON.stringify, JSON.encode, and JSON.stringify
 * 
 * @alias JSON.encode
 * @param {Object} value
 * 		The value whose JSON string representation is to be returned
 * @return {String}
 * 		The JSON string representation of the value
 * @see JSON.decode
 * @see JSON.parse
 */

/**
 * Returns a JSON string representation of its argument. More options are likely
 * to be available in the future. 
 * A synonym for Jaxer.Overrides.JSON.encode, JSON.encode, and JSON.stringify
 * 
 * @alias JSON.stringify
 * @param {Object} value
 * 		The value whose JSON string representation is to be returned
 * @return {String}
 * 		The JSON string representation of the value
 * @see JSON.decode
 * @see JSON.parse
 */

/**
 * Returns the JavaScript value represented by the given JSON string. More options are likely
 * to be available in the future. 
 * A synonym for Jaxer.Overrides.JSON.parse, Jaxer.Overrides.JSON.decode, and JSON.parse
 * 
 * @alias JSON.decode
 * @param {String} text
 * 		The JSON string describing the value to be created
 * @return {Object}
 * 		The value value represented by the JSON string
 * @see JSON.encode
 * @see JSON.stringify
 */

/**
 * Returns the JavaScript value represented by the given JSON string. More options are likely
 * to be available in the future. 
 * A synonym for Jaxer.Overrides.JSON.decode, Jaxer.Overrides.JSON.parse, and JSON.decode
 * 
 * @alias JSON.parse
 * @param {String} text
 * 		The JSON string describing the value to be created
 * @return {Object}
 * 		The value value represented by the JSON string
 * @see JSON.encode
 * @see JSON.stringify
 */

var nativeJSON = Components.classes["@mozilla.org/dom/json;1"].createInstance(Components.interfaces.nsIJSON);
Overrides.JSON =
{
	  encode: nativeJSON.encode
	, decode: nativeJSON.decode
	, stringify: nativeJSON.encode
	, parse: nativeJSON.decode
}

/**
 * Applies all the overrides on the given global object, including the no-ops
 * "setTimeout" and "setInterval"
 * 
 * @alias Jaxer.Overrides.applyAll
 * @advanced
 * @param {Object} global
 * 		The global (typically window) object
 */
Overrides.applyAll = function applyAll(global)
{
	overridesToApply.forEach(function(override)
	{
		global[override.name] = override;
	});
	
	noOps.forEach(function(noOpName)
	{
		global[noOpName] = function() {};
	});
	
	if (typeof global.document == "object") 
	{
		// Have to literally define and evaluate this function on the target global
		// because of issues with cross-global setters in FF3
		Includer.evalOn(Jaxer.Overrides.extendDomSetters.toSource() + "(document.defaultView)", global);
	}
	
	if (!('JSON' in global)) global.JSON = Overrides.JSON;
}

Overrides.applyAll(frameworkGlobal);
// These will also be applied on the developer's window during a request

frameworkGlobal.Overrides = Jaxer.Overrides = Overrides;

})();
