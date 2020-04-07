// IntRadio.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"
#include "IntRadio.h"
#include "medialibrary.h"

#include "afxinet.h"
#include "medialibrary.h"
#include "xmlif.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IDC_BUTTON_IR_SAVE	6000
#define IDC_BUTTON_IR_EDIT	6001
#define IDC_BUTTON_IR_DELETE 6002
#define IDC_BUTTON_IR_ADD	6003

#define IDC_RADIOTREE 6005
#define IDC_MYRADIOTREE 6006


DWORD shoutcastThreadID, shoutcastDirectoryThreadID;
IntRadio *gpDlg = NULL;

CString gRequestedURL;

HANDLE g_shoutcastThread, g_shoutcastDirectoryThread;

HANDLE g_shoutcastThreadCompletedEvent;

DWORD WINAPI ShoutcastThread(void *param);

void CreateShoutcastWorkerThread(IntRadio *pDlg)
{
	DWORD waitCause = WaitForSingleObject(g_shoutcastThreadCompletedEvent,0);

	if (waitCause == WAIT_OBJECT_0)
	{
	
		gpDlg = pDlg;
		g_shoutcastThread = CreateThread(0, 0, 
									(LPTHREAD_START_ROUTINE) ShoutcastThread,
                                    (LPVOID)0,
                                    0,
                                    &shoutcastThreadID);
	}
}


/////////////////////////////////////////////////////////////////////////////
// IntRadio dialog


IntRadio::IntRadio(CWnd* pParent /*=NULL*/)
	: CDialog(IntRadio::IDD, pParent)
{
	//{{AFX_DATA_INIT(IntRadio)
	//}}AFX_DATA_INIT

	g_shoutcastThreadCompletedEvent = CreateEvent(NULL,FALSE,TRUE,"shoutcastCompleted");


}


void IntRadio::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(IntRadio)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(IntRadio, CDialog)
	//{{AFX_MSG_MAP(IntRadio)
	ON_BN_CLICKED(IDC_BUTTON_IR_ADD, OnButtonAddNew)
	ON_BN_CLICKED(IDC_BUTTON_IR_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_IR_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_IR_EDIT, OnButtonEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_EN_SETFOCUS(IDC_STATION_NAME, OnSetfocusStationName)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_LISTCTRL, OnDblclkTreeListctrl)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE_LISTCTRL, OnSetfocusTreeListctrl)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_RADIOTREE, OnBegindragTree1)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE1, OnBegindragTree1)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IntRadio message handlers


void IntRadio::OnButtonAddNew() 
{

	HTREEITEM newItem = m_MyRadioTree.InsertItem("New station",1,1);

	m_MyRadioTree.SetItemData(newItem,0x00F00000);

	HTREEITEM newURL  = m_MyRadioTree.InsertItem("http://www.fullplaymedia.com",2,2,newItem);

	m_MyRadioTree.SetItemData(newURL,0xF0000000);

	m_MyRadioTree.Expand(newItem,TVE_EXPAND );

	m_MyRadioTree.PostMessage(TVM_EDITLABEL,0,(LPARAM)(HTREEITEM)newItem);

}

void IntRadio::OnButtonDelete() 
{
	HTREEITEM selectedItem = m_MyRadioTree.GetSelectedItem();
	if (selectedItem)
	{
		if (m_MyRadioTree.ItemHasChildren(selectedItem))
		{

			m_MyRadioTree.DeleteItem(selectedItem);
		}
	}

	pMainDlg->EnableSync(true);
}




void IntRadio::OnButtonSave() 
{
	pMainDlg->OnSync();
	
}

