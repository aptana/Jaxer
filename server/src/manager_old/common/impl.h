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

// Implementation common to both Windows and Unix.  It is included at the end
// of the cpp file.

static int htoi(char *s)
{
    int value;
    int c;

    c = ((unsigned char *)s)[0];
    if (isupper(c))
        c = tolower(c);
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

    c = ((unsigned char *)s)[1];
    if (isupper(c))
        c = tolower(c);
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

    return (value);
}

static int rawURlDecode(char* urlEncodedData, int len)
{
    // inplace decode the string and return the length of the decoded string.
    char *dest = urlEncodedData;
    char *data = urlEncodedData;

    while (len--)
    {
        if (*data == '+')
        {
            *dest = ' ';
        } else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) 
                 && isxdigit((int) *(data + 2)))
        {
            *dest = (char) htoi(data + 1);
            data += 2;
            len -= 2;
        } else {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = '\0';
    return (int) (dest - urlEncodedData);
}

#if 0
static void SetJaxerCmdLine(Administrator *a, const char *name, const char *value)
{
    g_JaxerCmdLine[name] = value;
    a->Write("*** ok\r\n");
}
#endif

static void SetCfg(Administrator *a, const char *name, const char *value)
{
    std::string s = "-";
    s += name;
    g_Settings[s] = value;
    a->Write("*** ok\r\n");
}

static void Setlog(Administrator *a, const char *name, const char *value)
{
    g_LogSettings[name] = value;
    a->Write("*** ok\r\n");
}

static void StartNewRequest(Connection *c)
{
    Log(eTRACE,"Enter StartNewRequest");

    c->m_next = 0;
    c->m_pending = false;

    // If we are the only waiting request, and
    // If there's an idle process available, go assign it.  Defer assignment if
    // currently paused.
    if (!g_connFirst && g_idleServers && g_ManagerMode != mm_paused) {
    
        Server *s = GetIdleServer();
        if (s)
        {
            AssignServer(s, c);
            //Log(eTRACE,"Return From StartNewRequest (after AssignServer)");
            return;
        }else
        {
            Log(eERROR, "StartNewRequest: get idle server failed");
        }
    }

    // Nope, go to add it to the waiting list.
    g_waitingConnections++;
    if (g_connFirst) {
        g_connLast->m_next = c;
        g_connLast = c;
    } else {
        g_connFirst = c;
        g_connLast = c;
    }

#if 0
    // If the waiting list is too long, go start another process.
    if (double(g_waitingConnections) / g_serverCount > 1.5)
        CreateNewProcess();
#else
    // For now, spin up a new process unconditionally.  We need a
    // more sophisticated algorithm that takes into account how long
    // it's been since a new process was started, otherwise we can
    // have a situation where a Jaxer hangs tring to make a recursive
    // web request.
    CreateNewProcess();
    //Log(eTRACE,"Return From StartNewRequest (after CreateNewProcess)");
#endif
}

static void CountRequest(Server *s)
{
    //Log(eTRACE,"Enter CountRequest");
    s->m_nRequests++;
    if ( (s->m_privateSettings.m_maxRequests > 0 && s->m_nRequests >= s->m_privateSettings.m_maxRequests) ||
        (s->m_privateSettings.m_maxRequests == 0 && g_MaxRequests > 0 && s->m_nRequests >= g_MaxRequests)) {
        ExitMessage *em = new ExitMessage;
        em->SetTarget(s);
        s->m_secondsToLive = SECONDS_TO_LIVE;
		Log(eINFO,"Stopping Jaxer (pid=%d) per setting: maxrequests(%d)", s->m_pid, g_MaxRequests);
        Message::Append(em);
    }
    //Log(eTRACE,"Return From CountRequest");
}

/***********************************************************************/

static void AddIdleServer(Server *s)
{
    Log(eTRACE, "Enter AddIdleServer: server=%d pid=%d", s, s->m_pid);
    if (!s->m_deletePending)
    {
        // We add new member to the end, and reset m_idleTime to zero.
        // In this way, we we try to delete an idle server, we just pick up
        // the first one (according to time)
        s->m_nextIdle = 0;
        s->m_idleTime = 0;

        if (!g_idleServers)
        {
            g_idleServers = s;
        }else
        { 
            Server *t = g_idleServers;
            while (t->m_nextIdle)
                t = t->m_nextIdle;
            t->m_nextIdle = s;
        }

        g_idleCount++;
    }
    Log(eTRACE, "AddIdleServer total idle servers=%d", g_idleCount);
}

static Server *GetIdleServer()
{
    Log(eTRACE, "Enter GetIdleServer");
    if (!g_idleServers)
    {
        Log(eERROR, "GetIdleServer: no idle server (g_idleCount=%d g_minIdleCount=%d)", 
            g_idleCount, g_minIdleCount);
        return 0;
    }
    if (g_idleCount < 1)
    {
        Log(eERROR, "GetIdleServer: idle server count is wrong (g_idleCount=%d g_minIdleCount=%d)", 
            g_idleCount, g_minIdleCount);
    }

    Server *s = g_idleServers;
    if (g_roundRobinJaxers)
    {
        g_idleServers = s->m_nextIdle;
        s->m_nextIdle = 0;
    }else
    {
        //starvation, reuse
        if (!s->m_nextIdle)
        {
            // This is the only one on the list
            g_idleServers = 0;
        }else
        {
            // Find the last one and remove it.
            while(s->m_nextIdle->m_nextIdle)
            {
                s = s->m_nextIdle;
            }
            
            Server *t = s->m_nextIdle;
            s->m_nextIdle = 0;
            s = t;      
        }
    }
        
    if (--g_idleCount < g_minIdleCount)
        g_minIdleCount = g_idleCount;

    Log(eTRACE, "GetIdleServer total idle servers=%d", g_idleCount);

    return s;
}

static void RemoveServer(Server *s)
{
    Log(eTRACE,"Enter RemoveServer");
    // Remove from idle list, if on it.
    if (g_idleServers == s) {
        GetIdleServer();
    } else if (g_idleServers) {
        for (Server *is = g_idleServers; is->m_nextIdle; is = is->m_nextIdle) {
            if (is->m_nextIdle == s) {
                is->m_nextIdle = s->m_nextIdle;
                s->m_nextIdle = 0;
                if (--g_idleCount < g_minIdleCount)
                    g_minIdleCount = g_idleCount;
                break;
            }
        }
    }

    // Remove from list of all servers.
    if (g_servers == s) {
        g_servers = s->m_next;
    } else {
        for (Server *as = g_servers; as->m_next; as = as->m_next) {
            if (as->m_next == s) {
                as->m_next = s->m_next;
                break;
            }
        }
    }

    // Cancel any messages queued for it.
    Message::CancelTarget(s);

    // Finally...
    g_serverCount--;

    Log(eTRACE, "RemoveServer #servers=%d #idleServers=%d", g_serverCount, g_idleCount);

    s->m_next = 0;
    s->m_nextIdle = 0;
}

static void LastServerStopped()
{
    Log(eTRACE,"Enter LastServerStopped");
    if (g_ManagerMode == mm_stopping)
    {
        lc.Terminate();
        if (g_IsService)
            g_ManagerMode = mm_stopped;
        else
            exit(0);
    }else if (g_ManagerMode == mm_reloading) 
    {
        g_ManagerMode = mm_running;
        if (g_NewConfig) {
            LoadConfig(g_NewConfig);
            g_NewConfig = 0;
        }
    }
}

/***********************************************************************/

Message *Message::s_first = new NullMessage;
Message *Message::s_last = Message::s_first;

