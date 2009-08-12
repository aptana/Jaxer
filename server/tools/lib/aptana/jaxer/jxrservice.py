#!/usr/bin/env python
import os
import sys
import time

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.conf
import aptana.process
import aptana.service
import aptana.shell

__version__ = "$Revision: 4132 $"[11:-2]


class ApacheService(aptana.service.Service):
    """Control apache lifecycle via apachectl."""

    def __init__(self, configuration, methodname="status", *args):
        super(ApacheService, self).__init__(configuration, methodname, args)

    def init(self):
        self.jaxerbase = self.conf["jam_install"]
        self.name = os.path.join(self.jaxerbase, "Apache22", "bin", "apachectl")
        self.shell.mkdirs(os.path.join(self.jaxerbase, "Apache22", "logs"), ignoreexisting=True)

    def getjaxerbase(self):
        return self._jaxerbase

    def setjaxerbase(self, jaxerbase):
        self._jaxerbase = jaxerbase

    jaxerbase = property(fget=getjaxerbase, fset=setjaxerbase, fdel=None, doc=None)

    def configure(self):
        self.logger.info(str(self) + ":configure()")
        apachectl = aptana.process.Process(self.name)
        apachectl.cwd = self.jaxerbase
        apachectl.arg("configtest")
        apachectl.execute(self.executor)

    def start(self):
        self.logger.info(str(self) + ":start()")
        apachectl = aptana.process.Process(self.name)
        apachectl.cwd = self.jaxerbase
        apachectl.arg("start")
        apachectl.background = True
        apachectl.execute(self.executor)

        if (self.delay is not None and self.delay > 0):
            time.sleep(self.delay)

    def stop(self,restart=False):
        self.logger.info(str(self) + ":stop()")
        try:
            apachectl = aptana.process.Process(self.name)
            apachectl.cwd = self.jaxerbase
            apachectl.arg("stop")
            apachectl.execute(self.executor)
            self.executor.pkill(self.conf["apache_name"], failsafe=True)
        except Exception:
            pass

    def status(self):
        self.logger.info(str(self) + ":status()")
        apachectl = aptana.process.Process(self.name)
        apachectl.cwd = self.jaxerbase
        apachectl.arg("status")
        apachectl.execute(self.executor)


