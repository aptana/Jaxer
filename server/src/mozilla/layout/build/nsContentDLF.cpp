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
#include "nsCOMPtr.h"
#include "nsContentDLF.h"
#include "nsGenericHTMLElement.h"
#include "nsGkAtoms.h"
#include "nsIComponentManager.h"
#include "nsIComponentRegistrar.h"
#include "nsICategoryManager.h"
#include "nsIDocumentLoaderFactory.h"
#include "nsIDocument.h"
#include "nsIDocumentViewer.h"
#include "nsIURL.h"
#include "nsICSSStyleSheet.h"
#include "nsNodeInfo.h"
#include "nsNodeInfoManager.h"
#include "nsString.h"
#include "nsContentCID.h"
#include "prprf.h"
#include "nsNetUtil.h"
#include "nsICSSLoader.h"
#include "nsCRT.h"
#include "nsIViewSourceChannel.h"

#include "imgILoader.h"
#include "nsIParser.h"

#ifdef JAXER
#include "nsIDOMDocument.h"
#include "aptEventDocInitializedData.h"
#include "aptEventTypeManager.h"
#endif /* JAXER */
// plugins
#include "nsIPluginManager.h"
#include "nsIPluginHost.h"
static NS_DEFINE_CID(kPluginManagerCID, NS_PLUGINMANAGER_CID);
static NS_DEFINE_CID(kPluginDocumentCID, NS_PLUGINDOCUMENT_CID);

#ifdef JAXER
static NS_DEFINE_CID(kEventTypeManagerCID, APT_EVENTTYPEMANAGER_CID);
#endif /* JAXER */

// URL for the "user agent" style sheet
#define UA_CSS_URL "resource://gre/res/ua.css"

// Factory code for creating variations on html documents

#undef NOISY_REGISTRY

static NS_DEFINE_IID(kHTMLDocumentCID, NS_HTMLDOCUMENT_CID);
static NS_DEFINE_IID(kXMLDocumentCID, NS_XMLDOCUMENT_CID);
#ifdef MOZ_SVG
static NS_DEFINE_IID(kSVGDocumentCID, NS_SVGDOCUMENT_CID);
#endif
static NS_DEFINE_IID(kImageDocumentCID, NS_IMAGEDOCUMENT_CID);
static NS_DEFINE_IID(kXULDocumentCID, NS_XULDOCUMENT_CID);

nsresult
NS_NewDocumentViewer(nsIDocumentViewer** aResult);

// XXXbz if you change the MIME types here, be sure to update
// nsIParser.h and DetermineParseMode in nsParser.cpp accordingly.
static const char* const gHTMLTypes[] = {
  "text/html",
  "text/plain",
  "text/css",
  "text/javascript",
  "text/ecmascript",
  "application/javascript",
  "application/ecmascript",
  "application/x-javascript",
#ifdef MOZ_VIEW_SOURCE
  "application/x-view-source", //XXX I wish I could just use nsMimeTypes.h here
#endif
  "application/xhtml+xml",
  0
};
  
static const char* const gXMLTypes[] = {
  "text/xml",
  "application/xml",
  "application/rdf+xml",
  "text/rdf",
  0
};

#ifdef MOZ_SVG
static const char* const gSVGTypes[] = {
  "image/svg+xml",
  0
};

PRBool NS_SVGEnabled();
#endif

static const char* const gXULTypes[] = {
  "application/vnd.mozilla.xul+xml",
  "mozilla.application/cached-xul",
  0
};

nsICSSStyleSheet* nsContentDLF::gUAStyleSheet;

nsresult
NS_NewContentDocumentLoaderFactory(nsIDocumentLoaderFactory** aResult)
{
  NS_PRECONDITION(aResult, "null OUT ptr");
  if (!aResult) {
    return NS_ERROR_NULL_POINTER;
  }
  nsContentDLF* it = new nsContentDLF();
  if (!it) {
    return NS_ERROR_OUT_OF_MEMORY;
  }

  return CallQueryInterface(it, aResult);
}

nsContentDLF::nsContentDLF()
{
}

nsContentDLF::~nsContentDLF()
{
}

NS_IMPL_ISUPPORTS1(nsContentDLF,
                   nsIDocumentLoaderFactory)

