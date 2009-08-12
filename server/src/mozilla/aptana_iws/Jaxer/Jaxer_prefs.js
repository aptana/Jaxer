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

#filter substitution

// SYNTAX HINTS:  dashes are delimiters.  Use underscores instead.
//  The first character after a period must be alphabetic.

// This option sets up an html document that will be loaded
// everytime a callback is processed.  This has to be a local file.
// If not specified, an empty document will be loaded.
// pref("Jaxer.dev.LoadDocForCallback", "resource:///framework/callback.html");

// If the Jaxer framework fails to load and execute properly, should Jaxer be bypassed
// and the original document sent to the browser? This is false by default
// to prevent server-side code to be sent to the client in such a case
pref("Jaxer.dev.BypassJaxerIfFrameworkFailToLoad", false);

// If the framework fails to load and execute properly, and Jaxer is not bypassed (see above),
// Jaxer can display an error document instead of the default
// pref("Jaxer.dev.FailedLoadFrameworkErrorDoc", "resource:///framework/ErrorDocFrameworkFailsLoad.html");

// This option enables whether communications between Jaxer and
// its manager is dumped.  This generates a lot of log messages!
pref("Jaxer.dev.DumpProtocol", false);

// Note: the logging levels used by the Jaxer framework are configured
// via configLog.js and not by the following which are used only by Jaxer core.

// This option sets the log level for Jaxer core.  Valid levels are:
// TRACE, DEBUG, INFO, WARN, ERROR, FATAL.
pref("Jaxer.Core.LogLevel", "INFO");

// This option list enables/disables whether Jaxer will
// fire NewHTMLEvent's for the specified element tag name.
// If the element is not listed here, the element will
// _NOT_ have an event fired.
pref("Jaxer.HTML.Tag.Monitor.1", "html");
pref("Jaxer.HTML.Tag.Monitor.2", "head");
pref("Jaxer.HTML.Tag.Monitor.3", "script");
pref("Jaxer.HTML.Tag.Monitor.4", "aptana:include");

// Setting from firefox.js
pref("general.useragent.locale", "@AB_CD@");

pref("layout.css.report_errors", false);
pref("svg.enabled", true);

pref("general.useragent.extra.jaxer", "Jaxer/@JAXER_FULL_VERSION@ (Aptana, Inc.)");
pref("intl.accept_languages", "en-us, en");
pref("intl.charset.detector", "");
pref("intl.charset.default", "ISO-8859-1");
// Use the unicode ellipsis char, \u2026
pref("intl.ellipsis", "…");

pref("dom.max_script_run_time", 10);
pref("Jaxer.JS.memory_limit", 24);

pref("Jaxer.dom.pixel_depth", 16);
pref("Jaxer.dom.screen_width", 800);
pref("Jaxer.dom.screen_height", 600);
