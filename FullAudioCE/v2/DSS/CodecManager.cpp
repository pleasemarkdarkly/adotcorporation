// CodecManager.cpp: implementation of the CCodecManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CodecManager.h"
#include "Drivers.h"
#include "MediaPlayer.h"
#include "FileInputStream.h"
#include "FileOutputStream.h"
#include "WaveOutStream.h"
#include "VideoOutputStream.h"
#if defined(WIN32) || defined(_WIN32_WCE)
#include <tchar.h>
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HRESULT CCodecManager::FinalConstruct()
{
	//Initialize m_pIDrivers
	return CDrivers::CreateInstance(m_dwAuthenticator,&m_pIDrivers);
}

void CCodecManager::FinalRelease()
{
	CUnknown::FinalRelease();
	if(m_pIDrivers)
		m_pIDrivers->Release();
}

HRESULT CCodecManager::CreateInstance(DWORD dwAuthenticator,IDssCodecManager **ppICodecManager)
{
	CCodecManager* pManager = new CCodecManager(dwAuthenticator);
	return ::InternalCreateInstance(pManager,ppICodecManager);
}

HRESULT STDCALL CCodecManager::GetDrivers(IDssDrivers** ppIDrivers)
{
	if(ppIDrivers == NULL) return E_POINTER;
	if(m_pIDrivers != NULL)
	{
		*ppIDrivers = m_pIDrivers;
		m_pIDrivers->AddRef();
		return S_OK;
	}
	else
	{
		*ppIDrivers = NULL;
		return E_UNEXPECTED;
	}
}

HRESULT STDCALL CCodecManager::CreateMediaPlayer(IDssMediaPlayer** ppIPlayer)
{
	if(ppIPlayer == NULL) return E_POINTER;

	return CMediaPlayer::CreateInstance(ppIPlayer);
}


//For given source stream, find applicable driver (and source index in driver)
//user can index dst indices for appropriate configuration of output file.
//call IDssDriver::GetCodec to return codec.
HRESULT STDCALL CCodecManager::FindCompatibleDriver(IDssInputStream* pInputStream, IDssSourceInfo** ppISourceInfo)
{
	if(pInputStream == NULL || ppISourceInfo == NULL) return E_POINTER;
	if(m_pIDrivers == NULL)
	{
		*ppISourceInfo = NULL;
		return E_UNEXPECTED;
	}
	HRESULT hr;
	DWORD dwNumDrivers;
	m_pIDrivers->GetNumDrivers(&dwNumDrivers);

//	for(DWORD i=0;i<dwNumDrivers;i++)
	for(DWORD i=dwNumDrivers;i>=0;--i)
	{
		IDssDriver* pD;
		if(SUCCEEDED(m_pIDrivers->GetDriver(i,&pD)))
		{
			hr = pD->GetFileInfo(pInputStream,ppISourceInfo);
			pD->Release();
			if(SUCCEEDED(hr))
				return hr;
		}
	}
	
	*ppISourceInfo = NULL;
	return E_FAIL;
}

//What would be really cool would be a way to go from one given source
//STREAMFORMATEX header to a destination STREAMFORMATEX header without too
//much thought on the part of the interface client.  Currently, the first
//STREAMFORMATEX in the destination list is considered the "recommended"
//format for best quality.
HRESULT STDCALL CCodecManager::OpenInputStreamFromFile(LPCTSTR szFileName, IDssInputStream** ppIFileInStream)
{
	HRESULT hr;
	CFileInputStream* pFile = new CFileInputStream;
	hr = pFile->Open(szFileName);
	if(SUCCEEDED(hr))
	{
		*ppIFileInStream = static_cast<IDssInputStream*>(pFile);
	}
	else
	{
		pFile->Release();
		*ppIFileInStream = NULL;
	}
	return hr;
}

HRESULT STDCALL CCodecManager::CreateOutputStreamFromFile(LPCTSTR szFileName,IDssAudioStreamFormat* pIFormat, IDssOutputStream** ppIFileOutStream)
{
	HRESULT hr;
	FileOutputStream* pFile = new FileOutputStream;	
	hr = pFile->Open(szFileName);
	if(SUCCEEDED(hr))
	{
		pFile->SetFormat(pIFormat);
		*ppIFileOutStream = static_cast<IDssOutputStream*>(pFile);
	}
	else
	{
		pFile->Release();
		*ppIFileOutStream = NULL;
	}
	return hr;
}

