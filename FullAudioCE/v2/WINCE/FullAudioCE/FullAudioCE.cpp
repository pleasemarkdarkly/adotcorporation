//........................................................................................
//........................................................................................
//.. File Name:  FullAudioCE.cpp
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: entry point and main windows procedure
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
#include "FullAudioCE.h"
#include <commctrl.h>
#include <aygshell.h>
#include <sipapi.h>
#include "GDIUtils.h"
#include "MyCollection.h"

#include "MyPlayer.h"

const char c_szStorageCardMediaFile[] = "\\Storage Card\\FullAudio.asx";
const char c_szLocalMediaFile[] = "\\Windows\\FullAudio.asx";

#define MAX_LOADSTRING 100
#define ID_VSCROLLBAR  100
#define ID_EDIT		   101

#define SHOW_PLAYER_CONTROL		0
#define HIDE_PLAYER_CONTROL		1

const RECT c_rcCaption  = { 0, 0, 139, 20 };	
const RECT c_rcEdit = { 140, 1, 239, 19 };
const RECT c_rcScrollBar = { 240-12, 19, 240, 227 }; 
const RECT c_rcCollection = { 0, 20, 226, 227 };
const RECT c_rcPlayer = { 0, 229, 240, 320 - MENU_HEIGHT };

const int c_iCollectionBorder[2] = { 260, 227 };

TCHAR szTitleFilter[] = TEXT("Filter on Title");
TCHAR szArtistFilter[] = TEXT("Filter on Artist");
TCHAR szAlbumFilter[] = TEXT("Filter on Album");
TCHAR szGenreFilter[] = TEXT("Filter on Genre");
TCHAR szMyCollection[] = TEXT("My Collection");

#define EDIT_BUFFER_TITLE	0
#define EDIT_BUFFER_ARTIST	1
#define EDIT_BUFFER_ALBUM	2
#define EDIT_BUFFER_GENRE	3

typedef TCHAR EditBuffer[40];
EditBuffer szEditBuffers[4];

//const RECT c_rcCollection = { 0, 20, 227
// Screen rect for application is rt, { 0, 20, 227, 280 - MENU_HEIGHT };

// Global Variables:
HINSTANCE			hInst;					// The current instance
HMENU				hMenuCBSort;		    // handle to sort submenu
HMENU				hMenuCBShow;			// handle to show submenu
HMENU				hMenuCBPlayer;			// handle to player popup menu
HWND				hwndCB;					// The command bar handle
HWND				hwndSB;					// The main window scroll bar
HWND				hwndMain;				// The main window.
HWND				hwndEdit;				// Filter Text Box;
HWND				hwndAbout;