void Message::Append(Message *m)
{
    Log(eTRACE,"Enter Message::Append");
    s_last->m_next = m;
    s_last = m;

    // Push this message to all idle servers now; active servers will get it
    // when they go idle.
    Server *idle = g_idleServers;
    Server *nextIdle = 0;
    g_idleServers = 0;
    g_idleCount = 0;

    for (Server *s = idle; s; s = nextIdle) {
        nextIdle = s->m_nextIdle;
        if (Message::Next(s))
            PushMessage(s);
        else
            AddIdleServer(s);
    }

    if (g_idleCount < g_minIdleCount)
        g_minIdleCount = g_idleCount;
}

bool Message::Next(Server *s)
{
    //Log(eTRACE,"Enter Message::Next");
    if (!s->m_lastSent) return false;

    while (Message *m = s->m_lastSent->m_next) {
        s->m_lastSent->m_refcount--;
        s->m_lastSent = m;
        m->m_refcount++;

        if (m->m_cancelled)
            continue;

        if (m->m_target && m->m_target != s)
            continue;

        return true;
    }

    return false;
}


void Message::BringUpToDate(Server *s)
{
    Log(eTRACE,"Enter Message::BringUpToDate");
    s->m_lastSent = s_last;
    s_last->m_refcount++;

#if 0
    // Send the new server all past setconfig settings.
    typedef std::map<std::string, std::string>::const_iterator iterator;
    for (iterator I = g_Settings.begin(); I != g_Settings.end(); ++I) {
        Message *m = new ConfigMessage((I->first.c_str())+1, I->second.c_str());
        m->SetTarget(s);
        Message::Append(m);
    }
#endif
    // Send the new server all past Pref settings.
    for (iter I = g_PrefSettings.begin(); I != g_PrefSettings.end(); ++I) {
        Message *m = new PrefMessage((I->first.c_str()), I->second.c_str());
        m->SetTarget(s);
        Message::Append(m);
    }
}


void Message::CancelTarget(Server *s)
{
    Log(eTRACE,"Enter Message::CancelTarget");
    if (s->m_lastSent)
    {
        s->m_lastSent->m_refcount--;
        s->m_lastSent = 0;
    }

    for (Message *m = s_first; m != s_last; m = m->m_next)
        if (m->m_target == s)
            m->m_cancelled = true;
}

void Message::DropSentMessages()
{
    Log(eTRACE,"Enter Message::DropSentMessages");
    // Drop messages that have been sent to all servers -- except for the very
    // last one sent to all servers, as servers hang on to the last message
    // they received.  And never delete all messages, even if there are
    // currently no servers.
    while (s_first->m_refcount == 0 && s_first != s_last) {
        Message *m = s_first;
        s_first = s_first->m_next;
        delete m;
    }
}

#if 0
ConfigMessage::ConfigMessage(const char *name, const char *value)
{
    Log(eTRACE,"Enter ConfigMessage::ConfigMessage");
    size_t namelen = strlen(name);
    size_t valuelen = strlen(value);
    unsigned char *p = m_buf;

    if (namelen > 255) namelen = 255;
    if (valuelen > 255) valuelen = 255;

    // Magic code to indicate config setting.
    *p++ = 2;

    // Append name.
    *p++ = (unsigned char)namelen;
    memcpy(p, name, namelen);
    p += namelen;

    // Append value.
    *p++ = (unsigned char)valuelen;
    memcpy(p, value, valuelen);

    // Length of entire message.
    m_len = unsigned((p + valuelen) - m_buf);
}


void ConfigMessage::GetBytes(Server *s, void *&data, unsigned &len)
{
    data = m_buf;
    len = m_len;
}
#endif

PrefMessage::PrefMessage(const char *name, const char *value)
{
    Log(eTRACE,"Enter PrefMessage::PrefMessage");
    size_t namelen = strlen(name);
    size_t valuelen = strlen(value);
    unsigned char *p = m_buf; 

    if (namelen > 255) namelen = 255;
    if (valuelen > 255) valuelen = 255;

    // Magic code to indicate pref setting.
    *p++ = eSET_PREF_MSG;
    unsigned char *msglen = p;
    p += 2; // pass the 2-byte msglen

    // Append name.
    *p++ = (unsigned char)(namelen + 1);
    memcpy(p, name, namelen);
    p += namelen;
    *p++ = 0; //add null

    // Append value.
    *p++ = (unsigned char)(valuelen+1);
    memcpy(p, value, valuelen);
    p += valuelen;
    *p++ = 0;

    // Length of entire message.
    m_len = unsigned(p - m_buf);
    *msglen++ = unsigned( ((m_len-3) & 0XFF00) >> 8);
    *msglen = unsigned( (m_len-3) & 0XFF);
}


void PrefMessage::GetBytes(Server *s, void *&data, unsigned &len)
{
    data = m_buf;
    len = m_len;
}

ExitMessage::ExitMessage()
{
    Log(eTRACE,"Enter ExitMessage::ExitMessage");
    // Magic code to indicate exit.
    m_buf[0] = eEXIT_MSG;
    m_buf[1] = m_buf[2] = 0;
}

void ExitMessage::GetBytes(Server *s, void *&data, unsigned &len)
{
    Log(eTRACE,"Enter ExitMessage::GetBytes");
    s->m_exiting = true;
    data = m_buf;
    len = 3;
}

static char *ScanWord(char *&p)
{
    // Skip white space to find start of new word.
    while (*p && isspace(*p))
        p++;
    char *w = p;

    // In scanning the next word, make sure we never advance p beyond the
    // end of the line.
    if (*p == '"') {
        // We have a quote-delimited word.
        char *v = p;
        bool escaped = false;
        while (*++p) {
            if (escaped) {
                *v++ = *p;
                escaped = false;
            } else {
                if (*p == '"') {
                    p++;
                    break;
                } else if (*p == '\\') {
                    escaped = true;
                } else {
                    *v++ = *p;
                }
            }
        }
        *v = 0;
    } else if (*p) {
        // We have a space-delimited word.
        while (*p && !isspace(*p))
            p++;
        if (*p)
            *p++ = 0;
    }

    return w;
}

static void SetPref(Administrator *a, const char *name, char *p, bool isStartup = true)
{
    // This line is questionable. Will p contain spaces?
    char *value = ScanWord(p);

    if (*value == 0) {
        a->Write("*** missing option and/or value\r\n");
        return;
    }

    g_PrefSettings[name] = value;

    if (!isStartup)
        Message::Append(new PrefMessage(name, value));
    a->Write("*** ok\r\n");
}

static void SlaughterAllServers(bool isExiting=false)
{
    Server *next;
	
	Log(eINFO, "Killing all Jaxer processes.");
	
    for (Server *s = g_servers; s; s = next) {
        next = s->m_next;
        SlaughterServer(s, isExiting);
    }

    // do not kill log server
    //lsi.Terminate();
}

static void DoExit(Administrator *a)
{
    SlaughterAllServers(true);
    exit(0);
}

static void DoPing(Administrator *a)
{
    g_secsSinceLastPing = 0;
    switch (g_ManagerMode) {
        case mm_stopping:
            a->Write("*** pong stopping\r\n");
            return;
        case mm_paused:
            a->Write("*** pong paused\r\n");
            return;
        case mm_reloading:
            a->Write("*** pong reloading\r\n");
            return;
        case mm_running:
            a->Write("*** pong running\r\n");
            break;
    }
}

static bool SetBooleanOption(Administrator *a, bool &opt, char *p)
{
    char *value = ScanWord(p);
    bool err = false;

    if (strcmp(value, "on") == 0)
        opt = true;
    else if (strcmp(value, "off") == 0)
        opt = false;
    else
        err = true;

    a->Write(err ? "*** bad value\r\n" : "*** ok\r\n");

    return (!err);
}

