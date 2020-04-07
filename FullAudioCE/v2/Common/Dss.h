#ifndef AFX_DSS_H__A70D0C4E_3A56_11D3_8D91_0090276149F2__INCLUDED_
#define AFX_DSS_H__A70D0C4E_3A56_11D3_8D91_0090276149F2__INCLUDED_

#if defined(WIN32) || defined(_WIN32_WCE)
#define WIN32_LEAN_AND_MEAN
#include <wtypes.h>
#else
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char  BYTE;

typedef long HRESULT;
#endif //WIN32

#if defined(_WIN32_WCE)
#define _ASSERTE(x) 
#else
#include <crtdbg.h>
#endif

#ifdef _DEBUG
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#endif

//pragmas
#pragma warning( once : 4290 )
#pragma warning(disable : 4250) //disable dominance warnings

//Thread return values
#if defined(_WIN32_WCE)
#undef THREAD_USES_CRT
#elif defined(WIN32)
#define THREAD_USES_CRT
#endif

#ifdef THREAD_USES_CRT
typedef unsigned int TRESULT;
#else
typedef unsigned long TRESULT;
#endif
//////////////////////////////////////////
//
//  Command completion codes for MediaPlayer
//
#define CMD_STOP 1
#define CMD_PAUSE 2
#define CMD_PLAY 3
#define CMD_SETFILE 4  
#define CMD_SETFRAME 5


/////////////////////////////////////////////////
//Known Format Types
//
#define FORMAT_RESERVED		0
#define FORMAT_PCM			1
#define FORMAT_MP3			2
#define FORMAT_REAL_G2		3
#define FORMAT_MS_WMA4		4
#define FORMAT_AUDIBLE		5
#define FORMAT_AVI			6

enum SeekOrigin
{
	soCurrent = 0,
	soEndOfFile = 1,
	soStartOfFile = 2
};

enum OutputStreamEvent
{
	oseFrameProgress = 0,
	oseWaveformVisualization ,
	oseSpectralVisualization  
};

enum ComputationalComplexity
{
	ccNone = 0,
	ccLow = 10,
	ccAverage = 20,
	ccHigh = 40
};

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct  _GUID
    {
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[ 8 ];
    }	GUID;
#define REFGUID const GUID&
#endif // !GUID_DEFINED

#ifndef __IID_DEFINED__
#define __IID_DEFINED__
typedef GUID IID;
#define REFIID const IID&
#define IsEqualIID IsEqualGUID
#endif //__IID_DEFINED__

//Interface Identifiers

// {CC3A0121-FAB4-4c4e-9F08-FD9C6ED3F3BB}
const IID IID_IDssUnknown = 
	{ 0xcc3a0121, 0xfab4, 0x4c4e, 
	{ 0x9f, 0x8, 0xfd, 0x9c, 0x6e, 0xd3, 0xf3, 0xbb } };

// {530B4118-8AB7-4b22-B49C-FC0F816EF6C5}
const IID IID_IDssInputStream = 
	{ 0x530b4118, 0x8ab7, 0x4b22, 
	{ 0xb4, 0x9c, 0xfc, 0xf, 0x81, 0x6e, 0xf6, 0xc5 } };

// {DBA8B5C1-47EE-413c-AAEA-34DFF50956CD}
const IID IID_IDssOutputStream =
	{ 0xdba8b5c1, 0x47ee, 0x413c, 
	{ 0xaa, 0xea, 0x34, 0xdf, 0xf5, 0x9, 0x56, 0xcd } };

// {4E90E205-D8D7-11d3-B696-009027614A9F}
const IID IID_DssAudioOutputStream = 
	{ 0x4e90e205, 0xd8d7, 0x11d3,
	{ 0xb6, 0x96, 0x0, 0x90, 0x27, 0x61, 0x4a, 0x9f } };

/*
// {4E90E206-D8D7-11d3-B696-009027614A9F}
const IID IID_DssVideoOutputStream = 
	{ 0x4e90e206, 0xd8d7, 0x11d3,
	{ 0xb6, 0x96, 0x0, 0x90, 0x27, 0x61, 0x4a, 0x9f } };
*/

// {C1214933-AD60-4b04-960B-75AE3F2AFBCA}
const IID IID_IDssOutputStreamEvent =
	{ 0xc1214933, 0xad60, 0x4b04, 
	{ 0x96, 0xb, 0x75, 0xae, 0x3f, 0x2a, 0xfb, 0xca } };

// {4E90E207-D8D7-11d3-B696-009027614A9F}
const IID IID_IDssAudioOutputStreamEvent = 
	{ 0x4e90e207, 0xd8d7, 0x11d3,
	{ 0xb6, 0x96, 0x0, 0x90, 0x27, 0x61, 0x4a, 0x9f } };

