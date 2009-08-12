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
 * @namespace {Jaxer.Util.Math} Namespace used to hold functions and other objects that extend JavaScript's math capabilities 
 */
Util.Math = {};

/**
 * Determine whether the specified value is an integer value
 * 
 * @alias Jaxer.Util.Math.isInteger
 * @param {Number} num
 * 		The number to test
 * @return {Boolean}
 * 		Returns true if the number is an integer value
 */
Util.Math.isInteger = function isInteger(num)
{
	return (typeof num == "number") &&
		isFinite(num) && 
		(Math.floor(num) == num);
};

function _forceInteger(num)
{
	if (typeof num == "string") num = parseInt(num); // string -> integer number or NaN
	if (typeof num == "number") num = Math.floor(num); // number -> integer number or NaN
	if ((typeof num != "number") || !isFinite(num)) num = null; // anything else or NaN -> null
	return num;
}

/**
 * Forces num into a finite integer. If it's a string, it first attempts to parse it to an integer.
 * If it's a number, it takes its integer part by applying Math.floor() to it. 
 * If it's anything else, o NaN (not a number), it uses the defaultNum or 0.
 * 
 * @alias Jaxer.Util.Math.forceInteger
 * @param {Object} num
 * 		The object to turn into an integer
 * @param {Object} defaultNum
 * 		The integer to use as a default (which will itself be forced to be 0 if not an integer)
 */
Util.Math.forceInteger = function forceInteger(num, defaultNum)
{
	num = _forceInteger(num);
	if (num == null) num = _forceInteger(defaultNum) || 0;
	return num;
}

})();
