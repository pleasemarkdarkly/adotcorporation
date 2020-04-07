//****************************************************************************
//
// MEMCMP.S - Routine to implement the C library function "memcmp".
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
// Standard C library function "memcmp".
//
//****************************************************************************
    _EXPORT_ memcmp
memcmp _LABEL_
    stmdb   r13!, {r3-r4, r14}
    cmp     r2, _CONST_ 4
    blt     CheckBytes
    tst     r0, _CONST_ 3
    bne     CheckBytes
    tst     r1, _CONST_ 3
    bne     CheckBytes
CheckWords _LABEL_
    ldr     r3, [r0], _CONST_ 4
    ldr     r4, [r1], _CONST_ 4
    cmp     r3, r4
    bne     DoneWords
    sub     r2, r2, _CONST_ 4
    cmp     r2, _CONST_ 4
    bge     CheckWords
DoneWords _LABEL_
    cmp     r3, r4
    subne   r0, r0, _CONST_ 4
    subne   r1, r1, _CONST_ 4
CheckBytes _LABEL_
    cmp     r2, _CONST_ 0
    moveq   r0, _CONST_ 0
    ldmeqia r13!, {r3-r4, pc}
CheckByte _LABEL_
    ldrb    r3, [r0], _CONST_ 1
    ldrb    r4, [r1], _CONST_ 1
    cmp     r3, r4
    bne     DoneBytes
    subs    r2, r2, _CONST_ 1
    bne     CheckByte
DoneBytes _LABEL_
    sub     r0, r3, r4
    ldmia   r13!, {r3-r4, pc}

    _END_
