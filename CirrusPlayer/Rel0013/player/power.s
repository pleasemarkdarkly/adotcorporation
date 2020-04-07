;*****************************************************************************
;
; POWER.S - Routines to put the EP7209 into various low power modes.
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
; Standby puts the EP7209 into STANDBY mode.  In this mode, the ARM processor
; stops executing instructions and all the peripherals are disabled (except
; for the real-time clock and interrupt controller).  This mode is exited by
; any external interrupt.
;
;*****************************************************************************
    EXPORT  Standby
Standby
    ;
    ; Load a pointer to the EP7209 internal registers.
    ;
    ldr     r0, =HwBaseAddress

    ;
    ; Put the EP7209 into STANDBY mode.
    ;
    str     r0, [r0, #HwStandby]

    ;
    ; Two single cycle NOP instructions.  These are needed to make exit from
    ; STANDBY mode as quick as possible.  Note that these instructions are not
    ; executed until the EP7209 comes out of STANDBY mode.
    ;
    mov     r0, r0
    mov     r0, r0

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; Halt puts the EP7209 into IDLE mode.  In this mode, all the peripherals
; continue to operate but the ARM processor stops executing instructions.
; This mode is exited by any interrupt.
;
;*****************************************************************************
    EXPORT  Halt
Halt
    ;
    ; Load a pointer to the EP7209 internal registers.
    ;
    ldr     r0, =HwBaseAddress

    ;
    ; Put the EP7209 into IDLE mode.
    ;
    str     r0, [r0, #HwHalt]

    ;
    ; Two single cycle NOP instructions.  These are needed to make exit from
    ; IDLE mode as quick as possible.  Note that these instructions are not
    ; executed until the EP7209 comes out of IDLE mode.
    ;
    mov     r0, r0
    mov     r0, r0

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    END
