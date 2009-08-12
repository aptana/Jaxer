#ifndef NSNATIVEMETHOD_H
#define NSNATIVEMETHOD_H

#include "nsINativeTypes.h"
#include "nsIXPCScriptable.h"
#include "nsStringAPI.h"
#include "nsTArray.h"
#include "nsCOMArray.h"
#include "nsIVariant.h"
#include "jsapi.h"
#include "ffi.h"

#ifdef WIN32
typedef int (__stdcall *FUNCTION)(void);
#else
typedef void *(FUNCTION)(void);
#endif

PRBool JSObjExtractNameTypeValue(JSContext* aContext, JSObject* aParentObj, JSObject* aObj, nsAString& aName, PRUint16& aType, jsval* aValue);

typedef nsTArray<PRUint16> nsArgumentTypes;

struct nsNativeData {
    ffi_type mType;
    void* mData;
    bool mShouldFree;
    union {
        JSBool mBool;
        int8 mInt8;
        int16 mInt16;
        int32 mInt32;
        int64 mInt64;
        float mFloat;
        double mDouble;
        void* mPointer;
    } value;
};

typedef nsTArray<nsNativeData*> nsNativeDataArray;


class nsNativeMethod : public nsINativeMethod,
                       public nsIXPCScriptable
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSINATIVEMETHOD
  NS_DECL_NSIXPCSCRIPTABLE

  nsNativeMethod();

  void Init(const nsAString& aName, void* aFunc, PRUint16 aCallType, PRUint16 aResultType, nsArgumentTypes& aArgTypes);

private:
  ~nsNativeMethod();

  NS_IMETHOD Prepare(JSContext* aContext, PRUint16 aType, jsval aValue, nsNativeDataArray& nativeData);
  NS_IMETHOD Execute(JSContext* aContext, PRUint32 aArgc, jsval* aArgv, jsval* aValue);

protected:
  /* additional members */
  nsString mName;
  void* mFunc;
  nsArgumentTypes mArgTypes;
  PRUint16 mCallType;
  PRUint16 mResultType;
};

#endif
