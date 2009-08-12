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
 * @namespace {Jaxer.Util.Stopwatch} The namespace that holds functions timing javascript
 */

Util.Stopwatch = {};

/**
 * A hashmap keyed by timer label, each element contains an Array of timing measurements.
 * @alias Jaxer.Util.Stopwatch.timings
 * @property {Object}
 */
Util.Stopwatch.timings  = {};

/**
 * A hashmap keyed by timer label, each element contains the timestamp last set.
 * @alias Jaxer.Util.Stopwatch.clocks
 * @property {Object}
 */
Util.Stopwatch.clocks 	= {};

/**
 * A hashmap keyed by lap label, each element contains the timestamp last set.
 * @alias Jaxer.Util.Stopwatch.laps
 * @property {Object}
 */
Util.Stopwatch.laps 	= {};

/**
 * Reset all the currently managed timers
 * @alias Jaxer.Util.Stopwatch.reset
 */
Util.Stopwatch.reset = function reset()
{
	Util.Stopwatch.timings 	= {};
	Util.Stopwatch.clocks 	= {};
};

/**
 * Start a timer for the provided label
 * @alias Jaxer.Util.Stopwatch.start
 * @param {String} label
 * 		A label to uniquely identify this measurement timer
 */
Util.Stopwatch.start = function start(label)
{
	label = label || 'unknownTimer';
	Util.Stopwatch.clocks[label] = (new Date()).getTime();
};

/**
 * Stops the current timer and stores the result for later analysis
 * @alias Jaxer.Util.Stopwatch.stop
 * @param {String} label
 * 		A label to uniquely identify this measurement timer
*/
Util.Stopwatch.stop = function stop(label)
{
	label = label || 'unknownTimer';
	if (!Util.Stopwatch.timings.hasOwnProperty(label)) 
	{
		Util.Stopwatch.timings[label] = [ (new Date()).getTime() -  Util.Stopwatch.clocks[label] ];
	} 
	else
	{
		Util.Stopwatch.timings[label].push((new Date()).getTime() -  Util.Stopwatch.clocks[label]);
	}
	delete Util.Stopwatch.clocks[label];
};

/**
 * A lap timer which will store the delta between each invocation for later analysis
 * @alias Jaxer.Util.Stopwatch.lap
 * @param {String} label
 * 		A label to uniquely identify this measurement timer
 */
Util.Stopwatch.lap = function lap(label)
{
	label = label || 'unknownLapTimer';

	if (Util.Stopwatch.timings.hasOwnProperty(label)) 
	{			
		Util.Stopwatch.timings[label].push( (new Date()).getTime() -  Util.Stopwatch.laps[label] );
	}
	else
	{
		Util.Stopwatch.timings[label] = [];
	}
	Util.Stopwatch.laps[label] = (new Date()).getTime();
};

/** 
 * This method is invoked to analyze/save/display the currently stored measurement timers.
 * 
 * If no function parameter is provided then contents of the measurement timers are written to the logfile.
 * If a function is provided then it is invoked with the timings hashmap (Jaxer.Util.Stopwatch.timings) as the only parameter.
 * 
 * @alias Jaxer.Util.Stopwatch.flush
 * @param {Function} [fn]
 * 		The provided function is executed before all current timers are cleared.  
 * 		The function is provided the timings hashmap (Jaxer.Util.Stopwatch.timings) as the only parameter.
 */
Util.Stopwatch.flush = function flush(fn)
{
	fn = fn || function(tm)
	{
		for (var label in tm) 
		{
			Jaxer.Log.info(label + ":" + tm[label]);
		};
	}
	fn(Util.Stopwatch.timings);
	Util.Stopwatch.reset();
};

/**
 * This method returns the value of the lapcounter for the provided label.
 * 
 * @alias Jaxer.Util.Stopwatch.lapCount
 * @param {String} label
 * 		A label to uniquely identify this measurement timer
 */
Util.Stopwatch.lapCount = function lapCount(label)
{
	label = label || 'unknownLapTimer';
	return (Util.Stopwatch.timings[label]) ? Util.Stopwatch.timings[label].length : 0;
}

// Log.trace("*** Stopwatch.js loaded");

})();

