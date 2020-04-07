//........................................................................................
//........................................................................................
//.. File Name: derived.h
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
//  This file contains the declaration of the derived sequences.

#ifndef __DERIVED_H__
#define __DERIVED_H__

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

    class c4_Cursor;                    // not defined here
    class c4_Sequence;                  // not defined here

    extern c4_Sequence* f4_CreateFilter(c4_Sequence&, c4_Cursor, c4_Cursor);
    extern c4_Sequence* f4_CreateSort(c4_Sequence&, c4_Sequence* =0);
    extern c4_Sequence* f4_CreateProject(c4_Sequence&, c4_Sequence&,
                                                    bool, c4_Sequence* =0);

/////////////////////////////////////////////////////////////////////////////

#endif
