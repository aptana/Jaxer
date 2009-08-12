#!/usr/bin/env python
import logging
import platform
import pprint
import optparse
import os
import re
import socket
import sys
import time

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.boot

__version__ = "$Revision: 4022 $"[11:-2]


#NOTE: If the configuration system is doing it's job these should not be needed externally.

def iswin():
    return sys.platform.lower().startswith("win")

def ismac():
    return sys.platform.lower().startswith("darwin")

def issolaris():
    return sys.platform.lower().startswith("sunos")

def islinux():
    return sys.platform.lower().find("linux") > -1 # can return linux2

def isunix():
    return not iswin()

def islocalhost(hostname):
    """Returns True if the specified hostname represents the localhost, False otherwise."""
    return (hostname == "localhost" or hostname == "localhost.localdomain" or hostname == socket.gethostname())


def osname():
    """Returns a normalized os name string."""
    return os.name().lower()

def osplatform():
    """Returns a normalized platform string useful for os specific configuration."""
    if (iswin()):
        return "win"
    elif (ismac()):
        return "mac"
    elif (issolaris()):
        return "solaris"
    elif (islinux()):
        return "linux"
    else:
        return "unix"

def osplatformbitsdist():
    platbitsdist = osplatform() + str(osbits())
    if (osdistname() is not None and len(osdistname()) > 0):
        platbitsdist += "." + osdistname()
    return platbitsdist

def osbits():
    """Returns int 32 for 32-bit os's and 64 for 64-bit os's.  If platform.architecture() is not implemented defaults to 32"""
    bits = platform.architecture()[0] # (bits, linkage)
    if (bits is None or len(bits) == 0): # default to 32
        bits = "32"
    else:
        bits = bits[0:2] # 64bit => 64
    return int(bits)

def osdistname():
    """Returns the specific distribution name if it's defined, otherwise returns an empty string."""
    distname = platform.dist()[0] # (distname, version, id)
    return distname.lower()

def osdump():
    """Prints out os platform specific details."""
    confstr = ""
    confstr += ("os.name:                 " + os.name + os.linesep)
    confstr += ("sys.platform:            " + sys.platform + os.linesep)
    confstr += ("platform.platform():     " + platform.platform() + os.linesep)
    confstr += ("platform.architecture(): " + str(platform.architecture()) + os.linesep)
    confstr += ("platform.uname():        " + str(platform.uname()) + os.linesep)
    confstr += ("platform.dist():         " + str(platform.dist()) + os.linesep)
    confstr += ("platform.libc_ver():     " + str(platform.libc_ver()) + os.linesep)
    return confstr

def ospkillcommand():
    """Returns the default os command used to kill a process by name."""
    if (iswin()):
        return "taskkill /f /im"
    elif (ismac()):
        return "killall"
    elif (issolaris()):
        return "killall"
    else: # linux or generic unix
        return "killall"


def createconfigurator(gconffilepath=None, conffilepath=None, env=None, opts=None, instrumentation=None):
    """Factory method to create a configurator from file, environment, and command line option data sources."""
    configurator = Configurator()
    # top level command line options override everything
    if (opts is not None):
        configurator.register(CommandLineConfiguration("opts", opts), instrumentation)
    # environment overrides conf file
    if (env is not None):
        configurator.register(EnvironmentConfiguration("env", env), instrumentation)
    # load from conf file(s)
    # gconffilepath points to a config file that contains platform-indepent settings.
    # conffilepath points to a config file that contains platform-dependent settings.
    
    # JZ: Limitation: We assume conffilepath & gconffilepath would have the SAME extensions.
    #     Otherwise, it will not work
    if ((conffilepath is not None and os.path.exists(conffilepath)) or (gconffilepath is not None and os.path.exists(gconffilepath))):
        conf = None
        if (conffilepath is not None and os.path.exists(conffilepath)):
            # exec a python script and import it's config hash
            if (conffilepath.endswith(".py")):
                conf = Configuration("py")
            else: # read properties file
                conf = Properties("prop")
            conf.load(conffilepath)
        
        if (gconffilepath is not None and os.path.exists(gconffilepath)):
            # exec a python script and import it's config hash
            if (conf is None):
                if (gconffilepath.endswith(".py")):
                    conf = Configuration("py")
                else: # read properties file
                    conf = Properties("prop")
            conf.load(gconffilepath)
        configurator.register(conf, instrumentation)
    # assign/broadcast instrumentation change
    configurator.changeinstrumentation(instrumentation)
    return configurator


class ConfigurationError(Exception):
    """Error occurred when loading configuration."""