class JaxerService(aptana.service.Service):
    """Control jaxer lifecycle."""

    def __init__(self, configuration, methodname="status", *args):
        super(JaxerService, self).__init__(configuration, methodname, args)

    def init(self):
        self.name = "jaxer"
        self.jaxerbase = self.conf["jam_install"]
        self.pidfilepath = os.path.join(self.jaxerbase, "jaxer", "jaxer.pid")
        #REVIEW: executable names should not be variants!
        if (aptana.conf.iswin()):
            self.jaxer              = "Jaxer.exe"
            self.jaxermanager       = "JaxerManager.exe"
            self.telljaxermanager   = "tellJaxerManager.exe"
        elif (aptana.conf.ismac()):
            self.jaxer              = "Jaxer"
            self.jaxermanager       = "JaxerManager"
            self.telljaxermanager   = "tellJaxerManager"
        else: # linux, solaris, or generic unix
            self.jaxer              = "jaxer"
            self.jaxermanager       = "jaxermanager"
            self.telljaxermanager   = "telljaxermanager"

    def getjaxerbase(self):
        return self._jaxerbase

    def setjaxerbase(self, jaxerbase):
        self._jaxerbase = jaxerbase

    jaxerbase = property(fget=getjaxerbase, fset=setjaxerbase, fdel=None, doc=None)

    def start(self):
        self.logger.info(str(self) + ":start()")

        # setup jaxer environment for the jaxermanager subprocess to inherit
        dyldlibrarypath = os.path.abspath(os.path.join(self.jaxerbase, "jaxer"))
        ldlibrarypath = os.path.abspath(os.path.join(self.jaxerbase, "jaxer"))
        jaxerbaseabs = os.path.abspath(self.jaxerbase)
        jaxertemp = os.path.abspath(os.path.join(self.jaxerbase, "tmp"))
        # Specify the log file location URI
        jaxerlogoutput = os.path.abspath(os.path.join(self.jaxerbase, "logs", "jaxer.log"))
        # Increase maximum memory allocated to jaxer
        jaxermaxmemory = "180"

        # setup jaxer directory structure
        self.shell.mkdirs(os.path.abspath(os.path.join(self.jaxerbase, "data")))
        self.shell.mkdirs(os.path.abspath(os.path.join(self.jaxerbase, "public")))
        self.shell.mkdirs(os.path.abspath(os.path.join(self.jaxerbase, "logs")))
        self.shell.mkdirs(os.path.abspath(os.path.join(self.jaxerbase, "local_jaxer")))
        self.shell.mkdirs(os.path.abspath(os.path.join(self.jaxerbase, "local_jaxer", "conf")))
        self.shell.mkdirs(os.path.abspath(os.path.join(self.jaxerbase, "local_jaxer", "extensions")))

        # copy config and resource files
        defaultfavicon  = os.path.abspath(os.path.join(self.jaxerbase, "jaxer", "default_public", "favicon.ico"))
        publicfavicon   = os.path.abspath(os.path.join(self.jaxerbase, "public", "favicon.ico"))
        if (not os.path.exists(publicfavicon) and os.path.exists(defaultfavicon)):
            self.shell.copy(defaultfavicon, publicfavicon)

        defaultredirect = os.path.abspath(os.path.join(self.jaxerbase, "jaxer", "default_public", "index.html.redirect"))
        publicredirect  = os.path.abspath(os.path.join(self.jaxerbase, "public", "index.html.redirect"))
        publicindex     = os.path.abspath(os.path.join(self.jaxerbase, "public", "index.html"))
        if (not os.path.exists(publicindex) and os.path.exists(defaultredirect)):
            self.shell.copy(defaultredirect, publicredirect)

        if (os.path.exists(os.path.abspath(os.path.join(self.jaxerbase, "tmp")))):
            self.shell.mkdirs(os.path.abspath(os.path.join(self.jaxerbase, "tmp")))

        defaultconfigjs = os.path.abspath(os.path.join(self.jaxerbase, "jaxer", "default_local_jaxer", "conf", "config.js"))
        configjs        = os.path.abspath(os.path.join(self.jaxerbase, "local_jaxer", "conf", "config.js"))
        if (not os.path.exists(configjs) and os.path.exists(defaultconfigjs)):
            self.shell.copy(defaultconfigjs, configjs)

        defaultconfiglogjs  = os.path.abspath(os.path.join(self.jaxerbase, "jaxer", "default_local_jaxer", "conf", "configLog.js"))
        configlogjs         = os.path.abspath(os.path.join(self.jaxerbase, "local_jaxer", "conf", "configLog.js"))
        if (not os.path.exists(configlogjs) and os.path.exists(defaultconfiglogjs)):
            self.shell.copy(defaultconfiglogjs, configlogjs)

        defaultconfigappsjs = os.path.abspath(os.path.join(self.jaxerbase, "jaxer", "default_local_jaxer", "conf", "configApps.js"))
        configappsjs        = os.path.abspath(os.path.join(self.jaxerbase, "local_jaxer", "conf", "configApps.js"))
        if (not os.path.exists(configappsjs) and os.path.exists(defaultconfigappsjs)):
            self.shell.copy(defaultconfigappsjs, configappsjs)

        defaultmgrcfg = os.path.abspath(os.path.join(self.jaxerbase, "jaxer", "default_local_jaxer", "conf", "JaxerManager.cfg"))
        mgrcfg        = os.path.abspath(os.path.join(self.jaxerbase, "local_jaxer", "conf", "JaxerManager.cfg"))
        if (not os.path.exists(mgrcfg) and os.path.exists(defaultmgrcfg)):
            self.shell.copy(defaultmgrcfg, mgrcfg)

        # Register first before starting any Jaxers
        jaxer = aptana.process.Process(os.path.join(".", self.jaxer))
        jaxer.cwd = os.path.join(self.jaxerbase, "jaxer")
        if (aptana.conf.ismac()):
            jaxer.environment["DYLD_LIBRARY_PATH"] = dyldlibrarypath
        jaxer.environment["LD_LIBRARY_PATH"] = ldlibrarypath
        jaxer.environment["JAXER_BASE"] = jaxerbaseabs
        jaxer.environment["JAXER_MAXMEMORY"] = jaxermaxmemory
        jaxer.arg("-reg")
        jaxer.shellstdoutredirect = "1> " + os.path.devnull
        jaxer.shellstderrredirect = "2>&1"
        try:
            jaxer.execute(self.executor)
        except aptana.process.ProcessError, pe:
            self.logger.error("Jaxer registration failed.  Please check all dependent libraries exist.")
            self.logger.error("For example: ldd jaxer")
            raise pe

        # Start JaxerManager with optional second parameter
        mgr = aptana.process.Process(os.path.join(".", self.jaxermanager))
        mgr.cwd = os.path.join(self.jaxerbase, "jaxer")
        if (aptana.conf.ismac()):
            mgr.environment["DYLD_LIBRARY_PATH"] = dyldlibrarypath
        mgr.environment["LD_LIBRARY_PATH"] = ldlibrarypath
        mgr.environment["JAXER_BASE"] = jaxerbaseabs
        mgr.environment["JAXER_MAXMEMORY"] = jaxermaxmemory
        mgr.arg("--configfile=\"" + mgrcfg + "\"")
        mgr.arg("--pid-file=\"" + self.pidfilepath + "\"")
        mgr.arg("--cfg:tempdir=\"" + jaxertemp + "\"")
        mgr.arg("--log:output=\"" + jaxerlogoutput + "\"")
        mgr.arg("--minprocesses=3")
        mgr.arg("--requesttimeout=60")
        for arg in self.args:
            mgr.arg(arg)
        mgr.background = True
        mgr.execute(self.executor)

        #if (self.delay is not None and self.delay > 0):
        #    time.sleep(self.delay)
        time.sleep(10)

    def stop(self,restart=False):
        self.logger.info(str(self) + ":stop()")

        # tell it to die
        tellmgr = aptana.process.Process(os.path.join(".", self.telljaxermanager))
        tellmgr.cwd = os.path.join(self.jaxerbase, "jaxer")
        tellmgr.arg("exit")
        tellmgr.ignoreexitvalue(-1)  # tellmanager always returns -1 on win32 even on success
        tellmgr.ignoreexitvalue(255) # tellmanager always returns 255 even on success
        tellmgr.execute(self.executor)

        time.sleep(10)

        # kill by pid
        if (os.path.exists(self.pidfilepath)):
            self.executor.killpidfile(15, self.pidfilepath, deleteafterkill=True, failsafe=True)

        # kill by name
        self.executor.pkill(self.conf["jaxermanager_name"], failsafe=True)

    def status(self):
        if (os.path.exists(self.pidfilepath)):
            pid = self.executor.readpidfile(self.pidfilepath)
            self.logger.info(str(self) + ":status(): running pid: " + str(pid))
        else:
            self.logger.info(str(self) + ":status(): not running")


