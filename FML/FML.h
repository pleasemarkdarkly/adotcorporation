// FML.h : main header file for the FML application
//

#if !defined(AFX_FML_H__1AE73FB3_9B4A_408C_AEB1_58F2666035EE__INCLUDED_)
#define AFX_FML_H__1AE73FB3_9B4A_408C_AEB1_58F2666035EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

char *GetRegisteredWindowClassName();

/////////////////////////////////////////////////////////////////////////////
// CFMLApp:
// See FML.cpp for the implementation of this class
//

class CFMLApp : public CWinApp
{
public:
	CFMLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFMLApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFMLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FML_H__1AE73FB3_9B4A_408C_AEB1_58F2666035EE__INCLUDED_)
