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
public class TestExternalClientAttribute extends AbstractTest
{
	/**
	 * ClientAttributeTest
	 */
	public TestExternalClientAttribute()
	{
	}

	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/runat/externalClientAttribute/";
	}
	
	/**
	 * testClientAttributeNoProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeNoProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("clientAttributeNoProperty.html"));
		
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
	 * testClientAttributeClientProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeClientProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("clientAttributeClientProperty.html"));
		
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
	 * testClientAttributeServerProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeServerProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("clientAttributeServerProperty.html"));
		
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
	 * testClientAttributeBothProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeBothProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("clientAttributeBothProperty.html"));
		
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
