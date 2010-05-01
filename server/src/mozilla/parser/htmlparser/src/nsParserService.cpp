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
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#include "nsDOMError.h"
#include "nsIAtom.h"
#include "nsParserService.h"
#include "nsHTMLEntities.h"
#include "nsElementTable.h"
#include "nsICategoryManager.h"
#include "nsCategoryManagerUtils.h"

#ifdef JAXER
#include "nsCRT.h"
#include "nsReadableUtils.h"
#include "plhash.h"
#include "nsIPrefService.h"
#include "nsIPrefBranch.h"
#include "nsServiceManagerUtils.h"
#include "nsIPrefLocalizedString.h"

static PRInt32 gTableRefCount;
static PLHashTable* gHTMLTagTable;
nsIPrefBranch * sPrefBranch = nsnull;

static PLHashNumber PR_CALLBACK HashNumber(const void* aKey)
{
  return PLHashNumber(NS_PTR_TO_INT32(aKey));
}
#endif /* JAXER */

extern "C" int MOZ_XMLCheckQName(const char* ptr, const char* end,
                                 int ns_aware, const char** colon);

nsParserService::nsParserService() : mEntries(0)
{
  mHaveNotifiedCategoryObservers = PR_FALSE;
#ifdef JAXER
  if (gTableRefCount++ == 0) {
    NS_ASSERTION(!gHTMLTagTable, "pre existing hash!");

    gHTMLTagTable = PL_NewHashTable(256,
                                    HashNumber,
                                    PL_CompareValues,
                                    PL_CompareValues,
                                    nsnull, NULL);

    CallGetService(NS_PREFSERVICE_CONTRACTID, &sPrefBranch);

    char prefNameBuf[1096];
    PRInt32 i = 0; 
    while (true) {
      ++i;
      sprintf(prefNameBuf, "Jaxer.HTML.Tag.Monitor.%d", i);

      nsXPIDLCString tagName;
      nsresult rv = sPrefBranch->GetCharPref(prefNameBuf, getter_Copies(tagName));
      if (NS_FAILED(rv))
        break;

      nsAutoString tag;
      CopyUTF8toUTF16(tagName.get(), tag);

      // Create key and drop it into the table.
      PRInt32 tag_id = HTMLStringTagToId(tag);
      PL_HashTableAdd(gHTMLTagTable, (const void*)tag_id, (void*)tag_id);
    }
  }
#endif /* JAXER */
}

nsParserService::~nsParserService()
{
  nsObserverEntry *entry = nsnull;
  while( (entry = static_cast<nsObserverEntry*>(mEntries.Pop())) ) {
    NS_RELEASE(entry);
  }
}

NS_IMPL_ISUPPORTS1(nsParserService, nsIParserService)

#ifdef JAXER
PRBool
nsParserService::IsTagMonitored(PRInt32 aID) const
{
  PLHashEntry **hep = PL_HashTableRawLookup(gHTMLTagTable, aID, (void*)aID);
  return hep && *hep;
}
#endif /* JAXER */

PRInt32
nsParserService::HTMLAtomTagToId(nsIAtom* aAtom) const
{
  nsAutoString tagName;
  aAtom->ToString(tagName);

  return nsHTMLTags::LookupTag(tagName);
}

PRInt32
nsParserService::HTMLCaseSensitiveAtomTagToId(nsIAtom* aAtom) const
{
  return nsHTMLTags::CaseSensitiveLookupTag(aAtom);
}

PRInt32
nsParserService::HTMLStringTagToId(const nsAString& aTag) const
{
  return nsHTMLTags::LookupTag(aTag);
}

const PRUnichar*
nsParserService::HTMLIdToStringTag(PRInt32 aId) const
{
  return nsHTMLTags::GetStringValue((nsHTMLTag)aId);
}
  
nsIAtom*
nsParserService::HTMLIdToAtomTag(PRInt32 aId) const
{
  return nsHTMLTags::GetAtom((nsHTMLTag)aId);
}

NS_IMETHODIMP
nsParserService::HTMLConvertEntityToUnicode(const nsAString& aEntity,
                                            PRInt32* aUnicode) const
{
  *aUnicode = nsHTMLEntities::EntityToUnicode(aEntity);

  return NS_OK;
}

NS_IMETHODIMP
nsParserService::HTMLConvertUnicodeToEntity(PRInt32 aUnicode,
                                            nsCString& aEntity) const
{
  const char* str = nsHTMLEntities::UnicodeToEntity(aUnicode);
  if (str) {
    aEntity.Assign(str);
  }

  return NS_OK;
}

NS_IMETHODIMP
nsParserService::IsContainer(PRInt32 aId, PRBool& aIsContainer) const
{
  aIsContainer = nsHTMLElement::IsContainer((eHTMLTags)aId);

  return NS_OK;
}

