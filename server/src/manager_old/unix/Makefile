#This makefile is intended to replace the platform-specific ones.
#
# Supported platforms:
#	SunOS, Linux (32bit & 64bit)
#
# To enable symbols:
# [g]make GENERATE_SYMBOLS=1 -f <This_file>
#

CPPFLAGS=
CXXFLAGS=
LDFLAGS=
IS_SUPPORTED=

BP_SRC_ROOT=../../google/google_breakpad/src
BP_CLIENT=$(BP_SRC_ROOT)/client
BP_COMMON=$(BP_SRC_ROOT)/common

OS_ARCH := $(subst /,_,$(shell uname -s))
OS_TEST := $(shell uname -m)

ifeq ($(OS_ARCH),SunOS)
  CC=cc
  CXX=gcc
  PLATFORM_NAME = solaris
  CPPFLAGS += -D_REENTRANT -DSOLARIS
  LDFLAGS += -lpthread -lssl -lelf -lstdc++ -lxnet
  SYMFLAGS = -g -gstabs+
  IS_SUPPORTED=1
else #OS_ARCH != SUNOS
ifeq ($(OS_ARCH), Linux)
  CXX=g++
  CC=gcc
  PLATFORM_NAME = linux
ifeq ($(OS_TEST), x86_64)
  CPPFLAGS += -DLINUX_64
  LDFLAGS += -lrt
else #OS_TEST != x86_64
  CPPFLAGS += -D_REENTRANT
  LDFLAGS += -lpthread -lrt
  SYMFLAGS = -g -gstabs
endif #OS_TEST == x86_64
  IS_SUPPORTED=1
endif #OS_ARCH == Linux
endif #OS_ARCH == SunOS

ifneq ($(IS_SUPPORTED), 1)
  echo "Unsupported OS_ARCH $(OS_ARCH)"
  exit 1
endif

BP_COMMON_PLATFORM=$(BP_COMMON)/$(PLATFORM_NAME)
BP_PLATFORM_HANDLER=$(BP_CLIENT)/$(PLATFORM_NAME)/handler

CPPFLAGS += -I$(BP_SRC_ROOT) \
	-I../../libevent/libevent-1.4.2-rc
	
CFLAGS += -I$(BP_SRC_ROOT)

LDFLAGS += -levent \
  -L../../libevent/libevent-1.4.2-rc/.libs

ifdef GENERATE_SYMBOLS
ifneq ($(OS_TEST), x86_64)
  #CPPFLAGS += $(SYMFLAGS)
  #CFLAGS += $(SYMFLAGS)
  LDFLAGS += $(SYMFLAGS)
endif
endif

OBJ_DIR=.
BIN_DIR=.

DEPEND_CC_SRC =
DEPEND_C_SRC =
ifeq ($(OS_ARCH),SunOS)
  DEPEND_CC_SRC=$(BP_PLATFORM_HANDLER)/solaris_lwp.cc \
    $(BP_CLIENT)/minidump_file_writer.cc\
	  $(BP_COMMON)/string_conversion.cc\
	  $(BP_COMMON_PLATFORM)/file_id.cc\
	  $(BP_PLATFORM_HANDLER)/minidump_generator.cc \
	  $(BP_PLATFORM_HANDLER)/exception_handler.cc\
	  $(BP_COMMON_PLATFORM)/guid_creator.cc
	  
  DEPEND_C_SRC = $(BP_COMMON)/convert_UTF.c
  MD5_SRC = $(BP_COMMON)/md5.c
endif
ifeq ($(OS_ARCH),Linux)
ifneq ($(OS_TEST), x86_64)
  DEPEND_CC_SRC=$(BP_PLATFORM_HANDLER)/linux_thread.cc \
    $(BP_SRC_ROOT)/client/minidump_file_writer.cc\
	  $(BP_COMMON)/string_conversion.cc\
	  $(BP_COMMON_PLATFORM)/file_id.cc\
	  $(BP_PLATFORM_HANDLER)/minidump_generator.cc \
    $(BP_PLATFORM_HANDLER)/exception_handler.cc\
	  $(BP_COMMON_PLATFORM)/guid_creator.cc
	  
  DEPEND_C_SRC=$(BP_COMMON)/convert_UTF.c
  MD5_SRC = $(BP_COMMON)/md5.c
endif
endif

MANAGER_SRC=jaxermanager.cpp

DEPEND_CC_OBJ = $(patsubst %.cc,%.o,$(DEPEND_CC_SRC))
DEPEND_C_OBJ  = $(patsubst  %.c,%.o,$(DEPEND_C_SRC))


MANAGER_OBJ= $(MANAGER_SRC)\
          $(DEPEND_CC_OBJ) $(DEPEND_C_OBJ) $(MD5_SRC)

BIN=$(BIN_DIR)/jaxermanager

.PHONY:all clean

all:$(BIN)

$(BIN_DIR)/jaxermanager:$(MANAGER_OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@


  	
clean:
	rm -f $(BIN) $(DEPEND_CC_OBJ) $(DEPEND_C_OBJ) *.o *.out *.dmp core


