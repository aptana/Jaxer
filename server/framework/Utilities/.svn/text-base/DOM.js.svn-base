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
 * @namespace {Jaxer.Util.DOM} Namespace used to hold functions and other
 * objects that extend JavaScript's DOM capabilities.
 */
Util.DOM = {};

/**
 * Convert an object's properties and property values into a string of
 * attributes suitable for use in creating a string representation of an
 * HTMLElement. Each property is used as the attribute name and each property
 * value becomes the attribute value. Attribute values are surrounding in
 * double-quotes and all property values containing double-quotes will have
 * those characters escaped with backslashes.
 * 
 * @alias Jaxer.Util.DOM.hashToAttributesString
 * @param {Object} hash
 * 		The source object
 * @return {String}
 * 		The resulting string of attribute name/value pairs
 */
Util.DOM.hashToAttributesString = function hashToAttributesString(hash)
{
	var result = [];
	
	for (var p in hash)
	{
		var value = hash[p].replace(/"/g, '\\"');
		
		result.push(p + "=\"" + value + "\"");
	}
	
	return result.join(" ");
};

/**
 * Convert an array into a string where each item is separated by a newline. If
 * the specfied item is not an Array, then the value itself will be returned.
 * 
 * @private
 * @param {Object} contents
 * 		The item to process
 * @return {Object}
 * 		The resulting string value if the "contents" item was an Array;
 * 		otherwise this returns the "contents" item itself
 */
function prepareContents(contents)
{
	return (contents instanceof Array) ? "\n" + contents.join("\n") + "\n" : contents;
}

/**
 * Create a new script element with the specified content and attributes
 * 
 * @alias Jaxer.Util.DOM.createScript
 * @param {Document} doc
 * 		The DocumentElement to use when creating elements for the active
 * 		document.
 * @param {String} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values.
 * 		If this is omitted, a default type attribute will be used
 * 		if set in Jaxer.Config.DEFAULT_EMITTED_SCRIPT_TYPE
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.createScript = function createScript(doc, contents, attributes)
{
	var script = doc.createElement("script");
	
	if (attributes)
	{
		for (var p in attributes)
		{
			if ((p != "type") || attributes[p]) // allow a way to specify no type attribute: just specify an empty type attribute
			{
				script.setAttribute(p, attributes[p]);
			}
		}
	}
	if (Config.DEFAULT_EMITTED_SCRIPT_TYPE && 		// the default is to emit a type
		(!attributes || !('type' in attributes)))	// and no other type directive was specified
	{
		script.setAttribute("type", Config.DEFAULT_EMITTED_SCRIPT_TYPE);
	}
	script.innerHTML = prepareContents(contents);
	
	return script;
};

/**
 * Creates a new script element and adds it before a specified element in the
 * DOM.
 * 
 * @alias Jaxer.Util.DOM.insertScriptBefore
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element in the DOM before which the new script element will be
 * 		inserted
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.insertScriptBefore = function insertScriptBefore(contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var doc = elt.ownerDocument;
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.parentNode.insertBefore(script, elt);
	
	return script;
};

/**
 * Creates a new script element and adds it as the first child of a specified
 * element in the DOM.
 * 
 * @alias Jaxer.Util.DOM.insertScriptAtBeginning
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element in the DOM where the new script element will be added as the
 * 		element's first child.
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.insertScriptAtBeginning = function insertScriptAtBeginning(contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var doc = elt.ownerDocument;
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.insertBefore(script, elt.firstChild);
	elt.insertBefore(doc.createTextNode("\n\t\t"), script);
	
	return script;
};

/**
 * Creates a new script element and adds it as the next sibling of the specified
 * element in the DOM.
 * 
 * @alias Jaxer.Util.DOM.insertScriptAfter
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element in the DOM after which the new script element will be
 * 		inserted
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.insertScriptAfter = function insertScriptAfter(contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var doc = elt.ownerDocument;
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.parentNode.insertBefore(script, elt.nextSibling);
	
	return script;
};

/**
 * Creates a new script element and adds it as the last child of a specified
 * element in the DOM.
 * 
 * @alias Jaxer.Util.DOM.insertScriptAtEnd
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element in the DOM where the new script element will be added as the
 * 		element's last child.
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.insertScriptAtEnd = function insertScriptAtEnd(contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var doc = elt.ownerDocument;
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.appendChild(doc.createTextNode("\n\t\t"));
	elt.appendChild(script);
	
	return script;
};

/**
 * Replace a specified element in the DOM with a new script element.
 * 
 * @alias Jaxer.Util.DOM.replaceWithScript
 * @param {Object} contents
 * 		The text content for the script element. This value will be set via
 * 		innerHTML once the new script element has been created
 * @param {Node} elt
 * 		The element to replace with the a script element
 * @param {Object} [attributes]
 * 		A list of attributes and attribute values to apply to the new
 * 		ScriptElement. Each property name will become the attribute name and
 * 		each property value will become that attributes value. Note that no
 * 		escaping is done on the attributes values, so it is expected those
 * 		values are valid attribute values
 * @return {ScriptElement}
 * 		The new script element with content and attributes applied.
 */
Util.DOM.replaceWithScript = function replaceWithScript(contents, elt, attributes)
{
	contents = prepareContents(contents);
	
	var doc = elt.ownerDocument;
	var script = Util.DOM.createScript(doc, contents, attributes);
	
	elt.parentNode.replaceChild(script, elt);
	
	return script;
};

/**
 * Inserts (and then parses/executes) the given HTML contents in the document
 * of the given element.
 * 
 * @param {String} contents
 * 		The HTML to insert into the document
 * @param {Node} elt
 * 		The element that marks where the HTML is to be inserted
 * @param {Number} [beforeReplaceAfter]
 * 		If this is < 0, insert the HTML before the element;
 * 		if this is > 0, insert the HTML after the element;
 * 		if this is 0, insert the HTML instead of the element.
 */
Util.DOM.insertHTML = function insertHTML(contents, elt, beforeReplaceAfter)
{
	if (typeof beforeReplaceAfter != "number") beforeReplaceAfter = 0;
	var doc = elt.ownerDocument;
	var range = doc.createRange();
	var parsed = range.createContextualFragment(contents);
	if (beforeReplaceAfter > 0)
	{
		range.setStartAfter(elt);
		elt.parentNode.insertBefore(parsed, elt.nextSibling);
	}
	else
	{
		range.setStartBefore(elt);
		elt.parentNode.insertBefore(parsed, elt);
	}
	if (beforeReplaceAfter == 0) elt.parentNode.removeChild(elt);
}

/**
 * Returns the serialized HTML of the given node and its contents, much like the
 * outerHTML property in IE. You can also pass the entire document as the node
 * to get the entire HTML of the document (page).
 * 
 * @alias Jaxer.Util.DOM.toHTML
 * @param {Node|Document} node
 * 		The HTML element or document (or DOM node) to be serialized
 * @return {String}
 * 		The HTML text
 */
Util.DOM.toHTML = function toHTML(node)
{
	var nodeIsDoc;
	var doc = node.ownerDocument; // ownerDocument is null iff ownerDocument is the document itself
	nodeIsDoc = (doc == null);
	if (nodeIsDoc) doc = node;
	var encoder = getDocumentEncoder(doc);
	if (!nodeIsDoc) encoder.setNode(node);
	return encoder.encodeToString();
}

function getDocumentEncoder(doc)
{
	const nsIDocumentEncoder = Components.interfaces.nsIDocumentEncoder;
	
	var contentType = doc.contentType;
	var documentEncoder = Components.classes["@mozilla.org/layout/documentEncoder;1?type=" + contentType].createInstance(nsIDocumentEncoder);
	documentEncoder.init(doc, contentType, nsIDocumentEncoder.OutputEncodeHTMLEntities | nsIDocumentEncoder.OutputRaw); /* these flags are compatible with Jaxer page serialization */
	documentEncoder.setCharset(doc.characterSet);
	return documentEncoder;
}

})();
