// PlayListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"
#include "PlayListDlg.h"
#include "medialibrary.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlayListDlg dialog

#define IDC_BUTTON_PL_SAVE	3000
#define IDC_BUTTON_PL_EDIT	3001
#define IDC_BUTTON_PL_DELETE	3002
#define IDC_BUTTON_PL_ADD	3003

#define IDC_MEDIADISPLAYTREE		3004


CPlayListDlg::CPlayListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlayListDlg)
	//}}AFX_DATA_INIT
}


void CPlayListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlayListDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlayListDlg, CDialog)
	//{{AFX_MSG_MAP(CPlayListDlg)
	ON_BN_CLICKED(IDC_BUTTON_PL_ADD, OnNewplaylist)
	ON_BN_CLICKED(IDC_BUTTON_PL_EDIT, OnEditplaylist)
	ON_BN_CLICKED(IDC_BUTTON_PL_DELETE,OnDelete)
	ON_BN_CLICKED(IDC_BUTTON_PL_SAVE,OnSave)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(TVN_BEGINDRAG, IDC_MEDIADISPLAYTREE, OnBegindragTree1)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayListDlg message handlers

BOOL CPlayListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bLDragging = false;

	hTLastSelectItem = NULL;
	m_hitemDrop = NULL;

	m_Brush = CreateSolidBrush(RGB(0xe2,0xeb,0xf5));

	FontNormal.CreateFont(-8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");

	SetFont(&FontNormal);

	hArrowCursor = LoadCursor(NULL,IDC_ARROW);
	hNoCursor = LoadCursor(NULL,IDC_NO);

	hDropCursor = LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_POINTER_COPY));


	bLabelAvailable.LoadBitmap(IDB_MUSIC_AVAILABLE);
	stAvailable.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,80,290,80+24),this);
	stAvailable.SetBitmap(HBITMAP(bLabelAvailable));

	bLabelPlaylists.LoadBitmap(IDB_PLAYLISTS);
	stPlaylists.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(298,80,298+290,80+24),this);
	stPlaylists.SetBitmap(HBITMAP(bLabelPlaylists));

	bTopNav.LoadBitmap(IDB_TOPNAV2);
	stTopNav.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,0,588,40),this);
	stTopNav.SetBitmap(HBITMAP(bTopNav));


	stInstructions.Create(_T("Drag items from music available to playlists.  Double click any track to preview."),WS_CHILD|WS_VISIBLE|SS_LEFT,CRect(5,50,583,70),this);
	stInstructions.SetFont(&FontNormal);

	m_xAdd.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON   ,CRect(229,5,229+72,35), this, IDC_BUTTON_PL_ADD);
	m_xEdit.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,  CRect(413,5,413+72,35), this, IDC_BUTTON_PL_EDIT);
	m_xDelete.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(321,5,321+72,35), this, IDC_BUTTON_PL_DELETE);
	m_xSave.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON  ,CRect(505,5,505+72,35), this, IDC_BUTTON_PL_SAVE);


	//up/down/over/disabled

	m_xAdd.SetTextColor(RGB(0,0,0));
	m_xAdd.SetToolTipText("");
	m_xAdd.SetSkin(IDB_B_LISTADD_UP,IDB_B_LISTADD_DOWN,IDB_B_LISTADD_OVER,IDB_B_LISTADD_DISABLED,0,0,0,0,0);

	m_xEdit.SetTextColor(RGB(0,0,0));
	m_xEdit.SetToolTipText("");
	m_xEdit.SetSkin(IDB_B_LISTEDIT_UP,IDB_B_LISTEDIT_DOWN,IDB_B_LISTEDIT_OVER,IDB_B_LISTEDIT_DISABLED,0,0,0,0,0);

	m_xDelete.SetTextColor(RGB(0,0,0));
	m_xDelete.SetToolTipText("");
	m_xDelete.SetSkin(IDB_B_LISTDELETE_UP,IDB_B_LISTDELETE_DOWN,IDB_B_LISTDELETE_OVER,IDB_B_LISTDELETE_DISABLED,0,0,0,0,0);

	m_xSave.SetTextColor(RGB(0,0,0));
	m_xSave.SetToolTipText("");
	m_xSave.SetSkin(IDB_B_SAVE_UP,IDB_B_SAVE_DOWN,IDB_B_SAVE_OVER,IDB_B_SAVE_DISABLED,0,0,0,0,0);

	m_mediaTree.Create(WS_CHILD|WS_VISIBLE|WS_EX_CLIENTEDGE|TVS_HASLINES|TVS_LINESATROOT
								|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_SINGLEEXPAND, 
								CRect(0,104,290,507),this, IDC_MEDIADISPLAYTREE);


	CImageList * pTreeImageList = new(CImageList);
	pTreeImageList->Create(16,16,ILC_COLOR8 + ILC_MASK,8,16);

	pTreeImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	pTreeImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	pTreeImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	pTreeImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	pTreeImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON4));

	m_mediaTree.SetImageList(pTreeImageList,TVSIL_NORMAL);

	m_playlistTree.Create(WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|WS_EX_CLIENTEDGE
								|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_EDITLABELS|TVS_SINGLEEXPAND , CRect(298,104,588,507),this, 1002);

	m_playlistTree.SetImageList(pTreeImageList,TVSIL_NORMAL);




	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



