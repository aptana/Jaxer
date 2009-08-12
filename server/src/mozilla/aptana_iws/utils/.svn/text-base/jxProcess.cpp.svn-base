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


/*****************************************************************************
 * 
 * nsProcess is used to execute new processes and specify if you want to
 * wait (blocking) or continue (non-blocking).
 *
 *****************************************************************************
 */

#include "nsCOMPtr.h"
#include "nsMemory.h"
#include "prmem.h"
#include "nsProcess.h"
#include "prtypes.h"
#include "prio.h"
#include "prerror.h"
#include "prenv.h"
#include "nsCRT.h"
#include "nsIClassInfoImpl.h"

#include <stdlib.h>
#include "jxProcess.h"
#include <stdio.h>
#include <stdarg.h>

//-------------------------------------------------------------------//
// jxIProcess implementation
//-------------------------------------------------------------------//

#define SET_ERROR_RETURN(e, s, hr) \
    { \
        m_nExitCode = e; \
        sprintf(m_sError, "" s " failed: (%d, %d)\n", PR_GetError(), PR_GetOSError()); \
        return hr; \
    }

NS_IMPL_THREADSAFE_ISUPPORTS1(jxProcessInputStream,
                              //nsISeekableStream,
                              nsIInputStream)

//NS_IMPL_ISUPPORTS1_CI(jxProcessInputStream, nsIInputStream)

NS_IMETHODIMP
jxProcessInputStream::ReadSegments(nsWriteSegmentFun aWriter, void* aClosure,
                         PRUint32 aCount, PRUint32* aResult)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
jxProcessInputStream::IsNonBlocking(PRBool *aNonBlocking)
{
    *aNonBlocking = PR_FALSE;
    return NS_OK;
}

nsresult
jxProcessInputStream::Close()
{
	m_bClosed = PR_TRUE;
    return NS_OK;
}

NS_IMETHODIMP
jxProcessInputStream::Available(PRUint32* aResult)
{
	*aResult = m_bClosed ? 0 : PR_UINT32_MAX;
	return m_bClosed ? NS_BASE_STREAM_CLOSED : NS_OK;
}

NS_IMETHODIMP
jxProcessInputStream::Read(char* aBuf, PRUint32 aCount, PRUint32* aResult)
{ 
	if (m_bClosed)
	{
		*aBuf = 0;
		*aResult = 0;
		return NS_OK;
	}
	*aResult = PR_Read(m_fd, aBuf, aCount);
    if (*aResult == -1)
    {
        *aResult = 0;
		return NS_ERROR_FAILURE;
    }
	return NS_OK;
}

//Constructor
jxProcess::jxProcess()
    : m_nExitCode(-1),
      m_nOutBufferSize(1024),
      m_Process(nsnull),
      m_argv(0),
      m_attr(0),
      m_Blocking(PR_TRUE)
{
    m_sError[0] = 0;
    m_StdIn[0] = m_StdIn[1] = m_StdOut[0] = m_StdOut[1] = m_StdErr[0] = m_StdErr[1] = 0;
	m_charset.Assign(NS_LITERAL_CSTRING("UTF-8"));
}

jxProcess::~jxProcess()
{
    Clear();
}

NS_IMPL_ISUPPORTS1_CI(jxProcess, jxIProcess)

/* void setOutputBufferSize (in unsigned long bytes); */
NS_IMETHODIMP jxProcess::SetOutputBufferSize(PRUint32 bytes)
{
    if (bytes < 1)
        SET_ERROR_RETURN(PR_FAILURE, "setOutputBufferSize (Invalid value)", NS_ERROR_FAILURE);
    m_nOutBufferSize = bytes;
    return NS_OK;
}

