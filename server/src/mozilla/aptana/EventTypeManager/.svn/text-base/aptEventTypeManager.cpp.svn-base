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

#include "aptIEventTypeManager.h"
#include "aptEventTypeManager.h"
#include "aptEventNames.h"
#include "nsICategoryManager.h"
#include "nsServiceManagerUtils.h"
#include "nsISupportsPrimitives.h"
#include "nsCOMPtr.h"
#include "nsIObserver.h"
#include "nsString.h"
#include "nsXPCOMCID.h"

NS_IMPL_ISUPPORTS1(aptEventTypeManager, aptIEventTypeManager)

aptEventTypeManager::aptEventTypeManager()
{
	int i;
	nsresult rv;

	for (i = 0; i < aptEventName_EOL; i++)
		mObservers[i] = new nsCOMArray<nsIObserver>;

	// Populate the Aptana EventType Manager with global event handler info.
	nsCOMPtr<nsICategoryManager> catmgr = do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv);
    if (NS_FAILED(rv))
		return;

	// Iterate over the names in the Aptana Event Name Table.
	for (i = 1; i < aptEventName_EOL; i++)
	{
		// Fetch the Event Name.
		const char *name = aptEventNames::GetStringValue(aptEventName(i));

		// Enumerate all event handlers that are globally registered to listen
		// to this event.
		nsCOMPtr<nsISimpleEnumerator> loaderEnum;
		rv = catmgr->EnumerateCategory(name, getter_AddRefs(loaderEnum));
		if (NS_FAILED(rv)) 
			continue;

		// There should be only one handler for each event, but we'll use the
		// last one if there are more than one.
		nsCOMPtr<nsISupports> entry;
		while (NS_SUCCEEDED(loaderEnum->GetNext(getter_AddRefs(entry)))) 
		{
			// Obtain the category entry value, i.e. the 2nd field in the
			// category record in the registry.  It might resemble something
			// like this: "HTMLParseComplete
			// Listener (JS)".
			nsCOMPtr<nsISupportsCString> category(do_QueryInterface(entry));
			if (!category)
				continue;

			nsCAutoString categoryEntry;
			if (NS_FAILED(category->GetData(categoryEntry)))
				continue;

			// Obtain the ContactID for the object/listener.
			nsXPIDLCString contractId;
			if (NS_FAILED(catmgr->GetCategoryEntry(name, categoryEntry.get(),
							                       getter_Copies(contractId))))
				continue;

			// Instantiate the object.
			nsCOMPtr<nsIObserver> observer = do_CreateInstance(contractId.get());

			// If we successfully instantiated the object, add it as an
			// observer.
			if (observer) {
				mObservers[i]->AppendObject(observer);
			}
		}
	}
}

aptEventTypeManager::~aptEventTypeManager()
{
	for (int i = 0; i < aptEventName_EOL; i++) {
		mObservers[i]->Clear();
		delete mObservers[i];
	}
}

NS_IMETHODIMP
aptEventTypeManager::AddEventTypeEntry(const char *aEventTypeName, nsIObserver *aObserver)
{
	NS_ENSURE_ARG_POINTER(aEventTypeName);

	nsAutoString etn;
	etn.AppendASCII(aEventTypeName);
	aptEventName en = aptEventNames::LookupName(etn);

	mObservers[en]->AppendObject(aObserver);

	return NS_OK;
}

NS_IMETHODIMP 
aptEventTypeManager::FireEvent(nsISupports *aEventData, PRInt32 aEventType)
{
  	NS_ENSURE_ARG_POINTER(aEventData);

	nsCOMArray<nsIObserver> *array = mObservers[aEventType];
	if (array->Count() > 0) {
		const char *et = aptEventNames::GetStringValue(aptEventName(aEventType));
		for (int i = 0; i < array->Count(); ++i) {
			array->ObjectAt(i)->Observe(aEventData, et, nsnull);
		}
	}

    return NS_OK;
}
