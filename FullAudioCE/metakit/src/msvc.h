//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  Specific header for the Microsoft Visual C++ development tools.

#define q4_MSVC 1

    // get rid of several common warning messages
#if !q4_STRICT
#pragma warning(disable: 4244) // conversion ..., possible loss of data
#pragma warning(disable: 4135) // conversion between diff. integral types
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4511) // copy constructor could not be generated
#pragma warning(disable: 4512) // assignment op could not be generated
#pragma warning(disable: 4514) // unreferenced inline removed
#endif

#if _MSC_VER >= 1100
#define q4_BOOL 1           // 5.0 supports the bool datatype
#else
#define q4_NO_NS 1          // 4.x doesn't use namespaces for STL
#endif

#if defined (_MT)
#define q4_MULTI 1          // uses multi-threading
#endif

#if !q4_STD && !q4_UNIV && !defined (q4_MFC)
#define d4_FW_H "mfc.h"     // default for MSVC is to use MFC
#endif
