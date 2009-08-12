#!/usr/bin/env python
import os
import sys
import time
import traceback

# Add aptana tools and lib dirs to module path, derived from abspath of this file within tools dir.
# This should happen when the interpreter boots, not once per looped execution,
# in case the cwd is changed.
_aptanatoolspath = os.path.abspath(os.path.dirname(__file__))
if (_aptanatoolspath not in sys.path):
    sys.path.append(_aptanatoolspath)
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), "lib"))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

# Import all aptana libraries that can be dynamically reloaded.
import aptan
import aptana.archive
import aptana.boot
import aptana.conf
import aptana.net
import aptana.notification
import aptana.process
import aptana.scm
import aptana.script
import aptana.service
import aptana.shell
import aptana.jaxer.jxrbuild
import aptana.jaxer.jxrcmdln
import aptana.jaxer.jxrservice
import aptana.jaxer.jxrtestsuite

__version__ = "$Revision: 3559 $"[11:-2]


def _libuncache(topdir=None):
    """Cleans out generated .pyc and .pyo files to purge module cache."""
    if (topdir is None): # default is tools dir
        topdir = os.path.abspath(os.path.dirname(__file__))
    # clean out generated .pyc and .pyo files
    for root, dirnames, filenames in os.walk(topdir):
        for filename in filenames:
            if (filename.endswith(".pyc") or filename.endswith(".pyo")):
                filepath = os.path.join(root, filename)
                os.remove(filepath)

def _libreload():
    """Dynamically reloads all aptana libraries to support non-stop looped execution."""
    reload(aptan)
    reload(aptana.archive)
    reload(aptana.boot)
    reload(aptana.conf)
    reload(aptana.net)
    reload(aptana.notification)
    reload(aptana.process)
    reload(aptana.scm)
    reload(aptana.script)
    reload(aptana.service)
    reload(aptana.shell)
    reload(aptana.jaxer.jxrbuild)
    reload(aptana.jaxer.jxrcmdln)
    reload(aptana.jaxer.jxrservice)
    reload(aptana.jaxer.jxrtestsuite)

def main(argv=None):
    """Main entry point from the command line.
       The main loop is separated from the jxrcmdln module file to allow it to be reloaded."""
    if (argv is None):
        argv = sys.argv

    # set defaults before creating a JaxerCmdln instance
    aptan = None
    name = "aptan"
    delay = 0
    loop = 1
    loopretrydelay = 180
    rc = 0

    # main loop - if loop < 0 runs forever
    i = 1
    while (loop < 0 or i <= loop):
        try:
            # first run
            if (i == 1):
                aptan = aptana.jaxer.jxrcmdln.JaxerCmdln()
                aptan.deletestopfile() # clean the stop file but don't process a stop on the first run
                rc = aptan.main(argv)
                # usage error - cannot possibly loop so return
                if (rc == 1 or not hasattr(aptan, "options")):
                    break
            else:
                # update _aptanalibpath before reloading to pick up changes this build.
                # REVIEW: will using scm lock it from being updated on windows?  hopefully it'll only lock the pyc
                scm = aptana.scm.SubversionClient()
                scm.update(_aptanalibpath)
                # reload libraries to pick up changes
                _libuncache()
                _libreload()
                # create new JaxerCmdln after reload
                aptan = aptana.jaxer.jxrcmdln.JaxerCmdln()
                if (aptan.stop()): # stop - returning the rc from the previous run
                    break
                rc = aptan.main(argv)
        except Exception: # doesn't catch SystemExit or KeyboardInterrupt
            if (rc is None):
                rc = 1
            traceback.print_exc(file=sys.__stderr__)
            # on any exception add extra sleep time to avoid killing the cpu
            if (loop != 1):
                print >> sys.__stderr__, "[%s]:(%s):STATUS: Sleeping for %s second(s) after exception..." % (time.strftime("%Y%m%d-%H:%M:%S"), name, loopretrydelay)
                time.sleep(loopretrydelay)
        else:
            if (rc is None):
                rc = 0
            # on success update local caches of aptan options to support modification in aptan.main()
            name = aptan.name
            delay = aptan.options.delay
            loop = aptan.options.loop
            loopretrydelay = aptan.options.loopretrydelay
            logredirection = aptan.options.logredirection
        finally:
            if (loop != 1):
                print >> sys.__stderr__, "[%s]:(%s):STATUS: rc=%s, i=%d, loop=%d, delay=%d, loopretrydelay=%d, argv=%s" % (time.strftime("%Y%m%d-%H:%M:%S"), name, str(rc), i, loop, delay, loopretrydelay, " ".join(argv))
            sys.__stderr__.flush()
            sys.stderr.flush()
            sys.__stdout__.flush()
            sys.stdout.flush()
            # always increment even if an exception was thrown
            i += 1

    return rc

if (__name__ == "__main__"):
    sys.exit(main())
