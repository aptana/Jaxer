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

#ifndef aptEventNames_h___
#define aptEventNames_h___

#include "nsAString.h"

enum aptEventName {
	aptEventName_unknown = 0,			// Must be first and zero
	aptEventName_Log,
	aptEventName_RequestStart,
	aptEventName_HTMLParseComplete,
	aptEventName_HTMLParseStart,
	aptEventName_BeforeScriptEvaluated,
	aptEventName_AfterScriptEvaluated,
	aptEventName_ScriptCompile,
	aptEventName_NewHTMLElement, 
	aptEventName_DocInitialized,
	aptEventName_RequestComplete,
	aptEventName_EOL					// Must be last
};

#define APT_EVENT_LOG				"jaxerEvent.Log"
#define APT_EVENT_REQUESTSTART		"jaxerEvent.RequestStart"
#define APT_EVENT_HTMLPARSECOMPLETE	"jaxerEvent.HTMLParseComplete"
#define APT_EVENT_HTMLPARSESTART	"jaxerEvent.HTMLParseStart"
#define APT_EVENT_BEFORESCRIPTEVALUATED	"jaxerEvent.BeforeScriptEvaluated"
#define APT_EVENT_AFTERSCRIPTEVALUATED	"jaxerEvent.AfterScriptEvaluated"
#define APT_EVENT_SCRIPTCOMPILE		"jaxerEvent.ScriptCompile"
#define APT_EVENT_NEWHTMLELEMENT	"jaxerEvent.NewHTMLElement" 
#define APT_EVENT_DOCINITIALIZED	"jaxerEvent.DocInitialized"
#define APT_EVENT_REQUESTCOMPLETE	"jaxerEvent.RequestComplete"

class aptEventNames {
public:
  static aptEventName LookupName(const nsAString& aEventName);
  static const char *GetStringValue(aptEventName aEnum);
};

#endif /* aptEventNames_h___ */
