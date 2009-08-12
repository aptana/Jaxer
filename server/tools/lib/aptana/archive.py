#!/usr/bin/env python
import gzip
import hashlib
import logging
import os
import sys
import tarfile
import zipfile
import zlib
import distutils.command.bdist_rpm

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.boot
import aptana.process
import aptana.shell

__version__ = "$Revision: 4185 $"[11:-2]


def createarchiver(filepath, mode="r", compression=None):
    """Factory method to create the appropriate archiver instance to handle the specified archive file."""
    # check by file extension
    if (filepath.endswith(".zip")):
        return ZipArchiver(filepath, mode, compression)
    elif (filepath.endswith(".tar") or filepath.endswith(".tar.gz") or filepath.endswith(".tgz")):
        return TarArchiver(filepath, mode, compression)
    elif (filepath.endswith(".gz")): # must come after .tar.gz
        return GZipArchiver(filepath, mode, compression)
    elif (filepath.endswith(".rpm")):
        return RpmArchiver(filepath, mode, compression)
    elif (filepath.endswith(".deb")):
        return DebArchiver(filepath, mode, compression)
    elif (filepath.endswith(".dmg")):
        return DmgArchiver(filepath, mode, compression)
    elif (filepath.endswith(".jar") or filepath.endswith(".war") or filepath.endswith(".ear")):
        return JarArchiver(filepath, mode, compression)
    # check by filetype
    elif (zipfile.is_zipfile(filepath)):
        return ZipArchiver(filepath, mode, compression)
    elif (tarfile.is_tarfile(filepath)):
        return TarArchiver(filepath, mode, compression)
    # unsupported
    else:
        raise ArchiveError("Unsupported archive file: " + filepath)


class ArchiveError(Exception):
    """Error occurred during archiving."""


class Archiver(object):
    """File archiver interface."""
    name = property()
    def include(sourcepath, excludeexpressions=None): pass
    def includeall(sourcepath, cwd=None): pass
    def exclude(sourcefilepath): pass
    def assemble(digestalgorithm=None): pass
    def compress(sourcefilepath, destfilepath): pass
    def decompress(sourcefilepath, destdirpath=None, destdirmode=0777): pass
    def digest(sourcefilepath, destfilepath=None, digestalgorithm="md5"): pass
    def list(sourcefilepath=None, destfilepath=None, destfilemode="w", verbose=False): pass
    def verify(sourcefilepath): pass


