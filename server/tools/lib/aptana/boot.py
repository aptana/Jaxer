#!/usr/bin/env python
import os
import logging
import sys
import time

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

__version__ = "$Revision: 3363 $"[11:-2]


class InstrumentationListener(object):
    def changeinstrumentation(instrumentation): pass


class Instrumented(InstrumentationListener):
    """Interface for logging and monitoring instrumentation."""
    logger = property()
    def changeinstrumentation(instrumentation): pass
    def flush(): pass
    def say(*args, **kwargs): pass
    def record(severity, message): pass


class Instrumentation(Instrumented):
    """Base class defining logging and monitoring instrumentation."""

    def __init__(self, logger=None):
        if (logger is None):
            logger = logging.getLogger() # root logger
        self._logger = logger
        self._eventlog = EventLog(logger.name)

    def __str__(self):
        return "%s: logname=%s, loglevel=%s, logger=%s, " % (self.__class__.__name__, self.logger.name, logging.getLevelName(self.logger.level), self.logger)

    #
    # attributes:
    #

    def getlogger(self):
        return self._logger

    def setlogger(self, logger):
        if (logger is None):
            logger = logging.getLogger() # root logger
        self._logger = logger

    logger = property(fget=getlogger, fset=setlogger, fdel=None, doc=None)

    def geteventlog(self):
        return self._eventlog

    eventlog = property(fget=geteventlog, fset=None, fdel=None, doc=None)

    #
    # Instrumented interface:
    #

    def changeinstrumentation(self, instrumentation):
        if (instrumentation is None):
            return
        self.logger.setLevel(instrumentation.logger.level)

    def flush(self):
        for handler in self.logger.handlers:
            handler.flush()
        # flush the root logger
        for handler in logging.getLogger().handlers:
            handler.flush()

    def say(self, *args, **kwargs):
        self.logger.debug(*args, **kwargs)

    def record(self, severity, message):
        self.eventlog.addevent(severity, message)


class EventLog(object):
    """Active event history."""
    def __init__(self, name):
        if (name is None or len(name) == 0):
            raise ValueError("Event log name cannot be None or an empty string.")
        self._name = name
        self._events = []

    def __str__(self):
        buffer = ""
        for event in self._events:
            buffer += str(event) + os.linesep
        return buffer

    def getevents(self):
        return self._events

    events = property(fget=getevents, fset=None, fdel=None, doc=None)

    def addevent(self, severity, message):
        if (severity is None or severity not in logging._levelNames):
            raise ValueError("Severity must be a valid log level.")
        id = len(self._events)
        entry = EventLogEntry(self._name, id, severity, message)
        self._events.append(entry)

    def clearevents(self):
        self._events = []


class EventLogEntry(object):
    """An entry within the log describing the event."""
    def __init__(self, source, id, severity, message):
        self._timestamp = time.time()
        self._source = source
        self._id = id
        self._severity = severity
        self._message = message

    def __str__(self):
        timetuple = time.localtime(self._timestamp)
        timestr = time.strftime("%Y%m%d-%H:%M:%S", timetuple)
        return "[%s]:(%s):{%s}:%s: %s" % (timestr, self._source, self._id, self._severity, self._message)

    def gettimestamp(self):
        return self._timestamp

    timestamp = property(fget=gettimestamp, fset=None, fdel=None, doc=None)

    def getsource(self):
        return self._source

    source = property(fget=getsource, fset=None, fdel=None, doc=None)

    def getid(self):
        return self._id

    def setid(self, id):
        self._id = id

    id = property(fget=getid, fset=setid, fdel=None, doc=None)

    def getseverity(self):
        return self._severity

    def setseverity(self, severity):
        self._severity = severity

    severity = property(fget=getseverity, fset=setseverity, fdel=None, doc=None)

    def getmessage(self):
        return self._message

    def setmessage(self, message):
        self._message = message

    message = property(fget=getmessage, fset=setmessage, fdel=None, doc=None)


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    logging.basicConfig()
    logging.getLogger().setLevel(logging.DEBUG)

    obj = Instrumentation()
    obj.logger = logging.getLogger() # root logger
    obj.say("hello")
    obj.flush()
    obj.record("INFO",  "burn the tape")
    obj.record("ERROR", "and this one also")
    print obj
    print obj.eventlog

if (__name__ == "__main__"):
    main()