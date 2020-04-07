#ifndef _DSS_CODECDLL_H__
#define _DSS_CODECDLL_H__

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CODECDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CODECDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CODECDLL_EXPORTS
#define CODECDLL_API __declspec(dllexport)
#else
#define CODECDLL_API __declspec(dllimport)
#endif

#include "Dss.h"

//Base Class for Codec
class CodecBase : public IDssCodec
{
public:	
	CodecBase() : m_dwFrameProgressRate(0), 
				  m_dwWaveformVisRate(0),
				  m_dwSpectralVisRate(0) {};

	~CodecBase()  
	{
		//m_spINotify.Detach();
	}

	virtual HRESULT STDCALL GetInputStream(IDssInputStream** ppIInputStream)
	{
		if(!ppIInputStream) return E_POINTER;
		*ppIInputStream = m_spIInStream;
		if(*ppIInputStream)
		{
			m_spIInStream->AddRef();
			return S_OK;
		}
		return E_UNEXPECTED;
	};
	
	virtual HRESULT STDCALL GetOutputStream(IDssOutputStream** ppIOutputStream)
	{
		if(!ppIOutputStream) return E_POINTER;
		*ppIOutputStream = m_spIOutStream;
		if(*ppIOutputStream)
		{
			m_spIOutStream->AddRef();
			return S_OK;
		}
		return E_UNEXPECTED;
	};

protected:
	IDssPtr<IDssInputStream> m_spIInStream;
	IDssPtr<IDssOutputStream> m_spIOutStream;
	IDssPtr<IDssCodecInfo> m_spICodecInfo;
	IDssPtr<IDssCodecEvent> m_spINotify;
	DWORD m_dwFrameProgressRate;
	DWORD m_dwWaveformVisRate;
	DWORD m_dwSpectralVisRate;
};

CODECDLL_API HRESULT GetDriverAuthenticationData(BYTE** ppData,DWORD* pdwDataLen);
CODECDLL_API HRESULT AuthenticateDriver(BYTE* pData,DWORD dwDataLen);
CODECDLL_API HRESULT GetDriverEntryPoint(IDssDriver** ppIDriver); 
#endif