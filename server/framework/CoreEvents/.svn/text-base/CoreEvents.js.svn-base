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

var log = Log.forModule("CoreEvents");

/**
 * @namespace {Jaxer.CoreEvents} A namespace object holding functions and
 * members used to handle the events fired from the Jaxer Core into the Jaxer
 * Framework.
 */
var CoreEvents = {};

// private variables
var handlers = {};
var eventNames = ["SERVER_START", "REQUEST_START", "PARSE_START", "NEW_ELEMENT", "INIT_HEAD", "PARSE_COMPLETE", "REQUEST_COMPLETE", "HANDLE_PAGE", "HANDLE_CALLBACK", "HANDLE_SERVICE"];
for (var i=0; i<eventNames.length; i++)
{
	var eventName = eventNames[i];
	CoreEvents[eventName] = eventName;
	log.trace("Defining CoreEvents." + eventName);
	handlers[eventName] = [];
}

/**
 * Returns a (copy of the) array of framework event names to which you can
 * register handlers
 * 
 * @advanced
 * @alias Jaxer.CoreEvents.getEventNames
 * @return {Array}
 * 		n array of handler names (modifying it has no impact on CoreEvents)
 */
CoreEvents.getEventNames = function getEventNames()
{
	return eventNames.concat(); // returns a copy so the original is not externally modifiable
}

/**
 * Bind a handler to the specified eventType.
 * 
 * @advanced
 * @alias Jaxer.CoreEvents.addHandler
 * @param {String} eventType
 * 		One of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 * @param {Function} handlerFunction
 * 		A function reference invoked when the event specified by eventType is triggered
 * @param {Function} [testFunction]
 * 		An optional function to be evaluated when the event is about to be
 * 		fired; it'll only be fired if the function evaluates to true. The one
 * 		argument passed to this function is the request (same as Jaxer.request)
 * @param {Object} [handlerFunctionObject]
 * 		Optional object on which to call the handlerFunction (it becomes the
 * 		value of 'this' within the function). 
 */
CoreEvents.addHandler = function addHandler(eventType, handlerFunction, testFunction, handlerFunctionObject)
{
	if (handlers.hasOwnProperty(eventType) == false) 
	{
		throw new Exception("Attempted to add a handler to an eventType for which there are no handler queues: " + eventType, log);
	}

	var handler = { handler: handlerFunction };
	handler.test = (testFunction ? testFunction : null);
	handler.object = (handlerFunctionObject ? handlerFunctionObject : null);
	
	log.trace("Adding a handler to " + eventType + ": " + handler.handler.toString().substr(0, 60) + "..."); // This may be verbose!
	handlers[eventType].push(handler);
	log.trace("Finished adding a handler");
};

/**
 * Remove an existing handler on an event type
 * 
 * @advanced
 * @alias Jaxer.CoreEvents.removeHandler
 * @param {String} eventType
 * 		One of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 * @param {Function} handlerFunction
 * 		The handler to remove
 */
CoreEvents.removeHandler = function removeHandler(eventType, handlerFunction)
{
	if (handlers.hasOwnProperty(eventType) == false) 
	{
		throw new Exception("Attempted to add a handler to an eventType for which there are no handler queues: " + eventType, log);
	}
	log.trace("Removing a handler for " + eventType + ": " + handlerFunction.toString().substr(0, 60) + "..."); // This may be verbose!
	var removed = false;
	for (var i=handlers.length-1; i>=0; i--)
	{
		var handler = handlers[i].handler;
		if (handler == handlerFunction)
		{
			handlers.splice(i, 1); // remove the handler
			removed = true;
			break;
		}
	}
	if (removed)
	{
		log.warn("Did not find the handler to remove");
	}
	else
	{
		log.trace("Finished removing a handler");
	}
}

/**
 * Returns an array of handlers for the specified eventType.
 * 
 * @advanced
 * @alias Jaxer.CoreEvents.getHandlers
 * @param {String} eventType
 * 		One of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 * @return {Array}
 * 		An array of handlers
 */
CoreEvents.getHandlers = function getHandlers(eventType)
{
	if (handlers.hasOwnProperty(eventType) == false)
	{
		throw new Exception("Invalid event queue specified: " + eventType, log);
	}
	
	return handlers[eventType];
};

/**
 * Removes the handlers for the specified eventType. 
 * 
 * @advanced
 * @alias Jaxer.CoreEvents.clearHandlers
 * @param {String} eventType
 * 		One of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 */
CoreEvents.clearHandlers = function clearHandlers(eventType)
{
	var handlersForType = CoreEvents.getHandlers(eventType);
	
	handlersForType = [];
};

/**
 * Triggers the handlers for the specified eventType.

 * @advanced
 * @alias Jaxer.CoreEvents.fire
 * @param {String} eventType
 * 		One of the allowed event types (see Jaxer.CoreEvents.getEventNames)
 */