class DefaultArchiver(Archiver):
    """File archiving abstract interface."""

    def __init__(self, name=None, destfilepath=None, mode=None, compression=None):
        self._name = name
        self._destfilepath = destfilepath
        self._sourcedirs = []
        #self._sourcefiles = []
        self._excludes = []
        self._instrumentation = aptana.boot.Instrumentation(logging.getLogger(self.__class__.__name__))
        self._executor = aptana.process.LocalhostProcessExecutor()
        self._shell = aptana.shell.Shell()
        self.setinstrumentation(self._instrumentation)

    def __str__(self):
        return "%s: name=%s, destfilepath=%s, sourcedirs=%s, excludes=%s, executor=%s, shell=%s, instrumentation=%s" % (self.name, self.destfilepath, str(self._sourcedirs), str(self._excludes), self._executor, self._shell, self.instrumentation)

    #
    # attributes:
    #

    def getinstrumentation(self):
        return self._instrumentation

    def setinstrumentation(self, instrumentation):
        self._instrumentation.changeinstrumentation(instrumentation)
        self._shell.instrumentation.changeinstrumentation(instrumentation)
        self._executor.instrumentation.changeinstrumentation(instrumentation)
        self.debug(instrumentation.logger.level)

    instrumentation = property(fget=getinstrumentation, fset=setinstrumentation, fdel=None, doc=None)

    def getname(self):
        return self._name

    name = property(fget=getname, fset=None, fdel=None, doc=None)

    def getdestfilepath(self):
        return self._destfilepath

    destfilepath = property(fget=getdestfilepath, fset=None, fdel=None, doc=None)

    #
    # Archiver interface:
    #

    def debug(self, debuglevel=0):
        pass

    #REVIEW: using chdir is not a good solution, instead add paths relative to a specific dir by string manipulation
    def includeall(self, sourcepath, cwd=None):
        fromcwd = os.getcwd()
        tocwd = fromcwd
        try:
            if (cwd is not None):
                tocwd = os.path.abspath(cwd)
                self._shell.chdir(tocwd)

            for root, dirs, files in os.walk(sourcepath):
                for dir in dirs:
                    if (dir in self._excludes):
                        dirs.remove(dir)
                    else:
                        dirpath = os.path.join(root, dir)
                        #print dirpath
                        self.include(dirpath)
                for file in files:
                    if (file in self._excludes):
                        files.remove(file)
                    else:
                        filepath = os.path.join(root, file)
                        #print filepath
                        self.include(filepath)
        finally:
            if (cwd is not None):
                self._shell.chdir(fromcwd)

    def include(self, sourcepath, excludeexpressions=None):
        self._sourcedirs.append(sourcepath)

    def exclude(self, sourcefilepath):
        self._excludes.append(sourcefilepath)

    def assemble(self, digestalgorithm=None):
        pass

    def compress(self, sourcefilepath, destfilepath):
        pass

    def decompress(self, sourcefilepath, destdirpath=None, destdirmode=0777):
        pass

    def digest(self, sourcefilepath, destfilepath=None, digestalgorithm="md5"):
        md = None
        sourcefile = None
        destfile = None
        try:
            sourcefile = open(sourcefilepath, "rb")
            md = hashlib.new(digestalgorithm)
            md.update(sourcefile.read())
            hexdigest = md.hexdigest()
            if (destfilepath is not None):
                destfile = open(destfilepath, "w")
                destfile.write(hexdigest)
            return hexdigest
        finally:
            if (sourcefile is not None):
                sourcefile.close()
            if (destfile is not None):
                destfile.close()

    def _buildlisting(self, sourcefilepath=None, verbose=False):
        return self._sourcedirs

    def list(self, sourcefilepath=None, destfilepath=None, destfilemode="w", verbose=False):
        namelist = self._buildlisting(sourcefilepath)
        namestr = os.linesep.join(namelist)
        if (destfilepath is None):
            print namestr
        else:
            destfile = None
            try:
                destfile = open(destfilepath, destfilemode)
                destfile.write(namestr)
            finally:
                if (destfile is not None):
                    destfile.close()

    def verify(self, sourcefilepath):
        return True

    def dump(self, sourcefilepath, destfilepath=None, destdirmode=0777):
        if (destfilepath is not None):
            destdirpath = os.path.abspath(os.path.dirname(destfilepath))
            self._shell.mkdirs(destdirpath, destdirmode, ignoreexisting=True)

        input = None
        output = None
        opened = False
        try:
            input = open(sourcefilepath)
            if (destfilepath is None):
                output = sys.stdout
            else:
                output = open(destfilepath, "w")
                opened = True
            output.write(input.read())
        finally:
            if (input is not None):
                input.close()
            if (output is not None and opened):
                output.close()


class GzipArchiver(DefaultArchiver):
    """Gzip archiving client."""

    def __init__(self, destfilepath=None, mode="w", compression=zlib.DEFLATED):
        super(GzipArchiver, self).__init__("gzip", destfilepath, mode, compression)

    def compress(self, sourcefilepath, destfilepath, mode="w", compression=zlib.DEFLATED):
        """Compress a single file into a gzip archive."""
        destdirpath = None
        if (destfilepath is None):
            destdirpath = os.getcwd()
            destfilepath = os.path.join(destdirpath, os.path.basename(sourcefilepath))
        else:
            destdirpath = os.path.dirname(destfilepath)
        if (not os.path.exists(destdirpath)):
            self._shell.mkdirs(destdirpath, destdirmode, ignoreexisting=True)

        input = None
        output = None
        try:
            input = open(sourcefilepath, "rb")
            output = gzip.GzipFile(destfilepath, mode, compression)
            output.write(input.read())
        finally:
            if (input is not None):
                input.close()
            if (output is not None):
                output.close()

    def decompress(self, sourcefilepath, destfilepath=None, destdirmode=0777):
        """Decompress a gzip file into the destination directory."""
        destdirpath = None
        if (destfilepath is None):
            destdirpath = os.getcwd()
            destfilepath = os.path.join(destdirpath, os.path.basename(sourcefilepath))
        else:
            destdirpath = os.path.dirname(destfilepath)
        if (not os.path.exists(destdirpath)):
            self._shell.mkdirs(destdirpath, destdirmode, ignoreexisting=True)

        input = None
        output = None
        try:
            input = gzip.GzipFile(sourcefilepath, "rb")
            output = open(destfilepath, "w")
            output.write(input.read())
        finally:
            if (input is not None):
                input.close()
            if (output is not None):
                output.close()


