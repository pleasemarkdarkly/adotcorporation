#ifndef AFX_DIRTREECTRL_H__1E4F97A0_B41E_11D2_955E_204C4F4F5020__INCLUDED_
#define AFX_DIRTREECTRL_H__1E4F97A0_B41E_11D2_955E_204C4F4F5020__INCLUDED_

// DirTreeCtrl.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CDirTreeCtrl 

class CDirTreeCtrl : public CTreeCtrl
{
// Konstruktion
public:
	CDirTreeCtrl();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDirTreeCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementierung
public:
	void SetChildrenParented(HTREEITEM hItem);
	void SetChildrenUnchecked(HTREEITEM hItem);
	bool IsParentChecked(HTREEITEM hParent);
	void SetChildrenState(HTREEITEM hItem, int State);

	enum CheckState{ NOSTATE = 0, CHECKED, CHILD_CHECKED, 
		PARENT_CHECKED, UNCHECKED };
	enum CheckType{ CHECK, UNCHECK, TOGGLE, REFRESH };


	HTREEITEM GetPrevCheckedItem( HTREEITEM hItem );
	HTREEITEM GetNextCheckedItem( HTREEITEM hItem );
	HTREEITEM GetFirstCheckedItem();
	bool IsItemChecked(HTREEITEM hItem);
	bool SetCheck( HTREEITEM hItem, CheckType nCheck );
	BOOL SetSelPath( LPCTSTR strPath );
	CString GetFullPath( HTREEITEM hItem );
	LPCTSTR GetSubPath( LPCTSTR strPath );
	BOOL DisplayTree( LPCTSTR strRoot, BOOL bFiles = FALSE );
	virtual ~CDirTreeCtrl();

	// Generierte Nachrichtenzuordnungsfunktionen

	BOOL IsValidPath( LPCTSTR strPath );

	int SetCheckState(HTREEITEM hItem, BOOL fCheck);

	    HTREEITEM GetNextItem( HTREEITEM hItem, UINT nCode ){
                return CTreeCtrl::GetNextItem( hItem, nCode );
        }
        HTREEITEM GetNextItem( HTREEITEM hItem);

		HTREEITEM GetPrevItem( HTREEITEM hItem );

		HTREEITEM GetLastItem( HTREEITEM hItem );

	void GetCheckedDirectories( HTREEITEM hItem);

	bool HasChanged() {return m_hasChanged; };


protected:
	void ExpandItem( HTREEITEM hItem, UINT nCode );
	HTREEITEM SearchSiblingItem( HTREEITEM hItem, LPCTSTR strText );
	BOOL FindSubDir( LPCTSTR strPath );
	HTREEITEM AddItem( HTREEITEM hParent, LPCTSTR strPath );
	void DisplayPath( HTREEITEM hParent, LPCTSTR strPath );
	BOOL DisplayDrives();
	BOOL m_bFiles;
	CString m_strError;
	BOOL GetSysImgList();
	CImageList m_imgList, m_StateImageList;
	CString m_strRoot;

	bool m_hasChanged;
	//{{AFX_MSG(CDirTreeCtrl)
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_DIRTREECTRL_H__1E4F97A0_B41E_11D2_955E_204C4F4F5020__INCLUDED_