static bool SetNumericOption(Administrator *a, unsigned &opt, char *p)
{
    char *value = ScanWord(p);
    unsigned v = 0;

    for (char *p = value; *p; p++) {
        if (*p < '0' || *p > '9') {
            a->Write("*** bad numeric value\r\n");
            return false;
        } else if (v >= unsigned(-1) / 10) {
            a->Write("*** numeric value too big\r\n");
            return false;
        }

        v = (v * 10) + *p - '0';
    }

    opt = v;
    a->Write("*** ok\r\n");
    return true;
}

static void DoSetMaxProcesses(Administrator *a, char *p)
{
    if (SetNumericOption(a, g_MaxServers, p))
    {
        g_ManagerSettings["maxprocesses"] = p;

        if (g_MaxServers < g_MinServers)
        {
            g_MinServers = g_MaxServers;
            g_ManagerSettings["minprocesses"] = p;

            if (!g_startJaxerTimeoutSet)
            {
                g_startJaxerTimeout = g_SJTBase + g_MinServers * g_SJTFactor;
                if (g_startJaxerTimeout < g_SJTMin)
                    g_startJaxerTimeout = g_SJTMin;
                char tbuf[10];
                sprintf(tbuf, "%d", g_startJaxerTimeout);
                g_ManagerSettings["startjaxertimeout"] = tbuf;
            }
        }
    }
}

static void DoSetMinProcesses(Administrator *a, char *p)
{
    if(SetNumericOption(a, g_MinServers, p))
    {
        g_ManagerSettings["minprocesses"] = p;

        if (!g_startJaxerTimeoutSet)
        {
            g_startJaxerTimeout = g_SJTBase + g_MinServers * g_SJTFactor;
            if (g_startJaxerTimeout < g_SJTMin)
                g_startJaxerTimeout = g_SJTMin;
            char tbuf[10];
            sprintf(tbuf, "%d", g_startJaxerTimeout);
            g_ManagerSettings["startjaxertimeout"] = tbuf;
        }

        
        if (g_MinServers > g_MaxServers)
        {
            g_MaxServers = g_MinServers;
            g_ManagerSettings["maxprocesses"] = p;
        }
    }
}

static void DoSetWebPort(Administrator *a, char *p)
{
    if (SetNumericOption(a, g_WebPort, p))
    {
        g_ManagerSettings["webport"] = p;
    }
}

static void DoSetCommandPort(Administrator *a, char *p)
{
    if (SetNumericOption(a, g_CommandPort, p))
    {
        g_ManagerSettings["commandport"] = p;
        SetCfg(a, "commandport", p);
    }
}

static void DoSetPingTimeout(Administrator *a, char *p)
{
    if (SetNumericOption(a, g_Timeout, p))
    {
        g_ManagerSettings["timeout"] = p;
        g_ManagerSettings["pingtimeout"] = p;
    }
}

static void DoSetRequestTimeout(Administrator *a, char *p)
{
    if (SetNumericOption(a, g_RequestTimeout, p))
    {
        g_ManagerSettings["requesttimeout"] = p;
    }
}

static void DoSetRoundRobinJaxers(Administrator *a, char *p)
{
    if (SetBooleanOption(a, g_roundRobinJaxers, p))
    {
        g_ManagerSettings["roundrobinjaxers"] = p;
    }
}

static void DoSetMaxMemory(Administrator *a, char *p)
{
    if (SetNumericOption(a, g_MaxMemory, p))
    {
        g_ManagerSettings["maxmemory"] = p;
    }
}

static void DoSetMaxRequests(Administrator *a, char *p)
{
    if (SetNumericOption(a, g_MaxRequests, p))
    {
        g_ManagerSettings["maxrequests"] = p;
    }
}

static void DoSetStartJaxerTimeout(Administrator *a, char *p)
{
    if (SetNumericOption(a, g_startJaxerTimeout, p))
    {
        g_ManagerSettings["startjaxertimeout"] = p;
        g_startJaxerTimeoutSet = true;
    }
}

static void DoSetJaxerIdleTimeout(Administrator *a, char *p)
{
    if (SetNumericOption(a, g_IdleBase, p))
    {
        g_ManagerSettings["jaxeridletimeout"] = p;
        g_IdleFactor = g_IdleBase / 10;
    }
}

static void DoSetTrace(Administrator *a, char *p)
{
    bool bTraceOn = false;
    if (SetBooleanOption(a, bTraceOn, p))
    {
      if (bTraceOn)
        g_eLogLevel = eTRACE;
      else
        g_eLogLevel = eINFO;
      g_ManagerSettings["loglevel"] = gsLogLevel[g_eLogLevel];
    }
}

static void DoSetLogLevel(Administrator *a, char *p)
{
    bool bValid = true;
    char *sLevel = ScanWord(p);
    if(sLevel)
    {
      if (strcmp(sLevel, "FATAL") == 0)
          g_eLogLevel = eFATAL;
      else if (strcmp(sLevel, "ERROR") == 0)
          g_eLogLevel = eERROR;
      else if (strcmp(sLevel, "WARN") == 0)
          g_eLogLevel = eWARN;
      else if (strcmp(sLevel, "INFO") == 0)
          g_eLogLevel = eINFO;
      else if (strcmp(sLevel, "DEBUG") == 0)
          g_eLogLevel = eDEBUG;
      else if (strcmp(sLevel, "TRACE") == 0)
          g_eLogLevel = eTRACE;
      else
      {
        a->Write("*** invalid value\r\n");
        bValid = false;
      }
    }else
    {
        a->Write("*** bad option\r\n");
        bValid = false;
    }
    if (bValid)
    {
        //g_traceOn = (g_eLogLevel == eTRACE);
        g_ManagerSettings["loglevel"] = gsLogLevel[g_eLogLevel];
        a->Write("*** ok\r\n");
    }
}

static void DoSetTmpDir(Administrator *a, char *p)
{
    a->Write("*** Warn -- deprecated:  Use --cfg:tempdir=<path>n\r\n");
    char *dir = p; //ScanWord(p);

    if (dir && *dir)
    {
            g_Settings["-tempdir"] = dir;
            a->Write("*** ok\r\n");
    }else
        a->Write("*** bad option\r\n");
}

static void DoSet(Administrator *a, char *p, bool isStartup)
{
    // Commands that can be set through set <cmd> <val>
    char *opt = ScanWord(p);
    if (strncmp("pref:", opt, 5) == 0)
        SetPref(a, opt+5, p, isStartup);
    else if (strncmp("cfg:", opt, 4) == 0)
        SetCfg(a, opt+4, p);
    else if (strcmp(opt, "maxprocesses") == 0)
        DoSetMaxProcesses(a, p);
    else if (strcmp(opt, "minprocesses") == 0)
    {
        DoSetMinProcesses(a, p);
    /*else if (strcmp(opt, "webport") == 0)
        DoSetWebPort(a, p);
    else if (strcmp(opt, "commandport") == 0)
        DoSetCommandPort(a, p);
    */
    }else if (strcmp(opt, "timeout") == 0)
    {
        a->Write("*** Warn -- timeout is deprecated:  Use pingtimeout\r\n");
        DoSetPingTimeout(a, p);
    }else if (strcmp(opt, "pingtimeout") == 0)
        DoSetPingTimeout(a, p);
    else if (strcmp(opt, "requesttimeout") == 0)
        DoSetRequestTimeout(a, p);
    else if (strcmp(opt, "roundrobinjaxers") == 0)
        DoSetRoundRobinJaxers(a, p);
    else if (strcmp(opt, "maxmemory") == 0)
        DoSetMaxMemory(a, p);
    else if (strcmp(opt, "maxrequests") == 0)
        DoSetMaxRequests(a, p);
    else if (strcmp(opt, "startjaxertimeout") == 0)
        DoSetStartJaxerTimeout(a, p);
    else if (strcmp(opt, "jaxeridletimeout") == 0)
        DoSetJaxerIdleTimeout(a, p);
    else if (strcmp(opt, "trace") == 0)
        DoSetTrace(a, p);
    else if (strcmp(opt, "loglevel") == 0)
        DoSetLogLevel(a, p);
    else if (strcmp(opt, "tempdir") == 0)
        DoSetTmpDir(a, p);
    else
        a->Write("*** bad option\r\n");
}

