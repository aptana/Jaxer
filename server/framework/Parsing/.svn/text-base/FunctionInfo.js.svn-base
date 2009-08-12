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

var log = Log.forModule("FunctionInfo");

// private constants
var INVOKE_NAME = "remote";

/**
 * @classDescription {Jaxer.FunctionInfo} Encapsulates function information
 * needed for generating client-side proxies and for server-side storage of
 * functions used in callbacks.
 */

/**
 * This class encapsulates function information needed for generating
 * client-side proxies and for server-side storage of functions used in
 * callbacks.
 * 
 * @constructor
 * @alias Jaxer.FunctionInfo
 * @param {String} functionName
 * 		This parameter is the name of the function this info represents.
 * @param {String[]} parameterNames
 * 		This parameter is an array of parameter names for the function this
 * 		info represents.
 * @param {String} source
 * 		This parameter is the actual source code of the function this info
 * 		represents.
 * @return {Jaxer.FunctionInfo}
 * 		Returns an instance of FunctionInfo.
 */

/**
 * This class encapsulates function information needed for generating
 * client-side proxies and for server-side storage of functions used in
 * callbacks.
 * 
 * @advanced
 * @constructor
 * @alias Jaxer.FunctionInfo
 * @param {Function} functionReference
 * 		This is a reference to the actual Javascript function instance this
 * 		info represents.
 * @param {Node} [functionNode]
 * 		This is an optional parameter that is the root node of an AST
 * 		representing the function this info represents.
 * @return {Jaxer.FunctionInfo}
 * 		Returns an instance of FunctionInfo.
 */
function FunctionInfo(functionReference, functionNode)
{
	if (arguments.length == 3)
	{
		this.name = arguments[0];
		this.parameterNames = arguments[1];
		this.source = arguments[2];
		this.prototypeProperties = [];
	}
	else
	{
		if (functionNode === undefined)
		{
			var source = functionReference.toString();
			var ast = parse(source, "<none>", 1);
			
			functionNode = ast.funDecls[0];
		}
		
		this.name = functionNode.name;
		this.parameterNames = functionNode.params;
		this.source = functionReference.toString();
		this.prototypeProperties = [];
		
		if (functionReference.hasOwnProperty("prototype"))
		{
			var proto = functionReference.prototype;
			var properties = [];
			
			for (var p in proto)
			{
				// make sure this is a local property only and a function
				if (proto.hasOwnProperty(p) && typeof(proto[p]) == "function")
				{
					properties.push(p);
				}
			}
			
			var self = this;
			
			properties.sort().forEach(function(p)
			{
				var ref = proto[p];
				var source = p + " = " + ref.toString();
				var ast = parse(source, "<none>", 1);
				var node = ast[0].expression[1];
				
				node.name = p;
				
				self.prototypeProperties.push(new FunctionInfo(ref, node));
			});
		}
	}
	this.removeFromClient = null;
	this.removeFromServer = null;
}

/**
 * Is the underlying function a native one (for which no actual source is
 * available)?
 * 
 * @advanced
 * @alias Jaxer.FunctionInfo.prototype.isNative
 * @return {Boolean}
 */
FunctionInfo.prototype.isNative = function isNative()
{
	return Util.isNativeFunctionSource(this.source);
}

/**
 * Create a string representation of the underlying function to be used
 * client-side as the source of this function.
 * 
 * @advanced
 * @alias Jaxer.FunctionInfo.prototype.createClientSource
 * @return {String}
 * 		Returns a string representation of this function info's underlying
 * 		Javascript function in a form needed to execute the function on the
 * 		client
 */
FunctionInfo.prototype.createClientSource = function()
{
	var buffer = [];
	var parentName = this.name;
	
	buffer.push(this.source);
	
	this.prototypeProperties.forEach(function(f)
	{
		buffer.push([parentName, ".prototype.", f.name, " = ", f.createClientSource(), ";"].join(""));
	});
	
	return buffer.join("\n");
};

/**
 * Create a string representation of the underlying function to be used
 * during a callback. This source will be stored in the callback database and
 * will be used to reconstitute the function during a callback. This is used
 * for cached and proxied functions.
 * 
 * @advanced
 * @alias Jaxer.FunctionInfo.prototype.createServerFunction
 * @param {String} [namespace]
 * 		If specified, the function is specified as a property on the given
 * 		namespace object. Otherwise, the function is specified as a global
 * 		property.
 * @param {Boolean} [proxied]
 * 		If specified and true, the function will have a property called "proxy"
 * 		and set to true.
 * @param {Boolean} [noLocalScope]
 * 		If specified and true, the function will NOT be assigned to a global
 * 		property with its name. That is, this.<name> will NOT be created.
 * @return {String}
 * 		Returns a string representation of this function info's underlying
 * 		Javascript function in a form needed to execute the function on the
 * 		server.
 */
FunctionInfo.prototype.createServerFunction = function(namespace, proxied, noLocalScope)
{
	var sources = [];
	
	var assignments = [];
	var refString = (namespace ? (namespace + ".") : "") + this.name;
	assignments.push(refString);
	if (!noLocalScope)
	{
		assignments.push("this." + this.name);
	}
	assignments.push(this.source);
	
	sources.push(assignments.join(" = ") + ";"); // main assignments of function body to properties
	if (proxied)
	{
		sources.push(refString + ".proxy = true;")
	}
	
	// add prototype methods
	this.prototypeProperties.forEach(function(f)
	{
		sources.push(refString + ".prototype." + f.name + " = " + f.source + ";");
	});
	
	return sources.join("\n");
};

frameworkGlobal.FunctionInfo = Jaxer.FunctionInfo = FunctionInfo;

Log.trace("*** FunctionInfo.js loaded");

})();
