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

#include "../../mozilla/aptana/include/JaxerDefs.h"

// IMPORTANT: all globals are accessed from the main thread only!
// (Only Windows version runs multi-theaded.)

// Configuration settings (reading from other threads allowed).
static unsigned g_WebPort = 4327;
static unsigned g_CommandPort = 4328;
static unsigned g_MinServers = 1;
static unsigned g_MaxServers = 10;
static unsigned g_MaxRequests = 0;

// Determine how long an idle server may live (seconds)
// At least g_IdleBase, and increases as the # of idle
// reduces
static unsigned g_IdleBase = 10;
static unsigned g_IdleFactor = 1;

static unsigned g_RequestTimeout = 20; //In seconds
static unsigned g_MaxMemory = 150;
static const char *g_ConfigFile = "JaxerManager.cfg";

static char *pidFilename = NULL;

//Whether manager is running as a service.  Only used for Windows
static bool g_IsService = false;

#ifndef PATH_MAX
#define PATH_MAX (FILENAME_MAX * 50)
#endif

// This is the max # of bytes we assume a single line in a file will have
// This is mainly for config and other files used by manager.
#define MAX_LINE_LENGTH 1024

static char g_JaxerLogFileUri[PATH_MAX] = "resource:///framework/JaxerCoreJSConsole.log";
static char g_JaxerLogFile[PATH_MAX] = "framework/JaxerCoreJSConsole.log";
static int g_JaxerLogBackups = 10;
static int g_JaxerLogSize = 1024 * 500; // 500K bytes

#if __APPLE__
static int psCommandFailed = 0;
#endif

static bool WritePidFile();


static eLogLevel g_eLogLevel = eINFO;

void Log(eLogLevel, char* sFormat, ...);
void Log(eLogLevel, int nError, char* sFormat, ...);

#ifdef _WIN32

#else

#include <unistd.h>
#include <sys/file.h>
#include <errno.h>

#define HANDLE int
#endif


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

// Time to give processes to exit gracefully.
#define SECONDS_TO_LIVE 10

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
static unsigned g_Timeout = 0;
static unsigned g_secsSinceLastPing = 0;

// Accumulated settings from setconfig commands.
static std::map<std::string, std::string> g_Settings;
static std::map<std::string, std::string> g_LogSettings;
static std::map<std::string, std::string> g_PrefSettings;
static std::map<std::string, std::string> g_ManagerSettings;
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
//static void DoLogFileRotation();
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

static void StartNewRequest(Connection *c);

static Server *g_servers;
static Server *g_idleServers;
static unsigned g_idleCount;
static unsigned g_minIdleCount;
static unsigned g_serverCount;
static bool g_processCreationPending;
static unsigned g_elapsedTimeServerBelowMin = 0;

// How to pick the next idle server for new request
static bool g_roundRobinJaxers = true;

static unsigned g_startJaxerTimeout = 30;

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


static void AddIdleServer(Server *s);
static Server *GetIdleServer();
static void RemoveServer(Server *s);
static void LastServerStopped();
void AcceptStdinCommand();
static void SlaughterAllServers(bool isExiting);

static bool ProcessCreationAllowed()
{
    if (!g_processCreationPending && g_serverCount < g_MaxServers) {
        if (g_ManagerMode == mm_running)
            return true;
    }

    return false;
}

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

    ServerSettings()
    :m_requestTimeoutExtra(0)
    ,m_maxRequests(0)
    {
    };

    void ResetOnNewRequest()
    {
        // any values that should be reset for a new request
        m_requestTimeoutExtra = 0;
    };

    int DoSet(Administrator *a, char* cmd);

    int DoGet(Administrator *a, char* cmd);
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
//static int BuildGetMgrSettingCmdResponse(char* buf, char* cmd);
static char *ScanWord(char *&p);
static void DoSet(Administrator *a, char *p, bool isStartup);
static void DoGet(Administrator *a, char *p);
static void ProcessNoRespCmd(Server* s, char* cmd);

#endif //JAXERMANAGER_DECL_H__

