// FileOutputStream.cpp: implementation of the FileOutputStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileOutputStream.h"
#include "StreamFormat.h"

#define WAV_OUTPUT
//#undef WAV_OUTPUT

#ifdef WAV_OUTPUT
#include <mmsystem.h>
long g_datasize = 0;
long g_RIFFsizePtr = 0;
long g_datasizePtr = 0;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
FileOutputStream::FileOutputStream() : 
m_pIFormat(NULL),
#ifdef WIN32
m_hFile(INVALID_HANDLE_VALUE)
#else
m_file(NULL) 
#endif
{};

void FileOutputStream::FinalRelease()
{
	CUnknown::FinalRelease();
	if(m_pIFormat) m_pIFormat->Release();
#ifdef WIN32
	if(m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
#else
	if(m_file != NULL)
		fclose(m_file);
#endif
}

HRESULT FileOutputStream::Open(LPCTSTR szFileName)
{
	if(szFileName == NULL) return E_POINTER;

	if(FAILED(DssCreateAudioStreamFormat(&m_pIFormat)))
		return E_FAIL;

#ifdef WIN32
	m_hFile = CreateFile(szFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(m_hFile != INVALID_HANDLE_VALUE)
#else
	m_file = fopen(szFileName,"w");
	if(m_file != NULL)
#endif
		return S_OK;
	else
		return E_FAIL;
}

//IOOutputStream interface
HRESULT STDCALL FileOutputStream::Write(BYTE* pBuffer, DWORD dwBytes, CodecEventInfo*, DWORD* pdwBytesWritten)
{
#ifdef WIN32
	WriteFile(m_hFile,pBuffer,dwBytes,pdwBytesWritten,NULL);
#else
	if(m_file) return E_POINTER;
	*pdwBytesWritten = static_cast<DWORD>(fwrite(pBuffer,1,dwBytes,m_file));
#endif
	g_datasize += dwBytes;
	return S_OK;
}

HRESULT STDCALL FileOutputStream::Advise(IDssEvent* pIEvent)
{
	return E_NOTIMPL;
}

/*
HRESULT FileOutputStream::SetCallbackRate(OutputStreamEvent event, DWORD dwRate)
{
	return E_NOTIMPL;
}
*/

HRESULT STDCALL FileOutputStream::GetFormat(IDssStreamFormat** ppIFormat)
{
	if(ppIFormat == NULL) return E_POINTER;
	*ppIFormat = m_pIFormat;
	m_pIFormat->AddRef();
	return S_OK;
}

HRESULT STDCALL FileOutputStream::SetFormat(IDssAudioStreamFormat* pIFormat)
{
	if(pIFormat == NULL) return E_POINTER;

	if(m_pIFormat)
		m_pIFormat->Release();

	m_pIFormat = pIFormat;
	pIFormat->AddRef();

#ifdef WAV_OUTPUT
	DWORD dwBytesWritten;
	Write((BYTE*)"RIFF2222WAVEfmt ",16,NULL,&dwBytesWritten);
	g_RIFFsizePtr = 4;
	WAVEFORMATEX wfx;
	CAudioStreamFormat* pFormat = static_cast<CAudioStreamFormat*>(pIFormat);

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.wBitsPerSample = pIFormat->GetBitsPerSample();
	wfx.nSamplesPerSec = pIFormat->GetSamplingFrequency();
	wfx.nChannels = pIFormat->GetChannels();
	wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
/*
	wfx.wFormatTag = pFormat->m_sfx.wFormatTag;
	wfx.nChannels = pFormat->m_sfx.nChannels;
	wfx.nSamplesPerSec = pFormat->m_sfx.nSamplesPerSec;
	wfx.nAvgBytesPerSec = pFormat->m_sfx.nBytesPerSec;
	wfx.wBitsPerSample = pFormat->m_sfx.wBitsPerSample;
	wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) / 8;
*/
	wfx.cbSize = 0;
	DWORD dwSize = 18;
	Write((BYTE*)&dwSize,4,NULL,&dwBytesWritten);
	Write((BYTE*)&wfx,18,NULL,&dwBytesWritten);
	Write((BYTE*)("data3333"),8,NULL,&dwBytesWritten);
	g_datasizePtr = 42;
	g_datasize = 0;
#endif

	return S_OK;
}


HRESULT STDCALL FileOutputStream::Stop()
{
#ifdef WAV_OUTPUT
#ifdef WIN32
	DWORD dwBytesWritten;
	SetFilePointer(m_hFile,4,NULL,FILE_BEGIN);
	DWORD dwData;
	dwData = g_datasize+4+sizeof(WAVEFORMATEX)+8;
	WriteFile(m_hFile,(BYTE*)&dwData,4,&dwBytesWritten,NULL);
	SetFilePointer(m_hFile,g_datasizePtr,NULL,FILE_BEGIN);
	dwData = g_datasize;
	WriteFile(m_hFile,(BYTE*)&dwData,4,&dwBytesWritten,NULL);
#endif //WIN32
#endif //WAV_OUTPUT
	return S_OK;
}

HRESULT STDCALL FileOutputStream::Pause()
{
	return S_OK;
}

HRESULT STDCALL FileOutputStream::Restart()
{
	return S_OK;
}

HRESULT STDCALL FileOutputStream::GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers)
{
	return E_NOTIMPL;
}