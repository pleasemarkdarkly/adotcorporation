//........................................................................................
//........................................................................................
//.. File Name:  MyPlayer.h
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content:  This class encapsulates the player control as well
//								as playback functionality.exposed through the mediaplayer.
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

#if !defined(AFX_MYPLAYER_H__8C5BC5B0_D79E_11D4_A920_005004AD38E7__INCLUDED_)
#define AFX_MYPLAYER_H__8C5BC5B0_D79E_11D4_A920_005004AD38E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef INCLUDE_DSS
#define USE_DSS
//#define USE_WMA_STANDALONE
#endif

#ifdef USE_DSS
#include "Dss.h"
#endif

class CMyPlayer 
{
public:
	CMyPlayer(HWND hWnd);
	virtual ~CMyPlayer();

	void SetWindowRect(const RECT& rt) { _rect = rt; }
	void Show(bool bShow) { _bShow = bShow; };

	bool IsPlaying() { return _bPlaying; };
	//void SetPlaying(bool bPlaying) { _bPlaying = bPlaying; };

	//message processors
	void Draw(HDC hdc, PAINTSTRUCT& ps);
	void LButtonDown(const POINT& pt);

#ifdef INCLUDE_DSS
public:
	void Play();
	void Stop();
	bool SetSong(const char* szFileName);
	bool IsStopped();
#endif

#ifdef USE_DSS
public:
	void Pause();
private:
	void DssInit();
	void DssCleanup();

	IDssCodecManager* _pIManager;	
	IDssMediaPlayer* _pIPlayer;
	TCHAR _SelectedSong[MAX_PATH];
#endif

#ifdef USE_WMA_STANDALONE
private:
	void WMAInit();
	void WMACleanup();

private:
	DWORD _dwThreadId;
	HANDLE _hThread;
#endif

private:
	HWND _hwnd;
	RECT _rect;
	bool _bShow;
	bool _bPlaying;


};

#endif // !defined(AFX_MYPLAYER_H__8C5BC5B0_D79E_11D4_A920_005004AD38E7__INCLUDED_)





