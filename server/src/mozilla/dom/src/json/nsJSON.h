/* ***** BEGIN ORIGINAL ATTRIBUTION BLOCK *****
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Mozilla Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2007
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Robert Sayre <sayrer@gmail.com>
 *
 * ***** END ORIGINAL ATTRIBUTION BLOCK ***** */

#ifndef nsJSON_h__
#define nsJSON_h__

#include "jsprvtd.h"
#include "nsIJSON.h"
#include "nsString.h"
#include "nsCOMPtr.h"
#include "nsIOutputStream.h"
#include "nsIUnicodeEncoder.h"
#include "nsIUnicodeDecoder.h"
#include "nsIRequestObserver.h"
#include "nsIStreamListener.h"
#include "nsTArray.h"

class nsIURI;

#define JSON_MAX_DEPTH  2048
#define JSON_PARSER_BUFSIZE 1024

#ifdef JAXER
#define NS_ERROR_JSON_MAX_RECURSION_EXCEEDED		NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_DOM,2000)
#endif /* JAXER */

class nsJSONWriter
{
public:
  nsJSONWriter();
  nsJSONWriter(nsIOutputStream *aStream);
  virtual ~nsJSONWriter();
  nsresult SetCharset(const char *aCharset);
  nsCOMPtr<nsIOutputStream> mStream;
  nsresult WriteString(const PRUnichar* aBuffer, PRUint32);
  nsresult Write(const PRUnichar *aBuffer, PRUint32 aLength);
  nsString mOutputString;
  PRBool DidWrite();
  void FlushBuffer();

protected:
  PRUnichar *mBuffer;
  PRUint32 mBufferCount;
  PRBool mDidWrite;
  nsresult WriteToStream(nsIOutputStream *aStream, nsIUnicodeEncoder *encoder,
                         const PRUnichar *aBuffer, PRUint32 aLength);

  nsCOMPtr<nsIUnicodeEncoder> mEncoder;
};

class nsJSON : public nsIJSON
{
public:
  nsJSON();
  virtual ~nsJSON();

  NS_DECL_ISUPPORTS
  NS_DECL_NSIJSON

protected:
  JSBool   ToJSON(JSContext *cx, jsval *vp);
  nsresult EncodeObject(JSContext *cx, jsval *vp, nsJSONWriter *writer,
                        JSObject *whitelist, PRUint32 depth);
#ifdef JAXER
  nsresult EncodePrimitive(JSContext *cx, jsval val, nsJSONWriter *writer);
#endif /* JAXER */
  nsresult EncodeInternal(nsJSONWriter *writer);
  nsresult DecodeInternal(nsIInputStream *aStream,
                          PRInt32 aContentLength,
                          PRBool aNeedsConverter);
  nsCOMPtr<nsIURI> mURI;
};

NS_IMETHODIMP
NS_NewJSON(nsISupports* aOuter, REFNSIID aIID, void** aResult);

enum JSONParserState {
    JSON_PARSE_STATE_INIT,
    JSON_PARSE_STATE_OBJECT_VALUE,
    JSON_PARSE_STATE_VALUE,
    JSON_PARSE_STATE_OBJECT,
    JSON_PARSE_STATE_OBJECT_PAIR,
    JSON_PARSE_STATE_OBJECT_IN_PAIR,
    JSON_PARSE_STATE_ARRAY,
    JSON_PARSE_STATE_STRING,
    JSON_PARSE_STATE_STRING_ESCAPE,
    JSON_PARSE_STATE_STRING_HEX,
    JSON_PARSE_STATE_NUMBER,
    JSON_PARSE_STATE_KEYWORD,
    JSON_PARSE_STATE_FINISHED
};

enum JSONDataType {
  JSON_DATA_STRING,
  JSON_DATA_KEYSTRING,
  JSON_DATA_NUMBER,
  JSON_DATA_KEYWORD
};

class nsJSONObjectStack : public nsTArray<JSObject *>,
                          public JSTempValueRooter
{
};

class nsJSONListener : public nsIStreamListener
{
public:
  nsJSONListener(JSContext *cx, jsval *rootVal, PRBool needsConverter);
  virtual ~nsJSONListener();

  NS_DECL_ISUPPORTS
  NS_DECL_NSIREQUESTOBSERVER
  NS_DECL_NSISTREAMLISTENER

protected:

  /* Used while handling \uNNNN in strings */
  PRUnichar mHexChar;
  PRUint8 mNumHex;

  JSContext *mCx;
  jsval *mRootVal;
  PRBool mNeedsConverter;
  nsCOMPtr<nsIUnicodeDecoder> mDecoder;
  JSONParserState *mStatep;
  JSONParserState mStateStack[JSON_MAX_DEPTH];
  nsString mStringBuffer;
  nsCString mSniffBuffer;

  nsresult PushState(JSONParserState state);
  nsresult PopState();
  nsresult ProcessBytes(const char* aBuffer, PRUint32 aByteLength);
  nsresult ConsumeConverted(const char* aBuffer, PRUint32 aByteLength);
  nsresult Consume(const PRUnichar *data, PRUint32 len);

  // helper to determine whether a character could be part of a number
  PRBool IsNumChar(PRUnichar c) 
  {
    if ((c <= '9' && c >= '0') ||
        c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E')
      return PR_TRUE;

    return PR_FALSE;
  }

  // These handle parsed tokens. Could be split to separate interface.
  nsJSONObjectStack mObjectStack;

  nsresult PushValue(JSObject *aParent, jsval aValue);
  nsresult PushObject(JSObject *aObj);
  nsresult OpenObject();
  nsresult CloseObject();
  nsresult OpenArray();
  nsresult CloseArray();

  nsresult HandleData(JSONDataType aType, const PRUnichar *aBuf,
                      PRUint32 aLength);
  nsresult HandleString(const PRUnichar *aBuf, PRUint32 aLength);
  nsresult HandleNumber(const PRUnichar *aBuf, PRUint32 aLength);
  nsresult HandleKeyword(const PRUnichar *aBuf, PRUint32 aLength);
  nsString mObjectKey;
};

#endif
