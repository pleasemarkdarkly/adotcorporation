//===========================================================================
//
//      atan2.c
//
//      Test of atan2() math library function
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
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE


// CONFIGURATION

#include <pkgconf/libm.h>   // Configuration header


// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/testcase.h>    // Test infrastructure
#include <math.h>                  // Header for this package
#include <sys/ieeefp.h>            // Cyg_libm_ieee_double_shape_type
#include "vectors/vector_support.h"// extra support for math tests

#include "vectors/atan2.h"

// FUNCTIONS

static void
test( CYG_ADDRWORD data )
{
    cyg_ucount32 vec_size;
    cyg_bool ret;

    vec_size = sizeof(atan2_vec) / sizeof(Cyg_libm_test_double_vec_t);
    ret = doTestVec( (CYG_ADDRWORD) &atan2, CYG_LIBM_TEST_VEC_DOUBLE,
                     CYG_LIBM_TEST_VEC_DOUBLE, CYG_LIBM_TEST_VEC_DOUBLE,
                     &atan2_vec[0], vec_size );

    if (ret==true)
    {
        CYG_TEST_PASS("atan2() is stable");
    } // if
    else
    {
        CYG_TEST_FAIL("atan2() failed tests");
    } // else

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for Math "
                    "library atan2() function");
} // test()


int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for Math library "
                  "atan2() function");

    START_TEST( test );

    CYG_TEST_PASS_FINISH("Testing is not applicable to this configuration");

} // main()


// EOF atan2.c
