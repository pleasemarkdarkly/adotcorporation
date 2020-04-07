// Driver.cpp: implementation of the Driver class.
//
//////////////////////////////////////////////////////////////////////

#include "DriverCrypt.h"
#include <windows.h>
#include <winbase.h>
#include "des.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//If USE_KEY is defined, authentication is enabled, otherwise,
//there's no restriction on the use of the given codec.

#define USE_KEY
#undef USE_KEY

//Codecs and their associated keys.  Please make sure to list each key
//here in this file, as its the best place to keep copies of this data.
//In order to generate keys, use GuidGen.exe, which generates 128-bit
//unique identifiers....just take the first 64 bits for use with our
//current cryptographic algorithm.

//WavCodec sample - no key (USE_KEY undefined)
//MP3Codec - { 0x8A7B8E22, 0x67A6480A }; //MP3Codec
//WMACodec - { 0xDA7B632D, 0xB489461F }; //WMACodec
//Some sample keys yet to be assigned.
// { 0x0E426D26, 0xD5CA4292 };
// { 0x843A482D, 0x0E0EE526 };
// { 0xDA7B632D, 0xB489461F };
// { 0x974488CB, 0x5CEBBA22 };
// { 0x8CED6926, 0x8A4A4838 };
// { 0x9655A99A, 0x3148CACC };

#ifdef USE_KEY
//Key Associated with the given driver.
static DWORD dwKey[2] = { 0xDA7B632D, 0xB489461F }; //WMACodec


static WORD wData[4]; //Stores copy of data to be authenticated.
static bool g_bAuthenticated = false;
#endif

//If return S_OK, *pData is NULL and *pdwDataLen is 0, then there's no 
//authentication required for codec. so before returning, set
//g_bAuthenticated to true.
HRESULT DriverCrypt::GetAuthenticationData(BYTE** ppData,DWORD* pdwDataLen)
{
	//Generate Cipher data.
#ifdef USE_KEY
	SYSTEMTIME st;
	GetLocalTime(&st);
	wData[3] = st.wSecond*1063; //magic numbers are primes that will take advantage of full range of 16 bits
	wData[1] = st.wMinute*1069; 
	wData[2] = st.wMilliseconds*61;
	wData[0] = st.wHour*2029;
	return Cipher((BYTE*)dwKey,(BYTE*)wData,4*sizeof(WORD),ppData,pdwDataLen);
#else
	*ppData = NULL;
	*pdwDataLen = 0;
	return S_OK;
#endif
}

HRESULT DriverCrypt::Authenticate(BYTE* pData,DWORD dwDataLen)
{
#ifdef USE_KEY
	if(dwDataLen != sizeof(wData))
		return E_FAIL;

	WORD* pwData = (WORD*) pData;
	for(int i=0;i<4;i++)
		if(*(pwData+i) != wData[i])
			return E_FAIL;
	g_bAuthenticated = true;
#endif
	return S_OK;
}

bool DriverCrypt::IsAuthenticated()
{
#ifdef USE_KEY
	return g_bAuthenticated;
#else
	return true;
#endif
}