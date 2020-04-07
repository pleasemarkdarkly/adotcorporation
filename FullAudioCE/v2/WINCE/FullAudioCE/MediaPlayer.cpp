//........................................................................................
//........................................................................................
//.. File Name:  MyCollection.h
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: implementation of CMediaPlayer class.
//.. Usage: Controls main menu															..
//.. Last Modified By: Matthew Tadd mattt@iobjects.com						..	
//.. Modification date: 12/27/2000														..
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.  									..
//..	 All rights reserved. This code may not be redistributed in source or linkable  ..
//.. 	 object form without the express written consent of Interactive Objects.        ..
//.. Contact Information: www.iobjects.com												..
//........................................................................................
//........................................................................................
#include "stdafx.h"

#ifdef INCLUDE_DSS

#include <tchar.h>
#include "AppCrypt.h"
#include "MediaPlayer.h"
//#include "WaveOutStream.h"
//#include "MediaInterfaceDLL.h"
//#include "Debug.h"

extern CMediaPlayer mediaPlayer;
//extern IMFunctionTableV1P0*		g_pIMFunctionTable;

LPCTSTR aSong::pszEmpty = TEXT("");


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMediaPlayer::CMediaPlayer()
	: m_pEvent(NULL), m_nState(Stopped), m_uProgress(0), m_uWaveform(0), m_uSpectral(0)
{
	DssInitialize(Authenticator());
	DssGetCodecManager(&m_spIManager);
	m_pEvent = new CMediaPlayerEvent(NULL);
	IDssMediaPlayerEvent* pITmp;
	HRESULT hr = InternalCreateInstance(m_pEvent,&pITmp);

	//Make a player interface
	hr = m_spIManager->CreateMediaPlayer(&m_spIPlayer);
}

CMediaPlayer::~CMediaPlayer()
{
	if(m_pEvent) m_pEvent->Release();

	m_spIManager.Release();
	m_spIPlayer.Release();
	m_spICodec.Release();
	m_spIDriver.Release();
	m_spIOutStream.Release();
	m_spICodecInfo.Release();

	DssUninitialize();
}

