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
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1999
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   John Bandhauer <jband@netscape.com> (original author)
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

/* Implements nsIStackFrame. */

#include "xpcprivate.h"
#ifdef JAXER
#include "jsfun.h"
#endif /* JAXER */

class XPCJSStackFrame : public nsIStackFrame
{
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSISTACKFRAME

    static nsresult CreateStack(JSContext* cx, JSStackFrame* fp,
                                XPCJSStackFrame** stack);

    static nsresult CreateStackFrameLocation(
                                        PRUint32 aLanguage,
                                        const char* aFilename,
                                        const char* aFunctionName,
                                        PRInt32 aLineNumber,
                                        nsIStackFrame* aCaller,
                                        XPCJSStackFrame** stack);

    XPCJSStackFrame();
    virtual ~XPCJSStackFrame();

    JSBool IsJSFrame() const
        {return mLanguage == nsIProgrammingLanguage::JAVASCRIPT;}

private:
    nsIStackFrame* mCaller;

    char* mFilename;
    char* mFunname;
    PRInt32 mLineno;
    PRUint32 mLanguage;
};

/**********************************************/

// static

nsresult
XPCJSStack::CreateStack(JSContext* cx, nsIStackFrame** stack)
{
    if(!cx || !cx->fp)
        return NS_ERROR_FAILURE;

    return XPCJSStackFrame::CreateStack(cx, cx->fp, (XPCJSStackFrame**) stack);
}

// static
nsresult
XPCJSStack::CreateStackFrameLocation(PRUint32 aLanguage,
                                     const char* aFilename,
                                     const char* aFunctionName,
                                     PRInt32 aLineNumber,
                                     nsIStackFrame* aCaller,
                                     nsIStackFrame** stack)
{
    return XPCJSStackFrame::CreateStackFrameLocation(
                                        aLanguage,
                                        aFilename,
                                        aFunctionName,
                                        aLineNumber,
                                        aCaller,
                                        (XPCJSStackFrame**) stack);
}


/**********************************************/

XPCJSStackFrame::XPCJSStackFrame()
    :   mCaller(nsnull),
        mFilename(nsnull),
        mFunname(nsnull),
        mLineno(0),
        mLanguage(nsIProgrammingLanguage::UNKNOWN)
{
}

XPCJSStackFrame::~XPCJSStackFrame()
{
    if(mFilename)
        nsMemory::Free(mFilename);
    if(mFunname)
        nsMemory::Free(mFunname);
    NS_IF_RELEASE(mCaller);
}

NS_IMPL_THREADSAFE_ISUPPORTS1(XPCJSStackFrame, nsIStackFrame)

#ifdef JAXER
static char*
ValueToSource(JSContext *cx, jsval v)
{
    JSString *jsstr = nsnull;
	char *str = nsnull;

    /* Avoid toSource bloat and fallibility for object types. */
    if (JSVAL_IS_PRIMITIVE(v)) {
        jsstr = js_ValueToSource(cx, v);
    } else if (VALUE_IS_FUNCTION(cx, v)) {
        /*
         * XXX Avoid function decompilation bloat for now.
         */
        jsstr = JS_GetFunctionId(JS_ValueToFunction(cx, v));
        if (!jsstr && !(jsstr = js_ValueToSource(cx, v))) {
            /*
             * Continue to soldier on if the function couldn't be
             * converted into a string.
             */
            JS_ClearPendingException(cx);
            str = "[unknown function]";
        }
    } else {
        /*
         * XXX Avoid toString on objects, it takes too long and uses too much
         * memory, for too many classes (see Mozilla bug 166743).
         */
        char buf[100];
        JS_snprintf(buf, sizeof buf, "[object %s]",
                    OBJ_GET_CLASS(cx, JSVAL_TO_OBJECT(v))->name);
        str = buf;
    }
	if (str) {
		return (char*)nsMemory::Clone(str, sizeof(char)*(strlen(str)+1));
	}
	if (jsstr) {
		return (char*)nsMemory::Clone(JS_GetStringBytes(jsstr), sizeof(char)*(JS_GetStringLength(jsstr)+1));
	}
    return nsnull;
}
#endif /* JAXER */

