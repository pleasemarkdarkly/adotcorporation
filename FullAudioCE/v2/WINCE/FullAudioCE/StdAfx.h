//........................................................................................
//........................................................................................
//.. File Name: stdafx.h														..
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: configuration include file
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

#if !defined(AFX_STDAFX_H__E6720D90_D5FA_11D4_A91B_005004AD38E7__INCLUDED_)
#define AFX_STDAFX_H__E6720D90_D5FA_11D4_A91B_005004AD38E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>

/**************************************************************
	Definitions of preprocessor directives in this file.

  INCLUDE_DSS		- Application interfaces with DSS for media playback.  Necessary for
				I		INCLUDE_MP3 and INCLUDE_WMA.

  INCLUDE_JUKEBOX	- Application interfaces with IO Jukebox for displaying FullAudio
						services (sorting/filtering/displaying titles.

  INCLUDE_MP3		- Support MP3 file type display and playback.  Requires INCLUDE_DSS.
  INCLUDE_WMA		- Support WMA file type display and playback.  Requires INCLUDE_DSS.
  
  SHOW_MP3_FILES	- Allows MP3 files in ASX to show up at runtime in the playlist.
  SHOW_WMA_FILES	- Allows WMA files in ASX to show up at runtime in the playlist.

**************************************************************/

#define INCLUDE_DSS
//#undef INCLUDE_DSS

#define INCLUDE_JUKEBOX
//#undef INCLUDE_JUKEBOX

#ifdef x86	//emulation only
#undef INCLUDE_DSS
#define SHOW_MP3_FILES
#define SHOW_WMA_FILES
#endif

#ifdef INCLUDE_DSS
#ifdef _ARM_				// Compaq iPaq
#define INCLUDE_MP3
//#define INCLUDE_WMA
#endif
#ifdef _SH3_				// HP Jornada 540
#define SHOW_MP3_FILES
#define SHOW_WMA_FILES
#undef INCLUDE_DSS
//#define INCLUDE_MP3
//#define INCLUDE_WMA
#endif
#endif

#ifdef INCLUDE_MP3
#define SHOW_MP3_FILES
#endif

#ifdef INCLUDE_WMA
#define SHOW_WMA_FILES
#endif

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E6720D90_D5FA_11D4_A91B_005004AD38E7__INCLUDED_)
