// StreamFormat.cpp: implementation of the CStreamFormat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StreamFormat.h"

#include <malloc.h>

//////////////////////////////////////////////////////////////////////
// CAudioStreamFormat
///////////////////////////////////////

HRESULT CAudioStreamFormat::CreateInstance(IDssAudioStreamFormat** ppIFormat)
{
	CAudioStreamFormat* pFormat = new CAudioStreamFormat;
	return ::InternalCreateInstance(pFormat,ppIFormat);
}

CAudioStreamFormat::~CAudioStreamFormat()
{
};

HRESULT STDCALL CAudioStreamFormat::NondelegatingQueryInterface(const IID& iid, void **ppv)
{
	if(IsEqualGuid(iid,IID_IDssStreamFormat))
	{
		IDssStreamFormat* pI = static_cast<IDssStreamFormat*>(this);
		pI->AddRef();
		*ppv = pI;
		return S_OK;
	}
	else if(IsEqualGuid(iid,IID_IDssAudioStreamFormat))
	{
		IDssAudioStreamFormat* pI = static_cast<IDssAudioStreamFormat*>(this);
		pI->AddRef();
		*ppv = pI;
		return S_OK;
	}
	else
		return CUnknown::NondelegatingQueryInterface(iid,ppv);
}


//////////////////////////////////////////////////////////////////////
// CVideoStreamFormat
///////////////////////////////////////
HRESULT CVideoStreamFormat::CreateInstance(IDssVideoStreamFormat** ppIFormat)
{
	CVideoStreamFormat* pFormat = new CVideoStreamFormat;
	return ::InternalCreateInstance(pFormat,ppIFormat);
}

CVideoStreamFormat::~CVideoStreamFormat()
{
	free(m_pBitmapInfo);
};

HRESULT STDCALL CVideoStreamFormat::NondelegatingQueryInterface(const IID& iid, void **ppv)
{
	if(IsEqualGuid(iid,IID_IDssStreamFormat))
	{
		IDssStreamFormat* pI = static_cast<IDssStreamFormat*>(this);
		pI->AddRef();
		*ppv = pI;
		return S_OK;
	}
	else if(IsEqualGuid(iid,IID_IDssVideoStreamFormat))
	{
		IDssVideoStreamFormat* pI = static_cast<IDssVideoStreamFormat*>(this);
		pI->AddRef();
		*ppv = pI;
		return S_OK;
	}
	else
		return CUnknown::NondelegatingQueryInterface(iid,ppv);
}

void
CVideoStreamFormat::SetBitmapInfo(BITMAPINFO* pBitmapInfo, DWORD dwBitmapInfoSize)
{
	m_pBitmapInfo = (BITMAPINFO*)malloc(dwBitmapInfoSize);
	if (m_pBitmapInfo)
	{
		memcpy(m_pBitmapInfo, pBitmapInfo, dwBitmapInfoSize);
		m_dwBitmapInfoSize = dwBitmapInfoSize;
	}
	else
		m_dwBitmapInfoSize = 0;
	memset(&m_videoFileInfo, 0, sizeof(VideoFileInfo));
}

BITMAPINFO*
CVideoStreamFormat::GetBitmapInfo()
{
	if (m_dwBitmapInfoSize)
	{
		BITMAPINFO* pBitmapInfo = (BITMAPINFO*)malloc(m_dwBitmapInfoSize);
		if (pBitmapInfo)
		{
			memcpy(pBitmapInfo, m_pBitmapInfo, m_dwBitmapInfoSize);
			return pBitmapInfo;
		}
	}

	return 0;
}