nsresult
XPCJSStackFrame::CreateStack(JSContext* cx, JSStackFrame* fp,
                             XPCJSStackFrame** stack)
{
    XPCJSStackFrame* self = new XPCJSStackFrame();
    JSBool failed = JS_FALSE;
    if(self)
    {
        NS_ADDREF(self);

        if(fp->down)
        {
            if(NS_FAILED(CreateStack(cx, fp->down,
                         (XPCJSStackFrame**) &self->mCaller)))
                failed = JS_TRUE;
        }

        if(!failed)
        {
            if (JS_IsNativeFrame(cx, fp))
                self->mLanguage = nsIProgrammingLanguage::CPLUSPLUS;
            else
                self->mLanguage = nsIProgrammingLanguage::JAVASCRIPT;
            if(self->IsJSFrame())
            {
                JSScript* script = JS_GetFrameScript(cx, fp);
                jsbytecode* pc = JS_GetFramePC(cx, fp);
                if(script && pc)
                {
                    const char* filename = JS_GetScriptFilename(cx, script);
                    if(filename)
                    {
                        self->mFilename = (char*)
                                nsMemory::Clone(filename,
                                        sizeof(char)*(strlen(filename)+1));
                    }

                    self->mLineno = (PRInt32) JS_PCToLineNumber(cx, script, pc);


                    JSFunction* fun = JS_GetFrameFunction(cx, fp);
                    if(fun)
                    {
                        const char* funname = JS_GetFunctionName(fun);
                        if(funname)
                        {
#ifdef JAXER
							char * args = nsnull;
							int len = 0;
							if (fp->argc > 0)
							{
								char **argv = (char**)nsMemory::Alloc(sizeof(char*)*fp->argc);
								uintN i;
								JSAutoRequest ar(cx);
								for (i = 0; i < fp->argc; ++i)
								{
									argv[i] = ValueToSource(cx, fp->argv[i]);
									if (argv[i])
										len += strlen(argv[i]);
									len += 1;
								}
								args = (char*)nsMemory::Alloc(sizeof(char)*(len+1));
								*args = '\0';
								for (i = 0; i < fp->argc; ++i)
								{
									if (i != 0)
										strcat(args, ",");
									if (argv[i])
										strcat(args, argv[i]);

								}
								for (i = 0; i < fp->argc; ++i)
								{
									if (argv[i])
										nsMemory::Free(argv[i]);
								}
								nsMemory::Free(argv);
							}
							static const char format[] = "%s(%s)";
							len += strlen(funname)+2+1;
							self->mFunname = (char*)nsMemory::Alloc(sizeof(char)*len);
							JS_snprintf(self->mFunname, sizeof(char)*len, format, funname, args ? args : "");
							if (args)
								nsMemory::Free(args);
#else
                        self->mFunname = (char*)
                                nsMemory::Clone(funname,
                                        sizeof(char)*(strlen(funname)+1));
#endif /* JAXER */
                        }
                    }
                }
                else
                {
                    self->mLanguage = nsIProgrammingLanguage::CPLUSPLUS;
                }
            }
        }
        if(failed)
            NS_RELEASE(self);
    }

    *stack = self;
    return self ? NS_OK : NS_ERROR_OUT_OF_MEMORY;
}