NS_IMETHODIMP
nsContentDLF::CreateInstance(const char* aCommand,
                             nsIChannel* aChannel,
                             nsILoadGroup* aLoadGroup,
                             const char* aContentType, 
                             nsISupports* aContainer,
                             nsISupports* aExtraInfo,
                             nsIStreamListener** aDocListener,
                             nsIContentViewer** aDocViewer)
{
  EnsureUAStyleSheet();

  // Are we viewing source?
#ifdef MOZ_VIEW_SOURCE
  nsCOMPtr<nsIViewSourceChannel> viewSourceChannel = do_QueryInterface(aChannel);
  if (viewSourceChannel)
  {
    aCommand = "view-source";

    // The parser freaks out when it sees the content-type that a
    // view-source channel normally returns.  Get the actual content
    // type of the data.  If it's known, use it; otherwise use
    // text/plain.
    nsCAutoString type;
    viewSourceChannel->GetOriginalContentType(type);
    PRBool knownType = PR_FALSE;
    PRInt32 typeIndex;
    for (typeIndex = 0; gHTMLTypes[typeIndex] && !knownType; ++typeIndex) {
      if (type.Equals(gHTMLTypes[typeIndex]) &&
          !type.EqualsLiteral("application/x-view-source")) {
        knownType = PR_TRUE;
      }
    }

    for (typeIndex = 0; gXMLTypes[typeIndex] && !knownType; ++typeIndex) {
      if (type.Equals(gXMLTypes[typeIndex])) {
        knownType = PR_TRUE;
      }
    }

#ifdef MOZ_SVG
    if (NS_SVGEnabled()) {
      for (typeIndex = 0; gSVGTypes[typeIndex] && !knownType; ++typeIndex) {
        if (type.Equals(gSVGTypes[typeIndex])) {
          knownType = PR_TRUE;
        }
      }
    }
#endif // MOZ_SVG

    for (typeIndex = 0; gXULTypes[typeIndex] && !knownType; ++typeIndex) {
      if (type.Equals(gXULTypes[typeIndex])) {
        knownType = PR_TRUE;
      }
    }

    if (knownType) {
      viewSourceChannel->SetContentType(type);
    } else {
      viewSourceChannel->SetContentType(NS_LITERAL_CSTRING("text/plain"));
    }
  } else if (0 == PL_strcmp("application/x-view-source", aContentType)) {
    aChannel->SetContentType(NS_LITERAL_CSTRING("text/plain"));
    aContentType = "text/plain";
  }
#endif
  // Try html
  int typeIndex=0;
  while(gHTMLTypes[typeIndex]) {
    if (0 == PL_strcmp(gHTMLTypes[typeIndex++], aContentType)) {
      return CreateDocument(aCommand, 
                            aChannel, aLoadGroup,
                            aContainer, kHTMLDocumentCID,
                            aDocListener, aDocViewer);
    }
  }

  // Try XML
  typeIndex = 0;
  while(gXMLTypes[typeIndex]) {
    if (0== PL_strcmp(gXMLTypes[typeIndex++], aContentType)) {
      return CreateDocument(aCommand, 
                            aChannel, aLoadGroup,
                            aContainer, kXMLDocumentCID,
                            aDocListener, aDocViewer);
    }
  }

#ifdef MOZ_SVG
  if (NS_SVGEnabled()) {
    // Try SVG
    typeIndex = 0;
    while(gSVGTypes[typeIndex]) {
      if (!PL_strcmp(gSVGTypes[typeIndex++], aContentType)) {
        return CreateDocument(aCommand,
                              aChannel, aLoadGroup,
                              aContainer, kSVGDocumentCID,
                              aDocListener, aDocViewer);
      }
    }
  }
#endif

  // Try XUL
  typeIndex = 0;
  while (gXULTypes[typeIndex]) {
    if (0 == PL_strcmp(gXULTypes[typeIndex++], aContentType)) {
      return CreateXULDocument(aCommand, 
                               aChannel, aLoadGroup,
                               aContentType, aContainer,
                               aExtraInfo, aDocListener, aDocViewer);
    }
  }

  // Try image types
  nsCOMPtr<imgILoader> loader(do_GetService("@mozilla.org/image/loader;1"));
  PRBool isReg = PR_FALSE;
  loader->SupportImageWithMimeType(aContentType, &isReg);
  if (isReg) {
    return CreateDocument(aCommand, 
                          aChannel, aLoadGroup,
                          aContainer, kImageDocumentCID,
                          aDocListener, aDocViewer);
  }

  nsCOMPtr<nsIPluginHost> ph (do_GetService(kPluginManagerCID));
  if(ph && NS_SUCCEEDED(ph->IsPluginEnabledForType(aContentType))) {
    return CreateDocument(aCommand,
                          aChannel, aLoadGroup,
                          aContainer, kPluginDocumentCID,
                          aDocListener, aDocViewer);
  }


  // If we get here, then we weren't able to create anything. Sorry!
  return NS_ERROR_FAILURE;
}


