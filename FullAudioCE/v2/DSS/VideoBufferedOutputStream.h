#ifndef AVIOUTPUTSTREAM_H_
#define AVIOUTPUTSTREAM_H_

#if 0
struct IDssOutputStream : public IDssUnknown
{
	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat) = 0;
	virtual HRESULT STDCALL Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten) = 0;
	virtual HRESULT STDCALL Advise(/*in*/ IDssOutputStreamEvent* pIEvent) = 0;

	//TODO : In version when QI is supported, move the following events to
	//a separate interface IDssWaveOutputStream that is derived from IDssOutputStream
	//The following are used in conjunction with the MediaPlayer interface and
	//should only be part of the IDssWaveOutputStream.
	virtual HRESULT STDCALL Stop() = 0;
	virtual HRESULT STDCALL Pause() = 0;
	virtual HRESULT STDCALL Restart() = 0;

	virtual HRESULT STDCALL SetVolume(WORD wLeftChannel,WORD wRightChannel) = 0;
	virtual HRESULT STDCALL GetVolume(WORD* pwLeftChannel, WORD* pwRightChannel) = 0;
	//
	virtual HRESULT STDCALL GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers) = 0;
};
#endif

#include "Dss.h"
#include "SimpleVector.h"
#include "SimpleList.h"
#include "CriticalSection.h"

typedef struct
{
	HBITMAP	hBitmap;
	int		iFrameIndex;
} DIBHolder;

class CAVIOutputStream : public CUnknown, public IDssVideoOutputStream
{
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssVideoOutputStream)

//	CWaveOutStream() : m_pWaveOut(NULL), m_pIEvent(NULL),  m_pIFormat(NULL) {};
	CAVIOutputStream() : m_pIFormat(NULL) {};
	virtual ~CAVIOutputStream() {};

	virtual void FinalRelease();
	
	static HRESULT Create(HWND hwnd, IDssStreamFormat* pIFormat,DWORD dwNumBuffers, DWORD dwBufferSize, IDssOutputStream** ppIAVIOutputStream);

//	virtual HRESULT SetFormat(IDssStreamFormat* pIFormat);

	HBITMAP GetNextBitmap();
	DIBHolder GetNextBitmapStruct();

//Interface Methods
	virtual HRESULT STDCALL Stop();
	virtual HRESULT STDCALL Pause();
	virtual HRESULT STDCALL Restart();
	virtual HRESULT STDCALL GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers);
	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat);
	virtual HRESULT Initialize(HWND hwnd, IDssStreamFormat* pIFormat, DWORD nBuffers, DWORD BufferSize);
	virtual HRESULT STDCALL Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten);
	virtual HRESULT STDCALL Advise(IDssEvent* pIEvent);

	virtual HRESULT STDCALL SetVolume(WORD wLeftChannel,WORD wRightChannel);
	virtual HRESULT STDCALL GetVolume(WORD* pwLeftChannel, WORD* pwRightChannel);

private:
//	CWaveOut* m_pWaveOut;
	IDssStreamFormat* m_pIFormat;
	IDssVideoOutputStreamEvent* m_pIEvent;
	BITMAPINFO* m_pBitmapInfo;

	// The array of bitmaps.
	int	m_nMaxBuffers;
	SimpleList<DIBHolder> m_lBitmaps;
//	SimpleList<HBITMAP> m_lBitmaps;

	void ClearBuffer();

	// The screen window.
	HWND	m_hwnd;

	CCriticalSection	m_cs;
	bool	m_bStopped;
};



#endif	// AVIOUTPUTSTREAM_H_
