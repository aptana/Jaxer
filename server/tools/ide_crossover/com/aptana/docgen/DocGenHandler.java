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
package com.aptana.docgen;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.aptana.docgen.reflection.AbstractMethod;
import com.aptana.docgen.reflection.ArrayType;
import com.aptana.docgen.reflection.Constructor;
import com.aptana.docgen.reflection.Exception;
import com.aptana.docgen.reflection.FunctionType;
import com.aptana.docgen.reflection.Method;
import com.aptana.docgen.reflection.Namespace;
import com.aptana.docgen.reflection.Parameter;
import com.aptana.docgen.reflection.Property;
import com.aptana.docgen.reflection.ReturnType;
import com.aptana.docgen.reflection.Type;
import com.aptana.docgen.reflection.TypeManager;
import com.aptana.ide.lexer.Lexeme;
import com.aptana.ide.parsing.bnf.AbstractHandler;
import com.aptana.ide.parsing.bnf.IReductionContext;

/**
 * @author Kevin Lindsey
 */
public class DocGenHandler extends AbstractHandler
{
	private static final String INSTANCE = "instance";
	private static final String STATIC = "static";
	private static final String INTERNAL = "internal";
	private static final String PRIVATE = "private";
	private static final String ADVANCED = "advanced";
	private static final Pattern THIS_LINE = Pattern.compile("^\\s*this\\.");
	
	private TypeManager _typeManager = new TypeManager();
	private List<String> _aliases = new ArrayList<String>();
	private List<Parameter> _parameters = new ArrayList<Parameter>();
	private List<String> _examples = new ArrayList<String>();
	private List<Exception> _exceptions = new ArrayList<Exception>();
	private List<ReturnType> _returnTypes = new ArrayList<ReturnType>();
	private List<Type> _propertyTypes = new ArrayList<Type>();
	private List<Namespace> _namespaces = new ArrayList<Namespace>();
	private List<String> _references = new ArrayList<String>();
	private List<Type> _extendsTypes = new ArrayList<Type>();
	private String _nextLine = null;
	
	private boolean _isConstructor = false;
	private boolean _isInternal = false;
	private boolean _isMethod = false;
	private boolean _isPrivate = false;
	private boolean _isProperty = false;
	private boolean _isAdvanced = false;
	
	private boolean _isNamespace = false;
	private boolean _isClassDescription = false;
	private boolean _isOverview = false;
	
	private List<String> _missingAliases = new ArrayList<String>();
	private List<String> _staticThisProperties = new ArrayList<String>();
	
	private Map<String,String> _imports = new HashMap<String,String>();
	private Map<Namespace, String[]> _namespaceExamples = new HashMap<Namespace, String[]>();
	
	/**
	 * addImport
	 * 
	 * @param sourceName
	 * @param targetName
	 */
	public void addImport(String sourceName, String targetName)
	{
		this._imports.put(sourceName, targetName);
	}
	
	/**
	 * applyNamespaces
	 */
	public void applyNamespaces()
	{
		TypeManager manager = this._typeManager;
		
		for (Namespace namespace : this._namespaces)
		{
			String name = namespace.getName();
			String[] examples = this._namespaceExamples.get(namespace);
			
			// set namespace's original location in case we create types here
			if (namespace.hasLocations())
			{
				// Highlander location
				String location = namespace.getLocations()[0];
				
				manager.setLocation(location);
			}
			
			// make sure we don't have a type by this name
			if (manager.hasType(name))
			{
				Type owningType = manager.getType(name);
				String description = owningType.getDescription();
				
				if (description == null || description.length() == 0)
				{
					owningType.setDescription(namespace.getDescription());
				}
				
				for (String example : examples)
				{
					owningType.addExample(example);
				}
			}
			else
			{
				// create property
				Type owningType = manager.getType(name);
				
				// apply description
				owningType.setDescription(namespace.getDescription());
				
				// apply examples
				for (String example : examples)
				{
					owningType.addExample(example);
				}
				
				// apply visibility
				if (this._isAdvanced)
				{
					owningType.setVisibility(ADVANCED);
				}
				else if (this._isPrivate)
				{
					owningType.setVisibility(PRIVATE);
				}
				else if (this._isInternal)
				{
					owningType.setVisibility(INTERNAL);
				}
				
				// tag as namespace type
				owningType.setIsNamespace(true);
			}
		}
		
		this._namespaces.clear();
	}
	
