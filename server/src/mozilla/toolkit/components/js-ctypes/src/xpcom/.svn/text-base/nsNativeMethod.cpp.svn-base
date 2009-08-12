#include "nsNativeMethod.h"
#include "nsComponentManagerUtils.h"
#include "nsServiceManagerUtils.h"
#include "nsIXPConnect.h"
#include "nsMemory.h"
#include "jsnum.h"


NS_IMPL_ISUPPORTS2(nsNativeMethod, nsINativeMethod, nsIXPCScriptable)

nsNativeMethod::nsNativeMethod()
{
  /* member initializers and constructor code */
}

nsNativeMethod::~nsNativeMethod()
{
  /* destructor code */
}

void nsNativeMethod::Init(const nsAString& aName, void* aFunc, PRUint16 aCallType, PRUint16 aResultType, nsArgumentTypes& aArgTypes)
{
    mName = aName;
    mFunc = aFunc;
    mCallType = aCallType;
    mResultType = aResultType;
    mArgTypes = aArgTypes;
}

NS_IMETHODIMP nsNativeMethod::Prepare(JSContext* aContext, PRUint16 aType, jsval aValue, nsNativeDataArray& nativeData)
{
    nsresult rv = NS_OK;

    nsNativeData* native = 0;
    if (aType != nsINativeTypes::STRUCT) {
      native = new nsNativeData;
      native->mShouldFree = false;
      nativeData.AppendElement(native);
    }

    int32 int32Buffer;
    double dblBuffer;

    switch (aType) {
        case nsINativeTypes::BOOL:
            if (!JS_ValueToBoolean(aContext, aValue, (JSBool*)&(native->value.mBool)))
                return NS_ERROR_FAILURE;

            native->mType = ffi_type_sint8;
            native->mData = (void*)&(native->value.mBool);
            break;
        case nsINativeTypes::INT8:
            if (!JS_ValueToECMAInt32(aContext, aValue, &int32Buffer))
                return NS_ERROR_FAILURE;

            native->value.mInt8 = (PRUint8)int32Buffer;
            native->mType = ffi_type_sint8;
            native->mData = (void*)&(native->value.mInt8);
            break;
        case nsINativeTypes::INT16:
            if (!JS_ValueToECMAInt32(aContext, aValue, &int32Buffer))
                return NS_ERROR_FAILURE;

            native->value.mInt16 = (PRInt16)int32Buffer;
            native->mType = ffi_type_sint16;
            native->mData = (void*)&(native->value.mInt16);
            break;
        case nsINativeTypes::INT32:
            if (!JS_ValueToECMAInt32(aContext, aValue, &(native->value.mInt32)))
                return NS_ERROR_FAILURE;

            native->mType = ffi_type_sint32;
            native->mData = (void*)&(native->value.mInt32);
            break;
        case nsINativeTypes::INT64:
            if(JSVAL_IS_INT(aValue))
            {
                if(!JS_ValueToECMAInt32(aContext, aValue, &int32Buffer))
                    return NS_ERROR_FAILURE;
                LL_I2L(native->value.mInt64, int32Buffer);
            }
            else
            {
                if(!JS_ValueToNumber(aContext, aValue, &dblBuffer))
                    return NS_ERROR_FAILURE;
                LL_D2L(native->value.mInt64, dblBuffer);
            }

            native->mType = ffi_type_sint64;
            native->mData = (void*)&(native->value.mInt64);
            break;
        case nsINativeTypes::FLOAT:
            if (!JS_ValueToNumber(aContext, aValue, &dblBuffer))
                return NS_ERROR_FAILURE;
            native->value.mFloat = (float)dblBuffer;

            native->mType = ffi_type_float;
            native->mData = (void*)&(native->value.mFloat);
            break;
        case nsINativeTypes::DOUBLE:
            if (!JS_ValueToNumber(aContext, aValue, &(native->value.mDouble)))
                return NS_ERROR_FAILURE;

            native->mType = ffi_type_double;
            native->mData = (void*)&(native->value.mDouble);
            break;
        case nsINativeTypes::INT8_P:
            if (!JS_ValueToECMAInt32(aContext, aValue, &int32Buffer))
                return NS_ERROR_FAILURE;

            native->value.mInt8 = (PRUint8)int32Buffer;
            native->mType = ffi_type_pointer;
            native->mData = (void*)&(native->value.mInt8);
            break;
        case nsINativeTypes::INT16_P:
            if (!JS_ValueToECMAInt32(aContext, aValue, &int32Buffer))
                return NS_ERROR_FAILURE;

            native->value.mInt16 = (PRInt16)int32Buffer;
            native->mType = ffi_type_pointer;
            native->mData = (void*)&(native->value.mInt16);
            break;
        case nsINativeTypes::INT32_P:
            if (!JS_ValueToECMAInt32(aContext, aValue, &(native->value.mInt32)))
                return NS_ERROR_FAILURE;

            native->mType = ffi_type_pointer;
            native->mData = (void*)&(native->value.mInt32);
            break;
        case nsINativeTypes::FLOAT_P:
            if (!JS_ValueToNumber(aContext, aValue, &dblBuffer))
                return NS_ERROR_FAILURE;
            native->value.mFloat = (float)dblBuffer;
            native->mType = ffi_type_pointer;
            native->mData = (void*)&(native->value.mFloat);
            break;
        case nsINativeTypes::DOUBLE_P:
            if (!JS_ValueToNumber(aContext, aValue, &(native->value.mDouble)))
                return NS_ERROR_FAILURE;
            native->mType = ffi_type_pointer;
            native->mData = (void*)&(native->value.mDouble);
            break;
        case nsINativeTypes::STRING: {
            char* bytes = nsnull;
            JSString* str;
            if (!(str = JS_ValueToString(aContext, aValue)) || !(bytes = JS_GetStringBytes(str)))
                return NS_ERROR_FAILURE;

            native->value.mPointer = (void*)bytes;

            native->mType = ffi_type_pointer;
            //native->mShouldFree = true;
            native->mData = (void*)&(native->value.mPointer);
            break;
        }
        case nsINativeTypes::WSTRING: {
            jschar* chars = nsnull;
            JSString* str;
            if (!(str = JS_ValueToString(aContext, aValue)) || !(chars = JS_GetStringChars(str)))
                return NS_ERROR_FAILURE;

            native->value.mPointer = (void*)chars;

            native->mType = ffi_type_pointer;
            //native->mShouldFree = true;
            native->mData = (void*)&(native->value.mPointer);
            break;
        }
        case nsINativeTypes::STRUCT: {
            JSObject* obj;
            JS_ValueToObject(aContext, aValue, &obj);
            if (obj) {
                jsval structFields;
                if (!JS_GetProperty(aContext, obj, "_fields_", &structFields)) {
                    ::JS_ReportError(aContext, "Couldn't find _fields_ property");
                    return NS_ERROR_FAILURE;
                }

                JSObject* _fields_;
                JS_ValueToObject(aContext, structFields, &_fields_);
                jsuint len;
                JS_GetArrayLength(aContext, _fields_, &len);

                nsAutoString fieldName;
                PRUint16 fieldType;
                jsval fieldValue;
                for (PRUint32 i=0; i<len; ++i) {
                    jsval item;
                    JS_GetElement(aContext, _fields_, i, &item);
                    JSObject* field;
                    JS_ValueToObject(aContext, item, &field);
                    if (JSObjExtractNameTypeValue(aContext, obj, field, fieldName, fieldType, &fieldValue) == PR_FALSE) {
                        ::JS_ReportError(aContext, "Failed to extract data from struct._fields_");
                        return NS_ERROR_FAILURE;
                    }

                    Prepare(aContext, fieldType, fieldValue, nativeData);
                }
            }
            break;
        }
    }
    return rv;
}