class ZipArchiver(DefaultArchiver):
    """Zip archiving client."""

    def __init__(self, destfilepath=None, mode="w", compression=zipfile.ZIP_DEFLATED):
        # must be defined before super constructor
        self._zip = None
        if (destfilepath is not None):
            self._zip = zipfile.ZipFile(destfilepath, mode, compression)
        super(ZipArchiver, self).__init__("zip", destfilepath, mode, compression)

    def __del__(self):
        if (self._zip is not None):
            self._zip.close()

    def debug(self, debuglevel=0):
        if (debuglevel is None):
            debuglevel = 0
        if (self._zip is not None):
            self._zip.debug = debuglevel

    def include(self, sourcepath, archfilename=None, compresstype=None, excludeexpressions=None):
        # python zipfile lib doesn't support storing directory entries
        if (os.path.isfile(sourcepath)):
            self._sourcedirs.append(sourcepath)
            self._zip.write(sourcepath, archfilename, compresstype)

    def assemble(self, digestalgorithm=None):
        #self._zip.testzip() # this causes errors...
        self._zip.close()
        if (digestalgorithm is not None):
            self.digest(self._destfilepath, self._destfilepath + "." + digestalgorithm, digestalgorithm)

    def compress(self, sourcefilepath, destfilepath, mode="w", compression=zipfile.ZIP_DEFLATED):
        """Compress a single file into a zip archive."""
        zip = None
        try:
            zip = zipfile.ZipFile(destfilepath, mode, compression)
            # compress a directory recursively
            if (os.path.isdir(sourcefilepath)):
                for root, dirs, files in os.walk(sourcefilepath):
                    for file in files:
                        zip.write(os.path.join(root, file))
            else:
                # compress a single file
                (base, filename) = os.path.split(sourcefilepath)
                zip.write(sourcefilepath, filename)
        finally:
            if (zip is not None):
                zip.close()

    def decompress(self, sourcefilepath, destdirpath=None, destdirmode=0777):
        """Decompress a zip file into the destination directory."""
        zip = None
        try:
            # open archive
            zip = zipfile.ZipFile(sourcefilepath)
            # create top level destination directory
            if (destdirpath is None):
                destdirpath = os.getcwd()
            self._shell.mkdirs(destdirpath, destdirmode, ignoreexisting=True)
            # process archive entries
            for name in zip.namelist():
                filepath = os.path.join(destdirpath, name)
                # extract directory entries and create them (python zip cannot add directory entries)
                # Ideally this would be name.endswith(os.sep) but the old build scripts create zips with /'s on windows,
                # so we need to explicitly check for both for now.
                if (name.endswith('/') or name.endswith('\\')):
                    self.debug("zip decompress mkdirs %s" % (filepath))
                    self._shell.mkdirs(filepath, destdirmode, ignoreexisting=True)
                else:
                    # extract files and create directories based on the dirname
                    dirpath = os.path.dirname(filepath)
                    self.debug("zip decompress mkdirs %s filepath %s" % (dirpath, filepath))
                    self._shell.mkdirs(dirpath, destdirmode, ignoreexisting=True)
                    outfile = None
                    try:
                        outfile = open(filepath, "wb")
                        outfile.write(zip.read(name))
                    finally:
                        if (outfile is not None):
                            outfile.close()
        finally:
            if (zip is not None):
                zip.close()

    def _buildlisting(self, sourcefilepath=None, verbose=False):
        namelist = None
        if (sourcefilepath is None):
            namelist = self._zip.namelist()
        else:
            zip = None
            try:
                zip = zipfile.ZipFile(sourcefilepath)
                namelist = zip.namelist()
            finally:
                if (zip is not None):
                    zip.close()
        return namelist


