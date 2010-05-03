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
public class Parameter extends AbstractDescriptor
{
	private String _name;
	private List<Type> _types = new ArrayList<Type>();
	private String _usage;
	
	/**
	 * Parameter
	 * 
	 * @param name
	 * @param type
	 */
	public Parameter(String name)
	{
		this._name = name;
		this._usage = "required";
	}
	
	/**
	 * addType
	 * 
	 * @param type
	 */
	public void addType(Type type)
	{
		// add type if we don't have it already
		if (type != null)
		{
			if (this._types.contains(type) == false)
			{
				this._types.add(type);
			}
			else
			{
				String message = MessageFormat.format(
					"The type was not added because this Parameter already contains this type.\n{0}",
					new Object[] {
						type.getSignature()
					}
				);
				
				System.out.println(message);
			}
		}
	}
	
	/**
	 * getName
	 * 
	 * @return
	 */
	public String getName()
	{
		return this._name;
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
	 * getType
	 * 
	 * @return
	 */
	public AbstractDescriptor[] getTypes()
	{
		return this._types.toArray(new Type[this._types.size()]);
	}
	
	/**
	 * isRequired
	 * 
	 * @return
	 */
	public String getUsage()
	{
		return this._usage;
	}
	
	/**
	 * @see com.aptana.docgen.reflection.AbstractSerializer#getXML(SerializationContext)
	 */
	public void getXML(SerializationContext context)
	{
		SourceWriter writer = context.getSourceWriter();
		
		writer.printWithIndent("<parameter");
		writer.print(" name=\"").print(this.getName()).print("\"");
		writer.print(" usage=\"").print(this.getUsage()).print("\"");
		
		if (this._types.size() > 0)
		{
			// TODO: Only showing first type since it is an attribute here
			Type type = this._types.get(0);
			
			writer.print(" type=\"").print(this.entitize(type.getName())).print("\"");
		}
		
		if (this.hasDescription() || this.hasExamples())
		{
			writer.println(">").increaseIndent();
			
			// add description and examples
			super.getXML(context);
			
			// close parameter
			writer.decreaseIndent().printlnWithIndent("</parameter>");
		}
		else
		{
			writer.println("/>");
		}
	}
	
	/**
	 * setRequired
	 * 
	 * @param isRequired
	 */
	public void setUsage(String usage)
	{
		this._usage = usage;
	}
}
