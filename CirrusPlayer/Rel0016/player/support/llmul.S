//****************************************************************************
//
// LLMUL.S - Routines to multiply two 64-bit values, and to multiply two
//           bit values producing a 64-bit value.
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
// Multiply two unsigned 32-bit values, producing a 64-bit value.
//
//****************************************************************************
    _EXPORT_ _ll_muluu
_ll_muluu _LABEL_
    mov     r3, _CONST_ 0
    mov     r2, r1
    mov     r1, _CONST_ 0
    b       _ll_mul

//****************************************************************************
//
// Multiply two 64-bit values.
//
//****************************************************************************
    _EXPORT_ _ll_mul
_ll_mul _LABEL_
    stmdb   r13!, {r4-r6, r14}
    mov     r14, r0
    mov     r5, r14, lsr _CONST_ 16
    bic     r4, r14, r5, lsl _CONST_ 16
    mov     r12, r2, lsr _CONST_ 16
    bic     r6, r2, r12, lsl _CONST_ 16
    mul     r0, r4, r6
    mul     r4, r12, r4
    mul     r12, r5, r12
    mul     r5, r6, r5
    adds    r0, r0, r4, lsl _CONST_ 16
    adc     r12, r12, r4, lsr _CONST_ 16
    adds    r0, r0, r5, lsl _CONST_ 16
    adc     r12, r12, r5, lsr _CONST_ 16
    mla     r1, r2, r1, r12
    mla     r1, r3, r14, r1
    ldmia   r13!, {r4-r6, pc}

    _END_
