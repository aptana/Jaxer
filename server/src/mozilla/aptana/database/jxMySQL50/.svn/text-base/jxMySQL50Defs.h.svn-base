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
#ifndef __JXMYSQL50DEFS_H
#define __JXMYSQL50DEFS_H

#include "nsError.h"

#define NS_ERROR_MODULE_JXMYSQL    99

#define JX_MYSQL50_ERROR_NOT_CONNECTED \
        NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_JXMYSQL, 1)

#define JX_MYSQL50_ERROR_CANT_CREATEINSTANCE \
        NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_JXMYSQL, 2)

#define JX_MYSQL50_ERROR_STMT_ALREADY_INITIALIZED \
        NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_JXMYSQL, 3)

#define JX_MYSQL50_ERROR_STMT_NULL \
        NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_JXMYSQL, 4)

#define JX_MYSQL50_ERROR_UNEXPECTED_ERROR \
        NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_JXMYSQL, 5)

#define JX_MYSQL50_NULL_RESULTSET \
        NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_JXMYSQL, 6)

#define JX_MYSQL50_ERROR_INVALID_TYPE \
        NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_JXMYSQL, 7)

#define JX_MYSQL50_ERROR_BIND_ARRAY_IS_NULL \
        NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_JXMYSQL, 8)

#define JX_MYSQL50_CANNOT_CONVERT_DATA NS_ERROR_CANNOT_CONVERT_DATA
#define JX_MYSQL50_ERROR_NULL_POINTER NS_ERROR_NULL_POINTER

#define JX_MYSQL50_ERROR_ILLEGAL_VALUE NS_ERROR_ILLEGAL_VALUE

#define JX_MYSQL50_ERROR_OUT_OF_MEMORY NS_ERROR_OUT_OF_MEMORY

#define JX_MYSQL50_MYSQL_ERROR \
        NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_JXMYSQL, 100)


#define NON_MYSQL_ERROR(n, ErrorString) \
    if (n == JX_MYSQL50_ERROR_NOT_CONNECTED) \
            ErrorString.AssignLiteral("Not connected to MYSQL DB"); \
    else if(n == JX_MYSQL50_ERROR_CANT_CREATEINSTANCE) \
            ErrorString.AssignLiteral("Create object failed"); \
    else if (n == JX_MYSQL50_ERROR_STMT_ALREADY_INITIALIZED) \
            ErrorString.AssignLiteral("Statement already initialized"); \
    else if (n == JX_MYSQL50_ERROR_STMT_NULL) \
            ErrorString.AssignLiteral("Statement is null"); \
    else if (n == JX_MYSQL50_ERROR_UNEXPECTED_ERROR) \
            ErrorString.AssignLiteral("Unexpected error"); \
    else if (n == JX_MYSQL50_NULL_RESULTSET) \
            ErrorString.AssignLiteral("Resultset is null"); \
    else if (n == JX_MYSQL50_ERROR_ILLEGAL_VALUE) \
            ErrorString.AssignLiteral("Illegal value (eg too small or toolarge) specified"); \
    else if (n == JX_MYSQL50_ERROR_INVALID_TYPE) \
            ErrorString.AssignLiteral("Invalid type"); \
    else if (n == JX_MYSQL50_ERROR_BIND_ARRAY_IS_NULL) \
            ErrorString.AssignLiteral("Bind array is null"); \
    else if (n == JX_MYSQL50_CANNOT_CONVERT_DATA) \
            ErrorString.AssignLiteral("Cannot convert data"); \
    else if (n == JX_MYSQL50_ERROR_NULL_POINTER) \
            ErrorString.AssignLiteral("The point is null"); \
    else if (n == JX_MYSQL50_ERROR_OUT_OF_MEMORY) \
            ErrorString.AssignLiteral("Out of memory"); \
    else \
        ErrorString.AssignLiteral(""); \


#if 0
#define JX_MYSQL50_ERROR_NOT_CONNECTED NS_ERROR_NOT_INITIALIZED
#define JX_MYSQL50_ERROR_CANT_CREATEINSTANCE NS_ERROR_NULL_POINTER
#define JX_MYSQL50_ERROR_STMT_ALREADY_INITIALIZED NS_ERROR_FAILURE
#define JX_MYSQL50_ERROR_STMT_NULL NS_ERROR_NOT_INITIALIZED
// #define JX_MYSQL50_ERROR_BIND_VARIABLE_TYPE_NOT_ARRAY 104
#define JX_MYSQL50_ERROR_UNEXPECTED_ERROR NS_ERROR_UNEXPECTED
#endif

#endif //__JXMYSQL50DEFS_H
