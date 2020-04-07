// StructHelper.h: interface for the StructHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRUCTHELPER_H__5134B1F9_4382_11D3_8D94_0090276149F2__INCLUDED_)
#define AFX_STRUCTHELPER_H__5134B1F9_4382_11D3_8D94_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dss.h"

CodecEventInfo* CloneCodecEventInfo(const CodecEventInfo* pEventInfo);
void DestroyCodecEventInfo(CodecEventInfo* p);


#endif // !defined(AFX_STRUCTHELPER_H__5134B1F9_4382_11D3_8D94_0090276149F2__INCLUDED_)
