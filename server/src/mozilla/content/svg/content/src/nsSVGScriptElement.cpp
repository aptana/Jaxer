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
 * The Original Code is the Mozilla SVG project.
 *
 * The Initial Developer of the Original Code is
 * Crocodile Clips Ltd..
 * Portions created by the Initial Developer are Copyright (C) 2002
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Alex Fritze <alex@croczilla.com> (original author)
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#include "nsSVGElement.h"
#include "nsGkAtoms.h"
#include "nsIDOMSVGScriptElement.h"
#include "nsIDOMSVGURIReference.h"
#include "nsCOMPtr.h"
#include "nsSVGAnimatedString.h"
#include "nsIDocument.h"
#include "nsIURI.h"
#include "nsNetUtil.h"
#include "nsScriptElement.h"
#include "nsIDOMText.h"

typedef nsSVGElement nsSVGScriptElementBase;

class nsSVGScriptElement : public nsSVGScriptElementBase,
                           public nsIDOMSVGScriptElement, 
                           public nsIDOMSVGURIReference,
                           public nsScriptElement
{
protected:
  friend nsresult NS_NewSVGScriptElement(nsIContent **aResult,
                                         nsINodeInfo *aNodeInfo);
  nsSVGScriptElement(nsINodeInfo *aNodeInfo);
  virtual nsresult Init();
  
public:
  // interfaces:
  
  NS_DECL_ISUPPORTS_INHERITED
  NS_DECL_NSIDOMSVGSCRIPTELEMENT
  NS_DECL_NSIDOMSVGURIREFERENCE

  // xxx If xpcom allowed virtual inheritance we wouldn't need to
  // forward here :-(
  NS_FORWARD_NSIDOMNODE(nsSVGScriptElementBase::)
  NS_FORWARD_NSIDOMELEMENT(nsSVGScriptElementBase::)
  NS_FORWARD_NSIDOMSVGELEMENT(nsSVGScriptElementBase::)

  // nsIScriptElement
  virtual void GetScriptType(nsAString& type);
  virtual already_AddRefed<nsIURI> GetScriptURI();
  virtual void GetScriptText(nsAString& text);
  virtual void GetScriptCharset(nsAString& charset); 

#ifdef JAXER
	virtual void SetScriptText(nsAString& text) { }
	virtual PRPackedBool GetIsRunatClient() { return PR_TRUE; }
	virtual PRPackedBool GetIsEmpty() { return PR_FALSE; }
	virtual void SetIsEmpty() { }
	virtual PRPackedBool GetEnableEval() { return PR_FALSE; }
	virtual void SetEnableEval() { }
	virtual void Execute() { }
#endif /* JAXER*/

  // nsScriptElement
  virtual PRBool HasScriptContent();

  // nsISVGValueObserver specializations:
  NS_IMETHOD DidModifySVGObservable (nsISVGValue* observable,
                                     nsISVGValue::modificationType aModType);

  // nsIContent specializations:
  virtual nsresult DoneAddingChildren(PRBool aHaveNotified);
  virtual nsresult BindToTree(nsIDocument* aDocument, nsIContent* aParent,
                              nsIContent* aBindingParent,
                              PRBool aCompileEventHandlers);

  virtual nsresult Clone(nsINodeInfo *aNodeInfo, nsINode **aResult) const;
    
protected:
  nsCOMPtr<nsIDOMSVGAnimatedString> mHref;
  PRUint32 mLineNumber;
  PRPackedBool mIsEvaluated;
  PRPackedBool mEvaluating;
};

NS_IMPL_NS_NEW_SVG_ELEMENT(Script)

//----------------------------------------------------------------------
// nsISupports methods

NS_IMPL_ADDREF_INHERITED(nsSVGScriptElement,nsSVGScriptElementBase)
NS_IMPL_RELEASE_INHERITED(nsSVGScriptElement,nsSVGScriptElementBase)

NS_INTERFACE_MAP_BEGIN(nsSVGScriptElement)
  NS_INTERFACE_MAP_ENTRY(nsIDOMNode)
  NS_INTERFACE_MAP_ENTRY(nsIDOMElement)
  NS_INTERFACE_MAP_ENTRY(nsIDOMSVGElement)
  NS_INTERFACE_MAP_ENTRY(nsIDOMSVGScriptElement)
  NS_INTERFACE_MAP_ENTRY(nsIDOMSVGURIReference)
  NS_INTERFACE_MAP_ENTRY(nsIScriptLoaderObserver)
  NS_INTERFACE_MAP_ENTRY(nsIScriptElement)
  NS_INTERFACE_MAP_ENTRY(nsIMutationObserver)
  NS_INTERFACE_MAP_ENTRY_CONTENT_CLASSINFO(SVGScriptElement)
