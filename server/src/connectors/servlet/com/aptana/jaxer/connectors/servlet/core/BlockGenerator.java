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

import java.io.IOException;
import java.net.URLEncoder;
import java.nio.ByteBuffer;
import java.nio.channels.ByteChannel;
import java.nio.channels.FileChannel;
import java.util.Collection;
import java.util.Enumeration;
import java.util.Map;

import javax.servlet.ServletInputStream;

/**
 * Generate a stream of Jaxer protocol Blocks. The blocks are written and flushed to the ByteChannel using the
 * ByteBuffer provided in the constructor. Methods that are named sendXxx generate the Block and flush it to the
 * channel. Methods that are named addXxx attempt to avoid a flush in order to maximize usage of the buffer. However
 * addXxx methods may flush the buffer if insufficient space is available.
 * 
 * @author gregw
 * @author Kevin Sawicki (ksawicki@aptana.com) - added documentation, chunking of post data
 */
public class BlockGenerator
{

	/**
	 * DESCRIPTION
	 */
	public static final byte[] DESCRIPTION = "Jaxer Java Filter".getBytes(); //$NON-NLS-1$

	/**
	 * VERSION
	 */
	public static final int VERSION = 3;

	/**
	 * HTMLDOCUMENT
	 * 
	 * @deprecated - either handler or filter now
	 */
	public static final int HTMLDOCUMENT = 0;

	/**
	 * HANDLER
	 */
	public static final int HANDLER = 1;

	/**
	 * FILTER
	 */
	public static final int FILTER = 2;

	Block.Type _block;
	ByteBuffer _buffer;
	ByteChannel _channel;
	int _tupples;

	private boolean handler = false;

	/**
	 * Creates a new block generator
	 * 
	 * @param channel
	 * @param buffer
	 */
	public BlockGenerator(ByteChannel channel, ByteBuffer buffer)
	{
		_channel = channel;
		_buffer = buffer;
	}

	/**
	 * Add Document Data.
	 * 
	 * @param bytes
	 * @param offset
	 * @param length
	 * @return true if any data is written.
	 * @throws IOException
	 */
	public boolean addDocumentData(byte[] bytes, int offset, int length) throws IOException
	{
		boolean written = false;
		while (length > Block.MAX_BLOCK_CONTENT)
		{
			written |= addDocumentData(bytes, offset, Block.MAX_BLOCK_CONTENT);
			flush();
			offset += Block.MAX_BLOCK_CONTENT;
			length = length - Block.MAX_BLOCK_CONTENT;
		}

		while (_block != null && _block != Block.Type.Document)
		{
			flush();
		}

		assert length < 0xffff;
		if (_block == null)
		{
			// do a complete flush so we know the position of the header for
			// a lazy write of length
			while (_buffer.position() != 0)
			{
				written = true;
				flush();
			}

			_block = Block.Type.Document;
			_buffer.put(Block.Type.Document.getOrdinal());
			_buffer.position(Block.HEADER_SIZE);
		}
		assert _block == Block.Type.Document;

		if (length > _buffer.remaining())
		{
			int dataSize = length + _buffer.position() - Block.HEADER_SIZE;
			_buffer.putChar(1, (char) dataSize);
			_block = null;
		}

		if (addBytes(bytes, offset, length))
		{
			written = true;
		}

		return written;
	}

	/**
	 * Add Document Data.
	 * 
	 * @param channel
	 * @throws IOException
	 */
	public void addDocumentData(ByteChannel channel) throws IOException
	{
		addData(Block.Type.Document, channel);
	}

	/**
	 * Add environment
	 * 
	 * @param env
	 * @throws IOException
	 */
	public void addEnvironment(Map<String, String> env) throws IOException
	{
		for (String key : env.keySet())
		{
			addHeader(key, env.get(key));
		}
	}

	/**
	 * Add environment
	 * 
	 * @param name
	 * @param value
	 *            a String, Collection of Strings or Array of Strings.
	 * @throws IOException
	 */
	public void addEnvironment(String name, String value) throws IOException
	{
		addTupple(Block.Type.Environment, name, value);
	}

	/**
	 * Add header
	 * 
	 * @param name
	 * @param value
	 *            a String, Collection of Strings or Array of Strings, or an Enumeration
	 * @throws IOException
	 */
	public void addHeader(String name, Object value) throws IOException
	{
		if (value instanceof Collection)
		{
			for (Object v : (Collection) value)
			{
				if (v != null)
				{
					addTupple(Block.Type.RequestHeader, name, v.toString());
				}
			}
		}
		else if (value instanceof Enumeration)
		{
			Enumeration e = (Enumeration) value;
			while (e.hasMoreElements())
			{
				addTupple(Block.Type.RequestHeader, name, (String) e.nextElement());
			}
		}
		else if (value instanceof Object[])
		{
			for (Object v : (Object[]) value)
			{
				if (v != null)
				{
					addTupple(Block.Type.RequestHeader, name, v.toString());
				}
			}
		}
		else if (value != null)
		{
			addTupple(Block.Type.RequestHeader, name, value.toString());
		}
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
		if (value != null)
		{
			addTupple(Block.Type.ResponseHeader, name, value.toString());
		}
	}

