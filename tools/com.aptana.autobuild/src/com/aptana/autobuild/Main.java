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

import java.io.File;
import java.io.IOException;

import org.mozilla.javascript.Context;
import org.mozilla.javascript.Function;
import org.mozilla.javascript.Script;

/**
 * @author Kevin Lindsey
 */
public class Main
{
	public static Global global;
	
	/**
	 * main
	 * 
	 * @param args
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException
	{
		int status = 0;
		
		if (args.length >= 2)
		{
			// get script source
			String scriptFilename = args[0];
			File scriptFile = new File(scriptFilename);
			String scriptContents = FileUtils.getFileContents(scriptFile, null);
			
			// get build file
			String buildFilename = args[1];
			File buildFile = new File(buildFilename);
			
			try
			{
				Context context = Context.enter();
				
				global = new Global(context);
				
				// compile script
				Script compiledScript = context.compileString(scriptContents, scriptFilename, 1, null);
				
				// place path in script's global. We do this after exec'ing so
				// we can overwrite globals created during that stage
				global.put(Global.LOCATION_PROPERTY, global, scriptFilename);
				
				// execute the script
				compiledScript.exec(context, global);
				
				// add any globals specified on the command-line
				for (int i = 2; i < args.length; i++)
				{
					String keyValue = args[i];
					int equalIndex = keyValue.indexOf('=');
					
					if (equalIndex != -1)
					{
						String key = keyValue.substring(0, equalIndex);
						String value = keyValue.substring(equalIndex + 1);
						
						global.put(key, global, value);
					}
				}
				
				// initialize script
				invoke("initParsing", new Object[0]);
				
				// process build file
				Double result = (Double) invoke("processFile", new Object[] { buildFile });
				
				// update return status
				status = result.intValue();
			}
			finally
			{
				Context.exit();
			}
		}
		else
		{
			System.out.println("usage: <script-file-path> <build-file-path> <globals>*");
			System.out.println();
			System.out.println("<script-file-path>: The absolute path to the build script. Most likely this");
			System.out.println("                    points to //depot/products/framework/scripts/auto_builder.js");
			System.out.println("<build-file-path>:  The absolute path to the build file. Most likely this points");
			System.out.println("                    to //depot/products/framework/build.lst");
			System.out.println("<globals>:          Zero or more key/value pairs which will create new globals");
			System.out.println("                    using the key name and value. Key/values are specified with");
			System.out.println("                    with the key name followed by an equal sign followed by the");
			System.out.println("                    value without spaces. For example, to set BUILD_OUTPUT to the");
			System.out.println("                    string \"test\", use: BUILD_OUTPUT=test. Note that all values");
			System.out.println("                    are treated as string values and do not require double quotes");
		}
		
		System.exit(status);
	}
	
	/**
	 * invoke
	 *
	 * @param functionName
	 * @param args
	 * @return result
	 */
	private static Object invoke(String functionName, Object[] functionArgs)
	{
		Object fObj = global.get(functionName, global);
		Object result = null;
			
		if (fObj instanceof Function)
		{
			Function function = (Function) fObj;
			
			result = function.call(Context.getCurrentContext(), global, global, functionArgs);
		}
		
		return result;
	}
}
