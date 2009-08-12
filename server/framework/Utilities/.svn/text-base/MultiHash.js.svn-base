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
 * @namespace {Jaxer.Util.MultiHash} Namespace used to hold functions that create
 * and manipulate a hash whose values are primitives or Arrays of primitives
 */
Util.MultiHash = {};

function isArray(obj)
{
	return obj &&
		(obj.__parent__) && 
		(obj.constructor == obj.__parent__.Array);
}

/**
 * Adds the name-value pair to the MultiHash: if the name does not exist,
 * it is added with the given value; if the name exists, the previous value 
 * is turned into an Array, if needed, and then the given value is appended to it
 * 
 * @alias Jaxer.Util.MultiHash.add
 * @param {Object} multiHash
 * 		The multihash to which the name-value should be added
 * @param {String} name
 * 		The name to use as a key
 * @param {Object} value
 * 		The value to associate with the key -- it must not be an array
 */
Util.MultiHash.add = function add(multiHash, name, value)
{
	if (name in multiHash)
	{
		var current = multiHash[name];
		if (isArray(current)) // assumed to be an Array
		{
			current.push(value);
		}
		else
		{
			multiHash[name] = [current, value];
		}
	}
	else
	{
		multiHash[name] = value;
	}
};

/**
 * Removes the name-value pair from the MultiHash: if the name does not exist,
 * or it does not have the value, nothing happens; if the name exists and has the value,
 * that value is removed, and if no more values remain for that name, the name
 * is removed
 * 
 * @alias Jaxer.Util.MultiHash.remove
 * @param {Object} multiHash
 * 		The multihash from which the name-value should be removed
 * @param {String} name
 * 		The name of the key
 * @param {Object} value
 * 		The value to remove from the key -- it must not be an array
 */
Util.MultiHash.remove = function add(multiHash, name, value)
{
	if (name in multiHash)
	{
		var current = multiHash[name];
		if (isArray(current)) // assumed to be an Array
		{
			var indexToRemove = -1;
			for (var i=0; i<current.length; i++)
			{
				if (current[i] == value)
				{
					indexToRemove = i;
					break;
				}
			};
			if (indexToRemove > -1)
			{
				current.splice(indexToRemove, 1);
			}
			if (current.length == 1)
			{
				multiHash[name] = current[0];
			}
		}
		else if (value == current)
		{
			delete multiHash[name];
		}
	}
};

/**
 * Analyzes the first MultiHash relative to the second
 * 
 * @alias Jaxer.Util.MultiHash.diff
 * @param {Object} after
 * 		The MultiHash whose changes we are interested in
 * @param {Object} before
 * 		The MultiHash relative to which the changes have occurred
 * @return {Object}
 * 		The results of the analysis, an object with three properties: 
 * 		'added' is a hash of all the name-value pairs added;
 * 		'changed' is a hash of all the name-value pairs that have changed 
 * 		(the value is the one in after); 
 * 		and 'deleted' is a hash of all the name-value pairs that have been deleted 
 * 		(the value is the one in before, of course)
 */
Util.MultiHash.diff = function diff(after, before)
{
	var result = {added: {}, changed: {}, deleted: {}};
	for (var a in after)
	{
		if (!(a in before))
		{
			result.added[a] = after[a];
		}
		else
		{
			var changed = false;
			var valueBefore = before[a];
			var valueAfter = after[a];
			if (isArray(valueBefore))
			{
				if (isArray(valueAfter))
				{
					if (valueBefore.length != valueAfter.length)
					{
						changed = true;
					}
					else
					{
						for (var i=0; i<valueAfter.length; i++)
						{
							if (valueBefore[i] != valueAfter[i])
							{
								changed = true; 
								break;
							}
						}
					}
				}
				else
				{
					changed = true;;
				}
			}
			else if (valueBefore != valueAfter)
			{
				changed = true;
			}
			if (changed)
			{
				result.changed[a] = valueAfter;
			}
		}
	}
	for (var b in before)
	{
		if (!(b in after))
		{
			result.deleted[b] = before[b];
		}
	}
	return result;
}

})();
