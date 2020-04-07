// ScanDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"
#include "ScanDialog.h"

#include "medialibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScanDialog dialog

#define IDC_ADDTEXT 7000
#define IDC_BUTTON_CANCEL 7001

#define SCAN_WIDTH  485
#define SCAN_HEIGHT 129


CScanDialog::CScanDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScanDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScanDialog)
	//}}AFX_DATA_INIT


}


void CScanDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanDialog)
	DDX_Control(pDX, IDCANCEL, m_CancelButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanDialog, CDialog)
	//{{AFX_MSG_MAP(CScanDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CANCEL,OnCancel)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanDialog message handlers

void CScanDialog::OnOK() 
{
	// TODO: Add extra validation here

	KillTimer(m_Timer);

	FontNormal.DeleteObject();

	bLeft.DeleteObject();
	
	bRight.DeleteObject();
	bBottom.DeleteObject();

	for (int i = 0; i < 10; i ++)
		bAnimBitmaps[i].DeleteObject();

	m_xCancel.DestroyWindow();

	stLeft.DestroyWindow(); stRight.DestroyWindow(); stBottom.DestroyWindow(); sCurrentBitmap.DestroyWindow();


	CDialog::OnOK();
}

LRESULT CScanDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == (WM_USER+1))
	{
		SetEvent(g_DisplayReady);
		return 1;
	}

	if (message == (WM_USER+2))
	{
		OnOK();
		return 1;
	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

HBRUSH CScanDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

BOOL CScanDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

		m_Brush = CreateSolidBrush(RGB(0xff,0xff,0xff));

	FontNormal.CreateFont(-8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");
	SetFont(&FontNormal);

	bLeft.LoadBitmap(IDB_SEARCH_A);
	stLeft.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,0,10,123),this);
	stLeft.SetBitmap(HBITMAP(bLeft));

	bRight.LoadBitmap(IDB_SEARCH_B);
	stRight.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(470,0,480,123),this);
	stRight.SetBitmap(HBITMAP(bRight));


	bBottom.LoadBitmap(IDB_SEARCH_C);
	stBottom.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(10,118,10+460,123),this);
	stBottom.SetBitmap(HBITMAP(bBottom));

	sCurrentBitmap.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(10,0,470,89),this);

	bAnimBitmaps[0].LoadBitmap(ANIM_SEARCH_1);
	bAnimBitmaps[1].LoadBitmap(ANIM_SEARCH_2);
	bAnimBitmaps[2].LoadBitmap(ANIM_SEARCH_3);
	bAnimBitmaps[3].LoadBitmap(ANIM_SEARCH_4);
	bAnimBitmaps[4].LoadBitmap(ANIM_SEARCH_5);
	bAnimBitmaps[5].LoadBitmap(ANIM_SEARCH_6);
	bAnimBitmaps[6].LoadBitmap(ANIM_SEARCH_7);
	bAnimBitmaps[7].LoadBitmap(ANIM_SEARCH_8);
	bAnimBitmaps[8].LoadBitmap(ANIM_SEARCH_9);
	bAnimBitmaps[9].LoadBitmap(ANIM_SEARCH_10);
	
	
	
	
	sCurrentBitmap.SetBitmap(HBITMAP(bAnimBitmaps[0]));
	currBitmap = 0;

	m_Adding.Create(_T(""),WS_CHILD|WS_VISIBLE|SS_LEFT,CRect(10,90,470-66-4,90+28),this,IDC_ADDTEXT);
	m_Adding.SetFont(&FontNormal);

	m_xCancel.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON  ,CRect(470-66,115-22,470,115), this, IDC_BUTTON_CANCEL);
	m_xCancel.SetTextColor(RGB(0,0,0));
	m_xCancel.SetToolTipText("");
	m_xCancel.SetSkin(IDB_BT_CANCEL_UP,IDB_BT_CANCEL_DOWN,IDB_BT_CANCEL_OVER,IDB_BT_CANCEL_DISABLED,0,0,0,0,0);

	SetWindowPos(NULL,0,0,SCAN_WIDTH,SCAN_HEIGHT,SWP_NOZORDER | SWP_NOMOVE);  

	PostMessage(WM_USER+1,0,0);



	m_Timer = SetTimer(0,ANIM_INTERVAL,NULL);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScanDialog::Done()
{
	PostMessage(WM_USER+2,0,0); 

}

void CScanDialog::OnCancel() 
{	

	theMediaDatabase.databaseStatus |= IML_ABORT_REFRESH;

}

void CScanDialog::OnTimer(UINT nIDEvent) 
{
	
	if ((currBitmap+1) < NUM_BITMAPS_IN_ANIM)
	{
		currBitmap++;
	}
	else
	{
		currBitmap = 0;
	}

	sCurrentBitmap.SetBitmap(HBITMAP(bAnimBitmaps[currBitmap]));
	
	CDialog::OnTimer(nIDEvent);
}

void CScanDialog::ShowUpdate(const char * currentlyAdding)
{
	currText = currentlyAdding;
	m_Adding.SetWindowText(currText);
}


BOOL CScanDialog::PreTranslateMessage(MSG* pMsg) 
{
    if( pMsg->message == WM_KEYDOWN )
     {
		 // filter out the enter/escape default handling

			    if (    pMsg->wParam == VK_RETURN 
                     || pMsg->wParam == VK_ESCAPE)
                {
                        ::TranslateMessage(pMsg);
                        ::DispatchMessage(pMsg);
                        return TRUE; // DO NOT process further
                }

	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CScanDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	ShellExecute(HWND(this),"open","help.htm",NULL,NULL,SW_NORMAL);
	return true;	
	//return CDialog::OnHelpInfo(pHelpInfo);
}