PRStatus jxProcess::Clear()
{
    PRStatus status = PR_SUCCESS;
    // This is called before each Run() to clear any stuff

	m_stderrIS = nsnull;
	m_stdoutIS = nsnull;
	m_conv = nsnull;
	m_stdoutCIS = nsnull;
	m_stderrCIS = nsnull;

    for(int i=0; i<2; i++)
    {
        if (m_StdIn[i])
        {
            if (PR_FAILURE == PR_Close(m_StdIn[i]))
                status = PR_FAILURE;
            m_StdIn[i] = 0;
        }
        if (m_StdOut[i])
        {
            if (PR_FAILURE == PR_Close(m_StdOut[i]))
                status = PR_FAILURE;
            m_StdOut[i] = 0;
        }
        if (m_StdErr[i])
        {
            if (PR_Close(m_StdErr[i]) == PR_FAILURE)
                status = PR_FAILURE;
            m_StdErr[i] = 0;
        }
    }

    //
    if (m_argv)
    {
        nsMemory::Free(m_argv);
        m_argv = 0;
    }
    if (m_attr)
    {
        PR_DestroyProcessAttr(m_attr);
        m_attr = 0;
    }

    if (m_Process)
    {
        if (m_Blocking)
        {
            PR_KillProcess(m_Process);
            PR_DELETE(m_Process);
        }else
        {
            if (PR_FAILURE == PR_DetachProcess(m_Process))
                PR_DELETE(m_Process);
        }
        m_Process = nsnull;
    }

    return status;

}


nsresult jxProcess::InitAfterRun()
{
	nsresult rv = NS_OK;

	m_conv = do_CreateInstance(NS_ISCRIPTABLEUNICODECONVERTER_CONTRACTID, &rv);
	if (NS_FAILED(rv))
		SET_ERROR_RETURN(PR_FAILURE, "InitAfterRun (create conv0", rv);

	rv = m_conv->SetCharset(m_charset.get());
	if (NS_FAILED(rv))
		SET_ERROR_RETURN(PR_FAILURE, "InitAfterRun (SetCharset)", rv);	

	//Setup the 2 input streams

	m_stdoutIS = new jxProcessInputStream();
	if (!m_stdoutIS)
	{
		SET_ERROR_RETURN(-1, "InitAfterRun (out of memory)", NS_ERROR_OUT_OF_MEMORY);
	}
	m_stdoutIS->Init(m_StdOut[0]);
	

	m_stdoutCIS = do_CreateInstance("@mozilla.org/intl/converter-input-stream;1", &rv);
	if (NS_FAILED(rv))
	{
		SET_ERROR_RETURN(-1, "InitAfterRun (create converter-input-stream)", rv);
	}

	rv = m_stdoutCIS->Init(m_stdoutIS, m_charset.get(),
		4096,  nsIConverterInputStream::DEFAULT_REPLACEMENT_CHARACTER);
	if (NS_FAILED(rv))
		SET_ERROR_RETURN(-1, "InitAfterRun (init converter-input-stream)", rv);

	m_stderrIS = new jxProcessInputStream();
	if (!m_stderrIS)
	{
		SET_ERROR_RETURN(-2, "InitAfterRun (out of memory)", NS_ERROR_OUT_OF_MEMORY);
	}
	m_stderrIS->Init(m_StdErr[0]);

	m_stderrCIS = do_CreateInstance("@mozilla.org/intl/converter-input-stream;1", &rv);
	if (NS_FAILED(rv))
		SET_ERROR_RETURN(-1, "InitAfterRun (create converter-input-stream)", rv);

	rv = m_stderrCIS->Init(m_stderrIS, m_charset.get(),
		4096,  nsIConverterInputStream::DEFAULT_REPLACEMENT_CHARACTER);
	if (NS_FAILED(rv))
		SET_ERROR_RETURN(-2, "InitAfterRun (init converter-input-stream)", rv);

	return rv;
}

