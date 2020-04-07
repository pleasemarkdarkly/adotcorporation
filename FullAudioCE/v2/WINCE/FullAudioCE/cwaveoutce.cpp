#include "stdafx.h"
#include <memory.h>
#include "CWaveoutCE.h"
//#include "Dss.h"
//#include "StructHelper.h"
//#include "CodecManager.h"

/*
#ifdef MP3_DEBUG
#include "DebugFile.h"
extern CDebugFile *g_pDebugFile;
extern CDebugFile *g_pDebugManager;
//extern CDebugFile *g_pDebugLastPlayed;
#define DEBUG2(x,y) x->Output(y)
#define DEBUG3(x,y,z) x->Output(y,z)
#define DEBUG4(x,y,z,a) x->Output(y,z,a)
#define DEBUG5(x,y,z,a,b) x->Output(y,z,a,b)
#else
#define DEBUG2(x,y) 
#define DEBUG3(x,y,z)
#define DEBUG4(x,y,z,a)
#define DEBUG5(x,y,z,a,b)
#endif
*/

///////////////////////////////////////////////////////////////////
//
// CWaveBuffer methods
//

/*    CWaveBuffer*/
CWaveBuffer::CWaveBuffer(){}

BOOL CWaveBuffer::Init(HWAVEOUT hWave, int Size) 
{
	m_nMaxBytes = Size;
    m_hWave  = hWave;    
	m_nBytes = 0;
    /*  Allocate a buffer and initialize the header */
    m_Hdr.lpData = (LPSTR)LocalAlloc(LPTR, Size);
    
	if (m_Hdr.lpData == NULL) 
		return FALSE;    
    m_Hdr.dwBufferLength  = Size;    
	m_Hdr.dwBytesRecorded = 0;
    m_Hdr.dwUser = 0;    
	m_Hdr.dwFlags = WHDR_DONE;    
	m_Hdr.dwLoops = 0;
    m_Hdr.lpNext = 0;    
	m_Hdr.reserved = 0;    
	/*  Prepare it */
    waveOutPrepareHeader(hWave, &m_Hdr, sizeof(WAVEHDR)); 
	
	//Set buffer initially to "Done"
	m_Hdr.dwFlags |= WHDR_DONE;
	return TRUE;
}

CWaveBuffer::~CWaveBuffer() 
{    
	if (m_Hdr.lpData) {
        memset(m_Hdr.lpData,0,m_nMaxBytes);
		waveOutUnprepareHeader(m_hWave, &m_Hdr, sizeof(WAVEHDR));
        LocalFree(m_Hdr.lpData);    
	}
}

BOOL CWaveBuffer::IsDone()
{
	if(m_Hdr.dwFlags & WHDR_DONE)
		return TRUE;
	else
		return FALSE;
}

void CWaveBuffer::Flush(){
    //ASSERT(m_nBytes != 0);    
	m_nBytes = 0;
    waveOutWrite(m_hWave, &m_Hdr, sizeof(WAVEHDR));
}

BOOL CWaveBuffer::Write(PBYTE pData, int nBytes, int& BytesWritten) {
    //ASSERT((DWORD)m_nBytes != m_Hdr.dwBufferLength);    
#if 0
	BytesWritten = min(nBytes,m_nMaxBytes);
	if(BytesWritten)
	{
		memset(m_Hdr.lpData,0,m_nMaxBytes);
		memcpy((PVOID)(m_Hdr.lpData), (PVOID)pData, BytesWritten);
		m_Hdr.dwBufferLength=BytesWritten;
		waveOutWrite(m_hWave, &m_Hdr, sizeof(WAVEHDR));        
	}
	return TRUE;    
#else
	BytesWritten = min(nBytes,m_nMaxBytes-m_nBytes);
	if(BytesWritten)
	{
		memcpy((PVOID)(m_Hdr.lpData + m_nBytes),(PVOID)pData,BytesWritten);
		m_nBytes += BytesWritten;
		if(m_nBytes == m_nMaxBytes)
		{
			m_Hdr.dwBufferLength = m_nMaxBytes;
			m_nBytes = 0;
			waveOutWrite(m_hWave,&m_Hdr,sizeof(WAVEHDR));
			return TRUE;
		}
	}
	return FALSE;
#endif
}


