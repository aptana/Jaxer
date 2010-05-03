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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.aptana.docgen.reflection.TypeManager;
import com.aptana.ide.lexer.IEnumerationMap;
import com.aptana.ide.lexer.ILexer;
import com.aptana.ide.lexer.Lexeme;
import com.aptana.ide.lexer.LexemeList;
import com.aptana.ide.lexer.LexerException;
import com.aptana.ide.lexer.matcher.MatcherTokenList;
import com.aptana.ide.parsing.IParseState;
import com.aptana.ide.parsing.bnf.LRParser;
import com.aptana.ide.parsing.bnf.LRParserBuilder;
import com.aptana.sdoc.FileUtils;

/**
 * @author Kevin Lindsey
 */
public class DocGenCreator
{
	private static final Pattern SINGLE_LINE_COMMENT = Pattern.compile("^\\s*//");
	private static final String DOCUMENTATION_END = "*/";
	private static final String DOCUMENTATION_START = "/**";
	
	private static final int SUCCESS = 0;
	private static final int ERROR = 1;
	
	private String _browser;
	private String _browserVersion;
	private LRParser _parser;
	private DocGenHandler _handler;
	private boolean _verbose;
	private boolean _showWarnings;

	/**
	 * main
	 * 
	 * @param args
	 */
	public static void main(String[] args)
	{
		int status = SUCCESS;
		
		if (args.length > 0)
		{
			DocGenCreator creator = new DocGenCreator();
			TypeManager typeManager = creator._handler.getTypeManager();
			boolean outputXML = false;
			String outputFilename = null;
			List<String> filters = new ArrayList<String>();

			// process args
			for (int i = 0; i < args.length; i++)
			{
				String arg0 = args[i];

				if (arg0.startsWith("-"))
				{
					if (arg0.equals("-v") || arg0.equals("--verbose"))
					{
						creator._verbose = true;
					}
					else if (arg0.equals("+v") || arg0.equals("++verbose"))
					{
						creator._verbose = false;
					}
					else if (arg0.equals("-fp") || arg0.equals("--filter-private"))
					{
						typeManager.setFilterPrivate(true);
					}
					else if (arg0.equals("+fp") || arg0.equals("++filter-private"))
					{
						typeManager.setFilterPrivate(false);
					}
					else if (arg0.equals("-l") || arg0.equals("--show-locations"))
					{
						typeManager.setShowLocations(true);
					}
					else if (arg0.equals("+l") || arg0.equals("++show-locations"))
					{
						typeManager.setShowLocations(false);
					}
					else if (arg0.equals("-w") || arg0.equals("--show-warnings"))
					{
						creator._showWarnings = true;
					}
					else if (arg0.equals("+w") || arg0.equals("++show-warnings"))
					{
						creator._showWarnings = false;
					}
					else
					{
						// advance to next argument
						i++;
	
						String arg1 = (i < args.length) ? trim(args[i]) : null;
	
						if (arg1 == null)
						{
							// set error status
							status = ERROR;
							
							// emit error
							System.err.println("There appears to be a missing argument after " + arg0);
							System.err.println("Run this tool with no arguments for additional help");
							break;
						}
						else if (arg0.equals("-b") || arg0.equals("--browser"))
						{
							creator._browser = arg1;
						}
						else if (arg0.equals("-bv") || arg0.equals("--browser-version"))
						{
							creator._browserVersion = arg1;
						}
						else if (arg0.equals("-e") || arg0.equals("--extract"))
						{
							System.out.println(creator.extractFileComments(arg1));
						}
						else if (arg0.equals("-f") || arg0.equals("--filter"))
						{
							filters.add(arg1);
						}
						else if (arg0.equals("-i") || arg0.equals("--import"))
						{
							int index = arg1.indexOf('=');
							
							if (index != -1 && index < arg1.length() - 1)
							{
								String targetName = arg1.substring(0, index);
								String sourceName = arg1.substring(index + 1);
								
								//typeManager.importTypes(sourceName, targetName);
								creator._handler.addImport(sourceName, targetName);
							}
						}
						else if (arg0.equals("-o") || arg0.equals("--output-file"))
						{
							outputFilename = arg1;
						}
						else if (arg0.equals("-r") || arg0.equals("--resource"))
						{
							creator.processResource(arg1);
							
							// handle namespaces
							creator._handler.applyNamespaces();
							
							outputXML = true;
						}
						else
						{
							// set error status
							status = ERROR;
							
							// emit error
							System.err.println("Unrecognized command-line argument: " + arg0);
							System.err.println("Run this tool with no arguments for additional help");
							break;
						}
					}
				}
				else
				{
					status = creator.processFile(trim(arg0));
					
					// handle namespaces
					creator._handler.applyNamespaces();
					
					outputXML = true;
				}
				
				if (status == ERROR)
				{
					break;
				}
			}
			
			if (status == SUCCESS)
			{
				// handle XML generation
				if (outputXML)
				{
					// add filters
					for (String filter : filters)
					{
						typeManager.addFilter(filter);
					}
					
					// get XML
					String xml = typeManager.getXML();
					
					// output the results
					if (outputFilename != null)
					{
						creator.writeToFile(outputFilename, xml);
					}
					else
					{
						System.out.println(xml);
					}
					
//					// output JSON
//					try
//					{
//						JSONObject json = XML.toJSONObject(xml);
//						
//						creator.writeToFile("combined_framework.json", json.toString(4));
//					}
//					catch (JSONException e)
//					{
//						// TODO Auto-generated catch block
//						e.printStackTrace();
//					}
				}
			}
		}
		else
		{
			usage();
		}
		
		System.exit(status);
	}

