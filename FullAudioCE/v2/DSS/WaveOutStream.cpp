// WaveOutStream.cpp: implementation of the CWaveOutStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveOutStream.h"
#include "StructHelper.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CWaveOutStream::FinalRelease()
{
	CUnknown::FinalRelease();
	if(m_pIFormat)
		m_pIFormat->Release();
	if(m_pIEvent)
		m_pIEvent->Release();
	if(m_pWaveOut)
		m_pWaveOut->Reset();
	delete m_pWaveOut;
}

HRESULT CWaveOutStream::Initialize(IDssAudioStreamFormat* pIFormat, DWORD nBuffers, DWORD BufferSize)
{
	if(!pIFormat) return E_POINTER;
	if(pIFormat->GetFormatTag() != FORMAT_PCM)
		return E_FAIL;

	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.wBitsPerSample = pIFormat->GetBitsPerSample();
	wfx.nSamplesPerSec = pIFormat->GetSamplingFrequency();
	wfx.nChannels = pIFormat->GetChannels();
	wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	m_pWaveOut = new CWaveOut;
	if(!m_pWaveOut) return E_OUTOFMEMORY;
	if(SUCCEEDED(m_pWaveOut->Open(&wfx, nBuffers, BufferSize)))
	{
		m_pIFormat = pIFormat;
		pIFormat->AddRef();
		return S_OK;
	}
	else
	{
		delete m_pWaveOut;
		return E_FAIL;
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
HRESULT STDCALL CWaveOutStream::Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten)
{
	HRESULT hr = S_OK;
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

	//Fire Event Notification
	if(pInfo)
	{
		if(m_pIEvent)
		{
			if(pInfo->pFrame)
				m_pIEvent->OnFrameProgress(pInfo->pFrame);
			if(pInfo->pWaveform || pInfo->pSpectral)
				m_pIEvent->OnVisualization(pInfo);
		}
		DestroyCodecEventInfo(pInfo);
	}

	return hr;
}

//IOOutputStream Event Management
HRESULT STDCALL CWaveOutStream::Advise(IDssEvent* pIEvent)
{
	if(pIEvent == NULL) return E_POINTER;

	IDssAudioOutputStreamEvent* pIAudioNotify;
	if (FAILED(pIEvent->QueryInterface(IID_IDssAudioOutputStreamEvent, reinterpret_cast<void**>(&pIAudioNotify))))
		return E_INVALID_EVENT;

	if(m_pIEvent)
		m_pIEvent->Release();

	m_pIEvent = pIAudioNotify;

	return S_OK;
}


HRESULT CWaveOutStream::SetFormat(IDssStreamFormat* pIFormat)
{
	if(!pIFormat) return E_POINTER;
	if(m_pIFormat) m_pIFormat->Release();
	// TODO: verify this is an audio stream format.
	m_pIFormat = (IDssAudioStreamFormat*)pIFormat;
	pIFormat->AddRef();
	return S_OK;
}

HRESULT STDCALL CWaveOutStream::GetFormat(IDssStreamFormat** ppIFormat)
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

HRESULT STDCALL CWaveOutStream::Stop()
{
	m_pWaveOut->Reset();
	return S_OK;
}

HRESULT STDCALL CWaveOutStream::Pause()
{
	m_pWaveOut->Pause();
	return S_OK;
}

HRESULT STDCALL CWaveOutStream::Restart()
{
	m_pWaveOut->Restart();
	return S_OK;
}

HRESULT STDCALL CWaveOutStream::GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers)
{
	return m_pWaveOut->GetBufferInfo(pdwFilledBuffers,pdwTotalBuffers);
}

HRESULT STDCALL CWaveOutStream::SetVolume(WORD wLeftChannel, WORD wRightChannel)
{
	HWAVEOUT hWave;
#if defined(UNDER_CE) && defined(MIPS)
	//Cassiopeia
	hWave = 0;
#else
	hWave = m_pWaveOut->m_hWave;
#endif
	
	waveOutSetVolume(hWave,((DWORD)wRightChannel << 16) | (DWORD)wLeftChannel);
	return S_OK;
}

HRESULT STDCALL CWaveOutStream::GetVolume(WORD* pwLeftChannel, WORD* pwRightChannel)
{
	HWAVEOUT hWave;
#if defined(UNDER_CE) && defined(MIPS)
	//Cassiopeia
	hWave = 0;
#else
	hWave = m_pWaveOut->m_hWave;
#endif
	DWORD dwVolume;
	waveOutGetVolume(hWave,&dwVolume);
	*pwLeftChannel = dwVolume & 0xFFFF;
	*pwRightChannel = (dwVolume >> 16) & 0xFFFF;
	return S_OK;
}

