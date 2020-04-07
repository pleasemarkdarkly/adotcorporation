//........................................................................................
//........................................................................................
//.. File Name:  ScrollBar.cpp
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: Implementation of Scrollbar class.
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

#include "stdafx.h"
#include "Scrollbar.h"

CScrollBar::CScrollBar()
{

	_si.cbSize = sizeof(SCROLLINFO);
}

CScrollBar::~CScrollBar()
{

}

HWND CScrollBar::Create(HINSTANCE hInst, HWND hWndParent, int id, const RECT& rc, DWORD dwStyle)
{
	_rect = rc;
	_hwnd = CreateWindow (
				TEXT("scrollbar"),  // Class name
				NULL,               // Window text
				WS_VISIBLE | WS_CHILD | dwStyle,            // Window style
				rc.left,                  // x-coordinate of the upper-left corner
				rc.top,                  // y-coordinate of the upper-left corner
				rc.right-rc.left,      // The width of the edit control window
				rc.bottom - rc.top,      // The height of the edit control window
				hWndParent,               // Window handle to the parent window
				(HMENU) id,// The control identifier
				hInst,              // The instance handle
				NULL);              // Specify NULL for this parameter when 
									// you create a control.
	return _hwnd;
}

int CScrollBar::SetScrollInfo(int nElements, int nPageSize)
{
	_si.fMask = SIF_RANGE | SIF_PAGE;
	_si.nMin = 0;
	_si.nMax = nElements - 1;
	_si.nPage = nPageSize;
	return ::SetScrollInfo(_hwnd,SB_CTL,&_si,TRUE);
}

int CScrollBar::SetScrollRange(int nMin, int nMax)
{
	_si.fMask = SIF_RANGE;
	_si.nMin = nMin;
	_si.nMax = nMax;
	return ::SetScrollInfo(_hwnd,SB_CTL,&_si,TRUE);
}

int CScrollBar::SetScrollPos(int nPos)
{
	_si.fMask = SIF_POS;
	_si.nPos = nPos;
	return ::SetScrollInfo(_hwnd,SB_CTL,&_si,TRUE);
}

int CScrollBar::SetScrollPage(int nPage)
{
	_si.fMask = SIF_PAGE;
	_si.nPage = nPage;
	return ::SetScrollInfo(_hwnd,SB_CTL,&_si,TRUE);
}

int CScrollBar::GetScrollPos()
{
	_si.fMask = SIF_POS;
	GetScrollInfo(_hwnd,SB_CTL,&_si);
	return _si.nPos;
}

int CScrollBar::GetThumbTrackPos()
{
	_si.fMask = SIF_TRACKPOS;
	GetScrollInfo(_hwnd,SB_CTL,&_si);
	return _si.nTrackPos;
}


