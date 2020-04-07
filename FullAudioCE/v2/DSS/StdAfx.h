// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__05FC83C9_EC84_11D2_A150_00A0C9777D6E__INCLUDED_)
#define AFX_STDAFX_H__05FC83C9_EC84_11D2_A150_00A0C9777D6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

// TODO: reference additional headers your program requires here
#define USE_EQUALIZER 1 //Use Equalizer stuff

#define MAX(a,b) (a > b) ? a : b
#define MIN(a,b) (a < b) ? a : b

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__05FC83C9_EC84_11D2_A150_00A0C9777D6E__INCLUDED_)
