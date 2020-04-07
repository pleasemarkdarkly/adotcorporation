#if !defined(AFX_WAITINGDIALOG_H__45AE8E93_2C19_4BAB_B8AD_401B0383B70C__INCLUDED_)
#define AFX_WAITINGDIALOG_H__45AE8E93_2C19_4BAB_B8AD_401B0383B70C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaitingDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaitingDialog dialog

#define NUM_BITMAPS_IN_WAIT_ANIM 2




class CWaitingDialog : public CDialog
{
// Construction
public:
	CWaitingDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaitingDialog)
	enum { IDD = IDD_DIALOG2 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitingDialog)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
public:
	void SetParameters(int AnimationSeries, CString text);

// Implementation
protected:

	CBitmap bLeft, bRight, bBottom;

	CBitmap bAnimBitmaps[10];

	CStatic stLeft, stRight, stBottom, sCurrentBitmap;

	int currBitmap, currSeries;



	unsigned int m_Timer;

public:
	void AnimateFrame();
	void Done();
	CStatic m_stText;

protected:
	CFont FontNormal;

	int m_Series;
	CString m_Text;

	DWORD timeTick;
	bool m_bExit;

	int NumBitmaps;


protected:

	HBRUSH m_Brush;

	// Generated message map functions
	//{{AFX_MSG(CWaitingDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CWaitingDialog *  pTheWaitDialog;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAITINGDIALOG_H__45AE8E93_2C19_4BAB_B8AD_401B0383B70C__INCLUDED_)
