//****************************************************************************
//
// _DFIXU.S - Routine to convert a double to an unsigned long.
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
// Convert a double to an unsigned long.
//
//****************************************************************************
    _EXPORT_ _dfixu
_dfixu _LABEL_
    movs    r3, r0, asr _CONST_ 20
    bmi     dfixu_error
    mov     r2, r0, lsl _CONST_ 11
    orr     r2, r2, r1, lsr _CONST_ 21
    orr     r2, r2, _CONST_ 0x80000000
    sub     r3, r3, _CONST_ 0x00000380
    rsbs    r3, r3, _CONST_ 0x0000009e
    movcs   r0, r2, lsr r3
    movcs   pc, r14
dfixu_error _LABEL_
    mov     r0, _CONST_ 0
    mov     pc, r14

    _END_
