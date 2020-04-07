#include "MediaPlayer.h"

#define TIMESTAMP(x) 
#define DEBUG2(a,b)
#define DEBUG3(a,b,c)
#define DEBUG4(a,b,c,d)
#define DEBUG5(a,b,c,d,e)

#define FIX_POP  // Remove scratches by toggling volume at different points in code.
#undef FIX_POP

//////////////////////////////////////////
//
// Internal constants
//
const DWORD c_waitTime = 3000;

#ifdef _WIN32_WCE_AUTOPC
TRESULT __cdecl CMediaPlayer::StartWorkerThread(void* pParam)
#else
TRESULT _stdcall CMediaPlayer::StartWorkerThread(void* pParam)
#endif
{
	CMediaPlayer* pThis = reinterpret_cast<CMediaPlayer*>(pParam);
	return pThis->WorkerThreadFunc();
}

TRESULT CMediaPlayer::WorkerThreadFunc()
{
	DWORD dwRes;
#ifdef FIX_POP
	int NoVolume;
	static bool bVolumeInitialized = false;
#endif
	HRESULT hr;
	bool bFirstTime;
	bool bEOF(false);

#ifdef MEASURE_PERFORMANCE
	DWORD dwPerfStart,dwPerfEnd;
#endif // MEASURE_PERFORMANCE
	HANDLE Events[2];
	Events[0] = m_hEvtTerminate;
	Events[1] = m_hEvtPlaying;

#ifdef UNDER_CE
//	Disabled above normal thread priority because it can be changed
//	in the main application, and there was negligible performance
//	increase on E-100 when playing 44kHz, 16-bit, stereo, downsampled 2-to-1
//	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
#endif

	for(;;)
	{
		dwRes = WaitForMultipleObjects(2,Events,FALSE,INFINITE);
		if((dwRes - WAIT_OBJECT_0) == 1) // m_hEvtPlaying
		{
			//TIMESTAMP(g_pDebugFile);
			//DEBUG2(g_pDebugFile,L"Begin Playing m_hEvtPlaying\r\n");			
			bEOF = false;
			bFirstTime = true;
			ResetEvent(m_hEvtStopped);
			
			Lock();
			if(m_bPaused)
			{
				bFirstTime = false;
#ifdef FIX_POP
				NoVolume = 0;
#endif
			}
#ifdef FIX_POP
			else
				NoVolume = 20;
#endif
			m_bPaused = false;
			Unlock();

			//Get output stream from codec.
			IDssOutputStream* pICodecOutputStream;
			m_pICodec->GetOutputStream(&pICodecOutputStream);
			
			pICodecOutputStream->Restart();

			//Get pointer to OutputStream here;
			for(;;)
			{
				if(bEOF)
					break;

				dwRes = WaitForSingleObject(m_hEvtPlaying,0);
				if(dwRes == WAIT_TIMEOUT)
					break;

#ifdef FIX_POP
				if(NoVolume > 0 && --NoVolume == 0)
				{
					pICodecOutputStream->SetVolume(m_wLeftChannel,m_wRightChannel);
				}
#endif

				hr = m_pICodec->Convert(bFirstTime);
				++(m_FrameNum);

				if(FAILED(hr))
				{
					//DEBUG3(g_pDebugFile,L"Thread - Decode Failed %x\n",hr);
					bEOF = false;
					break;
				}
				if(hr == S_FALSE)
				{
					//DEBUG2(g_pDebugFile,L"Read : EOF\n");
					bEOF = true;
				}
				
				bFirstTime = false;
			} // Play Loop
#ifdef PROFILE 
			sProfiler.dumpProfileData();
#endif 

			TIMESTAMP(g_pDebugFile);
			DEBUG2(g_pDebugFile,L"Done decoding song\r\n");

			//Done decoding
			if(bEOF && m_pINotify)
			{
			//Now we have to set up another loop to continue posting
			//OnFrameProgress notifications At this point, poll wave 
			//out headers until all headers are done, and notify OnFileEnd
//				for(int i=0;i<100;++i)
				// Ed's change: loop until broken.
				for(;;)
				{
					dwRes = WaitForSingleObject(m_hEvtPlaying,0);
					if(dwRes == WAIT_TIMEOUT)
					{
						TIMESTAMP(g_pDebugFile);
						DEBUG2(g_pDebugFile,L"WaitForSingleObject timeout--break\r\n");
						break;
					}
					TIMESTAMP(g_pDebugFile);
					DEBUG2(g_pDebugFile,L"Sleeping\r\n");
					Sleep(100); //TODO : Mmmmm, magic numbers!

					//Poll the waveout device
					hr = pICodecOutputStream->Write(NULL,0,NULL,NULL);
					// if hr == S_OK, we must keep looping
					// if hr == S_FALSE, we're done
					if(hr != S_OK)
						break;
				}
			} //bEOF && Notify
			
			Lock();
			if(m_bPaused)
			{
				if(!bEOF)
					pICodecOutputStream->Pause();
			}
			else 
			{
				if(!bEOF)
				{
#ifdef FIX_POP
					if(!NoVolume)
					{
						pICodecOutputStream->GetVolume(&m_wLeftChannel,&m_wRightChannel);
						pICodecOutputStream->SetVolume(0,0);
					}
#endif
					pICodecOutputStream->Stop();
				}
			}
			Unlock();

			//Release Output Stream
			pICodecOutputStream->Release();
			
//			if(bEOF && m_pINotify)		//Moved here due to problems with 
			if(bEOF && !m_bPaused && m_pINotify)		//Moved here due to problems with 
				m_pINotify->OnFileEnd();	//codec keeping its reference of the OutputStream
								
			ResetEvent(m_hEvtPlaying);
			SetEvent(m_hEvtStopped);

			//TIMESTAMP(g_pDebugFile);
			//DEBUG2(g_pDebugFile,L"End Playing m_hEvtStopped\r\n"); 
		}
		else if((dwRes - WAIT_OBJECT_0) == 0) //m_hEvtTerminate
		{
			break;
		}
		else //Timeout or abandoned
		{
			ResetEvent(m_hEvtPlaying);
			SetEvent(m_hEvtStopped);
		}
	}


	return 42;
}