NS_IMETHODIMP  
jxProcess::Run(PRBool blocking, const char** argv, PRUint32 argc)
{
    PRStatus status = PR_SUCCESS;
    m_Blocking = blocking;

    status = Clear();
    if (PR_FAILURE == status)
        SET_ERROR_RETURN(status, "Clear", NS_ERROR_FAILURE);
    
    // make sure that when we allocate we have 1 greater than the
    // count since we need to null terminate the list for the argv to
    // pass into PR_CreateProcess
    m_argv = (char **)nsMemory::Alloc(sizeof(char *) * (argc + 1) );
    if (!m_argv)
		SET_ERROR_RETURN(-2, "Run (out of memory)", NS_ERROR_OUT_OF_MEMORY);

    // copy the args
    PRUint32 i;
    for (i=0; i < argc; i++)
    {
        m_argv[i] = const_cast<char*>(argv[i]);
    }
    
    // null terminate the array
    m_argv[argc] = NULL;

    // Create the pipes
    status = PR_CreatePipe(&m_StdIn[0], &m_StdIn[1]);
    if (status == PR_FAILURE)
        SET_ERROR_RETURN(status, "PR_CreatePipe", NS_ERROR_FAILURE);

    //status = PR_CreatePipe(&ChildOut, &m_OutPipe);
    status = PR_CreatePipe(&m_StdOut[0], &m_StdOut[1]);
    if (status == PR_FAILURE)
        SET_ERROR_RETURN(status, "PR_CreatePipe", NS_ERROR_FAILURE);

    status = PR_CreatePipe(&m_StdErr[0], &m_StdErr[1]);
    if (status == PR_FAILURE)
        SET_ERROR_RETURN(status, "PR_CreatePipe", NS_ERROR_FAILURE);


    status = PR_SetFDInheritable(m_StdIn[1], PR_FALSE);
    if (status == PR_FAILURE)
        SET_ERROR_RETURN(status, "PR_SetFDInheritable", NS_ERROR_FAILURE);

    status = PR_SetFDInheritable(m_StdIn[0], PR_TRUE);
    if (status == PR_FAILURE)
        SET_ERROR_RETURN(status, "PR_SetFDInheritable", NS_ERROR_FAILURE);


    status = PR_SetFDInheritable(m_StdOut[0], PR_FALSE);
    if (status == PR_FAILURE)
        SET_ERROR_RETURN(status, "PR_SetFDInheritable", NS_ERROR_FAILURE);

    status = PR_SetFDInheritable(m_StdOut[1], PR_TRUE);
    if (status == PR_FAILURE)
        SET_ERROR_RETURN(status, "PR_SetFDInheritable", NS_ERROR_FAILURE);

    status = PR_SetFDInheritable(m_StdErr[0], PR_FALSE);
    if (status == PR_FAILURE)
        SET_ERROR_RETURN(status, "PR_SetFDInheritable", NS_ERROR_FAILURE);

    status = PR_SetFDInheritable(m_StdErr[1], PR_TRUE);
    if (status == PR_FAILURE)
        SET_ERROR_RETURN(status, "PR_SetFDInheritable", NS_ERROR_FAILURE);


    m_attr = PR_NewProcessAttr();
    if (m_attr == NULL)
    {
		SET_ERROR_RETURN(-1, "PR_NewProcessAttr", NS_ERROR_OUT_OF_MEMORY);
    }

    PR_ProcessAttrSetStdioRedirect(m_attr, PR_StandardInput, m_StdIn[0]);
    PR_ProcessAttrSetStdioRedirect(m_attr, PR_StandardOutput, m_StdOut[1]);
    PR_ProcessAttrSetStdioRedirect(m_attr, PR_StandardError, m_StdErr[1]);

    m_Process = PR_CreateProcess(m_argv[0], m_argv, NULL, m_attr);
    if (m_Process == NULL)
    {
        SET_ERROR_RETURN(status, "PR_CreateProcess", NS_ERROR_FILE_EXECUTION_FAILED);
    }


    PR_DestroyProcessAttr(m_attr);
    m_attr = 0;

    if (PR_FAILURE == PR_Close(m_StdIn[0]) ||
        PR_FAILURE == PR_Close(m_StdOut[1]) ||
        PR_FAILURE == PR_Close(m_StdErr[1]))
       SET_ERROR_RETURN(status, "PR_Close", NS_ERROR_FAILURE);
    
    m_StdIn[0] = m_StdOut[1] = m_StdErr[1] = 0;

	return InitAfterRun();
}

