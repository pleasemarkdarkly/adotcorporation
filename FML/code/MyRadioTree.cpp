// MyRadioTree.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"
#include "MyRadioTree.h"

#include "afxinet.h"

#include "medialibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyRadioTree

CMyRadioTree::CMyRadioTree()
{
}

CMyRadioTree::~CMyRadioTree()
{
}


BEGIN_MESSAGE_MAP(CMyRadioTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyRadioTree)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyRadioTree message handlers

BOOL CMyRadioTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD | WS_TABSTOP | WS_VISIBLE |TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_EDITLABELS;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	
	return CTreeCtrl::PreCreateWindow(cs);
}

void CMyRadioTree::AddStation(DWORD stationIndex, CObArray *pRadioList)
{

	CProvidedRadioStation *pStation;

	if (stationIndex < (DWORD)pRadioList->GetSize())
	{
		pStation = (CProvidedRadioStation *)pRadioList->GetAt(stationIndex);

		// check we don't already have this url

		bool duplicate = false;

		HTREEITEM hItem = GetRootItem();

		while ((hItem != NULL) && (!duplicate))
		{
			HTREEITEM hURL = GetChildItem(hItem);
			CString   itemURL = GetItemText(hURL);
			duplicate = (itemURL.CompareNoCase(pStation->m_url)==0);

			hItem = GetNextSiblingItem(hItem);
		}

		if (!duplicate)
		{

			HTREEITEM hStation = InsertItem(pStation->m_name,1,1);
			HTREEITEM hURL     = InsertItem(pStation->m_url,2,2,hStation);
		
			SetItemData(hStation,stationIndex);
			SetItemData(hURL,0XF0000000+stationIndex);
		}
	}
}

void CMyRadioTree::Populate()
{
	DeleteAllItems();

	DWORD numStations = theMediaDatabase.radioManager.GetSize();

	DWORD currIndex = 0;

	CRadioStation * pRadioStation;

	HTREEITEM radioText, radioURL;

	for (currIndex = 0; currIndex < numStations; currIndex ++)
	{
		pRadioStation = theMediaDatabase.radioManager.GetStationFromKey(currIndex);

		radioText = InsertItem(pRadioStation->GetStationTitle(),1,1);
		radioURL = InsertItem(pRadioStation->GetStationURL(),2,2,radioText);

		SetItemData(radioText,currIndex);
		SetItemData(radioURL,currIndex+0xF0000000);
	}

}

void CMyRadioTree::SyncWithDatabase(CObArray *pRadioList)
{
	HTREEITEM item = GetRootItem();

	theMediaDatabase.radioManager.ClearAll();

	while (item != NULL)
	{
		HTREEITEM urlItem = GetChildItem(item);

		CString name, url;

		name = GetItemText(item);

		name.Replace('<','[');
		name.Replace('>',']');

		url = GetItemText(urlItem);

		url.Replace('<','[');
		url.Replace('>',']');

		theMediaDatabase.radioManager.AddStationToDatabase(new CRadioStation(name,url,"x-ms-mp3"));

		item = GetNextSiblingItem(item);
	}
}

void CMyRadioTree::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	HTREEITEM curItem = pTVDispInfo->item.hItem;
	DWORD data = GetItemData(curItem);

	if ((data & 0x0F000000) == 0)
	{
		char szPreText[100];

		GetEditControl()->GetWindowText(szPreText,sizeof(szPreText));

		PreEditText = szPreText;
	}

	GetEditControl()->LimitText(80);

	*pResult = 0;

}

bool ValidURL(CString &url)
{
	bool ret = true;

	if (url.Find("http:\\\\")==0)
	{
		url.Replace("http:\\\\","http://");
		ret = false;
	}

	if (url.Find("http://") != 0)
	{
		url = CString("http://") + url;
		ret = false;
	}

	url.Replace('<','[');
	url.Replace('>',']');
	
	return ret;
}
		

void CMyRadioTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	HTREEITEM curItem = pTVDispInfo->item.hItem;
	SelectItem(curItem);

	char szNewLabel[80];
	
	GetEditControl()->GetWindowText(szNewLabel,sizeof(szNewLabel));
	CString csNewLabel = szNewLabel;

	bool modified = false;

	if (csNewLabel.Find('<',0) != -1)
	{
		csNewLabel.Replace('<','[');
		modified = true;
	}

	if (csNewLabel.Find('>',0) != -1)
	{

		csNewLabel.Replace('>',']');
		modified = true;
	}

	while (csNewLabel.Find(' ',0) == 0)
	{
		modified = true;
		csNewLabel = csNewLabel.Right(csNewLabel.GetLength()-1);
	}

	GetEditControl()->SetWindowText(csNewLabel);
	DWORD data = GetItemData(curItem);

	if (data & 0xF0000000)
	{
		if (data & 0x0F000000)
		{
			SetItemText(curItem,PreEditText);
			SetItemData(curItem,data & 0xF0FFFFFF);
			Invalidate(TRUE);
			PostMessage(TVM_ENDEDITLABELNOW ,0,(LPARAM)(HTREEITEM)curItem);
		}
		else 
		{
			int len = csNewLabel.GetLength();
		
			if (!ValidURL(csNewLabel) || (modified))

			{	
				SetItemData(curItem,data | 0x0F000000);
				PreEditText = csNewLabel;
				PostMessage(TVM_ENDEDITLABELNOW ,0,(LPARAM)(HTREEITEM)curItem);
				Invalidate(TRUE);
				PostMessage(TVM_EDITLABEL,0,(LPARAM)(HTREEITEM)curItem);
			}
			else
			{
				PostMessage(TVM_ENDEDITLABELNOW ,0,(LPARAM)(HTREEITEM)curItem);
			}

		}
	}
	else
	{
		if (data & 0x0F000000)
		{
			SetItemText(curItem,PreEditText);
			SetItemData(curItem,data & 0xF0FFFFFF);
			Invalidate(TRUE);
			PostMessage(TVM_ENDEDITLABELNOW ,0,(LPARAM)(HTREEITEM)curItem);
		}
		else
		{

			int len = csNewLabel.GetLength();
	
			if ((modified) && (len != 0))
			{
				PreEditText = csNewLabel;
			}
	
			if ((len == 0) || (modified))
			{
		
				SetItemData(curItem,data + 0x0F000000);
				Invalidate(TRUE);
				PostMessage(TVM_ENDEDITLABELNOW ,0,(LPARAM)(HTREEITEM)curItem);
				//SetItemText(curItem,"please enter a valid name");
				PostMessage(TVM_EDITLABEL,0,(LPARAM)(HTREEITEM)curItem);
			}
		}
	}

	if (data & 0x00F00000)
	{
		HTREEITEM childItem = GetChildItem(curItem);
		if (childItem)
		{
			PostMessage(TVM_EDITLABEL,0,(LPARAM)(HTREEITEM)childItem);
		}

		SetItemData(curItem,data&0xFF0FFFFF);
	}


	GetEditControl()->Invalidate(TRUE);
	Invalidate(TRUE);

	pMainDlg->EnableSync(true);

	*pResult = TRUE;
}


BOOL CMyRadioTree::PreTranslateMessage(MSG* pMsg) 
{
     if( pMsg->message == WM_KEYDOWN )
        {
                // When an item is being edited make sure the edit control
                // receives certain important key strokes
                if( GetEditControl() 
                                && (pMsg->wParam == VK_RETURN 
                                        || pMsg->wParam == VK_DELETE 
                                        || pMsg->wParam == VK_ESCAPE
                                        || GetKeyState( VK_CONTROL) 
                                        )
                        )
                {
                        ::TranslateMessage(pMsg);
                        ::DispatchMessage(pMsg);
                        return TRUE;                            // DO NOT process further
                }

			// handle delete key

			HTREEITEM delItem;
			if ((pMsg->wParam == VK_DELETE) && (delItem = GetSelectedItem()))
			{
				if (ItemHasChildren(delItem))
				{
					DeleteItem(delItem);
			
					pMainDlg->EnableSync(true);
				}
				
				::TranslateMessage(pMsg);
                ::DispatchMessage(pMsg);
				return TRUE;
			}

			// handle f2 key to start editing

			HTREEITEM EditItem;
			if ((pMsg->wParam == VK_F2) && (EditItem = GetSelectedItem()))
			{
				PostMessage(TVM_EDITLABEL,0,(LPARAM)(HTREEITEM)EditItem);
				
				::TranslateMessage(pMsg);
                ::DispatchMessage(pMsg);
				
				return TRUE;
			}
        }

	return CTreeCtrl::PreTranslateMessage(pMsg);
}


void CMyRadioTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (HitTest(point) == GetSelectedItem())
	{
		HTREEITEM sel = GetSelectedItem();
		if (sel != NULL)
		{
			if (ItemHasChildren(sel))
			{
				sel = GetChildItem(sel);
			}
	
			CString url = GetItemText(sel);
			url += "listen.pls";
	
			ShellExecute(HWND(this),"open",url,NULL,NULL,SW_NORMAL);
		}
	}

	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}
