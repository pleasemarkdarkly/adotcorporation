//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
// 
//  File:       U T I L . C P P
//
//  Contents: Utility functions used by updiag. 
//
//----------------------------------------------------------------------------

#include "stdafx.h"
//#include <windows.h>
//#include <stdlib.h>
//#include <tchar.h>
//#include <stdio.h>
#ifndef UNDER_CE 
#include <process.h>
#endif

#include "global.h"
#include "util.h"
#include "udevice.h"

// The following const strings are used for creating Date and Time strings.
const char wkday[7][4] =
{
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"
};

const char month[12][4] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

const char HttpDate[] = "%3.3s, %2.2d %3.3s %4.4d %2.2d:%2.2d:%2.2d GMT";


// The following functions are character manipulation functions.

//////////
// t2an
//      Converts a TCHAR string to an ANSI string.
//
// PARAMETERS
//      tStr:   TCHAR input string to convert.
//      aStr:   ANSI output string (must be cch+1 bytes or more).
//      cch:    Character count of tStr. 
//
// RETURNS
//      BOOL    TRUE.
//
BOOL t2an(LPCTSTR tStr, LPSTR aStr, UINT cch) 
{
    int idx = 0;
    while(cch--) 
    {
        aStr[idx] = (CHAR)tStr[idx];
        if (aStr[idx] == NULL)
            break;
        idx++;
    } 
    return TRUE;
} // end t2an


//////////
// a2tn
//      Converts an ANSI string to a TCHAR string.
//
// PARAMETERS
//      aStr:   ANSI input string to convert.
//      tStr:   TCHAR output string (must be cch+1 TCHAR or more).
//      cch:    Character count of aStr.
//
// RETURNS
//      BOOL    TRUE.
//
BOOL a2tn(LPCSTR aStr, LPTSTR tStr, UINT cch)
{
    int idx = 0;
    while(cch--) 
    {
        tStr[idx] = (TCHAR)aStr[idx];
        if (tStr[idx] == NULL) 
            break;
        idx++;
    } 
    return TRUE;
} // end a2tn


//////////
// w2t
//      Converts a wide string to a TCHAR string.
//
// PARAMETERS
//      wStr:   Wide char input string to convert.
//      tStr:   TCHAR output string (must be as large as wStr).
//
// RETURNS
//      BOOL    TRUE.
//
BOOL w2t(LPCWSTR wStr, LPTSTR tStr) 
{
    int idx = 0;
    while(1) 
    {
        tStr[idx] = (TCHAR)wStr[idx];
        if (tStr[idx] == NULL) 
            break;
        idx++;
    } 
    return TRUE;
} // end w2t


//////////
// t2w
//      Converts a TCHAR string to a wide string.
//
// PARAMETERS
//      tStr:   TCHAR input string to convert.
//      wStr:   Wide char output string (must be as large as tStr*2).
//
// RETURNS
//      BOOL    TRUE.
//
BOOL t2w(LPCTSTR tStr, LPWSTR wStr) 
{
    int idx = 0;
    while(1) 
    {
        wStr[idx] = (WCHAR)tStr[idx];
        if (wStr[idx] == NULL) 
            break;
        idx++;
    } 
    return TRUE;
} // end t2w


//////////
// a2t
//      Converts an ANSI string to a TCHAR string.
//
// PARAMETERS
//      aStr:   ANSI input string to convert.
//      tStr:   TCHAR output string (must be length of aStr + 1 * sizeof(TCHAR)).
//
// RETURNS
//      BOOL    TRUE.
//
BOOL a2t(LPCSTR aStr, LPTSTR tStr) 
{
    int idx = 0;
    while(1) 
    {
        tStr[idx] = (TCHAR)aStr[idx];
        if (tStr[idx] == NULL) 
            break;
        idx++;
    } 
    return TRUE;
} // end a2t


//////////
// t2a
//      Converts a TCHAR string to an ANSI string.
//
// PARAMETERS
//      tStr:   TCHAR input string to convert.
//      aStr:   ANSI output string (must be length of tStr + 1 * sizeof(char)).
//
// RETURNS
//      BOOL    TRUE.
//
BOOL t2a(LPCTSTR tStr, LPSTR aStr) 
{
    int idx = 0;
    while(1) 
    {
        aStr[idx] = (CHAR)tStr[idx];
        if (aStr[idx] == NULL) 
            break;
        idx++;
    } 
    return TRUE;
} // end t2a


//////////
// w2a
//      Converts a wide char string to an ANSI string.
//
// PARAMETERS
//      wStr:   Wide char input string to convert.
//      aStr:   ANSI output string (must be length of wStr + 1 * sizeof(char))
//
// RETURNS
//      BOOL    TRUE.
//
BOOL w2a(LPCWSTR wStr, LPSTR aStr) 
{
    int idx = 0;
    while(1) 
    {
        aStr[idx] = (CHAR)wStr[idx];
        if (aStr[idx] == NULL) 
            break;
        idx++;
    } 
    return TRUE;
} // end w2a


//////////
// a2w
//      Converts an ANSI string to a wide char string.
//
// PARAMETERS
//      aStr:   ANSI input string to convert.
//      wStr:   wide char output string (must be length of aStr + 1 * 2).
//
// RETURNS
//      BOOL    TRUE
//
BOOL a2w(LPCSTR aStr, LPWSTR wStr) 
{
    int idx = 0;
    while (1) 
    {
        wStr[idx] = (WCHAR)aStr[idx];
        if (wStr[idx] == NULL) 
            break;
        idx++;
    }
    return TRUE;
} // end a2w


