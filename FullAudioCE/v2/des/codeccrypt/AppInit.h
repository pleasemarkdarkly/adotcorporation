// AppInit.h: interface for the AppInit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPINIT_H__5FD5AF89_E343_4376_BF69_C3C1E81391F3__INCLUDED_)
#define AFX_APPINIT_H__5FD5AF89_E343_4376_BF69_C3C1E81391F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <wtypes.h>

class IAuthenticator
{
public:
	virtual DWORD GetNumKeys() = 0;
	virtual DWORD ValidateCipher(DWORD dwKeyIndex, BYTE* pSrc,DWORD dwSrcLen,BYTE** ppDst) = 0;
};


#endif // !defined(AFX_APPINIT_H__5FD5AF89_E343_4376_BF69_C3C1E81391F3__INCLUDED_)