BOOL CPlayListDlg::PreTranslateMessage(MSG* pMsg) 
{
	// filter out the default enter and escape handling
     if( pMsg->message == WM_KEYDOWN )
        {
                if (    pMsg->wParam == VK_RETURN 
                     || pMsg->wParam == VK_ESCAPE)
                {
                        ::TranslateMessage(pMsg);
                        ::DispatchMessage(pMsg);
                        return TRUE; // DO NOT process further
                }

        }
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CPlayListDlg::OnNewplaylist() 
{
	// make a new playlist
	CString playlistName = "New playlist";

	if (m_playlistTree.TestDuplicatePlaylistName(NULL,playlistName))
	{
		int i = 2;
		CString a; a.Format("%d",i);

		while (m_playlistTree.TestDuplicatePlaylistName(NULL,playlistName+a))
		{
			i++;
			a.Format("%d",i);
		}
		playlistName = playlistName + a;
	}

	HTREEITEM hNewPlaylist = m_playlistTree.InsertItem(playlistName,2,2);
		
	m_playlistTree.SetItemData(hNewPlaylist,TET_PLAYLIST+0);

	m_playlistTree.PostMessage(TVM_EDITLABEL,0,(LPARAM)(HTREEITEM)hNewPlaylist);


}


void CPlayListDlg::OnEditplaylist()
{
	HTREEITEM hItem  = m_playlistTree.GetSelectedItem();

	if (hItem != NULL)
	{
		m_playlistTree.EditLabel(hItem);
	}
}


void CPlayListDlg::OnDelete()
{
	HTREEITEM hItem  = m_playlistTree.GetSelectedItem();

	if (hItem != NULL)
	{
		m_playlistTree.DeleteItem(hItem);
		pMainDlg->EnableSync(true);
	}
}


void CPlayListDlg::OnSave()
{
	pMainDlg->OnSync();

}





HBRUSH CPlayListDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if ((nCtlColor == CTLCOLOR_DLG) ||
		(nCtlColor == CTLCOLOR_STATIC))
	{
		if (nCtlColor == CTLCOLOR_STATIC)
			pDC->SetBkMode(TRANSPARENT);

		return m_Brush;
	}
	else
	{

		HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		return hbr;
	}
}

void CPlayListDlg::PopulateTrees()
{
	m_mediaTree.Populate();

	m_playlistTree.Populate();

}

void CPlayListDlg::SyncWithDatabase()
{
	m_playlistTree.SyncWithDatabase();

}

void CPlayListDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	
	POINT pt = point;
	ClientToScreen( &pt );

	if (m_bLDragging)
	{
		m_bLDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		delete m_pDragImage;

		HTREEITEM m_hitemDrop = m_playlistTree.GetDropHilightItem();

		if (m_hitemDrop)
		{
			AddItem(m_hitemDrag,m_hitemDrop);
		}
		else
		{
			RECT listWindowRect;

			m_playlistTree.GetWindowRect(&listWindowRect);

			if ((pt.x > listWindowRect.left) &&
				(pt.x < listWindowRect.right) &&
				(pt.y < listWindowRect.bottom) &&
				(pt.y > listWindowRect.top))
			{
				// make a new playlist
				CString playlistName = m_mediaTree.GetItemText(m_hitemDrag);

				if (m_playlistTree.TestDuplicatePlaylistName(NULL,playlistName))
				{

					int i = 2;
					CString a; a.Format("%d",i);
	
					while (m_playlistTree.TestDuplicatePlaylistName(NULL,playlistName+a))
					{
						i++;
						a.Format("%d",i);
					}

					playlistName = playlistName + a;
				}

				HTREEITEM hNewPlaylist = m_playlistTree.InsertItem(playlistName,2,2);
		
				m_playlistTree.SetItemData(hNewPlaylist,TET_PLAYLIST+0);

				AddItem(m_hitemDrag,hNewPlaylist);

				m_playlistTree.PostMessage(TVM_EDITLABEL,0,(LPARAM)(HTREEITEM)hNewPlaylist);

			}
		}

		m_playlistTree.SelectDropTarget(NULL);
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

#define RECT_BORDER	10

void CPlayListDlg::DragMouseOnPlaylists(CPoint point)
{



	
	// Scroll Tree control depending on mouse position
	CRect rectClient;

	m_playlistTree.GetWindowRect(&rectClient);

	CPoint pt = point;
	m_playlistTree.ScreenToClient(&pt);


	HTREEITEM hTItem = m_playlistTree.HitTest(pt);
	int nScrollDir = -1;
	if ( point.y >= rectClient.bottom - RECT_BORDER)
		nScrollDir = SB_LINEDOWN;
	else
	if ( (point.y <= rectClient.top + RECT_BORDER) )
		nScrollDir = SB_LINEUP;

	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = m_playlistTree.GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		m_playlistTree.SendMessage(WM_VSCROLL, wParam);
	}
	
	nScrollDir = -1;
	if ( point.x <= rectClient.left + RECT_BORDER )
		nScrollDir = SB_LINELEFT;
	else
	if ( point.x >= rectClient.right - RECT_BORDER)
		nScrollDir = SB_LINERIGHT;
	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = m_playlistTree.GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		m_playlistTree.SendMessage(WM_HSCROLL, wParam);
	} 

	if ( hTItem != NULL ) 
	{

// uncomment this line to makes the tree auto-expand

//		m_playlistTree.Expand(hTItem, TVE_EXPAND);
		m_hitemDrop = hTItem;

		HTREEITEM dropItem = m_playlistTree.GetDropHilightItem();
		if (hTItem != dropItem)
		{
			CImageList::DragShowNolock(false);

			m_playlistTree.SelectDropTarget(hTItem);

			m_playlistTree.Invalidate();
			m_playlistTree.UpdateWindow();

			CImageList::DragShowNolock(true);

		}

	}

}


void CPlayListDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bLDragging)
	{
		POINT pt = point;
		ClientToScreen( &pt );

		CImageList::DragMove(pt);

		RECT listWindowRect;

		m_playlistTree.GetWindowRect(&listWindowRect);


		if ((pt.x > listWindowRect.left) &&
			(pt.x < listWindowRect.right) &&
			(pt.y < listWindowRect.bottom) &&
			(pt.y > listWindowRect.top))
		{


			SetCursor(hArrowCursor);

			DragMouseOnPlaylists(pt);


		}
		else
		{
			SetCursor(hNoCursor);

			m_playlistTree.SelectDropTarget(NULL);

		}
	
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CPlayListDlg::OnBegindragTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_mediaTree.SelectItem(m_hitemDrag);
	m_hitemDrop = NULL;
	m_pDragImage = m_mediaTree.CreateDragImage(m_hitemDrag);  // get the image list for dragging
	// CreateDragImage() returns NULL if no image list
	// associated with the tree view control
	if( !m_pDragImage )
		return;

	m_bLDragging = TRUE;

	m_playlistTree.SelectDropTarget(NULL);
	m_playlistTree.SelectItem(NULL);


	m_pDragImage->BeginDrag(0, CPoint(-5,-5));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter(NULL, pt);
	SetCapture();


}