	/**
	 * usage
	 */
	private static void usage()
	{
		InputStream is = DocGenCreator.class.getResourceAsStream("/support/usage.txt");

		if (is != null)
		{
			System.out.println(FileUtils.getStreamText(is));
		}
		else
		{
			System.out.println("error: usage file missing");
			System.out.println("usage: DocGenCreator <options>* <javascript-file>+");
		}
	}

	/**
	 * DocGenCreator
	 */
	public DocGenCreator()
	{
		// create docgen parser
		InputStream bnfInput = DocGenCreator.class.getResourceAsStream("/support/docgen.bnf");
		InputStream lexerInput = DocGenCreator.class.getResourceAsStream("/support/docgen.lxr");
		LRParserBuilder builder = new LRParserBuilder();

		this._parser = (LRParser) builder.buildParser(bnfInput, lexerInput);

		// ignore whitespace
		ILexer lexer = this._parser.getLexer();
		String language = this._parser.getLanguage();
		MatcherTokenList tokenList = (MatcherTokenList) lexer.getTokenList(language);
		IEnumerationMap typeMap = tokenList.getTypeMap();

		int[] set = new int[] { typeMap.getIntValue("WHITESPACE"), //$NON-NLS-1$
				typeMap.getIntValue("LINE_TERMINATOR") //$NON-NLS-1$
		};
		Arrays.sort(set);
		lexer.setIgnoreSet(language, set);

		// create handler and associate with parser
		this._handler = new DocGenHandler();
		this._parser.addHandler(this._handler);
	}

	/**
	 * extractComments
	 * 
	 * @param text
	 * @return
	 */
	private String extractComments(String name, String text)
	{
		return this.extractComments(name, text, false);
	}

