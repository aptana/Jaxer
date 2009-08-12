#!/usr/bin/env python
import datetime
import logging
import os
import re
import shutil
import sys
import time

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.boot
import aptana.conf
import aptana.scm

__version__ = "$Revision: 4316 $"[11:-2]


class Shell(object):
    """Shell and filesystem accessor."""

    def __init__(self):
        self._instrumentation = aptana.boot.Instrumentation(logging.getLogger(self.__class__.__name__))

    def __str__(self):
        return "%s: %s" % (self.__class__.__name__, self.instrumentation)

    #
    # attributes:
    #

    def getinstrumentation(self):
        return self._instrumentation

    def setinstrumentation(self, instrumentation):
        self._instrumentation.changeinstrumentation(instrumentation)

    instrumentation = property(fget=getinstrumentation, fset=setinstrumentation, fdel=None, doc=None)

    #
    # shutil proxy:
    #

    def destinsrc(self, src, dst):
        return shutil.destinsrc(src, dst)

    #NOTE: always use copy2 to preserve all file stat structure data
    def copy(self, src, dst):
        self.instrumentation.say("copy(" + src + ", " + dst + ")")
        shutil.copy2(src, dst)

    def copytree(self, src, dst, symlinks=False):
        self.instrumentation.say("copytree(" + src + ", " + dst + ", " + str(symlinks) + ")")
        shutil.copytree(src, dst, symlinks)
        
    def scmcopytree(self, src, dst, symlinks=False):
        self.instrumentation.say("scmcopytree(" + src + ", " + dst + ", " + str(symlinks) + ")")
        self.mkdir(dst,ignoreexisting=True)
        sourcedir = os.path.abspath(src)
        destdir = os.path.abspath(dst)
        for root, dirs, files in aptana.scm.scmwalk(src):
            root2 = root.replace(sourcedir,destdir)
            self.mkdir(root2,ignoreexisting=True)
            for file in files:
                shutil.copy2(os.path.join(root, file), os.path.join(root2, file))

    def move(self, src, dst):
        self.instrumentation.say("move(" + src + ", " + dst + ")")
        shutil.move(src, dst)

    def rmtree(self, path, ignoreerrors=False, onerror=None):
        # NOTE: rmtree breaks on windows so set ignoreerrors=True
        if (aptana.conf.iswin()):
            ignoreerrors = True
        self.instrumentation.say("rmtree(" + path + ", " + str(ignoreerrors) + ", " + str(onerror) + ")")
        # if it's a link, remove it, rmtree doesn't follow links
        if (os.path.islink(path)):
            os.remove(path)
        # only remove existing dirs
        elif (os.path.isdir(path)):
            shutil.rmtree(path, ignoreerrors, onerror)

    def rmtreebyctime(self, path, ctimeday=None):
        """Removes a directory tree if it's creation day does not match the specified day.  Day is currently the only granularity supported."""
        if (ctimeday is None):
            now = datetime.datetime.now()
            ctimeday = now.timetuple()[7]
        dirstat = os.stat(path)
        dirctime = datetime.datetime.fromtimestamp(dirstat.st_ctime)
        dirctimeday = dirctime.timetuple()[7]
        if (ctimeday != dirctimeday):
            self.instrumentation.say("removing " + path)
            self.rmtree(path)

    def removeoldfiles(self, path, days=14):
        """Remove files in path that are at least days old."""
        now = int(time.time())
        cutoff = now - (days * 24 * 3600)
        contents = os.listdir(path)
        for aname in contents:
            name = os.path.join(path, aname)
            if (os.path.isfile(name) and os.path.getmtime(name) <= cutoff):
                self.instrumentation.say("removing file " + name)
                os.remove(name)
                
    #
    # os lib proxy:
    #

    def chmod(self, path, mode=0777):
        self.instrumentation.say("chmod(" + path + ", " + str(mode) + ")")
        os.chmod(path, mode)

    def chown(self, path, uid, gid):
        self.instrumentation.say("chown(" + path + ", " + str(uid) + ", " + str(gid) + ")")
        os.chown(path, uid, gid)

    def chdir(self, path):
        self.instrumentation.say("chdir(from=" + os.getcwd() + ", to=" + path + ")")
        os.chdir(path)

    def mkdir(self, path, mode=0777, ignoreexisting=True):
        if (not os.path.isdir(path) or not ignoreexisting):
            self.instrumentation.say("mkdir(" + path + ", " + str(mode) + ", " + str(ignoreexisting) + ")")
            os.mkdir(path, mode)

    def mkdirs(self, path, mode=0777, ignoreexisting=True):
        if (not os.path.isdir(path) or not ignoreexisting):
            self.instrumentation.say("mkdirs(" + path + ", " + str(mode) + ", " + str(ignoreexisting) + ")")
            os.makedirs(path, mode)

    def touch(self, path, times=None):
        self.instrumentation.say("touch(" + path + ", " + str(times) + ")")
        # update atime and mtime - on win32 dirs are not files so this may fail
        if (os.path.exists(path)):
            os.utime(path, times)
        else:
            # create new 0-byte file
            self.write(path)

    def remove(self, path, ignoremissing=False):
        if (os.path.exists(path) or not ignoremissing):
            self.instrumentation.say("remove(" + path + ", " + str(ignoremissing) + ")")
            os.remove(path)

    def rename(self, old, new):
        self.instrumentation.say("rename(" + old + ", " + new + ")")
        os.rename(old, new)

    def renames(self, old, new):
        self.instrumentation.say("renames(" + old + ", " + new + ")")
        os.renames(old, new)

    #
    # file handling utils:
    #

    def read(self, filepath, mode="r"):
        self.instrumentation.say("read(" + filepath + ", " + mode + ")")
        file = None
        try:
            file = open(filepath, mode)
            return file.read()
        finally:
            if (file is not None):
                file.close()

    def write(self, filepath, data="", mode="w"):
        self.instrumentation.say("write(" + filepath + ", " + data[0:32] + "..., " + mode + ")")
        file = None
        try:
            file = open(filepath, mode)
            return file.write(data)
        finally:
            if (file is not None):
                file.close()

    def replace(self, filepath, pattern, replacement, mode="r+"):
        self.instrumentation.say("replace(" + filepath + ", " + pattern + ", " + replacement + ", " + mode + ")")
        file = None
        try:
            file = open(filepath, mode)
            contents = file.read()
            contents = re.sub(pattern, replacement, contents)
            file.seek(0)
            file.truncate(0)
            return file.write(contents)
        finally:
            if (file is not None):
                file.close()

    def matching(self, dirpath, prefix, pattern):
        self.instrumentation.say("matching(" + dirpath + ", " + prefix + ", " + pattern + ")")
        filenames = os.listdir(dirpath)
        selected = []
        for name in filenames:
            if (re.search(pattern, name)):
                selected.append(prefix + name)
        return selected

    def waitfor(self, filepath, timeoutsec=60):
        self.instrumentation.say("waitfor(" + filepath + ", " + str(timeoutsec) + ")")
        interval = 1;
        remaining = timeoutsec;
        while not os.path.exists(filepath) and remaining > 0:
            time.sleep(interval)
            remaining -= interval
        return os.path.exists(filepath)


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    shell = Shell()
    testdir = os.path.join(os.curdir, "..", "shelltest")
    print "Recursively running scmcopytree of . to %s" % (testdir)
    shell.scmcopytree(os.curdir,testdir)
    print shell

if (__name__ == "__main__"):
    main()
