#ifndef __MAVDEFS_H__
#define __MAVDEFS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "scserver.h"
#include "mavinfo.h"

#define WMDM_WAVE_FORMAT_ALL  (WORD)0xFFFF
#define WCS_MIME_TYPE_ALL     L"*/*"

extern HINSTANCE g_hinstance;
extern CSecureChannelServer *g_pAppSCServer;
extern CMaverickInfo g_MaverickInfo;

#endif // __MAVDEFS_H__
