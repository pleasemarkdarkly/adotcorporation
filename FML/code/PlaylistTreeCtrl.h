#if !defined(AFX_PLAYLISTTREECTRL_H__0B2FC2EF_47CA_43C8_A104_A9FB77A0E242__INCLUDED_)
#define AFX_PLAYLISTTREECTRL_H__0B2FC2EF_47CA_43C8_A104_A9FB77A0E242__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlaylistTreeCtrl.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CPlaylistTreeCtrl window
//class CPlaylistDragTarget;

//#include "MediaToPlaylistDropTarget.h"

class CMediaToPlayListDropTarget;
class CPlaylistTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CPlaylistTreeCtrl();

// Attributes
public:

// Operations
public:

	CString PreEditText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlaylistTreeCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool TestDuplicatePlaylistName(HTREEITEM curItem,CString name);
	virtual ~CPlaylistTreeCtrl();

	void Populate();


	bool CheckForItem(HTREEITEM hParent, long lParam);

	void SyncWithDatabase();

	HTREEITEM m_sourceItem;


	HTREEITEM AddItem(	HTREEITEM hParent, CString csItemName, 
						HTREEITEM hInsAfter, int iSelImage , int iNonSelImage, 
						long lParam);

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlaylistTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CMediaToPlayListDropTarget m_CTreeDropTarget;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYLISTTREECTRL_H__0B2FC2EF_47CA_43C8_A104_A9FB77A0E242__INCLUDED_)
