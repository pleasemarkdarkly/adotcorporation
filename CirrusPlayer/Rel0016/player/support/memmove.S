//****************************************************************************
//
// MEMMOVE.S - Routine to implement the C library function "memmove".
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
// Standard C library function "memmove".
//
//****************************************************************************
    _EXPORT_ __rt_memmove
__rt_memmove _LABEL_
    _EXPORT_ memmove
memmove _LABEL_
    cmp     r1, r0
    _IMPORT_ memcpy
    bhi     memcpy
    add     r1, r1, r2
    add     r0, r0, r2
    subs    r2, r2, _CONST_ 4
    blt     Down_TrailingBytes
    ands    r12, r0, _CONST_ 3
    bne     Down_AlignDst
    ands    r12, r1, _CONST_ 3
    bne     Down_SrcUnaligned
Down_SrcDstAligned _LABEL_
    subs    r2, r2, _CONST_ 12-4
    blt     Down_TrailingWords
    stmfd   r13!, {r4, r14}
    subs    r2, r2, _CONST_ 32-12
    blt     memmove1
memmove0 _LABEL_
    ldmdb   r1!, {r3, r4, r12, r14}
    stmdb   r0!, {r3, r4, r12, r14}
    ldmdb   r1!, {r3, r4, r12, r14}
    stmdb   r0!, {r3, r4, r12, r14}
    subs    r2, r2, _CONST_ 32
    bge     memmove0
memmove1 _LABEL_
    cmn     r2, _CONST_ 16
    ldmgedb r1!, {r3, r4, r12, r14}
    stmgedb r0!, {r3, r4, r12, r14}
    subge   r2, r2, _CONST_ 16
    adds    r2, r2, _CONST_ 32-12
    ldmgedb r1!, {r3, r12, r14}
    stmgedb r0!, {r3, r12, r14}
    subge   r2, r2, _CONST_ 12
    ldmfd   r13!, {r4, r14}
Down_TrailingWords _LABEL_
    adds    r2, r2, _CONST_ 12-4
    blt     Down_TrailingBytes
    subs    r2, r2, _CONST_ 4
    ldrlt   r3, [r1, _CONST_ -4]!
    strlt   r3, [r0, _CONST_ -4]!
    ldmgedb r1!, {r3, r12}
    stmgedb r0!, {r3, r12}
    subge   r2, r2, _CONST_ 4
Down_TrailingBytes _LABEL_
    adds    r2, r2, _CONST_ 4
    moveq   pc, r14
    cmp     r2, _CONST_ 2
    ldrb    r3, [r1, _CONST_ -1]!
    strb    r3, [r0, _CONST_ -1]!
    ldrgeb  r3, [r1, _CONST_ -1]!
    strgeb  r3, [r0, _CONST_ -1]!
    ldrgtb  r3, [r1, _CONST_ -1]!
    strgtb  r3, [r0, _CONST_ -1]!
    mov     pc, r14
Down_AlignDst _LABEL_
    cmp     r12, _CONST_ 2
    ldrb    r3, [r1, _CONST_ -1]!
    strb    r3, [r0, _CONST_ -1]!
    ldrgeb  r3, [r1, _CONST_ -1]!
    strgeb  r3, [r0, _CONST_ -1]!
    ldrgtb  r3, [r1, _CONST_ -1]!
    strgtb  r3, [r0, _CONST_ -1]!
    subs    r2, r2, r12
    blt     Down_TrailingBytes
    ands    r12, r1, _CONST_ 3
    beq     Down_SrcDstAligned
Down_SrcUnaligned _LABEL_
    bic     r1, r1, _CONST_ 3
    ldr     r3, [r1]
    cmp     r12, _CONST_ 2
    blt     Down_OneByte
    beq     Down_TwoBytes
Down_ThreeBytes _LABEL_
    cmp     r2, _CONST_ 16-4
    blt     memmove3
    sub     r2, r2, _CONST_ 16-4
    stmfd   r13!, {r4, r5, r14}
