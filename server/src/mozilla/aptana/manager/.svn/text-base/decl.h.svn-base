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

// Declarations common to both Windows and Unix.  It is included at the top
// of the cpp file.
#ifndef JAXERMANAGER_DECL_H__
#define JAXERMANAGER_DECL_H__
#include <map>
#include <string>
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>

#include "JaxerDefs.h"
#include "jaxerProtocolVersion.h"

#ifdef _WIN32

#else

#include <unistd.h>
#include <sys/file.h>
#include <errno.h>

#define HANDLE int
#define stricmp strcasecmp

#endif


#ifndef PATH_MAX
#define PATH_MAX (FILENAME_MAX * 50)
#endif

// This is the max # of bytes we assume a single line in a file will have
// This is mainly for config and other files used by manager.
#define MAX_LINE_LENGTH 1024

// Valid Manager setting names
enum eSettingNameIndex
{
     eNAME_MGR_CONFIG_FILE = 0
    ,eNAME_ADMIN_HTTP_INTERFACE
    ,eNAME_ADMIN_HTTP_PORT
    ,eNAME_ADMIN_JAXER_INTERFACE
    ,eNAME_ADMIN_JAXER_PORT
    ,eNAME_ENABLE_ADMIN_JAXER_CONNECTION
    ,eNAME_ENABLE_ADMIN_HTTP_CONNECTION
    ,eNAME_ENABLE_WEB_HTTP_CONNECTION
    ,eNAME_ENABLE_WEB_JAXER_CONNECTION
    ,eNAME_IWS_CONFIG_FILE
    ,eNAME_JAXER_IDLE_TIMEOUT_SECS
    ,eNAME_LOG_LEVEL
    ,eNAME_JAXER_MAX_MEMORY_MB
    ,eNAME_MAX_JAXERS
    ,eNAME_MAX_REQUESTS_PER_JAXER
    ,eNAME_MIN_JAXERS
    ,eNAME_PID_FILE
    ,eNAME_PING_TIMEOUT_SECS
    ,eNAME_JAXER_REQUEST_TIMEOUT_SECS
    ,eNAME_ROUND_ROBIN_JAXER
    ,eNAME_START_JAXER_TIMEOUT_SECS
    //,eNAME_TEMP_DIR
    ,eNAME_WEB_HTTP_INTERFACE
    ,eNAME_WEB_HTTP_PORT
    ,eNAME_WEB_JAXER_INTERFACE
    ,eNAME_WEB_JAXER_PORT
    ,eNAME_LAST
};

static const char *gSettingNames[eNAME_LAST] =
{
     "configfile"
    ,"adminhttpinterfacelocalip"
    ,"adminhttpport"
    ,"commandinterfacelocalip"
    ,"commandport"
    ,"enableadminconnection"
    ,"enableadminhttpconnection"
    ,"enableinternalwebserver"
    ,"enablewebjaxerconnection"
    ,"iwsconfigfile"
    ,"jaxeridletimeout"
    ,"loglevel"
    ,"maxmemory"
    ,"maxprocesses"
    ,"maxrequests"
    ,"minprocesses"
    ,"pid-file"
    ,"pingtimeout"
    ,"requesttimeout"
    ,"roundrobinjaxers"
    ,"startjaxertimeout"
    //,"tempdir"
    ,"webhttpinterfacelocalip"
    ,"webhttpport"
    ,"webinterfacelocalip"
    ,"webport"
};


// IMPORTANT: all globals are accessed from the main thread only!
// (Only Windows version runs multi-theaded.)

enum eManagerSettingType
{
	 eMST_STRING = 0
	,eMST_INT    = 1
	,eMST_BOOLEAN= 2
};

struct GeneralSetting
{
	eManagerSettingType mType;
	const char         *mCmdName;
	char               *msValue;
	bool                mRestart;
	bool                mbValue;
	unsigned            miValue;
	GeneralSetting(eManagerSettingType eType, const char* name, char* adefault, bool bRestart=false)
		: mType(eType)
		 ,mCmdName(name)
		 ,msValue(0)
		 ,mRestart(bRestart)
		 ,mbValue(false)
		 ,miValue(0)
	{
		SetValue(adefault);
	}

