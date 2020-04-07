//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
// 
//  File:       U T I L . H
//
//  Contents: Utility functions used by updiag. 
//
//----------------------------------------------------------------------------

#ifndef _UTIL_H
#define _UTIL_H

#include <tchar.h>


inline BOOL IsValidHandle(HANDLE h) 
{
    return (h && INVALID_HANDLE_VALUE != h);
}

// Helper functions for conversion.
BOOL w2t(LPCWSTR wStr, LPTSTR tStr);           // Converts wide characters to TCHARs.      
BOOL a2t(LPCSTR  aStr, LPTSTR tStr);           // Converts ASCII characters to TCHARs.
BOOL t2a(LPCTSTR tStr, LPSTR aStr);            // Converts TCHARs to ASCII characters
BOOL t2w(LPCTSTR tStr, LPWSTR wStr);           // Converts TCHARs to wide characters.
BOOL w2a(LPCWSTR wStr, LPSTR  aStr);           // Converts wide characters to ASCII characters.
BOOL a2w(LPCSTR aStr, LPWSTR wStr);            // Converts ASCII characters to wide characters.
BOOL t2an(LPCTSTR tStr, LPSTR aStr, UINT cch); // Converts TCHARs to ASCII characters, using length
BOOL a2tn(LPCSTR aStr, LPTSTR tStr, UINT cch); // Converts ASCII characters to TCHARs, using length


#endif // _UTIL_H


