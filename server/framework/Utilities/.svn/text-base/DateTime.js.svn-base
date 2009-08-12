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
 * @namespace {Jaxer.Util.DateTime} Namespace used to hold functions and other objects that extend JavaScript's datetime handling.
 */
Util.DateTime = {};

/**
 * Converts a date to a string and pads the month and date values to align
 * all date values in columns. Not yet internationalized.
 * 
 * @alias Jaxer.Util.DateTime.toPaddedString
 * @param {Date} date
 * 		The source date
 * @return {String}
 * 		The source data converted to a string with month and data values padded
 * 		with spaces to align all values
 */
Util.DateTime.toPaddedString = function toPaddedString(date)
{
	var m = date.getMonth();
	var d = date.getDate();
	var sep = "/";
	return date.toTimeString().substr(0, 8) + " " + (m < 10 ? " " : "") + m + sep + (d < 10 ? " " : "") + d + sep + date.getFullYear();
};

})();