static SHACTIVATEINFO s_sai;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    SplashWndProc	(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	AboutWndProc	(HWND, UINT, WPARAM, LPARAM);
HWND				CreateRpCommandBar(HWND);


int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	HWND	hWndSplash = NULL;
	
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name
	TCHAR	szTitle[MAX_LOADSTRING];

	LoadString(hInstance, IDC_WINSPLASH, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	//Create and show the splashscreen.
	LoadString(hInstance, IDC_WINABOUT, szWindowClass, MAX_LOADSTRING);
	hwndAbout = CreateWindow(szWindowClass,szTitle, WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL);
	
	ShowWindow(hwndAbout,nCmdShow);

	LoadString(hInstance, IDC_WINSPLASH, szWindowClass, MAX_LOADSTRING);
	hWndSplash = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (!hWndSplash)
	{	
		return FALSE;
	}

	ShowWindow(hWndSplash, nCmdShow);
	UpdateWindow(hWndSplash);


	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FULLAUDIOCE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if(msg.hwnd == NULL)
		{
			//probably a WM_USER message
			if(msg.message >= WM_USER && msg.message < WM_USER + 500)
			{
				//send the message to the main window
				msg.hwnd = hwndMain;
			}
		}

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

void CreateMainWindow()
{
	RECT rc;
	TCHAR szWindowClass[MAX_LOADSTRING];
	TCHAR szTitle[MAX_LOADSTRING];

    //Create and show the main screen.
	LoadString(hInst, IDC_FULLAUDIOCE, szWindowClass, MAX_LOADSTRING);
	LoadString(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	
	hwndMain = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInst, NULL);
	if (!hwndMain)
	{	
		return;
	}
	//When the main window is created using CW_USEDEFAULT the height of the menubar (if one
	// is created is not taken into account). So we resize the window after creating it
	// if a menubar is present
	{
		GetWindowRect(hwndMain, &rc);
		rc.bottom -= MENU_HEIGHT;
		if (hwndCB)
			MoveWindow(hwndMain, rc.left, rc.top, rc.right, rc.bottom, TRUE);
	}

	ShowWindow(hwndMain, SW_SHOW);
	UpdateWindow(hwndMain);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FULLAUDIO));
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	RegisterClass(&wc);

	LoadString(hInstance, IDC_WINSPLASH, szWindowClass, MAX_LOADSTRING);

	//SplashScreen Window Class
    wc.lpfnWndProc		= (WNDPROC) SplashWndProc;
    wc.lpszClassName	= szWindowClass;
	
	RegisterClass(&wc);

	LoadString(hInstance, IDC_WINABOUT, szWindowClass, MAX_LOADSTRING);

	wc.lpfnWndProc		= (WNDPROC) AboutWndProc;
	wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd = NULL;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_FULLAUDIOCE, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	//If it is already running, then focus on the window
	hWnd = FindWindow(szWindowClass, szTitle);	
	if (hWnd) 
	{
		//TODO - Show splashscreen again.

		SetForegroundWindow ((HWND) (((DWORD)hWnd) | 0x01));    
		return 0;
	} 

	MyRegisterClass(hInstance, szWindowClass);
	
	return TRUE;
}


void GrabEditText(int CheckedSortMenu, bool bClear)
{
	switch(CheckedSortMenu)
	{
	case IDM_SORT_TITLE: 
		GetWindowText(hwndEdit,szEditBuffers[EDIT_BUFFER_TITLE],40);
		break;
	case IDM_SORT_ARTIST: 
		GetWindowText(hwndEdit,szEditBuffers[EDIT_BUFFER_ARTIST],40);
		break;
	case IDM_SORT_ALBUM: 
		GetWindowText(hwndEdit,szEditBuffers[EDIT_BUFFER_ALBUM],40);
		break;
	case IDM_SORT_GENRE: 
		GetWindowText(hwndEdit,szEditBuffers[EDIT_BUFFER_GENRE],40);
		break;
	}
	
	if(bClear)
		SetWindowText(hwndEdit,TEXT(""));
}



