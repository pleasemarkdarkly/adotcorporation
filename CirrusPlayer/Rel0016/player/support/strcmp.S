//****************************************************************************
//
// STRCMP.S - Routine to implement the C library function "strcmp".
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
// Standard C library function "strcmp".
//
//****************************************************************************
    _EXPORT_ strcmp
strcmp _LABEL_
    tst     r0, _CONST_ 3
    tsteq   r1, _CONST_ 3
    bne     strcmp_byteloop
    str     r14, [r13, _CONST_ -4]!
    ldr     r14, =0x01010101
strcmp_loop _LABEL_
    ldr     r2, [r0], _CONST_ 4
    ldr     r3, [r1], _CONST_ 4
    sub     r12, r2, r14
    bic     r12, r12, r2
    ands    r12, r12, r14, lsl _CONST_ 7
    cmpeq   r2, r3
    beq     strcmp_loop
    ldr     r14, [r13], _CONST_ 4
    mov     r0, r3, lsl _CONST_ 24
    subs    r0, r0, r2, lsl _CONST_ 24
    tsteq   r12, _CONST_ 0x0FF0
    bne     strcmp_return
    mov     r0, r3, lsl _CONST_ 16
    subs    r0, r0, r2, lsl _CONST_ 16
    tsteq   r12, _CONST_ 0xFF00
    bne     strcmp_return
    mov     r0, r3, lsl _CONST_ 8
    subs    r0, r0, r2, lsl _CONST_ 8
    tsteq   r12, _CONST_ 0xFF0000
    subeqs  r0, r3, r2
strcmp_return _LABEL_
    movne   r0, r0, rrx
    mov     pc, r14
strcmp_byteloop _LABEL_
    ldrb    r2, [r0], _CONST_ 1
    ldrb    r3, [r1], _CONST_ 1
    cmp     r2, _CONST_ 1
    cmpcs   r2, r3
    bne     strcmp_byteend
    ldrb    r2, [r0], _CONST_ 1
    ldrb    r3, [r1], _CONST_ 1
    cmp     r2, _CONST_ 1
    cmpcs   r2, r3
    beq     strcmp_byteloop
strcmp_byteend _LABEL_
    sub     r0, r2, r3
    mov     pc, r14

    _END_
