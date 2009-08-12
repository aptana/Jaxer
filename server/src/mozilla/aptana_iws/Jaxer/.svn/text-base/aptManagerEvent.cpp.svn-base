#include "aptManagerEvent.h"
#include "nspr.h"
#include "nsIPrefService.h"
//#include "nsCOMPtr.h"
#include "nsIServiceManager.h"
//#include "nsXPCOM.h"
#include "aptCoreTrace.h"
#include "aptDocumentFetcherService.h"
#include "aptManagerCmdService.h"

extern aptCoreTrace gJaxerLog;
extern eJAXER_STATE gJaxerState;

NS_IMPL_THREADSAFE_ISUPPORTS1(aptManagerEvent, nsIRunnable)

aptManagerEvent::aptManagerEvent(eMSG_TYPE eType)
: mType(eType)
 ,mData(nsnull)
 ,mLen(0)
{
};

aptManagerEvent::~aptManagerEvent()
{
    gJaxerLog.Log(eTRACE, "in aptManagerEvent::~aptManagerEvent mType=%d", mType);
    if (mData)
        PR_Free(mData);
}

NS_IMETHODIMP aptManagerEvent::Run()
{
    nsresult rv;
    gJaxerLog.Log(eTRACE, "in aptManagerEvent::Run mType=%d", mType);
    switch (mType)
    {
    case eNEW_REQUEST_MSG:
        rv = OnNewRequest();
		// Do not know what state we are in, just exit.
		if (NS_FAILED(rv))
			rv = OnExit();
        break;

    case eNEW_HTTP_REQUEST_MSG:
	case eNEW_ADMIN_REQUEST_MSG:
        rv = OnNewHTTPRequest();
		// Do not know what state we are in, just exit.
		if (NS_FAILED(rv))
			rv = OnExit();
        break;

    case eSET_PREF_MSG:
        rv = onSetPrefs();
        break;

    case eEXIT_MSG:
        rv = OnExit();
        break;

    default:
        gJaxerLog.Log(eWARN, "aptManagerEvent::Run Invalid mType=%d", mType);
        //error
        break;
    }

    return rv;
}

nsresult aptManagerEvent::OnExit()
{
    gJaxerState = eEXITING;
    return NS_OK;
}

nsresult aptManagerEvent::OnNewRequest()
{
    nsresult rv;

    //make sure mWebSocket is valid
    if (mWebSocket == INVALID_SOCKET)
    {
        return NS_ERROR_FAILURE;
    }

    nsCOMPtr<aptIDocumentFetcherService> dfs = do_GetService("@aptana.com/httpdocumentfetcher;1", &rv);
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "OnNewRequest get aptIDocumentFetcherService failed: rv=0x%x", rv);
        return rv;
    }

    nsCOMPtr<aptIManagerCmdService> mcs = do_GetService("@aptana.com/managercmdservice;1", &rv);
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "OnNewRequest get aptIManagerCmdService failed: rv=0x%x", rv);
        return rv;
    }

    rv = dfs->ProcessRequest(mWebSocket);
    if (NS_FAILED(rv))
	{
		gJaxerLog.Log(eERROR, "OnNewRequest ProcessRequest failed: rv=0x%x", rv);
		return rv;
	}

    //Tell mgr we are done with this request
    rv = mcs->ExecEndReqCmd();
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "OnNewRequest execEndReqCmd failed: rv=0x%x", rv);
        return rv;
    }
    
    return rv;
}


