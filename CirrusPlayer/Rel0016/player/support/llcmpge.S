//****************************************************************************
//
// LLCMPGE.S - Routine to compare two unsigned 64-bit values to see if one is
//             greater than or equal to the other.
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
// Compare two unsigned 64-bit values to see if one is greater than or equal
// to the other.
//
//****************************************************************************
    _EXPORT_ _ll_cmpge
_ll_cmpge _LABEL_
    subs     r0, r0, r2
    sbcs     r1, r1, r3
    mov      pc, r14

    _END_
