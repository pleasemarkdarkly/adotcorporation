// VideoOutputStream.cpp: implementation of the CVideoOutputStream class.
//
//////////////////////////////////////////////////////////////////////

#include "VideoOutputStream.h"
#include "StructHelper.h"

#include <windows.h>
#include <malloc.h>

void CVideoOutputStream::FinalRelease()
{
	CUnknown::FinalRelease();
	if(m_pIFormat)
		m_pIFormat->Release();
	if(m_pIEvent)
		m_pIEvent->Release();
	free(m_pBitmapInfo);
}

HRESULT CVideoOutputStream::Initialize(IDssStreamFormat* pIFormat)
{
	if(!pIFormat) return E_POINTER;
	if(pIFormat->GetFormatTag() != FORMAT_AVI)
		return E_FAIL;

	IDssPtr<IDssVideoStreamFormat> pIVideoSrcFormat;
	if (FAILED(pIFormat->QueryInterface(IID_IDssVideoStreamFormat, reinterpret_cast<void**>(&pIVideoSrcFormat))))
		return E_INVALID_FORMATS;

	// Get the bitmap info.
	m_pBitmapInfo = pIVideoSrcFormat->GetBitmapInfo();

	m_pIEvent = 0;

	return S_OK;
}

//Write doesn't make a copy of pEventInfo, so the caller should 
//not use the info struct after its been passed as a parameter to
//the waveout stream

//This method performs two functions
// 1) Poll the waveout device to fire event information, and find out
//    status of playback buffers.
// 2) If pBuffer != NULL, write the data to the next playback buffer.
//
//Return values : 
//	S_OK - Playback buffers are not empty
//  S_FALSE - Playback buffers are empty
HRESULT STDCALL CVideoOutputStream::Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten)
{
	HRESULT hr = S_OK;

	if (pBuffer && dwBytes)
	{
		if(m_pIEvent)
		{
			VideoFrameData vfd;
			vfd.dwDataSize = dwBytes;
			vfd.pData = pBuffer;
			if (pEventInfo)
				vfd.frameInfo = *(pEventInfo->pFrame);
			m_pIEvent->OnFrameData(&vfd);
		}
	}
	else
	{
		if(m_pIEvent)
		{
			VideoFrameData vfd;
			vfd.dwDataSize = 0;
			vfd.pData = 0;
			return m_pIEvent->OnFrameData(&vfd);
		}
	}

	//Fire Event Notification
	if(pEventInfo)
	{
		if(m_pIEvent)
		{
			if(pEventInfo->pFrame)
				m_pIEvent->OnFrameProgress(pEventInfo->pFrame);
		}
		// This doesn't work on the PC, but has no problems on the handheld.  Why?
//		DestroyCodecEventInfo(pEventInfo);
	}

	return hr;
}

//IOOutputStream Event Management
HRESULT STDCALL CVideoOutputStream::Advise(IDssEvent* pIEvent)
{
	if(pIEvent == NULL) return E_POINTER;

	IDssVideoOutputStreamEvent* pIVideoEvent;
	if (FAILED(pIEvent->QueryInterface(IID_IDssVideoOutputStreamEvent, reinterpret_cast<void**>(&pIVideoEvent))))
		return E_INVALID_EVENT;

	if(m_pIEvent)
		m_pIEvent->Release();

	m_pIEvent = pIVideoEvent;

	return S_OK;
}


HRESULT STDCALL CVideoOutputStream::SetFormat(IDssStreamFormat* pIFormat)
{
	if(!pIFormat) return E_POINTER;

	IDssVideoStreamFormat* pIVideoSrcFormat;
	if (FAILED(pIFormat->QueryInterface(IID_IDssVideoStreamFormat, reinterpret_cast<void**>(&pIVideoSrcFormat))))
		return E_INVALID_FORMATS;

	if(m_pIFormat) m_pIFormat->Release();
	m_pIFormat = pIVideoSrcFormat;

	// Get the bitmap info.
	m_pBitmapInfo = pIVideoSrcFormat->GetBitmapInfo();

	return S_OK;
}

HRESULT STDCALL CVideoOutputStream::GetFormat(IDssStreamFormat** ppIFormat)
{
	if(!ppIFormat) return E_POINTER;
	*ppIFormat = m_pIFormat;
	if(m_pIFormat)
	{
		m_pIFormat->AddRef();
		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT STDCALL CVideoOutputStream::Stop()
{
	return S_OK;
}

HRESULT STDCALL CVideoOutputStream::Pause()
{
	return S_OK;
}

HRESULT STDCALL CVideoOutputStream::Restart()
{
	return S_OK;
}

HRESULT STDCALL CVideoOutputStream::GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers)
{
	*pdwTotalBuffers = 0;
	*pdwFilledBuffers = 0;

	return E_NOTIMPL;
}

HRESULT STDCALL CVideoOutputStream::SetVolume(WORD wLeftChannel, WORD wRightChannel)
{
	return E_NOTIMPL;
}

HRESULT STDCALL CVideoOutputStream::GetVolume(WORD* pwLeftChannel, WORD* pwRightChannel)
{
	*pwLeftChannel = 0;
	*pwRightChannel = 0;

	return E_NOTIMPL;
}

