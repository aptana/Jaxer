#!/bin/sh
# ***** BEGIN LICENSE BLOCK *****
#  Version: GPL 3
# 
#  This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
#  This program is licensed under the GNU General Public license, version 3 (GPL).
# 
#  This program is distributed in the hope that it will be useful, but
#  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
#  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
#  is prohibited.
# 
#  You can redistribute and/or modify this program under the terms of the GPL, 
#  as published by the Free Software Foundation.  You should
#  have received a copy of the GNU General Public License, Version 3 along
#  with this program; if not, write to the Free Software Foundation, Inc., 51
#  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
#  
#  Aptana provides a special exception to allow redistribution of this file
#  with certain other code and certain additional terms
#  pursuant to Section 7 of the GPL. You may view the exception and these
#  terms on the web at http://www.aptana.com/legal/gpl/.
#  
#  You may view the GPL, and Aptana's exception and additional terms in the file
#  titled license-jaxer.html in the main distribution folder of this program.
#  
#  Any modifications to this file must keep this entire header intact.
# 
# ***** END LICENSE BLOCK *****

# To generate symbols:
#   $0 GENERATE_SYMBOLS=1
# Otherwise just
#   $0
#
#

SYMB_FLAGS=""

if [[ $# -gt 0 && "$1" = "GENERATE_SYMBOLS=1" ]]
then
    SYMB_FLAGS="-g -gstabs+"
fi

#g++ -arch ppc -arch i386 -mmacosx-version-min=10.4 \
#-isysroot /Developer/SDKs/MacOSX10.4u.sdk \
#-I../../libevent/libevent-1.4.2-rc \
#-L../../libevent/libevent-1.4.2-rc/.libs \
#-levent \
#-o JaxerManager \
#jaxermanager.cpp

BP_SRC_ROOT=../../google/google_breakpad/src
BP_HANDLER_PDIR=$BP_SRC_ROOT/client/mac/handler
BP_COMMON_DIR=$BP_SRC_ROOT/common
BP_COMMON_PDIR=$BP_COMMON_DIR/mac
BP_CLIENT_DIR=$BP_SRC_ROOT/client

#DUMP_SYMS=$BP_SRC_ROOT/tools/mac/dump_syms/dump_syms

BP_SRC_CC="
$BP_HANDLER_PDIR/exception_handler
$BP_HANDLER_PDIR/minidump_generator
$BP_HANDLER_PDIR/dynamic_images
$BP_COMMON_PDIR/string_utilities
$BP_COMMON_PDIR/file_id
$BP_COMMON_PDIR/macho_id
$BP_COMMON_PDIR/macho_utilities
$BP_COMMON_PDIR/macho_walker
$BP_CLIENT_DIR/minidump_file_writer
$BP_COMMON_DIR/string_conversion
"

BP_SRC_C="
$BP_COMMON_DIR/convert_utf
$BP_COMMON_DIR/md5
"


ARCHS="i386 ppc"

LIPO_ARGS="-create -output jaxermanager"

for ARCH in $ARCHS
do
    LIPO_ARGS="$LIPO_ARGS -arch $ARCH jaxermanager.$ARCH"
    AR_ARGS="-cr lib$ARCH.a"

	for afile in $BP_SRC_CC
	do
		g++ -arch $ARCH -o $afile.o -c \
		-mmacosx-version-min=10.4 \
		-DOSTYPE=\"Darwin\" -DOSARCH=Darwin \
		-I$BP_SRC_ROOT \
		-fno-exceptions \
		-isysroot /Developer/SDKs/MacOSX10.4u.sdk -fno-strict-aliasing \
		-fpascal-strings -fno-common -fshort-wchar -pthread \
		-I/Developer/SDKs/MacOSX10.4u.sdk/Developer/Headers/FlatCarbon  -DNDEBUG -O2  \
		$afile.cc
        AR_ARGS="$AR_ARGS $afile.o"
	done
    for bfile in $BP_SRC_C
    do
		g++ -arch $ARCH -o $bfile.o -c \
		-mmacosx-version-min=10.4 \
		-DOSTYPE=\"Darwin\" -DOSARCH=Darwin \
		-I$BP_SRC_ROOT \
		-fno-exceptions \
		-isysroot /Developer/SDKs/MacOSX10.4u.sdk -fno-strict-aliasing \
		-fpascal-strings -fno-common -fshort-wchar -pthread \
		-I/Developer/SDKs/MacOSX10.4u.sdk/Developer/Headers/FlatCarbon  -DNDEBUG -O2  \
		$bfile.c
        AR_ARGS="$AR_ARGS $bfile.o"
    done

	ar $AR_ARGS
	
	g++ -arch $ARCH -mmacosx-version-min=10.4 -o jaxermanager.$ARCH \
	-isysroot /Developer/SDKs/MacOSX10.4u.sdk -framework Cocoa \
	$SYMB_FLAGS \
	-framework Carbon \
	-I$BP_SRC_ROOT \
	-I../../libevent/libevent-1.4.2-rc \
	-L../../libevent/libevent-1.4.2-rc/.libs \
	-levent \
	-DOSTYPE=\"Darwin\" -DOSARCH=Darwin \
	-I.  \
	-fPIC  \
	-fno-exceptions -Wall \
	-fno-strict-aliasing \
	-fpascal-strings -fno-common -fshort-wchar -pthread \
	-I/Developer/SDKs/MacOSX10.4u.sdk/Developer/Headers/FlatCarbon  -DNDEBUG -O2  \
	-L/Developer/SDKs/MacOSX10.4u.sdk/usr/lib \
	-lcrypto\
	jaxermanager.cpp \
	lib$ARCH.a

	#$DUMP_SYMS -a $ARCH jaxermanager.$ARCH > jaxermanager.$ARCH.sym
	#strip -S jaxermanager.$ARCH
done

lipo $LIPO_ARGS

