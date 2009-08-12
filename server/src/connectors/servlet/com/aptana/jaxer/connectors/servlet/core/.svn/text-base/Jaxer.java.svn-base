/** ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 * 
 *  This program is distributed in the hope that it will be useful, but
 *  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 *  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 *  is prohibited.
 * 
 *  You can redistribute and/or modify this program under the terms of the GPL, 
 *  as published by the Free Software Foundation.  You should
 *  have received a copy of the GNU General Public License, Version 3 along
 *  
 *  with this program; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *  
 *  Aptana provides a special exception to allow redistribution of this file
 *  with certain other code and certain additional terms
 *  pursuant to Section 7 of the GPL. You may view the exception and these
 *  terms on the web at http://www.aptana.com/legal/gpl/.
 *  
 *  You may view the GPL, and Aptana's exception and additional terms in the file
 *  titled license-jaxer.html in the main distribution folder of this program.
 *  
 *  Any modifications to this file must keep this entire header intact.
 * 
 * ***** END LICENSE BLOCK ***** */
package com.aptana.jaxer.connectors.servlet.core;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.aptana.jaxer.connectors.servlet.interfaces.ILoggingHandler;

/**
 * Jaxer class
 */
public class Jaxer
{
	// TODO make lots of this configurable
	private static final int BUFFER_SIZE = 8192;
	private static final int JAXER_PORT = 4327;

	private SocketAddress _server;
	private Queue<Connection> connectionQueue = new ConcurrentLinkedQueue<Connection>();
	private Queue<ByteBuffer> _bufferQ = new ConcurrentLinkedQueue<ByteBuffer>();

	private ILoggingHandler logger;

	/**
	 * Private default constructor
	 */
	private Jaxer()
	{
		// Does nothing
	}

	/**
	 * Creates a new jaxer
	 * 
	 * @param hostname
	 * @param port
	 */
	public Jaxer(String hostname, int port)
	{
		_server = new InetSocketAddress(hostname, port);
	}

	/**
	 * Creates a new jaxer with the default port
	 * 
	 * @param hostname
	 */
	public Jaxer(String hostname)
	{
		this(hostname, JAXER_PORT);
	}

	/**
	 * getBuffer
	 * 
	 * @return - byte buffer
	 */
	public ByteBuffer getBuffer()
	{
		ByteBuffer buffer = _bufferQ.poll();
		if (buffer == null)
		{
			buffer = ByteBuffer.allocate(BUFFER_SIZE);
		}
		return buffer;
	}

	/**
	 * Clears the connection queue
	 */
	public void clearConnections()
	{
		for (Connection c : connectionQueue)
		{
			c.destroy();
		}
		connectionQueue.clear();
	}

	/**
	 * returnBuffer
	 * 
	 * @param b
	 */
	public void returnBuffer(ByteBuffer b)
	{
		_bufferQ.offer(b);
	}

	/**
	 * newConnection
	 * 
	 * @return - connection
	 * @throws IOException
	 */
	public Connection newConnection() throws IOException
	{
		// TODO make lots of this configurable

		SocketChannel channel = SocketChannel.open();
		channel.configureBlocking(true);
		channel.socket().setKeepAlive(true);
		channel.socket().setSoLinger(false, 0);
		channel.socket().setSoTimeout(5000);
		channel.socket().setTcpNoDelay(true);
		channel.socket().setPerformancePreferences(0, 1, 0);

		if (channel.connect(_server))
		{
			return new Connection(this, channel);
		}
		throw new IllegalStateException();
	}

	/**
	 * returnConnection
	 * 
	 * @param c
	 */
	public void returnConnection(Connection c)
	{
		if (c.getChannel().isOpen())
		{
			connectionQueue.offer(c);
		}
		else
		{
			c.destroy();
		}
	}

	/**
	 * getVersion
	 * 
	 * @return - version
	 */
	public int getVersion()
	{
		return BlockGenerator.VERSION;
	}

	/**
	 * Get a new or recycled Connection. The health of the connection is checked by sending beginRequest block.
	 * 
	 * @param isHandler
	 * @return Open connection after beginRequest sent.
	 * @throws IOException
	 */
	public Connection beginRequest(boolean isHandler) throws IOException
	{
		boolean tried_new = false;
		while (true)
		{
			Connection c = connectionQueue.poll();
			if (c == null)
			{
				c = newConnection();
				tried_new = true;
			}

			try
			{
				c.setHandler(isHandler);
				c.sendBeginRequest();
				if (!c.getChannel().isOpen())
				{
					throw new IOException("EOF"); //$NON-NLS-1$
				}
				return c;
			}
			catch (IOException e)
			{
				c.destroy();
				if (tried_new)
				{
					throw e;
				}
			}
		}
	}

	/**
	 * Get a new or recycled Connection. The health of the connection is checked by sending beginRequest block.
	 * 
	 * @return Open connection after beginRequest sent.
	 * @throws IOException
	 */
	public Connection beginRequest() throws IOException
	{
		return beginRequest(false);
	}

	/**
	 * @return the logger
	 */
	public ILoggingHandler getLogger()
	{
		return logger;
	}

	/**
	 * @param logger
	 *            the logger to set
	 */
	public void setLogger(ILoggingHandler logger)
	{
		this.logger = logger;
	}

}
