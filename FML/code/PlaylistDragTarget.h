// PlaylistDragTarget.h: interface for the CPlaylistDragTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYLISTDRAGTARGET_H__EABB3A7F_FD36_41AD_A0DE_30BA5E3C22E3__INCLUDED_)
#define AFX_PLAYLISTDRAGTARGET_H__EABB3A7F_FD36_41AD_A0DE_30BA5E3C22E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMediaDragCtrl;
class CPlaylistTreeCtrl;

class CPlaylistDragTarget : public COleDropTarget  
{
// Overrides
	public:
			virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, 
			DWORD dwKeyState, CPoint point );
			virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, 
			DWORD dwKeyState, CPoint point );
			virtual void OnDragLeave( CWnd* pWnd );
			virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
			DROPEFFECT dropEffect, CPoint point);
// Members
	public:
		//Static variable to store the window handle of source tree control
		static HWND m_shWndTreeCtrl;
	private:
		// Pointer to source tree control
		CPlaylistTreeCtrl *m_pSourceTreeCtrl;
		// Pointer to destination tree  control
		CPlaylistTreeCtrl *m_pDestTreeCtrl;
		// Recursive Function for adding items from source tree to destination tree
		void AddItem(HTREEITEM hSrcTItem,HTREEITEM hDestTItem );
		// Function to get the index of the item’s image and its 
		// selected image within the tree control’s image list
		void GetItemImages(HTREEITEM hSrcTItem, int &nSelItem, int &nNonSelItem);
		HTREEITEM m_hDestItem;

};

#endif // !defined(AFX_PLAYLISTDRAGTARGET_H__EABB3A7F_FD36_41AD_A0DE_30BA5E3C22E3__INCLUDED_)
