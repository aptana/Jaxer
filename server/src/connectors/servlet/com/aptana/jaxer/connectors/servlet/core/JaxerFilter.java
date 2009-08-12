/** ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 * 
 *  This program is distributed in the hope that it will be useful, but
 *  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 *  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 *  is prohibited.
 * 
 *  You can redistribute and/or modify this program under the terms of the GPL, 
 *  as published by the Free Software Foundation.  You should
 *  have received a copy of the GNU General Public License, Version 3 along
 *  with this program; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *  
 *  Aptana provides a special exception to allow redistribution of this file
 *  with certain other code and certain additional terms
 *  pursuant to Section 7 of the GPL. You may view the exception and these
 *  terms on the web at http://www.aptana.com/legal/gpl/.
 *  
 *  You may view the GPL, and Aptana's exception and additional terms in the file
 *  titled license-jaxer.html in the main distribution folder of this program.
 *  
 *  Any modifications to this file must keep this entire header intact.
 * 
 * ***** END LICENSE BLOCK ***** */
package com.aptana.jaxer.connectors.servlet.core;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
import javax.servlet.ServletOutputStream;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletRequestWrapper;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpServletResponseWrapper;

import com.aptana.jaxer.connectors.servlet.interfaces.ICallbackResponse;
import com.aptana.jaxer.connectors.servlet.interfaces.IDocumentRootFilter;
import com.aptana.jaxer.connectors.servlet.interfaces.IDocumentRootResolver;
import com.aptana.jaxer.connectors.servlet.interfaces.IErrorPageFilter;
import com.aptana.jaxer.connectors.servlet.interfaces.IErrorPageHandler;
import com.aptana.jaxer.connectors.servlet.interfaces.IHostnameFilter;
import com.aptana.jaxer.connectors.servlet.interfaces.IIgnoreFilter;
import com.aptana.jaxer.connectors.servlet.interfaces.IIgnoreHandler;
import com.aptana.jaxer.connectors.servlet.interfaces.ILoggingFilter;
import com.aptana.jaxer.connectors.servlet.interfaces.ILoggingHandler;
import com.aptana.jaxer.connectors.servlet.interfaces.IRestartableFilter;
import com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsHandler;
import com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsProvider;
import com.aptana.jaxer.connectors.servlet.interfaces.IStatusLengthOnlyResponse;

/**
 * @author gregw
 * @author Kevin Sawicki (ksawicki@aptana.com)
 */