NS_IMETHODIMP
nsContentDLF::CreateInstanceForDocument(nsISupports* aContainer,
                                        nsIDocument* aDocument,
                                        const char *aCommand,
                                        nsIContentViewer** aDocViewerResult)
{
  nsresult rv = NS_ERROR_FAILURE;  

  EnsureUAStyleSheet();

  do {
    nsCOMPtr<nsIDocumentViewer> docv;
    rv = NS_NewDocumentViewer(getter_AddRefs(docv));
    if (NS_FAILED(rv))
      break;

#ifdef JAXER
	  if (gUAStyleSheet)
#endif /* JAXER */
    docv->SetUAStyleSheet(static_cast<nsIStyleSheet*>(gUAStyleSheet));

    // Bind the document to the Content Viewer
    nsIContentViewer* cv = static_cast<nsIContentViewer*>(docv.get());
    rv = cv->LoadStart(aDocument);
    NS_ADDREF(*aDocViewerResult = cv);
  } while (PR_FALSE);

  return rv;
}

NS_IMETHODIMP
nsContentDLF::CreateBlankDocument(nsILoadGroup *aLoadGroup,
                                  nsIPrincipal* aPrincipal,
                                  nsIDocument **aDocument)
{
  *aDocument = nsnull;

  nsresult rv = NS_ERROR_FAILURE;

  // create a new blank HTML document
  nsCOMPtr<nsIDocument> blankDoc(do_CreateInstance(kHTMLDocumentCID));

  if (blankDoc) {
    // initialize
    nsCOMPtr<nsIURI> uri;
    NS_NewURI(getter_AddRefs(uri), NS_LITERAL_CSTRING("about:blank"));
    if (uri) {
      blankDoc->ResetToURI(uri, aLoadGroup, aPrincipal);
      rv = NS_OK;
    }
  }

  // add some simple content structure
  if (NS_SUCCEEDED(rv)) {
    rv = NS_ERROR_FAILURE;

    nsNodeInfoManager *nim = blankDoc->NodeInfoManager();

    nsCOMPtr<nsINodeInfo> htmlNodeInfo;

    // generate an html html element
    nim->GetNodeInfo(nsGkAtoms::html, 0, kNameSpaceID_None,
                     getter_AddRefs(htmlNodeInfo));
    nsCOMPtr<nsIContent> htmlElement = NS_NewHTMLHtmlElement(htmlNodeInfo);

    // generate an html head element
    nim->GetNodeInfo(nsGkAtoms::head, 0, kNameSpaceID_None,
                     getter_AddRefs(htmlNodeInfo));
    nsCOMPtr<nsIContent> headElement = NS_NewHTMLHeadElement(htmlNodeInfo);

    // generate an html body element
    nim->GetNodeInfo(nsGkAtoms::body, 0, kNameSpaceID_None,
                     getter_AddRefs(htmlNodeInfo));
    nsCOMPtr<nsIContent> bodyElement = NS_NewHTMLBodyElement(htmlNodeInfo);

    // blat in the structure
    if (htmlElement && headElement && bodyElement) {
      NS_ASSERTION(blankDoc->GetChildCount() == 0,
                   "Shouldn't have children");
      rv = blankDoc->AppendChildTo(htmlElement, PR_FALSE);
      if (NS_SUCCEEDED(rv)) {
        rv = htmlElement->AppendChildTo(headElement, PR_FALSE);

        if (NS_SUCCEEDED(rv)) {
          // XXXbz Why not notifying here?
          htmlElement->AppendChildTo(bodyElement, PR_FALSE);
        }
      }
    }
  }

  // add a nice bow
  if (NS_SUCCEEDED(rv)) {
    blankDoc->SetDocumentCharacterSetSource(kCharsetFromDocTypeDefault);
    blankDoc->SetDocumentCharacterSet(NS_LITERAL_CSTRING("UTF-8"));
    
    *aDocument = blankDoc;
    NS_ADDREF(*aDocument);
  }
  return rv;
}