	GeneralSetting(const char* name, unsigned adefault, bool bRestart=false)
		: mType(eMST_INT)
		 ,mCmdName(name)
		 ,msValue(0)
		 ,mRestart(bRestart)
		 ,mbValue(false)
		 ,miValue(adefault)
	{
	}

	GeneralSetting(const char* name, bool adefault, bool bRestart=false)
		: mType(eMST_INT)
		 ,mCmdName(name)
		 ,msValue(0)
		 ,mRestart(bRestart)
		 ,mbValue(adefault)
		 ,miValue(0)
	{
	}
	~GeneralSetting()
	{
		if (msValue)
			free(msValue);
	}

	bool SetValue(char* value)
	{
		switch(mType)
		{
		case eMST_STRING:
			return SetStringValue(value);
			break;
		case eMST_BOOLEAN:
			return SetBooleanValue(value);
			break;
		case eMST_INT:
			return SetIntValue(value);
			break;
		}
		return true;
	}

	const char* GetAsString()
	{
		switch (mType)
		{
		case eMST_BOOLEAN:
			SetStringValue(mbValue ? "true" : "false", false);
			break;
		case eMST_INT:
			char buf[20];
			sprintf(buf, "%d", miValue);
			SetStringValue(buf, false);
			break;
        case eMST_STRING:
            break;
		}
		return msValue;
	}

private:
	bool SetStringValue(const char* value, bool bMatchType=true)
	{
		if (bMatchType && mType != eMST_STRING)
		{
			return false;
		}
		if (msValue)
		{
			free(msValue);
			msValue = 0;
		}
		msValue = strdup(value);
		return (msValue != 0);

	}

	bool SetIntValue(char* value)
	{
		if (mType != eMST_INT)
		{
			return false;
		}
		char *p = value;
		while(isspace(*p)) p++;
		unsigned v = 0;
		
		for (; *p; p++)
		{
			if (*p < '0' || *p > '9' || v >= unsigned(-1) / 10)
			{
				return false;
			}
			v = (v * 10) + *p - '0';
		}
		miValue = v;
		return true;
	}

	bool SetIntValue(unsigned value)
	{
		if (mType != eMST_INT)
		{
			return false;
		}
		miValue = value;
		return true;
	}

	bool SetBooleanValue(bool value)
	{
		if (mType != eMST_BOOLEAN)
		{
			return false;
		}
		mbValue = value;
		return true;
	}

	bool SetBooleanValue(char* value)
	{
		if (mType != eMST_BOOLEAN)
		{
			return false;
		}
		if (stricmp("true", value) == 0 || stricmp("on", value) == 0 || stricmp("1", value) == 0)
			mbValue = true;
		else if (stricmp("false", value) == 0 || stricmp("off", value) == 0 || stricmp("0", value) == 0)
			mbValue = false;
		else
			return false;
		return true;
	}
};

enum eConnectionType
{
	 eWEB_JAXER_PROTOCOL   = 0
    ,eWEB_HTTP_PROTOCOL    = 1
	,eADMIN_HTTP_PROTOCOL  = 2
	,eADMIN_JAXER_PROTOCOL = 3
};

struct ConnectionSetting
{
	eConnectionType mType;
	eMSG_TYPE       mMsgType;
	unsigned&       mPort;
	char*&          mInterface;
	bool&           mEnabled;
	ConnectionSetting(eConnectionType aType, eMSG_TYPE aMsgType, unsigned &aPort, char*& aInterface, bool &aEnabled)
		: mType(aType)
		 ,mMsgType(aMsgType)
		 ,mPort(aPort)
		 ,mInterface(aInterface)
		 ,mEnabled(aEnabled)
	{
	}
#if 0
	bool SetInterface(const char* aInterface)
	{
		if (mInterface)
		{
			free(mInterface);
			mInterface = 0;
		}
		mInterface = strdup(aInterface);
		return (mInterface != 0);
	}
#endif
};

struct ConnectionWithSetting
{
	const ConnectionSetting* m_cs;
	SOCKET                   m_socket;
	ConnectionWithSetting(const ConnectionSetting* cs, SOCKET s)
		: m_cs(cs)
		 ,m_socket(s)
	{};
};

/****** Configuration settings (reading from other threads allowed). ******/

