//===========================================================================
//
//      mbstowcs.cxx
//
//      ISO standard mbstowcs() routine 
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.          
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jjohnstn
// Contributors:  jjohnstn
// Date:          2000-11-01
// Purpose:       Provide ISO C mbstowcs()
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================
//
// This code was based on newlib/libc/stdlib/mbstowcs.c and newlib/libc/stdlib/mbstowcs_r.c
// The following is the modified from the original newlib description:
//
/*
FUNCTION
<<mbstowcs>>---multibyte string to wide char converter

INDEX
	mbstowcs

ANSI_SYNOPSIS
	#include <stdlib.h>
	int mbstowcs(wchar_t *<[pwc]>, const char *<[s]>, size_t <[n]>);

TRAD_SYNOPSIS
	#include <stdlib.h>
	int mbstowcs(<[pwc]>, <[s]>, <[n]>)
	wchar_t *<[pwc]>;
	const char *<[s]>;
	size_t <[n]>;

DESCRIPTION
When CYGINT_LIBC_I18N_MB_REQUIRED is not defined, this is a minimal ANSI-conforming 
implementation of <<mbstowcs>>.  In this case, the
only ``multi-byte character sequences'' recognized are single bytes,
and they are ``converted'' to wide-char versions simply by byte
extension.

When CYGINT_LIBC_I18N_MB_REQUIRED is defined, this routine calls the LC_CTYPE locale mbtowc_fn
repeatedly to perform the conversion, passing a state variable to allow state dependent
decoding.  The result is based on the locale setting which may
be restricted to a defined set of locales.

RETURNS
This implementation of <<mbstowcs>> returns <<0>> if
<[s]> is <<NULL>> or is the empty string; 
it returns <<-1>> if CYGINT_LIBC_I18N_MB_REQUIRED and one of the
multi-byte characters is invalid or incomplete;
otherwise it returns the minimum of: <<n>> or the
number of multi-byte characters in <<s>> plus 1 (to
compensate for the nul character).
If the return value is -1, the state of the <<pwc>> string is
indeterminate.  If the input has a length of 0, the output
string will be modified to contain a wchar_t nul terminator.

PORTABILITY
<<mbstowcs>> is required in the ANSI C standard.  However, the precise
effects vary with the locale.

<<mbstowcs>> requires no supporting OS subroutines.
*/

// CONFIGURATION

#include <pkgconf/libc_i18n.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <locale.h>
#include <stdlib.h>                // Header for this file
#include <string.h>                // strcmp definition
#include <stddef.h>                // size_t definition
#include "internal.h"           // __current_ctype_locale

#ifdef CYGSEM_LIBC_I18N_PER_THREAD_MBSTOWCS
# include <pkgconf/kernel.h>       // kernel configuration
# include <cyg/kernel/thread.hxx>  // per-thread data
# include <cyg/kernel/thread.inl>  // per-thread data
# include <cyg/kernel/mutex.hxx>   // mutexes
#endif

// TRACE

#if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_LIBC_I18N_MBSTOWCS_TRACE_LEVEL)
static int mbstowcs_trace = CYGNUM_LIBC_I18N_MBSTOWCS_TRACE_LEVEL;
# define TL1 (0 < mbstowcs_trace)
#else
# define TL1 (0)
#endif

// STATICS

// FUNCTIONS

size_t
mbstowcs ( wchar_t *pwcs, const char *s, size_t n ) 
{
  size_t  retval;
  
  CYG_REPORT_FUNCNAMETYPE( "mbstowcs", "returning %ud" );
  CYG_REPORT_FUNCARG3( "pwcs=%08x, s=%08x, n=%ud", pwcs, s, n );

  if (pwcs != NULL)
    CYG_CHECK_DATA_PTR( pwcs, "pwcs is not a valid pointer!" );
  if (s != NULL)
    CYG_CHECK_DATA_PTR( s, "s is not a valid pointer!" );
  
#ifdef CYGINT_LIBC_I18N_MB_REQUIRED  

  CYG_TRACE2( TL1, "Retrieved mbstowcs_last address %08x containing %d",
	      state, *state );

  wchar_t *ptr = pwcs;
  size_t max = n;
  int state = 0;
  char *t = (char *)s;
  int bytes;
  int (*mbtowc_fn)(wchar_t *, const char *, size_t, int *) = __current_ctype_locale->mbtowc_fn;
  
  if (mbtowc_fn)
    {
      while (n > 0)
	{
	  bytes = mbtowc_fn (ptr, t, MB_CUR_MAX, &state);
	  if (bytes == -1)
	    {
	      retval = (size_t)-1;
	      CYG_REPORT_RETVAL( retval );
	      return retval;
	    }
	  else if (bytes == 0)
	    {
	      retval = ptr - pwcs;
	      CYG_REPORT_RETVAL( retval );
	      return retval;
	    }
	  t += bytes;
	  ++ptr;
	  --n;
	}
      
      retval = max;
      CYG_REPORT_RETVAL( retval );
      return retval;
    }
  
#endif /* CYGINT_LIBC_I18N_MB_REQUIRED */
  
  int count = 0;
  
  if (n != 0) {
    do {
      if ((*pwcs++ = (wchar_t) *s++) == 0)
	break;
      count++;
    } while (--n != 0);
  }
  
  retval = count;
  CYG_REPORT_RETVAL( retval );
  return retval;
} // mbstowcs()

// EOF mbstowcs.cxx
