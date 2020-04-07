//****************************************************************************
//
// IRQ.S - Code for enabling and disabling the IRQ and FIQ interrupts of the
//         ARM processor.
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
// DisableFIQ masks out the FIQ interrupt.
//
//****************************************************************************
    _EXPORT_ DisableFIQ
DisableFIQ _LABEL_
    //
    // Mask out the FIQ interrupt in the ARM program status register.
    //
    mrs     r0, cpsr
    orr     r0, r0, _CONST_ 0x00000040
    msr     cpsr_cf, r0

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// EnableFIQ re-enables the FIQ interrupt.
//
//****************************************************************************
    _EXPORT_ EnableFIQ
EnableFIQ _LABEL_
    //
    // Enable the FIQ interrupt in the ARM program status register.
    //
    mrs     r0, cpsr
    bic     r0, r0, _CONST_ 0x00000040
    msr     cpsr_cf, r0

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// DisableIRQ masks out the IRQ interrupt.
//
//****************************************************************************
    _EXPORT_ DisableIRQ
DisableIRQ _LABEL_
    //
    // Mask out the IRQ interrupt in the ARM program status register.
    //
    mrs     r0, cpsr
    orr     r0, r0, _CONST_ 0x00000080
    msr     cpsr_cf, r0

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// EnableIRQ re-enables the IRQ interrupt.
//
//****************************************************************************
    _EXPORT_ EnableIRQ
EnableIRQ _LABEL_
    //
    // Enable the IRQ interrupt in the ARM program status register.
    //
    mrs     r0, cpsr
    bic     r0, r0, _CONST_ 0x00000080
    msr     cpsr_cf, r0

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// DisableTimer disables the timer interrupt.
//
//****************************************************************************
    _EXPORT_ DisableTimer
DisableTimer _LABEL_
    //
    // Disable the TIMER1 interrupt source.
    //
    ldr     r0, =HwBaseAddress
    ldr     r1, [r0, _CONST_ HwIntMask]
    bic     r1, r1, _CONST_ HwIrqTimer1
    str     r1, [r0, _CONST_ HwIntMask]

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// EnableTimer enables the timer interrupt.
//
//****************************************************************************
    _EXPORT_ EnableTimer
EnableTimer _LABEL_
    //
    // Enable the TIMER1 interrupt source.
    //
    ldr     r0, =HwBaseAddress
    ldr     r1, [r0, _CONST_ HwIntMask]
    orr     r1, r1, _CONST_ HwIrqTimer1
    str     r1, [r0, _CONST_ HwIntMask]

    //
    // Return to the caller.
    //
    mov     pc, lr

    _END_
