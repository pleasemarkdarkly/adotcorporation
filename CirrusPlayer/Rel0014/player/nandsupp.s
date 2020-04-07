;*****************************************************************************
;
; NANDSUPP.S - Routines to read/write the NAND FLASH (be it on-board NAND or
;              removeable SmartMedia).
;
; Copyright (c) 1999-2000 Cirrus Logic, Inc.
;
;*****************************************************************************

    GET     ../hwdefs.inc
    GET     ../config.inc

;*****************************************************************************
;
; The delay required for data to be copied from the cell array to the page
; register.
;
;*****************************************************************************
DELAY   EQU     0x00000100

;*****************************************************************************
;
; Read-only code area.
;
;*****************************************************************************
    AREA    |Assembly$$code|, CODE, READONLY

;*****************************************************************************
;
; NANDGetID read the ID from the NAND FLASH device.
;
;*****************************************************************************
    EXPORT  NANDGetID
NANDGetID
    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r1, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r2, [r1]

    ;
    ; Assert CLE.
    ;
    orr     r2, r2, #HwPortABCD_NAND_CLE
    str     r2, [r1]

    ;
    ; Write the 'read id' command.
    ;
    ldr     r3, =0x90
    strb    r3, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r2, r2, #HwPortABCD_NAND_CLE
    str     r2, [r1]

    ;
    ; Assert ALE.
    ;
    orr     r2, r2, #HwPortABCD_NAND_ALE
    str     r2, [r1]

    ;
    ; Write the address (zero).
    ;
    ldr     r3, =0x00
    strb    r3, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r2, r2, #HwPortABCD_NAND_ALE
    str     r2, [r1]

    ;
    ; Delay for ~100ns.
    ;
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    ;
    ; Read the manufacturer ID.
    ;
    ldrb    r3, [r0]

    ;
    ; Read the device ID.
    ;
    ldrb    r0, [r0]

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDRead_256 reads 512 bytes of data and 16 bytes of redundant data from a
; NAND FLASH device which has 256 bytes per page.
;
;*****************************************************************************
    IF (:DEF: SUPPORT_TINY_MEDIA)

    EXPORT  NANDRead_256
NANDRead_256
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r8}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r4, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r5, [r4]

    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Write the 'read1' command.
    ;
    ldr     r6, =0x00
    strb    r6, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Assert ALE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Write the page address.
    ;
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Wait for the page to copy from the cell array by sampling the READY/nBUSY
    ; signal from the NAND device.
    ;
1
        ldr     r6, [r4]
        tst     r6, #HwPortABCD_NAND_Ready
        beq     %b1

    ELSE

    ;
    ; Wait for the page to copy from the cell array by delaying.
    ;
    ldr     r6, =DELAY
1
        subs    r6, r6, #1
        bne     %b1

    ENDIF

    ;
    ; Read the 256 bytes from this page.
    ;
    ldr     r6, =256
2
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #24
        str     r8, [r2], #4
        subs    r6, r6, #4
        bne     %b2

    ;
    ; Read the 8 redundant bytes from this page.
    ;
    ldr     r6, =8
3
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #24
        str     r8, [r3], #4
        subs    r6, r6, #4
        bne     %b3

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Wait for the next page to copy from the cell array by sampling the
    ; READY/nBUSY signal from the NAND device.
    ;
4
        ldr     r6, [r4]
        tst     r6, #HwPortABCD_NAND_Ready
        beq     %b4

    ELSE

    ;
    ; Wait for the next page to copy from the cell array by delaying.
    ;
    ldr     r6, =DELAY
4
        subs    r6, r6, #1
        bne     %b4

    ENDIF

    ;
    ; Read the 256 bytes from the next page.
    ;
    ldr     r6, =256
5
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #24
        str     r8, [r2], #4
        subs    r6, r6, #4
        bne     %b5

    ;
    ; Read the 8 redundant bytes from the next page.
    ;
    ldr     r6, =8
6
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #24
        str     r8, [r3], #4
        subs    r6, r6, #4
        bne     %b6

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r8}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

    ENDIF

;*****************************************************************************
;
; NANDRead_512_3 reads 512 bytes of data and 16 bytes of redundant data from a
; NAND FLASH device which has 512 bytes per page and uses three address
; cycles.
;
;*****************************************************************************
    EXPORT  NANDRead_512_3
