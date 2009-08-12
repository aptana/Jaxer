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

#ifndef __JAXER_PROTOCOL_VERSION_H__
#define __JAXER_PROTOCOL_VERSION_H__

// Current protocol versions for different components

// Jaxer
/* ******************************* Version 2 *********************************
 * Backward Compatability: none
 * Changes:
 *     Removed Hello block.
 *     Added protocol version in BeginRequest block   
 * ***************************************************************************/

/* ******************************* Version 3 *********************************
 * Backward Compatability: none
 * Changes:
 *     Re-order block numbers.
 *     Request type in BeginRequest Block can be Handler(1) or Filter(2) 
 *     Added ResponseHeader after RequestHeader
 *     Added DOC_CONTENT_TYPE in environment Block
 *     Support removing haeders by Jaxer
 *     Support redirect. 
 * ***************************************************************************/

/* ******************************* Version 4 *********************************
 * Backward Compatability: 3
 * Changes:
 *     Support debug request.  
 * ***************************************************************************/
//Current Jaxer Protocol Version
#define JAXER_PROTOCOL_VERSION 4

// Mod_jaxer
//Current mod_jaxer (apache 2.*) protocol version
#define MOD_JAXER_PROTOCOL_VERSION 4

// Block types agreed beteween Jaxer and the Connector.
enum BlockType
{
    bt_BeginRequest = 0,
    bt_RequestHeader = 1,
    bt_ResponseHeader = 2,
    bt_Environment = 3,
    bt_Document = 4,
    bt_RequirePostData = 5,
    bt_PostData = 6,
    bt_EndRequest = 7,
    bt_Error = 8,
    bt_BadBlockType = 9,
	bt_DebugRequest = 10,
};

static char *g_BlockTypes[] =
{
    "BeginRequest",
    "RequestHeader",
    "ResponseHeader",
    "Environment",
    "Document",
    "RequirePostData",
    "PostData",
    "EndRequest",
    "Error",
    "BadBlockType",
	"DebugRequest",
};

// Error codes returned to web connector in BEGIN_REQUEST
enum BeginRequestError
{
    bre_ProtocolVersionTooLow = 1,
    bre_ProtocolVersionTooHigh = 2,
    bre_ProtocolVersionMustBeTheSame = 3,
    bre_CannotHandleRequestType = 4,
	bre_CannotFindJaxer = 5,
	bre_SpecifiedJaxerBusy = 6
};

enum eRequestType
{
	eRequestType_Handler = 1,
    eRequestType_Filter = 2
};

#endif // __JAXER_PROTOCOL_VERSION_H__
