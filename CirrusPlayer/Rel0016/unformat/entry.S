//****************************************************************************
//
// ENTRY.S - Startup code for the unformat utility.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

#include "../asmdefs.h"
#include "../hwport.h"
#include "../hwdefs.h"

//****************************************************************************
//
// Routines in nandsupp.s for accessing the NAND FLASH.
//
//****************************************************************************
    _IMPORT_ NANDGetID
    _IMPORT_ NANDRead_512_3
    _IMPORT_ NANDRead_512_4
    _IMPORT_ NANDWrite_512_3
    _IMPORT_ NANDWrite_512_4
    _IMPORT_ NANDErase_16
    _IMPORT_ NANDErase_32_3
    _IMPORT_ NANDErase_32_4
    _IMPORT_ NANDWaitTilNotBusy

//****************************************************************************
//
// Zero-initialized read/write data area.
//
//****************************************************************************
    _BSS_

//****************************************************************************
//
// Memory buffer used to contain the SVC mode stack.
//
//****************************************************************************
SVCStack _LABEL_
    _SPACE_ 0x100
SVCStackEnd _LABEL_

//****************************************************************************
//
// Memory buffer used to read data from the NAND FLASH.
//
//****************************************************************************
NANDBuffer _LABEL_
    _SPACE_ 0x210

//****************************************************************************
//
// Memory buffer used to contain the bad block table of the "Cirrus Simple
// Filesystem".
//
//****************************************************************************
BadBlocks _LABEL_
    _SPACE_ 0x84

//****************************************************************************
//
// Read/write data area.
//
//****************************************************************************
    _DATA_

//****************************************************************************
//
// An array of function pointers for accessing the NAND FLASH devices of the
// various sizes.
//
//****************************************************************************
NANDSmall _LABEL_
    _WORD_  NANDRead_512_3, NANDWrite_512_3, NANDErase_16       // 4,8 Meg
NANDMedium _LABEL_
    _WORD_  NANDRead_512_3, NANDWrite_512_3, NANDErase_32_3     // 16,32 Meg
NANDLarge _LABEL_
    _WORD_  NANDRead_512_4, NANDWrite_512_4, NANDErase_32_4     // 64,128 Meg

//****************************************************************************
//
// Read-only code area.
//
//****************************************************************************
    _TEXT_

//****************************************************************************
//
// The ARM vector table.  This must reside at location 0x00000000 in the ARM's
// address space.  When the ARM takes an exception, it changes the program
// counter to one of the first eight addresses in memory.  Each vector has only
// one instruction, so we use that instruction to load the program counter with
// the address of the handler routine, which is stored in a table following the
// vector table.
//
//****************************************************************************
    _ENTRY_

    //
    // Load pointers to the EP7209 internal registers.
    //
    ldr     r0, =0x80000000
    add     r1, r0, _CONST_ 0x00001000
    add     r2, r0, _CONST_ 0x00002000

    //
    // Clear the startup reason flags.
    //
    str     r0, [r0, _CONST_ HwStartFlagClear]

    //
    // Disable all the interrupt sources.
    //
    ldr     r3, =0x00000000
    str     r3, [r0, _CONST_ HwIntMask]
    str     r3, [r1, _CONST_ (HwIntMask2 - 0x1000)]
    str     r3, [r2, _CONST_ (HwIntMask3 - 0x2000)]

    //
    // Program the memory configuration register to properly access the
    // peripherals on the board.
    //
    ldr     r3, [r0, _CONST_ HwStatus]
    and     r3, r3, _CONST_ HwStatusBootWidthMask
    cmp     r3, _CONST_ HwStatusBootWidth16
    ldreq   r3, =(HwMemConfig1_Value | 0x01010100)
    ldrne   r3, =HwMemConfig1_Value
    str     r3, [r0, _CONST_ HwMemConfig1]
    ldreq   r3, =(HwMemConfig2_Value | 0x00000101)
    ldrne   r3, =HwMemConfig2_Value
    str     r3, [r0, _CONST_ HwMemConfig2]

    //
    // Program the system control registers.
    //
    ldr     r3, =HwControl1_Value | HwControlUartEnable
    str     r3, [r0, _CONST_ HwControl]
    ldr     r3, =HwControl2_Value
    str     r3, [r1, _CONST_ (HwControl2 - 0x1000)]
    ldr     r3, =HwControl3_Value
    str     r3, [r2, _CONST_ (HwControl3 - 0x2000)]

    //
    // Set the direction of the GPIO pins.
    //
    ldr      r3, =HwPortABCD_DefaultDir
    str      r3, [r0, _CONST_ HwDdrABCD]
    ldr      r3, =HwPortE_DefaultDir
    str      r3, [r0, _CONST_ HwDdrE]

    //
    // Set the initial state of the GPIO pins.
    //
    ldr      r3, =HwPortABCD_DefaultValue
    str      r3, [r0, _CONST_ HwPortABCD]
    ldr      r3, =HwPortE_DefaultValue
    str      r3, [r0, _CONST_ HwPortE]

    //
    // Fill the zero initialized data block in RAM with zeros.
    //
    _IMPORT_ _ZIBASE_
    _IMPORT_ _RWLIMIT_
    ldr     r1, =_ZIBASE_
    ldr     r2, =_RWLIMIT_
    ldr     r0, =0x00000000