NANDRead_512_3
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r8}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r4, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r5, [r4]

    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Write the 'read1' command.
    ;
    ldr     r6, =0x00
    strb    r6, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Assert ALE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Write the page address.
    ;
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Wait for the page to copy from the cell array by sampling the READY/nBUSY
    ; signal from the NAND device.
    ;
1
        ldr     r6, [r4]
        tst     r6, #HwPortABCD_NAND_Ready
        beq     %b1

    ELSE

    ;
    ; Wait for the page to copy from the cell array by delaying.
    ;
    ldr     r6, =DELAY
1
        subs    r6, r6, #1
        bne     %b1

    ENDIF

    ;
    ; Read the 512 bytes from this page.
    ;
    ldr     r6, =512
2
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #24
        str     r8, [r2], #4
        subs    r6, r6, #4
        bne     %b2

    ;
    ; Read the 16 redundant bytes from this page.
    ;
    ldr     r6, =16
3
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #24
        str     r8, [r3], #4
        subs    r6, r6, #4
        bne     %b3

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r8}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDRead_512_4 reads 512 bytes of data and 16 bytes of redundant data from a
; NAND FLASH device which has 512 bytes per page and uses four address cycles.
;
;*****************************************************************************
    EXPORT  NANDRead_512_4
NANDRead_512_4
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r8}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r4, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r5, [r4]

    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Write the 'read1' command.
    ;
    ldr     r6, =0x00
    strb    r6, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Assert ALE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Write the page address.
    ;
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Wait for the page to copy from the cell array by sampling the READY/nBUSY
    ; signal from the NAND device.
    ;
1
        ldr     r6, [r4]
        tst     r6, #HwPortABCD_NAND_Ready
        beq     %b1

    ELSE

    ;
    ; Wait for the page to copy from the cell array by delaying.
    ;
    ldr     r6, =DELAY
1
        subs    r6, r6, #1
        bne     %b1

    ENDIF

    ;
    ; Read the 512 bytes from this page.
    ;
    ldr     r6, =512
2
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #24
        str     r8, [r2], #4
        subs    r6, r6, #4
        bne     %b2

    ;
    ; Read the 16 redundant bytes from this page.
    ;
    ldr     r6, =16
3
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl #24
        str     r8, [r3], #4
        subs    r6, r6, #4
        bne     %b3

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r8}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDWrite_256 writes 256 bytes of data and 8 bytes of redundant data to a
; NAND FLASH device which has 256 bytes per page.
;
;*****************************************************************************
    IF (:DEF: SUPPORT_TINY_MEDIA)

    EXPORT  NANDWrite_256
NANDWrite_256
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r7}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r4, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r5, [r4]

    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Write the 'serial data input' command.
    ;
    ldr     r6, =0x00
    strb    r6, [r0]
    ldr     r6, =0x80
    strb    r6, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Assert ALE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Write the page address.
    ;
    ldr     r6, =0x00
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Write 256 bytes to this page.
    ;
    ldr     r6, =256
1
        ldr     r7, [r2], #4
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        subs    r6, r6, #4
        bne     %b1

    ;
    ; Write 8 redundant bytes to this page.
    ;
    ldr     r6, =8
2
        ldr     r7, [r3], #4
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        subs    r6, r6, #4
        bne     %b2

    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Write the 'auto program' command.
    ;
    ldr     r6, =0x10
    strb    r6, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r7}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

    ENDIF

;*****************************************************************************
;
; NANDWrite_512_3 writes 512 bytes of data and 16 bytes of redundant data to a
; NAND FLASH device which has 512 bytes per page and uses three address cycles
; for writing pages.
;
;*****************************************************************************
    EXPORT  NANDWrite_512_3
NANDWrite_512_3
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r7}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r4, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r5, [r4]

    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Write the 'serial data input' command.
    ;
    ldr     r6, =0x00
    strb    r6, [r0]
    ldr     r6, =0x80
    strb    r6, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Assert ALE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Write the page address.
    ;
    ldr     r6, =0x00
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Write 512 bytes to this page.
    ;
    ldr     r6, =512
1
        ldr     r7, [r2], #4
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        subs    r6, r6, #4
        bne     %b1

    ;
    ; Write 16 redundant bytes to this page.
    ;
    ldr     r6, =16
2
        ldr     r7, [r3], #4
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        subs    r6, r6, #4
        bne     %b2

    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Write the 'auto program' command.
    ;
    ldr     r6, =0x10
    strb    r6, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r7}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDWrite_512_4 writes 512 bytes of data and 16 bytes of redundant data to a
