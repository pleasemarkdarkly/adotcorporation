//****************************************************************************
//
// LLUSHR.S - Routine to right shift an unsigned 64-bit value.
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
// Shift right an unsigned 64-bit value.
//
//****************************************************************************
    _EXPORT_ _ll_ushift_r
_ll_ushift_r _LABEL_
    subs    r3, r2, _CONST_ 32
    bpl     _ll_ushift_r01
    rsb     r3, r2, _CONST_ 32
    mov     r0, r0, lsr r2
    orr     r0, r0, r1, lsl r3
    mov     r1, r1, lsr r2
    mov     pc, r14
_ll_ushift_r01 _LABEL_
    mov     r0, r1, lsr r3
    mov     r1, _CONST_ 0
    mov     pc, r14

    _END_
