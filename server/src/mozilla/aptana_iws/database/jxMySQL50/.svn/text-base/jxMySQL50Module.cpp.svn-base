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
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#endif

#ifdef __cplusplus
extern "C"   {
#endif

#include "my_global.h"
#include "mysql.h"

#ifdef __cplusplus
}  /*extern "C" */
#endif

#include "nsCOMPtr.h"
#include "nsXPCOMCID.h"
#include "nsIGenericFactory.h"
#include "nsICategoryManager.h"
#include "jxMySQL50.h"
#include "jxMySQL50ResultSet.h"
#include "jxMySQL50Statement.h"
#include "jxMySQL50Field.h"
#include "nsServiceManagerUtils.h"
#include "nsIClassInfoImpl.h"
#include "nsIScriptNameSpaceManager.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(jxMySQL50)
NS_DECL_CLASSINFO(jxMySQL50)

NS_GENERIC_FACTORY_CONSTRUCTOR(jxMySQL50ResultSet)
NS_DECL_CLASSINFO(jxMySQL50ResultSet)

NS_GENERIC_FACTORY_CONSTRUCTOR(jxMySQL50Statement)
NS_DECL_CLASSINFO(jxMySQL50Statement)

NS_GENERIC_FACTORY_CONSTRUCTOR(jxMySQL50Field)
NS_DECL_CLASSINFO(jxMySQL50Field)


static NS_METHOD jxMySQL50RegistrationProc(nsIComponentManager *aCompMgr,
                                          nsIFile *aPath,
                                          const char *registryLocation,
                                          const char *componentType,
                                          const nsModuleComponentInfo *info)
{
#if 0
  nsresult rv;
  nsCOMPtr<nsICategoryManager> catMan(do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv));
  if (NS_FAILED(rv))
    return rv;

  rv = catMan->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY, 
                                "MySQL50",  
                                JX_MYSQL50_CONTRACTID, 
                                PR_TRUE,  // persist category 
                                PR_TRUE,  // replace existing 
                                nsnull); 
#endif

    return NS_OK;
}


static const nsModuleComponentInfo components[] =
{
  {"jxMySQL50 Component", 
  	JX_MYSQL50_CID, 
  	JX_MYSQL50_CONTRACTID, 
  	jxMySQL50Constructor,
    jxMySQL50RegistrationProc,
    nsnull,
    NULL /* no factory destructor */,
    NS_CI_INTERFACE_GETTER_NAME(jxMySQL50),  /* interface getter */
    NULL /* no language helper */,
    &NS_CLASSINFO_NAME(jxMySQL50)
  },
  {"jxMySQL50ResultSet Component", 
  	JX_MYSQL50RESULTSET_CID, 
  	JX_MYSQL50RESULTSET_CONTRACTID, 
  	jxMySQL50ResultSetConstructor,
    NULL,
    NULL,
    NULL /* no factory destructor */,
    NULL,
    NULL /* no language helper */,
    NULL
  },
  {"jxMySQL50Statement Component", 
  	JX_MYSQL50STATEMENT_CID, 
  	JX_MYSQL50STATEMENT_CONTRACTID, 
  	jxMySQL50StatementConstructor,
    NULL,
    NULL,
    NULL /* no factory destructor */,
    NULL,
    NULL /* no language helper */,
    NULL
  },
  {"jxMySQL50Field Component", 
  	JX_MYSQL50FIELD_CID, 
  	JX_MYSQL50FIELD_CONTRACTID, 
  	jxMySQL50FieldConstructor,
    NULL,
    NULL,
    NULL /* no factory destructor */,
    NULL,
    NULL /* no language helper */,
    NULL
  }
};

NS_IMPL_NSGETMODULE(jxMySQL50Module, components)
