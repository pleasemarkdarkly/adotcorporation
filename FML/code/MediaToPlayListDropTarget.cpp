// MediaToPlayListDropTarget.cpp: implementation of the CMediaToPlayListDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FMLDlg.h"
#include "MediaToPlayListDropTarget.h"
#include "PlaylistTreeCtrl.h"
#include "MediaDragCtrl.h"
#include "medialibrary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define RECT_BORDER	10

HWND CMediaToPlayListDropTarget::m_shWndTreeCtrl = NULL;

/////////////////////////////////////////////////////////////////////////////
// CMediaToPlayListDropTarget

/********************************************************************
OnDragEnter()
	Called when the user drags the object in Tree control.
********************************************************************/
DROPEFFECT CMediaToPlayListDropTarget::OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, 
	DWORD dwKeyState, CPoint point 
)
{

	DROPEFFECT dropeffectRet = DROPEFFECT_COPY;
	if ( (dwKeyState & MK_SHIFT) == MK_SHIFT)
		dropeffectRet = DROPEFFECT_MOVE;
	return dropeffectRet;
}

/********************************************************************
OnDragOver()
	Called when the user drags the object over Tree control.
********************************************************************/
		
DROPEFFECT CMediaToPlayListDropTarget::OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, 
	DWORD dwKeyState, CPoint point )
{


	DROPEFFECT dropeffectRet = DROPEFFECT_MOVE;


	if ( (dwKeyState & MK_SHIFT) == MK_SHIFT)
		dropeffectRet = DROPEFFECT_COPY;
	// Expand and highlight the item under the mouse and 
	m_pDestTreeCtrl = (CPlaylistTreeCtrl *)pWnd;
	HTREEITEM hTItem = m_pDestTreeCtrl->HitTest(point);
	if ( hTItem != NULL ) 
	{
		m_pDestTreeCtrl->Expand(hTItem, TVE_EXPAND);
		m_pDestTreeCtrl->SelectDropTarget(hTItem);
	}
	else
	{
		dropeffectRet = DROPEFFECT_NONE;
	}
	
	// Scroll Tree control depending on mouse position
	CRect rectClient;
	pWnd->GetClientRect(&rectClient);
	pWnd->ClientToScreen(rectClient);
	pWnd->ClientToScreen(&point);
	int nScrollDir = -1;
	if ( point.y >= rectClient.bottom - RECT_BORDER)
		nScrollDir = SB_LINEDOWN;
	else
	if ( (point.y <= rectClient.top + RECT_BORDER) )
		nScrollDir = SB_LINEUP;

	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = pWnd->GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		pWnd->SendMessage(WM_VSCROLL, wParam);
	}
	
	nScrollDir = -1;
	if ( point.x <= rectClient.left + RECT_BORDER )
		nScrollDir = SB_LINELEFT;
	else
	if ( point.x >= rectClient.right - RECT_BORDER)
		nScrollDir = SB_LINERIGHT;
	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = pWnd->GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		pWnd->SendMessage(WM_HSCROLL, wParam);
	} 
	
	return dropeffectRet;
	
	
}

/********************************************************************
OnDragLeave()
	Called when the user drags the object out of Tree control.
********************************************************************/
void CMediaToPlayListDropTarget::OnDragLeave( CWnd* pWnd )
{
	// Remove Highlighting 
	m_pDestTreeCtrl = (CPlaylistTreeCtrl *)pWnd;
	m_pDestTreeCtrl->SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);
	
}

/********************************************************************
OnDrop()
	Called when the user drops the object in the  Tree control.
********************************************************************/
BOOL CMediaToPlayListDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point)
{


	//Get the selected item from Source and destination Tree controls 
	m_pSourceTreeCtrl = (CMediaDragCtrl *)CWnd::FromHandlePermanent(m_shWndTreeCtrl);
	//m_pDestTreeCtrl =  (CPlaylistTreeCtrl*)pWnd;

	HTREEITEM hSelectedItem = m_pSourceTreeCtrl->GetSelectedItem();
	HTREEITEM hTDropItem =m_pDestTreeCtrl->GetDropHilightItem();
	
	// Get the image indexes from ImageList, if one exists
	int nImage = 0;
	int nSelImage = 0;
	GetItemImages(hSelectedItem, nImage, nSelImage);
	
	// Add the items to destination tree control
	//m_hDestItem = m_pDestTreeCtrl->AddItem(hTDropItem, m_pSourceTreeCtrl->GetItemText(hSelectedItem), TVI_LAST, nImage, nSelImage, 0);

	
	AddItem(hSelectedItem,hTDropItem);
	
	//Remove highlighting
	m_pDestTreeCtrl->SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);

	return TRUE;
	
	
}

