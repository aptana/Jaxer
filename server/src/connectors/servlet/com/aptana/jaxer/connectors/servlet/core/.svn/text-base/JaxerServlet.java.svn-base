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
package com.aptana.jaxer.connectors.servlet.core;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;
import java.util.regex.Pattern;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsHandler;
import com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsProvider;
import com.aptana.jaxer.connectors.servlet.interfaces.IStatusLengthOnlyResponse;

/**
 * @author Kevin Sawicki (ksawicki@aptana.com)
 */
public class JaxerServlet extends HttpServlet implements IStatisticsProvider
{

	/**
	 * RESPONSE
	 */
	public static final String RESPONSE = ""; //$NON-NLS-1$

	/**
	 * REQUEST_PATH_PARAMETER
	 */
	public static final String REQUEST_PATH_PARAMETER = "REQUEST_PATH"; //$NON-NLS-1$

	/**
	 * REQUEST_PATH
	 */
	public static final List<String> REQUEST_PATH = Arrays.asList(new String[]{
			"/jaxer-callback", //$NON-NLS-1$
			"/jaxer-service", //$NON-NLS-1$
			"/jaxer-rpc", //$NON-NLS-1$
			"/jaxer-rest" //$NON-NLS-1$
		});

	/**
	 * serialVersionUID
	 */
	private static final long serialVersionUID = 1L;

	private IStatisticsHandler handler;
	private List<String> path;

	/**
	 * Creates a new jaxer servlet
	 */
	public JaxerServlet()
	{
		super();
		this.path = REQUEST_PATH;
	}

	/**
	 * @see javax.servlet.GenericServlet#init(javax.servlet.ServletConfig)
	 */
	public void init(ServletConfig config) throws ServletException
	{
		String newPath = config.getInitParameter(REQUEST_PATH_PARAMETER);
		if (newPath != null)
		{
			this.path = Arrays.asList(newPath.split(Pattern.quote(",")));
		}
	}

	/**
	 * @see javax.servlet.http.HttpServlet#doPost(javax.servlet.http.HttpServletRequest,
	 *      javax.servlet.http.HttpServletResponse)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException,
			IOException
	{
		doGet(request, response);
	}

	/**
	 * @see javax.servlet.http.HttpServlet#doGet(javax.servlet.http.HttpServletRequest,
	 *      javax.servlet.http.HttpServletResponse)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		String path = request.getServletPath();
		if (this.path.contains(path))
		{
			response.setContentLength(RESPONSE.length());
			response.getWriter().write(RESPONSE);
			if (this.handler != null && !(response instanceof IStatusLengthOnlyResponse))
			{
				this.handler.parseStatistics(request, response);
			}
		}
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsProvider#getStatisticsHandler()
	 */
	public IStatisticsHandler getStatisticsHandler()
	{
		return this.handler;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsProvider#setStatisticsHandler(com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsHandler)
	 */
	public void setStatisticsHandler(IStatisticsHandler handler)
	{
		this.handler = handler;
	}
}