// {4E90E208-D8D7-11d3-B696-009027614A9F}
const IID IID_IDssVideoOutputStreamEvent =
	{ 0x4e90e208, 0xd8d7, 0x11d3,
	{ 0xb6, 0x96, 0x0, 0x90, 0x27, 0x61, 0x4a, 0x9f } };

// {28D0CD82-699B-432c-90F3-5A5A902A3A17}
const IID IID_IDssCodecEvent =
	{ 0x28d0cd82, 0x699b, 0x432c, 
	{ 0x90, 0xf3, 0x5a, 0x5a, 0x90, 0x2a, 0x3a, 0x17 } };

// {7F05E38D-D764-437a-88E3-4CE12BB41D14}
const IID IID_IDssMediaPlayerEvent =
	{ 0x7f05e38d, 0xd764, 0x437a, 
	{ 0x88, 0xe3, 0x4c, 0xe1, 0x2b, 0xb4, 0x1d, 0x14 } };

// {D2AFB43A-3F6F-4b6d-93D4-0AF362460597}
const IID IID_IDssCodecInfo =
	{ 0xd2afb43a, 0x3f6f, 0x4b6d, 
	{ 0x93, 0xd4, 0xa, 0xf3, 0x62, 0x46, 0x5, 0x97 } };

// {CAB05877-81C0-4a6d-88CB-729EF43D4ED0}
const IID IID_IDssSourceInfo =
	{ 0xcab05877, 0x81c0, 0x4a6d, 
	{ 0x88, 0xcb, 0x72, 0x9e, 0xf4, 0x3d, 0x4e, 0xd0 } };

// {D9F65FFA-814A-41e9-A24D-D853A9F237D5}
const IID IID_IDssDictionary =
	{ 0xd9f65ffa, 0x814a, 0x41e9, 
	{ 0xa2, 0x4d, 0xd8, 0x53, 0xa9, 0xf2, 0x37, 0xd5 } };

// {93C80713-A69D-41eb-A5BA-FD6FC4F561D5}
const IID IID_IDssStreamFormat =
	{ 0x93c80713, 0xa69d, 0x41eb, 
	{ 0xa5, 0xba, 0xfd, 0x6f, 0xc4, 0xf5, 0x61, 0xd5 } };

// {4E90E209-D8D7-11d3-B696-009027614A9F}
const IID IID_IDssAudioStreamFormat =
	{ 0x4e90e209, 0xd8d7, 0x11d3,
	{ 0xb6, 0x96, 0x0, 0x90, 0x27, 0x61, 0x4a, 0x9f } };

// {4E90E20A-D8D7-11d3-B696-009027614A9F}
const IID IID_IDssVideoStreamFormat =
	{ 0x4e90e20a, 0xd8d7, 0x11d3,
	{ 0xb6, 0x96, 0x0, 0x90, 0x27, 0x61, 0x4a, 0x9f } };

// {03E4E8E4-72AD-4b2e-9F91-2D1BB3D77BDB}
const IID IID_IDssMediaPlayer =
	{ 0x3e4e8e4, 0x72ad, 0x4b2e, 
	{ 0x9f, 0x91, 0x2d, 0x1b, 0xb3, 0xd7, 0x7b, 0xdb } };

// {D827B26A-EA83-46a5-8E60-AEC0C07922AA}
const IID IID_IDssCodecManager =
	{ 0xd827b26a, 0xea83, 0x46a5, 
	{ 0x8e, 0x60, 0xae, 0xc0, 0xc0, 0x79, 0x22, 0xaa } };

// {310D714B-9EC0-4fb1-9540-E40A490CB95F}
const IID IID_IDssDrivers =
	{ 0x310d714b, 0x9ec0, 0x4fb1, 
	{ 0x95, 0x40, 0xe4, 0xa, 0x49, 0xc, 0xb9, 0x5f } };

// {93CEF022-178C-4001-9BC9-080F25D752E6}
const IID IID_IDssDriver = 
	{ 0x93cef022, 0x178c, 0x4001, 
	{ 0x9b, 0xc9, 0x8, 0xf, 0x25, 0xd7, 0x52, 0xe6 } };

// {22929846-26DB-4b1f-A453-66CFE4BD9C90}
const IID IID_IDssCodec =  
	{ 0x22929846, 0x26db, 0x4b1f, 
	{ 0xa4, 0x53, 0x66, 0xcf, 0xe4, 0xbd, 0x9c, 0x90 } };

template <class T>
class IDssPtr
{
public:
	//Constructors
	IDssPtr(T* p=NULL) : m_pI(p) { if (p) p->AddRef(); } 

	//Destructor
	~IDssPtr() { Release(); }
	
	//Reset
	void Release()
	{
		if(m_pI != NULL)
		{
			T* pOld = m_pI;
			m_pI = NULL;
			pOld->Release();
		}
	}

	//Conversion
	operator T*() { return m_pI; }

	T* Detach() 
	{
		T* pOld = m_pI;
		m_pI = NULL;
		return pOld;
	}

