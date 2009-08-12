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
#include <stdlib.h>

#define XPCOM_TRANSLATE_NSGM_ENTRY_POINT 1

#include "nspr.h"
#include "nsCOMPtr.h"
#include "nsIURL.h"
#include "nsNetUtil.h"
#include "nsXPCOM.h"
#include "nsString.h"
#include "nsISupportsPrimitives.h"
#include "nsIServiceManager.h"
#include "nsComponentManagerUtils.h"
#include "nsNativeCharsetUtils.h"
#include "nsIScriptNameSpaceManager.h"
#include "nsIObserverService.h"
#include "nsIPrefService.h"
#include "nsIPrefBranch.h"
#include "nsIGenericFactory.h"
#include "nsIClassInfoImpl.h"
#include "nsICategoryManager.h"
#include "nsIXULAppInfo.h"
#include "nsXULAppAPI.h"
#include "nsIXULRuntime.h"
#include "nsIAppStartupNotifier.h"
#include "nsDirectoryServiceDefs.h"
#include "nsAppDirectoryServiceDefs.h"
#include "nsICertOverrideService.h"
#include "nsICommandLineRunner.h"
#include "nsThreadUtils.h"
#include "jaxerBuildId.h"
#include "aptConsoleListener.h"
#include "nsIConsoleService.h"
#include "aptCoreTrace.h"
#include "aptDocumentFetcherService.h"
#include "aptEventTypeManager.h"
#include "aptEventBaseData.h"
#include "aptEventDocInitializedData.h"
#include "aptEventHTMLParseCompleteData.h"
#include "aptEventHTMLParseStartData.h"
#include "aptHTTPRequest.h"
#include "aptHTTPResponse.h"
#include "aptEventLogData.h"
#include "aptEventNewHTMLElementData.h"
#include "aptEventRequestCompleteData.h"
#include "aptEventRequestStartData.h"
#include "aptEventBeforeScriptEvaluatedData.h"
#include "aptEventAfterScriptEvaluatedData.h"
#include "aptEventScriptCompileData.h"
#include "aptJaxerGlobal.h"
#include "aptCoreLog.h"
#include "HTTPStream.h"
#include "FCGXStream.h"
#include "IFCGXStream.h"
#include "AppDirectoryProvider.h"
#include "PrefObserver.h"
#include "nsXPCOMGlue.h"
#include "plstr.h"
//#include "aptProcessRequestEvent.h"
#include "aptMgrCmdReader.h"
#include "aptManagerCmdService.h"
#include "aptManagerCmdResponse.h"

#ifdef _BUILD_STATIC_BIN
#include "nsStaticComponents.h"
#endif

#ifdef MOZ_CRASHREPORTER
#include "nsExceptionHandler.h"
#include "nsICrashReporter.h"
#define NS_CRASHREPORTER_CONTRACTID "@mozilla.org/toolkit/crash-reporter;1"
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <errno.h>
#include <unistd.h>
#endif

#include "JaxerDefs.h"

#if defined(MOZ_WIDGET_GTK2)
#include <gtk/gtk.h>
#endif


#define APPINFO_CID \
{ 0x95d89e3e, 0xa169, 0x41a3, { 0x8e, 0x56, 0x71, 0x99, 0x78, 0xe1, 0x5b, 0x12 } }

//NS_GENERIC_FACTORY_CONSTRUCTOR(aptConsoleListener)
//NS_DECL_CLASSINFO(aptConsoleListener)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptCoreLog)
NS_DECL_CLASSINFO(aptCoreLog)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventTypeManager)
NS_DECL_CLASSINFO(aptEventTypeManager)

//NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(FCGXStream, Init)
NS_GENERIC_FACTORY_CONSTRUCTOR(FCGXStream)
NS_DECL_CLASSINFO(FCGXStream)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptDocumentFetcherServiceImpl)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptManagerCmdService)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptManagerCmdResponse)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventBaseData)
NS_DECL_CLASSINFO(aptEventBaseData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventDocInitializedData)
NS_DECL_CLASSINFO(aptEventDocInitializedData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventHTMLParseCompleteData)
NS_DECL_CLASSINFO(aptEventHTMLParseCompleteData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventHTMLParseStartData)
NS_DECL_CLASSINFO(aptEventHTMLParseStartData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptHTTPRequest)
NS_DECL_CLASSINFO(aptHTTPRequest)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptHTTPResponse)
NS_DECL_CLASSINFO(aptHTTPResponse)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventLogData)
NS_DECL_CLASSINFO(aptEventLogData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventNewHTMLElementData)
NS_DECL_CLASSINFO(aptEventNewHTMLElementData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventRequestCompleteData)
NS_DECL_CLASSINFO(aptEventRequestCompleteData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventRequestStartData)
NS_DECL_CLASSINFO(aptEventRequestStartData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventBeforeScriptEvaluatedData)
NS_DECL_CLASSINFO(aptEventBeforeScriptEvaluatedData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventAfterScriptEvaluatedData)
NS_DECL_CLASSINFO(aptEventAfterScriptEvaluatedData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptEventScriptCompileData)
NS_DECL_CLASSINFO(aptEventScriptCompileData)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptJaxerGlobal)
NS_DECL_CLASSINFO(aptJaxerGlobal)

NS_GENERIC_FACTORY_CONSTRUCTOR(aptJaxerGlobalOwner)

/**
 * The nsXULAppInfo object implements nsIFactory so that it can be its own
 * singleton.
 */
static nsXREAppData appData = {
	sizeof(nsXREAppData),
	NULL,
	"Aptana, Inc.", //Vendor
	"Jaxer",        //name of the application
	MOZ_APP_VERSION,      //Major version
	JAXER_BUILD_NUMBER, //BuildID
	"jaxer@aptana.com", //application UUID
	"Copyright (c) 2007-2008 Aptana, Inc.", //Copyright
	0,
	NULL,
	NULL,
	NULL,
	"http://www.aptana.com/jaxer/crashreporter/", // The server URL to send crash reports to.
	NULL
};
 
static const char* gPlatformVersion = MOZILLA_VERSION;
static const char* gPlatformBuildID = JAXER_PLATFORM_BUILD_ID;
static PRBool gLogConsoleErrors = PR_TRUE;
static PRBool gSafeMode = PR_FALSE;

static const nsXREAppData* gAppData = &appData;

char* gWebServerConfig = "../local_jaxer/conf/InternalWebServer.cfg";

class nsXULAppInfo : public nsIXULAppInfo,
#ifdef MOZ_CRASHREPORTER
					public nsICrashReporter,
#endif
					public nsIXULRuntime

{
public:
	NS_DECL_ISUPPORTS_INHERITED
	NS_DECL_NSIXULAPPINFO
	NS_DECL_NSIXULRUNTIME
#ifdef MOZ_CRASHREPORTER
	NS_DECL_NSICRASHREPORTER
#endif
};

	NS_INTERFACE_MAP_BEGIN(nsXULAppInfo)
	NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIXULRuntime)
	NS_INTERFACE_MAP_ENTRY(nsIXULRuntime)
#ifdef MOZ_CRASHREPORTER
	NS_INTERFACE_MAP_ENTRY(nsICrashReporter)
#endif
	NS_INTERFACE_MAP_ENTRY_CONDITIONAL(nsIXULAppInfo, gAppData)
	NS_INTERFACE_MAP_END

NS_IMETHODIMP_(nsrefcnt)
nsXULAppInfo::AddRef()
{
	return 1;
}

NS_IMETHODIMP_(nsrefcnt)
nsXULAppInfo::Release()
{
	return 1;
}

