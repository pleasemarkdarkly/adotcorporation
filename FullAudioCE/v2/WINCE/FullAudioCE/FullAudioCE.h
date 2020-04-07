//........................................................................................
//........................................................................................
//.. File Name: FullAudioCE.h
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: Application specific defines for FullAudio for Windows CE.
//.. Usage: Controls main menu															..
//.. Last Modified By: Matthew Tadd mattt@iobjects.com						..	
//.. Modification date: 12/27/2000														..
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.  									..
//..	 All rights reserved. This code may not be redistributed in source or linkable  ..
//.. 	 object form without the express written consent of Interactive Objects.        ..
//.. Contact Information: www.iobjects.com												..
//........................................................................................
//........................................................................................

#if !defined(AFX_FULLAUDIOCE_H__E6720D94_D5FA_11D4_A91B_005004AD38E7__INCLUDED_)
#define AFX_FULLAUDIOCE_H__E6720D94_D5FA_11D4_A91B_005004AD38E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#define MENU_HEIGHT 26
#define WM_USER_PLAY			WM_USER + 1
#define WM_USER_STOP			WM_USER + 2
#define WM_USER_SETSONG			WM_USER + 3
#define WM_USER_UPDATE			WM_USER + 4

#define WM_USER_FRAME_PROGRESS	WM_USER + 13
#define WM_USER_FILE_END		WM_USER + 14
#define WM_USER_PLAYING			WM_USER + 15
#define WM_USER_STOPPED			WM_USER + 16
#define WM_USER_TOGGLE_PLAYER	WM_USER + 17



#endif // !defined(AFX_FULLAUDIOCE_H__E6720D94_D5FA_11D4_A91B_005004AD38E7__INCLUDED_)
