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
public class TestNumberSerialization extends AbstractTest
{
	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/serialization/number/";
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
	 * testSerializeInteger
	 * 
	 * @throws Exception
	 */
	public void testSerializeInteger() throws Exception
	{
		assertEquals("10", this.getOutput("serializeInteger.html"));
	}

	/**
	 * testSerializeNegativeInteger
	 * 
	 * @throws Exception
	 */
	public void testSerializeNegativeInteger() throws Exception
	{
		assertEquals("-10", this.getOutput("serializeNegativeInteger.html"));
	}
	
	/**
	 * testSerializeFloat
	 * 
	 * @throws Exception
	 */
	public void testSerializeFloat() throws Exception
	{
		assertEquals("10.5", this.getOutput("serializeFloat.html"));
	}
	
	/**
	 * testSerializeNegativeFloat
	 * 
	 * @throws Exception
	 */
	public void testSerializeNegativeFloat() throws Exception
	{
		assertEquals("-10.5", this.getOutput("serializeNegativeFloat.html"));
	}
	
	/**
	 * testSerializeScientificNotation
	 * 
	 * @throws Exception
	 */
	public void testSerializeScientificNotation() throws Exception
	{
		assertEquals("1.5e+50", this.getOutput("serializeScientificNotation.html"));
	}
	
	/**
	 * testSerializeNegativeScientificNotation
	 * 
	 * @throws Exception
	 */
	public void testSerializeNegativeScientificNotation() throws Exception
	{
		assertEquals("-1.5e+50", this.getOutput("serializeNegativeScientificNotation.html"));
	}
	
	/**
	 * testSerializeNegativeScientificNotation2
	 * 
	 * @throws Exception
	 */
	public void testSerializeNegativeScientificNotation2() throws Exception
	{
		assertEquals("-1.5e-50", this.getOutput("serializeNegativeScientificNotation2.html"));
	}
	
	/**
	 * testSerializeInfinity
	 * 
	 * @throws Exception
	 */
	public void testSerializeInfinity() throws Exception
	{
		assertEquals("Infinity", this.getOutput("serializeInfinity.html"));
	}
	
	/**
	 * testSerializeNaN
	 * 
	 * @throws Exception
	 */
	public void testSerializeNaN() throws Exception
	{
		assertEquals("NaN", this.getOutput("serializeNaN.html"));
	}
}
