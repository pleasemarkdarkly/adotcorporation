// String.h: interface for the CString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRING_H__A70D0C51_3A56_11D3_8D91_0090276149F2__INCLUDED_)
#define AFX_STRING_H__A70D0C51_3A56_11D3_8D91_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dss.h"

class CString  
{
public:
	CString();
	CString(LPCTSTR sz);
	CString(const CString& s);
	CString& operator=(LPCTSTR sz);
	CString& operator=(const CString& s);
	operator LPCTSTR() const;
	virtual ~CString();
protected:
	void Initialize(LPCTSTR sz);
	void Destroy();
	LPTSTR m_sz;
};

#endif // !defined(AFX_STRING_H__A70D0C51_3A56_11D3_8D91_0090276149F2__INCLUDED_)
