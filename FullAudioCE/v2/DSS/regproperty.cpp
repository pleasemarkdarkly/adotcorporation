/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// regproperty.cpp - Abstraction for DWORD or String value in Registry Key
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
#include "RegProperty.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRegProperty::~CRegProperty()
{
	if((m_PropertyType == regSTRING) && m_Property.lpwszProp)
		delete [] m_Property.lpwszProp;
}


CRegProperty::CRegProperty() : 
	m_PropertyType(regNONE)
{
	m_Property.dwProp = 0;
}

CRegProperty::CRegProperty(DWORD dw) : 
	m_PropertyType(regDWORD)
{
	m_Property.dwProp = dw;
}

CRegProperty::CRegProperty(LPCTSTR lpwsz) : 
	m_PropertyType(regSTRING)
{
	m_Property.lpwszProp = new TCHAR[_tcslen(lpwsz)+1];
	_tcscpy(m_Property.lpwszProp,lpwsz);
}

CRegProperty::operator DWORD() const
{
	if(m_PropertyType == regDWORD)
		return m_Property.dwProp;
	else
		return 0xFFFFFFFF;
}

CRegProperty::operator LPCTSTR() const
{
	if(m_PropertyType == regSTRING)
		return m_Property.lpwszProp;
	else
		return NULL;
}

int CRegProperty::Type() const
{
	return m_PropertyType;
}


void CRegProperty::_Destroy()
{
	if((m_PropertyType == regSTRING) && m_Property.lpwszProp)
		delete [] m_Property.lpwszProp;
}

CRegProperty& CRegProperty::operator=(DWORD dw)
{
	_Destroy();
	m_PropertyType = regDWORD;
	m_Property.dwProp = dw;
	return *this;
}

CRegProperty& CRegProperty::operator=(LPCTSTR lpwsz)
{
	LPTSTR& wsz = m_Property.lpwszProp;
	_Destroy();
	m_PropertyType = regSTRING;

	wsz = new TCHAR[_tcslen(lpwsz)+1];
	if(wsz)	_tcscpy(wsz,lpwsz);

	return *this;
}


CRegProperty& CRegProperty::operator=(const CRegProperty& rhs)
{
	if(&rhs == this) return *this;
	_Destroy();
	if(rhs.m_PropertyType == regSTRING)
	{
		m_PropertyType = regSTRING;
		m_Property.lpwszProp = new TCHAR[_tcslen(rhs.m_Property.lpwszProp) + 1];
		_tcscpy(m_Property.lpwszProp,rhs.m_Property.lpwszProp);
	}
	else if(rhs.m_PropertyType == regDWORD)
	{
		m_PropertyType = regDWORD;
		m_Property.dwProp = rhs.m_Property.dwProp;
	}
	else
	{
		m_PropertyType = regNONE;
		m_Property.dwProp = 0;
	}
	return *this;
}