NS_IMETHODIMP nsNativeMethod::Execute(JSContext* aContext, PRUint32 aArgc, jsval* aArgv, jsval* aValue)
{
    nsresult rv = NS_OK;

    JSAutoRequest ar(aContext);

    nsNativeDataArray nativeData;
    PRUint32 argCount = mArgTypes.Length();
    for (PRUint32 i=0; i<argCount; i++) {
        Prepare(aContext, mArgTypes[i], aArgv[i], nativeData);
    }

    ffi_type* types[100];
    void* values[100];
    PRUint32 dataCount = nativeData.Length();
    for (PRUint32 i=0; i<dataCount; i++) {
        types[i] = &(nativeData[i]->mType);
        values[i] = nativeData[i]->mData;
    }

    ffi_abi callType = FFI_SYSV;
    switch (mCallType) {
#ifdef WIN32
        case nsINativeTypes::STDCALL:
            callType = FFI_STDCALL;
            break;
#endif
        case nsINativeTypes::SYSV:
            callType = FFI_SYSV;
            break;
    }

    nsNativeData result;

    switch (mResultType) {
        case nsINativeTypes::VOID:
            result.mType = ffi_type_void;
            break;
        case nsINativeTypes::BOOL:
        case nsINativeTypes::INT8:
            result.mType = ffi_type_sint8;
            break;
        case nsINativeTypes::INT16:
            result.mType = ffi_type_sint16;
            break;
        case nsINativeTypes::INT32:
            result.mType = ffi_type_sint32;
            break;
        case nsINativeTypes::INT64:
            result.mType = ffi_type_sint64;
            break;
        case nsINativeTypes::FLOAT:
            result.mType = ffi_type_float;
            break;
        case nsINativeTypes::DOUBLE:
            result.mType = ffi_type_double;
            break;
        case nsINativeTypes::INT8_P:
        case nsINativeTypes::INT16_P:
        case nsINativeTypes::INT32_P:
        case nsINativeTypes::FLOAT_P:
        case nsINativeTypes::DOUBLE_P:
            result.mType = ffi_type_pointer;
            break;
        case nsINativeTypes::STRING: {
            result.mType = ffi_type_pointer;
            break;
        }
        case nsINativeTypes::WSTRING: {
            result.mType = ffi_type_pointer;
            break;
        }
    }

    void* proc = mFunc;

    ffi_cif cif;
    ffi_prep_cif(&cif, callType, dataCount, &(result.mType), types);
    ffi_call(&cif, FFI_FN(proc), (void*)&(result.value), values);

    switch (mResultType) {
        case nsINativeTypes::VOID:
            *aValue = JSVAL_NULL;
            break;
        case nsINativeTypes::BOOL:
            *aValue = BOOLEAN_TO_JSVAL(result.value.mBool ? JS_TRUE : JS_FALSE);
            break;
        case nsINativeTypes::INT8:
        case nsINativeTypes::INT8_P:
            *aValue = INT_TO_JSVAL(result.value.mInt8);
            break;
        case nsINativeTypes::INT16:
        case nsINativeTypes::INT16_P:
            *aValue = INT_TO_JSVAL(result.value.mInt16);
            break;
        case nsINativeTypes::INT32:
        case nsINativeTypes::INT32_P:
            *aValue = INT_TO_JSVAL(result.value.mInt32);
            break;
        case nsINativeTypes::INT64:
            JS_NewDoubleValue(aContext, (double)(result.value.mInt64), aValue);
            break;
        case nsINativeTypes::FLOAT:
        case nsINativeTypes::FLOAT_P:
            JS_NewDoubleValue(aContext, (double)(result.value.mFloat), aValue);
            break;
        case nsINativeTypes::DOUBLE:
        case nsINativeTypes::DOUBLE_P:
            JS_NewDoubleValue(aContext, result.value.mDouble, aValue);
            break;
        case nsINativeTypes::STRING: {
            JSString *jsstring = ::JS_NewStringCopyZ(aContext, *(char**)&result.value.mPointer);
            *aValue = STRING_TO_JSVAL(jsstring);
            break;
        }
        case nsINativeTypes::WSTRING: {
            JSString *jsstring = ::JS_NewUCStringCopyZ(aContext, reinterpret_cast<const jschar*>(*(PRUnichar**)&result.value.mPointer));
            *aValue = STRING_TO_JSVAL(jsstring);
            break;
        }
    }

    // Cleanup any allocated memory
    for (PRUint32 i=0; i<dataCount; i++)
    {
        if (nativeData[i]->mShouldFree)
            nsMemory::Free(nativeData[i]->value.mPointer);
        delete nativeData[i];
    }

    return rv;
}

