//........................................................................................
//........................................................................................
//.. File Name: DynamicMenuScreen.h															..
//.. Date: 09/11/2001																	..
//.. Author(s): Daniel Bolstad															..
//.. Description of content: definition of class CDynamicMenuScreen			 				..
//.. Usage: The CDynamicMenuScreen class is an abstract base class from which more specific	..
//..	    menu screens are derived.  It provides functions for adding, displaying,	..
//..	    and highlighting menu items as well as automatically handling navigation	..
//..	    through keypress messages.													..
//.. Last Modified By: Daniel Bolstad  danb@fullplaymedia.com								..
//.. Modification date: 09/11/2001														..
//........................................................................................
//.. Copyright:(c) 1995-2002 Fullplay Media Systems Inc.  									..
//..	 All rights reserved. This code may not be redistributed in source or linkable  ..
//.. 	 object form without the express written consent of Fullplay Media Systems.        ..
//.. Contact Information: www.fullplaymedia.com												..
//........................................................................................
//........................................................................................

#ifndef DYNAMICMENUSCREEN_H_
#define DYNAMICMENUSCREEN_H_

#include <gui/peg/peg.hpp>
#include <main/ui/ScrollingListScreen.h>

typedef struct
{
	PegBitmap*	pBitmap;
	WORD		wCaptionSID;
	bool		bHasSubmenu;
//	bool		bSelectAll;
} DynamicMenuItemRec;


class CDynamicMenuScreen : public CScrollingListScreen
{
public:
	CDynamicMenuScreen(pGetScreenFunc pGetChildFunc, WORD wScreenTitleSID);
	virtual ~CDynamicMenuScreen() = 0;

	SIGNED Message(const PegMessage &Mesg);

	// Hides any visible screens.
	virtual void HideScreen();

	// Call the constructors of all the child menus
	virtual void SetParent(CScreen* pScreen);

	// Sets the title of the menu...
	void SetMenuTitle(const TCHAR* pszMenuTitle);

    void Draw();

protected:

	// Called when the user hits the select button.
	// Acts based upon the currently highlighted menu item.
	virtual void ProcessMenuOption(int iMenuIndex) = 0;

	// The functions help get info about the menu items so we can draw them correctly
	// These functions must be defined by the derived class
//	virtual bool MenuItemIsChecked(int iMenuIndex) = 0;
	virtual bool MenuItemHasSubMenu(int iMenuIndex) = 0;
	virtual const TCHAR* MenuItemCaption(int iMenuIndex) = 0;
	virtual PegBitmap* MenuItemBitmap(int iMenuIndex) = 0;

	// Force a redraw of the menu.
	virtual void ForceRedraw();

	// Notification from the scrolling list that the line index has changed.
	virtual void NotifyLineIndexChange(int iOldIndex, int iNewIndex);

	// Notification from the scrolling list that the list has scrolled up.
	virtual void NotifyScrollUp();

	// Notification from the scrolling list that the list has scrolled down.
	virtual void NotifyScrollDown();

	// Resets the menu so that the first item is at the top of the menu.
	// Called when the menu is exited.
	void ResetToTop();

	// Draws the menu, filling in the correct captions and showing the needed icons.
	void DrawMenu();

	// Called when the user hits the previous button.
	// Acts based upon the currently highlighted menu item.
	virtual void GotoPreviousMenu();

	// A pointer to a child screen's singlton function
	pGetScreenFunc m_pGetChildScreenFunc;

protected:
   	PegIcon*	m_aryPrevMenuArrows[DISPLAY_LINES];
	int			m_iTitleOffset;

private:

	// Creates all the icons used by the menu screen.
	void BuildScreen();

	// Scrolls the highlighted caption one letter at a time
	bool ScrollHighlightedCaption();

	PegIcon*	m_aryIcons[DISPLAY_LINES];
	PegString*	m_aryCaptions[DISPLAY_LINES];
	PegIcon*	m_arySubMenuArrows[DISPLAY_LINES];

	PegRect		m_aryHighlightRects[DISPLAY_LINES];

	TCHAR*	m_pszMenuTitle;
	bool		m_bMenuIsTitled;

	static const int sc_iScrollStartInterval;
	static const int sc_iScrollEndInterval;
	static const int sc_iScrollContinueInterval;
};

#endif  // MAINDYNAMICMENUSCREEN_H_
