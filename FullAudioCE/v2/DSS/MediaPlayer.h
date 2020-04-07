#ifndef _CODEC_MANAGER_MEDIA_PLAYER_H__
#define _CODEC_MANAGER_MEDIA_PLAYER_H__

#include "Dss.h"
#include "CriticalSection.h"
#include "Middleman.h"

class CMediaPlayer : public CUnknown, public IDssMediaPlayer
{
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssMediaPlayer)

	~CMediaPlayer() {};	

	virtual HRESULT FinalConstruct();
	virtual void FinalRelease();

	static HRESULT CreateInstance(IDssMediaPlayer** ppIPlayer);
// Local helper functions
private:
	CMediaPlayer() {};

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

#ifdef _WIN32_WCE_AUTOPC
	static TRESULT __cdecl StartWorkerThread(void* pParam);
#else
	static TRESULT _stdcall StartWorkerThread(void* pParam);
#endif
	TRESULT WorkerThreadFunc();
	
	HRESULT InternalSetFrame(DWORD FrameNo);
	HRESULT InternalPlay();
	HRESULT InternalPause();
	HRESULT InternalStop();
public:
	static void ProcessCommand(const CMMNode* pNode);

//Interface Methods
public:
	virtual HRESULT STDCALL Initialize(/*in*/ IDssCodec* pICodec);
	virtual HRESULT STDCALL Uninitialize();

	//Event Interface Management
	virtual HRESULT STDCALL Advise(IDssEvent* pNotify);
	virtual HRESULT STDCALL SetCallbackRate(OutputStreamEvent event, DWORD dwRate);

	//Decoding Management
	virtual HRESULT STDCALL Play();
	virtual HRESULT STDCALL Pause();
	virtual HRESULT STDCALL Stop();
	virtual HRESULT STDCALL SetFrame(DWORD Frame);
	virtual HRESULT STDCALL SetChapter(DWORD Stream) { return (Stream == 0) ? S_OK : E_FAIL; } //MP3 return E_NOTIMPL

	//Information
	virtual HRESULT STDCALL GetCodecInfo(IDssCodecInfo** ppICodecInfo);

	virtual HRESULT STDCALL SetPriority(int nPriority);

private:
	IDssCodec* m_pICodec;
	IDssCodecInfo* m_pICodecInfo;
	IDssMediaPlayerEvent* m_pINotify;
	//IDssInputStream* m_pICodecInputStream;
	//IDssOutputStream* m_pICodecOutputStream;

//Concurrency
	bool m_bPaused;
	WORD m_wLeftChannel, m_wRightChannel;

	HANDLE m_hWorkerThread;
	TRESULT m_WorkerThreadId;

	HANDLE m_hEvtTerminate;
	HANDLE m_hEvtPlaying;
	HANDLE m_hEvtStopped;

	CCriticalSection m_cs;
//Client-Worker Thread Communication Management 
	CMiddleMan* m_pMiddleMan;

//lazyman variables
	DWORD m_FrameNum;
};

#endif // _CODEC_MANAGER_MEDIA_PLAYER_H__
