//****************************************************************************
//
// _F2D.S - Routine to convert a float to a double.
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
// Convert a float to a double.
//
//****************************************************************************
    _EXPORT_ _f2d
_f2d _LABEL_
    add     r12, r0, _CONST_ 0x00800000
    tst     r12, _CONST_ 0x7f000000
    beq     _f2d_uncommon
    mov     r1, r0, lsl _CONST_ 29
    movs    r0, r0, asr _CONST_ 3
    add     r0, r0, _CONST_ 0x38000000
    movpl   pc, r14
    sub     r0, r0, _CONST_ 0x70000000
    mov     pc, r14
_f2d_uncommon _LABEL_
    tst     r12, _CONST_ 0x00800000
    beq     _f2d_Inf_or_NaN
_f2d_denorm _LABEL_
    movs    r1, r0, lsl _CONST_ 1
    moveq   pc, r14
    and     r2, r0, _CONST_ 0x80000000
    add     r3, r2, _CONST_ 0x36800000
    mov     r0, r0, lsl _CONST_ 3
    movs    r1, r0, lsr _CONST_ 16
    addne   r3, r3, _CONST_ 0x01000000
    moveqs  r0, r0, lsl _CONST_ 16
    moveq   pc, r14
    tst     r0, _CONST_ 0xff000000
    addne   r3, r3, _CONST_ 0x00800000
    moveq   r0, r0, lsl _CONST_ 8
    tst     r0, _CONST_ 0xf0000000
    addne   r3, r3, _CONST_ 0x00400000
    moveq   r0, r0, lsl _CONST_ 4
    tst     r0, _CONST_ 0xc0000000
    addne   r3, r3, _CONST_ 0x00200000
    moveqs  r0, r0, lsl _CONST_ 2
    addmi   r3, r3, _CONST_ 0x00100000
    movpl   r0, r0, lsl _CONST_ 1
    mov     r1, r0, lsl _CONST_ 21
    add     r0, r3, r0, ASR _CONST_ 11
    mov     pc, r14
_f2d_Inf_or_NaN _LABEL_
    movs    r1, r0, lsl _CONST_ 9
    orreq   r0, r0, _CONST_ 0x00700000
    moveq   pc, r14
    ldr     r0, =0x7ff80000
    mov     r1, _CONST_ 0
    mov     pc, r14

    _END_
