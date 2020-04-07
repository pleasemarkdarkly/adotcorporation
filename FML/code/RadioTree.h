#if !defined(AFX_RADIOTREE_H__2BE29AE1_7810_4169_B3B5_4C1C8E2BABFA__INCLUDED_)
#define AFX_RADIOTREE_H__2BE29AE1_7810_4169_B3B5_4C1C8E2BABFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RadioTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRadioTree window

class CRadioTree : public CTreeCtrl
{
// Construction
public:
	CRadioTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioTree)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Populate(CObArray *pRadioList);
	virtual ~CRadioTree();



	// Generated message map functions
protected:
	//{{AFX_MSG(CRadioTree)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()


	CObArray *m_pRadioList;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOTREE_H__2BE29AE1_7810_4169_B3B5_4C1C8E2BABFA__INCLUDED_)
