// CodecManager.h: interface for the CCodecManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODECMANAGER_H__1A413A41_45E4_11D3_8D95_0090276149F2__INCLUDED_)
#define AFX_CODECMANAGER_H__1A413A41_45E4_11D3_8D95_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dss.h"

class CCodecManager : public CUnknown, public IDssCodecManager
{
private:
	CCodecManager(DWORD dwAuthenticator) : m_pIDrivers(NULL), m_dwAuthenticator(dwAuthenticator) {};
public:
	DECLARE_IUNKNOWN(IDssCodecManager)

	virtual ~CCodecManager() {};

	virtual HRESULT FinalConstruct();
	virtual void FinalRelease();

	static HRESULT CreateInstance(DWORD dwAuthenticator,IDssCodecManager** ppICodecManager);

	virtual HRESULT STDCALL GetDrivers(IDssDrivers** ppIDrivers);
	//Higher Level Functions
	virtual HRESULT STDCALL GetSupportedFileTypes(bool bSupportAll, LPTSTR* ppszFileTypes, DWORD* pchFileTypesLen);
	//For given source stream, find applicable driver (and source index in driver)
	//user can index dst indices for appropriate configuration of output file.
	//call IDssDriver::GetCodec to return codec.
	virtual HRESULT STDCALL FindCompatibleDriver(IDssInputStream* pInputStream, IDssSourceInfo** ppISourceInfo);
	
	//What would be really cool would be a way to go from one given source
	//STREAMFORMATEX header to a destination STREAMFORMATEX header without too
	//much thought on the part of the interface client.  Currently, the first
	//STREAMFORMATEX in the destination list is considered the "recommended"
	//format for best quality.
	virtual HRESULT STDCALL CreateMediaPlayer(IDssMediaPlayer** ppIPlayer);
	virtual HRESULT STDCALL OpenInputStreamFromFile(LPCTSTR szFileName, IDssInputStream** ppIFileInStream);
	virtual HRESULT STDCALL CreateOutputStreamFromFile(LPCTSTR szFileName,IDssAudioStreamFormat* pIFormat,IDssOutputStream** ppIFileOutStream);
	virtual HRESULT STDCALL CreateWaveOutStream(IDssAudioStreamFormat* pIStreamFormat,DWORD dwNumBuffers, DWORD dwBufferSize, IDssAudioOutputStream** ppIWaveOutStream);
//	virtual HRESULT STDCALL CreateVideoOutputStream(IDssVideoStreamFormat* pIFormat, IDssVideoOutputStream** ppIVideoOutputStream);

private:
	IDssDrivers* m_pIDrivers;
	DWORD m_dwAuthenticator;
};

#endif // !defined(AFX_CODECMANAGER_H__1A413A41_45E4_11D3_8D95_0090276149F2__INCLUDED_)