///////////////////////////////////////////////////////////////////
//
// CWaveOutBase methods
//

#define WAVEOUT_OPEN_RETRY_COUNT	50 	// Number of times to try to open the wave out device.
//#undef	WAVEOUT_OPEN_RETRY_COUNT

CWaveOutBase::CWaveOutBase() :
m_nBuffers(0),    
m_CurrentBuffer(0),   
m_FilledBuffers(0), 
m_Hdrs(NULL),    
m_hWave(NULL)
{};

HRESULT CWaveOutBase::Open(LPWAVEFORMATEX Format, int nBuffers, int BufferSize)
{
	MMRESULT mmRes;
	m_nBuffers = nBuffers;
	m_nBufferSize = BufferSize;
	/*  Create wave device */
#ifdef WAVEOUT_OPEN_RETRY_COUNT
	for (int j = 0; j < WAVEOUT_OPEN_RETRY_COUNT; ++j)
	{
#endif	// WAVEOUT_OPEN_RETRY_COUNT
		mmRes = waveOutOpen(&m_hWave,
					WAVE_MAPPER,                
					Format,
					NULL, 0, CALLBACK_NULL );
#ifdef WAVEOUT_OPEN_RETRY_COUNT
		if(mmRes == MMSYSERR_NOERROR)
			break;
		Sleep(100);
	}
#endif	// WAVEOUT_OPEN_RETRY_COUNT

	if(mmRes != MMSYSERR_NOERROR)
		return E_FAIL;

	/*  Initialize the wave buffers */
    m_Hdrs = new CWaveBuffer[nBuffers];
	if(!m_Hdrs) return E_OUTOFMEMORY;
	for (int i = 0; i < nBuffers; i++) 
		m_Hdrs[i].Init(m_hWave, BufferSize);  

	waveOutReset(m_hWave);
	return S_OK;
}

CWaveOutBase::~CWaveOutBase()
{
	waveOutReset(m_hWave);    /* First get our buffers back */
	delete [] m_Hdrs;		  /* Free the buffers */    
	waveOutClose(m_hWave);    /* Close the wave device */
}

void CWaveOutBase::Write(PBYTE Data, int nBytes, DWORD dwUser)
{
	while(!m_Hdrs[m_CurrentBuffer].IsDone())
		Sleep(10);

	int nWritten;
	
	m_LastPlayed = m_Hdrs[m_CurrentBuffer].m_Hdr.dwUser;
	m_Hdrs[m_CurrentBuffer].m_Hdr.dwUser = dwUser;
	m_Hdrs[m_CurrentBuffer].Write(Data,nBytes,nWritten);
	m_CurrentBuffer++;
	if(m_CurrentBuffer >= m_nBuffers)
		m_CurrentBuffer = 0;
}

void CWaveOutBase::Reset()
{
	waveOutReset(m_hWave);
	//empty wave buffers
	m_CurrentBuffer = 0;
	m_LastPlayed = 0;
}

void CWaveOutBase::Pause()
{
	waveOutPause(m_hWave);
}

void CWaveOutBase::Restart()
{
	waveOutRestart(m_hWave);
}

bool CWaveOutBase::IsPlaybackComplete()
{
	for(int i=0;i<m_nBuffers;++i)
	{	
		if(!m_Hdrs[i].IsDone())
			return false;
	}
	return true;
}

/*
HRESULT CWMAWaveOut::Open(LPWAVEFORMATEX Format, int nBuffers, int BufferSize)
{
	memcpy(&m_wfx,Format,sizeof(WAVEFORMATEX));
	HRESULT hr = CWaveOutBase::Open(Format,nBuffers,BufferSize);
	if(SUCCEEDED(hr))
	{
		m_bQueueing = true;
		m_bLastPlayedInfoAvailable = false;
		waveOutPause(m_hWave);
	}
	return hr;
}
*/

void CWMAWaveOut::Destroy()
{
	waveOutReset(m_hWave);    /* First get our buffers back */
	delete [] m_Hdrs;		  /* Free the buffers */    
	m_Hdrs = NULL;
    waveOutClose(m_hWave);    /* Close the wave device */
	m_hWave = NULL;
}