	/* ------------------------------------------------------------ */
	/**
	 * @param map
	 *            Map of name to value, Collection of values or Array of values.
	 * @throws IOException
	 */
	public void addHeaders(Map<String, Object> map) throws IOException
	{
		for (String key : map.keySet())
		{
			addHeader(key, map.get(key));
		}
	}

	/**
	 * Add post data
	 * 
	 * @param channel
	 * @throws IOException
	 */
	public void addPostData(FileChannel channel) throws IOException
	{
		addData(Block.Type.PostData, channel);
	}

	/**
	 * Add post data
	 * 
	 * @param parameterMap
	 * @throws IOException
	 */
	public void addPostData(Map<String, String[]> parameterMap) throws IOException
	{
		while (_block != null)
		{
			flush();
		}

		StringBuilder builder = new StringBuilder(256);
		boolean first = true;
		for (String name : parameterMap.keySet())
		{
			if (first)
			{
				first = false;
			}
			else
			{
				builder.append('&');
			}

			for (String value : parameterMap.get(name))
			{
				builder.append(URLEncoder.encode(name, "utf-8")); //$NON-NLS-1$
				builder.append('=');
				builder.append(URLEncoder.encode(value, "utf-8")); //$NON-NLS-1$
			}
		}
		addPostData(builder.toString());
	}

	/**
	 * Add empty post data
	 * 
	 * @throws IOException
	 */
	public void addEmptyPostData() throws IOException
	{
		addBlockHeader(Block.Type.PostData, 0);
	}

	/**
	 * Add post data
	 * 
	 * @param dataS
	 * @throws IOException
	 */
	public void addPostData(String dataS) throws IOException
	{
		byte[] data = dataS.getBytes("utf-8"); //$NON-NLS-1$
		int offset = 0;
		int length = data.length;

		while (length > Block.MAX_BLOCK_CONTENT)
		{
			addBlockHeader(Block.Type.PostData, Block.MAX_BLOCK_CONTENT);
			addBytes(data, offset, Block.MAX_BLOCK_CONTENT);
			flush();
			offset += Block.MAX_BLOCK_CONTENT;
			length = length - Block.MAX_BLOCK_CONTENT;
		}

		if (length > 0)
		{
			addBlockHeader(Block.Type.PostData, length);
			addBytes(data, offset, length);
		}
	}

	/**
	 * Add post data
	 * 
	 * @param inputStream
	 * @throws IOException
	 */
	public void addPostData(ServletInputStream inputStream) throws IOException
	{
		// TODO avoid this temp buffer!
		byte[] chunk = new byte[4096];

		while (true)
		{
			int read = inputStream.read(chunk);
			if (read == -1)
			{
				break;
			}
			addBlockHeader(Block.Type.PostData, read);
			addBytes(chunk, 0, read);
		}
	}

	/**
	 * Flush
	 * 
	 * @throws IOException
	 */
	public void flush() throws IOException
	{
		if (_block != null)
		{
			switch (_block)
			{
				case Environment:
				case RequestHeader:
					_buffer.putChar(Block.HEADER_SIZE, (char) (_tupples));
					_tupples = 0;

				default:
				{
					int dataSize = _buffer.position() - Block.HEADER_SIZE;
					_buffer.putChar(1, (char) dataSize);
				}
			}

			_block = null;
		}

		_buffer.flip();
		_channel.write(_buffer);
		_buffer.compact();
	}

	/**
	 * Get current block
	 * 
	 * @return - block type
	 */
	public Block.Type getCurrentBlock()
	{
		return _block;
	}

	/**
	 */
	public void reset()
	{
		_block = null;
		_buffer.clear();
		_tupples = 0;
	}

	/**
	 * Send begin request
	 * 
	 * @throws IOException
	 */
	public void sendBeginRequest() throws IOException
	{
		assert _block == null;
		addBlockHeader(Block.Type.BeginRequest, 3);
		add16(VERSION);
		if (isHandler())
		{
			addBytes(new byte[] { HANDLER });
		}
		else
		{
			addBytes(new byte[] { FILTER });
		}
		flush();
	}

