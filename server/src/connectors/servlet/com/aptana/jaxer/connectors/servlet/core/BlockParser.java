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
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.channels.ByteChannel;
import java.util.Collection;
import java.util.Map;

/**
 * Block Parser
 * 
 * @author Kevin Sawicki (ksawicki@aptana.com) - (Added documentation)
 */
public class BlockParser
{
	ByteChannel _channel;
	ByteBuffer _buffer;
	Block.Type _block;
	int _length;
	int _consumed;

	/**
	 * Block parser constructor
	 * 
	 * @param channel
	 * @param buffer
	 */
	public BlockParser(ByteChannel channel, ByteBuffer buffer)
	{
		_buffer = buffer;
		_buffer.clear();
		_buffer.flip();
		_channel = channel;
	}

	/**
	 * Gets the next block
	 * 
	 * @return - block
	 * @throws IOException
	 */
	public Block.Type nextBlock() throws IOException
	{
		// skip any un-consumed parts of the last block
		while (_block != null)
		{
			int needed = _length - _consumed;
			if (needed == 0)
			{
				_block = null;
				_length = 0;
				_consumed = 0;
				break;
			}
			int remaining = ensureBytes(1);
			int have = (remaining < needed) ? remaining : needed;
			_consumed += have;
			_buffer.position(_buffer.position() + have);
		}

		// Get the next header
		ensureBytes(3);
		_block = Block.Type.fromOrdinal(_buffer.get());
		_length = _buffer.getChar();

		return _block;
	}

	/**
	 * Gets the data remaining
	 * 
	 * @return - num remaining
	 */
	public int dataRemaining()
	{
		return _length - _consumed;
	}

	/**
	 * Gets the number
	 * 
	 * @return - number
	 * @throws IOException
	 */
	public int getNumber() throws IOException
	{
		assert _block != null;
		assert dataRemaining() >= Block.NUMBER_SIZE;

		ensureBytes(Block.NUMBER_SIZE);
		int n = _buffer.getChar();
		_consumed += Block.NUMBER_SIZE;
		return n;
	}

	/**
	 * Gets a string of certain size
	 * 
	 * @param size
	 * @return string
	 * @throws IOException
	 */
	public String getString(int size) throws IOException
	{
		assert _block != null;
		assert dataRemaining() >= size;
		assert size < _buffer.capacity();
		ensureBytes(size);

		String s = new String(_buffer.array(), _buffer.position(), size);
		_buffer.position(_buffer.position() + size);
		_consumed += size;
		return s;
	}

	/**
	 * Fills an environment map
	 * 
	 * @param map
	 * @throws IOException
	 */
	public void getEnvironment(Map<String, String> map) throws IOException
	{
		assert _block == Block.Type.Environment;
		assert dataRemaining() >= Block.NUMBER_SIZE;

		int tupples = getNumber();
		while (tupples-- > 0)
		{
			int name_length = getNumber();
			String name = getString(name_length);
			int value_length = getNumber();
			String value = getString(value_length);
			map.put(name, value);
		}
	}

	/**
	 * @param map
	 *            Map to be filled with String keys and values of either Strings or Arrays of Strings.
	 * @throws IOException
	 */
	public void getHeaders(Map<String, Object> map) throws IOException
	{
		assert _block == Block.Type.ResponseHeader;
		assert dataRemaining() >= Block.NUMBER_SIZE;

		int tupples = getNumber();
		while (tupples-- > 0)
		{
			int name_length = getNumber();
			String name = getString(name_length);
			int value_length = getNumber();
			String value = getString(value_length);

			Object existing = map.get(name);
			if (existing instanceof Collection)
			{
				Collection c = (Collection) existing;
				String[] n = new String[c.size() + 1];
				int i = 0;
				for (Object v : c)
				{
					n[i++] = v.toString();
				}
				n[i] = value;
				map.put(name, n);
			}
			else if (existing instanceof String[])
			{
				String[] a = (String[]) existing;
				String[] n = new String[a.length + 1];
				System.arraycopy(a, 0, n, 0, a.length);
				n[a.length] = value;
				map.put(name, n);
			}
			else if (existing != null)
			{
				String[] n = { existing.toString(), value };
				map.put(name, n);
			}
			else
			{
				map.put(name, value);
			}
		}
	}

	/**
	 * Writes to an output stream
	 * 
	 * @param out
	 * @throws IOException
	 */
	public void writeTo(OutputStream out) throws IOException
	{
		assert _block != null;
		int remaining = dataRemaining();
		while (remaining > 0)
		{
			ensureBytes(1);

			int have = _buffer.remaining();
			int chunk = have > remaining ? remaining : have;

			out.write(_buffer.array(), _buffer.position(), chunk);
			_buffer.position(_buffer.position() + chunk);
			_consumed += chunk;
			remaining = dataRemaining();
		}
	}

	/**
	 * Writes to a byte channel
	 * 
	 * @param out
	 * @throws IOException
	 */
	public void writeTo(ByteChannel out) throws IOException
	{
		assert _block != null;
		int remaining = dataRemaining();
		while (remaining > 0)
		{
			ensureBytes(1);

			int have = _buffer.remaining();
			if (have > remaining)
			{
				int limit = _buffer.limit();
				_buffer.limit(limit - have + remaining);
				_consumed += out.write(_buffer);
				_buffer.limit(limit);
			}
			else
			{
				_consumed += out.write(_buffer);
			}

			remaining = dataRemaining();
		}
	}

	private int ensureBytes(int atleast) throws IOException
	{
		int remaining = _buffer.remaining();
		while (remaining < atleast)
		{
			int read = fill();
			if (read < 0)
			{
				throw new IOException("EOF"); //$NON-NLS-1$
			}
			remaining = _buffer.remaining();
		}
		return remaining;
	}

	private int fill() throws IOException
	{
		_buffer.compact();
		int read = _channel.read(_buffer);
		_buffer.flip();
		return read;
	}

}
