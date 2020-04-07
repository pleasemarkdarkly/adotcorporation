// FMLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FML.h"
#include "FMLDlg.h"

#include "traydialog.h"
#include "configDirectories.h"
#include "intradio.h"
#include "medialibrary.h"
#include "global.h"
#include "iml_xml.h"
#include "afxhtml.h"
#include "encode.h"
#include "WaitingDialog.h"

#define CLIENT_AREA_LEFT 188
#define HELP_AREA_LEFT 188

#define CLIENT_AREA_TOP  50
#define CLIENT_AREA_RIGHT	775
#define CLIENT_AREA_BOTTOM	557


#define IDC_BUTTON_PL 2001
#define IDC_BUTTON_IR 2002
#define IDC_BUTTON_CF 2003
#define IDC_BUTTON_FP 2004

#define IDLE_SYNC_INTERVAL  20000  // 20 seconds

#define TRIGGER_SYNC 4   //80 seconds of idle before resyncing

#define APP_WIDTH  797
#define APP_HEIGHT 598

CFMLDlg * pMainDlg = NULL;

bool gFirstTime = FALSE;

bool gInitDone = FALSE;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFMLDlg dialog

CFMLDlg::CFMLDlg(CWnd* pParent /*=NULL*/)
	: CTrayDialog(CFMLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFMLDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFMLDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrayDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFMLDlg)
	//}}AFX_DATA_MAP

	//DDX_Control(pDX, IDC_ST_CONFIGURE, m_xConfigure);
	//DDX_Control(pDX, IDC_ST_PLAYLISTS, m_xPlaylists);
	//DDX_Control(pDX, IDC_ST_INTERNET_RADIO, m_xInternet);
	//DDX_Control(pDX, IDC_ST_HELP, m_xHelp);
	//DDX_Control(pDX, IDC_ST_FULLPLAY, m_xFullplay);
	//DDX_Control(pDX, IDC_ST_UPDATE, m_xUpdate);

}

BEGIN_MESSAGE_MAP(CFMLDlg, CTrayDialog)
	//{{AFX_MSG_MAP(CFMLDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_HIDE, OnHide)
	ON_COMMAND(ID_IMLMENU_OPEN, OnImlmenuOpen)
	ON_COMMAND(IDC_MENU_EXIT, OnMenuExit)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_CANCELMODE()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_CAPTURECHANGED()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_PL, OnStPlaylists)
	ON_BN_CLICKED(IDC_BUTTON_IR, OnStInternet)
	ON_BN_CLICKED(IDC_BUTTON_CF, OnStConfig)
	ON_BN_CLICKED(IDC_BUTTON_FP, OnStFullplay)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFMLDlg message handlers