	//Pointer Operations
	//Exception handling should be used here.
	T& operator*() {  _ASSERTE(m_pI != NULL); return *m_pI; }  
	T** operator&() { _ASSERTE(m_pI == NULL); return &m_pI; }
	T* operator->() { _ASSERTE(m_pI != NULL); return m_pI; }

	//Assignment from same interface
	T* operator=(T* pI)
	{
		if(m_pI != pI)
		{
			T* pOld = m_pI;
			m_pI = pI;
			if(m_pI != NULL) m_pI->AddRef();
			if(pOld != NULL) pOld->Release();
		}
		return m_pI;
	}
	
	//Boolean functions
	bool operator!() { return m_pI == NULL; }
	operator bool() const { return m_pI != NULL; }

private:
	T* m_pI;
};

#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )

// Msg: File not found on hard drive
const HRESULT E_FILE_NOT_FOUND = 
	MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x201);

// Msg: File not readable by decoder
const HRESULT E_FILE_WRONG_FORMAT = 
	MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x202);

// Msg: File not readable by decoder
const HRESULT E_FILE_UNSUPPORTED_ENCODING = 
	MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x203);

// Msg: Parameter out of range
const HRESULT E_OUTOFRANGE = 
	MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x204);

// Msg: Invalid format(s)
const HRESULT E_INVALID_FORMATS =
	MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x205);

// Msg: Too many instantiated objects of this type
const HRESULT E_TOO_MANY_OBJECTS =
	MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x206);

// Msg: Driver not authenticated by application.
const HRESULT E_DRIVER_NOT_AUTHENTICATED =
	MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x207);

// Msg: The event doesn't support the required interface.
const HRESULT E_INVALID_EVENT =
	MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x208);

struct FrameInfo
{
	DWORD CurrentFrame;
	DWORD NumFrames;
	DWORD CurrentChapter;
	DWORD NumChapter;
};

struct TimeInfo
{
	BYTE Hours;
	BYTE Minutes;
	BYTE Seconds;
	WORD Milliseconds;
	DWORD dwMilliseconds; 
};

struct VideoFrameData
{
	DWORD	dwDataSize;
	BYTE*	pData;
	FrameInfo	frameInfo;
};

//pWaveform
//If Channels == 2, then the first dwWaveformElementsPerChannel of pWaveform
//represent the left channel, the next dwWaveformElementsPerChannel of pWaveform
//are the right channel.  
//short Waveform_Left[dwWaveformElementsPerChannel] = pWaveform;
//short Waveform_Right[dwWaveformElementsPerChannel] = pWaveform + dwWaveformElementsPerChannel;
//
//pSpectral is of the following format:
//If Channels == 2 the first dwSpectralElementsPerChannel of pSpectral represent
//the left channel, the next dwSpectralElementsPerChannel of pSpectral are the
//right channel.  Each channel is a 2-dimensional array whose first dimension
//has dwSpectralDim1 elements, and the second dimension has dwSpectralDim2 elements.
//double Spectral_Left[dwSpectralDim1][dwSpectralDim2] = pSpectral;
//double Spectral_Right[dwSpectralDim1][dwSpectralDim2] = pSpectral + dwSpectralElementsPerChannel;
struct CodecEventInfo
{
	short* pWaveform;
	DWORD dwWaveformElementsPerChannel;
	float* pSpectral;
	DWORD dwSpectralElementsPerChannel;
	DWORD dwSpectralDim1;
	DWORD dwSpectralDim2;
	BYTE Channels;
	FrameInfo* pFrame;
};

typedef CodecEventInfo VisStruct;

#ifndef WIN32
struct WAVEFORMATEX { 
    WORD  wFormatTag; 
    WORD  nChannels; 
    DWORD nSamplesPerSec; 
    DWORD nAvgBytesPerSec; 
    WORD  nBlockAlign; 
    WORD  wBitsPerSample; 
    WORD  cbSize; 
}; 
#endif

struct STREAMFORMATEX
{
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;		//Sampling Frequency
	DWORD nBytesPerSec;			//= bitrate / 8
	WORD wBitsPerSample;		
	//DWORD nSamplesPerFrame;
	WORD cbSize;				//Number of bytes of extended information
};

typedef struct
{
	DWORD		dwTotalFrames;		// Total number of frames in the file.
	DWORD		dwStreams;			// The number of streams (video, audio, etc.) in the file.
	DWORD		dwMillisecondsPerFrame;	// The suggested rate of frame display.
	DWORD		dwWidth;			// The dimensions of the video image.
	DWORD		dwHeight;
} VideoFileInfo;

