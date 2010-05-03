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
public class Type extends AbstractDescriptor
{
	public static final Type OBJECT_CLASS = new Type("Object");
	
	private String _name;
	private Type _superClass;
	private List<Constructor> _constructors = new ArrayList<Constructor>();
	private List<Method> _methods = new ArrayList<Method>();
	private List<Property> _properties = new ArrayList<Property>();
	private boolean _isNamespace = false;
	
	/**
	 * Type
	 * 
	 * @param name
	 */
	public Type(String name)
	{
		this(name, OBJECT_CLASS);
	}
	
	/**
	 * Type
	 */
	public Type(String name, Type superClass)
	{
		this._name = name;
		this._superClass = superClass;
	}
	
	/**
	 * addConstructor
	 * 
	 * @param constructor
	 */
	public void addConstructor(Constructor constructor)
	{
		if (constructor != null)
		{
			if (this._constructors.contains(constructor) == false)
			{
				this._constructors.add(constructor);
			}
			else
			{
				String message = MessageFormat.format(
					"The constructor was not added because type {0} already contains a constructor with this signature: {1}.",
					new Object[] {
						this.getName(),
						constructor.getSignature()
					}
				);
				
				System.out.println(message);
			}
		}
	}
	
	/**
	 * addMethod
	 * 
	 * @param method
	 */
	public void addMethod(Method method)
	{
		if (method != null)
		{
			if (this._methods.contains(method) == false)
			{
				this._methods.add(method);
			}
			else
			{
				String message = MessageFormat.format(
					"The method was not added because type {0} already contains a method with this signature: {1}.",
					new Object[] {
						this.getName(),
						method.getSignature()
					}
				);
				
				System.out.println(message);
			}
		}
	}
	
	/**
	 * addProperty
	 * 
	 * @param property
	 */
	public void addProperty(Property property)
	{
		if (property != null)
		{
			if (this._properties.contains(property) == false)
			{
				this._properties.add(property);
			}
			else
			{
				String message = MessageFormat.format(
					"The property was not added because type {0} already contains a property with this signature: {1}.",
					new Object[] {
						this.getName(),
						property.getSignature()
					}
				);
				
				System.out.println(message);
			}
		}
	}
	
	/**
	 * Create a shallow clone of this type with a new name
	 * 
	 * @param name
	 * @return
	 */
	public Type cloneAs(String name)
	{
		Type result = null;
		
		if (name != null && name.length() > 0)
		{
			// create new type
			result = new Type(name);
			
			result._description = this._description;
			result._browsers = this._browsers;
			result._examples = this._examples;
			result._locations = this._locations;
			result._superClass = this._superClass;
			result._constructors = this._constructors;
			result._methods = this._methods;
			result._properties = this._properties;
		}
		
		return result;
	}
	
	/**
	 * getConstructor
	 * 
	 * @param ctor
	 * @return
	 */
	public Constructor getConstructor(Constructor ctor)
	{
		int index = this._constructors.indexOf(ctor);
		Constructor result = null;
		
		if (index != -1)
		{
			result = this._constructors.get(index);
		}
		
		return result;
	}
	
	/**
	 * getConstructors
	 * 
	 * @return
	 */
	public Constructor[] getConstructors()
	{
		return this._constructors.toArray(new Constructor[this._constructors.size()]);
	}
	
	/**
	 * getMethod
	 * 
	 * @param method
	 * @return
	 */
	public Method getMethod(Method method)
	{
		int index = this._methods.indexOf(method);
		Method result = null;
		
		if (index != -1)
		{
			result = this._methods.get(index);
		}
		
		return result;
	}
	
