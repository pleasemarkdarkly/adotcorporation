// MediaToPlayListDropTarget.h: interface for the CMediaToPlayListDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEDIATOPLAYLISTDROPTARGET_H__02AFD557_3992_4FD7_B6DE_4C855EA0C43C__INCLUDED_)
#define AFX_MEDIATOPLAYLISTDROPTARGET_H__02AFD557_3992_4FD7_B6DE_4C855EA0C43C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMediaDragCtrl;
class CPlaylistTreeCtrl;

class CMediaToPlayListDropTarget : public COleDropTarget  
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

		bool g_fDragging;

		CImageList*	m_pDragImage;

		BOOL		m_bLDragging;
		HTREEITEM	m_hitemDrag,m_hitemDrop;


	private:
		// Pointer to source tree control
		CMediaDragCtrl *m_pSourceTreeCtrl;
		// Pointer to destination tree  control
		CPlaylistTreeCtrl *m_pDestTreeCtrl;

		// Recursive Function for adding items from source tree to destination tree
		void AddItem(HTREEITEM hSrcTItem,HTREEITEM hDestTItem,bool bAddSiblings = 0 );
		// Function to get the index of the item’s image and its 
		// selected image within the tree control’s image list
		void GetItemImages(HTREEITEM hSrcTItem, int &nSelItem, int &nNonSelItem);
		HTREEITEM m_hDestItem;
};

#endif // !defined(AFX_MEDIATOPLAYLISTDROPTARGET_H__02AFD557_3992_4FD7_B6DE_4C855EA0C43C__INCLUDED_)