NS_IMETHODIMP
nsParserService::IsBlock(PRInt32 aId, PRBool& aIsBlock) const
{
  if((aId>eHTMLTag_unknown) && (aId<eHTMLTag_userdefined)) {
    aIsBlock=((gHTMLElements[aId].IsMemberOf(kBlock))       ||
              (gHTMLElements[aId].IsMemberOf(kBlockEntity)) ||
              (gHTMLElements[aId].IsMemberOf(kHeading))     ||
              (gHTMLElements[aId].IsMemberOf(kPreformatted))||
              (gHTMLElements[aId].IsMemberOf(kList)));
  }
  else {
    aIsBlock = PR_FALSE;
  }

  return NS_OK;
}

NS_IMETHODIMP
nsParserService::RegisterObserver(nsIElementObserver* aObserver,
                                  const nsAString& aTopic,
                                  const eHTMLTags* aTags)
{
  nsresult result = NS_OK;
  nsObserverEntry* entry = GetEntry(aTopic);

  if(!entry) {
    result = CreateEntry(aTopic,&entry);
    NS_ENSURE_SUCCESS(result,result);
  }

  while (*aTags) {
    if (*aTags <= NS_HTML_TAG_MAX) {
      entry->AddObserver(aObserver,*aTags);
    }
    ++aTags;
  }

  return result;
}

NS_IMETHODIMP
nsParserService::UnregisterObserver(nsIElementObserver* aObserver,
                                    const nsAString& aTopic)
{
  PRInt32 count = mEntries.GetSize();

  for (PRInt32 i=0; i < count; ++i) {
    nsObserverEntry* entry = static_cast<nsObserverEntry*>(mEntries.ObjectAt(i));
    if (entry && entry->Matches(aTopic)) {
      entry->RemoveObserver(aObserver);
    }
  }

  return NS_OK;
}

NS_IMETHODIMP
nsParserService::GetTopicObservers(const nsAString& aTopic,
                                   nsIObserverEntry** aEntry) {
  nsresult result = NS_OK;
  nsObserverEntry* entry = GetEntry(aTopic);

  if (!entry) {
    return NS_ERROR_NULL_POINTER;
  }

  NS_ADDREF(*aEntry = entry);

  return result;
}

nsresult
nsParserService::CheckQName(const nsAString& aQName,
                            PRBool aNamespaceAware,
                            const PRUnichar** aColon)
{
  const char* colon;
  const PRUnichar *begin, *end;
  begin = aQName.BeginReading();
  end = aQName.EndReading();
  int result = MOZ_XMLCheckQName(reinterpret_cast<const char*>(begin),
                                 reinterpret_cast<const char*>(end),
                                 aNamespaceAware, &colon);
  *aColon = reinterpret_cast<const PRUnichar*>(colon);

  if (result == 0) {
    return NS_OK;
  }

  // MOZ_EXPAT_EMPTY_QNAME || MOZ_EXPAT_INVALID_CHARACTER
  if (result == (1 << 0) || result == (1 << 1)) {
    return NS_ERROR_DOM_INVALID_CHARACTER_ERR;
  }

  return NS_ERROR_DOM_NAMESPACE_ERR;
}

class nsMatchesTopic : public nsDequeFunctor{
  const nsAString& mString;
public:
  PRBool matched;
  nsObserverEntry* entry;
  nsMatchesTopic(const nsAString& aString):mString(aString),matched(PR_FALSE){}
  virtual void* operator()(void* anObject){
    entry=static_cast<nsObserverEntry*>(anObject);
    matched=mString.Equals(entry->mTopic);
    return matched ? nsnull : anObject;
  }
};

// XXX This may be more efficient as a HashTable instead of linear search
nsObserverEntry*
nsParserService::GetEntry(const nsAString& aTopic)
{
  if (!mHaveNotifiedCategoryObservers) {
    mHaveNotifiedCategoryObservers = PR_TRUE;
    NS_CreateServicesFromCategory("parser-service-category",
                                  static_cast<nsISupports*>(static_cast<void*>(this)),
                                  "parser-service-start"); 
  }

  nsMatchesTopic matchesTopic(aTopic);
  mEntries.FirstThat(*&matchesTopic);
  return matchesTopic.matched?matchesTopic.entry:nsnull;
}

nsresult
nsParserService::CreateEntry(const nsAString& aTopic, nsObserverEntry** aEntry)
{
  *aEntry = new nsObserverEntry(aTopic);

  if (!*aEntry) {
    return NS_ERROR_OUT_OF_MEMORY;
  }

  NS_ADDREF(*aEntry);
  mEntries.Push(*aEntry);

  return NS_OK;
}
