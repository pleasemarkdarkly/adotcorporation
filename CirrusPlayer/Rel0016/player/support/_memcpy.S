//****************************************************************************
//
// _MEMCPY.S - Routine to perform a word aligned memory copy.
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
// Word aligned source and destination memory copy (used to initialize
// automatics).
//
//****************************************************************************
    _EXPORT_ _memcpy
_memcpy _LABEL_
    stmdb   r13!, {r4, r14}
    subs    r2, r2, _CONST_ 32
loop _LABEL_
    ldmgeia r1!, {r3, r4, r12, r14}
    stmgeia r0!, {r3, r4, r12, r14}
    ldmgeia r1!, {r3, r4, r12, r14}
    stmgeia r0!, {r3, r4, r12, r14}
    subges  r2, r2, _CONST_ 32
    bge     loop
    adds    r2, r2, _CONST_ 16
    ldmgeia r1!, {r3, r4, r12, r14}
    stmgeia r0!, {r3, r4, r12, r14}
    movs    r2, r2, lsl _CONST_ 29
    ldmcsia r1!, {r3, r4}
    stmcsia r0!, {r3, r4}
    ldrmi   r3, [r1]
    strmi   r3, [r0]
    ldmia   r13!, {r4, pc}

    _END_
