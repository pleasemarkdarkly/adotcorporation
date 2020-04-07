//........................................................................................
//........................................................................................
//.. File Name: std.cpp
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
//  This file contains the implementation of the standard version classes.

#include "header.h"

#if q4_STD // until end of source
/////////////////////////////////////////////////////////////////////////////

#include "column.h"     // c4_ColCache

#if !q4_INLINE
static char _mk4stdInl[] = "mk4str.inl";
#undef THIS_FILE
#define THIS_FILE _mk4strInl
#include "mk4str.inl"
#endif
    
#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if !q4_NO_NS
using namespace std;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_String;

/////////////////////////////////////////////////////////////////////////////

#if !q4_MSVC && !q4_WATC

    // MS C/C++ has this handy stricmp: a case-insensitive version of strcmp
    // This version only works with 7-bit ASCII characters 0x00 through 0x7F
        
    static int stricmp(const char* p1, const char* p2)
    {
        int c1, c2;
    
#ifdef d4_USE_UNOPTIMIZED_CODE
            do
            {
                c1 = tolower(*p1++);
                c2 = tolower(*p2++);
            } while (c1 != 0 && c1 == c2);
#else
            do
            {
                c1 = *p1++;
                c2 = *p2++;
            } while (c1 != 0 && (c1 == c2 || tolower(c1) == tolower(c2)));
                
            c1 = tolower(c1);
            c2 = tolower(c2);
#endif
            
        return c1 - c2;
    }
    
#endif

/////////////////////////////////////////////////////////////////////////////
// c4_String

c4_String c4_String::Mid(int nFirst_, int nCount_) const
{
    int n = length();
    if (nFirst_ > n)
        nFirst_ = n;
    if (nFirst_ + nCount_ > n)
        nCount_ = n - nFirst_;

    return substr(nFirst_, nCount_);
}

int c4_String::CompareNoCase(const char* str_) const
{
        // this is not very "standard library-ish" ...
    return *(const string*) this == str_ ? 0 : stricmp(c_str(), str_);
}

/////////////////////////////////////////////////////////////////////////////
#endif // q4_STD
