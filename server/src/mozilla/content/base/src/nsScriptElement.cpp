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
 * The Original Code is Mozilla Code.
 *
 * The Initial Developer of the Original Code is
 * Mozilla Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Jonas Sicking <jonas@sicking.cc> (original developer)
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#include "nsScriptElement.h"
#include "nsIContent.h"
#include "nsContentUtils.h"
#include "nsGUIEvent.h"
#include "nsEventDispatcher.h"
#include "nsPresContext.h"
#include "nsScriptLoader.h"
#include "nsIParser.h"
#include "nsAutoPtr.h"
#include "nsGkAtoms.h"

#ifdef JAXER
#include "nsServiceManagerUtils.h"
#include "nsIDocShell.h"
#include "aptIDocumentFetcherService.h"
#include "aptEventTypeManager.h"
#include "aptEventBeforeScriptEvaluatedData.h"
#include "aptEventAfterScriptEvaluatedData.h"

NS_DEFINE_CID(kEventTypeManagerCID, APT_EVENTTYPEMANAGER_CID);
static aptEventTypeManager *g_EventTypeManager = nsnull;
static aptIDocumentFetcherService *g_DocumentFetcherService = nsnull;

static
aptEventTypeManager * GetEventTypeManager()
{
  if (!g_EventTypeManager) {
  	nsresult rv;
  	nsCOMPtr<aptEventTypeManager> eventTypeManager = do_GetService(kEventTypeManagerCID, &rv);
	g_EventTypeManager = eventTypeManager;
  }
  return g_EventTypeManager;
}

static
PRBool IsJaxerDocShell(nsIDocShell *pDocShell)
{
	if (!g_DocumentFetcherService) {
		nsCOMPtr<aptIDocumentFetcherService> dfs(do_GetService("@aptana.com/httpdocumentfetcher;1"));
		NS_ENSURE_TRUE(dfs, PR_FALSE);
		g_DocumentFetcherService = dfs;
	}

	nsCOMPtr<nsIDocShell> docShell;
	g_DocumentFetcherService->GetDocShell(getter_AddRefs(docShell));
	if (!docShell) {
		return PR_FALSE;
	}

	return (docShell == pDocShell);
}
#endif /* JAXER */

NS_IMETHODIMP
nsScriptElement::ScriptAvailable(nsresult aResult,
                                 nsIScriptElement *aElement,
                                 PRBool aIsInline,
                                 nsIURI *aURI,
                                 PRInt32 aLineNo)
{
  if (!aIsInline && NS_FAILED(aResult)) {
    nsCOMPtr<nsIContent> cont =
      do_QueryInterface((nsIScriptElement*) this);

    nsCOMPtr<nsPresContext> presContext =
      nsContentUtils::GetContextForContent(cont);

    nsEventStatus status = nsEventStatus_eIgnore;
    nsScriptErrorEvent event(PR_TRUE, NS_LOAD_ERROR);

    event.lineNr = aLineNo;

    NS_NAMED_LITERAL_STRING(errorString, "Error loading script");
    event.errorMsg = errorString.get();

    nsCAutoString spec;
    aURI->GetSpec(spec);

    NS_ConvertUTF8toUTF16 fileName(spec);
    event.fileName = fileName.get();

    nsEventDispatcher::Dispatch(cont, presContext, &event, nsnull, &status);
  }

  return NS_OK;
}

NS_IMETHODIMP
nsScriptElement::ScriptEvaluated(nsresult aResult,
                                 nsIScriptElement *aElement,
                                 PRBool aIsInline)
{
  nsresult rv = NS_OK;
  if (!aIsInline) {
    nsCOMPtr<nsIContent> cont =
      do_QueryInterface((nsIScriptElement*) this);

    nsCOMPtr<nsPresContext> presContext =
      nsContentUtils::GetContextForContent(cont);

    nsEventStatus status = nsEventStatus_eIgnore;
    PRUint32 type = NS_SUCCEEDED(aResult) ? NS_LOAD : NS_LOAD_ERROR;
    nsEvent event(PR_TRUE, type);
    if (type == NS_LOAD) {
      // Load event doesn't bubble.
      event.flags |= NS_EVENT_FLAG_CANT_BUBBLE;
    }

    nsEventDispatcher::Dispatch(cont, presContext, &event, nsnull, &status);
  }

  return rv;
}

void
nsScriptElement::CharacterDataChanged(nsIDocument *aDocument,
                                      nsIContent* aContent,
                                      CharacterDataChangeInfo* aInfo)
{
  MaybeProcessScript();
}