BOOL CFMLDlg::OnInitDialog()
{
	bool bFirstTime = false;
	pMainDlg = this;

	m_IdleElapsed = 0;
	m_SyncNeeded = false;



	CTrayDialog::OnInitDialog();

	m_Brush = CreateSolidBrush(RGB(0xe2,0xeb,0xf5));

	m_CurrentViewIndex = 0;

	gInitDone = FALSE;

		m_xPlaylists.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_AUTORADIOBUTTON,CRect(10,72,10+156,72+36), this, IDC_BUTTON_PL);
	m_xInternet.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_AUTORADIOBUTTON,CRect(10,108,10+156,108+36), this, IDC_BUTTON_IR);
	m_xConfigure.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_AUTORADIOBUTTON,CRect(10,36,10+156,36+36), this, IDC_BUTTON_CF);
	m_xFullplay.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(10,522,10+156,522+42), this, IDC_BUTTON_FP);


	m_xConfigure.SetTextColor(RGB(0,0,255));
	m_xConfigure.SetToolTipText("Configure the media library");
	m_xConfigure.SetSkin(IDB_CONFIG_UP,IDB_CONFIG_DOWN,IDB_CONFIG_OVER,0,0,0,0,0,0);
	//m_xConfigure.SetSkin(IDB_B3_NORM,IDB_B3_DOWN,IDB_B3_OVER,IDB_B3_GREY,IDB_B3_FOCUS,0,0,0,0);

	m_xPlaylists.SetTextColor(RGB(0,0,255));
	m_xPlaylists.SetToolTipText("Create Playlists");
	m_xPlaylists.SetSkin(IDB_PLAYLIST_UP,IDB_PLAYLIST_DOWN,IDB_PLAYLIST_OVER,0,0,0,0,0,0);
	//m_xPlaylists.SetSkin(IDB_B1_NORM,IDB_B1_DOWN,IDB_B1_OVER,IDB_B1_GREY,IDB_B1_FOCUS,0,0,0,0);

	m_xInternet.SetTextColor(RGB(0,0,255));
	m_xInternet.SetToolTipText("Import Internet Radio Stations");
	m_xInternet.SetSkin(IDB_IR_UP,IDB_IR_DOWN,IDB_IR_OVER,0,0,0,0,0,0);
	//m_xInternet.SetSkin(IDB_B2_NORM,IDB_B2_DOWN,IDB_B2_OVER,0,IDB_B2_FOCUS,0,0,0,0);

	
	m_xFullplay.SetTextColor(RGB(0,0,255));
	m_xFullplay.SetToolTipText("Explore www.fullplaymedia.com");
	m_xFullplay.SetSkin(IDB_FULLPLAY_UP,IDB_FULLPLAY_OVER,IDB_FULLPLAY_OVER,0,0,0,0,0,0);
	//m_xFullplay.SetSkin(IDB_B5_NORM,IDB_B5_DOWN,IDB_B5_OVER,0,0,0,0,0,0);

	t.LoadBitmap(IDB_FML_TITLEBAR);
	a.LoadBitmap(IDB_A);
	b.LoadBitmap(IDB_B);
	c.LoadBitmap(IDB_C);
	d.LoadBitmap(IDB_D);
	e.LoadBitmap(IDB_E);
	f.LoadBitmap(IDB_F);
	ln.LoadBitmap(IDB_LEFTNAV_BG);

	m_stTitle.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,0,792,36),this);
	m_stTitle.SetBitmap(HBITMAP(t));

	m_stA.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(0,36,10,538+36),this);
	m_stA.SetBitmap(HBITMAP(a) );

	m_stB.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(166,36,166+22,36+538),this);
	m_stB.SetBitmap( HBITMAP(b) );

	m_stC.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(10,564,10+156,564+10),this);
	m_stC.SetBitmap( HBITMAP(c) );

	m_stD.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(188,36,188+588,36+14),this);
	m_stD.SetBitmap( HBITMAP(d) );

	m_stE.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(776,36,776+16,36+538),this);
	m_stE.SetBitmap( HBITMAP(e) );

	m_stF.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(188,556,188+588,556+18),this);
	m_stF.SetBitmap( HBITMAP(f) );

	m_stLN.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, CRect(10,144,10+156,144+378),this);
	m_stLN.SetBitmap( HBITMAP(ln) );

		// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
//			pSysMenu->AppendMenu(MF_SEPARATOR);
//			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetWindowPos(NULL,0,0,APP_WIDTH,APP_HEIGHT,SWP_NOZORDER | SWP_NOMOVE);  
		


	TraySetIcon(IDI_ICON7);
	TraySetToolTip("Fullplay Media Library");
	TraySetMenu(IDR_MENU1);
	TrayShow();

	// get the message pump going before doing further initialization

	PostMessage(WM_USER+6,0,0);

	return TRUE;
}

	
int CFMLDlg::Init()
{
	SetCursor(LoadCursor(NULL,IDC_APPSTARTING));
	
	CWaitingDialog *pWaitDlg = new CWaitingDialog();
	pWaitDlg->SetParameters(1,"Starting up, please wait...");
	pWaitDlg->Create(IDD_DIALOG2,NULL);
	pWaitDlg->ShowWindow(SW_SHOW);


	// Create a manual reset event to signal termination.
    g_hEventCleanup = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    // Verify that the control event was created.
    if (!g_hEventCleanup)
    {
        debugMessage("Could not create cleanup event!\n\r");
        return FALSE;
    }
	
	if (!WinsockInit()) 
    {
        debugMessage("Failed to initialize winsock.");
        return FALSE;
    }

	if (!BuildDescURL(&pDeviceDescription))
	{
		debugMessage("Failed to generate the description URL.\n\r");
		return FALSE;
	}




	m_pCurrentScreen = NULL;

			BOOL bFirstTime = FALSE;
			theMediaDatabase.LoadDatabase("fmldata.fml");

			if ((theMediaDatabase.libraryName.IsEmpty()) || (theMediaDatabase.libraryName == "###FirstTime"))
			{
					char hostname[50];
					gethostname(&hostname[0],50);
					CString defaultName= "FML on ";
					defaultName+=hostname;
					theMediaDatabase.libraryName = defaultName;
					gFirstTime = true;

					GetUniqueId(&deviceDescriptionUID[0],64);
					theMediaDatabase.libraryGUID = deviceDescriptionUID;
			}



	m_radioDlg.Create(IDD_INT_RADIO,this);

	m_configDlg.Create(IDD_CONFIG_DIR,this);

	m_playlistDlg.Create(IDD_NEW_PLAYLIST,this);

	m_playlistDlg.PopulateTrees();

	m_configDlg.UpdateInfoText();

			
			EnableSync(false);

			g_ServerActive = true;


			debugMessage("Starting uPnP discovery\n\r");

			startUPnPDiscovery(descFile);

			debugMessage("Starting HTTP server\n\r");

			StartHTTPServer();


			if (gFirstTime)
			{
				m_xConfigure.PostMessage(BM_CLICK,0,0);
			}
			else
			{
				m_xPlaylists.PostMessage(BM_CLICK,0,0);
			}

	pWaitDlg->DestroyWindow();
	delete pWaitDlg;

	SetCursor(LoadCursor(NULL,IDC_ARROW));

	m_Timer = SetTimer(0,IDLE_SYNC_INTERVAL,NULL);
	

	return 1;  // return TRUE  unless you set the focus to a control
}


