var __loaded = false;

window.onload = function()
{
	__loaded = document.location.href;
}

function evaluate(s)
{
	return eval(s);
}

// DO NOT use any HTML markup as the consumer of this is likely to escape that out to ensure the expected-vs.-found text renders properly
function __eol()
{
	return "\n";
}

/**
 * assertMatches
 * 
 * @param {Object} expression to compare against
 * @param {Object} value to test
 * @param {String} message to show if assertion false
 */
function assertMatches(value, expression, message)
{
	var exp = new RegExp(expression);
	if (!exp.test(value))
	{
		var text = "'" + value + "' does not match expression " + expression;
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * assertEquals
 * 
 * @param {Object} expected
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
function assertEquals(value, expected, message)
{
	if (expected != value)
	{
		var text = "Expected '" + expected + "' but found '" + value + "'";
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * assertFalse
 * 
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
function assertFalse(value, message)
{
	if (value)
	{
		var text = "Expected false but found '" + value + "'";
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * assertNotEquals
 * 
 * @param {Object} expected
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
function assertNotEquals(expected, value, message)
{
	if (expected == value)
	{
		var text = "Expected different values but found '" + value + "'";
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * assertNotNull
 * 
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
function assertNotNull(value, message)
{
	if (value === null)
	{
		var text = "Expected non-null value but found '" + value + "'";
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * assertNotUndefined
 * 
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
function assertNotUndefined(value, message)
{
	if (value === undefined)
	{
		var text = "Expected defined value but found undefined";
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * assertDefined
 * 
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
var assertDefined = assertNotUndefined;

/**
 * assertNull
 * 
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
function assertNull(value, message)
{
	if (value !== null)
	{
		var text = "Expected null but found '" + value + "'";
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * assertSame
 * 
 * @param {Object} expected
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
function assertSame(value, expected, message)
{
	if (expected !== value)
	{
		var text = "Object references do not match";
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * assertTrue
 * 
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
function assertTrue(value, message)
{
	if (value == false)
	{
		var text = "Expected true but found '" + value + "'";
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * assertUndefined
 * 
 * @param {Object} value
 * @param {String} message to show if assertion false
 */
function assertUndefined(value, message)
{
	if (value !== undefined)
	{
		var text = "Expected undefined but found '" + value + "'";
		if (message)
		{
			text += __eol() + message;
		}
		throw new Error(text);
	}
}

/**
 * force a failure
 * 
 * @param {String} message
 */
function fail(message)
{
	throw new Error(message);
}
