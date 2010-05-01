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
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Travis Bogard <travis@netscape.com>
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#include "nscore.h"
#include "nsScreen.h"
#include "nsPIDOMWindow.h"
#include "nsIBaseWindow.h"
#include "nsIDocShell.h"
#include "nsIDocShellTreeItem.h"
#include "nsIDeviceContext.h"
#include "nsIWidget.h"
#include "nsPresContext.h"
#include "nsCOMPtr.h"
#include "nsDOMClassInfo.h"
#include "nsIInterfaceRequestorUtils.h"
#ifdef JAXER
#include "nsContentUtils.h"
#endif /* JAXER */

//
//  Screen class implementation
//
nsScreen::nsScreen(nsIDocShell* aDocShell)
  : mDocShell(aDocShell)
{
}

nsScreen::~nsScreen()
{
}


// QueryInterface implementation for nsScreen
NS_INTERFACE_MAP_BEGIN(nsScreen)
  NS_INTERFACE_MAP_ENTRY(nsISupports)
  NS_INTERFACE_MAP_ENTRY(nsIDOMScreen)
  NS_DOM_INTERFACE_MAP_ENTRY_CLASSINFO(Screen)
NS_INTERFACE_MAP_END


NS_IMPL_ADDREF(nsScreen)
NS_IMPL_RELEASE(nsScreen)


NS_IMETHODIMP
nsScreen::SetDocShell(nsIDocShell* aDocShell)
{
   mDocShell = aDocShell; // Weak Reference
   return NS_OK;
}

NS_IMETHODIMP
nsScreen::GetTop(PRInt32* aTop)
{
  nsRect rect;
  nsresult rv = GetRect(rect);

  *aTop = rect.y;

  return rv;
}


NS_IMETHODIMP
nsScreen::GetLeft(PRInt32* aLeft)
{
  nsRect rect;
  nsresult rv = GetRect(rect);

  *aLeft = rect.x;

  return rv;
}


NS_IMETHODIMP
nsScreen::GetWidth(PRInt32* aWidth)
{
  nsRect rect;
  nsresult rv = GetRect(rect);

  *aWidth = rect.width;

  return rv;
}

NS_IMETHODIMP
nsScreen::GetHeight(PRInt32* aHeight)
{
  nsRect rect;
  nsresult rv = GetRect(rect);

  *aHeight = rect.height;

  return rv;
}

NS_IMETHODIMP
nsScreen::GetPixelDepth(PRInt32* aPixelDepth)
{
#ifndef JAXER
  nsIDeviceContext* context = GetDeviceContext();

  if (!context) {
    *aPixelDepth = -1;

    return NS_ERROR_FAILURE;
  }

  PRUint32 depth;
  context->GetDepth(depth);
#else
  PRUint32 depth = nsContentUtils::GetIntPref("Jaxer.dom.pixel_depth", 16);
#endif /* JAXER */

  *aPixelDepth = depth;

  return NS_OK;
}

NS_IMETHODIMP
nsScreen::GetColorDepth(PRInt32* aColorDepth)
{
  return GetPixelDepth(aColorDepth);
}

NS_IMETHODIMP
nsScreen::GetAvailWidth(PRInt32* aAvailWidth)
{
  nsRect rect;
  nsresult rv = GetAvailRect(rect);

  *aAvailWidth = rect.width;

  return rv;
}

NS_IMETHODIMP
nsScreen::GetAvailHeight(PRInt32* aAvailHeight)
{
  nsRect rect;
  nsresult rv = GetAvailRect(rect);

  *aAvailHeight = rect.height;

  return rv;
}

NS_IMETHODIMP
nsScreen::GetAvailLeft(PRInt32* aAvailLeft)
{
  nsRect rect;
  nsresult rv = GetAvailRect(rect);

  *aAvailLeft = rect.x;

  return rv;
}

NS_IMETHODIMP
nsScreen::GetAvailTop(PRInt32* aAvailTop)
{
  nsRect rect;
  nsresult rv = GetAvailRect(rect);

  *aAvailTop = rect.y;

  return rv;
}

nsIDeviceContext*
nsScreen::GetDeviceContext()
{
  nsCOMPtr<nsIDocShell> docShell = mDocShell;
  while (docShell) {
    // Now make sure our size is up to date.  That will mean that the device
    // context does the right thing on multi-monitor systems when we return it to
    // the caller.  It will also make sure that our prescontext has been created,
    // if we're supposed to have one.
    nsCOMPtr<nsPIDOMWindow> win = do_GetInterface(docShell);
    if (!win) {
      // No reason to go on
      return nsnull;
    }

    win->EnsureSizeUpToDate();

    nsCOMPtr<nsIBaseWindow> baseWindow = do_QueryInterface(docShell);
    NS_ENSURE_TRUE(baseWindow, nsnull);

    nsCOMPtr<nsIWidget> mainWidget;
    baseWindow->GetMainWidget(getter_AddRefs(mainWidget));
    if (mainWidget) {
      return mainWidget->GetDeviceContext();
    }

    nsCOMPtr<nsIDocShellTreeItem> curItem = do_QueryInterface(docShell);
    nsCOMPtr<nsIDocShellTreeItem> parentItem;
    curItem->GetParent(getter_AddRefs(parentItem));
    docShell = do_QueryInterface(parentItem);
  }

  return nsnull;
}

nsresult
nsScreen::GetRect(nsRect& aRect)
{
#ifndef JAXER
  nsIDeviceContext *context = GetDeviceContext();

  if (!context) {
    return NS_ERROR_FAILURE;
  }

  context->GetRect(aRect);
#else
  aRect.x = 0;
  aRect.y = 0;
  aRect.height = nsContentUtils::GetIntPref("Jaxer.dom.screen_height", 600);
  aRect.width = nsContentUtils::GetIntPref("Jaxer.dom.screen_width", 800);
#endif /* JAXER */

  aRect.x = nsPresContext::AppUnitsToIntCSSPixels(aRect.x);
  aRect.y = nsPresContext::AppUnitsToIntCSSPixels(aRect.y);
  aRect.height = nsPresContext::AppUnitsToIntCSSPixels(aRect.height);
  aRect.width = nsPresContext::AppUnitsToIntCSSPixels(aRect.width);

  return NS_OK;
}

nsresult
nsScreen::GetAvailRect(nsRect& aRect)
{
#ifndef JAXER
  nsIDeviceContext *context = GetDeviceContext();

  if (!context) {
    return NS_ERROR_FAILURE;
  }

  context->GetClientRect(aRect);
#else
  aRect.x = 0;
  aRect.y = 0;
  aRect.height = nsContentUtils::GetIntPref("Jaxer.dom.screen_height", 600);
  aRect.width = nsContentUtils::GetIntPref("Jaxer.dom.screen_width", 800);
#endif /* JAXER */

  aRect.x = nsPresContext::AppUnitsToIntCSSPixels(aRect.x);
  aRect.y = nsPresContext::AppUnitsToIntCSSPixels(aRect.y);
  aRect.height = nsPresContext::AppUnitsToIntCSSPixels(aRect.height);
  aRect.width = nsPresContext::AppUnitsToIntCSSPixels(aRect.width);

  return NS_OK;
}
