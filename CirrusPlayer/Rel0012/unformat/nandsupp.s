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
; NANDSetup loads values into r4 and r5 in preparation for accessing the
; NAND FLASH.
;
;*****************************************************************************
NANDSetup
    ;
    ; Load a pointer to the internal registers.
    ;
    ldr     r4, =0x80000000

    ;
    ; Get the current value of the GPIO pins.
    ;
    ldr     r5, [r4]

    ;
    ; The first thing we do when accessing the NAND FLASH is to assert CLE,
    ; so fall into that code now.
    ;

;*****************************************************************************
;
; NANDAssertCLE asserts the CLE signal to the NAND FLASH.
;
;*****************************************************************************
NANDAssertCLE
    ;
    ; Assert CLE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; NANDDeassertCLE deasserts the CLE signal to the NAND FLASH.
;
;*****************************************************************************
NANDDeassertCLE
    ;
    ; Deassert CLE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_CLE
    str     r5, [r4]

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; NANDAssertALE asserts the ALE signal to the NAND FLASH.
;
;*****************************************************************************
NANDAssertALE
    ;
    ; Assert ALE.
    ;
    orr     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; NANDDeassertALE deasserts the ALE signal to the NAND FLASH.
;
;*****************************************************************************
NANDDeassertALE
    ;
    ; Deassert ALE.
    ;
    bic     r5, r5, #HwPortABCD_NAND_ALE
    str     r5, [r4]

    ;
    ; Return to the caller.
    ;
    mov     pc, lr

;*****************************************************************************
;
; NANDGetID read the ID from the NAND FLASH device.
;
;*****************************************************************************
    EXPORT  NANDGetID
NANDGetID
    ;
    ; Save the non-volatile registers which we will use.
    ;
    stmdb   r13!, {r4-r5, lr}

    ;
    ; Prepare to access the NAND FLASH.  This also asserts CLE.
    ;
    bl      NANDSetup

    ;
    ; Write the 'read id' command.
    ;
    ldr     r1, =0x90
    strb    r1, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Assert ALE.
    ;
    bl      NANDAssertALE

    ;
    ; Write the address (zero).
    ;
    ldr     r1, =0x00
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
    bl      NANDDeassertALE

    ;
    ; Delay for ~100ns.
    ;
    mla     r1, r4, r4, r4
    mla     r1, r4, r4, r4

    ;
    ; Read the manufacturer ID.
    ;
    ldrb    r1, [r0]

    ;
    ; Read the device ID.
    ;
    ldrb    r0, [r0]

    ;
    ; Restore the non-volatile registers and return to the caller.
    ;
    ldmia   r13!, {r4-r5, pc}

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

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
    stmdb   r13!, {r4-r6, lr}

    ;
    ; Prepare to access the NAND FLASH.  This also asserts CLE.
    ;
    bl      NANDSetup

    ;
    ; Write the 'read1' command.
    ;
    ldr     r3, =0x00
    strb    r3, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Assert ALE.
    ;
    bl      NANDAssertALE

    ;
    ; Write the page address.
    ;
    strb    r3, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; The remainder of the read sequence is identical to the four address cycle
    ; case, so branch to that code.
    ;
    b       FinishRead512

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
    stmdb   r13!, {r4-r6, lr}

    ;
    ; Prepare to access the NAND FLASH.  This also asserts CLE.
    ;
    bl      NANDSetup

    ;
    ; Write the 'read1' command.
    ;
    ldr     r3, =0x00
    strb    r3, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Assert ALE.
    ;
    bl      NANDAssertALE

    ;
    ; Write the page address.
    ;
    strb    r3, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
FinishRead512
    bl      NANDDeassertALE

    IF (:DEF: HwPortABCD_NAND_Ready)

    ;
    ; Wait for the page to copy from the cell array by sampling the READY/nBUSY
    ; signal from the NAND device.
    ;
1
        ldr     r3, [r4]
        tst     r3, #HwPortABCD_NAND_Ready
        beq     %b1

    ELSE

    ;
    ; Wait for the page to copy from the cell array by delaying.
    ;
    ldr     r3, =DELAY
1
        subs    r3, r3, #1
        bne     %b1

    ENDIF

    ;
    ; Read the 512 bytes and 16 redundant bytes from this page.
    ;
    ldr     r3, =528
