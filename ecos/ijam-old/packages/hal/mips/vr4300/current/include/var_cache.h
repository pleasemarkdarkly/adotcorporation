#ifndef CYGONCE_VAR_CACHE_H
#define CYGONCE_VAR_CACHE_H

//=============================================================================
//
//      var_cache.h
//
//      HAL cache control API
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:        nickg
// Date:        1998-02-17
// Purpose:     Cache control API
// Description: The macros defined here provide the HAL APIs for handling
//              cache control operations.
// Usage:
//              #include <cyg/hal/var_cache.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/hal/plf_cache.h>

//-----------------------------------------------------------------------------
// Cache sizes.

// Data cache
#define HAL_DCACHE_SIZE                 (8*1024)        // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            16              // Size of a data cache line
#define HAL_DCACHE_WAYS                 1               // Associativity of the cache

// Instruction cache
#define HAL_ICACHE_SIZE                 (16*1024)       // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            32              // Size of a cache line
#define HAL_ICACHE_WAYS                 1               // Associativity of the cache

#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))
#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

//-----------------------------------------------------------------------------
// The VR4300 mostly uses the default MIPS cache controls defined in hal_cache.h
// Here we define the cache enable and disable macros. The only control we appear
// to have is the kseg0 cache state in config0. So all these macros at present
// manipulate this.

#ifndef HAL_DCACHE_ENABLE_DEFINED
#define HAL_DCACHE_ENABLE()                     \
CYG_MACRO_START                                 \
    asm volatile ( "mfc0   $2,$16\n"            \
                   "nop; nop; nop\n"            \
                   "la     $3,0xFFFFFFF8\n"     \
                   "and    $2,$2,$3\n"          \
                   "ori    $2,$2,3\n"           \
                   "mtc0   $2,$16\n"            \
                   "nop; nop; nop;\n"           \
                   :                            \
                   :                            \
                   : "$2", "$3"                 \
                   );                           \
CYG_MACRO_END
#define HAL_DCACHE_ENABLE_DEFINED
#endif

// Disable the data cache
#ifndef HAL_DCACHE_DISABLE_DEFINED
#define HAL_DCACHE_DISABLE()                    \
CYG_MACRO_START                                 \
    asm volatile ( "mfc0   $2,$16\n"            \
                   "nop; nop; nop\n"            \
                   "la     $3,0xFFFFFFF8\n"     \
                   "and    $2,$2,$3\n"          \
                   "ori    $2,$2,2\n"           \
                   "mtc0   $2,$16\n"            \
                   "nop; nop; nop;\n"           \
                   :                            \
                   :                            \
                   : "$2", "$3"                 \
                   );                           \
CYG_MACRO_END
#define HAL_DCACHE_DISABLE_DEFINED
#endif

#ifndef HAL_DCACHE_IS_ENABLED_DEFINED
#define HAL_DCACHE_IS_ENABLED(_state_)          \
CYG_MACRO_START                                 \
    CYG_WORD32 _cstate_;                        \
    asm volatile ( "mfc0   %0,$16\n"            \
                   : "=r"(_cstate_)             \
                   );                           \
    if( (_cstate_ & 7) == 2 ) _state_ = 0;      \
    else _state_ = 1;                           \
CYG_MACRO_END
#define HAL_DCACHE_IS_ENABLED_DEFINED
#endif

#ifndef HAL_ICACHE_ENABLE_DEFINED
#define HAL_ICACHE_ENABLE() HAL_DCACHE_ENABLE()
#define HAL_ICACHE_ENABLE_DEFINED
#endif

// Disable the instruction cache
#ifndef HAL_ICACHE_DISABLE_DEFINED
#define HAL_ICACHE_DISABLE() HAL_DCACHE_DISABLE()
#define HAL_ICACHE_DISABLE_DEFINED
#endif

#ifndef HAL_ICACHE_IS_ENABLED_DEFINED
#define HAL_ICACHE_IS_ENABLED(_state_) HAL_DCACHE_IS_ENABLED(_state_)
#define HAL_ICACHE_IS_ENABLED_DEFINED
#endif

//-----------------------------------------------------------------------------
// The VR4300 has no cache locking facility so we define the guard macros
// to disable the definitions in hal_arch.h.

#define HAL_DCACHE_LOCK_DEFINED
#define HAL_DCACHE_UNLOCK_DEFINED
#define HAL_DCACHE_UNLOCK_ALL_DEFINED

#define HAL_ICACHE_LOCK_DEFINED
#define HAL_ICACHE_UNLOCK_DEFINED

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_VAR_CACHE_H
// End of var_cache.h