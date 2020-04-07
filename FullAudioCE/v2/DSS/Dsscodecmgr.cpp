// mp3dll.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "Dss.h"
#include "CWaveOutCE.h"

#include "FileInputStream.h"
#include "FileOutputStream.h"
#include "CodecManager.h"
#include "Dictionary.h"

//#define CM_DEBUG	//Turns on Debug File Output
/************* Enabling/Disabling Codec Support *********************
To disable specific codec support, please go into the codec's
respective registry key located in the registry hive
	HKEY_LOCAL_MACHINE\Software\Iobjects\Codecs
and make sure the DLL name listed there is incorrect, either by
changing the registry value, or the actual DLL's real name.  Please
keep in mind the wacky rules of LoadLibrary when renaming DLL's under
Windows CE, as listed below.

Two different modules cannot have the same filename, given that the 
extensions are different. These effectively have the same "module" name. 
For example, if LoadLibrary is made on "Sample.cpl", the operating system 
will not load Sample.cpl, but instead will again load Sample.dll. A 
similar limitation exists for modules with the same name but residing in 
different directories. For example, if LoadLibrary is called on 
"\\Windows\Sample.dll", and then LoadLibrary is called on 
"\\MyDir\Sample.dll", "\\Windows\Sample.dll" will simply be reloaded.

A search path to the executable module cannot be specified. Unless the 
full path to the module is specified, Windows CE versions 2.10 and later 
search the following path for the module: 

The absolute path is determined as follows:

1. ROM for statically-linked .DDL files 
2. The .EXE launch directory 
3. The windows (\windows) directory 
4. The root (\) directory 
5. An OEM-dependent directory 
6. The OEM-defined shell (\ppshell) directory, Embedded Toolkit users only 

Windows CE versions 1.0 and 2.0 search the following path for the module: 
1. The root directory of the PC Card RAM expansion card, if one exists. 
2. The windows directory (\Windows). 
3. The root directory (\). 
*********************************************************************/
//#define MEASURE_PERFORMANCE		//Toggles Audio Output and Performance measurements



/*
const DWORD s_waitTime = 3000;

DownSampleRate g_DownSampleRate = NO_DOWNSAMPLING;
ChannelType g_Channel = BOTH_CHANNELS;
CCodecManager* g_Player = NULL;

void ProcessCommand(const CMMNode*);
CMiddleMan* g_pMiddleMan = NULL;
CEQNotifyProxy* g_pEQProxy = NULL;

long g_FrameNotifyRate = 0;
long g_EqualizerNotifyRate = 0;
static CWaveOut* s_pWaveOut = NULL;
BYTE* s_pWaveBuffer = NULL;
*/

/////////////////////////////////////////////////////////////////////////////
// CCodecManager


