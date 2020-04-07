// FileInputStream.cpp: implementation of the CFileInputStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef WIN32
#include <windows.h>
#endif
#include "FileInputStream.h"

#if defined(_WIN32_WCE_AUTOPC)
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define GetFilePointer(hFile) SetFilePointer(hFile, 0, NULL, FILE_CURRENT)
#define INVALID_SET_FILE_POINTER -1

CFileInputStream::CFileInputStream()
: m_pIFormat(NULL)
{
#ifdef WIN32
	m_hFile = INVALID_HANDLE_VALUE;
#else
	m_file = NULL;
#endif
}

void CFileInputStream::FinalRelease()
{
	CUnknown::FinalRelease();

	if(m_pIFormat)
		m_pIFormat->Release();

#ifdef WIN32
	CloseHandle(m_hFile);
#else
	if(m_file != NULL)
		fclose(m_file);
#endif
}

HRESULT CFileInputStream::Open(LPCTSTR szFileName)
{
	if(szFileName == NULL)
		return E_POINTER;

//	if(FAILED(DssCreateStreamFormat(&m_pIFormat)))
//		return E_FAIL;
#ifdef WIN32
	DEBUG2(TEXT("Opening file %s\n"),szFileName);
	m_hFile = CreateFile(szFileName,
						GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		DEBUG2(TEXT("Open File failed. (%d)\n"),GetLastError());
		return E_FAIL;
	}
	else
	{
		DEBUG1(TEXT("Open File succeeded\n"));
		return S_OK;
	}
#else
	m_file = fopen(szFileName,"rb");
	if(m_file == NULL)
		return E_FAIL;
	else
		return S_OK;
#endif
}

// IOInputStream Interface
HRESULT STDCALL CFileInputStream::Seek(long Offset, SeekOrigin Origin)
{
#ifdef WIN32
	if(m_hFile == INVALID_HANDLE_VALUE)
#else
	if(m_file == NULL)
#endif
		return E_POINTER;

#ifdef WIN32
	int origin;
	switch(Origin)
	{
	case soCurrent:
		origin = FILE_CURRENT;
		break;
	case soEndOfFile:
		origin = FILE_END;
		break;
	case soStartOfFile:
		origin = FILE_BEGIN;
		break;
	}
	if(INVALID_SET_FILE_POINTER == SetFilePointer(m_hFile,Offset,NULL,origin))
#else
	if(fseek(m_file,Offset,static_cast<int>(Origin)))
#endif
		return E_FAIL;
	else
		return S_OK;
}

HRESULT STDCALL CFileInputStream::Read(BYTE* pBuffer,DWORD dwBytes, DWORD* pdwBytesRead)
{
#ifdef WIN32
	if(m_hFile == INVALID_HANDLE_VALUE)
#else
	if(m_file == NULL)
#endif
		return E_POINTER;

#ifdef WIN32
	BOOL b = ReadFile(m_hFile,pBuffer,dwBytes,pdwBytesRead,NULL);
	if(!b)
	{
		DWORD dw = GetLastError();
	}
#else
	*pdwBytesRead = static_cast<DWORD>(fread(pBuffer,sizeof(BYTE),dwBytes,m_file));
#endif
	return S_OK;
}

HRESULT STDCALL CFileInputStream::IsEOF()
{
#ifdef WIN32
	if(m_hFile == INVALID_HANDLE_VALUE) return E_POINTER;
#else
	if(m_file == NULL) return E_POINTER;
#endif

#ifdef WIN32
	if(GetFilePointer(m_hFile) < GetFileSize(m_hFile,NULL))
#else
	if(feof(m_file)) 
#endif
		return S_OK;
	else 
		return S_FALSE;
}

HRESULT STDCALL CFileInputStream::FileSize(DWORD* pdwFileSize)
{
#ifdef WIN32
	if(m_hFile == INVALID_HANDLE_VALUE || pdwFileSize == NULL) return E_POINTER;
#else
	if(m_file == NULL || pdwFileSize == NULL) return E_POINTER;
#endif

#ifdef WIN32
	*pdwFileSize = GetFileSize(m_hFile,NULL);
#else
	long pos = ftell(m_file);
	fseek(m_file,0,SEEK_END);
	*pdwFileSize = static_cast<DWORD>(ftell(m_file));
	fseek(m_file,pos,SEEK_SET);
#endif
	return S_OK;
}

HRESULT STDCALL CFileInputStream::GetCurrentPosition(long* plOffset)
{
#ifdef WIN32
	if(m_hFile == NULL || plOffset == NULL) return E_POINTER;
#else
	if(m_file == NULL || plOffset == NULL) return E_POINTER;
#endif

#ifdef WIN32
	*plOffset = GetFilePointer(m_hFile);
#else
	*plOffset = ftell(m_file);
#endif
	return S_OK;
}

HRESULT STDCALL CFileInputStream::GetFormat(IDssStreamFormat** ppIStreamFormat)
{
	if(ppIStreamFormat == NULL) return E_POINTER;
	*ppIStreamFormat = m_pIFormat;
	m_pIFormat->AddRef();
	return S_OK;
}

HRESULT STDCALL CFileInputStream::SetFormat(IDssStreamFormat* pIFormat)
{
	if(!pIFormat) return E_POINTER;
	if(m_pIFormat) m_pIFormat->Release();
	m_pIFormat = pIFormat;
	pIFormat->AddRef();
	return S_OK;
}
