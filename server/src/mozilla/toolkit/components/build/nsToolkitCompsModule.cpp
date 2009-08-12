/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
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
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Joe Hewitt <hewitt@netscape.com> (Original Author)
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#include "nsIGenericFactory.h"
#ifndef JAXER
#include "nsAppStartup.h"
#include "nsUserInfo.h"
#include "nsXPFEComponentsCID.h"
#endif /* JAXER */
#include "nsToolkitCompsCID.h"

#if defined(XP_WIN) && !defined(MOZ_DISABLE_PARENTAL_CONTROLS)
#include "nsParentalControlsServiceWin.h"
#endif

#ifdef ALERTS_SERVICE
#include "nsAlertsService.h"
#endif

#ifndef JAXER
#ifndef MOZ_SUITE
// XXX Suite isn't ready to include this just yet
#ifdef MOZ_RDF
#include "nsDownloadManager.h"
#include "nsDownloadProxy.h"
#endif
#endif // MOZ_SUITE

#include "nsTypeAheadFind.h"

#ifdef MOZ_URL_CLASSIFIER
#include "nsUrlClassifierDBService.h"
#include "nsUrlClassifierStreamUpdater.h"
#include "nsUrlClassifierUtils.h"
#include "nsUrlClassifierHashCompleter.h"
#include "nsDocShellCID.h"
#endif
#endif /* JAXER */

#ifdef MOZ_FEEDS
#include "nsScriptableUnescapeHTML.h"
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef JAXER

NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsAppStartup, Init)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsUserInfo)

#if defined(XP_WIN) && !defined(MOZ_DISABLE_PARENTAL_CONTROLS)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsParentalControlsServiceWin)
#endif

#ifdef ALERTS_SERVICE
NS_GENERIC_FACTORY_CONSTRUCTOR(nsAlertsService)
#endif

#ifndef MOZ_SUITE
// XXX Suite isn't ready to include this just yet
#ifdef MOZ_RDF
NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(nsDownloadManager,
                                         nsDownloadManager::GetSingleton) 
NS_GENERIC_FACTORY_CONSTRUCTOR(nsDownloadProxy)
#endif
#endif // MOZ_SUITE

NS_GENERIC_FACTORY_CONSTRUCTOR(nsTypeAheadFind)

#ifdef MOZ_URL_CLASSIFIER
NS_GENERIC_FACTORY_CONSTRUCTOR(nsUrlClassifierStreamUpdater)
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsUrlClassifierUtils, Init)
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsUrlClassifierHashCompleter, Init)

static NS_IMETHODIMP
nsUrlClassifierDBServiceConstructor(nsISupports *aOuter, REFNSIID aIID,
                                    void **aResult)
{
    nsresult rv;
    NS_ENSURE_ARG_POINTER(aResult);
    NS_ENSURE_NO_AGGREGATION(aOuter);

    nsUrlClassifierDBService *inst = nsUrlClassifierDBService::GetInstance(&rv);
    if (NULL == inst) {
        return rv;
    }
    /* NS_ADDREF(inst); */
    rv = inst->QueryInterface(aIID, aResult);
    NS_RELEASE(inst);

    return rv;
}
#endif
#endif /* JAXER */

#ifdef MOZ_FEEDS
NS_GENERIC_FACTORY_CONSTRUCTOR(nsScriptableUnescapeHTML)
#endif

/////////////////////////////////////////////////////////////////////////////

static const nsModuleComponentInfo components[] =
{
#ifndef JAXER
  { "App Startup Service",
    NS_TOOLKIT_APPSTARTUP_CID,
    NS_APPSTARTUP_CONTRACTID,
    nsAppStartupConstructor },

  { "User Info Service",
    NS_USERINFO_CID,
    NS_USERINFO_CONTRACTID,
    nsUserInfoConstructor },
#ifdef ALERTS_SERVICE
  { "Alerts Service",
    NS_ALERTSSERVICE_CID, 
    NS_ALERTSERVICE_CONTRACTID,
    nsAlertsServiceConstructor },
#endif
#if defined(XP_WIN) && !defined(MOZ_DISABLE_PARENTAL_CONTROLS)
  { "Parental Controls Service",
    NS_PARENTALCONTROLSSERVICE_CID,
    NS_PARENTALCONTROLSSERVICE_CONTRACTID,
    nsParentalControlsServiceWinConstructor },
#endif
#ifndef MOZ_SUITE
// XXX Suite isn't ready to include this just yet
#ifdef MOZ_RDF
  { "Download Manager",
    NS_DOWNLOADMANAGER_CID,
    NS_DOWNLOADMANAGER_CONTRACTID,
    nsDownloadManagerConstructor },
  { "Download",
    NS_DOWNLOAD_CID,
    NS_TRANSFER_CONTRACTID,
    nsDownloadProxyConstructor },
#endif
#endif // MOZ_SUITE
  { "TypeAheadFind Component",
    NS_TYPEAHEADFIND_CID,
    NS_TYPEAHEADFIND_CONTRACTID,
    nsTypeAheadFindConstructor
  },
#ifdef MOZ_URL_CLASSIFIER
  { "Url Classifier DB Service",
    NS_URLCLASSIFIERDBSERVICE_CID,
    NS_URLCLASSIFIERDBSERVICE_CONTRACTID,
    nsUrlClassifierDBServiceConstructor },
  { "Url Classifier DB Service",
    NS_URLCLASSIFIERDBSERVICE_CID,
    NS_URICLASSIFIERSERVICE_CONTRACTID,
    nsUrlClassifierDBServiceConstructor },
  { "Url Classifier Stream Updater",
    NS_URLCLASSIFIERSTREAMUPDATER_CID,
    NS_URLCLASSIFIERSTREAMUPDATER_CONTRACTID,
    nsUrlClassifierStreamUpdaterConstructor },
  { "Url Classifier Utils",
    NS_URLCLASSIFIERUTILS_CID,
    NS_URLCLASSIFIERUTILS_CONTRACTID,
    nsUrlClassifierUtilsConstructor },
  { "Url Classifier Hash Completer",
    NS_URLCLASSIFIERHASHCOMPLETER_CID,
    NS_URLCLASSIFIERHASHCOMPLETER_CONTRACTID,
    nsUrlClassifierHashCompleterConstructor },
#endif
#endif /* JAXER */
#ifdef MOZ_FEEDS
  { "Unescape HTML",
    NS_SCRIPTABLEUNESCAPEHTML_CID,
    NS_SCRIPTABLEUNESCAPEHTML_CONTRACTID,
    nsScriptableUnescapeHTMLConstructor },
#endif
};

NS_IMPL_NSGETMODULE(nsToolkitCompsModule, components)
