// PlaylistTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"
#include "MediaToPlaylistDropTarget.h"

#include "PlaylistTreeCtrl.h"
#include "medialibrary.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlaylistTreeCtrl

CPlaylistTreeCtrl::CPlaylistTreeCtrl()
{
	m_sourceItem=NULL;
}

CPlaylistTreeCtrl::~CPlaylistTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CPlaylistTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CPlaylistTreeCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlaylistTreeCtrl message handlers

void SaveAndSync()
{
	pMainDlg->EnableSync(true);
}


int CPlaylistTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_CTreeDropTarget.Register(this);
	
	return 0;
}

void CPlaylistTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hTSelItem = pNMTreeView->itemNew.hItem;

	if ((GetItemData(hTSelItem) & TET_MASK) == TET_MEDIA)
	{

		// Highlight selected item
		SelectItem(hTSelItem);
		Select(hTSelItem, TVGN_DROPHILITE);
		m_sourceItem=hTSelItem;

		COleDataSource *poleSourceObj = new COleDataSource ;
		CMediaToPlayListDropTarget::m_shWndTreeCtrl = m_hWnd;
		
		// Begin Drag operation
		DROPEFFECT dropeffect = poleSourceObj->DoDragDrop();
		// Remove the highlighting
		SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);
		// If user is moving item , delete selected item
		if ( dropeffect == DROPEFFECT_MOVE)
			DeleteItem(hTSelItem); 
		delete poleSourceObj;

		m_sourceItem=NULL;
	}

	*pResult = 0;
}

void CPlaylistTreeCtrl::Populate()
{

	//HTREEITEM hPlaylists = InsertItem("Playlists",1, 1);
	HTREEITEM hEntry;
	HTREEITEM hTrack;
	DWORD i;

	DeleteAllItems();

	DWORD numLists = theMediaDatabase.playListManager.GetNumPlaylists();
	CString playListName;

	for (i = 0;i<numLists;i++)
	{
		playListName = theMediaDatabase.playListManager.GetPlayListTitle(i);

		hEntry = InsertItem(playListName,2,2);
		SetItemData(hEntry,TET_PLAYLIST+i);

		// now add the songs for this playlist

		CPlayList * pPlaylist;
		CString tempString;

		pPlaylist = theMediaDatabase.playListManager.GetPlayList(i); 
		DWORD mediaKey;

		if (pPlaylist)
		{

			for (int j = 0; j < pPlaylist->playListEntries.GetSize();j++)
			{
				mediaKey = pPlaylist->playListEntries.GetAt(j);

				CMediaElement *pMedia = theMediaDatabase.mediaManager.GetMediaFromKey(mediaKey);
				if (pMedia)
				{

					// format string as "Artist - Title"
					tempString = theMediaDatabase.artistManager.GetArtistFromKey(pMedia->GetArtistIndex());
					tempString += " - ";
					tempString += pMedia->GetElementTitle();

					hTrack = InsertItem(tempString,4,4,hEntry,NULL);
					SetItemData(hTrack,TET_MEDIA+mediaKey);
				}
			}
		}
	}

}

void CPlaylistTreeCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	if ((GetItemData(pTVDispInfo->item.hItem) & TET_MASK) == TET_PLAYLIST)
	{
		DWORD data = GetItemData(pTVDispInfo->item.hItem);

		if ((data & 0x0F000000) == 0)
		{
			char szPreText[100];
	
			GetEditControl()->GetWindowText(szPreText,sizeof(szPreText));


			PreEditText = szPreText;

			GetEditControl()->LimitText(80);

		}

		
		*pResult = 0;

	}
	else
	{
		*pResult = 1;
	}

}

void CPlaylistTreeCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{

	
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	HTREEITEM curItem = pTVDispInfo->item.hItem;
	SelectItem(curItem);

	char szNewLabel[80];
	
	GetEditControl()->GetWindowText(szNewLabel,sizeof(szNewLabel));
	CString csNewLabel = szNewLabel;

	bool modified = false;

	if (csNewLabel.Find('<',0) != -1)
	{
		csNewLabel.Replace('<','[');
		modified = true;
	}

	if (csNewLabel.Find('>',0) != -1)
	{

		csNewLabel.Replace('>',']');
		modified = true;
	}


	GetEditControl()->SetWindowText(csNewLabel);

	DWORD data = GetItemData(curItem);


	if (data & 0x0F000000)
	{
		bool duplicate = TestDuplicatePlaylistName(curItem,PreEditText);

		if (duplicate)
		{
			int i = 1;
			CString a = "";

			while (TestDuplicatePlaylistName(curItem,PreEditText+a))
			{
				i++;
				a.Format("%d",i);
			}

			PreEditText = PreEditText + a;
		}


		SetItemText(curItem,PreEditText);
		SetItemData(curItem,data & 0xF0FFFFFF);
		Invalidate(TRUE);
		PostMessage(TVM_ENDEDITLABELNOW ,0,(LPARAM)(HTREEITEM)curItem);
	}
	else 
	{

			while (csNewLabel.Find(' ',0) == 0)
			{
				modified = true;
				csNewLabel = csNewLabel.Right(csNewLabel.GetLength()-1);
			}


			HTREEITEM hItem = GetRootItem();
			bool duplicate = TestDuplicatePlaylistName(curItem,csNewLabel);

			if (duplicate)
			{
				int i = 1;
				CString a = "";

				while (TestDuplicatePlaylistName(curItem,PreEditText+a))
				{
					i++;
					a.Format("%d",i);
				}

				PreEditText = PreEditText + a;
			}



			int len = csNewLabel.GetLength();

			if ((modified) && (len != 0))
			{
				PreEditText = csNewLabel;
			}


			if ((duplicate) || (len == 0) || modified)
			{

				SetItemData(curItem,data + 0x0F000000);
				Invalidate(TRUE);
				PostMessage(TVM_ENDEDITLABELNOW ,0,(LPARAM)(HTREEITEM)curItem);

				PostMessage(TVM_EDITLABEL,0,(LPARAM)(HTREEITEM)curItem);
			}
	}

	GetEditControl()->Invalidate(TRUE);
	Invalidate(TRUE);

	pMainDlg->EnableSync(true);

	*pResult = TRUE;
}

BOOL CPlaylistTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
      if( pMsg->message == WM_KEYDOWN )
        {
                // When an item is being edited make sure the edit control
                // receives certain important key strokes
                if( GetEditControl() 
                                && (pMsg->wParam == VK_RETURN 
                                        || pMsg->wParam == VK_DELETE 
                                        || pMsg->wParam == VK_ESCAPE
                                        || GetKeyState( VK_CONTROL) 
                                        )
                        )
                {
                        ::TranslateMessage(pMsg);
                        ::DispatchMessage(pMsg);
                        return TRUE;                            // DO NOT process further
                }

			// handle delete key

			HTREEITEM delItem;
			if ((pMsg->wParam == VK_DELETE) && (delItem = GetSelectedItem()))
			{
				DeleteItem(delItem);
				::TranslateMessage(pMsg);
                ::DispatchMessage(pMsg);
				SaveAndSync();
				return TRUE;
			}

			// handle f2 key to start editing

			HTREEITEM EditItem;
			if ((pMsg->wParam == VK_F2) && (EditItem = GetSelectedItem()))
			{
				PostMessage(TVM_EDITLABEL,0,(LPARAM)(HTREEITEM)EditItem);
				
				::TranslateMessage(pMsg);
                ::DispatchMessage(pMsg);
				
				return TRUE;
			}
        }

	return CTreeCtrl::PreTranslateMessage(pMsg);
}

