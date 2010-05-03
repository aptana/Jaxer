/**
 * Copyright (c) 2005-2006 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.sdoc;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * @author Kevin Lindsey
 */
public class MoveComments
{
	private static final String DOCUMENTATION_END = "*/";
	private static final String DOCUMENTATION_START = "/**";
	private static final String ALIAS_TAG = "@alias";
	private static final String THIS_DOT = "this.";
	private static final String IN_PLACE_INVOCATION = "(function() {";

	private String _newDir;
	private String _oldDir;
	private String _outDir;

	/**
	 * MoveComments
	 * 
	 * @param oldDir
	 * @param newDir
	 * @param outDir
	 */
	public MoveComments(String oldDir, String newDir, String outDir)
	{
		this._oldDir = oldDir;
		this._newDir = newDir;
		this._outDir = outDir;
	}

	/**
	 * applyComments
	 * 
	 * @param filename
	 * @param comments
	 * @throws IOException
	 */
	private void applyComments(String filename, Map<String,List<Comment>> comments) throws IOException
	{
		// read new file's text
		StringBuffer buffer = this.getNewFileSource(filename);

		// create output file
		File outputFile = new File(this._outDir + filename);

		// make sure our file's directory exists
		outputFile.getParentFile().mkdirs();

		// insert comments
		Iterator<Map.Entry<String, List<Comment>>> i = comments.entrySet().iterator();

		while (i.hasNext())
		{
			Map.Entry<String, List<Comment>> entry = i.next();
			String followingLine = entry.getKey();
			List<Comment> values = entry.getValue();
			int offset = 0;

			for (int j = 0; j < values.size(); j++)
			{
				Comment comment = values.get(j);
				int currentOffset = offset;

				offset = buffer.indexOf(followingLine, currentOffset);

				// handle case where this constructs changed to object literals
				if (offset == -1)
				{
					if (followingLine.startsWith(THIS_DOT))
					{
						String transformedLine = followingLine.substring(THIS_DOT.length()).replace('=', ':').replaceAll(";$", ",");
						
						offset = buffer.indexOf(transformedLine, currentOffset);
						
						if (offset == -1)
						{
							transformedLine = transformedLine.replaceAll(" : ", ": ");
							
							offset = buffer.indexOf(transformedLine, currentOffset);
						}
					}
				}
				
				if (offset != -1)
				{
					int lineStart = offset;
					
					if (offset > 0)
					{
						lineStart = this.findLineStart(buffer, offset - 1);
					}
						
					if (this.followsMultiLineComment(buffer, lineStart - 1) == false)
					{
						// append inserted text
						String prefix = buffer.substring(lineStart, offset);
						buffer.insert(lineStart, prefix + "\n");
						buffer.insert(offset, comment.getText());
						
						offset += prefix.length() + 1 + comment.getText().length() + 1;
					}
					else
					{
						String alias = this.extractAlias(comment.getText());
						
						offset += this.injectAlias(buffer, lineStart, alias) + 1;
					}
				}
				else
				{
					System.out.println("  Couldn't find: " + followingLine);
					
					offset = 0;
				}
			}
		}

		// write content to new file
		FileWriter fw = new FileWriter(outputFile);
		fw.write(buffer.toString());
		fw.close();
	}

	/**
	 * extractAlias
	 *
	 * @param text
	 * @return extract alias
	 */
	private String extractAlias(String text)
	{
		int startingIndex = text.indexOf(ALIAS_TAG);
		String result = null;
		
		if (startingIndex != -1)
		{
			int endingIndex = startingIndex;
			char c = text.charAt(endingIndex);
			
			while (c != '\r' && c != '\n')
			{
				endingIndex++;
				
				if (endingIndex < text.length())
				{
					c = text.charAt(endingIndex);
				}
				else
				{
					break;
				}
			}
			
//			if (endingIndex < text.length())
//			{
//				if (text.charAt(endingIndex) == '\r')
//				{
//					endingIndex++;
//				}
//				
//				if (endingIndex < text.length())
//				{
//					endingIndex++;
//				}
//			}
			
			result = text.substring(startingIndex, endingIndex);
		}
		
		return result;
	}