	/**
	 * emitBlock
	 */
	private void emitBlock(String description)
	{
		String location = this._typeManager.getLocation();
		
		if (this._aliases != null & this._aliases.size() > 0)
		{
			for (String alias : this._aliases)
			{
				if (this._isConstructor)
				{
					this.processConstructor(alias, description);
				}
				else if (this._isMethod)
				{
					this.processMethod(alias, description);
				}
				else if (this._isProperty)
				{
					this.processProperty(alias, description);
				}
				else
				{
					this.processType(alias, description);
				}
				
				// do possible "this" check
				if (this._nextLine != null && this._nextLine.length() > 0)
				{
					Matcher m = THIS_LINE.matcher(this._nextLine);
					
					if (m.find())
					{
						if (alias.indexOf(".prototype.") == -1)
						{
							this._staticThisProperties.add(location);
						}
					}
				}
			}
		}
		else
		{
			// save any examples associated with this namespace
			if (this._isNamespace)
			{
				// get namespace that was added
				Namespace namespace = this._namespaces.get(this._namespaces.size() - 1);
				
				// associate any examples we found with namespace, if we have any
				this._namespaceExamples.put(namespace, this._examples.toArray(new String[this._examples.size()]));
			}
			
			// make sure it's not a block type that can emit aliases
			if (this._isNamespace == false && this._isClassDescription == false && this._isOverview == false)
			{
				// don't require aliases in private blocks
				if (this._isPrivate == false)
				{
					if (this._missingAliases.contains(location) == false)
					{
						this._missingAliases.add(location);
					}
				}
			}
		}
		
		this.beforeParse();
	}
	
	/**
	 * getLocations
	 * 
	 * @return
	 */
	public String[] getMissingAliases()
	{
		String[] result = this._missingAliases.toArray(new String[this._missingAliases.size()]);
		
		this._missingAliases.clear();
		
		return result;
	}
	
	/**
	 * getStaticThisProperties
	 * 
	 * @return
	 */
	public String[] getStaticThisProperties()
	{
		String[] result = this._staticThisProperties.toArray(new String[this._staticThisProperties.size()]);
		
		this._staticThisProperties.clear();
		
		return result;
	}
	
	/**
	 * getTypeManager
	 * 
	 * @return
	 */
	public TypeManager getTypeManager()
	{
		return this._typeManager;
	}
	
	/**
	 * importName
	 * 
	 * @param name
	 * @return
	 */
	private String importName(String name)
	{
		// potentially import into another namespace
		for (String key : this._imports.keySet())
		{
			if (name.startsWith(key))
			{
				name = this._imports.get(key) + name.substring(key.length());
			}
		}
		
		return name;
	}
	
	/**
	 * onAddBlock
	 * 
	 * @return
	 */
	public Object onAddBlock(IReductionContext context)
	{
		return context.getAction();
	}
	
