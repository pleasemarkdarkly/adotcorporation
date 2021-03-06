/*==========================================================================
//
//	sh.ld
//
//	Linker script for SH
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	jskov
// Contributors:jskov
// Date:	1999-04-24
// Purpose:	SH Linker script
//
//####DESCRIPTIONEND####
//
//========================================================================*/

STARTUP(vectors.o)
ENTRY(__reset)
#ifdef EXTRAS
INPUT(extras.o)
#endif
GROUP(libtarget.a libgcc.a)

#define ALIGN_LMA 16
#define FOLLOWING(_section_) AT ((LOADADDR (_section_) + SIZEOF (_section_) + ALIGN_LMA - 1) & ~ (ALIGN_LMA - 1))
#define LMA_EQ_VMA
#define FORCE_OUTPUT . = .


#define SECTIONS_BEGIN

#define SECTION_vectors(_region_, _vma_, _lma_)        \
  .vectors _vma_ : _lma_                                              \
  { FORCE_OUTPUT; KEEP(*(.vectors)) }         \
  > _region_                                   \
  __vector_code_lma = LOADADDR(.vectors);

#define SECTION_text(_region_, _vma_, _lma_)           \
  .text _vma_ : _lma_                                                 \
  { __stext = .;       \
    *(.text*) *(.gnu.warning) *(.gnu.linkonce*) *(.init) }      \
  > _region_                                                    \
  __etext = .;  PROVIDE (__etext = .);

#define SECTION_fini(_region_, _vma_, _lma_)   \
    .fini _vma_ : _lma_                                       \
    { FORCE_OUTPUT; *(.fini) }        \
    > _region_

#define SECTION_rodata1(_region_, _vma_, _lma_)        \
    .rodata1 _vma_ : _lma_                                            \
    { FORCE_OUTPUT; *(.rodata1) }             \
    > _region_

#define SECTION_rodata(_region_, _vma_, _lma_) \
    .rodata _vma_ : _lma_                                     \
    { FORCE_OUTPUT; *(.rodata*) }     \
    > _region_

#define SECTION_fixup(_region_, _vma_, _lma_)                      \
    .fixup _vma_ : _lma_                                                          \
    { __FIXUP_START__ = ABSOLUTE(.); *(.fixup) __FIXUP_END__ = ABSOLUTE(.);}\
    > _region_

#define SECTION_gcc_except_table(_region_, _vma_, _lma_)       \
    .gcc_except_table _vma_ : _lma_                                           \
    { __EXCEPT_START__ = ABSOLUTE(.); *(.gcc_except_table)              \
      __EXCEPT_END__ = ABSOLUTE(.);}                                    \
    > _region_

#define SECTION_data(_region_, _vma_, _lma_)                                  \
    .data _vma_ : _lma_                                                       \
    { __ram_data_start = ABSOLUTE(.); *(.data*)                               \
     . = ALIGN(8); ___CTOR_LIST__ = ABSOLUTE(.);                              \
      KEEP(*(SORT(.ctors*))) ___CTOR_END__ = ABSOLUTE(.);                     \
    ___DTOR_LIST__ = ABSOLUTE(.);                                             \
      KEEP(*(SORT(.dtors*))) ___DTOR_END__ = ABSOLUTE(.);                     \
    . = ALIGN(8);                                                             \
      KEEP(*( SORT (.ecos.table.*))) ;                                        \
    } > _region_                                                              \
    __rom_data_start = LOADADDR(.data);                                       \
    __ram_data_end = .; PROVIDE(__ram_data_end = .);                          \
    _edata = .; PROVIDE (edata = .);

#define SECTION_bss(_region_, _vma_, _lma_)                      \
    .bss _vma_ : _lma_                                                          \
    { __bss_start = ABSOLUTE (.);                       \
      FORCE_OUTPUT; *(.dynbss) *(.bss) *(COMMON)        \
      __bss_end = ABSOLUTE (.); }                       \
    > _region_

#define SECTIONS_END . = ALIGN(4); _end = .; PROVIDE (end = .);

#include <pkgconf/system.h>
#include CYGHWR_MEMORY_LAYOUT_LDI
#include CYGBLD_HAL_PLATFORM_H

// Define VSR and vector tables to reside at fixed address.
_hal_vsr_table = CYGHWR_HAL_VSR_TABLE;
_hal_virtual_vector_table = CYGHWR_HAL_VECTOR_TABLE;