//LOG SETTINGS
#define DEFAULT_JAXER_LOG_FILE "./Jaxer.log"
#define DEFAULT_JAXER_LOG_BACKUPS "10"
#define DEFAULT_JAXER_LOG_FILE_Size_BYTES "512000"

#define DEFAULT_MGR_CONFIG_FILE "../local_jaxer/conf/JaxerManager.cfg"
#define DEFAULT_ADMIN_HTTP_INTERFACE "127.0.0.1"
#define DEFAULT_ADMIN_HTTP_PORT "8082"
#define DEFAULT_ADMIN_JAXER_INTERFACE "127.0.0.1"
#define DEFAULT_ADMIN_JAXER_PORT "4328"
#define DEFAULT_ENABLE_ADMIN_JAXER_CONNECTION "true"
#define DEFAULT_ENABLE_ADMIN_HTTP_CONNECTION "true"
#define DEFAULT_ENABLE_WEB_HTTP_CONNECTION "true"
#define DEFAULT_ENABLE_WEB_JAXER_CONNECTION "true"
#define DEFAULT_IWS_CONFIG_FILE "../local_jaxer/conf/InternalWebServer.cfg"
#define DEFAULT_JAXER_IDLE_TIMEOUT_SECS "10"
#define DEFAULT_LOG_LEVEL "INFO"
#define DEFAULT_JAXER_MAX_MEMORY_MB "150"
#define DEFAULT_MAX_JAXERS "10"
#define DEFAULT_MAX_REQUESTS_PER_JAXER "0"
#define DEFAULT_MIN_JAXERS "1"
#define DEFAULT_PID_FILE "../tmp/jaxermanager.pid"
#define DEFAULT_PING_TIMEOUT_SECS "0"
#define DEFAULT_JAXER_REQUEST_TIMEOUT_SECS "60"
#define DEFAULT_ROUND_ROBIN_JAXER "true"
#define DEFAULT_START_JAXER_TIMEOUT_SECS "30"
//#define DEFAULT_TEMP_DIR ""
#define DEFAULT_WEB_HTTP_INTERFACE "127.0.0.1"
#define DEFAULT_WEB_HTTP_PORT "8083"
#define DEFAULT_WEB_JAXER_INTERFACE "127.0.0.1"
#define DEFAULT_WEB_JAXER_PORT "4327"

#define DEFAULT_JAXER_TEMP_DIR "."

//Timeout for a debug connection waiting for an idle Debug Jaxer
#define DEBUG_CONNECTION_TIMEOUT_SECS 10

// Time to give processes to exit gracefully.
#define SECONDS_TO_LIVE 10

