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

/**
 * @author Kevin Lindsey
 */
public class StringUtils
{
	/**
	 * join
	 * 
	 * @param separator
	 * @param items
	 * @return String
	 */
	public static String join(String separator, String[] items)
	{
		StringBuffer buffer = new StringBuffer();
		
		if (items != null && items.length > 0)
		{
			buffer.append(items[0]);
			
			for (int i = 1; i < items.length; i++)
			{
				buffer.append(separator);
				buffer.append(items[i]);
			}
		}
		
		return buffer.toString();
	}
}