class Configuration(object):
    """Configuration data source interface."""
    name = property()
    def transform(name): pass
    def exists(name): pass
    def get(name, defaultvalue=None): pass
    def set(name, value): pass
    def check(name, value): pass
    def remove(name): pass
    def clear(): pass
    def load(input=None): pass
    def store(output=None): pass


class DefaultConfiguration(Configuration):
    """Base class defining a configuration data source backed by a dictionary."""

    def __init__(self, name="cfg", defaults=None):
        if (name is None or len(name) == 0):
            raise ValueError("name cannot be null or an empty string.")
        self._name = name
        self._filepath = None
        if (defaults is None):
            defaults = {}
        self._conf = defaults
        self._instrumentation = aptana.boot.Instrumentation(logging.getLogger(self.__class__.__name__))

    def __repr__(self):
        pp = pprint.PrettyPrinter()
        return self._name + "=" + pp.pformat(self._conf)

    def __str__(self):
        return self._name

    #
    # Dictionary interface:
    #

    def __getitem__(self, name):
        return self.get(name)

    def __setitem__(self, name, value):
        self.set(name, value)

    def __getattr__(self, name):
        """For attributes not found in self, redirect to the conf dictionary."""
        try:
            return self.__dict__[name]
        except KeyError:
            if (hasattr(self._conf, name)):
                return getattr(self._conf, name)

    #
    # attributes:
    #
    
    def getname(self):
        return self._name

    name = property(fget=getname, fset=None, fdel=None, doc="The name of the configuration data source.")

    def getfilepath(self):
        return self._filepath

    filepath = property(fget=getfilepath, fset=None, fdel=None, doc="The file path if this configuration was loaded from a file.")

    def getinstrumentation(self):
        return self._instrumentation

    def setinstrumentation(self, instrumentation):
        self._instrumentation.changeinstrumentation(instrumentation)

    instrumentation = property(fget=getinstrumentation, fset=setinstrumentation, fdel=None, doc=None)

    #
    # Configuration interface:
    #

    def transform(self, name):
        """Handler for transforming or normalizing the attribute name."""
        return name

    def exists(self, name):
        name = self.transform(name)
        exists = self._conf.has_key(name)
        self.instrumentation.say(self.name + ":exists(" + name + ") = " + str(exists))
        return exists

    def get(self, name, defaultvalue=None):
        name = self.transform(name)
        value = defaultvalue
        if (self.exists(name)):
            value = self._conf[name]
        self.instrumentation.say(self.name + ":get(" + name + ") = " + str(value))
        return value

    def set(self, name, value):
        name = self.transform(name)
        oldvalue = self._conf.pop(name, None)
        self.instrumentation.say(self.name + ":set(" + name + ", " + str(value) + ") = " + str(oldvalue))
        self._conf[name] = value
        return oldvalue

    def check(self, name, value):
        name = self.transform(name)
        check = False
        if (self.exists(name)):
            check = (self.get(name) == value)
        self.instrumentation.say(self.name + ":check(" + name + ", " + str(value) + ") = " + str(check))
        return check

    def remove(self, name):
        name = self.transform(name)
        oldvalue = self._conf.pop(key)
        self.instrumentation.say(self.name + ":remove(" + name + ", " + value + ") = " + str(oldvalue))
        return oldvalue

    def clear(self):
        self.instrumentation.say(self.name + ":clear()")
        self._conf.clear()

    def load(self, input=None):
        if (input is None):
            input = sys.stdin
        self.instrumentation.say(self.name + ":load(" + str(input) + ")")
        if (isinstance(input, file)):
            self._filepath = input.name
        else:
            self._filepath = input

        globals = {} # out hash of all global mappings evaluated from the file
        locals  = {} # out hash of all local mappings evaluated from the file
        execfile(input, globals, locals)
        # we have a hash of hash so take the config hash we are interested in
        if (locals.has_key(self._name)):
            self._conf = locals[self._name]
        else:
            # exact name not found so copy the entries from nested hashes (should only be one)
            for key in locals.keys():
                self._conf.update(locals[key])

    def store(self, output=None):
        if (output is None):
            output = sys.stdout
        self.instrumentation.say(self.name + ":store(" + str(output) + ")")
        opened = False
        try:
            if (not isinstance(output, file)):
                output = open(output, "w")
                opened = True
            output.write(repr(self)) # repr will write cfg={...}
            output.flush()
        finally:
            if (output is not None and opened):
                output.close()


