/**
 * This file Copyright (c) 2005-2008 Aptana, Inc. This program is
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

import java.util.ArrayList;
import java.util.List;

/**
 * @author Kevin Lindsey
 */
public class FunctionType extends Type
{
	private List<Type> _parameterTypes = new ArrayList<Type>();
	private List<Type> _returnTypes = new ArrayList<Type>();

	/**
	 * FunctionType
	 */
	public FunctionType()
	{
		super("Function");
	}

	/**
	 * addParameterType
	 * 
	 * @param parameterType
	 */
	public void addParameterType(Type parameterType)
	{
		if (parameterType != null)
		{
			this._parameterTypes.add(parameterType);
		}
	}

	/**
	 * getReturnType
	 * 
	 * @return
	 */
	public Type[] getReturnTypes()
	{
		return this._returnTypes.toArray(new Type[this._returnTypes.size()]);
	}

	/**
	 * addReturnType
	 * 
	 * @param returnType
	 */
	public void addReturnType(Type returnType)
	{
		if (returnType != null)
		{
			this._returnTypes.add(returnType);
		}
	}

	/**
	 * @see com.aptana.docgen.reflection.Type#getName()
	 */
	@Override
	public String getName()
	{
		StringBuilder builder = new StringBuilder();
		
		builder.append(super.getName());
		
		if (this._parameterTypes != null && this._parameterTypes.size() > 0)
		{
			Type type = this._parameterTypes.get(0);
			
			builder.append("(").append(type.getName());
			
			for (int i = 1; i < this._parameterTypes.size(); i++)
			{
				builder.append(",").append(this._parameterTypes.get(i).getName());
			}
			
			builder.append(")");
		}
		
		if (this._returnTypes != null && this._returnTypes.size() > 0)
		{
			Type returnType = this._returnTypes.get(0);
			
			builder.append("->");
			
			if (this._returnTypes.size() == 1)
			{
				builder.append(returnType.getName());
			}
			else
			{
				builder.append("(").append(returnType.getName());
				
				for (int i = 1; i < this._returnTypes.size(); i++)
				{
					builder.append("|").append(this._returnTypes.get(i).getName());
				}
				
				builder.append(")");
			}
		}
		
		return builder.toString();
	}
}
