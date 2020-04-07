// WaitingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"
#include "WaitingDialog.h"
#include "medialibrary.h"
#include "FMLDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitingDialog dialog

#define IDC_THETEXT 8000

#define WAIT_WIDTH   365
#define WAIT_HEIGHT	 165


CWaitingDialog *  pTheWaitDialog;

CWaitingDialog::CWaitingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitingDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitingDialog)
	//}}AFX_DATA_INIT
}


void CWaitingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitingDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitingDialog, CDialog)
	//{{AFX_MSG_MAP(CWaitingDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaitingDialog message handlers

BOOL CWaitingDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	timeTick = 0;
	m_bExit = false;
	
	m_Brush = CreateSolidBrush(RGB(0xff,0xff,0xff));

	FontNormal.CreateFont(-8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");
	SetFont(&FontNormal);

	bLeft.LoadBitmap(IDB_NOTIFY_A);
	stLeft.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,0,24,160),this);
	stLeft.SetBitmap(HBITMAP(bLeft));

	bRight.LoadBitmap(IDB_NOTIFY_B);
	stRight.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(24+312,0,24+312+24,160),this);
	stRight.SetBitmap(HBITMAP(bRight));


	bBottom.LoadBitmap(IDB_NOTIFY_C);
	stBottom.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(24,160-12,24+312,160),this);
	stBottom.SetBitmap(HBITMAP(bBottom));


	m_stText.Create(_T(""),WS_CHILD|WS_VISIBLE|SS_LEFT,CRect(24,130,24+312,130+18),this,IDC_THETEXT);
	m_stText.SetFont(&FontNormal);
	m_stText.SetWindowText(m_Text);


	if (m_Series == 0)
	{

		NumBitmaps = 6;
		bAnimBitmaps[0].LoadBitmap(ANIM_NOTIFY_1);
		bAnimBitmaps[1].LoadBitmap(ANIM_NOTIFY_2);
		bAnimBitmaps[2].LoadBitmap(ANIM_NOTIFY_3);
		bAnimBitmaps[3].LoadBitmap(ANIM_NOTIFY_4);
		bAnimBitmaps[4].LoadBitmap(ANIM_NOTIFY_5);
		bAnimBitmaps[5].LoadBitmap(ANIM_NOTIFY_6);
	
	}
	else if (m_Series == 1)
	{
		NumBitmaps = 8;
		bAnimBitmaps[0].LoadBitmap(ANIM_START_1);
		bAnimBitmaps[1].LoadBitmap(ANIM_START_2);
		bAnimBitmaps[2].LoadBitmap(ANIM_START_3);
		bAnimBitmaps[3].LoadBitmap(ANIM_START_4);
		bAnimBitmaps[4].LoadBitmap(ANIM_START_5);
		bAnimBitmaps[5].LoadBitmap(ANIM_START_6);
		bAnimBitmaps[6].LoadBitmap(ANIM_START_7);
		bAnimBitmaps[7].LoadBitmap(ANIM_START_8);
	
	}

	
	
	sCurrentBitmap.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(24,0,24+312,130),this);


	sCurrentBitmap.SetBitmap(HBITMAP(bAnimBitmaps[0]));
	currBitmap = 0;

	SetWindowPos(NULL,0,0,WAIT_WIDTH,WAIT_HEIGHT,SWP_NOZORDER | SWP_NOMOVE);  


	//PostMessage(WM_USER+1,0,0);

	
	//m_Timer = SetTimer(0,400,0);
	


	//PostMessage(WM_USER+1,0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaitingDialog::OnTimer(UINT nIDEvent) 
{
	
	
	if ((currBitmap+1) < NumBitmaps)
	{
		currBitmap++;
	}
	else
	{
		currBitmap = 0;
	}

	timeTick++;

	if (timeTick == 2) SetEvent(g_WaitDisplayReady);

	
	//sCurrentBitmap.Invalidate(TRUE);
	//Invalidate(TRUE);

	if (m_bExit)
	{
		if (timeTick > 6)
		{
			KillTimer(m_Timer);

			PostMessage(WM_USER+3,0,0);
		}
		
	}
	else
	{
		sCurrentBitmap.SetBitmap(HBITMAP(bAnimBitmaps[currBitmap]));
	}


	
	CDialog::OnTimer(nIDEvent);
}

HBRUSH CWaitingDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

LRESULT CWaitingDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == (WM_USER+1))
	{
		
		return 1;
	}

	if (message == (WM_USER+2))
	{
		m_bExit = true;
		
		return 1;
	}
	if (message == (WM_USER+3))
	{
		OnOK();
		return 1;
	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CWaitingDialog::Done()
{

	PostMessage(WM_USER+2,0,0); 

}

void CWaitingDialog::SetParameters(int AnimationSeries, CString text) 
{
	m_Series = AnimationSeries;
	m_Text = text;

}

void CWaitingDialog::AnimateFrame()
{

	if ((currBitmap+1) < NumBitmaps)
	{
		currBitmap++;
	}
	else
	{
		currBitmap = 0;
	}

	sCurrentBitmap.SetBitmap(HBITMAP(bAnimBitmaps[currBitmap]));

}

BOOL CWaitingDialog::DestroyWindow() 
{



	return CDialog::DestroyWindow();
}

void CWaitingDialog::OnOK() 
{
	
	
	FontNormal.DeleteObject();

	bLeft.DeleteObject();
	
	bRight.DeleteObject();
	bBottom.DeleteObject();

	bAnimBitmaps[0].DeleteObject();
	bAnimBitmaps[1].DeleteObject();

	stLeft.DestroyWindow(); stRight.DestroyWindow(); stBottom.DestroyWindow(); sCurrentBitmap.DestroyWindow();

	

	
	CDialog::OnOK();
}



BOOL CWaitingDialog::PreTranslateMessage(MSG* pMsg) 
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
