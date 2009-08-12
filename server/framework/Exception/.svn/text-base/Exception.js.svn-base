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
 * @classDescription {Jaxer.Exception} Exception used by the Jaxer framework.
 */

/**
 * The exception used by the Jaxer framework. At runtime this is available from
 * the window object
 * 
 * @constructor
 * @alias Jaxer.Exception
 * @param {String, Error} info
 * 		The descriptive text of the Exception to be thrown, or an Error-derived
 * 		object
 * @param {Object} [logger]
 * 		An optional reference to an instance of the framework logger to be used.
 * 		A default setting is used if this parameter is omitted
 * @return {Jaxer.Exception}
 * 		An instance of Exception
 */
var Exception = function Exception(info, logger)
{
	this.name = "Exception";
	var infoIsError = (info instanceof Error);
	this.innerErrorName = infoIsError ? info.name : null;
	var error;
	if (infoIsError)
	{
		error = info;
	}
	else
	{
		error = new Error((info == null || info == "" || (typeof info.toString != "function")) ? "(unknown error)" : info.toString());
		if (info != null && typeof info == "object") // try to extract more information from info
		{
			for (var p in info)
			{
				error[p] = info[p];
			}
		}
	}
	var details = [];
	
	// message 
	// fileName
	// lineNumber
	// name 
	
	for (var prop in error)
	{
		var val = error[prop];
		var type = typeof val;
		if (prop != 'details' &&
			prop != 'stack' && // we show our own stack
			val != null &&
			type != "undefined" &&
			type != "function" &&
			typeof val.toString == "function")
		{
			this[prop] = error[prop];
			var v = Util.String.trim(val.toString());
			var p = '' + prop + ": ";
			var indent = p.replace(/.{1}/g, ' ');
			if (prop != 'message') 
			{
				details.push(p + v.replace(/\n/g, "\n" + indent));
			}
		}
	}
	
	var tmp_stack = Components.stack.caller;
	
	var orig_location = 'UNKNOWN';
	
	while (tmp_stack)
	{
		var orig_location = tmp_stack.filename.replace(/^.*\//, '')+':'+tmp_stack.lineNumber;
		tmp_stack = tmp_stack.caller;
	}

	this.details = 'Error : ['+orig_location+']\n'+ this.message;
	this.description = this.message;
	
	logger = logger || Log.genericLogger;

	logger.error(this.description+'\n'+this.details, null, this.constructor.caller);
};

Exception.prototype = new Error();
Exception.prototype.constructor = Exception;

/**
 * Provides a string representation of the Exception description.
 * 
 * @alias Jaxer.Exception.prototype.toString
 * @return {String}
 * 		A description of the exception
 */
Exception.prototype.toString = function toString()
{
	return this.description;
};

// Static utility functions
/**
 * Returns a JavaScript Error (or Error-derived) object based on the given
 * object
 * 
 * @alias Jaxer.Exception.toError
 * @method
 * @param {Object} obj
 * 		If this is already derived from an Error, it will just be returned.
 * 		Otherwise it will be stringified and used as the description of the
 * 		error.
 * @return {Error}
 * 		The Error-derived representation
 */
Exception.toError = function toError(obj)
{
	if (typeof obj == "undefined") 
	{
		return new Error("undefined");
	}
	else if (obj instanceof Error || obj instanceof Exception) 
	{
		return obj;
	}
	else if (typeof obj.toString == "function") 
	{
		return new Error(obj.toString());
	}
	else 
	{
		return new Error("Cannot convert error object to text message");
	}
};

var detailProperties = ["message", "innerErrorName", "fileName", "lineNumber", "stack"];

/**
 * Get verbose details on the error
 * 
 * @alias Jaxer.Exception.toDetails
 * @param {Object} obj
 * 		The error object on which details are desired. An attempt is made to
 * 		convert it into an Error-derived object before details are retrieved.
 * @return {String}
 * 		The detailed description
 */
Exception.toDetails = function toDetails(obj)
{
	var error = Exception.toError(obj);
	var details = [];
	detailProperties.forEach(function(prop)
	{
		if (typeof error[prop] != "undefined")
		{
			var val = error[prop];
			if (val != null && val != "")
			{
				details.push([prop + ": ", val].join(prop == "stack" ? "\n" : ""));
			}
		}
	});
	return details.join("\n");
}

frameworkGlobal.Exception = Jaxer.Exception = Exception;

Jaxer.Log.trace("*** Exception.js loaded");

})();