/********************************************************************
AddItem()
	Recursive function for adding items from selected tree control to
	destination tree control
********************************************************************/
void CMediaToPlayListDropTarget::AddItem(HTREEITEM hSrcTItem,HTREEITEM hDestTItem, bool bAddSiblings)
{
	int nImage;
	int nSelImage;
	while (	(hSrcTItem != NULL) && (hSrcTItem != m_hDestItem)) 
	{
		// Get the item text and Image indexes for source tree control
		CString csItem = m_pSourceTreeCtrl->GetItemText(hSrcTItem);
		GetItemImages(hSrcTItem, nImage, nSelImage);

		DWORD dwDstItemData = m_pDestTreeCtrl->GetItemData(hDestTItem);
		DWORD dwSrcItemData = m_pSourceTreeCtrl->GetItemData(hSrcTItem);

		DWORD dwDstItemType = (dwDstItemData & TET_MASK);
		DWORD dwSrcItemType = (dwSrcItemData & TET_MASK);

		if (dwDstItemType == TET_PLAYLIST)
		{
			// check if this is a different playlist

//			HTREEITEM hSrcParent = GetParentItem(hSrcTItem);

//			if (m_pDestTreeCtrl->GetItemData(hSrcParent) != dwSrcItemData)
//			{
				// not dropping in the same playlist
				HTREEITEM hDestChildItem = m_pDestTreeCtrl->AddItem(hDestTItem, csItem, TVI_LAST, nImage, nSelImage, dwSrcItemData);
				HTREEITEM hSrcChildItem = m_pSourceTreeCtrl->GetChildItem(hSrcTItem);

				if(hSrcChildItem != NULL) 
				{
					AddItem(hSrcChildItem, hDestTItem, true);
				}
				
				if (bAddSiblings)
				{
					hSrcTItem = m_pSourceTreeCtrl->GetNextSiblingItem(hSrcTItem);
					
				}
				else
				{
					hSrcTItem = NULL;
				}
				if ((void*)m_pSourceTreeCtrl == (void*)m_pDestTreeCtrl)
				{
					m_pDestTreeCtrl->Select(hDestChildItem,TVGN_CARET);
				}
//			}
		}
		else if (dwDstItemType == TET_MEDIA)
		{
			// add all items to the parent of the dest item

			HTREEITEM hDstParent = m_pDestTreeCtrl->GetParentItem(hDestTItem);

			HTREEITEM hInsertAfter = m_pDestTreeCtrl->GetPrevSiblingItem(hDestTItem);
			if (hInsertAfter == NULL)
			{
				hInsertAfter = TVI_FIRST;
			}

			HTREEITEM hDestChildItem = m_pDestTreeCtrl->AddItem(hDstParent, csItem, hInsertAfter, nImage, nSelImage, dwSrcItemData);

			HTREEITEM hSrcChildItem = m_pSourceTreeCtrl->GetChildItem(hSrcTItem);

			if(hSrcChildItem != NULL) 
			{
				AddItem(hSrcChildItem, hDestTItem,true);
			}

			if (bAddSiblings)
			{
				hSrcTItem = m_pSourceTreeCtrl->GetNextSiblingItem(hSrcTItem);
					
			}
			else
			{
				hSrcTItem = NULL;
			}

			if ((void*)m_pSourceTreeCtrl == (void*)m_pDestTreeCtrl)
			{
				m_pDestTreeCtrl->Select(hDestChildItem,TVGN_CARET);
			}
		}
	}
	
}
/********************************************************************
GetItemImages()
	Gets image indexes associated with a tree item
********************************************************************/
void CMediaToPlayListDropTarget::GetItemImages(HTREEITEM hSrcTItem, int &nSelItemImage, int &nNonSelItemImage)
{
	
	CImageList *pImageList = m_pSourceTreeCtrl->GetImageList(TVSIL_NORMAL);
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
		if ( m_pDestTreeCtrl->GetImageList(TVSIL_NORMAL) == NULL )
		{
			m_pDestTreeCtrl->SetImageList(pImageList, TVSIL_NORMAL);
		}
		// Get the image indexes
		m_pSourceTreeCtrl->GetItemImage(hSrcTItem, nSelItemImage, nNonSelItemImage);
	}
}
