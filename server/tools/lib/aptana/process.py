#!/usr/bin/env python
import logging
import os
import re
import subprocess
import sys
import time

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.boot
import aptana.conf
import aptana.script
import aptana.shell

__version__ = "$Revision: 3339 $"[11:-2]


class ProcessError(Exception):
    """Signaled when a process terminates abnormally."""


class Process(object):
    """Process command line builder."""

    EXIT_SUCCESS = 0
    EXIT_FAILURE = 1

    def __init__(self, cmd, quotecmdwhitespace=True):
        if (quotecmdwhitespace is True and cmd is not None and cmd.find(" ") > -1):
            cmd = "\"" + cmd + "\""
        self._name = cmd
        self._title = cmd
        self._args = []
        self._cwd = None
        self._hostname = None
        self._environment = {}
        self.setenvironment(os.environ) # copy global environment into a local dictionary
        self._exitvalue = Process.EXIT_SUCCESS
        self._childpid = -1
        self._output = []
        self._background = False
        self._ignored = [] # ignored process exitvalues for when you need to hack around a misbehaving process
        self._shellexec = True # use the shell to execute the process
        self._shellstdinredirect = None
        self._shellstdoutredirect = None
        self._shellstderrredirect = None

    def __str__(self):
        return self.command()

    #
    # attributes:
    #

    def getname(self):
        return self._name

    name = property(fget=getname, fset=None, fdel=None, doc=None)

    def gettitle(self):
        return self._title

    def settitle(self, title):
        self._title = title

    title = property(fget=gettitle, fset=settitle, fdel=None, doc=None)

    def getcwd(self):
        return self._cwd

    def setcwd(self, cwd):
        self._cwd = cwd

    cwd = property(fget=getcwd, fset=setcwd, fdel=None, doc=None)

    def gethostname(self):
        return self._hostname

    def sethostname(self, hostname):
        self._hostname = hostname

    hostname = property(fget=gethostname, fset=sethostname, fdel=None, doc=None)

    def getenvironment(self):
        return self._environment

    def setenvironment(self, environment):
        self._environment.clear()
        if (environment is not None):
            for key, value in environment.items():
                self._environment[key] = value

    environment = property(fget=getenvironment, fset=setenvironment, fdel=None, doc=None)

    def getargs(self):
        return self._args

    def clearargs(self):
        self._args = []

    args = property(fget=getargs, fset=None, fdel=None, doc=None)

    def arg(self, arg=None):
        if (arg is None or len(arg) == 0):
            return
        arg = arg.strip()
        self._args.append(arg)

    def quotearg(self, arg, quote='"'):
        arg = quote + arg.strip() + quote
        self._args.append(arg)

    def safearg(self, arg, quote='"'):
        arg = arg.strip()
        if (arg.find(" ") > -1):
            arg = quote + arg + quote
        self._args.append(arg)

    def getexitvalue(self):
        return self._exitvalue

    def setexitvalue(self, exitvalue):
        self._exitvalue = exitvalue

    exitvalue = property(fget=getexitvalue, fset=setexitvalue, fdel=None, doc=None)

    def ignoreexitvalue(self, value):
        self._ignored.append(int(value))

    def canignoreexitvalue(self, value):
        return (self._ignored.count(value) > 0)

    def wassuccessful(self):
        return (self._exitvalue == Process.EXIT_SUCCESS)

    def getoutput(self):
        return self._output

    output = property(fget=getoutput, fset=None, fdel=None, doc=None)

    def clearoutput(self):
        self._output = []

    def getbackground(self):
        return self._background

    def setbackground(self, background=False):
        self._background = background

    background = property(fget=getbackground, fset=setbackground, fdel=None, doc=None)

    def getchildpid(self):
        return self._childpid

    def setchildpid(self, childpid):
        self._childpid = childpid

    childpid = property(fget=getchildpid, fset=setchildpid, fdel=None, doc=None)

    #
    # Shell execution interface:
    #

    def getshellexec(self):
        return self._shellexec

    def setshellexec(self, shellexec):
        self._shellexec = shellexec

    shellexec = property(fget=getshellexec, fset=setshellexec, fdel=None, doc=None)

    def getshellstdinredirect(self):
        return self._shellstdinredirect

    def setshellstdinredirect(self, shellstdinredirect):
        self._shellstdinredirect = shellstdinredirect

    shellstdinredirect = property(fget=getshellstdinredirect, fset=setshellstdinredirect, fdel=None, doc=None)

    def getshellstdoutredirect(self):
        return self._shellstdoutredirect

    def setshellstdoutredirect(self, shellstdoutredirect):
        self._shellstdoutredirect = shellstdoutredirect

    shellstdoutredirect = property(fget=getshellstdoutredirect, fset=setshellstdoutredirect, fdel=None, doc=None)

    def getshellstderrredirect(self):
        return self._shellstderrredirect

    def setshellstderrredirect(self, shellstderrredirect):
        self._shellstderrredirect = shellstderrredirect

    shellstderrredirect = property(fget=getshellstderrredirect, fset=setshellstderrredirect, fdel=None, doc=None)

    def shelltitle(self, title=None):
        if (title is None):
            title = self._title
        if (aptana.conf.iswin()):
            proc = Process("title")
            proc.arg(title)
            proc.execute()

    #
    # Process execution interface:
    #

    def command(self):
        stdinshellstr = self.shellstdinredirect
        if (stdinshellstr is None):
            stdinshellstr = ""
        stdoutshellstr = self.shellstdoutredirect
        if (stdoutshellstr is None):
            stdoutshellstr = ""
        stderrshellstr = self.shellstderrredirect
        if (stderrshellstr is None):
            stderrshellstr = ""
        bgshellstr = ""
        if (self.background is True and self.shellexec is True and aptana.conf.isunix()):
            bgshellstr = "&"
        command = " ".join([stdinshellstr, self._name, " ".join(self._args), stdoutshellstr, stderrshellstr, bgshellstr])
        command = command.strip()
        return command

    def execute(self, executor=None):
        if (executor is None):
            if (self.hostname is None or aptana.conf.islocalhost(self.hostname)):
                executor = LocalhostProcessExecutor()
            else:
                executor = RshProcessExecutor()
        return executor.execute(self, self.cwd, self.environment, self.hostname)


