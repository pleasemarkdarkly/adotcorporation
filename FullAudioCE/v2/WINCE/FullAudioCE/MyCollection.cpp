//........................................................................................
//........................................................................................
//.. File Name:  MyCollection.h
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: implementation of MyCollection class.
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

#include "MyCollection.h"
#include "ASX.h"
#include "GDIUtils.h"
#include "DlgAcquire.h"
#include "resource.h"
#include "PopupMenu.h"

#define FLAG_REMOTE		0x1

#define ELEMENT_HEIGHT 17
#define ICON_WIDTH 14

extern HINSTANCE hInst;	

LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);

CMyCollection::CMyCollection(CScrollBar* pScrollBar) 
: _dwNumElements(0), _pElements(NULL), 
_pScrollBar(pScrollBar),
#ifdef INCLUDE_JUKEBOX 
_jplaylist("collection"), 
#endif
 _sortcolumn(TITLE), _flags(SHOW_TITLE), _hMenuPlayer(0)
{
	
}

CMyCollection::~CMyCollection() 
{
	delete [] _pElements; 
}

int CMyCollection::SetWindowSize(const RECT& rect)
{
	_rectCollection = rect;
	_rectIcons = rect;
	_rectIcons.right = ICON_WIDTH;
	_rectElements = rect;
	_rectElements.left = ICON_WIDTH + 1;

	RECT rt = _pScrollBar->GetRect();
	//Set Scrollbar positiong on screen;
	MoveWindow(*_pScrollBar,
			   rt.left,
			   rt.top,
			   rt.right - rt.left,
			   (rect.bottom+2)-rt.top,
			   TRUE);
	
	_dwNumElements = (_rectElements.bottom - _rectElements.top) / ELEMENT_HEIGHT;

	if(_pElements) delete [] _pElements;
	
	_pElements = new CMyElement[_dwNumElements];
	
	rt.left = _rectElements.left;
	rt.right = _rectElements.right;
	rt.top = _rectElements.top;
	rt.bottom = _rectElements.top + ELEMENT_HEIGHT - 1;

	for(DWORD dw=0;dw<_dwNumElements;dw++)
	{
		_pElements[dw]._rect = rt;
//		_tcscpy(_pElements[dw]._psz,TEXT("Test"));
        _tcscpy(_pElements[dw]._psz,TEXT(""));
		_pElements[dw]._bInvalidated = true;
		rt.top += ELEMENT_HEIGHT;
		rt.bottom += ELEMENT_HEIGHT;
	}
	
	PopulateFromPlaylist(_IndexToFirstElement);

	::InvalidateRect(_hwnd,&_rectCollection,FALSE);
	return 0;
}

BOOL CMyCollection::Invalidate(bool bInvalidateElements) 
{ 
	if(bInvalidateElements)
	{
		//Mark elements as dirty;
		for(DWORD dw = 0;dw < _dwNumElements;dw++)
		{
			_pElements[dw]._bInvalidated = true;
		}
	}
	return ::InvalidateRect(_hwnd,&_rectCollection,FALSE); 
};

void CMyCollection::Draw(HDC hdc, PAINTSTRUCT& ps)
{
#ifdef INCLUDE_JUKEBOX
	//each element will draw its text and icon
	HFONT hfont;
	HGDIOBJ hold;
	COLORREF color;
	HBITMAP hbmpRemote = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_REMOTE));
	HBITMAP hbmpLocal = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_LOCAL));
	HDC		hdcMem = CreateCompatibleDC(hdc);
	for(DWORD dw=0;dw<_dwNumElements;dw++)
	{
		CMyElement& ele = _pElements[dw];		
		if(ele._bInvalidated)
		{
			int idx = int(dw) + _IndexToFirstElement;
			if(idx < _jplaylist.GetSize())
			{
				JSong jsong(_jplaylist.GetSong(idx));
				int flags = jsong.GetFlags();
				//Draw Icon
				if(flags & FLAG_REMOTE)
					hold = SelectObject(hdcMem,hbmpRemote);
				else
					hold = SelectObject(hdcMem,hbmpLocal);

				BitBlt( hdc,
						_rectIcons.left,
						ele._rect.top,
						_rectIcons.right,
						ele._rect.bottom-ele._rect.top,
						hdcMem,
						0,
						0,
						SRCCOPY);
				SelectObject(hdcMem,hold);

				//Draw Text
				//Blank out row
				FillRect(hdc,&ele._rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
				//DrawText(hdc, ele._psz, _tcslen(ele._psz), &ele._rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
				
				if(_SelectedIndex == int(dw) + _IndexToFirstElement)
					color = SetTextColor(hdc,RGB(255,0,0));

				if(flags & FLAG_REMOTE)
				{
					//set font to italics
					hfont = CreateFontIndirect(SetupFont(fontTahoma,false,true));
					hold = SelectObject(hdc,hfont);
				}

				DrawText(hdc, ele._psz, _tcslen(ele._psz), &ele._rect, DT_SINGLELINE );

				if(flags & FLAG_REMOTE)
				{
					//set font back to regular
					SelectObject(hdc,hold);
					DeleteObject(hfont);
				}

				if(_SelectedIndex == int(dw) + _IndexToFirstElement)
					SetTextColor(hdc,color);
			}
			else
			{
				//whiteout element area
				RECT rt = ele._rect;
				rt.left = 0;
				FillRect(hdc,&rt,(HBRUSH)GetStockObject(WHITE_BRUSH));
			}
		}
	}
	DeleteObject(hbmpRemote);
	DeleteObject(hbmpLocal);
	DeleteDC(hdcMem);
#endif //INCLUDE_JUKEBOX
}

