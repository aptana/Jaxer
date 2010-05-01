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
#ifndef NS_PARSERSERVICE_H__
#define NS_PARSERSERVICE_H__

#include "nsIParserService.h"
#include "nsDTDUtils.h"
#include "nsVoidArray.h"

extern "C" int MOZ_XMLIsLetter(const char* ptr);
extern "C" int MOZ_XMLIsNCNameChar(const char* ptr);
extern "C" int MOZ_XMLTranslateEntity(const char* ptr, const char* end,
                                      const char** next, PRUnichar* result);

class nsParserService : public nsIParserService {
public:
  nsParserService();
  virtual ~nsParserService();

  NS_DECL_ISUPPORTS

#ifdef JAXER
  PRBool IsTagMonitored(PRInt32 aID) const;
#endif

  PRInt32 HTMLAtomTagToId(nsIAtom* aAtom) const;

  PRInt32 HTMLCaseSensitiveAtomTagToId(nsIAtom* aAtom) const;

  PRInt32 HTMLStringTagToId(const nsAString& aTag) const;

  const PRUnichar *HTMLIdToStringTag(PRInt32 aId) const;
  
  nsIAtom *HTMLIdToAtomTag(PRInt32 aId) const;

  NS_IMETHOD HTMLConvertEntityToUnicode(const nsAString& aEntity, 
                                        PRInt32* aUnicode) const;
  NS_IMETHOD HTMLConvertUnicodeToEntity(PRInt32 aUnicode,
                                        nsCString& aEntity) const;
  NS_IMETHOD IsContainer(PRInt32 aId, PRBool& aIsContainer) const;
  NS_IMETHOD IsBlock(PRInt32 aId, PRBool& aIsBlock) const;

   // Observer mechanism
  NS_IMETHOD RegisterObserver(nsIElementObserver* aObserver,
                              const nsAString& aTopic,
                              const eHTMLTags* aTags = nsnull);

  NS_IMETHOD UnregisterObserver(nsIElementObserver* aObserver,
                                const nsAString& aTopic);
  NS_IMETHOD GetTopicObservers(const nsAString& aTopic,
                               nsIObserverEntry** aEntry);

  nsresult CheckQName(const nsAString& aQName,
                      PRBool aNamespaceAware, const PRUnichar** aColon);

  PRBool IsXMLLetter(PRUnichar aChar)
  {
    return MOZ_XMLIsLetter(reinterpret_cast<const char*>(&aChar));
  }
  PRBool IsXMLNCNameChar(PRUnichar aChar)
  {
    return MOZ_XMLIsNCNameChar(reinterpret_cast<const char*>(&aChar));
  }
  PRUint32 DecodeEntity(const PRUnichar* aStart, const PRUnichar* aEnd,
                        const PRUnichar** aNext, PRUnichar* aResult)
  {
    *aNext = nsnull;
    return MOZ_XMLTranslateEntity(reinterpret_cast<const char*>(aStart),
                                  reinterpret_cast<const char*>(aEnd),
                                  reinterpret_cast<const char**>(aNext),
                                  aResult);
  }

protected:
  nsObserverEntry* GetEntry(const nsAString& aTopic);
  nsresult CreateEntry(const nsAString& aTopic,
                       nsObserverEntry** aEntry);

  nsDeque  mEntries;  //each topic holds a list of observers per tag.
  PRBool   mHaveNotifiedCategoryObservers;
};

#endif