//Contains data relevant to IODriver
struct DriverDetails
{
	LPCTSTR szName;
	LPCTSTR szManufacturer;
	LPCTSTR szVersion;	
	LPCTSTR szCopyright;
	LPCTSTR szFileExtensions;  //Standard file filter
};

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DSSDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DSSDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//#define DSSDLL_EXPORTS
#ifdef DSSDLL_EXPORTS
#define DSSDLL_API __declspec(dllexport)
#else
#define DSSDLL_API __declspec(dllimport)
#endif

//Interfaces and brief definitions
struct IDssUnknown;	  //virtual base class to handle reference counting for all interfaces

//Interfaces handling data input/output
struct IDssInputStream;
struct IDssOutputStream;
struct IDssAudioOutputStream;
//struct IDssVideoOutputStream;

//Event Interfaces passing information back to each client interface
struct IDssOutputStreamEvent;
struct IDssAudioOutputStreamEvent;
//struct IDssVideoOutputStreamEvent;
struct IDssCodecEvent;
struct IDssMediaPlayerEvent;  //Handles notification back to IDssMediaPlayer interface

//Interfaces used for accessing information (read-only properties) of objects
struct IDssCodecInfo;
struct IDssSourceInfo;
struct IDssDictionary;
struct IDssStreamFormat;
struct IDssAudioStreamFormat;
//struct IDssVideoStreamFormat;

//High Level interfaces
struct IDssMediaPlayer; //Handles control of codec, input, and output streams
struct IDssCodecManager; 

//Low Level interfaces
struct IDssDrivers; //Handles collection of all drivers (codec Dll) on system
struct IDssDriver; //Handles instances of codecs in each driver Dll, allows user
				 //to determine if driver contains codecs valid for given source format,
				 //and to determine available destination formats.
struct IDssCodec;  //A given codec (algorithm) to convert from one format to another.

//reduces code size and prepares for COM port
//#define STDCALL __stdcall
#define STDCALL
////////////////////////////////////////////////////////////////
//
//  Abstract Base Class of all objects
//
struct IDssUnknown
{
	
	virtual HRESULT STDCALL AddRef() = 0; //Increments Reference count on given object
	virtual HRESULT STDCALL Release() = 0; //Decrements Reference coutn on object, if no
								   //references, delete object.
	virtual HRESULT STDCALL QueryInterface(const IID&,/*out*/ void**) = 0; 
									//Allows one to query interfaces for a given object.
};

////////////////////////////////////////////////////////////////
//
//  Event interfaces
//
typedef IDssUnknown IDssEvent;

struct IDssMediaPlayerEvent : virtual public IDssEvent
{
	virtual void STDCALL OnCommandComplete(int Command,DWORD Param,HRESULT Result) = 0;
	virtual void STDCALL OnFileEnd() = 0; //Called after mediaplayer receives last info concerning waveout playback
};

struct IDssCodecEvent : virtual public IDssEvent
{
};

struct IDssOutputStreamEvent : virtual public IDssEvent
{
	virtual void STDCALL OnFrameProgress(/*in*/ FrameInfo* pInfo) = 0;
};

struct IDssAudioOutputStreamEvent : public IDssOutputStreamEvent
{
	virtual void STDCALL OnVisualization(/*in*/ VisStruct* pVis) = 0;
};

struct IDssVideoOutputStreamEvent : public IDssOutputStreamEvent
{
	virtual HRESULT STDCALL OnFrameData(/*in*/ VideoFrameData* pData) = 0;
};

///////////////////////////////////////////////////////////////
//
//  Stream I/O interfaces
//
struct IDssInputStream : public IDssUnknown
{
	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat) = 0;
	virtual HRESULT STDCALL SetFormat(IDssStreamFormat* pIFormat) = 0;
	virtual HRESULT STDCALL Seek(long Offset, SeekOrigin Origin) = 0;
	virtual HRESULT STDCALL GetCurrentPosition(long* plOffset) = 0;
	virtual HRESULT STDCALL Read(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*out*/ DWORD* pdwBytesRead) = 0;
	virtual HRESULT STDCALL FileSize(DWORD* pdwFileSize) = 0;
	virtual HRESULT STDCALL IsEOF() = 0;
};


struct IDssOutputStream : public IDssUnknown
{
	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat) = 0;
	virtual HRESULT STDCALL Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten) = 0;
	virtual HRESULT STDCALL Advise(/*in*/ IDssEvent* pIEvent) = 0;

	//TODO : In version when QI is supported, move the following events to
	//a separate interface IDssWaveOutputStream that is derived from IDssOutputStream
	//The following are used in conjunction with the MediaPlayer interface and
	//should only be part of the IDssWaveOutputStream.
	virtual HRESULT STDCALL Stop() = 0;
	virtual HRESULT STDCALL Pause() = 0;
	virtual HRESULT STDCALL Restart() = 0;

	//
	virtual HRESULT STDCALL GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers) = 0;
};

