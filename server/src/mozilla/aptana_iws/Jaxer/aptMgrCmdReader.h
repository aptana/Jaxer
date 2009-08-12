#ifndef __APT_MGR_CMD_READER_H__
#define __APT_MGR_CMD_READER_H__

#include "nsThreadUtils.h"
#include "JaxerDefs.h"

#ifdef _WIN32
#include <windows.h>
#include <Winsock2.h>
#endif

class aptMgrCmdReader : public nsIRunnable 
{
public:
    NS_DECL_ISUPPORTS

    NS_IMETHOD Run();
    aptMgrCmdReader(HANDLE num);
    ~aptMgrCmdReader();

protected:
    
    HANDLE       mMgrSocket; /* aptMgrCmdReader is not owner of this handle, so it shouldn't close it */
    SOCKET       mFD;

#ifdef _WIN32
    OVERLAPPED      mOverlapped;
#endif

    PRUint16     mRecvIn;
    PRUint8      mRecvBuf[CMDBUFSIZE];
};

#endif // __APT_MGR_CMD_READER_H__
