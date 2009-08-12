#!/usr/bin/env python
import os
import random
import re
import sys
import time

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.conf
import aptana.process
import aptana.script
import aptana.shell
import aptana.jaxer.jxrservice

__version__ = "$Revision: 4629 $"[11:-2]


class JaxerTestScript(aptana.script.Script):
    """Jaxer server unit test fixture."""

    def init(self):
        """Initializes the testsuite."""
        self.timeout = 300
        self.url = "%s&rnd=%d&writeTo=%s" % (self.conf["test_url"], random.randint(0, 65535), self.conf["test_resultfilename"])
        self.iws_url = "%s&rnd=%d&writeTo=%s" % (self.conf["test_iws_url"], random.randint(0, 65535), self.conf["test_iws_resultfilename"])
        self.mysqlurl = "%s&rnd=%d&writeTo=%s" % (self.conf["test_url"],
            random.randint(0, 65535), self.conf["test_mysqlresultfilename"])
        self.iws_mysqlurl = "%s&rnd=%d&writeTo=%s" % (self.conf["test_iws_url"],
            random.randint(0, 65535), self.conf["test_iws_mysqlresultfilename"])

        self.resultsfilepath = self.conf["test_resultsfile"]
        self.iws_resultsfilepath = self.conf["test_iws_resultsfile"]
        self.mysqlresultsfilepath = self.conf["test_mysqlresultsfile"]
        self.iws_mysqlresultsfilepath = self.conf["test_iws_mysqlresultsfile"]

        # clean up any old test results
        self.logger.info("remove %s" % (self.resultsfilepath))
        self.shell.remove(self.resultsfilepath, ignoremissing=True)
        self.logger.info("remove %s" % (self.iws_resultsfilepath))
        self.shell.remove(self.iws_resultsfilepath, ignoremissing=True)
        self.logger.info("remove %s" % (self.mysqlresultsfilepath))
        self.shell.remove(self.mysqlresultsfilepath, ignoremissing=True)
        self.logger.info("remove %s" % (self.iws_mysqlresultsfilepath))
        self.shell.remove(self.iws_mysqlresultsfilepath, ignoremissing=True)
        # create browser service
        self._browser = aptana.jaxer.jxrservice.BrowserService(self.conf)
        self._browser.clone(self)
        self._browser.init()
        # create servers service
        self._servers = aptana.jaxer.jxrservice.ServerController(self.conf)
        self._servers.clone(self)
        self._servers.init()

    def destroy(self):
        """Destroys the testsuite and kills the browser process."""
        self._browser.stop()
        self._servers.stop()

    #
    # attributes:
    #

    def gettimeout(self):
        return self._timeout

    def settimeout(self, timeoutsec=300):
        self._timeout = timeoutsec

    timeout = property(fget=gettimeout, fset=settimeout, fdel=None, doc=None)

    def geturl(self):
        return self._url

    def seturl(self, url):
        self._url = url

    url = property(fget=geturl, fset=seturl, fdel=None, doc=None)

    def getresultsfilepath(self):
        return self._resultsfilepath

    def setresultsfilepath(self, resultsfilepath):
        self._resultsfilepath = resultsfilepath

    resultsfilepath = property(fget=getresultsfilepath, fset=setresultsfilepath, fdel=None, doc=None)

    def getiwsresultsfilepath(self):
        return self._iws_resultsfilepath

    def setiwsresultsfilepath(self, resultsfilepath):
        self._iws_resultsfilepath = resultsfilepath

    iwsresultsfilepath = property(fget=getiwsresultsfilepath, fset=setiwsresultsfilepath, fdel=None, doc=None)

    def getmysqlresultsfilepath(self):
        return self._mysqlresultsfilepath

    def setmysqlresultsfilepath(self, mysqlresultsfilepath):
        self._mysqlresultsfilepath = mysqlresultsfilepath

    mysqlresultsfilepath = property(fget=getmysqlresultsfilepath, fset=setmysqlresultsfilepath, fdel=None, doc=None)

    def getiwsmysqlresultsfilepath(self):
        return self._iwsmysqlresultsfilepath

    def setiwsmysqlresultsfilepath(self, mysqlresultsfilepath):
        self._iwsmysqlresultsfilepath = mysqlresultsfilepath

    iwsmysqlresultsfilepath = property(fget=getiwsmysqlresultsfilepath, fset=setiwsmysqlresultsfilepath, fdel=None, doc=None)

    #
    # test cases:
    #

    def testserver(self, resultfile):
        """Runs unit tests, assumes the jaxer distribution is available and that
           jaxer servers are running."""
        self.logger.info("===== Running Jaxer server unit tests - saving results to: " + resultfile)

        # start browser
        self.logger.info("unit test: url= %s" % (self.url))
        self._browser.url = self.url
        self._browser.restart()

        # wait for unit tests to complete or timeout
        self.logger.info("unit test: waitfor %s" % (resultfile))
        self.shell.waitfor(resultfile, self.timeout)
        if (not os.path.exists(resultfile)):
            self.fail("Unit tests did not save results to: " + resultfile)
        results = self.shell.read(resultfile)

        # determine unit test result and summarize failures
        matches = re.findall('class="FAILURE">([^<]+)</li>', results)
        if (len(matches) > 0):
            self.fail("Unit tests failed:\n" + "\n".join(matches))

    def autotestserver(self):
        """Automatically starts server processes and runs unit tests."""
        #jaxer = aptana.jaxer.jxrservice.JaxerService(self.conf)
        #jaxer.clone(self)
        #jaxer.init()

        #apache = aptana.jaxer.jxrservice.ApacheService(self.conf)
        #apache.clone(self)
        #apache.init()

        self.logger.info("auto test: start")
        self._servers.restart()
        #jaxer.restart()
        #apache.restart()

        # run tests
        self.logger.info("auto test: waiting for httpd")
        if (self.shell.waitfor(self.conf["httpd_pid"])):
            time.sleep(10) # even after the pid becomes available a 500 can still be thrown
            self.logger.info("auto test1: %s" % (self.resultsfilepath))
            self.testserver(self.resultsfilepath)
            #iws test
            self.url = self.iws_url
            self.logger.info("auto test2: url=%s %s" % (self.url, self.iws_resultsfilepath))
            self.testserver(self.iws_resultsfilepath)
        else:
            self.fail("Timeout waiting for apache to start.")

        self.logger.info("auto test: stop server")
        self._servers.stop()
        
        # run tests using mysql DB.
        # We assume mysqld is already running, and are all setup based on the configs.
        if (self.conf["test_mysql_db"] == "True"):
            self.logger.info("===== Running Jaxer server unit tests using mysql")
            time.sleep(10) # make sure jaxers are stopped before continue
            # update config to use mysql
            for name in os.listdir(self.conf["test_mysqlconfigsdir"]):
                srcname = os.path.join(self.conf["test_mysqlconfigsdir"], name);
                destname = os.path.join(self.conf["jam_install"], "local_jaxer", "conf", name)
                self.shell.copy(srcname, destname)

            # update url
            self.url = self.mysqlurl
            
            self._servers.restart()
            if (self.shell.waitfor(self.conf["httpd_pid"])):
                time.sleep(10) # even after the pid becomes available a 500 can still be thrown
                self.testserver(self.mysqlresultsfilepath)

                #iws test
                self.url = self.iws_mysqlurl
                self.testserver(self.iws_mysqlresultsfilepath)

            else:
                self.fail("Timeout waiting for apache to start using mysql.")

            self._servers.stop()
        
        #jaxer.stop()
        #apache.stop()


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    js = JaxerTestScript()

if (__name__ == "__main__"):
    main()
