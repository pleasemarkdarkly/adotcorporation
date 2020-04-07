//****************************************************************************
//
// LLUDIV.S - Routine to divide two unsigned 64-bit values.
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
// 64-bit unsigned division.
//
//****************************************************************************
    _EXPORT_ _ll_udiv
_ll_udiv _LABEL_
    stmdb   r13!, {r4-r6, r14}
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2
    mov     r14, r3
__ll_udiv1 _LABEL_
    mov     r0, _CONST_ 0
    mov     r1, _CONST_ 0
    mov     r3, r5
    mov     r2, r4
    teq     r14, _CONST_ 0
    teqeq   r6, _CONST_ 0
    beq     _ll_udiv03
    movs    r12, _CONST_ 0
_ll_udiv00 _LABEL_
    adds    r6, r6, r6
    adcs    r14, r14, r14
    bcs     _ll_udiv01
    cmp     r14, r3
    cmpeq   r6, r2
    addls   r12, r12, _CONST_ 1
    bls     _ll_udiv00
    adds    r12, r12, _CONST_ 0
_ll_udiv01 _LABEL_
    movs    r14, r14, rrx
    mov     r6, r6, rrx
_ll_udiv02 _LABEL_
    subs    r4, r2, r6
    sbcs    r5, r3, r14
    movcs   r3, r5
    movcs   r2, r4
    adcs    r0, r0, r0
    adc     r1, r1, r1
    movs    r14, r14, lsr _CONST_ 1
    mov     r6, r6, rrx
    subs    r12, r12, _CONST_ 1
    bge     _ll_udiv02
_ll_udiv03 _LABEL_
    ldmia   r13!, {r4-r6, pc}

    _END_