	/**
	 * extractComments
	 * 
	 * @param filename
	 * @throws IOException
	 */
	private String extractComments(String name, String text, boolean includeFollowingLine)
	{
		if (this._verbose)
		{
			System.out.println("Extracting comments from source");
		}
		
		List<String> comments = new ArrayList<String>();

		int commentStart = text.indexOf(DOCUMENTATION_START);

		while (commentStart != -1)
		{
			int commentEnd = text.indexOf(DOCUMENTATION_END, commentStart);

			if (commentEnd != -1)
			{
				if (commentStart > 0)
				{
					commentStart--;

					// back up to line start
					while (commentStart >= 0)
					{
						char c = text.charAt(commentStart);

						if (Character.isWhitespace(c) == false || c == '\r' || c == '\n')
						{
							if (c != '/')
							{
								// move forward one so we don't include this character
								commentStart++;

								// exit
								break;
							}
						}

						// back up on more character
						commentStart--;
					}
				}

				// adjust ending offset to include tailing '*/'
				commentEnd += DOCUMENTATION_END.length();
				
				// find offsets of line following block
//				int nextLineStartingOffset = Math.max(text.indexOf('\r', commentEnd), text.indexOf('\n', commentEnd)) + 1;
//				int nextLineEndingOffset = Math.min(text.indexOf('\r', nextLineStartingOffset), text.indexOf('\n', nextLineStartingOffset));
				int nextLineStartingOffset = this.findLineEnd(text, commentEnd) + 1;
				int nextLineEndingOffset = this.findLineEnd(text, nextLineStartingOffset);
				
//				if (nextLineEndingOffset == -1)
//				{
//					nextLineEndingOffset = text.length();
//				}

				// include next line in comment block, if requested
				if (includeFollowingLine && nextLineEndingOffset != -1)
				{
					commentEnd = nextLineEndingOffset;
				}

				// get comment text
				String comment = text.substring(commentStart, commentEnd);
				Matcher matcher = SINGLE_LINE_COMMENT.matcher(comment);

				if (matcher.find() == false)
				{
					String location = "@location=" + name + ":" + commentStart + "-" + commentEnd;
					String nextLine = "@next-line=" + text.substring(nextLineStartingOffset, nextLineEndingOffset);
					
					comments.add(location + "\n" + nextLine + "\n" + comment);
				}
				// else
				// {
				// System.err.println("skipping commented sdoc");
				// System.err.println(comment);
				// }

				commentStart = text.indexOf(DOCUMENTATION_START, commentEnd);
			}
			else
			{
				commentStart = -1;
			}
		}

		StringWriter sw = new StringWriter();
		PrintWriter writer = new PrintWriter(sw);

		for (String comment : comments)
		{
			writer.println(comment);
		}

		return sw.toString();
	}
	
	/**
	 * findLineEnd
	 * 
	 * @param source
	 * @param startingOffset
	 * @return
	 */
	private int findLineEnd(String source, int startingOffset)
	{
		int returnChar = source.indexOf('\r', startingOffset);
		int newlineChar = source.indexOf('\n', startingOffset);
		int result = -1;
		
		if (returnChar != -1)
		{
			if (newlineChar != -1)
			{
				result = Math.min(returnChar, newlineChar);
			}
			else
			{
				result = returnChar;
			}
		}
		else
		{
			if (newlineChar != -1)
			{
				result = newlineChar;
			}
		}
		
		if (result == -1)
		{
			result = source.length();
		}
		
		return result;
	}

	/**
	 * extractFileComments
	 * 
	 * @param filename
	 * @return
	 */
	private String extractFileComments(String filename)
	{
		String source = this.getFileContents(filename);

		// extract scriptdoc comments
		return this.extractComments(filename, source, true);
	}

	/**
	 * getFileContents
	 * 
	 * @param filename
	 * @return
	 */
	private String getFileContents(String filename)
	{
		String result = "";

		try
		{
			File file = new File(filename);

			if (file.exists())
			{
				if (file.canRead())
				{
					result = FileUtils.getStreamText(new FileInputStream(file));
				}
				else
				{
					System.out.println("File is not readable: " + filename);
				}
			}
			else
			{
				System.out.println("File does not exist: " + filename);
			}
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}

		return result;
	}

	/**
	 * run
	 */
	private int processFile(String filename)
	{
		if (this._verbose)
		{
			System.out.println("Processing file: " + filename);
		}
		
		return this.processSource(filename, this.getFileContents(filename));
	}

	/**
	 * processResource
	 * 
	 * @param resource
	 */
	private void processResource(String resource)
	{
		if (this._verbose)
		{
			System.out.println("Processing resource: " + resource);
		}
		
		InputStream is = this.getClass().getResourceAsStream(resource);

		if (is != null)
		{
			this.processSource(resource, FileUtils.getStreamText(is));
		}
		else
		{
			System.out.println("Resource does not exist: " + resource);
		}
	}