	/**
	 * onAddTag
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onAddTag(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		
		return nodes[1];
	}
	
	/**
	 * onAddText
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onAddText(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		String accumulator = (String) nodes[0];
		String text = (String) nodes[1];
		
		return accumulator + " " + text;
	}
	
	/**
	 * onAddType
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onAddType(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		List<Type> result = (List<Type>) nodes[0];
		Type type = (Type) nodes[2];
		
		result.add(type);
		
		return result;
	}
	
	/**
	 * onAdvanced
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onAdvanced(IReductionContext context)
	{
		this._isAdvanced = true;
		
		return "";
	}
	
	/**
	 * onAlias
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onAlias(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		String alias = this.importName((String) nodes[1]);
		
		this._aliases.add(alias);
		
		return alias;
	}
	
	/**
	 * onAuthor
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onAuthor(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		
		return "author: " + (String) nodes[1];
	}
	
	/**
	 * onBlock
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onBlock(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		String text = (String) nodes[1];
		
		this.emitBlock(text);
		
		return text;
	}
	
	/**
	 * onClassDescription
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onClassDescription(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme typeLexeme = (Lexeme) nodes[2];
		String typeName = typeLexeme.getText();
		String description = (String) nodes[4];
		Type type = this._typeManager.getType(typeName);
		
		// set description
		type.setDescription(description);
		
		// set class description flag
		this._isClassDescription = true;
		
		return description;
	}
	
	/**
	 * onComplexArrayType
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onComplexArrayType(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Type memberType = (Type) nodes[2];
		
		return new ArrayType(memberType);
	}
	
	/**
	 * onConstructor
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onConstructor(IReductionContext context)
	{
		this._isConstructor = true;
		this._isMethod = false;
		
		return "";
	}
	
	/**
	 * onExtends
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onExtends(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		List<Type> types = (List<Type>) nodes[2];
		
		// add types
		for (Type type : types)
		{
			this._extendsTypes.add(type);
		}
		
		return "";
	}
	
	/**
	 * onExample
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onExample(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		String text = (String) nodes[1];
		String result;

		// normalize line starts
		Pattern lineStart = Pattern.compile("^\\s+\\*\\s", Pattern.MULTILINE);
		Matcher m = lineStart.matcher(text);
		StringBuffer sb = new StringBuffer();

		while (m.find())
		{
			m.appendReplacement(sb, "");
		}
		
		m.appendTail(sb);
		
		// get result
		result = sb.toString();
		
		// trim leading newline, if any
		result = result.replaceFirst("^\\s*(\\r|\\n)", "");
		
		// trim trailing whitespace
		result = result.replaceFirst("\\s+$", "");

		// save text
		this._examples.add(result);

		return result;
	}
	
	/**
	 * onException
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onException(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Exception exception = new Exception();
		List<Type> types = (List<Type>) nodes[2];
		
		// add types
		for (Type type : types)
		{
			exception.addType(type);
		}
		
		// add to list of exceptions
		this._exceptions.add(exception);
		
		return "";
	}
	
	/**
	 * onExceptionText
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onExceptionText(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Exception exception = new Exception();
		List<Type> types = (List<Type>) nodes[2];
		
		// add description
		String text = (String) nodes[4];
		
		exception.setDescription(text);
		
		// add types
		for (Type type : types)
		{
			exception.addType(type);
		}
		
		// add to list of exceptions
		this._exceptions.add(exception);
		
		return "";
	}
	
	/**
	 * onFirstBlock
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onFirstBlock(IReductionContext context)
	{
		return context.getAction();
	}
	
	/**
	 * onFirstTag
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onFirstTag(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		
		return nodes[0];
	}
	
	/**
	 * onFirstText
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onFirstText(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme textLexeme = (Lexeme) nodes[0];
		
		return textLexeme.getText();
	}
	
	/**
	 * onFirstType
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onFirstType(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		List<Type> result = new ArrayList<Type>();
		
		result.add((Type) nodes[0]);
		
		return result;
	}
	
	/**
	 * onFunction
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onFunction(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		FunctionType result = new FunctionType();
		List<Type> paramTypes = (List<Type>) nodes[2];
		Type returnType = (Type) nodes[5];
		
		result.addReturnType(returnType);
		
		// add parameters
		for (Type type : paramTypes)
		{
			result.addParameterType(type);
		}
		
		return result;
	}
	
	/**
	 * onFunctionComplexReturn
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onFunctionComplexReturn(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		FunctionType result = new FunctionType();
		List<Type> paramTypes = (List<Type>) nodes[2];
		List<Type> returnTypes = (List<Type>) nodes[6];
		
		// add parameters
		for (Type type : paramTypes)
		{
			result.addParameterType(type);
		}
		
		for (Type returnType : returnTypes)
		{
			result.addReturnType(returnType);
		}
		
		return result;
	}
	
	/**
	 * onInternal
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onInternal(IReductionContext context)
	{
		this._isInternal = true;
		this._isPrivate = false;
		
		return "";
	}
	
	/**
	 * onLocation
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onLocation(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme locationLexeme = (Lexeme) nodes[0];
		String location = locationLexeme.getText();
		
		this._typeManager.setLocation(location);
		
		return location;
	}
	
	/**
	 * onMethod
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onMethod(IReductionContext context)
	{
		this._isMethod = true;
		
		return "";
	}
	
	/**
	 * onNamespace
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onNamespace(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme typeLexeme = (Lexeme) nodes[2];
		String name = this.importName(typeLexeme.getText());
		Namespace namespace = new Namespace(name);
		
		// add to list of namespaces
		this._namespaces.add(namespace);
		
		// set namespace flag
		this._isNamespace = true;
		
		return namespace;
	}
	
	/**
	 * onNamespaceText
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onNamespaceText(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme typeLexeme = (Lexeme) nodes[2];
		String name = typeLexeme.getText();
		Namespace namespace = new Namespace(name);
		String description = (String) nodes[4];
		
		// set description
		namespace.setDescription(description);
		
		// add to list of namespaces
		this._namespaces.add(namespace);
		
		// set namespace flag
		this._isNamespace = true;
		
		return namespace;
	}
	
	/**
	 * onNextLine
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onNextLine(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme nextLineLexeme = (Lexeme) nodes[0];
		String nextLineText = nextLineLexeme.getText();
		int index = nextLineText.indexOf('=');
		String nextLine = nextLineText.substring(index + 1);
		
		this._nextLine = nextLine;
		
		return nextLine;
	}
	
	/**
	 * onNoParameterTypes
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onNoParameterTypes(IReductionContext context)
	{
		return new ArrayList<Type>();
	}
	
	/**
	 * onNoReturnType
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onNoReturnTypes(IReductionContext context)
	{
		return new ArrayList<Type>();
	}
	
	/**
	 * onOptionalParamName
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onOptionalParamName(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme lexeme = (Lexeme) nodes[1];
		String name = lexeme.getText();
		Parameter result = new Parameter(name);
		
		result.setUsage("optional");
		
		return result;
	}
	
	/**
	 * onOptionalVarArgs
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onOptionalVarArgs(IReductionContext context)
	{
		Parameter result = new Parameter("...");
		
		result.setUsage("zero-or-more");
		
		return result;
	}
	
	/**
	 * onOverview
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onOverview(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		String text = (String) nodes[1];
		
		// add overview text to list
		this._typeManager.addOverview(text);
		
		// set overview flag
		this._isOverview = true;
		
		return text;
	}
	
	/**
	 * onParam
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onParam(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		List<Type> types = (List<Type>) nodes[2];
		Parameter result = (Parameter) nodes[4];
		
		// add types
		for (Type type : types)
		{
			result.addType(type);
		}
		
		this._parameters.add(result);
		
		return "";
	}
	
	
	/**
	 * onParameterTypes
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onParameterTypes(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		
		return nodes[1];
	}
	
	/**
	 * onParamName
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onParamName(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme lexeme = (Lexeme) nodes[0];
		String name = lexeme.getText();
		Parameter result = new Parameter(name);
		
		return result;
	}
	
	/**
	 * onParamText
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onParamText(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		List<Type> types = (List<Type>) nodes[2];
		Parameter result = (Parameter) nodes[4];
		String text = (String) nodes[5];
		
		result.setDescription(text);
		
		// add types
		for (Type type : types)
		{
			result.addType(type);
		}
		
		this._parameters.add(result);
		
		return "";
	}
	
	/**
	 * onPassThrough
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onPassThrough(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		
		return nodes[0];
	}
	
	/**
	 * onPrivate
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onPrivate(IReductionContext context)
	{
		this._isPrivate = true;
		this._isInternal = false;
		
		return "";
	}
	
	/**
	 * onProcedure
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onProcedure(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		FunctionType result = new FunctionType();
		List<Type> paramTypes = (List<Type>) nodes[2];
		
		// add parameters
		for (Type type : paramTypes)
		{
			result.addParameterType(type);
		}
		
		return result;
	}
	
	/**
	 * onProperty
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onProperty(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		
		// set property flag
		this._isProperty = true;
		
		// and clear method flag
		this._isMethod = false;
		
		// save types for later processing
		this._propertyTypes = (List<Type>) nodes[2];
		
		return "";
	}
	
	/**
	 * onReturn
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onReturn(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		List<Type> types = (List<Type>) nodes[2];
		
		// add types
		for (Type type : types)
		{
			ReturnType result = new ReturnType(type);
			
			this._returnTypes.add(result);
		}
		
		return "";
	}
	
	/**
	 * onReturnText
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onReturnText(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		List<Type> types = (List<Type>) nodes[2];
		String text = (String) nodes[4];
		
		// add types
		for (Type type : types)
		{
			ReturnType result = new ReturnType(type);
			
			result.setDescription(text);
			
			this._returnTypes.add(result);
		}
		
		return "";
	}
	
	/**
	 * onReturnType
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onReturnType(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		List<Type> result = new ArrayList<Type>();
		Type returnType = (Type) nodes[1];
		
		result.add(returnType);
		
		return result;
	}
	
	/**
	 * onReturnTypes
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onReturnTypes(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		
		return nodes[2];
	}
	
	/**
	 * onSeeText
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onSeeText(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		String reference = (String) nodes[1];
		
		this._references.add(reference);
		
		return reference;
	}
	
	/**
	 * onSimpleArrayType
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onSimpleArrayType(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme lexeme = (Lexeme) nodes[0];
		String memberTypeName = this.importName(lexeme.getText());
		Type memberType = this._typeManager.getType(memberTypeName);
		
		return new ArrayType(memberType);
	}
	
	/**
	 * onSimpleFunction
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onSimpleFunction(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		FunctionType result = new FunctionType();
		Type returnType = (Type) nodes[2];
		
		result.addReturnType(returnType);
		
		return result;
	}
	
	
	/**
	 * onSimpleFunction2
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onSimpleFunction2(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		FunctionType result = new FunctionType();
		Type returnType = (Type) nodes[4];
		
		result.addReturnType(returnType);
		
		return result;
	}
	
	/**
	 * onSimpleFunctionComplexReturn
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onSimpleFunctionComplexReturn(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		FunctionType result = new FunctionType();
		List<Type> returnTypes = (List<Type>) nodes[3];
		
		for (Type returnType : returnTypes)
		{
			result.addReturnType(returnType);
		}
		
		return result;
	}
	
	/**
	 * onSimpleFunctionComplexReturn2
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onSimpleFunctionComplexReturn2(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		FunctionType result = new FunctionType();
		List<Type> returnTypes = (List<Type>) nodes[5];
		
		for (Type returnType : returnTypes)
		{
			result.addReturnType(returnType);
		}
		
		return result;
	}
	
	/**
	 * onSimpleProcedure
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onSimpleProcedure(IReductionContext context)
	{
		return new FunctionType();
	}
	
	/**
	 * onTagsBlock
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onTagsBlock(IReductionContext context)
	{
		this.emitBlock(null);
		
		return "";
	}
	
	/**
	 * onTestFunction
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Object onTestFunction(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		FunctionType result = new FunctionType();
		List<Type> paramTypes = (List<Type>) nodes[1];
		List<Type> returnTypes = (List<Type>) nodes[2];
		
		// add parameters
		for (Type type : paramTypes)
		{
			result.addParameterType(type);
		}
		
		// add return types
		for (Type returnType : returnTypes)
		{
			result.addReturnType(returnType);
		}
		
		return result;
	}
	
	/**
	 * onText
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onText(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme lexeme = (Lexeme) nodes[0];
		String text = lexeme.getText();
		
		return text;
	}
	
	/**
	 * onTextBlock
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onTextBlock(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		String text = (String) nodes[1];
		
		this.emitBlock(text);
		
		return text;
	}
	
	/**
	 * onType
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onType(IReductionContext context)
	{
		Object[] nodes = context.getNodes();
		Lexeme lexeme = (Lexeme) nodes[0];
		String typeName = this.importName(lexeme.getText());
		
		return this._typeManager.getType(typeName);
	}
	
	/**
	 * onUnknown
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onUnknown(IReductionContext context)
	{
		// do nothing
		
		return "";
	}
	
	/**
	 * onUnknownText
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onUnknownText(IReductionContext context)
	{
		// do nothing
		
		return "";
	}
	
	/**
	 * onVarArgs
	 * 
	 * @param action
	 * @param nodes
	 * @return
	 */
	public Object onVarArgs(IReductionContext context)
	{
		Parameter result = new Parameter("...");
		
		result.setUsage("one-or-more");
		
		return result;
	}
	