/*** nsIXPCScriptable interface ***/

/* readonly attribute string className; */
NS_IMETHODIMP nsNativeMethod::GetClassName(char * *aClassName)
{
    NS_ENSURE_ARG_POINTER(aClassName);
    *aClassName = (char *) nsMemory::Clone("nsNativeMethod", 12);
    if (!*aClassName)
        return NS_ERROR_OUT_OF_MEMORY;
    return NS_OK;
}

/* readonly attribute PRUint32 scriptableFlags; */
NS_IMETHODIMP nsNativeMethod::GetScriptableFlags(PRUint32 *aScriptableFlags)
{
    *aScriptableFlags =
        nsIXPCScriptable::WANT_CALL |
        nsIXPCScriptable::WANT_NEWRESOLVE |
        nsIXPCScriptable::ALLOW_PROP_MODS_DURING_RESOLVE;
    return NS_OK;
}

/* PRBool call (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in PRUint32 argc, in JSValPtr argv, in JSValPtr vp); */
NS_IMETHODIMP nsNativeMethod::Call(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                          JSObject * obj, PRUint32 argc, jsval * argv, jsval * vp, PRBool *_retval)
{
    if (!mFunc) {
        ::JS_ReportError(cx, "Function is null");
        *_retval = PR_TRUE;
        return NS_ERROR_FAILURE;
    }

    if (argc != mArgTypes.Length()) {
        ::JS_ReportError(cx, "Number of arguments does not match declaration");
        *_retval = PR_FALSE;
        return NS_ERROR_FAILURE;
    }

    nsresult rv = NS_OK;
    rv = Execute(cx, argc, argv, vp);

    *_retval = PR_TRUE;

FAIL:
    return rv;
}

