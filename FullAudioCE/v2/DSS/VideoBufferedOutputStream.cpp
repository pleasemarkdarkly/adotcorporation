// WaveOutStream.cpp: implementation of the CAVIOutputStream class.
//
//////////////////////////////////////////////////////////////////////

#include "AVIOutputStream.h"

#include <windows.h>
#include <malloc.h>

// TODO: Get rid of this MAJOR HACK!
// HACK HACK HACK HACK!!!
CAVIOutputStream*	g_pAVIOutputStream = 0;

HRESULT CAVIOutputStream::Create(HWND hwnd, IDssStreamFormat* pIFormat,DWORD dwNumBuffers, DWORD dwBufferSize, IDssOutputStream** ppIAVIOutputStream)
{
	g_pAVIOutputStream = 0;
	if(!pIFormat || !ppIAVIOutputStream) return E_POINTER;

	CAVIOutputStream* pAVIOut = new CAVIOutputStream;
	if(!pAVIOut) 
	{
		*ppIAVIOutputStream = NULL;
		return E_OUTOFMEMORY;
	}
	HRESULT hr = pAVIOut->Initialize(hwnd, pIFormat, dwNumBuffers, dwBufferSize);
	if(SUCCEEDED(hr))
	{
		*ppIAVIOutputStream = static_cast<IDssOutputStream*>(pAVIOut);
		g_pAVIOutputStream = pAVIOut;
	}
	else
	{
		delete pAVIOut;
		*ppIAVIOutputStream = NULL;
	}
	return hr;	
}

void CAVIOutputStream::FinalRelease()
{
	CUnknown::FinalRelease();
	if(m_pIFormat)
		m_pIFormat->Release();
	if(m_pIEvent)
		m_pIEvent->Release();
	free(m_pBitmapInfo);
}

HRESULT CAVIOutputStream::Initialize(HWND hwnd, IDssStreamFormat* pIFormat, DWORD nBuffers, DWORD BufferSize)
{
	if(!pIFormat) return E_POINTER;
	if(pIFormat->GetFormatTag() != FORMAT_AVI)
		return E_FAIL;

	IDssVideoStreamFormat* pIVideoSrcFormat;
	if (FAILED(pIFormat->QueryInterface(IID_IDssVideoStreamFormat, reinterpret_cast<void**>(&pIVideoSrcFormat))))
		return E_INVALID_FORMATS;

	m_hwnd = hwnd;
	m_nMaxBuffers = nBuffers;
	m_bStopped = false;

	// Get the bitmap info.
	m_pBitmapInfo = pIVideoSrcFormat->GetBitmapInfo();
	pIVideoSrcFormat->Release();

	m_pIEvent = 0;
/*
	// Create the buffer.
	for (int i = 0; i < nBuffers; ++i)
	{
		DIBHolder dibHolder;
		dibHolder.hBitmap = CreateDIBSection(GetDC(m_hwnd), m_pBitmapInfo, DIB_RGB_COLORS, (void**)&dibHolder.pBits, 0, 0);
		m_vBitmaps.PushBack(dibHolder);
	}
*/

	return S_OK;
}

