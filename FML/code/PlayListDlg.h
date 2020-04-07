#if !defined(AFX_PLAYLISTDLG_H__EE700E12_AADB_4728_9FD4_0E156F93F6C0__INCLUDED_)
#define AFX_PLAYLISTDLG_H__EE700E12_AADB_4728_9FD4_0E156F93F6C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlayListDlg.h : header file
//

#include "MediaDragCtrl.h"
#include "PlaylistTreeCtrl.h"
#include "xSkinButton.h"

/////////////////////////////////////////////////////////////////////////////
// CPlayListDlg dialog

class CPlayListDlg : public CDialog
{
// Construction
public:
	CPlayListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPlayListDlg)
	enum { IDD = IDD_NEW_PLAYLIST };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayListDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	CPlaylistTreeCtrl	m_playlistTree;
	CMediaDragCtrl	    m_mediaTree;


// Implementation
	public:
		void PopulateTrees();
//		CMediaDragCtrl m_mediaTree;
//	CPlaylistTreeCtrl  m_playlistTree;
protected:

	CBitmap bLabelAvailable;
	CBitmap bLabelPlaylists, bTopNav;

	CStatic stAvailable;
	CStatic stPlaylists, stTopNav;

	CStatic stInstructions;

	HBRUSH m_Brush;

	CFont FontNormal;


	bool g_fDragging;

	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;

	HTREEITEM hTLastSelectItem;

		HCURSOR hArrowCursor;
	HCURSOR hNoCursor;
	HCURSOR hDropCursor;



	CxSkinButton m_xAdd;
	CxSkinButton m_xEdit;
	CxSkinButton m_xDelete;
public:
	void SyncWithDatabase();
	CxSkinButton m_xSave;
protected:


	void DragMouseOnPlaylists(CPoint point);
	void AddItem(HTREEITEM hSrcTItem,HTREEITEM hDestTItem, bool bAddSiblings = 0);
	void GetItemImages(HTREEITEM hSrcTItem, int &nSelItemImage, int &nNonSelItemImage);


	//void PopulateMediaTree(CDragDropTreeCtrl &pTree);


	// Generated message map functions
	//{{AFX_MSG(CPlayListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNewplaylist();
	afx_msg void OnEditplaylist();
	afx_msg void OnDelete();
	afx_msg void OnSave();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBegindragTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYLISTDLG_H__EE700E12_AADB_4728_9FD4_0E156F93F6C0__INCLUDED_)