; NAND FLASH device which has 512 bytes per page and uses four address cycles
; for writing pages.
;
;*****************************************************************************
    EXPORT  NANDWrite_512_4
NANDWrite_512_4
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r7}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r4, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r5, [r4]

    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Write the 'serial data input' command.
    ;
    ldr     r6, =0x00
    strb    r6, [r0]
    ldr     r6, =0x80
    strb    r6, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Assert ALE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Write the page address.
    ;
    ldr     r6, =0x00
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Write 512 bytes to this page.
    ;
    ldr     r6, =512
1
        ldr     r7, [r2], #4
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        subs    r6, r6, #4
        bne     %b1

    ;
    ; Write 16 redundant bytes to this page.
    ;
    ldr     r6, =16
2
        ldr     r7, [r3], #4
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        mov     r7, r7, lsr #8
        strb    r7, [r0]
        subs    r6, r6, #4
        bne     %b2

    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Write the 'auto program' command.
    ;
    ldr     r6, =0x10
    strb    r6, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r7}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDErase_16 erases a block of a NAND FLASH device which has 16 pages per
; block.
;
;*****************************************************************************
    EXPORT  NANDErase_16
NANDErase_16
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r2, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r3, [r2]

    ;
    ; Assert CLE.
    ;
    orr     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Write the 'auto block erase setup' command.
    ;
    ldr     r4, =0x60
    strb    r4, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Assert ALE.
    ;
    orr     r3, r3, #HwPortABCD_NAND_ALE
    str     r3, [r2]

    ;
    ; Write the page address.
    ;
    mov     r1, r1, lsl #4
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r3, r3, #HwPortABCD_NAND_ALE
    str     r3, [r2]

    ;
    ; Assert CLE.
    ;
    orr     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Write the 'erase' command.
    ;
    ldr     r4, =0xd0
    strb    r4, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDErase_32_3 erases a block of a NAND FLASH device which has 32 pages per
; block and uses two address cycles for the erasing blocks.
;
;*****************************************************************************
    EXPORT  NANDErase_32_3
NANDErase_32_3
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r2, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r3, [r2]

    ;
    ; Assert CLE.
    ;
    orr     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Write the 'auto block erase setup' command.
    ;
    ldr     r4, =0x60
    strb    r4, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Assert ALE.
    ;
    orr     r3, r3, #HwPortABCD_NAND_ALE
    str     r3, [r2]

    ;
    ; Write the page address.
    ;
    mov     r1, r1, lsl #5
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r3, r3, #HwPortABCD_NAND_ALE
    str     r3, [r2]

    ;
    ; Assert CLE.
    ;
    orr     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Write the 'erase' command.
    ;
    ldr     r4, =0xd0
    strb    r4, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDErase_32_4 erases a block of a NAND FLASH device which has 32 pages per
; block and uses three address cycles for erasing blocks.
;
;*****************************************************************************
    EXPORT  NANDErase_32_4
NANDErase_32_4
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r2, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r3, [r2]

    ;
    ; Assert CLE.
    ;
    orr     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Write the 'auto block erase setup' command.
    ;
    ldr     r4, =0x60
    strb    r4, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Assert ALE.
    ;
    orr     r3, r3, #HwPortABCD_NAND_ALE
    str     r3, [r2]

    ;
    ; Write the page address.
    ;
    mov     r1, r1, lsl #5
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r3, r3, #HwPortABCD_NAND_ALE
    str     r3, [r2]

    ;
    ; Assert CLE.
    ;
    orr     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Write the 'erase' command.
    ;
    ldr     r4, =0xd0
    strb    r4, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r3, r3, #HwPortABCD_NAND_CLE
    str     r3, [r2]

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDReadRedt_256 reads 16 bytes of redundant data from a NAND FLASH device
; which has 256 bytes per page.
;
;*****************************************************************************
    IF (:DEF: SUPPORT_TINY_MEDIA)

    EXPORT  NANDReadRedt_256
NANDReadRedt_256
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r7}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r3, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r4, [r3]

    ;
    ; Assert CLE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Write the 'read3' command.
    ;
    ldr     r5, =0x50
    strb    r5, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Assert ALE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    ;
    ; Write the page address.
    ;
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Wait for the page to copy from the cell array by sampling the READY/nBUSY
    ; signal from the NAND device.
    ;
