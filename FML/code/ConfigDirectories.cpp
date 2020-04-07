// onfigDirectories.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"
#include "configDirectories.h"

#include "dirtreectrl.h"
#include "direct.h"
#include "global.h"
#include "medialibrary.h"
#include "FMLDlg.h"
#include "UniqStrArr.h"

#include "WaitingDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IDC_BUTTON_CF_SAVE 5001
#define IDC_BUTTON_CF_INFO 5002
#define IDC_NAME_EDIT      5003
#define IDC_BUTTON_CF_SCAN 5004

#define IDC_INFOTEXT 5005
/////////////////////////////////////////////////////////////////////////////
// ConfigDirectories dialog

#include "version.h"

char * szStaticFormat = "Fullplay Media Library.\n\n"
						SZ_VERSION
						"\n\nCopyright (c) 2003 Fullplay Media Systems, Inc.\n\nPress F1 for help.\n\nDatabase information:\n\n"
                         "   Genres: %d\n\n   Artists: %d\n\n   Albums: %d\n\n   Tracks: %d\n\nDJs on network: %d\n\n";//Total queries: %d\n\n";


//#define IDC_TREE1 5001

ConfigDirectories::ConfigDirectories(CWnd* pParent /*=NULL*/)
	: CDialog(ConfigDirectories::IDD, pParent)
{
	//{{AFX_DATA_INIT(ConfigDirectories)
	//}}AFX_DATA_INIT

	m_InitDone = false;

}

BOOL ConfigDirectories::OnInitDialog() 
{
	CDialog::OnInitDialog();


	m_Brush = CreateSolidBrush(RGB(0xe2,0xeb,0xf5));

	m_WhiteBrush = CreateSolidBrush(RGB(0xff,0xff,0xff));

	FontNormal.CreateFont(-8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");
	FontBold.CreateFont(-8,0,0,0,FW_BOLD,0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");

	SetFont(&FontNormal);


	bTopNav.LoadBitmap(IDB_TOPNAV);
	stTopNav.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,0,588,40),this);
	stTopNav.SetBitmap(HBITMAP(bTopNav));


	bFinder.LoadBitmap(IDB_LABEL_FINDER);
	stFinder.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,80,290,80+24),this);
	stFinder.SetBitmap(HBITMAP(bFinder));

	bInfo.LoadBitmap(IDB_LABEL_INFO);
	stInfo.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(298,80,298+290,80+24),this);
	stInfo.SetBitmap(HBITMAP(bInfo));

	stInstructions.Create(_T("Choose folders to scan for music.  Click on the Scan For Music button when ready."),WS_CHILD|WS_VISIBLE|SS_LEFT,CRect(5,50,583,70),this);
	stInstructions.SetFont(&FontNormal);

	m_xSave.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON  ,CRect(505,5,505+72,35), this, IDC_BUTTON_CF_SAVE);

	m_xSave.SetTextColor(RGB(0,0,0));
	m_xSave.SetToolTipText("");
	m_xSave.SetSkin(IDB_B_SAVE_UP,IDB_B_SAVE_DOWN,IDB_B_SAVE_OVER,IDB_B_SAVE_DISABLED,0,0,0,0,0);

	stNetworkName.Create(_T("Network name for your media library"),WS_CHILD|WS_VISIBLE|SS_LEFT,CRect(5,13,215,30),this);
	stNetworkName.SetFont(&FontBold);

	m_xInfo.Create(_T(""),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON  ,CRect(220,13,220+15,28), this, IDC_BUTTON_CF_INFO);
	m_xInfo.SetToolTipText("This is the name that identifies your media library to a DJ");
	m_xInfo.SetSkin(IDB_INFO,IDB_INFO,IDB_INFO,IDB_INFO,0,0,0,0,0);

	NetworkNameEdit.Create(WS_CHILD|WS_VISIBLE|ES_LEFT,CRect(240,10,485,30),this,IDC_NAME_EDIT);
	NetworkNameEdit.SetLimitText(80);
	NetworkNameEdit.ModifyStyleEx(0,WS_EX_CLIENTEDGE,SWP_FRAMECHANGED);
	NetworkNameEdit.SetFont(&FontNormal);


	//m_DirTreeFrame.Create(NULL,SS_WHITERECT|WS_CHILD | WS_VISIBLE | WS_TABSTOP|WS_EX_CLIENTEDGE,
	//	 CRect(0,104,290,507),this,0xffff);
	//m_DirTreeFrame.ModifyStyleEx(0,WS_EX_CLIENTEDGE,SWP_FRAMECHANGED  );

	m_bLDragging = false;

	m_DirTree.Create(WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT
								|TVS_HASBUTTONS|TVS_SINGLEEXPAND|TVS_DISABLEDRAGDROP,//|TVS_CHECKBOXES,
					CRect(0,104,290,507),this, IDC_TREE1);  // TVS_SINGLEEXPAND|

	m_DirTree.ModifyStyleEx(0,WS_EX_CLIENTEDGE,SWP_FRAMECHANGED   );



	int numSelected = theMediaDatabase.contentDirectories.GetSize();

