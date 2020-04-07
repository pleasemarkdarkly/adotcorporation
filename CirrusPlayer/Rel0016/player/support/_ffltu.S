//****************************************************************************
//
// _FFLTU.S - Routine to convert an unsigned long to a float.
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
// Convert an unsigned long to a float.
//
//****************************************************************************
    _EXPORT_ _ffltu
_ffltu _LABEL_
    mov     r3, _CONST_ 0x40000000
    movs    r1, r0, lsr _CONST_ 16
    addne   r3, r3, _CONST_ 0x08000000
    moveqs  r0, r0, lsl _CONST_ 16
    moveq   pc, r14
    tst     r0, _CONST_ 0xFF000000
    addne   r3, r3, _CONST_ 0x04000000
    moveq   r0, r0, lsl _CONST_ 8
    tst     r0, _CONST_ 0xF0000000
    addne   r3, r3, _CONST_ 0x02000000
    moveq   r0, r0, lsl _CONST_ 4
    tst     r0, _CONST_ 0xC0000000
    addne   r3, r3, _CONST_ 0x01000000
    moveqs  r0, r0, lsl _CONST_ 2
    addmi   r3, r3, _CONST_ 0x00800000
    movpl   r0, r0, lsl _CONST_ 1
    movs    r1, r0, lsl _CONST_ 25
    adc     r0, r3, r0, asr _CONST_ 8
    movcc   pc, r14
    movne   pc, r14
    bic     r0, r0, _CONST_ 1
    mov     pc, r14

    _END_
