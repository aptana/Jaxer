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

import java.io.File;
import java.io.IOException;

import junit.framework.TestCase;

import org.mozilla.javascript.Context;
import org.mozilla.javascript.Function;
import org.mozilla.javascript.Script;

/**
 * @author Kevin Lindsey
 */
public abstract class AbstractJavascriptTest extends TestCase
{
	/**
	 * The environment variable name that points to the root of the depot
	 */
	protected static final String TREEROOT = "TREEROOT";
	
	private Global _global;
	
	/**
	 * AbstractJavascriptTest
	 */
	public AbstractJavascriptTest()
	{
	}
	
	/**
	 * runScript
	 * 
	 * @param name
	 * @param source
	 */
	protected void runScript(String name, String source)
	{
		String wrappedSource = "function " + name + "() {" + source + "}";
		
		this.compile(wrappedSource, name);
		this.invoke(name, new Object[] {});
	}
	
	/**
	 * setUp
	 */
	@Override
	protected void setUp() throws Exception
	{
		super.setUp();
		
		Context context = Context.enter();
		
		try
		{
			this._global = new Global(context);
			
			String[] lines = new String[] {
				"var Aptana = {};",
				"var Log = {",
				"    trace: function(msg) {",
				"        alert(msg)",
				"    },",
				"    forModule: function(name) {",
				"        return {};",
				"    }",
				"};"
			};
			
			this.loadSource(StringUtils.join("\n", lines), "<built-in>");
			
			String[] parts = new String[] {
				System.getenv(TREEROOT),
				"products",
				"server",
				"framework",
				"config.js"
			};
			this.load(StringUtils.join(File.separator, parts));
			
//			this.load("../framework/Logging/log4javascript_uncompressed.js");
//			this.load("../framework/Logging/FileAppender.js");
//			this.load("../framework/Logging/CoreAppender.js");
//			this.load("../framework/Logging/Log.js");
		}
		finally
		{
			Context.exit();
		}
	}
	
	/**
	 * load
	 * 
	 * @param filename
	 * @throws IOException
	 */
	protected void load(String filename) throws IOException
	{
		File file = new File(filename);
		String source = FileUtils.getFileContents(file, null);
		
		this.loadSource(source, file.getAbsolutePath());
	}
	
	/**
	 * loadSource
	 * 
	 * @param source
	 * @param filename
	 */
	protected void loadSource(String source, String filename)
	{
		this.compile(source, filename);
	}
	
	/**
	 * compile
	 * 
	 * @param content
	 * @param filename
	 */
	protected void compile(String content, String filename)
	{
		Context context = Context.enter();
		
		try
		{
			Script compiledScript = context.compileString(content, filename, 1, null);
			
			compiledScript.exec(context, this._global);
		}
		finally
		{
			Context.exit();
		}
	}
	
	/**
	 * invoke
	 *
	 * @param functionName
	 * @param args
	 * @return result
	 */
	protected Object invoke(String functionName, Object[] functionArgs)
	{
		Object fObj = this._global.get(functionName, this._global);
		Object result = null;
		
		Context.enter();
		
		try
		{
			if (fObj instanceof Function)
			{
				Function function = (Function) fObj;
				
				result = function.call(Context.getCurrentContext(), this._global, this._global, functionArgs);
			}
		}
		finally
		{
			Context.exit();
		}
		
		return result;
	}
}
