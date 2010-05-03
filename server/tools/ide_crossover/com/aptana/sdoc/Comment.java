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

/**
 * @author Kevin Lindsey
 */
public class Comment
{
	private String _text;
	private String _followingLine;
	
	/**
	 * Comment
	 * 
	 * @param text
	 * @param followingLine
	 */
	public Comment(String text, String followingLine)
	{
		this._text = text;
		this._followingLine = followingLine;
	}
	
	/**
	 * getText
	 *
	 * @return comment text
	 */
	public String getText()
	{
		return this._text;
	}
	
	/**
	 * getFollowingLine
	 *
	 * @return following line text
	 */
	public String getFollowingLine()
	{
		return this._followingLine;
	}
}
