/**
 * Copyright (c) 2005-2006 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.sdoc;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringWriter;

/**
 * @author Kevin Lindsey
 *
 */
public class FileUtils
{
	/**
	 * getStreamText
	 * 
	 * @param stream
	 * @return String
	 */
	public static String getStreamText(InputStream stream)
	{
		String result = null;
		
		try
		{
			// create output buffer
			StringWriter sw = new StringWriter();

			// read contents into a string buffer
			try
			{
				// get buffered reader
				InputStreamReader isr = new InputStreamReader(stream);
				BufferedReader reader = new BufferedReader(isr);

				// create temporary buffer
				char[] buf = new char[1024];

				// fill buffer
				int numRead = reader.read(buf);

				// keep reading until the end of the stream
				while (numRead != -1)
				{
					// output temp buffer to output buffer
					sw.write(buf, 0, numRead);

					// fill buffer
					numRead = reader.read(buf);
				}
			}
			finally
			{
				if (stream != null)
				{
					stream.close();
				}
			}

			// return string buffer's content
			result = sw.toString();
		}
		catch (Exception e)
		{
		}
		
		return result;
	}
}
