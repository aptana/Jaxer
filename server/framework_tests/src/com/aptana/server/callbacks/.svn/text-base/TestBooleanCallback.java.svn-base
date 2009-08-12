/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.callbacks;

import java.net.URL;

import com.aptana.server.tests.AbstractTest;
import com.gargoylesoftware.htmlunit.WebClient;
import com.gargoylesoftware.htmlunit.html.HtmlElement;
import com.gargoylesoftware.htmlunit.html.HtmlPage;

/**
 * @author Kevin Lindsey
 */
public class TestBooleanCallback extends AbstractTest
{
	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + "html-junit/callbacks/returnValues/";
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
	 * testSerializeDate
	 * 
	 * @throws Exception
	 */
	public void testSerializeDate() throws Exception
	{
		assertEquals("true", this.getOutput("returnBoolean.html"));
	}
}
