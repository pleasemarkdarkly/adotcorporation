//****************************************************************************
//
// NANDSUPP.S - Routines to read/write the NAND FLASH (be it on-board NAND or
//              removeable SmartMedia).
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

#include "../asmdefs.h"
#include "../hwport.h"
#include "../hwdefs.h"

//****************************************************************************
//
// The delay required for data to be copied from the cell array to the page
// register.
//
//****************************************************************************
#define DELAY                                   0x00000100

//****************************************************************************
//
// Read-only code area.
//
//****************************************************************************
    _TEXT_

//****************************************************************************
//
// NANDSetup loads values into r4 and r5 in preparation for accessing the
// NAND FLASH.
//
//****************************************************************************
NANDSetup _LABEL_
    //
    // Load a pointer to the internal registers.
    //
    ldr     r4, =0x80000000

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r5, [r4]

    //
    // The first thing we do when accessing the NAND FLASH is to assert CLE,
    // so fall into that code now.
    //

//****************************************************************************
//
// NANDAssertCLE asserts the CLE signal to the NAND FLASH.
//
//****************************************************************************
NANDAssertCLE _LABEL_
    //
    // Assert CLE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// NANDDeassertCLE deasserts the CLE signal to the NAND FLASH.
//
//****************************************************************************
NANDDeassertCLE _LABEL_
    //
    // Deassert CLE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// NANDAssertALE asserts the ALE signal to the NAND FLASH.
//
//****************************************************************************
NANDAssertALE _LABEL_
    //
    // Assert ALE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// NANDDeassertALE deasserts the ALE signal to the NAND FLASH.
//
//****************************************************************************
NANDDeassertALE _LABEL_
    //
    // Deassert ALE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// NANDGetID read the ID from the NAND FLASH device.
//
//****************************************************************************
    _EXPORT_ NANDGetID
NANDGetID _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r5, lr}

    //
    // Prepare to access the NAND FLASH.  This also asserts CLE.
    //
    bl      NANDSetup

    //
    // Write the 'read id' command.
    //
    ldr     r1, =0x90
    strb    r1, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Assert ALE.
    //
    bl      NANDAssertALE

    //
    // Write the address (zero).
    //
    ldr     r1, =0x00
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bl      NANDDeassertALE

    //
    // Delay for ~100ns.
    //
    mla     r1, r4, r4, r4
    mla     r1, r4, r4, r4

    //
    // Read the manufacturer ID.
    //
    ldrb    r1, [r0]

    //
    // Read the device ID.
    //
    ldrb    r0, [r0]

    //
    // Restore the non-volatile registers and return to the caller.
    //
    ldmia   r13!, {r4-r5, pc}

//****************************************************************************
//
// NANDRead_512_3 reads 512 bytes of data and 16 bytes of redundant data from a
// NAND FLASH device which has 512 bytes per page and uses three address
// cycles.
//
//****************************************************************************
    _EXPORT_ NANDRead_512_3
NANDRead_512_3 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r6, lr}

    //
    // Prepare to access the NAND FLASH.  This also asserts CLE.
    //
    bl      NANDSetup

    //
    // Write the 'read1' command.
    //
    ldr     r3, =0x00
    strb    r3, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Assert ALE.
    //
    bl      NANDAssertALE

    //
    // Write the page address.
    //
    strb    r3, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // The remainder of the read sequence is identical to the four address
    // cycle case, so branch to that code.
    //
    b       FinishRead512

//****************************************************************************
//
// NANDRead_512_4 reads 512 bytes of data and 16 bytes of redundant data from a
// NAND FLASH device which has 512 bytes per page and uses four address cycles.
//
//****************************************************************************
    _EXPORT_ NANDRead_512_4
NANDRead_512_4 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r6, lr}

    //
    // Prepare to access the NAND FLASH.  This also asserts CLE.
    //
    bl      NANDSetup

    //
    // Write the 'read1' command.
    //
    ldr     r3, =0x00
    strb    r3, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Assert ALE.
    //
    bl      NANDAssertALE

    //
    // Write the page address.
    //
    strb    r3, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
FinishRead512 _LABEL_
    bl      NANDDeassertALE

#ifdef HwPortABCD_NAND_Ready

    //
    // Wait for the page to copy from the cell array by sampling the
    // READY/nBUSY signal from the NAND device.
    //
NANDRead_512_4_1 _LABEL_
        ldr     r3, [r4]
        tst     r3, _CONST_ HwPortABCD_NAND_Ready
        beq     NANDRead_512_4_1

#else

    //
    // Wait for the page to copy from the cell array by delaying.
    //
    ldr     r3, =DELAY
NANDRead_512_4_1 _LABEL_
        subs    r3, r3, _CONST_ 1
        bne     NANDRead_512_4_1

#endif

    //
    // Read the 512 bytes and 16 redundant bytes from this page.
    //
    ldr     r3, =528
NANDRead_512_4_2 _LABEL_
        ldrb    r6, [r0]
        strb    r6, [r2], _CONST_ 1
        subs    r3, r3, _CONST_ 1
        bne     NANDRead_512_4_2

    //
    // Restore the non-volatile registers and return to the caller.
    //
    ldmia   r13!, {r4-r6, pc}

//****************************************************************************
//
// NANDWrite_512_3 writes 512 bytes of data and 16 bytes of redundant data to a
// NAND FLASH device which has 512 bytes per page and uses three address cycles
// for writing pages.
//
//****************************************************************************
    _EXPORT_ NANDWrite_512_3
