// Drivers.cpp: implementation of the CDrivers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegPropertyBag.h"
#include "Drivers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE_AUTOPC
extern "C" {
	void NKDbgPrintfW(LPWSTR lpszFmt,...);
}
#define DEBUG1(a) NKDbgPrintfW(a)
#define DEBUG2(a,b) NKDbgPrintfW(a,b)
#define DEBUG3(a,b,c) NKDbgPrintfW(a,b,c)
#else
#define DEBUG1(a)
#define DEBUG2(a,b)
#define DEBUG3(a,b,c)
#endif

HRESULT CDrivers::CreateInstance(DWORD dwAuthenticator,IDssDrivers **ppIDrivers)
{
	
	CDrivers *pDrivers = new CDrivers(dwAuthenticator);
	return ::InternalCreateInstance(pDrivers,ppIDrivers);
}

HRESULT CDrivers::FinalConstruct()
{
	m_head = NULL;
	m_dwNumDrivers = 0;
	//Load libraries
	LONG lRes;
	CRegKey keyCodecs;

	DEBUG1(L"+++Opening Registry Key\n");
	lRes = keyCodecs.Create(HKEY_LOCAL_MACHINE,TEXT("Software\\iobjects\\DssCodecs"));
	if(lRes != ERROR_SUCCESS)
	{
		DEBUG1(L"+++Opening failed\n");
		return S_OK;
	}
	DEBUG1(L"+++Opening succeeded\n");
	TCHAR szName[200];
	DWORD cbName;
	DWORD dwIndex = 0;
	CDriver* pCurrentDriver = NULL;
	do
	{
		cbName = 200*sizeof(TCHAR);
		lRes = RegEnumKeyEx(keyCodecs,dwIndex++,szName,&cbName,NULL,NULL,NULL,NULL);
		if(lRes == ERROR_SUCCESS)
		{
			pCurrentDriver = new CDriver;
			if(pCurrentDriver == NULL)
				return E_OUTOFMEMORY;

			DEBUG2(L"+++Initializing %s\n",szName);
			HRESULT hr = pCurrentDriver->Initialize(CRegPropertyBag(keyCodecs,szName),m_dwAuthenticator);
			if(SUCCEEDED(hr))
			{
				DEBUG2(L"+++Initialization succeeded for %s\n",szName);
				if(m_head == NULL)
				{
					m_head = pCurrentDriver;
					m_head->m_next = NULL;
				}
				else
				{
					pCurrentDriver->m_next = m_head;
					m_head = pCurrentDriver;
				}
				m_dwNumDrivers++;
			}
			else
				delete pCurrentDriver;
		}
		else 
			break;
	} while(true);

	return S_OK;
}
	
void CDrivers::FinalRelease()
{
	CUnknown::FinalRelease();
	CDriver* t;
	while(m_head)
	{
		 t = m_head->m_next;
		 delete m_head;
		 m_head = t;
	}
}

HRESULT STDCALL CDrivers::GetNumDrivers(DWORD* pdwNumDrivers)
{
	if(pdwNumDrivers == NULL)
		return E_POINTER;
	*pdwNumDrivers = m_dwNumDrivers;
	return S_OK;
}

HRESULT STDCALL CDrivers::GetDriver(DWORD dwIndex, IDssDriver** ppIDriver)
{
	if(ppIDriver == NULL)
		return E_POINTER;
	if(dwIndex >= m_dwNumDrivers)
		return E_OUTOFRANGE;
	
	CDriver* t = m_head;
	for(DWORD d = 0;d < dwIndex;d++)
		t = t->m_next;
	
	*ppIDriver = t->m_pIDriver;
	t->m_pIDriver->AddRef();
	return S_OK;
}