NS_IMETHODIMP
jxProcess::Kill()
{
    nsresult rv = NS_OK;
    if (m_Process)
        rv = PR_KillProcess(m_Process);
    PR_DELETE(m_Process);
    m_Process = nsnull;

    if (rv== PR_FAILURE)
        SET_ERROR_RETURN(PR_FAILURE, "PR_KillProcess", NS_ERROR_FAILURE);
    
    Clear();
    return rv;
}

NS_IMETHODIMP
jxProcess::Wait()
{
    nsresult rv = NS_OK;
    if (m_Process)
    {
        if (m_Blocking && PR_FAILURE == PR_WaitProcess(m_Process, &m_nExitCode))
        {
            SET_ERROR_RETURN(PR_FAILURE, "PR_WaitProcess", NS_ERROR_FAILURE);
        }else if (!m_Blocking && PR_FAILURE == PR_DetachProcess(m_Process))
        {
            SET_ERROR_RETURN(PR_FAILURE, "PR_DetachProcess", NS_ERROR_FAILURE);
        }
    }
    m_Process = nsnull;

    Clear();

    return rv;
}

NS_IMETHODIMP
jxProcess::EndStdin()
{
    if (m_StdIn[1] && PR_FAILURE == PR_Close(m_StdIn[1]))
        SET_ERROR_RETURN(PR_FAILURE, "PR_WaitProcess", NS_ERROR_FAILURE);
            
    m_StdIn[1] = 0;

    return NS_OK;
}

NS_IMETHODIMP
jxProcess::GetExitValue(PRInt32 *aExitValue)
{
    *aExitValue = m_nExitCode;
    
    return NS_OK;
}


NS_IMETHODIMP
jxProcess::GetError(nsAString& aOutput)
{
	CopyASCIItoUTF16(m_sError, aOutput);
	return NS_OK;
}

nsresult jxProcess::ReadBytes(PRFileDesc* fd, PRUint8 **buf, PRUint32 *nbytes)
{
    if (!buf || !nbytes)
        SET_ERROR_RETURN(PR_FAILURE, "PR_Read (Invalid point)", NS_ERROR_FAILURE);

    if (!m_Process)
        SET_ERROR_RETURN(PR_FAILURE, "PR_Read (No Valid process)", NS_ERROR_FAILURE);

    if (!fd)
        SET_ERROR_RETURN(PR_FAILURE, "PR_Read (Pipe closed)", NS_ERROR_FAILURE);

    PRUint32 nlen = m_nOutBufferSize;
    
    *buf = (PRUint8 *)nsMemory::Alloc(nlen +1);
    if (! *buf)
    {
		SET_ERROR_RETURN(PR_FAILURE, "ReadBytes", NS_ERROR_OUT_OF_MEMORY);
    }

    PRUint32 nread = PR_Read(fd, *buf, nlen);
    if (nread == -1)
    {
        nsMemory::Free(*buf);
        SET_ERROR_RETURN(PR_FAILURE, "PR_Read", NS_ERROR_FAILURE);
    }

    *nbytes = nread;
    return NS_OK;
}

/* AString readStdoutString (); */
NS_IMETHODIMP jxProcess::ReadStdoutString(nsAString & _retval)
{
	nsresult rv;
	if (! m_stdoutCIS)
		SET_ERROR_RETURN(PR_FAILURE, "ReadStdoutString (null input stream)", NS_ERROR_NULL_POINTER);

	//Check if closed
	if (m_stdoutIS->m_bClosed)
	{
		SET_ERROR_RETURN(PR_FAILURE, "ReadStdoutString (stream closed)", NS_BASE_STREAM_CLOSED);
	}

	PRUint32 nRead = 0;
	rv = m_stdoutCIS->ReadString(m_nOutBufferSize, _retval, &nRead);
	if (NS_FAILED(rv))
		SET_ERROR_RETURN(PR_FAILURE, "ReadStdoutString (ReadString from stream)", rv);
	return rv;
}

/* void readStdoutBytes ([array, size_is (bytes)] out octet data, out unsigned long bytes); */
NS_IMETHODIMP jxProcess::ReadStdoutBytes(PRUint8 **data, PRUint32 *bytes)
{
    return ReadBytes(m_StdOut[0], data, bytes);
}

