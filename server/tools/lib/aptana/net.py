#!/usr/bin/env python
import ftplib
#import httplib
import logging
import os
import re
import sys
import urllib
import urlparse

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.boot
import aptana.shell

__version__ = "$Revision: 3374 $"[11:-2]


def urlnormalize(url):
    """Normalize the url or path by detecting and escaping win32 drive letters."""
    # if no scheme was found assume it's a file path that needs conversion
    # also detect win32 drive letters which show up as one character schemes
    type = urllib.splittype(url)[0] # (type, url)
    if (type is None or len(type) == 1): # c:
        url = urllib.pathname2url(url)
    return url


def createclient(url):
    """Factory method to create the appropriate handler from the url scheme."""
    type = urllib.splittype(url)[0] # (type, url)
    if (type == "ftp"):
        return FtpClient()
    elif (type == "http"):
        return DefaultClient()
    elif (type == "file"):
        return FileClient()
    else:
        return FileClient()


class URLClient(object):
    """Basic URL handling interface."""
    def get(sourceurl, desturl): pass
    def put(sourceurl, desturl): pass
    def delete(url, ignoremissing=False): pass


class DefaultClient(URLClient):
    """Thin multi-protocol URL based client for getting/putting byte streams."""

    def __init__(self):
        self._instrumentation = aptana.boot.Instrumentation(logging.getLogger(self.__class__.__name__))
        self._shell = aptana.shell.Shell()
        self.setinstrumentation(self._instrumentation)

    def __str__(self):
        return "%s: %s" % (self.__class__.__name__, self.instrumentation)

    #
    # attributes:
    #

    def getinstrumentation(self):
        return self._instrumentation

    def setinstrumentation(self, instrumentation):
        self._instrumentation.changeinstrumentation(instrumentation)
        self._shell.instrumentation.changeinstrumentation(instrumentation)

    instrumentation = property(fget=getinstrumentation, fset=setinstrumentation, fdel=None, doc=None)

    #
    # URLClient interface:
    #

    def get(self, sourceurl, desturl):
        """Base get uses urllib which supports ftp, http, etc..."""
        # url manipulation - dest url is assumed to be a local file
        destparts = urlparse.urlsplit(desturl)
        (destdirs, destfilename) = os.path.split(destparts.path)

        # create destination directory if it doesn't exist
        if (len(destdirs) > 0 and not os.path.isdir(destdirs)):
            self._shell.mkdirs(destdirs)

        self.instrumentation.say("get(%s, %s)" % (sourceurl, desturl))

        # get the url
        urllib.urlretrieve(sourceurl, desturl)

    def put(self, sourceurl, desturl):
        pass

    def delete(self, url, ignoremissing=False):
        pass


class FileClient(DefaultClient):
    """file: URL client."""

    def get(self, sourceurl, desturl):
        self.put(sourceurl, desturl)

    def put(self, sourceurl, desturl):
        sourceurl = urllib.url2pathname(sourceurl)
        desturl = urllib.url2pathname(desturl)
        self.instrumentation.say("put(%s, %s)" % (sourceurl, desturl))
        destdir = os.path.dirname(desturl)
        self._shell.mkdirs(destdir, ignoreexisting=True)
        self._shell.copy(sourceurl, desturl)

    def delete(self, url, ignoremissing=False):
        url = urllib.url2pathname(url)
        self.instrumentation.say("delete(%s, %s)" % (url, str(ignoremissing)))
        self._shell.remove(url, ignoremissing)


