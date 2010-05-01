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
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#ifndef nsIParserService_h__
#define nsIParserService_h__

#include "nsISupports.h"
#include "nsStringGlue.h"
#include "nsHTMLTags.h"
#include "nsIElementObserver.h"

class nsIParser;
class nsIParserNode;

#define NS_PARSERSERVICE_CONTRACTID "@mozilla.org/parser/parser-service;1"

// {90a92e37-abd6-441b-9b39-4064d98e1ede}
#define NS_IPARSERSERVICE_IID \
{ 0x90a92e37, 0xabd6, 0x441b, { 0x9b, 0x39, 0x40, 0x64, 0xd9, 0x8e, 0x1e, 0xde } }

// {78081E70-AD53-11d5-8498-0010A4E0C706}
#define NS_IOBSERVERENTRY_IID \
{ 0x78081e70, 0xad53, 0x11d5, { 0x84, 0x98, 0x00, 0x10, 0xa4, 0xe0, 0xc7, 0x06 } }


class nsIObserverEntry : public nsISupports {
 public:
  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IOBSERVERENTRY_IID)

  NS_IMETHOD Notify(nsIParserNode* aNode,
                    nsIParser* aParser,
                    nsISupports* aWebShell,
                    const PRUint32 aFlags) = 0;

};


NS_DEFINE_STATIC_IID_ACCESSOR(nsIObserverEntry, NS_IOBSERVERENTRY_IID)

class nsIParserService : public nsISupports {
 public:
  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IPARSERSERVICE_IID)

#ifdef JAXER
  virtual PRBool IsTagMonitored(PRInt32 aID) const = 0;
#endif

  /**
   * Looks up the nsHTMLTag enum value corresponding to the tag in aAtom. The
   * lookup happens case insensitively.
   *
   * @param aAtom The tag to look up.
   *
   * @return PRInt32 The nsHTMLTag enum value corresponding to the tag in aAtom
   *                 or eHTMLTag_userdefined if the tag does not correspond to
   *                 any of the tag nsHTMLTag enum values.
   */
  virtual PRInt32 HTMLAtomTagToId(nsIAtom* aAtom) const = 0;

  /**
   * Looks up the nsHTMLTag enum value corresponding to the tag in aAtom.
   *
   * @param aAtom The tag to look up.
   *
   * @return PRInt32 The nsHTMLTag enum value corresponding to the tag in aAtom
   *                 or eHTMLTag_userdefined if the tag does not correspond to
   *                 any of the tag nsHTMLTag enum values.
   */
  virtual PRInt32 HTMLCaseSensitiveAtomTagToId(nsIAtom* aAtom) const = 0;

  /**
   * Looks up the nsHTMLTag enum value corresponding to the tag in aTag. The
   * lookup happens case insensitively.
   *
   * @param aTag The tag to look up.
   *
   * @return PRInt32 The nsHTMLTag enum value corresponding to the tag in aTag
   *                 or eHTMLTag_userdefined if the tag does not correspond to
   *                 any of the tag nsHTMLTag enum values.
   */
  virtual PRInt32 HTMLStringTagToId(const nsAString& aTag) const = 0;

  /**
   * Gets the tag corresponding to the nsHTMLTag enum value in aId. The
   * returned tag will be in lowercase.
   *
   * @param aId The nsHTMLTag enum value to get the tag for.
   *
   * @return const PRUnichar* The tag corresponding to the nsHTMLTag enum
   *                          value, or nsnull if the enum value doesn't
   *                          correspond to a tag (eHTMLTag_unknown,
   *                          eHTMLTag_userdefined, eHTMLTag_text, ...).
   */
  virtual const PRUnichar *HTMLIdToStringTag(PRInt32 aId) const = 0;

  /**
   * Gets the tag corresponding to the nsHTMLTag enum value in aId. The
   * returned tag will be in lowercase.
   *
   * @param aId The nsHTMLTag enum value to get the tag for.
   *
   * @return nsIAtom* The tag corresponding to the nsHTMLTag enum value, or
   *                  nsnull if the enum value doesn't correspond to a tag
   *                  (eHTMLTag_unknown, eHTMLTag_userdefined, eHTMLTag_text,
   *                  ...).
   */
  virtual nsIAtom *HTMLIdToAtomTag(PRInt32 aId) const = 0;
  
  NS_IMETHOD HTMLConvertEntityToUnicode(const nsAString& aEntity, 
                                        PRInt32* aUnicode) const = 0;

  NS_IMETHOD HTMLConvertUnicodeToEntity(PRInt32 aUnicode,
                                        nsCString& aEntity) const = 0;

  NS_IMETHOD IsContainer(PRInt32 aId, PRBool& aIsContainer) const = 0;
  NS_IMETHOD IsBlock(PRInt32 aId, PRBool& aIsBlock) const = 0;

  // Observer mechanism
  NS_IMETHOD RegisterObserver(nsIElementObserver* aObserver,
                              const nsAString& aTopic,
                              const eHTMLTags* aTags = nsnull) = 0;

  NS_IMETHOD UnregisterObserver(nsIElementObserver* aObserver,
                                const nsAString& aTopic) = 0;
  NS_IMETHOD GetTopicObservers(const nsAString& aTopic,
                               nsIObserverEntry** aEntry) = 0;

  virtual nsresult CheckQName(const nsAString& aQName,
                              PRBool aNamespaceAware,
                              const PRUnichar** aColon) = 0;
  virtual PRBool IsXMLLetter(PRUnichar aChar) = 0;
  virtual PRBool IsXMLNCNameChar(PRUnichar aChar) = 0;

  /**
   * Decodes an entity into a UTF-16 character. If a ; is found between aStart
   * and aEnd it will try to decode the entity and set aNext to point to the
   * character after the ;. The resulting UTF-16 character will be written in
   * aResult, so if the entity is a valid numeric entity there needs to be
   * space for at least two PRUnichars.
   *
   * @param aStart pointer to the character after the ampersand. 
   * @param aEnd pointer to the position after the last character of the
   *             string.
   * @param aNext [out] will be set to the character after the ; or null if
   *                    the decoding was unsuccessful.
   * @param aResult the buffer to write the resulting UTF-16 character in.
   * @return the number of PRUnichars written to aResult.
   */
  virtual PRUint32 DecodeEntity(const PRUnichar* aStart, const PRUnichar* aEnd,
                                const PRUnichar** aNext,
                                PRUnichar* aResult) = 0;
};

NS_DEFINE_STATIC_IID_ACCESSOR(nsIParserService, NS_IPARSERSERVICE_IID)

#endif // nsIParserService_h__