HRESULT STDCALL  CCodecManager::CreateWaveOutStream(IDssAudioStreamFormat* pIFormat,DWORD dwNumBuffers, DWORD dwBufferSize, IDssAudioOutputStream** ppIWaveOutStream)
{
	if(!pIFormat || !ppIWaveOutStream) return E_POINTER;

	CWaveOutStream* pWaveOut = new CWaveOutStream;
	if(!pWaveOut) 
	{
		*ppIWaveOutStream = NULL;
		return E_OUTOFMEMORY;
	}
	HRESULT hr = pWaveOut->Initialize(pIFormat,dwNumBuffers,dwBufferSize);
	if(SUCCEEDED(hr))
	{
		*ppIWaveOutStream = static_cast<IDssAudioOutputStream*>(pWaveOut);
	}
	else
	{
		delete pWaveOut;
		*ppIWaveOutStream = NULL;
	}
	return hr;	
}

/*
HRESULT STDCALL
CCodecManager::CreateVideoOutputStream(IDssVideoStreamFormat* pIFormat, IDssVideoOutputStream** ppIVideoOutputStream)
{
	if(!pIFormat || !ppIVideoOutputStream) return E_POINTER;

	CVideoOutputStream* pVideoOut = new CVideoOutputStream;
	if(!pVideoOut) 
	{
		*ppIVideoOutputStream = NULL;
		return E_OUTOFMEMORY;
	}
	HRESULT hr = pVideoOut->Initialize(pIFormat);
	if(SUCCEEDED(hr))
	{
		*ppIVideoOutputStream = static_cast<IDssVideoOutputStream*>(pVideoOut);
	}
	else
	{
		delete pVideoOut;
		*ppIVideoOutputStream = NULL;
	}
	return hr;	
}
*/

const TCHAR c_szAllName[] = TEXT("All (*.*)");
const TCHAR c_szAllExt[] = TEXT("*.*");

//Higher Level Functions
HRESULT STDCALL  CCodecManager::GetSupportedFileTypes(bool bSupportAll, LPTSTR* ppszFileTypes, DWORD* pchFileTypesLen)
{
	if(ppszFileTypes == NULL || pchFileTypesLen == NULL)
		return E_POINTER;

	if(m_pIDrivers == NULL) 
		return E_UNEXPECTED;

	DWORD i;
	DWORD dwTotal = 0;
	DWORD dwLen;
	//Calculate Length;
	DWORD dwNumDrivers;
	m_pIDrivers->GetNumDrivers(&dwNumDrivers);

	for(i=0;i<dwNumDrivers;i++)
	{
		DriverDetails* dd;
		IDssDriver* pD;
		if(SUCCEEDED(m_pIDrivers->GetDriver(i,&pD)))
		{
			pD->GetDriverDetails(&dd);
			const TCHAR* psz = dd->szFileExtensions;
			dwLen = _tcslen(psz) + 1;
			psz += dwLen;
			dwLen += _tcslen(psz) + 1;
			pD->Release();
			dwTotal += dwLen;
		}
	}

	if(bSupportAll)
	{
		dwTotal += _tcslen(c_szAllName) + 1;
		dwTotal += _tcslen(c_szAllExt) + 1;
	}

	//Allocate String
	TCHAR* pszFileTypes = new TCHAR[dwTotal+1];
	if(!pszFileTypes) return E_OUTOFMEMORY;
	TCHAR* psz = pszFileTypes;
	*pchFileTypesLen = dwTotal;

	//Now fill the string

	for(i=0;i<dwNumDrivers;i++)
	{
		DriverDetails* dd;
		IDssDriver* pD;
		if(SUCCEEDED(m_pIDrivers->GetDriver(i,&pD)))
		{
			pD->GetDriverDetails(&dd);
			const TCHAR* psz2 = dd->szFileExtensions;

			dwLen = _tcslen(psz2) + 1;
			psz2 += dwLen;
			dwLen += _tcslen(psz2) + 1;
			
			psz2 = dd->szFileExtensions;
			while(dwLen)
			{
				*psz = *psz2;
				psz++;
				psz2++;
				dwLen--;
			}
			pD->Release();
		}
	}

	if(bSupportAll)
	{
		_tcscpy(psz,c_szAllName);
		psz += _tcslen(c_szAllName) + 1;
		_tcscpy(psz,c_szAllExt);
		psz += _tcslen(c_szAllExt) + 1;
	}
	*psz = '\0';

	*ppszFileTypes = pszFileTypes;
	return S_OK;
}
