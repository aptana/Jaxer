/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests.parsing;

import java.io.File;
import java.io.IOException;

import com.aptana.server.tests.AbstractJavascriptTest;
import com.aptana.server.tests.StringUtils;

/**
 * @author Kevin Lindsey
 */
public class TestRange extends AbstractJavascriptTest
{
	/**
	 * setUp
	 */
	@Override
	protected void setUp() throws Exception
	{
		super.setUp();
		
		String[] parts = new String[] {
			System.getenv(TREEROOT),
			"products",
			"server",
			"framework",
			"Parsing",
			"Range.js"
		};
		this.load(StringUtils.join(File.separator, parts));
	}

	/**
	 * testRangeEmpty
	 * 
	 * @throws IOException
	 */
	public void testRangeEmpty() throws IOException
	{
		String[] lines = new String[] {
			"var range = new Aptana.Range(10,10);",
			"assertTrue(range.isEmpty());",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeNotEmpty
	 * 
	 * @throws IOException
	 */
	public void testRangeNotEmpty() throws IOException
	{
		String[] lines = new String[] {
			"var range = new Aptana.Range(10,11);",
			"assertFalse(range.isEmpty());",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeContainsToLeft
	 * 
	 * @throws IOException
	 */
	public void testRangeContainsToLeft() throws IOException
	{
		String[] lines = new String[] {
			"var range = new Aptana.Range(1,100);",
			"assertFalse(range.contains(0));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeContainsOnLeft
	 * 
	 * @throws IOException
	 */
	public void testRangeContainsOnLeft() throws IOException
	{
		String[] lines = new String[] {
			"var range = new Aptana.Range(1,100);",
			"assertTrue(range.contains(1));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeContainsInMiddle
	 * 
	 * @throws IOException
	 */
	public void testRangeContainsInMiddle() throws IOException
	{
		String[] lines = new String[] {
			"var range = new Aptana.Range(1,100);",
			"assertTrue(range.contains(50));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeContainsOnRight
	 * 
	 * @throws IOException
	 */
	public void testRangeContainsOnRight() throws IOException
	{
		String[] lines = new String[] {
			"var range = new Aptana.Range(1,100);",
			"assertTrue(range.contains(99));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeContainsToRight
	 * 
	 * @throws IOException
	 */
	public void testRangeContainsToRight() throws IOException
	{
		String[] lines = new String[] {
			"var range = new Aptana.Range(1,100);",
			"assertFalse(range.contains(100));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeOverlapToLeft
	 * 
	 * @throws IOException
	 */
	public void testRangeOverlapToLeft() throws IOException
	{
		String[] lines = new String[] {
			"var range1 = new Aptana.Range(50,100);",
			"var range2 = new Aptana.Range(0,10);",
			"assertFalse(range1.isOverlapping(range2));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeOverlapEndLeft
	 * 
	 * @throws IOException
	 */
	public void testRangeOverlapEndLeft() throws IOException
	{
		String[] lines = new String[] {
			"var range1 = new Aptana.Range(50,100);",
			"var range2 = new Aptana.Range(0,50);",
			"assertFalse(range1.isOverlapping(range2));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeOverlapStraddleLeft
	 * 
	 * @throws IOException
	 */
	public void testRangeOverlapStraddleLeft() throws IOException
	{
		String[] lines = new String[] {
			"var range1 = new Aptana.Range(50,100);",
			"var range2 = new Aptana.Range(25,75);",
			"assertTrue(range1.isOverlapping(range2));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeOverlapStartLeft
	 * 
	 * @throws IOException
	 */
	public void testRangeOverlapStartLeft() throws IOException
	{
		String[] lines = new String[] {
			"var range1 = new Aptana.Range(50,100);",
			"var range2 = new Aptana.Range(50,75);",
			"assertTrue(range1.isOverlapping(range2));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeOverlapInside
	 * 
	 * @throws IOException
	 */
	public void testRangeOverlapInside() throws IOException
	{
		String[] lines = new String[] {
			"var range1 = new Aptana.Range(50,100);",
			"var range2 = new Aptana.Range(70,80);",
			"assertTrue(range1.isOverlapping(range2));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeOverlapEndRight
	 * 
	 * @throws IOException
	 */
	public void testRangeOverlapEndRight() throws IOException
	{
		String[] lines = new String[] {
			"var range1 = new Aptana.Range(50,100);",
			"var range2 = new Aptana.Range(70,100);",
			"assertTrue(range1.isOverlapping(range2));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeOverlapStraddleRight
	 * 
	 * @throws IOException
	 */
	public void testRangeOverlapStraddleRight() throws IOException
	{
		String[] lines = new String[] {
			"var range1 = new Aptana.Range(50,100);",
			"var range2 = new Aptana.Range(70,125);",
			"assertTrue(range1.isOverlapping(range2));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeOverlapStartRight
	 * 
	 * @throws IOException
	 */
	public void testRangeOverlapStartRight() throws IOException
	{
		String[] lines = new String[] {
				"var range1 = new Aptana.Range(50,100);",
				"var range2 = new Aptana.Range(100,125);",
				"assertFalse(range1.isOverlapping(range2));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
	
	/**
	 * testRangeOverlapToRight
	 * 
	 * @throws IOException
	 */
	public void testRangeOverlapToRight() throws IOException
	{
		String[] lines = new String[] {
				"var range1 = new Aptana.Range(50,100);",
				"var range2 = new Aptana.Range(110,125);",
				"assertFalse(range1.isOverlapping(range2));",
		};
		
		this.runScript("test", StringUtils.join("\n", lines));
	}
}