	/**
	 * extractComments
	 * 
	 * @param filename
	 * @throws IOException
	 */
	private Map<String,List<Comment>> extractComments(String filename) throws IOException
	{
		Map<String,List<Comment>> results = new HashMap<String,List<Comment>>();

		File source = new File(this._oldDir + filename);
		FileInputStream fis = new FileInputStream(source);
		String text = FileUtils.getStreamText(fis);

		fis.close();

		// search for comments that contain @alias
		int commentStart = text.indexOf(DOCUMENTATION_START);

		while (commentStart != -1)
		{
			int commentEnd = text.indexOf(DOCUMENTATION_END, commentStart);

			if (commentEnd != -1)
			{
				commentEnd += DOCUMENTATION_END.length();
				String commentText = text.substring(commentStart, commentEnd);

				if (commentText.indexOf(ALIAS_TAG) != -1)
				{
					String followingLine = this.getFollowingLine(text, commentEnd);
					String precedingLine = this.getPrecedingLine(text, commentStart);
					String sourceToMatch = (precedingLine.equals(IN_PLACE_INVOCATION)) ? precedingLine : followingLine;
					//String sourceToMatch = followingLine;
					
					Comment comment = new Comment(commentText, sourceToMatch);

					if (results.containsKey(sourceToMatch))
					{
						List<Comment> items = results.get(sourceToMatch);

						items.add(comment);
					}
					else
					{
						List<Comment> items = new ArrayList<Comment>();

						items.add(comment);

						results.put(sourceToMatch, items);
					}
				}

				commentStart = text.indexOf(DOCUMENTATION_START, commentEnd);
			}
			else
			{
				commentStart = -1;
			}
		}

		return results;
	}

	/**
	 * getPrecedingLine
	 *
	 * @param text
	 * @param commentStart
	 * @return preceding line's text
	 */
	private String getPrecedingLine(String text, int commentStart)
	{
		int endingIndex = commentStart - 1;
		
		// move just before end of previous line
		while (endingIndex >= 0)
		{
			char c = text.charAt(endingIndex);
			
			if (c == '\n')
			{
				if (endingIndex >= 0 && text.charAt(endingIndex) == '\r')
				{
					endingIndex--;
				}
				
				break;
			}
			else if (c == '\r')
			{
				break;
			}
			else
			{
				endingIndex--;
			}
		}
		
		int startingIndex = endingIndex - 1;
		
		// move to beginning of this line
		while (startingIndex >= 0)
		{
			char c = text.charAt(startingIndex);
			
			if (c == '\n' || c == '\r')
			{
				startingIndex++;
				break;
			}
			else
			{
				startingIndex--;
			}
		}
		
		return text.substring(startingIndex, endingIndex);
	}

	/**
	 * findLineStart
	 *
	 * @param buffer
	 * @param i
	 * @return line start
	 */
	private int findLineStart(StringBuffer buffer, int offset)
	{
		int lineStart = offset;

		// move backwards to start of line
		char c = buffer.charAt(lineStart);

		while (Character.isWhitespace(c))
		{
			if (c == '\n' || c == '\r')
			{
				lineStart++;

				break;
			}
			else
			{
				lineStart--;

				if (lineStart == 0)
				{
					break;
				}
				else
				{
					c = buffer.charAt(lineStart);
				}
			}
		}
		
		return lineStart;
	}

	/**
	 * followsMultiLineComment
	 *
	 * @param buffer
	 * @param lineStart
	 * @return returns true if the current offset follows a multi-line comment (ignores what space)
	 */
	private boolean followsMultiLineComment(StringBuffer buffer, int lineStart)
	{
		int offset = lineStart;
		boolean result = false;
		
		while (offset >= 0 && Character.isWhitespace(buffer.charAt(offset)))
		{
			offset--;
		}
		
		if (offset - 1 >= 0)
		{
			result = buffer.charAt(offset - 1) == '*' && buffer.charAt(offset) == '/';
		}
		
		return result;
	}

