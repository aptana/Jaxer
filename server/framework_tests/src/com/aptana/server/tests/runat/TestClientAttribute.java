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

import java.net.URL;
import java.util.List;

import com.aptana.server.tests.AbstractTest;
import com.gargoylesoftware.htmlunit.html.HtmlElement;

/**
 * @author Kevin Lindsey
 */
public class TestClientAttribute extends AbstractTest
{
	/**
	 * ClientAttributeTest
	 */
	public TestClientAttribute()
	{
	}
	
	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/runat/clientAttribute/";
	}

	/**
	 * getCode
	 * 
	 * @param urlString
	 * @throws Exception 
	 */
	private String getCode(String urlString) throws Exception
	{
		URL url = this.getURL(urlString);
		
		// get script elements
		List results = this.getScriptElements(url);

		// make sure we have a script element
		assertEquals(1, results.size());

		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);

		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));

		// test content of script block
		return script.getFirstChild().asText();
	}
	
	/**
	 * testClientAttributeNoProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeNoProperty() throws Exception
	{
		String code = getCode("clientAttributeNoProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; }", code);
	}
	
	/**
	 * testClientAttributeClientProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeClientProperty() throws Exception
	{
		String code = getCode("clientAttributeClientProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"client\";", code);
	}
	
	/**
	 * testClientAttributeServerProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeServerProperty() throws Exception
	{
		String code = getCode("clientAttributeServerProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"server\";", code);
	}
	
	/**
	 * testClientAttributeBothProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeBothProperty() throws Exception
	{
		String code = getCode("clientAttributeBothProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"both\";", code);
	}
	
	/**
	 * testClientAttributeServerCacheProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeServerCacheProperty() throws Exception
	{
		String code = getCode("clientAttributeServerCacheProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"server-cache\";", code);
	}
	
	/**
	 * testClientAttributeBothCacheProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeBothCacheProperty() throws Exception
	{
		String code = getCode("clientAttributeBothCacheProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"both-cache\";", code);
	}
	
	/**
	 * testClientAttributeServerProxyProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeServerProxyProperty() throws Exception
	{
		String code = getCode("clientAttributeServerProxyProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"server-proxy\";", code);
	}
	
	/**
	 * testClientAttributeBothProxyProperty
	 * 
	 * @throws Exception
	 */
	public void testClientAttributeBothProxyProperty() throws Exception
	{
		String code = getCode("clientAttributeBothProxyProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"both-proxy\";", code);
	}
}
