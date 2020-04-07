// Driver.cpp: implementation of the CDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Driver.h"

#ifdef _WIN32_WCE_AUTOPC
extern "C" {
	void NKDbgPrintfW(LPWSTR lpszFmt,...);
}
#define DEBUG1(a) NKDbgPrintfW(a)
#define DEBUG2(a,b) NKDbgPrintfW(a,b)
#define DEBUG3(a,b,c) NKDbgPrintfW(a,b,c)
#else
//#define DEBUG1(a) MessageBox(NULL, a, a, MB_OK)
#define DEBUG1(a)
#define DEBUG2(a,b)
#define DEBUG3(a,b,c)

#if 0
#define MAXSTRINGLEN 2048

void DEBUG2(LPCTSTR str,...)
{
	va_list list;
	va_start( list, str );
	TCHAR sz[MAXSTRINGLEN];
#ifdef UNDER_CE
	_vstprintf(sz,str,list);
#else
	vsprintf(sz,str,list);
#endif
	MessageBox(NULL,sz,TEXT("Debug"),MB_OK);
}

void DEBUG3(LPCTSTR str,...)
{
	va_list list;
	va_start( list, str );
	TCHAR sz[MAXSTRINGLEN];
#ifdef UNDER_CE
	_vstprintf(sz,str,list);
#else
	vsprintf(sz,str,list);
#endif
	MessageBox(NULL,sz,TEXT("Debug"),MB_OK);
}
#endif

#endif


class IAuthenticator
{
public:
	virtual DWORD GetNumKeys() = 0;
	virtual DWORD ValidateCipher(DWORD dwKeyIndex, BYTE* pSrc,DWORD dwSrcLen,BYTE** ppDst) = 0;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef HRESULT ( * lpEntryFunc)(IDssDriver**);
typedef HRESULT ( * lpGetAuthenticationData)(BYTE** ppData,DWORD* pdwDataLen);
typedef HRESULT ( * lpAuthenticate)(BYTE* pData,DWORD dwDataLen);

HRESULT CDriver::Initialize(const CRegPropertyBag& bag,DWORD dwAuthenticator) 
{
	HRESULT hr = E_FAIL;
#ifdef _WIN32_WCE_AUTOPC
	CRegProperty reg = bag.Get(TEXT("Dll"));
	if(reg.Type() == regSTRING)
	{
		DEBUG2(L"RegQueryValueEx succeeded: %s\n",LPCTSTR(reg));
		this->m_hLib = LoadLibrary(reg);
	}
	else
	{
		DEBUG1(L"RegQueryValueEx failed...forcing to \\Windows\\DSSMP3.DLL\n");
		this->m_hLib = LoadLibrary(TEXT("\\Windows\\DSSMp3.dll"));
	}
#else
	this->m_hLib = LoadLibrary(bag.Get(TEXT("Dll")));
#endif
	if(m_hLib)
	{
		DEBUG1(L"LoadLibrary succeeded\n");
		IAuthenticator* pIApp = reinterpret_cast<IAuthenticator*>(dwAuthenticator);
		DEBUG1(L"Calling GetNumKeys() on IAuthenticator\n");
		DWORD dwKeys = pIApp->GetNumKeys();
		DEBUG2(L"GetNumKeys() returned %d\n",dwKeys);
		BYTE* pData;
		DWORD dwDataLen;

		lpGetAuthenticationData GAD;
		GAD = (lpGetAuthenticationData) GetProcAddress(this->m_hLib,TEXT("GetDriverAuthenticationData"));
		DEBUG2(L"GetProcAddress for GetDriverAuthenticationData %d\n",GAD);
		if(!GAD)
		{
			DEBUG2(L"##GetLastError is %d\n",GetLastError());
		}
		hr = (*GAD)(&pData,&dwDataLen);	
		if(FAILED(hr))
		{
			DEBUG2(L"GetAuthenticationData failed %d\n",hr);
			return hr;
		}
		DEBUG1(L"GetAuthenticationData succeeded\n");

		lpAuthenticate Authenticate;
		Authenticate = (lpAuthenticate) GetProcAddress(this->m_hLib,TEXT("AuthenticateDriver"));
		DEBUG2(L"GetProcAddress for AuthenticateDriver %d\n",Authenticate);
		if(!Authenticate)
		{
			DEBUG2(L"##GetLastError is %d\n",GetLastError());
		}

		if(pData && dwDataLen)
		{
			//Driver requires authentication.
			for(DWORD dw=0;dw<dwKeys;dw++)
			{
				BYTE* pData2 = 0;
				DEBUG2(L"~~~~Validating Key %d\n",0);
				DWORD dwData2Len = pIApp->ValidateCipher(dw,pData,dwDataLen,&pData2);
				DEBUG1(L"~~--Validated Key\n");
				if(dwData2Len)
				{
					DEBUG1(L"~~~~Authenticating Key\n");
					hr = (*Authenticate)(pData2,dwData2Len);
					DEBUG2(L"~~--Authenticated Key %d\n",hr);
					delete [] pData2;
					if(SUCCEEDED(hr))
						break;
				}
			}
			delete [] pData;
		}

		if(SUCCEEDED(hr))
		{
			lpEntryFunc EntryPointFunc;			
			EntryPointFunc = (lpEntryFunc) GetProcAddress(this->m_hLib,TEXT("GetDriverEntryPoint"));
			DEBUG2(L"GetProcAddress for GetDriverEntryPoint %d\n",EntryPointFunc);
			if(!EntryPointFunc)
			{
				DEBUG2(L"##GetLastError is %d\n",GetLastError());
			}

			hr = (*EntryPointFunc)(&(this->m_pIDriver));
			DEBUG2(L"EntryPointFunc() returned %d\n",hr);
		}
	}

	if(SUCCEEDED(hr))
	{
#ifdef _WIN32_WCE_AUTOPC
		this->m_sCodecName = NULL;
		this->m_sCodecVersion = NULL;
		this->m_sCodecExtension = NULL;
#else
		this->m_sCodecName = bag.Get(TEXT("Name"));
		this->m_sCodecVersion = bag.Get(TEXT("Version"));
		this->m_sCodecExtension = bag.Get(TEXT("Extension"));
#endif
	}
	return hr;
}

CDriver::~CDriver()
{
	if(m_pIDriver)
		m_pIDriver->Release();

	if(m_hLib)
		FreeLibrary(m_hLib);
}
