//****************************************************************************
//
// WCSNCMP.S - Routine to implement the library function "wcsncmp".
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
// Unicode version of "strncmp".
//
//****************************************************************************
    _EXPORT_ wcsncmp
wcsncmp _LABEL_
    ldrh    r3, [r0], _CONST_ 2
    ldrh    r12, [r1], _CONST_ 2
    cmp     r3, _CONST_ 1
    cmpcs   r3, r12
    bne     wcsncmp0
    subs    r2, r2, _CONST_ 1
    bne     wcsncmp
wcsncmp0 _LABEL_
    sub     r0, r3, r12
    mov     pc, r14

    _END_