1
        ldr     r5, [r3]
        tst     r5, #HwPortABCD_NAND_Ready
        beq     %b1

    ELSE

    ;
    ; Wait for the page to copy from the cell array by delaying.
    ;
    ldr     r5, =DELAY
1
        subs    r5, r5, #1
        bne     %b1

    ENDIF

    ;
    ; Read the 8 redundant bytes from this page.
    ;
    ldr     r5, =8
2
        ldrb    r7, [r0]
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #8
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #16
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #24
        str     r7, [r2], #4
        subs    r5, r5, #4
        bne     %b2

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Wait for the next page to copy from the cell array by sampling the
    ; READY/nBUSY signal from the NAND device.
    ;
3
        ldr     r5, [r3]
        tst     r5, #HwPortABCD_NAND_Ready
        beq     %b3

    ELSE

    ;
    ; Wait for the next page to copy from the cell array by delaying.
    ;
    ldr     r5, =DELAY
3
        subs    r5, r5, #1
        bne     %b3

    ENDIF

    ;
    ; Read the 8 redundant bytes from the next page.
    ;
    ldr     r5, =8
4
        ldrb    r7, [r0]
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #8
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #16
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #24
        str     r7, [r2], #4
        subs    r5, r5, #4
        bne     %b4

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r7}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

    ENDIF

;*****************************************************************************
;
; NANDReadRedt_512_3 reads 16 bytes of redundant data from a NAND FLASH device
; which has 512 bytes per page and uses three address cycles.
;
;*****************************************************************************
    EXPORT  NANDReadRedt_512_3
NANDReadRedt_512_3
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r7}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r3, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r4, [r3]

    ;
    ; Assert CLE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Write the 'read3' command.
    ;
    ldr     r5, =0x50
    strb    r5, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Assert ALE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    ;
    ; Write the page address.
    ;
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Wait for the page to copy from the cell array by sampling the READY/nBUSY
    ; signal from the NAND device.
    ;
1
        ldr     r5, [r3]
        tst     r5, #HwPortABCD_NAND_Ready
        beq     %b1

    ELSE

    ;
    ; Wait for the page to copy from the cell array by delaying.
    ;
    ldr     r5, =DELAY
1
        subs    r5, r5, #1
        bne     %b1

    ENDIF

    ;
    ; Read the 16 redundant bytes from this page.
    ;
    ldr     r5, =16
2
        ldrb    r7, [r0]
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #8
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #16
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #24
        str     r7, [r2], #4
        subs    r5, r5, #4
        bne     %b2

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r7}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDReadRedt_512_4 reads 16 bytes of redundant data from a NAND FLASH device
; which has 512 bytes per page and uses four address cycles.
;
;*****************************************************************************
    EXPORT  NANDReadRedt_512_4
NANDReadRedt_512_4
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r7}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r3, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r4, [r3]

    ;
    ; Assert CLE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Write the 'read3' command.
    ;
    ldr     r5, =0x50
    strb    r5, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Assert ALE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    ;
    ; Write the page address.
    ;
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Wait for the page to copy from the cell array by sampling the READY/nBUSY
    ; signal from the NAND device.
    ;
1
        ldr     r5, [r3]
        tst     r5, #HwPortABCD_NAND_Ready
        beq     %b1

    ELSE

    ;
    ; Wait for the page to copy from the cell array by delaying.
    ;
    ldr     r5, =DELAY
1
        subs    r5, r5, #1
        bne     %b1

    ENDIF

    ;
    ; Read the 16 redundant bytes from this page.
    ;
    ldr     r5, =16
2
        ldrb    r7, [r0]
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #8
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #16
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl #24
        str     r7, [r2], #4
        subs    r5, r5, #4
        bne     %b2

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r7}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDWriteRedt_256 writes 8 bytes of redundant data to a NAND FLASH device
; which has 256 bytes per page.
;
;*****************************************************************************
    IF (:DEF: SUPPORT_TINY_MEDIA)

    EXPORT  NANDWriteRedt_256
NANDWriteRedt_256
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r6}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r3, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r4, [r3]

    ;
    ; Assert CLE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Write the 'serial data input' command.
    ;
    ldr     r5, =0x50
    strb    r5, [r0]
    ldr     r5, =0x80
    strb    r5, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Assert ALE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    ;
    ; Write the page address.
    ;
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    ;
    ; Write 8 redundant bytes to this page.
    ;
    ldr     r5, =8
