//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  Specific header for all flavors of Windows and DOS.

#if defined (_MSDOS)
#define q4_DOS 1
#endif

#if defined (_WINDOWS)
#define q4_WIN 1
#endif

#if defined (_WIN32)
#define q4_WIN32 1
#endif

#if q4_WIN32                    // WIN32 implies WIN
#undef q4_WIN
#define q4_WIN 1
#endif

#if q4_WIN                      // WIN implies not DOS, even for Win3
#undef q4_DOS
#endif