void CFMLDlg::UpdateScreenSelection(int iSelection)
{
	if (m_CurrentViewIndex != iSelection)
	{
		DWORD leftPosition = CLIENT_AREA_LEFT;
		if (m_pCurrentScreen)
			m_pCurrentScreen->SetWindowPos(	NULL, CLIENT_AREA_LEFT,CLIENT_AREA_TOP, 0, 0,
										SWP_HIDEWINDOW | SWP_NOSIZE);

		switch (iSelection)
		{
		case 1:
				m_pCurrentScreen = &m_playlistDlg;
				if (m_xPlaylists.GetCheck() != 1)
				{
					m_xPlaylists.SetCheck(1);
				}
				
			break;
		case 2:
				m_pCurrentScreen = &m_radioDlg;
				
			break;
		case 3:
				m_pCurrentScreen = &m_configDlg;
				if (m_xConfigure.GetCheck() != 1)
				{
					m_xConfigure.SetCheck(1);
				}
				
				
			break;
		default:
			m_pCurrentScreen = &m_playlistDlg;
			break;
		}
	
		if (m_pCurrentScreen)
		{
				m_pCurrentScreen->SetWindowPos(	NULL, leftPosition,CLIENT_AREA_TOP, 0, 0,
										SWP_SHOWWINDOW | SWP_NOSIZE);
				m_pCurrentScreen->SetFocus();
		}
	
		m_CurrentViewIndex = iSelection;
	}
}








void CFMLDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CTrayDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFMLDlg::OnPaint() 
{
	CTrayDialog::OnPaint();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFMLDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void CFMLDlg::OnStConfig() 
{
	UpdateScreenSelection(3);

}



void CFMLDlg::OnStInternet() 
{
	UpdateScreenSelection(2); 

}




void CFMLDlg::OnHide() 
{
	this->ShowWindow(SW_HIDE);

}

void CFMLDlg::OnImlmenuOpen() 
{
	if (g_ServerActive)
		this->ShowWindow(SW_RESTORE);	
}

void CFMLDlg::OnMenuExit() 
{
	if (g_ServerActive)
		OnOK();
}

void CFMLDlg::OnOK() 
{
	KillTimer(m_Timer);

	SetWindowText("Please wait while FML shuts down...");

	m_playlistDlg.SyncWithDatabase();
	m_radioDlg.SyncWithDatabase();
	m_configDlg.SyncWithDatabase();

	theMediaDatabase.SaveDatabase("fmldata.fml");

	debugMessage("Cleaning up - signalling threads to die\n\r");

	// Signal misc threads to terminate.
    SetEvent(g_hEventCleanup);

	// Stop the HTTP server threads
	StopHTTPServer();


	if (pDeviceRoot)
	{
		DoCleanupCd(pDeviceRoot);
		pDeviceRoot = NULL;
	}

	if (pDeviceDescription)
	{
		free(pDeviceDescription);
		pDeviceDescription = NULL;
	}

	theMediaDatabase.Shutdown();

	TrayHide();

	CTrayDialog::OnOK();
}

void CFMLDlg::OnClose()
{
	OnOK();
}

void CFMLDlg::EnableAllButtons()
{

	m_xConfigure.EnableWindow(true);
	m_xPlaylists.EnableWindow(true);;
	m_xInternet.EnableWindow(true);;
	m_xFullplay.EnableWindow(true);
}

void CFMLDlg::DisableAllButtons()
{
	m_xConfigure.EnableWindow(false);
	m_xPlaylists.EnableWindow(false);;
	m_xInternet.EnableWindow(false);;
	m_xFullplay.EnableWindow(false);
}


void CFMLDlg::OnDestroy() 
{
	CTrayDialog::OnDestroy();

}

void CFMLDlg::OnCancelMode() 
{
	CTrayDialog::OnCancelMode();
}

void CFMLDlg::OnCancel() 
{
	OnOK();	
}

void CFMLDlg::OnStPlaylists() 
{
	UpdateScreenSelection(1);
}

void CFMLDlg::OnStFullplay() 
{
	ShellExecute(HWND(this),"open","http://www.fullplaymedia.com",NULL,NULL,SW_NORMAL);
}



HBRUSH CFMLDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

BOOL CFMLDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
    cs.lpszClass = GetRegisteredWindowClassName();

	cs.cx = APP_WIDTH ;//794;
	cs.cy = APP_HEIGHT; //576;
	
	return CTrayDialog::PreCreateWindow(cs);
}


