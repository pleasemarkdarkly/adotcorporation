//****************************************************************************
//
// MEMSET.S - Routine to implement the C library function "memset".
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
// Standard C library function "memset".
//
//****************************************************************************
    _EXPORT_ __rt_memclr_w
__rt_memclr_w _LABEL_
    mov     r2, r1
    mov     r1, _CONST_ 0
    _EXPORT_ __rt_memset
__rt_memset _LABEL_
    _EXPORT_ memset
memset _LABEL_
    stmdb   r13!, {r0, r14}
    subs    r2, r2, _CONST_ 4
    bmi     TrailingBytes
    ands    r12, r0, _CONST_ 3
    bne     AlignDst
DstAligned _LABEL_
    and     r1, r1, _CONST_ 0xff
    orr     r1, r1, r1, LSL _CONST_ 8
    orr     r1, r1, r1, LSL _CONST_ 16
    mov     r3, r1
    mov     r12, r1
    mov     r14, r1
    subs    r2, r2, _CONST_ 12-4
    blt     TrailingWords
    subs    r2, r2, _CONST_ 32-12
    blt     memset1
memset0 _LABEL_
    stmia   r0!, {r1, r3, r12, r14}
    stmia   r0!, {r1, r3, r12, r14}
    subs    r2, r2, _CONST_ 32
    bge     memset0
    cmn     r2, _CONST_ 16
    stmgeia r0!, {r1, r3, r12, r14}
    subge   r2, r2, _CONST_ 16
memset1 _LABEL_
    adds    r2, r2, _CONST_ 32-12
memset2 _LABEL_
    stmgeia r0!, {r3, r12, r14}
    subges  r2, r2, _CONST_ 12
    bge     memset2
TrailingWords _LABEL_
    adds    r2, r2, _CONST_ 12-4
    blt     TrailingBytes
    subs    r2, r2, _CONST_ 4
    strlt   r1, [r0], _CONST_ 4
    stmgeia r0!, {r1, r3}
    subge   r2, r2, _CONST_ 4
TrailingBytes _LABEL_
    adds    r2, r2, _CONST_ 4
    ldmeqia r13!, {r0, pc}
    cmp     r2, _CONST_ 2
    strb    r1, [r0], _CONST_ 1
    strgeb  r1, [r0], _CONST_ 1
    strgtb  r1, [r0], _CONST_ 1
    ldmia   r13!, {r0, pc}
AlignDst _LABEL_
    rsb     r12, r12, _CONST_ 4
    cmp     r12, _CONST_ 2
    strb    r1, [r0], _CONST_ 1
    strgeb  r1, [r0], _CONST_ 1
    strgtb  r1, [r0], _CONST_ 1
    subs    r2, r2, r12
    blt     TrailingBytes
    b       DstAligned

    _END_