bool CPlaylistTreeCtrl::CheckForItem(HTREEITEM hParent, long lParam)
{
	bool found = false;

	HTREEITEM checkItem = GetChildItem(hParent);
	while ((checkItem != NULL) && (!found))
	{
		found = (GetItemData(checkItem)==(DWORD)lParam && (checkItem !=m_sourceItem));
		checkItem = GetNextSiblingItem(checkItem);

	}

	return found;
}



HTREEITEM CPlaylistTreeCtrl::AddItem( 
	HTREEITEM hParent, 
	CString csItemName, 
	HTREEITEM hInsAfter, 
	int iSelImage , 
	int iNonSelImage, 
	long lParam
)
{
	HTREEITEM hItem = NULL;
	if ((lParam & TET_MASK) == TET_MEDIA)
	{
		// first check that we don't already have this in the playlist

		if (!CheckForItem(hParent,lParam))
		{

			CString displayName;
			DWORD mediaKey = (lParam & (~TET_MASK));
			CMediaElement *pMedia = theMediaDatabase.mediaManager.GetMediaFromKey(mediaKey);
			if (pMedia)
			{
			// format string as "Artist - Title"
				displayName = theMediaDatabase.artistManager.GetArtistFromKey(pMedia->GetArtistIndex());
				displayName += " - ";
				displayName += pMedia->GetElementTitle();
			}
	
			TV_ITEM tvItem;
			TV_INSERTSTRUCT tvIns;
	
			tvItem.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			int nLength = displayName.GetLength() ;
			tvItem.pszText = displayName.GetBuffer(nLength);
			tvItem.cchTextMax = nLength;
			tvItem.lParam = lParam;
	
			tvItem.iImage = iNonSelImage;
			tvItem.iSelectedImage = iSelImage;
		
			tvIns.item = tvItem;
			tvIns.hInsertAfter = hInsAfter;
			tvIns.hParent = hParent;
		
			hItem = InsertItem(&tvIns);
		


			SaveAndSync();
		}

	}

	
	return (hItem);
}


void CPlaylistTreeCtrl::SyncWithDatabase()
{

	CDWordArray tempArray;
	CString currName;

	theMediaDatabase.playListManager.ClearAll();

	HTREEITEM hRoot = GetRootItem();
	HTREEITEM hChild;

	while(hRoot != NULL)
	{
		// process each play list

		currName = GetItemText(hRoot);

		currName.Replace('<','[');
		currName.Replace('>',']');

		tempArray.RemoveAll();

		hChild = GetChildItem(hRoot);

		while (hChild != NULL)
		{
			tempArray.Add(GetItemData(hChild)&(~TET_MASK));
			hChild = GetNextSiblingItem(hChild);
		}

		theMediaDatabase.playListManager.AddPlayList(currName,tempArray);

		hRoot = GetNextSiblingItem(hRoot);
	}

}


BOOL CPlaylistTreeCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD | WS_TABSTOP | WS_VISIBLE |TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_EDITLABELS;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	
	return CTreeCtrl::PreCreateWindow(cs);
}

void CPlaylistTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (HitTest(point) == GetSelectedItem())
	{
		DWORD mediaKey = GetItemData(GetSelectedItem());
		if ((mediaKey & TET_MASK) == TET_MEDIA)
		{
			CMediaElement * theElement = theMediaDatabase.mediaManager.GetMediaFromKey(mediaKey - TET_MEDIA);
			ShellExecute(HWND(this),"open",theElement->GetFilePath(),NULL,NULL,SW_SHOWNOACTIVATE);
		}	
	}
	
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

bool CPlaylistTreeCtrl::TestDuplicatePlaylistName(HTREEITEM curItem, CString name)
{
	HTREEITEM hItem = GetRootItem();
	bool duplicate = false;

	while ((hItem != NULL) && (!duplicate))
	{
		if (hItem != curItem)
		{
			CString itemName = GetItemText(hItem);
			duplicate = (itemName.CompareNoCase(name)==0);
		}
		hItem = GetNextSiblingItem(hItem);
	}

	return duplicate;


}