void CFMLDlg::OnSync() 
{
	m_SyncNeeded = false;

	SetCursor(LoadCursor(NULL,IDC_WAIT));

	CWaitingDialog *pWaitDlg = new CWaitingDialog();
	pWaitDlg->SetParameters(0,"Communicating with DJs...");
	pWaitDlg->Create(IDD_DIALOG2,NULL);
	pWaitDlg->ShowWindow(SW_SHOW);

	m_playlistDlg.SyncWithDatabase();
	m_radioDlg.SyncWithDatabase();
	m_configDlg.SyncWithDatabase();
	
	theMediaDatabase.SaveDatabase("fmldata.fml");

	theMediaDatabase.ReAdvertiseUPnP();

	EnableSync(false);

	pWaitDlg->DestroyWindow();
	delete pWaitDlg;

	SetCursor(LoadCursor(NULL,IDC_ARROW));
}

void CFMLDlg::EnableSync(bool Enable)
{

	m_SyncNeeded = Enable;
	m_IdleElapsed = 0;

	m_playlistDlg.m_xSave.EnableWindow(Enable);
	m_radioDlg.m_xSave.EnableWindow(Enable);
	m_configDlg.m_xSave.EnableWindow(Enable);

}


void CFMLDlg::OnSize(UINT nType, int cx, int cy) 
{
	int ncx = APP_WIDTH ;//794;
	int ncy = APP_HEIGHT; //576;

	CTrayDialog::OnSize(nType, cx,cy);
	
	// TODO: Add your message handler code here
	
}

void CFMLDlg::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	CTrayDialog::OnCaptureChanged(pWnd);
}

LRESULT CFMLDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == (WM_USER+6))	
	{
		Init();
		return 1;
	}
	return CTrayDialog::DefWindowProc(message, wParam, lParam);
}

BOOL CFMLDlg::PreTranslateMessage(MSG* pMsg) 
{
     if( pMsg->message == WM_KEYDOWN )
        {

			    if (    pMsg->wParam == VK_RETURN 
                     || pMsg->wParam == VK_ESCAPE)
                {
                        //::TranslateMessage(pMsg);
                        //::DispatchMessage(pMsg);
                        return TRUE; // DO NOT process further
                }



	 }
	return CTrayDialog::PreTranslateMessage(pMsg);
}


void CFMLDlg::OnTimer(UINT nIDEvent) 
{
	if (m_SyncNeeded)
	{
	
		m_IdleElapsed++;

		if (m_IdleElapsed >= TRIGGER_SYNC)
		{
			SetCursor(LoadCursor(NULL,IDC_WAIT));
			m_playlistDlg.SyncWithDatabase();
			m_radioDlg.SyncWithDatabase();
			theMediaDatabase.SaveDatabase("fmldata.fml");
			theMediaDatabase.ReAdvertiseUPnP();
			EnableSync(false);
			m_SyncNeeded = false;
			m_IdleElapsed = 0;
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		}
	}
	
	CTrayDialog::OnTimer(nIDEvent);
}

BOOL CFMLDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	ShellExecute(HWND(this),"open","help.htm",NULL,NULL,SW_NORMAL);
	
	return true;//CTrayDialog::OnHelpInfo(pHelpInfo);
}

