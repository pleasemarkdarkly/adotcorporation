//........................................................................................
//........................................................................................
//.. File Name:  MyPlayer.cpp
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: implementation of MyPlayer class.
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

#include "FullAudioCE.h"
#include "regkey.h"
#include "MyPlayer.h"
#include "AppCrypt.h"
#include "PopupMenu.h"

#ifdef USE_WMA_STANDALONE
#include "CWaveOutCE.h"
#ifdef _ARM_
#include "WMA_Dec_WinCE_ARM.h"
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern HINSTANCE hInst;
extern HWND hwndMain;
extern HMENU hMenuCBPlayer;

#define LOGO_PLAYING_LEFT	148
#define LOGO_STOPPED_LEFT	74

CMyPlayer::CMyPlayer(HWND hWnd) : _hwnd(hWnd)
{
#ifdef USE_DSS
	DssInit();
#endif
#ifdef USE_WMA_STANDALONE
	WMAInit();
#endif
}

CMyPlayer::~CMyPlayer()
{
#ifdef USE_DSS
	DssCleanup();
#endif
#ifdef USE_WMA_STANDALONE
	WMACleanup();
#endif
}



//message processors
void CMyPlayer::Draw(HDC hdc, PAINTSTRUCT& ps)
{
	if(_bShow)
	{
		HDC hdcMem;
		HBITMAP hbmp;
		HGDIOBJ hold;
		hdcMem = CreateCompatibleDC(hdc);
		hbmp = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FALOGO));
		hold = SelectObject(hdcMem,hbmp);
		BitBlt(hdc, LOGO_STOPPED_LEFT , _rect.top, 92, 40, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem,hold);
		DeleteObject(hbmp);
		DeleteDC(hdcMem);
	}
	else
	{
		FillRect(hdc,&_rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
	}
}

void CMyPlayer::LButtonDown(const POINT& pt)
{
	if(!_bShow)
	{
		if(_bPlaying)
		{
			int iMenuId = HandlePopupMenu(hMenuCBPlayer,_hwnd,pt);
			if(iMenuId)
				PostMessage(_hwnd,WM_COMMAND,iMenuId,0);
		}
		else
			PostMessage(_hwnd,WM_USER_TOGGLE_PLAYER,0,0);
	}
	else
	{
		PostMessage(_hwnd,WM_USER_TOGGLE_PLAYER,0,0);
	}
}

#ifdef USE_DSS

#define DSSMSG(xx) Messag
//////////////////////////////////////////////////////////////////////////////////////////////
//	Media player event callback class
////////////////////////////////////////////////////////////////////////////////////////////////
#include "MediaPlayer.h"