class ProcessExecutor(object):
    """Process executor abstract interface."""

    def __init__(self):
        self._capture = True
        self._captureprefix = True
        self._pkillcommand = aptana.conf.ospkillcommand()
        self._instrumentation = aptana.boot.Instrumentation(logging.getLogger(self.__class__.__name__))
        self._shell = aptana.shell.Shell()
        self.setinstrumentation(self._instrumentation)

    def __str__(self):
        return "%s: capture=%d, captureprefix=%d, pkillcommand=%s, shell=%s, instrumentation=%s" % (self.__class__.__name__, self.capture, self.captureprefix, self.pkillcommand, self._shell, self.instrumentation)

    #
    # attributes:
    #

    def getinstrumentation(self):
        return self._instrumentation

    def setinstrumentation(self, instrumentation):
        self._instrumentation.changeinstrumentation(instrumentation)
        self._shell.instrumentation.changeinstrumentation(instrumentation)

    instrumentation = property(fget=getinstrumentation, fset=setinstrumentation, fdel=None, doc=None)

    def getcapture(self):
        return self._capture

    def setcapture(self, capture):
        self._capture = capture

    capture = property(fget=getcapture, fset=setcapture, fdel=None, doc=None)

    def getcaptureprefix(self):
        return self._captureprefix

    def setcaptureprefix(self, captureprefix):
        self._captureprefix = captureprefix

    captureprefix = property(fget=getcaptureprefix, fset=setcaptureprefix, fdel=None, doc=None)

    def getpkillcommand(self):
        return self._pkillcommand

    def setpkillcommand(self, pkillcommand):
        self._pkillcommand = pkillcommand

    pkillcommand = property(fget=getpkillcommand, fset=setpkillcommand, fdel=None, doc=None)

    #
    # ProcessExecutor interface:
    #

    def execute(self, command, cmdcwd=None, cmdenv=None, cmdhostname=None):
        pass

    def processof(self, command, cmdcwd=None, cmdenv=None, cmdhostname=None):
        # for backward compatibility convert strings or other objects into Process instances
        if (not isinstance(command, Process)):
            command = Process(str(command), quotecmdwhitespace=False)
            command.cwd = cmdcwd
            if (cmdenv is not None):
                command.environment = cmdenv
            command.hostname = cmdhostname
        return command

    def _cmdparts(self, command):
        """Returns a tuple of (executable name, prefix used in capture/filter mode, full command string with all arguments)."""
        cmdstr = command.command()
        cmdname = os.path.basename(command.name) # if the name is the whole command
        cmdprefix = ""
        if (self.captureprefix is True):
            cmdprefix = "[%s]:" % cmdname
        return (cmdname, cmdprefix, cmdstr)

    def _cmdchdir(self, cmdcwd):
        """Handles pre and post cwd for process execution.  Creates the cwd if it doesn't exist."""
        fromcwd = os.getcwd()
        tocwd = fromcwd
        if (cmdcwd is not None):
            # sometimes we are asked to cd before the directory exists, or if an error occurred
            if (not os.path.exists(cmdcwd)):
                self._shell.mkdirs(cmdcwd)
            self._shell.chdir(cmdcwd)
            tocwd = os.getcwd() # tocwd must be fully resolved not a path fragment
        return (fromcwd, tocwd)

    def convertexitvalue(self, exitvalue):
        if (aptana.conf.isunix()): # on unix the exit value is that of waitpid
            exitvalue = exitvalue >> 8 # high-byte == status
        return exitvalue

    def pkill(self, proctitle, failsafe=False):
        """Kills a process by name using an os specific command. i.e.: pkill, killall"""
        self.instrumentation.say("pkill(%s, %s)" % (proctitle, str(failsafe)))
        try:
            proc = Process(self._pkillcommand, quotecmdwhitespace=False)
            proc.arg(proctitle)
            proc.execute(self)
        except Exception:
            if (not failsafe):
                raise

    def kill(self, signal, pid, failsafe=False):
        """Kills a process by pid using the built-in kill facility."""
        self.instrumentation.say("kill(%d, %d, %s)" % (int(signal), int(pid), str(failsafe)))
        try:
            os.kill(int(pid), int(signal))
        except Exception:
            if (not failsafe):
                raise

    def killpidfile(self, signal, pidfilepath, deleteafterkill=True, failsafe=False):
        """Kills a process by id read from the specified file."""
        pids = self.readpidfile(pidfilepath)
        for pid in pids:
            self.kill(signal, pid, failsafe)
        if (deleteafterkill):
            self._shell.remove(pidfilepath)
        return len(pids)

    def readpidfile(self, pidfilepath):
        """Reads the process id from a file."""
        pids = []
        file = None
        try:
            file = open(pidfilepath, "r")
            for line in file.readlines():
                line = line.strip()
                pid = re.match(r"^[0-9]+", line)
                if (pid is not None):
                    pids.append(pid.group(0))
        finally:
            if (file is not None):
                file.close()
        return pids

    def writepidfile(self, pid, pidfilepath):
        """Writes the process id to a file."""
        return self._shell.write(pidfilepath, pid)