1
        ldr     r6, [r2], #4
        strb    r6, [r0]
        mov     r6, r6, lsr #8
        strb    r6, [r0]
        mov     r6, r6, lsr #8
        strb    r6, [r0]
        mov     r6, r6, lsr #8
        strb    r6, [r0]
        subs    r5, r5, #4
        bne     %b1

    ;
    ; Assert CLE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Write the 'auto program' command.
    ;
    ldr     r5, =0x10
    strb    r5, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r6}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

    ENDIF

;*****************************************************************************
;
; NANDWriteRedt_512_3 writes 16 bytes of redundant data to a NAND FLASH device
; which has 512 bytes per page and uses three address cycles for writing
; pages.
;
;*****************************************************************************
    EXPORT  NANDWriteRedt_512_3
NANDWriteRedt_512_3
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r6}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r3, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r4, [r3]

    ;
    ; Assert CLE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Write the 'serial data input' command.
    ;
    ldr     r5, =0x50
    strb    r5, [r0]
    ldr     r5, =0x80
    strb    r5, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Assert ALE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    ;
    ; Write the page address.
    ;
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    ;
    ; Write 16 redundant bytes to this page.
    ;
    ldr     r5, =16
1
        ldr     r6, [r2], #4
        strb    r6, [r0]
        mov     r6, r6, lsr #8
        strb    r6, [r0]
        mov     r6, r6, lsr #8
        strb    r6, [r0]
        mov     r6, r6, lsr #8
        strb    r6, [r0]
        subs    r5, r5, #4
        bne     %b1

    ;
    ; Assert CLE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Write the 'auto program' command.
    ;
    ldr     r5, =0x10
    strb    r5, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r6}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDWriteRedt_512_4 writes 16 bytes of redundant data to a NAND FLASH device
; which has 512 bytes per page and uses four address cycles for writing pages.
;
;*****************************************************************************
    EXPORT  NANDWriteRedt_512_4
NANDWriteRedt_512_4
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r6}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r3, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r4, [r3]

    ;
    ; Assert CLE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Write the 'serial data input' command.
    ;
    ldr     r5, =0x00
    strb    r5, [r0]
    ldr     r5, =0x80
    strb    r5, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Assert ALE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    ;
    ; Write the page address.
    ;
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_ALE
    str     r4, [r3]

    ;
    ; Write 16 redundant bytes to this page.
    ;
    ldr     r5, =16
1
        ldr     r6, [r2], #4
        strb    r6, [r0]
        mov     r6, r6, lsr #8
        strb    r6, [r0]
        mov     r6, r6, lsr #8
        strb    r6, [r0]
        mov     r6, r6, lsr #8
        strb    r6, [r0]
        subs    r5, r5, #4
        bne     %b1

    ;
    ; Assert CLE.
    ;
    orr     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Write the 'auto program' command.
    ;
    ldr     r5, =0x10
    strb    r5, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r4, r4, #HwPortABCD_NAND_CLE
    str     r4, [r3]

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4-r6}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDWaitTilNotBusy waits until the NAND device is not busy (i.e. a
; previously started program or erase operation has completed).
;
;*****************************************************************************
    EXPORT  NANDWaitTilNotBusy
NANDWaitTilNotBusy

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r1, =HwBaseAddress

    ;
    ; Wait for the erase to complete by sampling the READY/nBUSY signal from
    ; the NAND device.
    ;
1
        ldr     r2, [r1]
        tst     r2, #HwPortABCD_NAND_Ready
        beq     %b1

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ELSE

    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4}

    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r1, =HwBaseAddress

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r2, [r1]

    ;
    ; Assert CLE.
    ;
    orr     r2, r2, #HwPortABCD_NAND_CLE
    str     r2, [r1]

    ;
    ; Write the 'read status' command.
    ;
    ldr     r3, =0x70
    strb    r3, [r0]

    ;
    ; Deassert CLE.
    ;
    bic     r2, r2, #HwPortABCD_NAND_CLE
    str     r2, [r1]

    ;
    ; Wait until the operation has completed.
    ;
    ldr     r4, =0x00000040
1
        ldrb    r3, [r0]
        and     r3, r3, r4
        cmp     r3, r4
        bne     %b1

    ;
    ; Restore the non-volatile registers.
    ;
    ldmia   r13!, {r4}

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

    ENDIF

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

    END