static void DoSetConfig(Administrator *a, char *p)
{
    char *opt = ScanWord(p);

    // This line is questionable. Will p contain spaces?
    char *value = ScanWord(p);

    if (*value == 0) {
        a->Write("*** missing option and/or value\r\n");
        return;
    }

    std::string s = "-";
    s += opt;
    // Remember it, so that we can pass it along to newly started servers.
    g_Settings[s] = value;

    //Message::Append(new ConfigMessage(opt, value));
    a->Write("*** ok\r\n");
}

static void StopAllServers()
{
    Message::Append(new ExitMessage());
    for (Server *s = g_servers; s; s = s->m_next)
        if (s->m_secondsToLive == -1)
            s->m_secondsToLive = SECONDS_TO_LIVE;
}

static void DoReload(Administrator *a, char *p)
{
    switch (g_ManagerMode) {
        case mm_stopping:
            a->Write("*** cannot reload, already stopping\r\n");
            return;
        case mm_paused:
            a->Write("*** cannot reload, already paused\r\n");
            return;
        case mm_reloading:
            a->Write("*** already reloading\r\n");
            return;
        case mm_running:
            break;
    }

    char *fname = ScanWord(p);

    if (*fname) {
        g_NewConfig = fopen(fname, "r");
        if (!g_NewConfig) {
            a->Write("*** unable to read \"");
            a->Write(fname);
            a->Write("\"\r\n");
            return;
        }
    }

    StopAllServers();
    g_ManagerMode = mm_reloading;
    a->Write("*** ok\r\n");
}

static void DoRestart(Administrator *a, char *p)
{
    switch (g_ManagerMode) {
        case mm_stopping:
            a->Write("*** cannot restart, already stopping\r\n");
            return;
        case mm_paused:
            a->Write("*** cannot restart, already paused\r\n");
            return;
        case mm_reloading:
            a->Write("*** already restarting\r\n");
            return;
        case mm_running:
            break;
    }

    char *fname = ScanWord(p);

    if (*fname) {
        g_NewConfig = fopen(fname, "r");
        if (!g_NewConfig) {
            a->Write("*** unable to read \"");
            a->Write(fname);
            a->Write("\"\r\n");
            return;
        }
    }
    SlaughterAllServers();
    g_ManagerMode = mm_reloading;
    a->Write("*** ok\r\n");
}

static void DoStop(Administrator *a)
{
#ifdef _WIN32
    if (g_IsService)
    {
        onStopService(a);
        return;
    }
#endif

    Log(eINFO, "Terminating all jaxers per stop request.");
    StopAllServers();
    g_ManagerMode = mm_stopping;
    a->Write("*** ok\r\n");
    if (g_serverCount == 0) {
        a->Flush();
        lc.Terminate();
        exit(0);
    }
}

static void DoPause(Administrator *a)
{
    switch (g_ManagerMode) {
        case mm_running:
            g_ManagerMode = mm_paused;
            a->Write("*** ok\r\n");
            break;
        case mm_stopping:
            a->Write("*** cannot pause, already stopping\r\n");
            break;
        case mm_paused:
            a->Write("*** already paused\r\n");
            break;
        case mm_reloading:
            a->Write("*** cannot pause, currently reloading\r\n");
            break;
    }
}

static void DoResume(Administrator *a)
{
    if (g_ManagerMode != mm_paused) {
        a->Write("*** not paused\r\n");
        return;
    }

    g_ManagerMode = mm_running;

    // While there are waiting connections and idle servers, go unite them.
    while (g_waitingConnections > 0 && g_idleCount > 0) {
        Server *s = GetIdleServer();
        if (s)
        {
            Connection *c = g_connFirst;
            g_connFirst = g_connFirst->m_next;
            g_waitingConnections--;
            if (g_connFirst == 0)
                g_connLast = 0;
            AssignServer(s, c);
        }else
        {
            Log(eERROR, "DoResume. Failed to get idle server");
            a->Write("*** failed\r\n");
            return;
        }
    }

    a->Write("*** ok\r\n");
}

static void DoGetVersion(Administrator *a)
{
    a->Write("*** " JAXER_BUILD_ID "\r\n");
}

static void DoTestCrash(Administrator *a, char *p)
{
    //Just cause a crash for now
    int *i = reinterpret_cast<int*>(0x45);
    *i = 5;  // crash!
}

static void DoGet(Administrator *a, char *p)
{
    char *opt = ScanWord(p);
    const char *name;
    const char *value;

    if (strcmp(opt, "version") == 0)
        DoGetVersion(a);
    else
    {
        typedef std::map<std::string, std::string>::const_iterator iterator;
        iterator I;

        if (strcmp(opt, "all") == 0)
        {
            a->Write("version=" JAXER_BUILD_ID "\r\n");

            for (I = g_ManagerSettings.begin(); I != g_ManagerSettings.end(); ++I)
            {
                name = I->first.c_str();
                value = I->second.c_str();
                a->Write(I->first.c_str());
                a->Write("=");
                a->Write(I->second.c_str());
                a->Write("\r\n");
            }
            for (I = g_LogSettings.begin(); I != g_LogSettings.end(); ++I)
            {
                name = I->first.c_str();
                value = I->second.c_str();
                a->Write("log:");
                a->Write(I->first.c_str());
                a->Write("=");
                a->Write(I->second.c_str());
                a->Write("\r\n");
            }
        }else
        {
            I = g_ManagerSettings.find(opt);
            if (I != g_ManagerSettings.end())
            {
                a->Write(I->second.c_str());
                a->Write("\r\n");
            }else if (strncmp("log:", opt, 4) == 0 &&
                (I = g_LogSettings.find(opt+4)) != g_LogSettings.end())
            {
                a->Write(I->second.c_str());
                a->Write("\r\n");
            }else
            {
                a->Write("*** bad option\r\n");
            }
        }
    }
    a->Flush();
}

static void DoSetLogfileUri(Administrator *a, char *p)
{
    // The path will be in the form of a valid uri accepted by Mozilla code.
    // We do not handle all valid uri's.
    a->Write("*** Warn -- deprecated. Use --log:output=<absolute_path>\r\n");
    char *path = ScanWord(p);
    size_t n1 = strlen("resource:///");
    size_t n2 = strlen("file://");

    if (path && *path)
    {
        if (strlen(path) >= PATH_MAX)
        {
            a->Write("*** path too long.  Ignored.\r\n");
            return;
        }
        strcpy(g_JaxerLogFileUri, path);
        rawURlDecode(g_JaxerLogFileUri, (int) strlen(g_JaxerLogFileUri));

        //convert it to a local filename.
        if(strlen(g_JaxerLogFileUri)>n1 && strncmp(g_JaxerLogFileUri,"resource:///", n1) == 0)
            strcpy(g_JaxerLogFile, g_JaxerLogFileUri+n1);
        else if(strlen(g_JaxerLogFileUri)>n2 && strncmp(g_JaxerLogFileUri,"file://", n2) == 0)
        {
#ifdef _WIN32
            if (g_JaxerLogFileUri[n2] == '/') n2++;
            if (g_JaxerLogFileUri[n2+1] == '|') g_JaxerLogFileUri[n2+1] = ':';
#else
            if (g_JaxerLogFileUri[n2+1] == '/') n2++;
#endif
            strcpy(g_JaxerLogFile, g_JaxerLogFileUri+n2);
        }else
            strcpy(g_JaxerLogFile, g_JaxerLogFileUri);

        //g_ManagerSettings["jaxerlogfileuri"] = path;
        g_LogSettings["output"] = g_JaxerLogFile;

        a->Write("*** ok\r\n");
    }else
        a->Write("*** bad option\r\n");

}

