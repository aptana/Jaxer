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

#include "aptEventNames.h"
#include "nsString.h"

//entries must be in the same order as the enum aptEventName entries in aptEventNames.h 
static const char* const kEventNameTable[] = {
	0,
	APT_EVENT_LOG,
	APT_EVENT_REQUESTSTART,
	APT_EVENT_HTMLPARSECOMPLETE,
	APT_EVENT_HTMLPARSESTART,
	APT_EVENT_BEFORESCRIPTEVALUATED,
	APT_EVENT_AFTERSCRIPTEVALUATED,
	APT_EVENT_SCRIPTCOMPILE,
	APT_EVENT_NEWHTMLELEMENT,
	APT_EVENT_DOCINITIALIZED,
	APT_EVENT_REQUESTCOMPLETE,
	0
};
	

static inline bool equal(const nsAString::char_type *p, const char *q) {
	while (*q)
		if (*p++ != *q++)
			return false;
	return true;
}

static inline bool equal(const nsAString::char_type *p, const char *q, int len) {
	for (int i = 0; i < len; i++)
		if (p[i] != q[i])
			return false;
	return true;
}

aptEventName
aptEventNames::LookupName(const nsAString& aEventName)
{
	// All event names start with "aptanaEvent.", so get that out of the way.
	const nsAString::char_type *b = aEventName.BeginReading();
	const nsAString::char_type *e = aEventName.EndReading();
	if (e - b <= 11)
		return aptEventName_EOL;
	if (!equal(b, "jaxerEvent.", 11))
		return aptEventName_EOL;

	// Now that's out of the way, see which one it is.
	b += 11;
	switch (*b++) {
		case 'A':
			if (equal(b, "fterScriptEvaluated"))
				return aptEventName_AfterScriptEvaluated;
			break;
		case 'B':
			if (equal(b, "eforeScriptEvaluated"))
				return aptEventName_BeforeScriptEvaluated;
			break;
		case 'D':
			if (equal(b, "ocInitialized"))
				return aptEventName_DocInitialized;
			break;
		case 'H':
			if (equal(b, "TMLParseComplete"))
				return aptEventName_HTMLParseComplete;
			else if (equal(b, "TMLParseStart"))
				return aptEventName_HTMLParseStart;
			break;
		case 'L':
			if (equal(b, "og"))
				return aptEventName_Log;
			break;
		case 'N':
			if (equal(b, "ewHTMLElement"))
				return aptEventName_NewHTMLElement;
			break;
		case 'R':
			if (equal(b, "equestStart"))
				return aptEventName_RequestStart;
			else if(equal(b, "equestComplete"))
				return aptEventName_RequestComplete;
			break;
		case 'S':
			if (equal(b, "criptCompile"))
				return aptEventName_ScriptCompile;
			break;
		default:
			break;
	}

	return aptEventName_EOL;
}

const char *
aptEventNames::GetStringValue(aptEventName aEnum)
{
	if (aEnum < aptEventName_unknown || aEnum > aptEventName_EOL)
		return nsnull;

	return kEventNameTable[aEnum];
}