void CPlayListDlg::AddItem(HTREEITEM hSrcTItem,HTREEITEM hDestTItem, bool bAddSiblings)
{
	int nImage;
	int nSelImage;
	while (	(hSrcTItem != NULL) && (hSrcTItem != m_hitemDrop)) 
	{
		// Get the item text and Image indexes for source tree control
		CString csItem = m_mediaTree.GetItemText(hSrcTItem);
		GetItemImages(hSrcTItem, nImage, nSelImage);

		DWORD dwDstItemData = m_playlistTree.GetItemData(hDestTItem);
		DWORD dwSrcItemData = m_mediaTree.GetItemData(hSrcTItem);

		DWORD dwDstItemType = (dwDstItemData & TET_MASK);
		DWORD dwSrcItemType = (dwSrcItemData & TET_MASK);

		if (dwDstItemType == TET_PLAYLIST)
		{
			// check if this is a different playlist

//			HTREEITEM hSrcParent = GetParentItem(hSrcTItem);

//			if (m_playlistTree.GetItemData(hSrcParent) != dwSrcItemData)
//			{
				// not dropping in the same playlist
				HTREEITEM hDestChildItem = m_playlistTree.AddItem(hDestTItem, csItem, TVI_LAST, nImage, nSelImage, dwSrcItemData);
				HTREEITEM hSrcChildItem = m_mediaTree.GetChildItem(hSrcTItem);

				if(hSrcChildItem != NULL) 
				{
					AddItem(hSrcChildItem, hDestTItem, true);
				}
				
				if (bAddSiblings)
				{
					hSrcTItem = m_mediaTree.GetNextSiblingItem(hSrcTItem);
					
				}
				else
				{
					hSrcTItem = NULL;
				}
/*				if ((void*)m_pSourceTreeCtrl == (void*)m_pDestTreeCtrl)
				{
					m_playlistTree.Select(hDestChildItem,TVGN_CARET);
				}*/
//			}
		}
		else if (dwDstItemType == TET_MEDIA)
		{
			// add all items to the parent of the dest item

			HTREEITEM hDstParent = m_playlistTree.GetParentItem(hDestTItem);

			HTREEITEM hInsertAfter = m_playlistTree.GetPrevSiblingItem(hDestTItem);
			if (hInsertAfter == NULL)
			{
				hInsertAfter = TVI_FIRST;
			}

			HTREEITEM hDestChildItem = m_playlistTree.AddItem(hDstParent, csItem, hInsertAfter, nImage, nSelImage, dwSrcItemData);

			HTREEITEM hSrcChildItem = m_mediaTree.GetChildItem(hSrcTItem);

			if(hSrcChildItem != NULL) 
			{
				AddItem(hSrcChildItem, hDestTItem,true);
			}

			if (bAddSiblings)
			{
				hSrcTItem = m_mediaTree.GetNextSiblingItem(hSrcTItem);
					
			}
			else
			{
				hSrcTItem = NULL;
			}

/*			if ((void*)m_pSourceTreeCtrl == (void*)m_pDestTreeCtrl)
			{
				m_playlistTree.Select(hDestChildItem,TVGN_CARET);
			}*/
		}
	}
	
}


void CPlayListDlg::GetItemImages(HTREEITEM hSrcTItem, int &nSelItemImage, int &nNonSelItemImage)
{
	
	CImageList *pImageList = m_mediaTree.GetImageList(TVSIL_NORMAL);
	//If no image list is associated with the tree control, return
	if (pImageList == NULL )
	{
		nSelItemImage =0;
		nNonSelItemImage = 0;
	}
	else
	{
		// If no image list is associated with Destination tree control
		// Set the image list of source tree control
		if ( m_playlistTree.GetImageList(TVSIL_NORMAL) == NULL )
		{
			m_playlistTree.SetImageList(pImageList, TVSIL_NORMAL);
		}
		// Get the image indexes
		m_mediaTree.GetItemImage(hSrcTItem, nSelItemImage, nNonSelItemImage);
	}
}

BOOL CPlayListDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	ShellExecute(HWND(this),"open","help.htm",NULL,NULL,SW_NORMAL);
	
	return true;//CDialog::OnHelpInfo(pHelpInfo);
}