class FtpClient(DefaultClient):
    """ftp: URL client."""

    def connect(self, hostname, username, password, debuglevel=0):
        if (debuglevel is None):
            debuglevel = 0
        ftp = ftplib.FTP(hostname)
        ftp.set_debuglevel(debuglevel)
        if (password is not None and len(password) > 0):
            password = urllib.unquote(password)
        ftp.login(username, password)
        return ftp

    #
    # URLClient interface:
    #

    def get(self, sourceurl, desturl):
        # url manipulation
        sourceparts = urlparse.urlsplit(sourceurl, "ftp")
        (sourcedirs, sourcefilename) = os.path.split(sourceparts.path)

        # remove leading slash for home dir
        if (sourcedirs.startswith("/~")):
            sourcedirs = sourcedirs[1:]
        destparts = urlparse.urlsplit(desturl)
        (destdirs, destfilename) = os.path.split(destparts.path)

        self.instrumentation.say("get(%s, %s)" % (sourceurl, desturl))

        ftp  = None
        file = None
        try:
            # connect
            ftp = self.connect(sourceparts.hostname, sourceparts.username, sourceparts.password, self.instrumentation.logger.level)
            # create dest dir ignoring errors if it already exists
            if (len(destdirs) > 0 and not os.path.isdir(destdirs)):
                self._shell.mkdirs(destdirs)
            # open dest url (only on the local file system for now)
            #file = urllib.urlopen(desturl)
            file = open(desturl, "w")
            # cd before get
            ftp.cwd(sourcedirs)
            # get the file, passing the write method on the file object as a callback
            ftp.retrbinary("RETR " + sourcefilename, file.write)
        finally:
            if (file is not None):
                file.close()
            if (ftp is not None):
                ftp.quit()

    def put(self, sourceurl, desturl):
        # url manipulation
        sourceurl = urlnormalize(sourceurl)
        sourceparts = urlparse.urlsplit(sourceurl)
        (sourcedirs, sourcefilename) = os.path.split(sourceparts.path)
        destparts = urlparse.urlsplit(desturl, "ftp")
        (destdirs, destfilename) = os.path.split(destparts.path)
        # remove leading slash for home dir
        if (destdirs.startswith("/~")):
            destdirs = destdirs[1:]
        # if dest url specifies only a dest dir (ends with slash) use the file name from source url
        if (len(destfilename) == 0):
            destfilename = sourcefilename

        self.instrumentation.say("put(%s, %s)" % (sourceurl, desturl))

        ftp  = None
        file = None
        try:
            # connect
            ftp = self.connect(destparts.hostname, destparts.username, destparts.password, self.instrumentation.logger.level)
            # create dest dir ignoring errors if it already exists
            try:
                ftp.mkd(destdirs)
            except ftplib.error_perm, ep:
                matches = re.findall("^([0-9]+)", ep.message)
                if (len(matches) == 0):
                    raise
                replycode = int(matches[0])
                if (replycode < 550 and replycode != 521):
                    raise

            # cd to the directory in which to store the file
            ftp.cwd(destdirs)
            # open source url
            file = urllib.urlopen(sourceurl)
            # store file
            ftp.storbinary("STOR " + destfilename, file)
        finally:
            if (file is not None):
                file.close()
            if (ftp is not None):
                ftp.quit()

    def delete(self, url, ignoremissing=False):
        # url manipulation
        urlparts = urlparse.urlsplit(url, "ftp")
        (dirs, filename) = os.path.split(urlparts.path)
        # remove leading slash for home dir
        if (dirs.startswith("/~")):
            dirs = dirs[1:]

        self.instrumentation.say("delete(%s, %s)" % (url, str(ignoremissing)))

        ftp = None
        try:
            # connect
            ftp = self.connect(urlparts.hostname, urlparts.username, urlparts.password, self.instrumentation.logger.level)
            # cd to the dir containing file
            try:
                ftp.cwd(dirs)
            except ftplib.error_perm, ep:
                if (not ignoremissing):
                    raise
            # delete file
            try:
                ftp.delete(filename)
            except ftplib.error_perm, ep:
                if (not ignoremissing):
                    raise
        finally:
            if (ftp is not None):
                ftp.quit()


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    if (len(argv) < 3):
        print "Usage: %s ftphostname username password" % (sys.argv[0])
        return

    client = createclient("http:")
    client.instrumentation.logger.level = logging.DEBUG
    client.get("http://www.aptana.com", "aptana.tmp")

    file = createclient("file:")
    file.instrumentation.logger.level = logging.DEBUG
    file.delete("aptana.tmp")
    file.get(__file__, __file__ + ".file.get.tmp")
    file.delete(__file__ + ".file.get.tmp")
    file.put(__file__, __file__ + ".file.put.tmp")
    file.delete(__file__ + ".file.put.tmp")
    file.delete(__file__ + ".file.put.tmp", ignoremissing=True)

    hostname = argv[0]
    username = argv[1]
    password = urllib.quote(argv[2])

    ftp = createclient("ftp:")
    ftp.instrumentation.logger.level = logging.DEBUG
    ftpurl = "ftp://%s:%s@%s/~/__pythonftptest__" % (username, password, hostname)
    ftp.delete("%s/ftpremote.tmp" % (ftpurl), ignoremissing=True)
    ftp.put(os.path.abspath(__file__),
            "%s/ftpremote.tmp" % (ftpurl))
    ftp.delete("%s/ftpremote.tmp" % (ftpurl), ignoremissing=True)
    ftp.put("file:" + os.path.abspath(__file__),
            "%s/ftpremote.tmp" % (ftpurl))
    ftp.delete("%s/ftpremote.tmp" % (ftpurl), ignoremissing=True)
    ftp.put("http://ec2-67-202-39-216.compute-1.amazonaws.com/downloads/jaxer/mac/Jaxer_package_withApache.dmg",
            "%s/" % (ftpurl))
    ftp.get("%s/Jaxer_package_withApache.dmg" % (ftpurl), "ftp.tmp")
    os.remove("ftp.tmp")
    ftp.delete("%s/Jaxer_package_withApache.dmg" % (ftpurl), ignoremissing=True)
    ftp.delete("%s" % (ftpurl), ignoremissing=True)
 
if (__name__ == "__main__"):
    main()