	/**
	 * processComments
	 * 
	 * @param source
	 */
	private int processSource(String name, String source)
	{
		int status = SUCCESS;
		
		// extract scriptdoc comments
		String comments = this.extractComments(name, source);

		// create parse state, apply edit, and parse
		IParseState parseState = this._parser.createParseState(null);
		parseState.setEditState(comments, comments, 0, 0);

		// update browser info
		this._handler.setBrowser(this._browser);
		this._handler.setBrowserVersion(this._browserVersion);

		try
		{
			if (this._verbose)
			{
				System.out.println("Parsing source");
			}
			
			this._parser.parse(parseState);
			
			String message = this._parser.getMessage();

			if (message != null && message.length() > 0)
			{
				System.err.println("Parse error in " + name + ": " + this._parser.getMessage());
				
				LexemeList lexemes = parseState.getLexemeList();
				int i = lexemes.size() - 1;
				int start = 0;
				int end = 0;
				
				if (i >= 0)
				{
					Lexeme lexeme = lexemes.get(i);
					
					start = lexeme.offset;
					end = lexeme.getEndingOffset();
					
					i--;
				}
				
				while (i >= 0)
				{
					Lexeme lexeme = lexemes.get(i);
					
					if (lexeme.getType().equals("START_DOCUMENTATION"))
					{
						start = lexeme.offset;
						break;
					}
					else
					{
						i--;
					}
				}
				
				if (start != -1 && end != -1)
				{
					System.err.println(comments.substring(start, end));
				}
				
				status = ERROR;
			}
			else if (this._verbose)
			{
				System.out.println("Parse complete");
			}
			
			// show warnings
			if (this._showWarnings)
			{
				this.showWarnings("Missing @alias tags", this._handler.getMissingAliases(), source);
				this.showWarnings("Static this properties", this._handler.getStaticThisProperties(), source);
			}
		}
		catch (LexerException e)
		{
			e.printStackTrace();
			status = ERROR;
		}
		
		return status;
	}

	/**
	 * showWarnings
	 * 
	 * @param title
	 * @param locations
	 */
	private void showWarnings(String title, String[] locations, String source)
	{
		if (locations.length > 0)
		{
			// create an underline the width of the title
			char[] underlineChars = new char[title.length()];
			
			Arrays.fill(underlineChars, '=');
			
			// print title and underline
			System.out.println(title);
			System.out.println(new String(underlineChars));
			
			// print each location
			for (String location : locations)
			{
				int lastColon = location.lastIndexOf(":");
				int hyphen = location.indexOf('-', lastColon);
				int startingOffset = Integer.parseInt(location.substring(lastColon + 1, hyphen));
				int endingOffset = Integer.parseInt(location.substring(hyphen + 1));
				String block = source.substring(startingOffset, endingOffset);
				
				System.out.println(location);
				System.out.println(block);
			}
		}
	}
	
	/**
	 * trim
	 * 
	 * @param text
	 * @return
	 */
	private static String trim(String text)
	{
		String result = text;

		if (text != null && text.length() > 0)
		{
			int start = 0;
			int end = text.length();
			char firstChar = text.charAt(start);
			char lastChar = text.charAt(end - 1);

			if (firstChar == lastChar)
			{
				if (firstChar == '\'' || firstChar == '"')
				{
					start++;
				}
				if (lastChar == '\'' || lastChar == '"')
				{
					end--;
				}

				result = text.substring(start, end);
			}
		}

		return result;
	}

	/**
	 * writeToFile
	 * 
	 * @param filename
	 * @param text
	 */
	private void writeToFile(String filename, String text)
	{
		if (this._verbose)
		{
			System.out.println("Writing output to file: " + filename);
		}
		
		File f = new File(filename);
		FileWriter fw = null;

		try
		{
			fw = new FileWriter(f);
			fw.write(text);
		}
		catch (IOException e)
		{

		}
		finally
		{
			if (fw != null)
			{
				try
				{
					fw.close();
				}
				catch (IOException e)
				{
				}
			}
		}
	}
}