nsresult
nsContentDLF::CreateDocument(const char* aCommand,
                             nsIChannel* aChannel,
                             nsILoadGroup* aLoadGroup,
                             nsISupports* aContainer,
                             const nsCID& aDocumentCID,
                             nsIStreamListener** aDocListener,
                             nsIContentViewer** aDocViewer)
{
  nsresult rv = NS_ERROR_FAILURE;

  nsCOMPtr<nsIURI> aURL;
  rv = aChannel->GetURI(getter_AddRefs(aURL));
  if (NS_FAILED(rv)) return rv;

#ifdef NOISY_CREATE_DOC
  if (nsnull != aURL) {
    nsAutoString tmp;
    aURL->ToString(tmp);
    fputs(NS_LossyConvertUTF16toASCII(tmp).get(), stdout);
    printf(": creating document\n");
  }
#endif

  nsCOMPtr<nsIDocument> doc;
  nsCOMPtr<nsIDocumentViewer> docv;
  do {
    // Create the document
    doc = do_CreateInstance(aDocumentCID, &rv);
    if (NS_FAILED(rv))
      break;

    // Create the document viewer  XXX: could reuse document viewer here!
    rv = NS_NewDocumentViewer(getter_AddRefs(docv));
    if (NS_FAILED(rv))
      break;
#ifdef JAXER
	  if (gUAStyleSheet)
#endif /* JAXER */
    docv->SetUAStyleSheet(gUAStyleSheet);

    doc->SetContainer(aContainer);

    // Initialize the document to begin loading the data.  An
    // nsIStreamListener connected to the parser is returned in
    // aDocListener.
    rv = doc->StartDocumentLoad(aCommand, aChannel, aLoadGroup, aContainer, aDocListener, PR_TRUE);
    if (NS_FAILED(rv))
      break;

    // Bind the document to the Content Viewer
    rv = docv->LoadStart(doc);
    *aDocViewer = docv;
    NS_IF_ADDREF(*aDocViewer);
  } while (PR_FALSE);

#ifdef JAXER
  // Allocate a DocInitialized event object.
  nsCOMPtr<aptIEventDocInitialized> di = do_CreateInstance(APT_EVENT_DOC_INITIALIZED_CONTRACTID);
  NS_ENSURE_STATE(di);

  nsCOMPtr<nsIDOMDocument> aDOMDocument = do_QueryInterface(doc);

  // Set some state for the event.
  di->SetDOM(aDOMDocument);
  di->Init();

  // Fire the DocInitialized event out to all observers.
  nsCOMPtr<aptEventTypeManager> mEventTypeManager = do_GetService(kEventTypeManagerCID, &rv);
  rv = mEventTypeManager->FireEvent(di, aptEventName_DocInitialized);
#endif /* JAXER */
  return rv;
}

nsresult
nsContentDLF::CreateXULDocument(const char* aCommand,
                                nsIChannel* aChannel,
                                nsILoadGroup* aLoadGroup,
                                const char* aContentType,
                                nsISupports* aContainer,
                                nsISupports* aExtraInfo,
                                nsIStreamListener** aDocListener,
                                nsIContentViewer** aDocViewer)
{
  nsresult rv;
  nsCOMPtr<nsIDocument> doc = do_CreateInstance(kXULDocumentCID, &rv);
  if (NS_FAILED(rv)) return rv;

  nsCOMPtr<nsIDocumentViewer> docv;
  rv = NS_NewDocumentViewer(getter_AddRefs(docv));
  if (NS_FAILED(rv)) return rv;

  // Load the UA style sheet if we haven't already done that
#ifdef JAXER
	if (gUAStyleSheet)
#endif /* JAXER */
  docv->SetUAStyleSheet(gUAStyleSheet);

  nsCOMPtr<nsIURI> aURL;
  rv = aChannel->GetURI(getter_AddRefs(aURL));
  if (NS_FAILED(rv)) return rv;

  /* 
   * Initialize the document to begin loading the data...
   *
   * An nsIStreamListener connected to the parser is returned in
   * aDocListener.
   */

  doc->SetContainer(aContainer);

  rv = doc->StartDocumentLoad(aCommand, aChannel, aLoadGroup, aContainer, aDocListener, PR_TRUE);
  if (NS_SUCCEEDED(rv)) {
    /*
     * Bind the document to the Content Viewer...
     */
    rv = docv->LoadStart(doc);
    *aDocViewer = docv;
    NS_IF_ADDREF(*aDocViewer);
  }
   
  return rv;
}

