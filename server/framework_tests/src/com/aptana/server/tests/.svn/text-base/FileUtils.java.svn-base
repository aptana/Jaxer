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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.StringWriter;

/**
 * @author Kevin Lindsey
 */
public class FileUtils
{
	/**
	 * @param file
	 * @param charset
	 * @return String
	 * @throws IOException
	 */
	public static String getFileContents(File file, String charset) throws IOException
	{
		FileInputStream stream = new FileInputStream(file);
		InputStreamReader inputReader;

		if (charset != null)
		{
			inputReader = new InputStreamReader(stream, charset);
		}
		else
		{
			inputReader = new InputStreamReader(stream);
		}

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

		return sw.toString();
	}
}
