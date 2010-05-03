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
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.aptana.ide.io.SourceWriter;

/**
 * @author Kevin Lindsey
 */
public abstract class AbstractDescriptor extends AbstractSerializer implements ISignature
{
	/**
	 * ADVANCED_DESCRIPTION
	 */
	private static final String ADVANCED_DESCRIPTION = "(Advanced)";
	private static final Pattern SPECIAL_CHARS = Pattern.compile("[<>&'\"]");
	private static final Map<String,String> ENTITIES = new HashMap<String,String>();
	private static final String PRIVATE = "private";
	private static final String INTERNAL = "internal";
	private static final String ADVANCED = "advanced";
	
	protected String _description;
	protected List<String> _examples = new ArrayList<String>();
	protected List<Browser> _browsers = new ArrayList<Browser>();
	protected List<String> _references = new ArrayList<String>();
	protected String _visibility;
	protected List<String> _locations = new ArrayList<String>();
	
	private boolean _useCDATA = true;
	
	/**
	 * static constructor
	 */
	static
	{
		ENTITIES.put("<", "&lt;");
		ENTITIES.put(">", "&gt;");
		ENTITIES.put("&", "&amp;");
		ENTITIES.put("'", "&apos;");
		ENTITIES.put("\"", "&quot;");
	}
	
	/**
	 * AbstractDescriptor
	 */
	public AbstractDescriptor()
	{
	}
	
	/**
	 * addBrowser
	 * 
	 * @param browser
	 */
	public void addBrowser(String browser, String version)
	{
		if (browser != null && browser.length() > 0)
		{
			Browser b;
			
			if (version != null && version.length() > 0)
			{
				b = new Browser(browser, version);
			}
			else
			{
				b = new Browser(browser);
			}
			
			if (this._browsers.contains(b) == false)
			{
				this._browsers.add(b);
			}
		}
	}

	/**
	 * addExample
	 * 
	 * @param example
	 */
	public void addExample(String example)
	{
		if (example != null && example.length() > 0)
		{
			this._examples.add(example);
		}
	}
	
	/**
	 * addOffset
	 * 
	 * @param location
	 */
	public void addLocation(String location)
	{
		if (this._locations.contains(location) == false)
		{
			this._locations.add(location);
		}
	}
	
	/**
	 * addReference
	 * 
	 * @param reference
	 */
	public void addReference(String reference)
	{
		if (reference != null && reference.length() > 0)
		{
			this._references.add(reference);
		}
	}
	
	/**
	 * entitize
	 * 
	 * @param source
	 * @return
	 */
	protected String entitize(String source)
	{
		String result = null;
		
		if (source != null)
		{
			Matcher m = SPECIAL_CHARS.matcher(source);
			StringBuffer sb = new StringBuffer();
			
			while (m.find())
			{
				m.appendReplacement(sb, ENTITIES.get(m.group()));
			}
			
			m.appendTail(sb);
			
			result = sb.toString();
		}
		
		return result;
	}

	/**
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	public boolean equals(Object arg0)
	{
		boolean result = false;
		
		if (arg0 == this)
		{
			result = true;
		}
		else if (arg0 instanceof ISignature)
		{
			ISignature that = (ISignature) arg0;
			
			result = this.getSignature().equals(that.getSignature());
		}
		else
		{
			super.equals(arg0);
		}
		
		return result;
	}

	/**
	 * getBrowsers
	 * 
	 * @return
	 */
	public String[] getBrowsers()
	{
		return this._browsers.toArray(new String[this._browsers.size()]);
	}

	/**
	 * getDescription
	 * 
	 * @return
	 */
	public String getDescription()
	{
		return this._description;
	}
	
	/**
	 * getExamples
	 * 
	 * @return
	 */
	public String[] getExamples()
	{
		return this._examples.toArray(new String[this._examples.size()]);
	}
	
	/**
	 * getLocations
	 * 
	 * @return
	 */
	public String[] getLocations()
	{
		return this._locations.toArray(new String[this._locations.size()]);
	}
	
	/**
	 * @see com.aptana.docgen.reflection.ISignature#getSignature()
	 */
	public String getSignature()
	{
		SourceWriter writer = new SourceWriter();
		
		this.getSignature(writer);
		
		return writer.toString();
	}
	
	/**
	 * @see com.aptana.docgen.reflection.ISignature#getSignature(com.aptana.ide.io.SourceWriter)
	 */
	public abstract void getSignature(SourceWriter writer);
	
