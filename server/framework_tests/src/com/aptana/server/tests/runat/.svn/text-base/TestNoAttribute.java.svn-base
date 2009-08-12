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
public class TestNoAttribute extends AbstractTest
{
	/**
	 * ClientAttributeTest
	 */
	public TestNoAttribute()
	{
	}
	
	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/runat/noAttribute/";
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
	 * testNoProperty
	 * 
	 * @throws Exception
	 */
	public void testNoProperty() throws Exception
	{
		String code = getCode("noAttributeNoProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; }", code);
	}
	
	/**
	 * testClientProperty
	 * 
	 * @throws Exception
	 */
	public void testClientProperty() throws Exception
	{
		String code = getCode("noAttributeClientProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"client\";", code);
	}
	
	/**
	 * testServerProperty
	 * 
	 * @throws Exception
	 */
	public void testServerProperty() throws Exception
	{
		String code = getCode("noAttributeServerProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"server\";", code);
	}
	
	/**
	 * testBothProperty
	 * 
	 * @throws Exception
	 */
	public void testBothProperty() throws Exception
	{
		String code = getCode("noAttributeBothProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"both\";", code);
	}
	
	/**
	 * testServerCacheProperty
	 * 
	 * @throws Exception
	 */
	public void testServerCacheProperty() throws Exception
	{
		String code = getCode("noAttributeServerCacheProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"server-cache\";", code);
	}
	
	/**
	 * testBothCacheProperty
	 * 
	 * @throws Exception
	 */
	public void testBothCacheProperty() throws Exception
	{
		String code = getCode("noAttributeBothCacheProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"both-cache\";", code);
	}
	
	/**
	 * testServerProxyProperty
	 * 
	 * @throws Exception
	 */
	public void testServerProxyProperty() throws Exception
	{
		String code = getCode("noAttributeServerProxyProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"server-proxy\";", code);
	}
	
	/**
	 * testBothProxyProperty
	 * 
	 * @throws Exception
	 */
	public void testBothProxyProperty() throws Exception
	{
		String code = getCode("noAttributeBothProxyProperty.html");
		
		assertEquals("var x = 10; function myFunction() { return false; } myFunction.runat = \"both-proxy\";", code);
	}
}
