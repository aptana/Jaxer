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
#include "stdafx.h"
#include "jaxer_config.h"
#include <stdlib.h>


CJaxer_config::CJaxer_config(const char* filename)
:m_LogFileBackups(10)
,m_LogFileSizeKB(50)
,m_JaxerServerPort(4327)
,m_LogLevel(eINFO)
{
    m_VirtualDirName[0] = 0;
    m_VirtualDirPath[0] = 0;
    if (filename)
        strcpy(m_filename, filename);
    else
        m_filename[0] = 0;

    m_fp = 0;

    m_FilterExtsHeader = new SL();
    m_FilterExtsTail = m_FilterExtsHeader;

    m_FilterCTsHeader = new SL();
    m_FilterCTsTail = m_FilterCTsHeader;

    strcpy(m_JaxerServerIP, "127.0.0.1");
}

CJaxer_config::~CJaxer_config(void)
{
    if (m_fp)
    {
        fclose(m_fp);
    }

    while(m_FilterExtsHeader)
    {
        SL *p = m_FilterExtsHeader->next;
        m_FilterExtsHeader = m_FilterExtsHeader->next;
        delete p;
    }
}

void CJaxer_config::SetConfigFilename(const char* v)
{
    strcpy(m_filename, v);
}

BOOL CJaxer_config::LoadConfig()
{
    BOOL rc = FALSE;

    if (m_filename && *m_filename)
    {
        FILE *fp = fopen(m_filename, "r");
        if (fp)
        {
            char buf[MAX_LINE_LENGTH + 1];
            char *prp;

            rc = TRUE;

            while (NULL != (prp = fgets(buf, MAX_LINE_LENGTH, fp)))
            {
                char *comment = strchr(prp, '#');
                if (comment)
                    *comment = 0;
                
                if (*prp)
                {
                    if ((rc = ReadProperty(prp)) == FALSE)
                        break;
                }
            }
            fclose(fp);
        }
    }

    return rc;
}

static size_t trim(char *s)
{
    size_t i;

    /* check for empty strings */
    if (!(i = strlen(s)))
        return 0;
    for (i = i - 1; (i >= 0) &&
         isspace((int)((unsigned char)s[i])); i--);

    s[i + 1] = '\0';

    for (i = 0; ('\0' != s[i]) &&
         isspace((int)((unsigned char)s[i])); i++);

    if (i > 0) {
        strcpy(s, &s[i]);
    }

    return strlen(s);
}

BOOL CJaxer_config::ReadProperty(const char *str)
{
    BOOL rc = TRUE;
    char buf[MAX_LINE_LENGTH + 1];
    char *prp = &buf[0];

    strcpy(prp, str);
    if (trim(prp))
    {
        char *v = strchr(prp, '=');
        if (v)
        {
            *v = '\0';
            v++;
            trim(prp);
            trim(v);
            if (strlen(v) && strlen(prp))
            {
                if(stricmp(prp,"LogFileName")==0)
                {
                    SetLogFileName(v);
                }else if(stricmp(prp,"LogLevel")==0)
                {
                    SetLogLevel(v);
                }else if(stricmp(prp,"LogFileBackups")==0)
                {
                    SetLogFileBackups(v);
                }else if(stricmp(prp,"LogFileSizeKB")==0)
                {
                    SetLogFileSizeKB(v);
                }else if(stricmp(prp,"VirtualDirName")==0)
                {
                    SetVirtualDirName(v);
                }else if(stricmp(prp,"VirtualDirPath")==0)
                {
                    SetVirtualDirPath(v);
                }else if(stricmp(prp,"FilterExts")==0)
                {
                    AddFilterExts(v);
                }else if(stricmp(prp,"FilterContentTypes")==0)
                {
                    AddFilterCTs(v);
                }else if(stricmp(prp,"JaxerServerIP")==0)
                {
                    SetJaxerServerIP(v);
                }else if(stricmp(prp,"JaxerServerPort")==0)
                {
                    SetJaxerServerPort(v);
                }else
                {
                    //error
                }
            }
        }
    }
    return rc;
}

BOOL CJaxer_config::SetLogFileName(const char* v)
{
    strcpy(m_LogFileName, v);
    return TRUE;
}