void CMyCollection::LButtonDown(const POINT& pt)
{
	int idx;
	bool bChangeSelection = true;

	idx = (pt.y - _rectElements.top) / ELEMENT_HEIGHT;
#ifdef INCLUDE_JUKEBOX
	if(idx >= _jplaylist.GetSize())
		return;
	
	int flags = _jplaylist.GetSong(idx + _IndexToFirstElement).GetFlags();

	if(pt.x < _rectIcons.right)
	{
		if(flags & FLAG_REMOTE)
		{
			if(HandlePopupMenu(hInst,_hwnd,pt,IDM_POPUP_ACQUIRE))
			{
				//process acquisition of media
				if(DialogBox(hInst,MAKEINTRESOURCE(IDD_ACQUIRE),_hwnd,(DLGPROC)Acquire))
				{
					//acquired
					_jplaylist.SetFlags(idx + _IndexToFirstElement,flags &= ~FLAG_REMOTE);
				}
			}
			else
				bChangeSelection = false;
		}
		
		_pElements[idx]._bInvalidated = true;
	}
	else
	{
	
		if(_SelectedIndex == int(idx) + _IndexToFirstElement)
		{
			if(_hMenuPlayer)
			{
				int iMenuId = HandlePopupMenu(_hMenuPlayer,_hwnd,pt);
				if(iMenuId)
					PostMessage(_hwnd,WM_COMMAND,iMenuId,0);
			};
		}
		else
		{
			//select media (only on touch, not touch and hold)
			_pElements[idx]._bInvalidated = true;
		}
	}
#endif

	if(bChangeSelection)
		_SelectedIndex = int(idx) + _IndexToFirstElement;
	
	//select song

	Invalidate(false);
}

int CMyCollection::LoadMedia(const char* szFileName)
{
	int num_songs;

#ifdef INCLUDE_JUKEBOX
	num_songs = ASX::Load(Jukebox::GetInstance(),szFileName);

	_jplaylist = Jukebox::GetInstance()->GetStandardPlaylist(_sortcolumn);
	const char* szSongFileName;

	for(int i = 0; i < _jplaylist.GetSize(); i++)
	{
		JSong jsong(_jplaylist.GetSong(i));
		szSongFileName = jsong.FileName();
		if(strstr(szSongFileName,"\\Remote\\"))
		{
			//Remote file, so flag it.
			//jsong.SetFlags(FLAG_REMOTE);
			_jplaylist.SetFlags(i,FLAG_REMOTE);
		}
		else 
		{
			//jsong.SetFlags(0);
			_jplaylist.SetFlags(i,0);
		}
	}
#endif

	PopulateFromPlaylist(0);

	return num_songs;
}

int CMyCollection::PopulateFromPlaylist(int iIndexToFirstElement)
{
	static WCHAR szTmp[MAX_ELEMENTSTRING];
	int num_displayed = 0;

	_IndexToFirstElement = iIndexToFirstElement;

#ifdef INCLUDE_JUKEBOX

	if(_jplaylist.GetSize() < int(_dwNumElements))
	{
		//don't show scrollbar
		ShowWindow(*_pScrollBar,SW_HIDE);
	}
	else
	{
		//show scrollbar
		ShowWindow(*_pScrollBar,SW_SHOWNA);
	}

	int idx = _IndexToFirstElement;
	for(DWORD dw=0; dw < _dwNumElements; dw++)
	{
		if(idx < _jplaylist.GetSize())
		{
			JSong js = _jplaylist.GetSong(idx);
			//alright, now toggle showing album, artist, and title based on _flags
			WCHAR* psz = szTmp;
			psz[0] = '\0';

			if(_flags & SHOW_GENRE)
			{
				MultiByteToWideChar(CP_ACP,0,js.Genre(),-1,psz,MAX_ELEMENTSTRING);
			}
			if(_flags & SHOW_ARTIST)
			{
				while(*psz) { psz++; }
				if(psz > szTmp)
				{
					_tcscpy(psz,TEXT(" - "));
					while(*psz) { psz++; }
				}
				MultiByteToWideChar(CP_ACP,0,js.Artist(),-1,psz,MAX_ELEMENTSTRING);				
			}
			if(_flags & SHOW_ALBUM)
			{
				while(*psz) { psz++; }
				if(psz > szTmp)
				{
					_tcscpy(psz,TEXT(" - "));
					while(*psz) { psz++; }
				}
				MultiByteToWideChar(CP_ACP,0,js.Album(),-1,psz,MAX_ELEMENTSTRING-wcslen(szTmp));
			}
			if(_flags & SHOW_TITLE)
			{
				while(*psz) { psz++; }
				if(psz > szTmp)
				{
					_tcscpy(psz,TEXT(" - "));
					while(*psz) { psz++; }
				}
				MultiByteToWideChar(CP_ACP,0,js.Title(),-1,psz,MAX_ELEMENTSTRING-wcslen(szTmp));
			}
			_tcscpy(_pElements[dw]._psz,szTmp);	
			num_displayed++;
		}
		else
		{
			//no more items to display
			_tcscpy(_pElements[dw]._psz,TEXT(""));
		}
					
		idx++;
	}
#endif //INCLUDE_JUKEBOX
	Invalidate();
	
	return num_displayed;
}

