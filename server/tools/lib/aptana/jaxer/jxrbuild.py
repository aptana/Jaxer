#!/usr/bin/env python
import glob
import logging
import os
import re
import sys
import time
import traceback

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.archive
import aptana.conf
import aptana.net
import aptana.process
import aptana.scm
import aptana.script
import aptana.shell
import aptana.jaxer.jxrtestsuite

__version__ = "$Revision: 4629 $"[11:-2]


class BuildError(aptana.script.ScriptError):
    """Signals that a build error has occurred usually to halt further execution."""


class Build(aptana.script.Script):
    """Monolithic build script for jaxer."""

    _buildnumber = 0
    _buildevents = []

    #
    # script interface:
    #

    def init(self):
        """Initializes variables used by the build process."""
        # use relative path if svnroot is not defined
        if (self.conf["svnroot"] is None or len(self.conf["svnroot"]) == 0):
            self.svnroot = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", ".." , "..", ".."))
        else:
            self.svnroot = self.conf["svnroot"]
        self.logger.debug("svnroot=%s" % (self.svnroot))

        # use relative path if buildroot is not defined
        if (self.conf["buildroot"] is None or len(self.conf["buildroot"]) == 0):
            self.buildroot = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", ".."))
        else:
            self.buildroot = self.conf["buildroot"]
        self.logger.debug("buildroot=%s" % (self.buildroot))

        # use relative path if docgenroot is not defined
        if (self.conf["docgenroot"] is None or len(self.conf["docgenroot"]) == 0):
            self.docgenroot = os.path.join(self.svnroot, "ide_crossover", "libs")
        else:
            self.docgenroot = self.conf["docgenroot"]
        self.logger.debug("docgenroot=%s" % (self.docgenroot))

        self.docroot = os.path.join(self.buildroot, "jam", "jaxer", "aptana", "doc", "api")
        self.logger.debug("docroot=%s" % (self.docroot))

        self.toolsroot = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
        self.logger.debug("toolsroot=%s" % (self.toolsroot))
        self.toolsconfroot = os.path.join(self.toolsroot, "conf")
        self.logger.debug("toolsconfroot=%s" % (self.toolsconfroot))
        self.toolscontrolroot = os.path.join(self.toolsroot, "ctrl")
        self.logger.debug("toolscontrolroot=%s" % (self.toolscontrolroot))
        #self.toolstemproot = os.path.join(self.toolsroot, "tmp")
        #self.logger.debug("toolstemproot=%s" % (self.toolstemproot))
        #self.toolslogroot = os.path.join(self.toolsroot, "logs")
        #self.logger.debug("toolslogroot=%s" % (self.toolslogroot))

        self.buildstatefilepath = os.path.join(self.toolsconfroot, "buildstate.properties")
        self.logger.debug("buildstatefilepath=%s" % (self.buildstatefilepath))

        self.topdir = self.conf["install_dirname"]
        self.logger.debug("topdir=%s" % (self.topdir))

        # root all system commands in the buildroot
        self.origcwd = os.getcwd()
        self.shell.chdir(self.buildroot)

        self.copymozconfig()
        self.mozilladir = os.path.abspath("src/mozilla").replace("\\", "/")
        self.logger.debug("mozilladir=%s" % (self.mozilladir))

        self.ffdir = self.readmozconfig() # /ff-release
        self.logger.debug("ffdir=%s" % (self.ffdir))

        self.framework = self.mozilladir + "/" + self.ffdir + "/dist/bin/framework"
        self.logger.debug("framework=%s" % (self.framework))
        
        self.proframework = self.mozilladir + "/" + self.ffdir + "/dist/bin/proframework"
        self.logger.debug("proframework=%s" % (self.proframework))

    def destroy(self):
        """Closes any resources or files created during the initialization process."""
        self.shell.remove("src/mozilla/.mozconfig", ignoremissing=True)
        self.shell.remove(os.path.join(self.toolscontrolroot, "forcebuild.ctrl"), ignoremissing=True)
        self.shell.chdir(self.origcwd)

    #
    # attributes:
    #
    
    def getbuildnumber(self):
        return self._buildnumber

    def setbuildnumber(self, buildnumber):
        self._buildnumber = buildnumber

    buildnumber = property(fget=getbuildnumber, fset=setbuildnumber, fdel=None, doc=None)

    #
    # build methods:
    #

    def copymozconfig(self):
        """Copy platform specific .mozconfig-<platform> to .mozconfig"""
        self.logbuildevent("===== Copying .mozconfig...")
        if (self.conf["generate_debug_symbols"] == "True"):
            self.logbuildevent("===== Debug symbols will be generated in this build...")
            self.shell.copy("src/mozilla/" + self.conf["mozconfig_crs"], "src/mozilla/.mozconfig")
        else:
            self.shell.copy("src/mozilla/" + self.conf["mozconfig"], "src/mozilla/.mozconfig")

    def readmozconfig(self):
        """Find out where mozilla is putting the stuff it builds."""
        contents = self.shell.read("src/mozilla/.mozconfig")
        matches = re.search('MOZ_OBJDIR=@TOPSRCDIR@/(.+)', contents)
        return matches.group(1)

    def syncversionheaders(self):
        """Syncs build header files from svn."""
        self.scm.update(self.buildroot + "/src/mozilla/aptana/jaxerBuildId.h")
        self.scm.update(self.buildroot + "/src/mozilla/aptana/Jaxer/Jaxer_prefs.js")
        self.scm.update(self.buildroot + "/src/mozilla/aptana/Jaxer/module.ver")
        self.scm.update(self.buildroot + "/src/connectors/apache/apache-modules/mod_jaxer/mod_jaxer.rc")
        self.scm.update(self.buildroot + "/src/manager/win32/JaxerManager.rc")
        self.scm.update(self.buildroot + "/src/mozilla/aptana/JaxerPro/localinc.h")
        self.scm.update(self.buildroot + "/distro/rpm/SPECS/jaxer.spec")
        #JZ testing
        self.scm.update(self.buildroot + "/src/mozilla/configure")

    def cleanversionheaders(self):
        """Delete builds header files that may have been modified in the last build run, otherwise they may not be updated correctly."""
        self.shell.remove(self.buildroot + "/src/mozilla/aptana/jaxerBuildId.h", ignoremissing=True)
        self.shell.remove(self.buildroot + "/src/mozilla/aptana/Jaxer/Jaxer_prefs.js", ignoremissing=True)
        self.shell.remove(self.buildroot + "/src/mozilla/aptana/Jaxer/module.ver", ignoremissing=True)
        self.shell.remove(self.buildroot + "/src/connectors/apache/apache-modules/mod_jaxer/mod_jaxer.rc", ignoremissing=True)
        self.shell.remove(self.buildroot + "/src/manager/win32/JaxerManager.rc", ignoremissing=True)
        self.shell.remove(self.buildroot + "/src/mozilla/aptana/JaxerPro/localinc.h", ignoremissing=True)
        self.shell.remove(self.buildroot + "/distro/rpm/SPECS/jaxer.spec", ignoremissing=True)
        # JZ testing
        self.shell.remove(self.buildroot + "/src/mozilla/configure", ignoremissing=True)

    def writeversionheaders(self):
        """Write build number and full version into header files."""
        if (self.buildnumber is None):
            return

        # Put the build number into a header file.  Don't touch the header file unless
        # the number actually changes in order to minimize recompilations.
        contents = self.shell.read("src/mozilla/aptana/jaxerBuildId.h")
        matches = re.search(r'#define JAXER_BUILD_ID "([0-9]+)\.([0-9]+)\.([0-9]+)\.([x0-9]+)(.*)"', contents)
        # Save these for later use.  shortversion is already needed by gendoc()
        # REVIEW naming, plus should jaxerBuildId.h define the version or should the build conf?
        self.fullversion = "%s.%s.%s.%d" % (matches.group(1), matches.group(2), matches.group(3), self.buildnumber)
        #JZ shortversion may be removed
        self.shortversion = "%s.%s" % (matches.group(1), matches.group(2))
        self.fullversionstring = "%s.%s.%s.%d%s" % (matches.group(1), matches.group(2), matches.group(3), self.buildnumber, matches.group(5))
        self.fullversioncommas = ",".join(self.fullversion.split("."))
        self.fullversionstringcommas = ",".join(self.fullversionstring.split("."))
        if (str(self.buildnumber) != str(matches.group(4))):
            self.logger.info("Updating version to %s." % self.fullversionstring)
            self.shell.replace('src/mozilla/aptana/jaxerBuildId.h', r'#define JAXER_BUILD_ID "([0-9.]+)\.(.+)"', '#define JAXER_BUILD_ID "%s"' % self.fullversionstring)
            self.shell.replace('src/mozilla/aptana/jaxerBuildId.h', r'#define JAXER_BUILD_NUMBER "xxxx', '#define JAXER_BUILD_NUMBER "%d' % self.buildnumber)
            self.shell.replace('src/mozilla/aptana/jaxerBuildId.h', r'#define JAXER_BUILD_ID_L L"([0-9.]+)\.(.+)"', '#define JAXER_BUILD_ID_L L"%s"' % self.fullversionstring)
            self.shell.replace('src/mozilla/aptana/Jaxer/Jaxer_prefs.js', r'__FULLVERSION__', self.fullversionstring)
            self.shell.replace('src/connectors/apache/apache-modules/mod_jaxer/mod_jaxer.rc', r'X\.X\.X\.X', self.fullversionstring)
            self.shell.replace('src/connectors/apache/apache-modules/mod_jaxer/mod_jaxer.rc', r'X\,X\,X\,X', self.fullversionstringcommas)
            if (aptana.conf.iswin()):
                self.shell.replace('src/mozilla/aptana/Jaxer/module.ver', r'WIN32_MODULE_PRODUCTVERSION\=.*', 'WIN32_MODULE_PRODUCTVERSION=%s' % self.fullversionstringcommas)
                self.shell.replace('src/mozilla/aptana/Jaxer/module.ver', r'WIN32_MODULE_PRODUCTVERSION_STRING\=.*', 'WIN32_MODULE_PRODUCTVERSION_STRING=%s' % self.fullversionstring)
                self.shell.replace('src/mozilla/aptana/manager/module.ver', r'WIN32_MODULE_PRODUCTVERSION\=.*', 'WIN32_MODULE_PRODUCTVERSION=%s' % self.fullversionstringcommas)
                self.shell.replace('src/mozilla/aptana/manager/module.ver', r'WIN32_MODULE_PRODUCTVERSION_STRING\=.*', 'WIN32_MODULE_PRODUCTVERSION_STRING=%s' % self.fullversionstring)
                self.shell.replace('src/connectors/iis/jaxer_isapi/jaxer_isapi.rc', r'X\.X\.X\.X', self.fullversionstring)
                self.shell.replace('src/connectors/iis/jaxer_isapi/jaxer_isapi.rc', r'X\,X\,X\,X', self.fullversionstringcommas)
            if (aptana.conf.islinux()):
                self.shell.replace('distro/rpm/SPECS/jaxer.spec', r'JAXER_BUILD_VERSION', self.fullversion)

        # Regenerate localinc.h
        localinc = "src/mozilla/aptana/JaxerPro/localinc.h"
        tmpfp = open("src/mozilla/aptana/JaxerPro/localinc.h", "w")
        tmpfp.write("#define TIME_NOW %d\n" % time.time())
        tmpfp.close()

    def logbuildevent(self, message):
        self.logger.info(message)
        buildeventstring = "[%s] %s\n" % (time.ctime(), message)
        self._buildevents.append(buildeventstring)

    def cleanjaxer(self):
        """Cleans out generated object code and shared libraries to prepare for a clean build."""
        self.logbuildevent("===== Cleaning Jaxer...")
        self.cleanmozilla()
        self.cleanjs()
        self.cleanconnectors()
        self.cleanlibevent()
        self.cleanlauncher()

    def buildjaxer(self):
        """Builds jaxer and all of it's subcomponents."""
        self.logbuildevent("===== Building Jaxer...")
        self.buildlibevent()
        self.buildmozilla()  
        self.buildconnectors()
        self.buildlauncher()

    def cleanmozilla(self):
        """Cleans mozilla."""
        self.logbuildevent("===== Cleaning mozilla...")
        if (aptana.conf.iswin()):
            bash = aptana.process.Process("bash")
            #bash.arg("--login")
            bash.arg("-c")
            bash.quotearg("cd " + self.mozilladir + " ; ./configure ; make distclean")
            bash.execute(self.executor)
        else:
            configure = aptana.process.Process("./configure")
            configure.cwd = self.mozilladir
            configure.execute(self.executor)

            make = aptana.process.Process("make")
            make.cwd = self.mozilladir
            make.arg("distclean")
            make.execute(self.executor)
        # clean target dir
        self.shell.rmtree("src/mozilla/" + self.ffdir)
                   
    def buildmozilla(self):
        """Builds mozilla."""
        self.logbuildevent("===== Building mozilla...")

        if (aptana.conf.iswin()):
            bash = aptana.process.Process("bash")
            #bash.arg("--login")
            bash.arg("-c")
            bash.quotearg("cd " + self.mozilladir + " ; touch configure && make -f client.mk build")
            bash.execute(self.executor)
        else: # mac, solaris, linux, or generic unix
            self.shell.touch(os.path.join(self.mozilladir, "configure"))

            makecmd = "make"
            if (aptana.conf.issolaris()):
                makecmd = "gmake"
            make = aptana.process.Process(makecmd)
            make.cwd = self.mozilladir
            #JZ make.environment["LD_LIBRARY_PATH"] = "."
            make.arg("-f")
            make.arg("client.mk")
            make.arg("build")
            make.execute(self.executor)

            
        self.shell.mkdirs("src/mozilla/" + self.ffdir + "/dist/bin/connectors")

        # copy ms c runtime libraries
        # NOTE: jemalloc replaces MSVCR (only for Jaxer though)
        #REVIEW: pulling these runtimes from system32 mandates a certain version of windows
        if (aptana.conf.iswin()):
            self.shell.copy("c:/windows/system32/msvcp71.dll",
                            "src/mozilla/" + self.ffdir + "/dist/bin/msvcp71.dll")
            self.shell.copy("c:/windows/system32/msvcr71.dll",
                            "src/mozilla/" + self.ffdir + "/dist/bin/msvcr71.dll")
            self.shell.copy("src/microsoft/msvcr80.dll",
                            "src/mozilla/" + self.ffdir + "/dist/bin/msvcr80.dll")
            self.shell.copy("src/microsoft/msvcp80.dll",
                            "src/mozilla/" + self.ffdir + "/dist/bin/msvcp80.dll")
            self.shell.copy("src/microsoft/msvcm80.dll",
                            "src/mozilla/" + self.ffdir + "/dist/bin/msvcm80.dll")
            self.shell.copy("src/microsoft/Microsoft.VC80.CRT.manifest",
                            "src/mozilla/" + self.ffdir + "/dist/bin/Microsoft.VC80.CRT.manifest")
            self.shell.copy("src/microsoft/msvcr80.dll",
                            "src/mozilla/" + self.ffdir + "/dist/bin/connectors/msvcr80.dll")
            self.shell.copy("src/microsoft/msvcp80.dll",
                            "src/mozilla/" + self.ffdir + "/dist/bin/connectors/msvcp80.dll")
            self.shell.copy("src/microsoft/msvcm80.dll",
                            "src/mozilla/" + self.ffdir + "/dist/bin/connectors/msvcm80.dll")
            self.shell.copy("src/microsoft/Microsoft.VC80.CRT.manifest",
                            "src/mozilla/" + self.ffdir + "/dist/bin/connectors/Microsoft.VC80.CRT.manifest")

    def cleanjs(self):
        """Cleans JavaScript framework."""
        self.logbuildevent("===== Cleaning JavaScript framework...")
        self.shell.remove("framework/buildConfig.js", ignoremissing=True)
        self.shell.remove("framework/proBuildConfig.js", ignoremissing=True)
        self.shell.rmtree(self.framework, ignoreerrors=True)
        self.shell.rmtree(self.proframework, ignoreerrors=True)

    def cleanconnectors(self):
        """Cleans all connectors."""
        self.logbuildevent("===== Cleaning connectors...")
        self.cleanservlet()
        self.cleanmodjaxer()
        self.cleanjaxerisapi()

    def buildconnectors(self):
        """Builds all connectors."""
        self.logbuildevent("===== Building connectors...")
        self.buildservlet()
        self.buildmodjaxer()
        self.buildjaxerisapi()

    def cleanservlet(self):
        """Cleans out generated class and jar/war files."""
        self.logbuildevent("===== Cleaning connector servlet...")
        servlethome = "src/connectors/servlet"
        # delete compiled class files
        for root, dirnames, filenames in os.walk(servlethome):
            for filename in filenames:
                if (filename.endswith(".class")):
                    self.shell.remove(root + os.sep + filename)
        # delete generated jars
        self.shell.remove(servlethome + "/lib/servlet_interfaces.jar", ignoremissing=True)
        self.shell.remove(servlethome + "/lib/servlet_core.jar", ignoremissing=True)
        self.shell.remove(servlethome + "/servlet_interfaces.jar", ignoremissing=True) # artifact
        self.shell.remove(servlethome + "/servlet_core.jar", ignoremissing=True) # artifact
        # delete generated products tree
        self.shell.rmtree(servlethome + "/products", ignoreerrors=True)
        # delete published wars in the dist
        self.shell.remove("src/mozilla/" + self.ffdir + "/dist/bin/connectors/jaxer-app.war", ignoremissing=True)
        self.shell.remove("src/mozilla/" + self.ffdir + "/dist/bin/connectors/jaxer-server.war", ignoremissing=True)

    def buildservlet(self):
        """Builds servlet connector and packages it's components into jar/war files."""
        self.cleanservlet()
        self.logbuildevent("===== Building connector servlet...")
        servlethome = "src/connectors/servlet"

        # Compile the interfaces used by the servlet
        javac = aptana.process.Process("javac")
        javac.cwd = servlethome
        if (self.logger.level == logging.DEBUG):
            javac.arg("-verbose")
        javac.arg("com/aptana/jaxer/connectors/servlet/interfaces/*.java")
        javac.arg("-classpath")
        javac.arg("lib/servlet-api-2.5-6.1.4.jar")
        javac.execute(self.executor)

        # Create an interfaces jar file
        jar = aptana.archive.JarArchiver()
        jar.instrumentation = self.instrumentation
        jar.include("com/aptana/jaxer/connectors/servlet/interfaces/*.class")
        jar.assemble("lib/servlet_interfaces.jar", cwd=servlethome)

        # Compile the filter and servlet classes
        javac = aptana.process.Process("javac")
        javac.cwd = servlethome
        if (self.logger.level == logging.DEBUG):
            javac.arg("-verbose")
        javac.arg("com/aptana/jaxer/connectors/servlet/core/*.java")
        javac.arg("-classpath")
        javac.arg("lib/servlet-api-2.5-6.1.4.jar" + os.path.pathsep + "lib/servlet_interfaces.jar")
        javac.execute(self.executor)

        # Create a servlet core jar file
        jar = aptana.archive.JarArchiver()
        jar.instrumentation = self.instrumentation
        jar.include("com/aptana/jaxer/connectors/servlet/core/*.class")
        jar.assemble("lib/servlet_core.jar", cwd=servlethome)

        # Create the structure for the jaxer-server/callback WAR file
        self.shell.mkdirs(servlethome + "/products/jaxer-server/WEB-INF/lib")
        self.shell.copy(servlethome + "/jaxer-server/web.xml",
                        servlethome + "/products/jaxer-server/WEB-INF")
        self.shell.copy(servlethome + "/lib/servlet_interfaces.jar",
                        servlethome + "/products/jaxer-server/WEB-INF/lib")
        self.shell.copy(servlethome + "/lib/servlet_core.jar",
                        servlethome + "/products/jaxer-server/WEB-INF/lib")

        war = aptana.archive.JarArchiver()
        war.instrumentation = self.instrumentation
        war.include("products/jaxer-server")
        war.assemble("products/jaxer-server.war", cwd=servlethome)

        # Create the structure of the skeleton jaxer app WAR file
        self.shell.mkdirs(servlethome + "/products/jaxer-app/WEB-INF/lib")
        self.shell.copy(servlethome + "/jaxer-app/web.xml",
                        servlethome + "/products/jaxer-app/WEB-INF")
        self.shell.copy(servlethome + "/lib/servlet_interfaces.jar",
                        servlethome + "/products/jaxer-app/WEB-INF/lib")
        self.shell.copy(servlethome + "/lib/servlet_core.jar",
                        servlethome + "/products/jaxer-app/WEB-INF/lib")

        war = aptana.archive.JarArchiver()
        war.instrumentation = self.instrumentation
        war.include("products/jaxer-app")
        war.assemble("products/jaxer-app.war", cwd=servlethome)

        self.shell.copy(servlethome + "/products/jaxer-app.war",
                        "src/mozilla/" + self.ffdir + "/dist/bin/connectors")
        self.shell.copy(servlethome + "/products/jaxer-server.war",
                        "src/mozilla/" + self.ffdir + "/dist/bin/connectors")

    def cleanmodjaxer(self):
        """Cleans out mod_jaxer generated object code and shared libraries."""
        self.logbuildevent("===== Cleaning mod_jaxer...")

        self.shell.remove("src/connectors/apache/apache-modules/mod_jaxer/Release/mod_jaxer.so", ignoremissing=True)
        self.shell.remove("src/connectors/apache/apache-modules/mod_jaxer/Release20/mod_jaxer20.so", ignoremissing=True)
        #self.shell.remove("src/connectors/apache/apache-modules/mod_jaxer/.libs/mod_jaxer.so", ignoremissing=True)
        self.shell.rmtree("src/connectors/apache/apache-modules/mod_jaxer/.libs", ignoreerrors=True)
        for root, dirnames, filenames in os.walk("src/connectors/apache/apache-modules/mod_jaxer"):
            for filename in filenames:
                if (filename.endswith(".o") or filename.endswith(".so") or filename.endswith(".slo") or filename.endswith(".lo") or filename.endswith(".la")):
                    self.shell.remove(root + os.sep + filename)
        #TODO: clean Release20 on win32
        self.shell.remove("src/mozilla/" + self.ffdir + "/dist/bin/connectors/mod_jaxer.so", ignoremissing=True)
        self.shell.remove("src/mozilla/" + self.ffdir + "/dist/bin/connectors/mod_jaxer20.so", ignoremissing=True)

    def buildmodjaxer(self):
        """Build mod_jaxer."""
        self.logbuildevent("===== Building mod_jaxer...")

        if (aptana.conf.iswin()):
            msbuild = aptana.process.Process("MSBuild")
            msbuild.cwd = "src/connectors/apache/apache-modules/mod_jaxer"
            msbuild.arg("mod_jaxer.vcproj")
            msbuild.execute(self.executor)

            self.shell.copy("src/connectors/apache/apache-modules/mod_jaxer/Release/mod_jaxer.so",
                            "src/mozilla/" + self.ffdir + "/dist/bin/connectors/mod_jaxer.so")
            # Build mod_jaxer20 (for apache 2.0).
            #self.logger.info("\n===== Building mod_jaxer20.")
            #self.executor.execute('cd src/connectors/apache/apache-modules/mod_jaxer && nmake /f Makefile.win _apache20r')
            self.shell.copy("src/connectors/apache/apache-modules/mod_jaxer/Release20/mod_jaxer20.so",
                            "src/mozilla/" + self.ffdir + "/dist/bin/connectors/mod_jaxer20.so")
        elif (aptana.conf.ismac()):
            #REVIEW: buildMacUniversal.sh expects apxs in a previously built distribution
            #bash = aptana.process.Process("bash")
            #bash.cwd = "src/connectors/apache/apache-modules/mod_jaxer"
            #bash.arg("./buildMacUniversal.sh")
            #bash.execute(self.executor)

            apxs = aptana.process.Process(self.conf["apxs_name"]) #os.path.abspath("src/httpd-2.2.6/support/apxs"))
            apxs.cwd = "src/connectors/apache/apache-modules/mod_jaxer"
            apxs.arg("-Wc,-arch")
            apxs.arg("-Wc,i386")
            apxs.arg("-Wl,-arch")
            apxs.arg("-Wl,i386")
            apxs.arg("-c")
            apxs.arg("-Wc,-arch")
            apxs.arg("-Wc,x86_64")
            apxs.arg("-Wl,-arch")
            apxs.arg("-Wl,x86_64")
            
            apxs.arg("-c")
            apxs.arg("-Wc,-arch")
            apxs.arg("-Wc,ppc")
            apxs.arg("-Wl,-arch")
            apxs.arg("-Wl,ppc")
            
            apxs.arg("-c")
            apxs.arg("-Wc,-arch")
            apxs.arg("-Wc,ppc64")
            apxs.arg("-Wl,-arch")
            apxs.arg("-Wl,ppc64")
            apxs.arg("-c")
            apxs.arg("*.c")
            apxs.execute(self.executor)

            self.shell.copy("src/connectors/apache/apache-modules/mod_jaxer/.libs/mod_jaxer.so",
                            "src/mozilla/" + self.ffdir + "/dist/bin/connectors/mod_jaxer.so")
        else: # linux, solaris, or generic unix
            apxs = aptana.process.Process(self.conf["apxs_name"])
            apxs.cwd = "src/connectors/apache/apache-modules/mod_jaxer"
            apxs.arg("-c")
            apxs.arg("*.c")
            apxs.execute(self.executor)

            self.shell.copy("src/connectors/apache/apache-modules/mod_jaxer/.libs/mod_jaxer.so",
                            "src/mozilla/" + self.ffdir + "/dist/bin/connectors/mod_jaxer.so")

    def cleanjaxerisapi(self):
        """Cleans out jaxer_isapi generated object code and shared libraries."""
        self.logbuildevent("===== Cleaning jaxer_isapi...")

        #self.shell.remove("src/connectors/iis/jaxer_isapi/Release/jaxer_isapi.dll", ignoremissing=True)
        self.shell.remove("src/mozilla/" + self.ffdir + "/dist/bin/connectors/jaxer_isapi.dll", ignoremissing=True)
        self.shell.remove("src/mozilla/" + self.ffdir + "/dist/bin/connectors/jaxer_isapi.conf", ignoremissing=True)
        self.shell.remove("src/mozilla/" + self.ffdir + "/dist/bin/connectors/jaxer_isapi_readme.txt", ignoremissing=True)

    def buildjaxerisapi(self):
        """Build jaxer_isapi."""
        if (not aptana.conf.iswin()):
            return
        self.logbuildevent("===== Building jaxer_isapi...")

        nmake = aptana.process.Process("nmake")
        nmake.cwd = "src/connectors/iis/jaxer_isapi"
        nmake.arg("/f")
        nmake.arg("Makefile.win")
        nmake.execute(self.executor)

        self.shell.copy("src/connectors/iis/jaxer_isapi/Release/jaxer_isapi.dll",
                        "src/mozilla/" + self.ffdir + "/dist/bin/connectors/jaxer_isapi.dll")
        self.shell.copy("src/connectors/iis/jaxer_isapi/jaxer_isapi.conf",
                        "src/mozilla/" + self.ffdir + "/dist/bin/connectors/jaxer_isapi.conf")
        self.shell.copy("src/connectors/iis/jaxer_isapi/ReadMe.txt",
                        "src/mozilla/" + self.ffdir + "/dist/bin/connectors/jaxer_isapi_readme.txt")

    def cleanlibevent(self):
        """Cleans out libevent object code and shared libraries."""
        if (aptana.conf.iswin()):
            return
        self.logbuildevent("===== Cleaning libevent...")

        make = aptana.process.Process("make")
        make.cwd = "src/libevent/libevent-1.4.2-rc"
        make.arg("clean-recursive")
        make.execute(self.executor)

    def buildlibevent(self):
        """Builds libevent."""
        if (aptana.conf.iswin()):
            return
        self.logbuildevent("===== Building libevent...")

        configure = aptana.process.Process("./configure")
        configure.cwd = "src/libevent/libevent-1.4.2-rc"
        if (aptana.conf.ismac()):
            configure.environment["CFLAGS"] = "-arch ppc -arch i386"
        configure.execute(self.executor)

        make = aptana.process.Process("make")
        make.cwd = "src/libevent/libevent-1.4.2-rc"
        if (aptana.conf.ismac()):
            make.environment["CFLAGS"] = "-arch ppc -arch i386"
        make.execute(self.executor)

    def cleanlauncher(self):
        """Cleans out jaxer launcher app."""
        self.logbuildevent("===== Cleaning Jaxer Launcher...")
        self.shell.rmtree("src/launcher/mac/build", ignoreerrors=True)

    def buildlauncher(self):
        """Build Jaxer Launcher."""
        if (not aptana.conf.ismac()):
            return
        self.logbuildevent("===== Building Jaxer Launcher...")
        xcodebuild = aptana.process.Process("xcodebuild")
        xcodebuild.cwd = "src/launcher/mac"
        xcodebuild.execute(self.executor)

    def cleanall(self):
        """Cleans out all files generated by the build process."""
        self.cleandist()
        self.cleanjaxer()

    def buildall(self):
        """Does an incremental build."""
        self.cleandist()
        self.cleanselected()
        self.cleanconnectors()
        self.writeversionheaders()
        self.buildjaxer()
        self.packdist()
        
        if (self.conf["pack_jaxerpro"] == "True"):
            self.logbuildevent("===== JaxerPro packages will be generated.")
            self.packprodist()

    def rebuildall(self):
        """Does a cleanall then buildall to complete a full build."""
        self.logbuildevent("===== This will be a clean build")
        self.cleanall()
        self.buildall()

    def cleandoc(self):
        """Remove any old documentation."""
        self.logbuildevent("===== Cleaning documentation from: " + self.docroot)
        self.shell.rmtree(self.docroot, ignoreerrors=True)

    def syncdoc(self):
        """Syncs latest docgen libraries from svn."""
        self.scm.cleanup(self.docgenroot)
        self.scm.update(self.docgenroot)

    def cleandist(self):
        """Cleans out packaged jam distributions."""
        self.logbuildevent("===== Cleaning jam distributions...")
        self.shell.rmtree("jam", ignoreerrors=True) # remove jam, this should normally have been renamed to the install dir
        self.shell.rmtree("projam", ignoreerrors=True)

        self.shell.rmtree(self.topdir, ignoreerrors=True)
        self.shell.remove("distro/Jaxer_package_withApache.zip", ignoremissing=True)
        self.shell.remove("distro/Jaxer_package_withApache.tar.gz", ignoremissing=True)
        self.shell.remove("distro/Jaxer_package_withApache.dmg", ignoremissing=True)
        self.shell.remove("distro/Jaxer_package.zip", ignoremissing=True)
        self.shell.remove("distro/Jaxer_package.tar.gz", ignoremissing=True)
        self.shell.remove("distro/Jaxer_update.zip", ignoremissing=True)
        self.shell.remove("distro/Jaxer_update.tar.gz", ignoremissing=True)
        self.shell.remove("distro/Jaxer.rpm", ignoremissing=True)
        self.shell.remove("distro/Jaxer.deb", ignoremissing=True)

        self.shell.remove("distro/JaxerPro_package_withApache.zip", ignoremissing=True)
        self.shell.remove("distro/JaxerPro_package_withApache.tar.gz", ignoremissing=True)
        self.shell.remove("distro/JaxerPro_package_withApache.dmg", ignoremissing=True)
        self.shell.remove("distro/JaxerPro_package.zip", ignoremissing=True)
        self.shell.remove("distro/JaxerPro_package.tar.gz", ignoremissing=True)
        self.shell.remove("distro/JaxerPro_update.zip", ignoremissing=True)
        self.shell.remove("distro/JaxerPro_update.tar.gz", ignoremissing=True)

        self.cleandoc()
        
    def cleanselected(self):
        """Cleans out certain build results needed."""
        self.logbuildevent("===== Cleaning certain folders...")
        self.shell.rmtree("src/mozilla/" + self.ffdir + "/dist/bin", ignoreerrors=True)
        self.shell.rmtree("src/mozilla/" + self.ffdir + "/i386/dist/bin", ignoreerrors=True)
        self.shell.rmtree("src/mozilla/" + self.ffdir + "/ppc/dist/bin", ignoreerrors=True)
        self.shell.rmtree("src/mozilla/" + self.ffdir + "/dist/crashreporter-symbols/", ignoreerrors=True)
        self.shell.rmtree("src/mozilla/" + self.ffdir + "/i386/dist/crashreporter-symbols/", ignoreerrors=True)
        self.shell.rmtree("src/mozilla/" + self.ffdir + "/ppc/dist/crashreporter-symbols/", ignoreerrors=True)
        if (os.path.exists("src/mozilla/" + self.ffdir)):
            self.shell.rmtreebyctime("src/mozilla/" + self.ffdir)   

    def packdist(self):
        """Package jam distribution."""
        self.logbuildevent("===== Packaging jam distributions...")

        # First clone the distribution in svn (minus the .svn directories) to produce a master.
        self.shell.rmtree("jam", ignoreerrors=True)

        self.logbuildevent("  === Copying data from distro/jam...")
        for root, dirs, files in aptana.scm.scmwalk("distro/jam"):
            matches = re.search('distro[\\\/]+(.+)', root)
            destdir = matches.group(1)
            self.logger.debug("destdir is %s" % (destdir))
            self.shell.mkdirs(destdir)

            for file in files:
                self.shell.copy(os.path.join(root, file), os.path.join(destdir, file))

            # We don"t ship MySQL any more in any package.
            if ("mysql" in dirs):
                dirs.remove("mysql")

            if (not aptana.conf.issolaris()) and ("Apache22-solaris" in dirs):
                self.logbuildevent("  === Removing Apache22-solaris...")
                dirs.remove("Apache22-solaris")

            if (not aptana.conf.ismac()) and ("Apache22-mac-universal" in dirs):
                self.logbuildevent("  === Removing Apache22-mac-universal...")
                dirs.remove("Apache22-mac-universal")

            if (not aptana.conf.iswin()) and ("Apache22" in dirs):
                self.logbuildevent("  === Removing Apache22...")
                dirs.remove("Apache22")

            if (not aptana.conf.islinux()) and ("Apache22-cent32" in dirs):
                self.logbuildevent("  === Removing Apache22-cent32...")
                dirs.remove("Apache22-cent32")

        # Now copy the built stuff into the master clone.
        ffbasedir = os.path.join("src", "mozilla", self.ffdir, "dist", "bin")
        jamjaxerbasedir = os.path.join("jam","jaxer")
        self.logbuildevent("  === Copying data from %s to %s..." % (ffbasedir, jamjaxerbasedir))
        for root, dirs, files in aptana.scm.scmwalk(ffbasedir):
            destdir = root.replace(ffbasedir, jamjaxerbasedir)
            self.shell.mkdirs(destdir, ignoreexisting=True)

            # special case for the root
            if (os.path.abspath(destdir) == os.path.abspath(jamjaxerbasedir)):
                if (aptana.conf.iswin()):
                    self.logbuildevent("   == Win32 removing firefox");
                    if ("firefox.exe" in files):
                        files.remove("firefox.exe")
                    if ("JaxerPro.exe" in files):
                        files.remove("JaxerPro.exe")
                else:
                    self.logbuildevent("   == non-Win32 removing firefox");
                    if ("firefox" in files):
                        files.remove("firefox")
                    if ("firefox-bin" in files):
                        files.remove("firefox-bin")
                    if ('jaxerpro' in files):
                        self.logbuildevent("   == non-Win32 removing jaxerpro");
                        files.remove('jaxerpro')
                    if 'JaxerPro' in files:
                        self.logbuildevent("   == non-Win32 removing JaxerPro");
                        files.remove('JaxerPro')
                    # make all files in dist/bin executable: (jaxer, jaxermanager, telljaxermanager, jaxerlogger)
                    for file in files:
                        self.shell.chmod(os.path.join(root, file), 0755)
                if ("chrome" in dirs):
                    self.logbuildevent("   == removing dir chrome");
                    dirs.remove("chrome")
                if ("proframework" in dirs):
                    self.logbuildevent("   == removing dir proframework");
                    dirs.remove("proframework")
                if ("dictionaries" in dirs):
                    dirs.remove("dictionaries")
                if ("plugins" in dirs):
                    dirs.remove("plugins")
                if ("searchplugins" in dirs):
                    dirs.remove("searchplugins")

            # copy all files to jam/jaxer
            for file in files:
                self.shell.copy(os.path.join(root, file), os.path.join(destdir, file))

        self.logbuildevent("  === Renaming/Removing extra stuff...")
        if (aptana.conf.iswin()):
            pass
        elif (aptana.conf.ismac()):
            self.logbuildevent("   == Renaming jam/Apache22-mac-universal to jam/Apache22")
            self.shell.rename("jam/Apache22-mac-universal", "jam/Apache22")
            self.logbuildevent("   == Extra processing for macos...")
            self.shell.scmcopytree("src/launcher/mac/build/Default/Jaxer Launcher.app", "jam/Jaxer Launcher.app")
            self.shell.scmcopytree("src/scripts/unix", "jam/scripts")
            self.shell.remove("jam/StartServers.bat")
            self.shell.remove("jam/ConfigureFirewall.exe")
        elif (aptana.conf.issolaris()):
            self.logbuildevent("   == Renaming jam/Apache22-local to jam/Apache22")
            self.shell.rename("jam/Apache22-local", "jam/Apache22")
            self.shell.scmcopytree("src/scripts/solaris", "jam/scripts")
            self.shell.remove("jam/StartServers.bat")
            self.shell.remove("jam/ConfigureFirewall.exe")
        else: # linux or generic unix
            self.logbuildevent("   == Renaming jam/Apache22-local to jam/Apache22")
            self.shell.rename("jam/Apache22-local", "jam/Apache22")
            self.shell.scmcopytree("src/scripts/unix", "jam/scripts")
            self.shell.remove("jam/StartServers.bat")
            self.shell.remove("jam/ConfigureFirewall.exe")

        self.logbuildevent("  === Copying chrome (should be reviewed/removed in the future)...")
        self.shell.copy("src/mozilla/" + self.ffdir +"/dist/bin/chrome/en-US.manifest",
                        "jam/jaxer/chrome/en-US.manifest")
        self.shell.copy("src/mozilla/" + self.ffdir +"/dist/bin/chrome/en-US.jar",
                        "jam/jaxer/chrome/en-US.jar")

        # Create subset distributions and zip them.
        self.logbuildevent("===== Creating and zipping distributions...")

        self.shell.rmtree(self.topdir, ignoreerrors=True)
        self.shell.rename("jam", self.topdir)

        if (aptana.conf.ismac()):
            self.logbuildevent("===== Building Jaxer_package_withApache.dmg...")
            dmgbuilder = aptana.process.Process("./dmgbuilder")
            dmgbuilder.cwd = "dmgbuilder"
            dmgbuilder.arg("dmgbuild.config")
            dmgbuilder.execute(self.executor)
        else:
            self.logbuildevent("===== Building Jaxer_package_withApache.zip...")
            zip = aptana.archive.ZipArchiver(os.path.join("distro", "Jaxer_package_withApache.zip"))
            zip.instrumentation = self.instrumentation
            zip.includeall(self.topdir)
            zip.assemble()
            if (not aptana.conf.iswin()):
                tar = aptana.archive.TarArchiver("distro/Jaxer_package_withApache.tar.gz")
                tar.instrumentation = self.instrumentation
                tar.includeall(self.topdir)
                tar.assemble()

            # purge apache from the jaxer package
            self.logbuildevent("===== Building Jaxer_package.zip...")
            self.shell.rmtree(os.path.join(self.topdir, "Apache22"), ignoreerrors=True)

            zip = aptana.archive.ZipArchiver(os.path.join("distro", "Jaxer_package.zip"))
            zip.instrumentation = self.instrumentation
            zip.includeall(self.topdir)
            zip.assemble()
            if (not aptana.conf.iswin()):
                tar = aptana.archive.TarArchiver("distro/Jaxer_package.tar.gz")
                tar.instrumentation = self.instrumentation
                tar.includeall(self.topdir)
                tar.assemble()

        self.logbuildevent("===== Building Jaxer_update.zip...")
        #REVIEW: Jaxer_update.zip has root of ./jaxer/ instead of ./AptanaJaxer/jaxer
        zip = aptana.archive.ZipArchiver(os.path.join("distro", "Jaxer_update.zip"))
        zip.instrumentation = self.instrumentation
        zip.exclude(".svn")
        zip.includeall("jaxer", cwd=self.topdir)
        zip.assemble()
        if (not aptana.conf.iswin()):
            tar = aptana.archive.TarArchiver("distro/Jaxer_update.tar.gz")
            tar.instrumentation = self.instrumentation
            tar.exclude(".svn")
            tar.includeall("jaxer", cwd=self.topdir)
            tar.assemble()

        #Build Jaxer rpm package
        self.logbuildevent("===== Build Jaxer rpm...")
        if (aptana.conf.islinux()):
            self.logger.info("rpm: linux, rpm_root_dir=%s" % (self.conf["rpm_build_root"]))
            rpm_root_dir = self.conf["rpm_root"]
            rpm_opt_dir = os.path.join(self.conf["rpm_build_root"], "opt/AptanaJaxer")

            # clear certain ffolders
            self.shell.rmtree(rpm_opt_dir, ignoreerrors=True)
            self.shell.rmtree(os.path.join(rpm_root_dir, "RPMS/i386"), ignoreerrors=True)
            self.shell.rmtree(os.path.join(rpm_root_dir, "RPMS/x86_64"), ignoreerrors=True)
            self.logger.info("rpm: opt dir=%s" % (rpm_opt_dir))
            self.shell.mkdirs(rpm_opt_dir)
            self.shell.mkdirs(os.path.join(rpm_root_dir, "RPMS/i386"))
            self.shell.mkdirs(os.path.join(rpm_root_dir, "RPMS/x86_64"))

            self.shell.scmcopytree(self.topdir, rpm_opt_dir)
            self.logger.info("rpm: copied %s to %s" % (self.topdir, rpm_opt_dir))

            rpmbuild = aptana.process.Process("rpmbuild")
            rpmbuild.cwd = self.mozilladir
            rpmbuild.arg("-bb")
            rpmbuild.arg("--buildroot")
            rpmbuild.arg(self.conf["rpm_build_root"])
            rpmbuild.arg(self.conf["rpm_spec"])
            rpmbuild.execute(self.executor)
            self.logger.info("rpm: rpmbnuild done")
            if (aptana.conf.osbits() == 32):
                rpmfile = "jaxer-" + self.fullversion + "-1.i386.rpm"
                self.logger.info("rpm: 32: rpmfile=%s" % (rpmfile))
                self.shell.copy(os.path.join(self.conf["rpm_root"], "RPMS/i386", rpmfile), "distro/Jaxer.rpm")
            else:
                rpmfile = "jaxer-" + self.fullversion + "-1.x86_64.rpm"
                self.logger.info("rpm: 64: rpmfile=%s" % (rpmfile))
                self.shell.copy(os.path.join(self.conf["rpm_root"], "RPMS/x86_64", rpmfile), "distro/Jaxer.rpm")
            self.logger.info("rpm: end rpm build")

        #########STT DEB BUILD##########
        #Build Jaxer debian package
        self.logbuildevent("===== Build Jaxer debian package...")
        if (aptana.conf.islinux()):

            if (aptana.conf.osbits() == 32):
                deb_arch = "i386"
            else:
                deb_arch = "amd64"

            # Build the package
            self.logger.info("deb: clean build")
            debbuild = aptana.process.Process("fakeroot")
            debbuild.cwd = "distro"
            debbuild.arg("./debian/rules")
            debbuild.arg("JAM_DIR=../" + self.topdir)
            debbuild.arg("JAXER_BUILD_ID=" + self.fullversion)
            debbuild.arg("DEB_ARCH=" + deb_arch)
            debbuild.arg("clean")
            debbuild.arg("binary")
            debbuild.execute(self.executor)
            self.logger.info("deb: deb build done.")
        #########END DEB BUILD##########

        # leave installdir, it's useful for debugging and will be cleaned on the next run anyway
        #self.shell.rmtree(self.topdir, ignoreerrors=True)
        # we need the slot for late use.
        self.shell.rename(self.topdir, "jam")

    def packprodist(self):
        """Package pro jam distribution."""
        self.logbuildevent("===== Packaging pro jam distributions...")

        # First clone the distribution in svn (minus the .svn directories) to produce a master.
        self.shell.rmtree("projam", ignoreerrors=True)

        self.logbuildevent("  === Copying data from distro/jam...")
        for root, dirs, files in aptana.scm.scmwalk("distro/jam"):
            matches = re.search('distro[\\\/]+(.+)', root)
            destdir = "pro" + matches.group(1)
            self.logger.debug("destdir is %s" % (destdir))
            self.shell.mkdirs(destdir)

            for file in files:
                self.shell.copy(os.path.join(root, file), os.path.join(destdir, file))

            # We don"t ship MySQL any more in any package.
            if ("mysql" in dirs):
                dirs.remove("mysql")

            if (not aptana.conf.issolaris()) and ("Apache22-solaris" in dirs):
                self.logbuildevent("  === Removing Apache22-solaris...")
                dirs.remove("Apache22-solaris")

            if (not aptana.conf.ismac()) and ("Apache22-mac-universal" in dirs):
                self.logbuildevent("  === Removing Apache22-mac-universal...")
                dirs.remove("Apache22-mac-universal")

            if (not aptana.conf.iswin()) and ("Apache22" in dirs):
                self.logbuildevent("  === Removing Apache22...")
                dirs.remove("Apache22")

            if (not aptana.conf.islinux()) and ("Apache22-cent32" in dirs):
                self.logbuildevent("  === Removing Apache22-cent32...")
                dirs.remove("Apache22-cent32")

        # Now copy the built stuff into the master clone.
        ffbasedir = os.path.join("src", "mozilla", self.ffdir, "dist", "bin")
        jamjaxerbasedir = os.path.join("projam","jaxer")
        self.logbuildevent("  === Copying data from %s to %s..." % (ffbasedir, jamjaxerbasedir))
        for root, dirs, files in aptana.scm.scmwalk(ffbasedir):
            destdir = root.replace(ffbasedir, jamjaxerbasedir)
            self.shell.mkdirs(destdir, ignoreexisting=True)

            # special case for the root
            if (os.path.abspath(destdir) == os.path.abspath(jamjaxerbasedir)):
                if (aptana.conf.iswin()):
                    self.logbuildevent("   == Win32 removing firefox");
                    if ("firefox.exe" in files):
                        files.remove("firefox.exe")
                    if ("Jaxer.exe" in files):
                        files.remove("Jaxer.exe")
                else:
                    self.logbuildevent("   == non-Win32 removing firefox");
                    if ("firefox" in files):
                        files.remove("firefox")
                    if ("firefox-bin" in files):
                        files.remove("firefox-bin")
                    if ('jaxer' in files):
                        self.logbuildevent("   == non-Win32 removing jaxer");
                        files.remove('jaxer')
                    if 'Jaxer' in files:
                        self.logbuildevent("   == non-Win32 removing Jaxer");
                        files.remove('Jaxer')
                    # make all files in dist/bin executable: (jaxer, jaxermanager, telljaxermanager, jaxerlogger)
                    for file in files:
                        self.shell.chmod(os.path.join(root, file), 0755)
                if ("chrome" in dirs):
                    self.logbuildevent("   == removing dir chrome");
                    dirs.remove("chrome")
                if ("dictionaries" in dirs):
                    dirs.remove("dictionaries")
                if ("plugins" in dirs):
                    dirs.remove("plugins")
                if ("searchplugins" in dirs):
                    dirs.remove("searchplugins")

            # copy all files to jam/jaxer
            for file in files:
                self.shell.copy(os.path.join(root, file), os.path.join(destdir, file))

        self.logbuildevent("  === Renaming/Removing extra stuff...")
        if (aptana.conf.iswin()):
            self.logbuildevent("   == Renaming projam/jaxer/JaxerPro.exe to projam/jaxer/Jaxer.exe")
            self.shell.rename("projam/jaxer/JaxerPro.exe", "projam/jaxer/Jaxer.exe")
        elif (aptana.conf.ismac()):
            self.logbuildevent("   == Renaming projam/Apache22-mac-universal to projam/Apache22")
            self.shell.rename("projam/Apache22-mac-universal", "projam/Apache22")
            self.logbuildevent("   == Renaming projam/jaxer/JaxerPro to projam/jaxer/Jaxer")
            self.shell.rename("projam/jaxer/JaxerPro", "projam/jaxer/Jaxer")
            self.logbuildevent("   == Extra processing for macos...")
            self.shell.scmcopytree("src/launcher/mac/build/Default/Jaxer Launcher.app", "projam/Jaxer Launcher.app")
            self.shell.scmcopytree("src/scripts/unix", "projam/scripts")
            self.shell.remove("projam/StartServers.bat")
            self.shell.remove("projam/ConfigureFirewall.exe")
        elif (aptana.conf.issolaris()):
            self.logbuildevent("   == Renaming projam/Apache22-local to projam/Apache22")
            self.shell.rename("projam/Apache22-local", "projam/Apache22")
            self.logbuildevent("   == Renaming projam/jaxer/jaxerpro to projam/jaxer/jaxer")
            self.shell.rename("projam/jaxer/jaxerpro", "projam/jaxer/jaxer")
            self.shell.scmcopytree("src/scripts/solaris", "projam/scripts")
            self.shell.remove("projam/StartServers.bat")
            self.shell.remove("projam/ConfigureFirewall.exe")
        else: # linux or generic unix
            self.logbuildevent("   == Renaming projam/Apache22-local to projam/Apache22")
            self.shell.rename("projam/Apache22-local", "projam/Apache22")
            self.logbuildevent("   == Renaming projam/jaxer/jaxerpro to projam/jaxer/jaxer")
            self.shell.rename("projam/jaxer/jaxerpro", "projam/jaxer/jaxer")
            self.shell.scmcopytree("src/scripts/unix", "projam/scripts")
            self.shell.remove("projam/StartServers.bat")
            self.shell.remove("projam/ConfigureFirewall.exe")

        self.logbuildevent("  === Copying chrome (should be reviewed/removed in the future)...")
        self.shell.copy("src/mozilla/" + self.ffdir +"/dist/bin/chrome/en-US.manifest",
                        "projam/jaxer/chrome/en-US.manifest")
        self.shell.copy("src/mozilla/" + self.ffdir +"/dist/bin/chrome/en-US.jar",
                        "projam/jaxer/chrome/en-US.jar")

        # copy Pro license file
        self.shell.copy("src/aptana_private/JaxerPro/LICENSE-JAXER.html",
                         "projam/jaxer/LICENSE-JAXER.html")

        # overlay proframework content on top of framework, and delete proframework
        #for name in glob.glob('projam/jaxer/proframework/*'):
        #    self.shell.copy(name, 'projam/jaxer/framework/')
            
        #for name in glob.glob('projam/jaxer/proframework/src/*'):
        #    self.shell.copy(name, 'projam/jaxer/framework/src/')
        
        self.shell.rmtree("projam/jaxer/framework/src", ignoreerrors=True)
        self.shell.scmcopytree("projam/jaxer/proframework", "projam/jaxer/framework")
            
        self.shell.rmtree("projam/jaxer/proframework", ignoreerrors=True)
        
        # Create subset distributions and zip them.
        self.logbuildevent("===== Creating and zipping distributions...")

        self.shell.rmtree(self.topdir, ignoreerrors=True)
        self.shell.rename("projam", self.topdir)

        if (aptana.conf.ismac()):
            self.logbuildevent("===== Building JaxerPro_package_withApache.dmg...")
            dmgbuilder = aptana.process.Process("./dmgbuilder")
            dmgbuilder.cwd = "dmgbuilder"
            dmgbuilder.arg("dmgprobuild.config")
            dmgbuilder.execute(self.executor)
        else:
            self.logbuildevent("===== Building JaxerPro_package_withApache.zip...")
            zip = aptana.archive.ZipArchiver(os.path.join("distro", "JaxerPro_package_withApache.zip"))
            zip.instrumentation = self.instrumentation
            zip.includeall(self.topdir)
            zip.assemble()
            if (not aptana.conf.iswin()):
                tar = aptana.archive.TarArchiver("distro/JaxerPro_package_withApache.tar.gz")
                tar.instrumentation = self.instrumentation
                tar.includeall(self.topdir)
                tar.assemble()

            # purge apache from the jaxer package
            self.logbuildevent("===== Building JaxerPro_package.zip...")
            self.shell.rmtree(os.path.join(self.topdir, "Apache22"), ignoreerrors=True)

            zip = aptana.archive.ZipArchiver(os.path.join("distro", "JaxerPro_package.zip"))
            zip.instrumentation = self.instrumentation
            zip.includeall(self.topdir)
            zip.assemble()
            if (not aptana.conf.iswin()):
                tar = aptana.archive.TarArchiver("distro/JaxerPro_package.tar.gz")
                tar.instrumentation = self.instrumentation
                tar.includeall(self.topdir)
                tar.assemble()

        self.logbuildevent("===== Building JaxerPro_update.zip...")
        #REVIEW: Jaxer_update.zip has root of ./jaxer/ instead of ./AptanaJaxer/jaxer
        zip = aptana.archive.ZipArchiver(os.path.join("distro", "JaxerPro_update.zip"))
        zip.instrumentation = self.instrumentation
        zip.exclude(".svn")
        zip.includeall("jaxer", cwd=self.topdir)
        zip.assemble()
        if (not aptana.conf.iswin()):
            tar = aptana.archive.TarArchiver("distro/JaxerPro_update.tar.gz")
            tar.instrumentation = self.instrumentation
            tar.exclude(".svn")
            tar.includeall("jaxer", cwd=self.topdir)
            tar.assemble()

        # leave installdir, it's useful for debugging and will be cleaned on the next run anyway
        #self.shell.rmtree(self.topdir, ignoreerrors=True)
        self.shell.rename(self.topdir, "projam")

    def copydist(self, failed=False):
        """Copy distribution to file server."""
        self.transferdist(scheme="file", failed=failed)

    def ftpdist(self, failed=False):
        """FTP distribution to ftp server."""
        self.transferdist(scheme="ftp", failed=failed)

    def transferdist(self, scheme="ftp", failed=False):
        """Transfer distribution to a server specifiying the transport protocol."""
        if (self.conf["jam_" + scheme + "_enabled"] != "True"):
            return

        # Get build repository URL's.
        buildlatesturl  = self.conf["jam_" + scheme + "_buildlatesturl"]
        buildarchiveurl = self.conf["jam_" + scheme + "_buildarchiveurl"]
        # Determine package extension for current platform.
        fullpackageextension = "zip"
        if (aptana.conf.ismac()):
            fullpackageextension = "dmg"
        # Add postfix to files if unit tests failed.
        filepostfix = ""
        if (failed):
            filepostfix = "_failed"

        # Create protocol client.
        client = aptana.net.createclient(scheme + ":")
        client.instrumentation = self.instrumentation

        # Remove previous "latest" failed zip files, if they exist.
        if (not failed):
            client.delete("%s/Jaxer_package_withApache_failed.zip" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/Jaxer_package_withApache_failed.tar.gz" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/Jaxer_package_withApache_failed.dmg" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/Jaxer_package_failed.zip" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/Jaxer_package_failed.tar.gz" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/Jaxer_update_failed.zip" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/Jaxer_update_failed.tar.gz" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/Jaxer_failed.rpm" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/Jaxer_failed.deb" % (buildlatesturl), ignoremissing=True)

            # PRO
            client.delete("%s/JaxerPro_package_withApache_failed.zip" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/JaxerPro_package_withApache_failed.tar.gz" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/JaxerPro_package_withApache_failed.dmg" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/JaxerPro_package_failed.zip" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/JaxerPro_package_failed.tar.gz" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/JaxerPro_update_failed.zip" % (buildlatesturl), ignoremissing=True)
            client.delete("%s/JaxerPro_update_failed.tar.gz" % (buildlatesturl), ignoremissing=True)

        # Put full distribution.
        client.put("%s/distro/Jaxer_package_withApache.%s" % (self.buildroot, fullpackageextension),
                   "%s/Jaxer_package_withApache%s.%s" % (buildlatesturl, filepostfix, fullpackageextension))
        client.put("%s/distro/Jaxer_package_withApache.%s" % (self.buildroot, fullpackageextension),
                   "%s/Jaxer_package_withApache_%s%s.%s" % (buildarchiveurl, self.buildnumber, filepostfix, fullpackageextension))
        # Pro
        if (self.conf["pack_jaxerpro"] == "True"):
            client.put("%s/distro/JaxerPro_package_withApache.%s" % (self.buildroot, fullpackageextension),
                   "%s/JaxerPro_package_withApache%s.%s" % (buildlatesturl, filepostfix, fullpackageextension))
            client.put("%s/distro/JaxerPro_package_withApache.%s" % (self.buildroot, fullpackageextension),
                   "%s/JaxerPro_package_withApache_%s%s.%s" % (buildarchiveurl, self.buildnumber, filepostfix, fullpackageextension))
        
        # Put full distribution in tar.gz
        if (not aptana.conf.ismac() and not aptana.conf.iswin()):
            client.put("%s/distro/Jaxer_package_withApache.tar.gz" % (self.buildroot),
                       "%s/Jaxer_package_withApache%s.tar.gz" % (buildlatesturl, filepostfix))
            client.put("%s/distro/Jaxer_package_withApache.tar.gz" % (self.buildroot),
                       "%s/Jaxer_package_withApache_%s%s.tar.gz" % (buildarchiveurl, self.buildnumber, filepostfix))
            # PRO
            if (self.conf["pack_jaxerpro"] == "True"):
                client.put("%s/distro/JaxerPro_package_withApache.tar.gz" % (self.buildroot),
                       "%s/JaxerPro_package_withApache%s.tar.gz" % (buildlatesturl, filepostfix))
                client.put("%s/distro/JaxerPro_package_withApache.tar.gz" % (self.buildroot),
                       "%s/JaxerPro_package_withApache_%s%s.tar.gz" % (buildarchiveurl, self.buildnumber, filepostfix))

        # Put jaxer only distribution.
        if (not aptana.conf.ismac()):
            client.put("%s/distro/Jaxer_package.zip" % (self.buildroot),
                       "%s/Jaxer_package%s.zip" % (buildlatesturl, filepostfix))
            client.put("%s/distro/Jaxer_package.zip" % (self.buildroot),
                       "%s/Jaxer_package_%s%s.zip" % (buildarchiveurl, self.buildnumber, filepostfix))
            #PRO
            if (self.conf["pack_jaxerpro"] == "True"):
                client.put("%s/distro/JaxerPro_package.zip" % (self.buildroot),
                       "%s/JaxerPro_package%s.zip" % (buildlatesturl, filepostfix))
                client.put("%s/distro/JaxerPro_package.zip" % (self.buildroot),
                       "%s/JaxerPro_package_%s%s.zip" % (buildarchiveurl, self.buildnumber, filepostfix))

            if (not aptana.conf.iswin()):
                client.put("%s/distro/Jaxer_package_withApache.tar.gz" % (self.buildroot),
                           "%s/Jaxer_package_withApache%s.tar.gz" % (buildlatesturl, filepostfix))
                client.put("%s/distro/Jaxer_package_withApache.tar.gz" % (self.buildroot),
                           "%s/Jaxer_package_withApache_%s%s.tar.gz" % (buildarchiveurl, self.buildnumber, filepostfix))

                if (self.conf["pack_jaxerpro"] == "True"):
                    client.put("%s/distro/JaxerPro_package_withApache.tar.gz" % (self.buildroot),
                           "%s/JaxerPro_package_withApache%s.tar.gz" % (buildlatesturl, filepostfix))
                    client.put("%s/distro/JaxerPro_package_withApache.tar.gz" % (self.buildroot),
                           "%s/JaxerPro_package_withApache_%s%s.tar.gz" % (buildarchiveurl, self.buildnumber, filepostfix))

        # Put jaxer update.
        client.put("%s/distro/Jaxer_update.zip" % (self.buildroot),
                   "%s/Jaxer_update%s.zip" % (buildlatesturl, filepostfix))
        client.put("%s/distro/Jaxer_update.zip" % (self.buildroot),
                   "%s/Jaxer_update_%s%s.zip" % (buildarchiveurl, self.buildnumber, filepostfix))

        if (self.conf["pack_jaxerpro"] == "True"):
            client.put("%s/distro/JaxerPro_update.zip" % (self.buildroot),
                   "%s/JaxerPro_update%s.zip" % (buildlatesturl, filepostfix))
            client.put("%s/distro/JaxerPro_update.zip" % (self.buildroot),
                   "%s/JaxerPro_update_%s%s.zip" % (buildarchiveurl, self.buildnumber, filepostfix))

        # Put jaxer update in tar.gz
        if (not aptana.conf.iswin()):
            client.put("%s/distro/Jaxer_update.tar.gz" % (self.buildroot),
                       "%s/Jaxer_update%s.tar.gz" % (buildlatesturl, filepostfix))
            client.put("%s/distro/Jaxer_update.tar.gz" % (self.buildroot),
                       "%s/Jaxer_update_%s%s.tar.gz" % (buildarchiveurl, self.buildnumber, filepostfix))

            if (self.conf["pack_jaxerpro"] == "True"):
                client.put("%s/distro/JaxerPro_update.tar.gz" % (self.buildroot),
                       "%s/JaxerPro_update%s.tar.gz" % (buildlatesturl, filepostfix))
                client.put("%s/distro/JaxerPro_update.tar.gz" % (self.buildroot),
                       "%s/JaxerPro_update_%s%s.tar.gz" % (buildarchiveurl, self.buildnumber, filepostfix))

        # Put rpm/deb package
        if (aptana.conf.islinux()):
            if (aptana.conf.osbits() == 32):
                rpmfile = "jaxer-" + self.fullversion + "-1.i386.rpm"
                debfile = "jaxer-" + self.fullversion + "_i386.deb"
            else:
                rpmfile = "jaxer-" + self.fullversion + "-1.x86_64.rpm"
                debfile = "jaxer-" + self.fullversion + "_amd64.deb"
            self.logbuildevent("===== Copying rpm package %s to server..." % (rpmfile))
            client.put("%s/distro/Jaxer.rpm" % (self.buildroot),
                       "%s/%s" % (buildarchiveurl, rpmfile))
            client.put("%s/distro/Jaxer.rpm" % (self.buildroot),
                       "%s/Jaxer%s.rpm" % (buildlatesturl, filepostfix))

            self.logbuildevent("===== Copying debian package %s to server..." % (debfile))
            client.put("%s/distro/Jaxer.deb" % (self.buildroot),
                       "%s/%s" % (buildarchiveurl, debfile))
            client.put("%s/distro/Jaxer.deb" % (self.buildroot),
                       "%s/Jaxer%s.deb" % (buildlatesturl, filepostfix))

        # Put debug symbols
        if (self.conf["generate_debug_symbols"] == "True"):
            self.logbuildevent("===== Copying debug symbols to server...")
            basecrsurl = self.conf["jam_" + scheme + "_buildcrsurl"]
            self.logger.info("basecrsurl=%s" % (basecrsurl))
            if (aptana.conf.ismac()):
                client.put("%s/src/mozilla/%s/i386/dist/crashreporter-symbols/Jaxer_Symbols_%s.zip" % (self.buildroot, self.ffdir, self.jaxerbuildnumber),
                   "%s/Jaxer_Symbols_%s.zip" % (basecrsurl, self.buildnumber))
            elif (not aptana.conf.islinux() or aptana.conf.osbits() == 32):
                self.logger.info("SRC JaxerBuildNumber=%s" % (self.jaxerbuildnumber))
                self.logger.info("SRC_FILE: %s/src/mozilla/%s/dist/crashreporter-symbols/Jaxer_Symbols_%s.zip" % (self.buildroot, self.ffdir, self.jaxerbuildnumber))
                client.put("%s/src/mozilla/%s/dist/crashreporter-symbols/Jaxer_Symbols_%s.zip" % (self.buildroot, self.ffdir, self.jaxerbuildnumber),
                   "%s/Jaxer_Symbols_%s.zip" % (basecrsurl, self.buildnumber))
        
        
    def uninstall(self):
        """Uninstalls jaxer, reverses the steps done in install()."""
        # detach old image if it exists
        if (aptana.conf.ismac() and os.path.isdir(self.conf["jam_mount"])):
            dmg = aptana.archive.DmgArchiver()
            dmg.insrumentation = self.instrumentation
            dmg.detach(self.conf["jam_mount"])
        self.shell.rmtree(self.topdir, ignoreerrors=True)
        self.shell.rmtree(self.conf["jam_install"], ignoreerrors=True)

    def install(self):
        """Installs jaxer from a packaged distribution."""
        #REVIEW: unzip_location is used to root the unzip one level above the jam_install path
        if (aptana.conf.iswin()):
            zip = aptana.archive.ZipArchiver()
            zip.insrumentation = self.instrumentation
            zip.decompress(self.conf["jam_package"], self.conf["unzip_location"])
        else: # mac, solaris, linux, or generic unix
            if (aptana.conf.ismac()):
                dmg = aptana.archive.DmgArchiver()
                dmg.insrumentation = self.instrumentation
                # detach old image if it exists
                if (os.path.isdir(self.conf["jam_mount"])):
                    dmg.detach(self.conf["jam_mount"])
                # attach new image
                dmg.attach(self.conf["jam_package"], readonly=True)
                # copy files from image
                self.shell.scmcopytree(self.conf["jam_mount"] + "/" + self.conf["install_dirname"], self.conf["jam_install"])
                dmg.detach(self.conf["jam_mount"])
            else:
                zip = aptana.archive.ZipArchiver()
                zip.insrumentation = self.instrumentation
                zip.decompress(self.conf["jam_package"], self.conf["unzip_location"])
            # make bins and scripts executable
            for name in glob.glob(os.path.join(self.conf["jam_install"], "scripts", "*.sh")):
                self.shell.chmod(name, 0755)
            for name in glob.glob(os.path.join(self.conf["jam_install"], "Apache22", "bin", "*")):
                self.shell.chmod(name, 0755)
            for name in glob.glob(os.path.join(self.conf["jam_install"], "Apache22", "build", "*")):
                self.shell.chmod(name, 0755)
            for name in glob.glob(os.path.join(self.conf["jam_install"], "jaxer", "*")):
                self.shell.chmod(name, 0755)

    def reinstall(self):
        """Uninstalls and the installs jaxer from a fresh distribution."""
        self.uninstall()
        self.install()


    #
    # automatic build methods:
    #

    def autorebuild(self):
        self.autobuild(fullbuild=True)

    def autobuild(self, fullbuild=False):
        """Automated Jaxer build."""
        self.logger.info("Checking if svn server is running...")
        if (not self.scm.isrunning(self.buildroot)):
            self.notifier.enabled = False # disable email notifications
            self.logger.error("svn server is down.")
            return

        lastbuiltrevision = 0
        lastgoodrevision = 0
        props = None
        try:
            self.logger.info("Looking for new checkins...")

            # unlock any files from previous runs
            self.scm.cleanup(self.buildroot)

            # get latest revision as reported by svn
            latestrevision = self.scm.serverrevisionof(self.buildroot)
            self.logger.info("Found latest revision: %d" % latestrevision)

            # load persisted revision
            props = aptana.conf.Properties()
            props.instrumentation = self.instrumentation
            if (os.path.exists(self.buildstatefilepath)):
                props.load(self.buildstatefilepath)

            lastgoodrevision = int(props.get("lastgoodrevision", 0))
            lastbuiltrevision = int(props.get("lastbuiltrevision", 0))
            self.logger.info("Found lastgoodrevision: %d saved in file: %s" % (lastgoodrevision, self.buildstatefilepath))
            self.logger.info("Found lastbuiltrevision: %d saved in file: %s" % (lastbuiltrevision, self.buildstatefilepath))

            # if the persisted revision is the latest revision and it's not a forced build then return
            if (self.scm.islatestrevision(self.buildroot, lastbuiltrevision) and not os.path.exists(os.path.join(self.toolscontrolroot, "forcebuild.ctrl"))):
                self.notifier.enabled = False # disable email notifications
                self.logger.info("Build not required.")
                return

            # clean old log files
            logfiledir=os.path.abspath(os.path.dirname(self.conf["logfile"]))
            self.logger.info("Delete old logfiles under %s..." % logfiledir)
            self.shell.removeoldfiles(logfiledir, 14)
            
            # Remove force-build regardless of whether we got here from a forced
            # build or from a check-in.
            self.shell.remove(os.path.join(self.toolscontrolroot, "forcebuild.ctrl"), ignoremissing=True)

            # clean headers
            self.cleanversionheaders()
            self.syncversionheaders()

            # Update to latest svn revision.
            svninfo = self.scm.infodict(self.buildroot)
            lastbuiltrevision = int(svninfo["Revision"])

            reposinfo = self.scm.infodict(svninfo["URL"])
            # Check revisions before syncing.
            if (svninfo["Revision"] != reposinfo["Revision"]):
                self.logger.info("Updating from revision: %s to: %s" % (svninfo["Revision"], reposinfo["Revision"]))
                self.scm.update(self.buildroot)
                #Update tunk/tools
                self.scm.update(self.buildroot + '/../../tools')
                
                # After updating, make sure to get the latest info for the build number.
                reposinfo = self.scm.infodict(svninfo["URL"])

            #Update tools/ide_crossover/libs
            self.scm.update(self.buildroot + '/tools/ide_crossover/libs')
                
            # set build number
            self.buildnumber = int(reposinfo["Revision"])
            
            # set jaxer build number (eg 3989_RC_C)
            self.logger.info("Build root=%s" % self.buildroot)
            tmpfname = self.buildroot + '/src/mozilla/aptana/jaxerBuildId.h'
            tmpf = open(tmpfname, 'r')
            tcontent = tmpf.read()
            tmpf.close()
            y = re.search('^\s*#define\s+JAXER_BUILD_NUMBER\s+"[0-9x]+([^"]*)"', tcontent, re.MULTILINE)
            self.jaxerbuildnumber = reposinfo["Revision"] + y.group(1)
            self.logger.info("Jaxer build number=%s" % self.jaxerbuildnumber)
            #define JAXER_BUILD_NUMBER "3665_RC_B" 
            
            
            # Set log archive name to be copied by top level script.
            opts = self.conf.lookup("opts")
            filename, ext = os.path.splitext(os.path.basename(self.conf["logfile"]))
            logarchivefilename = "%s-%d%s.zip" % (filename, self.buildnumber, ext)
            opts["logarchivefile"] = os.path.join(os.path.dirname(self.conf["logfile"]), logarchivefilename)

            # populate svnchangelog.
            # - Use lastgoodrevision + 1 since svn log will include the
            # revision specified inside the log.
            svnchangelog = None
            if (lastgoodrevision ==  0):
                svnchangelog = "No last good revision.\n"
            elif (self.buildnumber > lastgoodrevision):
                svnchangelog = self.scm.logchangessince(self.buildroot, "%s:BASE" % (lastgoodrevision + 1))
            else:
                svnchangelog = "No changes.\n"

            # Do build.
            if (fullbuild is True or self.conf["do_clean_build"] == "True"):
                self.rebuildall()
            else:
                self.buildall()

            #REVIEW: use jam_package
            if (aptana.conf.ismac()):
                filetocheck = os.path.join(self.buildroot, "distro", "Jaxer_package_withApache.dmg")
            elif (aptana.conf.iswin()):
                filetocheck = os.path.join(self.buildroot, "distro", "Jaxer_package.zip")
            else: # solaris, linux, or generic unix
                filetocheck = os.path.join(self.buildroot, "distro", "Jaxer_package_withApache.zip")

            # See if jam got built; if not, there must have been a build error.
            if (not os.path.exists(filetocheck)):
                raise BuildError("jam package not found at: " + filetocheck)

            # run unit tests
            if (self.conf["test_enabled"] == "True"):
                # reinstalls jaxer from a fresh distribution, deletes the previous install
                self.reinstall()

                self.logbuildevent("===== Running Jaxer unit tests...")
                test = aptana.jaxer.jxrtestsuite.JaxerTestScript(self.conf, "autotestserver")
                test.clone(self)
                self.chain(test, debug=True)
            else:
                self.logbuildevent("===== Skipping Jaxer unit tests.")

        except Exception:
            type, exc, tb = sys.exc_info()

            # Report failed build.
            tracebackstr = "".join(traceback.format_exception(type, exc, tb))
            self.logger.error("Jaxer build %d failed!\n%s" % (self.buildnumber, tracebackstr))

            # Prepare failure email.
            self.notifier.subject = "Jaxer build %d failed! - %s" % (self.buildnumber, str(exc))
            message = ""
            for buildevent in self._buildevents:
                message += "%s" % (buildevent)
            message += "\nChanges since the last good build:\n%s" % (svnchangelog)
            self.notifier.message += message

            self.logger.info("after unit test: check resultfile")
            if (os.path.exists(self.conf["test_resultsfile"])):
                self.notifier.addattachment(self.conf["test_resultsfile"], mimetype=["text","html"])
                
            self.logger.info("after unit test: check iws resultfile")
            if (os.path.exists(self.conf["test_iws_resultsfile"])):
                self.notifier.addattachment(self.conf["test_iws_resultsfile"], mimetype=["text","html"])
                
            self.logger.info("after unit test: check mysql resultfile")
            if (os.path.exists(self.conf["test_mysqlresultsfile"])):
                self.notifier.addattachment(self.conf["test_mysqlresultsfile"], mimetype=["text","html"])
            
            self.logger.info("after unit test: check iws mysql resultfile")
            if (os.path.exists(self.conf["test_iws_mysqlresultsfile"])):
                self.notifier.addattachment(self.conf["test_iws_mysqlresultsfile"], mimetype=["text","html"])
            
            # attach jaxer log
            if (os.path.exists(os.path.join(self.conf["jam_install"], "logs", "jaxer.log"))):
                self.notifier.addattachment(os.path.join(self.conf["jam_install"], "logs", "jaxer.log"), compress=True)
                
            # Persist the revision number.
            props.setproperty("lastbuiltrevision", str(self.buildnumber))
            props.store(self.buildstatefilepath)
            self.logger.info("Saved last built revision: %d to file: %s" % (self.buildnumber, self.buildstatefilepath))

            # Copy failed distribution to file server and save it as such.
            self.copydist(failed=True)
            self.ftpdist(failed=True)

            # fail this script
            self.fail(tracebackstr)
        else:
            # Report successful build.
            self.logger.info("Jaxer build %d succeeded." % (self.buildnumber))

            # Prepare success email.
            self.notifier.subject = "Jaxer build %d succeeded." % (self.buildnumber)
            message = ""
            for buildevent in self._buildevents:
                message += "%s" % (buildevent)
            message += "\nChanges in this build:\n%s" % (svnchangelog)
            self.notifier.message += message

            self.logger.info("Attache test results.")
            if (os.path.exists(self.conf["test_resultsfile"])):
                self.logger.info("  Results file %s" % (self.conf["test_resultsfile"]))
                self.notifier.addattachment(self.conf["test_resultsfile"], mimetype=["text","html"])
            if (os.path.exists(self.conf["test_iws_resultsfile"])):
                self.logger.info("  Results file %s" % (self.conf["test_iws_resultsfile"]))
                self.notifier.addattachment(self.conf["test_iws_resultsfile"], mimetype=["text","html"])
            if (os.path.exists(self.conf["test_mysqlresultsfile"])):
                self.logger.info("  Results file %s" % (self.conf["test_mysqlresultsfile"]))
                self.notifier.addattachment(self.conf["test_mysqlresultsfile"], mimetype=["text","html"])
            if (os.path.exists(self.conf["test_iws_mysqlresultsfile"])):
                self.logger.info("  Results file %s" % (self.conf["test_iws_mysqlresultsfile"]))
                self.notifier.addattachment(self.conf["test_iws_mysqlresultsfile"], mimetype=["text","html"])
            # attach jaxer log
            if (os.path.exists(os.path.join(self.conf["jam_install"], "logs", "jaxer.log"))):
                self.notifier.addattachment(os.path.join(self.conf["jam_install"], "logs", "jaxer.log"), compress=True)   

            # Persist the revision number.
            props.setproperty("lastbuiltrevision", str(self.buildnumber))
            props.setproperty("lastgoodrevision", str(self.buildnumber))
            props.store(self.buildstatefilepath)
            self.logger.info("Saved last good revision: %d to file: %s" % (self.buildnumber, self.buildstatefilepath))

            # Copy successful and tested distribution to file server.
            self.copydist(failed=False)
            self.ftpdist(failed=False)

            # Update and check in a file to trigger an IDE build.
            self.triggerbuildstudio()

    def triggerbuildstudio(self):
        """Update and check in a file to trigger an IDE build."""
        if (self.conf.exists("studiotriggerlocation") and self.conf["studiotriggerlocation"] is not None and len(self.conf["studiotriggerlocation"]) > 0):
            self.logger.info("Checking in jaxer version file to trigger studio build.")
            self.shell.write(self.conf["studiotriggerlocation"], str(self.buildnumber))
            self.scm.commit("Automatic checkin by build system.", self.conf["studiotriggerlocation"])


    def autotest(self):
        """Automated Jaxer unit tests."""
        self.logger.info("Checking if svn server is running...")
        if (not self.scm.isrunning(_aptanalibpath)):
            self.notifier.enabled = False # disable email notifications
            self.logger.error("svn server is down.")
            return
        
        jaxerversion=None
        lastgoodrevision=0
        try:
            self.logbuildevent("===== Rsyncing packages...")
            
            # Get timestamp for existing jam_package
            jam_package = self.conf['jam_package']
            originaltimestamp = 0
            if (os.path.exists(jam_package)):
                jampackagestat = os.stat(jam_package)
                originaltimestamp = jampackagestat.st_mtime
            
            # Rsync latest jam_package
            rsync = aptana.process.Process("rsync")
            rsync.cwd = self.conf["jam_package_dir"]
            rsync.arg("-azv")
            rsync.arg("%s@%s:%s/%s" % (self.conf['jam_ssh_user'],self.conf['jam_ssh_host'],self.conf['jam_ssh_path_latest'],self.conf['jam_package_file']))
            rsync.arg('.')
            self.logger.debug(str(rsync))
            rsync.execute(self.executor)
            # TODO error check
            
            # See if jam was rsynced.
            if (not os.path.exists(jam_package)):
                raise BuildError("jam package not found at: " + jam_package)

            jampackagestat = os.stat(jam_package)
            timestamp = jampackagestat.st_mtime
            
            self.logger.debug("timestamp %s originaltimestamp %s" % (timestamp, originaltimestamp))

            # if the mtime hasn't changed and it's not a forced test then return
            if (timestamp <= originaltimestamp and not os.path.exists(os.path.join(self.toolscontrolroot, "forcetest.ctrl"))):
                self.notifier.enabled = False # disable email notifications
                self.logger.info("Test not required.")
                return

            # Remove force-test regardless of whether we got here from a forced
            # build or from a check-in.
            self.shell.remove(os.path.join(self.toolscontrolroot, "forcetest.ctrl"), ignoremissing=True)
            
            
            self.logbuildevent("===== Installing Jaxer...")
            self.reinstall()
            # TODO error check

            file = open("%s/jaxer/version.txt" % (self.conf['jam_install']))
            line = file.readline()

            jaxerversion = line.split('"')[1]
            buildnumberstring = jaxerversion.split('.')[3]
            buildnumbermatch = re.match('^\d+',buildnumberstring)
            if (buildnumbermatch is not None):
                self.setbuildnumber(int(buildnumbermatch.group()))

            # Set log archive name to be copied by top level script.
            opts = self.conf.lookup("opts")
            filename, ext = os.path.splitext(os.path.basename(self.conf["logfile"]))
            logarchivefilename = "%s-%d%s.zip" % (filename, self.buildnumber, ext)
            opts["logarchivefile"] = os.path.join(os.path.dirname(self.conf["logfile"]), logarchivefilename)

            if (self.buildnumber is not None):
                # load persisted revision.  This is really only needed for svn log.
                props = aptana.conf.Properties()
                props.instrumentation = self.instrumentation
                if (os.path.exists(self.buildstatefilepath)):
                    props.load(self.buildstatefilepath)

                    lastgoodrevision = int(props.get("lastgoodrevision", 0))
                    self.logger.info("Found lastgoodrevision: %d saved in file: %s" % (lastgoodrevision, self.buildstatefilepath))

            # populate svnchangelog.
            # - Use lastgoodrevision + 1 since svn log will include the
            # revision specified inside the log.
            svnchangelog = None
            if (self.buildnumber is None):
                svnchangelog = "Can't detect revision for jaxer version %s" % (jaxerversion)
            elif (lastgoodrevision ==  0):
                svnchangelog = "No last good revision.\n"
            elif (self.buildnumber > lastgoodrevision):
                svnchangelog = self.scm.logchangessince(self.buildroot, "%s:%s" % (lastgoodrevision + 1, self.buildnumber))
            else:
                svnchangelog = "No changes.\n"

            # reinstalls jaxer from a fresh distribution, deletes the previous install
            self.reinstall()

            # run unit tests
            self.logbuildevent("===== Running Jaxer unit tests...")
            test = aptana.jaxer.jxrtestsuite.JaxerTestScript(self.conf, "autotestserver")
            test.clone(self)
            self.chain(test, debug=True)

        except Exception:
            type, exc, tb = sys.exc_info()

            # Report failed build.
            tracebackstr = "".join(traceback.format_exception(type, exc, tb))
            self.logger.error("Jaxer unit tests for %s failed!\n%s" % (jaxerversion, tracebackstr))

            # Prepare failure email.
            self.notifier.subject = "Jaxer unit tests for %s failed! - %s" % (jaxerversion, str(exc))
            message = ""
            for buildevent in self._buildevents:
                message += "%s" % (buildevent)
            message += "\nChanges since the last good build:\n%s" % (svnchangelog)
            self.notifier.message += message
            if (os.path.exists(self.conf["test_resultsfile"])):
                self.notifier.addattachment(self.conf["test_resultsfile"], mimetype=["text","html"])
            if (os.path.exists(self.conf["test_mysqlresultsfile"])):
                self.notifier.addattachment(self.conf["test_mysqlresultsfile"], mimetype=["text","html"])
            # attach jaxer log
            if (os.path.exists(os.path.join(self.conf["jam_install"], "logs", "jaxer.log"))):
                self.notifier.addattachment(os.path.join(self.conf["jam_install"], "logs", "jaxer.log"), compress=True)

            # fail this script
            self.fail(tracebackstr)
        else:
            # Report successful build.
            self.logger.info("Jaxer unit tests for %s succeeded." % (jaxerversion))

            # Prepare success email.
            self.notifier.subject = "Jaxer unit tests for %s succeeded." % (jaxerversion)
            message = ""
            for buildevent in self._buildevents:
                message += "%s" % (buildevent)
            message += "\nChanges in this build:\n%s" % (svnchangelog)
            self.notifier.message += message
            if (os.path.exists(self.conf["test_resultsfile"])):
                self.notifier.addattachment(self.conf["test_resultsfile"], mimetype=["text","html"])
            if (os.path.exists(self.conf["test_mysqlresultsfile"])):
                self.notifier.addattachment(self.conf["test_mysqlresultsfile"], mimetype=["text","html"])
            # attach jaxer log
            if (os.path.exists(os.path.join(self.conf["jam_install"], "logs", "jaxer.log"))):
                self.notifier.addattachment(os.path.join(self.conf["jam_install"], "logs", "jaxer.log"), compress=True)

            # Persist the revision number.
            props.setproperty("lastgoodrevision", str(self.buildnumber))
            props.store(self.buildstatefilepath)
            self.logger.info("Saved last good revision: %d to file: %s" % (self.buildnumber, self.buildstatefilepath))


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    build = Build()
    print build
 
if (__name__ == "__main__"):
    main()
