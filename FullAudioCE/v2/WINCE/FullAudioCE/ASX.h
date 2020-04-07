//........................................................................................
//........................................................................................
//.. File Name: ASX.h	
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: Library to parse ASX file for metadata and load IO Jukebox. 
//.. Usage: Controls main menu															..
//.. Last Modified By: Matthew Tadd mattt@iobjects.com						..	
//.. Modification date: 12/27/2000														..
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.  									..
//..	 All rights reserved. This code may not be redistributed in source or linkable  ..
//.. 	 object form without the express written consent of Interactive Objects.        ..
//.. Contact Information: www.iobjects.com												..
//........................................................................................
//........................................................................................

#if !defined(AFX_ASX_H__6B35EB80_D5DB_11D4_A91B_005004AD38E7__INCLUDED_)
#define AFX_ASX_H__6B35EB80_D5DB_11D4_A91B_005004AD38E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef INCLUDE_JUKEBOX

#include "Jukebox.h"
#include "JPlaylist.h"

class ASX  
{
public:
	static int Load(Jukebox* pJukebox,const char* pszFileName);
};

#endif //INCLUDE_JUKEBOX

#endif // !defined(AFX_ASX_H__6B35EB80_D5DB_11D4_A91B_005004AD38E7__INCLUDED_)
