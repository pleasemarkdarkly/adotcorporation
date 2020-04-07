#ifndef __ENCODE_H
#define __ENCODE_H

#include "stdafx.h"

//typedef unsigned char BYTE;
//typedef unsigned long DWORD;


bool HrBlobTo64Sz(BYTE bIn[], DWORD cbIn, char **cOut);

bool Hr64SzToBlob(char* cIn, BYTE **pbOut, DWORD *pcbOut);


#endif // __ENCODE_H