class TarArchiver(DefaultArchiver):
    """tar archiving client."""

    def __init__(self, destfilepath=None, mode="w:", compression="gz"):
        if (compression is not None):
            mode += compression
        # must be defined before super constructor
        self._tar = None
        if (destfilepath is not None):
            self._tar = tarfile.open(destfilepath, mode)
        super(TarArchiver, self).__init__("tar", destfilepath, mode, compression)

    def __del__(self):
        if (self._tar is not None):
            self._tar.close()

    def debug(self, debuglevel=0):
        if (debuglevel is None):
            debuglevel = 0
        if (self._tar is not None):
            self._tar.errorlevel = debuglevel
            self._tar.debug = debuglevel

    def include(self, sourcefilepath):
        self._tar.add(sourcefilepath, recursive=False) # default is recursive

    def assemble(self, digestalgorithm=None):
        self._tar.close()

    def compress(self, sourcefilepath, destfilepath, mode="w:", compression="gz"):
        if (compression is not None):
            mode += compression
        if (destfilepath.endswith(".gz") or destfilepath.endswith(".tgz")):
            mode = "w:gz"
        tar = None
        try:
            tar = tarfile.open(destfilepath, mode)
            tar.errorlevel = self.instrumentation.logger.level
            tar.debug = self.instrumentation.logger.level
            tar.add(sourcefilepath) # default is recursive
        finally:
            if (tar is not None):
                tar.close()

    def decompress(self, sourcefilepath, destdirpath=None, mode="r"):
        tar = None
        try:
            if (destdirpath is None):
                destdirpath = os.getcwd()
            tar = tarfile.open(sourcefilepath, mode)
            tar.errorlevel = self.instrumentation.logger.level
            tar.debug = self.instrumentation.logger.level
            tar.extractall(destdirpath);
        finally:
            if (tar is not None):
                tar.close()

    def _buildlisting(self, sourcefilepath=None, verbose=False):
        namelist = None
        if (sourcefilepath is None):
            namelist = self._tar.getnames()
        else:
            tar = None
            try:
                tar = tarfile.open(sourcefilepath)
                namelist = tar.getnames()
            finally:
                if (tar is not None):
                    tar.close()
        return namelist


#TODO:
class RpmArchiver(DefaultArchiver):
    """linux rpm archiving client."""

    def __init__(self, name="bdist", destfilepath=None, mode=None, compression=None):
        if (compression is not None):
            mode += compression
        self._rpm = None
        if (destfilepath is not None):
            self._rpm = distutils.command.bdist_rpm
        super(RpmArchiver, self).__init__(name, destfilepath, mode, compression)


#TODO:
class DebArchiver(DefaultArchiver):
    """Debian package archiving client."""

    def __init__(self, name="bdist", destfilepath=None, mode=None, compression=None):
        if (compression is not None):
            mode += compression
        self._deb = None
        # bdist_deb isn't part of the default distribution.
        super(DebArchiver, self).__init__(name, destfilepath, mode, compression)


