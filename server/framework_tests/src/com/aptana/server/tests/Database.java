/**
 * Copyright (c) 2005-2006 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Date;

/**
 * @author Kevin Lindsey
 */
public class Database
{
	private static final String driver = "com.mysql.jdbc.Driver";
	private static final String protocol = "jdbc:mysql";

	private String _host;
	private String _database;
	private String _user;
	private String _password;
	private Connection _connection;
	
	private CallbackPageTable _callbackPageTable;

	/**
	 * Database
	 * 
	 * @param configuration
	 */
	public Database(String host, String database, String user, String password)
	{
		this._host = host;
		this._database = database;
		this._user = user;
		this._password = password;
	}

	/**
	 * connect
	 * 
	 * @return Connection
	 */
	public Connection connect()
	{
		if (this._connection == null)
		{
			String connectionString = protocol + "://" + this._host + "/" + this._database;
			String user = this._user;
			String password = this._password;

			try
			{
				Class.forName(driver);
				this._connection = (Connection) DriverManager.getConnection(connectionString, user, password);
			}
			catch (ClassNotFoundException e)
			{
				e.printStackTrace();
			}
			catch (SQLException e)
			{
				e.printStackTrace();
			}
		}

		return this._connection;
	}

	/**
	 * createTables
	 */
	protected void createTables()
	{
		this._callbackPageTable = new CallbackPageTable(this);
	}
	
	/**
	 * disconnect
	 */
	public void disconnect()
	{
		if (this._connection != null)
		{
			try
			{
				this._connection.close();
			}
			catch (SQLException e)
			{
				e.printStackTrace();
			}

			this._connection = null;
		}
	}

	/**
	 * doCommand
	 * 
	 * @param query
	 * @throws SQLException
	 */
	public void doCommand(String query) throws SQLException
	{
		Connection connection = this.getConnection();
		Statement stmt = null;

		try
		{
			stmt = (Statement) connection.createStatement();

			stmt.executeQuery(query);
		}
		finally
		{
			if (stmt != null)
			{
				try
				{
					stmt.close();
				}
				catch (SQLException sqlEx)
				{
				}
			}
		}
	}

	/**
	 * doDelete
	 * 
	 * @param table
	 * @param where
	 * @throws SQLException
	 */
	public void doDelete(String table, String where) throws SQLException
	{
		String delete = "DELETE FROM " + table + " WHERE " + where;

		this.doUpdate(delete);
	}

	/**
	 * doUpdate
	 * 
	 * @param table
	 * @param fields
	 * @param values
	 * @throws SQLException
	 */
	public void doInsert(String table, String fields, String values) throws SQLException
	{
		String insert = "INSERT INTO " + table + "(" + fields + ")" + " VALUES(" + values + ")";

		this.doUpdate(insert);
	}

	/**
	 * doTruncate
	 * 
	 * @param table
	 * @throws SQLException
	 */
	public void doTruncate(String table) throws SQLException
	{
		String truncate = "TRUNCATE " + table;

		this.doUpdate(truncate);
	}

	/**
	 * doInsert
	 * 
	 * @param query
	 * @throws SQLException
	 */
	public void doUpdate(String query) throws SQLException
	{
		Connection connection = this.getConnection();
		Statement stmt = null;

		try
		{
			stmt = (Statement) connection.createStatement();

			stmt.executeUpdate(query);
		}
		finally
		{
			if (stmt != null)
			{
				try
				{
					stmt.close();
				}
				catch (SQLException sqlEx)
				{
				}
			}
		}
	}

