#ifndef _CRYPT_H__
#define _CRYPT_H__

#include <wtypes.h>

HRESULT Cipher(BYTE* pKey,BYTE* pPlain,DWORD dwPlainLen,BYTE** ppCipher,DWORD* pdwCipherLen);
HRESULT Decipher(BYTE* pKey,BYTE* pCipher,DWORD dwCipherLen,BYTE** ppPlain,DWORD* pdwPlainLen);


#endif