// static
nsresult
XPCJSStackFrame::CreateStackFrameLocation(PRUint32 aLanguage,
                                          const char* aFilename,
                                          const char* aFunctionName,
                                          PRInt32 aLineNumber,
                                          nsIStackFrame* aCaller,
                                          XPCJSStackFrame** stack)
{
    JSBool failed = JS_FALSE;
    XPCJSStackFrame* self = new XPCJSStackFrame();
    if(self)
        NS_ADDREF(self);
    else
        failed = JS_TRUE;

    if(!failed)
    {
        self->mLanguage = aLanguage;
        self->mLineno = aLineNumber;
    }

    if(!failed && aFilename)
    {
        self->mFilename = (char*)
                nsMemory::Clone(aFilename,
                        sizeof(char)*(strlen(aFilename)+1));
        if(!self->mFilename)
            failed = JS_TRUE;
    }

    if(!failed && aFunctionName)
    {
        self->mFunname = (char*)
                nsMemory::Clone(aFunctionName,
                        sizeof(char)*(strlen(aFunctionName)+1));
        if(!self->mFunname)
            failed = JS_TRUE;
    }

    if(!failed && aCaller)
    {
        NS_ADDREF(aCaller);
        self->mCaller = aCaller;
    }

    if(failed && self)
    {
        NS_RELEASE(self);   // sets self to nsnull
    }

    *stack = self;
    return self ? NS_OK : NS_ERROR_OUT_OF_MEMORY;
}

/* readonly attribute PRUint32 language; */
NS_IMETHODIMP XPCJSStackFrame::GetLanguage(PRUint32 *aLanguage)
{
    *aLanguage = mLanguage;
    return NS_OK;
}

/* readonly attribute string languageName; */
NS_IMETHODIMP XPCJSStackFrame::GetLanguageName(char * *aLanguageName)
{
    static const char js[] = "JavaScript";
    static const char cpp[] = "C++";
    char* temp;

    if(IsJSFrame())
        *aLanguageName = temp = (char*) nsMemory::Clone(js, sizeof(js));
    else
        *aLanguageName = temp = (char*) nsMemory::Clone(cpp, sizeof(cpp));

    return temp ? NS_OK : NS_ERROR_OUT_OF_MEMORY;
}

/* readonly attribute string filename; */
NS_IMETHODIMP XPCJSStackFrame::GetFilename(char * *aFilename)
{
    XPC_STRING_GETTER_BODY(aFilename, mFilename);
}

/* readonly attribute string name; */
NS_IMETHODIMP XPCJSStackFrame::GetName(char * *aFunction)
{
    XPC_STRING_GETTER_BODY(aFunction, mFunname);
}

/* readonly attribute PRInt32 lineNumber; */
NS_IMETHODIMP XPCJSStackFrame::GetLineNumber(PRInt32 *aLineNumber)
{
    if(!aLineNumber)
        return NS_ERROR_NULL_POINTER;

    *aLineNumber = mLineno;
    return NS_OK;
}

/* readonly attribute string sourceLine; */
NS_IMETHODIMP XPCJSStackFrame::GetSourceLine(char * *aSourceLine)
{
    if(!aSourceLine)
        return NS_ERROR_NULL_POINTER;
    *aSourceLine = nsnull;
    return NS_OK;
}

/* readonly attribute nsIStackFrame caller; */
NS_IMETHODIMP XPCJSStackFrame::GetCaller(nsIStackFrame * *aCaller)
{
    if(!aCaller)
        return NS_ERROR_NULL_POINTER;

    if(mCaller)
        NS_ADDREF(mCaller);
    *aCaller = mCaller;
    return NS_OK;
}

/* string toString (); */
NS_IMETHODIMP XPCJSStackFrame::ToString(char **_retval)
{
    if(!_retval)
        return NS_ERROR_NULL_POINTER;

    const char* frametype = IsJSFrame() ? "JS" : "native";
    const char* filename = mFilename ? mFilename : "<unknown filename>";
    const char* funname = mFunname ? mFunname : "<TOP_LEVEL>";
    static const char format[] = "%s frame :: %s :: %s :: line %d";
    int len = sizeof(char)*
                (strlen(frametype) + strlen(filename) + strlen(funname)) +
              sizeof(format) + 6 /* space for lineno */;

    char* buf = (char*) nsMemory::Alloc(len);
    if(!buf)
        return NS_ERROR_OUT_OF_MEMORY;

    JS_snprintf(buf, len, format, frametype, filename, funname, mLineno);
    *_retval = buf;
    return NS_OK;
}