	/**
	 * doWithEach
	 * 
	 * @param query
	 * @param processor
	 * @throws SQLException
	 */
	public void doWithEach(String query, IResultProcessor processor) throws SQLException
	{
		if (processor == null)
		{
			throw new IllegalArgumentException("processor must be defined");
		}

		Statement statement = null;
		ResultSet resultSet = null;

		try
		{
			statement = getConnection().createStatement();
			resultSet = statement.executeQuery(query);

			while (resultSet.next())
			{
				processor.processResult(resultSet);
			}
		}
		finally
		{
			if (resultSet != null)
			{
				try
				{
					resultSet.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}

			if (statement != null)
			{
				try
				{
					statement.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}
		}
	}

	/**
	 * getCallbackPageTable
	 * 
	 * @return CallbackPageTable
	 */
	public CallbackPageTable getCallbackPageTable()
	{
		return this._callbackPageTable;
	}
	
	/**
	 * getConnection
	 * 
	 * @return Connection
	 */
	public Connection getConnection()
	{
		return this._connection;
	}

	/**
	 * getDate
	 * 
	 * @param query
	 * @return int
	 * @throws SQLException
	 */
	public Date getDate(String query) throws SQLException
	{
		return this.getDate(query, 1);
	}

	/**
	 * getDate
	 * 
	 * @param query
	 * @param index
	 * @return int
	 * @throws SQLException
	 */
	public Date getDate(String query, int index) throws SQLException
	{
		Statement st = null;
		ResultSet rs = null;
		Date result = null;

		try
		{
			st = this.getConnection().createStatement();
			rs = st.executeQuery(query);

			if (rs.next())
			{
				result = rs.getDate(index);
			}
		}
		finally
		{
			if (rs != null)
			{
				try
				{
					rs.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}

			if (st != null)
			{
				try
				{
					st.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}
		}

		return result;
	}

	/**
	 * getDouble
	 * 
	 * @param query
	 * @return int
	 * @throws SQLException
	 */
	public double getDouble(String query) throws SQLException
	{
		return this.getDouble(query, 1);
	}

	/**
	 * getDouble
	 * 
	 * @param query
	 * @param index
	 * @return int
	 * @throws SQLException
	 */
	public double getDouble(String query, int index) throws SQLException
	{
		Statement st = null;
		ResultSet rs = null;
		double result = 0;

		try
		{
			st = this.getConnection().createStatement();
			rs = st.executeQuery(query);

			if (rs.next())
			{
				result = rs.getDouble(index);
			}
		}
		finally
		{
			if (rs != null)
			{
				try
				{
					rs.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}

			if (st != null)
			{
				try
				{
					st.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}
		}

		return result;
	}

	/**
	 * getInt
	 * 
	 * @param query
	 * @return int
	 * @throws SQLException
	 */
	public int getInt(String query) throws SQLException
	{
		return this.getInt(query, 1);
	}

	/**
	 * getInt
	 * 
	 * @param query
	 * @param index
	 * @return int
	 * @throws SQLException
	 */
	public int getInt(String query, int index) throws SQLException
	{
		Statement st = null;
		ResultSet rs = null;
		int result = 0;

		try
		{
			st = this.getConnection().createStatement();
			rs = st.executeQuery(query);

			if (rs.next())
			{
				result = rs.getInt(index);
			}
		}
		finally
		{
			if (rs != null)
			{
				try
				{
					rs.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}

			if (st != null)
			{
				try
				{
					st.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}
		}

		return result;
	}
	
	/**
	 * getLong
	 * 
	 * @param query
	 * @return long
	 * @throws SQLException
	 */
	public long getLong(String query) throws SQLException
	{
		return this.getLong(query, 1);
	}
	
	/**
	 * getLong
	 * 
	 * @param query
	 * @param index
	 * @return long
	 * @throws SQLException
	 */
	public long getLong(String query, int index) throws SQLException
	{
		Statement st = null;
		ResultSet rs = null;
		long result = 0;
		
		try
		{
			st = this.getConnection().createStatement();
			rs = st.executeQuery(query);
			
			if (rs.next())
			{
				result = rs.getLong(index);
			}
		}
		finally
		{
			if (rs != null)
			{
				try
				{
					rs.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}
			
			if (st != null)
			{
				try
				{
					st.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}
		}
		
		return result;
	}

	/**
	 * getLastInsertId
	 * 
	 * @return int
	 * @throws SQLException
	 */
	public int getLastInsertId() throws SQLException
	{
		return this.getInt("SELECT LAST_INSERT_ID()");
	}

	/**
	 * get row count of table
	 * 
	 * @param table
	 * @return row count
	 * @throws SQLException
	 */
	public int getRowCount(String table) throws SQLException
	{
		return getInt("SELECT COUNT(*) FROM " + table);
	}

	/**
	 * getString
	 * 
	 * @param query
	 * @return String
	 * @throws SQLException
	 */
	public String getString(String query) throws SQLException
	{
		return this.getString(query, 1);
	}

	/**
	 * getString
	 * 
	 * @param query
	 * @param index
	 * @return String
	 * @throws SQLException
	 */
	public String getString(String query, int index) throws SQLException
	{
		Statement st = null;
		ResultSet rs = null;
		String result = null;

		try
		{
			st = this.getConnection().createStatement();
			rs = st.executeQuery(query);

			if (rs.next())
			{
				result = rs.getString(index);
			}
		}
		finally
		{
			if (rs != null)
			{
				try
				{
					rs.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}

			if (st != null)
			{
				try
				{
					st.close();
				}
				catch (SQLException e)
				{
					e.printStackTrace();
				}
			}
		}

		return result;
	}

	/**
	 * getNextDate
	 * 
	 * @param date
	 * @return String
	 * @throws SQLException
	 */
	public String getNextDate(String date) throws SQLException
	{
		String query = "SELECT DATE_ADD('" + date + "', INTERVAL 1 DAY)";

		return this.getString(query);
	}
	
	/**
	 * startup
	 */
	public void startup()
	{
		this.connect();
		this.createTables();
	}
	
	/**
	 * shutdown
	 */
	public void shutdown()
	{
		this.disconnect();
	}
}
