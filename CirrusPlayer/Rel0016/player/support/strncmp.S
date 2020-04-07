//****************************************************************************
//
// STRNCMP.S - Routine to implement the C library function "strncmp".
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
// Standard C library function "strncmp".
//
//****************************************************************************
    _EXPORT_ strncmp
strncmp _LABEL_
    str     r14, [r13, _CONST_ -4]!
    orr     r3, r0, r1
    tst     r3, _CONST_ 3
    ldreq   r3, =0x01010101
    beq     strncmp01
    b       strncmp02
strncmp00 _LABEL_
    add     r0, r0, _CONST_ 4
    add     r1, r1, _CONST_ 4
    sub     r2, r2, _CONST_ 4
strncmp01 _LABEL_
    cmp     r2, _CONST_ 4
    ldrcs   r12, [r0]
    ldrcs   r14, [r1]
    cmpcs   r12, r14
    bne     strncmp02
    sub     r14, r12, r3
    bic     r12, r14, r12
    tst     r12, r3, lsl _CONST_ 7
    beq     strncmp00
strncmp02 _LABEL_
    cmp     r2, _CONST_ 0
    moveq   r0, _CONST_ 0
    ldreq   pc, [r13], _CONST_ 4
strncmp03 _LABEL_
    ldrb    r3, [r0], _CONST_ 1
    ldrb    r12, [r1], _CONST_ 1
    cmp     r3, _CONST_ 1
    cmpcs   r3, r12
    bne     strncmp04
    subs    r2, r2, _CONST_ 1
    bne     strncmp03
strncmp04 _LABEL_
    sub     r0, r3, r12
    ldr     pc, [r13], _CONST_ 4

    _END_
