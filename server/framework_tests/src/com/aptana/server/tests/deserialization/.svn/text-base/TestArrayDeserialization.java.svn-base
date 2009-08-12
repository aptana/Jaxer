/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests.deserialization;

import java.net.URL;

import com.aptana.server.tests.AbstractTest;
import com.gargoylesoftware.htmlunit.WebClient;
import com.gargoylesoftware.htmlunit.html.HtmlElement;
import com.gargoylesoftware.htmlunit.html.HtmlPage;

/**
 * @author Kevin Lindsey
 */
public class TestArrayDeserialization extends AbstractTest
{
	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/deserialization/array/";
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
	 * testDeserializeEmptyArray
	 * 
	 * @throws Exception
	 */
	public void testDeserializeEmptyArray() throws Exception
	{
		assertEquals("[]", this.getOutput("deserializeEmptyArray.html"));
	}
	
	/**
	 * testDeserializeArrayOfBooleans
	 * 
	 * @throws Exception
	 */
	public void testDeserializeArrayOfBooleans() throws Exception
	{
		assertEquals("[true,false,false,true]", this.getOutput("deserializeArrayOfBooleans.html"));
	}
	
	/**
	 * testDeserializeArrayOfNumbers
	 * 
	 * @throws Exception
	 */
	public void testDeserializeArrayOfNumbers() throws Exception
	{
		assertEquals("[1,2.1,0.3,1e-12]", this.getOutput("deserializeArrayOfNumbers.html"));
	}
	
	/**
	 * testDeserializeArrayOfStrings
	 * 
	 * @throws Exception
	 */
	public void testDeserializeArrayOfStrings() throws Exception
	{
		assertEquals("[\"\",\"abc\",\"def\"]", this.getOutput("deserializeArrayOfStrings.html"));
	}
	
	/**
	 * testDeserializeArrayOfArrays
	 * 
	 * @throws Exception
	 */
	public void testDeserializeArrayOfArrays() throws Exception
	{
		assertEquals("[[],[true,false,false,true],[1,2.1,0.3,1e-12],[\"\",\"abc\",\"def\"]]", this.getOutput("deserializeArrayOfArrays.html"));
	}
}
