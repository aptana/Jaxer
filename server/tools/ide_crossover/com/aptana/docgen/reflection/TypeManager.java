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
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.aptana.ide.io.SourceWriter;

/**
 * @author Kevin Lindsey
 */
public class TypeManager implements ISerialize
{
	private static final String PROTOTYPE = "prototype";

	private Map<String, Type> _types = new HashMap<String, Type>();
	private Set<String> _builtinTypes = new HashSet<String>();
	private String _browser = null;
	private String _browserVersion = null;
	private List<String> _filters = new ArrayList<String>();
	private Pattern _filterPattern = null;
	private List<String> _overviews = new ArrayList<String>();
	private String _location;
	private boolean _filterPrivate = false;
	private boolean _showLocations = false;

	/**
	 * TypeManager
	 */
	public TypeManager()
	{
		this.addBuiltins();
	}

	/**
	 * addBuiltin
	 * 
	 * @param typeName
	 */
	private void addBuiltin(String typeName)
	{
		this._builtinTypes.add(typeName);
		this._types.put(typeName, new Type(typeName));
	}

	/**
	 * addBuiltins
	 */
	private void addBuiltins()
	{
		this.addBuiltin("Array");
		this.addBuiltin("Arguments");
		this.addBuiltin("Boolean");
		this.addBuiltin("Date");
		this.addBuiltin("Document");
		this.addBuiltin("DocumentElement");
		this.addBuiltin("Error");
		//this.addBuiltin("Function");
		this.addBuiltin("HTMLElement");
		this.addBuiltin("Node");
		this.addBuiltin("Number");
		this.addBuiltin("null");
		this.addBuiltin("Object");
		this.addBuiltin("RegExp");
		this.addBuiltin("ScriptElement");
		this.addBuiltin("String");
		this.addBuiltin("undefined");
		this.addBuiltin("XMLHttpRequest");
	}

	/**
	 * addFilter
	 * 
	 * @param filter
	 */
	public void addFilter(String filter)
	{
		if (filter != null && filter.length() > 0)
		{
			if (this._filters.contains(filter) == false)
			{
				this._filters.add(filter);
			}
		}
	}
	
	/**
	 * addOverview
	 * 
	 * @param text
	 */
	public void addOverview(String text)
	{
		if (text != null && text.length() > 0)
		{
			this._overviews.add(text);
		}
	}
	
	/**
	 * createFilterPattern
	 */
	private void createFilterPattern()
	{
		// create filter regex
		if (this._filters != null && this._filters.size() > 0)
		{
			boolean hasMultipleFilters = this._filters.size() > 1;
			
			// create buffer
			StringBuffer buffer = new StringBuffer();
			
			// append start-with
			buffer.append("^");
			
			if (hasMultipleFilters)
			{
				buffer.append("(");
			}
			
			String filter = this._filters.get(0);
			
			buffer.append(filter.replace(".", "\\."));
			
			for (int i = 1; i < this._filters.size(); i++)
			{
				filter = this._filters.get(i);
				
				buffer.append("|").append(filter.replace(".", "\\."));
			}
			
			if (hasMultipleFilters)
			{
				buffer.append(")");
			}
			
			this._filterPattern = Pattern.compile(buffer.toString());
		}
		else
		{
			this._filterPattern = null;
		}
	}

	/**
	 * getBrowser
	 * 
	 * @return
	 */
	public String getBrowser()
	{
		return this._browser;
	}

	/**
	 * getBrowserVersion
	 * 
	 * @return
	 */
	public String getBrowserVersion()
	{
		return this._browserVersion;
	}

	/**
	 * getFilterPrivate
	 * 
	 * @return
	 */
	public boolean getFilterPrivate()
	{
		return this._filterPrivate;
	}
	
	/**
	 * getLocation
	 * 
	 * @return
	 */
	public String getLocation()
	{
		return this._location;
	}
	
	/**
	 * getMethodName
	 * 
	 * @param fullyQualifiedMethodName
	 * @return
	 */
	public String getMemberName(String fullyQualifiedMethodName)
	{
		int lastDot = fullyQualifiedMethodName.lastIndexOf('.');
		String result = null;

		if (lastDot != -1 && lastDot != fullyQualifiedMethodName.length())
		{
			result = fullyQualifiedMethodName.substring(lastDot + 1);
		}

		return result;
	}

