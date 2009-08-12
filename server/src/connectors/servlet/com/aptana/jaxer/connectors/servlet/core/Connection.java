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

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.ByteChannel;
import java.nio.channels.SocketChannel;
import java.util.HashMap;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.aptana.jaxer.connectors.servlet.interfaces.ILoggingHandler;

/**
 * @author Kevin Sawicki (ksawicki@aptana.com) (Added documentation)
 */
public class Connection
{

	/**
	 * CONTENT_LENGTH
	 */
	public static final String CONTENT_LENGTH = "Content-Length";

	private boolean _idle = false;
	private boolean _committed = false;
	private Jaxer _jaxer;
	private ByteChannel _channel;
	private BlockParser _parser;
	private BlockGenerator _generator;

	/**
	 * Creates a new connection
	 * 
	 * @param jaxer
	 * @param channel
	 * @throws IOException
	 */
	public Connection(Jaxer jaxer, SocketChannel channel) throws IOException
	{
		_jaxer = jaxer;
		_channel = channel;

		_parser = new BlockParser(_channel, _jaxer.getBuffer());
		_generator = new BlockGenerator(_channel, _jaxer.getBuffer());

		// _generator.sendHello();
		//
		// Block.Type block = _parser.nextBlock();
		// assert block == Block.Type.Hello;
		_idle = true;
	}

	/**
	 * Sets the connection as a handler
	 * 
	 * @param handler
	 */
	public void setHandler(boolean handler)
	{
		if (_generator != null)
		{
			_generator.setHandler(handler);
		}
	}

	/**
	 * Is the connection a handler?
	 * 
	 * @return - true if handler, false if filter
	 */
	public boolean isHandler()
	{
		if (_generator != null)
		{
			return _generator.isHandler();
		}
		return false;
	}

	/**
	 * @return the idle
	 */
	public boolean isIdle()
	{
		return _idle;
	}

	/**
	 * @return the committed
	 */
	public boolean isCommitted()
	{
		return _committed;
	}

	/**
	 * Flushes
	 * 
	 * @throws IOException
	 */
	public void flush() throws IOException
	{
		_generator.flush();
	}

	/**
	 * Sends the begin request
	 * 
	 * @throws IOException
	 */
	public void sendBeginRequest() throws IOException
	{
		assert _idle;
		assert !_committed;
		_idle = false;
		_generator.sendBeginRequest();
		Block.Type block = _parser.nextBlock();
		if (block == Block.Type.BeginRequest)
		{
			ByteArrayOutputStream stream = new ByteArrayOutputStream();
			_parser.writeTo(stream);
			byte[] bytes = stream.toByteArray();
			if (_jaxer != null && _jaxer.getLogger() != null)
			{
				ILoggingHandler logger = _jaxer.getLogger();
				if (bytes.length >= 3)
				{
					logger
							.logInfo("Jaxer protocol:" + bytes[0] + "" + bytes[1] + " Jaxer handle bit:" + bytes[2],
									null);
				}
				if (bytes.length >= 5)
				{
					logger.logError("Jaxer error code:" + bytes[3] + "" + bytes[4], new IOException());
				}
				if (bytes.length >= 8)
				{
					logger.logError("Jaxer error message" + new String(bytes, 7, bytes.length - 7), new IOException());
				}
			}
			if (bytes.length >= 3)
			{
				if (bytes[2] == 0)
				{
					throw new IOException("Jaxer protocol not compatible");
				}
			}
			else
			{
				throw new IOException("Jaxer begin request did not have an accept bit in the block");
			}
		}
		else
		{
			throw new IOException("Begin request block not returned by Jaxer");
		}

	}

	/**
	 * Sends the end request
	 * 
	 * @throws IOException
	 */
	public void sendEndRequest() throws IOException
	{
		assert !_idle;
		_generator.sendEndRequest();
	}

	/**
	 * Adds document data
	 * 
	 * @param data
	 * @param offset
	 * @param length
	 * @throws IOException
	 */
	public void addDocumentData(byte[] data, int offset, int length) throws IOException
	{
		assert !_idle;

		if (_generator.addDocumentData(data, offset, length))
		{
			_committed = true;
		}
	}

