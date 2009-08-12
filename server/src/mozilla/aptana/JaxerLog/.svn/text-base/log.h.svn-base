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
#ifndef __JAXER_LOG_H__
#define __JAXER_LOG_H__
#define MAX_BUF_SIZE 0x8000

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else

#define HANDLE int
#define INVALID_HANDLE_VALUE -1
#define CloseHandle close
#define DWORD int
#define BOOL bool
#define TCHAR char
#define TRUE true
#define FALSE false
#define _tcslen strlen
#define _tcscpy strcpy
#define _tcsncmp strncmp
#define _istspace isspace
#define _tstoi atoi
#define _pclose pclose

#endif

#ifndef PATH_MAX
#define PATH_MAX (FILENAME_MAX * 50)
#endif

enum eLogLevel
{
  eTRACE = 0,
  eDEBUG,
  eINFO,
  eWARN,
  eERROR,
  eFATAL,
  eNOTICE
};

static char *gsLogLevel[10] =
{
  "TRACE",
  "DEBUG",
  "INFO",
  "WARN",
  "ERROR",
  "FATAL",
  "INFO",
  NULL
};

#ifdef _DEBUG_JXLOG
#ifdef _WIN32
#define JAXER_DEBUG_DUMP_FILE "c:\\jaxer_debug.log"
#else
#define JAXER_DEBUG_DUMP_FILE "/tmp/jaxer_debug.log"
#include <stdarg.h>
#endif


// Simple function for debugging.
static void logme(char* fmt, ...)
{
  char buf[1024];
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  vsprintf(buf, fmt, arg_ptr);
  va_end(arg_ptr);

  time_t rawtime;
  struct tm* tinfo;
  time( &rawtime);
  tinfo = localtime(&rawtime);
  FILE *fp = fopen(JAXER_DEBUG_DUMP_FILE, "a");
  fprintf(fp, "%02d:%02d:%02d %s\n",
    tinfo->tm_hour, tinfo->tm_min, tinfo->tm_sec, buf);
  fflush(fp);
  fclose(fp);
}
#else
static void logme(char* fmt, ...){} 
#endif //DEBUG

#endif

