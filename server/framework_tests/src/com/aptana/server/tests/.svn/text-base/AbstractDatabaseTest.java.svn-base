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

/**
 * @author Kevin Lindsey
 */
public class AbstractDatabaseTest extends AbstractTest
{
	private static final String HTMLUNIT_DB_HOST = "HTMLUNIT_DB_HOST";
	private static final String HTMLUNIT_DB_NAME = "HTMLUNIT_DB_NAME";
	private static final String HTMLUNIT_DB_PASSWORD = "HTMLUNIT_DB_PASSWORD";
	private static final String HTMLUNIT_DB_USER = "HTMLUNIT_DB_USER";
	
	protected Database database;
	
	/**
	 * AbstractDatabaseTest
	 */
	public AbstractDatabaseTest()
	{
	}
	
	/**
	 * create a new Database object
	 * 
	 * @return Database
	 */
	protected Database createDatabase()
	{
		return new Database(
			this.getDatabaseHost(),
			this.getDatabaseName(),
			this.getDatabaseUser(),
			this.getDatabasePassword()
		);
	}
	
	/**
	 * getDatabaseHost
	 * 
	 * @return String
	 */
	protected String getDatabaseHost()
	{
		return System.getenv(HTMLUNIT_DB_HOST);
	}

	/**
	 * getDatabaseName
	 * 
	 * @return String
	 */
	protected String getDatabaseName()
	{
		return System.getenv(HTMLUNIT_DB_NAME);
	}

	/**
	 * getDatabasePassword
	 * 
	 * @return String
	 */
	protected String getDatabasePassword()
	{
		return System.getenv(HTMLUNIT_DB_PASSWORD);
	}

	/**
	 * getDatabaseUser
	 * 
	 * @return String
	 */
	protected String getDatabaseUser()
	{
		return System.getenv(HTMLUNIT_DB_USER);
	}
	
	/**
	 * setUp
	 */
	@Override
	protected void setUp() throws Exception
	{
		super.setUp();
		
		// verify needed environment variables exist
		verifyEnvironment(new String[] {
			HTMLUNIT_DB_HOST,
			HTMLUNIT_DB_NAME,
			HTMLUNIT_DB_PASSWORD,
			HTMLUNIT_DB_USER
		});
		
		// create database
		this.database = this.createDatabase();
		
		// connect to database
		this.database.startup();
		
		// make sure callback table is empty
		this.database.getCallbackPageTable().truncate();
	}

	/**
	 * tearDown
	 */
	@Override
	protected void tearDown() throws Exception
	{
		this.database.shutdown();
		
		super.tearDown();
	}
}