NS_IMETHODIMP
nsXULAppInfo::GetVendor(nsACString& aResult)
{
	aResult.Assign(gAppData->vendor);
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetName(nsACString& aResult)
{
	aResult.Assign(gAppData->name);
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetID(nsACString& aResult)
{
	aResult.Assign(gAppData->ID);
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetVersion(nsACString& aResult)
{
	aResult.Assign(gAppData->version);
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetPlatformVersion(nsACString& aResult)
{
	aResult.Assign(gPlatformVersion);
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetAppBuildID(nsACString& aResult)
{
	aResult.Assign(gAppData->buildID);
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetPlatformBuildID(nsACString& aResult)
{
	aResult.Assign(gPlatformBuildID);
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetLogConsoleErrors(PRBool *aResult)
{
	*aResult = gLogConsoleErrors;
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::SetLogConsoleErrors(PRBool aValue)
{
	gLogConsoleErrors = aValue;
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetInSafeMode(PRBool *aResult)
{
	*aResult = gSafeMode;
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetOS(nsACString& aResult)
{
	aResult.AssignLiteral(OS_TARGET);
	return NS_OK;
}

NS_IMETHODIMP
nsXULAppInfo::GetXPCOMABI(nsACString& aResult)
{
#ifdef TARGET_XPCOM_ABI
	aResult.AssignLiteral(TARGET_XPCOM_ABI);
	return NS_OK;
#else
	return NS_ERROR_NOT_AVAILABLE;
#endif
}

#ifdef MOZ_CRASHREPORTER
NS_IMETHODIMP
nsXULAppInfo::AnnotateCrashReport(const nsACString& key,
                                 const nsACString& data)
{
	return CrashReporter::AnnotateCrashReport(key, data);
}

NS_IMETHODIMP
nsXULAppInfo::WriteMinidumpForException(void* aExceptionInfo)
{
#ifdef XP_WIN32
  return CrashReporter::WriteMinidumpForException(static_cast<EXCEPTION_POINTERS*>(aExceptionInfo));
#else
  return NS_ERROR_NOT_IMPLEMENTED;
#endif
}

#endif

static const nsXULAppInfo kAppInfo;
static NS_METHOD
AppInfoConstructor(nsISupports* aOuter,
                                   REFNSIID aIID, void **aResult)
{
	NS_ENSURE_NO_AGGREGATION(aOuter);

	return const_cast<nsXULAppInfo*>(&kAppInfo)->
		QueryInterface(aIID, aResult);
}


static NS_METHOD 
RegisterJaxerGlobal(nsIComponentManager *aCompMgr,
                        nsIFile *aPath,
                        const char *registryLocation,
                        const char *componentType,
                        const nsModuleComponentInfo *info)
{
	nsresult rv = NS_OK;
	nsCOMPtr<nsICategoryManager> catman =
		do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv);
	NS_ENSURE_SUCCESS(rv, rv);
	
	nsXPIDLCString previous;
	rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_PRIVILEGED_PROPERTY_CATEGORY,
                                  "Jaxer",
                                  APT_JAXERGLOBALOWNER_CONTRACTID,
                                  PR_TRUE, PR_TRUE, getter_Copies(previous));
	NS_ENSURE_SUCCESS(rv, rv);
	
	return rv;
}

static const nsModuleComponentInfo kComponents[] =
{
#if 0
    {
        "aptConsoleListener", 
        APT_CONSOLELISTENER_CID, 
        APT_CONSOLELISTENER_CONTRACTID, 
        aptConsoleListenerConstructor
    },
#endif
    {
        "aptCoreLog", 
        APT_CORELOG_CID, 
        APT_CORELOG_CONTRACTID, 
        aptCoreLogConstructor,
		nsnull,
		nsnull,
		nsnull, /* no factory destructor */
		NS_CI_INTERFACE_GETTER_NAME(aptCoreLog),  /* interface getter */
		nsnull /* no language helper */,
		&NS_CLASSINFO_NAME(aptCoreLog),
		nsIClassInfo::SINGLETON
    },
    {
        "aptEventTypeManager", 
        APT_EVENTTYPEMANAGER_CID, 
        APT_EVENTTYPEMANAGER_CONTRACTID, 
        aptEventTypeManagerConstructor
    },
    {
        "FCGXStream", 
        FCGXSTREAM_CID, 
        FCGXSTREAM_CONTRACTID, 
        FCGXStreamConstructor
    },
    {
        "aptDocumentFetcherService", 
        APT_DOCUMENTFETCHERSERVICE_CID,
        APT_DOCUMENTFETCHERSERVICE_CONTRACTID,
        aptDocumentFetcherServiceImplConstructor
    },
    {
        "aptManagerCmdService", 
        APT_MANAGERCMDSERVICE_CID,
        APT_MANAGERCMDSERVICE_CONTRACTID,
        aptManagerCmdServiceConstructor
    },
    {
        "aptManagerCmdResponse", 
        APT_MANAGERCMDRESPONSE_CID,
        APT_MANAGERCMDRESPONSE_CONTRACTID,
        aptManagerCmdResponseConstructor
    },
    {
        "aptEventBaseData", 
        APT_EVENT_BASE_CID, 
        APT_EVENT_BASE_CONTRACTID, 
        aptEventBaseDataConstructor
    },
    {
        "aptEventDocInitializedData", 
        APT_EVENT_DOC_INITIALIZED_CID, 
        APT_EVENT_DOC_INITIALIZED_CONTRACTID, 
        aptEventDocInitializedDataConstructor
    },
    {
        "aptEventHTMLParseCompleteData", 
        APT_EVENT_HTML_PARSE_COMPLETE_CID, 
        APT_EVENT_HTML_PARSE_COMPLETE_CONTRACTID, 
        aptEventHTMLParseCompleteDataConstructor
    },
    {
        "aptEventHTMLParseStartData", 
        APT_EVENT_HTML_PARSE_START_CID, 
        APT_EVENT_HTML_PARSE_START_CONTRACTID, 
        aptEventHTMLParseStartDataConstructor
    },
    {
        "aptHTTPRequest", 
        APT_HTTP_REQUEST_CID, 
        APT_HTTP_REQUEST_CONTRACTID, 
        aptHTTPRequestConstructor
    },
    {
        "aptHTTPResponse", 
        APT_HTTP_RESPONSE_CID, 
        APT_HTTP_RESPONSE_CONTRACTID, 
        aptHTTPResponseConstructor
    },
    {
        "aptEventLogData", 
        APT_EVENT_LOG_CID, 
        APT_EVENT_LOG_CONTRACTID, 
        aptEventLogDataConstructor
    },
    {
        "aptEventNewHTMLElementData", 
        APT_EVENT_NEW_HTML_ELEMENT_CID, 
        APT_EVENT_NEW_HTML_ELEMENT_CONTRACTID, 
        aptEventNewHTMLElementDataConstructor
    },
    {
        "aptEventRequestCompleteData", 
        APT_EVENT_REQUEST_COMPLETE_CID, 
        APT_EVENT_REQUEST_COMPLETE_CONTRACTID, 
        aptEventRequestCompleteDataConstructor
    },
    {
        "aptEventRequestStartData", 
        APT_EVENT_REQUEST_START_CID, 
        APT_EVENT_REQUEST_START_CONTRACTID, 
        aptEventRequestStartDataConstructor
    },
    {
        "aptEventBeforeScriptEvaluatedData", 
        APT_EVENT_BEFORE_SCRIPT_EVALUATED_CID, 
        APT_EVENT_BEFORE_SCRIPT_EVALUATED_CONTRACTID, 
        aptEventBeforeScriptEvaluatedDataConstructor
    },
    {
        "aptEventAfterScriptEvaluatedData", 
        APT_EVENT_AFTER_SCRIPT_EVALUATED_CID, 
        APT_EVENT_AFTER_SCRIPT_EVALUATED_CONTRACTID, 
        aptEventAfterScriptEvaluatedDataConstructor
    },
	{
		"aptEventScriptCompileData",
		APT_EVENT_SCRIPT_COMPILE_CID,
		APT_EVENT_SCRIPT_COMPILE_CONTRACTID,
		aptEventScriptCompileDataConstructor
	},
	{
        "aptJaxerGlobal", 
        APT_JAXERGLOBAL_CID, 
        APT_JAXERGLOBAL_CONTRACTID, 
        aptJaxerGlobalConstructor
    },
    {
        "aptJaxerGlobalOwner", 
        APT_JAXERGLOBALOWNER_CID, 
        APT_JAXERGLOBALOWNER_CONTRACTID, 
        aptJaxerGlobalOwnerConstructor,
		RegisterJaxerGlobal
    },
    {
		"nsXULAppInfo",
		APPINFO_CID,
		XULAPPINFO_SERVICE_CONTRACTID,
		AppInfoConstructor
	}
#ifdef MOZ_CRASHREPORTER
	,
	{
		"nsXULAppInfo",
		APPINFO_CID,
		NS_CRASHREPORTER_CONTRACTID,
		AppInfoConstructor
	}
#endif
};

NS_IMPL_NSGETMODULE(Jaxer, kComponents) 

static nsStaticModuleInfo const kStaticModules =
{
    "Jaxer",
    Jaxer_NSGetModule
};
static PRUint32 const kModulesCount = 1;

// The global object that is used for Jaxer logging.  It must be Inited after XPCOM is coded
// before it can be used.
aptCoreTrace gJaxerLog;

#if defined(JAXER_PRO_BUILD)
#include "JaxerLicenseValidator.h"
#endif

// Other globals
static HANDLE g_pipe = 0;
eJAXER_STATE gJaxerState = eRUNNING;

static void removeArg(int *argc, char **argv) {
  do {
    *argv = *(argv + 1);
    ++argv;
  } while (*argv);

  --(*argc);
}

static int JaxerStartup(int &argc, char **argv)
{
    // tell manager I am up and establish the communication channel for later use

    // return 0 on success, none-0 if fails.
    // program should terminate if this function fails.

    // TODO: we should get this from the cmdline and get rid of the env var

    char *pipename = getenv("JAXER_PIPENAME");
    if (pipename == NULL) {
        gJaxerLog.Log(eFATAL, "Must be launched by Jaxer Server Manager.\n");
        fprintf(stderr, "Must be launched by Jaxer Server Manager.\n");
        return 1;
    }

#ifdef _WIN32
    // Manager is waiting for us to connect back to it.  Don't keep it waiting.
    g_pipe = CreateFile(pipename,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        NULL);
    if (g_pipe == INVALID_HANDLE_VALUE) {
        gJaxerLog.Log(eFATAL, "Invalid handle value when creating the pipe");
        return 1;
    }

    // Put pipe into message mode.
    DWORD mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(g_pipe, &mode, NULL, NULL)) {
        gJaxerLog.Log(eFATAL, "Unable to put pipe into message mode.");
        return 1;
    }
#else
    // Manager created a Unix-domain socket pair and passed one of the sockets
    // as standard input.
    g_pipe = 0;

    // Let Manager know we're alive.
    char byte = 1;
    int rc;
    do {
        rc = send(g_pipe, &byte, 1, 0);
    } while (rc < 0 && errno == EINTR);
    if (rc < 0) {
        gJaxerLog.Log(eFATAL, "Error in sending 'alive status' to Jaxer Manager");
        return 1;
    } else if (rc == 0) {
        gJaxerLog.Log(eFATAL, "Manager stop talking to us immediately");
        return 1;
    }
#endif
    return 0;
}

static int jaxerMain(int argc, char **argv);

int main(int argc, char **argv)
{
#ifdef WIN32
    PRBool bLoadMySQL = PR_FALSE;

#ifdef DEBUG
    // the pref setting for debug is too late.  Let's use a cmdline arg of -debug
    for(int i=1; i<argc; i++)
    {
        if (stricmp(argv[i], "-debug") == 0)
        {        
            char szMessage [256];
            wsprintf (szMessage, "Please attach a debugger to the process 0x%X"
                                 " [%d] and click OK",
                      GetCurrentProcessId(), GetCurrentProcessId() );
            MessageBox(NULL, szMessage, "Jaxer Debug Time!",
                       MB_OK | MB_SERVICE_NOTIFICATION);

            removeArg(&argc, &argv[i]);
            break;
        }
    }
#endif
#endif

    nsresult rv;

	nsIServiceManager* serviceManager;
    AppDirectoryProvider appDirectoryProvider;
    nsCOMPtr<nsIDirectoryServiceProvider> dirProvider(&appDirectoryProvider);


	/* Preprocess specific command line args */
	for (int i = 1; i < argc; /*NOP*/ ) {
		if (!strcmp(argv[i], "-tempdir") || !strcmp(argv[i], "--tempdir")) {
			removeArg(&argc, &argv[i]);
			if (i < argc) {
				appDirectoryProvider.SetTempPath(nsDependentCString(argv[i]));
				removeArg(&argc, &argv[i]);
			}
		} else if (!strcmp(argv[i], "-extensions") || !strcmp(argv[i], "--extensions")) {
			removeArg(&argc, &argv[i]);
			if (i < argc) {
				appDirectoryProvider.SetExtensionsPath(nsDependentCString(argv[i]));
				removeArg(&argc, &argv[i]);
			}
		} else if (!strcmp(argv[i], "-handlewinmysqlclientbug") || !strcmp(argv[i], "--handlewinmysqlclientbug")) {
			removeArg(&argc, &argv[i]);
			if (i < argc) {
#ifdef _WIN32
				if(!stricmp(argv[i], "true") || !strcmp(argv[i], "1"))
                    bLoadMySQL = PR_TRUE;
#endif
				removeArg(&argc, &argv[i]);
			}
		} else if (!PL_strcasecmp(argv[i], "-webserverconfig")) {
			removeArg(&argc, &argv[i]);
			if (i < argc) {
				gWebServerConfig = strdup(argv[i]);
				removeArg(&argc, &argv[i]);
			}
		} else {
			++i;
		}
	}

#ifdef _WIN32
    HINSTANCE hMySQL = NULL;
    BOOL bSuccess = FALSE;
    if (bLoadMySQL)
    {
        //printf("Enabled LoadMySQL\n");
        hMySQL = LoadLibrary("libmysql.dll");
        if (hMySQL)
        {
            bSuccess = DisableThreadLibraryCalls(hMySQL);
            if (!bSuccess)
                fprintf(stderr, "Failed to disable thread library call: err=%d\n", GetLastError());
        }else
        {
            fprintf(stderr, "Failed to load libmysql.dll: err=%d\n", GetLastError());
        }
    }
#endif

#ifdef _BUILD_STATIC_BIN
    // Combine the static components and the app components.
    PRUint32 combinedCount = kStaticModuleCount + kModulesCount;
    static nsStaticModuleInfo *sCombined = new nsStaticModuleInfo[combinedCount];
    NS_ENSURE_TRUE(sCombined, NS_ERROR_OUT_OF_MEMORY);

    memcpy(sCombined, kPStaticModules,
           sizeof(nsStaticModuleInfo) * kStaticModuleCount);
    memcpy(sCombined + kStaticModuleCount, &kStaticModules,
           sizeof(nsStaticModuleInfo) * kModulesCount);

    // Spin up the XPCOM infrastructure.
    NS_InitXPCOM3(&serviceManager, nsnull, dirProvider, sCombined, combinedCount);
#else
    // Spin up the XPCOM infrastructure.
    NS_InitXPCOM3(&serviceManager, nsnull, dirProvider, &kStaticModules, kModulesCount); 
#endif
	
    // Exit immediately if we're only interested in XPCOM auto-registration.
    int exitCode = 1;
    if (argc > 1 && !strcmp(argv[1], "-reg")) {
        exitCode = 0;
        gJaxerLog.Log(eDEBUG, "Jaxer -reg call.");
    } else {
        // Give FCGX a chance to adopt stdin/stdout.
	do {
		/* Initialize appData */
		nsCOMPtr<nsIFile> processDir;
		rv = NS_GetSpecialDirectory(NS_XPCOM_CURRENT_PROCESS_DIR, getter_AddRefs(processDir));
		if (NS_FAILED(rv)) {
			fprintf(stderr, "Get current process directory failed: rv=0x%x", rv);
			break;
		}

		nsCOMPtr<nsILocalFile> xreDirectory(do_QueryInterface(processDir));
		appData.xreDirectory = xreDirectory;
		NS_IF_ADDREF(appData.xreDirectory);

#ifdef MOZ_CRASHREPORTER
		rv = CrashReporter::SetExceptionHandler(appData.xreDirectory, appData.crashReporterURL);
		if (NS_SUCCEEDED(rv)) {
			// pass some basic info from the app data
			if (appData.vendor)
				CrashReporter::AnnotateCrashReport(NS_LITERAL_CSTRING("Vendor"),
											 nsDependentCString(appData.vendor));
			if (appData.name)
				CrashReporter::AnnotateCrashReport(NS_LITERAL_CSTRING("ProductName"),
											 nsDependentCString(appData.name));
			if (appData.version)
				CrashReporter::AnnotateCrashReport(NS_LITERAL_CSTRING("Version"),
											 nsDependentCString(appData.version));
			if (appData.buildID)
				CrashReporter::AnnotateCrashReport(NS_LITERAL_CSTRING("BuildID"),
											 nsDependentCString(appData.buildID));
		}

		nsCOMPtr<nsIFile> dumpD;
		rv = NS_GetSpecialDirectory(NS_OS_TEMP_DIR, getter_AddRefs(dumpD));
		if (NS_FAILED(rv)) {
			fprintf(stderr, "Jaxer: Cannot get OS tmp dir. rv=0x%x", rv);
			break;
		}

		nsAutoString pathStr;
		rv = dumpD->GetPath(pathStr);
		if(NS_SUCCEEDED(rv)) {
		  CrashReporter::SetMinidumpPath(pathStr);
		}
#endif

		// call Jaxer main
		exitCode = jaxerMain(argc, argv);
	} while(PR_FALSE);
    }


#ifdef MOZ_CRASHREPORTER
	CrashReporter::UnsetExceptionHandler();
#endif
	NS_IF_RELEASE(appData.xreDirectory);

	appDirectoryProvider.DoShutdown();
	dirProvider = nsnull;

	NS_ShutdownXPCOM(serviceManager);

#ifdef _BUILD_STATIC_BIN
    delete[] sCombined;
#endif

#ifdef _WIN32
    if (bLoadMySQL && hMySQL)
        FreeLibrary(hMySQL);
#endif

    return exitCode;
}

class AutoPrefObserver {
  public:
    AutoPrefObserver() {
		nsresult rv = PrefObserver::InitPrefObserver();
		if (NS_FAILED(rv)) {
			gJaxerLog.Log(eWARN, "Jaxer: Failed to initialize pref observer. rv=0x%x", rv);
		}
    }
    ~AutoPrefObserver() {
		PrefObserver::ReleasePrefObserver();
    }
};

class AutoConsoleListener {
  public:
    aptConsoleListener CL;
    AutoConsoleListener() {
		CL.Register();
    }
    ~AutoConsoleListener() {
		CL.Unregister();
    }
};

static
int jaxerMain(int argc, char **argv)
{
    nsresult rv;
    int ret = 0;

    nsCOMPtr<nsIPrefBranch> prefBranch(do_GetService(NS_PREFSERVICE_CONTRACTID));

	/* Activate our JavaScript Console listener */
    AutoConsoleListener aCL;


    /* Get the commandport, we use it as a lock for logging */
    char * sLock = "4328";
	for (int i = 1; i < argc-1; ++i) {
		if (!strcmp(argv[i], "-commandport") || !strcmp(argv[i], "--commandport")) {
            sLock = argv[i+1];
		}
	}

    // Let's Init the logger so we can write log msgs.
    rv = gJaxerLog.Init(sLock);
    if (NS_FAILED(rv)) {
        fprintf(stderr, "Cannot init aptCoreTrace for jaxer logging.  error=0x%x", rv);
        return 1;
    }

    gJaxerLog.Log(eDEBUG, "Jaxer: Log inited.");

    // We need to do this (seemingly superfluous operation) in order to make the
    // Mozilla core aware that this thread is the "main" thread.  If we don't do
    // an operation like this here, Mozilla will cough up assertions later when
    // we attempt to load the HTML page.
    nsCOMPtr<nsIURI> url;
    rv = NS_NewURI(getter_AddRefs(url), "resource://framework");
    if (NS_FAILED(rv)) {
        gJaxerLog.Log(eERROR, "Create new URI for resource://framework failed: rv=0x%x", rv);
        return 1;
    }

    if (!prefBranch) {
        gJaxerLog.Log(eERROR, "Get PrefService failed.");
        return 1;
    }

    {
		nsCOMPtr<nsIFile> file;
		PRBool exists = PR_FALSE;
		rv = NS_GetSpecialDirectory(NS_APP_PREF_DEFAULTS_50_DIR, getter_AddRefs(file));
		NS_ENSURE_SUCCESS(rv, 1);
		rv = file->Append(NS_LITERAL_STRING("Jaxer_prefs.js"));
		NS_ENSURE_SUCCESS(rv, 1);
		rv = file->Exists(&exists);
		if (NS_FAILED(rv) || !exists)
		{
			nsString aMsg;
			file->GetPath(aMsg);
            gJaxerLog.Log(eINFO, "Preferences file (%s) could not be located.", NS_ConvertUTF16toUTF8(aMsg).get());
			return 1;
		}
        gJaxerLog.Log(eTRACE, "Loaded Jaxer_prefs.js");
		
		PRInt32 type;
		rv = prefBranch->GetPrefType("Jaxer.HTML.Tag.Monitor.1", &type);
        if ( NS_FAILED(rv) || type != nsIPrefBranch::PREF_STRING) {
			nsString aMsg;
			file->GetPath(aMsg);
            gJaxerLog.Log(eINFO, "Jaxer preferences were not found in file (%s).", NS_ConvertUTF16toUTF8(aMsg).get());
			return 1;
		}
    }

    /* Initialize preference observer */
	AutoPrefObserver apo;

	/* Send startup event */
    nsCOMPtr<nsIObserver> startupNotifier (do_CreateInstance(NS_APPSTARTUPNOTIFIER_CONTRACTID, &rv));
    startupNotifier->Observe(nsnull, APPSTARTUP_TOPIC, nsnull);

	/* Initialize command line */
	nsCOMPtr<nsICommandLineRunner> cmdLine(do_CreateInstance("@mozilla.org/toolkit/command-line;1"));
    if (!cmdLine) {
        gJaxerLog.Log(eERROR, "Failed to create nsICommandLineRunner");
        return 1;
    }

	nsCOMPtr<nsIFile> workingDir;
	rv = NS_GetSpecialDirectory(NS_OS_CURRENT_WORKING_DIR, getter_AddRefs(workingDir));
    if (NS_FAILED(rv)) {
        gJaxerLog.Log(eERROR, "Get current working directory failed: rv=0x%x", rv);
        return 1;
    }

	rv = cmdLine->Init(argc, argv, workingDir, nsICommandLine::STATE_INITIAL_LAUNCH);
    if (NS_FAILED(rv)) {
        gJaxerLog.Log(eERROR, "CommandLine init failed: rv=0x%x", rv);
        ret = 1;
        return 1;
    }
    
	rv = cmdLine->Run();
    if (NS_FAILED(rv)) {
        gJaxerLog.Log(eERROR, "CommandLine Run failed: rv=0x%x", rv);
        return 1;
    }
	
	// Init on main thread - same reasons as above
	{
	    nsCOMPtr<nsICertOverrideService> overrideService = do_GetService(NS_CERTOVERRIDE_CONTRACTID);
	}
    
#if defined(MOZ_WIDGET_GTK2)
    gtk_init_check(&argc, &argv);
#endif

#if defined(JAXER_PRO_BUILD)
	char sUser[400];
	if (validateLicense(sUser) != PR_TRUE)
	{
		gJaxerLog.Terminate();
		return 1;
	}
#endif


    //rv = FCGXStream::Startup(argc, argv);
    if (JaxerStartup(argc, argv) != 0)
    {
        gJaxerLog.Log(eERROR, "Jaxer: Failed to establish communication with manager.");
        return 1;
    }
    gJaxerLog.Log(eTRACE, "Inited manager communication");

    // Set WebConfig
	HTTPStream::SetWebServerConfigFile(gWebServerConfig);
	

    nsCOMPtr<aptIDocumentFetcherService> dfs;
    nsCOMPtr<aptIManagerCmdService> mcs;
    nsCOMPtr<nsIRunnable> aEvent;
    nsCOMPtr<nsIThread> aRunner;
    nsIThread *MainThread;

    dfs = do_GetService("@aptana.com/httpdocumentfetcher;1", &rv);
    rv = dfs->Init();
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "Init aptIDocumentFetcherService failed: rv=0x%x", rv);
        ret = 1;
        goto Done;
    }
    gJaxerLog.Log(eTRACE, "Inited aptIDocumentFetcherService");

    mcs = do_GetService("@aptana.com/managercmdservice;1", &rv);
    rv = mcs->Init((PRInt32)g_pipe);
    if (NS_FAILED(rv))
    {
        gJaxerLog.Log(eERROR, "Init aptIManagerCmdService failed: rv=0x%x", rv);
        ret = 1;
        goto Done;
    }
    
	/* Jaxer has started up! */
#if defined(JAXER_PRO_BUILD)
	gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] %s", JAXER_BUILD_ID, sUser);
#else
    gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Community Edition, licensed by Aptana, Inc. under GPL/APL (see LICENSE.TXT)", JAXER_BUILD_ID);
#endif


    aEvent = new aptMgrCmdReader(g_pipe);
    if (!aEvent)
    {
        //return NS_ERROR_OUT_OF_MEMORY;
        ret = 1;
        goto Done;
    }
    rv = NS_NewThread(getter_AddRefs(aRunner), aEvent);
    if (NS_FAILED(rv)) {
        ret = 1;
        goto Done;
    }

    // Start waiting for incoming requests
    MainThread = NS_GetCurrentThread();
    while (gJaxerState != eEXITING) {

        gJaxerLog.Log(eTRACE, "Waiting for next request.");
        PRBool processedEvent;
        rv = MainThread->ProcessNextEvent(PR_TRUE, &processedEvent);
        if (NS_FAILED(rv))
        {
            //Process event failed
        }
        if (!processedEvent)
        {
            //Process event failed
        }
    }

Done:
    gJaxerLog.Log(eTRACE, "Main done.  Wait for thread");
    gJaxerState = eEXITING;

#ifdef _WIN32
	CloseHandle(g_pipe);
#else
	close(g_pipe);
#endif

    if (aRunner)
        aRunner->Shutdown();
    gJaxerLog.Log(eTRACE, "Main done.  Thread returned");

    gJaxerLog.Terminate();
    //FCGXStream::Cleanup();
	return ret;
}
