/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 *
 * @author Kevin Lindsey
 */

var $console;
var $eol = "<br />";
var $space = "&nbsp;";

var $fail_count;
var $success_count;
var $test_count;

/**
 * $run_tests
 */
function $run_tests()
{
	var tests = [];
	var scripts = document.getElementsByTagName("script");
	var functionName = /function\s+(test\w+)/g;
	
	for (var i = 0; i < scripts.length; i++)
	{
		var script = scripts.item(i);
		var code = script.text;
		var result = functionName.exec(code);
		
		while (result != null)
		{
			tests.push(result[1]);
			result = functionName.exec(code);
		}
	}
	
	$fail_count = 0;
	$success_count = 0;
	$test_count = 0;
	
	for (var i = 1; i <= tests.length; i++)
	{
		var name = tests[i - 1];
		
		if (window[name])
		{
			// increase test count
			$test_count++;
			
			// save current failure so we can determine if this test
			// is successful or not			
			var before_fail_count = $fail_count;
			
			var message = [
				i,
				name
			];
			
			// run test
			try
			{
				window[name]();
			}
			catch (e)
			{
				$fail_count++;
				
				message.push("failed");
				message.push((e.message) ? e.message : "");
			}
			
			// see if we failed, and if we didn't, increase success count
			if (before_fail_count == $fail_count)
			{
				$success_count++;
				message.push("passed");
				message.push("");
			}
			
			$trace(message.join("\t"));
		}
	}
	
//	$trace("Test count: " + $test_count + $eol);
//	
//	if ($test_count > 0)
//	{
//		if ($fail_count == 0)
//		{
//			$trace("All tests passed!" + $eol);
//		}
//		else
//		{
//			if ($success_count > 0)
//			{
//				$trace("Success count: " + $success_count + $eol);
//			}
//			
//			$trace("Fail count: " + $fail_count + $eol);
//		}
//	}
}

/**
 * $assertEquals
 * 
 * @param {Object} expected
 * @param {Object} value
 * @param {Object} message
 */
function $assertEquals(expected, value, message)
{
	if (expected != value)
	{
		var text = "expected '" + expected + "' but found '" + value + "'";
				
		if (message)
		{
			text += $eol + message;
		}
		
		throw new Error(text);
	}
}

/**
 * $assertFalse
 * 
 * @param {Object} value
 * @param {Object} message
 */
function $assertFalse(value, message)
{
	if (value)
	{
		var text = null;
		
		if (message)
		{
			text = $eol + message;
		}
		
		throw new Error(text);
	}
}

/**
 * $assertNotEquals
 * 
 * @param {Object} expected
 * @param {Object} value
 * @param {Object} message
 */
function $assertNotEquals(expected, value, message)
{
	if (expected == value)
	{
		var text = "expected '" + expected + "' but found '" + value + "'";
				
		if (message)
		{
			text += $eol + message;
		}
		
		throw new Error(text);
	}
}

/**
 * $assertNotNull
 * 
 * @param {Object} value
 * @param {Object} message
 */
function $assertNotNull(value, message)
{
	if (expected === null)
	{
		var text = "expected non-null value but found '" + value + "'";
				
		if (message)
		{
			text += $eol + message;
		}
		
		throw new Error(text);
	}
}

/**
 * $assertNotUndefined
 * 
 * @param {Object} value
 * @param {Object} message
 */
function $assertNotUndefined(value, message)
{
	if (expected === undefined)
	{
		var text = "expected defined value but found undefined";
				
		if (message)
		{
			text += $eol + message;
		}
		
		throw new Error(text);
	}
}

/**
 * $assertNull
 * 
 * @param {Object} value
 * @param {Object} message
 */
function $assertNull(value, message)
{
	if (value !== null)
	{
		var text = "expected null but found '" + value + "'";
				
		if (message)
		{
			text += $eol + message;
		}
		
		throw new Error(text);
	}
}

/**
 * $assertQuery
 * 
 * @param {String} query
 * @param {String} value
 */
function $assertQuery(query, value)
{
	var rs = Aptana.DB.frameworkExecute(query);
	
	$assertEquals(value, rs.singleResult);
}

/**
 * $assertSame
 * 
 * @param {Object} expected
 * @param {Object} value
 * @param {Object} message
 */
function $assertSame(expected, value, message)
{
	if (expected !== value)
	{
		var text = "object references do not match";
				
		if (message)
		{
			text += $eol + message;
		}
		
		throw new Error(text);
	}
}

/**
 * $assertTrue
 * 
 * @param {Object} value
 * @param {Object} message
 */
function $assertTrue(value, message)
{
	if (value == false)
	{
		var text = null;
		
		if (message)
		{
			text = $eol + message;
		}
		
		throw new Error(text);
	}
}

/**
 * $assertUndefined
 * 
 * @param {Object} value
 * @param {Object} message
 */
function $assertUndefined(value, message)
{
	if (expected !== undefined)
	{
		var text = "expected undefined but found '" + value + "'";
				
		if (message)
		{
			text += $eol + message;
		}
		
		throw new Error(text);
	}
}

/**
 * $trace
 * 
 * @param {Object} message
 */
function $trace(message)
{
	if (!$console)
	{
		$console = document.getElementById("output");
		
		if ($console)
		{
			$console.innerHTML = "";
		}
		else
		{
			$console = document.createElement("table");
			
			document.documentElement.body.appendChild($console);
		}
	}
	
	if ($console)
	{
		var parts = message.split(/\t/);
		var row = document.createElement("tr");
		
		for (var i = 0; i < parts.length; i++)
		{
			var part = parts[i] || $space;
			var td = document.createElement("td");
			
			td.innerHTML = part;
			row.appendChild(td);
		}
		
		$console.appendChild(row);
	}
}

// client-side support functions

/**
 * frameworkCommand
 * 
 * @param {Sring} command
 */
function frameworkCommand(command)
{
	Log.trace("framework command");
	Aptana.DB.frameworkExecute(command);
}

/**
 * frameworkQuery
 * 
 * @param {String} query
 * @return {String}
 */
function frameworkQuery(query)
{
	Log.trace("framework query");
	var rs = Aptana.DB.frameworkExecute(query);
	
	return rs.singleResult;
}

// proxy for client-side invocation
frameworkCommand.runat = "server-proxy";
frameworkQuery.runat = "server-proxy";
