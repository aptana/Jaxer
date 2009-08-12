#include "nsNativeTypes.h"
#include "nsNativeMethod.h"
#include "nsServiceManagerUtils.h"
#include "prlink.h"
#include "jsapi.h"
#include "jsnum.h"


/**
 * JS Utilities
 */
PRBool JSValToUint16(PRUint16* aProp, JSContext* aContext, jsval aValue)
{
    uint16 temp;
    if (::JS_ValueToUint16(aContext, aValue, &temp)) {
        *aProp = (PRUint16)temp;
    }
    else {
        ::JS_ReportError(aContext, "Parameter must be an integer");
        return JS_FALSE;
    }

    return JS_TRUE;
}

PRBool JSValToString(nsAString& aProp, JSContext* aContext, jsval aValue)
{
   if (JSVAL_IS_STRING(aValue)) {
        JSString *jsstr = ::JS_ValueToString(aContext, aValue);
        nsDependentString str((PRUnichar *)::JS_GetStringChars(jsstr), ::JS_GetStringLength(jsstr));
        aProp.Assign(str);
    }
    else {
        ::JS_ReportError(aContext, "Parameter must be a string");
        return JS_FALSE;
    }

    return JS_TRUE;
}


NS_IMPL_ISUPPORTS1(nsNativeTypes, nsINativeTypes)

nsNativeTypes::nsNativeTypes()
{
  /* member initializers and constructor code */
}

nsNativeTypes::~nsNativeTypes()
{
  /* destructor code */
}

/* readonly attribute AString name; */
NS_IMETHODIMP nsNativeTypes::GetName(nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean open (in AString name); */
NS_IMETHODIMP nsNativeTypes::Open(const nsAString & aName, PRBool *_retval)
{
    PRLibSpec libSpec;
#ifdef WIN32
    nsString flatName = PromiseFlatString(aName);
    libSpec.value.pathname_u = flatName.get();
    libSpec.type = PR_LibSpec_PathnameU;
#else
    nsCString flatName = PromiseFlatCString(NS_ConvertUTF16toUTF8(aName));
    libSpec.value.pathname = flatName.get();
    libSpec.type = PR_LibSpec_Pathname;
#endif
    mLibrary = PR_LoadLibraryWithFlags(libSpec, 0);
    *_retval = (mLibrary != 0 ? PR_TRUE : PR_FALSE);

    return (mLibrary != 0 ? NS_OK : NS_ERROR_FAILURE);
}

/* boolean close (); */
NS_IMETHODIMP nsNativeTypes::Close(PRBool *_retval)
{
    *_retval = PR_FALSE;

    nsresult rv = NS_ERROR_FAILURE;

    if (mLibrary) {
        PR_UnloadLibrary(mLibrary);
        *_retval = PR_TRUE;
        rv = NS_OK;
    }

    return rv;
}

/* nsINativeMethod declare (); */
NS_IMETHODIMP nsNativeTypes::Declare(nsINativeMethod **_retval)
{
    *_retval = 0;
    nsresult rv;

    nsCOMPtr<nsIXPConnect> xpc = do_GetService(nsIXPConnect::GetCID());

    nsAXPCNativeCallContext* ncc;
    rv = xpc->GetCurrentNativeCallContext(&ncc);
    NS_ENSURE_SUCCESS(rv, rv);

    if (!ncc)
        return NS_ERROR_FAILURE;

    JSContext *ctx = nsnull;
    rv = ncc->GetJSContext(&ctx);
    NS_ENSURE_SUCCESS(rv, rv);

    JSAutoRequest ar(ctx);

    PRUint32 argc;
    jsval *argv = nsnull;

    ncc->GetArgc(&argc);
    ncc->GetArgvPtr(&argv);

    // we always need at least a method name, a call type and a return type
    if (argc < 3)
        return NS_ERROR_INVALID_ARG;

    rv = NS_OK;

#define GET_UINT_ARG(dest,whicharg) \
    do { if (!JSValToUint16(dest, ctx, whicharg)) { rv = NS_ERROR_INVALID_ARG; goto FAIL; } } while (0)

#define GET_STRING_ARG(dest,whicharg) \
    do { if (!JSValToString(dest, ctx, whicharg)) { rv = NS_ERROR_INVALID_ARG; goto FAIL; } } while (0)

    nsString name;
    PRUint16 callType;
    PRUint16 resultType;
    nsArgumentTypes argTypes;

    for (PRUint32 i=0; i<argc; i++) {
        if (i == 0) {
            GET_STRING_ARG(name, argv[i]);
        }
        else if (i == 1) {
            GET_UINT_ARG(&callType, argv[i]);
        }
        else if (i == 2) {
            GET_UINT_ARG(&resultType, argv[i]);
        }
        else {
            PRUint16 argType;
            if (JSVAL_IS_OBJECT(argv[i])) {
                argType = nsINativeTypes::STRUCT;
                argTypes.AppendElement(argType);
            }
            else {
                GET_UINT_ARG(&argType, argv[i]);
                argTypes.AppendElement(argType);
            }
        }
    }

#undef GET_UINT_ARG
#undef GET_STRING_ARG

    if (mLibrary) {
        void* pFunc = (void*)PR_FindFunctionSymbol(mLibrary, NS_LossyConvertUTF16toASCII(name).get());
        if (pFunc) {
            nsNativeMethod* pCall = new nsNativeMethod;
            pCall->Init(name, pFunc, callType, resultType, argTypes);
            NS_ADDREF(*_retval = pCall);
        }
    }

FAIL:
    return rv;
}
