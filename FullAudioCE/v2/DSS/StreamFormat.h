// StreamFormat.h: interface for the CStreamFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREAMFORMAT_H__2699AB59_4A96_11D3_8D96_0090276149F2__INCLUDED_)
#define AFX_STREAMFORMAT_H__2699AB59_4A96_11D3_8D96_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dss.h"

class CAudioStreamFormat : public CUnknown, public IDssAudioStreamFormat
{
	CAudioStreamFormat() {};
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssAudioStreamFormat)
	
	virtual HRESULT STDCALL NondelegatingQueryInterface(const IID& iid, void** ppv);

	virtual ~CAudioStreamFormat();
	static HRESULT CreateInstance(IDssAudioStreamFormat** ppIFormat);

//Interface Methods
	virtual void STDCALL SetFormatTag(WORD wFormatTag) { m_sfx.wFormatTag = wFormatTag; }
	virtual WORD STDCALL GetFormatTag() { return m_sfx.wFormatTag; }
	virtual void STDCALL SetChannels(WORD wChannels) { m_sfx.nChannels = wChannels; }
	virtual WORD STDCALL GetChannels() { return m_sfx.nChannels; }
	virtual void STDCALL SetSamplingFrequency(DWORD nSamplesPerSec) { m_sfx.nSamplesPerSec = nSamplesPerSec; }
	virtual DWORD STDCALL GetSamplingFrequency() { return m_sfx.nSamplesPerSec; }
	virtual void STDCALL SetBytesPerSec(DWORD nBytesPerSec) { m_sfx.nBytesPerSec = nBytesPerSec; }
	virtual DWORD STDCALL GetBytesPerSec() { return m_sfx.nBytesPerSec; }
	virtual void STDCALL SetBitsPerSample(WORD wBitsPerSample) { m_sfx.wBitsPerSample = wBitsPerSample; }
	virtual WORD STDCALL GetBitsPerSample() { return m_sfx.wBitsPerSample; }

//private:
	STREAMFORMATEX m_sfx;
};

class CVideoStreamFormat : public CUnknown, public IDssVideoStreamFormat
{
	CVideoStreamFormat() : m_dwBitmapInfoSize(0), m_pBitmapInfo(0) {};
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssVideoStreamFormat)

	virtual HRESULT STDCALL NondelegatingQueryInterface(const IID& iid, void** ppv);
		
	virtual ~CVideoStreamFormat();
	static HRESULT CreateInstance(IDssVideoStreamFormat** ppIFormat);

//Interface Methods
	virtual void STDCALL SetFormatTag(WORD wFormatTag) { m_sfx.wFormatTag = wFormatTag; }
	virtual WORD STDCALL GetFormatTag() { return m_sfx.wFormatTag; }
	virtual void STDCALL SetChannels(WORD wChannels) { m_sfx.nChannels = wChannels; }
	virtual WORD STDCALL GetChannels() { return m_sfx.nChannels; }
	virtual void STDCALL SetSamplingFrequency(DWORD nSamplesPerSec) { m_sfx.nSamplesPerSec = nSamplesPerSec; }
	virtual DWORD STDCALL GetSamplingFrequency() { return m_sfx.nSamplesPerSec; }
	virtual void STDCALL SetBytesPerSec(DWORD nBytesPerSec) { m_sfx.nBytesPerSec = nBytesPerSec; }
	virtual DWORD STDCALL GetBytesPerSec() { return m_sfx.nBytesPerSec; }
	virtual void STDCALL SetBitsPerSample(WORD wBitsPerSample) { m_sfx.wBitsPerSample = wBitsPerSample; }
	virtual WORD STDCALL GetBitsPerSample() { return m_sfx.wBitsPerSample; }

	// ************
	// Ed's changes
	virtual void STDCALL SetBitmapInfo(BITMAPINFO* pBitmapInfo, DWORD dwBitmapInfoSize);
	// It's the caller's responsibility to free the memory.
	virtual STDCALL BITMAPINFO* GetBitmapInfo();
	virtual void STDCALL SetVideoFileInfo(const VideoFileInfo& videoFileInfo) { m_videoFileInfo = videoFileInfo; }
	virtual VideoFileInfo STDCALL GetVideoFileInfo() { return m_videoFileInfo; }

//private:
	STREAMFORMATEX m_sfx;

	// ************
	// Ed's changes
	BITMAPINFO*		m_pBitmapInfo;
	DWORD			m_dwBitmapInfoSize;
	VideoFileInfo	m_videoFileInfo;
};

#endif // !defined(AFX_STREAMFORMAT_H__2699AB59_4A96_11D3_8D96_0090276149F2__INCLUDED_)