static GeneralSetting g_GeneralSettings[eNAME_LAST] =
{
	 GeneralSetting(eMST_STRING, gSettingNames[eNAME_MGR_CONFIG_FILE], DEFAULT_MGR_CONFIG_FILE, true)
	,GeneralSetting(eMST_STRING, gSettingNames[eNAME_ADMIN_HTTP_INTERFACE], DEFAULT_ADMIN_HTTP_INTERFACE, true)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_ADMIN_HTTP_PORT], DEFAULT_ADMIN_HTTP_PORT, true)
	,GeneralSetting(eMST_STRING, gSettingNames[eNAME_ADMIN_JAXER_INTERFACE], DEFAULT_ADMIN_JAXER_INTERFACE, true)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_ADMIN_JAXER_PORT], DEFAULT_ADMIN_JAXER_PORT, true)
	,GeneralSetting(eMST_BOOLEAN, gSettingNames[eNAME_ENABLE_ADMIN_JAXER_CONNECTION], DEFAULT_ENABLE_ADMIN_JAXER_CONNECTION, true)
	,GeneralSetting(eMST_BOOLEAN, gSettingNames[eNAME_ENABLE_ADMIN_HTTP_CONNECTION], DEFAULT_ENABLE_ADMIN_HTTP_CONNECTION, true)
	,GeneralSetting(eMST_BOOLEAN, gSettingNames[eNAME_ENABLE_WEB_HTTP_CONNECTION], DEFAULT_ENABLE_WEB_HTTP_CONNECTION, true)
	,GeneralSetting(eMST_BOOLEAN, gSettingNames[eNAME_ENABLE_WEB_JAXER_CONNECTION], DEFAULT_ENABLE_WEB_JAXER_CONNECTION, true)
	,GeneralSetting(eMST_STRING, gSettingNames[eNAME_IWS_CONFIG_FILE], DEFAULT_IWS_CONFIG_FILE, true)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_JAXER_IDLE_TIMEOUT_SECS], DEFAULT_JAXER_IDLE_TIMEOUT_SECS, false)
	,GeneralSetting(eMST_STRING, gSettingNames[eNAME_LOG_LEVEL], DEFAULT_LOG_LEVEL, false)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_JAXER_MAX_MEMORY_MB], DEFAULT_JAXER_MAX_MEMORY_MB, false)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_MAX_JAXERS], DEFAULT_MAX_JAXERS, false)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_MAX_REQUESTS_PER_JAXER], DEFAULT_MAX_REQUESTS_PER_JAXER, false)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_MIN_JAXERS], DEFAULT_MIN_JAXERS, false)
	,GeneralSetting(eMST_STRING, gSettingNames[eNAME_PID_FILE], DEFAULT_PID_FILE, true)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_PING_TIMEOUT_SECS], DEFAULT_PING_TIMEOUT_SECS, false)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_JAXER_REQUEST_TIMEOUT_SECS], DEFAULT_JAXER_REQUEST_TIMEOUT_SECS, false)
	,GeneralSetting(eMST_BOOLEAN, gSettingNames[eNAME_ROUND_ROBIN_JAXER], DEFAULT_ROUND_ROBIN_JAXER, false)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_START_JAXER_TIMEOUT_SECS], DEFAULT_START_JAXER_TIMEOUT_SECS, false)
//	,GeneralSetting(eMST_STRING, gSettingNames[eNAME_TEMP_DIR], ".", true)
//	,GeneralSetting(eMST_STRING, NAME_TIMEOUT_SECS, false)
	,GeneralSetting(eMST_STRING, gSettingNames[eNAME_WEB_HTTP_INTERFACE], DEFAULT_WEB_HTTP_INTERFACE, true)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_WEB_HTTP_PORT], DEFAULT_WEB_HTTP_PORT, true)
	,GeneralSetting(eMST_STRING, gSettingNames[eNAME_WEB_JAXER_INTERFACE], DEFAULT_WEB_JAXER_INTERFACE, true)
	,GeneralSetting(eMST_INT, gSettingNames[eNAME_WEB_JAXER_PORT], DEFAULT_WEB_JAXER_PORT, true)
};

// ===The (interface/port)'s and what kind of request to expect===

static ConnectionSetting g_ConnectionSettings[] =
{
	 ConnectionSetting(eWEB_JAXER_PROTOCOL, eNEW_REQUEST_MSG, g_GeneralSettings[eNAME_WEB_JAXER_PORT].miValue,
	     g_GeneralSettings[eNAME_WEB_JAXER_INTERFACE].msValue, g_GeneralSettings[eNAME_ENABLE_WEB_JAXER_CONNECTION].mbValue)
	,ConnectionSetting(eWEB_HTTP_PROTOCOL, eNEW_HTTP_REQUEST_MSG, g_GeneralSettings[eNAME_WEB_HTTP_PORT].miValue,
	     g_GeneralSettings[eNAME_WEB_HTTP_INTERFACE].msValue, g_GeneralSettings[eNAME_ENABLE_WEB_HTTP_CONNECTION].mbValue)
	,ConnectionSetting(eADMIN_HTTP_PROTOCOL, eNEW_ADMIN_REQUEST_MSG, g_GeneralSettings[eNAME_ADMIN_HTTP_PORT].miValue,
	     g_GeneralSettings[eNAME_ADMIN_HTTP_INTERFACE].msValue, g_GeneralSettings[eNAME_ENABLE_ADMIN_HTTP_CONNECTION].mbValue)
	,ConnectionSetting (eADMIN_JAXER_PROTOCOL, eINVALID_MSG, g_GeneralSettings[eNAME_ADMIN_JAXER_PORT].miValue,
	    g_GeneralSettings[eNAME_ADMIN_JAXER_INTERFACE].msValue, g_GeneralSettings[eNAME_ENABLE_ADMIN_JAXER_CONNECTION].mbValue)
};