zero _LABEL_
        cmp     r1, r2
        strne   r0, [r1], _CONST_ 4
        bne     zero

    //
    // Set up the stack pointer.
    //
    ldr     r13, =SVCStackEnd

    //
    // Select the first NAND FLASH.
    //
    ldr     r0, =0x80000000
    ldr     r1, [r0]
    bic     r1, r1, _CONST_ HwPortABCD_NAND1_CS
    str     r1, [r0]

    //
    // Unformat the first NAND FLASH.
    //
    bl      Unformat

    //
    // Deslect the first NAND FLASH.
    //
    ldr     r0, =0x80000000
    ldr     r1, [r0]
    orr     r1, r1, _CONST_ HwPortABCD_NAND1_CS
    str     r1, [r0]

#ifdef HwPortABCD_NAND2_CS

    //
    // Select the second NAND FLASH.
    //
    bic     r1, r1, _CONST_ HwPortABCD_NAND2_CS
    str     r1, [r0]

    //
    // Unformat the second NAND FLASH.
    //
    bl      Unformat

    //
    // Deselect the second NAND FLASH.
    //
    ldr     r0, =0x80000000
    ldr     r1, [r0]
    orr     r1, r1, _CONST_ HwPortABCD_NAND2_CS
    str     r1, [r0]

#endif

    //
    // Write a '!' to the serial port to indicate that we've finished the
    // unformat.
    //
    ldr     r1, =0x21
    str     r1, [r0, _CONST_ HwUartData]

    //
    // Loop forever.
    //
    b       .

    //
    // Tell the assembler to put in-line data here.
    //
    _LTORG_

//****************************************************************************
//
// Unformat performs the actual unformat of the NAND FLASH.
//
//****************************************************************************
Unformat _LABEL_
    //
    // Save the link register on the stack.
    //
    stmdb   r13!, {lr}

    //
    // Read the ID of the NAND FLASH.
    //
    ldr     r0, =0x60000000
    bl      NANDGetID

    //
    // Determine the size of the device based on the ID.  Is this a 4MB FLASH?
    //
    cmp     r0, _CONST_ 0x6b
    cmpne   r0, _CONST_ 0xe3
    cmpne   r0, _CONST_ 0xe5
    bne     not_4MB
        //
        // We have a 4MB FLASH.  It has 512 blocks and 16 pages per block.
        //
        ldr     r11, =512
        ldr     r12, =16

        //
        // This FLASH is classified as a "small" FLASH.
        //
        ldr     r10, =NANDSmall

        //
        // Start the actual unformat.
        //
        b       got_id

    //
    // Is this a 8MB FLASH?
    //
not_4MB _LABEL_
    cmp     r0, _CONST_ 0xe6
    bne     not_8MB
        //
        // We have a 8MB FLASH.  It has 1024 blocks and 16 pages per block.
        //
        ldr     r11, =1024
        ldr     r12, =16

        //
        // This FLASH is classified as a "small" FLASH.
        //
        ldr     r10, =NANDSmall

        //
        // Start the actual unformat.
        //
        b       got_id

    //
    // Is this a 16MB FLASH?
    //
not_8MB _LABEL_
    cmp     r0, _CONST_ 0x73
    bne     not_16MB
        //
        // We have a 16MB FLASH.  It has 1024 blocks and 32 pages per block.
        //
        ldr     r11, =1024
        ldr     r12, =32

        //
        // This FLASH is classified as a "medium" FLASH.
        //
        ldr     r10, =NANDMedium

        //
        // Start the actual unformat.
        //
        b       got_id

    //
    // Is this a 32MB FLASH?
    //
not_16MB _LABEL_
    cmp     r0, _CONST_ 0x75
    bne     not_32MB
        //
        // We have a 32MB FLASH.  It has 2048 blocks and 32 pages per block.
        //
        ldr     r11, =2048
        ldr     r12, =32

        //
        // This FLASH is classified as a "medium" FLASH.
        //
        ldr     r10, =NANDMedium

        //
        // Start the actual unformat.
        //
        b       got_id

    //
    // Is this a 64MB FLASH?
    //
