/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests.serialization;

import java.net.URL;

import com.aptana.server.tests.AbstractTest;
import com.gargoylesoftware.htmlunit.WebClient;
import com.gargoylesoftware.htmlunit.html.HtmlElement;
import com.gargoylesoftware.htmlunit.html.HtmlPage;

/**
 * @author Kevin Lindsey
 */
public class TestObjectSerialization extends AbstractTest
{
	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/serialization/object/";
	}

	/**
	 * geOutput
	 * 
	 * @param pageName
	 * @return String
	 * @throws Exception
	 */
	private String getOutput(String pageName) throws Exception
	{
		URL url = this.getURL(pageName);
		WebClient webClient = this.getWebClient();
		HtmlPage page = (HtmlPage) webClient.getPage(url);

		HtmlElement output = page.getHtmlElementById("output");

		return output.asText();
	}

	/**
	 * testSerializeEmptyObject
	 * 
	 * @throws Exception
	 */
	public void testSerializeEmptyObject() throws Exception
	{
		assertEquals("{}", this.getOutput("serializeEmptyObject.html"));
	}
	
	/**
	 * testSerializeObjectOfBooleans
	 * 
	 * @throws Exception
	 */
	public void testSerializeObjectOfBooleans() throws Exception
	{
		assertEquals("{\"a\":true,\"b\":false}", this.getOutput("serializeObjectOfBooleans.html"));
	}
	
	/**
	 * testSerializeObjectOfNumbers
	 * 
	 * @throws Exception
	 */
	public void testSerializeObjectOfNumbers() throws Exception
	{
		assertEquals(
			"{\"a\":10,\"b\":-10,\"c\":10.5,\"d\":-10.5,\"e\":1.05e+50,\"f\":-1.05e+50,\"g\":1.05e-50,\"h\":-1.05e-50}",
			this.getOutput("serializeObjectOfNumbers.html")
		);
	}
	
	/**
	 * testSerializeObjectOfStrings
	 * 
	 * @throws Exception
	 */
	public void testSerializeObjectOfStrings() throws Exception
	{
		assertEquals(
			"{\"a\":\"hello\",\"b\":\"world\"}",
			this.getOutput("serializeObjectOfStrings.html")
		);
	}
	
	/**
	 * testSerializeCustomObject
	 * 
	 * @throws Exception
	 */
	public void testSerializeCustomObject() throws Exception
	{
		assertEquals("{\"a\":\"a\",\"b\":\"b\"}", this.getOutput("serializeCustomObject.html"));
	}
}