	/**
	 * getMemberOwningTypeName
	 * 
	 * @param fullyQualifiedMemberName
	 * @return
	 */
	public String getMemberOwningTypeName(String fullyQualifiedMemberName)
	{
		int lastDot = fullyQualifiedMemberName.lastIndexOf('.');
		String result = null;

		if (lastDot != -1)
		{
			String fullyQualifiedTypeName;
			String candidate = fullyQualifiedMemberName.substring(0, lastDot);
			int nextToLastDot = candidate.lastIndexOf('.');

			if (nextToLastDot != -1)
			{
				String name = candidate.substring(nextToLastDot + 1);

				if (PROTOTYPE.equals(name))
				{
					fullyQualifiedTypeName = candidate.substring(0, nextToLastDot);
				}
				else
				{
					fullyQualifiedTypeName = candidate;
				}
			}
			else
			{
				fullyQualifiedTypeName = candidate;
			}

			result = fullyQualifiedTypeName;
		}
		
		return result;
	}
	
	/**
	 * getMethodOwningType
	 * 
	 * @param fullyQualifiedMemberName
	 * @return
	 */
	public Type getMemberOwningType(String fullyQualifiedMemberName)
	{
		String typeName = this.getMemberOwningTypeName(fullyQualifiedMemberName);
		Type result = null;

		if (typeName != null && typeName.length() > 0)
		{
			result = this.getType(typeName);
		}

		if (result == null)
		{
			System.out.println("Warning: Could not find type for member '" + fullyQualifiedMemberName + "'");
		}

		return result;
	}

	/**
	 * getType
	 * 
	 * @param fullyQualifiedTypeName
	 * @return
	 */
	public Type getType(String fullyQualifiedTypeName)
	{
		Type result = null;

		if (fullyQualifiedTypeName != null)
		{
			if (this._types.containsKey(fullyQualifiedTypeName))
			{
				result = this._types.get(fullyQualifiedTypeName);

				// make sure we tag this type with all browser types
				if (this._browser != null && this._browser.length() > 0)
				{
					result.addBrowser(this._browser, this._browserVersion);
				}
			}
			else
			{
				result = new Type(fullyQualifiedTypeName);

				if (this._browser != null && this._browser.length() > 0)
				{
					result.addBrowser(this._browser, this._browserVersion);
				}

				this._types.put(fullyQualifiedTypeName, result);
			}
		}
		else
		{
			System.out.println("Warning: requested type name was null");
		}
		
		// tag location where this type was referenced
		if (result != null && this._location != null)
		{
			result.addLocation(this._location);
		}

		return result;
	}

	/**
	 * @see com.aptana.docgen.reflection.ISerialize#getXML()
	 */
	public String getXML()
	{
		// create new serialization context
		SerializationContext context = new SerializationContext();
		
		// apply configurations
		context.setHidePrivate(this._filterPrivate);
		context.setShowLocations(this._showLocations);
		
		// convert to XML
		this.getXML(context);
		
		// return string result
		return context.toString();
	}

	/**
	 * @see com.aptana.docgen.reflection.ISerialize#getXML(SerializationContext)
	 */
	public void getXML(SerializationContext context)
	{
		SourceWriter writer = context.getSourceWriter();
		
		// create filter pattern
		this.createFilterPattern();
		
		// begin XML emission
		writer.printlnWithIndent("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>");
		writer.printlnWithIndent("<javascript>").increaseIndent();

		// emit overviews
		if (this._overviews != null && this._overviews.size() > 0)
		{
			for (String overview : this._overviews)
			{
				writer.printWithIndent("<overview>").print(overview).println("</overview>");
			}
		}
		
		// get a list of types ordered by name
		Set<String> keySet = this._types.keySet();
		String[] typeNames = keySet.toArray(new String[keySet.size()]);
		Arrays.sort(typeNames);

		// emit XML for each unfiltered type
		for (String typeName : typeNames)
		{
			if (this._builtinTypes.contains(typeName) == false)
			{
				Type type = this._types.get(typeName);

				if (this.isFilteredType(type) == false)
				{
					type.getXML(context);
				}
			}
		}

		writer.decreaseIndent().printlnWithIndent("</javascript>");
	}

