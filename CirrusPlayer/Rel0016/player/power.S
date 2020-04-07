//****************************************************************************
//
// POWER.S - Routines to put the EP7209 into various low power modes.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

#include "../asmdefs.h"
#include "../hwport.h"
#include "../hwdefs.h"

//****************************************************************************
//
// Read-only code area.
//
//****************************************************************************
    _TEXT_

//****************************************************************************
//
// Standby puts the EP7209 into STANDBY mode.  In this mode, the ARM processor
// stops executing instructions and all the peripherals are disabled (except
// for the real-time clock and interrupt controller).  This mode is exited by
// any external interrupt.
//
//****************************************************************************
    _EXPORT_ Standby
Standby _LABEL_
    //
    // Load a pointer to the EP7209 internal registers.
    //
    ldr     r0, =HwBaseAddress

    //
    // Put the EP7209 into STANDBY mode.
    //
    str     r0, [r0, _CONST_ HwStandby]

    //
    // Two single cycle NOP instructions.  These are needed to make exit from
    // STANDBY mode as quick as possible.  Note that these instructions are not
    // executed until the EP7209 comes out of STANDBY mode.
    //
    mov     r0, r0
    mov     r0, r0

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// Halt puts the EP7209 into IDLE mode.  In this mode, all the peripherals
// continue to operate but the ARM processor stops executing instructions.
// This mode is exited by any interrupt.
//
//****************************************************************************
    _EXPORT_ Halt
Halt _LABEL_
    //
    // Load a pointer to the EP7209 internal registers.
    //
    ldr     r0, =HwBaseAddress

    //
    // Put the EP7209 into IDLE mode.
    //
    str     r0, [r0, _CONST_ HwHalt]

    //
    // Two single cycle NOP instructions.  These are needed to make exit from
    // IDLE mode as quick as possible.  Note that these instructions are not
    // executed until the EP7209 comes out of IDLE mode.
    //
    mov     r0, r0
    mov     r0, r0

    //
    // Return to the caller.
    //
    mov     pc, lr

    _END_