	/**
	 * getFiles
	 * 
	 * @param path
	 * @return All js files
	 * @throws IOException
	 */
	private String[] getFiles(String path) throws IOException
	{
		File dir = new File(path);
		final List<File> directories = new ArrayList<File>();
		List<File> results = new ArrayList<File>();

		directories.add(dir);

		while (directories.size() > 0)
		{
			File currentDirectory = directories.get(0);

			directories.remove(0);

			File[] files = currentDirectory.listFiles(new FileFilter()
			{
				/**
				 * accept
				 * 
				 * @param pathname
				 * @return boole
				 */
				public boolean accept(File pathname)
				{
					boolean result = false;

					if (pathname.isDirectory())
					{
						directories.add(pathname);
					}
					else
					{
						result = pathname.getName().toLowerCase().endsWith(".js");
					}

					return result;
				}
			});

			results.addAll(Arrays.asList(files));
		}

		String[] result = new String[results.size()];
		int baseLength = dir.getCanonicalPath().length();

		for (int i = 0; i < results.size(); i++)
		{
			File file = results.get(i);

			result[i] = file.getCanonicalPath().substring(baseLength);
		}

		return result;
	}

	/**
	 * getFollowingLine
	 * 
	 * @param text
	 * @param commentEnd
	 * @return following line text without leading whitespace
	 */
	private String getFollowingLine(String text, int commentEnd)
	{
		int index = commentEnd;

		// find end of current line
		while (index < text.length())
		{
			char c = text.charAt(index);

			if (Character.isWhitespace(c))
			{
				index++;
			}
			else
			{
				break;
			}
		}

		int startingIndex = index;

		if (startingIndex < text.length())
		{
			index++;
	
			// find eol
			while (index < text.length())
			{
				char c = text.charAt(index);
	
				if (c == '\r' || c == '\n')
				{
					break;
				}
				else
				{
					index++;
				}
			}
		}

		return text.substring(startingIndex, index);
	}

	/**
	 * getNewFileSource
	 *
	 * @param filename
	 * @return file source buffer
	 * @throws IOException 
	 */
	private StringBuffer getNewFileSource(String filename) throws IOException
	{
		// read new file's text
		File source = new File(this._newDir + filename);
		FileInputStream fis = new FileInputStream(source);
		String text = FileUtils.getStreamText(fis);
		StringBuffer buffer = new StringBuffer(text);

		fis.close();
		
		return buffer;
	}

	/**
	 * injectAlias - assumes a multi-line comment precedes lineStart
	 *
	 * @param buffer
	 * @param offset
	 * @param alias
	 * @return inserted character count
	 */
	private int injectAlias(StringBuffer buffer, int offset, String alias)
	{
		int result = 0;
		
		while (offset >= 0 && buffer.charAt(offset) != '/')
		{
			offset--;
		}
		
		if (offset >= 0)
		{
			offset--;
			
			int lineStart = this.findLineStart(buffer, offset - 1);
			
			String prefix = buffer.substring(lineStart, offset);
			String insertText = "* " + alias;
			
			buffer.insert(lineStart, prefix + "\n");
			buffer.insert(offset, insertText);
			
			result = prefix.length() + 1 + insertText.length();
		}
		
		return result;
	}

	/**
	 * moveComments
	 * 
	 * @param originalFiles
	 * @param newFiles
	 * @throws IOException
	 */
	public void moveComments(String[] originalFiles, String[] newFiles) throws IOException
	{
		for (int i = 0; i < originalFiles.length; i++)
		{
			String filename = originalFiles[i];

			if (Arrays.binarySearch(newFiles, filename) >= 0)
			{
				// extract comments
				Map<String,List<Comment>> commentsByLine = this.extractComments(filename);

				if (commentsByLine.size() > 0)
				{
					System.out.println("processing " + filename);

					this.applyComments(filename, commentsByLine);
				}
			}
		}
	}

	/**
	 * updateComments
	 * 
	 * @throws IOException
	 */
	public void updateComments() throws IOException
	{
		String[] originalFiles = this.getFiles(this._oldDir);
		String[] newFiles = this.getFiles(this._newDir);

		Arrays.sort(newFiles);

		this.moveComments(originalFiles, newFiles);
	}
}
