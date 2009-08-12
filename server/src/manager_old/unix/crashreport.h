/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set sw=4 ts=4 et: */
/* ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 * 
 *  This program is distributed in the hope that it will be useful, but
 *  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 *  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 *  is prohibited.
 * 
 *  You can redistribute and/or modify this program under the terms of the GPL, 
 *  as published by the Free Software Foundation.  You should
 *  have received a copy of the GNU General Public License, Version 3 along
 *  with this program; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *  
 *  Aptana provides a special exception to allow redistribution of this file
 *  with certain other code and certain additional terms
 *  pursuant to Section 7 of the GPL. You may view the exception and these
 *  terms on the web at http://www.aptana.com/legal/gpl/.
 *  
 *  You may view the GPL, and Aptana's exception and additional terms in the file
 *  titled license-jaxer.html in the main distribution folder of this program.
 *  
 *  Any modifications to this file must keep this entire header intact.
 * 
 * ***** END LICENSE BLOCK ***** */
#ifndef __JAXER_CRASHREPORT_H__
#define __JAXER_CRASHREPORT_H__

#ifdef __sun__
#include "client/solaris/handler/exception_handler.h"
#include "client/solaris/handler/solaris_lwp.h"
#else
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include "client/mac/handler/exception_handler.h"
#include "client/mac/handler/minidump_generator.h"
#else
#include "client/linux/handler/exception_handler.h"
#include "client/linux/handler/linux_thread.h"
#endif // LINUX
#endif // __sun__


namespace google_breakpad {

#if 0
static int kCustomInfoCount = 2;
static CustomInfoEntry kCustomInfoEntries[] = {
    CustomInfoEntry(L"prod", L"JaxerManager"),
    CustomInfoEntry(L"ver", JAXER_BUILD_ID_L),
};

static CustomClientInfo custom_info = {kCustomInfoEntries, kCustomInfoCount};
#endif

static bool BeforeDumpCallback(void* context)
{
    return true;
}

static bool AfterDumpCallback(const char* dump_path,
                     const char* minidump_id,
                     void* context,
                     bool succeeded)
{
    // We cannot write a log msg as it is implemeted now, as that might trig
    // another exception.  Just write to stderr for now.
    if (succeeded)
        fprintf(stderr, "Generated crash Dump (%s/%s.dmp).\r\n", dump_path, minidump_id);
    else
        fprintf(stderr, "Failed to generate a trash dump.\r\n");

    // This is a little risky here ...
    SlaughterAllServers(true);
    lc.Terminate();
    lsi.Terminate();
  
    return succeeded;
}

} // namespace google_breakpad

#endif // __JAXER_CRASHREPORT_H__
