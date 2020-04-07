//........................................................................................
//........................................................................................
//.. File Name: format.h
//.. Last Modified By: Donni Reitz-Pesek	donni@iobjects.com
//.. Modification date: 2/17/2000
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.
//..	 All rights reserved. This code may not be redistributed in source or linkable
//.. 	 object form without the express written consent of Interactive Objects.
//.. Contact Information: www.iobjects.com
//........................................................................................
//........................................................................................
//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the declaration of the format handler classes.

#ifndef __FORMAT_H__
#define __FORMAT_H__

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

    class c4_Handler;                   // not defined here

    extern c4_Handler* f4_CreateFormat(const c4_Property&, c4_HandlerSeq&);
    extern int f4_ClearFormat(char);
    extern int f4_CompareFormat(char, const c4_Bytes&, const c4_Bytes&);

/////////////////////////////////////////////////////////////////////////////

#endif
