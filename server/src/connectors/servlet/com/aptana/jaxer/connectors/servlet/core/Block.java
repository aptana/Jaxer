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

/**
 * Block class
 * 
 * @author Kevin Sawicki (ksawicki@aptana.com) (Added documentation)
 */
public class Block
{
	static final int HEADER_SIZE = 3;

	/**
	 * NUMBER_SIZE
	 */
	public static final int NUMBER_SIZE = 2;

	/**
	 * MAX_BLOCK_CONTENT
	 */
	public static int MAX_BLOCK_CONTENT = 0xffff - HEADER_SIZE;

	/**
	 * Type
	 */
	enum Type
	{
		/**
		 * BeginRequest
		 */
		BeginRequest(0, false),
		/**
		 * RequestHeader
		 */
		RequestHeader(1, true),
		/**
		 * Header
		 */
		ResponseHeader(2, true),
		/**
		 * Environment
		 */
		Environment(3, true),
		/**
		 * Document
		 */
		Document(4, true),
		/**
		 * RequirePostData
		 */
		RequirePostData(5, false),
		/**
		 * PostData
		 */
		PostData(6, true),
		/**
		 * EndRequest
		 */
		EndRequest(7, false),
		/**
		 * Error
		 */
		Error(8, false);

		private byte ordinal;
		private boolean hasBody;

		Type(int b, boolean hasBody)
		{
			this.ordinal = (byte) (0xff & b);
			this.hasBody = hasBody;
		}

		/**
		 * Gets the ordinal byte
		 * 
		 * @return - ordinal byte
		 */
		public byte getOrdinal()
		{
			return ordinal;
		}

		/**
		 * True if has body
		 * 
		 * @return - true if has body
		 */
		public boolean hasBody()
		{
			return hasBody;
		}

		private static Block.Type[] __byordinal = { BeginRequest, RequestHeader, ResponseHeader, Environment, Document,
				RequirePostData, PostData, EndRequest, Error };

		/**
		 * From ordinal
		 * 
		 * @param b
		 * @return - block type
		 */
		public static Block.Type fromOrdinal(byte b)
		{
			return __byordinal[b];
		}
	}
}
