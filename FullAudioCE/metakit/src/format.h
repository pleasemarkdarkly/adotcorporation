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
