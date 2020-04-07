/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// regkey.h - Win32 Registry API wrapper
//
// Author: Matthew Tadd		mtadd@iobjects.com
//
// Copyright (c) 1999-2001 Interactive Objects, Inc. All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Interactive Objects, Inc.
// is strictly prohibited.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifndef _REGKEY_H_
#define _REGKEY_H_
#include <windows.h>

class CRegKey
{
public:
	CRegKey();
	~CRegKey();

// Attributes
public:
	operator HKEY() const;
	HKEY m_hKey;

// Operations
public:
	LONG SetValue(DWORD dwValue, LPCTSTR lpszValueName);
	LONG QueryValue(DWORD& dwValue, LPCTSTR lpszValueName);
	LONG QueryValue(LPTSTR szValue, LPCTSTR lpszValueName, DWORD* pdwCount);
	LONG SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL);

	LONG SetKeyValue(LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL);
	static LONG WINAPI SetValue(HKEY hKeyParent, LPCTSTR lpszKeyName,
		LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL);

	LONG Create(HKEY hKeyParent, LPCTSTR lpszKeyName,
		LPTSTR lpszClass = REG_NONE, DWORD dwOptions = REG_OPTION_NON_VOLATILE,
		REGSAM samDesired = KEY_ALL_ACCESS,
		LPSECURITY_ATTRIBUTES lpSecAttr = NULL,
		LPDWORD lpdwDisposition = NULL);

	LONG Open(HKEY hKeyParent, LPCTSTR lpszKeyName,
		REGSAM samDesired = KEY_ALL_ACCESS);

	LONG Close();
	HKEY Detach();
	void Attach(HKEY hKey);
	LONG DeleteSubKey(LPCTSTR lpszSubKey);
	LONG RecurseDeleteKey(LPCTSTR lpszKey);
	LONG DeleteValue(LPCTSTR lpszValue);
};

#endif