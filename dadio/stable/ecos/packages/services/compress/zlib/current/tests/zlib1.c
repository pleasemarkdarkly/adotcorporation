//=================================================================
//
//        zlib1.c
//
//        zlib compression/decompression test 1
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          2001-03-09
// Description:   Tests zlib compress/decompress functionality.
//####DESCRIPTIONEND####


#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_KERNEL_API_C

#include <cyg/compress/zlib.h>
#include <stdlib.h>

#define NTHREADS 1
#define STACKSIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL > 8192 ? CYGNUM_HAL_STACK_SIZE_TYPICAL : 8192)

static cyg_handle_t thread[NTHREADS];

static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];

static char license_txt[] =
"Red Hat eCos Public License v1.1


1. DEFINITIONS

1.1. \"Contributor\" means each entity that creates or
contributes to the creation of Modifications.

1.2. \"Contributor Version\" means the combination of the
Original Code, prior Modifications used by a
Contributor, and the Modifications made by that
particular Contributor.

1.3. \"Covered Code\" means the Original Code or
Modifications or the combination of the Original Code
and Modifications, in each case including portions
thereof.

1.4. \"Electronic Distribution Mechanism\" means a
mechanism generally accepted in the software development
community for the electronic transfer of data.

1.5. \"Executable\" means Covered Code in any form other
than Source Code.
1.6. \"Initial Developer\" means the individual or entity
identified as the Initial Developer in the Source Code
notice required by Exhibit A.

1.7. \"Larger Work\" means a work which combines Covered
Code or portions thereof with code not governed by the
terms of this License.

1.8. \"License\" means this document.

1.9. \"Modifications\" means any addition to or deletion
from the substance or structure of either the Original
Code or any previous Modifications. When Covered Code is
released as a series of files, a Modification is:

     A. Any addition to or deletion from the
     contents of a file containing Original Code or
     previous Modifications.

     B. Any new file that contains any part of the
     Original Code or previous Modifications.

1.10. \"Original Code\" means Source Code of computer
software code which is described in the Source Code
notice required by Exhibit A as Original Code, and
which, at the time of its release under this License is
not already Covered Code governed by this License.

1.11. \"Source Code\" means the preferred form of the
Covered Code for making modifications to it, including
all modules it contains, plus any associated interface
definition files, scripts used to control compilation
and installation of an Executable, or a list of source
code differential comparisons against either the
Original Code or another well known, available Covered
Code of the Contributor's choice. The Source Code can be
in a compressed or archival form, provided the
appropriate decompression or de-archiving software is
widely available for no charge.

1.12. \"You\" means an individual or a legal entity
exercising rights under, and complying with all of the
terms of, this License or a future version of this
License issued under Section 6.1. For legal entities,
\"You\" includes any entity which controls, is controlled
by, or is under common control with You. For purposes of
this definition, \"control\" means (a) the power, direct
or indirect, to cause the direction or management of
such entity, whether by contract or otherwise, or (b)
ownership of fifty percent (50%) or more of the
outstanding shares or beneficial ownership of such
entity.

1.13. \"Red Hat Branded Code\" is code that Red Hat
distributes and/or permits others to distribute under
different terms than the Red Hat eCos Public License.
Red Hat's Branded Code may contain part or all of the
Covered Code.
";

static void entry0( cyg_addrword_t data )
{
    int i;
    unsigned long len;
    int err;
    int buf_size = sizeof(license_txt)+512;
    unsigned char* packed = malloc(buf_size);
    unsigned char* unpacked = malloc(buf_size);
    
    if (NULL == packed || NULL == unpacked)
        CYG_TEST_NA("Not enough memory for buffers");

    CYG_TEST_INFO("Compressing");

    len = buf_size;
    err = compress(packed, &len, license_txt, sizeof(license_txt));

    if (Z_OK != err)
        CYG_TEST_NA("Not enough memory for compression");


    CYG_TEST_INFO("Decompressing");
    err = uncompress(unpacked, &buf_size, packed, len);

    switch (err) {
    case Z_OK:
        break;
    case Z_MEM_ERROR:
        CYG_TEST_NA("Not enough memory for decompression");
        break;
    case Z_BUF_ERROR:
        CYG_TEST_FAIL_FINISH("Decompressed data larger than original");
        break;
    case Z_DATA_ERROR:
        CYG_TEST_FAIL_FINISH("Decompression failed");
        break;
    default:
        CYG_TEST_FAIL_FINISH("Unknown decompression error");
        break;
    }

    for (i = 0; i < sizeof(license_txt)-1; i++) {
        if (license_txt[i] != unpacked[i])
            CYG_TEST_FAIL_FINISH("Verify failed");
    }

    CYG_TEST_PASS_FINISH("zlib1 OK");
}

void zlib1_main( void )
{
    CYG_TEST_INIT();

    cyg_thread_create(4, entry0 , (cyg_addrword_t)0, "zlib1",
        (void *)stack[0], STACKSIZE,&thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    zlib1_main();
}

#else /* def CYGFUN_KERNEL_API_C */
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel C API layer disabled");
}
#endif /* def CYGFUN_KERNEL_API_C */

// EOF zlib1.c