	/**
	 * Resets the buffer
	 */
	public void resetBuffer()
	{
		if (_committed)
		{
			throw new IllegalStateException();
		}
		if (_generator.getCurrentBlock() == Block.Type.Document)
		{
			_generator.reset();
		}
	}

	/**
	 * Streams a response
	 * 
	 * @param request
	 * @param response
	 * @param file
	 * @throws IOException
	 */
	public void streamResponse(HttpServletRequest request, HttpServletResponse response, File file) throws IOException
	{
		Map<String, Object> headers = new HashMap<String, Object>();
		Block.Type block = _parser.nextBlock();
		while (block != Block.Type.EndRequest)
		{
			switch (block)
			{
				case ResponseHeader:
					_parser.getHeaders(headers);
					for (String name : headers.keySet())
					{
						Object value = headers.get(name);

						if ("status".equalsIgnoreCase(name)) //$NON-NLS-1$
						{
							String v = value.toString();
							int space = v.indexOf(' ');
							if (space > 0)
							{
								response.setStatus(Integer.parseInt(v.substring(0, space)));
							}
							else
							{
								response.setStatus(Integer.parseInt(value.toString()));
							}
						}
						else if (CONTENT_LENGTH.equalsIgnoreCase(name))
						{
							String v = value.toString();
							int length = Integer.parseInt(v);
							response.setContentLength(length);
						}
						else if (value instanceof String[])
						{
							for (String v : (String[]) value)
							{
								if (v != null && v.equals(""))
								{
									response.setHeader(name, v);
								}
								else
								{
									response.addHeader(name, v);
								}
							}
						}
						else if (value != null)
						{
							if (value.equals(""))
							{
								response.setHeader(name, (String) value);
							}
							else
							{
								response.addHeader(name, (String) value);
							}
						}
					}
					headers.clear();
					break;

				case Document:
				{
					_parser.writeTo(response.getOutputStream());
					break;
				}

				case RequirePostData:
				{
					if (file != null)
					{
						if (file.length() > 0)
						{
							_generator.addPostData(new FileOutputStream(file).getChannel());
						}
						else
						{
							_generator.addBlockHeader(Block.Type.PostData, 0);
							_generator.flush();
						}
					}
					else
					{
						if (request.getContentLength() != 0)
						{
							_generator.addPostData(request.getInputStream());
						}
						else
						{
							_generator.addEmptyPostData();
						}
					}
					_generator.sendEndRequest();
					break;
				}
				default:
					break;
			}
			block = _parser.nextBlock();
		}
		_committed = false;
		_idle = true;
	}

	/**
	 * Adds a header
	 * 
	 * @param name
	 * @param values
	 * @throws IOException
	 */
	public void addHeader(String name, Object values) throws IOException
	{
		assert !_idle;
		_generator.addHeader(name, values);
	}

	/**
	 * Adds a response header
	 * 
	 * @param name
	 * @param value
	 * @throws IOException
	 */
	public void addResponseHeader(String name, String value) throws IOException
	{
		assert !_idle;
		_generator.addResponseHeader(name, value);
	}

	/**
	 * Add environment block
	 * 
	 * @param name
	 * @param value
	 * @throws IOException
	 */
	public void addEnv(String name, String value) throws IOException
	{
		assert !_idle;
		_generator.addEnvironment(name, value);
	}

	/**
	 * Gets the channel
	 * 
	 * @return - byte channel
	 */
	public ByteChannel getChannel()
	{
		return _channel;
	}

	/**
	 * Destroys
	 */
	public synchronized void destroy()
	{
		_idle = false;
		if (_parser != null)
		{
			_jaxer.returnBuffer(_parser._buffer);
			_parser._buffer = null;
			_parser = null;
			_jaxer.returnBuffer(_generator._buffer);
			_generator._buffer = null;
			_generator = null;
		}
		if (_channel != null)
		{
			try
			{
				_channel.close();
			}
			catch (IOException e)
			{
			}
		}
	}

}
