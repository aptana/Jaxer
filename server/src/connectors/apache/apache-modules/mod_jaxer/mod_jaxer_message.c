/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
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
#include "mod_jaxer_message.h"
#include <string.h>
#include <stdio.h>
#include "mod_jaxer_protocol.h"

/**
 * See mod_jaxer_message.h for function documentations.
 */
void jxr_msg_init(unsigned char* buf, apr_size_t *pos, char msgtype)
{
	((Jaxer_Header*)buf)->type = msgtype;
	*pos = sizeof(Jaxer_Header);
}

void jxr_msg_reset_pos(apr_size_t *pos)
{
	*pos = sizeof(Jaxer_Header);
}
void jxr_msg_end(unsigned char* buf, apr_size_t *pos)
{
	apr_size_t dLen = *pos - sizeof(Jaxer_Header);

	((Jaxer_Header*)buf)->contentLengthB1 = (unsigned char) ((dLen>>8) & 0xFF);
	((Jaxer_Header*)buf)->contentLengthB0 = (unsigned char) (dLen & 0xFF);
}


void jxr_msg_append_int16(unsigned char* buf, apr_size_t *pos, int val)
{
    buf[(*pos)++] = (unsigned char) ((val >> 8) & 0xFF);
    buf[(*pos)++] = (unsigned char) (val & 0xFF);
}

void jxr_msg_append_byte(unsigned char* buf, apr_size_t *pos, int val)
{
    buf[(*pos)++] = (unsigned char) (val & 0xFF);
}
	
void jxr_msg_append_int32(unsigned char* buf, apr_size_t *pos, int val)
{
    buf[(*pos)++] = (unsigned char) ((val >> 24) & 0xFF);
    buf[(*pos)++] = (unsigned char) ((val >> 16) & 0xFF);
    buf[(*pos)++] = (unsigned char) ((val >> 8) & 0xFF);
    buf[(*pos)++] = (unsigned char) (val & 0xFF);
}

 

void jxr_msg_append_string(unsigned char* buf, apr_size_t *pos, unsigned char* str)
{
	/* Added string is not NULL terminated! */
	int leng;
    if (str == 0)
	{
        jxr_msg_append_int16(buf, pos, 0);
        return;
    }
    leng = (int) strlen(str);
    jxr_msg_append_int16(buf, pos, leng);
	strncpy(buf + (*pos), str, leng);
	*pos += leng;
}

    
void jxr_msg_append_bytes_prefix_len(unsigned char* buf, apr_size_t *pos, unsigned char* b, apr_size_t off, apr_size_t numBytes)
{
    jxr_msg_append_int16(buf, pos, (int)numBytes);
    memcpy((buf+ *pos), (b+off), numBytes);
    *pos += numBytes;
}

void jxr_msg_append_bytes(unsigned char* buf, apr_size_t *pos, unsigned char* b, apr_size_t numBytes)
{
    memcpy((buf+ *pos), b, numBytes);
    *pos += numBytes;
}


int jxr_msg_get_int16(unsigned char* buf, apr_size_t *pos)
{
    int b1 = buf[(*pos)++] & 0xFF;
    int b2 = buf[(*pos)++] & 0xFF;
    return (b1<<8) + b2;
}


int jxr_msg_peek_int16(unsigned char* buf, apr_size_t pos) 
{
    int b1 = buf[pos] & 0xFF;
    int b2 = buf[pos+1] & 0xFF;
    return (b1<<8) + b2;
}


char jxr_msg_get_byte(unsigned char* buf, apr_size_t *pos)
{
    char res = (char) buf[(*pos)++];
    return res;
}


char jxr_msg_peek_byte(unsigned char* buf, apr_size_t *pos)
{
    char res = (char)buf[*pos];
    return res;
}

    
apr_size_t jxr_msg_get_bytes(unsigned char* buf, apr_size_t *pos, unsigned char* dest)
{
    apr_size_t length = jxr_msg_get_int16(buf, pos);
    if ((length == 0xFFFF) || (length == -1))
	{
        return 0;
    }

    memcpy(dest, buf+ (*pos), length);
    *pos += length;
    return length;
}

apr_size_t jxr_msg_get_string(unsigned char* buf, apr_size_t *pos, unsigned char* dest)
{
    apr_size_t len = jxr_msg_get_bytes(buf, pos, dest);
	dest[len] = 0;
	return len;
}
    
int jxr_msg_get_int32(unsigned char* buf, apr_size_t *pos)
{
    int b1 = buf[(*pos)++] & 0xFF;
    b1 <<= 8;
    b1 |= (buf[(*pos)++] & 0xFF);
    b1 <<= 8;
    b1 |= (buf[(*pos)++] & 0xFF);
    b1 <<=8;
    b1 |= (buf[(*pos)++] & 0xFF);
    return  b1;
}


apr_size_t jxr_msg_get_header_length()
{
    return sizeof(Jaxer_Header);
}

char jxr_msg_get_type(unsigned char* buf)
{
	Jaxer_Header *hdr = (Jaxer_Header*) buf;
	return hdr->type;
}
    
apr_size_t jxr_msg_get_length(unsigned char*buf, apr_size_t *pos)
{
	Jaxer_Header *hdr = (Jaxer_Header*) buf;
	int b1 = hdr->contentLengthB1 & 0xFF;
    int b2 = hdr->contentLengthB0 & 0xFF;

	*pos = sizeof(Jaxer_Header);
    return (b1<<8) + b2;
}


int
jxr_init_header(char type, apr_size_t contentLength, Jaxer_Header * header)
{
	header->type = (unsigned char) type;
	header->contentLengthB1 = (unsigned char) ((contentLength >> 8) & 0xff);
	header->contentLengthB0 = (unsigned char) ((contentLength) & 0xff);
	return 1;
}
