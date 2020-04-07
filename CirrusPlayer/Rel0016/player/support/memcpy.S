//****************************************************************************
//
// MEMCPY.S - Routine to implement the C library function "memcpy".
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
// Standard C library function "memcpy".
//
//****************************************************************************
    _EXPORT_ __rt_memcpy
__rt_memcpy _LABEL_
    _EXPORT_ __rt_memcpy_w
__rt_memcpy_w _LABEL_
    _EXPORT_ memcpy
memcpy _LABEL_
    stmdb   r13!, {r0, r14}
    subs    r2, r2, _CONST_ 4
    blt     Up_TrailingBytes
    ands    r12, r0, _CONST_ 3
    bne     Up_AlignDst
    ands    r12, r1, _CONST_ 3
    bne     Up_SrcUnaligned
Up_SrcDstAligned _LABEL_
    subs    r2, r2, _CONST_ 12-4
    blt     Up_TrailingWords
    subs    r2, r2, _CONST_ 32-12
    blt     memcpy1
    stmfd   r13!, {r4}
memcpy0 _LABEL_
    ldmia   r1!, {r3, r4, r12, r14}
    stmia   r0!, {r3, r4, r12, r14}
    ldmia   r1!, {r3, r4, r12, r14}
    stmia   r0!, {r3, r4, r12, r14}
    subs    r2, r2, _CONST_ 32
    bge     memcpy0
    cmn     r2, _CONST_ 16
    ldmgeia r1!, {r3, r4, r12, r14}
    stmgeia r0!, {r3, r4, r12, r14}
    subge   r2, r2, _CONST_ 16
    ldmfd   r13!, {r4}
memcpy1 _LABEL_
    adds    r2, r2, _CONST_ 32-12
memcpy2 _LABEL_
    ldmgeia r1!, {r3, r12, r14}
    stmgeia r0!, {r3, r12, r14}
    subges  r2, r2, _CONST_ 12
    bge     memcpy2
Up_TrailingWords _LABEL_
    adds    r2, r2, _CONST_ 12-4
    blt     Up_TrailingBytes
    subs    r2, r2, _CONST_ 4
    ldrlt   r3, [r1], _CONST_ 4
    strlt   r3, [r0], _CONST_ 4
    ldmgeia r1!, {r3, r12}
    stmgeia r0!, {r3, r12}
    subge   r2, r2, _CONST_ 4
Up_TrailingBytes _LABEL_
    adds    r2, r2, _CONST_ 4
    ldmeqia r13!, {r0, pc}
    cmp     r2, _CONST_ 2
    ldrb    r3, [r1], _CONST_ 1
    strb    r3, [r0], _CONST_ 1
    ldrgeb  r3, [r1], _CONST_ 1
    strgeb  r3, [r0], _CONST_ 1
    ldrgtb  r3, [r1], _CONST_ 1
    strgtb  r3, [r0], _CONST_ 1
    ldmia   r13!, {r0, pc}
Up_AlignDst _LABEL_
    rsb     r12, r12, _CONST_ 4
    cmp     r12, _CONST_ 2
    ldrb    r3, [r1], _CONST_ 1
    strb    r3, [r0], _CONST_ 1
    ldrgeb  r3, [r1], _CONST_ 1
    strgeb  r3, [r0], _CONST_ 1
    ldrgtb  r3, [r1], _CONST_ 1
    strgtb  r3, [r0], _CONST_ 1
    subs    r2, r2, r12
    blt     Up_TrailingBytes
    ands    r12, r1, _CONST_ 3
    beq     Up_SrcDstAligned
Up_SrcUnaligned _LABEL_
    bic     r1, r1, _CONST_ 3
    ldr     r14, [r1], _CONST_ 4
    cmp     r12, _CONST_ 2
    bgt     Up_OneByte
    beq     Up_TwoBytes
Up_ThreeBytes _LABEL_
    cmp     r2, _CONST_ 16-4
    blt     memcpy4
    sub     r2, r2, _CONST_ 16-4
    stmfd   r13!, {r4, r5}
