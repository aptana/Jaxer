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
#ifndef FCGID_PROTOCOL_H
#define FCGID_PROTOCOL_H
#include "httpd.h"


/*
 * Max bucket size for breaking brigade buckes
 */
#define  MAX_PACKET_SIZE			(8192)

enum BlockType
{
    // BLOCKTYPE_HELLO = 0,
    BLOCKTYPE_BEGIN_REQUEST = 1,
    BLOCKTYPE_HTTP_HEADER = 2,
    BLOCKTYPE_ENVIRO_VARS = 3,
    BLOCKTYPE_DOCUMENT = 4,
    BLOCKTYPE_REQUEST_POSTDATA = 5,
    BLOCKTYPE_POSTDATA = 6,
    BLOCKTYPE_ENDREQUEST = 7,
	BLOCKTYPE_ERROR = 8
#if 0
    BLOCKTYPE_REQUEST_DOC = 9,
    BLOCKTYPE_DOC_STATUS = 10
#endif
};


/*
 * verbal description of the block types.  Must match BolckType
 */
static const char *sBlockType[] =
{
	"HELLO_INVALID",
	"BEGIN_REQUEST",
	"HTTP_HEADER",
	"ENVIRO_VARS",
	"DOCUMENT",
	"REQUEST_POSTDATA",
	"POSTDATA",
	"ENDREQUEST",
	"ERROR",
	"BAD_BLOCK_TYPE"
};

enum RequestType
{
	RequestType_HTMLDocument = 0
};

/*
 * Jaxer protocol version
 */
static int JAXER_PROTOCOL_VERSION	=	2;

// Error codes returned to web connector in BEGIN_REQUEST
enum BeginRequestError
{
    bre_ProtocolVersionTooLow = 1,
    bre_ProtocolVersionTooHigh = 2,
    bre_ProtocolVersionMustBeTheSame = 3,
    bre_CannotHandleRequestType = 4
};

/*
 * Jaxer protocol header definition
 */
typedef struct {
	unsigned char type;
	unsigned char contentLengthB1;
	unsigned char contentLengthB0;
} Jaxer_Header;

/*
 * Size of the Jaxer protocol header
 */
#define JAXER_HEADER_LEN            (3)

/*
 * Header type -- environment or HTTP header
 */
#define ENV_PRMS 2
#define HDR_PRMS 1

/*
 * Values for role component of FCGI_BeginRequestBody
 */
#define FCGI_RESPONDER  1
#define FCGI_FILTER     3


#endif
