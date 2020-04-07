//........................................................................................
//........................................................................................
//.. File Name:  GDIUtils.h
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: Helper functionality for Windows CE GDI.
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

#ifndef _GDI_UTILS_H__
#define _GDI_UTILS_H__

#define SHGetMenu(hWndMB)  (HMENU)SendMessage((hWndMB), SHCMBM_GETMENU, (WPARAM)0, (LPARAM)0);
#define SHGetSubMenu(hWndMB,ID_MENU) (HMENU)SendMessage((hWndMB), SHCMBM_GETSUBMENU, (WPARAM)0, (LPARAM)ID_MENU);
#define SHSetSubMenu(hWndMB,ID_MENU) (HMENU)SendMessage((hWndMB), SHCMBM_SETSUBMENU, (WPARAM)0, (LPARAM)ID_MENU);

#define DEFAULT_FONT_HEIGHT 14

enum FontEnum
{
	fontTahoma = 0,
	fontLinoType = 1,
	fontCourier = 2,
};

LOGFONT* SetupFont(FontEnum font, bool bBold = false, bool bItalic = false);
BOOL DrawLine(HDC hdc, int X1, int Y1, int X2, int Y2);

bool IsMenuItemChecked(HMENU hMenu, UINT iMenuItemID);
BOOL SetMenuItemChecked(HMENU hMenu, UINT iMenuItemID, bool bChecked);

#endif