#Running Command: hdiutil create -type SPARSE -fs HFS+ -volname "Aptana Jaxer Package" -size 1024m "../distro/Jaxer_package_withApache.dmg.sparseimage"
class DmgArchiver(DefaultArchiver):
    """macos disk image archiving client.  Wrapper for hdiutil."""

    def __init__(self, name="hdiutil", destfilepath=None, mode=None, compression=None):
        super(DmgArchiver, self).__init__(name, destfilepath, mode, compression)

    #TODO: add options hash with defaults
    def attach(self, sourcefilepath, readonly=True):
        hdiutil = aptana.process.Process(self.name)
        hdiutil.arg("attach")
        if (readonly):
            hdiutil.arg("-readonly")
        hdiutil.quotearg(sourcefilepath)
        hdiutil.execute(self._executor)

    def detach(self, sourcefilepath):
        hdiutil = aptana.process.Process(self.name)
        hdiutil.arg("detach")
        hdiutil.quotearg(sourcefilepath)
        hdiutil.execute(self._executor)


class JarArchiver(DefaultArchiver):
    """Java jar/war/ear archiver."""

    def __init__(self, name="jar", destfilepath=None, mode=None, compression=None):
        super(JarArchiver, self).__init__(name, destfilepath, mode, compression)
        self._debuglevel = 0

    def debug(self, debuglevel=0):
        self._debuglevel = debuglevel

    def mode(self, mode):
        if (self._debuglevel == logging.DEBUG and mode.find("v") == -1):
            mode += "v"
        return mode

    def assemble(self, destfilepath, mode="cf", cwd=None):
        if (cwd is None):
            cwd = os.getcwd()
        jar = aptana.process.Process(self.name)
        jar.cwd = cwd
        jar.arg(self.mode(mode))
        jar.arg(destfilepath)
        for filepath in self._sourcedirs:
            jar.arg(filepath)
        jar.execute(self._executor)

    def compress(self, sourcefilepath, destfilepath, mode="cf"):
        jar = aptana.process.Process(self.name)
        jar.arg(self.mode(mode))
        jar.arg(destfilepath)
        jar.arg(sourcefilepath)
        jar.execute(self._executor)

    def decompress(self, sourcefilepath, destdirpath=None, mode="xf"):
        if (destdirpath is None):
            destdirpath = os.getcwd()
        jar = aptana.process.Process(self.name)
        jar.cwd = destdirpath
        jar.arg(self.mode(mode))
        jar.arg(sourcefilepath)
        jar.execute(self._executor)

    def list(self, sourcefilepath, mode="tf"):
        jar = aptana.process.Process(self.name)
        jar.arg(self.mode(mode))
        jar.arg(sourcefilepath)
        jar.execute(self._executor)


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    arch = DefaultArchiver()
    arch.debug(3)
    arch.exclude(".svn")
    arch.includeall(".")
    arch.list()

    hexdigest = arch.digest(__file__, "hexdigest.log", "md5")
    file = open("hexdigest.log", "r")
    contents = file.read()
    file.close()
    os.remove("hexdigest.log")
    print hexdigest
    assert hexdigest == contents

    jar = JarArchiver()
    jar.debug(3)
    #TODO:

    gzip = GzipArchiver()
    gzip.debug(3)
    gzip.compress(__file__, __file__ + ".gz")
    gzip.decompress(__file__ + ".gz", __file__ + ".gzbak")
    gzip.dump(__file__ + ".gzbak")
    os.remove(__file__ + ".gz")
    os.remove(__file__ + ".gzbak")

    zip = ZipArchiver("../cwd.zip")
    zip.debug(3)
    zip.exclude(".svn")
    zip.includeall(".")
    zip.list()
    zip.list(None, "../cwd-zip.log")
    zip.assemble()
    zip.list("../cwd.zip")
    zip.list("../cwd.zip", "../cwd2-zip.log")
    os.remove("../cwd.zip")
    os.remove("../cwd-zip.log")
    os.remove("../cwd2-zip.log")

    tar = TarArchiver("cwd.tgz")
    tar.debug(3)
    tar.exclude(".svn")
    tar.includeall(".")
    tar.list()
    tar.list(None, "cwd-tgz.log")
    tar.assemble()
    tar.list("cwd.tgz")
    tar.list("cwd.tgz", "cwd2-tgz.log")
    os.remove("cwd.tgz")
    os.remove("cwd-tgz.log")
    os.remove("cwd2-tgz.log")

if (__name__ == "__main__"):
    main()
