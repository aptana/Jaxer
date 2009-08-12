#ifndef _BLOCK_DUMP_H__
#define _BLOCK_DUMP_H__

#include "prprf.h"
#include "aptCoreTrace.h"
//#include "FCGXStream.h"

extern aptCoreTrace gJaxerLog;

// Debug protocol dumping code
#ifndef NDEBUG_XXX

class BlockDumper
{
    char    mBuf[128];
    char   *mNextHex;
    char   *mNextChar;
    int     mOffset;
    char    mDir[8];

public:
    BlockDumper(const char *dir)
        : mNextHex(0), mNextChar(0), mOffset(0)
    {
        strcpy(mDir, dir);
    }

    void Flush(bool bResetOffset=false)
    {
        if (!gJaxerLog.GetDumpEnabled())
            return;

        if (mNextChar) {
            gJaxerLog.LogBytes(eNOTICE, mBuf, mNextChar - mBuf);
            mNextChar = mNextHex = 0;
            if (bResetOffset)
                mOffset = 0;
        }
    }

    void LogHeader(BlockType bt, int len = -1)
    {
        if (!gJaxerLog.GetDumpEnabled())
            return;

        Flush();
        PRInt32 n;
        if (len >= 0)
            n = PR_snprintf(mBuf, 128, "%s block %s (%d bytes)", mDir, g_BlockTypes[bt], len);
        else
            n = PR_snprintf(mBuf, 128, "%s block %s", mDir, g_BlockTypes[bt]);
        gJaxerLog.LogBytes(eNOTICE, mBuf, n);
        mNextHex = mNextChar = 0;
        mOffset = 0;
    }

    void LogContents(PRUint8 *p, int len, bool reset = false)
    {
        if (!gJaxerLog.GetDumpEnabled())
            return;

        if (reset)
            mOffset = 0;

        while (len > 0) {
            len--;
            int offset = mOffset & 15;
            if (offset == 0) {
                PR_snprintf(mBuf, 128, "%s %4.4x: ", mDir, mOffset);
                mNextHex = mBuf + 11;
                memset(mNextHex, ' ', 3 * 16);
                mNextChar = mNextHex + 3 * 16;
            }

            const char *hex = "0123456789abcdef";
            *mNextHex++ = hex[*p >> 4];
            *mNextHex++ = hex[*p & 0xf];
            mNextHex++;

            int c = *p++;
            *mNextChar++ = (c >= 32 && c <= 127) ? char(c) : ' ';

            mOffset++;
            if (offset == 15)
                Flush();
        }

        Flush(true);
    }

    void LogProtocolError(PRUint8 *p, int len)
    {
        gJaxerLog.Log(eNOTICE, "Protocol error -- bytes at point of error:");
        if (len == 0)
            gJaxerLog.Log(eNOTICE, "   no bytes available");
        else
            LogContents(p, len, true);
        Flush();
    }
};

#else

class BlockDumper
{
public:
    BlockDumper(const char *dir) { }
    void Flush() { }
    void LogHeader(BlockType bt, int len = -1) { }
    void LogContents(PRUint8 *p, int len) { }
    void LogProtocolError(PRUint8 *p, int len) { }
};

#endif

#endif // _BLOCK_DUMP_H__
