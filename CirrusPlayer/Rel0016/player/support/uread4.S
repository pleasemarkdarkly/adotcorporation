//****************************************************************************
//
// UREAD4.S - Routine to read a possibly unaligned word.
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
// Read a possibly unaligned word.
//
//****************************************************************************
    _EXPORT_ __rt_uread4
__rt_uread4 _LABEL_
    tst     r0, _CONST_ 3
    ldreq   r0, [r0]
    moveq   pc, lr
    ldrb    r1, [r0, _CONST_ 1]
    ldrb    r2, [r0, _CONST_ 2]
    ldrb    r3, [r0, _CONST_ 3]
    ldrb    r0, [r0]
    orr     r0, r0, r1, lsl _CONST_ 8
    orr     r0, r0, r2, lsl _CONST_ 16
    orr     r0, r0, r3, lsl _CONST_ 24
    mov     pc, r14

    _END_