HRESULT CMediaPlayer::CreateInstance(IDssMediaPlayer** ppIPlayer)
{
	CMediaPlayer* pPlayer = new CMediaPlayer;
	return ::InternalCreateInstance(pPlayer,ppIPlayer);
}

HRESULT CMediaPlayer::FinalConstruct()
{	
	m_bPaused = false;
	m_pICodec = NULL;
	m_pINotify = NULL;
//	m_pICodecInputStream = NULL;
//	m_pICodecOutputStream = NULL;
	m_pICodecInfo = NULL;
	m_pMiddleMan = new CMiddleMan(CMediaPlayer::ProcessCommand);
	if(!m_pMiddleMan)
		return E_OUTOFMEMORY;

	if(m_hEvtTerminate = CreateEvent(NULL,TRUE,FALSE,NULL))
	{
		if(m_hEvtStopped = CreateEvent(NULL,TRUE,TRUE,NULL))
		{
			if(m_hEvtPlaying = CreateEvent(NULL,TRUE,FALSE,NULL))
			{
			
#if defined(THREAD_USES_CRT)
				m_hWorkerThread = (HANDLE)_beginthreadex(NULL,
#else
				m_hWorkerThread = CreateThread(NULL,
#endif
												 200*1024,
												 CMediaPlayer::StartWorkerThread,
												 reinterpret_cast<void*>(this),
												 CREATE_SUSPENDED,
												 &m_WorkerThreadId);

				if(m_hWorkerThread)
				{
					m_pINotify = NULL;
					m_pICodec = NULL;
					ResumeThread(m_hWorkerThread);
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

void CMediaPlayer::FinalRelease()
{
	CUnknown::FinalRelease();
	SetEvent(m_hEvtTerminate);
	ResetEvent(m_hEvtPlaying);
		
	WaitForSingleObject(m_hWorkerThread,2000);

#ifdef FIX_POP
	if(m_pICodec) 
	{
		IDssOutputStream* pIOutputStream;
		m_pICodec->GetOutputStream(&pIOutputStream);
		pIOutputStream->SetVolume(m_wLeftChannel,m_wRightChannel);
		pIOutputStream->Release();
		m_pICodec->Release();
	}
#endif

	if(m_pICodecInfo) 
		m_pICodecInfo->Release();
	if(m_pINotify) 
		m_pINotify->Release();
//	if(m_pICodecInputStream) 
//		m_pICodecInputStream->Release();
//	if(m_pICodecOutputStream) 
//		m_pICodecOutputStream->Release();
	
	CloseHandle(m_hWorkerThread);
	CloseHandle(m_hEvtStopped);
	CloseHandle(m_hEvtTerminate);
	CloseHandle(m_hEvtPlaying);
}

void CMediaPlayer::ProcessCommand(const CMMNode* pNode)
{
	HRESULT hRes;
	DEBUG4(g_pDebugManager,L"ProcessCommand(%d,%d)\r\n",pNode->Command,pNode->Param1);
	switch(pNode->Command)
	{
	case CMD_STOP:
		DEBUG2(g_pDebugManager,L"ProcessCommand CMD_STOP\r\n");
		hRes = pNode->m_pPlayer->InternalStop();
		DEBUG3(g_pDebugManager,L"ProcessCommand STOP return %d\r\n",hRes);
		break;
	case CMD_PAUSE:
		DEBUG2(g_pDebugManager,L"ProcessCommand CMD_PAUSE\r\n");
		hRes = pNode->m_pPlayer->InternalPause();
		DEBUG3(g_pDebugManager,L"ProcessCommand PAUSE return %d\r\n",hRes);
		break;
	case CMD_PLAY:
		DEBUG2(g_pDebugManager,L"ProcessCommand CMD_PLAY\r\n");
		hRes = pNode->m_pPlayer->InternalPlay();
		DEBUG3(g_pDebugManager,L"ProcessCommand PLAY return %d\r\n",hRes);
		break;
	case CMD_SETFRAME:
		DEBUG2(g_pDebugManager,L"ProcessCommand CMD_SETFRAME\r\n");
		hRes = pNode->m_pPlayer->InternalSetFrame((DWORD)(pNode->Param1));
		DEBUG3(g_pDebugManager,L"ProcessCommand SETFRAME return %d\r\n",hRes);
		break;
	}
	if(pNode->m_pPlayer->m_pINotify)
		pNode->m_pPlayer->m_pINotify->OnCommandComplete(pNode->Command,pNode->Param1,hRes);

	if (pNode->Command == CMD_SETFILE)
		delete [] (TCHAR*)(pNode->Param1);
}

HRESULT CMediaPlayer::InternalSetFrame(DWORD FrameNo)
{
	DEBUG2(g_pDebugManager,L"InternalSetFrame\r\n");

	if(m_pICodec == NULL) 
		return E_FAIL;

	FrameInfo fi;
	IDssSourceInfo* pISourceInfo;
	m_pICodecInfo->QueryInterface(IID_IDssSourceInfo,reinterpret_cast<void**>(&pISourceInfo));
	pISourceInfo->GetFrameInfo(&fi);
	pISourceInfo->Release();

	if(FrameNo < 0 || FrameNo > fi.NumFrames)
		return E_INVALIDARG;

	TIMESTAMP(g_pDebugManager);
	DEBUG2(g_pDebugManager,L"InternalSetFrame\r\n");

	Lock();
	m_bPaused = false;
	Unlock();

	ResetEvent(m_hEvtPlaying);

	DEBUG2(g_pDebugManager,L"WaitForSingleObject m_hEvtStopped\r\n");
	DWORD dwRes2 = WaitForSingleObject(m_hEvtStopped,c_waitTime);
	if(dwRes2 == WAIT_TIMEOUT || dwRes2 == WAIT_FAILED)
	{
		DEBUG2(g_pDebugManager,L"SetFrame TimedOut\r\n");	
		return E_FAIL;
	}

	TIMESTAMP(g_pDebugManager);
	DEBUG2(g_pDebugManager,L"SetFrame Wait succeeded\r\n");
	
	HRESULT hr;

	hr = m_pICodec->SetFrame(FrameNo);
	m_FrameNum = FrameNo;

	return hr;
}

HRESULT CMediaPlayer::InternalPlay()
{
	TIMESTAMP(g_pDebugManager);
	DEBUG2(g_pDebugManager,L"InternalPlay\r\n");

	if(m_pICodec == NULL)
		return E_FAIL;

	SetEvent(m_hEvtPlaying);

	return S_OK;
}

HRESULT CMediaPlayer::InternalPause()
{
	TIMESTAMP(g_pDebugManager);
	DEBUG2(g_pDebugManager,L"InternalPause\r\n");
	
	if(m_pICodec == NULL)
		return E_FAIL;

	Lock();
	m_bPaused = true;
	Unlock();

	TIMESTAMP(g_pDebugManager);
	DEBUG2(g_pDebugManager,L"InternalPause Reset m_hEvtPlaying\r\n");

	ResetEvent(m_hEvtPlaying);
	
	return S_OK;
}

HRESULT CMediaPlayer::InternalStop()
{
	TIMESTAMP(g_pDebugManager);
	DEBUG2(g_pDebugManager,L"InternalReset\r\n");

	if(m_pICodec == NULL)
		return E_FAIL;

	Lock();
	m_bPaused = false;
	Unlock();

	if(WaitForSingleObject(m_hEvtPlaying,0) == WAIT_OBJECT_0)
	{
		ResetEvent(m_hEvtPlaying);
	}
	else
	{
		IDssOutputStream* pICodecOutputStream;
		if(SUCCEEDED(m_pICodec->GetOutputStream(&pICodecOutputStream)))
		{
			pICodecOutputStream->Stop();
			pICodecOutputStream->Release();
		}
	}
		
	ResetEvent(m_hEvtPlaying);

	DEBUG2(g_pDebugManager,L"WaitForSingleObject m_hEvtStopped\r\n");
	DWORD dwRes2 = WaitForSingleObject(m_hEvtStopped,c_waitTime);
	if(dwRes2 == WAIT_TIMEOUT || dwRes2 == WAIT_FAILED)
	{
		DEBUG2(g_pDebugManager,L"Reset TimedOut\r\n");	
		return E_FAIL;
	}

	DEBUG2(g_pDebugManager,L"Reset Wait succeeded\r\n");

	HRESULT hr;
	hr = m_pICodec->SetFrame(0);
	m_FrameNum = 0;

	return hr;	
}

HRESULT STDCALL CMediaPlayer::Uninitialize()
{
	
#ifdef FIX_POP
	if(m_pICodec)
	{
		IDssOutputStream* pICodecOutputStream;
		m_pICodec->GetOutputStream(&pICodecOutputStream);
		pICodecOutputStream->SetVolume(m_wLeftChannel,m_wRightChannel);
		pICodecOutputStream->Release();
	}
#endif

	if(m_pICodec)
	{
		InternalStop();
	}
	
	if(m_pICodec)
	{
		m_pICodec->Release();
		m_pICodec = NULL;
	}
	if(m_pICodecInfo)
	{
		m_pICodecInfo->Release();
		m_pICodecInfo = NULL;
	}
	if(m_pINotify)
	{
		m_pINotify->Release();
		m_pINotify = NULL;
	}
	return S_OK;
}

HRESULT STDCALL CMediaPlayer::Initialize(IDssCodec* pICodec)
{
	if(!pICodec) return E_POINTER;
	if(m_pICodec)
		Uninitialize();

	m_pICodec = pICodec;
	pICodec->AddRef();
	
	pICodec->GetCodecInfo(&m_pICodecInfo);

#ifdef FIX_POP
	IDssOutputStream* pIOutputStream;
	pICodec->GetOutputStream(&pIOutputStream);
	pIOutputStream->GetVolume(&m_wLeftChannel,&m_wRightChannel);
	pIOutputStream->Release();
#endif

	//hacka hacka hacka
	//if codec out stream is a waveout stream, then add appropriate hooks
	//so worker thread can pause/reset wave out buffers
	//step 1, figure out if the IDssOutputStream is a CWaveOutStream
//	pICodec->GetInputStream(&m_pICodecInputStream);
//	pICodec->GetOutputStream(&m_pICodecOutputStream);

	return S_OK;
}	

HRESULT STDCALL CMediaPlayer::SetFrame(DWORD FrameNo)
{
	if(!m_pICodec) return E_FAIL;

	if(m_pMiddleMan)
	{
		m_pMiddleMan->Push(CMMNode(this,CMD_SETFRAME,(DWORD)FrameNo));
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDCALL CMediaPlayer::Play()
{
	if(!m_pICodec) return E_FAIL;
	if(m_pMiddleMan)
	{
		m_pMiddleMan->Push(CMMNode(this,CMD_PLAY,0));
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDCALL CMediaPlayer::Pause()
{
	if(!m_pICodec) return E_FAIL;
	if(m_pMiddleMan)
	{
		m_pMiddleMan->Push(CMMNode(this,CMD_PAUSE,0));
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDCALL CMediaPlayer::Stop()
{
	if(!m_pICodec) return E_FAIL;
	if(m_pMiddleMan)
	{
		m_pMiddleMan->Push(CMMNode(this,CMD_STOP,0));
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDCALL CMediaPlayer::GetCodecInfo(IDssCodecInfo **ppICodecInfo)
{
	if(!m_pICodec) return E_FAIL;
	return m_pICodec->GetCodecInfo(ppICodecInfo);
}

HRESULT STDCALL CMediaPlayer::SetCallbackRate(OutputStreamEvent event, DWORD dwRate)
{
	if(!m_pICodec)
		return E_FAIL;
	
	return m_pICodec->SetCallbackRate(event,dwRate);
}

HRESULT STDCALL CMediaPlayer::Advise(IDssEvent* pINotify)
{
	if(!pINotify) return E_POINTER;
	if(!m_pICodec) return E_FAIL;

	IDssMediaPlayerEvent* pIMPNotify;
	if (FAILED(pINotify->QueryInterface(IID_IDssMediaPlayerEvent, reinterpret_cast<void**>(&pIMPNotify))))
		return E_INVALID_EVENT;

	if(m_pINotify)
		m_pINotify->Release();

	m_pINotify = pIMPNotify;
	
	return m_pICodec->Advise(pINotify);
}

HRESULT STDCALL CMediaPlayer::SetPriority(int nPriority)
{
	BOOL b = ::SetThreadPriority(m_hWorkerThread,nPriority);
	return (b) ? S_OK : E_FAIL;
}