//TITLE, ARTIST, ALBUM, YEAR, GENRE

int CMyCollection::SortMedia(eColumn col)
{
	_sortcolumn = col;

#ifdef INCLUDE_JUKEBOX
	_jplaylist = Jukebox::GetInstance()->GetStandardPlaylist(_sortcolumn);

	_pScrollBar->SetScrollInfo(_jplaylist.GetSize(), _dwNumElements);

	_pScrollBar->SetScrollPos(0);

	//TODO: remember last selected element if at all possible
#endif //INCLUDE_JUKEBOX

	_SelectedIndex = 0;
	PopulateFromPlaylist(0);

#ifdef INCLUDE_JUKEBOX
	return _jplaylist.GetSize();
#else
	return 0;
#endif
}


int CMyCollection::ShowMedia(int flags)
{
	_flags = flags;
	return PopulateFromPlaylist(_IndexToFirstElement);
}

bool CMyCollection::IsSelectedRemote()
{
#ifdef INCLUDE_JUKEBOX
	return _jplaylist.GetSong(_SelectedIndex).GetFlags() & FLAG_REMOTE;
#else
	return false;
#endif
}

const char* CMyCollection::GetSelectedFileName()
{
#ifdef INCLUDE_JUKEBOX
	return _jplaylist.GetSong(_SelectedIndex).FileName();
#else
	return NULL;
#endif
}

const char* CMyCollection::GetSelectedSongName()
{
#ifdef INCLUDE_JUKEBOX
	return _jplaylist.GetSong(_SelectedIndex).Title();
#else
	return NULL;
#endif
}

bool CMyCollection::SetNextSong()
{
#ifdef INCLUDE_JUKEBOX
	int idx = _SelectedIndex + 1;

	while(idx != _SelectedIndex)
	{
		if(idx >= _jplaylist.GetSize()) idx = 0;
		if(!(_jplaylist.GetSong(idx).GetFlags() & FLAG_REMOTE))
			break;
		idx++;
	}

	if(idx == _SelectedIndex)
		return false;
	
	_SelectedIndex = idx;
	Invalidate(true);

	return true;
#endif
	return false;
}

bool CMyCollection::SetPrevSong()
{
#ifdef INCLUDE_JUKEBOX
	int idx = _SelectedIndex - 1;

	while(idx != _SelectedIndex)
	{
		if(idx < 0) idx = _jplaylist.GetSize() - 1;
		if(!(_jplaylist.GetSong(idx).GetFlags() & FLAG_REMOTE))
			break;
		idx--;
	}

	if(idx == _SelectedIndex)
		return false;

	idx = _SelectedIndex;
#endif
	return true;
}

/*
	// returns -1 if no match is available, 0 if set up correctly
	// col should indicate which column view is indexed on
	int StartCharComplete(char firstChar, eColumn col, JPlaylist pl);
	// returns the index value of the exact match, or -1 for no match, 
	//-2 if entire section matches, -9 is an error
	// use in character completion after mStartCharComplete for each sucessive character
	int NextChar(char ch, int position, eColumn col, JPlaylist pl);
	//prints the section that matches characters thus far (will only include those that fit 
	//in the strings[] array)
*/
#define MAX_FILTERSTRING 40
int CMyCollection::Filter(LPCTSTR szFilter)
{
	char psz[MAX_FILTERSTRING];
	int res = 0;

	//okay, a little bit of metakit magic here.
#ifdef INCLUDE_JUKEBOX

	WideCharToMultiByte(CP_ACP,0,szFilter,-1,psz,MAX_FILTERSTRING,NULL,NULL);

	if (strlen(psz) == 0)
		_jplaylist = Jukebox::GetInstance()->GetStandardPlaylist(_sortcolumn);
	else {
	//wipe playlist
	//_jplaylist.Empty();
	
	//alright, lets start applying filter to playlist
	int pos = 0;
	JPlaylist aPlaylist("apl");
	res = MetakitManager::GetInstance()->StartCharComplete(
			psz[pos++], 
			_sortcolumn, 
			Jukebox::GetInstance()->GetStandardPlaylist(_sortcolumn),
			aPlaylist);
	_jplaylist = aPlaylist;

	
	while(psz[pos] && _jplaylist.GetSize())
	{
		MetakitManager::GetInstance()->NextChar(psz[pos], pos ,_sortcolumn,_jplaylist);
		pos++;
	}
	}
#endif
	//at this point, _jplaylist should be filtered.
	PopulateFromPlaylist(0);
	return res;
}
