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
public class TestBothAndProxyAttribute extends AbstractDatabaseTest
{
	/**
	 * ClientAttributeTest
	 */
	public TestBothAndProxyAttribute()
	{
	}
	
	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/runat/bothProxyAttribute/";
	}

	/**
	 * testBothAndProxyAttribute
	 * 
	 * @throws Exception
	 */
	public void testBothAndProxyAttribute() throws Exception
	{
		// get script elements
		URL url = this.getURL("bothProxyAttribute.html");
		List results = this.getScriptElementsWithProxy(url);

		// make sure we have a script element
		assertEquals(1, results.size());

		// get the script element
		HtmlElement script = (HtmlElement) results.get(0);

		// make sure the runat attribute was removed
		assertFalse(script.isAttributeDefined("runat"));
		
		// test content
		String code = script.getFirstChild().asText();
		String expected = "function myFunction() { return false; }";
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
