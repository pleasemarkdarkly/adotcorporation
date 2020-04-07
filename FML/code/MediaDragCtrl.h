#if !defined(AFX_MEDIADRAGCTRL_H__1AB1840C_A42A_448C_B3B5_349D156DFA15__INCLUDED_)
#define AFX_MEDIADRAGCTRL_H__1AB1840C_A42A_448C_B3B5_349D156DFA15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MediaDragCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMediaDragCtrl window
#include "MediaToPlayListDropTarget.h"

class CMediaDragCtrl : public CTreeCtrl
{
// Construction
public:
	CMediaDragCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMediaDragCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMediaDragCtrl();

	void Populate();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMediaDragCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CMediaToPlayListDropTarget m_CTreeDropTarget;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEDIADRAGCTRL_H__1AB1840C_A42A_448C_B3B5_349D156DFA15__INCLUDED_)