HRESULT CMediaPlayer::SetSong(const aSong* pSong) //LPCTSTR szFileName)
{
	HRESULT hr;

	// Play Nice
	if (!pSong) return E_FAIL;

	// Open File
	IDssPtr<IDssInputStream> spIInStream;

	hr = m_spIManager->OpenInputStreamFromFile(pSong->GetFileName(), &spIInStream);
	if(FAILED(hr)) return hr;
	
	// Search for Driver
	IDssPtr<IDssSourceInfo> spISourceInfo;
	hr = m_spIManager->FindCompatibleDriver(spIInStream,&spISourceInfo);
	if(FAILED(hr)) return hr;
	
	// Query Driver handle
	IDssPtr<IDssDriver> spIDriver;
	hr = spISourceInfo->GetDriver(&spIDriver);
	if(FAILED(hr)) return hr;

	// Same Codec Type
#if 0
	if (static_cast<IDssDriver*>(m_spIDriver) == static_cast<IDssDriver*>(spIDriver))
	{
		//We've already got this driver...so let's check if we've got the same
		//source format, if so, then we don't need to reinitialize the codec.
		DWORD dwSrcIndex;
		hr = spISourceInfo->GetSrcIndex(&dwSrcIndex);
		if(FAILED(hr)) return hr;

		if(dwSrcIndex == m_dwSrcIndex)
		{
			//Initialize Codec
			m_spIPlayer->Stop();
			WaitForSingleObject(m_pEvent->m_hEvtStopped,INFINITE);
			ResetEvent(m_pEvent->m_hEvtStopped);
			m_spIPlayer->Uninitialize();
			hr = m_spICodec->Initialize(spIInStream,m_spIOutStream);
			m_spIPlayer->Initialize(m_spICodec);
			if(FAILED(hr)) return hr;
		}
	}
	else
#endif
	{
		//If we get here, then all our existing member data is obsolete.
		m_spIDriver = spIDriver;
		m_spICodec.Release();
		m_spIOutStream.Release();

		// Query src info for src index
		hr = spISourceInfo->GetSrcIndex(&m_dwSrcIndex);
		if(FAILED(hr)) return hr;

		// 
		hr = spISourceInfo->GetOutputFrameSize(&m_dwFrameSize);
		if(FAILED(hr)) return hr;
		
		// Get Source format
		IDssPtr<IDssStreamFormat> spISrcFormat;
		IDssPtr<IDssStreamFormat> spIDstFormat;
		spIDriver->GetSrcFormat(m_dwSrcIndex,&spISrcFormat);

		IDssPtr<IDssAudioStreamFormat> pIAudioSrcFormat;
		IDssPtr<IDssAudioStreamFormat> pIAudioDstFormat;
		if (FAILED(spISrcFormat->QueryInterface(IID_IDssAudioStreamFormat, reinterpret_cast<void**>(&pIAudioSrcFormat))))
			return E_INVALID_FORMATS;
		spISrcFormat.Release();

		// (Find the one that represents down sampling 2/1)
		DWORD dwDstIndex;
		bool bFound = false;
		spIDriver->GetNumDstFormats(m_dwSrcIndex, &dwDstIndex);
		for(DWORD d=0; d<dwDstIndex; d++)
		{
			spIDriver->GetDstFormat(m_dwSrcIndex,d,&spIDstFormat);
			if (FAILED(spIDstFormat->QueryInterface(IID_IDssAudioStreamFormat, reinterpret_cast<void**>(&pIAudioDstFormat))))
				continue;
			spIDstFormat.Release();
#if defined(_SH3_)
//This is for the Jornada 540 to get it to downsample 2 to 1 and play mono
			if( (pIAudioSrcFormat->GetFormatTag() == FORMAT_MP3) &&
				(pIAudioSrcFormat->GetBitsPerSample() == pIAudioDstFormat->GetBitsPerSample()) && 
				(1 == pIAudioDstFormat->GetChannels()) &&
				(pIAudioSrcFormat->GetSamplingFrequency() == (pIAudioDstFormat->GetSamplingFrequency() * 2))) 
			{
				bFound = true;
				break;
			}
			else if((pIAudioSrcFormat->GetFormatTag() != FORMAT_MP3) &&
				(pIAudioSrcFormat->GetBitsPerSample() == pIAudioDstFormat->GetBitsPerSample()) && 
				(pIAudioSrcFormat->GetChannels() == pIAudioDstFormat->GetChannels()) &&
				(pIAudioSrcFormat->GetSamplingFrequency() == (pIAudioDstFormat->GetSamplingFrequency())) )
			{
				bFound = true;
				break;
			}
#else
			if( (pIAudioSrcFormat->GetFormatTag() == FORMAT_MP3) &&
				(pIAudioSrcFormat->GetBitsPerSample() == pIAudioDstFormat->GetBitsPerSample()) && 
				(pIAudioSrcFormat->GetChannels() == pIAudioDstFormat->GetChannels()) &&
				(pIAudioSrcFormat->GetSamplingFrequency() == (pIAudioDstFormat->GetSamplingFrequency() * 2))) 
			{
				bFound = true;
				break;
			}
			else if(pIAudioSrcFormat->GetFormatTag() != FORMAT_MP3 &&
				(pIAudioSrcFormat->GetBitsPerSample() == pIAudioDstFormat->GetBitsPerSample()) && 
				(pIAudioSrcFormat->GetChannels() == pIAudioDstFormat->GetChannels()) &&
				(pIAudioSrcFormat->GetSamplingFrequency() == (pIAudioDstFormat->GetSamplingFrequency())) )
			{
				bFound = true;
				break;
			}
#endif
			else
				pIAudioDstFormat.Release();
		}
  
		if(!bFound)	return E_FAIL;

		// Tell player to release it's codec resources
		hr = m_spIPlayer->Uninitialize();
		if(FAILED(hr)) return hr;

		hr = m_spIDriver->CreateCodec(pIAudioSrcFormat,pIAudioDstFormat,&m_spICodec);
		if(FAILED(hr)) return hr;

		hr = m_spIManager->CreateWaveOutStream(pIAudioDstFormat,16,m_dwFrameSize,&m_spIOutStream);
		if(FAILED(hr))
		{
//			g_pIMFunctionTable->pfnPrintError(TEXT("CreateWaveOutStream failed"));
			return hr;
		}

		//And finally the codec.
		hr = m_spICodec->Initialize(spIInStream,m_spIOutStream);
		if(FAILED(hr))
		{
//			g_pIMFunctionTable->pfnPrintError(TEXT("Initialize failed\n"));
			return hr;
		}

		//stick codec into player interface
		hr = m_spIPlayer->Initialize(m_spICodec);
		m_spIPlayer->Advise(m_pEvent);
		m_spICodecInfo.Release();
		m_spIPlayer->GetCodecInfo(&m_spICodecInfo);
		if(FAILED(hr))	return hr;
	}

	// Reinstate notification callbacks
	IDssSourceInfo* pISourceInfo = pSong->m_spISourceInfo;
	if (pISourceInfo)
	{
		FrameInfo fi;
		if (SUCCEEDED(pISourceInfo->GetFrameInfo(&fi)))
		{
			TimeInfo ti;
			if (SUCCEEDED(pISourceInfo->GetDuration(&ti)) && (ti.Milliseconds > 0))
			{
				UINT uRate;

				uRate = m_uProgress * fi.NumFrames / ti.dwMilliseconds;
				if (uRate) m_spIPlayer->SetCallbackRate(oseFrameProgress, uRate);

				uRate = m_uWaveform * fi.NumFrames / ti.dwMilliseconds;
				if (uRate) m_spIPlayer->SetCallbackRate(oseWaveformVisualization, uRate); 

				uRate = m_uSpectral * fi.NumFrames / ti.dwMilliseconds;
				if (uRate) m_spIPlayer->SetCallbackRate(oseSpectralVisualization, uRate);
			}
		}
	}

	// If we are already playing, we should continue doing so
	if (Playing == m_nState) m_spIPlayer->Play();

	return S_OK;
}

