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

import net.sourceforge.groboutils.junit.v1.MultiThreadedTestRunner;
import net.sourceforge.groboutils.junit.v1.TestRunnable;

import com.gargoylesoftware.htmlunit.WebClient;

/**
 * @author Kevin Lindsey
 */
public abstract class AbstractThreadedTest extends AbstractTest
{
	private static final String HTMLUNIT_REQUEST_COUNT = "HTMLUNIT_REQUEST_COUNT";
	private static final String HTMLUNIT_THREAD_COUNT = "HTMLUNIT_THREAD_COUNT";
	
	/**
	 * @author Kevin Lindsey
	 */
	private class WorkerThread extends TestRunnable
	{
		private String _name;

		/**
		 * WorkerThread
		 * 
		 * @param name
		 */
		public WorkerThread(String name)
		{
			this._name = name;
		}

		/**
		 * Cause thread to sleep for a random amount of time that is somewhere between 0.0 and 0.5 seconds
		 * 
		 * @throws Throwable
		 */
		private void doRandomSleep() throws Throwable
		{
			long l;

			l = Math.round(Math.random() * 50);
			Thread.sleep(l * 10);
		}

		/**
		 * getName
		 * 
		 * @return String
		 */
		public String getName()
		{
			return this._name;
		}

		/**
		 * This is where the real test takes place.
		 */
		public void runTest() throws Throwable
		{
			final WebClient webClient = getWebClient();

			for (int i = 0; i < JaxerHtmlTestUtil.getRequestCount(); i++)
			{
				doRandomSleep();
				performTest(webClient);
			}
		}
	}

	/**
	 * get base URL as a string
	 * 
	 * @return String
	 */
	protected String getBaseURLString()
	{
		return super.getBaseURLString() + this.getTestName();
	}

	/**
	 * getTestName
	 * 
	 * @return String
	 */
	protected abstract String getTestName();

	/**
	 * Sub-classes perform the actual test here
	 * 
	 * @throws Exception
	 */
	protected abstract void performTest(WebClient webClient) throws Exception;

	/**
	 * setUp
	 */
	@Override
	protected void setUp() throws Exception
	{
		super.setUp();
		
		// verify needed environment variables exist
		verifyEnvironment(new String[] {
			HTMLUNIT_REQUEST_COUNT,
			HTMLUNIT_THREAD_COUNT
		});
	}

	/**
	 * This is where JUnit calls into our test. We will use the MultiThreadedTestRunner here. The MTTR takes an array of
	 * TestRunnable objects as parameters in its constructor. We run the MTTR and it manages all the worker threads
	 * under the covers.
	 */
	public void testHTMLTest() throws Throwable
	{
		// HOw many worker threads should we launch?
		int runnerCount = JaxerHtmlTestUtil.getThreadCount();

		// Instantiate the worker threads
		TestRunnable tcs[] = new TestRunnable[runnerCount];

		for (int i = 0; i < runnerCount; ++i)
		{
			tcs[i] = new WorkerThread(Integer.toString(i));
		}

		// Pass the worker array to the MTTR
		MultiThreadedTestRunner mttr = new MultiThreadedTestRunner(tcs);

		// Kickstart the MTTR & fire off the worker threads
		mttr.runTestRunnables();
	}
}