//////////
// MemAlloc
//      Memory allocation routine.
//
// PARAMETERS
//      cb:   Number of bytes to allocate.
//
// RETURNS
//      VOID*   Pointer to the newly allocated memory.
//
VOID* MemAlloc(size_t cb)
{
    return (malloc(cb));
} // end  MemAlloc


//////////
// MemFree
//      Memory free routine.
//
// PARAMETERS
//      pv:   Pointer to a memory block to free.  The memory block
//            was allocated using MemAlloc.
//
// RETURNS
//      Nothing.
//
VOID MemFree(VOID* pv)
{
    free(pv);
} // end MemFree


//////////
// GetDateTime
//      Get the current system time and format it according to HTTP rules.
//
// PARAMETERS
//      szDateTime:   Pointer to a buffer where the date string will be written.
//
// RETURNS
//      Nothing.
//
VOID GetDateTime(CHAR* szDateTime)
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    sprintf(szDateTime, HttpDate, 
        &wkday[st.wDayOfWeek][0],
        st.wDay,
        &month[st.wMonth - 1][0],
        st.wYear,
        st.wHour,
        st.wMinute,
        st.wSecond);
} // end GetDateTime


//////////
// GetUniqueId
//      Get a unique string. The ID is typically returned to subscribers.
//      Builds unique string by obtaining the current time and appending
//      the process ID and a counter of how many times this function
//      has been called since the process started.
//
// PARAMETERS
//      szUniqueId:  Pointer to a buffer where the unique ID will be written.
//      size:        Size of the buffer.
//
// RETURNS
//      BOOL    TRUE if it succeeds.
//              FALSE if the buffer is too small.
//

// Set the initial counter value.
static DWORD g_IdIncr = 1;
BOOL GetUniqueId(CHAR* szUniqueId, unsigned int size)
{
    if (size < 8)
    {
        return FALSE;
    }
    // Increment the counter.
    g_IdIncr++;

    int pid = 0;

// Get the process ID based on the platform.
#ifndef UNDER_CE 
	pid = _getpid();
#else
	pid = (int) GetCurrentProcessId();
#endif

    // Get the time.
    DWORD tick = GetTickCount();
    if (size < 16)
    {
        sprintf(szUniqueId, "%8.8x", tick);
    }
    else if (size < 24)
    {
        sprintf(szUniqueId, "%8.8x_%x", tick, pid);
    }
    else
    {
        sprintf(szUniqueId, "%8.8x_%x_%x", tick, pid, g_IdIncr);
    }
    
    return TRUE;
} // end GetUniqueId


//////////
// GetServerString
//      Build the SERVER HTTP header string, based on the OS version and product.
//
// PARAMETERS
//      szServer:    Pointer to a buffer where the server string will be written.
//      size:        Size of the buffer.
//
// RETURNS
//      Nothing.
//
VOID GetServerString(CHAR* szServer, unsigned int size)
{
    CHAR szWin95[] = "Windows";
    CHAR szWinNT[] = "Windows NT";
    CHAR szWinCE[] = "Windows CE";
    CHAR szUPNP[] = " UPnP/1.0 ";
    unsigned int nVerSize = 7;     // Approximate size of version string nnn.mmm.
    OSVERSIONINFO VerInfo;
    CHAR* szWin = NULL;

    // Get the OS version information.
    VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (GetVersionEx(&VerInfo))
    {
        switch (VerInfo.dwPlatformId)
        {
        case VER_PLATFORM_WIN32_NT:
            szWin = szWinNT;
            break;

        case VER_PLATFORM_WIN32_WINDOWS:
            szWin = szWin95;
            break;

#ifdef UNDER_CE 
        case VER_PLATFORM_WIN32_CE:
            szWin = szWinCE;
            break;
#endif
        default:
            // Not likely to be on Win95, but account for anyway.
            szWin = szWin95;
            VerInfo.dwMajorVersion = 5;     // Make a guess.
            VerInfo.dwMinorVersion = 0;
            break;
        }

    }
    else
    {
        szWin = szWin95;
        VerInfo.dwMajorVersion = 5;     // Make a guess.
        VerInfo.dwMinorVersion = 0;
    }

    if (size > sizeof(szWinNT) + nVerSize + sizeof(szUPNP) + 
                sizeof(c_szProduct) + sizeof(c_szProductVersion) + 3)
    {
        // Full string.
        sprintf(szServer, "%s/%d.%d%s%s/%s",
                            szWin, 
                            VerInfo.dwMajorVersion,
                            VerInfo.dwMinorVersion,
                            szUPNP,
                            c_szProduct,
                            c_szProductVersion);
    }
    else if (size > sizeof(szWinNT) + nVerSize + sizeof(szUPNP) + 2)
    {
        // Buffer too small; try shorter string.
        sprintf(szServer, "%s/%d.%d%s",
                            szWin, 
                            VerInfo.dwMajorVersion,
                            VerInfo.dwMinorVersion,
                            szUPNP);
    }
    else
    {
        // Really too small; empty string.
        *szServer = 0;
    }

} // end GetServerString