void DestroyCodecEventInfo(CodecEventInfo* p)
{
	if(p != NULL)
	{
		delete p->pFrame;
		delete [] p->pSpectral;
		delete [] p->pWaveform;
		delete p;
	}
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
HRESULT STDCALL CAVIOutputStream::Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten)
{
	HRESULT hr = S_OK;
	// TODO: this

//#define DS2TO1
#ifdef DS2TO1
static bool bReject = false;
if (bReject)
{
bReject = false;
return S_OK;
}
else
bReject = true;
#endif	// DS2TO1

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

#if 0
		BYTE* pbBits;
		HBITMAP hBitmap = CreateDIBSection(GetDC(m_hwnd), m_pBitmapInfo, DIB_RGB_COLORS, (void**)&pbBits, 0, 0);
		if (hBitmap)
		{
			// If all buffers are full, then wait until a buffer is free or until playback is stopped.
			// TODO: Replace this with events.
			int nFilledBuffers = 0;
			do
			{
				m_cs.Lock();
				nFilledBuffers = m_lBitmaps.Size();
				m_cs.Unlock();
				if (nFilledBuffers < m_nMaxBuffers)
					break;
				Sleep(100);
			} while (!m_bStopped);
			memcpy(pbBits, pBuffer, dwBytes);
			*pdwBytesWritten = dwBytes;
			m_cs.Lock();
			DIBHolder temp;
			temp.iFrameIndex = pEventInfo->pFrame->CurrentFrame;
			temp.hBitmap = hBitmap;
//			m_lBitmaps.PushBack(hBitmap);
			m_lBitmaps.PushBack(temp);
			m_cs.Unlock();
		}
		else
			hr = S_FALSE;
#endif
	}
	else
	{
#if 0
		CLock cLock(&m_cs);
		if (m_lBitmaps.IsEmpty())
			hr = S_FALSE;
#endif
		if(m_pIEvent)
		{
			VideoFrameData vfd;
			vfd.dwDataSize = 0;
			vfd.pData = 0;
			return m_pIEvent->OnFrameData(&vfd);
		}
	}

#if 0
	CodecEventInfo* pInfo = NULL;
	if(pBuffer)
	{
		//Write the data to the next playback buffer.
		m_pWaveOut->Write(pBuffer,dwBytes,reinterpret_cast<DWORD>(pEventInfo));
		//Get notification information from last played buffer
		pInfo = reinterpret_cast<CodecEventInfo*>(m_pWaveOut->GetLastPlayed());
	}
	else 
	{
		//Get notification information from last played buffer
		pInfo = reinterpret_cast<CodecEventInfo*>(m_pWaveOut->GetLastPlayedUserInfo());
		//find out if all the buffers are done playing.
		if(m_pWaveOut->IsBufferEmpty())
			hr = S_FALSE;
	}
#endif

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
HRESULT STDCALL CAVIOutputStream::Advise(IDssEvent* pIEvent)
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


HRESULT STDCALL CAVIOutputStream::GetFormat(IDssStreamFormat** ppIFormat)
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

HRESULT STDCALL CAVIOutputStream::Stop()
{
	// TODO: this
	CLock cLock(&m_cs);
	m_bStopped = true;
//	m_pWaveOut->Reset();
	ClearBuffer();
	return S_OK;
}

HRESULT STDCALL CAVIOutputStream::Pause()
{
	// TODO: this
	CLock cLock(&m_cs);
//	m_pWaveOut->Pause();
	return S_OK;
}

HRESULT STDCALL CAVIOutputStream::Restart()
{
	// TODO: this
	CLock cLock(&m_cs);
	m_bStopped = false;
//	m_pWaveOut->Restart();
	return S_OK;
}

HRESULT STDCALL CAVIOutputStream::GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers)
{
	CLock cLock(&m_cs);
	*pdwTotalBuffers = m_nMaxBuffers;
	*pdwFilledBuffers = m_lBitmaps.Size();
	return S_OK;
}

HRESULT STDCALL CAVIOutputStream::SetVolume(WORD wLeftChannel, WORD wRightChannel)
{
	return S_FALSE;
}

HRESULT STDCALL CAVIOutputStream::GetVolume(WORD* pwLeftChannel, WORD* pwRightChannel)
{
	*pwLeftChannel = 0;
	*pwRightChannel = 0;

	return S_OK;
}

HBITMAP
CAVIOutputStream::GetNextBitmap()
{
	CLock cLock(&m_cs);
	if (!m_lBitmaps.IsEmpty())
	{
		DIBHolder temp = m_lBitmaps.PopFront();
		return temp.hBitmap;
	}
	else
		return 0;
}

DIBHolder
CAVIOutputStream::GetNextBitmapStruct()
{
	CLock cLock(&m_cs);
	if (!m_lBitmaps.IsEmpty())
		return m_lBitmaps.PopFront();
	else
	{
		DIBHolder empty = { 0, 0 };
		return empty;
	}
}

void
CAVIOutputStream::ClearBuffer()
{
	while (!m_lBitmaps.IsEmpty())
		DeleteObject(m_lBitmaps.PopFront().hBitmap);
}