	/**
	 * processAbstractMethod
	 * 
	 * @param method
	 */
	private void processAbstractMethod(AbstractMethod abstractMethod, String description)
	{
		// make sure we tag this method with all browser types
		abstractMethod.addBrowser(this._typeManager.getBrowser(), this._typeManager.getBrowserVersion());
		
		// apply description
		if (description != null && description.length() > 0)
		{
			abstractMethod.setDescription(description);
		}
		
		// add parameters
		for (Parameter parameter : this._parameters)
		{
			abstractMethod.addParameter(parameter);
		}
		
		// add return types
		for (ReturnType returnType : this._returnTypes)
		{
			abstractMethod.addReturnType(returnType);
		}
		
		// apply exceptions
		for (Exception exception : this._exceptions)
		{
			abstractMethod.addException(exception);
		}
		
		// apply references
		for (String reference : this._references)
		{
			abstractMethod.addReference(reference);
		}
		
		// apply visibility
		if (this._isAdvanced)
		{
			abstractMethod.setVisibility(ADVANCED);
		}
		else if (this._isPrivate)
		{
			abstractMethod.setVisibility(PRIVATE);
		}
		else if (this._isInternal)
		{
			abstractMethod.setVisibility(INTERNAL);
		}
	}
	
	/**
	 * processConstructor
	 */
	private void processConstructor(String alias, String description)
	{
		// get owning type
		Type type = this._typeManager.getType(alias);
		
		// apply superclass
		if (this._extendsTypes != null && this._extendsTypes.size() > 0)
		{
			// TODO: Only supporting single inheritance
			Type superClass = this._extendsTypes.get(0);
			
			type.setSuperClass(superClass);
		}
		
		// add examples
		for (String example : this._examples)
		{
			type.addExample(example);
		}
		
		// create constructor
		Constructor constructor = new Constructor();
		
		// do common method processing
		this.processAbstractMethod(constructor, description);
		
		// add to type
		if (type.hasConstructor(constructor))
		{
			Constructor ctor = type.getConstructor(constructor);
			
			ctor.addBrowser(this._typeManager.getBrowser(), this._typeManager.getBrowserVersion());
			
			// TODO: Should merge constructors
		}
		else
		{
			type.addConstructor(constructor);
		}
	}
	