static void DoSetLogfileSize(Administrator *a, char *p)
{
    a->Write("*** Warn -- deprecated:  Use --log:filesize=n\r\n");
    char *size = ScanWord(p);
    int nSize = 0;

    if (size && *size)
        nSize = atoi(size);
    if (nSize>0)
    {
        g_JaxerLogSize = nSize;
        //g_ManagerSettings["jaxerlogfilesize"] = size;
        g_LogSettings["filesize"] = size;
        a->Write("*** ok\r\n");
    }else
        a->Write("*** bad option\r\n");
}

static void DoSetLogfileBackups(Administrator *a, char *p)
{
    a->Write("*** Warn -- deprecated:  Use --log:numbackups=n\r\n");
    char *size = ScanWord(p);

    int nSize = 0;

    if (size && *size)
        nSize = atoi(size);
    if (nSize>0)
    {
        g_JaxerLogBackups = nSize;
        //g_ManagerSettings["jaxerlogfilebackups"] = size;
        g_LogSettings["numbackups"] = size;
        a->Write("*** ok\r\n");
    }else{
        a->Write("*** bad option\r\n");
	}
}


static void DoCommand(Administrator *a, bool isStartUp)
{
    // commands that can be issued after manager has started.
    char *p = a->m_line;
    *a->m_lineEnd = 0;

    char *cmd = ScanWord(p);

    if (*cmd == 0)
        ;
    else if (strcmp(cmd, "ping") == 0)
        DoPing(a);
    else if (strcmp(cmd, "exit") == 0)
        DoExit(a);
    else if (strcmp(cmd, "set") == 0)
        DoSet(a, p, isStartUp);
    else if (strcmp(cmd, "setconfig") == 0)
        DoSetConfig(a, p);
    else if (strcmp(cmd, "reload") == 0)
        DoReload(a, p);
    else if (strcmp(cmd, "restart") == 0)
        DoRestart(a, p);
    else if (strcmp(cmd, "stop") == 0)
        DoStop(a);
    else if (strcmp(cmd, "pause") == 0)
        DoPause(a);
    else if (strcmp(cmd, "resume") == 0)
        DoResume(a);
    else if (strcmp(cmd, "get") == 0)
        DoGet(a, p);
    else if (strcmp(cmd, "testcrash") == 0)
        DoTestCrash(a, p);
    else if (isStartUp && strcmp(cmd, "jaxerlogfileuri") == 0)
        DoSetLogfileUri(a, p);
    else if (isStartUp && strcmp(cmd, "jaxerlogfilesize") == 0)
        DoSetLogfileSize(a, p);
    else if (isStartUp && strcmp(cmd, "jaxerlogfilebackups") == 0)
        DoSetLogfileBackups(a, p);
    else if (isStartUp && strncmp(cmd, "log:", 4) == 0)
        Setlog(a, cmd +4, p);
    else if (isStartUp)
    {
        while (*p && isspace(*p))
            p++;
        if (! *p)
        {
            a->Write("*** Missing value\r\n");
        }else
        {
            char buf[MAX_LINE_LENGTH];
            sprintf(buf, "%s %s", cmd, p);
            DoSet(a, buf, isStartUp);
        }
    }else
        a->Write("*** Unknown command\r\n");

    a->Flush();
    a->m_lineEnd = a->m_line;
}

static void LoadConfig(FILE *fd)
{
	char *p;

    //Administrator has a fixed size of 512.
	char buffer[512];

    while (fgets(buffer, 512, fd)) 
	{
		Administrator a(INVALID_SOCKET);
		
        buffer[511] = 0;
		strcpy(a.m_line, buffer);
		a.m_lineEnd = a.m_line + strlen(a.m_line) - 1;
		
		p = strchr(a.m_line, '#');
		
        if (p != NULL)
            *p = 0;
        
        if(strlen(a.m_line) != 0)
		{
			Log(eTRACE, "Executing: %s", a.m_line);
			DoCommand(&a, true);
		}

	}
    fclose(fd);
}

static void DoPidFile(char *fname)
{
	pidFilename = fname;
}

static bool WritePidFile()
{
	if(pidFilename != NULL)
	{
	    FILE *f = fopen(pidFilename, "w");
	    if (f) {
		fprintf(f, "%d\n", getpid());
		fclose(f);
		g_ManagerSettings["pid-file"] = pidFilename;
        return true;
	    } else {
		Log(eERROR, errno, "Could not create the pid file");
	    }
	}
    return false;
}



static void ProcessEnvs()
{
    Administrator a(INVALID_SOCKET);

    // JAXER_LOGFILE.
    char* logfile = getenv("JAXER_LOGFILE");
    if (logfile)
    {
        a.Write("ENV: JAXER_LOGFILE=");
        a.Write(logfile);
        a.Write("\n");

        if (strlen(logfile) >= PATH_MAX)
        {
            a.Write("*** path too long.  Ignored.\r\n");
        }else
        {
            char s[PATH_MAX];
            sprintf(s, "\"%s\"", logfile);
            char *p = s;
            while(*p)
            {
                if (*p == '\\') *p = '/';
                p++;
            }
            DoSetLogfileUri(&a, s);
        }
    }
    //JAXER_MAXMEMORY
    char* maxmemory = getenv("JAXER_MAXMEMORY");
    if (maxmemory)
    {
        a.Write("ENV: JAXER_MAXMEMORY=");
        a.Write(maxmemory);
        a.Write("\n");

        if (SetNumericOption(&a, g_MaxMemory, maxmemory))
        {
            g_ManagerSettings["maxmemory"] = maxmemory;
        }
    }
    a.Flush();
}

static void DisplayUsage()
{
#ifdef _WIN32
    printf("\nRun as a standalone program:\n"
        "  JaxerManager [JaxerManager options]\n"
        "\nRun as a Windows Service:\n"
        "  Install Service:\n"
        "    JaxerManager -i[nstall] [JaxerManager options]\n"
        "      [JaxerManager options] take effect when the service is started.\n"
        "\n  Uninstall Service:\n"
        "    JaxerManager -u[ninstall]\n"
        "\n  Start Service:\n"
        "    JaxerManager -s[tart]\n"
        "      Or from Windows Services Control Panel.\n"
        "\n  Stop Service:\n"
        "    JaxerManager -stop\n"
        "      Or from Windows Services Control Panel.\n");

#endif
    printf("\nJaxerManager options:\n"
           "    --configfile=<file>\n"
           "    --webport=<port>\n"
           "    --commandport=<port>\n"
           "    --minprocesses=<n>\n"
           "    --maxprocesses=<n>\n"
           "    --roundrobinjaxers=<on | off>\n"
           "    --startjaxertimeout=<n>\n"
           "    --jaxeridletimeout=<n>\n"
           "    --pingtimeout=<n>\n"
           "    --maxmemory=<n>\n"
           "    --maxrequests=<n>\n"
           "    --requesttimeout=<n>\n"
           "    --loglevel=FATAL | ERROR | WARN | INFO | DEBUG | TRACE\n"
           "    --log:output=<absolute_path>\n"
           "    --log:filesize=<n>\n"
           "    --log:numbackups=<n>\n"
           "    --pref:<pref_name>=<pref_value>\n"
           "    --cfg:<name>=<value>\n");
}

