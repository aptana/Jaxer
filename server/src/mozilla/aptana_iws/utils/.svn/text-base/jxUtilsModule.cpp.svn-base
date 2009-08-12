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

#include "nsCOMPtr.h"
#include "nsXPCOMCID.h"
#include "nsIGenericFactory.h"
#include "nsICategoryManager.h"
#include "nsIScriptNameSpaceManager.h"

#include "jxProcess.h"
#include "jxWindow.h"

#include "nsServiceManagerUtils.h"
#include "nsIClassInfoImpl.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(jxProcess)
NS_DECL_CLASSINFO(jxProcess)

NS_GENERIC_FACTORY_CONSTRUCTOR(jxWindow)
NS_DECL_CLASSINFO(jxWindow)

static NS_METHOD RegistrationProc(nsIComponentManager *aCompMgr,
                                          nsIFile *aPath,
                                          const char *registryLocation,
                                          const char *componentType,
                                          const nsModuleComponentInfo *info)
{
#if 0
	nsresult rv;
	nsCOMPtr<nsICategoryManager> catMan(do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv));
	NS_ENSURE_SUCCESS(rv, rv);
	
	rv = catMan->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY, 
                                "jxProcess",  
                                JX_PROCESS_CONTRACTID, 
                                PR_TRUE,  // persist category 
                                PR_TRUE,  // replace existing 
                                nsnull);
	NS_ENSURE_SUCCESS(rv, rv);

	rv = catMan->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY, 
                                "jxWindow",  
                                JX_WINDOW_CONTRACTID, 
                                PR_TRUE,  // persist category 
                                PR_TRUE,  // replace existing 
                                nsnull);
	NS_ENSURE_SUCCESS(rv, rv);
#endif

    return NS_OK;
}


static const nsModuleComponentInfo components[] =
{
  {"jxProcess Component", 
  	JX_PROCESS_CID, 
  	JX_PROCESS_CONTRACTID, 
  	jxProcessConstructor,
    RegistrationProc, /* single registration proc for all components */
    nsnull,
    nsnull, /* no factory destructor */
    NS_CI_INTERFACE_GETTER_NAME(jxProcess),  /* interface getter */
    nsnull /* no language helper */,
    &NS_CLASSINFO_NAME(jxProcess)
  }, 
  {"jxWindow Component", 
  	JX_WINDOW_CID, 
  	JX_WINDOW_CONTRACTID, 
  	jxWindowConstructor,
    nsnull,
    nsnull,
    nsnull, /* no factory destructor */
    NS_CI_INTERFACE_GETTER_NAME(jxWindow),  /* interface getter */
    nsnull /* no language helper */,
    &NS_CLASSINFO_NAME(jxWindow)
  }
};

NS_IMPL_NSGETMODULE(jxUtilsModule, components)