HRESULT CMediaPlayer::Advise(IDssEvent *pIEvent, UINT uProgress, UINT uWaveform, UINT uSpectral)
{
	m_uProgress = uProgress;
	m_uWaveform = uWaveform;
	m_uSpectral = uSpectral;
	
	if(m_pEvent) m_pEvent->Release();
	m_pEvent = new CMediaPlayerEvent(pIEvent);
	IDssMediaPlayerEvent* pITmp;
	HRESULT hr = InternalCreateInstance(m_pEvent,&pITmp);
	if(FAILED(hr)) return hr;

	return m_spIPlayer->Advise(m_pEvent);
}

HRESULT CMediaPlayer::Play(bool bSync)
{
//Debug(TEXT("1"));
	m_nState = Playing;

	// Sync here

	return m_spIPlayer->Play();
}

HRESULT CMediaPlayer::Stop(bool bSync)
{
	m_nState = Stopped;

	if (bSync)
	{
		WaitForSingleObject(m_pEvent->m_hEvtStopped,INFINITE);
		ResetEvent(m_pEvent->m_hEvtStopped);
	}

	return m_spIPlayer->Stop();
}

HRESULT CMediaPlayer::Pause(bool bSync)
{
	m_nState = Paused;

	// Sync here

	return m_spIPlayer->Pause();
}

HRESULT CMediaPlayer::SetVolume(WORD wLeft, WORD wRight)
{
	if (m_spIOutStream)
	{
		return m_spIOutStream->SetVolume(wLeft, wRight);
	}

	return E_FAIL;
}

HRESULT CMediaPlayer::GetVolume(WORD& wLeft, WORD& wRight)
{
	if (m_spIOutStream)
	{
		return m_spIOutStream->GetVolume(&wLeft, &wRight);
	}

	return E_FAIL;
}

HRESULT CMediaPlayer::SetFrame(DWORD dwFrame)
{
//	m_spIPlayer->Stop();
//	WaitForSingleObject(m_pEvent->m_hEvtStopped,INFINITE);
//	ResetEvent(m_pEvent->m_hEvtStopped);

	HRESULT hReturn = m_spIPlayer->SetFrame(dwFrame);

	if (SUCCEEDED(hReturn))
	{
		// If we are already playing, we should continue doing so
		if (Playing == m_nState) m_spIPlayer->Play();
	}

	return hReturn;
}

HRESULT CMediaPlayer::GetFrameInfo(DWORD& FrameNo, DWORD& NumFrames)
{
	if(!m_spICodec) return E_POINTER;
	
	HRESULT hr;
	IDssPtr<IDssCodecInfo> spICodecInfo;
	hr = m_spICodec->GetCodecInfo(&spICodecInfo);
	if(FAILED(hr)) return hr;

	IDssPtr<IDssSourceInfo> spISourceInfo;
	hr = spICodecInfo->QueryInterface(IID_IDssSourceInfo,reinterpret_cast<void**>( &spISourceInfo ));
	if(FAILED(hr)) return hr;

	FrameInfo fi;
	hr = spISourceInfo->GetFrameInfo(&fi);
	if(FAILED(hr)) return hr;

	FrameNo = fi.CurrentFrame;
	NumFrames = fi.NumFrames;

	return S_OK;
}

#endif //INCLUDE_DSS