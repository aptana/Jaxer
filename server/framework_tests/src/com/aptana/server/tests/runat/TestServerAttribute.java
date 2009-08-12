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

import com.aptana.server.tests.AbstractDatabaseTest;
import com.aptana.server.tests.CallbackPageTable;
import com.gargoylesoftware.htmlunit.html.HtmlElement;

/**
 * @author Kevin Lindsey
 */
public class TestServerAttribute extends AbstractDatabaseTest
{
	/**
	 * ClientAttributeTest
	 */
	public TestServerAttribute()
	{
	}
	
	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/runat/serverAttribute/";
	}
	
	/**
	 * getCode
	 * 
	 * @param urlString
	 * @return String
	 * @throws Exception 
	 */
	private String getCode(String urlString) throws Exception
	{
		URL url = this.getURL(urlString);
		
		return this.getCode(url);
	}
	
	/**
	 * getCode
	 * 
	 * @param url
	 * @return String
	 * @throws Exception 
	 */
	private String getCode(URL url) throws Exception
	{
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
		String code = getCode("serverAttributeClientProperty.html");
		
		// make sure we have a script element
		assertEquals("function myFunction() { return false; }", code);
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
		String code = getCode("serverAttributeClientProperty.html");
		
		// make sure we have a script element
		assertEquals("function myFunction() { return false; }", code);
	}
	
	/**
	 * testServerAttributeServerCacheProperty
	 * 
	 * @throws Exception
	 */
	public void testServerAttributeServerCacheProperty() throws Exception
	{
		// get script elements
		URL url = this.getURL("serverAttributeServerCacheProperty.html");
		List results = this.getScriptElements(url);
		
		// make sure we have a script element
		assertEquals(0, results.size());
		
		// get reference to callback page table
		CallbackPageTable callbackPage = this.database.getCallbackPageTable();
		
		// make sure we have a record in the database
		assertEquals(1, callbackPage.getRowCount());
		
		// check crc32
		long crc32 = this.database.getLong("SELECT crc32 FROM " + callbackPage.getName());
		assertEquals(1119500781, crc32);
		
		// check page name
		String name = this.database.getString("SELECT name FROM " + callbackPage.getName());
		String nameTarget = url.toString().substring("http://".length());
		assertEquals(nameTarget, name);
		
		// check stored value
		String value = this.database.getString("SELECT value FROM " + callbackPage.getName());
		StringBuffer buffer = new StringBuffer();
		buffer.append("myFunction = function myFunction() {\n");
		buffer.append("    return false;\n");
		buffer.append("}");
		assertEquals(buffer.toString(), value);
		
		// check access count
		int accessCount = this.database.getInt("SELECT access_count FROM " + callbackPage.getName());
		assertEquals(0, accessCount);
	}
	
	/**
	 * testServerAttributeBothCacheProperty
	 * 
	 * @throws Exception
	 */
	public void testServerAttributeBothCacheProperty() throws Exception
	{
		URL url = this.getURL("serverAttributeBothCacheProperty.html");
		
		// get script elements
		String code = getCode(url);
		
		// make sure we have a script element
		assertEquals("function myFunction() { return false; }", code);
		
		// get reference to callback page table
		CallbackPageTable callbackPage = this.database.getCallbackPageTable();
		
		// make sure we have a record in the database
		assertEquals(1, callbackPage.getRowCount());
		
		// check crc32
		long crc32 = this.database.getLong("SELECT crc32 FROM " + callbackPage.getName());
		assertEquals(1119500781, crc32);
		
		// check page name
		String name = this.database.getString("SELECT name FROM " + callbackPage.getName());
		String nameTarget = url.toString().substring("http://".length());
		assertEquals(nameTarget, name);
		
		// check stored value
		String value = this.database.getString("SELECT value FROM " + callbackPage.getName());
		StringBuffer buffer = new StringBuffer();
		buffer.append("myFunction = function myFunction() {\n");
		buffer.append("    return false;\n");
		buffer.append("}");
		assertEquals(buffer.toString(), value);
		
		// check access count
		int accessCount = this.database.getInt("SELECT access_count FROM " + callbackPage.getName());
		assertEquals(0, accessCount);
	}
	
	/**
	 * testServerAttributeServerProxyProperty
	 * 
	 * @throws Exception
	 */
	public void testServerAttributeServerProxyProperty() throws Exception
	{
		URL url = this.getURL("serverAttributeServerProxyProperty.html");
		
		// get script elements
		List results = this.getScriptElementsWithProxy(url);
		
		// make sure we have a script element
		assertEquals(1, results.size());

		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);

		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));

		// test content of script block
		String code = script.getFirstChild().asText();
		code = code.substring(code.indexOf("function myFunction()"));	// strip crc
		String expected = "function myFunction() {return Aptana.Callback.invokeFunction.call(null, \"myFunction\", arguments);}";
		expected += " function myFunctionAsync(callback) {return Aptana.Callback.invokeFunctionAsync.call(null, callback, \"myFunction\", arguments);}";
		assertEquals(expected, code);
		
		// get reference to callback page table
		CallbackPageTable callbackPage = this.database.getCallbackPageTable();
		
		// make sure we have a record in the database
		assertEquals(1, callbackPage.getRowCount());
		
		// check crc32
		long crc32 = this.database.getLong("SELECT crc32 FROM " + callbackPage.getName());
		assertEquals(1119500781, crc32);
		
		// check page name
		String name = this.database.getString("SELECT name FROM " + callbackPage.getName());
		String nameTarget = url.toString().substring("http://".length());
		assertEquals(nameTarget, name);
		
		// check stored value
		String value = this.database.getString("SELECT value FROM " + callbackPage.getName());
		StringBuffer buffer = new StringBuffer();
		buffer.append("myFunction = function myFunction() {\n");
		buffer.append("    return false;\n");
		buffer.append("}");
		assertEquals(buffer.toString(), value);
		
		// check access count
		int accessCount = this.database.getInt("SELECT access_count FROM " + callbackPage.getName());
		assertEquals(0, accessCount);
	}
	
	/**
	 * testServerAttributeBothProxyProperty
	 * 
	 * @throws Exception
	 */
	public void testServerAttributeBothProxyProperty() throws Exception
	{
		URL url = this.getURL("serverAttributeBothProxyProperty.html");
		
		// get script elements
		List results = this.getScriptElementsWithProxy(url);
		
		// make sure we have a script element
		assertEquals(1, results.size());

		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);

		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));

		// test content of script block
		String code = script.getFirstChild().asText();
		// TODO: This should match TestBothAndProxyAttribute.testBothAndProxyAttribute()
		String expected = "function myFunction() { return false; }";
		//expected += " function myFunctionAsync(callback) {return Aptana.Callback.invokeFunctionAsync.call(null, callback, \"myFunction\", arguments);}";
		expected += " Aptana.Server.myFunction = function myFunction() {return Aptana.Callback.invokeFunction.call(null, \"myFunction\", arguments);}";
		expected += " Aptana.Server.myFunctionAsync = function myFunctionAsync(callback) {return Aptana.Callback.invokeFunctionAsync.call(null, callback, \"myFunction\", arguments);}";
		assertEquals(expected, code);
		
		// get reference to callback page table
		CallbackPageTable callbackPage = this.database.getCallbackPageTable();
		
		// make sure we have a record in the database
		assertEquals(1, callbackPage.getRowCount());
		
		// check crc32
		long crc32 = this.database.getLong("SELECT crc32 FROM " + callbackPage.getName());
		assertEquals(1119500781, crc32);
		
		// check page name
		String name = this.database.getString("SELECT name FROM " + callbackPage.getName());
		String nameTarget = url.toString().substring("http://".length());
		assertEquals(nameTarget, name);
		
		// check stored value
		String value = this.database.getString("SELECT value FROM " + callbackPage.getName());
		StringBuffer buffer = new StringBuffer();
		buffer.append("myFunction = function myFunction() {\n");
		buffer.append("    return false;\n");
		buffer.append("}");
		assertEquals(buffer.toString(), value);
		
		// check access count
		int accessCount = this.database.getInt("SELECT access_count FROM " + callbackPage.getName());
		assertEquals(0, accessCount);
	}
}