class CEvent : public CUnknown,  public IDssMediaPlayerEvent, public IDssAudioOutputStreamEvent
{
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssMediaPlayerEvent)

		CEvent(DWORD dwThreadId) : m_dwThreadId(dwThreadId) {};
	virtual ~CEvent() {};

	virtual HRESULT FinalConstruct()
	{
		if(!s_hEvtDone)
			s_hEvtDone = CreateEvent(NULL,TRUE,FALSE,NULL);
		return S_OK;
	}

	virtual void FinalRelease()
	{
		if(s_hEvtDone)
			CloseHandle(s_hEvtDone);
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
//		DebugString(TEXT("Frame %d/%d\n"),pInfo->CurrentFrame,pInfo->NumFrames);
//		g_pTrackPos->SetValue(pInfo->CurrentFrame);
//		SetTrackTime(pInfo->CurrentFrame);
	}
	virtual void STDCALL OnVisualization(/*in*/ VisStruct* pVis)
	{
		if(pVis->pWaveform)
		{
//			DebugString(TEXT("Waveform: %d, %d, %x\n"),pVis->Channels,pVis->dwWaveformElementsPerChannel,pVis->pWaveform);
		}
		if(pVis->pSpectral)
		{
//			DebugString(TEXT("Spectral: %d, %d, %d, %d, %x\n"),pVis->Channels,pVis->dwSpectralElementsPerChannel,pVis->dwSpectralDim1,pVis->dwSpectralDim2,pVis->pSpectral);
		}
	}
	virtual void STDCALL OnCommandComplete(int Command,DWORD Param,HRESULT Result)
	{
//		Debug(TEXT("Command %d,%d,%d\n"),Command,Param,Result);
		switch (Command)
		{
			PostThreadMessage(m_dwThreadId,WM_USER_UPDATE,0,0);
			case CMD_STOP:
//				ControlText::EnableMarqueeScrolling();
//				g_pStopII->SetCurrentImage();
//				g_pTrackPos->SetValue(0);
//				SetTrackTime(0);
				SetEvent(s_hEvtDone);
				PostThreadMessage(m_dwThreadId,WM_USER_STOPPED,0,0);
				break;

			case CMD_PLAY:
//				ControlText::DisableMarqueeScrolling();
//				g_pPlayII->SetCurrentImage();
				PostThreadMessage(m_dwThreadId,WM_USER_PLAYING,0,0);
				break;

			case CMD_PAUSE:
//				ControlText::EnableMarqueeScrolling();
//				g_pPauseII->SetCurrentImage();
				break;

			case CMD_SETFRAME:
//				g_pTrackPos->SetValue(Param);
//				SetTrackTime(Param);
				break;
		}
	}
	virtual void STDCALL OnFileEnd()
	{
//		Debug(TEXT("File End\n"));
//		g_pIMFunctionTable->pfnPrintError(TEXT("File end"));
//		g_bContinuePlay = true;
		SetEvent(s_hEvtDone);
//		g_pIMFunctionTable->pfnNotifyTrackEnd();
//		g_bContinuePlay = false;
		PostThreadMessage(m_dwThreadId,WM_USER_FILE_END,0,0);
	}
	static HANDLE s_hEvtDone;
	DWORD m_dwThreadId;
};

HANDLE CEvent::s_hEvtDone = NULL;

static CMediaPlayer* s_pMediaPlayer = NULL;
static CEvent* s_pEvent = NULL;
static aSong* s_pCurrentSong = NULL;

void CMyPlayer::Play()
{
	s_pMediaPlayer->Play();
}

void CMyPlayer::Pause()
{
	if (s_pMediaPlayer->GetState() == CMediaPlayer::Playing)
		s_pMediaPlayer->Pause();
	else if (s_pMediaPlayer->GetState() == CMediaPlayer::Paused)
		s_pMediaPlayer->Play();
}

bool CMyPlayer::IsStopped()
{
	return s_pMediaPlayer->GetState() != CMediaPlayer::Playing;
}

void CMyPlayer::Stop()
{
	s_pMediaPlayer->Stop();
}

bool CMyPlayer::SetSong(const char* szFileName)
{
	TCHAR wszFileName[MAX_PATH];

	MultiByteToWideChar(CP_ACP,0,szFileName,-1,wszFileName,MAX_PATH);

	if(_tcscmp(wszFileName,_SelectedSong) != 0)
	{
		aSong*	pNewSong;
		HRESULT hr = aSong::GetSong(wszFileName, pNewSong);
		if(FAILED(hr))
		{
	//		g_pIMFunctionTable->pfnPrintError(TEXT("Failed to open file %s."), szFileName);
			return false;
		}

		// Delete the current song pointer and set the new song pointer.
		delete s_pCurrentSong;
		s_pCurrentSong = pNewSong;
		hr = s_pMediaPlayer->SetSong(s_pCurrentSong);
		if (FAILED(hr))
		{
			//g_pIMFunctionTable->pfnPrintError(TEXT("Failed to set song %s."), szFileName);
			return false;
		}
		
		_tcscpy(_SelectedSong,wszFileName);
	}
//	if (g_bContinuePlay)
	if (s_pMediaPlayer->GetState() == CMediaPlayer::Playing)
	{
//		Debug(TEXT("Continuing play"));
		s_pMediaPlayer->Stop();
		s_pMediaPlayer->Play();
	}
	else
		// Hack to shut down the wave out device.
		s_pMediaPlayer->Stop();

	return true;
}