	/**
	 * getMethods
	 * 
	 * @return
	 */
	public Method[] getMethods()
	{
		return this._methods.toArray(new Method[this._methods.size()]);
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
	 * getProperty
	 * 
	 * @param property
	 * @return
	 */
	public Property getProperty(Property property)
	{
		int index = this._properties.indexOf(property);
		Property result = null;
		
		if (index != -1)
		{
			result = this._properties.get(index);
		}
		
		return result;
	}
	
	/**
	 * getProperties
	 * 
	 * @return
	 */
	public Property[] getProperties()
	{
		return this._properties.toArray(new Property[this._properties.size()]);
	}
	
	/**
	 * getSignature
	 * 
	 * @param writer
	 */
	public void getSignature(SourceWriter writer)
	{
		writer.printIndent();
		
		// process visibility
		if (this._visibility != null && this._visibility.length() > 0)
		{
			writer.print(this._visibility).print(" ");
		}
		
		if (this.getSuperClass() != Type.OBJECT_CLASS)
		{
			writer.print("type ").print(this.getName()).print(" extends ").println(this.getSuperClass().getName());
		}
		else
		{
			writer.print("type ").println(this.getName());
		}
		writer.printlnWithIndent("{").increaseIndent();
		
		if (this.hasConstructors())
		{
			// copy constructor list and sort
			List<Constructor> constructors = new ArrayList<Constructor>(this._constructors);
			
			SignatureUtils.sort(constructors);
			
			for (Constructor constructor : constructors)
			{
				constructor.getSignature(writer);
				writer.println();
			}
		}
		
		if (this.hasProperties())
		{
			// copy property list and sort
			List<Property> properties = new ArrayList<Property>(this._properties);
			
			SignatureUtils.sort(properties);
			
			for (Property property : properties)
			{
				property.getSignature(writer);
				writer.println();
			}
		}
		
		if (this.hasMethods())
		{
			// copy method list and sort
			List<Method> methods = new ArrayList<Method>(this._methods);
			
			SignatureUtils.sort(methods);
			
			for (Method method : methods)
			{
				method.getSignature(writer);
				writer.println();
			}
		}
		
		writer.decreaseIndent().printlnWithIndent("}");
	}
	
	/**
	 * getSuperClass
	 * 
	 * @return
	 */
	public Type getSuperClass()
	{
		return this._superClass;
	}
	
	/**
	 * getXML
	 * 
	 * @param context
	 */
	public void getXML(SerializationContext context)
	{
		SourceWriter writer = context.getSourceWriter();
		boolean publicOnly = context.getHidePrivate();
		
		if (this.hasMembers(publicOnly) || this._isNamespace)
		{
			// emit offsets
			if (context.getShowLocations() && this.hasLocations())
			{
				if (this._locations.size() == 1)
				{
					writer.printWithIndent("<!-- ").print(this._locations.get(0)).println(" -->");
				}
				else
				{
					writer.printlnWithIndent("<!--").increaseIndent();
					
					writer.printWithIndent(this._locations.get(0));
					
					for (int i = 1; i < this._locations.size(); i++)
					{
						String location = this._locations.get(i);
						
						writer.println(",").printWithIndent(location);
					}
					
					writer.println();
					
					writer.decreaseIndent().printlnWithIndent("-->");
				}
			}
			
			writer.printWithIndent("<class");
			writer.print(" type=\"").print(this.entitize(this.getName())).print("\"");
			writer.print(" superclass=\"").print(this.entitize(this.getSuperClass().getName())).print("\"");
			
			if (this._visibility != null && this._visibility.length() > 0)
			{
				writer.print(" visibility=\"").print(this._visibility).print("\"");
			}
			
			if (this.hasContent(publicOnly))
			{
				writer.println(">").increaseIndent();
				
				super.getXML(context);
				
				if (this.hasConstructors(publicOnly))
				{
					// copy constructor list and sort
					List<Constructor> constructors = new ArrayList<Constructor>(this._constructors);
					
					SignatureUtils.sort(constructors);
					
					writer.printlnWithIndent("<constructors>").increaseIndent();
					
					for (Constructor ctor : constructors)
					{
						if (publicOnly == false || ctor.isPrivate() == false)
						{
							ctor.getXML(context);
						}
					}
					
					writer.decreaseIndent().printlnWithIndent("</constructors>");
				}
				
				if (this.hasProperties(publicOnly))
				{
					// copy property list and sort
					List<Property> properties = new ArrayList<Property>(this._properties);
					
					SignatureUtils.sort(properties);
					
					writer.printlnWithIndent("<properties>").increaseIndent();
					
					for (Property property : properties)
					{
						if (publicOnly == false || property.isPrivate() == false)
						{
							property.getXML(context);
						}
					}
					
					writer.decreaseIndent().printlnWithIndent("</properties>");
				}
				
				if (this.hasMethods(publicOnly))
				{
					// copy method list and sort
					List<Method> methods = new ArrayList<Method>(this._methods);
					
					SignatureUtils.sort(methods);
					
					writer.printlnWithIndent("<methods>").increaseIndent();
					
					for (Method method : methods)
					{
						if (publicOnly == false || method.isPrivate() == false)
						{
							method.getXML(context);
						}
					}
					
					writer.decreaseIndent().printlnWithIndent("</methods>");
				}
				
				writer.decreaseIndent().printlnWithIndent("</class>");
			}
			else
			{
				writer.println("/>");
			}
		}
	}
	
	/**
	 * hasContent
	 * 
	 * @return
	 */
	public boolean hasContent()
	{
		return this.hasContent(true);
	}
	
	/**
	 * hasContent
	 * 
	 * @param includePrivate
	 * @return
	 */
	public boolean hasContent(boolean publicOnly)
	{
		return
			super.hasContent() ||
			this.hasConstructors(publicOnly) ||
			this.hasProperties(publicOnly) ||
			this.hasMethods(publicOnly);
	}
	
	/**
	 * hasConstructor
	 * 
	 * @param constructor
	 * @return
	 */
	public boolean hasConstructor(Constructor constructor)
	{
		return this._constructors.contains(constructor);
	}
	
	/**
	 * hasConstructors
	 * 
	 * @return
	 */
	public boolean hasConstructors()
	{
		return this.hasConstructors(false);
	}
	
	/**
	 * hasConstructors
	 * 
	 * @return
	 */
	public boolean hasConstructors(boolean publicOnly)
	{
		boolean result = false;
		
		if (this._constructors != null && this._constructors.size() > 0)
		{
			if (publicOnly)
			{
				result = false;
				
				for (Constructor constructor : this._constructors)
				{
					if (constructor.isPrivate() == false)
					{
						result = true;
						break;
					}
				}
			}
			else
			{
				result = true;
			}
		}
		
		return result;
	}
	
	/**
	 * hasMembers
	 * 
	 * @return
	 */
	public boolean hasMembers()
	{
		return this.hasMembers(false);
	}
	
	/**
	 * hasMembers
	 * 
	 * @param publicOnly
	 * @return
	 */
	public boolean hasMembers(boolean publicOnly)
	{
		return this.hasConstructors(publicOnly) || this.hasMethods(publicOnly) || this.hasProperties(publicOnly);
	}
	
	/**
	 * hasMethod
	 * 
	 * @param method
	 * @return
	 */
	public boolean hasMethod(Method method)
	{
		return this._methods.contains(method);
	}
	
	/**
	 * hasMethods
	 * 
	 * @return
	 */
	public boolean hasMethods()
	{
		return this.hasMethods(false);
	}
	
	/**
	 * hasMethods
	 * 
	 * @return
	 */
	public boolean hasMethods(boolean publicOnly)
	{
		boolean result = false;
		
		if (this._methods != null && this._methods.size() > 0)
		{
			if (publicOnly)
			{
				result = false;
				
				for (Method method : this._methods)
				{
					if (method.isPrivate() == false)
					{
						result = true;
						break;
					}
				}
			}
			else
			{
				result = true;
			}
		}
		
		return result;
	}
	
	/**
	 * hasProperty
	 * 
	 * @param property
	 * @return
	 */
	public boolean hasProperty(Property property)
	{
		return this._properties.contains(property);
	}
	
	/**
	 * hasProperties
	 * 
	 * @return
	 */
	public boolean hasProperties()
	{
		return this.hasProperties(false);
	}
	
	/**
	 * hasProperties
	 * 
	 * @return
	 */
	public boolean hasProperties(boolean publicOnly)
	{
		boolean result = false;
		
		if (this._properties != null && this._properties.size() > 0)
		{
			if (publicOnly)
			{
				result = false;
				
				for (Property property : this._properties)
				{
					if (property.isPrivate() == false)
					{
						result = true;
						break;
					}
				}
			}
			else
			{
				result = true;
			}
		}
		
		return result;
	}
	
	/**
	 * isNamespace
	 * 
	 * @return
	 */
	public boolean isNamespace()
	{
		return this._isNamespace;
	}
	
	/**
	 * mergeInto
	 * 
	 * @param targetType
	 */
	public void mergeInto(Type targetType)
	{
		if (targetType != null && targetType != this)
		{
			// merge constructors
			for (Constructor constructor : this._constructors)
			{
				if (targetType.hasConstructor(constructor) == false)
				{
					targetType.addConstructor(constructor);
				}
			}
			
			// merge properties
			for (Property property : this._properties)
			{
				if (targetType.hasProperty(property) == false)
				{
					targetType.addProperty(property);
				}
			}
			
			// merge methods
			for (Method method : this._methods)
			{
				if (targetType.hasMethod(method) == false)
				{
					targetType.addMethod(method);
				}
			}
		}
	}
	
	/**
	 * setIsNamespace
	 * 
	 * @param value
	 */
	public void setIsNamespace(boolean value)
	{
		this._isNamespace = value;
	}
	
	/**
	 * setSuperClass
	 * 
	 * @param superClass
	 */
	public void setSuperClass(Type superClass)
	{
		if (superClass != null)
		{
			this._superClass = superClass;
		}
	}
}
