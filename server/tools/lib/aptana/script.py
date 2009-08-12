#!/usr/bin/env python
import copy
import logging
import os
import re
import sys
import time
import traceback
import unittest

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.boot
import aptana.conf
import aptana.notification
import aptana.process
import aptana.scm
import aptana.shell

__version__ = "$Revision: 3616 $"[11:-2]


class ScriptError(Exception):
    """Signaled during a script runtime error."""


class Script(unittest.TestCase, aptana.boot.Instrumented):
    """Base script for build and testing frameworks."""

    def __init__(self, configuration, methodname="execute"):
        super(Script, self).__init__(methodname)
        self._suite = ScriptSuite()
        self._runner = ScriptRunner()
        self._conf = configuration
        self._shell = aptana.shell.Shell()
        self._executor = aptana.process.LocalhostProcessExecutor()
        self._scm = aptana.scm.SubversionClient()
        self._notifier = aptana.notification.SmtpNotifier()
        self._instrumentation = aptana.boot.Instrumentation(logging.getLogger(self.__class__.__name__))
        self.setinstrumentation(self._instrumentation)

    def __str__(self):
        return "%s:\nconf=%s\nshell=%s\nexecutor=%s\nscm=%s\nnotifier=%s\ninstrumentation=%s\nsuite=%s\nrunner=%s\n" % (self.__class__.__name__, self.conf, self.shell, self.executor, self.scm, self.notifier, self.instrumentation, self.suite, self.runner)

    def clone(self, script=None):
        """Clones self if script is None or propagates attributes from script to self."""
        clone = None
        # shallow copy of self
        if (script is None):
            clone = copy.copy(self)
        # copy attributes from specified script
        else:
            self.runner = script.runner
            self.conf = script.conf
            self.notifier = script.notifier
            self.instrumentation.changeinstrumentation(script.instrumentation)
            clone = self
        return clone

    #
    # attributes:
    #

    def getsuite(self):
        return self._suite

    def setsuite(self, suite):
        self._suite = suite

    suite = property(fget=getsuite, fset=setsuite, fdel=None, doc=None)

    def getrunner(self):
        return self._runner

    def setrunner(self, runner):
        self._runner = runner

    runner = property(fget=getrunner, fset=setrunner, fdel=None, doc=None)

    def getconf(self):
        return self._conf

    def setconf(self, conf):
        self._conf = conf

    conf = property(fget=getconf, fset=setconf, fdel=None, doc=None)

    def getshell(self):
        return self._shell

    def setshell(self, shell):
        self._shell = shell

    shell = property(fget=getshell, fset=setshell, fdel=None, doc=None)

    def getexecutor(self):
        return self._executor

    def setexecutor(self, executor):
        self._executor = executor

    executor = property(fget=getexecutor, fset=setexecutor, fdel=None, doc=None)

    def getscm(self):
        return self._scm

    def setscm(self, scm):
        self._scm = scm

    scm = property(fget=getscm, fset=setscm, fdel=None, doc=None)

    def getnotifier(self):
        return self._notifier

    def setnotifier(self, notifier):
        self._notifier = notifier

    notifier = property(fget=getnotifier, fset=setnotifier, fdel=None, doc=None)

    def getinstrumentation(self):
        return self._instrumentation

    def setinstrumentation(self, instrumentation):
        self.changeinstrumentation(instrumentation)

    instrumentation = property(fget=getinstrumentation, fset=setinstrumentation, fdel=None, doc=None)

    #
    # Instrumented interface:
    #

    def getlogger(self):
        return self._instrumentation.logger

    def setlogger(self, logger):
        self._instrumentation.logger = logger

    logger = property(fget=getlogger, fset=setlogger, fdel=None, doc=None)

    def changeinstrumentation(self, instrumentation):
        self._instrumentation = instrumentation
        self._conf.instrumentation = instrumentation
        self._shell.instrumentation = instrumentation
        self._executor.instrumentation = instrumentation
        self._scm.instrumentation = instrumentation
        self._notifier.instrumentation = instrumentation

    def flush(self):
        self._instrumentation.flush()

    def say(self, *args, **kwargs):
        self._instrumentation.say(*args, **kwargs)

    def record(self, severity, message):
        self._instrumentation.record(severity, message)

    #
    # Script interface:
    #

    def init(self):
        pass
    
    def execute(self):
        pass

    def destroy(self):
        pass

    def chain(self, script, debug=False):
        """Chain a test with the current one."""
        script.main(debug)

    def main(self, debug=False):
        """Easy way to start running a script using the default ScriptRunner."""
        if (self._suite is None):
            self._suite = ScriptSuite()
        self._suite.addTest(self)
        if (self._runner is None):
            self._runner = ScriptRunner()
        self._runner.run(self._suite, debug)

    #
    # unittest.TestCase interface:
    #

    def defaultTestResult(self):
        return ScriptResult()

    def setUp(self):
        """Hook method for setting up the test fixture before exercising it."""
        self.init()

    def tearDown(self):
        """Hook method for deconstructing the test fixture after testing it."""
        self.destroy()

    def debug(self):
        """Run the test without collecting errors in a TestResult.
           Propagates any exception thrown, calls tearDown internally if an
           exception is thrown from a test or fail* method."""
        # die if test method does not exist
        testMethod = getattr(self, self._testMethodName)
        # die on setUp failures
        self.setUp()
        # run test
        try:
            testMethod()
        except Exception:
            raise
        finally:
            try:
                self.tearDown()
            except Exception:
                # log but don't propagate tearDown exceptions to avoid masking
                traceback.print_exc(file=sys.stderr)


