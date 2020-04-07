 // Driver.h: interface for the CDriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVER_H__A70D0C50_3A56_11D3_8D91_0090276149F2__INCLUDED_)
#define AFX_DRIVER_H__A70D0C50_3A56_11D3_8D91_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RegPropertyBag.h"
#include "Dss.h"
#include "DssString.h"
#include "Drivers.h"

class CDriver
{
friend class CDrivers;
public:
	CDriver() : m_pIDriver(NULL), m_next(NULL), m_hLib(NULL) {};
	virtual ~CDriver();
	
	HRESULT Initialize(const CRegPropertyBag& bag,DWORD dwAuthenticator);
	
protected:
	HMODULE m_hLib;
	CString m_sCodecName;
	CString m_sCodecVersion;
	CString m_sCodecExtension;
	IDssDriver* m_pIDriver; //Just contains this member implemented by each codec DLL.
	CDriver* m_next;
};





#endif // !defined(AFX_DRIVER_H__A70D0C50_3A56_11D3_8D91_0090276149F2__INCLUDED_)
