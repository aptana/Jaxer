/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests;

import org.mozilla.javascript.Context;
import org.mozilla.javascript.Function;
import org.mozilla.javascript.Scriptable;
import org.mozilla.javascript.ScriptableObject;

/**
 * @author Kevin Lindsey
 */
public class Global extends ScriptableObject
{
	private static final long serialVersionUID = 5683163913933637642L;
	
	/**
	 * Global
	 * 
	 * @param context
	 */
	public Global(Context context)
	{
		// create unsealed standard objects
		context.initStandardObjects(this, false);
	
		// create global properties
		this.createAllProperties();
	}

	/**
	 * createAllProperties
	 *
	 */
	private void createAllProperties()
	{
		this.createProperties();
		this.createFunctionProperties();
	}

	/**
	 * createFunctionProperties
	 *
	 */
	private void createFunctionProperties()
	{
		String[] propertyNames = this.getFunctionPropertyNames();

		if (propertyNames != null)
		{
			this.defineFunctionProperties(propertyNames, this.getClass(), READONLY | PERMANENT);
		}
	}

	/**
	 * getFunctionPropertyNames
	 *
	 * @return String[] or null
	 */
	private String[] getFunctionPropertyNames()
	{
		return new String[] {
			"alert",
			"assertFalse",
			"assertTrue"
		};
	}

	/**
	 * createProperties
	 *
	 */
	private void createProperties()
	{
		this.put("window", this, this);
	}

	/**
	 * @see org.mozilla.javascript.ScriptableObject#getClassName()
	 */
	public String getClassName()
	{
		return "Global";
	}

	/**
	 * alert
	 * 
	 * @param cx
	 * @param thisObj
	 * @param args
	 * @param funObj
	 */
	public static void alert(Context cx, Scriptable thisObj, Object[] args, Function funObj)
	{
		for (int i = 0; i < args.length; i++)
		{
			String text = Context.toString(args[i]);
			
			System.out.println(text);
		}
	}
	
	/**
	 * assertFalse
	 * 
	 * @param cx
	 * @param thisObj
	 * @param args
	 * @param funObj
	 */
	public static void assertFalse(Context cx, Scriptable thisObj, Object[] args, Function funObj)
	{
		if (args.length > 0)
		{
			boolean result = Context.toBoolean(args[0]);
			
			if (args.length > 1)
			{
				String message = Context.toString(args[1]);
				
				junit.framework.TestCase.assertFalse(message, result);
			}
			else
			{
				junit.framework.TestCase.assertFalse(result);
			}
		}
	}
	
	/**
	 * assertTrue
	 * 
	 * @param cx
	 * @param thisObj
	 * @param args
	 * @param funObj
	 */
	public static void assertTrue(Context cx, Scriptable thisObj, Object[] args, Function funObj)
	{
		if (args.length > 0)
		{
			boolean result = Context.toBoolean(args[0]);
			
			if (args.length > 1)
			{
				String message = Context.toString(args[1]);
				
				junit.framework.TestCase.assertTrue(message, result);
			}
			else
			{
				junit.framework.TestCase.assertTrue(result);
			}
		}
	}
}