struct IDssAudioOutputStream : public IDssOutputStream
{
	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat) = 0;
	virtual HRESULT STDCALL Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten) = 0;
	virtual HRESULT STDCALL Advise(/*in*/ IDssEvent* pIEvent) = 0;

	//TODO : In version when QI is supported, move the following events to
	//a separate interface IDssWaveOutputStream that is derived from IDssOutputStream
	//The following are used in conjunction with the MediaPlayer interface and
	//should only be part of the IDssWaveOutputStream.
	virtual HRESULT STDCALL Stop() = 0;
	virtual HRESULT STDCALL Pause() = 0;
	virtual HRESULT STDCALL Restart() = 0;

	virtual HRESULT STDCALL SetVolume(WORD wLeftChannel,WORD wRightChannel) = 0;
	virtual HRESULT STDCALL GetVolume(WORD* pwLeftChannel, WORD* pwRightChannel) = 0;
	//
	virtual HRESULT STDCALL GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers) = 0;
};

struct IDssVideoOutputStream : public IDssOutputStream
{
	virtual HRESULT STDCALL GetFormat(IDssStreamFormat** ppIFormat) = 0;
	virtual HRESULT STDCALL Write(/*in*/ BYTE* pBuffer, DWORD dwBytes,/*in*/ CodecEventInfo* pEventInfo,/*out*/ DWORD* pdwBytesWritten) = 0;
	virtual HRESULT STDCALL Advise(/*in*/ IDssEvent* pIEvent) = 0;

	//TODO : In version when QI is supported, move the following events to
	//a separate interface IDssWaveOutputStream that is derived from IDssOutputStream
	//The following are used in conjunction with the MediaPlayer interface and
	//should only be part of the IDssWaveOutputStream.
	virtual HRESULT STDCALL Stop() = 0;
	virtual HRESULT STDCALL Pause() = 0;
	virtual HRESULT STDCALL Restart() = 0;

	//
	virtual HRESULT STDCALL GetBufferInfo(DWORD* pdwFilledBuffers,DWORD* pdwTotalBuffers) = 0;
};

//////////////////////////////////////////////////////////////////////
//
// High level interfaces
//
struct IDssMediaPlayer : public IDssUnknown
{
	virtual HRESULT STDCALL Initialize(/*in*/ IDssCodec* pICodec) = 0;
	virtual HRESULT STDCALL Uninitialize() = 0;

	//Event Interface Management
	virtual HRESULT STDCALL Advise(IDssEvent* pNotify) = 0;
	virtual HRESULT STDCALL SetCallbackRate(OutputStreamEvent event, DWORD dwRate) = 0;

	//Decoding Management
	virtual HRESULT STDCALL Play() = 0;
	virtual HRESULT STDCALL Pause() = 0;
	virtual HRESULT STDCALL Stop() = 0;

	virtual HRESULT STDCALL SetFrame(DWORD Frame) = 0;
	
	virtual HRESULT STDCALL SetChapter(DWORD Stream) = 0; //MP3 return E_NOTIMPL
	
	//Information
	virtual HRESULT STDCALL GetCodecInfo(IDssCodecInfo** ppICodecInfo) = 0;
	
	//Worker thread management
	virtual HRESULT STDCALL SetPriority(int nPriority) = 0;
};

struct IDssCodecManager : public IDssUnknown
{
	virtual HRESULT STDCALL GetDrivers(IDssDrivers** ppIDrivers) = 0; 
	//Higher Level Functions
	virtual HRESULT STDCALL GetSupportedFileTypes(bool bSupportAll, LPTSTR* ppszFileTypes, DWORD* pchFileTypesLen) = 0;
	//For given source stream, find applicable driver (and source index in driver)
	//user can index dst indices for appropriate configuration of output file.
	//call IDssDriver::GetCodec to return codec.
	virtual HRESULT STDCALL FindCompatibleDriver(IDssInputStream* pInputStream, IDssSourceInfo** ppISourceInfo) = 0;
	
	//What would be really cool would be a way to go from one given source
	//STREAMFORMATEX header to a destination STREAMFORMATEX header without too
	//much thought on the part of the interface client.  Currently, the first
	//STREAMFORMATEX in the destination list is considered the "recommended"
	//format for best quality.
	virtual HRESULT STDCALL CreateMediaPlayer(IDssMediaPlayer** ppIPlayer) = 0;
	virtual HRESULT STDCALL OpenInputStreamFromFile(LPCTSTR szFileName, IDssInputStream** ppIFileInStream) = 0;
	// TODO: Instead of always streaming to an audio file, the file type should change depending on the format.
	virtual HRESULT STDCALL CreateOutputStreamFromFile(LPCTSTR szFileName,IDssAudioStreamFormat* pIFormat,IDssOutputStream** ppIFileOutStream) = 0;
	virtual HRESULT STDCALL CreateWaveOutStream(IDssAudioStreamFormat* pIFormat,DWORD dwNumBuffers, DWORD dwBufferSize, IDssAudioOutputStream** ppIWaveOutStream) = 0;
//	virtual HRESULT STDCALL CreateVideoOutputStream(IDssVideoStreamFormat* pIFormat, IDssVideoOutputStream** ppIVideoOutputStream) = 0;
};

