#include "stdafx.h"
#include <memory.h>
#include "CWaveBuffer.h"

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
	
	BytesWritten = min(nBytes,m_nMaxBytes);
	if(BytesWritten)
	{
		memset(m_Hdr.lpData,0,m_nMaxBytes);
		memcpy((PVOID)(m_Hdr.lpData), (PVOID)pData, BytesWritten);
		m_Hdr.dwBufferLength=BytesWritten;
		waveOutWrite(m_hWave, &m_Hdr, sizeof(WAVEHDR));        
	}
	return TRUE;    
}

