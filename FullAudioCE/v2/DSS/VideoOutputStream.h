#ifndef VIDEOOUTPUTSTREAM_H_
#define VIDEOOUTPUTSTREAM_H_

#include "Dss.h"
#include "CriticalSection.h"

class CVideoOutputStream : public CUnknown, public IDssVideoOutputStream
{
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssVideoOutputStream)

	CVideoOutputStream() : m_pIFormat(NULL), m_pIEvent(NULL) {};
	virtual ~CVideoOutputStream() {};

	virtual void FinalRelease();
	
	virtual HRESULT SetFormat(IDssStreamFormat* pIFormat);

//Interface Methods
	virtual HRESULT STDCALL Stop();
	virtual HRESULT STDCALL Pause();
	virtual HRESULT STDCALL Restart();
	virtual HRESULT STDCALL GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers);
	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat);
	virtual HRESULT Initialize(IDssStreamFormat* pIFormat);
	virtual HRESULT STDCALL Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten);
	virtual HRESULT STDCALL Advise(IDssEvent* pIEvent);

	virtual HRESULT STDCALL SetVolume(WORD wLeftChannel,WORD wRightChannel);
	virtual HRESULT STDCALL GetVolume(WORD* pwLeftChannel, WORD* pwRightChannel);

private:

	IDssStreamFormat* m_pIFormat;
	IDssVideoOutputStreamEvent* m_pIEvent;
	BITMAPINFO* m_pBitmapInfo;

	CCriticalSection	m_cs;
	bool	m_bStopped;
};



#endif	// VIDEOOUTPUTSTREAM_H_