public class JaxerFilter implements Filter, IHostnameFilter, IDocumentRootFilter, IIgnoreFilter, IErrorPageFilter,
		ILoggingFilter, IStatisticsProvider, IRestartableFilter
{

	/**
	 * CALLBACK_RESPONSE
	 */
	public static final String CALLBACK_RESPONSE = ""; //$NON-NLS-1$

	/**
	 * REQUEST_PATH_PARAMETER
	 */
	public static final String REQUEST_PATH_PARAMETER = "REQUEST_PATH"; //$NON-NLS-1$

	/**
	 * JAXER_ROUTE
	 */
	public static String JAXER_ROUTE = "/jaxer-route";  //$NON-NLS-1$

	/**
	 * DEFAULT_PATTERNS
	 */
	public static String[] DEFAULT_PATTERNS = new String[] { "/jaxer-callback(/.*)?$", "^.*/jaxer-rpc(/.*)?$",
			"^.*/jaxer-service(/.*)?$", "^" + JAXER_ROUTE + "\\b.*$", };

	
	/**
	 * @param path
	 * @return - true if a callback
	 */
	public boolean isForJaxer(String path)
	{
		if (path != null)
		{
			for (String routeToJaxerServletPattern : this.callbackPath)
			{
				if (path.matches(routeToJaxerServletPattern))
				{
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * JAXER_HOSTNAME
	 */
	public static final String JAXER_HOSTNAME = "JAXER_HOSTNAME";

	/**
	 * JAXER_PORT
	 */
	public static final String JAXER_PORT = "JAXER_PORT";

	/**
	 * IGNORE_PATHS
	 */
	public static final String IGNORE_PATHS = "IGNORE_PATHS";

	/**
	 * JAXER_PROCESSED
	 */
	public static final String JAXER_PROCESSED = "JAXER_PROCESSED";

	/**
	 * Codes for responses that shouldn't go through jaxer
	 */
	public static int[] IGNORE_CODES = { 204, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413,
			414, 415, 416, 417, 500, 501, 502, 503, 504, 505 };

	private Jaxer _jaxer = null;
	private String hostname = null;
	private int port;
	private IIgnoreHandler handler = null;
	private IDocumentRootResolver resolver = null;
	private IErrorPageHandler errorHandler = null;
	private ILoggingHandler logHandler = null;
	private IStatisticsHandler statsHandler = null;
	private ServletContext context = null;
	private List<String> callbackPath = Arrays.asList(DEFAULT_PATTERNS);
	private String[] ignorePaths;

	/**
	 * @see javax.servlet.Filter#init(javax.servlet.FilterConfig)
	 */
	public void init(FilterConfig filterConfig) throws ServletException
	{
		if (filterConfig != null)
		{
			context = filterConfig.getServletContext();
			if (hostname == null)
			{
				hostname = filterConfig.getInitParameter(JAXER_HOSTNAME);
			}
			if (port < 1)
			{
				try
				{
					port = Integer.parseInt(filterConfig.getInitParameter(JAXER_PORT));
				}
				catch (Exception e)
				{
				}
			}
			if (handler == null)
			{
				String ignorables = filterConfig.getInitParameter(IGNORE_PATHS);
				if (ignorables != null)
				{
					try
					{
						ignorePaths = ignorables.split(",");
					}
					catch (Exception e)
					{
						ignorePaths = new String[0];
					}
				}
				else
				{
					ignorePaths = new String[0];
				}
			}
			String callbackPath = filterConfig.getInitParameter(REQUEST_PATH_PARAMETER);
			if (callbackPath != null)
			{
				this.callbackPath = Arrays.asList(callbackPath.split(Pattern.quote(",")));
			}
		}

		_jaxer = new Jaxer(hostname, port);
		_jaxer.setLogger(this.logHandler);

		if (this.statsHandler != null)
		{
			this.statsHandler.setData(REQUEST_PATH_PARAMETER, this.callbackPath);
		}
	}

	/**
	 * @see javax.servlet.Filter#destroy()
	 */
	public void destroy()
	{
	}

	/**
	 * @see javax.servlet.Filter#doFilter(javax.servlet.ServletRequest, javax.servlet.ServletResponse,
	 *      javax.servlet.FilterChain)
	 */
	public void doFilter(ServletRequest req, ServletResponse res, FilterChain chain) throws IOException,
			ServletException
	{
		HttpServletRequest request = (HttpServletRequest) req;
		HttpServletResponse response = (HttpServletResponse) res;
		String path = request.getServletPath();
		if (handler != null)
		{
			if (handler.shouldIgnore(req, res))
			{
				chain.doFilter(request, response);
				return;
			}
		}
		else
		{
			if (path != null)
			{
				for (int i = 0; i < ignorePaths.length; i++)
				{
					if (path.indexOf(ignorePaths[i]) != -1)
					{
						chain.doFilter(request, response);
						return;
					}
				}
			}
		}
		int statusCode = -1;

		String documentRoot = null;

		if (this.resolver != null)
		{
			documentRoot = this.resolver.getDocumentRoot(req, res);
		}
		else if (context != null)
		{
			documentRoot = context.getRealPath("/");
		}

		String pageFile = null;
		if (this.resolver != null)
		{
			pageFile = this.resolver.getPageFile(req, res);
		}
		else if (context != null)
		{
			pageFile = context.getRealPath(request.getServletPath());
		}

		// Check the response for an error status code or length of 0 before sending to jaxer
		// Get the response content length and status without setting it on the actual response object to inspect it
		BufferedResponseWrapper bufferedWrapper = new BufferedResponseWrapper(response);
		boolean callback = false;
		if (isForJaxer(path))
		{
			callback = true;
			bufferedWrapper.setContentLength(CALLBACK_RESPONSE.length());
			bufferedWrapper.getWriter().write(CALLBACK_RESPONSE);
			if (this.statsHandler != null)
			{
				this.statsHandler.parseStatistics(request, bufferedWrapper);
			}
		} else if (!(new File(pageFile).exists()))
		{
			// BEGIN HACK
			// Rewrite the request path by prefixing it with JAXER_ROUTE
			// This is (in a crude way) trying to emulate the 
			try {
				path = JAXER_ROUTE + path;
				Method setRequestURI = request.getClass().getMethod("setRequestURI", String.class);
				setRequestURI.invoke(request, path);
				Method setServletPath = request.getClass().getMethod("setServletPath", String.class);
				setServletPath.invoke(request, path);
				// Re-adjust pageFile
				if (this.resolver != null)
				{
					pageFile = this.resolver.getPageFile(req, res);
				}
				else if (context != null)
				{
					pageFile = context.getRealPath(request.getServletPath());
				}
				// Re-adjust documentRoot
				if (this.resolver != null)
				{
					documentRoot = this.resolver.getDocumentRoot(req, res);
				}
				else if (context != null)
				{
					documentRoot = context.getRealPath("/");
				}
				// Treat the request similar to callback
				callback = true;
				bufferedWrapper.setContentLength(CALLBACK_RESPONSE.length());
				bufferedWrapper.getWriter().write(CALLBACK_RESPONSE);
				if (this.statsHandler != null)
				{
					this.statsHandler.parseStatistics(request, bufferedWrapper);
				}
			} catch (SecurityException e) {
				if (logHandler != null) logHandler.logError("Exception in jaxer-route", e);return;
			} catch (NoSuchMethodException e) {
				if (logHandler != null) logHandler.logError("Exception in jaxer-route", e);return;
			} catch (IllegalArgumentException e) {
				if (logHandler != null) logHandler.logError("Exception in jaxer-route", e);return;
			} catch (IllegalAccessException e) {
				if (logHandler != null) logHandler.logError("Exception in jaxer-route", e);return;
			} catch (InvocationTargetException e) {
				if (logHandler != null) logHandler.logError("Exception in jaxer-route", e);return;
			}
			// END HACK
		}
		else
		{
			chain.doFilter(request, bufferedWrapper);
		}

		statusCode = bufferedWrapper.getStatus();
		for (int i = 0; i < IGNORE_CODES.length; i++)
		{
			if (IGNORE_CODES[i] == statusCode)
			{
				chain.doFilter(request, response);
				return;
			}
		}

		if (bufferedWrapper.getContentLength() < 1 && !callback)
		{
			chain.doFilter(request, response);
			return;
		}

		ResponseWrapper wrappedResponse = null;
		RequestWrapper wrappedRequest = null;
		Connection connection = null;
		try
		{
			connection = _jaxer.beginRequest(callback);

			if(callback)
			{
				wrappedResponse = new CallbackResponseWrapper(connection, response);
			}
			else
			{
				wrappedResponse = new ResponseWrapper(connection, response);
			}
			
			wrappedRequest = null;

			Enumeration names = request.getHeaderNames();
			while (names.hasMoreElements())
			{
				String name = (String) names.nextElement();
				if (name.length() > 0)
				{
					if (name.length() == 1)
					{
						name = name.substring(0, 1).toUpperCase();
					}
					else
					{
						name = name.substring(0, 1).toUpperCase() + name.substring(1, name.length());
					}
				}
				connection.addHeader(name, request.getHeaders(name));
			}
			if (!connection.isHandler())
			{
				for (String name : bufferedWrapper.headers.keySet())
				{
					if (name.length() > 0)
					{
						if (name.length() == 1)
						{
							name = name.substring(0, 1).toUpperCase();
						}
						else
						{
							name = name.substring(0, 1).toUpperCase() + name.substring(1, name.length());
						}
					}
					connection.addResponseHeader(name, bufferedWrapper.headers.get(name));
				}
			}
			String pathInfo = request.getPathInfo();
			if (pathInfo == null)
			{
				pathInfo = ""; //$NON-NLS-1$
			}
			String scriptName = request.getRequestURI().substring(0,
					request.getRequestURI().length() - pathInfo.length());
			String pathTranslated = request.getPathTranslated();

			int len = req.getContentLength();
			if (len < 0)
			{
				len = 0;
			}
			if ((pathTranslated == null) || (pathTranslated.length() == 0))
			{
				pathTranslated = request.getRequestURI();
			}

			String query = request.getQueryString();
			String reqURI = request.getRequestURI();
			if (query != null && reqURI != null)
			{
				reqURI += "?" + query;
			}
			connection.addEnv("REQUEST_URI", reqURI); //$NON-NLS-1$
			connection.addEnv("QUERY_STRING", query); //$NON-NLS-1$
			connection.addEnv("AUTH_TYPE", request.getAuthType()); //$NON-NLS-1$
			connection.addEnv("CONTENT_LENGTH", Integer.toString(len)); //$NON-NLS-1$
			if (wrappedResponse != null)
			{
				connection.addEnv("DOC_CONTENT_TYPE", wrappedResponse.getContentType());
			}
			connection.addEnv("CONTENT_TYPE", req.getContentType()); //$NON-NLS-1$
			connection.addEnv("GATEWAY_INTERFACE", "CGI/1.1"); //$NON-NLS-1$ //$NON-NLS-2$
			connection.addEnv("PATH_INFO", pathInfo); //$NON-NLS-1$
			connection.addEnv("PATH_TRANSLATED", pathTranslated); //$NON-NLS-1$

			String localAddr = req.getLocalAddr();
			int localPort = req.getLocalPort();
			if (localAddr != null && localPort > -1)
			{
				connection.addEnv("HTTP_HOST", localAddr + ":" + localPort);
			}

			connection.addEnv("REMOTE_ADDR", req.getRemoteHost()); //$NON-NLS-1$
			connection.addEnv("REMOTE_HOST", req.getRemoteHost()); //$NON-NLS-1$
			connection.addEnv("REMOTE_PORT", Integer.toString(req.getRemotePort()));//$NON-NLS-1$

			connection.addEnv("REMOTE_USER", request.getRemoteUser()); //$NON-NLS-1$
			connection.addEnv("REQUEST_METHOD", request.getMethod()); //$NON-NLS-1$

			if (pageFile != null)
			{
				connection.addEnv("SCRIPT_NAME", scriptName); //$NON-NLS-1$
				connection.addEnv("SCRIPT_FILENAME", pageFile); //$NON-NLS-1$
			}
			else
			{
				connection.addEnv("SCRIPT_NAME", scriptName); //$NON-NLS-1$
				connection.addEnv("SCRIPT_FILENAME", scriptName); //$NON-NLS-1$
			}

			connection.addEnv("SERVER_NAME", req.getServerName()); //$NON-NLS-1$
			connection.addEnv("SERVER_PORT", Integer.toString(req.getServerPort())); //$NON-NLS-1$
			connection.addEnv("SERVER_PROTOCOL", req.getProtocol()); //$NON-NLS-1$

			if (documentRoot != null)
			{
				connection.addEnv("DOCUMENT_ROOT", documentRoot);
			}

			if (statusCode != -1)
			{
				connection.addEnv("STATUS_CODE", Integer.toString(statusCode));
			}

			connection.addEnv("HTTPS", "off");

			connection.flush();

			if (request.getContentLength() != 0 || request.getContentType() != null)
			{
				wrappedRequest = new RequestWrapper(request);
				chain.doFilter(wrappedRequest, wrappedResponse);
			}
			else
			{
				chain.doFilter(request, wrappedResponse);
			}

			wrappedResponse.finish();
			connection.sendEndRequest();
			connection.streamResponse(request, wrappedResponse, wrappedRequest == null ? null : wrappedRequest._file);

			_jaxer.returnConnection(connection);

			if (this.statsHandler != null)
			{
				this.statsHandler.eventOccurred(request, response, JAXER_PROCESSED);
			}
			bufferedWrapper.headers.clear();
			wrappedResponse.headers.clear();
			connection = null;
		}
		catch (Exception e)
		{
			if (logHandler != null)
			{
				logHandler.logInfo("Exception from Jaxer filter", e);
			}
			if (errorHandler != null)
			{
				try
				{
					errorHandler.showErrorPage(response, e);
				}
				catch (Exception e1)
				{
					if (logHandler != null)
					{
						logHandler.logInfo("Exception showing error page", e1);
					}
				}
			}
			else
			{
				if (e instanceof ServletException)
				{
					throw (ServletException) e;
				}
				else if (e instanceof IOException)
				{
					throw (IOException) e;
				}

			}
		}
		finally
		{
			try
			{
				if (wrappedRequest != null && wrappedRequest._file != null)
				{
					wrappedRequest._file.delete();
				}
				if (connection != null)
				{
					connection.destroy();
				}
			}
			catch (Exception e)
			{
			}
		}
	}

	/**
	 * Request wrapper class
	 */
	public static class RequestWrapper extends HttpServletRequestWrapper
	{
		File _file;
		InStream _in;
		BufferedReader _reader;

		/**
		 * Request wrapper constructor
		 * 
		 * @param request
		 */
		public RequestWrapper(HttpServletRequest request)
		{
			super(request);
		}

		/**
		 * @see javax.servlet.ServletRequestWrapper#getInputStream()
		 */
		public ServletInputStream getInputStream() throws IOException
		{
			if (_reader != null)
			{
				throw new IllegalStateException("getReader() called"); //$NON-NLS-1$
			}
			if (_in == null)
			{
				_file = File.createTempFile("jxr", ".dat"); //$NON-NLS-1$ //$NON-NLS-2$
				_file.deleteOnExit();
				_in = new InStream(_file, super.getInputStream());
			}
			return _in;
		}

		/**
		 * @see javax.servlet.ServletRequestWrapper#getReader()
		 */
		public BufferedReader getReader() throws IOException
		{
			if (_reader == null)
			{
				if (_in != null)
				{
					throw new IllegalStateException("getInputStream() called"); //$NON-NLS-1$
				}

				_file = File.createTempFile("jxr", ".dat"); //$NON-NLS-1$ //$NON-NLS-2$
				_file.deleteOnExit();
				_in = new InStream(_file, super.getInputStream());

				String encoding = getCharacterEncoding();
				_reader = new BufferedReader(encoding == null ? new InputStreamReader(_in) : new InputStreamReader(_in,
						encoding));
			}
			return _reader;
		}
	}

	/**
	 * InStream class
	 */
	public static class InStream extends ServletInputStream
	{
		boolean _closed = false;
		FileOutputStream _out;
		ServletInputStream _in;

		/**
		 * In stream constructor
		 * 
		 * @param file
		 * @param in
		 * @throws FileNotFoundException
		 */
		public InStream(File file, ServletInputStream in) throws FileNotFoundException
		{
			_out = new FileOutputStream(file);
			_in = in;
		}

		/**
		 * @see java.io.InputStream#read()
		 */
		public int read() throws IOException
		{
			if (_closed)
			{
				return -1;
			}
			int b = _in.read();
			if (b < 0)
			{
				_closed = true;
				_out.close();
			}
			else
			{
				_out.write(b);
			}
			return b;
		}

		/**
		 * @see java.io.InputStream#markSupported()
		 */
		public boolean markSupported()
		{
			return false;
		}

		/**
		 * @see javax.servlet.ServletInputStream#readLine(byte[], int, int)
		 */
		public int readLine(byte[] b, int off, int len) throws IOException
		{
			if (_closed)
			{
				return -1;
			}
			int l = _in.readLine(b, off, len);
			if (l < 0)
			{
				_closed = true;
				_out.close();
			}
			else
			{
				_out.write(b, off, l);
			}
			return l;
		}

		/**
		 * @see java.io.InputStream#read(byte[], int, int)
		 */
		public int read(byte[] b, int off, int len) throws IOException
		{
			if (_closed)
			{
				return -1;
			}
			int l = _in.read(b, off, len);
			if (l < 0)
			{
				_closed = true;
				_out.close();
			}
			else
			{
				_out.write(b, off, l);
			}
			return l;
		}

		/**
		 * @see java.io.InputStream#read(byte[])
		 */
		public int read(byte[] b) throws IOException
		{
			return read(b, 0, b.length);
		}

		/**
		 * @see java.io.InputStream#skip(long)
		 */
		public long skip(long n) throws IOException
		{
			if (_closed)
			{
				return -1;
			}
			long i = 0;
			for (i = 0; i < n; i++)
			{
				int b = _in.read();
				if (b < 0)
				{
					_closed = true;
					_out.close();
					return i == 0 ? -1 : i;
				}
				else
				{
					_out.write(b);
				}
			}
			return i;
		}

	}

	/**
	 * Buffered Response Wrapper class
	 */
	public static class BufferedResponseWrapper extends HttpServletResponseWrapper implements IStatusLengthOnlyResponse
	{

		ByteArrayOutputStream buffer;
		BufferedServletOutputStream servletStream;
		PrintWriter writer;
		int status;
		int length;
		int writerLength;
		Map<String, String> headers;

		/**
		 * Constructor
		 * 
		 * @param response
		 */
		public BufferedResponseWrapper(HttpServletResponse response)
		{
			super(response);
			length = 0;
			writerLength = 0;
			status = HttpServletResponse.SC_OK;
			headers = new HashMap<String, String>();
		}

		/**
		 * @see javax.servlet.ServletResponseWrapper#setContentLength(int)
		 */
		public void setContentLength(int length)
		{
			this.length = length;
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#addHeader(java.lang.String, java.lang.String)
		 */
		public void addHeader(String name, String value)
		{
			headers.put(name, value);
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#setHeader(java.lang.String, java.lang.String)
		 */
		public void setHeader(String name, String value)
		{
			headers.put(name, value);
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#setIntHeader(java.lang.String, int)
		 */
		public void setIntHeader(String name, int value)
		{
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#setDateHeader(java.lang.String, long)
		 */
		public void setDateHeader(String name, long value)
		{
		}

		/**
		 * @see javax.servlet.ServletResponseWrapper#getOutputStream()
		 */
		public ServletOutputStream getOutputStream() throws IOException
		{
			if (servletStream == null)
			{
				if (buffer == null)
				{
					buffer = new ByteArrayOutputStream();
				}
				servletStream = new BufferedServletOutputStream(buffer);
			}
			return servletStream;
		}

		/**
		 * @see javax.servlet.ServletResponseWrapper#getWriter()
		 */
		public PrintWriter getWriter() throws IOException
		{
			if (writer == null)
			{
				String encoding = getCharacterEncoding();
				ServletOutputStream stream = getOutputStream();
				if (encoding == null)
				{
					writer = new PrintWriter(stream, true);
				}
				else
				{
					writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(stream, encoding)), true)
					{

						public void write(int c)
						{
							writerLength++;
							super.write(c);
						}

						public void write(char[] buf, int off, int len)
						{
							writerLength += len;
							super.write(buf, off, len);
						}

					};
				}
			}
			return writer;
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#setStatus(int, java.lang.String)
		 */
		public void setStatus(int status, String arg1)
		{
			this.status = status;
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#setStatus(int)
		 */
		public void setStatus(int status)
		{
			this.status = status;
		}

		/**
		 * @see com.aptana.jaxer.connectors.servlet.interfaces.IStatusLengthOnlyResponse#getContentLength()
		 */
		public int getContentLength()
		{
			if (buffer != null)
			{
				return Math.max(Math.max(length, buffer.size()), writerLength);
			}
			else
			{
				return Math.max(length, writerLength);
			}
		}

		/**
		 * @see com.aptana.jaxer.connectors.servlet.interfaces.IStatusLengthOnlyResponse#getStatus()
		 */
		public int getStatus()
		{
			return this.status;
		}

	}

	public static class CallbackResponseWrapper extends ResponseWrapper implements ICallbackResponse
	{
		public CallbackResponseWrapper(Connection connection, HttpServletResponse response)
		{
			super(connection, response);
		}
	}
	
	/**
	 * Response Wrapper class
	 */
	public static class ResponseWrapper extends HttpServletResponseWrapper
	{
		Connection _connection;
		PrintWriter _writer;
		OutStream _out;
		Map<String, Set<String>> headers;
		boolean isFinished = false;

		/**
		 * Response wrapper constructor
		 * 
		 * @param connection
		 * @param response
		 */
		public ResponseWrapper(Connection connection, HttpServletResponse response)
		{
			super(response);
			_connection = connection;
			headers = new HashMap<String, Set<String>>();
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#addHeader(java.lang.String, java.lang.String)
		 */
		public void addHeader(String name, String value)
		{
			Set<String> values = null;
			if (headers.containsKey(name))
			{
				values = headers.get(name);
			}
			else
			{
				values = new HashSet<String>();
				headers.put(name, values);
			}
			if (values.add(value))
			{
				super.addHeader(name, value);
			}
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#addIntHeader(java.lang.String, int)
		 */
		public void addIntHeader(String name, int value)
		{
			Set<String> values = null;
			if (headers.containsKey(name))
			{
				values = headers.get(name);
			}
			else
			{
				values = new HashSet<String>();
				headers.put(name, values);
			}
			if (values.add(Integer.toString(value)))
			{
				super.addIntHeader(name, value);
			}
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#addDateHeader(java.lang.String, long)
		 */
		public void addDateHeader(String name, long value)
		{
			Set<String> values = null;
			if (headers.containsKey(name))
			{
				values = headers.get(name);
			}
			else
			{
				values = new HashSet<String>();
				headers.put(name, values);
			}
			if (values.add(Long.toString(value)))
			{
				super.addDateHeader(name, value);
			}
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#setHeader(java.lang.String, java.lang.String)
		 */
		public void setHeader(String name, String value)
		{
			super.setHeader(name, value);
			Set<String> values = null;
			if (headers.containsKey(name))
			{
				values = headers.get(name);
				values.clear();
			}
			else
			{
				values = new HashSet<String>();
				headers.put(name, values);
			}
			values.add(value);
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#setIntHeader(java.lang.String, int)
		 */
		public void setIntHeader(String name, int value)
		{
			super.setIntHeader(name, value);
			Set<String> values = null;
			if (headers.containsKey(name))
			{
				values = headers.get(name);
				values.clear();
			}
			else
			{
				values = new HashSet<String>();
				headers.put(name, values);
			}
			values.add(Integer.toString(value));
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#setDateHeader(java.lang.String, long)
		 */
		public void setDateHeader(String name, long value)
		{
			super.setDateHeader(name, value);
			Set<String> values = null;
			if (headers.containsKey(name))
			{
				values = headers.get(name);
				values.clear();
			}
			else
			{
				values = new HashSet<String>();
				headers.put(name, values);
			}
			values.add(Long.toString(value));
		}

		/**
		 * @see javax.servlet.ServletResponseWrapper#flushBuffer()
		 */
		public void flushBuffer() throws IOException
		{
			if (_writer != null)
			{
				_writer.flush();
			}
			_connection.flush();
		}

		/**
		 * @see javax.servlet.ServletResponseWrapper#reset()
		 */
		public void reset()
		{
			resetBuffer();
			super.reset();
		}

		/**
		 * @see javax.servlet.ServletResponseWrapper#resetBuffer()
		 */
		public void resetBuffer()
		{
			if (_connection.isCommitted())
			{
				throw new IllegalStateException();
			}
			if (_out != null)
			{
				_out._written = false;
			}
			_connection.resetBuffer();
			super.resetBuffer();
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#sendError(int, java.lang.String)
		 */
		public void sendError(int sc, String msg) throws IOException
		{
			resetBuffer();
			super.sendError(sc, msg);
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#sendError(int)
		 */
		public void sendError(int sc) throws IOException
		{
			resetBuffer();
			super.sendError(sc);
		}

		/**
		 * @see javax.servlet.http.HttpServletResponseWrapper#sendRedirect(java.lang.String)
		 */
		public void sendRedirect(String location) throws IOException
		{
			resetBuffer();
			super.sendRedirect(location);
		}

		/**
		 * @see javax.servlet.ServletResponseWrapper#getOutputStream()
		 */
		public ServletOutputStream getOutputStream() throws IOException
		{
			if (!isFinished)
			{
				if (_writer != null)
				{
					throw new IllegalStateException("getWriter() called"); //$NON-NLS-1$
				}
				if (_out == null)
				{
					_out = new OutStream(_connection);
				}
				return _out;
			}
			else
			{
				return super.getOutputStream();
			}
		}

		/**
		 * @see javax.servlet.ServletResponseWrapper#getWriter()
		 */
		public PrintWriter getWriter() throws IOException
		{
			if (!isFinished)
			{
				if (_writer == null)
				{
					if (_out != null)
					{
						throw new IllegalStateException("getOutputStream() called"); //$NON-NLS-1$
					}

					_out = new OutStream(_connection);

					String encoding = getCharacterEncoding();
					_writer = encoding == null ? new PrintWriter(_out) : new PrintWriter(new OutputStreamWriter(_out,
							encoding));
				}
				return _writer;
			}
			else
			{
				return super.getWriter();
			}
		}

		void finish() throws IOException
		{
			if (_writer != null)
			{
				_writer.close();
			}
			else if (_out != null)
			{
				_out.close();
			}
			isFinished = true;
		}
	}

	/**
	 * Buffered servlet output stream
	 */
	public static class BufferedServletOutputStream extends ServletOutputStream
	{

		OutputStream stream;

		/**
		 * Constructor
		 * 
		 * @param stream
		 */
		public BufferedServletOutputStream(OutputStream stream)
		{
			this.stream = stream;
		}

		/**
		 * @see java.io.OutputStream#write(int)
		 */
		public void write(int b) throws IOException
		{
			this.stream.write(b);
		}

	}

	/**
	 * Out Stream class
	 */
	public static class OutStream extends ServletOutputStream
	{
		Connection _connection;
		boolean _closed;
		boolean _written;

		/**
		 * Out stream constructor
		 * 
		 * @param connection
		 * @throws IOException
		 */
		public OutStream(Connection connection) throws IOException
		{
			_connection = connection;
			_written = false;
		}

		/**
		 * @see java.io.OutputStream#flush()
		 */
		public void flush() throws IOException
		{
			_connection.flush();
		}

		/**
		 * @see java.io.OutputStream#close()
		 */
		public void close() throws IOException
		{
			if (!_closed)
			{
				if (!_written)
				{
					_connection.addDocumentData(new byte[] {}, 0, 0);
				}
				_connection.flush();
				_closed = true;
			}
		}

		/**
		 * @see java.io.OutputStream#write(int)
		 */
		public void write(int b) throws IOException
		{
			if (!_closed)
			{
				_written = true;
				_connection.addDocumentData(new byte[] { (byte) b }, 0, 1);
			}
		}

		/**
		 * @see java.io.OutputStream#write(byte[])
		 */
		public void write(byte[] b) throws IOException
		{
			if (!_closed)
			{
				_written = true;
				_connection.addDocumentData(b, 0, b.length);
			}
		}

		/**
		 * @see java.io.OutputStream#write(byte[], int, int)
		 */
		public void write(byte[] b, int off, int len) throws IOException
		{
			if (!_closed)
			{
				_written = true;
				_connection.addDocumentData(b, off, len);
			}
		}
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IHostnameFilter#getHostname()
	 */
	public String getHostname()
	{
		return hostname;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IHostnameFilter#setHostname(java.lang.String)
	 */
	public void setHostname(String hostname)
	{
		this.hostname = hostname;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IDocumentRootFilter#getDocumentRootResolver()
	 */
	public IDocumentRootResolver getDocumentRootResolver()
	{
		return this.resolver;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IDocumentRootFilter#setDocumentRootResolver(com.aptana.jaxer.connectors.servlet.interfaces.IDocumentRootResolver)
	 */
	public void setDocumentRootResolver(IDocumentRootResolver resolver)
	{
		this.resolver = resolver;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IHostnameFilter#getPort()
	 */
	public int getPort()
	{
		return this.port;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IHostnameFilter#setPort(int)
	 */
	public void setPort(int port)
	{
		this.port = port;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IIgnoreFilter#getIgnoreHandler()
	 */
	public IIgnoreHandler getIgnoreHandler()
	{
		return this.handler;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IIgnoreFilter#setIgnoreHandler(com.aptana.jaxer.connectors.servlet.interfaces.IIgnoreHandler)
	 */
	public void setIgnoreHandler(IIgnoreHandler handler)
	{
		this.handler = handler;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IErrorPageFilter#getErrorPageHandler()
	 */
	public IErrorPageHandler getErrorPageHandler()
	{
		return this.errorHandler;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IErrorPageFilter#setErrorPageHandler(com.aptana.jaxer.connectors.servlet.interfaces.IErrorPageHandler)
	 */
	public void setErrorPageHandler(IErrorPageHandler handler)
	{
		this.errorHandler = handler;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.ILoggingFilter#getLoggingHandler()
	 */
	public ILoggingHandler getLoggingHandler()
	{
		return this.logHandler;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.ILoggingFilter#setLoggingHandler(com.aptana.jaxer.connectors.servlet.interfaces.ILoggingHandler)
	 */
	public void setLoggingHandler(ILoggingHandler handler)
	{
		this.logHandler = handler;
		if (_jaxer != null)
		{
			_jaxer.setLogger(this.logHandler);
		}
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsProvider#getStatisticsHandler()
	 */
	public IStatisticsHandler getStatisticsHandler()
	{
		return this.statsHandler;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsProvider#setStatisticsHandler(com.aptana.jaxer.connectors.servlet.interfaces.IStatisticsHandler)
	 */
	public void setStatisticsHandler(IStatisticsHandler handler)
	{
		this.statsHandler = handler;
	}

	/**
	 * @see com.aptana.jaxer.connectors.servlet.interfaces.IRestartableFilter#restart()
	 */
	public void restart()
	{
		if (_jaxer != null)
		{
			_jaxer.clearConnections();
		}
	}
}
