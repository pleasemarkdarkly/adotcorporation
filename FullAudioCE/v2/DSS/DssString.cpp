// String.cpp: implementation of the CString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dssstring.h"
#ifdef WIN32
#include <tchar.h>
#endif
#include <malloc.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString::CString()
{
	Initialize(NULL);
}

CString::CString(LPCTSTR sz)
{
	Initialize(sz);
}

CString::CString(const CString& s)
{
	Initialize(s.m_sz);
}

CString::~CString()
{
	Destroy();
}

CString& CString::operator=(LPCTSTR sz)
{
	Destroy();
	Initialize(sz);
	return *this;
}

CString& CString::operator=(const CString& s)
{
	if(this == &s) 
	{
		Destroy();
		Initialize(s.m_sz);
	}
	return *this;
}

CString::operator LPCTSTR() const
{
	return const_cast<LPCTSTR>(m_sz);
}

void CString::Initialize(LPCTSTR sz)
{
	m_sz = _tcsdup(sz);
}

void CString::Destroy()
{
	free(m_sz);
}