	/**
	 * Send end request
	 * 
	 * @throws IOException
	 */
	public void sendEndRequest() throws IOException
	{
		assert _block == null;
		addBlockHeader(Block.Type.EndRequest, 0);
		flush();
	}

	/**
	 * Send hello
	 * 
	 * @throws IOException
	 * @deprecated - send hello no longer used is > 1 versions of the protocol
	 */
	public void sendHello() throws IOException
	{
		// assert _block == null;
		// addBlockHeader(Block.Type.Hello, 2 + DESCRIPTION.length);
		// add16(VERSION);
		// addBytes(DESCRIPTION);
		// flush();
	}

	/**
	 * Send require post data
	 * 
	 * @throws IOException
	 */
	public void sendRequirePostData() throws IOException
	{
		assert _block == null;
		addBlockHeader(Block.Type.RequirePostData, 0);
		flush();
	}

	/* ------------------------------------------------------------ */
	private void add16(int bits) throws IOException
	{
		ensureSpace(2);
		_buffer.putChar((char) bits);
	}

	/* ------------------------------------------------------------ */
	void addBlockHeader(Block.Type type, int dataSize) throws IOException
	{
		ensureSpace(3);
		_buffer.put(type.getOrdinal());
		_buffer.putChar((char) dataSize);
	}

	/* ------------------------------------------------------------ */
	private void addBytes(byte[] bytes) throws IOException
	{
		addBytes(bytes, 0, bytes.length);
	}

	/* ------------------------------------------------------------ */
	private boolean addBytes(byte[] bytes, int offset, int length) throws IOException
	{
		boolean written = false;
		while (length > 0)
		{
			int remaining = _buffer.remaining();
			while (remaining == 0)
			{
				flush();
				remaining = _buffer.remaining();
				written = true;
			}
			int chunk = length < remaining ? length : remaining;
			_buffer.put(bytes, offset, chunk);
			offset += chunk;
			length -= chunk;
		}
		return written;
	}

	/* ------------------------------------------------------------ */
	private void addData(Block.Type type, ByteChannel from) throws IOException
	{
		while (_block != null && _block != type)
		{
			flush();
		}

		while (from.isOpen())
		{
			if (_block == null)
			{
				// do a complete flush so we know the position of the header for
				// a lazy write of length
				while (_buffer.position() != 0)
				{
					flush();
				}

				_block = Block.Type.Document;
				_buffer.put(type.getOrdinal());
				_buffer.position(Block.HEADER_SIZE);
			}
			assert _block == type;

			int dataSize = from.read(_buffer);
			if (dataSize < 0)
			{
				_buffer.position(0);
				return;
			}
			_buffer.putChar(1, (char) dataSize);
			_block = null;
			flush();
		}
	}

	/* ------------------------------------------------------------ */
	private void addTupple(Block.Type type, String name, String value) throws IOException
	{
		if (name == null || value == null)
		{
			return;
		}

		byte[] nb = name.getBytes();
		byte[] vb = value.getBytes();

		int length = nb.length + vb.length + 4;
		assert length < 0xffff - Block.HEADER_SIZE - 1;

		while (_block != null && _block != type)
		{
			flush();
		}

		if (_block == null)
		{
			// do a complete flush so we know the position of the header for
			// a lazy write of length
			while (_buffer.position() != 0)
			{
				flush();
			}

			_block = type;
			_buffer.put(type.getOrdinal());
			_buffer.position(Block.HEADER_SIZE + Block.NUMBER_SIZE);
		}
		assert _block == type;

		if (length > _buffer.remaining())
		{
			int dataSize = length + _buffer.position() - Block.HEADER_SIZE;
			_buffer.putChar(1, (char) dataSize);
			_buffer.putChar(Block.HEADER_SIZE, (char) (_tupples + 1));
			_block = null;
			_tupples = 0;
		}
		else
		{
			_tupples++;
		}
		add16(nb.length);
		addBytes(nb);
		add16(vb.length);
		addBytes(vb);
	}

	/* ------------------------------------------------------------ */
	private void ensureSpace(int space) throws IOException
	{
		while (_buffer.remaining() < space)
		{
			flush();
		}
	}

	/**
	 * Is jaxer a filter for the current request?
	 * 
	 * @return - true if a filter
	 */
	public boolean isFilter()
	{
		return !handler;
	}

	/**
	 * Is jaxer a handler for the current request
	 * 
	 * @return - true if a handler
	 */
	public boolean isHandler()
	{
		return handler;
	}

	/**
	 * Sets this block generator as a handler
	 * 
	 * @param handler -
	 *            true if a handler, false if a filter
	 */
	public void setHandler(boolean handler)
	{
		this.handler = handler;
	}
}
