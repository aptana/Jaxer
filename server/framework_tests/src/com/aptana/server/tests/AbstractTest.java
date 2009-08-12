/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;

import junit.framework.TestCase;

import org.jaxen.XPath;

import com.gargoylesoftware.htmlunit.WebClient;
import com.gargoylesoftware.htmlunit.html.HtmlPage;
import com.gargoylesoftware.htmlunit.html.xpath.HtmlUnitXPath;

/**
 * @author Kevin Lindsey
 */
public abstract class AbstractTest extends TestCase
{
	private static final String HTMLUNIT_SERVER_NAME = "HTMLUNIT_SERVER_NAME";
	private static final String HTMLUNIT_SERVER_PORT = "HTMLUNIT_SERVER_PORT";
	
	protected org.apache.log4j.Logger logger = null;

	/**
	 * AbstractTest
	 */
	public AbstractTest()
	{
	}

	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		StringBuffer buffer = new StringBuffer();
		String port = this.getServerPort();

		buffer.append("http://");
		buffer.append(this.getServerName());
		
		// only add non-standard ports
		if ("80".equals(port) == false)
		{
			buffer.append(":");
			buffer.append(port);
		}
	
		buffer.append("/htmlunit/");

		return buffer.toString();
	}

	/**
	 * getServerName
	 * 
	 * @return String
	 */
	protected String getServerName()
	{
		return System.getenv(HTMLUNIT_SERVER_NAME);
	}
	
	/**
	 * getServerPort
	 * 
	 * @return String
	 */
	protected String getServerPort()
	{
		return System.getenv(HTMLUNIT_SERVER_PORT);
	}
	
	/**
	 * getScriptElements
	 * 
	 * @param url
	 * @param index
	 * @return
	 * @throws Exception
	 */
	private List getScriptElements(URL url, int index) throws Exception
	{
		WebClient webClient = this.getWebClient();
		HtmlPage page = (HtmlPage) webClient.getPage(url);

		// get last script element
		XPath path = new HtmlUnitXPath("//script[position() > " + index + "]");

		return path.selectNodes(page);
	}
	
	/**
	 * getScriptElements
	 * 
	 * @param url
	 * @return List
	 * @throws Exception
	 */
	protected List getScriptElements(URL url) throws Exception
	{
		return this.getScriptElements(url, 2);
	}
	
	/**
	 * getScriptElementsWithProxy
	 * 
	 * @param url
	 * @return
	 * @throws Exception
	 */
	protected List getScriptElementsWithProxy(URL url) throws Exception
	{
		return this.getScriptElements(url, 3);
	}

	/**
	 * getURL
	 * 
	 * @return URL
	 * @throws MalformedURLException
	 */
	protected URL getURL(String pageName) throws MalformedURLException
	{
		String urlString = this.getBaseURLString() + pageName;

		logger.info("URL in use is: " + urlString);

		return new URL(urlString);
	}

	/**
	 * getWebClient
	 * 
	 * @return WebClient
	 * @throws Exception
	 */
	protected WebClient getWebClient() throws Exception
	{
		WebClient result = new WebClient();

		// set timeout
		result.setTimeout(JaxerHtmlTestUtil.getWebClientTimeout());
		
		// turn off JS
		result.setJavaScriptEnabled(false);

		return result;
	}

	/**
	 * setUp
	 */
	protected void setUp() throws Exception
	{
		// create logger
		org.apache.log4j.BasicConfigurator.configure();
		logger = JaxerHtmlTestUtil.getLogger();

		// verify needed environment variables exist
		verifyEnvironment(new String[] {
			HTMLUNIT_SERVER_NAME,
			HTMLUNIT_SERVER_PORT
		});
	}
	
	/**
	 * verifyEnvironment
	 * 
	 * @param names
	 * @throws Exception 
	 */
	protected void verifyEnvironment(String[] names) throws Exception
	{
		StringBuffer buffer = new StringBuffer();

		for (int i = 0; i < names.length; i++)
		{
			String name = names[i];

			if (System.getenv(name) == null)
			{
				buffer.append("Environment variable not defined: ");
				buffer.append(name);
				buffer.append("\n");
			}
		}

		String message = buffer.toString();

		if (message != null && message.length() > 0)
		{
			throw new Exception(message);
		}
	}
}