	/**
	 * getVisibility
	 * 
	 * @return
	 */
	public String getVisibility()
	{
		return this._visibility;
	}
	
	/**
	 * @see com.aptana.docgen.reflection.AbstractSerializer#getXML(SerializationContext)
	 */
	public void getXML(SerializationContext context)
	{
		SourceWriter writer = context.getSourceWriter();
		
		if (this.hasDescription())
		{
			writer.printWithIndent("<description>");
			writer.print(this.entitize(this._description));
			
			if (this.isAdvanced())
			{
				writer.print("&lt;br&gt;&lt;br&gt;").print(ADVANCED_DESCRIPTION);
			}
			
			writer.println("</description>");
		}
		else if (this.isAdvanced())
		{
			writer.printWithIndent("<description>").print(ADVANCED_DESCRIPTION).println("</description>");
		}
		
		if (this.hasBrowsers())
		{
			writer.printlnWithIndent("<browsers>").increaseIndent();
			
			for (Browser browser : this._browsers)
			{
				browser.getXML(context);
			}
			
			writer.decreaseIndent().printlnWithIndent("</browsers>");
		}
		
		if (this.hasExamples())
		{
			writer.printlnWithIndent("<examples>").increaseIndent();
			
			for (String example : this._examples)
			{
				if (this._useCDATA)
				{
					writer.printWithIndent("<example><![CDATA[");
					writer.print(example);
					writer.println("]]></example>");
				}
				else
				{
					writer.printlnWithIndent("<example>").increaseIndent();
					writer.printlnWithIndent(this.entitize(example));
					writer.decreaseIndent().printlnWithIndent("</example>");
				}
			}
			
			writer.decreaseIndent().printlnWithIndent("</examples>");
		}
		
		if (this.hasReferences())
		{
			writer.printlnWithIndent("<references>").increaseIndent();
			
			for (String reference : this._references)
			{
				writer.printWithIndent("<reference name=\"").print(reference).println("\"/>");
			}
			
			writer.decreaseIndent().printlnWithIndent("</references>");
		}
	}
	
	/**
	 * hasBrowsers
	 * 
	 * @return
	 */
	public boolean hasBrowsers()
	{
		return this._browsers != null && this._browsers.size() > 0;
	}

	/**
	 * hasContent
	 * 
	 * @return
	 */
	public boolean hasContent()
	{
		return this.hasLocations() || this.hasDescription() || this.hasExamples() || this.hasBrowsers() || this.hasReferences();
	}
	
	/**
	 * hasDescription
	 * 
	 * @return
	 */
	public boolean hasDescription()
	{
		return this._description != null && this._description.length() > 0;
	}
	
	/**
	 * hasExamples
	 * 
	 * @return
	 */
	public boolean hasExamples()
	{
		return this._examples != null && this._examples.size() > 0;
	}
	
	/**
	 * hasReferences
	 * 
	 * @return
	 */
	public boolean hasReferences()
	{
		return this._references != null && this._references.size() > 0;
	}
	
	/**
	 * hasLocations
	 * 
	 * @return
	 */
	public boolean hasLocations()
	{
		return this._locations != null && this._locations.size() > 0;
	}
	
	/**
	 * @see java.lang.Object#hashCode()
	 */
	public int hashCode()
	{
		return this.getSignature().hashCode();
	}
	
	/**
	 * isAdvanced
	 * 
	 * @return
	 */
	public boolean isAdvanced()
	{
		return this._visibility != null && this._visibility.equals(ADVANCED);
	}
	
	/**
	 * isInternal
	 * 
	 * @return
	 */
	public boolean isInternal()
	{
		return this._visibility != null && this._visibility.equals(INTERNAL);
	}
	
	/**
	 * isPrivate
	 * 
	 * @return
	 */
	public boolean isPrivate()
	{
		return this._visibility != null && this._visibility.equals(PRIVATE);
	}
	
	/**
	 * setDescription
	 * 
	 * @param description
	 */
	public void setDescription(String description)
	{
		this._description = description;
	}
	
	/**
	 * setUseCDATA
	 * 
	 * @param value
	 */
	public void setUseCDATA(boolean value)
	{
		this._useCDATA = value;
	}
	
	/**
	 * setVisibility
	 * 
	 * @param visibility
	 */
	public void setVisibility(String visibility)
	{
		this._visibility = visibility;
	}
}
