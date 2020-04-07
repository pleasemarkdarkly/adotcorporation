//........................................................................................
//........................................................................................
//.. File Name: PopupMenu.h
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content:  Interface to Popup Menu handlers.
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

#ifndef _POPUP_MENU_H__
#define _POPUP_MENU_H__

int HandlePopupMenu (HINSTANCE hInst, HWND hwnd, POINT point, int idMenu);
int HandlePopupMenu (HMENU hPopupMenu, HWND hwnd, POINT point);

#endif