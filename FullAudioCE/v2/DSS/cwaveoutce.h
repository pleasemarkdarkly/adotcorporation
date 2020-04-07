#ifndef __CWAVEOUT__
#define __CWAVEOUT__

#include "CWaveBuffer.h"
#include "Dss.h"

////////////////////////////////////////////////////////////////////////
//
// Simplest implementation of wave out class.  No funny fixes here.
//
class CWaveOutBase
{
	//Handles Internal management of waveout buffers
public:
	CWaveOutBase();
	virtual ~CWaveOutBase();
	virtual HRESULT Open(LPWAVEFORMATEX Format, int nBuffers, int BufferSize);
	virtual void Write(PBYTE Data, int nBytes, DWORD dwUser);
	virtual void Reset();
	virtual void Pause();
	virtual void Restart();
	virtual HRESULT Advise(IDssEvent* pINotify) { return E_NOTIMPL; };
	virtual HRESULT GetBufferInfo(DWORD* pdwFilled,DWORD* pdwTotal) { return E_NOTIMPL; };
	MMRESULT m_mmRes; // "return value" from constructor
//protected:
	int m_nBuffers;
	int m_nBufferSize;
	int m_CurrentBuffer;
	int m_FilledBuffers;
	CWaveBuffer* m_Hdrs;
	HWAVEOUT m_hWave;
	DWORD	m_LastPlayed;
};

//////////////////////////////////////////////////////////////////////////
//
//	CWaveOut_Polling 
//
class CWaveOutPolling : public CWaveOutBase {
    public:
        CWaveOutPolling();
        virtual ~CWaveOutPolling() {};
		virtual HRESULT Open(LPWAVEFORMATEX Format, int nBuffers, int BufferSize);
		virtual void Write(PBYTE Data, int nBytes, DWORD dwUser);
		virtual void Reset(); 
		virtual void Pause();
		virtual void Restart();
//		void SetFrame(long FrameNo);
		DWORD GetLastPlayed();
		DWORD GetLastPlayedUserInfo();
		bool IsBufferEmpty();
		bool m_bLastPlayedInfoAvailable;
		virtual void Destroy();
		virtual HRESULT GetBufferInfo(DWORD* pdwFilled,DWORD* pdwTotal);
	private:
		bool m_bQueueing;
		int m_FilledBuffers;
		WAVEFORMATEX m_wfx;

};

///////////////////////////////////////////////////////////////////////
//
// CWaveOut_Callback_Function
//
//
class CWaveOutCallbackFunction : public CWaveOutBase {
    public:
        CWaveOutCallbackFunction() : m_bQueueing(true), m_pINotify(NULL){};
        ~CWaveOutCallbackFunction() {};        
		virtual HRESULT Open(LPWAVEFORMATEX Format, int nBuffers, int BufferSize);
		virtual void Write(PBYTE Data, int nBytes, DWORD dwUser);
//		DWORD GetLastPlayed();
//		DWORD GetLastPlayedUserInfo(DWORD);
		virtual HRESULT Advise(IDssEvent* pINotify);
	private:
		static void CALLBACK WaveOutCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInst, DWORD dw1, DWORD dw2);
		
		bool		 m_bQueueing;//Lets not queue for right now.
		IDssAudioOutputStreamEvent* m_pINotify;
};

#endif