	/**
	 * hasType
	 * 
	 * @param fullyQualifiedTypeName
	 * @return
	 */
	public boolean hasType(String fullyQualifiedTypeName)
	{
		return this._types.containsKey(fullyQualifiedTypeName);
	}
	
	/**
	 * Import all types from one namespace into another. If a type starts with sourceName, then it will be imported into
	 * targetName. This allows for either single types to be imported (use full type names for both source and target)
	 * or for whole packages/namespaces to be imported (use package names for source and target). Note that no imports
	 * will be performed if either sourceName or targetName are substrings of one another. In addition, if the target
	 * type already exists in the type set, then the source type will merged into the existing target type.
	 * 
	 * @param sourceName
	 * @param targetName
	 */
	public void importTypes(String sourceName, String targetName)
	{
		if (sourceName != null && sourceName.length() > 0 && targetName != null && targetName.length() > 0)
		{
			if (sourceName.startsWith(targetName) == false && targetName.startsWith(sourceName) == false)
			{
				List<Type> newTypes = new ArrayList<Type>();

				for (String key : this._types.keySet())
				{
					if (key.startsWith(sourceName))
					{
						// get source type
						Type sourceType = this._types.get(key);

						// calculate imported name
						String newName = targetName + key.substring(sourceName.length());

						// only import if the cloned type doesn't exist already
						if (this._types.containsKey(newName) == false)
						{
							// clone type
							Type newType = sourceType.cloneAs(newName);

							// and to list
							newTypes.add(newType);
						}
						else
						{
							Type targetType = this.getType(newName);
							
							sourceType.mergeInto(targetType);
						}
					}
				}

				// add new types to type manager. Note that we must do this as a separate step otherwise we will get
				// concurrent modification exception if we add the type during the cloning step
				for (Type type : newTypes)
				{
					this._types.put(type.getName(), type);
				}
			}
		}
	}

	/**
	 * isFilteredType
	 * 
	 * @return
	 */
	public boolean isFilteredType(Type type)
	{
		boolean result = this._filterPrivate && type.isPrivate();
		
		if (result == false && this._filterPattern != null)
		{
			Matcher m = this._filterPattern.matcher(type.getName());
			
			result = m.find();
		}
		
		return result;
	}

	/**
	 * isInstanceMember
	 * 
	 * @param fullyQualifiedMemberName
	 * @return
	 */
	public boolean isInstanceMember(String fullyQualifiedMemberName)
	{
		int lastDot = fullyQualifiedMemberName.lastIndexOf('.');
		boolean result = false;

		if (lastDot != -1)
		{
			String candidate = fullyQualifiedMemberName.substring(0, lastDot);
			int nextToLastDot = candidate.lastIndexOf('.');

			if (nextToLastDot != -1)
			{
				String name = candidate.substring(nextToLastDot + 1);

				result = PROTOTYPE.equals(name);
			}
		}

		return result;
	}
	
	/**
	 * isStaticMember
	 * 
	 * @param fullyQualifiedMemberName
	 * @return
	 */
	public boolean isStaticMember(String fullyQualifiedMemberName)
	{
		return this.isInstanceMember(fullyQualifiedMemberName) == false;
	}

	/**
	 * setBrowser
	 * 
	 * @param browser
	 */
	public void setBrowser(String browser)
	{
		this._browser = browser;
	}

	/**
	 * setBrowserVersion
	 * 
	 * @param browserVersion
	 */
	public void setBrowserVersion(String browserVersion)
	{
		this._browserVersion = browserVersion;
	}
	
	/**
	 * setFilterPrivate
	 * 
	 * @param value
	 */
	public void setFilterPrivate(boolean value)
	{
		this._filterPrivate = value;
	}
	
	/**
	 * setLocation
	 * 
	 * @param location
	 */
	public void setLocation(String location)
	{
		this._location = location;
	}
	
	/**
	 * setShowLocations
	 * 
	 * @param value
	 */
	public void setShowLocations(boolean value)
	{
		this._showLocations = value;
	}
}