static void ProcessCfgFile(int& argc, char **argv)
{
    int i;
    Administrator a(INVALID_SOCKET);
    static const char *kCfgTag = "--configfile=";
    static const size_t nCfgTag = strlen(kCfgTag);
    for (i=1; i<argc; i++)
    {
        if (strncmp(argv[i], kCfgTag, nCfgTag) == 0)
        {
            a.Write("Option: ");
            a.Write(argv[i]);
            a.Write("\n");
            char* pCfgFile = argv[i] + nCfgTag;
            if (!pCfgFile)
            {
                a.Write("*** missing value\r\n");
            }else
            {
                FILE *fp = fopen(pCfgFile, "r");
                if (!fp)
                    a.Write("*** failed to open config file\r\n");
                else
                    LoadConfig(fp);
                a.Write("*** ok\r\n");
            }

            a.Flush();

            int j;
            argc--;
            for (j=i; j<argc; j++)
                argv[j] = argv[j+1];
            break;
        }
    }
}

static void ProcessArgs(int argc, char **argv)
{
    Administrator a(INVALID_SOCKET);

    ProcessCfgFile(argc, argv);

    // Process any environment variables relevent to us first.
    ProcessEnvs();

    
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        a.Write("Option: ");
        a.Write(arg);
        a.Write("\n");

        char *value = strchr(arg, '=');
        if (value)
            *value++ = 0;

        if (!value || *value == 0) {
            if (strcmp(arg, "--help")) {
                a.Write("*** missing value\n");
                continue;
                // arg = "--help";
            }
        }
       
        if (strcmp(arg, "--configfile") == 0) {
            g_ConfigFile = value;
            g_ManagerSettings["configfile"] = value;
        } else if (strcmp(arg, "--webport") == 0)
            DoSetWebPort(&a, value);
        else if (strcmp(arg, "--commandport") == 0)
            DoSetCommandPort(&a, value);
        else if (strcmp(arg, "--minprocesses") == 0)
            DoSetMinProcesses(&a, value);
        else if (strcmp(arg, "--maxprocesses") == 0)
            DoSetMaxProcesses(&a, value);
        else if (strcmp(arg, "--timeout") == 0)
        {
            a.Write("*** Warn -- deprecated:  Use --pingtimeout=<n>\r\n");
            DoSetPingTimeout(&a, value);
        }else if (strcmp(arg, "--pingtimeout") == 0)
            DoSetPingTimeout(&a, value);
        else if (strcmp(arg, "--requesttimeout") == 0)
            DoSetRequestTimeout(&a, value);
        else if (strcmp(arg, "--roundrobinjaxers") == 0)
            DoSetRoundRobinJaxers(&a, value);
        else if (strcmp(arg, "--maxmemory") == 0)
            DoSetMaxMemory(&a, value);
        else if (strcmp(arg, "--maxrequests") == 0)
            DoSetMaxRequests(&a, value);
        else if (strcmp(arg, "--startjaxertimeout") == 0)
            DoSetStartJaxerTimeout(&a, value);
        else if (strcmp(arg, "--jaxeridletimeout") == 0)
            DoSetJaxerIdleTimeout(&a, value);
        else if (strcmp(arg, "--pid-file") == 0)
            DoPidFile(value);
        else if (strcmp(arg, "--trace") == 0)
            DoSetTrace(&a, value);
        else if (strcmp(arg, "--loglevel") == 0)
            DoSetLogLevel(&a, value);
        else if (strcmp(arg, "--jaxerlogfileuri") == 0)
            DoSetLogfileUri(&a, value);
        else if (strcmp(arg, "--jaxerlogfilesize") == 0)
            DoSetLogfileSize(&a, value);
        else if (strcmp(arg, "--jaxerlogfilebackups") == 0)
            DoSetLogfileBackups(&a, value);
        else if (strcmp(arg, "--tempdir") == 0)
            DoSetTmpDir(&a, value);
        else if (memcmp(arg, "--cfg:", 6) == 0)
            SetCfg(&a, arg +6, value);
        else if (memcmp(arg, "--pref:", 6) == 0)
            SetPref(&a, arg +7, value);
        else if (memcmp(arg, "--log:", 6) == 0)
            Setlog(&a, arg +6, value);
        else {
            a.Flush();
            
			if (strcmp(arg, "--help") != 0)
            {
                fprintf(stderr, "\nunknown option: %s\n", arg);
            }else
            {
                DisplayUsage();
                exit(0);
            }
        }
    }
    a.Flush();
}

static void PeriodicIdleServerPurge()
{
    // Simply use a idle time to kill the idle server
    // Do not kill if below min
    // Kill at most one at a time
    
    Server *s = g_idleServers;
    while(s)
    {
        s->m_idleTime++;
        s = s->m_nextIdle;
    }
    
    // If we are already at the minimum, no kill
    // If no idle server, no kill

    if (g_serverCount <= g_MinServers || g_minIdleCount < 1) 
    {
        g_minIdleCount = g_idleCount;
        return;
    }

    unsigned nidle = g_minIdleCount;
    if (nidle > g_serverCount - g_MinServers)
        nidle = g_serverCount - g_MinServers;

    unsigned idleLimit = g_IdleBase + g_IdleFactor * (g_MaxServers - g_MinServers - nidle);
    Log(eTRACE, "PeriodicIdleServerPurge: gs=%d gmin=%d gi=%d nIdle=%d, IdleLimit=%d",
      g_serverCount, g_MinServers, g_minIdleCount, nidle, idleLimit);

    if (g_idleServers->m_idleTime > idleLimit)
    {
        // Kill it
        ExitMessage *em = new ExitMessage;
        em->SetTarget(g_idleServers);
        g_idleServers->m_secondsToLive = SECONDS_TO_LIVE;
        Log(eDEBUG, " Terminate extra idle server (after idling %d seconds): svr=%d. pid given in next message",
            g_idleServers->m_idleTime, g_idleServers);
        Log(eINFO, " Terminate extra idle server: pid=%d", g_idleServers->m_pid);
        Message::Append(em);
    }

    // Reset for next interval.
    g_minIdleCount = g_idleCount;
}