//	int j = 0;

//	while (j < numSelected)
//	{
//		m_DirTree.SetSelPath(theMediaDatabase.contentDirectories.GetAt(j));
//		j++;
//	}

	m_DirTree.DisplayTree(NULL,FALSE);


	m_WhiteFrame.Create(NULL,SS_WHITERECT|WS_CHILD | WS_VISIBLE | WS_TABSTOP|WS_EX_CLIENTEDGE,
		 CRect(298,104,588,507),this,0xffff);
	m_WhiteFrame.ModifyStyleEx(0,WS_EX_CLIENTEDGE,SWP_FRAMECHANGED   );

	CString csInfo;
	csInfo.Format(szStaticFormat,theMediaDatabase.numGenres,theMediaDatabase.numArtists,
								 theMediaDatabase.numAlbums,theMediaDatabase.numTracks);

	stInfoText.Create(csInfo,WS_CHILD|WS_VISIBLE|SS_LEFT,CRect(308,114,578,450),this,IDC_INFOTEXT);
	stInfoText.SetFont(&FontNormal);
	NetworkNameEdit.SetWindowText(theMediaDatabase.libraryName);

	CStringArray *arr = theMediaDatabase.GetDirectories();

	m_xScan.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON  ,CRect(308,497-22,308+120,497), this, IDC_BUTTON_CF_SCAN);
	m_xScan.SetTextColor(RGB(0,0,0));
	m_xScan.SetToolTipText("");
	m_xScan.SetSkin(IDB_B_SCAN_UP,IDB_B_SCAN_DOWN,IDB_B_SCAN_OVER,IDB_B_SCAN_DISABLED,0,0,0,0,0);

	m_InitDone = true;


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void ConfigDirectories::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ConfigDirectories)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ConfigDirectories, CDialog)
	//{{AFX_MSG_MAP(ConfigDirectories)
	ON_BN_CLICKED(IDC_BUTTON_CF_SCAN, OnRescan)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_NAME_EDIT, OnChangeEdit)
	ON_BN_CLICKED(IDC_BUTTON_CF_SAVE, OnSave)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConfigDirectories message handlers


void ConfigDirectories::OnRescan() 
{

	SetCursor(LoadCursor(NULL,IDC_WAIT));
	pMainDlg->EnableMenu(false);

	g_ServerActive = false;
	CWaitingDialog *pWaitDlg = new CWaitingDialog();
	pWaitDlg->SetParameters(0,"Preparing to rescan your folders...");
	pWaitDlg->Create(IDD_DIALOG2,NULL);
	pWaitDlg->ShowWindow(SW_SHOW);

	pMainDlg->m_playlistDlg.SyncWithDatabase();
	pMainDlg->m_radioDlg.SyncWithDatabase();

	SyncWithDatabase();

	
	theMediaDatabase.ByeByeUpnp();

	pWaitDlg->DestroyWindow();
	delete pWaitDlg;
	pMainDlg->EnableSync(false);

	if (theMediaDatabase.contentDirectories.GetSize())
	{
		theMediaDatabase.BuildDatabase();
	}
	else
	{
		theMediaDatabase.mediaManager.ClearAll();
	}

	SetCursor(LoadCursor(NULL,IDC_WAIT));

	pWaitDlg = new CWaitingDialog();
	pWaitDlg->SetParameters(0,"Processing and communicating with DJs...");
	pWaitDlg->Create(IDD_DIALOG2,NULL);
	pWaitDlg->ShowWindow(SW_SHOW);
			
	theMediaDatabase.SaveDatabase("fmldata.fml");
	theMediaDatabase.playListManager.RevalidateAllPlaylistEntries();
	pMainDlg->m_playlistDlg.m_mediaTree.Populate();
	pMainDlg->m_playlistDlg.m_playlistTree.Populate();
	
	// do this twice to make sure it takes
	theMediaDatabase.AnnounceUpnp();
	theMediaDatabase.AnnounceUpnp();

	

	// update the stats
	UpdateInfoText();

	g_ServerActive = true;

	pWaitDlg->DestroyWindow();
	delete pWaitDlg;
	SetCursor(LoadCursor(NULL,IDC_ARROW));
	pMainDlg->EnableMenu(true);

}

