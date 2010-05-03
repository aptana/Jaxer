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

import java.io.IOException;

/**
 * @author Kevin Lindsey
 */
public class Main
{
	/**
	 * main
	 * 
	 * @param args
	 * @throws IOException
	 */
	public static void main(String[] args) throws IOException
	{
		if (args.length == 3)
		{
			String oldDir = args[0];
			String newDir = args[1];
			String outDir = args[2];

			MoveComments mc = new MoveComments(oldDir, newDir, outDir);

			mc.updateComments();
		}
		else
		{
			System.out.println("usage: Main <old-directory> <new-directory> <output-directory>");
			System.out.println();
			System.out.println("<old-directory>    contains the last version of YUI to which we");
			System.out.println("                   attached @alias tags");
			System.out.println("<new-directory>    contains the newest version of YUI which does");
			System.out.println("                   not have @alias tags");
			System.out.println("<output-directory> The directory where the resulting output will");
			System.out.println("                   be sent");
		}
	}
}
