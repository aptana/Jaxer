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
 * @classDescription {Jaxer.Log.Level} Logging level object used by the Logging
 * facility to set or determine the current log levels.
 */

/**
 * The constructor of a logging level object.Messages must exceed a certain
 * severity level before they are logged.
 * 
 * @constructor
 * @alias Jaxer.Log.Level
 * @param {String} name
 * 		The name of the level
 * @param {Number} value
 * 		The numeric value to associate with this level. Higher numbers are more
 * 		severe.
 * @return {Jaxer.Log.Level}
 * 		Returns an instance of Level.
 */
function Level(name, value)
{
	this.name = name.toUpperCase();
	this.value = value;
}

/**
 * The textual representation of a level, namely its name
 * 
 * @alias Jaxer.Log.Level.prototype.toString
 * @return {String} The name
 */
Level.prototype.toString = function toString()
{
	return this.name;
}

/**
 * A common comparison operator on Jaxer.Log.Level objects: is the current level
 * at or above the given level?
 * 
 * @alias Jaxer.Log.Level.prototype.isAtLeast
 * @return {Boolean}
 * 		true if matches or exceeds the given level
 */
Level.prototype.isAtLeast = function isAtLeast(otherLevel)
{
	return this.value >= otherLevel.value;
}

/**
 * A common comparison operator on Jaxer.Log.Level objects: is the current level
 * at or below the given level?
 * 
 * @alias Jaxer.Log.Level.prototype.isAtOrBelow
 * @return {Boolean}
 * 		true if matches or falls below the given level
 */
Level.prototype.isAtOrBelow = function isAtOrBelow(otherLevel)
{
	return this.value <= otherLevel.value;
}

/**
 * A common comparison operator on Jaxer.Log.Level objects: is the current level
 * below the given level?
 * 
 * @alias Jaxer.Log.Level.prototype.isBelow
 * @return {Boolean}
 * 		true if falls below the given level
 */
Level.prototype.isBelow = function isBelow(otherLevel)
{
	return this.value < otherLevel.value;
}

Log.Level = Level;

})();