	/**
	 * processMethod
	 * 
	 * @param alias
	 * @param description
	 */
	private void processMethod(String alias, String description)
	{
		// get owning type
		Type type = this._typeManager.getMemberOwningType(alias);
		
		if (type != null)
		{
			// get method name
			String methodName = this._typeManager.getMemberName(alias);
			
			// create method
			Method method = new Method(methodName);
			
			// set static or instance
			String scope = this._typeManager.isInstanceMember(alias) ? INSTANCE : STATIC;
			
			method.setScope(scope);
			
			// add examples
			for (String example : this._examples)
			{
				method.addExample(example);
			}
			
			// do common method processing
			this.processAbstractMethod(method, description);
			
			// add to type
			if (type.hasMethod(method))
			{
				Method m = type.getMethod(method);
				
				m.addBrowser(this._typeManager.getBrowser(), this._typeManager.getBrowserVersion());
			}
			else
			{
				type.addMethod(method);
			}
		}
	}
	
	/**
	 * processProperty
	 * 
	 * @param alias
	 * @param description
	 */
	private void processProperty(String alias, String description)
	{
		// get owning type
		Type type = this._typeManager.getMemberOwningType(alias);
		
		if (type != null)
		{
			// get property name
			String propertyName = this._typeManager.getMemberName(alias);
			
			// create property
			Property property = new Property(propertyName);
			
			// make sure we tag this property with all browser types
			property.addBrowser(this._typeManager.getBrowser(), this._typeManager.getBrowserVersion());
			
			// set static or instance
			String scope = this._typeManager.isInstanceMember(alias) ? INSTANCE : STATIC;
			
			property.setScope(scope);
			
			// apply description
			if (description != null && description.length() > 0)
			{
				property.setDescription(description);
			}
			
			// add types
			for (Type propertyType : this._propertyTypes)
			{
				property.addType(propertyType);
			}
			
			// add examples
			for (String example : this._examples)
			{
				property.addExample(example);
			}
			
			// apply references
			for (String reference : this._references)
			{
				property.addReference(reference);
			}
			
			// apply visibility
			if (this._isAdvanced)
			{
				property.setVisibility(ADVANCED);
			}
			else if (this._isPrivate)
			{
				property.setVisibility(PRIVATE);
			}
			else if (this._isInternal)
			{
				property.setVisibility(INTERNAL);
			}
			
			// add to owning type
			if (type.hasProperty(property))
			{
				Property p = type.getProperty(property);
				
				// make sure we tag this property with all browser types
				p.addBrowser(this._typeManager.getBrowser(), this._typeManager.getBrowserVersion());
				
				// TODO: Should merge properties
			}
			else
			{
				type.addProperty(property);
			}
		}
	}
	