HBRUSH ConfigDirectories::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if ((nCtlColor == CTLCOLOR_DLG)||
		(nCtlColor == CTLCOLOR_STATIC))
	{
		if (nCtlColor == CTLCOLOR_STATIC)
		{

			if (pWnd->GetDlgCtrlID() == IDC_INFOTEXT)
			{
				pDC->SetBkMode(TRANSPARENT);
				return m_WhiteBrush;
			}
			else
			{
				pDC->SetBkMode(TRANSPARENT);
				return m_Brush;
			}
		}
		return m_Brush;
	}
	else
	{
		if (nCtlColor == CTLCOLOR_STATIC)
			pDC->SetBkMode(TRANSPARENT);

		HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		return hbr;
	}
}

LRESULT ConfigDirectories::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CDialog::DefWindowProc(message, wParam, lParam);
}

BOOL ConfigDirectories::PreTranslateMessage(MSG* pMsg) 
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
			m_DirTree.GetCheckedDirectories(m_DirTree.GetRootItem());

			m_DirTree.DeleteAllItems();

			m_DirTree.DisplayTree( NULL , FALSE );
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE; // DO NOT process further
		}

	}
	return CDialog::PreTranslateMessage(pMsg);
}

void ConfigDirectories::SyncWithDatabase()
{
	if (m_DirTree.HasChanged())
	{
		theMediaDatabase.oldContentDirectories.RemoveAll();

		int numItems = theMediaDatabase.contentDirectories.GetSize();
		for (int j = 0; j < numItems; j++)
		{
			theMediaDatabase.oldContentDirectories.Add(theMediaDatabase.contentDirectories.GetAt(j));
		}

		theMediaDatabase.contentDirectories.RemoveAll();

		m_DirTree.GetCheckedDirectories(m_DirTree.GetRootItem());
	}

	/*
	int j,k;j = 0;
	while (j < theMediaDatabase.contentDirectories.GetSize())
	{	k = j + 1;
		while (k < theMediaDatabase.contentDirectories.GetSize())
		{
			if (theMediaDatabase.contentDirectories.GetAt(k).Find(theMediaDatabase.contentDirectories.GetAt(j))==0)
			{
				theMediaDatabase.contentDirectories.RemoveAt(k);
			}
			else if (theMediaDatabase.contentDirectories.GetAt(j).Find(theMediaDatabase.contentDirectories.GetAt(k))==0)
			{
				theMediaDatabase.contentDirectories.RemoveAt(j);
			}
			else
			{
				k++;
			}
		}
		j++;
	}
	*/


	char NetworkName[40];
	memset(NetworkName,0,40);
	NetworkNameEdit.GetWindowText(NetworkName,39);
	CString netName = NetworkName;
	netName.Replace('>',']');
	netName.Replace('<','[');
	
	// remove leading spaces
	
	while (netName.Find(' ',0) == 0)
	{
		netName = netName.Right(netName.GetLength()-1);
	}
	if (netName.IsEmpty())
	{
		char hostname[50];
		gethostname(&hostname[0],50);
		CString defaultName= "FML on ";
		defaultName+=hostname;
		netName = defaultName;
	}
	theMediaDatabase.libraryName = netName;
	NetworkNameEdit.SetWindowText(netName);


}

void ConfigDirectories::OnSave()
{
	pMainDlg->OnSync();
}

void ConfigDirectories::OnChangeEdit() 
{
	if (m_InitDone)
	{
		pMainDlg->EnableSync(true);
	}
}

BOOL ConfigDirectories::OnHelpInfo(HELPINFO* pHelpInfo) 
{
		ShellExecute(HWND(this),"open","help.htm",NULL,NULL,SW_NORMAL);

	
	return true;//CDialog::OnHelpInfo(pHelpInfo);
}


void ConfigDirectories::UpdateInfoText()
{
		// update the stats
	CString csInfo;
	csInfo.Format(szStaticFormat,theMediaDatabase.numGenres,theMediaDatabase.numArtists,
								 theMediaDatabase.numAlbums,theMediaDatabase.numTracks,
								 theMediaDatabase.DJAddresses.GetSize());//, theMediaDatabase.totalQueries);
	
	stInfoText.SetWindowText(csInfo);

}
