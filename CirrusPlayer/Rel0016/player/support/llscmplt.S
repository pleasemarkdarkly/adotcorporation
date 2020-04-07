//****************************************************************************
//
// LLSCMPLT.S - Routine to compare two signed 64-bit values to see if one is
//              less than the other.
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
// Compare two signed 64-bit values to see if one is less than the other.
//
//****************************************************************************
    _EXPORT_ _ll_scmplt
_ll_scmplt _LABEL_
    cmp     r1, r3
    movlt   r0, _CONST_ 1
    movgt   r0, _CONST_ 0
    movne   pc, r14
    cmp     r0, r2
    movlo   r0, _CONST_ 1
    movhs   r0, _CONST_ 0
    mov     pc, r14

    _END_
