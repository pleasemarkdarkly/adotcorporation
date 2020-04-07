//........................................................................................
//........................................................................................
//.. File Name: MediaPlayer.h
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content:  High-level media player interface to DSS.
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

#if !defined(AFX_MEDIAPLAYER_H__C2738A8F_5A2D_47F8_9463_2266010A4703__INCLUDED_)
#define AFX_MEDIAPLAYER_H__C2738A8F_5A2D_47F8_9463_2266010A4703__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef INCLUDE_DSS

#include "Dss.h"
//#include "Debug.h"
#include <stdlib.h>

class aSong
{
	friend class CMediaPlayer;

public:
	static HRESULT GetSong(LPCTSTR pszFileName, aSong*& pSong)
	{
		IDssPtr<IDssCodecManager> spIManager;
		DssGetCodecManager(&spIManager);

		// Create Input Stream
		IDssPtr<IDssInputStream> spIInStream;
		HRESULT hr = spIManager->OpenInputStreamFromFile(pszFileName, &spIInStream);
		if(SUCCEEDED(hr))
		{	
			// Find a Compatible Driver
			IDssPtr<IDssSourceInfo> spISourceInfo;
			hr = spIManager->FindCompatibleDriver(spIInStream, &spISourceInfo);
			if(SUCCEEDED(hr))
			{	
				IDssPtr<IDssStreamFormat> spISrcFormat;
				hr = spIInStream->GetFormat(&spISrcFormat);
				if(SUCCEEDED(hr))
				{
					IDssPtr<IDssAudioStreamFormat> pIAudioSrcFormat;
					if (FAILED(spISrcFormat->QueryInterface(IID_IDssAudioStreamFormat, reinterpret_cast<void**>(&pIAudioSrcFormat))))
						return E_INVALID_FORMATS;
					IDssPtr<IDssDictionary> spIDictionary;
					spISourceInfo->GetFileInfoMap(&spIDictionary);
					
					// Access Named Input Values
					pSong = new aSong(pszFileName, spISourceInfo, pIAudioSrcFormat, spIDictionary);
				}
			}
		}

		return hr;
	}

	~aSong()
	{ if (m_pszFileName) free(m_pszFileName); }

	LPCTSTR GetFileName() const
	{ return m_pszFileName; }

	LPCTSTR GetArtist() const
	{
		LPCTSTR pszValue;
		if (!m_spIDictionary || FAILED(m_spIDictionary->FindEntry(TEXT("Artist"), &pszValue)))
			return pszEmpty;
		return pszValue;
	}

	LPCTSTR GetAlbum() const
	{
		LPCTSTR pszValue;
		if (!m_spIDictionary || FAILED(m_spIDictionary->FindEntry(TEXT("Album"), &pszValue)))
			return pszEmpty;
		return pszValue;
	}

	LPCTSTR GetTitle() const
	{
		LPCTSTR pszValue;
		if (!m_spIDictionary || FAILED(m_spIDictionary->FindEntry(TEXT("Title"), &pszValue)))
			return pszEmpty;
		return pszValue;
	}

	// Dictionary-access functions.
	HRESULT DictionaryCount(DWORD* pdwNumElements)
	{
		if (m_spIDictionary)
			return m_spIDictionary->Count(pdwNumElements);
		else
			return E_FAIL;
	}
	HRESULT DictionaryGetEntry(DWORD dwIndex, LPCTSTR* ppszKey, LPCTSTR* ppszValue)
	{
		if (m_spIDictionary)
			return m_spIDictionary->GetEntry(dwIndex, ppszKey, ppszValue);
		else
			return E_FAIL;
	}
	HRESULT DictionaryAddEntry(LPCTSTR szKey, LPCTSTR szValue)
	{
		if (m_spIDictionary)
			return m_spIDictionary->AddEntry(szKey, szValue);
		else
			return E_FAIL;
	}
	HRESULT DictionaryFindEntry(LPCTSTR szKey, LPCTSTR* ppszValue)
	{
		if (m_spIDictionary)
			return m_spIDictionary->FindEntry(szKey, ppszValue);
		else
			return E_FAIL;
	}

	WORD GetChannels() const
	{ return (!m_spISrcFormat) ? 0 : m_spISrcFormat->GetChannels(); }

	WORD GetBitsPerSample() const
	{ return (!m_spISrcFormat) ? 0 : m_spISrcFormat->GetBitsPerSample(); }

	DWORD GetSamplingFrequency() const
	{ return (!m_spISrcFormat) ? 0 : m_spISrcFormat->GetSamplingFrequency(); }

	DWORD GetBytesPerSec() const
	{ return (!m_spISrcFormat) ? 0 : m_spISrcFormat->GetBytesPerSec(); }

	DWORD GetTotalFrames() const
	{ 
		if (m_spISourceInfo)
		{
			FrameInfo fi;
			m_spISourceInfo->GetFrameInfo(&fi);
			return fi.NumFrames;
		}

		return 0;
	}

	DWORD GetDuration() const
	{
		if (m_spISourceInfo)
		{
			TimeInfo ti;
			m_spISourceInfo->GetDuration(&ti);
			return ti.dwMilliseconds;
		}
		return 0;
	}

private:
	aSong(LPCTSTR pszFileName, IDssSourceInfo* pISourceInfo, IDssAudioStreamFormat* pISrcFormat, IDssDictionary* pIDictionary)
		: m_pszFileName(NULL), m_spISourceInfo(pISourceInfo), m_spISrcFormat(pISrcFormat), m_spIDictionary(pIDictionary)
	{ if (pszFileName) m_pszFileName = _tcsdup(pszFileName); }