/* PRBool getProperty (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in JSVal id, in JSValPtr vp); */
NS_IMETHODIMP nsNativeMethod::GetProperty(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                                 JSObject * obj, jsval id, jsval * vp, PRBool *_retval)
{
    *_retval = PR_FALSE;
    return NS_OK;
}


/* PRBool setProperty (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in JSVal id, in JSValPtr vp); */
NS_IMETHODIMP nsNativeMethod::SetProperty(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                         JSObject * obj, jsval id, jsval * vp, PRBool *_retval)
{
    *_retval = PR_FALSE;
    return NS_OK;
}

/* void preCreate (in nsISupports nativeObj, in JSContextPtr cx, in JSObjectPtr globalObj, out JSObjectPtr parentObj); */
NS_IMETHODIMP nsNativeMethod::PreCreate(nsISupports *nativeObj, JSContext * cx,
                       JSObject * globalObj, JSObject * *parentObj)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void create (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj); */
NS_IMETHODIMP nsNativeMethod::Create(nsIXPConnectWrappedNative *wrapper, JSContext * cx, JSObject * obj)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void postCreate (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj); */
NS_IMETHODIMP nsNativeMethod::PostCreate(nsIXPConnectWrappedNative *wrapper, JSContext * cx, JSObject * obj)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool addProperty (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in JSVal id, in JSValPtr vp); */
NS_IMETHODIMP nsNativeMethod::AddProperty(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                                 JSObject * obj, jsval id, jsval * vp, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool delProperty (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in JSVal id, in JSValPtr vp); */
NS_IMETHODIMP nsNativeMethod::DelProperty(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                                 JSObject * obj, jsval id, jsval * vp, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool enumerate (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj); */
NS_IMETHODIMP nsNativeMethod::Enumerate(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                               JSObject * obj, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool newEnumerate (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in PRUint32 enum_op, in JSValPtr statep, out JSID idp); */
NS_IMETHODIMP nsNativeMethod::NewEnumerate(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                                  JSObject * obj, PRUint32 enum_op, jsval * statep, jsid *idp, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool newResolve (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in JSVal id, in PRUint32 flags, out JSObjectPtr objp); */
NS_IMETHODIMP nsNativeMethod::NewResolve(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                                JSObject * obj, jsval id, PRUint32 flags, JSObject * *objp, PRBool *_retval)
{
    *_retval = PR_TRUE;
    return NS_OK;
}

/* PRBool convert (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in PRUint32 type, in JSValPtr vp); */
NS_IMETHODIMP
nsNativeMethod::Convert(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                                JSObject * obj, PRUint32 type, jsval * vp, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void finalize (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj); */
NS_IMETHODIMP nsNativeMethod::Finalize(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                              JSObject * obj)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool checkAccess (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in JSVal id, in PRUint32 mode, in JSValPtr vp); */
NS_IMETHODIMP nsNativeMethod::CheckAccess(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                                 JSObject * obj, jsval id, PRUint32 mode, jsval * vp, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool construct (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in PRUint32 argc, in JSValPtr argv, in JSValPtr vp); */
NS_IMETHODIMP nsNativeMethod::Construct(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                               JSObject * obj, PRUint32 argc, jsval * argv, jsval * vp, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool hasInstance (in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in JSVal val, out PRBool bp); */
NS_IMETHODIMP nsNativeMethod::HasInstance(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                                 JSObject * obj, jsval val, PRBool *bp, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void trace (in nsIXPConnectWrappedNative wrapper, in JSTracerPtr trc, in JSObjectPtr obj); */
NS_IMETHODIMP nsNativeMethod::Trace(nsIXPConnectWrappedNative *wrapper,
                                  JSTracer *trc, JSObject *obj)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool equality(in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj, in JSVal val); */
NS_IMETHODIMP nsNativeMethod::Equality(nsIXPConnectWrappedNative *wrapper,
                                    JSContext *cx, JSObject *obj, jsval val,
                                    PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* JSObjectPtr outerObject(in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj); */
NS_IMETHODIMP nsNativeMethod::OuterObject(nsIXPConnectWrappedNative *wrapper,
                                        JSContext *cx, JSObject *obj,
                                        JSObject **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* JSObjectPtr innerObject(in nsIXPConnectWrappedNative wrapper, in JSContextPtr cx, in JSObjectPtr obj); */
NS_IMETHODIMP nsNativeMethod::InnerObject(nsIXPConnectWrappedNative *wrapper,
                                        JSContext *cx, JSObject *obj,
                                        JSObject **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void postCreatePrototype(in JSContextPtr cx, in JSObjectPtr proto); */
NS_IMETHODIMP nsNativeMethod::PostCreatePrototype(JSContext *cx, JSObject *proto)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/*
  Extract the name, type and value of a struct member.

  Struct.prototype = {
    _fields_ = [{"x" : INT32}],
    x: 32
  }

  will yield name = "x", type = INT32, value = 32.
*/
PRBool JSObjExtractNameTypeValue(JSContext* aContext, JSObject* aParentObj, JSObject* aObj, nsAString& aName, PRUint16& aType, jsval* aValue)
{
    JSObject* iter = ::JS_NewPropertyIterator(aContext, aObj);

    jsid propId;
    if (!::JS_NextProperty(aContext, iter, &propId)) {
        ::JS_ReportError(aContext, "Unable to find property in struct._fields_");
        return PR_FALSE;
    }
    if (propId == JSVAL_VOID) {
        ::JS_ReportError(aContext, "Unable to find property in struct._fields_");
        return PR_FALSE;
    }

    jsval propVal;
    if (!::JS_IdToValue(aContext, propId, &propVal)) {
        ::JS_ReportError(aContext, "Unable to find property in struct._fields_");
        return PR_FALSE;
    }

    JSString* propName = ::JS_ValueToString(aContext, propVal);
    jschar* strChars = ::JS_GetStringChars(propName);
    jsuint strLen = ::JS_GetStringLength(propName);

    if (!::JS_GetUCProperty(aContext, aObj, strChars, strLen, &propVal))
        return PR_FALSE;

    /* Create a Moz string from the property name, then assign it to the
       output parameter. */
    nsDependentString pName(reinterpret_cast<PRUnichar*>(strChars), strLen);
    aName.Assign(pName);

    if (!::JS_ValueToUint16(aContext, propVal, &aType)) {
        ::JS_ReportError(aContext, "Unable to find value of property in struct._fields_");
        return PR_FALSE;
    }

    if (!::JS_GetUCProperty(aContext, aParentObj, strChars, strLen, aValue)) {
        ::JS_ReportError(aContext, "Unable to find property in struct");
        return PR_FALSE;
    }

    return PR_TRUE;
}