memcpy3 _LABEL_
    mov     r3, r14, lsr _CONST_ 8
    ldmia   r1!, {r4, r5, r12, r14}
    orr     r3, r3, r4, lsl _CONST_ 24
    mov     r4, r4, lsr _CONST_ 8
    orr     r4, r4, r5, lsl _CONST_ 24
    mov     r5, r5, lsr _CONST_ 8
    orr     r5, r5, r12, lsl _CONST_ 24
    mov     r12, r12, lsr _CONST_ 8
    orr     r12, r12, r14, lsl _CONST_ 24
    stmia   r0!, {r3, r4, r5, r12}
    subs    r2, r2, _CONST_ 16
    bge     memcpy3
    ldmfd   r13!, {r4, r5}
    adds    r2, r2, _CONST_ 16-4
    blt     memcpy5
memcpy4 _LABEL_
    mov     r12, r14, lsr _CONST_ 8
    ldr     r14, [r1], _CONST_ 4
    orr     r12, r12, r14, lsl _CONST_ 24
    str     r12, [r0], _CONST_ 4
    subs    r2, r2, _CONST_ 4
    bge     memcpy4
memcpy5 _LABEL_
    sub     r1, r1, _CONST_ 3
    b       Up_TrailingBytes
Up_TwoBytes _LABEL_
    cmp     r2, _CONST_ 16-4
    blt     memcpy7
    sub     r2, r2, _CONST_ 16-4
    stmfd   r13!, {r4, r5}
memcpy6 _LABEL_
    mov     r3, r14, lsr _CONST_ 16
    ldmia   r1!, {r4, r5, r12, r14}
    orr     r3, r3, r4, lsl _CONST_ 16
    mov     r4, r4, lsr _CONST_ 16
    orr     r4, r4, r5, lsl _CONST_ 16
    mov     r5, r5, lsr _CONST_ 16
    orr     r5, r5, r12, lsl _CONST_ 16
    mov     r12, r12, lsr _CONST_ 16
    orr     r12, r12, r14, lsl _CONST_ 16
    stmia   r0!, {r3, r4, r5, r12}
    subs    r2, r2, _CONST_ 16
    bge     memcpy6
    ldmfd   r13!, {r4, r5}
    adds    r2, r2, _CONST_ 16-4
    blt     memcpy8
memcpy7 _LABEL_
    mov     r12, r14, lsr _CONST_ 16
    ldr     r14, [r1], _CONST_ 4
    orr     r12, r12, r14, lsl _CONST_ 16
    str     r12, [r0], _CONST_ 4
    subs    r2, r2, _CONST_ 4
    bge     memcpy7
memcpy8 _LABEL_
    sub     r1, r1, _CONST_ 2
    b       Up_TrailingBytes
Up_OneByte _LABEL_
    cmp     r2, _CONST_ 16-4
    blt     memcpy10
    sub     r2, r2, _CONST_ 16-4
    stmfd   r13!, {r4, r5}
memcpy9 _LABEL_
    mov     r3, r14, lsr _CONST_ 24
    ldmia   r1!, {r4, r5, r12, r14}
    orr     r3, r3, r4, lsl _CONST_ 8
    mov     r4, r4, lsr _CONST_ 24
    orr     r4, r4, r5, lsl _CONST_ 8
    mov     r5, r5, lsr _CONST_ 24
    orr     r5, r5, r12, lsl _CONST_ 8
    mov     r12, r12, lsr _CONST_ 24
    orr     r12, r12, r14, lsl _CONST_ 8
    stmia   r0!, {r3, r4, r5, r12}
    subs    r2, r2, _CONST_ 16
    bge     memcpy9
    ldmfd   r13!, {r4, r5}
    adds    r2, r2, _CONST_ 16-4
    blt     memcpy11
memcpy10 _LABEL_
    mov     r12, r14, lsr _CONST_ 24
    ldr     r14, [r1], _CONST_ 4
    orr     r12, r12, r14, lsl _CONST_ 8
    str     r12, [r0], _CONST_ 4
    subs    r2, r2, _CONST_ 4
    bge     memcpy10
memcpy11 _LABEL_
    sub     r1, r1, _CONST_ 1
    b       Up_TrailingBytes

    _END_
