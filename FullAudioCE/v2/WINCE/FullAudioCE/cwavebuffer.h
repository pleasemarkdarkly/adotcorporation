#ifndef __CWAVEBUFFER__
#define __CWAVEBUFFER__

#include <windows.h>
#ifdef WIN32
#include <mmsystem.h>
#endif
class CWaveBuffer {
  	public:        
		CWaveBuffer();        
		~CWaveBuffer();

        BOOL Init(HWAVEOUT hWave, int Size);        
		BOOL IsDone();
		        
		BOOL Write(PBYTE pData, int nBytes, int& BytesWritten);
        void Flush();    
		WAVEHDR      m_Hdr;
        HWAVEOUT     m_hWave;        
		int          m_nBytes;
		int			 m_nMaxBytes;
		
};

#endif