//........................................................................................
//........................................................................................
//.. File Name:  GDIUtils.cpp
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: implementation of GDI utilities.
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
#include "stdafx.h"
#include "GDIUtils.h"

// Tahoma, Frutiger Linotype, Courier New, Bookdings

static TCHAR szFontTahoma[] = _T("Tahoma");
static TCHAR szFontLinotype[] = _T("Frutiger Linotype");
static TCHAR szFontCourierNew[] = _T("Courier New");

static LOGFONT fontNormal =  { 
	                          DEFAULT_FONT_HEIGHT, 0, 0, 0, 400, FALSE, FALSE, FALSE, 
							  ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
							  DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS
							 };

LOGFONT* SetupFont(FontEnum font, bool bBold, bool bItalic)
{
	switch(font)
	{
	case fontLinoType: _tcscpy(fontNormal.lfFaceName,szFontLinotype); break;
	case fontCourier: _tcscpy(fontNormal.lfFaceName,szFontCourierNew); break;
	default: //fontTahoma: 
		_tcscpy(fontNormal.lfFaceName,szFontTahoma); 
	}
	fontNormal.lfWeight = (bBold) ? 700 : 400;
	fontNormal.lfItalic = (bItalic) ? TRUE : FALSE;
	return &fontNormal;
}


BOOL DrawLine(HDC hdc, int X1, int Y1, int X2, int Y2)
{
	static POINT pts[2];
	pts[0].x = X1;
	pts[0].y = Y1;
	pts[1].x = X2;
	pts[1].y = Y2;
	return Polyline(hdc, (POINT*)&pts, 2);
}


bool IsMenuItemChecked(HMENU hMenu, UINT iMenuItemID)
{
	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	GetMenuItemInfo(hMenu, iMenuItemID, FALSE, &mii);
	return static_cast<bool>(mii.fState & MFS_CHECKED);
}

BOOL SetMenuItemChecked(HMENU hMenu, UINT iMenuItemID, bool bChecked)
{
   if(bChecked)
	CheckMenuItem(hMenu,iMenuItemID,MF_BYCOMMAND | MF_CHECKED);
   else
	CheckMenuItem(hMenu,iMenuItemID,MF_BYCOMMAND | MF_UNCHECKED);
   return TRUE;
/*
	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	GetMenuItemInfo(hMenu,iMenuItemID,FALSE,&mii);
	if(bChecked)
	{
		mii.fState &= ~MFS_UNCHECKED;
		mii.fState |= MFS_CHECKED;
	}
	else
	{
		mii.fState |= MFS_UNCHECKED;
		mii.fState &= ~MFS_CHECKED;
	}
	return SetMenuItemInfo(hMenu,iMenuItemID,FALSE,&mii);
*/
}