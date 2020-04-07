//........................................................................................
//........................................................................................
//.. File Name:  Mycollection.h
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: framing logic for displaying lists of media in IO Jukebox
//	It'll behave similar to a listbox, displaying track names, 
//	if a file has been acquired or not, and a scroll bar.

//	Tracks acquired will be in normal type.
//	Tracks remote will be in italics.

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

#ifndef _MYCOLLECTION_H__
#define _MYCOLLECTION_H__

#include "Scrollbar.h"
#ifdef INCLUDE_JUKEBOX
#include "Jukebox.h"
#endif INCLUDE_JUKEBOX


#ifndef INCLUDE_JUKEBOX
typedef enum eColumn { ID, TITLE, ARTIST, ALBUM, YEAR, GENRE, FILEPATH, FILENAME, SONGLENGTH, PLAYCOUNT,
					  SAMPLEFREQ, BITRATE, NUMCHANNELS, FLAGS, EMBVIEW, VIEWS, CREATIONDATE, NOCOL };
#endif

#define SHOW_GENRE		0x8
#define SHOW_ARTIST		0x4
#define SHOW_ALBUM		0x2
#define SHOW_TITLE		0x1

#define MAX_ELEMENTSTRING 300

class CMyElement
{
friend class CMyCollection;
	CMyElement() : _bInvalidated(false) {};
	~CMyElement() {};

private:
	bool  _bInvalidated;
	TCHAR _psz[MAX_ELEMENTSTRING];
	RECT  _rect;
};

class CMyCollection : public CWindow
{
public:
	CMyCollection(CScrollBar* pScrollBar);
	~CMyCollection();

	int GetNumElements() { return (int)_dwNumElements; };

	int SetWindowSize(const RECT& rect);
	int LoadMedia(const char* szFileName);
    int PopulateFromPlaylist(int iIndexToFirstElement);
	int SortMedia(eColumn col);
	int ShowMedia(int flags);
	int Filter(LPCTSTR szFilter);
	void AttachPopupPlayer(HMENU hMenuPlayer) { _hMenuPlayer = hMenuPlayer; }

	//player required functionality
	bool IsSelectedRemote();
	const char* GetSelectedFileName();
	const char* GetSelectedSongName();
	bool SetNextSong();
	bool SetPrevSong();

	//message processors
	void Draw(HDC hdc, PAINTSTRUCT& ps);
	void LButtonDown(const POINT& pt);

private:
	BOOL Invalidate(bool bInvalidateElements = true);
    void UnselectElements();

	// window measurements
	RECT _rectCollection;
	RECT _rectIcons;
	RECT _rectElements;
	
	// data links
	DWORD _dwNumElements;
	CMyElement* _pElements;
	
	CScrollBar* _pScrollBar;
	
	int _SelectedIndex;
	int _SelectedSongId;
	int _IndexToFirstElement;

#ifdef INCLUDE_JUKEBOX
	structColumns _sortcolumns;
	JPlaylist _jplaylist;
#endif

	eColumn _sortcolumn;

	int _flags;
	HMENU _hMenuPlayer;
};

#endif