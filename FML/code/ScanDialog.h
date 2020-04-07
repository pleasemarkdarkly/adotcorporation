#if !defined(AFX_SCANDIALOG_H__A8AAD3D6_83E5_4F4E_82C0_93B2317BE4AC__INCLUDED_)
#define AFX_SCANDIALOG_H__A8AAD3D6_83E5_4F4E_82C0_93B2317BE4AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScanDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScanDialog dialog
#include "xSkinButton.h"
#include "resource.h"

#define NUM_BITMAPS_IN_ANIM 10
#define ANIM_INTERVAL 400

class CScanDialog : public CDialog
{
// Construction
public:
	void Done();
	CScanDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScanDialog)
	enum { IDD = IDD_SCANDIALOG };
	CButton	m_CancelButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

		CString currText;

	CBitmap bLeft, bRight, bBottom;

	CBitmap bAnimBitmaps[NUM_BITMAPS_IN_ANIM];

	CxSkinButton m_xCancel;

	CStatic stLeft, stRight, stBottom, sCurrentBitmap;

	int currBitmap;

	unsigned int m_Timer;

public:

	CStatic m_Adding;

	void ShowUpdate(const char * currentlyAdding);
protected:
	CFont FontNormal;


protected:

	HBRUSH m_Brush;

	// Generated message map functions
	//{{AFX_MSG(CScanDialog)
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANDIALOG_H__A8AAD3D6_83E5_4F4E_82C0_93B2317BE4AC__INCLUDED_)
