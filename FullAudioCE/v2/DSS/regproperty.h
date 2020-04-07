/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// regproperty.h - Abstraction for DWORD or String value in Registry Key
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
#if !defined(AFX_PROPERTY_H__0B5F5709_D25F_11D2_86E9_00A0C9777D43__INCLUDED_)
#define AFX_PROPERTY_H__0B5F5709_D25F_11D2_86E9_00A0C9777D43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

enum {
	regNONE,
	regDWORD,
	regSTRING
};

class CRegProperty  
{
	int m_PropertyType;
	union {
		DWORD dwProp;
		LPTSTR lpwszProp;
	} m_Property;
	void _Destroy();
public:
	CRegProperty();
	CRegProperty(DWORD dw);
	CRegProperty(LPCTSTR lpwsz);
	virtual ~CRegProperty();
	operator DWORD() const;
	operator LPCTSTR() const;
	int Type() const;
	CRegProperty& operator=(DWORD dw);
	CRegProperty& operator=(LPCTSTR lpwsz);
	CRegProperty& operator=(const CRegProperty& rhs);
};

#endif // !defined(AFX_PROPERTY_H__0B5F5709_D25F_11D2_86E9_00A0C9777D43__INCLUDED_)
