//****************************************************************************
//
// UWRITE4.S - Routine to write a possibly unaligned word.
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
// Write a possibly unaligned word.
//
//****************************************************************************
    _EXPORT_ __rt_uwrite4
__rt_uwrite4 _LABEL_
    tst     r1, _CONST_ 3
    streq   r0, [r1]
    moveq   pc, r14
    strb    r0, [r1]
    mov     r0, r0, lsr _CONST_ 8
    strb    r0, [r1, _CONST_ 1]
    mov     r0, r0, lsr _CONST_ 8
    strb    r0, [r1, _CONST_ 2]
    mov     r0, r0, lsr _CONST_ 8
    strb    r0, [r1, _CONST_ 3]
    mov     pc, r14

    _END_