void CWMAWaveOut::WriteInterleaved(PBYTE pInterleaved, int nBytes)
{
	while(nBytes > 0)
	{
		while(!m_Hdrs[m_CurrentBuffer].IsDone())
			Sleep(10);

		int nWritten;
	
		m_Hdrs[m_CurrentBuffer].Write(pInterleaved,nBytes,nWritten);
		m_CurrentBuffer++;
		if(m_CurrentBuffer >= m_nBuffers)
			m_CurrentBuffer = 0;
		nBytes -= nWritten;
		pInterleaved += nWritten;
	}
}

//void WriteNonInterleaved(PBYTE pLeft, PBYTE pRight, int nBytes, int cbSample);

//////////////////////////////////////////////////////////////////////////////
//
// CWaveOutPolling
//
//


#define RESET_CLOSES_WAVEOUT 	// Close the waveout resource whenever waveout is stopped
//#undef RESET_CLOSES_WAVEOUT	// Use of this definition does not solve popping noises in MP3.

#if 0
CWaveOutPolling::CWaveOutPolling()
: m_bQueueing(true), 
m_bLastPlayedInfoAvailable(false)
{}


/*
CWaveOutPolling::~CWaveOutPolling()
{
	// Before we delete the headers in ~CWaveOutBase , make sure
	// all user information has been destroyed.
	if(m_Hdrs)
	{
		for(int i=0;i<m_nBuffers;i++)
		{
			if(m_Hdrs[i].m_Hdr.dwUser)
			{
				DestroyCodecEventInfo((CodecEventInfo*)m_Hdrs[i].m_Hdr.dwUser);
				m_Hdrs[i].m_Hdr.dwUser = 0;
			}
		}
	}
}
*/

HRESULT CWaveOutPolling::Open(LPWAVEFORMATEX Format, int nBuffers, int BufferSize)
{
	memcpy(&m_wfx,Format,sizeof(WAVEFORMATEX));
	HRESULT hr = CWaveOutBase::Open(Format,nBuffers,BufferSize);
	if(SUCCEEDED(hr))
	{
		m_bQueueing = true;
		m_bLastPlayedInfoAvailable = false;
		waveOutPause(m_hWave);
	}
	return hr;
}

void CWaveOutPolling::Destroy()
{
	waveOutReset(m_hWave);    /* First get our buffers back */
	delete [] m_Hdrs;		  /* Free the buffers */    
	m_Hdrs = NULL;
    waveOutClose(m_hWave);    /* Close the wave device */
	m_hWave = NULL;
}

void CWaveOutPolling::Restart()
{
#ifdef RESET_CLOSES_WAVEOUT
	if(!m_hWave)
	{
		HRESULT hr = CWaveOutBase::Open(&m_wfx,m_nBuffers,m_nBufferSize);
		if(SUCCEEDED(hr))
		{
			m_bQueueing = true;
			m_bLastPlayedInfoAvailable = false;
			m_CurrentBuffer = 0;
			m_FilledBuffers = 0;
			waveOutPause(m_hWave);
		}
	}
	else
		waveOutRestart(m_hWave);
#else
	waveOutRestart(m_hWave);
#endif
}

void CWaveOutPolling::Reset()
{    
#ifdef RESET_CLOSES_WAVEOUT
	Destroy();
#else //CWaveOutBase::Reset();
	waveOutReset(m_hWave);
	//empty all buffers
#if 1
	for(int i=0;i<m_nBuffers;i++)
	{
		//m_Hdrs[i].m_Hdr.dwBufferLength = m_Hdrs[i].m_nMaxBytes;
		//m_Hdrs[i].m_Hdr.dwBytesRecorded = 0;
		//m_Hdrs[i].m_Hdr.dwFlags = WHDR_DONE;
		if(m_Hdrs[i].m_Hdr.dwUser)
		{
			DestroyCodecEventInfo((CodecEventInfo*)m_Hdrs[i].m_Hdr.dwUser);
			m_Hdrs[i].m_Hdr.dwUser = 0;
		}
		//memset(m_Hdrs[i].m_Hdr.lpData,0,m_Hdrs[i].m_nMaxBytes);
	}
	if(m_bLastPlayedInfoAvailable)
	{
		DestroyCodecEventInfo((CodecEventInfo*)m_LastPlayed);
		m_LastPlayed = 0;
		m_bLastPlayedInfoAvailable = false;
	}
#endif
	m_CurrentBuffer = 0;
	m_FilledBuffers=0;
	m_bQueueing = true;
	waveOutPause(m_hWave);
#endif
}