class CommandLineConfiguration(DefaultConfiguration):
    """Command line option configuration using optparse."""

    def __init__(self, name="opts", defaults=None):
        opts = defaults
        if ((opts is not None) and (opts.__class__ == optparse.Values)):
            opts = opts.__dict__
        super(CommandLineConfiguration, self).__init__(name, opts)


class EnvironmentConfiguration(DefaultConfiguration):
    """Environment configuration copied from os.environ"""

    def __init__(self, name="env", defaults=None):
        env = defaults
        if (env is None):
            env = os.environ
        envcopy = {}
        for key, value in env.items():
            envcopy[key] = value
            envcopy[self.transform(key)] = value
        super(EnvironmentConfiguration, self).__init__(name, envcopy)

    def transform(self, name):
        """Transform environment style to configuration style. i.e.: PYTHON_HOME => python_home"""
        name = name.lower()
        return name
    

class Properties(DefaultConfiguration):
    """
    A Python replacement for java.util.Properties class
    This is modelled as closely as possible to the Java original.

    Created - Anand B Pillai <abpillai@gmail.com>    
    """

    def __init__(self, name="prop", defaults=None, escape=True):
        super(Properties, self).__init__(name, defaults)
        self._escape = escape

        # Dictionary of properties with 'pristine' keys
        # This is used for dumping the properties to a file
        # using the 'store' method
        self._origprops = {}

        # Dictionary mapping keys from property
        # dictionary to pristine dictionary
        self._keymap = {}

        self._othercharre = re.compile(r'(?<!\\)(\s*\=)|(?<!\\)(\s*\:)')
        self._othercharre2 = re.compile(r'(\s*\=)|(\s*\:)')
        self._bspacere = re.compile(r'\\(?!\s$)')
        self._curliesre = re.compile('[$]{.+?}')

    #
    # Dictionary interface:
    #

    def __getitem__(self, name):
        return self.getproperty(name)

    def __setitem__(self, name, value):
        self.setproperty(name, value)

    #
    # Properties interface:
    #

    def getpropertydict(self):
        return self._conf

    def _parse(self, lines):
        """Parse a list of lines and create an internal property dictionary.

        Every line in the file must consist of either a comment
        or a key-value pair. A key-value pair is a line consisting
        of a key which is a combination of non-white space characters
        The separator character between key-value pairs is a '=',
        ':' or a whitespace character not including the newline.
        If the '=' or ':' characters are found, in the line, even
        keys containing whitespace chars are allowed.

        A line with only a key according to the rules above is also
        fine. In such case, the value is considered as the empty string.
        In order to include characters '=' or ':' in a key or value,
        they have to be properly escaped using the backslash character.

        Some examples of valid key-value pairs:
        
        key     value
        key=value
        key:value
        key     value1,value2,value3
        key     value1,value2,value3 \
                value4, value5
        key
        This key= this value
        key = value1 value2 value3

        Any line that starts with a '#' is considerered a comment
        and skipped. Also any trailing or preceding whitespaces
        are removed from the key/value. """

        lineno = 0
        i = iter(lines)

        for line in i:
            lineno += 1
            line = line.strip()
            # Skip null lines
            if (not line):
                continue
            # Skip lines which are comments
            if (line[0] == "#"):
                continue
            # Some flags
            escaped = False
            # Position of first separation char
            sepidx = -1
            # A flag for performing wspace re check
            flag = 0
            # Check for valid space separation
            # First obtain the max index to which we
            # can search.
            m = self._othercharre.search(line)
            if (m):
                first, last = m.span()
                start, end = 0, first
                flag = 1
                wspacere = re.compile(r'(?<![\\\=\:])(\s)')        
            else:
                if (self._othercharre2.search(line)):
                    # Check if either '=' or ':' is present
                    # in the line. If they are then it means
                    # they are preceded by a backslash.

                    # This means, we need to modify the
                    # wspacere a bit, not to look for
                    # : or = characters.
                    wspacere = re.compile(r'(?<![\\])(\s)')        
                start, end = 0, len(line)

            m2 = wspacere.search(line, start, end)
            if (m2):
                #print 'Space match=>',line
                # Means we need to split by space.
                first, last = m2.span()
                sepidx = first
            elif (m):
                #print 'Other match=>',line
                # No matching wspace char found, need
                # to split by either '=' or ':'
                first, last = m.span()
                sepidx = last - 1
                #print line[sepidx]

            # If the last character is a backslash
            # it has to be preceded by a space in which
            # case the next line is read as part of the
            # same property
            while line[-1] == '\\':
                # Read next line
                nextline = i.next()
                nextline = nextline.strip()
                lineno += 1
                # This line will become part of the value
                line = line[:-1] + nextline

            # Now split to key,value according to separation char
            if (sepidx != -1):
                key, value = line[:sepidx], line[sepidx+1:]
            else:
                key, value = line, ''

            self._processpair(key, value)

    def _processpair(self, key, value):
        """Process a (key, value) pair."""
        oldkey = key
        oldvalue = value

        # Create key intelligently
        keyparts = self._bspacere.split(key)
        #print keyparts

        strippable = False
        lastpart = keyparts[-1]

        if (lastpart.find('\\ ') != -1):
            keyparts[-1] = lastpart.replace('\\', '')
        # If no backspace is found at the end, but empty
        # space is found, strip it
        elif (lastpart and lastpart[-1] == ' '):
            strippable = True

        key = ''.join(keyparts)
        if (strippable):
            key = key.strip()
            oldkey = oldkey.strip()

        if (self._escape):
            oldvalue = self.unescape(oldvalue)
            value = self.unescape(value)

        # Allow self referential ${var} properties to be interpolated.
        found = self._curliesre.findall(value)
        for f in found:
            srcKey = f[2:-1] # remove ${...}
            if (self._conf.has_key(srcKey)):
                value = value.replace(f, self._conf[srcKey], 1)

        self._conf[key] = value.strip()

        # Check if an entry exists in pristine keys
        if (self._keymap.has_key(key)):
            oldkey = self._keymap.get(key)
            self._origprops[oldkey] = oldvalue.strip()
        else:
            self._origprops[oldkey] = oldvalue.strip()
            # Store entry in keymap
            self._keymap[key] = oldkey

    def escape(self, value):
        """Java escapes the '=' and ':' in the value string with backslashes in the store method.
           So let us do the same."""
        newvalue = value.replace(':', '\:')
        newvalue = newvalue.replace('=', '\=')
        return newvalue

    def unescape(self, value):
        """Reverse of escape."""
        newvalue = value.replace('\:', ':')
        newvalue = newvalue.replace('\=', '=')
        return newvalue

    def getproperty(self, key, defaultvalue=None):
        """Return a property for the given key."""
        return self._conf.get(key, defaultvalue)

    def setproperty(self, key, value):
        """Set the property for the given key."""
        if (isinstance(key, str) and isinstance(value, str)):
            self._processpair(key, value)
        else:
            raise TypeError("Both key and value should be strings.")

    def propertynames(self):
        """Return an iterator over all the keys of the property dictionary, i.e the names of the properties."""
        return self._conf.keys()

    def load(self, input):
        """Load properties from an open file stream or from a file path."""
        opened = False
        try:
            if (not isinstance(input, file)):
                input = open(input, "r")
                opened = True
            self.instrumentation.say(self.name + ":load(" + str(input) + ")")
            self._parse(input.readlines())
        finally:
            if (input is not None and opened):
                input.close()

    def store(self, output=None, comments=None, writetimestamp=True):
        """Write the properties list to the stream 'output' along with the optional 'comments'."""
        if (output is None):
            output = sys.stdout
        opened = False
        if (not isinstance(output, file)):
            output = open(output, "w")
            opened = True
        self.instrumentation.say(self.name + ":store(" + str(output) + ")")
        try:
            # Write comments
            if (comments is not None):
                output.write(''.join(('#', comments, os.linesep)))
            # Write timestamp
            if (writetimestamp):
                tstamp = time.strftime('%a %b %d %H:%M:%S %Z %Y', time.localtime())
            output.write(''.join(('#', tstamp, os.linesep)))
            # Write properties from the pristine dictionary
            for prop, val in self._origprops.items():
                if (self._escape):
                    newval = self.escape(val)
                else:
                    newval = val
                output.write(''.join((prop, '=', newval, os.linesep)))
            output.flush()
        finally:
            if (output is not None and opened):
                output.close()

    def list(self, output=None):
        """Prints a listing of the properties to the stream 'output' which defaults to the standard output."""
        if (output is None):
            output = sys.stdout
        self.instrumentation.say(self.name + ":list(" + str(output) + ")")
        output.write('-- listing properties --' + os.linesep)
        for key, value in self._conf.items():
            output.write(''.join((key, '=', value, os.linesep)))


