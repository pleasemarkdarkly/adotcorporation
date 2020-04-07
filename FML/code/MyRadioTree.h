#if !defined(AFX_MYRADIOTREE_H__04FADF27_4922_4C8D_8A9D_8AAF11D249BD__INCLUDED_)
#define AFX_MYRADIOTREE_H__04FADF27_4922_4C8D_8A9D_8AAF11D249BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyRadioTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyRadioTree window

class CMyRadioTree : public CTreeCtrl
{
// Construction
public:
	CMyRadioTree();

// Attributes
public:

	CString PreEditText;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyRadioTree)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Populate();
	void AddStation(DWORD stationIndex, CObArray *pRadioList);
	virtual ~CMyRadioTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyRadioTree)
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void SyncWithDatabase(CObArray *pRadioList);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYRADIOTREE_H__04FADF27_4922_4C8D_8A9D_8AAF11D249BD__INCLUDED_)