2
        ldrb    r6, [r0]
        strb    r6, [r2], #1
        subs    r3, r3, #1
        bne     %b2

    ;
    ; Restore the non-volatile registers and return to the caller.
    ;
    ldmia   r13!, {r4-r6, pc}

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

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
    stmdb   r13!, {r4-r6, lr}

    ;
    ; Prepare to access the NAND FLASH.  This also asserts CLE.
    ;
    bl      NANDSetup

    ;
    ; Write the 'serial data input' command.
    ;
    ldr     r3, =0x00
    strb    r3, [r0]
    ldr     r3, =0x80
    strb    r3, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Assert ALE.
    ;
    bl      NANDAssertALE

    ;
    ; Write the page address.
    ;
    ldr     r3, =0x00
    strb    r3, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; The remainder of the write sequence is identical to the four address
    ; cycle case, so branch to that code.
    ;
    b       FinishWrite512

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
    stmdb   r13!, {r4-r6, lr}

    ;
    ; Prepare to access the NAND FLASH.  This also asserts CLE.
    ;
    bl      NANDSetup

    ;
    ; Write the 'serial data input' command.
    ;
    ldr     r3, =0x00
    strb    r3, [r0]
    ldr     r3, =0x80
    strb    r3, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Assert ALE.
    ;
    bl      NANDAssertALE

    ;
    ; Write the page address.
    ;
    ldr     r3, =0x00
    strb    r3, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; Deassert ALE.
    ;
FinishWrite512
    bl      NANDDeassertALE

    ;
    ; Write 512 bytes and 16 redundant bytes to this page.
    ;
    ldr     r3, =528
1
        ldrb    r6, [r2], #1
        strb    r6, [r0]
        subs    r3, r3, #1
        bne     %b1

    ;
    ; Assert CLE.
    ;
    bl      NANDAssertCLE

    ;
    ; Write the 'auto program' command.
    ;
    ldr     r3, =0x10
    strb    r3, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Restore the non-volatile registers and return to the caller.
    ;
    ldmia   r13!, {r4-r6, pc}

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; NANDErase_32_3 erases a block of a NAND FLASH device which has 32 pages per
; block and uses two address cycles for erasing blocks.
;
;*****************************************************************************
    EXPORT  NANDErase_32_3
NANDErase_32_3
    ;
    ; Erasing a NAND block which has 32 pages is identical to erasing a NAND
    ; block which has 16 pages, except the block address is shifted one more
    ; bit to the left.  So, shift the block address by one and fall into the
    ; code for erasing a NAND block with 16 pages.
    ;
    mov     r1, r1, lsl #1

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
    stmdb   r13!, {r4-r5, lr}

    ;
    ; Prepare to access the NAND FLASH.  This also asserts CLE.
    ;
    bl      NANDSetup

    ;
    ; Write the 'auto block erase setup' command.
    ;
    ldr     r2, =0x60
    strb    r2, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Assert ALE.
    ;
    bl      NANDAssertALE

    ;
    ; Write the page address.
    ;
    mov     r1, r1, lsl #4
    strb    r1, [r0]
    mov     r1, r1, lsr #8
    strb    r1, [r0]

    ;
    ; The remainder of the erase sequence is identical to the four address
    ; cycle case, so branch to that code.
    ;
    b       FinishErase

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
    stmdb   r13!, {r4-r5, lr}

    ;
    ; Prepare to access the NAND FLASH.  This also asserts CLE.
    ;
    bl      NANDSetup

    ;
    ; Write the 'auto block erase setup' command.
    ;
    ldr     r2, =0x60
    strb    r2, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Assert ALE.
    ;
    bl      NANDAssertALE

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
FinishErase
    bl      NANDDeassertALE

    ;
    ; Assert CLE.
    ;
    bl      NANDAssertCLE

    ;
    ; Write the 'erase' command.
    ;
    ldr     r2, =0xd0
    strb    r2, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Restore the non-volatile registers and return to the caller.
    ;
    ldmia   r13!, {r4-r5, pc}

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
    ldr     r1, =0x80000000

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
    stmdb   r13!, {r4-r5, lr}

    ;
    ; Prepare to access the NAND FLASH.  This also asserts CLE.
    ;
    bl      NANDSetup

    ;
    ; Write the 'read status' command.
    ;
    ldr     r1, =0x70
    strb    r1, [r0]

    ;
    ; Deassert CLE.
    ;
    bl      NANDDeassertCLE

    ;
    ; Wait until the operation has completed.
    ;
    ldr     r1, =0x40
1
        ldrb    r2, [r0]
        tst     r2, r1
        beq     %b1

    ;
    ; Restore the non-volatile registers and return to the caller.
    ;
    ldmia   r13!, {r4-r5, pc}

    ENDIF

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

    END
