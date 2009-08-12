#!/usr/bin/env python
import os
import sys

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.script

__version__ = "$Revision: 3624 $"[11:-2]


class ServiceError(Exception):
    """Error thrown while manipulating a service."""


class Service(aptana.script.Script):
    """Service abstract interface for managed server processes."""

    def __init__(self, configuration, methodname="status", *args):
        super(Service, self).__init__(configuration, methodname)
        self._args = args
        self._name = None
        self._title = None
        self._pidfilepath = None
        self._delay = 0

    def __str__(self):
        return "%s: %s" % (self.__class__.__name__, self._name)

    #
    # attributes:
    #

    def getargs(self):
        return self._args

    args = property(fget=getargs, fset=None, fdel=None, doc=None)

    def getname(self):
        return self._name

    def setname(self, name):
        self._name = name
        
    name = property(fget=getname, fset=setname, fdel=None, doc=None)

    def gettitle(self):
        return self._title

    def settitle(self, title):
        self._title = title

    title = property(fget=gettitle, fset=settitle, fdel=None, doc=None)

    def getpidfilepath(self):
        return self._pidfilepath

    def setpidfilepath(self, pidfilepath):
        self._pidfilepath = pidfilepath

    pidfilepath = property(fget=getpidfilepath, fset=setpidfilepath, fdel=None, doc=None)

    def getdelay(self):
        return self._delay

    def setdelay(self, delay):
        self._delay = delay

    delay = property(fget=getdelay, fset=setdelay, fdel=None, doc=None)

    #
    # Service interface:
    #

    def configure(self):
        pass

    def start(self):
        pass

    def stop(self,restart=False):
        pass

    def restart(self):
        self.stop(restart=True)
        self.start()

    def status(self):
        pass


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    service = Service()
    service.main()
 
if (__name__ == "__main__"):
    main()
