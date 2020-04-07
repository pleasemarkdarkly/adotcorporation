;*****************************************************************************
;
; BOOTCODE.S - Bootup code for the Internet audio player.
;
; Copyright (c) 2000 Cirrus Logic, Inc.
;
;*****************************************************************************
    GET     ../hwdefs.inc
    GET     ../config.inc

;*****************************************************************************
;
; Routines in nandsupp.s for accessing the NAND FLASH.
;
;*****************************************************************************
    IMPORT  NANDGetID
    IMPORT  NANDRead_512_3
    IMPORT  NANDRead_512_4
    IMPORT  NANDWrite_512_3
    IMPORT  NANDWrite_512_4
    IMPORT  NANDErase_16
    IMPORT  NANDErase_32_3
    IMPORT  NANDErase_32_4
    IMPORT  NANDWaitTilNotBusy

;*****************************************************************************
;
; Zero-initialized read/write data area for stacks.
;
;*****************************************************************************
    AREA    |Assembly$$zistack|, DATA, READWRITE, NOINIT

;*****************************************************************************
;
; Memory buffer used to contain the SVC mode stack.
;
;*****************************************************************************
SVCStack
    %       0x100
SVCStackEnd

;*****************************************************************************
;
; Zero-initialized read/write data area for data.
;
;*****************************************************************************
    AREA    |Assembly$$data|, DATA, READWRITE, NOINIT

;*****************************************************************************
;
; Memory buffer used to read data from the NAND FLASH.
;
;*****************************************************************************
NANDBuffer
    %       0x210

;*****************************************************************************
;
; Memory buffer used to contain the bad block table of the "Cirrus Simple
; Filesystem".
;
;*****************************************************************************
BadBlocks
    %       0x84

;*****************************************************************************
;
; Read-only constant data area.
;
;*****************************************************************************
    AREA    |Assembly$$constdata|, DATA, READONLY

;*****************************************************************************
;
; An array of function pointers for accessing the NAND FLASH devices of the
; various sizes.
;
;*****************************************************************************
NANDSmall
    DCD     NANDRead_512_3, NANDWrite_512_3, NANDErase_16       ; 4,8 Meg
NANDMedium
    DCD     NANDRead_512_3, NANDWrite_512_3, NANDErase_32_3     ; 16,32 Meg
NANDLarge
    DCD     NANDRead_512_4, NANDWrite_512_4, NANDErase_32_4     ; 64,128 Meg

;*****************************************************************************
;
; Read-only code area.
;
;*****************************************************************************
    AREA    |Assembly$$code|, CODE, READONLY