////////////////////////////////////////////////////////////////////
//
//  Low Level Interfaces
//
// IDssDrivers contains a collection of IDssDriver objects, as well as
// some functions which behave
struct IDssDrivers : public IDssUnknown
{
	virtual HRESULT STDCALL GetNumDrivers(DWORD* pdwNumDrivers) = 0;
	virtual HRESULT STDCALL GetDriver(DWORD dwIndex, IDssDriver** ppIDriver) = 0;
	//virtual HRESULT STDCALL AddDriver(LPCTSTR szFileName, DWORD* pdwIndex) = 0;
	//virtual HRESULT STDCALL RemoveDriver(DWORD dwIndex) = 0;
};

struct IDssDriver : public IDssUnknown
{
	virtual HRESULT STDCALL GetNumSrcFormats(DWORD* pdwNumSrcFormats) = 0;
	virtual HRESULT STDCALL GetSrcFormat(DWORD dwSrcIndex,IDssStreamFormat** ppSrcFormat) = 0;
	virtual HRESULT STDCALL GetNumDstFormats(DWORD dwSrcIndex,DWORD* pdwNumDstFormats) = 0;
	virtual HRESULT STDCALL GetDstFormat(DWORD dwSrcIndex,DWORD dwDstIndex,IDssStreamFormat** ppDstFormat) = 0;
	virtual HRESULT STDCALL CreateCodec(IDssStreamFormat* pSrcFormat,IDssStreamFormat* pDstFormat, IDssCodec** ppICodec) = 0;
	virtual HRESULT STDCALL GetDriverDetails(/*out*/ DriverDetails** pDetails) = 0;
	//Need function to test if IDssInputStream is a valid source file that can be
	//decoded by this driver.  If valid, function sets the Stream Format for
	//the given IDssInputStream, and returns a IDssSourceInfo struct containing elements
	//similar to the CodecDetails struct
	virtual HRESULT STDCALL GetFileInfo(IDssInputStream* pInputStream,IDssSourceInfo** ppIInfo) = 0;
};

struct IDssCodec : public IDssUnknown
{
	//Initialize Codec...choose InputStream, OutputStream
	//Perform initialization specific to first time codec is used.
	virtual HRESULT STDCALL Initialize(IDssInputStream*, IDssOutputStream*) =0;
	//Decode frame currently pointed to by IDssInputStream and write result to
	//IOOutputStream.  Increment frame counter 1.
	virtual HRESULT STDCALL Convert(bool bFirstTime) = 0;
	virtual HRESULT STDCALL SetFrame(DWORD dwFrame) = 0; 
	virtual HRESULT STDCALL SetChapter(DWORD dwChapter) = 0;  //For Audible
	virtual HRESULT STDCALL Advise(IDssEvent* pEvent) = 0; 
	virtual HRESULT STDCALL SetCallbackRate(OutputStreamEvent event, DWORD dwRate) = 0;
	//Codec Info - each codec creates one instance of the codec info interface, which
	//contains read only properties 
	virtual HRESULT STDCALL GetInputStream(IDssInputStream** ppIInputStream) = 0;
	virtual HRESULT STDCALL GetOutputStream(IDssOutputStream** ppIOutputStream) = 0;
	virtual HRESULT STDCALL GetCodecInfo(IDssCodecInfo** ppICodecInfo) = 0;
};


//////////////////////////////////////////////////////////////////////
//
//  Information interfaces
//
struct IDssDictionary : public IDssUnknown
{
	virtual HRESULT STDCALL Count(DWORD* pdwNumElements) = 0;
	virtual HRESULT STDCALL GetEntry(DWORD dwIndex, LPCTSTR* ppszKey, LPCTSTR* ppszValue) = 0;
	virtual HRESULT STDCALL AddEntry(LPCTSTR szKey, LPCTSTR szValue) = 0;
	virtual HRESULT STDCALL FindEntry(LPCTSTR szKey, LPCTSTR* ppszValue) = 0;
};

struct IDssSourceInfo : public IDssUnknown
{
	virtual HRESULT STDCALL GetDriver(/*out*/ IDssDriver** ppIDriver) = 0;				// c f
	virtual HRESULT STDCALL GetSrcIndex(/*out*/ DWORD* pdwSrcIndex) = 0;				// c f		
	virtual HRESULT STDCALL GetDuration(/*in/out*/ TimeInfo* pDuration) = 0;			// c f
	virtual HRESULT STDCALL GetFrameInfo(/*in/out*/ FrameInfo* pFrameInfo) = 0;			// c f*
	virtual HRESULT STDCALL GetInputFrameSize(DWORD* pdwInFrameSize) = 0;						// c f
	virtual HRESULT STDCALL GetOutputFrameSize(DWORD* pdwOutFrameSize) = 0;
	virtual HRESULT STDCALL GetFileInfoMap(/*out*/ IDssDictionary** ppIDict) = 0;		// c f
};