BOOL CJaxer_config::SetLogFileBackups(const char* v)
{
    m_LogFileBackups = atoi(v);
    if (m_LogFileBackups<=1)
        m_LogFileBackups = 1;
    else if (m_LogFileBackups > 100)
        m_LogFileBackups = 100;
    return TRUE;
}

BOOL CJaxer_config::SetLogFileSizeKB(const char* v)
{
    m_LogFileSizeKB = atoi(v);
    if (m_LogFileSizeKB<=1)
        m_LogFileBackups = 1;
    return TRUE;
}

BOOL CJaxer_config::SetVirtualDirName(const char* v)
{
    if(*v != '/')
    {
        strcpy(m_VirtualDirName, "/");
        strcat(m_VirtualDirName, v);
    }else
    {
        strcpy(m_VirtualDirName, v);
    }

    char *p = m_VirtualDirName;
    while(*p)
    {
        *p = tolower(*p);
        p++;
    }

    return TRUE;
}

BOOL CJaxer_config::SetVirtualDirPath(const char* v)
{
    strcpy(m_VirtualDirPath, v);
    char *p = m_VirtualDirPath;
    while(*p)
    {
        if (*p == '\\')
        {
            *p = '/';
        }
        p++;
    }

    return TRUE;
}

BOOL CJaxer_config::AddFilterExts(const char* v)
{
    char buf[0x2000];
    strcpy(buf, v);
    char *p = strtok(buf, ", ");
    while(p != NULL)
    {
        trim(p);
        if(p)
        {
            SL *ext = new SL(p);
            if (!ext)
                return FALSE;
        
            m_FilterExtsTail->next = ext;
            m_FilterExtsTail = m_FilterExtsTail->next;
        }

        p = strtok(NULL, ", ");
    }
    
    return TRUE;
}

BOOL CJaxer_config::AddFilterCTs(const char* v)
{
    //CT: Content_type
    char buf[0x2000];
    strcpy(buf, v);
    char *p = strtok(buf, ", ");
    while(p != NULL)
    {
        trim(p);
        if(p)
        {
            SL *ct = new SL(p);
            if (!ct)
                return FALSE;
        
            m_FilterCTsTail->next = ct;
            m_FilterCTsTail = m_FilterCTsTail->next;
        }

        p = strtok(NULL, ", ");
    }
    
    return TRUE;
}

BOOL CJaxer_config::SetJaxerServerIP(const char* v)
{
    strcpy(m_JaxerServerIP, v);
    return TRUE;
}

BOOL CJaxer_config::SetJaxerServerPort(const char* v)
{
    m_JaxerServerPort = atoi(v);
    return (m_JaxerServerPort>0);
}

BOOL CJaxer_config::SetLogLevel(const char* v)
{
    if(stricmp("TRACE", v) == 0)
        m_LogLevel = eTRACE;
    else if (stricmp("DEBUG", v) == 0)
        m_LogLevel = eDEBUG;
    else if (stricmp("INFO", v) == 0)
        m_LogLevel = eINFO;
    else if (stricmp("WARN", v) == 0)
        m_LogLevel = eWARN;
    else if (stricmp("ERROR", v) == 0)
        m_LogLevel = eERROR;
    else if (stricmp("FATAL", v) == 0)
        m_LogLevel = eFATAL;
    else
        m_LogLevel = eINFO;
    return TRUE;
}

BOOL CJaxer_config::ShouldFilterExt(const char* ext)
{
    // Just see if ext is in the list
    SL *p = m_FilterExtsHeader->next;
    while(p)
    {
        if (stricmp(ext, p->str) == 0)
            return TRUE;
        p = p->next;
    }
    return FALSE;
}

BOOL CJaxer_config::ShouldFilterCT(const char* ct)
{
    // Just see if content_type is in the list
    SL *p = m_FilterCTsHeader->next;
    while(p)
    {
        if (stricmp(ct, p->str) == 0)
            return TRUE;
        p = p->next;
    }
    return FALSE;
}

BOOL CJaxer_config::IsURLMine(const char* url)
{
    // Just see if it starts with ROOT
    size_t len = strlen(m_VirtualDirName);
    const char* p = strstr(url, m_VirtualDirName);
    if (p==url && (url[len] == '\0' || url[len] == '/' || url[len] == '?'))
    {
        return TRUE;
    }

    return FALSE;    
}