memmove2 _LABEL_
    mov     r14, r3, lsl _CONST_ 8
    ldmdb   r1!, {r3, r4, r5, r12}
    orr     r14, r14, r12, lsr _CONST_ 24
    mov     r12, r12, lsl _CONST_ 8
    orr     r12, r12, r5, lsr _CONST_ 24
    mov     r5, r5, lsl _CONST_ 8
    orr     r5, r5, r4, lsr _CONST_ 24
    mov     r4, r4, lsl _CONST_ 8
    orr     r4, r4, r3, lsr _CONST_ 24
    stmdb   r0!, {r4, r5, r12, r14}
    subs    r2, r2, _CONST_ 16
    bge     memmove2
    ldmfd   r13!, {r4, r5, r14}
    adds    r2, r2, _CONST_ 16-4
    blt     memmove4
memmove3 _LABEL_
    mov     r12, r3, lsl _CONST_ 8
    ldr     r3, [r1, _CONST_ -4]!
    orr     r12, r12, r3, lsr _CONST_ 24
    str     r12, [r0, _CONST_ -4]!
    subs    r2, r2, _CONST_ 4
    bge     memmove3
memmove4 _LABEL_
    add     r1, r1, _CONST_ 3
    b       Down_TrailingBytes
Down_TwoBytes _LABEL_
    cmp     r2, _CONST_ 16-4
    blt     memmove6
    sub     r2, r2, _CONST_ 16-4
    stmfd   r13!, {r4, r5, r14}
memmove5 _LABEL_
    mov     r14, r3, lsl _CONST_ 16
    ldmdb   r1!, {r3, r4, r5, r12}
    orr     r14, r14, r12, lsr _CONST_ 16
    mov     r12, r12, lsl _CONST_ 16
    orr     r12, r12, r5, lsr _CONST_ 16
    mov     r5, r5, lsl _CONST_ 16
    orr     r5, r5, r4, lsr _CONST_ 16
    mov     r4, r4, lsl _CONST_ 16
    orr     r4, r4, r3, lsr _CONST_ 16
    stmdb   r0!, {r4, r5, r12, r14}
    subs    r2, r2, _CONST_ 16
    bge     memmove5
    ldmfd   r13!, {r4, r5, r14}
    adds    r2, r2, _CONST_ 16-4
    blt     memmove7
memmove6 _LABEL_
    mov     r12, r3, lsl _CONST_ 16
    ldr     r3, [r1, _CONST_ -4]!
    orr     r12, r12, r3, lsr _CONST_ 16
    str     r12, [r0, _CONST_ -4]!
    subs    r2, r2, _CONST_ 4
    bge     memmove6
memmove7 _LABEL_
    add     r1, r1, _CONST_ 2
    b       Down_TrailingBytes
Down_OneByte _LABEL_
    cmp     r2, _CONST_ 16-4
    blt     memmove9
    sub     r2, r2, _CONST_ 16-4
    stmfd   r13!, {r4, r5, r14}
memmove8 _LABEL_
    mov     r14, r3, lsl _CONST_ 24
    ldmdb   r1!, {r3, r4, r5, r12}
    orr     r14, r14, r12, lsr _CONST_ 8
    mov     r12, r12, lsl _CONST_ 24
    orr     r12, r12, r5, lsr _CONST_ 8
    mov     r5, r5, lsl _CONST_ 24
    orr     r5, r5, r4, lsr _CONST_ 8
    mov     r4, r4, lsl _CONST_ 24
    orr     r4, r4, r3, lsr _CONST_ 8
    stmdb   r0!, {r4, r5, r12, r14}
    subs    r2, r2, _CONST_ 16
    bge     memmove8
    ldmfd   r13!, {r4, r5, r14}
    adds    r2, r2, _CONST_ 16-4
    blt     memmove10
memmove9 _LABEL_
    mov     r12, r3, lsl _CONST_ 24
    ldr     r3, [r1, _CONST_ -4]!
    orr     r12, r12, r3, lsr _CONST_ 8
    str     r12, [r0, _CONST_ -4]!
    subs    r2, r2, _CONST_ 4
    bge     memmove9
memmove10 _LABEL_
    add     r1, r1, _CONST_ 1
    b       Down_TrailingBytes

    _END_