not_32MB _LABEL_
    cmp     r0, _CONST_ 0x76
    bne     not_64MB
        //
        // We have a 64MB FLASH.  It has 4096 blocks and 32 pages per block.
        //
        ldr     r11, =4096
        ldr     r12, =32

        //
        // This FLASH is classified as a "large" FLASH.
        //
        ldr     r10, =NANDLarge

        //
        // Start the actual unformat.
        //
        b       got_id

    //
    // Is this a 128MB FLASH?
    //
not_64MB _LABEL_
    cmp     r0, _CONST_ 0x79
    bne     not_128MB
        //
        // We have a 128MB FLASH.  It has 8192 blocks and 32 pages per block.
        //
        ldr     r11, =8192
        ldr     r12, =32

        //
        // This FLASH is classified as a "large" FLASH.
        //
        ldr     r10, =NANDLarge

        //
        // Start the actual unformat.
        //
        b       got_id

    //
    // We do not recognize this FLASH, so do nothing.
    //
not_128MB _LABEL_
    ldmia   r13!, {pc}

    //
    // Now, read the first page of the NAND FLASH.
    //
got_id _LABEL_
    ldr     r0, =0x60000000
    ldr     r1, =0x00000000
    ldr     r2, =NANDBuffer
    mov     lr, pc
    ldr     pc, [r10]

    //
    // If the first page of the NAND FLASH starts with "BKOK", then perform
    // an unformat of the "Cirrus Simple Filesystem".
    //
    ldr     r0, =NANDBuffer
    ldr     r1, [r0]
    ldr     r2, =0x4b4f4b42
    cmp     r1, r2
    beq     simplefs

    //
    // We have a SSFDC compliant file system.  Loop through all the available
    // blocks.
    //
    ldr     r9, =0x00000000
ssfdc_next_block _LABEL_
        //
        // Read the first page of the current block.
        //
        ldr     r0, =0x60000000
        mul     r1, r9, r12
        ldr     r2, =NANDBuffer
        mov     lr, pc
        ldr     pc, [r10]

        //
        // See if this is a bad block, as indicated by the sixth byte of the
        // redundant area.
        //
        ldr     r0, =NANDBuffer + 0x200
        ldrb    r0, [r0, _CONST_ 5]
        cmp     r0, _CONST_ 0xff
        bne     ssfdc_do_not_erase

            //
            // Delay a bit so that the sequential read above completes.
            //
            ldr     r0, =0x100
ssfdc_delay _LABEL_
                subs    r0, r0, _CONST_ 1
                bne     ssfdc_delay

            //
            // This is not a bad block, so erase it.
            //
            ldr     r0, =0x60000000
            mov     r1, r9
            mov     lr, pc
            ldr     pc, [r10, _CONST_ 8]

            //
            // Wait until the erase has completed.
            //
            bl      NANDWaitTilNotBusy

        //
        // Advance to the next block and loop back if there are more blocks to
        // check.
        //
ssfdc_do_not_erase _LABEL_
        add     r9, r9, _CONST_ 1
        cmp     r9, r11
        bne     ssfdc_next_block

    //
    // We've erased all the blocks that are not bad, so we are done.
    //
    ldmia   r13!, {pc}

    //
    // We have the "Cirrus Simple Filesystem".  Save the bad block table from
    // the first sector.
    //
simplefs _LABEL_
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

    //
    // Delay a bit so that the sequential read above completes.
    //
    ldr     r0, =0x100
simplefs_delay _LABEL_
        subs    r0, r0, _CONST_ 1
        bne     simplefs_delay

    //
    // Get the number of bad blocks in the device, and the pointer to the first
    // bad block in the table.
    //
    ldr     r7, =BadBlocks
    ldrh    r8, [r7], _CONST_ 2

    //
    // Loop through all the blocks in the device.
    //
    ldr     r9, =0x00000000
simplefs_next_block _LABEL_
        //
        // See if this block is bad.
        //
        cmp     r8, _CONST_ 0
        beq     simplefs_erase
        ldrh    r0, [r7]
        cmp     r0, r9
        addeq   r7, r7, _CONST_ 2
        subeq   r8, r8, _CONST_ 1
        beq     simplefs_do_not_erase

            //
            // Erase the current block.
            //
simplefs_erase _LABEL_
            ldr     r0, =0x60000000
            mov     r1, r9
            mov     lr, pc
            ldr     pc, [r10, _CONST_ 8]

            //
            // Wait until the erase has completed.
            //
            bl      NANDWaitTilNotBusy

        //
        // Advance to the next block and loop back if there are more blocks.
        //
simplefs_do_not_erase _LABEL_
        add     r9, r9, _CONST_ 1
        cmp     r9, r11
        bne     simplefs_next_block

    //
    // We've erased all the blocks that are not bad, so we are done.
    //
    ldmia   r13!, {pc}

    _END_