class LocalhostProcessExecutor(ProcessExecutor):
    """Executes commands on the localhost."""

    def __init__(self):
        super(LocalhostProcessExecutor, self).__init__()

    def execute(self, command, cmdcwd=None, cmdenv=None, cmdhostname="localhost"):
        """Dispatches a command to the proper execution method."""
        if (self.capture is True):
            return self.popen(command, cmdcwd, cmdenv, cmdhostname)
        else:
            return self.system(command, cmdcwd, cmdenv, cmdhostname)

    def system(self, command, cmdcwd=None, cmdenv=None, cmdhostname="localhost"):
        """Execute command using system, temporarily suspending the
           current process and returning the exit value of the subprocess."""
        command = self.processof(command, cmdcwd, cmdenv, cmdhostname)
        command.clearoutput()
        (cmdname, cmdprefix, cmdstr) = self._cmdparts(command)
        (fromcwd, tocwd) = self._cmdchdir(cmdcwd)

        self.instrumentation.say("cwd(%s) => system(%s)" % (tocwd, cmdstr))

        exitvalue = Process.EXIT_FAILURE
        try:
            exitvalue = os.system(cmdstr)
            exitvalue = self.convertexitvalue(exitvalue)
            #print "os.system: exitvalue=%d" % (exitvalue)
        finally:
            command.exitvalue = exitvalue
            if (cmdcwd is not None):
                self._shell.chdir(fromcwd)

        if (exitvalue != Process.EXIT_SUCCESS and not command.canignoreexitvalue(exitvalue)):
            raise ProcessError("System command: \"%s\" cwd: \"%s\" returned: [%d]" % (cmdstr, tocwd, exitvalue))

        return exitvalue

    def popen(self, command, cmdcwd=None, cmdenv=None, cmdhostname="localhost"):
        """Execute command using subprocess.Popen."""
        command = self.processof(command, cmdcwd, cmdenv, cmdhostname)
        command.clearoutput()
        (cmdname, cmdprefix, cmdstr) = self._cmdparts(command)
        (fromcwd, tocwd) = self._cmdchdir(cmdcwd)

        stdin     = None
        stdouterr = None
        exitvalue = Process.EXIT_FAILURE
        try:
            # if using shell use command as a string, otherwise as an array cmdstr.split()
            # the following Popen options are platform specific and should not be used:
            # win32: creationflags, startupinfo
            # posix: preexec_fn, close_fds
            #
            # using popen options:
            # shell=True so things like file globbing will still work
            # bufsize=0 means unbuffered io which is slower but better for output observed by beings
            # stderr=subprocess.STDOUT means concatenate stderr into stdour similar to bourne shell: 2>&1
            self.instrumentation.say("cwd(%s) => subprocess.Popen(%s)" % (tocwd, cmdstr))
            # if the process is spawned background/detached we don't want to pipe otherwise we'll block
            if (command.background):
                if (aptana.conf.iswin()): # shell must be False is creating a detached proces, unless the command is an array
                    command.shellexec = False
                proc = subprocess.Popen(cmdstr, executable=None, cwd=tocwd, env=cmdenv, shell=command.shellexec, bufsize=0, universal_newlines=False, stdin=None, stdout=None, stderr=None)
                command.childpid = proc.pid
                #self.instrumentation.say("parent pid(%d) => sub pid(%d)" % (os.getpid(), proc.pid))
                # we aren't waiting so the value of proc.returncode is None
                exitvalue = Process.EXIT_SUCCESS
            # piped mode so read and filter output streams
            else:
                # concat stderr to stdout
                proc = subprocess.Popen(cmdstr, executable=None, cwd=tocwd, env=cmdenv, shell=command.shellexec, bufsize=0, universal_newlines=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                command.childpid = proc.pid
                (stdin, stdouterr) = (proc.stdin, proc.stdout)
                # pipe subprocess stdout and stderr to the python stdout (connected to shell or possibly redirected)
                # accumulate output in a list
                # do not use the (for line in file) idiom because it invokes iterators that use read ahead
                while (True):
                    line = stdouterr.readline()
                    if (not line):
                        break
                    command.output.append(line)
                    print cmdprefix, line, # print without a newline as the line already contains one
                # Popen returns the correct process exit value not the value of waitpid, so need to shift off the high-byte
                exitvalue = proc.wait()
        finally:
            if (stdin is not None):
                stdin.close()
            if (stdouterr is not None):
                stdouterr.close()
            command.exitvalue = exitvalue
            #print "subprocess.Popen: exitvalue=%d" % (exitvalue)
            if (cmdcwd is not None):
                self._shell.chdir(fromcwd)

        if (exitvalue != Process.EXIT_SUCCESS and not command.canignoreexitvalue(exitvalue)):
            raise ProcessError("System command: \"%s\" cwd: \"%s\" returned: [%d]" % (cmdstr, tocwd, exitvalue))

        return exitvalue


class RshProcessExecutor(ProcessExecutor):
    """Executes commands on a remote host via rsh."""

    def __init__(self):
        super(RshProcessExecutor, self).__init__()
        self._username = None
        self._timeout = 0
        self._redirectinput = False

    #
    # attributes:
    #

    def getusername(self):
        return self._username

    def setusername(self, username):
        self._username = username

    username = property(fget=getusername, fset=setusername, fdel=None, doc=None)

    def gettimeout(self):
        return self._timeout

    def settimeout(self, timeout):
        self._timeout = timeout

    timeout = property(fget=gettimeout, fset=settimeout, fdel=None, doc=None)

    def getredirectinput(self):
        return self._redirectinput

    def setredirectinput(self, redirectinput):
        self._redirectinput = redirectinput

    redirectinput = property(fget=getredirectinput, fset=setredirectinput, fdel=None, doc=None)

    #
    # ProcessExecutor interface:
    #

    #REVIEW: chdir can be done with && or ; on the remote host
    def execute(self, command, cmdcwd=None, cmdenv=None, cmdhostname="localhost"):
        command = self.processof(command, cmdcwd, cmdenv, cmdhostname)
        command.clearoutput()

        rsh = "rsh"
        if (self._username):
            rsh += " -l " + self._username
        if (self._timeout):
            rsh += " -t " + self._timeout
        if (self._redirectinput):
            rsh += " -n"
        rsh += " " + cmdhostname
        rsh += " " + str(command)

        self.instrumentation.say("execute(" + rsh + ")")

        exitvalue = os.system(rsh)
        exitvalue = self.convertexitvalue(exitvalue)
        command.exitvalue = exitvalue

        if (exitvalue != Process.EXIT_SUCCESS and not command.canignoreexitvalue(exitvalue)):
            raise ProcessError("Remote command: \"%s\" cwd: \"%s\" returned: [%d]" % (rsh, cmdcwd, exitvalue))

        return exitvalue


class ProcessTestCase(aptana.script.Script):

    def __init__(self, configuration, cmd, methodname="testall"):
        super(ProcessTestCase, self).__init__(configuration, methodname)
        self._cmd = cmd

    def command(self):
        cmdstr = " ".join(self._cmd)
        print "====== Test command: %s" % (cmdstr)
        return cmdstr

    def testossystem(self):
        exitvalue = os.system(self.command()) # if cmd is a Process get the command from __str__()
        exitvalue = ProcessExecutor().convertexitvalue(exitvalue)
        print "os.system: exitvalue=" + str(exitvalue)

    def testospopen(self):
        (popen1stdout) = os.popen(self.command(), "r")
        self._readwrite(popen1stdout)

        popen1exitvalue = popen1stdout.close()
        print "os.popen: popen1exitvalue=" + str(popen1exitvalue)

    def testospopen2(self):
        (popen2stdin, popen2stdout) = os.popen2(self.command())
        self._readwrite(popen2stdout)

        popen2exitvalue = popen2stdin.close()
        print "os.popen2: popen2stdinexitvalue=" + str(popen2exitvalue)
        popen2exitvalue = popen2stdout.close()
        print "os.popen2: popen2stdoutexitvalue=" + str(popen2exitvalue)

    def testospopen3(self):
        (popen3stdin, popen3stdout, popen3stderr) = os.popen3(self.command())
        self._readwrite(popen3stdout)
        self._readwrite(popen3stderr)

        popen3exitvalue = popen3stdin.close()
        print "os.popen3: popen3stdinexitvalue=" + str(popen3exitvalue)
        popen3exitvalue = popen3stdout.close()
        print "os.popen3: popen3stdoutexitvalue=" + str(popen3exitvalue)
        popen3exitvalue = popen3stderr.close()
        print "os.popen3: popen3stderrexitvalue=" + str(popen3exitvalue)

    def testospopen4(self):
        (popen4stdin, popen4stdout) = os.popen4(self.command())
        self._readwrite(popen4stdout)

        popen4exitvalue = popen4stdin.close()
        print "os.popen4: popen4stdinexitvalue=" + str(popen4exitvalue)
        popen4exitvalue = popen4stdout.close()
        print "os.popen4: popen4stdoutexitvalue=" + str(popen4exitvalue)

    def testossubprocesscommunicate(self):
        proc = subprocess.Popen(self._cmd, stdin=None, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, preexec_fn=None, close_fds=True, shell=False, cwd=None, env=None, universal_newlines=False, startupinfo=None, creationflags=0)
        (stdout, stderr) = proc.communicate()
        # output is returned as a string
        print stdout
        print stderr
        print "subprocess.Popen: testossubprocesscommunicate=" + str(proc.returncode)

    def testsubprocesspopen(self):
        proc = subprocess.Popen(self._cmd, shell=False, bufsize=0, close_fds=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (stdin, stdouterr) = (proc.stdin, proc.stdout)
        self._readwrite(stdouterr)
        exitvalue = proc.wait()
        assert exitvalue == proc.returncode
        print "subprocess.Popen: testsubprocesspopen=" + str(exitvalue)

    def testsubprocesspopenshell(self):
        proc = subprocess.Popen(self.command(), shell=True, bufsize=0, close_fds=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (stdin, stdouterr) = (proc.stdin, proc.stdout)
        self._readwrite(stdouterr)
        exitvalue = proc.wait()
        assert exitvalue == proc.returncode
        print "subprocess.Popen: testsubprocesspopenshell=" + str(exitvalue)

    def testprocess(self):
        proc = Process(self.command(), quotecmdwhitespace=False)
        proc.environment["TEST_PROCESS_LOCAL_ENV"] = "TEST"
        proc.execute()
        print "output:", "".join(proc.output)
        assert not os.environ.has_key("TEST_PROCESS_LOCAL_ENV")
        assert proc.childpid > -1
        print "Process: testprocess=" + str(proc.exitvalue)

    def testall(self):
        self.testossystem()
        self.testospopen()
        self.testospopen2()
        self.testospopen3()
        self.testospopen4()
        self.testsubprocesscommunicate()
        self.testsubprocesspopen()
        self.testsubprocesspopenshell()
        self.testprocess()

    def _readwrite(self, handle):
        cmdname = self._cmd[0]
        cmdprefix = "[%s]:" % cmdname
        for line in handle.readlines():
            print cmdprefix, line, # no newline


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    conf = aptana.conf.createconfigurator()
    suite = aptana.script.ScriptSuite()
    cmdarray = ["python", "-c", "\"print 'python'\""]
    suite.addTest(ProcessTestCase(conf, cmdarray, "testossystem"))
    suite.addTest(ProcessTestCase(conf, cmdarray, "testospopen"))
    suite.addTest(ProcessTestCase(conf, cmdarray, "testospopen2"))
    suite.addTest(ProcessTestCase(conf, cmdarray, "testospopen3"))
    suite.addTest(ProcessTestCase(conf, cmdarray, "testospopen4"))
    suite.addTest(ProcessTestCase(conf, cmdarray, "testossubprocesscommunicate"))
    suite.addTest(ProcessTestCase(conf, cmdarray, "testsubprocesspopen"))
    suite.addTest(ProcessTestCase(conf, cmdarray, "testsubprocesspopenshell"))
    suite.addTest(ProcessTestCase(conf, cmdarray, "testprocess"))
    suite.main()

if (__name__ == "__main__"):
    main()
