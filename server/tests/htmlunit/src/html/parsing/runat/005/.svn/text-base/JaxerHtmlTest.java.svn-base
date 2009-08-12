import java.util.List;
import java.io.IOException;
import java.net.URL;
import org.w3c.dom.Document;
import com.gargoylesoftware.htmlunit.WebClient;
import com.gargoylesoftware.htmlunit.html.*;
import net.sourceforge.groboutils.junit.v1.MultiThreadedTestRunner;
import net.sourceforge.groboutils.junit.v1.TestRunnable;
import junit.framework.TestCase;
import com.aptana.*;




/**
 * @author Curt Tudor
 *
 * 
 */
public class JaxerHtmlTest extends TestCase 
{
	// *************************************************************************
	private	String 	HTMLUNIT_TEST_NAME = "html/parsing/runat/005";	// <-- used in URL; specific to THIS test
	// *************************************************************************

	private	String 	HTMLUNIT_SERVER_NAME 	= null;		// <-- set from env var
	private	String 	HTMLUNIT_SERVER_PORT 	= null;		// <-- set from env var
    private URL 	url = null;
	private org.apache.log4j.Logger  logger = null;
	
	protected void setUp() throws Exception 
	{
		org.apache.log4j.BasicConfigurator.configure();
		logger = JaxerHtmlTestUtil.getLogger();

		HTMLUNIT_SERVER_NAME = JaxerHtmlTestUtil.getEnv("HTMLUNIT_SERVER_NAME");
		if (null == HTMLUNIT_SERVER_NAME)
		{
			throw new Exception("env var HTMLUNIT_SERVER_NAME is not set");
		}
		
		HTMLUNIT_SERVER_PORT = JaxerHtmlTestUtil.getEnv("HTMLUNIT_SERVER_PORT");
		if (null == HTMLUNIT_SERVER_PORT)
		{
			throw new Exception("env var HTMLUNIT_SERVER_PORT is not set");
		}

    	String sURL = new String("http://" + HTMLUNIT_SERVER_NAME + ":" + HTMLUNIT_SERVER_PORT + "/htmlunit/" + HTMLUNIT_TEST_NAME + "/test.html");
		logger.info("URL in use is: " + sURL);
    	url = new URL(sURL);
	}



    private class WorkerThread extends TestRunnable 
    {
        private String name;

        private WorkerThread(String name) {

            this.name = name;
        }


		// Cause thread to sleep for a random amount of time that is somewhere
		// between 0.0 and 0.5 seconds
        private void doRandomSleep() throws Throwable
        {
            long l;
            l = Math.round(Math.random() * 50);
            Thread.sleep(l * 10);
        }

		
		// **************************************************
		// This is where the real test takes place.
		// **************************************************
        public void runTest() throws Throwable 
        {
		    final WebClient webClient = new WebClient();
			webClient.setTimeout(JaxerHtmlTestUtil.getWebClientTimeout());
			HtmlPage page;

			for (int i=0; i < JaxerHtmlTestUtil.getRequestCount(); i++)
			{
				doRandomSleep();
			    page = (HtmlPage)webClient.getPage(url);

				// Make sure the <script> with runat=both is present in the document
				HtmlElement elementS2 = page.getHtmlElementById("s");
			    assertNotNull( elementS2);

			    assertFalse( JaxerHtmlTestUtil.performCommonPageValidation(page) );
			}
        }
    }


	// This is where JUnit calls into our test.  We will use the MultiThreadedTestRunner here.
	// The MTTR takes an array of TestRunnable objects as parameters in its constructor.
	// We run the MTTR and it manages all the worker threads under the covers.
    public void testHTMLTest() throws Throwable 
    {
		// HOw many worker threads should we launch?
		int runnerCount = JaxerHtmlTestUtil.getThreadCount();

        // Instantiate the worker threads
		TestRunnable tcs[] = new TestRunnable[ runnerCount ];

      	for (int i = 0; i < runnerCount; ++i)
      	{
        	tcs[i] = new WorkerThread(Integer.toString(i));
      	}

        // Pass the worker array to the MTTR
        MultiThreadedTestRunner mttr = new MultiThreadedTestRunner(tcs);

        // Kickstart the MTTR & fire off the worker threads
        mttr.runTestRunnables();
    }
}
