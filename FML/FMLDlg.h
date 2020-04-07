// FMLDlg.h : header file
//

#if !defined(AFX_FMLDLG_H__2A802DC1_E19C_4822_9278_B25C5D852558__INCLUDED_)
#define AFX_FMLDLG_H__2A802DC1_E19C_4822_9278_B25C5D852558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFMLDlg dialog


#include "traydialog.h"
#include "xSkinButton.h"
#include "intradio.h"
#include "playlistdlg.h"
#include "configdirectories.h"


class CFMLDlg : public CTrayDialog
{
// Construction
public:
	CFMLDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPhase1_guiDlg)
	enum { IDD = IDD_FML_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhase1_guiDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HBRUSH m_Brush;

	// the fancy buttons
	CxSkinButton m_xConfigure;
	CxSkinButton m_xPlaylists;
	CxSkinButton m_xInternet;
	CxSkinButton m_xFullplay;

	// the bitmaps

	CStatic m_stTitle,m_stLN,m_stA,m_stB,m_stC,m_stD,m_stE,m_stF,m_stStart;
	CBitmap t,a,b,c,d,e,f,ln,start;

	unsigned int m_Timer, m_IdleElapsed;

	bool m_SyncNeeded;


	CWnd * m_pCurrentScreen;
public:

	void OnSync() ;
	// the dialog windows that appear in the client area when buttons are pressed
	CPlayListDlg	m_playlistDlg;
	IntRadio		m_radioDlg;
	
	ConfigDirectories m_configDlg;

	int m_CurrentViewIndex;

	void EnableAllButtons();
	void DisableAllButtons();

	void EnableSync(bool Enable);

	int Init();
	void UpdateScreenSelection(int iSelection);

protected:

	void OnStPlaylists();
	void OnStInternet();
	void OnStConfig();
	void OnStFullplay();


	// Generated message map functions
	//{{AFX_MSG(CPhase1_guiDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnHide();
	afx_msg void OnImlmenuOpen();
	afx_msg void OnMenuExit();
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnCancelMode();
	virtual void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CFMLDlg * pMainDlg;



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FMLDLG_H__2A802DC1_E19C_4822_9278_B25C5D852558__INCLUDED_)
