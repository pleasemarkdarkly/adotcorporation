/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// regpropertybag.cpp - Abstraction for a Win32 Registry Key
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
#include <tchar.h>
#include "diag.h"
#include "RegPropertyBag.h"
#include "DataTypes.h"

#ifdef _WIN32_WCE_AUTOPC
extern "C" {
	void NKDbgPrintfW(LPWSTR lpszFmt,...);
}
#define DEBUG1(a) NKDbgPrintfW(a)
#define DEBUG2(a,b) NKDbgPrintfW(a,b)
#define DEBUG3(a,b,c) NKDbgPrintfW(a,b,c)
#define DEBUG4(a,b,c,d) NKDbgPrintW(a,b,c,d)
#else
#define DEBUG1(a)
#define DEBUG2(a,b)
#define DEBUG3(a,b,c)
#define DEBUG4(a,b,c,d)
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegPropertyBag::CRegPropertyBag(HKEY hKeyParent,LPCTSTR lpwszKeyName,REGSAM samDesired)
{
	m_pszParentKey = NULL;
	m_pszKeyName = NULL;
	m_pKey = new CRegKey;
	m_hKeyParent = hKeyParent;
	if(m_pKey)
	{
		CRegKey& key = *m_pKey;
		if(samDesired & KEY_CREATE_SUB_KEY)
		{
			LONG lRes = key.Create(hKeyParent,lpwszKeyName);
			if(lRes != ERROR_SUCCESS)
			{
				DEBUG3(TEXT("Regkey creation failed (%s) (%d)\n"),lpwszKeyName,lRes);
			}
		}
		else
		{
			LONG lRes = key.Open(hKeyParent,lpwszKeyName,samDesired);
			if(lRes != ERROR_SUCCESS)
			{
				DEBUG3(TEXT("Regkey Open failed (%s) (%d)\n"),lpwszKeyName,lRes);
			}

		}
		TCHAR* wsz;
		if(wsz = _tcsrchr(lpwszKeyName,'\\'))
		{
			int len = wsz-lpwszKeyName;
			if(len)
			{
				m_pszParentKey = new TCHAR[len+1];
				_tcsncpy(m_pszParentKey,lpwszKeyName,len);
				m_pszParentKey[len] = '\0';
				
				len = _tcslen(lpwszKeyName) - len - 1;
				m_pszKeyName = new TCHAR[len+1];
				_tcsncpy(m_pszKeyName,wsz+1,len);
				m_pszKeyName[len] = '\0';
			}	
		}

		m_sam = samDesired;
	}
}

CRegPropertyBag::~CRegPropertyBag()
{
	if(m_pKey) delete m_pKey;
	if(m_pszKeyName) delete [] m_pszKeyName;
	if(m_pszParentKey) delete [] m_pszParentKey;
}

CRegProperty CRegPropertyBag::Get(LPCTSTR lpwszValueName) const
{
#ifdef _WIN32_WCE_AUTOPC
	//hacka hacka hacka
	static TCHAR sz[1000];
	DWORD dwSzLen = 500;
	LONG lRes = m_pKey->QueryValue(sz,lpwszValueName,&dwSzLen);
	if(lRes == ERROR_SUCCESS)
	{
		DEBUG3(L"AutoPC Get succeeded (%s)=(%s)\n",lpwszValueName,sz);
		return CRegProperty(sz);
	}
	else
	{
		DEBUG3(L"AutoPC Get failed (%s) (%d)\n",lpwszValueName,lRes);
		return CRegProperty();
	}

#else
	DWORD DataSize;
	DWORD Type;
	LONG lRes;


	HKEY hKey = static_cast<HKEY>(*m_pKey);
	DEBUG2(L"!!RegPropertyBag::Get %s\n",lpwszValueName);
	lRes = RegQueryValueEx(hKey,lpwszValueName,NULL,&Type,reinterpret_cast<LPBYTE>(NULL),&DataSize);
	if(lRes == ERROR_SUCCESS)
	{
		DEBUG2(L"!!RegPropertyBag Get is of length %d\n",DataSize);
		if(Type == REG_DWORD || Type == REG_DWORD_BIG_ENDIAN)
		{
			DWORD dw;
			RegQueryValueEx(hKey,lpwszValueName,NULL,&Type,reinterpret_cast<LPBYTE>(&dw),&DataSize);
			return CRegProperty(dw);
		}
		else if(Type == REG_SZ || REG_EXPAND_SZ || REG_MULTI_SZ)
		{
			DEBUG1(L"!!RegPropertyBag::Get is a string\n");
			TCHAR wsz[MAX_HTTP_PATH];
			RegQueryValueEx(hKey,lpwszValueName,NULL,&Type,reinterpret_cast<LPBYTE>(wsz),&DataSize);
			DEBUG2(L"!!The string is: %s\n",wsz);
			return CRegProperty(wsz);
		}
	}
	else
	{
		DEBUG2(L"!!RegPropertyBag::Get failed %d\n",lRes);
	}
	return CRegProperty();
#endif
}



bool CRegPropertyBag::Set(LPCTSTR lpwszValueName,const CRegProperty& Value) const
{
	if(m_pKey)
	{
		if(m_sam & KEY_SET_VALUE)
		{
			HKEY hKey = static_cast<HKEY>(*m_pKey);
			switch (Value.Type())
			{
			case regDWORD:
				{
				DWORD dw = (DWORD)Value;
				return ERROR_SUCCESS == 
					RegSetValueEx(hKey,lpwszValueName,NULL,REG_DWORD,(LPBYTE)&dw,4);
				}
			case regSTRING:
				{
				LPCTSTR wsz = (LPCTSTR)Value;
				DWORD DataSize = sizeof(TCHAR)*(_tcslen(wsz)+1);
				LONG lRes = RegSetValueEx(hKey,lpwszValueName,NULL,REG_SZ,((LPBYTE)wsz),DataSize);
				return ERROR_SUCCESS == lRes;
					
				}
			}
		}
	}
	return false;
}

bool CRegPropertyBag::Delete(LPCTSTR lpwszValueName) const
{
	if(!m_pKey) 
		return false;
	else
		return ERROR_SUCCESS == m_pKey->DeleteValue(lpwszValueName);
}

bool CRegPropertyBag::DeleteKey() const
{
	if(~m_sam & KEY_WRITE) return false;

	FILETIME time;
	DWORD dwSize = 256;
	TCHAR szBuffer[256];
	while (RegEnumKeyEx(m_pKey->m_hKey, 0, szBuffer, &dwSize, NULL, NULL, NULL,
		&time)==ERROR_SUCCESS)
	{
		if (ERROR_SUCCESS != m_pKey->RecurseDeleteKey(szBuffer))
			return false;
		dwSize = 256;
	}
	m_pKey->Close();
	m_pKey->Open(m_hKeyParent,m_pszParentKey,m_sam);
	return ERROR_SUCCESS == m_pKey->DeleteSubKey(m_pszKeyName);
}