void SetEditText(int CheckedSortMenu,LPTSTR& pszCaption)
{
	int len;
	switch(CheckedSortMenu)
	{
	case IDM_SORT_TITLE: 
		pszCaption = szTitleFilter; 
		SetWindowText(hwndEdit,szEditBuffers[EDIT_BUFFER_TITLE]);
		len = _tcslen(szEditBuffers[EDIT_BUFFER_TITLE]);
		break;
	case IDM_SORT_ARTIST: 
		pszCaption = szArtistFilter;
		SetWindowText(hwndEdit,szEditBuffers[EDIT_BUFFER_ARTIST]);
		len = _tcslen(szEditBuffers[EDIT_BUFFER_ARTIST]);
		break;
	case IDM_SORT_ALBUM: 
		pszCaption = szAlbumFilter;
		SetWindowText(hwndEdit,szEditBuffers[EDIT_BUFFER_ALBUM]);
		len = _tcslen(szEditBuffers[EDIT_BUFFER_ALBUM]);
		break;
	case IDM_SORT_GENRE: 
		pszCaption = szGenreFilter;
		SetWindowText(hwndEdit,szEditBuffers[EDIT_BUFFER_GENRE]);
		len = _tcslen(szEditBuffers[EDIT_BUFFER_GENRE]);
		break;
	}
	


	InvalidateRect(hwndMain,&c_rcCaption,TRUE);
	//Deselect Text
	SendMessage(hwndEdit,EM_SETSEL,len,len);
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static CMyCollection* pColl = NULL;
	static CScrollBar* pScrollBar = NULL;
	static CMyPlayer* pPlayerWindow = NULL;
	static int CheckedSortMenu = IDM_SORT_TITLE;
	static int ShowFlags = SHOW_TITLE;
	static int NumShowChecked = 1;
	static int ShowPlayer = 1;
	static TCHAR* pszCaption = NULL;
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	TCHAR szHello[MAX_LOADSTRING];

	switch (message) 
	{
		case WM_LBUTTONDOWN:
			//todo : handle touch and hold
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			RECT rt;
			rt = c_rcCollection;
			rt.bottom = c_iCollectionBorder[ShowPlayer];
			if(PtInRect(&rt,pt))
			{
				SetFocus(hWnd);
				pColl->LButtonDown(pt);
				break;
			}

			rt = c_rcPlayer;
			rt.top = c_iCollectionBorder[ShowPlayer];
			if(PtInRect(&rt,pt))
			{
				SetFocus(hWnd);
				pPlayerWindow->LButtonDown(pt);
				break;
			}

			return DefWindowProc(hWnd, message, wParam, lParam);
		case WM_VSCROLL:
			if(pScrollBar)
			{
				int iNewVertPos;
				int iVertPos;
				iVertPos = pScrollBar->GetScrollPos();
				switch(LOWORD(wParam))
				{
				case SB_LINEUP:
					pScrollBar->SetScrollPos(iVertPos-1); break;
				case SB_LINEDOWN:
					pScrollBar->SetScrollPos(iVertPos+1); break;
				case SB_PAGEUP:
					pScrollBar->SetScrollPos(iVertPos - pColl->GetNumElements()); break;
				case SB_PAGEDOWN:
					pScrollBar->SetScrollPos(iVertPos + pColl->GetNumElements()); break;
				case SB_THUMBTRACK:
					iNewVertPos = pScrollBar->GetThumbTrackPos();
					pScrollBar->SetScrollPos(iNewVertPos); break;
				}
	
				iNewVertPos = pScrollBar->GetScrollPos();
				if(iNewVertPos != iVertPos)
				{
					pColl->PopulateFromPlaylist(iNewVertPos);
				}
			}
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case ID_EDIT:
				{
					switch(wmEvent)
					{
					case EN_SETFOCUS:
						// open SIP
						SetEditText(CheckedSortMenu,pszCaption);

						SipShowIM( SIPF_ON );

						GetWindowText(hwndEdit,szHello,MAX_LOADSTRING);
						pColl->Filter(szHello);

						InvalidateRect(hWnd,NULL,FALSE);
						break;
					case EN_KILLFOCUS:
						// close SIP
						GrabEditText(CheckedSortMenu,true);
						pszCaption = szMyCollection;
						SipShowIM( SIPF_OFF );
						InvalidateRect(hWnd,NULL,FALSE);
						break;
					case EN_UPDATE:
						if(hwndEdit == GetFocus())
						{
							GetWindowText(hwndEdit,szHello,MAX_LOADSTRING);
							pColl->Filter(szHello);
						}
						break;
					default:
						return DefWindowProc(hWnd,message,wParam,lParam);
					}
				}
				break;
				case IDM_HELP_ABOUT:
					//DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
					ShowWindow(hwndAbout,SW_SHOW);
				    break;
				case IDM_MAIN_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				case IDOK:
					SendMessage(hWnd, WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, 0), (LPARAM)hWnd);
					SendMessage (hWnd, WM_CLOSE, 0, 0);
					break;
				case IDM_SORT_TITLE:
					if(CheckedSortMenu != IDM_SORT_TITLE)
					{
						SetMenuItemChecked(hMenuCBSort,CheckedSortMenu,false);
						GrabEditText(CheckedSortMenu,true);
						if(pColl) pColl->SortMedia(TITLE);
						CheckedSortMenu = IDM_SORT_TITLE;
						SetEditText(CheckedSortMenu,pszCaption);
						SetMenuItemChecked(hMenuCBSort,CheckedSortMenu,true);
					}
					break;
				case IDM_SORT_ARTIST:
					if(CheckedSortMenu != IDM_SORT_ARTIST)
					{
						SetMenuItemChecked(hMenuCBSort,CheckedSortMenu,false);
						GrabEditText(CheckedSortMenu,true);
						if(pColl) pColl->SortMedia(ARTIST); 
						CheckedSortMenu = IDM_SORT_ARTIST;
						SetEditText(CheckedSortMenu,pszCaption);
						SetMenuItemChecked(hMenuCBSort,CheckedSortMenu,true);
					}
					break;
				case IDM_SORT_ALBUM:
					if(CheckedSortMenu != IDM_SORT_ALBUM)
					{
						SetMenuItemChecked(hMenuCBSort,CheckedSortMenu,false);
						GrabEditText(CheckedSortMenu,true);
						if(pColl) pColl->SortMedia(ALBUM); 
						CheckedSortMenu = IDM_SORT_ALBUM;
						SetEditText(CheckedSortMenu,pszCaption);
						SetMenuItemChecked(hMenuCBSort,CheckedSortMenu,true);
					}
					break;
				case IDM_SORT_GENRE:
					if(CheckedSortMenu != IDM_SORT_GENRE)
					{
						SetMenuItemChecked(hMenuCBSort,CheckedSortMenu,false);
						GrabEditText(CheckedSortMenu,true);
						if(pColl) pColl->SortMedia(GENRE); 
						CheckedSortMenu = IDM_SORT_GENRE;
						SetEditText(CheckedSortMenu,pszCaption);
						SetMenuItemChecked(hMenuCBSort,CheckedSortMenu,true);
					}
					break;
				case IDM_SHOW_ALBUM:
					if(ShowFlags & SHOW_ALBUM)
					{
						if(NumShowChecked > 1)
						{
							//uncheck and reshow
							SetMenuItemChecked(hMenuCBShow,IDM_SHOW_ALBUM,false);
							ShowFlags &= ~SHOW_ALBUM;
							NumShowChecked--;
							if(pColl) pColl->ShowMedia(ShowFlags);
						}
					}
					else
					{
						//check and reshow
						SetMenuItemChecked(hMenuCBShow,IDM_SHOW_ALBUM,true);
						ShowFlags |= SHOW_ALBUM;
						NumShowChecked++;
						if(pColl) pColl->ShowMedia(ShowFlags);
					}
				break;
				case IDM_SHOW_GENRE:
					if(ShowFlags & SHOW_GENRE)
					{
						if(NumShowChecked > 1)
						{
							//uncheck and reshow
							SetMenuItemChecked(hMenuCBShow,IDM_SHOW_GENRE,false);
							ShowFlags &= ~SHOW_GENRE;
							NumShowChecked--;
							if(pColl) pColl->ShowMedia(ShowFlags);
						}
					}
					else
					{
						//check and reshow
						SetMenuItemChecked(hMenuCBShow,IDM_SHOW_GENRE,true);
						ShowFlags |= SHOW_GENRE;
						NumShowChecked++;
						if(pColl) pColl->ShowMedia(ShowFlags);
					}
				case IDM_SHOW_ARTIST:
					if(ShowFlags & SHOW_ARTIST)
					{
						if(NumShowChecked > 1)
						{
							//uncheck and reshow
							SetMenuItemChecked(hMenuCBShow,IDM_SHOW_ARTIST,false);
							ShowFlags &= ~SHOW_ARTIST;
							NumShowChecked--;
							if(pColl) pColl->ShowMedia(ShowFlags);
						}
					}
					else
					{
						//check and reshow
						SetMenuItemChecked(hMenuCBShow,IDM_SHOW_ARTIST,true);
						ShowFlags |= SHOW_ARTIST;
						NumShowChecked++;
						if(pColl) pColl->ShowMedia(ShowFlags);
					}
				break;
				case IDM_SHOW_TITLE:
					if(ShowFlags & SHOW_TITLE)
					{
						if(NumShowChecked > 1)
						{
							//uncheck and reshow
							SetMenuItemChecked(hMenuCBShow,IDM_SHOW_TITLE,false);
							ShowFlags &= ~SHOW_TITLE;
							NumShowChecked--;
							if(pColl) pColl->ShowMedia(ShowFlags);
						}
					}
					else
					{
						//check and reshow
						SetMenuItemChecked(hMenuCBShow,IDM_SHOW_TITLE,true);
						ShowFlags |= SHOW_TITLE;
						NumShowChecked++;
						if(pColl) pColl->ShowMedia(ShowFlags);
					}
				break;
				//Player Popup Menu
				case IDM_PLAYER_PLAY:
				{
					//MessageBox(NULL,TEXT("Play Clicked"),NULL,MB_OK);
					//if content is local then start playing
					if(pColl->IsSelectedRemote())
						MessageBox(NULL,TEXT("The selected song is remote and cannot be played until acquired."),NULL,MB_OK);
					else
					{
#ifdef INCLUDE_DSS
						if(pPlayerWindow->SetSong(pColl->GetSelectedFileName()))
						{
							pPlayerWindow->Play();
							EnableMenuItem(hMenuCBPlayer,IDM_PLAYER_PLAY,MF_BYCOMMAND | MF_GRAYED);
							EnableMenuItem(hMenuCBPlayer,IDM_PLAYER_STOP,MF_BYCOMMAND | MF_ENABLED);
						}
						else
						{
							MessageBox(NULL,TEXT("Couldn't recognize format."),NULL,MB_OK);
						}
#else
						MessageBox(NULL,TEXT("Playback is not enabled in this version."),NULL,MB_OK);
#endif
					}
					break;
				}
				case IDM_PLAYER_STOP:
				{
					EnableMenuItem(hMenuCBPlayer,IDM_PLAYER_STOP,MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenuCBPlayer,IDM_PLAYER_PLAY,MF_BYCOMMAND | MF_ENABLED);
#ifdef INCLUDE_DSS
					pPlayerWindow->Stop();
#endif
					break;
				}
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
		{
			RECT rt;
			RECT rtCaption;
			HGDIOBJ hold;
			HFONT hfont;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);
			rtCaption = rt;
			rt.bottom -= 40 + MENU_HEIGHT; //bottom for collection list.
			

			//white text on black background saying "My Collection"
			SetTextColor(hdc,RGB(255,255,255));
			SetBkColor(hdc,RGB(0,0,0));
			rtCaption.bottom = 20;
			FillRect(hdc, &rtCaption, (HBRUSH)GetStockObject(BLACK_BRUSH));
			rtCaption.left = 20;
			rtCaption.right = c_rcEdit.left;
			LOGFONT* pFont = SetupFont(fontTahoma,true,false);
			pFont->lfHeight = 17;
			hfont = CreateFontIndirect(SetupFont(fontTahoma,true,false));
			pFont->lfHeight = DEFAULT_FONT_HEIGHT;
			hold = SelectObject(hdc,hfont);
			DrawText(hdc, pszCaption, _tcslen(pszCaption), &rtCaption, DT_SINGLELINE); // | DT_VCENTER | DT_CENTER);
			SetTextColor(hdc,RGB(0,0,0));
			SetBkColor(hdc,RGB(255,255,255));

			SelectObject(hdc,hold);
			DeleteObject(hfont);
			rt.top = 20;

			// Screen rect for application is rt, { 0, 20, 227, 280 - MENU_HEIGHT };
			if(pColl) pColl->Draw(hdc, ps);
/*
			//Hello World
			LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
			rt.right -= 13;
			DrawText(hdc, szHello, _tcslen(szHello), &rt, 
				DT_SINGLELINE | DT_VCENTER | DT_CENTER);
*/
			//Line between collection and logo
			DrawLine(hdc, 0, c_iCollectionBorder[ShowPlayer], 240, c_iCollectionBorder[ShowPlayer]);

			if(pPlayerWindow) pPlayerWindow->Draw(hdc,ps);

			//Full Audio Logo at bottom of window

			EndPaint(hWnd, &ps);
			break; 
		}
