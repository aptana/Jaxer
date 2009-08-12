#ifndef __JAXER_DEFS_H__
#define __JAXER_DEFS_H__

#ifndef _WIN32

#define HANDLE int
#define SOCKET int
#define INVALID_SOCKET -1
#define INVALID_HANDLE_VALUE -1
#define DWORD int

#endif

#define CMDBUFSIZE 8192

enum eJAXER_STATE
{
     eRUNNING
    ,eEXITING
};

enum eMSG_TYPE
{
     eNEW_REQUEST_MSG   = 1 //MGR->JXR: Start a new request
    ,eEND_REQUEST_MSG   = 2 //JXR->MGR: Done with the request
    ,eEXIT_MSG          = 3 //MGR->JXR: Asked to exit
    ,eNO_RESP_CMD_MSG   = 4
    ,eNEED_RESP_CMD_MSG = 5
    ,eCMD_RESP_MSG      = 6
    ,eSET_PREF_MSG      = 7 //MGR->JXR
    ,eINVALID_MSG       = 8
	,eNEW_HTTP_REQUEST_MSG = 9
	,eNEW_ADMIN_REQUEST_MSG = 10
};


#endif // __JAXER_DEFS_H__
