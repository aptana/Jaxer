CXX=g++
CC=gcc

BP_SRC_ROOT=../../..
BP_CLIENT_DIR=$(BP_SRC_ROOT)/client
BP_HANDLER_PDIR=$(BP_CLIENT_DIR)/mac/handler
BP_COMMON_DIR=$(BP_SRC_ROOT)/common
BP_COMMON_PDIR=$(BP_COMMON_DIR)/mac

CXXFLAGS=-Wall -D_REENTRANT \
	-arch ppc\
	-mmacosx-version-min=10.4 \
	-DOSTYPE=\"Darwin\" -DOSARCH=Darwin \
	-I$(BP_SRC_ROOT) \
	-I$(BP_COMMON_PDIR) \
	-I. \
	-isysroot /Developer/SDKs/MacOSX10.4u.sdk \
	-I/Developer/SDKs/MacOSX10.4u.sdk/Developer/Headers/FlatCarbon  \
	-DNDEBUG -O2 

LDFLAGS= -lcrypto \
	-framework Foundation 

OBJ_DIR=.
BIN_DIR=.

LIB_CC_SRC= $(BP_HANDLER_PDIR)/exception_handler.cc \
	$(BP_HANDLER_PDIR)/minidump_generator.cc \
	$(BP_HANDLER_PDIR)/dynamic_images.cc \
	$(BP_COMMON_PDIR)/string_utilities.cc \
	$(BP_COMMON_PDIR)/file_id.cc \
	$(BP_COMMON_PDIR)/macho_id.cc \
	$(BP_COMMON_PDIR)/macho_utilities.cc \
	$(BP_COMMON_PDIR)/macho_walker.cc \
	$(BP_CLIENT_DIR)/minidump_file_writer.cc \
	$(BP_COMMON_DIR)/string_conversion.cc

LIB_C_SRC= $(BP_COMMON_DIR)/convert_utf.c \
	$(BP_COMMON_DIR)/md5.c

LIB_CC_OBJ=$(patsubst %.cc,$(OBJ_DIR)/%.o,$(LIB_CC_SRC))
LIB_C_OBJ=$(patsubst %.c,$(OBJ_DIR)/%.o,$(LIB_C_SRC))

LIB_A = libmac.a

DUMP_SYMS_OBJ= $(LIB_A) \
    dump_syms_tool.m \
	$(BP_COMMON_PDIR)/dump_syms.mm

BIN=$(BIN_DIR)/dump_syms

.PHONY:all clean

all:$(BIN)

$(BIN_DIR)/dump_syms:$(DUMP_SYMS_OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(LIB_A):$(LIB_CC_OBJ) $(LIB_C_OBJ)
	ar -cr $(LIB_A) $(LIB_CC_OBJ) $(LIB_C_OBJ)

clean:
	rm -f $(BIN) $(LIB_A) *.o *.dmp core

