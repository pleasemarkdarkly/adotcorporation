// FML.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FML.h"
#include "FMLDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL RestoreWindowInstance(UINT idr_MainFrame, const char *szClassName="");
void UnregisterWindowInstance(HWND hwnd=NULL);
char *GetRegisteredWindowClassName();

#define RestoreApplication(y) RestoreWindowInstance(IDR_MAINFRAME,y)


const char *JustFilename(const char *fn)
{
  char t[MAX_PATH];
  char *f = NULL;
  if (GetFullPathName(fn, sizeof(t), t, &f)) {
    return f;
  }
  return fn;
}

BOOL _AppClassRegistered = FALSE;
/*static */ char _AppClassName[256]= "";

char *GetRegisteredWindowClassName()
{
    TRACE("AppInstance: GetRegisteredWindowClassName(): [%s]\n",_AppClassName);
    return _AppClassName;
}

void UnregisterWindowInstance(HWND hwnd/*=NULL*/)
{
   if (!_AppClassRegistered) return;
   char szClass[256];
   strcpy(szClass,_AppClassName);
   if ((hwnd == NULL) || (hwnd != NULL && GetClassName(hwnd,szClass,sizeof(szClass)))) {
      UnregisterClass(szClass,AfxGetInstanceHandle());
   }
}

//---------------------------------------------------------------------
// RestoreWindowInstance(const char *szClassName)
// Single Instance Logic based on window class name registration.
//
// - Return TRUE if window class is found and bring app to foreground.
// - Return FALSE if window class is NOT found.  Register first instance
//   window class.
//

BOOL RestoreWindowInstance(UINT idr_MainFrame, const char *szClassName /* = "" */)
{
    strcpy(_AppClassName,szClassName);
    if (strlen(_AppClassName) == 0) {
        char szFileName[MAX_PATH];
        //GetModuleFileName(0,_AppClassName,sizeof(_AppClassName));
        //ExtractFileName(_AppClassName,_AppClassName,TRUE);
        GetModuleFileName(0,szFileName,sizeof(szFileName));
        strcpy(_AppClassName,JustFilename(szFileName));
        strcat(_AppClassName,"WindowClass");
    }
    CWnd *pWndPrev = CWnd::FindWindow(NULL,"Fullplay Media Library  ");
    if (pWndPrev) {

		pWndPrev->ShowWindow(SW_RESTORE);
		pWndPrev->FlashWindow(true);
		
        return TRUE;
    }
    // First instance. Proceed as normal.
    // Register our unique class name that we wish to use
    WNDCLASS wndcls;
    ZeroMemory(&wndcls, sizeof(WNDCLASS));

    wndcls.style          = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    //wndcls.lpfnWndProc    = ::DefWindowProc;
    wndcls.lpfnWndProc    = AfxWndProc;
    
    wndcls.hInstance      = AfxGetInstanceHandle();
    //wndcls.hIcon          = NULL; // LoadIcon(idr_mainframe); // or load a different icon
    wndcls.hIcon          = LoadIcon(wndcls.hInstance,MAKEINTRESOURCE(idr_MainFrame)); // or load a different icon
    wndcls.hCursor        = NULL; //LoadCursor( IDC_ARROW );
    wndcls.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
    wndcls.lpszMenuName   = NULL;
    wndcls.lpszClassName  = _AppClassName;
    TRACE("AppInstance: Window Class Name: [%s]\n",_AppClassName);
    // Register new class and exit if it fails
//    if(!AfxRegisterClass(&wndcls)) {
        //TRACE("AppInstance:: Failure to register window class: \"%s\"\n",_AppClassName);
    //}
    //_AppClassRegistered = TRUE;
    return FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CFMLApp

BEGIN_MESSAGE_MAP(CFMLApp, CWinApp)
	//{{AFX_MSG_MAP(CFMLApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFMLApp construction

CFMLApp::CFMLApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFMLApp object

CFMLApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFMLApp initialization

BOOL CFMLApp::InitInstance()
{
	if (RestoreApplication("FML")) return FALSE;

	AfxOleInit();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CFMLDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