/* AString readStderrString (); */
NS_IMETHODIMP jxProcess::ReadStderrString(nsAString & _retval)
{
    nsresult rv;
	if (! m_stderrCIS)
		SET_ERROR_RETURN(PR_FAILURE, "ReadStderrString (null input stream)", NS_ERROR_NULL_POINTER);

	//Check if closed
	if (m_stderrIS->m_bClosed)
	{
		SET_ERROR_RETURN(PR_FAILURE, "ReadStderrString (stream closed)", NS_BASE_STREAM_CLOSED);
	}

	PRUint32 nRead = 0;
	rv = m_stderrCIS->ReadString(m_nOutBufferSize, _retval, &nRead);
	if (NS_FAILED(rv))
		SET_ERROR_RETURN(PR_FAILURE, "ReadStderrString (ReadString from stream)", rv);
	return rv;
}

/* void writeString (in AString str); */
NS_IMETHODIMP jxProcess::WriteString(const nsAString & str)
{
	// Convert -- todo, keep one copy in class
	nsresult rv = NS_OK;
	if (!m_conv && ! m_charset.Equals("UFT-16"))
		SET_ERROR_RETURN(PR_FAILURE, "WriteString (null convertor)", NS_ERROR_NULL_POINTER);

	if (m_charset.Equals("UFT-16") || m_charset.Equals("utf-16")
		|| m_charset.Equals("UTF16") || m_charset.Equals("utf16"))
	{
		return WriteBytes((PRUint8*)(PromiseFlatString(str).get()), str.Length() * 2);
	}else
	{
		PRUint32 aLen = 0;
		PRUint8* aData = 0;
		rv = m_conv->ConvertToByteArray(str, &aLen, &aData);
		if (NS_FAILED(rv))
			SET_ERROR_RETURN(PR_FAILURE, "WriteString (ConvertToByteArray)", rv);

		return WriteBytes(aData, aLen);
	}
	return NS_OK;
}

/* void writeBytes ([array, size_is (bytes), const] in octet data, in unsigned long bytes); */
NS_IMETHODIMP jxProcess::WriteBytes(const PRUint8 *data, PRUint32 bytes)
{
    if (!m_Process)
        SET_ERROR_RETURN(PR_FAILURE, "WriteBytes (No Valid process)", NS_ERROR_FAILURE);

    if (!m_StdIn[1])
        SET_ERROR_RETURN(PR_FAILURE, "WriteBytes (Pipe closed)", NS_ERROR_FAILURE);

    const char* p = (const char*) data;
    PRInt32 nBytes;
    PRUint32 nTotal = bytes;
    PRUint32 nWritten = 0;
    while (nWritten < nTotal)
    {
        nBytes = PR_Write(m_StdIn[1], p + nWritten, nTotal-nWritten);
        if (nBytes == -1)
            SET_ERROR_RETURN(PR_FAILURE, "PR_Write", NS_ERROR_FAILURE);
        if (nBytes == 0)
            break;
        nWritten += nBytes;
    }

    if (nWritten < nWritten)
        SET_ERROR_RETURN(PR_FAILURE, "WriteBytes (Incomplete write)", NS_ERROR_FAILURE);
        
    return NS_OK;
}

/* attribute string charset; */
NS_IMETHODIMP jxProcess::GetCharset(char * *aCharset)
{
    *aCharset = ToNewCString(m_charset);
	if (!*aCharset)
		SET_ERROR_RETURN(PR_FAILURE, "GetCharset", NS_ERROR_OUT_OF_MEMORY);
	return NS_OK;
}
NS_IMETHODIMP jxProcess::SetCharset(const char * aCharset)
{
	nsresult rv;
	if (m_conv)
	{
		rv = m_conv->SetCharset(aCharset);
		if (NS_FAILED(rv))
			SET_ERROR_RETURN(PR_FAILURE, "SetCharset (SetCharset)", rv);
	}
	m_charset.Assign(aCharset);
	return NS_OK;
}

void jxProcess::SetError(PRInt32 nExit, char* fmt, ...)
{
	m_nExitCode = nExit;
	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(m_sError, fmt, arg_ptr);
	va_end(arg_ptr);
}
