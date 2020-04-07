//****************************************************************************
//
// UDIV.S - Routine to divide two 32-bit unsigned numbers.
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
// 32-bit unsigned division.
//
//****************************************************************************
    _EXPORT_ __udivsi3
__udivsi3 _LABEL_
    mov     r2, r0
    mov     r0, r1
    mov     r1, r2
    _EXPORT_ __rt_udiv
__rt_udiv _LABEL_
    mov     r2, _CONST_ 0
    rsbs    r12, r0, r1, lsr _CONST_ 3
    blo     udiv03
    rsbs    r12, r0, r1, lsr _CONST_ 8
    blo     udiv01
    mov     r0, r0, lsl _CONST_ 8
    orr     r2, r2, _CONST_ 255 << 24
    rsbs    r12, r0, r1, lsr _CONST_ 4
    blo     udiv02
    rsbs    r12, r0, r1, lsr _CONST_ 8
    blo     udiv01
    mov     r0, r0, lsl _CONST_ 8
    orr     r2, r2, _CONST_ 255 << 16
    rsbs    r12, r0, r1, lsr _CONST_ 8
    movcs   r0, r0, lsl _CONST_ 8
    orrcs   r2, r2, _CONST_ 255 << 8
    rsbs    r12, r0, r1, lsr _CONST_ 4
    blo     udiv02
    rsbs    r12, r0, _CONST_ 0
    movcs   pc, r14
udiv00 _LABEL_
    movcs   r0, r0, lsr _CONST_ 8
udiv01 _LABEL_
    rsbs    r12, r0, r1, lsr _CONST_ 7
    subcs   r1, r1, r0, lsl _CONST_ 7
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr _CONST_ 6
    subcs   r1, r1, r0, lsl _CONST_ 6
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr _CONST_ 5
    subcs   r1, r1, r0, lsl _CONST_ 5
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr _CONST_ 4
    subcs   r1, r1, r0, lsl _CONST_ 4
    adc     r2, r2, r2
udiv02 _LABEL_
    rsbs    r12, r0, r1, lsr _CONST_ 3
    subcs   r1, r1, r0, lsl _CONST_ 3
    adc     r2, r2, r2
udiv03 _LABEL_
    rsbs    r12, r0, r1, lsr _CONST_ 2
    subcs   r1, r1, r0, lsl _CONST_ 2
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr _CONST_ 1
    subcs   r1, r1, r0, lsl _CONST_ 1
    adc     r2, r2, r2
    rsbs    r12, r0, r1
    subcs   r1, r1, r0
    adcs    r2, r2, r2
    bcs     udiv00
    mov     r0, r2
    mov     pc, r14
    _EXPORT_ __umodsi3
__umodsi3 _LABEL_
    stmdb   r13!, {lr}
    bl      __udivsi3
    mov     r0, r1
    ldmia   r13!, {pc}

    _END_