static unsigned g_nConnectionSettings = sizeof(g_ConnectionSettings)/sizeof(*g_ConnectionSettings);
// === End of (interface/port)'s ===


// Determine how long an idle server may live (seconds)
// At least g_IdleBase, and increases as the # of idle
// reduces
static unsigned g_IdleBase = 10;
static unsigned g_IdleFactor = 1;

//Whether manager is running as a service.  Only used for Windows
static bool g_IsService = false;

static eLogLevel g_eLogLevel = eINFO;

// If no startjaxertimeout is specified on the cmdline or cfg file,
// then we compute the timeout based on g_minServers as
// timeout = max (g_SJTMin, g_SJTBase + g_minServers * g_SJTFactor)

static bool     g_startJaxerTimeoutSet = false;
static unsigned g_SJTMin = 30;
static unsigned g_SJTBase = 10;
#ifdef _WIN32
static unsigned g_SJTFactor = 2;
#else
static unsigned g_SJTFactor = 6;
#endif

//static unsigned g_MinServers = 1;
//static unsigned g_MaxServers = 10;
//static unsigned g_MaxRequests = 0;
//static unsigned g_RequestTimeout = 20; //In seconds
//static unsigned g_MaxMemory = 150;
//static const char *g_ConfigFile = "../local_jaxer/conf/JaxerManager.cfg";

// If internal webserver is enabled, then Config file for internal webserver

//static char *pidFilename = "jaxermanager.pid";

//static char g_JaxerLogFileUri[PATH_MAX] = "resource:///framework/JaxerCoreJSConsole.log";
//static unsigned g_Timeout = 0;
//static std::map<std::string, std::string> g_ManagerSettings;
//static void DoLogFileRotation();
// How to pick the next idle server for new request
//static bool g_roundRobinJaxers = true;

//static unsigned g_startJaxerTimeout = 30;
//void AcceptStdinCommand();
//static int BuildGetMgrSettingCmdResponse(char* buf, char* cmd);

//Check the first few bytes to see if this is an HTTP request
//static bool IsHTTPRequest(const char* str, size_t len);



#if __APPLE__
static int psCommandFailed = 0;
#endif

static bool WritePidFile();

void Log(eLogLevel, char* sFormat, ...);
void Log(eLogLevel, int nError, char* sFormat, ...);


class LogServerInfo
{
    int m_pid;
    char *m_sFifoName;
    char* m_sLogFileName;
    bool m_bExiting;
    HANDLE m_hProcess;
    HANDLE m_hProcStdInWr;
    HANDLE m_hProcStdErrRd;    

public:
    LogServerInfo():m_pid(-1), m_sFifoName(0), m_bExiting(false), m_hProcess(INVALID_HANDLE_VALUE),
    m_hProcStdInWr(INVALID_HANDLE_VALUE){};
    bool IsExiting() {return m_bExiting;}
    bool SetFifoName(const char* sFifo)
    {
        if (m_sFifoName)
            delete[] m_sFifoName;
        m_sFifoName = new char[strlen(sFifo)+1];
        if (!m_sFifoName)
            return false;
        strcpy(m_sFifoName, sFifo);
        return true;
    }


    bool SetLogFileName(const char* sLogName)
    {
        if (m_sLogFileName)
            delete[] m_sLogFileName;
        m_sLogFileName = new char[strlen(sLogName)+1];
        if (!m_sLogFileName)
            return false;
        strcpy(m_sLogFileName, sLogName);
        return true;
    }
    void SetPid(int pid) {m_pid = pid;};
    const char* GetFifoName() {return m_sFifoName;}
    const char* GetLogFileName() {return m_sLogFileName;}
    int GetPid() { return m_pid;}
    void SetProcessHandle(HANDLE hProcess) { m_hProcess = hProcess;}
    HANDLE GetProcessHandle() { return m_hProcess;}
    void SetProcessStdinHandle(HANDLE hProcess) { m_hProcStdInWr = hProcess;}
    HANDLE GetProcessStdinHandle() { return m_hProcStdInWr;}
    void SetProcessStderrHandle(HANDLE hProcess) { m_hProcStdErrRd = hProcess;}
    HANDLE GetProcessStderrHandle() { return m_hProcStdErrRd;}
    void Terminate()
    {
        m_bExiting=true;

#ifdef _WIN32
        if (m_hProcStdInWr != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_hProcStdInWr);
            m_hProcStdInWr = INVALID_HANDLE_VALUE;

        }
        if (m_hProcStdErrRd != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_hProcStdErrRd);
            m_hProcStdErrRd = INVALID_HANDLE_VALUE;
        }
        if (m_hProcess != INVALID_HANDLE_VALUE)
        {
            TerminateProcess(m_hProcess, 0);
            CloseHandle(m_hProcess);
            m_hProcess = INVALID_HANDLE_VALUE;
        }