#ifdef INCLUDE_DSS
		case WM_USER_UPDATE:
		{
			if(pPlayerWindow->IsStopped())
			{
				EnableMenuItem(hMenuCBPlayer,IDM_PLAYER_PLAY,MF_BYCOMMAND | MF_ENABLED);
				EnableMenuItem(hMenuCBPlayer,IDM_PLAYER_STOP,MF_BYCOMMAND | MF_GRAYED);
			}
			else
			{
				EnableMenuItem(hMenuCBPlayer,IDM_PLAYER_PLAY,MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(hMenuCBPlayer,IDM_PLAYER_STOP,MF_BYCOMMAND | MF_ENABLED);
			}
		}
		case WM_USER_STOPPED:
		{
			//MessageBox(NULL,TEXT("Stopped"),NULL,MB_OK);
			break;
		}
		case WM_USER_PLAYING:
		{
			//MessageBox(NULL,TEXT("Playing"),NULL,MB_OK);
			break;
		}
		case WM_USER_FRAME_PROGRESS:
		{
			break;
		}
		case WM_USER_FILE_END:
		{
			//MessageBox(NULL,TEXT("File Ended"),NULL,MB_OK);
			if(pColl->SetNextSong())
			{
				//MessageBox(NULL,TEXT("Found Next Song"),NULL,MB_OK);
				if(pPlayerWindow->SetSong(pColl->GetSelectedFileName()))
				{
					//MessageBox(NULL,TEXT("Set Next Song and Playing"),NULL,MB_OK);
					pPlayerWindow->Play();
				}
			}
			break;
		}
#endif
		case WM_USER_TOGGLE_PLAYER:
		{
			RECT rt;
			ShowPlayer = 1 - ShowPlayer;
			rt = c_rcCollection;
			rt.bottom = c_iCollectionBorder[ShowPlayer] - 1;
			pColl->SetWindowSize(rt);
			rt.left = 0;
			rt.right = 240;
			rt.top = rt.bottom + 2;
			rt.bottom = 320 - MENU_HEIGHT;
			pPlayerWindow->SetWindowRect(rt);
			pPlayerWindow->Show(ShowPlayer == 1);
			InvalidateRect(hWnd,NULL,TRUE);
			UpdateWindow(hWnd);
			break;
		}
		case WM_CREATE:
			hwndCB = CreateRpCommandBar(hWnd);
			
//			hMenuCBSort = (HMENU)SHMenuBar_GetMenu (hwndCB, IDM_SORT_MENU);
//			hMenuCBShow = (HMENU)SHMenuBar_GetMenu (hwndCB, IDM_SHOW_MENU);
//			hMenuCBSort = (HMENU)SHGetMenu (hwndCB, IDM_SORT_MENU);
//			hMenuCBShow = (HMENU)SHGetMenu (hwndCB, IDM_SHOW_MENU);
			hMenuCBSort = (HMENU)SHGetMenu (hwndCB);
			hMenuCBShow = (HMENU)SHGetMenu (hwndCB);

			hMenuCBPlayer = LoadMenu (hInst, MAKEINTRESOURCE(IDM_POPUP_PLAYER));	

			hwndEdit = CreateWindow(TEXT("edit"), 
									NULL,
									WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
									c_rcEdit.left,
									c_rcEdit.top,
									c_rcEdit.right - c_rcEdit.left,
									c_rcEdit.bottom - c_rcEdit.top,
									hWnd,
								    (HMENU) ID_EDIT,
									hInst,
									NULL);
			
			pszCaption = szMyCollection;

			//setup scrollbar
			pScrollBar = new CScrollBar();
			pScrollBar->Create(hInst,hWnd,ID_VSCROLLBAR,c_rcScrollBar,SBS_VERT);

			pPlayerWindow = new CMyPlayer(hWnd);
			pPlayerWindow->Show(true);
			pPlayerWindow->SetWindowRect(c_rcPlayer);

			//setup collection.
			pColl = new CMyCollection(pScrollBar);
			pColl->AttachWindow(hWnd);
			pColl->AttachPopupPlayer(hMenuCBPlayer);
			pColl->SetWindowSize(c_rcCollection);

			//Set Scrollbar positiong on screen;
			MoveWindow(*pScrollBar,
			   c_rcScrollBar.left,
			   c_rcScrollBar.top,
			   c_rcScrollBar.right - c_rcScrollBar.left,
			   (c_iCollectionBorder[ShowPlayer]+1)-c_rcScrollBar.top,
			   TRUE);
			
			//TODO : Detect Media
			if(pColl->LoadMedia(c_szStorageCardMediaFile) == 0)
			{
				//MessageBox(NULL,TEXT("No Storage Card Found.\n"),NULL,MB_OK);
				pColl->LoadMedia(c_szLocalMediaFile);
			}
			pColl->SortMedia(TITLE);

			{
				for(int i=0;i<4;i++)
				{
					szEditBuffers[i][0] = 0;
				}	
			}
			break;
		case WM_DESTROY:
			CommandBar_Destroy(hwndCB);

			delete pColl;
			delete pScrollBar;
			delete pPlayerWindow;

			DestroyMenu(hMenuCBPlayer);

			PostQuitMessage(0);
			break;
		case WM_SETTINGCHANGE:
			SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
     		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

LRESULT CALLBACK SplashWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR szHello[MAX_LOADSTRING];

	switch (message) 
	{
		case WM_CREATE:
			SetTimer(hWnd,1,1000,NULL);
			break;
		case WM_PAINT:
			RECT rt;
			HBITMAP hbmp;
			HGDIOBJ hold;
			HDC hdcMem;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);

			//Full Audio bitmap on splashscreen
			hbmp = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FULLAUDIO));
			hdcMem = CreateCompatibleDC(hdc);
			hold = SelectObject(hdcMem,hbmp);
			BitBlt(hdc, 29, 85, 182, 75, hdcMem, 0, 0, SRCCOPY);
			SelectObject(hdcMem,hold);
			DeleteObject(hbmp);
			DeleteDC(hdcMem);

			// version text
			rt.top = 170; rt.bottom = 200;
			LoadString(hInst, IDS_APPVERSION, szHello, MAX_LOADSTRING);
			DrawText(hdc, szHello, _tcslen(szHello), &rt, 
				DT_SINGLELINE | DT_VCENTER | DT_CENTER);

			// copyright text
			GetClientRect(hWnd, &rt);
			rt.top = 260;
			LoadString(hInst, IDS_COPYRIGHT, szHello, MAX_LOADSTRING);
			DrawText(hdc, szHello, _tcslen(szHello), &rt, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			EndPaint(hWnd, &ps);
			break; 
		case WM_LBUTTONDOWN:
		case WM_TIMER:
			KillTimer(hWnd,1);
			CreateMainWindow();
			PostMessage(hWnd, WM_CLOSE,0,0);
			break;
		case WM_DESTROY:
//			PostQuitMessage(0);
			break;
		case WM_SETTINGCHANGE:
			SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
     		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


HWND CreateRpCommandBar(HWND hwnd)
{
	SHMENUBARINFO mbi;

	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hwnd;
	mbi.nToolBarId = IDM_MENU;
	mbi.hInstRes   = hInst;
	mbi.nBmpId     = 0;
	mbi.cBmpImages = 0;

	if (!SHCreateMenuBar(&mbi)) 
		return NULL;

	return mbi.hwndMB;
}

// Mesage handler for the About box.
LRESULT CALLBACK AboutWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_LBUTTONDOWN:
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK) {
				ShowWindow(hWnd,SW_HIDE);
			}
			break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			HDC hdcMem;
			HBITMAP hbmp;
			HGDIOBJ hold;
			hdcMem = CreateCompatibleDC(hdc);
			hbmp = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_ABOUT));
			hold = SelectObject(hdcMem,hbmp);
			BitBlt(hdc, 0 , 20, 240, 280, hdcMem, 0, 0, SRCCOPY);
			SelectObject(hdcMem,hold);
			DeleteObject(hbmp);
			DeleteDC(hdcMem);
			EndPaint(hWnd,&ps);
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
    return 0;
}


