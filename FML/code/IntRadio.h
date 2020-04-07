#if !defined(AFX_INTRADIO_H__27E250E3_92E4_468B_BBDA_DFFCE4446127__INCLUDED_)
#define AFX_INTRADIO_H__27E250E3_92E4_468B_BBDA_DFFCE4446127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IntRadio.h : header file

//

#include "medialibrary.h"
#include "xSkinButton.h"

#include "RadioTree.h"
#include "MyRadioTree.h"



/////////////////////////////////////////////////////////////////////////////
// IntRadio dialog

class IntRadio : public CDialog
{
// Construction
public:
	IntRadio(CWnd* pParent = NULL);   // standard constructor

	CRadioStation *currentSelection;

	CObArray providedStations;

	HCURSOR hArrowCursor;
	HCURSOR hNoCursor;

	CRadioTree m_RadioTree;
	CMyRadioTree m_MyRadioTree;

// Dialog Data
	//{{AFX_DATA(IntRadio)
	enum { IDD = IDD_INT_RADIO };
	CButton	m_ConnectButton;
	CListCtrl	m_List;
	CListBox	m_StationList;
	CStatic	m_Status;
	CComboBox	m_ComboSpeed;
	CComboBox	m_GenreCombo;
	CEdit	m_StationURL;
	CEdit	m_StationName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(IntRadio)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:


	bool g_fDragging;

	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;

	HBRUSH m_Brush;


	CBitmap bAvailable;
	CBitmap bMyStations, bTopNav;

	CStatic stAvailable;
	CStatic stMyStations, stTopNav;

	CStatic stInstructions;

	CFont FontNormal;

	CxSkinButton m_xAdd;
	CxSkinButton m_xEdit;
	CxSkinButton m_xDelete;
public:
	CxSkinButton m_xSave;

	void SyncWithDatabase();

protected:

	// Generated message map functions
	//{{AFX_MSG(IntRadio)
	afx_msg void OnButtonAddNew();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnConnect();
	afx_msg void OnSetfocusStationName();
	afx_msg void OnDblclkTreeListctrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTreeListctrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTRADIO_H__27E250E3_92E4_468B_BBDA_DFFCE4446127__INCLUDED_)