BOOL IntRadio::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Brush = CreateSolidBrush(RGB(0xe2,0xeb,0xf5));

	m_bLDragging = false;

	hArrowCursor = LoadCursor(NULL,IDC_ARROW);
	hNoCursor = LoadCursor(NULL,IDC_NO);

	FontNormal.CreateFont(-8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");

	SetFont(&FontNormal);

	bAvailable.LoadBitmap(IDB_STATIONS_AVAILABLE);
	stAvailable.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,80,290,80+24),this);
	stAvailable.SetBitmap(HBITMAP(bAvailable));

	bMyStations.LoadBitmap(IDB_MYSTATIONS);
	stMyStations.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(298,80,298+290,80+24),this);
	stMyStations.SetBitmap(HBITMAP(bMyStations));

	bTopNav.LoadBitmap(IDB_TOPNAV2);
	stTopNav.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,0,588,40),this);
	stTopNav.SetBitmap(HBITMAP(bTopNav));


	stInstructions.Create(_T("Drag items from radio stations available to my radio stations.  Double click any station to preview."),WS_CHILD|WS_VISIBLE|SS_LEFT,CRect(5,50,583,70),this);
	stInstructions.SetFont(&FontNormal);

	m_xAdd.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON   ,CRect(229,5,229+72,35), this, IDC_BUTTON_IR_ADD);
	m_xEdit.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,  CRect(413,5,413+72,35), this, IDC_BUTTON_IR_EDIT);
	m_xDelete.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(321,5,321+72,35), this, IDC_BUTTON_IR_DELETE);
	m_xSave.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON  ,CRect(505,5,505+72,35), this, IDC_BUTTON_IR_SAVE);


	//up/down/over/disabled

	m_xAdd.SetTextColor(RGB(0,0,0));
	m_xAdd.SetToolTipText("");
	m_xAdd.SetSkin(IDB_B_RADIOADD_UP,IDB_B_RADIOADD_DOWN,IDB_B_RADIOADD_OVER,IDB_B_RADIOADD_DISABLED,0,0,0,0,0);

	m_xEdit.SetTextColor(RGB(0,0,0));
	m_xEdit.SetToolTipText("");
	m_xEdit.SetSkin(IDB_B_RADIOEDIT_UP,IDB_B_RADIOEDIT_DOWN,IDB_B_RADIOEDIT_OVER,IDB_B_RADIOEDIT_DISABLED,0,0,0,0,0);

	m_xDelete.SetTextColor(RGB(0,0,0));
	m_xDelete.SetToolTipText("");
	m_xDelete.SetSkin(IDB_B_RADIODELETE_UP,IDB_B_RADIODELETE_DOWN,IDB_B_RADIODELETE_OVER,IDB_B_RADIODELETE_DISABLED,0,0,0,0,0);

	m_xSave.SetTextColor(RGB(0,0,0));
	m_xSave.SetToolTipText("");
	m_xSave.SetSkin(IDB_B_SAVE_UP,IDB_B_SAVE_DOWN,IDB_B_SAVE_OVER,IDB_B_SAVE_DISABLED,0,0,0,0,0);

	m_RadioTree.Create(WS_CHILD|WS_VISIBLE|WS_EX_CLIENTEDGE|TVS_HASLINES|TVS_LINESATROOT
								|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_SINGLEEXPAND, 
								CRect(0,104,290,507),this, IDC_RADIOTREE);
	
	CImageList * pTreeImageList = new(CImageList);
	pTreeImageList->Create(16,16,ILC_COLOR8 + ILC_MASK,8,16);

	pTreeImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	pTreeImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON5));
	pTreeImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON6));
	pTreeImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON7));

	m_MyRadioTree.Create(WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|WS_EX_CLIENTEDGE
								|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_EDITLABELS|TVS_SINGLEEXPAND , CRect(298,104,588,507),this, IDC_MYRADIOTREE);
	
	m_RadioTree.SetImageList(pTreeImageList,TVSIL_NORMAL);
	m_MyRadioTree.SetImageList(pTreeImageList,TVSIL_NORMAL);
	m_MyRadioTree.Populate();

	g_fDragging = false;
	CreateShoutcastWorkerThread(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




DWORD WINAPI ShoutcastThread(void *param)
{

	// only the cancel button of the dialog is enabled at this point

	CString buf;
	CString line;
	CStdioFile *shoutcast;

	gpDlg->providedStations.RemoveAll();


	gpDlg->m_RadioTree.DeleteAllItems();

	HTREEITEM root;

	root = gpDlg->m_RadioTree.InsertItem("Getting station list...",3,3);
	gpDlg->m_RadioTree.SetItemData(root,0xFFEEEEEE);

	CInternetSession session("iML/1.0");
	try
	{
		shoutcast = session.OpenURL("http://dj.fullplaymedia.com/system/djradio.asp",1,
			INTERNET_FLAG_DONT_CACHE |INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_RELOAD,NULL,0);
	}
	catch (CException *e)
	{
		e->Delete();
		session.Close();

		gpDlg->m_RadioTree.DeleteAllItems();
		HTREEITEM error;
		error = gpDlg->m_RadioTree.InsertItem("Error connecting. Press F5 to retry.",3,3);
		gpDlg->m_RadioTree.SetItemData(error,0xFFEEEEEE);

		SetEvent(g_shoutcastThreadCompletedEvent);
		return 0;
	}

	if (!shoutcast)
	{
		gpDlg->m_RadioTree.DeleteAllItems();
		HTREEITEM error;
		error = gpDlg->m_RadioTree.InsertItem("Error connecting. Press F5 to retry.",3,3);
		gpDlg->m_RadioTree.SetItemData(error,0xFFEEEEEE);

		SetEvent(g_shoutcastThreadCompletedEvent);
		return 0;
	}

	while (shoutcast->ReadString(line))	
	{
		buf += line;
	}

	shoutcast->Close();
	session.Close();

	HELEMENT docRoot;

	DWORD parse = XMLParse(buf,&docRoot);
	if (parse)
	{
		SetEvent(g_shoutcastThreadCompletedEvent);
		return 0;
	}


	// now parse the XML we got back

	HELEMENT currGenre;

	gpDlg->providedStations.RemoveAll();
	gpDlg->m_RadioTree.DeleteAllItems();

	currGenre = XMLGetFirstChild(docRoot);

	while (currGenre != NULL)
	{
		CString genre, name, url;

		genre = ""; name = ""; url = "";
		HELEMENT info;
		info = XMLGetFirstChild(currGenre);

		while (info != NULL)
		{
			CString elementName = XMLGetElementName(info);
			if (elementName == "name")
			{
				genre = XMLGetElementValue(info);
			}
			else if (elementName == "station")
			{
				HELEMENT stationinfo = XMLGetFirstChild(info);

				while (stationinfo != NULL)
				{
					CString ename = XMLGetElementName(stationinfo);
					if (ename == "stationName")
					{
						name = XMLGetElementValue(stationinfo);
					}
					else if (ename == "stationURL")
					{
						url = XMLGetElementValue(stationinfo);
					}
					stationinfo = XMLGetNextSibling(stationinfo);
				}
			}

			if (genre.GetLength() && name.GetLength() && url.GetLength())
			{
				gpDlg->providedStations.Add(new CProvidedRadioStation(genre,name,url));
			}

			info = XMLGetNextSibling(info);
		}

		currGenre = XMLGetNextSibling(currGenre);

	}

	XMLFree(docRoot);

	gpDlg->m_RadioTree.Populate(&gpDlg->providedStations);

	SetEvent(g_shoutcastThreadCompletedEvent);
	return 0;
}



/*
void DlgImportShoutcast::OnButtonAdd() 
{

	// Get the indexes of all the selected items.
	int nCount = m_List.GetSelCount();
	CArray<int,int> selItemsArray;

	selItemsArray.SetSize(nCount);
	m_List.GetSelItems(nCount, selItemsArray.GetData());
	
	CString itemName;
	CString itemURL;
	CShoutcastStation *temp;

	// now add the entries to our database
	for (int i = 0 ; i < nCount ; i ++)
	{
		DWORD nIndex = selItemsArray.GetAt(i);
		temp = (CShoutcastStation*)stationArray->GetAt(nIndex);
		itemName  = temp->description;
		itemURL   = "http://";
		itemURL  += temp->playurl;
		itemURL  += "/";

		theMediaDatabase.radioManager.AddStationToDatabase(new CRadioStation(itemName,itemURL));
	}

	m_Status.SetWindowText("Added stations to database.");


	
} */






void IntRadio::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (m_bLDragging)
	{
		POINT pt = point;
		ClientToScreen( &pt );
		CImageList::DragMove(pt);

		RECT listWindowRect;

		m_MyRadioTree.GetWindowRect(&listWindowRect);

		if ((pt.x > listWindowRect.left) &&
			(pt.x < listWindowRect.right) &&
			(pt.y < listWindowRect.bottom) &&
			(pt.y > listWindowRect.top))
		{
			SetCursor(hArrowCursor);

		}
		else
		{
			SetCursor(hNoCursor);
		}

	}
	
	CDialog::OnMouseMove(nFlags, point);

}


