// RadioTree.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"
#include "RadioTree.h"
#include "medialibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioTree

CRadioTree::CRadioTree()
{
}

CRadioTree::~CRadioTree()
{
}


BEGIN_MESSAGE_MAP(CRadioTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CRadioTree)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioTree message handlers

BOOL CRadioTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD |WS_TABSTOP | WS_VISIBLE |TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	//cs.style = WS_CHILD|WS_VISIBLE
	
	return CTreeCtrl::PreCreateWindow(cs);
}

void CRadioTree::Populate(CObArray *pRadioList)
{
	m_pRadioList = pRadioList;
	DeleteAllItems();

	DWORD totalItems = pRadioList->GetSize();

	HTREEITEM hGenre, hStation;

	CProvidedRadioStation *pStation;

	CString previousGenre = ""; bool newGenre;

	int previousGenreIndex = -1;

	for (DWORD stationIndex=0; stationIndex < totalItems; stationIndex++)
	{
		pStation = (CProvidedRadioStation *)pRadioList->GetAt(stationIndex);

		newGenre = (previousGenre != pStation->m_genre);

		if(newGenre)
		{
			hGenre = InsertItem(pStation->m_genre,0,0);
			SetItemData(hGenre,0xFFEEEEEE);
			previousGenre = pStation->m_genre;
		}

		hStation = InsertItem(pStation->m_name,1,1,hGenre,NULL);
		SetItemData(hStation,stationIndex);
	}



}

BOOL CRadioTree::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void CRadioTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (HitTest(point) == GetSelectedItem())
	{
		HTREEITEM sel = GetSelectedItem();
	
		if (sel != NULL)
		{
			DWORD data = GetItemData(sel);
	
			if (data != 0xFFEEEEEE )
			{
				CProvidedRadioStation *pStation;
				pStation = (CProvidedRadioStation *)m_pRadioList->GetAt(data);
	
				if (pStation != NULL)
				{
					CString url = pStation->m_url;
					url += "listen.pls";
					ShellExecute(HWND(this),"open",url,NULL,NULL,SW_NORMAL);
				}
			}
		}
	}
	
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}
