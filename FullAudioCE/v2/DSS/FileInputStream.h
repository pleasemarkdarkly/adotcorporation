// FileInputStream.h: interface for the CFileInputStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEINPUTSTREAM_H__7AAC066C_3F8F_11D3_8D91_0090276149F2__INCLUDED_)
#define AFX_FILEINPUTSTREAM_H__7AAC066C_3F8F_11D3_8D91_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WCE
#include <stdio.h>
#endif
#include "Dss.h"
#include "StreamFormat.h"

class CFileInputStream : public CUnknown, public IDssInputStream 
{
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssInputStream)

	CFileInputStream();
	virtual ~CFileInputStream() {};
	
	virtual void FinalRelease();

	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat);
	virtual HRESULT STDCALL SetFormat(IDssStreamFormat* pIFormat);

	virtual HRESULT Open(LPCTSTR szFileName);
	
	// IOInputStream Interface
	virtual HRESULT STDCALL Seek(long Offset, SeekOrigin Origin);
	virtual HRESULT STDCALL GetCurrentPosition(long* plOffset);
	virtual HRESULT STDCALL Read(BYTE* pBuffer,DWORD dwBytes, DWORD* pdwBytesRead);
	virtual HRESULT STDCALL FileSize(DWORD* pdwFileSize);
	virtual HRESULT STDCALL IsEOF();

private:

#ifdef WIN32
	HANDLE m_hFile;
#else
	FILE* m_file;
#endif

	IDssStreamFormat* m_pIFormat;
};

#endif // !defined(AFX_FILEINPUTSTREAM_H__7AAC066C_3F8F_11D3_8D91_0090276149F2__INCLUDED_)
