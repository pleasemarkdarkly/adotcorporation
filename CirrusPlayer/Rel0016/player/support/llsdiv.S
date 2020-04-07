//****************************************************************************
//
// LLSDIV.S - Routine to divide two signed 64-bit values.
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
// 64-bit signed division.
//
//****************************************************************************
    _EXPORT_ _ll_srdv
_ll_srdv _LABEL_
    stmdb   r13!, {r4-r7, r14}
    mov     r4, r2
    mov     r5, r3
    mov     r6, r0
    mov     r7, r1
    b       __ll_sdiv1
    _EXPORT_ _ll_sdiv
_ll_sdiv _LABEL_
    stmdb   r13!, {r4-r7, r14}
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2
    mov     r7, r3
__ll_sdiv1 _LABEL_
    ands    r12, r7, _CONST_ 1 << 31
    bpl     _ll_sdiv00
    rsbs    r6, r6, _CONST_ 0
    rsc     r7, r7, _CONST_ 0
_ll_sdiv00 _LABEL_
    eors    r12, r12, r5, asr _CONST_ 32
    bcc     _ll_sdiv01
    rsbs    r4, r4, _CONST_ 0
    rsc     r5, r5, _CONST_ 0
_ll_sdiv01 _LABEL_
    mov     r0, _CONST_ 0
    mov     r1, _CONST_ 0
    mov     r3, r5
    mov     r2, r4
    teq     r7, _CONST_ 0
    teqeq   r6, _CONST_ 0
    beq     _ll_sdiv05
    movs    r14, _CONST_ 0
_ll_sdiv02 _LABEL_
    adds    r6, r6, r6
    adcs    r7, r7, r7
    bcs     _ll_sdiv03
    cmp     r7, r3
    cmpeq   r6, r2
    addls   r14, r14, _CONST_ 1
    bls     _ll_sdiv02
    adds    r14, r14, _CONST_ 0
_ll_sdiv03 _LABEL_
    movs    r7, r7, rrx
    mov     r6, r6, rrx
_ll_sdiv04 _LABEL_
    subs    r4, r2, r6
    sbcs    r5, r3, r7
    movcs   r3, r5
    movcs   r2, r4
    adcs    r0, r0, r0
    adc     r1, r1, r1
    movs    r7, r7, lsr _CONST_ 1
    mov     r6, r6, rrx
    subs    r14, r14, _CONST_ 1
    bge     _ll_sdiv04
_ll_sdiv05 _LABEL_
    movs    r12, r12, lsl _CONST_ 1
    bcc     _ll_sdiv06
    rsbs    r0, r0, _CONST_ 0
    rsc     r1, r1, _CONST_ 0
_ll_sdiv06 _LABEL_
    movs    r12, r12, lsl _CONST_ 1
    bcc     _ll_sdiv07
    rsbs    r2, r2, _CONST_ 0
    rsc     r3, r3, _CONST_ 0
_ll_sdiv07 _LABEL_
    ldmia   r13!, {r4-r7, pc}

    _END_
