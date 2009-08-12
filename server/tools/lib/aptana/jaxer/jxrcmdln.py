#!/usr/bin/env python
import logging
from logging import _levelNames
import os
import socket
import sys
import time
import traceback

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

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
import aptana.jaxer.jxrservice
import aptana.jaxer.jxrtestsuite
from contrib import cmdln

__version__ = "$Revision: 4036 $"[11:-2]


class JaxerCmdln(cmdln.Cmdln):
    """${name}: CLI for Aptana Jaxer build tasks.

    Usage:
        ${option_list}
        ${command_list}
        ${help_list}
    """

    def __init__(self, *args, **kwargs):
        super(JaxerCmdln, self).__init__(self, *args, **kwargs)
        #cmdln.Cmdln.__init__(self, *args, **kwargs)
        cmdln.Cmdln.do_help.aliases.append("h")
        # public attributes:
        # define the name of the command used in help messages as ${name}
        self.name = "aptan"
        # setting version adds --version top level option to display __version__
        self.version = __version__
        # private attributes:
        # false if stdout or stderr
        self._isloggingtofile = False
        # save original cwd, if it's changed module reloading might break
        self._origcwd = os.getcwd()
        self._toolspath = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
        self._defaultconffilepath = os.path.join(self._toolspath, "conf", "build.properties")
        self._defaultgconffilepath = os.path.join(self._toolspath, "conf", "global_build.properties")
        self._forcestopfilepath = os.path.join(self._toolspath, "ctrl", "forcestop.ctrl")

    def getloglevels(self):
        """Returns a list of allowable log level names."""
        levels = []
        for level in logging._levelNames:
            if (isinstance(level, str)):
                levels.append(level)
        return levels


    #
    # cmdln interface:
    # NOTE: Methods are declared in the order they are called.
    #

    def deletestopfile(self):
        if (os.path.exists(self._forcestopfilepath)):
            os.remove(self._forcestopfilepath)

    def stop(self, deletestopfile=True):
        """Force stop a looping script."""
        stop = os.path.exists(self._forcestopfilepath)
        if (stop):
            # use print because the log system might not have been initialized yet
            print "[%s]:(%s):STATUS: Force stop detected from: %s" % (time.strftime("%Y%m%d-%H:%M:%S"), self.name, self._forcestopfilepath)
            if (deletestopfile is True):
                self.deletestopfile()
        return stop

    #TODO: allow commands or an entire argv to be processed from an svn file
    def forcecmd(self, argv):
        """Processes a command file from svn."""
        localhostname = socket.gethostname()
        ctrlpath = os.path.join(self._toolspath, "ctrl")
        for root, dirs, files in os.walk(ctrlpath):
            for filename in files:
                matches = re.search(r'^([a-zA-Z0-9-]-)*force-([a-zA-Z0-9])+[.]cmd$', filename)
                hostname = matches.group(1)
                if (hostname != localhostname):
                    continue
                cmd = matches.group(2)
                try:
                    print cmd
                    # add to argv or dispatch via reflection
                    #self.do_$cmd()
                finally:
                    self.scm.delete(os.path.join(root, filename))

    def main(self, argv=None, loop=cmdln.LOOP_NEVER):
        """Main entry point to run a command."""
        try:
            rc = super(JaxerCmdln, self).main(argv, loop)
            return rc
        finally:
            self.shutdown()

    def get_optparser(self, argv):
        """Hook for handling top level command line options."""
        optparser = super(JaxerCmdln, self).get_optparser(argv)
        #HACK: add -? as a way to show help
        optparser._short_opt.get("-h")._short_opts.append("-?")
        #HACK: add -V to display version
        optparser._short_opt["-V"] = optparser._long_opt.get("--version")
        optparser._short_opt["-V"]._short_opts.append("-V")
        # add top level options
        optparser.add_option(
            "--conf",
            action="store",
            metavar="build.properties",
            default=self._defaultconffilepath, # default to using relative paths to resolve the conf file
            help="Specifies the path to the configuration file."
        )
        optparser.add_option(
            "--gconf",
            action="store",
            metavar="global_build.properties",
            default=self._defaultgconffilepath, # default to using relative paths to resolve the conf file
            help="Specifies the path to the global configuration file."
        )
        optparser.add_option(
            "--delay",
            action="store",
            type="int",
            metavar="0",
            default=0,
            help="Causes the command execution to be delayed n seconds, when looping this delays each subsequent execution."   
        )
        optparser.add_option(
            "--loop",
            action="store",
            type="int",
            metavar="1",
            default=1,
            help="Causes the command execution to be repeated n times, -n will loop forever."   
        )
        optparser.add_option(
            "--loopretrydelay",
            action="store",
            type="int",
            metavar="180",
            default=180,
            help="Causes a failed command execution to be delayed n seconds when looping."   
        )
        optparser.add_option(
            "--logfile",
            action="store",
            metavar="stdout|stderr|command.log",
            default="stdout",
            help="Logs all command output to a stream or file, stream can be: stdout|stderr file can be any filepath."
        )
        optparser.add_option(
            "--logarchivefile",
            action="store",
            metavar="arch.log|arch.log.zip",
            help="Archive file to copy the current log file after command execution.  If the file name ends with .zip it will be compressed."
        )
        optparser.add_option(
            "--loglevel",
            action="store",
            metavar=logging._levelNames[logging.INFO],
            default=logging._levelNames[logging.INFO],
            choices=self.getloglevels(),
            help="Sets log level to one of: " + str(self.getloglevels())
        )
        optparser.add_option(
            "--logformat",
            action="store",
            metavar="%(asctime)s:(%(name)s):%(levelname)s: %(message)s",
            default="%(asctime)s:(%(name)s):%(levelname)s: %(message)s",
            help="Sets log format string.  See python logging package for docs."
        )
        optparser.add_option(
            "--logdateformat",
            action="store",
            metavar="[%Y%m%d-%H:%M:%S]",
            default="[%Y%m%d-%H:%M:%S]",
            help="Sets log date format string.  See python logging package for docs."
        )
        optparser.add_option(
            "--logredirection",
            action="store_true",
            metavar="",
            default=True,
            dest="logredirection",
            help="Enables redirection of stdout and stderr to the logfile if a logfile is specified.  This is the default setting."
        )
        # don't specify a default or it will override the one set for --logredirection
        optparser.add_option(
            "--nologredirection",
            action="store_false",
            metavar="",
            dest="logredirection",
            help="Disables redirection of stdout and stderr to the logfile."
        )
        optparser.add_option(
            "--notify",
            action="store_true",
            metavar="",
            default=False,
            help="Enables sending of notifications containing command output or execution status."
        )
        optparser.add_option(
            "--notifyhost",
            action="store",
            metavar="mail.aptana.com",
            default="mail.aptana.com",
            help="Configures the smtp host used for E-mail notifications."
        )
        optparser.add_option(
            "--notifyfrom",
            action="store",
            metavar="builds@aptana.com",
            default="builds@aptana.com",
            help="Configures the smtp sender used for E-mail notifications."
        )
        optparser.add_option(
            "--notifyto",
            action="append",
            metavar="user@localhost.localdomain",
            default=[],
            help="Adds smtp recipients used for E-mail notifications, each notifyto option adds to the recipient list."
        )
        # process overrides and add them as top level options
        self.parseoverrides(optparser, argv)
        return optparser

    def parseoverrides(self, optparser, argv):
        """Process java style -Dname=value configuration overrides."""
        for arg in argv:
            pos = arg.find("-D")
            if (pos == -1):
                continue
            prefix = arg[0: pos + 2] # -D | --D
            pair = arg[pos + 2:] # name=value
            tokens = pair.split("=", 2)
            if (len(tokens) == 1):
                optparser.add_option(
                    prefix + tokens[0],
                    action="store",
                    metavar="True|False",
                    default=True,
                    help="Configuration property override."
                )
            elif (len(tokens) == 2):
                optparser.add_option(
                    prefix + tokens[0],
                    action="store",
                    metavar="value",
                    default=tokens[1],
                    help="Configuration property override."
                )
            else: # empty -D | --D
                pass
            # remove from argv
            argv.remove(arg)

    def postoptparse(self):
        """Hook method called after option processing has completed."""
        # init logging system
        self._isloggingtofile = self.initlogging()
        # redirect stdout/stderr to the logfile if specified and if we are logging to a file
        if (self.options.logredirection is True and self._isloggingtofile is True):
            # replace stdout and stderr with the file stream
            sys.stdout = logging.getLogger().handlers[0].stream
            sys.stderr = logging.getLogger().handlers[0].stream

        # create logger
        self.logger = logging.getLogger(self.name)
        self.logger.setLevel(logging._levelNames[self.options.loglevel])

        # create instrumentation
        self.instrumentation = aptana.boot.Instrumentation(self.logger)

        # create configuration
        self.conf = aptana.conf.createconfigurator(self.options.gconf, self.options.conf, os.environ, self.options, self.instrumentation)
        self.conf.instrumentation = self.instrumentation

        # init shell
        self.shell = aptana.shell.Shell()
        self.shell.instrumentation = self.instrumentation

        # init executor
        self.executor = aptana.process.LocalhostProcessExecutor()
        self.executor.instrumentation = self.instrumentation

        # create notifier
        self.notifier = aptana.notification.SmtpNotifier()
        self.notifier.instrumentation = self.instrumentation
        self.notifier.enabled = self.conf["notify"]
        self.notifier.compressall = False
        self.notifier.hostname = self.conf["notifyhost"]
        self.notifier.sender = self.conf["notifyfrom"]
        recipients = self.conf["notifyto"]
        if (recipients is not None):
            for recipient in recipients:
                self.notifier.addrecipient(recipient)

        # create a properly configured default script for new scripts to clone from
        script = aptana.script.Script(self.conf)
        script.notifier = self.notifier
        script.instrumentation.changeinstrumentation(self.instrumentation)
        self.defaultscript = script

    def initlogging(self):
        """Initializes the logging system."""
        # normalize level from string to code
        levelcode = logging._levelNames[self.options.loglevel]

        usinglogfile = False
        # init logging system with a stream handler
        if (self.options.logfile is None or self.options.logfile == "stdout" or self.options.logfile == "stderr"):
            stream = sys.stdout
            if (self.options.logfile == "stderr"):
                stream = sys.stderr
            # configure the log system
            logging.basicConfig(
                stream=stream,
                level=levelcode,
                format=self.options.logformat,
                datefmt=self.options.logdateformat
            )
        # init logging system with a file handler
        else:
            # resolve log file to absolute paths
            self.options.logfile = os.path.abspath(self.options.logfile)
            # create log directory if it doesn't exist
            if (not os.path.isdir(os.path.dirname(self.options.logfile))):
                os.makedirs(os.path.dirname(self.options.logfile))
            # configure the log system
            logging.basicConfig(
                filename=self.options.logfile,
                filemode="w+",
                level=levelcode,
                format=self.options.logformat,
                datefmt=self.options.logdateformat
            )
            usinglogfile = True
        return usinglogfile

    def precmd(self, argv):
        """Hook called before a command is executed."""
        # process command execution delay
        delay = self.options.delay
        if (delay > 0):
            #print >> sys.__stderr__, "[%s]:(%s):STATUS: Sleeping for %d second(s) before executing: %s" % (time.strftime("%Y%m%d-%H:%M:%S"), self.name, delay, " ".join(argv))
            #self.logger.info("Sleeping for %d second(s) before executing: %s" % (delay, " ".join(argv)))
            time.sleep(delay)
        return argv

    def runscript(self, script):
        """Initializes the script and runs it."""
        script.clone(self.defaultscript)
        return script.main()

    def cmdexc(self, argv):
        """Hook called if an exception is thrown from a command."""
        handled = super(JaxerCmdln, self).cmdexc(argv)
        if (not handled):
            # postcmd() is not called by cmd() if an exception is thrown so explicitly chain it here
            self.postcmd(argv, True)
        return handled

    def postcmd(self, argv, failed=False):
        """Hook called after a command has executed."""
        # flush all handlers attached to the logger
        #NOTE: only the root logger seems to contain handlers
        for handler in logging.getLogger().handlers:
            handler.flush()

        # archive log file if specified
        if (self._isloggingtofile and self.options.logarchivefile is not None):
            # process archive log file if specified
            if (self.options.logarchivefile is not None):
                self.options.logarchivefile = os.path.abspath(self.options.logarchivefile)
                # create log archive directory if it doesn't exist
                self.shell.mkdirs(os.path.dirname(self.options.logarchivefile), ignoreexisting=True)
            # compress
            if (self.options.logarchivefile.endswith(".zip")):
                self.shell.remove(self.options.logarchivefile, ignoremissing=True)
                zip = aptana.archive.ZipArchiver(self.options.logarchivefile)
                zip.instrumentation = self.instrumentation
                zip.include(self.options.logfile, os.path.basename(self.options.logfile))
                zip.assemble()
            # copy
            else:
                self.shell.remove(self.options.logarchivefile, ignoremissing=True)
                self.shell.copy(self.options.logfile, self.options.logarchivefile)

        # send notifications if specified
        if (self.options.notify is True and self.notifier.enabled is True):
            subject = self.notifier.subject
            # add a default subject if none was set
            if (subject is None or len(subject) == 0):
                subject = " ".join(argv)
                # append the status to the subject
                if (failed is True):
                    subject += " failed!"
                else:
                    subject += " succeeded!"

            # put the exception message in the subject and add the full stack to the message
            if (failed is True):
                type, exc, tb = sys.exc_info()
                tracebackstr = "".join(traceback.format_exception(type, exc, tb))
                subject += " failed! - " + str(exc)
                self.notifier.message += tracebackstr

            self.notifier.subject = subject

            # if we are logging to a file attach it to the notification
            if (self._isloggingtofile is True):
                self.notifier.addattachment(self.options.logfile, compress=True)
            
            self.notifier.notify()

    def shutdown(self):
        """Shutdown handler."""
        # return back to the original cwd - this is critical for proper module reloading
        if (os.path.isdir(self._origcwd)): # could have been deleted while we were running
            os.chdir(self._origcwd)
        # reassign stdout/stderr back to their original values
        sys.stdout = sys.__stdout__
        sys.stderr = sys.__stderr__
        # shutdown the logging system
        logging.shutdown()
        logging.getLogger().handlers = []


    #
    # sub command processors: do_<subcmd>
    #

    @cmdln.option("--buildnumber", action="store", type="int", metavar="1", help="Stores the build number used to generate version files.")
    def do_autobuild(self, subcmd, suboptions, action="autobuild", *args):
        """${cmd_name}: Starts automatic building of jaxer in daemon mode.

        ${cmd_usage}
            ${name} ${cmd_name} autobuild

        ${cmd_option_list}"""
        script = aptana.jaxer.jxrbuild.Build(self.conf, action)
        self.runscript(script)

        
    def do_autotest(self, subcmd, suboptions, action="autotest", *args):
        """${cmd_name}: Starts automatic testing of jaxer in daemon mode.
        
        ${cmd_usage}
            ${name} ${cmd_name} autotest
            
        ${cmd_option_list}"""
        script = aptana.jaxer.jxrbuild.Build(self.conf, action)
        self.runscript(script)


    @cmdln.option("--buildnumber", action="store", type="int", metavar="1", help="Stores the build number used to generate version files.")
    def do_build(self, subcmd, suboptions, action="buildall", *args):
        """${cmd_name}: Builds jaxer and it's subcomponents.

        ${cmd_usage}
            ${name} ${cmd_name} autobuild
            ${name} ${cmd_name} buildall
            ${name} ${cmd_name}  buildjaxer
            ${name} ${cmd_name}   buildmozilla
            ${name} ${cmd_name}   buildjs
            ${name} ${cmd_name}   buildconnectors
            ${name} ${cmd_name}    buildservlet
            ${name} ${cmd_name}    buildmodjaxer
            ${name} ${cmd_name}    buildjaxerisapi
            ${name} ${cmd_name}   buildlibevent
            ${name} ${cmd_name}   buildmanager
            ${name} ${cmd_name}   buildlogger
            ${name} ${cmd_name}   buildtellmanager
            ${name} ${cmd_name}   buildlauncher
            ${name} ${cmd_name}  gendoc

        ${cmd_option_list}"""
        script = aptana.jaxer.jxrbuild.Build(self.conf, action)
        self.runscript(script)


    def do_clean(self, subcmd, suboptions, action="cleanall", *args):
        """${cmd_name}: Cleans out build artifacts.

        ${cmd_usage}
            ${name} ${cmd_name} cleanall
            ${name} ${cmd_name}  cleanjaxer
            ${name} ${cmd_name}   cleanmozilla
            ${name} ${cmd_name}   cleanjs
            ${name} ${cmd_name}   cleanconnectors
            ${name} ${cmd_name}    cleanservlet
            ${name} ${cmd_name}    cleanmodjaxer
            ${name} ${cmd_name}    cleanjaxerisapi
            ${name} ${cmd_name}   cleanlibevent
            ${name} ${cmd_name}   cleanmanager
            ${name} ${cmd_name}   cleanlogger
            ${name} ${cmd_name}   cleantellmanager
            ${name} ${cmd_name}   cleanlauncher
            ${name} ${cmd_name}  cleandoc
            ${name} ${cmd_name}  cleanselected
            
            ${name} ${cmd_name} tools

        ${cmd_option_list}"""
        if (action == "tools"):
            # clean out generated .pyc and .pyo files
            for root, dirnames, filenames in os.walk(self._toolspath):
                for filename in filenames:
                    if (filename.endswith(".pyc") or filename.endswith(".pyo")):
                        filepath = os.path.join(root, filename)
                        self.shell.remove(filepath)
        else:
            script = aptana.jaxer.jxrbuild.Build(self.conf, action)
            self.runscript(script)


    def do_diag(self, subcmd, suboptions, module, *args):
        """${cmd_name}: Run self diagnostic tests of aptana python libraries.
        Calls a static method and passes it argv.

        ${cmd_usage}
            ${name} ${cmd_name} aptana.archive.main
            ${name} ${cmd_name} aptana.boot.main
            ${name} ${cmd_name} aptana.conf.main
            ${name} ${cmd_name} aptana.net.main
            ${name} ${cmd_name} aptana.notification.main
            ${name} ${cmd_name} aptana.process.main
            ${name} ${cmd_name} aptana.scm.main
            ${name} ${cmd_name} aptana.script.main
            ${name} ${cmd_name} aptana.service.main
            ${name} ${cmd_name} aptana.shell.main

            ${name} ${cmd_name} aptana.jaxer.jxrbuild.main
            ${name} ${cmd_name} aptana.jaxer.jxrcmdln.main
            ${name} ${cmd_name} aptana.jaxer.jxrservice.main
            ${name} ${cmd_name} aptana.jaxer.jxrtestsuite.main

            ${name} ${cmd_name} <package>.<method>
        ${cmd_option_list}"""
        eval(module + "(" + str(args) + ")", globals())


    def do_dist(self, subcmd, suboptions, action="install", *args):
        """${cmd_name}: Manipulates jaxer distribution files.

        ${cmd_usage}
            ${name} ${cmd_name} install
            ${name} ${cmd_name} uninstall
            ${name} ${cmd_name} reinstall

            ${name} ${cmd_name} cleandist
            ${name} ${cmd_name} packdist
            ${name} ${cmd_name} copydist
            ${name} ${cmd_name} ftpdist

        ${cmd_option_list}"""
        script = aptana.jaxer.jxrbuild.Build(self.conf, action)
        self.runscript(script)


    def do_doc(self, subcmd, suboptions, action="gendoc", *args):
        """${cmd_name}: Generates jaxer documentation.

        ${cmd_usage}
            ${name} ${cmd_name} gendoc
            ${name} ${cmd_name} cleandoc
            ${name} ${cmd_name} syncdoc
        ${cmd_option_list}"""
        script = aptana.jaxer.jxrbuild.Build(self.conf, action)
        self.runscript(script)


    @cmdln.option("--url", action="store", type="string", metavar="http://localhost:8081", default="http://localhost:8081", help="Stores the URL to open in a browser.")
    def do_service(self, subcmd, suboptions, servicename="servers", action="start", *args):
        """${cmd_name}: Controls jaxer services.

        ${cmd_usage}
            ${name} ${cmd_name} servers start
            ${name} ${cmd_name} servers stop
            ${name} ${cmd_name} servers restart
            ${name} ${cmd_name}  apache configure
            ${name} ${cmd_name}  apache start
            ${name} ${cmd_name}  apache stop
            ${name} ${cmd_name}  apache restart
            ${name} ${cmd_name}  apache status
            ${name} ${cmd_name}  jaxer  start
            ${name} ${cmd_name}  jaxer  stop
            ${name} ${cmd_name}  jaxer  restart
            ${name} ${cmd_name} browser start
            ${name} ${cmd_name} browser stop
            ${name} ${cmd_name} browser restart
        ${cmd_option_list}"""
        services = []
        if (servicename == "servers"):
            #services.append(aptana.jaxer.jxrservice.ApacheService(self.conf, action))
            #services.append(aptana.jaxer.jxrservice.JaxerService(self.conf, action))
            services.append(aptana.jaxer.jxrservice.ServerController(self.conf, action))
        elif (servicename == "apache"):
            services.append(aptana.jaxer.jxrservice.ApacheService(self.conf, action))
        elif (servicename == "jaxer"):
            services.append(aptana.jaxer.jxrservice.JaxerService(self.conf, action))
        elif (servicename == "browser"):
            browser = aptana.jaxer.jxrservice.BrowserService(self.conf, action)
            browser.url = suboptions.url
            services.append(browser)
        else:
            self.logger.error("unsupported service: " + servicename)
            return
        # run all specified services
        for service in services:
            self.runscript(service)


    def do_show(self, subcmd, suboptions, *args):
        """${cmd_name}: Shows os and configuration values.

        ${cmd_usage}
        ${cmd_option_list}"""
        print aptana.conf.osdump()
        self.logger.info(self.conf)


    def do_system(self, subcmd, suboptions, *args):
        """${cmd_name}: Executes an os system command.

        ${cmd_usage}
        ${cmd_option_list}"""
        self.executor.execute(" ".join(args))


    def do_test(self, subcmd, suboptions, action="testserver", *args):
        """${cmd_name}: Runs the specified test case from the test suite.

        ${cmd_usage}
            ${name} ${cmd_name} autotestserver
            ${name} ${cmd_name}  testserver
        ${cmd_option_list}"""
        script = aptana.jaxer.jxrtestsuite.JaxerTestScript(self.conf, action)
        self.runscript(script)


def main(argv=None):
    """Main entry point from the command line."""
    return JaxerCmdln().main(argv)

if (__name__ == "__main__"):
    sys.exit(main())
