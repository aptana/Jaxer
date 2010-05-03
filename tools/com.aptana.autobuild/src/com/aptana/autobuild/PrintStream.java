/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.autobuild;

import org.mozilla.javascript.Context;
import org.mozilla.javascript.Function;
import org.mozilla.javascript.Scriptable;
import org.mozilla.javascript.ScriptableObject;

/**
 * @author Kevin Lindsey
 */
public class PrintStream extends ScriptableObject
{
	private static final long serialVersionUID = 7834359584107217231L;
	private java.io.PrintStream _stream;

	/**
	 * Create a new instance of PrintStream
	 * 
	 * @param scope
	 * @param stream
	 *            The underlying stream to wrap
	 */
	public PrintStream(Scriptable scope, java.io.PrintStream stream)
	{
		super();

		this.setParentScope(scope);
		this._stream = stream;

		this.defineFunctionProperties(
			new String[] { "print", "println" }, //$NON-NLS-1$ //$NON-NLS-2$
			PrintStream.class,
			ScriptableObject.READONLY
		);
	}

	/**
	 * @see org.mozilla.javascript.ScriptableObject#getClassName()
	 */
	public String getClassName()
	{
		return "PrintStream";
	}

	/**
	 * print
	 * 
	 * @param cx
	 * @param thisObj
	 * @param args
	 * @param funObj
	 */
	public static void print(Context cx, Scriptable thisObj, Object[] args, Function funObj)
	{
		PrintStream instance = (PrintStream) thisObj;

		for (int i = 0; i < args.length; i++)
		{
			String arg = Context.toString(args[0]);

			instance._stream.print(arg);
		}
	}

	/**
	 * println
	 * 
	 * @param cx
	 * @param thisObj
	 * @param args
	 * @param funObj
	 */
	public static void println(Context cx, Scriptable thisObj, Object[] args, Function funObj)
	{
		PrintStream instance = (PrintStream) thisObj;

		for (int i = 0; i < args.length; i++)
		{
			String arg = Context.toString(args[0]);

			instance._stream.println(arg);
		}
	}
}
