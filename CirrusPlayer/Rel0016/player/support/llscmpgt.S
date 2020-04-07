//****************************************************************************
//
// LLSCMPGT.S - Routine to compare two signed 64-bit values to see if one is
//              greater than the other.
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
// Compare two signed 64-bit values to see if one is greater than the other.
//
//****************************************************************************
    _EXPORT_ _ll_scmpgt
_ll_scmpgt _LABEL_
    cmp     r1, r3
    movgt   r0, _CONST_ 1
    movlt   r0, _CONST_ 0
    movne   pc, r14
    cmp     r0, r2
    movhi   r0, _CONST_ 1
    movls   r0, _CONST_ 0
    mov     pc, r14

    _END_
