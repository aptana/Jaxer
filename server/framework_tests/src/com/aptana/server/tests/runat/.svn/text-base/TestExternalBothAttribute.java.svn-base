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
public class TestExternalBothAttribute extends AbstractTest
{
	/**
	 * ClientAttributeTest
	 */
	public TestExternalBothAttribute()
	{
	}

	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/runat/externalBothAttribute/";
	}
	
	/**
	 * testBothAttributeNoProperty
	 * 
	 * @throws Exception
	 */
	public void testBothAttributeNoProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("bothAttributeNoProperty.html"));
		
		// make sure we have a script element
		assertEquals(1, results.size());
		
		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);
		
		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));
		
		// make sure the runat attribute was removed
		assertTrue(script.isAttributeDefined("src"));
		
		// make sure we're pointing to the correct JS file
		assertEquals("bothNoProperty.js", script.getAttributeValue("src"));
	}
	
	/**
	 * testBothAttributeClientProperty
	 * 
	 * @throws Exception
	 */
	public void testBothAttributeClientProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("bothAttributeClientProperty.html"));
		
		// make sure we have a script element
		assertEquals(1, results.size());
		
		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);
		
		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));
		
		// make sure the runat attribute was removed
		assertTrue(script.isAttributeDefined("src"));
		
		// make sure we're pointing to the correct JS file
		assertEquals("bothClientProperty.js", script.getAttributeValue("src"));
	}
	
	/**
	 * testBothAttributeServerProperty
	 * 
	 * @throws Exception
	 */
	public void testBothAttributeServerProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("bothAttributeServerProperty.html"));
		
		// make sure we have a script element
		assertEquals(1, results.size());
		
		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);
		
		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));
		
		// make sure the runat attribute was removed
		assertTrue(script.isAttributeDefined("src"));
		
		// make sure we're pointing to the correct JS file
		assertEquals("bothServerProperty.js", script.getAttributeValue("src"));
	}
	
	/**
	 * testBothAttributeBothProperty
	 * 
	 * @throws Exception
	 */
	public void testBothAttributeBothProperty() throws Exception
	{
		// get script elements
		List results = this.getScriptElements(this.getURL("bothAttributeBothProperty.html"));
		
		// make sure we have a script element
		assertEquals(1, results.size());
		
		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);
		
		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));
		
		// make sure the runat attribute was removed
		assertTrue(script.isAttributeDefined("src"));
		
		// make sure we're pointing to the correct JS file
		assertEquals("bothBothProperty.js", script.getAttributeValue("src"));
	}
}
