package com.aptana;

import java.io.*;
import java.util.*;
import com.gargoylesoftware.htmlunit.html.*;
import junit.framework.*;
//import net.sourceforge.groboutils.junit.v1.MultiThreadedTestRunner;
//import net.sourceforge.groboutils.junit.v1.TestRunnable;
import junit.framework.TestCase;



/**
 * @author Curt Tudor
 *
 * 
 */
public class JaxerHtmlTestUtil
{
	private static Properties getEnvVars() throws Throwable 
	{
		Process p = null;
		Properties envVars = new Properties();
		Runtime r = Runtime.getRuntime();
		String OS = System.getProperty("os.name").toLowerCase();
		if ( (OS.indexOf("nt") > -1)
			|| (OS.indexOf("windows 20") > -1 )  
			|| (OS.indexOf("windows xp") > -1) ) {
			p = r.exec( "cmd.exe /c set" );
		}
		else {
			// our last hope, we assume Unix
			p = r.exec( "env" );
		}
  		BufferedReader br = new BufferedReader( new InputStreamReader( p.getInputStream() ) );
		String line;
		while( (line = br.readLine()) != null ) 
		{
			int idx = line.indexOf( '=' );
			String key = line.substring( 0, idx ).toLowerCase();
			String value = line.substring( idx+1 );
			envVars.setProperty( key, value );
		}
		return envVars;
	}

	
	public static String getEnv(String name) throws Exception 
	{
		String retval = null;
		try {
			Properties p = getEnvVars();
			retval = p.getProperty(name.toLowerCase());
		}
		catch (Throwable e) {
			e.printStackTrace();
		}
		return retval;
	}
	
	public static org.apache.log4j.Logger getLogger() 
	{
		org.apache.log4j.BasicConfigurator.configure();
		org.apache.log4j.Logger logger = org.apache.log4j.Logger.getRootLogger();

		String HTMLUNIT_LOG_LEVEL = null;
		try {HTMLUNIT_LOG_LEVEL = getEnv("HTMLUNIT_LOG_LEVEL");	} catch (Exception x) {}
		
		if (null == HTMLUNIT_LOG_LEVEL)
		{
			logger.setLevel(org.apache.log4j.Level.FATAL);
		}
		else if (HTMLUNIT_LOG_LEVEL.toLowerCase() == "trace")
		{
			logger.setLevel(org.apache.log4j.Level.TRACE);
		}
		else if (HTMLUNIT_LOG_LEVEL.toLowerCase() == "debug")
		{
			logger.setLevel(org.apache.log4j.Level.DEBUG);
		}
		else if (HTMLUNIT_LOG_LEVEL.toLowerCase() == "info")
		{
			logger.setLevel(org.apache.log4j.Level.INFO);
		}
		else if (HTMLUNIT_LOG_LEVEL.toLowerCase() == "warn")
		{
			logger.setLevel(org.apache.log4j.Level.WARN);
		}
		else if (HTMLUNIT_LOG_LEVEL.toLowerCase() == "error")
		{
			logger.setLevel(org.apache.log4j.Level.ERROR);
		}

		return logger;
	}
	
	public static int getThreadCount()  throws Exception
	{
		// Default to 5 threads 
		int threadCount = 5;
		
		try {
			String s = getEnv("HTMLUNIT_THREAD_COUNT");
			if (null != s) {
				threadCount = Integer.parseInt(s);
			}
		}
      	catch (Exception e) {
			throw new Exception("env var HTMLUNIT_THREAD_COUNT is not a valid number: " + e.toString());
      	}

		return threadCount;
	}
	
	public static int getRequestCount()  throws Exception
	{
		// Default to 50 requests 
		int requestCount = 50;
		
		try {
			String s = getEnv("HTMLUNIT_REQUEST_COUNT");
			if (null != s) {
				requestCount = Integer.parseInt(s);
			}
		}
      	catch (Exception e) {
			throw new Exception("env var HTMLUNIT_REQUEST_COUNT is not a valid number: " + e.toString());
      	}

		return requestCount;
	}
	
	public static int getWebClientTimeout()  throws Exception
	{
		// Default to 10 sceonds 
		int timeout = 10*1000;
		
		try {
			String s = getEnv("HTMLUNIT_WEBCLIENT_TIMEOUT");
			if (null != s) {
				timeout = Integer.parseInt(s);
			}
		}
      	catch (Exception e) {
			throw new Exception("env var HTMLUNIT_WEBCLIENT_TIMEOUT is not a valid number: " + e.toString());
      	}

		return timeout;
	}
	
	public static boolean performCommonPageValidation(HtmlPage page)
	{
		HtmlElement element = null;

		// Make sure that no extraneous JavaScript syntax errs were rendered in the document
		element = null;
		try {
			element = page.getHtmlElementById("JaxerJSError");
		}
		catch (Exception e)	{}
	    	
		return (element != null);
	}
}
