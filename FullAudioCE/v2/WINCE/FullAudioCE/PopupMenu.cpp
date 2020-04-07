//........................................................................................
//........................................................................................
//.. File Name:  PopupMenu.cpp
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: implementation of Popup Menu handlers.
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
#include "PopupMenu.h"

int HandlePopupMenu (HINSTANCE hInst, HWND hwnd, POINT point, int idMenu)
             
{
  HMENU hMenu;
  HMENU hMenuTrackPopup;
  
  /* Get the menu for the popup from the resource file. */
  hMenu = LoadMenu (hInst, MAKEINTRESOURCE(idMenu));
  if (!hMenu)
    return 0;
  
    /* Get the first menu in it which we will use for the call to
    * TrackPopup(). This could also have been created on the fly using
    * CreatePopupMenu and then we could have used InsertMenu() or
    * AppendMenu.
  */
  hMenuTrackPopup = GetSubMenu (hMenu, 0);
  
  /* Convert the mouse point to screen coordinates since that is what
  * TrackPopup expects.
  */
  ClientToScreen (hwnd, (LPPOINT)&point);
  
  /* Draw and track the "floating" popup */
  int res = TrackPopupMenu (hMenuTrackPopup, TPM_RETURNCMD, point.x, point.y, 0, hwnd, NULL);
  
  /* Destroy the menu since were are done with it. */
  DestroyMenu (hMenu);
  return res;
}

int HandlePopupMenu (HMENU hPopupMenu, HWND hwnd, POINT point)
             
{
  HMENU hMenuTrackPopup;
  
  if (!hPopupMenu)
    return 0;
  
    /* Get the first menu in it which we will use for the call to
    * TrackPopup(). This could also have been created on the fly using
    * CreatePopupMenu and then we could have used InsertMenu() or
    * AppendMenu.
  */
  hMenuTrackPopup = GetSubMenu (hPopupMenu, 0);
  
  /* Convert the mouse point to screen coordinates since that is what
  * TrackPopup expects.
  */
  ClientToScreen (hwnd, (LPPOINT)&point);
  
  /* Draw and track the "floating" popup */
  return TrackPopupMenu (hMenuTrackPopup, TPM_RETURNCMD, point.x, point.y, 0, hwnd, NULL);
}