class Configurator(DefaultConfiguration, aptana.boot.Instrumented):
    """Configurator acts as a registry for managing different configuration data sources.
       It chooses the lookup order for each get/set to facilitate overrides."""

    def __init__(self, name="configurator"):
        self._name = name
        self._instrumentation = aptana.boot.Instrumentation(logging.getLogger(self.__class__.__name__))
        # registry of all configuration data sources
        self._registry = {}
        # define registration and lookup order for configuration data sources
        self._registryorder = []
        # define lookup order for configuration attribute queries
        self._queryorder = []

        # only use dist name if it's defined on this platform
        if (osdistname() is not None and len(osdistname()) > 0):
            # <platformbits>.<distname>.<key>=<value> => linux64.redhat.tmp=/tmp
            self._queryorder.append(osplatform() + str(osbits()) + "." + osdistname() + ".")
            # <platform>.<distname>.<key>=<value> => linux.redhat.tmp=/tmp
            self._queryorder.append(osplatform() + "." + osdistname() + ".")
        # <platformbits>.<key>=<value> => linux64.tmp=/tmp
        self._queryorder.append(osplatform() + str(osbits()) + ".")
        # <platform>.<key>=<value> => linux.tmp=/tmp
        self._queryorder.append(osplatform() + ".")
        # <key>=<value> => tmp=/tmp
        self._queryorder.append("")
 
        # prepend <hostname>. to all queries to allow for host specific config
        hostname = socket.gethostname()
        reversedqueryorder = list(self._queryorder)
        reversedqueryorder.reverse()
        for query in reversedqueryorder:
            self._queryorder.insert(0, hostname + "." + query)

    def __repr__(self):
        #REVIEW: this is not the most efficient way to concat
        buffer = os.linesep
        for conf in self._registryorder:
            buffer += repr(conf) + os.linesep + os.linesep
        return buffer

    def __str__(self):
        return self.__repr__()

    #
    # Dictionary interface:
    #

    def __setitem__(self, name, value):
        """immutable"""

    def __getattr__(self, name):
        """For attributes not found in self, check each conf data source."""
        try:
            return self.__dict__[name]
        except KeyError:
            if (self.exists(name)):
                return self.get(name)

    #
    # attributes:
    #

    def getname(self):
        return self._name

    name = property(fget=getname, fset=None, fdel=None, doc=None)

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
        self._instrumentation.changeinstrumentation(instrumentation)
        for conf in self._registryorder:
            conf.instrumentation.changeinstrumentation(instrumentation)

    def flush(self):
        self._instrumentation.flush()
        for conf in self._registryorder:
            conf.instrumentation.flush()

    def say(self, *args, **kwargs):
        self._instrumentation.say(*args, **kwargs)

    #
    # Registry interface:
    #

    def lookup(self, name):
        conf = None
        if (self._registry.has_key(name)):
            conf = self._registry[name]
        return conf

    def register(self, conf, instrumentation=None):
        conf.instrumentation.changeinstrumentation(instrumentation)
        self._registry[conf.name] = conf
        self._registryorder.append(conf)

    def unregister(self, name):
        conf = self._registry[name]
        self._registryorder.remove(conf)
        self._registry.pop(name)

    #
    # Configuration interface:
    #

    def exists(self, name):
        for conf in self._registryorder:
            for query in self._queryorder:
                exists = conf.exists(query + name)
                if (exists):
                    return True
        return False

    def get(self, name):
        for conf in self._registryorder:
            for query in self._queryorder:
                value = conf.get(query + name)
                if (value != None):
                    return value
        return None

    def set(self, name, value):
        """immutable"""

    def remove(self, name):
        """immutable"""

    def clear(self):
        """immutable"""

    def load(self, input):
        """immutable"""

    def store(self, output=None):
        for conf in self._registryorder:
            conf.store(output)

    
def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    print osdump()

    propfilepath = "test.properties"
    props = Properties()
    props.setproperty("abc", "123")
    props.setproperty("url", "http://www.aptana.com")
    props["dict"] = "access"
    props.store(propfilepath)
    print props.getpropertydict()

    loaded = Properties()
    loaded.load(propfilepath)
    loaded.list()
    print loaded.getpropertydict()

    os.remove(propfilepath)

#    conf = DefaultConfiguration("cfg")
#    conf.load("build-settings.py")
#    print conf
#    print repr(conf)
#    conf.store("conftest.py")
#
#    confreloaded = Configuration("z")
#    confreloaded.load("conftest.py")
#    print confreloaded
#
#    os.remove("conftest.py")

    envconf = EnvironmentConfiguration("env")
    print repr(envconf)

    #logging.basicConfig()

    configurator = Configurator()
#    #configurator.register(props)
#    configurator.register(conf)
    configurator.register(envconf)
    print configurator.get("SHELL")

if (__name__ == "__main__"):
    main()
