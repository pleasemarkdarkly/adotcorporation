// App.cpp: implementation of the App class.
//
//////////////////////////////////////////////////////////////////////

#include "AppCrypt.h"
#include <windows.h>
#include "des.h"
#include "AppInit.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


static const DWORD g_AppKeys[2][2] = {
	{ 0x8A7B8E22, 0x67A6480A }, //MP3Codec
	{ 0xDA7B632D, 0xB489461F }, //WMACodec
};


class CAuthenticator : public IAuthenticator
{
public:
	virtual DWORD GetNumKeys()
	{
		return sizeof(g_AppKeys)/8;
	};
	
	virtual DWORD ValidateCipher(DWORD dwKeyIndex, BYTE* pSrc,DWORD dwSrcLen,BYTE** ppDst)
	{
		DWORD dwDstLen;
		HRESULT hr = Decipher((BYTE*)g_AppKeys[dwKeyIndex],pSrc,dwSrcLen,ppDst,&dwDstLen);
		if(SUCCEEDED(hr))
			return dwDstLen;
		else
		{
			*ppDst = NULL;
			return 0;
		}
	};
};

DWORD Authenticator()
{
	static CAuthenticator local;
	return reinterpret_cast<DWORD>(static_cast<IAuthenticator*>(&local));
}