void CWaveOutPolling::Pause()
{
	CWaveOutBase::Pause();
#if 0
	//Determine number of filled buffers and if less
	//than total buffers, set start queueing
	m_FilledBuffers = 0;
	for(int i=0;i<m_nBuffers;++i)
	{
		if(!m_Hdrs[i].IsDone())
			++m_FilledBuffers;
	}
	if(m_FilledBuffers < m_nBuffers)
		m_bQueueing = true;
#endif
}

void CWaveOutPolling::Write(PBYTE pData, int nBytes, DWORD dwUser) 
{
	if(m_bQueueing)
	{
		if(m_FilledBuffers >= m_nBuffers)
		{
			m_bQueueing = false;
			waveOutRestart(m_hWave);
		}
	}

//	CWaveOutBase::Write(pData,nBytes,dwUser);
	while(!m_Hdrs[m_CurrentBuffer].IsDone())
		Sleep(10);

	int nWritten;
	if(m_bLastPlayedInfoAvailable)
	{
		DestroyCodecEventInfo(reinterpret_cast<CodecEventInfo*>(m_LastPlayed));
	}
	m_LastPlayed = m_Hdrs[m_CurrentBuffer].m_Hdr.dwUser;
	m_bLastPlayedInfoAvailable = (m_LastPlayed != 0);

	m_Hdrs[m_CurrentBuffer].m_Hdr.dwUser = dwUser;
	m_Hdrs[m_CurrentBuffer].Write(pData,nBytes,nWritten);
    ++m_FilledBuffers;
	m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers; 

	++m_FilledBuffers;

/*
#if 0
	int nWritten;
	if(m_Hdrs[m_CurrentBuffer].Write(pData,nBytes,nWritten))
    {      
        ++m_FilledBuffers;
		m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers; 
        nBytes -= nWritten;            
		pData += nWritten;        
	}
	else
		break;
#endif
*/
}

DWORD CWaveOutPolling::GetLastPlayed()
{
	DWORD LastPlayed = m_LastPlayed;
	m_LastPlayed = 0;
	m_bLastPlayedInfoAvailable = false;
	return LastPlayed;
}

bool CWaveOutPolling::IsBufferEmpty()
{
	for(int i=0;i<m_nBuffers;++i)
	{	
		if(!m_Hdrs[i].IsDone())
			return false;
	}
	return true;
}


//Actually, what I need is the following:
// I want to find out which was the most recently played header.
// Then, of the done headers with non-null dwUser information, find
// out which one has the most recent frame information, and return
// that information, setting all other done non-null dwUser information
// to 0.
DWORD CWaveOutPolling::GetLastPlayedUserInfo()
{
	int iMostRecentLastPlayed = -1;
	DWORD HighestFrame = 0;
	// loop will go through all headers, find the ones that are
	// done, and through comparisons, set iMostRecentLastPlayed to
	// the index for the m_Hdrs array that contains the information
	// we're interested in.  All other done headers have their user
	// information set to 0.
	for(int i=0;i<m_nBuffers;++i)
	{
		if(m_Hdrs[i].IsDone())
		{
			CodecEventInfo* pInfo = reinterpret_cast<CodecEventInfo*>(m_Hdrs[i].m_Hdr.dwUser);
			if(pInfo && pInfo->pFrame)
			{
				if(pInfo->pFrame->CurrentFrame > HighestFrame)
				{
					HighestFrame = pInfo->pFrame->CurrentFrame;
					if(iMostRecentLastPlayed >= 0)
					{
						DestroyCodecEventInfo(reinterpret_cast<CodecEventInfo*>(m_Hdrs[iMostRecentLastPlayed].m_Hdr.dwUser));
						m_Hdrs[iMostRecentLastPlayed].m_Hdr.dwUser = 0;
					}
					iMostRecentLastPlayed = i;
				}
				else
				{
					DestroyCodecEventInfo(pInfo);
					m_Hdrs[i].m_Hdr.dwUser = 0;
				}
			}
		}
	}

	if(iMostRecentLastPlayed >= 0)
	{
		HighestFrame = m_Hdrs[iMostRecentLastPlayed].m_Hdr.dwUser;
		m_Hdrs[iMostRecentLastPlayed].m_Hdr.dwUser = 0;
		return HighestFrame;
	}
	return 0;
}

