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

#ifndef __APT_PROCESS_H__
#define __APT_PROCESS_H__

#include "jxIProcess.h"
#include "nsProcess.h"
#include "nsIScriptableUConv.h"
#include "nsIConverterInputStream.h"
#include "nsIInputStream.h"

////////////////////////////////////////////////////////////////////////////////

#define JX_PROCESS_CONTRACTID "@aptana.com/utils/jxProcess;1"

#define JX_PROCESS_CID \
{ 0x68a2fbec, 0x82f0, 0x4f1b, { 0xa5, 0x80, 0xa2, 0x52, 0x84, 0xbe, 0x71, 0x30 } }

#define MIN_ERROR_STRING_LEN 512

class jxProcessInputStream : public nsIInputStream
{
public:
	NS_DECL_ISUPPORTS
    NS_DECL_NSIINPUTSTREAM

	jxProcessInputStream()
		:m_fd(nsnull)
		,m_bClosed(PR_TRUE)
	{
	}

	~jxProcessInputStream(){};

	void Init(PRFileDesc *fd)
	{
		m_fd = fd;
		m_bClosed = PR_FALSE;
	}

	PRBool m_bClosed;

private:
	PRFileDesc* m_fd;
};

class jxProcess : public jxIProcess
{
public:
    jxProcess();

    // nsISupports interface...
    NS_DECL_ISUPPORTS

    // jxIProcess interface...
    NS_DECL_JXIPROCESS

    
private:
    ~jxProcess();
	nsresult InitAfterRun(); // for blocking
    PRStatus Clear();
    nsresult ReadBytes(PRFileDesc* fd, PRUint8 **buf, PRUint32 *nbytes); 

    // If we have an error, the error message goes here
    char     m_sError[MIN_ERROR_STRING_LEN];
	void SetError(PRInt32 nExit, char* fmt, ...);
    
    // How large the StdErr & Stdout buffer should be.  Rest will be discarded.
    PRUint32 m_nOutBufferSize;

    // Exit code of the process
    PRInt32  m_nExitCode;

    // Connecting to the processes std's -- not ours!
    PRFileDesc *m_StdIn[2];
    PRFileDesc *m_StdOut[2];
    PRFileDesc *m_StdErr[2];

    PRProcess *m_Process;
    PRBool     m_Blocking;

    // tmp vars kept here so we can be sure resources are released
    char ** m_argv;
    PRProcessAttr *m_attr;
	nsCAutoString m_charset;
	nsCOMPtr<nsIScriptableUnicodeConverter> m_conv;
	nsCOMPtr<nsIConverterInputStream> m_stdoutCIS;
	nsCOMPtr<nsIConverterInputStream> m_stderrCIS;
	nsCOMPtr<jxProcessInputStream> m_stdoutIS;
	nsCOMPtr<jxProcessInputStream> m_stderrIS;

};

////////////////////////////////////////////////////////////////////////////////

#endif // __APT_PROCESS_H__
