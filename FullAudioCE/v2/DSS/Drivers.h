// Drivers.h: interface for the CDrivers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERS_H__A70D0C4F_3A56_11D3_8D91_0090276149F2__INCLUDED_)
#define AFX_DRIVERS_H__A70D0C4F_3A56_11D3_8D91_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dss.h"
#include "Driver.h"

class CDriver;

class CDrivers : public CUnknown, public IDssDrivers
{
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssDrivers)

	virtual ~CDrivers() {}
	virtual HRESULT FinalConstruct();
	virtual void FinalRelease();

	static HRESULT CreateInstance(DWORD dwAuthenticator,IDssDrivers** ppIDrivers);

//Interface methods
	virtual HRESULT STDCALL GetNumDrivers(DWORD* pdwNumDrivers);
	virtual HRESULT STDCALL GetDriver(DWORD dwIndex, IDssDriver** ppIDriver);

private:
	CDrivers(DWORD dwAuthenticator) : m_dwAuthenticator(dwAuthenticator), m_head(NULL), m_dwNumDrivers(0) {}

protected:
	CDriver* m_head;
	DWORD m_dwNumDrivers;
	DWORD m_dwAuthenticator;
};




#endif // !defined(AFX_DRIVERS_H__A70D0C4F_3A56_11D3_8D91_0090276149F2__INCLUDED_)