#else
        if (m_pid>0)
        {
            kill(m_pid, SIGKILL);
            m_pid = -1;
        }
#endif
        
        if (m_sFifoName)
        {
            unlink(m_sFifoName);
            delete[] m_sFifoName;
            m_sFifoName = 0;
        }
        if (m_sLogFileName)
        {
            delete[] m_sLogFileName;
            m_sLogFileName = 0;
        }
    }
    ~LogServerInfo(){ Terminate();}
};

static void LogTrace(const char* text, int len);


// Global manager mode.
enum ManagerMode
{
    mm_running,
    mm_stopping,
    mm_paused,
    mm_reloading,
    mm_stopped
};

static ManagerMode g_ManagerMode = mm_running;

// If mode is mm_reloading and g_NewConfig is set, it will be read and applied
// once all Jaxers have stopped.
static FILE *g_NewConfig = 0;

// Timeout tracking.
static unsigned g_secsSinceLastPing = 0;

// Accumulated settings from setconfig commands.
static std::map<std::string, std::string> g_Settings;
static std::map<std::string, std::string> g_LogSettings;
static std::map<std::string, std::string> g_PrefSettings;
typedef std::map<std::string, std::string>::const_iterator iter;
static void InitSettings();

class Message;
struct Administrator;
struct Connection;
struct Server;

static void CountRequest(Server *s);
static void DoCommand(Administrator *a, bool isStartUp = false);
static void LoadConfig(FILE *fd);
static void ProcessArgs(int argc, char **argv);
static void DoPeriodic();
static void DisplayUsage();
#ifdef _WIN32
static void onStopService(Administrator* a);
#endif

enum { BUFSIZE = CMDBUFSIZE };

/***********************************************************************/

struct AdminBufferList
{
    AdminBufferList *m_next;
    int              m_in;
    int              m_out;
    bool             m_flush;
    char             m_data[BUFSIZE];

    AdminBufferList() : m_next(0), m_in(0), m_out(0), m_flush(false)
    {
    }
};

// List of connections waiting for an available process.
static Connection *g_connFirst;
static Connection *g_connLast;
static unsigned g_waitingConnections;

// List of connections waiting for the debug jaxers
static Connection *g_debugConnections;
static Connection *g_debugConnectionsTail;

static void StartNewRequest(Connection *c);

static Server *g_servers;
static Server *g_idleServers;
static Server *g_debugServers;
static unsigned g_idleCount;
static unsigned g_minIdleCount;
static unsigned g_debugCount;
static unsigned g_serverCount;
static bool g_processCreationPending;
static unsigned g_elapsedTimeServerBelowMin = 0;


static void AddIdleServer(Server *s);
static Server *GetIdleServer();
static void RemoveServer(Server *s);
static void LastServerStopped();
static void SlaughterAllServers(bool isExiting);
static bool ProcessCreationAllowed();

/***********************************************************************/

class Message
{
protected:
    unsigned    m_refcount;
    Server     *m_target;
    bool        m_cancelled;

    static Message *s_first;
    static Message *s_last;

public:
    Message() : m_refcount(0), m_target(0), m_cancelled(false), m_next(0)  { }
    virtual ~Message() { }

    void SetTarget(Server *s) { m_target = s; }

    virtual void GetBytes(Server *s, void *&data, unsigned &len) = 0;

    static void Append(Message *m);
    static bool Next(Server *s);
    static void BringUpToDate(Server *s);
    static void CancelTarget(Server *s);
    static void DropSentMessages();

    Message    *m_next;
};

