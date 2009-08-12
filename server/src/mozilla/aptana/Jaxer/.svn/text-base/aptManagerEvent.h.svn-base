#ifndef __APT_EXIT_EVENT_H__
#define __APT_EXIT_EVENT_H__

#include "nsThreadUtils.h"
#include "JaxerDefs.h"

#ifdef _WIN32
#include <Winsock2.h>
#endif

class aptManagerEvent : public nsIRunnable 
{
public:
    NS_DECL_ISUPPORTS

    NS_IMETHOD Run();

    aptManagerEvent(eMSG_TYPE eType);
    ~aptManagerEvent();

    void SetWebSocket(SOCKET s) { mWebSocket=s;}
    void SetMgrSocket(HANDLE s) { mMgrSocket=s;}
    nsresult SetEventData(PRUint8* data, PRUint16 len);

protected:

    nsresult OnExit();
    nsresult OnNewRequest();
	nsresult OnNewHTTPRequest();
    nsresult onSetPrefs();
    eMSG_TYPE    mType;
    SOCKET       mWebSocket;
    HANDLE       mMgrSocket;

    PRUint16     mLen;
    PRUint8      *mData;
};


#endif // __APT_EXIT_EVENT_H__