static nsresult
RegisterTypes(nsICategoryManager* aCatMgr,
              const char* const* aTypes,
              PRBool aPersist = PR_TRUE)
{
  nsresult rv = NS_OK;
  while (*aTypes) {
    const char* contentType = *aTypes++;
#ifdef NOISY_REGISTRY
    printf("Register %s => %s\n", contractid, aPath);
#endif
    // add the MIME types layout can handle to the handlers category.
    // this allows users of layout's viewers (the docshell for example)
    // to query the types of viewers layout can create.
    rv = aCatMgr->AddCategoryEntry("Gecko-Content-Viewers", contentType,
                                   "@mozilla.org/content/document-loader-factory;1",
                                   aPersist, PR_TRUE, nsnull);
    if (NS_FAILED(rv)) break;
  }
  return rv;
}

static nsresult UnregisterTypes(nsICategoryManager* aCatMgr,
                                const char* const* aTypes)
{
  nsresult rv = NS_OK;
  while (*aTypes) {
    const char* contentType = *aTypes++;
    rv = aCatMgr->DeleteCategoryEntry("Gecko-Content-Viewers", contentType, PR_TRUE);
    if (NS_FAILED(rv)) break;
  }
  return rv;

}

#ifdef MOZ_SVG
NS_IMETHODIMP
nsContentDLF::RegisterSVG()
{
  nsresult rv;
  nsCOMPtr<nsICategoryManager> catmgr(do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv));
  if (NS_FAILED(rv)) return rv;

  return RegisterTypes(catmgr, gSVGTypes, PR_FALSE);
}

NS_IMETHODIMP
nsContentDLF::UnregisterSVG()
{
  nsresult rv;
  nsCOMPtr<nsICategoryManager> catmgr(do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv));
  if (NS_FAILED(rv)) return rv;

  return UnregisterTypes(catmgr, gSVGTypes);
}
#endif

NS_IMETHODIMP
nsContentDLF::RegisterDocumentFactories(nsIComponentManager* aCompMgr,
                                        nsIFile* aPath,
                                        const char *aLocation,
                                        const char *aType,
                                        const nsModuleComponentInfo* aInfo)
{
  nsresult rv;

  nsCOMPtr<nsICategoryManager> catmgr(do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv));
  if (NS_FAILED(rv)) return rv;

  do {
    rv = RegisterTypes(catmgr, gHTMLTypes);
    if (NS_FAILED(rv))
      break;
    rv = RegisterTypes(catmgr, gXMLTypes);
    if (NS_FAILED(rv))
      break;
    rv = RegisterTypes(catmgr, gXULTypes);
    if (NS_FAILED(rv))
      break;
  } while (PR_FALSE);
  return rv;
}

NS_IMETHODIMP
nsContentDLF::UnregisterDocumentFactories(nsIComponentManager* aCompMgr,
                                          nsIFile* aPath,
                                          const char* aRegistryLocation,
                                          const nsModuleComponentInfo* aInfo)
{
  nsresult rv;
  nsCOMPtr<nsICategoryManager> catmgr(do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv));
  if (NS_FAILED(rv)) return rv;

  do {
    rv = UnregisterTypes(catmgr, gHTMLTypes);
    if (NS_FAILED(rv))
      break;
    rv = UnregisterTypes(catmgr, gXMLTypes);
    if (NS_FAILED(rv))
      break;
#ifdef MOZ_SVG
    rv = UnregisterTypes(catmgr, gSVGTypes);
    if (NS_FAILED(rv))
      break;
#endif
    rv = UnregisterTypes(catmgr, gXULTypes);
    if (NS_FAILED(rv))
      break;
  } while (PR_FALSE);

  return rv;
}

/* static */ nsresult
nsContentDLF::EnsureUAStyleSheet()
{
#ifdef JAXER
	if (PR_TRUE)
		return NS_OK;
#endif /* JAXER */
  if (gUAStyleSheet)
    return NS_OK;

  // Load the UA style sheet
  nsCOMPtr<nsIURI> uri;
  nsresult rv = NS_NewURI(getter_AddRefs(uri), NS_LITERAL_CSTRING(UA_CSS_URL));
  if (NS_FAILED(rv)) {
#ifdef DEBUG
    printf("*** open of %s failed: error=%x\n", UA_CSS_URL, rv);
#endif
    return rv;
  }
  nsCOMPtr<nsICSSLoader> cssLoader;
  NS_NewCSSLoader(getter_AddRefs(cssLoader));
  if (!cssLoader)
    return NS_ERROR_OUT_OF_MEMORY;
  rv = cssLoader->LoadSheetSync(uri, PR_TRUE, &gUAStyleSheet);
#ifdef DEBUG
  if (NS_FAILED(rv))
    printf("*** open of %s failed: error=%x\n", UA_CSS_URL, rv);
#endif
  return rv;
}