	LPTSTR m_pszFileName;
	
	mutable IDssPtr<IDssSourceInfo> m_spISourceInfo; // Not used presently
	mutable IDssPtr<IDssAudioStreamFormat> m_spISrcFormat;
	mutable IDssPtr<IDssDictionary> m_spIDictionary;

	static LPCTSTR pszEmpty;
};



class CMediaPlayerEvent : public CUnknown, public IDssMediaPlayerEvent, public IDssCodecEvent, public IDssAudioOutputStreamEvent
{
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(CMediaPlayerEvent)

	CMediaPlayerEvent(IDssEvent* pINotify) 
	{
		if (pINotify)
		{
			pINotify->QueryInterface(IID_IDssMediaPlayerEvent, reinterpret_cast<void**>(&m_spIMediaPlayerNotify));
			pINotify->QueryInterface(IID_IDssAudioOutputStreamEvent, reinterpret_cast<void**>(&m_spIAudioNotify));
		}
	}


	virtual HRESULT FinalConstruct()
	{
		m_hEvtStopped = CreateEvent(NULL,TRUE,FALSE,NULL);
		if(!m_hEvtStopped) return E_FAIL;

		return S_OK;
	}

	virtual void FinalRelease()
	{
		CloseHandle(m_hEvtStopped);
	}

	HRESULT STDCALL NondelegatingQueryInterface(const IID& iid, void **ppv)
	{
		if(IsEqualGuid(iid,IID_IDssMediaPlayerEvent))
		{
			IDssMediaPlayerEvent* pI = static_cast<IDssMediaPlayerEvent*>(this);
			pI->AddRef();
			*ppv = pI;
			return S_OK;
		}
		else if(IsEqualGuid(iid,IID_IDssCodecEvent))
		{
			IDssCodecEvent* pI = static_cast<IDssCodecEvent*>(this);
			pI->AddRef();
			*ppv = pI;
			return S_OK;
		}
		else if(IsEqualGuid(iid,IID_IDssAudioOutputStreamEvent))
		{
			IDssAudioOutputStreamEvent* pI = static_cast<IDssAudioOutputStreamEvent*>(this);
			pI->AddRef();
			*ppv = pI;
			return S_OK;
		}
		else
			return CUnknown::NondelegatingQueryInterface(iid,ppv);
	}

	virtual void STDCALL OnFrameProgress(/*in*/ FrameInfo* pInfo)
	{
		if(m_spIAudioNotify)
			m_spIAudioNotify->OnFrameProgress(pInfo);
	}
	virtual void STDCALL OnVisualization(/*in*/ VisStruct* pVis)
	{
		if(m_spIAudioNotify)
			m_spIAudioNotify->OnVisualization(pVis);
	}
	virtual void STDCALL OnCommandComplete(int Command,DWORD Param,HRESULT Result)
	{
		if(Command == CMD_STOP)
			SetEvent(m_hEvtStopped);
		if(m_spIMediaPlayerNotify)
			m_spIMediaPlayerNotify->OnCommandComplete(Command,Param,Result);
	}
	virtual void STDCALL OnFileEnd()
	{
		if(m_spIMediaPlayerNotify)
			m_spIMediaPlayerNotify->OnFileEnd();
	}

	IDssPtr<IDssMediaPlayerEvent> m_spIMediaPlayerNotify;
	IDssPtr<IDssAudioOutputStreamEvent> m_spIAudioNotify;
	HANDLE m_hEvtStopped;
};

class CMediaPlayer  
{
public:
	enum {Stopped, Paused, Playing};

	CMediaPlayer();
	virtual ~CMediaPlayer();

	int GetState() {return m_nState;}

	HRESULT SetVolume(WORD wLeft, WORD wRight);
	HRESULT GetVolume(WORD& wLeft, WORD& wRight);


	HRESULT Advise(IDssEvent *pIEvent, UINT uProgress, UINT uWaveform, UINT uSpectral);

	HRESULT SetSong(const aSong* pSong);

	//Decoding Management
	virtual HRESULT Play(bool bSync = false); 
	virtual HRESULT Pause(bool bSync = false);
	virtual HRESULT Stop(bool bSync = false);
	virtual HRESULT SetFrame(DWORD Frame);
	virtual HRESULT GetFrameInfo(DWORD& FrameNo, DWORD& NumFrames);

private:
	IDssPtr<IDssCodecManager> m_spIManager;
	IDssPtr<IDssMediaPlayer> m_spIPlayer;
	IDssPtr<IDssCodec> m_spICodec;
	IDssPtr<IDssDriver> m_spIDriver;
	IDssPtr<IDssAudioOutputStream> m_spIOutStream;
	DWORD m_dwFrameSize;
	DWORD m_dwSrcIndex;
	DWORD m_dwDstIndex;
	//TODO : Adding appropriate member data members to keep track of last
	//status of player, so we can be more smart about initializing
	//codecs and what not between songs.
	//IDssPtr<IDssCodec> m_spICodec;
	
	IDssPtr<IDssCodecInfo> m_spICodecInfo;
	CMediaPlayerEvent* m_pEvent;

	int m_nState;
	
	UINT m_uProgress;
	UINT m_uWaveform;
	UINT m_uSpectral;
};

#endif //INCLUDE_DSS

#endif // !defined(AFX_MEDIAPLAYER_H__C2738A8F_5A2D_47F8_9463_2266010A4703__INCLUDED_)
