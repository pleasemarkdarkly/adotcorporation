//===========================================================================
//
//      strcmpi.cxx
//
//      ANSI standard strcmpi() routine
//
//===========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-14
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_string.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <string.h>                // Header for this file
#include <stddef.h>         // Compiler definitions such as size_t, NULL etc.
#include <cyg/libc/string/stringsupp.hxx> // Useful string function support and
                                          // prototypes

// EXPORTED SYMBOLS

externC int
strcmpi( const char *s1, const char *s2 ) CYGBLD_ATTRIB_WEAK_ALIAS(__strcmpi);

// FUNCTIONS

#define __tolower(a) (a>='A'&&a<='Z'?a-'A'+'a':a)

int
__strcmpi( const char *s1, const char *s2 )
{ 
    CYG_REPORT_FUNCNAMETYPE( "__strcmpi", "returning %d" );
    CYG_REPORT_FUNCARG2( "s1=%08x, s2=%08x", s1, s2 );

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    int retval;

    CYG_UNUSED_PARAM( int, retval ); // in case tracing is off

    while (*s1 != '\0' && __tolower(*s1) == __tolower(*s2))
    {
        s1++;
        s2++;
    }

    retval = (*(unsigned char *) s1) - (*(unsigned char *) s2);

    CYG_REPORT_RETVAL( retval );

    return retval;
} // __strcmpi()

// EOF strcmpi.cxx
