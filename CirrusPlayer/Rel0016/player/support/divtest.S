//****************************************************************************
//
// DIVTEST.S - Routine to perform a division test.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

#include "../../asmdefs.h"

//****************************************************************************
//
// Read-only code area.
//
//****************************************************************************
    _TEXT_

//****************************************************************************
//
// Division test.  This is used in cases where a division is performed but the
// result is never used in the code.  Therefore, simply do not perform any
// test.
//
//****************************************************************************
    _EXPORT_ __rt_divtest
__rt_divtest _LABEL_
    mov     pc, r14

    _END_
