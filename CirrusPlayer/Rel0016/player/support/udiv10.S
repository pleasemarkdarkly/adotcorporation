//****************************************************************************
//
// UDIV10.S - Routine to divide an unsigned number by 10.
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
// 32-bit unsigned division by 10.
//
//****************************************************************************
    _EXPORT_ __rt_udiv10
__rt_udiv10 _LABEL_
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
    mov     pc, r14

    _END_