void
nsScriptElement::AttributeChanged(nsIDocument* aDocument,
                                  nsIContent* aContent,
                                  PRInt32 aNameSpaceID,
                                  nsIAtom* aAttribute,
                                  PRInt32 aModType,
                                  PRUint32 aStateMask)
{
  MaybeProcessScript();
}

void
nsScriptElement::ContentAppended(nsIDocument* aDocument,
                                 nsIContent* aContainer,
                                 PRInt32 aNewIndexInContainer)
{
  MaybeProcessScript();
}

void
nsScriptElement::ContentInserted(nsIDocument *aDocument,
                                 nsIContent* aContainer,
                                 nsIContent* aChild,
                                 PRInt32 aIndexInContainer)
{
  MaybeProcessScript();
}

static PRBool
InNonScriptingContainer(nsINode* aNode)
{
  aNode = aNode->GetNodeParent();
  while (aNode) {
    // XXX noframes and noembed are currently unconditionally not
    // displayed and processed. This might change if we support either
    // prefs or per-document container settings for not allowing
    // frames or plugins.
    if (aNode->IsNodeOfType(nsINode::eHTML)) {
      nsIAtom *localName = static_cast<nsIContent*>(aNode)->Tag();
      if (localName == nsGkAtoms::iframe ||
          localName == nsGkAtoms::noframes ||
          localName == nsGkAtoms::noembed) {
        return PR_TRUE;
      }
    }
    aNode = aNode->GetNodeParent();
  }

  return PR_FALSE;
}

nsresult
nsScriptElement::MaybeProcessScript()
{
  nsCOMPtr<nsIContent> cont =
    do_QueryInterface((nsIScriptElement*) this);

  NS_ASSERTION(cont->DebugGetSlots()->mMutationObservers.Contains(this),
               "You forgot to add self as observer");

  if (mIsEvaluated || !mDoneAddingChildren || !cont->IsInDoc() ||
      mMalformed || InNonScriptingContainer(cont) ||
      !HasScriptContent()) {
    return NS_OK;
  }

#ifdef JAXER
	nsCOMPtr<nsIDOMHTMLScriptElement> htmlScriptElement = do_QueryInterface((nsIScriptElement*) this);
	nsCOMPtr<nsISupports> container = cont->GetOwnerDoc()->GetContainer();
	nsCOMPtr<nsIDocShell> docShell = do_QueryInterface(container);
	if (docShell && !IsJaxerDocShell(docShell)) {
		htmlScriptElement = nsnull;
	}
	if (htmlScriptElement) {
		// Allocate a BeforeScriptEvaluated event object.
		nsCOMPtr<aptIEventBeforeScriptEvaluated> bse =
			do_CreateInstance(APT_EVENT_BEFORE_SCRIPT_EVALUATED_CONTRACTID);
		// Set some state for the event
		bse->Init();
		bse->SetElement(htmlScriptElement);
		nsCOMPtr<nsIURI> scriptURI = this->GetScriptURI();
		bse->SetScriptURI(scriptURI);
		// Fire the BeforeScriptEvaluated event out to all observers.
		GetEventTypeManager()->FireEvent(bse, aptEventName_BeforeScriptEvaluated);
		PRBool aEnableScriptEval = PR_TRUE;
		nsresult rv = bse->GetEnableScriptEvaluation(&aEnableScriptEval);
		// If event handler has disallowed this element to be evaluated...
		if (NS_FAILED(rv) || !aEnableScriptEval) {
			PreventExecution();
			return NS_OK;
		}
	}
#endif /* JAXER */
  nsresult scriptresult = NS_OK;
  nsRefPtr<nsScriptLoader> loader = cont->GetOwnerDoc()->ScriptLoader();
  mIsEvaluated = PR_TRUE;
  scriptresult = loader->ProcessScriptElement(this);

#ifdef JAXER
	if (htmlScriptElement) {
		// Allocate a AfterScriptEvaluated event object.
		nsCOMPtr<aptIEventAfterScriptEvaluated> ase =
			do_CreateInstance(APT_EVENT_AFTER_SCRIPT_EVALUATED_CONTRACTID);
		// Set some state for the event
		ase->Init();
		ase->SetElement(htmlScriptElement);
		// Fire the AfterScriptEvaluated event out to all observers.
		GetEventTypeManager()->FireEvent(ase, aptEventName_AfterScriptEvaluated);
	}
#endif /* JAXER */
  // The only error we don't ignore is NS_ERROR_HTMLPARSER_BLOCK
  // However we don't want to override other success values
  // (such as NS_CONTENT_SCRIPT_IS_EVENTHANDLER)
  if (NS_FAILED(scriptresult) &&
      scriptresult != NS_ERROR_HTMLPARSER_BLOCK) {
    scriptresult = NS_OK;
  }

  return scriptresult;
}
