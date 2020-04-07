//........................................................................................
//........................................................................................
//.. File Name:  Scrollbar.h 
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content:  Interface to Scrollbar and base class for MS HWND handle. 
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

#ifndef _SCROLL_BAR_H__
#define _SCROLL_BAR_H__


class CWindow 
{

public:
	void Update() { ::UpdateWindow(_hwnd); }

	void AttachWindow(HWND hWnd) { _hwnd = hWnd; }
	operator HWND() const { return _hwnd; }
protected:
	HWND _hwnd;
};

class CScrollBar : public CWindow
{
public:
	CScrollBar();
	~CScrollBar();

	HWND Create(HINSTANCE hInst, HWND hWndParent, int id, const RECT& rc, DWORD dwStyle);
	const RECT& GetRect() const { return _rect; }
	int SetScrollInfo(int nElements, int nPageSize);
	int SetScrollRange(int nMin, int nMax);
	int SetScrollPage(int nPage);
	int SetScrollPos(int nPos);
	int GetScrollPos();
	int GetThumbTrackPos();

private:
	SCROLLINFO _si;
	RECT _rect;
};





#endif