class ServerController(aptana.service.Service):
    """Wrapper around StartServers.bat and start.sh scripts until control can be passed to python for everything."""

    def __init__(self, configuration, methodname="status", *args):
        super(ServerController, self).__init__(configuration, methodname, args)

    def init(self):
        self.jaxerbase = self.conf["jam_install"]
        self.name = "servers"

    def start(self):
        self.logger.info("start(%s)" % (self.conf["start_servers"]))
        start = aptana.process.Process(self.conf["start_servers"])
        start.cwd = self.jaxerbase
        start.arg("start")
        start.background = True
        start.execute(self.executor)

    def stop(self,restart=False):
        self.logger.info("stop(%s)" % (self.conf["stop_servers"]))
        stop = aptana.process.Process(self.conf["stop_servers"])
        stop.cwd = self.jaxerbase
        if (aptana.conf.iswin()):
            stop.arg("stop")
        # We should be able to restart even if the server isn't running
        if (restart):
            stop.ignoreexitvalue(1)
        stop.execute(self.executor)
        # kill jaxermanager by name
        self.executor.pkill(self.conf["jaxermanager_name"], failsafe=True)
        # kill apache by name
        self.executor.pkill(self.conf["apache_name"], failsafe=True)


class BrowserService(aptana.service.Service):
    """Control a browser."""

    def __init__(self, configuration, methodname="status", *args):
        super(BrowserService, self).__init__(configuration, methodname, args)
        self._name = "firefox"
        self._title = "firefox-bin"
        self._url = "http://localhost:8081"

    def init(self):
        self.name = self.conf["browser"]
        self.title = self.conf["browser_name"]

    def geturl(self):
        return self._url

    def seturl(self, url):
        self._url = url

    url = property(fget=geturl, fset=seturl, fdel=None, doc=None)

    def start(self):
        self.logger.info(str(self) + ":start()")
        proc = aptana.process.Process(self.name)
        proc.quotearg(self.url)
        proc.background = True
        proc.execute(self.executor)

    def stop(self,restart=False):
        self.logger.info(str(self) + ":stop()")
        self.executor.pkill(self.title, failsafe=True)


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    service = aptana.service.Service("httpd")
    print service
 
if (__name__ == "__main__"):
    main()