;*****************************************************************************
;
; The ARM vector table.  This must reside at location 0x00000000 in the ARM's
; address space.  When the ARM takes an exception, it changes the program
; counter to one of the first eight addresses in memory.  Each vector has only
; one instruction, so we use that instruction to load the program counter with
; the address of the handler routine, which is stored in a table following the
; vector table.
;
;*****************************************************************************
    ENTRY

    ;
    ; Load pointers to the EP7209 internal registers.
    ;
    ldr     r0, =0x80000000
    add     r1, r0, #0x00001000
    add     r2, r0, #0x00002000

    ;
    ; Clear the startup reason flags.
    ;
    str     r0, [r0, #HwStartFlagClear]

    ;
    ; Disable all the interrupt sources.
    ;
    ldr     r3, =0x00000000
    str     r3, [r0, #HwIntMask]
    str     r3, [r1, #(HwIntMask2 - 0x1000)]
    str     r3, [r2, #(HwIntMask3 - 0x2000)]

    ;
    ; Program the memory configuration register to properly access the
    ; peripherals on the board.
    ;
    ldr     r3, [r0, #HwStatus]
    and     r3, r3, #HwStatusBootWidthMask
    cmp     r3, #HwStatusBootWidth16
    ldreq   r3, =(HwMemConfig1_Value | 0x01010100)
    ldrne   r3, =HwMemConfig1_Value
    str     r3, [r0, #HwMemConfig1]
    ldreq   r3, =(HwMemConfig2_Value | 0x00000101)
    ldrne   r3, =HwMemConfig2_Value
    str     r3, [r0, #HwMemConfig2]

    ;
    ; Program the system control registers.
    ;
    ldr     r3, =HwControl1_Value | HwControlUartEnable
    str     r3, [r0, #HwControl]
    ldr     r3, =HwControl2_Value
    str     r3, [r1, #(HwControl2 - 0x1000)]
    ldr     r3, =HwControl3_Value
    str     r3, [r2, #(HwControl3 - 0x2000)]

    ;
    ; Set the direction of the GPIO pins.
    ;
    ldr      r3, =HwPortABCD_DefaultDir
    str      r3, [r0, #HwDdrABCD]
    ldr      r3, =HwPortE_DefaultDir
    str      r3, [r0, #HwDdrE]

    ;
    ; Set the initial state of the GPIO pins.
    ;
    ldr      r3, =HwPortABCD_DefaultValue
    str      r3, [r0, #HwPortABCD]
    ldr      r3, =HwPortE_DefaultValue
    str      r3, [r0, #HwPortE]

    ;
    ; Fill the zero initialized data block in RAM with zeros.
    ;
    IMPORT  |Image$$ZI$$Base|
    IMPORT  |Image$$RW$$Limit|
    ldr     r1, =|Image$$ZI$$Base|
    ldr     r2, =|Image$$RW$$Limit|
    ldr     r0, =0x00000000
1
        cmp     r1, r2
        strne   r0, [r1], #4
        bne     %b1

    ;
    ; Set up the stack pointer.
    ;
    ldr     r13, =SVCStackEnd

    ;
    ; Select the first NAND FLASH.
    ;
    ldr     r0, =0x80000000
    ldr     r1, [r0]
    bic     r1, r1, #HwPortABCD_NAND1_CS
    str     r1, [r0]

    ;
    ; Unformat the first NAND FLASH.
    ;
    bl      Unformat

    ;
    ; Deslect the first NAND FLASH.
    ;
    ldr     r0, =0x80000000
    ldr     r1, [r0]
    orr     r1, r1, #HwPortABCD_NAND1_CS
    str     r1, [r0]

    IF (:DEF: HwPortABCD_NAND2_CS)

    ;
    ; Select the second NAND FLASH.
    ;
    bic     r1, r1, #HwPortABCD_NAND2_CS
    str     r1, [r0]

    ;
    ; Unformat the second NAND FLASH.
    ;
    bl      Unformat

    ;
    ; Deselect the second NAND FLASH.
    ;
    ldr     r0, =0x80000000
    ldr     r1, [r0]
    orr     r1, r1, #HwPortABCD_NAND2_CS
    str     r1, [r0]

    ENDIF

    ;
    ; Write a '!' to the serial port to indicate that we've finished the
    ; unformat.
    ;
    ldr     r1, =0x21
    str     r1, [r0, #HwUartData]

    ;
    ; Loop forever.
    ;
    b       .

    ;
    ; Tell the assembler to put in-line data here.
    ;
    LTORG

;*****************************************************************************
;
; Unformat performs the actual unformat of the NAND FLASH.
;
;*****************************************************************************
Unformat
    ;
    ; Save the link register on the stack.
    ;
    stmdb   r13!, {lr}

    ;
    ; Read the ID of the NAND FLASH.
    ;
    ldr     r0, =0x60000000
    bl      NANDGetID

    ;
    ; Determine the size of the device based on the ID.  Is this a 4MB FLASH?
    ;
    cmp     r0, #0x6b
    cmpne   r0, #0xe3
    cmpne   r0, #0xe5
    bne     %f1
        ;
        ; We have a 4MB FLASH.  It has 512 blocks and 16 pages per block.
        ;
        ldr     r11, =512
        ldr     r12, =16

        ;
        ; This FLASH is classified as a "small" FLASH.
        ;
        ldr     r10, =NANDSmall

        ;
        ; Start the actual unformat.
        ;
        b       %f7

    ;
    ; Is this a 8MB FLASH?
    ;
1
    cmp     r0, #0xe6
    bne     %f2
        ;
        ; We have a 8MB FLASH.  It has 1024 blocks and 16 pages per block.
        ;
        ldr     r11, =1024
        ldr     r12, =16

        ;
        ; This FLASH is classified as a "small" FLASH.
        ;
        ldr     r10, =NANDSmall

        ;
        ; Start the actual unformat.
        ;
        b       %f7

    ;
    ; Is this a 16MB FLASH?
    ;
2
    cmp     r0, #0x73
    bne     %f3
        ;
        ; We have a 16MB FLASH.  It has 1024 blocks and 32 pages per block.
        ;
        ldr     r11, =1024
        ldr     r12, =32

        ;
        ; This FLASH is classified as a "medium" FLASH.
        ;
        ldr     r10, =NANDMedium

        ;
        ; Start the actual unformat.
        ;
        b       %f7

    ;
    ; Is this a 32MB FLASH?
    ;
3
    cmp     r0, #0x75
    bne     %f4
        ;
        ; We have a 32MB FLASH.  It has 2048 blocks and 32 pages per block.
        ;
        ldr     r11, =2048
        ldr     r12, =32

        ;
        ; This FLASH is classified as a "medium" FLASH.
        ;
        ldr     r10, =NANDMedium

        ;
        ; Start the actual unformat.
        ;
        b       %f7

    ;
    ; Is this a 64MB FLASH?
    ;
4
    cmp     r0, #0x76
    bne     %f5
        ;
        ; We have a 64MB FLASH.  It has 4096 blocks and 32 pages per block.
        ;
        ldr     r11, =4096
        ldr     r12, =32

        ;
        ; This FLASH is classified as a "large" FLASH.
        ;
        ldr     r10, =NANDLarge

        ;
        ; Start the actual unformat.
        ;
        b       %f7

    ;
    ; Is this a 128MB FLASH?
    ;
5
    cmp     r0, #0x79
    bne     %f6
        ;
        ; We have a 128MB FLASH.  It has 8192 blocks and 32 pages per block.
        ;
        ldr     r11, =8192
        ldr     r12, =32

        ;
        ; This FLASH is classified as a "large" FLASH.
        ;
        ldr     r10, =NANDLarge

        ;
        ; Start the actual unformat.
        ;
        b       %f7

    ;
    ; We do not recognize this FLASH, so do nothing.
    ;
6
    ldmia   r13!, {pc}

    ;
    ; Now, read the first page of the NAND FLASH.
    ;
7
    ldr     r0, =0x60000000
    ldr     r1, =0x00000000
    ldr     r2, =NANDBuffer
    mov     lr, pc
    ldr     pc, [r10]

    ;
    ; If the first page of the NAND FLASH starts with "BKOK", then perform
    ; an unformat of the "Cirrus Simple Filesystem".
    ;
    ldr     r0, =NANDBuffer
    ldr     r1, [r0]
    ldr     r2, =0x4b4f4b42
    cmp     r1, r2
    beq     %f11

    ;
    ; We have a SSFDC compliant file system.  Loop through all the available
    ; blocks.
    ;
    ldr     r9, =0x00000000
8
        ;
        ; Read the first page of the current block.
        ;
        ldr     r0, =0x60000000
        mul     r1, r9, r12
        ldr     r2, =NANDBuffer
        mov     lr, pc
        ldr     pc, [r10]

        ;
        ; See if this is a bad block, as indicated by the sixth byte of the
        ; redundant area.
        ;
        ldr     r0, =NANDBuffer + 0x200
        ldrb    r0, [r0, #5]
        cmp     r0, #0xff
        bne     %f10

            ;
            ; Delay a bit so that the sequential read above completes.
            ;
            ldr     r0, =0x100
9
                subs    r0, r0, #1
                bne     %b9

            ;
            ; This is not a bad block, so erase it.
            ;
            ldr     r0, =0x60000000
            mov     r1, r9
            mov     lr, pc
            ldr     pc, [r10, #8]

            ;
            ; Wait until the erase has completed.
            ;
            bl      NANDWaitTilNotBusy

        ;
        ; Advance to the next block and loop back if there are more blocks to
        ; check.
        ;
10
        add     r9, r9, #1
        cmp     r9, r11
        bne     %b8

    ;
    ; We've erased all the blocks that are not bad, so we are done.
    ;
    ldmia   r13!, {pc}

    ;
    ; We have the "Cirrus Simple Filesystem".  Save the bad block table from
    ; the first sector.
    ;
11
    ldr     r0, =NANDBuffer + 4
    ldr     r1, =BadBlocks
    ldmia   r0!, {r2-r9}
    stmia   r1!, {r2-r9}
    ldmia   r0!, {r2-r9}
    stmia   r1!, {r2-r9}
    ldmia   r0!, {r2-r9}
    stmia   r1!, {r2-r9}
    ldmia   r0!, {r2-r9}
    stmia   r1!, {r2-r9}
    ldmia   r0, {r2}
    stmia   r1, {r2}

    ;
    ; Delay a bit so that the sequential read above completes.
    ;
    ldr     r0, =0x100
12
        subs    r0, r0, #1
        bne     %b12

    ;
    ; Get the number of bad blocks in the device, and the pointer to the first
    ; bad block in the table.
    ;
    ldr     r7, =BadBlocks
    ldrh    r8, [r7], #2

    ;
    ; Loop through all the blocks in the device.
    ;
    ldr     r9, =0x00000000
13
        ;
        ; See if this block is bad.
        ;
        cmp     r8, #0
        beq     %f14
        ldrh    r0, [r7]
        cmp     r0, r9
        addeq   r7, r7, #2
        subeq   r8, r8, #1
        beq     %f15

            ;
            ; Erase the current block.
            ;
14
            ldr     r0, =0x60000000
            mov     r1, r9
            mov     lr, pc
            ldr     pc, [r10, #8]

            ;
            ; Wait until the erase has completed.
            ;
            bl      NANDWaitTilNotBusy

        ;
        ; Advance to the next block and loop back if there are more blocks.
        ;
15
        add     r9, r9, #1
        cmp     r9, r11
        bne     %b13

    ;
    ; We've erased all the blocks that are not bad, so we are done.
    ;
    ldmia   r13!, {pc}

    END
