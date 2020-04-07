#if !defined(AFX_ONFIGDIRECTORIES_H__301936FD_3D65_4C1C_82B0_2C3EC803033D__INCLUDED_)
#define AFX_ONFIGDIRECTORIES_H__301936FD_3D65_4C1C_82B0_2C3EC803033D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// onfigDirectories.h : header file
//

#include "dirtreectrl.h"
#include "xSkinButton.h"
/////////////////////////////////////////////////////////////////////////////
// ConfigDirectories dialog



class ConfigDirectories : public CDialog
{
// Construction
public:
	ConfigDirectories(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ConfigDirectories)
	enum { IDD = IDD_CONFIG_DIR };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConfigDirectories)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDirTreeCtrl  m_DirTree;

	CFont FontNormal, FontBold;

	CEdit NetworkNameEdit;

	CStatic stNetworkName;

	CBitmap bTopNav, bFinder, bInfo;

	CStatic stTopNav, stFinder, stInfo;

	CStatic stInstructions, stInfoText;

public:
	void OnSave();
	void SyncWithDatabase();
	void UpdateInfoText();
	CxSkinButton m_xSave;
protected:

	CxSkinButton m_xInfo, m_xScan;

	bool m_InitDone;


	CStatic m_WhiteFrame, m_DirTreeFrame;
	HBRUSH m_Brush, m_WhiteBrush;
	bool m_bLDragging;

	CString m_StrPath;
	// Generated message map functions
	//{{AFX_MSG(ConfigDirectories)
	virtual BOOL OnInitDialog();
	afx_msg void OnRescan();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnChangeEdit();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ONFIGDIRECTORIES_H__301936FD_3D65_4C1C_82B0_2C3EC803033D__INCLUDED_)
