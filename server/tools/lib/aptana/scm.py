#!/usr/bin/env python
import logging
import os
import re
import sys
import traceback

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.boot
import aptana.process

__version__ = "$Revision: 3406 $"[11:-2]


def scmwalk(root=os.curdir, topdown=True, onerror=None):
    """Wrapper for os.walk.  Rather than writing the .svn removal in multiple places after an os.walk, call this function."""
    for path, dirs, files in os.walk(os.path.abspath(root)):
        if '.svn' in dirs:
            dirs.remove('.svn')
        yield path, dirs, files


class ScmError(Exception):
    """Signaled after a failed call to an SCM system."""


class ScmClient(object):
    """Abstract interface to an SCM system."""

    def __init__(self, name, **options):
        super(ScmClient, self).__init__()
        if (name is None or len(name) == 0):
            raise ValueError("The base command name cannot be null or an empty string.")
        self._name = name
        self._username = None
        self._password = None
        self._instrumentation = aptana.boot.Instrumentation(logging.getLogger(self.__class__.__name__))
        self._executor = aptana.process.LocalhostProcessExecutor()
        self.setinstrumentation(self._instrumentation)

    def __str__(self):
        return "%s: name=%s, username=%s, password=%s, instrumentation=%s" % (self.__class__.__name__, self.name, str(self.username), str(self.password), str(self.instrumentation))

    #
    # attributes:
    #

    def getinstrumentation(self):
        return self._instrumentation

    def setinstrumentation(self, instrumentation):
        self._instrumentation.changeinstrumentation(instrumentation)
        self._executor.instrumentation.changeinstrumentation(instrumentation)

    instrumentation = property(fget=getinstrumentation, fset=setinstrumentation, fdel=None, doc=None)

    def getname(self):
        return self._name

    name = property(fget=getname, fset=None, fdel=None, doc=None)

    def getusername(self):
        return self._username

    def setusername(self, username):
        self._username = username

    username = property(fget=getusername, fset=setusername, fdel=None, doc=None)

    def getpassword(self):
        return self._password

    def setpassword(self, password):
        self._password = password #REVIEW: probably need shell meta escapes here

    password = property(fget=getpassword, fset=setpassword, fdel=None, doc=None)

    #
    # ScmClient interface:
    #

    def prepareoptions(self, **options):
        return ""

    def execute(self, subcommand="", *args, **options):
        proc = aptana.process.Process(self.name)
        proc.arg(subcommand)
        proc.arg(self.prepareoptions(**options))
        for arg in args:
            proc.safearg(arg)
        proc.execute(self._executor)
        outputstr = "".join(proc.output)
        return outputstr


class SubversionClient(ScmClient):
    """Subversion client."""

    # revision range constants
    RANGESEP    = ":"
    HEAD        = "HEAD"
    BASE        = "BASE"
    COMMITTED   = "COMMITTED"
    PREV        = "PREV"

    def __init__(self, name="svn", **options):
        super(SubversionClient, self).__init__(name, **options)
        self._configdir = None
        self._authcache = True
        self._interactive = True

    def __str__(self):
        return "%s, configdir=%s, authcache=%d, interactive=%d" % (super(SubversionClient, self).__str__(), self.configdir, self.authcache, self.interactive)

    #
    # attributes:
    #

    def getconfigdir(self):
        return self._configdir

    def setconfigdir(self, configdir):
        self._configdir = configdir

    configdir = property(fget=getconfigdir, fset=setconfigdir, fdel=None, doc=None)

    def getauthcache(self):
        return self._authcache

    def setauthcache(self, authcache):
        self._authcache = bool(authcache)

    authcache = property(fget=getauthcache, fset=setauthcache, fdel=None, doc=None)

    def getinteractive(self):
        return self._interactive

    def setinteractive(self, interactive):
        self._interactive = bool(interactive)

    interactive = property(fget=getinteractive, fset=setinteractive, fdel=None, doc=None)

    #
    # ScmClient interface:
    #

    def prepareoptions(self, **options):
        """Add top level options to the svn command plus any additional options or subcommand options specified."""
        optstr = ""
        # process standard top level svn options
        if (self.username is not None):
            optstr += " --username " + self.username
        if (self.password is not None):
            optstr += " --password " + self.password
        if (self.configdir is not None):
            optstr += " --config-dir " + self.configdir
        if (self.authcache is False):
            optstr += " --no-auth-cache"
        if (self.interactive is False):
            optstr += " --non-interactive"

        # process svn subcommand options in keyword arguments
        for name, value in options.items():
            optstr += " --" + name
            if ((value is not None) and (value is not True) and (value is not False)):
                optstr += "=" + str(value)

        return optstr

    #
    # svn subcommand interface:
    #

    def info(self, *paths, **options):
        return self.execute("info", *paths, **options)

    def infodict(self, *paths, **options):
        """Return svn info for the target files/revisions as a dictionary."""
        result = self.info(*paths, **options)
        info = {}
        for line in result.splitlines():
            tokens = line.strip().split(": ", 2)
            if (len(tokens) > 1):
                info[tokens[0]] = tokens[1]
        return info

    def cat(self, *paths, **options):
        return self.execute("cat", *paths, **options)

    def list(self, *paths, **options):
        return self.execute("list", *paths, **options)

    def log(self, *paths, **options):
        return self.execute("log", *paths, **options)

    def update(self, *paths, **options):
        return self.execute("update", *paths, **options)

    def add(self, *paths, **options):
        return self.execute("add", *paths, **options)

    def revert(self, *paths, **options):
        return self.execute("revert", *paths, **options)

    def delete(self, *paths, **options):
        return self.execute("delete", *paths, **options)

    def checkout(self, *paths, **options):
        return self.execute("checkout", *paths, **options)

    def commit(self, *paths, **options):
        return self.execute("commit", *paths, **options)

    def lock(self, *paths, **options):
        return self.execute("lock", *paths, **options)

    def unlock(self, *paths, **options):
        return self.execute("unlock", *paths, **options)

    def cleanup(self, *paths, **options):
        return self.execute("cleanup", *paths, **options)

    #
    # util methods:
    #

    def logchangessince(self, path, revision):
        return self.log(path, verbose=True, revision=revision)

    def revisionof(self, path):
        svninfo = self.infodict(path)
        return int(svninfo["Revision"])

    def serverrevisionof(self, path):
        workingcopysvninfo = self.infodict(path)
        serversvninfo = self.infodict(workingcopysvninfo["URL"])
        return int(serversvninfo["Revision"])

    def islatestrevision(self, path, revision):
        serverrevision = self.serverrevisionof(path)
        islatestrevision = False
        if (revision >= serverrevision):
            islatestrevision = True
        return islatestrevision

    def isrunning(self, path):
        """Checks if the subversion server is running."""
        try:
            latestrevision = self.serverrevisionof(path)
            return True
        except Exception:
            traceback.print_exc()
            return False


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    logging.basicConfig()
    svn = SubversionClient()

    argvlen = len(argv)
    path = rev = None
    if (argvlen):
        path = argv[0]
        if (argvlen > 1):
            rev  = int(argv[1])
    else:
        path = _aptanalibpath

    if (rev is None):
        rev = svn.revisionof(path)

    print svn.serverrevisionof(path)
    print svn.islatestrevision(path, rev)
    print svn.logchangessince(path, str(rev) + ":BASE")

    logging.shutdown()

if (__name__ == "__main__"):
    main()
