// RefreshDatabaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"
#include "RefreshDatabaseDlg.h"
#include "medialibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RefreshDatabaseDlg dialog


RefreshDatabaseDlg::RefreshDatabaseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(RefreshDatabaseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(RefreshDatabaseDlg)
	//}}AFX_DATA_INIT

	ReadyToDraw = false;

}


void RefreshDatabaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RefreshDatabaseDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RefreshDatabaseDlg, CDialog)
	//{{AFX_MSG_MAP(RefreshDatabaseDlg)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_OK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RefreshDatabaseDlg message handlers

void RefreshDatabaseDlg::OnButtonStop() 
{
	if (AfxMessageBox("Are you sure ?",MB_YESNO) == IDYES)
	{
		theMediaDatabase.databaseStatus |= IML_ABORT_REFRESH;
		StopText = "Stopped.";

	}

	OnOK();
}


void RefreshDatabaseDlg::RefreshStoppedNotify()
{

   OnOK();
}

BOOL RefreshDatabaseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Brush = CreateSolidBrush(RGB(0xe2,0xeb,0xf5));

	pAnimView = new CAnimatedHtmlView();

	// let the message loop pick this up just now
	PostMessage(WM_USER+1,0,0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void RefreshDatabaseDlg::RefreshBuildNotify()
{
}

HBRUSH RefreshDatabaseDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if ((nCtlColor == CTLCOLOR_DLG) ||
		(nCtlColor == CTLCOLOR_STATIC))
	{
		if (nCtlColor == CTLCOLOR_STATIC)
			pDC->SetBkMode(TRANSPARENT);

		return m_Brush;
	}
	else
	{

		HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		return hbr;
	}
}

LRESULT RefreshDatabaseDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == (WM_USER+1))
	{
		SetEvent(g_DisplayReady);
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void RefreshDatabaseDlg::OnOk() 
{
	CDialog::OnOK();
	
}
