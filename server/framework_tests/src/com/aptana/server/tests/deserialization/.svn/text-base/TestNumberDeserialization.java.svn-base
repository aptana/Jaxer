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
public class TestNumberDeserialization extends AbstractTest
{
	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/deserialization/number/";
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
	 * testDeserializeInteger
	 * 
	 * @throws Exception
	 */
	public void testDeserializeInteger() throws Exception
	{
		assertEquals("10", this.getOutput("deserializeInteger.html"));
	}
	
	/**
	 * testDeserializeNegativeInteger
	 * 
	 * @throws Exception
	 */
	public void testDeserializeNegativeInteger() throws Exception
	{
		assertEquals("-10", this.getOutput("deserializeNegativeInteger.html"));
	}
	
	/**
	 * testDeserializeFloat
	 * 
	 * @throws Exception
	 */
	public void testDeserializeFloat() throws Exception
	{
		assertEquals("10.5", this.getOutput("deserializeFloat.html"));
	}
	
	/**
	 * testDeserializeNegativeFloat
	 * 
	 * @throws Exception
	 */
	public void testDeserializeNegativeFloat() throws Exception
	{
		assertEquals("-10.5", this.getOutput("deserializeNegativeFloat.html"));
	}
	
	/**
	 * testDeserializeScientificNotation
	 * 
	 * @throws Exception
	 */
	public void testDeserializeScientificNotation() throws Exception
	{
		assertEquals("1.5e+50", this.getOutput("deserializeScientificNotation.html"));
	}
	
	/**
	 * testDeserializeNegativeScientificNotation
	 * 
	 * @throws Exception
	 */
	public void testDeserializeNegativeScientificNotation() throws Exception
	{
		assertEquals("-1.5e+50", this.getOutput("deserializeNegativeScientificNotation.html"));
	}
	
	/**
	 * testDeserializeNegativeScientificNotation2
	 * 
	 * @throws Exception
	 */
	public void testDeserializeNegativeScientificNotation2() throws Exception
	{
		assertEquals("-1.5e-50", this.getOutput("deserializeNegativeScientificNotation2.html"));
	}
	
	/**
	 * testDeserializeInfinity
	 * 
	 * @throws Exception
	 */
	public void testDeserializeInfinity() throws Exception
	{
		assertEquals("Infinity", this.getOutput("deserializeInfinity.html"));
	}
	
	/**
	 * testDeserializeNaN
	 * 
	 * @throws Exception
	 */
	public void testDeserializeNaN() throws Exception
	{
		assertEquals("NaN", this.getOutput("deserializeNaN.html"));
	}
}
