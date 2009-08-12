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
#pragma once

#include <stdio.h>
#include "jaxer_log.h"
#define MAX_LINE_LENGTH 256

struct SL
{
    char str[32];
    SL* next;
    SL(const char*s=0, SL* n=0)
        :next(n)
    {
        str[0] = 0;
        if(s)
        {
            strcpy(str, s);
        }
    };
    ~SL(){};
};

class CJaxer_config
{
public:
    CJaxer_config(const char* filename = 0);
    ~CJaxer_config(void);
    BOOL LoadConfig();
    BOOL SetConfigValue(const char* name, const char *value);
    BOOL SetLogFileName(const char* v);
    BOOL SetLogFileBackups(const char* v);
    BOOL SetLogFileSizeKB(const char* v);
    BOOL SetVirtualDirName(const char* v);
    BOOL SetVirtualDirPath(const char* v);
    BOOL AddFilterExts(const char* v);
    BOOL AddFilterCTs(const char* v);
    BOOL SetJaxerServerIP(const char* v);
    BOOL SetJaxerServerPort(const char* v);
    BOOL SetLogLevel(const char* v);
    void SetConfigFilename(const char* v);

    const char* GetLogFileName() const {return m_LogFileName;}
    int GetLogFileBackups() const { return m_LogFileBackups;}
    int GetLogFileSizeKB() const { return m_LogFileSizeKB;}
    eLogLevel GetLogLevel() const { return m_LogLevel;}
    const char *GetJaxerServerIP() const {return m_JaxerServerIP;}
    int GetJaxerServerPort() const { return m_JaxerServerPort;}

    BOOL ShouldFilterExt(const char* ext);
    BOOL ShouldFilterCT(const char* ct);
    BOOL IsURLMine(const char* ext);
    const char* GetVirtualDirPath() const { return m_VirtualDirPath;}
    const char* GetVirtualDirName() const { return m_VirtualDirName;}

private:
    BOOL ReadProperty(const char *str);

    char m_filename[2*MAX_PATH];
    // HeaderMap m_config_map;
    FILE* m_fp;
    SL* m_FilterExtsHeader;
    SL* m_FilterExtsTail;

    SL* m_FilterCTsHeader;
    SL* m_FilterCTsTail;

    char m_LogFileName[2*MAX_PATH];
    int m_LogFileBackups;
    int m_LogFileSizeKB;
    char  m_VirtualDirName[MAX_PATH];
    char m_VirtualDirPath[2*MAX_PATH];
    char m_JaxerServerIP[32];
    int m_JaxerServerPort;
    eLogLevel m_LogLevel;
};

CJaxer_config& GetJaxerConfig();