void IntRadio::OnLButtonUp(UINT nFlags, CPoint point) 
{
	bool change = false;

	POINT pt = point;
	ClientToScreen( &pt );

	CDialog::OnLButtonUp(nFlags, point);

	if (m_bLDragging)
	{
		m_bLDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		delete m_pDragImage;


		RECT listWindowRect;

		m_MyRadioTree.GetWindowRect(&listWindowRect);

		if ((pt.x > listWindowRect.left) &&
			(pt.x < listWindowRect.right) &&
			(pt.y < listWindowRect.bottom) &&
			(pt.y > listWindowRect.top))
		{

			HTREEITEM hChild = m_RadioTree.GetChildItem(m_hitemDrag);

			while (hChild)
			{
				m_MyRadioTree.AddStation(m_RadioTree.GetItemData(hChild),&providedStations);
				change = true;
				hChild = m_RadioTree.GetNextSiblingItem(hChild);
			}

			DWORD itemData = m_RadioTree.GetItemData(m_hitemDrag);

			if (itemData != 0xFFEEEEEE)
			{
				m_MyRadioTree.AddStation(itemData,&providedStations);
				change = true;
			}

		}
	}

	if (change)
		pMainDlg->EnableSync(true);

}



void IntRadio::OnBegindragTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	m_hitemDrag = pNMTreeView->itemNew.hItem;

	DWORD data = m_RadioTree.GetItemData(m_hitemDrag);

	if (data != 0xFFFFFFFF)
	{
		m_RadioTree.SelectItem(m_hitemDrag);

		m_hitemDrop = NULL;

		m_pDragImage = m_RadioTree.CreateDragImage(m_hitemDrag);  // get the image list for dragging
		// CreateDragImage() returns NULL if no image list
		// associated with the tree view control
		if( !m_pDragImage )
			return;
	
		m_bLDragging = TRUE;
		m_pDragImage->BeginDrag(0, CPoint(-5,-5));
		POINT pt = pNMTreeView->ptDrag;
		ClientToScreen( &pt );
		m_pDragImage->DragEnter(NULL, pt);
		SetCapture();
	}


}

