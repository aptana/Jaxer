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
#ifndef __MOD_JAXER_MESSAGE__H
#define __MOD_JAXER_MESSAGE__H

#include "apr_user.h"
#include "mod_jaxer_protocol.h"

/*
 * Help functions to manipulate a buffer that follow the Jaxer protocol.
 * The first bytes of the buffer contains the Jaxer_Header.
 * Most functions takes a point to the buffer and a point to an offset.  The offset
 * will be updated upon return.
 */

	/*
	 * Set the block type to msgtype, set *pos to the end of the Jaxer_Header for appending
	 */
	void jxr_msg_init(unsigned char* buf, apr_size_t *pos, char msgtype);
      
    /**
     * For a packet to be sent to the web server, finish the process of
     * accumulating data and write the length of the data payload into
     * the header.  
     */
      void jxr_msg_end(unsigned char* buf, apr_size_t *pos);


	  void jxr_msg_reset_pos(apr_size_t *pos);

    /**
     * Add a short integer (2 bytes) to the message.
     */
    void jxr_msg_append_int16(unsigned char* buf, apr_size_t *pos, int val);


    /**
     * Append a byte (1 byte) to the message.
     */
    void jxr_msg_append_byte(unsigned char* buf, apr_size_t *pos, int val);
    
    /**
     * Append an int (4 bytes) to the message.
     */
    void jxr_msg_append_int32(unsigned char* buf, apr_size_t *pos, int val);
    

    
    /**
     * Write a String out at the current write position.  Strings are
     * encoded with the length in two bytes first, then the string.
	 * There is no NULL terminating the string.
     */
    void jxr_msg_append_string(unsigned char* buf, apr_size_t *pos, unsigned char* str);

    
    /** 
     * Copy a chunk of bytes into the packet, starting at the current
     * write position.  The chunk of bytes is encoded with the length
     * in two bytes first, then the data itself.
     *
     * @param b The array from which to copy bytes.
     * @param off The offset into the array at which to start copying
     * @param numBytes The number of bytes to copy.  
     */
    void jxr_msg_append_bytes_prefix_len(unsigned char* buf, apr_size_t *pos, unsigned char* b, apr_size_t off, apr_size_t numBytes);

    /*
     * copy numBytes from b and append it to buf.  There is no 2-byte len prefix
     */
    void jxr_msg_append_bytes(unsigned char* buf, apr_size_t *pos, unsigned char* b, apr_size_t numBytes);
    
    /**
     * Read an integer from packet, and advance the read position past
     * it.  Integers are encoded as two unsigned bytes with the
     * high-order byte first.  
     */
    int jxr_msg_get_int16(unsigned char* buf, apr_size_t *pos);

	/**
	 * Like read, except do not advance *pos
	 */
    int jxr_msg_peek_int16(unsigned char* buf, apr_size_t pos);

    char jxr_msg_get_byte(unsigned char* buf, apr_size_t *pos);  
    char jxr_msg_peek_byte(unsigned char* buf, apr_size_t *pos);

	/**
	 * Read string from the buffer, and terminate the string with NULL.
	 * return the string length.
	 */
	apr_size_t jxr_msg_get_string(unsigned char* buf, apr_size_t *pos, unsigned char* dest);
    
    /**
     * Copy a chunk of bytes from the packet into an array.
     *
     * @return The number of bytes copied.
     */
    apr_size_t jxr_msg_get_bytes(unsigned char* buf, apr_size_t *pos, unsigned char* dest);
    
    /**
     * Read a 32 bits integer from packet.  Integers are encoded as four unsigned bytes with the
     * high-order byte first.
     */
    int jxr_msg_get_int32(unsigned char* buf, apr_size_t *pos);


	/**
	 * return the constantr size of the Jaxer_Header
	 */
    apr_size_t jxr_msg_get_header_length();
    
    /**
	 * return the length of the message (after the header), and set *pos to the end of
	 * the header
	 */
	apr_size_t jxr_msg_get_length(unsigned char*buf, apr_size_t *pos);

	/**
	 * return the message type.
	 */
	char jxr_msg_get_type(unsigned char* buf);
    

	int jxr_init_header(char type, apr_size_t contentLength, Jaxer_Header * header);
 
#endif //__MOD_JAXER_MESSAGE__H