//extends IDssSourceInfo interface for information specific to an
//instantiated codec
struct IDssCodecInfo : public IDssUnknown
{
	virtual HRESULT STDCALL GetDstIndex(/*out*/ DWORD* pdwDstIndex) = 0;				// c
	virtual HRESULT STDCALL GetCurrentTimeInfo(/*in/out*/ TimeInfo* pCurrentTime) = 0;	// c
//  virtual HRESULT STDCALL GetComputationalComplexity(ComputationalComplexity* pComplexity) = 0;
};

// ************
// Ed's changes
struct IDssStreamFormat : public IDssUnknown
{
	virtual void STDCALL SetFormatTag(WORD wFormatTag) = 0;
	virtual WORD STDCALL GetFormatTag() = 0;
	virtual void STDCALL SetSamplingFrequency(DWORD nSamplesPerSec) = 0;
	virtual DWORD STDCALL GetSamplingFrequency() = 0;
	virtual void STDCALL SetBytesPerSec(DWORD nBytesPerSec) = 0;
	virtual DWORD STDCALL GetBytesPerSec() = 0;
	virtual void STDCALL SetBitsPerSample(WORD wBitsPerSample) = 0;
	virtual WORD STDCALL GetBitsPerSample() = 0;
};

struct IDssAudioStreamFormat : public IDssStreamFormat
{
	virtual void STDCALL SetFormatTag(WORD wFormatTag) = 0;
	virtual WORD STDCALL GetFormatTag() = 0;
	virtual void STDCALL SetChannels(WORD wChannels) = 0;
	virtual WORD STDCALL GetChannels() = 0;
	virtual void STDCALL SetSamplingFrequency(DWORD nSamplesPerSec) = 0;
	virtual DWORD STDCALL GetSamplingFrequency() = 0;
	virtual void STDCALL SetBytesPerSec(DWORD nBytesPerSec) = 0;
	virtual DWORD STDCALL GetBytesPerSec() = 0;
	virtual void STDCALL SetBitsPerSample(WORD wBitsPerSample) = 0;
	virtual WORD STDCALL GetBitsPerSample() = 0;
};

struct IDssVideoStreamFormat : public IDssStreamFormat
{
	virtual void STDCALL SetBitmapInfo(BITMAPINFO* pBitmapInfo, DWORD dwBitmapInfoSize) = 0;
	virtual BITMAPINFO* STDCALL GetBitmapInfo() = 0;
	virtual void STDCALL SetVideoFileInfo(const VideoFileInfo& videoFileInfo) = 0;
	virtual VideoFileInfo STDCALL GetVideoFileInfo() = 0;
};

//////////////////////////////////////////////////////////////////////////
//
// Digital Streaming Services API
//
DSSDLL_API HRESULT STDCALL DssInitialize(DWORD dwAuthenticator);		
DSSDLL_API void STDCALL DssUninitialize();

DSSDLL_API HRESULT STDCALL DssGetCodecManager(IDssCodecManager** ppIManager);
DSSDLL_API HRESULT STDCALL DssCreateDictionary(IDssDictionary** ppIDictionary);
// ************
// Ed's changes
//DSSDLL_API HRESULT STDCALL DssCreateStreamFormat(IDssStreamFormat** ppIStreamFormat);
DSSDLL_API HRESULT STDCALL DssCreateAudioStreamFormat(IDssAudioStreamFormat** ppIStreamFormat);
//DSSDLL_API HRESULT STDCALL DssCreateVideoStreamFormat(IDssVideoStreamFormat** ppIStreamFormat);


inline bool IsEqualGuid(REFGUID rguid1, REFGUID rguid2)
{
   return (
	  ((PLONG) &rguid1)[0] == ((PLONG) &rguid2)[0] &&
	  ((PLONG) &rguid1)[1] == ((PLONG) &rguid2)[1] &&
	  ((PLONG) &rguid1)[2] == ((PLONG) &rguid2)[2] &&
	  ((PLONG) &rguid1)[3] == ((PLONG) &rguid2)[3]);
};

//All Com dependent stuff in here.
struct INondelegatingUnknown
{
	virtual HRESULT STDCALL NondelegatingAddRef() = 0;
	virtual HRESULT STDCALL NondelegatingRelease() = 0;
	virtual HRESULT STDCALL NondelegatingQueryInterface(const IID& iid, void** ppv) = 0;
};

