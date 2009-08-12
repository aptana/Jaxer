#! /bin/sh
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

#BP_SRC_ROOT=../../google/google_breakpad/src
BP_SRC_ROOT=../../..
BP_HANDLER_PDIR=$BP_SRC_ROOT/client/mac/handler
BP_COMMON_DIR=$BP_SRC_ROOT/common
BP_COMMON_PDIR=$BP_COMMON_DIR/mac
BP_CLIENT_DIR=$BP_SRC_ROOT/client

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

set -x

    AR_ARGS="cr libmac.a"

	for afile in $BP_SRC_CC
	do
		g++ -o $afile.o -c \
		-mmacosx-version-min=10.4 \
		-DOSTYPE=\"Darwin\" -DOSARCH=Darwin \
		-I$BP_SRC_ROOT \
		-isysroot /Developer/SDKs/MacOSX10.4u.sdk \
		-I/Developer/SDKs/MacOSX10.4u.sdk/Developer/Headers/FlatCarbon  \
        -DNDEBUG -O2  \
		$afile.cc
        AR_ARGS="$AR_ARGS $afile.o"
	done
    for bfile in $BP_SRC_C
    do
		g++ -o $bfile.o -c \
		-mmacosx-version-min=10.4 \
		-DOSTYPE=\"Darwin\" -DOSARCH=Darwin \
		-I$BP_SRC_ROOT \
		-isysroot /Developer/SDKs/MacOSX10.4u.sdk \
		-I/Developer/SDKs/MacOSX10.4u.sdk/Developer/Headers/FlatCarbon  \
        -DNDEBUG -O2  \
		$bfile.c
        AR_ARGS="$AR_ARGS $bfile.o"
    done

	ar $AR_ARGS
	
	g++ -mmacosx-version-min=10.4 -o dump_syms \
	-O2 \
    -DOSTYPE=\"Darwin\" -DOSARCH=Darwin \
	-I$BP_SRC_ROOT \
	-I$BP_COMMON_PDIR \
	-I. \
	-isysroot /Developer/SDKs/MacOSX10.4u.sdk -framework Foundation \
    -lcrypto\
    dump_syms_tool.m \
	$BP_SRC_ROOT/common/mac/dump_syms.mm \
    libmac.a

