//==========================================================================
//
//      edb7xxx_flash.c
//
//      Flash programming
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-26
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>

#define  _FLASH_PRIVATE_
#include <cyg/io/flash.h>

#include "flash.h"

#define _si(p) ((p[0]<<8)|p[1])

int
flash_hwr_init(void)
{
    unsigned short data[4];
    extern char flash_query, flash_query_end;
    typedef int code_fun(unsigned char *);
    code_fun *_flash_query;
    int code_len, stat, num_regions, region_size;

    // Copy 'program' code to RAM for execution
    code_len = (unsigned long)&flash_query_end - (unsigned long)&flash_query;
    _flash_query = (code_fun *)flash_info.work_space;
    memcpy(_flash_query, &flash_query, code_len);
    HAL_DCACHE_SYNC();  // Should guarantee this code will run

#if 0
    {
    volatile int cache_test;
    volatile int *cp = 0xE0000000;
    // See if the data cache is working
    *cp = 0x12345678;
    printf("cache test = %x\n", *cp);
    HAL_DCACHE_DISABLE();
    *cp = 0xDEADDEAD;
    printf("cache test = %x\n", *cp);
    HAL_DCACHE_ENABLE();
    printf("cache test = %x\n", *cp);
    }
#endif

    stat = (*_flash_query)(data);
#if 0
    printf("stat = %x\n", stat);
    dump_buf(data, sizeof(data));
#endif

    if (data[0] != FLASH_Intel_code) {
        printf("Not Intel = %x\n", data[0]);
        return FLASH_ERR_HWR;
    }

    if (data[1] == (unsigned short)0x8897) {
        num_regions = 64*2;
        region_size = 0x20000;
    } else {
        printf("Unknown device type: %x\n", data[1]);
        return FLASH_ERR_HWR;
    }

    // Hard wired for now
    flash_info.block_size = region_size;
    flash_info.blocks = num_regions;
    flash_info.start = (void *)0xE0000000;
    flash_info.end = (void *)(0xE0000000+(num_regions*region_size));
    return FLASH_ERR_OK;
}

// Map a hardware status to a package error
int
flash_hwr_map_error(int err)
{
    if (err & 0x007E007E) {
        printf("Err = %x\n", err);
        if (err & 0x00100010) {
            return FLASH_ERR_PROGRAM;
        } else 
        if (err & 0x00200020) {
            return FLASH_ERR_ERASE;
        } else 
        return FLASH_ERR_HWR;  // FIXME
    } else {
        return FLASH_ERR_OK;
    }
}

// See if a range of FLASH addresses overlaps currently running code
bool
flash_code_overlaps(void *start, void *end)
{
    extern char _stext, _etext;

    return ((((unsigned long)&_stext >= (unsigned long)start) &&
             ((unsigned long)&_stext < (unsigned long)end)) ||
            (((unsigned long)&_etext >= (unsigned long)start) &&
             ((unsigned long)&_etext < (unsigned long)end)));
}
