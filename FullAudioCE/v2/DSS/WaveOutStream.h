// WaveOutStream.h: interface for the CWaveOutStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEOUTSTREAM_H__20934D13_3FB5_11D3_8D91_0090276149F2__INCLUDED_)
#define AFX_WAVEOUTSTREAM_H__20934D13_3FB5_11D3_8D91_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dss.h"

#include "CWaveOutCE.h"
#include "StructHelper.h"

typedef CWaveOutPolling CWaveOut;

class CWaveOutStream : public CUnknown, public IDssAudioOutputStream
{
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssOutputStream)

	CWaveOutStream() : m_pWaveOut(NULL), m_pIEvent(NULL),  m_pIFormat(NULL) {};
	virtual ~CWaveOutStream() {};

	virtual void FinalRelease();

	virtual HRESULT SetFormat(IDssStreamFormat* pIFormat);

//Interface Methods
	virtual HRESULT STDCALL Stop();
	virtual HRESULT STDCALL Pause();
	virtual HRESULT STDCALL Restart();
	virtual HRESULT STDCALL GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers);
	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat);
	virtual HRESULT Initialize(IDssAudioStreamFormat* pIFormat, DWORD nBuffers, DWORD BufferSize);
	virtual HRESULT STDCALL Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten);
	virtual HRESULT STDCALL Advise(IDssEvent* pIEvent);

	virtual HRESULT STDCALL SetVolume(WORD wLeftChannel,WORD wRightChannel);
	virtual HRESULT STDCALL GetVolume(WORD* pwLeftChannel, WORD* pwRightChannel);

private:
	CWaveOut* m_pWaveOut;
	IDssAudioStreamFormat* m_pIFormat;
	IDssAudioOutputStreamEvent* m_pIEvent;
};

#endif // !defined(AFX_WAVEOUTSTREAM_H__20934D13_3FB5_11D3_8D91_0090276149F2__INCLUDED_)
