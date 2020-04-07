//****************************************************************************
//
// SDIV10.S - Routine to divide a signed number by 10.
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
// 32-bit signed division by 10.
//
//****************************************************************************
    _EXPORT_ __rt_sdiv10
__rt_sdiv10 _LABEL_
    movs    r3, r0
    rsbmi   r0, r0, _CONST_ 0
    sub     r1, r0, _CONST_ 10
    sub     r0, r0, r0, lsr _CONST_ 2
    add     r0, r0, r0, lsr _CONST_ 4
    add     r0, r0, r0, lsr _CONST_ 8
    add     r0, r0, r0, lsr _CONST_ 16
    mov     r0, r0, lsr _CONST_ 3
    add     r2, r0, r0, lsl _CONST_ 2
    subs    r1, r1, r2, lsl _CONST_ 1
    addpl   r0, r0, _CONST_ 1
    addmi   r1, r1, _CONST_ 10
    movs    r3, r3
    rsbmi   r0, r0, _CONST_ 0
    rsbmi   r1, r1, _CONST_ 0
    mov     pc, r14

    _END_