static void DoPeriodic()
{	
    if (g_ManagerMode == mm_stopping || g_ManagerMode == mm_stopped)
        return;

    g_elapsedTimeServerBelowMin++;

    // If we go too long without a ping, exit.
    if (g_Timeout > 0) {
        if (g_secsSinceLastPing++ >= g_Timeout) {
            Administrator a(INVALID_SOCKET);
            Log(eINFO, "Timeout reached without ping, exiting.");
			DoStop(&a);
        }
    } else {
        g_secsSinceLastPing = 0;
    }

    Message::DropSentMessages();

    // Look for servers that were told to exit, but haven't done so fast
    // enough.  Slaughter them.  Also look for servers that are taking too
    // long to process a request.  Slaughter them too.
	//
	// On Mac, also look for processes that are taking up too much memory
    //
	Server *next;
    for (Server *s = g_servers; s; s = next) {
        next = s->m_next;
        if (s->m_secondsToLive > 0) {
            if (--s->m_secondsToLive == 0) {
                Log(eERROR, "Jaxer process (pid: %d) taking too long to die, killing it", s->m_pid);
                SlaughterServer(s);
                continue;
            }
        }
        
        if (g_RequestTimeout > 0 && s->m_boundConnection) {
            if (s->m_secsSinceRequestStart++ >= (g_RequestTimeout + s->m_privateSettings.m_requestTimeoutExtra)) {
                Log(eERROR, "Jaxer process (pid: %d) taking too long to handle request, killing it", s->m_pid);
                SlaughterServer(s);
                continue;
            }
        }
		
#if __APPLE__
		FILE *fp;
		char readBuffer[MAX_LINE_LENGTH];

		if(psCommandFailed == 0)
		{
			sprintf(readBuffer, "/bin/ps -o rss -p %d", s->m_pid);
			fp = popen(readBuffer, "r");
			if (fp == NULL)
			{
				Log(eERROR, errno, "Could not open /bin/ps to determine memory usage");
				psCommandFailed = 1;
			}
			else
			{
				// Read in the first line, which should be 'RSS'
				if(fgets(readBuffer, MAX_LINE_LENGTH, fp) != NULL)
				{
					// Now read in the actual memory usage
					if(fgets(readBuffer, MAX_LINE_LENGTH, fp) != NULL)
					{
						long memUsed = strtol(readBuffer, (char **)NULL, 10);
						
						if(memUsed == 0)
						{
							Log(eERROR, errno, "Could not determine the memory used in string '%s'", readBuffer);
						}
						else
						{
							if(memUsed > (long)(g_MaxMemory * 1024))
							{
								Log(eERROR, "Jaxer process (pid: %d) is consuming too much memory(%d), killing it", s->m_pid, memUsed);
								SlaughterServer(s);
							}
						}
					}
				}
				
				if(pclose(fp) == -1)
				{
					//LogEntry("Could not close /bin/ps process.", errno);
				}
			}
		}
#endif

    }

    // Don't cull idle servers if paused, otherwise there won't be enough
    // when resumed.
    if (g_ManagerMode == mm_paused)
        return;

    // See if we are below watermark for too long
    if (g_serverCount>=g_MinServers)
    {
        //reset time count
        g_elapsedTimeServerBelowMin = 0;
    }else if (g_startJaxerTimeout>0 && g_elapsedTimeServerBelowMin>g_startJaxerTimeout)
    {
        //Manager has trouble in starting Jaxers.  Terminate
        Log(eFATAL, "Manager fails to start jaxers within specified time (%d seconds).  Exiting", g_startJaxerTimeout);
        Administrator a(INVALID_SOCKET);
        DoStop(&a);
        return;
    }

    PeriodicIdleServerPurge();
#if 0
    // See if too many servers have been idle during this last interval.
    // First, determine max number of servers we'll kill.
    unsigned toKill = (g_minIdleCount + 49) / 50;

    // If this will take us below the minimum number, back off.
    if (g_serverCount <= g_MinServers)
        toKill = 0;
    else if (g_serverCount - g_MinServers < toKill)
        toKill = g_serverCount - g_MinServers;

    Log(eTRACE, "DoPeriodic: gs=%d gmin=%d gi=%d kill=%d",
      g_serverCount, g_MinServers, g_minIdleCount, toKill);
    // Kill them.
    while (toKill > 0) {
        ExitMessage *em = new ExitMessage;
        em->SetTarget(g_idleServers);
        g_idleServers->m_secondsToLive = SECONDS_TO_LIVE;
        Log(eDEBUG, " Terminate extra idle server: svr=%d. pid given in next message", g_idleServers);
        Log(eINFO, " Terminate extra idle server: pid=%d", g_idleServers->m_pid);
        Message::Append(em);
        toKill--;
    }

    // Reset for next interval.
    g_minIdleCount = g_idleCount;
#endif
}

#ifdef _WIN32

static void LogEntry(FILE *stream, LPCTSTR logLevel, LPCTSTR text)
{
    static char *buf = 0;
    static int buf_len = 0;
    static int nExtra = 150;
    int ndata = (int) strlen(text)*sizeof(TCHAR);
    if (buf_len < ndata+nExtra)
    {
        delete[] buf;
        buf = new char[ndata+nExtra];
        if (!buf)
        {
            printf("out of memory\n"); fflush(stdout);
            buf_len = 0;
            return;
        }
        buf_len = ndata+nExtra;
    }

	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

    //static const int DATE_LEN = 20;
    //TCHAR dateStr[DATE_LEN+100];
    int nWritten =sprintf(buf,
                    "%02d:%02d:%02d %02d/%02d/%04d [%6d] [%s] [JaxerManager] ",
                    timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
                    timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_year+1900,
                    getpid(), logLevel);
    
    memcpy(buf+nWritten, text, ndata);
    nWritten += ndata;
    buf[nWritten++] = '\r';
    buf[nWritten++] = '\n';
    buf[nWritten] =0;

    lc.WriteLog(buf, nWritten);


    // Do not double log trace unless it cannot be written to logfile.
    if (strcmp(logLevel,"TRACE") && strcmp(logLevel, "DEBUG"))
    {
	    fprintf(stream, "%s\n", text);
	    fflush(stream);
    }
}

static void LogEntry(FILE *stream, LPCTSTR logLevel, LPCTSTR text, int errorNumber)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = errorNumber; 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)text)+50)*sizeof(TCHAR)); 

    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s: failed with error %d: %s"), 
        text, dw, lpMsgBuf); 
    
	LogEntry(stream, logLevel, (LPCTSTR)lpDisplayBuf);
	
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

#else



static void LogEntry(FILE *stream, char *logLevel, char *text)
{
    static char *buf = 0;
    static int buf_len = 0;
    static int nExtra = 150;
    int ndata = strlen(text);
    if (buf_len < ndata+nExtra)
    {
        delete[] buf;
        buf = new char[ndata+nExtra];
        if (!buf)
        {
            printf("out of memory\n"); fflush(stdout);
            buf_len = 0;
            return;
        }
        buf_len = ndata+nExtra;
    }


    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
  
    int nWritten = sprintf(buf,
        "%02d:%02d:%02d %02d/%02d/%04d [%6d] [%s] [JaxerManager] %s\n",
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
        timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_year+1900,
        getpid(), logLevel, text);

lc.WriteLog(buf, nWritten);


    if (strcmp(logLevel,"TRACE") && strcmp(logLevel, "DEBUG"))
    {
	    fprintf(stream, "%s\n", text);
	    fflush(stream);
    }
}

static void LogEntry(FILE *stream, char *logLevel, char *text, int errorNumber)
{
	char buf[1024];
	sprintf(buf, "%s: error %d: %s", text, errorNumber, strerror(errorNumber));
	LogEntry(stream, logLevel, buf);
}

#endif

static void LogTrace(const char* text, int len)
{
    if (g_eLogLevel == eTRACE)
    {
//TMP SOLARIS PROBLEM
        char largebuf[1024];
        if (len >=1024) len = 1023;
        strncpy(largebuf, text, len);
        largebuf[len] = '\0';
        LogEntry(stdout, "TRACE", largebuf);
#if 0
        char t = text[len];
        text[len] = 0;
        LogEntry(stdout, "TRACE", text);
        text[len] = t;
#endif
    }
}

void InitSettings()
{
    // Put the default settings into g_Settings & g_ManagerSettings first

    //TODO: strings do not need to be copied

    char buf[256];
    sprintf(buf, "%d", g_CommandPort);
    g_Settings["-commandport"] = buf;
    g_ManagerSettings["commandport"] = buf;

    sprintf(buf, "%d", g_MaxServers);
    g_ManagerSettings["maxprocesses"] = buf;

    sprintf(buf, "%d", g_MinServers);
    g_ManagerSettings["minprocesses"] = buf;

    sprintf(buf, "%d", g_startJaxerTimeout);
    g_ManagerSettings["startjaxertimeout"] = buf;

    sprintf(buf, "%d", g_IdleBase);
    g_ManagerSettings["jaxeridletimeout"] = buf;

    // configfile
    
    sprintf(buf, "%d", g_WebPort);
    g_ManagerSettings["webport"] = buf;
    

    sprintf(buf, "%d", g_Timeout);
    g_ManagerSettings["timeout"] = buf;
    g_ManagerSettings["pingtimeout"] = buf;

    sprintf(buf, "%d", g_MaxMemory);
    g_ManagerSettings["maxmemory"] = buf;

    sprintf(buf, "%d", g_MaxRequests);
    g_ManagerSettings["maxrequests"] = buf;

    sprintf(buf, "%d", g_RequestTimeout);
    g_ManagerSettings["requesttimeout"] = buf;

    g_ManagerSettings["roundrobinjaxers"] = g_roundRobinJaxers ? "on" : "off";

    sprintf(buf, "%d", g_JaxerLogSize);
    g_LogSettings["filesize"] = buf;

    sprintf(buf, "%d", g_JaxerLogBackups);
    g_LogSettings["numbackups"] = buf;
    g_LogSettings["output"] = g_JaxerLogFile;


    g_ManagerSettings["loglevel"] = gsLogLevel[g_eLogLevel];
}

