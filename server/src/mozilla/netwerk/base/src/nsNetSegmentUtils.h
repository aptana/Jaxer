/* ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 *  It is derived from Mozilla software and modified by Aptana, Inc.
 *  Aptana, Inc. has elected to use and license the Mozilla software 
 *  under the terms of the GPL, and licenses this file to you under the terms
 *  of the GPL.
 *  
 *  Contributor(s): Aptana, Inc.
 *  The portions modified by Aptana are Copyright (C) 2007-2008 Aptana, Inc.
 *  All Rights Reserved.
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
/* ***** BEGIN ORIGINAL ATTRIBUTION BLOCK *****
 *
 * The Original Code is Mozilla.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2002
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Darin Fisher <darin@netscape.com>
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#ifndef nsNetSegmentUtils_h__
#define nsNetSegmentUtils_h__

#include "necko-config.h"
#include "nsIOService.h"

#ifdef NECKO_SMALL_BUFFERS
#define NET_DEFAULT_SEGMENT_SIZE  2048
#define NET_DEFAULT_SEGMENT_COUNT 4
#else
#ifdef JAXER
#define NET_DEFAULT_SEGMENT_SIZE  32768
#else
#define NET_DEFAULT_SEGMENT_SIZE  4096
#endif /* JAXER */
#define NET_DEFAULT_SEGMENT_COUNT 16
#endif

/**
 * returns preferred allocator for given segment size.  NULL implies
 * system allocator.  this result can be used when allocating a pipe.
 */
static inline nsIMemory *
net_GetSegmentAlloc(PRUint32 segsize)
{
    return (segsize == NET_DEFAULT_SEGMENT_SIZE)
                     ? nsIOService::gBufferCache
                     : nsnull;
}

/**
 * applies defaults to segment params in a consistent way.
 */
static inline void
net_ResolveSegmentParams(PRUint32 &segsize, PRUint32 &segcount)
{
    if (!segsize)
        segsize = NET_DEFAULT_SEGMENT_SIZE;
    if (!segcount)
        segcount = NET_DEFAULT_SEGMENT_COUNT;
}

#endif // !nsNetSegmentUtils_h__
