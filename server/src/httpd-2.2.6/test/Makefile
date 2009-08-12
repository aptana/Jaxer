top_srcdir   = /Users/paul/Documents/Jaxer-dev/httpd-2.2.6
top_builddir = /Users/paul/Documents/Jaxer-dev/httpd-2.2.6
srcdir       = /Users/paul/Documents/Jaxer-dev/httpd-2.2.6/test
builddir     = /Users/paul/Documents/Jaxer-dev/httpd-2.2.6/test
VPATH        = /Users/paul/Documents/Jaxer-dev/httpd-2.2.6/test

# no targets: we don't want to build anything by default. if you want the
# test programs, then "make test"
TARGETS =

PROGRAMS =

PROGRAM_LDADD        = $(EXTRA_LDFLAGS) $(PROGRAM_DEPENDENCIES) $(EXTRA_LIBS)
PROGRAM_DEPENDENCIES =  \
	$(top_srcdir)/srclib/apr-util/libaprutil.la \
	$(top_srcdir)/srclib/apr/libapr.la

include $(top_builddir)/build/rules.mk

test: $(PROGRAMS)

# example for building a test proggie
# dbu_OBJECTS = dbu.lo
# dbu: $(dbu_OBJECTS)
#	$(LINK) $(dbu_OBJECTS) $(PROGRAM_LDADD)