void AcceptStdinCommand()
{
    char cmdBuf[MAX_LINE_LENGTH];
    while( gets(cmdBuf))
    {
        Administrator a(INVALID_SOCKET);
        strcpy(a.m_line, cmdBuf);
		a.m_lineEnd = a.m_line + strlen(a.m_line);
        DoCommand(&a, false);
    }
}

#if 0
// Simple function for debugging.
void logme(char* fmt, ...)
{
  char buf[1024];
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  vsprintf(buf, fmt, arg_ptr);
  va_end(arg_ptr);

  time_t rawtime;
  struct tm* tinfo;
  time( &rawtime);
  tinfo = localtime(&rawtime);
  FILE *fp = fopen("/opt/AptanaJaxer/logs/tmp.log", "a");
  fprintf(fp, "%02d:%02d:%02d %s\n",
    tinfo->tm_hour, tinfo->tm_min, tinfo->tm_sec, buf);
  fflush(fp);
  fclose(fp);
}
#endif

static int log_helper(char *str, size_t size, const char *format, va_list args)
{
	int r;
	if (size == 0)
		return -1;
#ifdef _WIN32
	r = _vsnprintf(str, size, format, args);
#else
	r = vsnprintf(str, size, format, args);
#endif
	str[size-1] = '\0';
	if (r < 0 || ((size_t)r) >= size) {
		/* different platforms behave differently on overflow;
		 * handle both kinds. */
		return -1;
	}
	return r;
}

void Log(eLogLevel eLevel, char* fmt, ...)
{
  if (eLevel < g_eLogLevel) return;

  char buf[0x8000];
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  log_helper(buf, 0x8000, fmt, arg_ptr);
  // vsprintf(buf, fmt, arg_ptr);
  va_end(arg_ptr);

  LogEntry((eLevel==eERROR ||eLevel==eFATAL) ? stderr:stdout, gsLogLevel[eLevel], buf);
}

void Log(eLogLevel eLevel, int nError,  char* fmt, ...)
{
  if (eLevel < g_eLogLevel) return;

  char buf[0x8000];
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  log_helper(buf, 0x8000, fmt, arg_ptr);
  //vsprintf(buf, fmt, arg_ptr);
  va_end(arg_ptr);

  LogEntry((eLevel==eERROR ||eLevel==eFATAL) ? stderr:stdout, gsLogLevel[eLevel], buf, nError);
}

/*
 * Fill the buf with information to make buf a valid CMD response.
 * return the data length.
 * Caller has allocated space for buf.
 */
static int BuildCmdResponse(char* buf, const char* data, int datalen, bool hasMore)
{
    // buf has all the space allocated
    // Process the request
    // [0]  -eCMD_RESP
    // [12] -len
    // [3]  - 1
    // [4]  - 1 or 0
    // [5*] -data

    if (datalen == -1)
        datalen = (int) strlen(data);

    int len = datalen+2;
    buf[0] = eCMD_RESP_MSG;
    buf[1] = unsigned((len & 0XFF00) >> 8);
    buf[2] = unsigned(len & 0XFF);
    
    buf[3] = 1; // OK
    buf[4] = hasMore ? 1 : 0; // no more
    memcpy(buf+5, data, datalen);
    
    return len + 3; // length of the entire data
}

/*
 * Construct a error response msg in buf.  Return the length of the msg.
 */
static int BuildErrorResponse(char* buf, int errorCode, const char* errorText)
{
    // buf has all the space allocated
    // Process the request
    // [0]  -eCMD_RESP
    // [12] -len
    // [3]  - 0
    // [45]  - error code
    // [67] -errorText length
    // [8*] -errorText

    int len = (int) strlen(errorText);
    buf[0] = eCMD_RESP_MSG;
    buf[1] = unsigned(((len+5) & 0XFF00) >> 8);
    buf[2] = unsigned((len+5) & 0XFF);
    
    buf[3] = 0; // !OK
    buf[4] = unsigned((errorCode & 0XFF00) >> 8);
    buf[5] = unsigned(errorCode & 0XFF);

    buf[6] = unsigned((len & 0XFF00) >> 8);
    buf[7] = unsigned(len & 0XFF);

    sprintf(buf+8, errorText);
    
    fprintf(stderr, "ERRORTEXT=%s\n", errorText);
    return len + 8; // length of the entire data
}

VOID ProcessNoRespCmd (Server* s, char* cmd)
{
    unsigned char* p = (unsigned char*) cmd;

    DWORD dwLen = p[1] << 8 | p[2];
    cmd += 3;
    Log(eDEBUG, "Executing NoRespCmd %s from server %d", cmd, s->m_pid);

    char *opt = ScanWord(cmd);
    if (strcmp("test", opt) == 0)
    {
        Log(eDEBUG, "Executing NoRespCmd test from server %d", s->m_pid);
    }else if (strcmp("set", opt) == 0)
    {
        char tmpbuf[CMDBUFSIZE];
        Administrator a(INVALID_SOCKET, tmpbuf);
        DoSet(&a, cmd, false);
        a.Flush();
        Log(eDEBUG, "Results from executing NoRespCmd set %s from server %d: %s.", cmd, s->m_pid, tmpbuf);
    }else if (strcmp("setme", opt) == 0)
    {
        char tmpbuf[CMDBUFSIZE];
        Administrator a(INVALID_SOCKET, tmpbuf);
        s->m_privateSettings.DoSet(&a, cmd);
        a.Flush();
        Log(eDEBUG, "Results from executing NoRespCmd setme %s from server %d: %s.", cmd, s->m_pid, tmpbuf);
    }else
    {
        Log(eINFO, "[NOT IMPLEMENTED] Executing NoRespCmd %s %s from server %d", opt, cmd, s->m_pid);
    }
}

int ServerSettings::DoSet(Administrator *a, char* cmd)
{
    //execute the cmd and print out any messages to output.
    // return 0 on success.
    char *name = ScanWord(cmd);
    if (strcmp(name, "requesttimeoutextra") == 0)
    {
        if (!SetNumericOption(a, m_requestTimeoutExtra, cmd))
            return 1;
            
    }else if (strcmp(name, "maxrequests") == 0)
    {
        if (!SetNumericOption(a, m_maxRequests, cmd))
            return 1;
    }else
    {
        a->Write("*** bad cmd\r\n");
        return 2;
    }
    return 0;
}

int ServerSettings::DoGet(Administrator *a, char* cmd)
{
    //execute the cmd and print out any messages to output.
    // return 0 on success.
    char *name = ScanWord(cmd);
    char buf[100];
    if (strcmp(name, "requesttimeoutextra") == 0)
    {
        sprintf(buf, "%d\r\n", m_requestTimeoutExtra);
            
    }else if (strcmp(name, "maxrequests") == 0)
    {
        sprintf(buf, "%d\r\n", m_maxRequests);
    }else if (strcmp(name, "all") == 0)
    {
        sprintf(buf, "requesttimeoutextra=%d\r\n"
            "maxrequests=%d\r\n",
            m_requestTimeoutExtra,
            m_maxRequests);
    }else
    {
        a->Write("*** bad cmd\r\n");
        return 2;
    }
    a->Write(buf);
    return 0;
}