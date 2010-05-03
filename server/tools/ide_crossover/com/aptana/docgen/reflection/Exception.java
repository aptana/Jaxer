/**
 * This file Copyright (c) 2005-2007 Aptana, Inc. This program is
 * dual-licensed under both the Aptana Public License and the GNU General
 * Public license. You may elect to use one or the other of these licenses.
 *
 * This program is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT. Redistribution, except as permitted by whichever of
 * the GPL or APL you select, is prohibited.
 *
 * 1. For the GPL license (GPL), you can redistribute and/or modify this
 * program under the terms of the GNU General Public License,
 * Version 3, as published by the Free Software Foundation.  You should
 * have received a copy of the GNU General Public License, Version 3 along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Aptana provides a special exception to allow redistribution of this file
 * with certain Eclipse Public Licensed code and certain additional terms
 * pursuant to Section 7 of the GPL. You may view the exception and these
 * terms on the web at http://www.aptana.com/legal/gpl/.
 *
 * 2. For the Aptana Public License (APL), this program and the
 * accompanying materials are made available under the terms of the APL
 * v1.0 which accompanies this distribution, and is available at
 * http://www.aptana.com/legal/apl/.
 *
 * You may view the GPL, Aptana's exception and additional terms, and the
 * APL in the file titled license.html at the root of the corresponding
 * plugin containing this source file.
 *
 * Any modifications to this file must keep this entire header intact.
 */
package com.aptana.docgen.reflection;

import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.List;

import com.aptana.docgen.SignatureUtils;
import com.aptana.ide.io.SourceWriter;

/**
 * @author Kevin Lindsey
 */
public class Exception extends AbstractDescriptor
{
	private List<Type> _types = new ArrayList<Type>();

	/**
	 * Exception
	 */
	public Exception()
	{
	}
	
	/**
	 * addType
	 * 
	 * @param type
	 */
	public void addType(Type type)
	{
		if (type != null)
		{
			if (this._types.contains(type) == false)
			{
				this._types.add(type);
			}
			else
			{
				String message = MessageFormat.format(
					"The type was not added because this Exception already contains this type.\n{0}",
					new Object[] {
						type.getSignature()
					}
				);
				
				System.out.println(message);
			}
		}
	}

	/**
	 * @see com.aptana.docgen.reflection.AbstractDescriptor#getSignature(com.aptana.ide.io.SourceWriter)
	 */
	public void getSignature(SourceWriter writer)
	{
		if (this._types != null && this._types.size() > 0)
		{
			// copy type list and sort
			List<Type> types = new ArrayList<Type>(this._types);
			
			SignatureUtils.sort(types);
			
			Type type = types.get(0);
			
			writer.print(type.getName());
			
			for (int i = 1; i < types.size(); i++)
			{
				type = types.get(i);
				
				writer.print("|").print(type.getName());
			}
		}
	}

	/**
	 * @see com.aptana.docgen.reflection.AbstractDescriptor#getXML(SerializationContext)
	 */
	public void getXML(SerializationContext context)
	{
		if (this.hasContent())
		{
			SourceWriter writer = context.getSourceWriter();
			
			for (Type type : this._types)
			{
				writer.printWithIndent("<exception");
				writer.print(" type=\"").print(this.entitize(type.getName())).print("\"");
				
				if (super.hasContent())
				{
					writer.println(">").increaseIndent();
					
					super.getXML(context);
					
					writer.decreaseIndent().printlnWithIndent("</exception>");
				}
				else
				{
					writer.println("/>");
				}
			}
		}
	}
	
	/**
	 * @see com.aptana.docgen.reflection.AbstractDescriptor#hasContent()
	 */
	public boolean hasContent()
	{
		return super.hasContent() || this.hasTypes();
	}

	/**
	 * hasTypes
	 * 
	 * @return
	 */
	public boolean hasTypes()
	{
		return this._types != null && this._types.size() > 0;
	}
}