BOOL IntRadio::PreTranslateMessage(MSG* pMsg) 
{
     if( pMsg->message == WM_KEYDOWN )
        {

			    if (    pMsg->wParam == VK_RETURN 
                     || pMsg->wParam == VK_ESCAPE)
                {
                        ::TranslateMessage(pMsg);
                        ::DispatchMessage(pMsg);
                        return TRUE; // DO NOT process further
                }

				if (pMsg->wParam == VK_F5)
				{
					CreateShoutcastWorkerThread(this);
                    ::TranslateMessage(pMsg);
                    ::DispatchMessage(pMsg);
				}


        }

	return CDialog::PreTranslateMessage(pMsg);
}


HBRUSH IntRadio::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if ((nCtlColor == CTLCOLOR_DLG) ||
		(nCtlColor == CTLCOLOR_STATIC))
	{
		if (nCtlColor == CTLCOLOR_STATIC)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(0,0,0));
		}
			

		return m_Brush;
	}
	else
	{

		HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		return hbr;
	}
}

void IntRadio::OnConnect() 
{
	CreateShoutcastWorkerThread(this);	
}

void IntRadio::OnSetfocusStationName() 
{
	m_StationList.SetCurSel(-1);

}

void IntRadio::OnDblclkTreeListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	POSITION pos = m_List.GetFirstSelectedItemPosition();
	int	nItem = m_List.GetNextSelectedItem(pos);

	CString itemURL;
	CShoutcastStation *temp;

	temp	 = (CShoutcastStation*)stationArray->GetAt(nItem);

	itemURL  = temp->playurl;
	itemURL += "listen.pls";
	ShellExecute(HWND(this),"open",itemURL,NULL,NULL,SW_NORMAL);
*/	
	*pResult = 0;
}

void IntRadio::OnSetfocusTreeListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_StationList.SetCurSel(-1);
	*pResult = 0;
}


void IntRadio::SyncWithDatabase()
{
	m_MyRadioTree.SyncWithDatabase(&providedStations);

	//m_MyRadioTree.Populate();

}

void IntRadio::OnButtonEdit()
{
	HTREEITEM curSel = m_MyRadioTree.GetSelectedItem();
	if (curSel)
	{
		m_MyRadioTree.EditLabel(curSel);
	}
}




BOOL IntRadio::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	ShellExecute(HWND(this),"open","help.htm",NULL,NULL,SW_NORMAL);
	
	return true;//CDialog::OnHelpInfo(pHelpInfo);
}