NANDWrite_512_3 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r6, lr}

    //
    // Prepare to access the NAND FLASH.  This also asserts CLE.
    //
    bl      NANDSetup

    //
    // Write the 'serial data input' command.
    //
    ldr     r3, =0x00
    strb    r3, [r0]
    ldr     r3, =0x80
    strb    r3, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Assert ALE.
    //
    bl      NANDAssertALE

    //
    // Write the page address.
    //
    ldr     r3, =0x00
    strb    r3, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // The remainder of the write sequence is identical to the four address
    // cycle case, so branch to that code.
    //
    b       FinishWrite512

//****************************************************************************
//
// NANDWrite_512_4 writes 512 bytes of data and 16 bytes of redundant data to a
// NAND FLASH device which has 512 bytes per page and uses four address cycles
// for writing pages.
//
//****************************************************************************
    _EXPORT_ NANDWrite_512_4
NANDWrite_512_4 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r6, lr}

    //
    // Prepare to access the NAND FLASH.  This also asserts CLE.
    //
    bl      NANDSetup

    //
    // Write the 'serial data input' command.
    //
    ldr     r3, =0x00
    strb    r3, [r0]
    ldr     r3, =0x80
    strb    r3, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Assert ALE.
    //
    bl      NANDAssertALE

    //
    // Write the page address.
    //
    ldr     r3, =0x00
    strb    r3, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
FinishWrite512 _LABEL_
    bl      NANDDeassertALE

    //
    // Write 512 bytes and 16 redundant bytes to this page.
    //
    ldr     r3, =528
NANDWrite_512_4_1 _LABEL_
        ldrb    r6, [r2], _CONST_ 1
        strb    r6, [r0]
        subs    r3, r3, _CONST_ 1
        bne     NANDWrite_512_4_1

    //
    // Assert CLE.
    //
    bl      NANDAssertCLE

    //
    // Write the 'auto program' command.
    //
    ldr     r3, =0x10
    strb    r3, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Restore the non-volatile registers and return to the caller.
    //
    ldmia   r13!, {r4-r6, pc}

//****************************************************************************
//
// NANDErase_32_3 erases a block of a NAND FLASH device which has 32 pages per
// block and uses two address cycles for erasing blocks.
//
//****************************************************************************
    _EXPORT_ NANDErase_32_3
NANDErase_32_3 _LABEL_
    //
    // Erasing a NAND block which has 32 pages is identical to erasing a NAND
    // block which has 16 pages, except the block address is shifted one more
    // bit to the left.  So, shift the block address by one and fall into the
    // code for erasing a NAND block with 16 pages.
    //
    mov     r1, r1, lsl _CONST_ 1

//****************************************************************************
//
// NANDErase_16 erases a block of a NAND FLASH device which has 16 pages per
// block.
//
//****************************************************************************
    _EXPORT_ NANDErase_16
NANDErase_16 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r5, lr}

    //
    // Prepare to access the NAND FLASH.  This also asserts CLE.
    //
    bl      NANDSetup

    //
    // Write the 'auto block erase setup' command.
    //
    ldr     r2, =0x60
    strb    r2, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Assert ALE.
    //
    bl      NANDAssertALE

    //
    // Write the page address.
    //
    mov     r1, r1, lsl _CONST_ 4
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // The remainder of the erase sequence is identical to the four address
    // cycle case, so branch to that code.
    //
    b       FinishErase

//****************************************************************************
//
// NANDErase_32_4 erases a block of a NAND FLASH device which has 32 pages per
// block and uses three address cycles for erasing blocks.
//
//****************************************************************************
    _EXPORT_ NANDErase_32_4
NANDErase_32_4 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r5, lr}

    //
    // Prepare to access the NAND FLASH.  This also asserts CLE.
    //
    bl      NANDSetup

    //
    // Write the 'auto block erase setup' command.
    //
    ldr     r2, =0x60
    strb    r2, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Assert ALE.
    //
    bl      NANDAssertALE

    //
    // Write the page address.
    //
    mov     r1, r1, lsl _CONST_ 5
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
FinishErase _LABEL_
    bl      NANDDeassertALE

    //
    // Assert CLE.
    //
    bl      NANDAssertCLE

    //
    // Write the 'erase' command.
    //
    ldr     r2, =0xd0
    strb    r2, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Restore the non-volatile registers and return to the caller.
    //
    ldmia   r13!, {r4-r5, pc}

//****************************************************************************
//
// NANDWaitTilNotBusy waits until the NAND device is not busy (i.e. a
// previously started program or erase operation has completed).
//
//****************************************************************************
    _EXPORT_ NANDWaitTilNotBusy
NANDWaitTilNotBusy _LABEL_

#ifdef HwPortABCD_NAND_Ready

    //
    // Load a pointer to the internal registers.
    //
    ldr     r1, =0x80000000

    //
    // Wait for the erase to complete by sampling the READY/nBUSY signal from
    // the NAND device.
    //
NANDWaitTilNotBusy_1 _LABEL_
        ldr     r2, [r1]
        tst     r2, _CONST_ HwPortABCD_NAND_Ready
        beq     NANDWaitTilNotBusy_1

    //
    // Return to the caller.
    //
    mov     pc, lr

#else

    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r5, lr}

    //
    // Prepare to access the NAND FLASH.  This also asserts CLE.
    //
    bl      NANDSetup

    //
    // Write the 'read status' command.
    //
    ldr     r1, =0x70
    strb    r1, [r0]

    //
    // Deassert CLE.
    //
    bl      NANDDeassertCLE

    //
    // Wait until the operation has completed.
    //
    ldr     r1, =0x40
NANDWaitTilNotBusy_1 _LABEL_
        ldrb    r2, [r0]
        tst     r2, r1
        beq     NANDWaitTilNotBusy_1

    //
    // Restore the non-volatile registers and return to the caller.
    //
    ldmia   r13!, {r4-r5, pc}

#endif

    _END_
