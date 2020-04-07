//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  Specific header for the Metrowerks CodeWarrior C++ development tools.

#define q4_MWCW 1

/////////////////////////////////////////////////////////////////////////////

#if q4_68K
#if !__option(IEEEdoubles)
#error Cannot build MetaKit with 10-byte doubles
#endif
#endif

#if __option(bool)
#define q4_BOOL 1
    // undo previous defaults, because q4_BOOL is not set early enough
#undef false
#undef true
#undef bool
#endif

#undef _MSC_VER

#pragma export on

/////////////////////////////////////////////////////////////////////////////