nsresult aptManagerEvent::OnNewHTTPRequest()
{
    nsresult rv;

	// This request follows the http protocol.
	// eg GET /foo.html?id=123 HTTP/1.1
	//    ...
	//
    //make sure mWebSocket is valid
    if (mWebSocket == INVALID_SOCKET)
    {
        return NS_ERROR_FAILURE;
    }

    nsCOMPtr<aptIDocumentFetcherService> dfs = do_GetService("@aptana.com/httpdocumentfetcher;1", &rv);
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "OnNewRequest get aptIDocumentFetcherService failed: rv=0x%x", rv);
        return rv;
    }

    nsCOMPtr<aptIManagerCmdService> mcs = do_GetService("@aptana.com/managercmdservice;1", &rv);
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "OnNewRequest get aptIManagerCmdService failed: rv=0x%x", rv);
        return rv;
    }

    rv = dfs->ProcessHTTPRequest(mWebSocket, mType);
    if (NS_FAILED(rv))
	{
		gJaxerLog.Log(eERROR, "OnNewRequest ProcessHTTPRequest failed: rv=0x%x", rv);
		return rv;
	}

    //Tell mgr we are done with this request
    rv = mcs->ExecEndReqCmd();
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "OnNewRequest execEndReqCmd failed: rv=0x%x", rv);
        return rv;
    }
    
    return rv;
}

nsresult aptManagerEvent::onSetPrefs()
{
    gJaxerLog.Log(eTRACE, "Entered SetPref");
    PRUint8 *p = mData;
    PRUint16 totlen = p[0] << 8 | p[1];
    p += 2;
    PRUint8 namelen = *p++;
    const char *name = (const char *)p;
    p += namelen;
    PRUint8 valuelen = *p++;
    const char *value = (const char *)p;

    //check to see if totlen matches
    if (totlen != namelen + valuelen +2)
    {
        gJaxerLog.Log(eWARN, "Invalid pref msg: msg len=%d: name len=%d value len=%d.", totlen, namelen, valuelen);
        return NS_ERROR_FAILURE;
    }

    if (gJaxerLog.GetCoreLogLevel() == eTRACE)
    {
        //TRACE
        //name & value are already null-terminated
        gJaxerLog.Log(eTRACE, "Got Pref name=%s value=%s", name, value);
    }

    nsCOMPtr<nsIPrefBranch> prefBranch(do_GetService(NS_PREFSERVICE_CONTRACTID));
    if (!prefBranch)
    {
        gJaxerLog.Log(eWARN, "Failed to get PrefBranch. pref %s=%s cannot be set.", name, value);
        return NS_ERROR_FAILURE;
    }

    PRInt32 nType;
    nsresult rv = prefBranch->GetPrefType(name, &nType);
    if (NS_FAILED(rv))
    {
        // Does this mean the entry is not there?
        gJaxerLog.Log(eWARN, "Failed to get PrefType for %s. value %s cannot be set.", name, value);
        return rv;
    }
    switch (nType)
    {
    case nsIPrefBranch::PREF_STRING:
        rv = prefBranch->SetCharPref(name, value);
        break;
    case nsIPrefBranch::PREF_INT:
        rv = prefBranch->SetIntPref(name, atoi(value));
        break;
    case nsIPrefBranch::PREF_BOOL:
        rv = prefBranch->SetBoolPref(name, (strcmp("true", value)) ? PR_FALSE : PR_TRUE);
        break;
    default:
        if (strcmp("true", value) == 0 || strcmp("flase", value) == 0)
            rv = prefBranch->SetBoolPref(name, (strcmp("true", value)) ? PR_FALSE : PR_TRUE);
        else
        {
            const char*p = value;
            while (isspace(*p)) p++;
            while (*p && isdigit(*p)) p++;
            while (*p && isspace(*p)) p++;
            if (*p)
                rv = prefBranch->SetCharPref(name, value);
            else
                rv = prefBranch->SetIntPref(name, atoi(value));
        }
    }

    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eWARN, "Failed to set pref %s=%s: rv=0x%x.", name, value, rv);
    }
    return rv;
}

nsresult aptManagerEvent::SetEventData(PRUint8* data, PRUint16 len)
{
    if (mData)
        PR_Free(mData);
    mData = nsnull;

    mData = (PRUint8 *)PR_Malloc(len);
    if (!mData)
    {
        //error
        //return
        gJaxerLog.Log(eERROR, "SetEventData: Failed to allocate memory.");
        return NS_ERROR_OUT_OF_MEMORY;
    }
 
    memcpy(mData, data, len);

    return NS_OK;
}
