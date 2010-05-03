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

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringWriter;
import java.lang.reflect.InvocationTargetException;

import org.mozilla.javascript.Context;
import org.mozilla.javascript.Function;
import org.mozilla.javascript.Script;
import org.mozilla.javascript.Scriptable;
import org.mozilla.javascript.ScriptableObject;

/**
 * @author Kevin Lindsey
 */
public class Global extends ScriptableObject
{
	/**
	 * The "location" property name
	 */
	public static final String LOCATION_PROPERTY = "location";	//$NON-NLS-1$
	
	private static final long serialVersionUID = 5683163913933637642L;

	private PrintStream _err;
	private PrintStream _out;
	private ClassLoader _classLoader;
	private boolean _server;
	
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
		
		// add classes
		try
		{
			ScriptableObject.defineClass(this, File.class);
		}
		catch (IllegalAccessException e)
		{
			e.printStackTrace();
		}
		catch (InstantiationException e)
		{
			e.printStackTrace();
		}
		catch (InvocationTargetException e)
		{
			e.printStackTrace();
		}
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
			"include",
			"loadBundle",
			"setClassLoader"
		};
	}

	/**
	 * createProperties
	 *
	 */
	private void createProperties()
	{
		// create standard error stream, cache, and add to global
		this._err = new PrintStream(this, System.err);
		this.defineProperty("err", this._err, READONLY | PERMANENT); //$NON-NLS-1$
		
		// create standard error stream, cache, and add to global
		this._out = new PrintStream(this, System.out);
		this.defineProperty("out", this._out, READONLY | PERMANENT); //$NON-NLS-1$
		
		this._classLoader = null;
		this.defineProperty("classLoader", this._classLoader, READONLY | PERMANENT); //$NON-NLS-1$
		
		this._server = true;
		this.defineProperty("server", this._server, READONLY | PERMANENT); //$NON-NLS-1$
		
		this.defineProperty("window", this, READONLY | PERMANENT); //$NON-NLS-1$
	}

	/**
	 * @see org.mozilla.javascript.ScriptableObject#getClassName()
	 */
	public String getClassName()
	{
		return "Global";
	}
	
	/**
	 * Load a script into a currently executing library
	 * 
	 * @param cx
	 *            The scripting context
	 * @param thisObj
	 *            The object that activated this function call
	 * @param args
	 *            The arguments passed to this function call
	 * @param funObj
	 *            The function object that invoked this method
	 */
	public static void include(Context cx, Scriptable thisObj, Object[] args, Function funObj)
	{
		if (args.length > 0)
		{
			String libraryName = Context.toString(args[0]);
			java.io.File library = new java.io.File(libraryName);

			if (library.exists() == false)
			{
				java.io.File self = new java.io.File((String) Main.global.get(LOCATION_PROPERTY, Main.global));
				java.io.File parent = self.getParentFile();
				String name = parent.getAbsolutePath() + java.io.File.separator + libraryName;

				library = new java.io.File(name);
			}

			if (library.exists())
			{
				try
				{
					// grab the script's source
					String source = readContent(new FileInputStream(library));

					// get script where this is executing
					Scriptable scope = Main.global;

					// compile the script
					Script script = cx.compileString(source, library.getAbsolutePath(), 1, null);

					// exec
					script.exec(cx, scope);
				}
				catch (FileNotFoundException e)
				{
				}
				catch (IOException e)
				{
				}
			}
		}
	}
	
	/**
	 * Load the specified bundle and add it to the active JS class loader
	 * 
	 * @param cx
	 *            The scripting context
	 * @param thisObj
	 *            The object that activated this function call
	 * @param args
	 *            The arguments passed to this function call
	 * @param funObj
	 *            The function object that invoked this method
	 */
	public static void loadBundle(Context cx, Scriptable thisObj, Object[] args, Function funObj)
	{
		// do nothing
	}
	
	/**
	 * setClassLoader
	 * 
	 * @param cx
	 *            The scripting context
	 * @param thisObj
	 *            The object that activated this function call
	 * @param args
	 *            The arguments passed to this function call
	 * @param funObj
	 *            The function object that invoked this method
	 * @return new class loader or class loader passed into method
	 */
	public static ClassLoader setClassLoader(Context cx, Scriptable thisObj, Object[] args, Function funObj)
	{
		// do nothing
		return null;
	}
	
	/**
	 * readContent
	 * 
	 * @param stream
	 * @return String
	 * @throws IOException
	 */
	public static String readContent(InputStream stream) throws IOException
	{
		if (stream == null)
		{
			return null;
		}

		InputStreamReader inputReader = new InputStreamReader(stream);
		BufferedReader reader = new BufferedReader(inputReader);
		StringWriter sw = new StringWriter();

		try
		{
			char[] chars = new char[1024];
			int numRead = reader.read(chars);

			while (numRead != -1)
			{
				sw.write(chars, 0, numRead);
				numRead = reader.read(chars);
			}
		}
		finally
		{
			reader.close();
		}

		String contents = sw.toString();

		return contents;
	}

}
