// FileOutputStream.h: interface for the FileOutputStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEOUTPUTSTREAM_H__7AAC066E_3F8F_11D3_8D91_0090276149F2__INCLUDED_)
#define AFX_FILEOUTPUTSTREAM_H__7AAC066E_3F8F_11D3_8D91_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WCE
#include <stdio.h>
#endif
#include "Dss.h"
#include "StructHelper.h"

class FileOutputStream : public CUnknown, public IDssAudioOutputStream
{
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssAudioOutputStream)

	FileOutputStream();
	virtual ~FileOutputStream() {};

	virtual void FinalRelease();

	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat);
	HRESULT STDCALL SetFormat(IDssAudioStreamFormat* pIFormat);
	virtual HRESULT Open(LPCTSTR szFileName);

	//IOOutputStream interface
	virtual HRESULT STDCALL Write(BYTE* pBuffer, DWORD dwBytes, CodecEventInfo* pEventInfo, DWORD* pdwBytesWritten);
	virtual HRESULT STDCALL Advise(/*in*/ IDssEvent* pIEvent);
	//virtual HRESULT SetCallbackRate(OutputStreamEvent event, DWORD dwRate);
	virtual HRESULT STDCALL Stop();
	virtual HRESULT STDCALL Pause();
	virtual HRESULT STDCALL Restart();
	virtual HRESULT STDCALL GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers);

	virtual HRESULT STDCALL SetVolume(WORD wLeftChannel,WORD wRightChannel) { return E_NOTIMPL; }
	virtual HRESULT STDCALL GetVolume(WORD* pwLeftChannel, WORD* pwRightChannel) { return E_NOTIMPL; } 

private:
#ifdef WIN32
	HANDLE m_hFile;
#else
	FILE* m_file;
#endif
	IDssAudioStreamFormat* m_pIFormat;
};

#endif // !defined(AFX_FILEOUTPUTSTREAM_H__7AAC066E_3F8F_11D3_8D91_0090276149F2__INCLUDED_)