HRESULT CWaveOutPolling::GetBufferInfo(DWORD* pdwFilled,DWORD* pdwTotal)
{
	if(pdwFilled == NULL || pdwTotal == NULL) return E_POINTER;
	*pdwTotal = m_nBuffers;
	DWORD d = m_nBuffers;
	for(int i=0;i<m_nBuffers;i++)
	{
		if(m_Hdrs[i].m_Hdr.dwFlags & WHDR_DONE)
			d--;
	}
	*pdwFilled = d;
	return S_OK;
}


//TODO : Required for CMediaPlayer??
#if 0
void CWaveOut_Polling::SetFrame(long FrameNo)
{
	this->Reset();
	int b = m_nBuffers + m_CurrentBuffer;
	for(int i=0;i<m_nBuffers;++i)
	{
		m_Hdrs[(b--) % m_nBuffers].m_Hdr.dwUser = (FrameNo > 0) ? FrameNo : 0;	
		--FrameNo;
	}
	m_LastPlayed = FrameNo;
//	DEBUG3(g_pDebugLastPlayed,L"CWaveOut_Polling::SetFrame setting m_LastPlayed %d\n",m_LastPlayed);
}
#endif



////////////////////////////////////////////////////////////////////////
//
//	CWaveOutCallbackFunction
//
HRESULT CWaveOutCallbackFunction::Open(LPWAVEFORMATEX Format, int nBuffers, int BufferSize)
{
	MMRESULT mmRes;
	/*  Create wave device */    
	mmRes = waveOutOpen(&m_hWave,
						WAVE_MAPPER,                
						Format,
						(DWORD)CWaveOutCallbackFunction::WaveOutCallback, 
						reinterpret_cast<DWORD>(this), 
						CALLBACK_FUNCTION );
	if(mmRes != MMSYSERR_NOERROR)
		return E_FAIL;

	/*  Initialize the wave buffers */
    m_Hdrs = new CWaveBuffer[nBuffers];
	if(!m_Hdrs) return E_OUTOFMEMORY;
	for (int i = 0; i < nBuffers; i++) 
        m_Hdrs[i].Init(m_hWave, BufferSize);  

	waveOutReset(m_hWave);
	return S_OK;
}

void CWaveOutCallbackFunction::Write(PBYTE Data, int nBytes, DWORD dwUser)
{
	int nWritten;
	m_Hdrs[m_CurrentBuffer].m_Hdr.dwUser = dwUser;	
	m_Hdrs[m_CurrentBuffer].Write(Data,nBytes,nWritten);
	m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers; 
	++m_FilledBuffers;
}

HRESULT CWaveOutCallbackFunction::Advise(IDssEvent* pINotify)
{
	if(!pINotify) return E_POINTER;
	IDssAudioOutputStreamEvent* pIAudioNotify;
	if (FAILED(pINotify->QueryInterface(IID_IDssAudioOutputStreamEvent, reinterpret_cast<void**>(&pIAudioNotify))))
		return E_INVALID_EVENT;
	if(m_pINotify) m_pINotify->Release();
	m_pINotify = pIAudioNotify;
	return S_OK;
}

void CALLBACK CWaveOutCallbackFunction::WaveOutCallback(
  HWAVEOUT hwo,      
  UINT uMsg,         
  DWORD dwInstance,  
  DWORD dwParam1,    
  DWORD dwParam2     
)
{
	if(uMsg == WOM_DONE)
	{
		CodecEventInfo* pInfo = reinterpret_cast<CodecEventInfo*>(reinterpret_cast<WAVEHDR*>(dwParam1)->dwUser);
		if(pInfo)
		{
			CWaveOutCallbackFunction* pThis = reinterpret_cast<CWaveOutCallbackFunction*>(dwInstance);
			if(pThis->m_pINotify)
			{
				if(pInfo->pFrame)
					pThis->m_pINotify->OnFrameProgress(pInfo->pFrame);
	
				if(pInfo->pWaveform || pInfo->pSpectral)
					pThis->m_pINotify->OnVisualization(pInfo);
			}
			DestroyCodecEventInfo(pInfo);
		}
	}
}
#endif //polling and callback ifdef 0 