///////////////////////////////////////////////////////////
//
// DllMain and its handlers
//
/*
void ProcessAttach()
{
#ifdef MEASURE_PERFORMANCE
	g_pPerformance = new CDebugFile;
	g_pPerformance->Open(L"\\Performance.txt");
#endif // MEASURE_PERFORMANCE
#ifdef CM_DEBUG
	g_pDebugFile = new CDebugFile;
	g_pDebugFile->Open(L"\\CodecManager.txt");

	g_pDebugManager = new CDebugFile;
	g_pDebugManager->Open(L"\\DLLManager.txt");
//	g_pDebugLastPlayed = new CDebugFile;
//	g_pDebugLastPlayed->Open(L"\\LastPlayed.txt");
	g_pDebugFile->Output(L"Process Attach\r\n");
	g_pDebugFile->Output(L"g_pMiddleMan = %d\r\n",g_pMiddleMan);

	UINT devs = waveOutGetNumDevs(); 
	g_pDebugFile->Output(L"Wave Out Devices: %d\r\n",devs);
	WAVEOUTCAPS wc;
	for(UINT i=0;i<devs;++i)
	{
		g_pDebugFile->Output(L"*********Device********\r\n");
		waveOutGetDevCaps(i,&wc,sizeof(WAVEOUTCAPS));
		g_pDebugFile->Output(L"Manufacturer: %d\r\n",wc.wMid);
		g_pDebugFile->Output(L"Product: %d\r\n",wc.wPid);
		g_pDebugFile->Output(L"Product Name: %S\r\n",wc.szPname);
		g_pDebugFile->Output(L"Channels: %d\r\n",wc.wChannels);
		g_pDebugFile->Output(L"Supported Formats: \r\n");
		if(wc.dwFormats & WAVE_FORMAT_1M08)
			g_pDebugFile->Output(L"  11.025 kHz, mono, 8-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_1M16)
			g_pDebugFile->Output(L"  11.025 kHz, mono, 16-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_1S08)
			g_pDebugFile->Output(L"  11.025 kHz, stereo, 8-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_1S16)
			g_pDebugFile->Output(L"  11.025 kHz, stereo, 16-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_2M08)
			g_pDebugFile->Output(L"  22.05 kHz, mono, 8-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_2M16)
			g_pDebugFile->Output(L"  22.05 kHz, mono, 16-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_2S08)
			g_pDebugFile->Output(L"  22.05 kHz, stereo, 8-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_2S16)
			g_pDebugFile->Output(L"  22.05 kHz, stereo, 16-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_4M08)
			g_pDebugFile->Output(L"  44.1 kHz, mono, 8-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_4M16)
			g_pDebugFile->Output(L"  44.1 kHz, mono, 16-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_4S08)
			g_pDebugFile->Output(L"  44.1 kHz, stereo, 8-bit \r\n");
		if(wc.dwFormats & WAVE_FORMAT_4S16)
			g_pDebugFile->Output(L"  44.1 kHz, stereo, 16-bit \r\n");
		g_pDebugFile->Output(L"Additional Features:\r\n");
		if(wc.dwSupport & WAVECAPS_LRVOLUME)
			g_pDebugFile->Output(L"  Supports separate left and right volume control. \r\n");
		if(wc.dwSupport & WAVECAPS_PITCH)
			g_pDebugFile->Output(L"  Supports pitch control. \r\n");
		if(wc.dwSupport & WAVECAPS_PLAYBACKRATE)
			g_pDebugFile->Output(L"  Supports playback rate control. \r\n");
		if(wc.dwSupport & WAVECAPS_SYNC)
			g_pDebugFile->Output(L"  The driver is synchronous and will block while playing a buffer. \r\n");
		if(wc.dwSupport & WAVECAPS_VOLUME)
			g_pDebugFile->Output(L"  Supports volume control. \r\n");
		if(wc.dwSupport & WAVECAPS_SAMPLEACCURATE)
			g_pDebugFile->Output(L"  Returns sample-accurate position information. \r\n");
		g_pDebugFile->Output(L"\r\n");
	}
#endif //CM_DEBUG
}
*/
/*
void ProcessDetach()
{
	if(g_Player)
		delete g_Player;
	if(g_pMiddleMan)
		delete g_pMiddleMan;
	if(g_pEQProxy)
		delete g_pEQProxy;
#ifdef MEASURE_PERFORMANCE
	delete g_pPerformance;
#endif //  MEASURE_PERFORMANCE
#ifdef CM_DEBUG
	g_pDebugFile->Output(L"Process Detach\r\n");
	delete g_pDebugFile;
	delete g_pDebugManager;
//	delete g_pDebugLastPlayed;
#endif //CM_DEBUG
}
*/

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
//			ProcessAttach();
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
//			ProcessDetach();
			break;
    }
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////
//
// Digital Streaming Services API Implmentation
//
struct DssControl
{
	DssControl() : bServiceInitialized(false), pIManager(NULL) {};
	bool bServiceInitialized;
	IDssCodecManager* pIManager;
};

DssControl g_DssControl;

HRESULT STDCALL DssInitialize(DWORD dwAuthenticator)
{
	if(g_DssControl.bServiceInitialized)
		return E_FAIL;
	
	IDssCodecManager* pIManager;
	HRESULT hr = CCodecManager::CreateInstance(dwAuthenticator,&pIManager);
	if(SUCCEEDED(hr))
	{
		g_DssControl.bServiceInitialized = true;
		g_DssControl.pIManager = pIManager;
	}
	return hr;
}

void STDCALL DssUninitialize()
{
	if(g_DssControl.bServiceInitialized)
	{
		g_DssControl.pIManager->Release();
	}
}


HRESULT STDCALL DssGetCodecManager(IDssCodecManager** ppIManager)
{
	if(ppIManager == NULL) return E_POINTER;
	if(g_DssControl.pIManager)
	{
		*ppIManager = g_DssControl.pIManager;
		(*ppIManager)->AddRef();
		return S_OK;
	}
	else
	{
		*ppIManager = NULL;
		return E_FAIL;
	}
}

HRESULT STDCALL DssCreateDictionary(IDssDictionary** ppIDictionary)
{
	if(ppIDictionary == NULL) return E_POINTER;
	return CDictionary::CreateInstance(ppIDictionary);
}

// ************
// Ed's changes
/*
HRESULT STDCALL DssCreateStreamFormat(IDssStreamFormat** ppIStreamFormat)
{
	if(ppIStreamFormat == NULL) return E_POINTER;
	return CStreamFormat::CreateInstance(ppIStreamFormat);
}
*/

HRESULT STDCALL DssCreateAudioStreamFormat(IDssAudioStreamFormat** ppIAudioStreamFormat)
{
	if(ppIAudioStreamFormat == NULL) return E_POINTER;
	return CAudioStreamFormat::CreateInstance(ppIAudioStreamFormat);
}

HRESULT STDCALL DssCreateVideoStreamFormat(IDssVideoStreamFormat** ppIVideoStreamFormat)
{
	if(ppIVideoStreamFormat == NULL) return E_POINTER;
	return CVideoStreamFormat::CreateInstance(ppIVideoStreamFormat);
}
