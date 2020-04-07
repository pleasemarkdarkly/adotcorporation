/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// regpropertybag.h - Abstraction for a Win32 Registry Key
//
// Author: Matthew Tadd		mtadd@iobjects.com
//		   Chuck Ferring	chuckf@iobjects.com
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
#if !defined(AFX_REGPROPERTYBAG_H__0B5F570A_D25F_11D2_86E9_00A0C9777D43__INCLUDED_)
#define AFX_REGPROPERTYBAG_H__0B5F570A_D25F_11D2_86E9_00A0C9777D43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RegProperty.h"
#include "RegKey.h"

class CRegPropertyBag  
{
	CRegKey* m_pKey;
	REGSAM m_sam;
	LPTSTR m_pszParentKey;
	LPTSTR m_pszKeyName;
	HKEY m_hKeyParent;

public:
	CRegPropertyBag(HKEY hKeyParent,LPCTSTR lpwszKeyName,REGSAM samDesired = KEY_ALL_ACCESS);
	CRegProperty Get(LPCTSTR lpwszValueName) const;
	bool Set(LPCTSTR lpwszValueName,const CRegProperty& Value) const;
	bool Delete(LPCTSTR lpwszValueName) const;
	bool DeleteKey() const;
	virtual ~CRegPropertyBag();
};

#endif // !defined(AFX_REGPROPERTYBAG_H__0B5F570A_D25F_11D2_86E9_00A0C9777D43__INCLUDED_)
