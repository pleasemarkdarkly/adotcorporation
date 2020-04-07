;*****************************************************************************
;
; IRQ.S - Code for enabling and disabling the IRQ and FIQ interrupts of the
;         ARM processor.
;
; Copyright (c) 1999-2000 Cirrus Logic, Inc.
;
;*****************************************************************************

    GET     ../hwdefs.inc

;*****************************************************************************
;
; Read-only code area.
;
;*****************************************************************************
    AREA    |Assembly$$code|, CODE, READONLY

;*****************************************************************************
;
; DisableFIQ masks out the FIQ interrupt.
;
;*****************************************************************************
    EXPORT  DisableFIQ
DisableFIQ
    ;
    ; Mask out the FIQ interrupt in the ARM program status register.
    ;
    mrs     r0, cpsr
    orr     r0, r0, #0x00000040
    msr     cpsr_cf, r0

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; EnableFIQ re-enables the FIQ interrupt.
;
;*****************************************************************************
    EXPORT  EnableFIQ
EnableFIQ
    ;
    ; Enable the FIQ interrupt in the ARM program status register.
    ;
    mrs     r0, cpsr
    bic     r0, r0, #0x00000040
    msr     cpsr_cf, r0

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; DisableIRQ masks out the IRQ interrupt.
;
;*****************************************************************************
    EXPORT  DisableIRQ
DisableIRQ
    ;
    ; Mask out the IRQ interrupt in the ARM program status register.
    ;
    mrs     r0, cpsr
    orr     r0, r0, #0x00000080
    msr     cpsr_cf, r0

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; EnableIRQ re-enables the IRQ interrupt.
;
;*****************************************************************************
    EXPORT  EnableIRQ
EnableIRQ
    ;
    ; Enable the IRQ interrupt in the ARM program status register.
    ;
    mrs     r0, cpsr
    bic     r0, r0, #0x00000080
    msr     cpsr_cf, r0

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; DisableTimer disables the timer interrupt.
;
;*****************************************************************************
    EXPORT  DisableTimer
DisableTimer
    ;
    ; Disable the TIMER1 interrupt source.
    ;
    ldr     r0, =HwBaseAddress
    ldr     r1, [r0, #HwIntMask]
    bic     r1, r1, #HwIrqTimer1
    str     r1, [r0, #HwIntMask]

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; EnableTimer enables the timer interrupt.
;
;*****************************************************************************
    EXPORT  EnableTimer
EnableTimer
    ;
    ; Enable the TIMER1 interrupt source.
    ;
    ldr     r0, =HwBaseAddress
    ldr     r1, [r0, #HwIntMask]
    orr     r1, r1, #HwIrqTimer1
    str     r1, [r0, #HwIntMask]

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    END
