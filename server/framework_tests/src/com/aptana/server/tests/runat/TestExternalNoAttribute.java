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
import com.gargoylesoftware.htmlunit.html.HtmlElement;

/**
 * @author Kevin Lindsey
 */
public class TestExternalNoAttribute extends AbstractTest
{
	/**
	 * ClientAttributeTest
	 */
	public TestExternalNoAttribute()
	{
	}

	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/runat/externalNoAttribute/";
	}

	/**
	 * testNoAttributeNoProperty
	 * 
	 * @throws Exception
	 */
	public void testNoAttributeNoProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("noAttributeNoProperty.html"));

		// make sure we have a script element
		assertEquals(1, results.size());
		
		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);
		
		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));

		// make sure the runat attribute was removed
		assertTrue(script.isAttributeDefined("src"));
		
		// make sure we're pointing to the correct JS file
		assertEquals("clientNoProperty.js", script.getAttributeValue("src"));
	}
	
	/**
	 * testNoAttributeClientProperty
	 * 
	 * @throws Exception
	 */
	public void testNoAttributeClientProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("noAttributeClientProperty.html"));
		
		// make sure we have a script element
		assertEquals(1, results.size());
		
		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);
		
		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));
		
		// make sure the runat attribute was removed
		assertTrue(script.isAttributeDefined("src"));
		
		// make sure we're pointing to the correct JS file
		assertEquals("clientClientProperty.js", script.getAttributeValue("src"));
	}
	
	/**
	 * testNoAttributeServerProperty
	 * 
	 * @throws Exception
	 */
	public void testNoAttributeServerProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("noAttributeServerProperty.html"));
		
		// make sure we have a script element
		assertEquals(1, results.size());
		
		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);
		
		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));
		
		// make sure the runat attribute was removed
		assertTrue(script.isAttributeDefined("src"));
		
		// make sure we're pointing to the correct JS file
		assertEquals("clientServerProperty.js", script.getAttributeValue("src"));
	}
	
	/**
	 * testNoAttributeBothProperty
	 * 
	 * @throws Exception
	 */
	public void testNoAttributeBothProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("noAttributeBothProperty.html"));
		
		// make sure we have a script element
		assertEquals(1, results.size());
		
		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);
		
		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));
		
		// make sure the runat attribute was removed
		assertTrue(script.isAttributeDefined("src"));
		
		// make sure we're pointing to the correct JS file
		assertEquals("clientBothProperty.js", script.getAttributeValue("src"));
	}
}