class NullMessage : public Message
{
public:
    NullMessage() : Message() { m_cancelled = true; }

    virtual void GetBytes(Server *s, void *&data, unsigned &len) {
        data = 0;
        len = 0;
    }
};


class PrefMessage : public Message
{
    unsigned char   m_buf[2*256];
    unsigned        m_len;
    //message will be null terminated
    // eg name = "foo" value = "bars"
    // m_buf = 4 "foo" 0 5 "bars" 0
    // m_len = 1 + 4 + 1 + 5

public:
    PrefMessage(const char *name, const char *value);
    virtual void GetBytes(Server *s, void *&data, unsigned &len);
};

class ExitMessage : public Message
{
    unsigned char   m_buf[3];

public:
    ExitMessage();
    virtual void GetBytes(Server *s, void *&data, unsigned &len);
};


// A collection of Jaxer-specific settings that would override the default
// setting.
struct ServerSettings
{
    // Add this on top of the global setting for this request only
    unsigned m_requestTimeoutExtra; //In seconds

    // If not zero, use this instead of the global
    unsigned m_maxRequests;

	Server* m_owner;

    ServerSettings()
    :m_requestTimeoutExtra(0)
    ,m_maxRequests(0)
	,m_owner(NULL)
    {
    };

    void ResetOnNewRequest()
    {
        // any values that should be reset for a new request
        m_requestTimeoutExtra = 0;
    };

    int DoSet(Administrator *a, char* cmd);

    int DoGet(Administrator *a, char* cmd);

	void SetJaxerForDebug();
};


class BlockDumper
{
    char    mBuf[128];
    char   *mNextHex;
    char   *mNextChar;
    int     mOffset;

public:
    BlockDumper()
        : mNextHex(0), mNextChar(0), mOffset(0)
    {
    }


    void Flush()
    {
        if (g_eLogLevel > eTRACE)
            return;

        if (mNextChar) {
            LogTrace(mBuf, mNextChar - mBuf);
            mNextChar = mNextHex = 0;
        }
    }

    void LogHeader(char* title, int len = -1)
    {
        if (g_eLogLevel > eTRACE)
            return;

        Flush();
        int n;
        if (len >= 0)
            n = sprintf(mBuf, "%s (%d bytes)", title, len);
        else
            n = sprintf(mBuf, "%s", title);
        LogTrace(mBuf, n);
        mNextHex = mNextChar = 0;
        mOffset = 0;
    }

    void LogContents(char *p, int len, bool reset = true)
    {
        if (g_eLogLevel > eTRACE)
            return;

        if (reset)
            mOffset = 0;

        while (len > 0) {
            len--;
            int offset = mOffset & 15;
            if (offset == 0) {
                sprintf(mBuf, "%4.4x: ", mOffset);
                mNextHex = mBuf + 6;
                memset(mNextHex, ' ', 3 * 16);
                mNextChar = mNextHex + 3 * 16;
            }

            const char *hex = "0123456789abcdef";
            *mNextHex++ = hex[((unsigned char) *p) >> 4];
            *mNextHex++ = hex[((unsigned char)*p) & 0xf];
            mNextHex++;

            int c = *p++;
            *mNextChar++ = (c >= 32 && c <= 127) ? char(c) : ' ';

            mOffset++;
            if (offset == 15)
                Flush();
        }
        Flush();
    }

    void LogProtocolError(char *p, int len)
    {
        Log(eTRACE, "Protocol error -- bytes at point of error:");
        if (len == 0)
            Log(eTRACE, "   no bytes available");
        else
            LogContents(p, len, true);
        Flush();
    }
};

void logme(char* format, ...);

// CMD functions
static int BuildCmdResponse(char* buf, const char* data, int datalen, bool hasMore);
static int BuildErrorResponse(char* buf, int errorCode, const char* errorText);
static char *ScanWord(char *&p);
static void DoSet(Administrator *a, char *p, bool isStartup);
static void DoGet(Administrator *a, char *p);
static void ProcessNoRespCmd(Server* s, char* cmd);
static char *get_jaxer_id_from_http_req(char* req, int len);
static bool find_server_and_handle_connection(Connection* c, const char* jaxer_id, Server* hdr);

#endif //JAXERMANAGER_DECL_H__

