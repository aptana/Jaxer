/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests.runat;

import java.util.List;

import com.aptana.server.tests.AbstractTest;

/**
 * @author Kevin Lindsey
 */
public class TestExternalServerAttribute extends AbstractTest
{
	/**
	 * ClientAttributeTest
	 */
	public TestExternalServerAttribute()
	{
	}

	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/runat/externalServerAttribute/";
	}
	
	/**
	 * testServerAttributeNoProperty
	 * 
	 * @throws Exception
	 */
	public void testServerAttributeNoProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("serverAttributeNoProperty.html"));
		
		// make sure we have a script element
		assertEquals(0, results.size());
	}
	
	/**
	 * testServerAttributeClientProperty
	 * 
	 * @throws Exception
	 */
	public void testServerAttributeClientProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("serverAttributeClientProperty.html"));
		
		// make sure we have a script element
		assertEquals(0, results.size());
	}
	
	/**
	 * testServerAttributeServerProperty
	 * 
	 * @throws Exception
	 */
	public void testServerAttributeServerProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("serverAttributeServerProperty.html"));
		
		// make sure we have a script element
		assertEquals(0, results.size());
	}
	
	/**
	 * testServerAttributeBothProperty
	 * 
	 * @throws Exception
	 */
	public void testServerAttributeBothProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("serverAttributeBothProperty.html"));
		
		// make sure we have a script element
		assertEquals(0, results.size());
	}
}