CoreEvents.fire = function fire(eventType)
{
	try 
	{
		if (Jaxer.fatalError &&
			eventType != CoreEvents.PARSE_START &&
			eventType != CoreEvents.PARSE_COMPLETE) // only these special events can deal with fatal errors meaningfully - TODO: verify this
		{
			Jaxer.notifyFatal("Aborting " + eventType + " due to Jaxer.fatalError: " + Jaxer.fatalError);
			return;
		}
		
		var evt = arguments[1];
		
		if (eventType == CoreEvents.REQUEST_START) 
		{
			Jaxer.lastScriptRunat = "";
		}
		
		if (log.getLevel() == Log.TRACE)
		{
			var msg = "At this time, ";
			if (evt.Request) msg += "evt.Request.isHandler=" + evt.Request.isHandler + " and ";
			msg += "eventType: " + eventType + (((eventType == CoreEvents.NEW_ELEMENT) && evt.Element) ? ' - ' + evt.Element.tagName : '');
			log.trace(msg);
		}
		
		var args = new Array(arguments.length - 1);
		for (var i = 1; i < arguments.length; i++) 
		{
			args[i - 1] = arguments[i];
		}
		
		if (eventType == CoreEvents.PARSE_COMPLETE)
		{
			if (Jaxer.request.isHandler) 
			{
				if (Jaxer.request.isCallback) 
				{
					fireEvent(CoreEvents.HANDLE_CALLBACK, args);
				}
				else if (Jaxer.request.app.handler)
				{
					fireEvent(CoreEvents.HANDLE_SERVICE, args);
				}
				else
				{
					Jaxer.response.notifyError("Jaxer is the handler for this request but it's not a callback or other recognized request type.\nJaxer.request: " + uneval(Jaxer.request));
					return;
				}
			}
			else 
			{
				fireEvent(CoreEvents.HANDLE_PAGE, args);
			}
		}
		
		fireEvent(eventType, args);

	} 
	catch (e) 
	{
		var eventName = (eventType == CoreEvents.NEW_ELEMENT && args.length > 2 && args[2].tagName) ? (CoreEvents.NEW_ELEMENT + " (" + args[2].tagName + ")") : String(eventType);
		var message = "Error during framework event " + eventName + ": " + e;
		if (typeof e.stack != "undefined")
		{
			message += "\n" + e.stack;
		}
		if (eventType == CoreEvents.SERVER_START || (!Jaxer.request)) 
		{
			Jaxer.notifyFatal(message);
		}
		else 
		{
			Jaxer.response.notifyError(message);
		}
	}
		
};

//var time;

function fireEvent(eventType, args)
{
//		switch (eventType)
//		{
//			case CoreEvents.REQUEST_START:
//				time = new Date().getTime();
//				break;
//			case CoreEvents.REQUEST_COMPLETE:
//				Jaxer.Log.info("Took: " + ((new Date().getTime()) - time) + " ms");
//				break;
//		}

		if (!handlers.hasOwnProperty(eventType)) 
		{
			throw new Exception("Attempted to fire an event for which there are no handler queues: " + eventType, log);
		}
		
		var handlersForType = CoreEvents.getHandlers(eventType);
		
		log.trace("Firing an event for " + eventType);
		
		for (var i = 0; i < handlersForType.length; i++) 
		{
			var handler = handlersForType[i].handler;
			var test = handlersForType[i].test;
			var object = handlersForType[i].object;
			
			if (!test || test(Jaxer.request)) 
			{
				log.trace("Calling event handler: " + handler.toString().substr(0, 60) + "..."); // This may be verbose!
				handler.apply(object, args);
			}
		}
}

/**
 * Should the given script element be evaluated (server-side) by Jaxer Core
 * rather than be handled separately by the Jaxer framework or be ignored by Jaxer
 * as far as evaluation goes. Note that if the script element has a src attribute
 * then the framework will inline a call to load the script from the src, and
 * that's why this method returns a true in that case.
 * 
 * @advanced
 * @alias Jaxer.CoreEvents.isScriptEvaluateEnabled
 * @param {Object} scriptElement
 * 		The DOM script element
 * @return {Boolean}
 * 		true if it should be evaluated, false if it should not
 */
CoreEvents.isScriptEvaluateEnabled = function isScriptEvaluateEnabled(scriptElement)
{
	var runat = scriptElement.getAttribute(RUNAT_ATTR);
	return (
		runat &&
		runat.match(RUNAT_ANY_SERVER_REGEXP)
		);
}

/**
 * Should the given script element be loaded and evaluated (server-side) by the Jaxer framework
 * 
 * @advanced
 * @alias Jaxer.CoreEvents.isScriptLoadEnabled
 * @param {Object} scriptElement
 * 		The DOM script element
 * @return {Boolean}
 * 		true if it should be loaded and valuated, false if it should not
 */
CoreEvents.isScriptLoadEnabled = function isScriptLoadEnabled(scriptElement)
{
	var runat = scriptElement.getAttribute(RUNAT_ATTR);
	var hasSrc = scriptElement.hasAttribute(SRC_ATTR);
	return (
		runat &&
		runat.match(RUNAT_ANY_SERVER_REGEXP) &&
		hasSrc);
}

frameworkGlobal.CoreEvents = Jaxer.CoreEvents = CoreEvents;

Jaxer.Log.trace("*** CoreEvents.js loaded");
	
})();