class CUnknown : public INondelegatingUnknown
{
public:
	// Nondelegating IUnknown implementation
	virtual HRESULT STDCALL NondelegatingQueryInterface(const IID& iid, void** ppv) 
	{
		// CUnknown supports only IUnknown.
		if (IsEqualGuid(iid,IID_IDssUnknown))
		{
			IDssUnknown* pIUnk = reinterpret_cast<IDssUnknown*>(static_cast<INondelegatingUnknown*>(this));
			*ppv = pIUnk;
			pIUnk->AddRef();
			return S_OK;
		}	
		else
		{
			*ppv = NULL ;
			return E_NOINTERFACE ;
		}
	}

	virtual HRESULT STDCALL NondelegatingAddRef() 
	{
		return ::InterlockedIncrement(&m_cRef) ;
	}

	virtual HRESULT STDCALL NondelegatingRelease() 
	{
		::InterlockedDecrement(&m_cRef) ;
		if (m_cRef == 0)
		{
			FinalRelease() ;
			delete this ;
			return 0 ;
		}
		return m_cRef ;
	}

	// Constructor
	CUnknown(IDssUnknown* pUnknownOuter = NULL) : m_cRef(1)
	{
		// Set m_pUnknownOuter pointer.
		if (pUnknownOuter == NULL)
		{
#ifdef _DEBUG
			Trace(TEXT("Not aggregating; delegate to nondelegating IUnknown.\r\n")) ;
#endif
			m_pUnknownOuter = reinterpret_cast<IDssUnknown*>
								 (static_cast<INondelegatingUnknown*>
								 (this)) ;  // notice cast
		}
		else
		{
#ifdef _DEBUG
			Trace(TEXT("Aggregating; delegate to outer IUnknown.\r\n")) ;
#endif
			m_pUnknownOuter = pUnknownOuter ;
		}
	}

	// Destructor
	virtual ~CUnknown() {};

	// Initialization (especially for aggregates)
	virtual HRESULT FinalConstruct() { return S_OK ;}

	// Notification to derived classes that we are releasing
	virtual void FinalRelease() 
	{ 
#ifdef _DEBUG
		Trace("Increment reference count for final release.\r\n") ;
#endif
		m_cRef = 1;
	};

#ifdef _DEBUG
	void Trace(LPCTSTR szFormat, ...)
	{
		TCHAR m_sz[256];
		va_list list;
		va_start( list, szFormat );
		_vstprintf(m_sz,szFormat,list);
		OutputDebugString(m_sz);
	}
#endif

protected:
	// Support for delegation
	IDssUnknown* GetOuterUnknown() const
		{ return m_pUnknownOuter ;}

	// Reference count for this object
	long m_cRef ;
private:
	
	// Pointer to (external) outer IUnknown
	IDssUnknown* m_pUnknownOuter ;
};

///////////////////////////////////////////////////////////
//
// Delegating IUnknown
//   - Delegates to the nondelegating IUnknown, or to the
//     outer IUnknown if the component is aggregated.
//
#ifdef _DEBUG
#define DECLARE_IUNKNOWN(Derived_Class) \
	virtual HRESULT STDCALL \
		QueryInterface(const IID& iid, void** ppv) \
	{	                                                     \
		return GetOuterUnknown()->QueryInterface(iid,ppv) ;  \
	} ;	                                                     \
	virtual HRESULT STDCALL AddRef()	                     \
	{	                                                     \
		HRESULT hr = GetOuterUnknown()->AddRef();			 \
		Trace("++++AddRef " #Derived_Class "::%x ref %d\n",this,hr); \
		return hr;							                 \
	} ;	                                                     \
	virtual HRESULT STDCALL Release()	                     \
	{	                                                     \
		HRESULT hr = GetOuterUnknown()->Release();			 \
		Trace("----Release " #Derived_Class "::%x ref %d\n",this,hr); \
		return hr;							                 \
	} ;
#else
#define DECLARE_IUNKNOWN(Derived_Class) \
	virtual HRESULT STDCALL \
		QueryInterface(const IID& iid, void** ppv) \
	{	                                                     \
		return GetOuterUnknown()->QueryInterface(iid,ppv) ;  \
	} ;	                                                     \
	virtual HRESULT STDCALL AddRef()	                     \
	{	                                                     \
		return GetOuterUnknown()->AddRef() ;                 \
	} ;	                                                     \
	virtual HRESULT STDCALL Release()	                     \
	{	                                                     \
		return GetOuterUnknown()->Release() ;                \
	} ;
#endif

template<class _ComClass,class _Interface>
HRESULT InternalCreateInstance(_ComClass* p,_Interface** pp)
{
	HRESULT hRes = E_OUTOFMEMORY;
	if(p != NULL)
	{
		p->NondelegatingAddRef();
		hRes = p->FinalConstruct();
		p->NondelegatingRelease();
		if(hRes != S_OK)
		{
			delete p;
			p = NULL;
		}
	}
	*pp = static_cast<_Interface*>(p);
	return hRes;
};

#endif //AFX_DSS_H__A70D0C4E_3A56_11D3_8D91_0090276149F2__INCLUDED_