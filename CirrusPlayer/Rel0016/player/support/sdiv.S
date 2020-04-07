//****************************************************************************
//
// SDIV.S - Routine to divide two 32-bit signed numbers.
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
// 32-bit signed division.
//
//****************************************************************************
    _EXPORT_ __divsi3
__divsi3 _LABEL_
    mov     r2, r0
    mov     r0, r1
    mov     r1, r2
    _EXPORT_ __rt_sdiv
__rt_sdiv _LABEL_
    ands    r2, r0, _CONST_ 1 << 31
    rsbmi   r0, r0, _CONST_ 0
    eors    r3, r2, r1, asr _CONST_ 32
    rsbcs   r1, r1, _CONST_ 0
    rsbs    r12, r0, r1, lsr _CONST_ 3
    blo     sdiv03
    rsbs    r12, r0, r1, lsr _CONST_ 8
    blo     sdiv01
    mov     r0, r0, lsl _CONST_ 8
    orr     r2, r2, _CONST_ 255 << 24
    rsbs    r12, r0, r1, lsr _CONST_ 4
    blo     sdiv02
    rsbs    r12, r0, r1, lsr _CONST_ 8
    blo     sdiv01
    mov     r0, r0, lsl _CONST_ 8
    orr     r2, r2, _CONST_ 255 << 16
    rsbs    r12, r0, r1, lsr _CONST_ 8
    movcs   r0, r0, lsl _CONST_ 8
    orrcs   r2, r2, _CONST_ 255 << 8
    rsbs    r12, r0, r1, lsr _CONST_ 4
    blo     sdiv02
    rsbs    r12, r0, _CONST_ 0
    movcs   pc, r14
sdiv00 _LABEL_
    movcs   r0, r0, lsr _CONST_ 8
sdiv01 _LABEL_
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
sdiv02 _LABEL_
    rsbs    r12, r0, r1, lsr _CONST_ 3
    subcs   r1, r1, r0, lsl _CONST_ 3
    adc     r2, r2, r2
sdiv03 _LABEL_
    rsbs    r12, r0, r1, lsr _CONST_ 2
    subcs   r1, r1, r0, lsl _CONST_ 2
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr _CONST_ 1
    subcs   r1, r1, r0, lsl _CONST_ 1
    adc     r2, r2, r2
    rsbs    r12, r0, r1
    subcs   r1, r1, r0
    adcs    r2, r2, r2
    bcs     sdiv00
    eors    r0, r2, r3, asr _CONST_ 31
    add     r0, r0, r3, lsr _CONST_ 31
    rsbcs   r1, r1, _CONST_ 0
    mov     pc, r14
    _EXPORT_ __modsi3
__modsi3 _LABEL_
    stmdb   r13!, {lr}
    bl      __divsi3
    mov     r0, r1
    ldmia   r13!, {pc}

    _END_