class ScriptResult(unittest._TextTestResult):
    """Script test results for accumulating errors and failures."""

    def __init__(self, stream, descriptions, verbosity):
        super(ScriptResult, self).__init__(stream, descriptions, verbosity)

    def clear(self):
        self.failures = []
        self.errors = []
        self.testsRun = 0
        self.shouldStop = 0


class ScriptSuite(unittest.TestSuite):
    """Script suite for aggregating test fixtures."""

    def __init__(self, tests=()):
        super(ScriptSuite, self).__init__(tests)
        self._runner = None

    def main(self, debug=False):
        """Easy way to start running a suite using the default ScriptRunner."""
        if (self._runner is None):
            self._runner = ScriptRunner()
        self._runner.run(self, debug)


class ScriptRunner(unittest.TextTestRunner):
    """Entry point for running scripts."""

    def __init__(self, stream=None, descriptions=1, verbosity=1):
        if (stream is None):
            stream = sys.stderr
        super(ScriptRunner, self).__init__(stream, descriptions, verbosity)

    def _makeResult(self):
        return ScriptResult(self.stream, self.descriptions, self.verbosity)

    def run(self, test, debug=False):
        """Run the given test case or test suite."""
        result = self._makeResult()
        startTime = time.time()

        if (debug):
            test.debug()
        else:
            test(result)

        stopTime = time.time()
        timeTaken = (stopTime - startTime)

        result.printErrors()
        self.stream.writeln(result.separator2)

        self.stream.writeln("Ran %d script%s in %.3fs" %
                            (result.testsRun, result.testsRun != 1 and "s" or "", timeTaken))
        self.stream.writeln()
        if (result.wasSuccessful()):
            self.stream.writeln("OK")
        else:
            self.stream.write("FAILED (")
            failed, errored = map(len, (result.failures, result.errors))
            if (failed):
                self.stream.write("failures=%d" % failed)
            if (errored):
                if (failed):
                    self.stream.write(", ")
                self.stream.write("errors=%d" % errored)
            self.stream.writeln(")")

        return result


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    script = Script()
    script.main()
    #unittest.main(__file__, "script", script.execute)
 
if (__name__ == "__main__"):
    main()