void CMyPlayer::DssInit()
{
	{
		CRegKey rk;
		if(ERROR_SUCCESS == rk.Open(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\iobjects\\DSSCodecs")))
		{
			rk.DeleteSubKey(TEXT("MP3Codec"));
			rk.DeleteSubKey(TEXT("WMACodec"));
		}
	}

	CRegKey rkDSS;
	rkDSS.Create(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\iobjects\\DSSCodecs"));
	
#ifdef INCLUDE_MP3
	CRegKey rkMP3;
	rkMP3.Create(rkDSS,TEXT("MP3Codec"));
	rkMP3.SetValue(TEXT("\\Windows\\MP3Codec.dll"),TEXT("Dll"));
	rkMP3.SetValue(TEXT("MP3 CE Decoder"),TEXT("Name"));
	rkMP3.SetValue(TEXT("Interactive Objects v2.3"),TEXT("Version"));
	rkMP3.SetValue(TEXT("*.MP3"),TEXT("Extension"));
#endif

#ifdef INCLUDE_WMA
	CRegKey rkWMA;
	rkWMA.Create(rkDSS,TEXT("WMACodec"));
	rkWMA.SetValue(TEXT("\\Windows\\WMACodec.dll"),TEXT("Dll"));
	rkWMA.SetValue(TEXT("WMA CE Decoder"),TEXT("Name"));
	rkWMA.SetValue(TEXT("Microsoft"),TEXT("Version"));
	rkWMA.SetValue(TEXT("*.WMA"),TEXT("Extension"));
#endif

	s_pMediaPlayer = new CMediaPlayer;
	s_pEvent = new CEvent(GetCurrentThreadId());
	s_pEvent->FinalConstruct();
	s_pMediaPlayer->Advise(s_pEvent, 100, 0, 0);

}


void CMyPlayer::DssCleanup()
{
	ResetEvent(CEvent::s_hEvtDone);
	s_pMediaPlayer->Stop();
	WaitForSingleObject(CEvent::s_hEvtDone, 3000);

	s_pEvent->FinalRelease();
	delete s_pEvent;
	delete s_pMediaPlayer;
}

#endif


#ifdef USE_WMA_STANDALONE
#include "WMA_Dec_WinCE_ARM.h"

tWMAFileHdrState g_hdrstate;
#if defined(_ARM_)
	tWMAFileState g_realstate;
	tWMAFileState* g_state = &g_realstate;
#	define tHWMAFileState tWMAFileState
#else
	tHWMAFileState g_state;
#endif
tWMAFileHeader g_hdr;
tWMAFileLicParams g_lic;

WAVEFORMATEX g_wfx;

const int MAX_BUFSIZE = WMA_MAX_DATA_REQUESTED;
unsigned char g_pBufLic[WMA_MAX_DATA_REQUESTED];
static unsigned char g_pBuffer[WMA_MAX_DATA_REQUESTED];

#define TEST_INTERLEAVED_DATA

// should work with any buffer size.  No point in it being larger than 2048.
//#define MAX_SAMPLES 256
//#define MAX_SAMPLES 2003
#define MAX_SAMPLES 2048
//#define MAX_SAMPLES 16384

#ifdef TEST_INTERLEAVED_DATA
	short g_pLeft [MAX_SAMPLES * 2];
	short *g_pRight = NULL;
#else
	short g_pLeft [MAX_SAMPLES];
	short g_pRight [MAX_SAMPLES];
#endif

#define STRING_SIZE 256
unsigned char g_szTitle[STRING_SIZE];
unsigned char g_szAuthor[STRING_SIZE];
unsigned char g_szCopyright[STRING_SIZE];
unsigned char g_szDescription[STRING_SIZE];
unsigned char g_szRating[STRING_SIZE];

unsigned char g_pmid[20] =
{
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x4d, 0x20,
    0x33, 0x54, 0x31, 0x30, 0x30, 0x35, 0x30, 0x30, 0x39, 0x35
};

static HANDLE s_hLicenseFile = INVALID_HANDLE_VALUE;
static HANDLE s_hFile = INVALID_HANDLE_VALUE;


#define DEBUG_WMA(xx)		MessageBox(NULL,TEXT(xx),NULL,MB_OK);
//#define DEBUG_WMA(xx)		

extern "C" tWMA_U32
WMAFileCBGetData(tHWMAFileState *state,
                 tWMA_U32 offset,
                 tWMA_U32 num_bytes,
                 unsigned char **ppData)
{
    tWMA_U32 nWanted = num_bytes <= (tWMA_U32) MAX_BUFSIZE ? num_bytes : (tWMA_U32) MAX_BUFSIZE;
    if(num_bytes != nWanted)
    {
        //TCHAR szMsg[500];
        //wsprintf( szMsg, TEXT("** WMAFileCBGetData: Requested too much (%lu != %lu).\n"), num_bytes, nWanted );
        //ASSERTMSG( szMsg, (num_bytes == nWanted) );
		DEBUG_WMA("WMAFileCBGetData: Requested too much");
    }

	DWORD dwBytesRead;
	SetFilePointer(s_hFile,offset,NULL,FILE_BEGIN);
	ReadFile(s_hFile,(LPVOID)g_pBuffer,nWanted,&dwBytesRead,NULL);

    //g_cbBuffer = ret;
    *ppData = g_pBuffer;
	return dwBytesRead;	
}


/* WMAFileCBGetLicenseData */
extern "C" tWMA_U32 WMAFileCBGetLicenseData(tHWMAFileState *state, tWMA_U32 offset, tWMA_U32 num_bytes, unsigned char **ppData)
{
    tWMA_U32        dwBytesRead;
    unsigned char  *g_plicData;
    tWMA_U32        iLicenseLength;
    tWMA_U32        nWanted;
    int			 g_cbBufLic;

    //log that we are in this function
    //WMADebugMessage("!! WMAFileCBGetLicenseData ptrState : %d offset : %d numb_bytes %d ppData %d\n",state,offset,num_bytes,ppData);

    g_plicData = WMAGetLicenseStore(&g_hdrstate, &iLicenseLength);

    nWanted = num_bytes <= (tWMA_U32) MAX_BUFSIZE ? num_bytes : (tWMA_U32) MAX_BUFSIZE;
  
	if(num_bytes != nWanted)
    {
       // WMADebugMessage( "** WMAFileCBGetLicenseData: Requested too much (%lu).\n",
       //         num_bytes);
		DEBUG_WMA("WMAFileCBGetLicenseData: Requested too much");
    }

    if (iLicenseLength) 
	{
        g_cbBufLic = ((offset +nWanted) > iLicenseLength) ? (iLicenseLength - offset): nWanted;
        dwBytesRead = g_cbBufLic;
        *ppData = g_plicData + offset;
    }
    else 
	{
        SetFilePointer(s_hLicenseFile,offset,NULL,FILE_BEGIN);
		ReadFile(s_hLicenseFile,(LPVOID)g_pBufLic,nWanted,&dwBytesRead,NULL);
        g_cbBufLic = dwBytesRead;
        *ppData = g_pBufLic;
    }

    // show how much we are returning
//    WMADebugMessage( "++ WMAFileCBGetLicenseData: %lu bytes from %lu.\n",
//            ret, offset);

    return dwBytesRead;
}

DWORD WINAPI WMADecoderThread(LPVOID pParam)
{
	DWORD hMainThreadId = (DWORD)pParam;
	tWMAFileStatus rc;	
	MSG msg;

	static CWMAWaveOut WaveOut;
	WaveOut.Open(&g_wfx,16,MAX_SAMPLES);
	//force thread to receive messages
	PeekMessage(&msg,NULL,0,0,PM_NOREMOVE);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if(msg.message == WM_USER_PLAY)
		{
			WaveOut.Open(&g_wfx,16,MAX_SAMPLES);

			do
			{
				rc = WMAFileDecodeData (g_state);
				if(rc != cWMA_NoErr)
				{
					if ( rc == cWMA_NoMoreFrames || rc == cWMA_Failed )
						PostThreadMessage(hMainThreadId,WM_USER_FILE_END,0,0);
					break;
				}
				
				do
				{
					tWMA_U32 num_samples;
					
#ifdef _ARM_
					if ( g_hdr.num_channels == 1 ) {
						num_samples = WMAFileGetPCM (g_state, g_pLeft, g_pLeft, MAX_SAMPLES);
					} else {
						num_samples = WMAFileGetPCM (g_state, g_pLeft, g_pRight, MAX_SAMPLES);
					}
#else
					num_samples = WMAFileGetPCM (g_state, g_pLeft, g_pRight, MAX_SAMPLES);
#endif
					if (num_samples == 0)
					{
						/* no more, so on with the decoding... */
						break;
					}
#ifdef TEST_INTERLEAVED_DATA
				  WaveOut.WriteInterleaved((PBYTE)g_pLeft,num_samples*2);
#else	// TEST_INTERLEAVED_DATA

#endif	// TEST_INTERLEAVED_DATA
				} while (1);
			} while (!PeekMessage(&msg,NULL,0,0,PM_NOREMOVE));
			//ProfStop(L"mydata.dat");

			while(!WaveOut.IsPlaybackComplete())
				Sleep(10);

			WaveOut.Destroy();
		}
	}
	return 0;
}

void CMyPlayer::WMAInit()
{
	_hThread = CreateThread(NULL,0,WMADecoderThread,GetCurrentThread(),0,&_dwThreadId);
	while(!PostThreadMessage(_dwThreadId,WM_USER_STOP,0,0));
}

void CMyPlayer::WMACleanup()
{
	PostThreadMessage(_dwThreadId,WM_QUIT,0,0);
	CloseHandle(_hThread);

#ifdef _ARM_
	WMAFileDecodeClose (g_state);
#else
	WMAFileDecodeClose (&g_state);
#endif
	CloseHandle(s_hFile);
	CloseHandle(s_hLicenseFile);
}

void CMyPlayer::Play()
{
	PostThreadMessage(_dwThreadId,WM_USER_PLAY,0,0);
}

void CMyPlayer::Stop()
{
	if(PostThreadMessage(_dwThreadId,WM_USER_STOP,0,0))
	{
		WaitForSingleObject(_hThread,INFINITE);
	}
}

bool CMyPlayer::SetSong(const char* szFileName)
{
	TCHAR wszFileName[MAX_PATH];
    tWMAFileContDesc desc;

	if(PostThreadMessage(_dwThreadId,WM_USER_STOP,0,0))
	{
		WaitForSingleObject(_hThread,INFINITE);
	}

	MultiByteToWideChar(CP_ACP,0,szFileName,-1,wszFileName,MAX_PATH);

	CloseHandle(s_hFile);
	s_hFile = CreateFile(wszFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if(s_hFile == INVALID_HANDLE_VALUE)
	{
		DEBUG_WMA("File Not Found");
	}

	tWMAFileStatus rc;	
	rc = WMAFileIsWMA (&g_hdrstate);
    if(rc != cWMA_NoErr)
    {
        //wsprintf( szMsg, TEXT("** The file is not a WMA file. %s.\n"), g_szWMAFile );
        // MessageBox(hwndMe, szMsg, _T("WMADecSH4"), MB_OK);
        //ASSERTMSG( szMsg, (rc == cWMA_NoErr) );
        //iRetValue = 3;
        return false;
    }

#ifdef _ARM_
    rc = WMAFileDecodeInit (g_state);
#else
    rc = WMAFileDecodeInit (&g_state);
#endif
    if(rc != cWMA_NoErr)
    {
        //wsprintf( szMsg, TEXT("** Cannot initialize the WMA decoder.\n") );
        // MessageBox(hwndMe, szMsg, _T("WMADecSH4"), MB_OK);
        //ASSERTMSG( szMsg, (rc == cWMA_NoErr) );
        //iRetValue = 4;
        //goto lexit;
		return false;
    }

    /* get header information */

    rc = WMAFileDecodeInfo (g_state, &g_hdr);
    if(rc != cWMA_NoErr)
    {
        //wsprintf( szMsg, TEXT("** Failed to retrieve information.\n") );
        // MessageBox(hwndMe, szMsg, _T("WMADecSH4"), MB_OK);
        //ASSERTMSG( szMsg, (rc == cWMA_NoErr) );
        //iRetValue = 5;
        //goto lexit;
		return false;
    }

    /* set up the content description struct */

    memset((void *)&desc, 0, sizeof(desc));
    desc.title_len = STRING_SIZE;
    desc.pTitle    = (unsigned char *)g_szTitle;
    desc.author_len = STRING_SIZE;
    desc.pAuthor    = (unsigned char *)g_szAuthor;
    desc.copyright_len = STRING_SIZE;
    desc.pCopyright    = (unsigned char *)g_szCopyright;
    desc.description_len = STRING_SIZE;
    desc.pDescription    = (unsigned char *)g_szDescription;
    desc.rating_len = STRING_SIZE;
    desc.pRating    = (unsigned char *)g_szRating;

    /* get content description */

    rc = WMAFileContentDesc (g_state, &desc);
    if(rc != cWMA_NoErr)
    {
        //wsprintf( szMsg, TEXT("** Failed to retrieve content description.\n") );
        // MessageBox(hwndMe, szMsg, _T("WMADecSH4"), MB_OK);
        //ASSERTMSG( szMsg, (rc == cWMA_NoErr) );
        //iRetValue = 6;
        //goto lexit;
		return false;
    }

    /* display information */

/*
    switch(g_hdr.sample_rate)
    {
#ifndef _ARM_
    case cWMA_SR_08kHz:
        g_SampleRate =  8000;
        break;
    case cWMA_SR_11_025kHz:
        g_SampleRate = 11025;
        break;
    case cWMA_SR_16kHz:
        g_SampleRate = 16000;
        break;
#endif
    case cWMA_SR_22_05kHz:
        g_SampleRate = 22050;
        break;
    case cWMA_SR_32kHz:
        g_SampleRate = 32000;
        break;
    case cWMA_SR_44_1kHz:
        g_SampleRate = 44100;
        break;
#ifndef _ARM_
    case cWMA_SR_48kHz:
        g_SampleRate = 48000;
        break;
#endif
    default:
        g_SampleRate = g_hdr.sample_rate;
        break;
    }
    g_fShowContentInfo = 1;
*/

    /* if DRM, init with the license file */

    if(g_hdr.has_DRM)
    {
#if 0
        tWMA_U32        iLicenseLength;
        g_lic.pPMID = (unsigned char *)&g_pmid;
        g_lic.cbPMID = sizeof(g_pmid);

//        WMAGetLicenseStore(g_state, &iLicenseLength);
		WMAGetLicenseStore(&g_hdrstate, &iLicenseLength);
        if (iLicenseLength == 0) {
			CloseHandle(s_hLicenseFile);
			s_hLicenseFile = CreateFile(s_wszLicenseFile,GENERIC_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
            //g_fpLic = fopen (strLic, "rb");
            if(s_hLicenseFile == INVALID_HANDLE_VALUE)
            {
                //wsprintf( szMsg, TEXT("** Cannot open the license file %s.\n"), strLic );
                // MessageBox(hwndMe, szMsg, _T("WMADecSH4"), MB_OK);
                //ASSERTMSG( szMsg, (g_fpLic != NULL) );
                //iRetValue = 7;
                //goto lexit;
				return false;
            }
        }

        rc = WMAFileLicenseInit (g_state, &g_lic, 3);
        if(rc != cWMA_NoErr)
        {
            //wsprintf( szMsg, TEXT("** WMALicenseInit failed (%u)."), rc );
            // MessageBox(hwndMe, szMsg, _T("WMADecSH4"), MB_OK);
            //ASSERTMSG( szMsg, (rc == cWMA_NoErr) );
            //iRetValue = 7;
            //goto lexit;
			return false;
        }
#else
		return false;
#endif
    }

//    RetailOutput(TEXT("Sample Rate %d Hzs.\r\nBitRate %d.\r\n"),
//       g_SampleRate, g_hdr.bitrate );


    g_wfx.wFormatTag      = WAVE_FORMAT_PCM;
    g_wfx.nSamplesPerSec  = g_hdr.sample_rate;
    g_wfx.nChannels       = g_hdr.num_channels;
    g_wfx.wBitsPerSample  = 16;
    g_wfx.nBlockAlign     = ((g_wfx.wBitsPerSample + 7) / 8) * g_wfx.nChannels;
    g_wfx.nAvgBytesPerSec = g_wfx.nBlockAlign * g_wfx.nSamplesPerSec;
    g_wfx.cbSize          = 0;

	return true;
}





#endif