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

import java.util.ArrayList;
import java.util.List;

import com.aptana.docgen.SignatureUtils;
import com.aptana.ide.io.SourceWriter;

/**
 * @author Kevin Lindsey
 */
public abstract class AbstractMethod extends AbstractDescriptor
{
	protected String _name;
	private String _scope = "instance";
	private String _elementName;
	private List<Parameter> _parameters = new ArrayList<Parameter>();
	private List<ReturnType> _returnTypes = new ArrayList<ReturnType>();
	protected List<Exception> _exceptions = new ArrayList<Exception>();

	/**
	 * AbstractMethod
	 */
	protected AbstractMethod(String elementName)
	{
		this._elementName = elementName;
	}
	
	/**
	 * addParameter
	 * 
	 * @param parameter
	 */
	public void addParameter(Parameter parameter)
	{
		if (parameter != null)
		{
			this._parameters.add(parameter);
		}
	}

	/**
	 * addReturnType
	 * 
	 * @param returnType
	 */
	public void addReturnType(ReturnType returnType)
	{
		if (returnType != null)
		{
			this._returnTypes.add(returnType);
		}
	}
	
	/**
	 * getScope
	 * 
	 * @return
	 */
	public String getScope()
	{
		return this._scope;
	}

	/**
	 * @see com.aptana.docgen.reflection.AbstractDescriptor#getSignature(com.aptana.ide.io.SourceWriter)
	 */
	public void getSignature(SourceWriter writer)
	{
		// process scope
		if ("static".equals(this._scope))
		{
			writer.printWithIndent(this._scope).print(" ");
		}
		else
		{
			writer.printIndent();
		}
		
		// process visibility
		if (this._visibility != null && this._visibility.length() > 0)
		{
			writer.print(this._visibility).print(" ");
		}
		
		// process name
		if (this.hasName())
		{
			writer.print("method ").print(this._name);
		}
		else
		{
			writer.print("#constructor");
		}

		// process parameters
		if (this.hasParameters())
		{
			writer.print("(");
			
			Parameter parameter = this._parameters.get(0);
			
			parameter.getSignature(writer);
			
			for (int i = 1; i < this._parameters.size(); i++)
			{
				parameter = this._parameters.get(i);
				
				writer.print(", ");
				parameter.getSignature(writer);
			}
			
			writer.print(")");
		}
		
		// process return types
		if (this.hasReturnTypes() && this.hasName())
		{
			// make a copy of the return types and sort the list
			List<ReturnType> returnTypes = new ArrayList<ReturnType>(this._returnTypes);
			
			SignatureUtils.sort(returnTypes);
			
			// add separator
			writer.print("->");
			
			boolean multipleReturnTypes = returnTypes.size() > 1;
			
			// open parenthesis
			if (multipleReturnTypes)
			{
				writer.print("(");
			}
			
			// emit first type
			ReturnType returnType = returnTypes.get(0);
			
			returnType.getSignature(writer);
			
			// emit remaining types
			for (int i = 1; i < returnTypes.size(); i++)
			{
				returnType = returnTypes.get(i);
				
				writer.print("|");
				returnType.getSignature(writer);
			}
			
			// close parenthesis
			if (multipleReturnTypes)
			{
				writer.print(")");
			}
		}
		
		// process exceptions
		if (this.hasExceptions())
		{
			// make a copy of the exception types and sort the list
			List<Exception> exceptions = new ArrayList<Exception>(this._exceptions);
			
			SignatureUtils.sort(exceptions);
			
			// add separator
			writer.print(" throws ");
			
			// emit first type
			Exception exception = this._exceptions.get(0);
			
			exception.getSignature(writer);
			
			// emit remaining types
			for (int i = 1; i < this._exceptions.size(); i++)
			{
				exception = this._exceptions.get(i);
				
				writer.print(", ");
				exception.getSignature(writer);
			}
		}
	}

	/**
	 * @see com.aptana.docgen.reflection.ISerialize#getXML(SerializationContext)
	 */
	public void getXML(SerializationContext context)
	{
		SourceWriter writer = context.getSourceWriter();
		
		// start element
		writer.printWithIndent("<").print(this._elementName);
		
		if (this._name != null && this._name.length() > 0)
		{
			writer.print(" name=\"").print(this._name).print("\"");
		}
		
		if (this._visibility != null && this._visibility.length() > 0)
		{
			writer.print(" visibility=\"").print(this._visibility).print("\"");
		}
		
		writer.print(" scope=\"").print(this._scope).println("\">").increaseIndent();
		
		// add inherited metadata
		super.getXML(context);
		
		// add parameters
		if (this.hasParameters())
		{
			writer.printlnWithIndent("<parameters>").increaseIndent();
			
			for (Parameter parameter : this._parameters)
			{
				parameter.getXML(context);
			}
			
			writer.decreaseIndent().printlnWithIndent("</parameters>");
		}
		
		// add return types
		if (this.hasReturnTypes())
		{
			// make a copy of the return types and sort the list
			List<ReturnType> returnTypes = new ArrayList<ReturnType>(this._returnTypes);
			
			SignatureUtils.sort(returnTypes);
			
			writer.printlnWithIndent("<return-types>").increaseIndent();
			
			for (ReturnType returnType : returnTypes)
			{
				returnType.getXML(context);
			}
			
			writer.decreaseIndent().printlnWithIndent("</return-types>");
		}
		
		// add exceptions
		if (this.hasExceptions())
		{
			// make a copy of the exception types and sort the list
			List<Exception> exceptions = new ArrayList<Exception>(this._exceptions);
			
			SignatureUtils.sort(exceptions);
			
			writer.printlnWithIndent("<exceptions>").increaseIndent();
			
			for (Exception exception : exceptions)
			{
				exception.getXML(context);
			}
			
			writer.decreaseIndent().printlnWithIndent("</exceptions>");
		}
		
		writer.decreaseIndent().printWithIndent("</").print(this._elementName).println(">");
	}
	
	/**
	 * setScope
	 * 
	 * @param scope
	 */
	public void setScope(String scope)
	{
		this._scope = scope;
	}

	/**
	 * addException
	 * 
	 * @param exception
	 */
	public void addException(Exception exception)
	{
		if (exception != null)
		{
			this._exceptions.add(exception);
		}
	}

	/**
	 * @see com.aptana.docgen.reflection.AbstractDescriptor#hasContent()
	 */
	public boolean hasContent()
	{
		return
			super.hasContent() ||
			this.hasExceptions() ||
			this.hasParameters() ||
			this.hasReturnTypes();
	}
	
	/**
	 * hasExceptions
	 * 
	 * @return
	 */
	public boolean hasExceptions()
	{
		return this._exceptions != null && this._exceptions.size() > 0;
	}
	
	/**
	 * hasName
	 * 
	 * @return
	 */
	public boolean hasName()
	{
		return this._name != null && this._name.length() > 0;
	}
	
	/**
	 * hasParameters
	 * 
	 * @return
	 */
	public boolean hasParameters()
	{
		return this._parameters != null && this._parameters.size() > 0;
	}
	
	/**
	 * hasReturnTypes
	 * 
	 * @return
	 */
	public boolean hasReturnTypes()
	{
		return this._returnTypes != null && this._returnTypes.size() > 0;
	}
}