NS_INTERFACE_MAP_END_INHERITING(nsSVGScriptElementBase)

//----------------------------------------------------------------------
// Implementation

nsSVGScriptElement::nsSVGScriptElement(nsINodeInfo *aNodeInfo)
  : nsSVGScriptElementBase(aNodeInfo),
    mLineNumber(0),
    mIsEvaluated(PR_FALSE),
    mEvaluating(PR_FALSE)
{
  AddMutationObserver(this);
}

nsresult
nsSVGScriptElement::Init()
{
  nsresult rv;

  // nsIDOMSVGURIReference properties

  // DOM property: href , #REQUIRED attrib: xlink:href
  // XXX: enforce requiredness
  {
    rv = NS_NewSVGAnimatedString(getter_AddRefs(mHref));
    NS_ENSURE_SUCCESS(rv,rv);
    rv = AddMappedSVGValue(nsGkAtoms::href, mHref, kNameSpaceID_XLink);
    NS_ENSURE_SUCCESS(rv,rv);
  }

  return NS_OK;
}

//----------------------------------------------------------------------
// nsIDOMNode methods

NS_IMPL_ELEMENT_CLONE_WITH_INIT(nsSVGScriptElement)

//----------------------------------------------------------------------
// nsIDOMSVGScriptElement methods

/* attribute DOMString type; */
NS_IMETHODIMP
nsSVGScriptElement::GetType(nsAString & aType)
{
  GetAttr(kNameSpaceID_None, nsGkAtoms::type, aType);

  return NS_OK;
}
NS_IMETHODIMP
nsSVGScriptElement::SetType(const nsAString & aType)
{
  NS_ERROR("write me!");
  return NS_ERROR_NOT_IMPLEMENTED;
}

//----------------------------------------------------------------------
// nsIDOMSVGURIReference methods

/* readonly attribute nsIDOMSVGAnimatedString href; */
NS_IMETHODIMP
nsSVGScriptElement::GetHref(nsIDOMSVGAnimatedString * *aHref)
{
  *aHref = mHref;
  NS_IF_ADDREF(*aHref);
  return NS_OK;
}

//----------------------------------------------------------------------
// nsIScriptElement methods

void
nsSVGScriptElement::GetScriptType(nsAString& type)
{
  GetType(type);
}

// variation of this code in nsHTMLScriptElement - check if changes
// need to be transfered when modifying

already_AddRefed<nsIURI>
nsSVGScriptElement::GetScriptURI()
{
  nsIURI *uri = nsnull;
  nsAutoString src;
  mHref->GetAnimVal(src);
  if (!src.IsEmpty()) {
    nsCOMPtr<nsIURI> baseURI = GetBaseURI();
    NS_NewURI(&uri, src, nsnull, baseURI);
  }
  return uri;
}

void
nsSVGScriptElement::GetScriptText(nsAString& text)
{
  nsContentUtils::GetNodeTextContent(this, PR_FALSE, text);
}

void
nsSVGScriptElement::GetScriptCharset(nsAString& charset)
{
  charset.Truncate();
}

//----------------------------------------------------------------------
// nsScriptElement methods

PRBool
nsSVGScriptElement::HasScriptContent()
{
  nsAutoString src;
  mHref->GetAnimVal(src);
  return !src.IsEmpty() || nsContentUtils::HasNonEmptyTextContent(this);
}

//----------------------------------------------------------------------
// nsISVGValueObserver methods

NS_IMETHODIMP
nsSVGScriptElement::DidModifySVGObservable(nsISVGValue* aObservable,
                                           nsISVGValue::modificationType aModType)
{
  nsresult rv = nsSVGScriptElementBase::DidModifySVGObservable(aObservable,
                                                               aModType);
  
  // if aObservable==mHref:
  MaybeProcessScript();
  
  return rv;
}

//----------------------------------------------------------------------
// nsIContent methods

nsresult
nsSVGScriptElement::DoneAddingChildren(PRBool aHaveNotified)
{
  mDoneAddingChildren = PR_TRUE;
  return MaybeProcessScript();
}

nsresult
nsSVGScriptElement::BindToTree(nsIDocument* aDocument, nsIContent* aParent,
                               nsIContent* aBindingParent,
                               PRBool aCompileEventHandlers)
{
  nsresult rv = nsSVGScriptElementBase::BindToTree(aDocument, aParent,
                                                   aBindingParent,
                                                   aCompileEventHandlers);
  NS_ENSURE_SUCCESS(rv, rv);

  if (aDocument) {
    MaybeProcessScript();
  }

  return NS_OK;
}
