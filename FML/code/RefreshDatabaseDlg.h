#if !defined(AFX_REFRESHDATABASEDLG_H__9F03741A_4A21_46AA_9420_F2D28BFB30F0__INCLUDED_)
#define AFX_REFRESHDATABASEDLG_H__9F03741A_4A21_46AA_9420_F2D28BFB30F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RefreshDatabaseDlg.h : header file
//

#include "afxhtml.h"


class CAnimatedHtmlView : public CHtmlView
{
public:
	CAnimatedHtmlView()
	{ 
		CHtmlView::CHtmlView();
	};
};


/////////////////////////////////////////////////////////////////////////////
// RefreshDatabaseDlg dialog

class RefreshDatabaseDlg : public CDialog
{
// Construction
public:
	RefreshDatabaseDlg(CWnd* pParent = NULL);   // standard constructor
	void RefreshStoppedNotify();
	void RefreshBuildNotify();
	CString StopText;
	bool ReadyToDraw;

// Dialog Data
	//{{AFX_DATA(RefreshDatabaseDlg)
	enum { IDD = IDD_DIALOG1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RefreshDatabaseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	CBitmap m_bA,mbB,mbC;
	CAnimatedHtmlView * pAnimView; 

	HBRUSH m_Brush;
	// Generated message map functions
	//{{AFX_MSG(RefreshDatabaseDlg)
	afx_msg void OnButtonStop();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

/*
	CEdit	m_Genres;
	CEdit	m_PathName;
	CEdit	m_Genre;
	CButton	m_FrameText;
	CButton	m_Stop;
	CButton	m_IDOK;
	CEdit	m_SongTitle;
	CEdit	m_TitleNum;
	CEdit	m_ArtistNum;
	CEdit	m_ArtistName;
	CEdit	m_AlbumNum;
	CEdit	m_AlbumName;
*/

#endif // !defined(AFX_REFRESHDATABASEDLG_H__9F03741A_4A21_46AA_9420_F2D28BFB30F0__INCLUDED_)
