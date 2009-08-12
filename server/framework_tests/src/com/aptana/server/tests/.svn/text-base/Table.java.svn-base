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

import java.sql.SQLException;

/**
 * @author Kevin Lindsey
 */
public class Table
{
	private Database _owningDatabase;
	private String _name;

	/**
	 * Table
	 * 
	 * @param owningDatabase
	 * @param name
	 */
	public Table(Database owningDatabase, String name)
	{
		this._owningDatabase = owningDatabase;
		this._name = name;
	}

	/**
	 * get name
	 * 
	 * @return String
	 */
	public String getName()
	{
		return this._name;
	}

	/**
	 * get owning database
	 * 
	 * @return Database
	 */
	public Database getOwningDatabase()
	{
		return this._owningDatabase;
	}

	/**
	 * get row count
	 * 
	 * @return row count
	 * @throws SQLException
	 */
	public int getRowCount() throws SQLException
	{
		return this._owningDatabase.getRowCount(this._name);
	}

	/**
	 * truncate
	 * 
	 * @throws SQLException
	 */
	public void truncate() throws SQLException
	{
		this._owningDatabase.doTruncate(this._name);
	}
}