	/**
	 * processType
	 * 
	 * @param alias
	 * @param description
	 */
	private void processType(String alias, String description)
	{
		// get type
		Type type = this._typeManager.getType(alias);
		
		// apply superclass
		if (this._extendsTypes != null && this._extendsTypes.size() > 0)
		{
			// TODO: Only supporting single inheritance
			Type superClass = this._extendsTypes.get(0);
			
			type.setSuperClass(superClass);
		}
		
		// apply description
		if (description != null && description.length() > 0)
		{
			type.setDescription(description);
		}
		
		// apply references
		for (String reference : this._references)
		{
			type.addReference(reference);
		}
		
		// apply visibility
		if (this._isAdvanced)
		{
			type.setVisibility(ADVANCED);
		}
		else if (this._isPrivate)
		{
			type.setVisibility(PRIVATE);
		}
		else if (this._isInternal)
		{
			type.setVisibility(INTERNAL);
		}
	}
	
	/**
	 * @see com.aptana.ide.parsing.bnf.IReductionHandler#beforeParse()
	 */
	public void beforeParse()
	{
		this._aliases.clear();
		this._parameters.clear();
		this._examples.clear();
		this._exceptions.clear();
		this._returnTypes.clear();
		this._propertyTypes.clear();
		this._references.clear();
		this._extendsTypes.clear();
		this._nextLine = null;
		
		this._isConstructor = false;
		this._isInternal = false;
		this._isMethod = true;
		this._isPrivate = false;
		this._isProperty = false;
		this._isAdvanced = false;
		
		this._isNamespace = false;
		this._isClassDescription = false;
		this._isOverview = false;
	}
	
	/**
	 * setBrowser
	 * 
	 * @param browser
	 */
	public void setBrowser(String browser)
	{
		this._typeManager.setBrowser(browser);
	}
	
	/**
	 * setBrowserVersion
	 * 
	 * @param browser
	 */
	public void setBrowserVersion(String browserVersion)
	{
		this._typeManager.setBrowserVersion(browserVersion);
	}
}
