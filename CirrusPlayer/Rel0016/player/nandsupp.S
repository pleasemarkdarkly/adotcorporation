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
// NANDGetID read the ID from the NAND FLASH device.
//
//****************************************************************************
    _EXPORT_ NANDGetID
NANDGetID _LABEL_
    //
    // Load a pointer to the internal registers.
    //
    ldr     r1, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r2, [r1]

    //
    // Assert CLE.
    //
    orr     r2, r2, _CONST_ HwPortABCD_NAND_CLE
    str     r2, [r1]

    //
    // Write the 'read id' command.
    //
    ldr     r3, =0x90
    strb    r3, [r0]

    //
    // Deassert CLE.
    //
    bic     r2, r2, _CONST_ HwPortABCD_NAND_CLE
    str     r2, [r1]

    //
    // Assert ALE.
    //
    orr     r2, r2, _CONST_ HwPortABCD_NAND_ALE
    str     r2, [r1]

    //
    // Write the address (zero).
    //
    ldr     r3, =0x00
    strb    r3, [r0]

    //
    // Deassert ALE.
    //
    bic     r2, r2, _CONST_ HwPortABCD_NAND_ALE
    str     r2, [r1]

    //
    // Delay for ~100ns.
    //
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    //
    // Read the manufacturer ID.
    //
    ldrb    r3, [r0]

    //
    // Read the device ID.
    //
    ldrb    r0, [r0]

    //
    // Return to the caller.
    //
    mov     pc, lr

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
    stmdb   r13!, {r4-r8}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r4, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r5, [r4]

    //
    // Assert CLE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Write the 'read1' command.
    //
    ldr     r6, =0x00
    strb    r6, [r0]

    //
    // Deassert CLE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Assert ALE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

    //
    // Write the page address.
    //
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

#ifdef HwPortABCD_NAND_Ready

    //
    // Wait for the page to copy from the cell array by sampling the
    // READY/nBUSY signal from the NAND device.
    //
NANDRead_512_3_1 _LABEL_
        ldr     r6, [r4]
        tst     r6, _CONST_ HwPortABCD_NAND_Ready
        beq     NANDRead_512_3_1

#else

    //
    // Wait for the page to copy from the cell array by delaying.
    //
    ldr     r6, =DELAY
NANDRead_512_3_1 _LABEL_
        subs    r6, r6, _CONST_ 1
        bne     NANDRead_512_3_1

#endif

    //
    // Read the 512 bytes from this page.
    //
    ldr     r6, =512
NANDRead_512_3_2 _LABEL_
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 24
        str     r8, [r2], _CONST_ 4
        subs    r6, r6, _CONST_ 4
        bne     NANDRead_512_3_2

    //
    // Read the 16 redundant bytes from this page.
    //
    ldr     r6, =16
NANDRead_512_3_3 _LABEL_
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 24
        str     r8, [r3], _CONST_ 4
        subs    r6, r6, _CONST_ 4
        bne     NANDRead_512_3_3

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4-r8}

    //
    // Return to the caller.
    //
    mov     pc, lr

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
    stmdb   r13!, {r4-r8}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r4, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r5, [r4]

    //
    // Assert CLE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Write the 'read1' command.
    //
    ldr     r6, =0x00
    strb    r6, [r0]

    //
    // Deassert CLE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Assert ALE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

    //
    // Write the page address.
    //
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

#ifdef HwPortABCD_NAND_Ready

    //
    // Wait for the page to copy from the cell array by sampling the
    // READY/nBUSY signal from the NAND device.
    //
NANDRead_512_4_1 _LABEL_
        ldr     r6, [r4]
        tst     r6, _CONST_ HwPortABCD_NAND_Ready
        beq     NANDRead_512_4_1

#else

    //
    // Wait for the page to copy from the cell array by delaying.
    //
    ldr     r6, =DELAY
NANDRead_512_4_1 _LABEL_
        subs    r6, r6, _CONST_ 1
        bne     NANDRead_512_4_1

#endif

    //
    // Read the 512 bytes from this page.
    //
    ldr     r6, =512
NANDRead_512_4_2 _LABEL_
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 24
        str     r8, [r2], _CONST_ 4
        subs    r6, r6, _CONST_ 4
        bne     NANDRead_512_4_2

    //
    // Read the 16 redundant bytes from this page.
    //
    ldr     r6, =16
NANDRead_512_4_3 _LABEL_
        ldrb    r8, [r0]
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 8
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 16
        ldrb    r7, [r0]
        orr     r8, r8, r7, lsl _CONST_ 24
        str     r8, [r3], _CONST_ 4
        subs    r6, r6, _CONST_ 4
        bne     NANDRead_512_4_3

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4-r8}

    //
    // Return to the caller.
    //
    mov     pc, lr

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
    stmdb   r13!, {r4-r7}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r4, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r5, [r4]

    //
    // Assert CLE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Write the 'serial data input' command.
    //
    ldr     r6, =0x00
    strb    r6, [r0]
    ldr     r6, =0x80
    strb    r6, [r0]

    //
    // Deassert CLE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Assert ALE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

    //
    // Write the page address.
    //
    ldr     r6, =0x00
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

    //
    // Write 512 bytes to this page.
    //
    ldr     r6, =512
NANDWrite_512_3_1 _LABEL_
        ldr     r7, [r2], _CONST_ 4
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        subs    r6, r6, _CONST_ 4
        bne     NANDWrite_512_3_1

    //
    // Write 16 redundant bytes to this page.
    //
    ldr     r6, =16
NANDWrite_512_3_2 _LABEL_
        ldr     r7, [r3], _CONST_ 4
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        subs    r6, r6, _CONST_ 4
        bne     NANDWrite_512_3_2

    //
    // Assert CLE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Write the 'auto program' command.
    //
    ldr     r6, =0x10
    strb    r6, [r0]

    //
    // Deassert CLE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4-r7}

    //
    // Return to the caller.
    //
    mov     pc, lr

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
    stmdb   r13!, {r4-r7}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r4, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r5, [r4]

    //
    // Assert CLE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Write the 'serial data input' command.
    //
    ldr     r6, =0x00
    strb    r6, [r0]
    ldr     r6, =0x80
    strb    r6, [r0]

    //
    // Deassert CLE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Assert ALE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

    //
    // Write the page address.
    //
    ldr     r6, =0x00
    strb    r6, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_ALE
    str     r5, [r4]

    //
    // Write 512 bytes to this page.
    //
    ldr     r6, =512
NANDWrite_512_4_1 _LABEL_
        ldr     r7, [r2], _CONST_ 4
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        subs    r6, r6, _CONST_ 4
        bne     NANDWrite_512_4_1

    //
    // Write 16 redundant bytes to this page.
    //
    ldr     r6, =16
NANDWrite_512_4_2 _LABEL_
        ldr     r7, [r3], _CONST_ 4
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        mov     r7, r7, lsr _CONST_ 8
        strb    r7, [r0]
        subs    r6, r6, _CONST_ 4
        bne     NANDWrite_512_4_2

    //
    // Assert CLE.
    //
    orr     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Write the 'auto program' command.
    //
    ldr     r6, =0x10
    strb    r6, [r0]

    //
    // Deassert CLE.
    //
    bic     r5, r5, _CONST_ HwPortABCD_NAND_CLE
    str     r5, [r4]

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4-r7}

    //
    // Return to the caller.
    //
    mov     pc, lr

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
    stmdb   r13!, {r4}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r2, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r3, [r2]

    //
    // Assert CLE.
    //
    orr     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Write the 'auto block erase setup' command.
    //
    ldr     r4, =0x60
    strb    r4, [r0]

    //
    // Deassert CLE.
    //
    bic     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Assert ALE.
    //
    orr     r3, r3, _CONST_ HwPortABCD_NAND_ALE
    str     r3, [r2]

    //
    // Write the page address.
    //
    mov     r1, r1, lsl _CONST_ 4
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r3, r3, _CONST_ HwPortABCD_NAND_ALE
    str     r3, [r2]

    //
    // Assert CLE.
    //
    orr     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Write the 'erase' command.
    //
    ldr     r4, =0xd0
    strb    r4, [r0]

    //
    // Deassert CLE.
    //
    bic     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4}

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// NANDErase_32_3 erases a block of a NAND FLASH device which has 32 pages per
// block and uses two address cycles for the erasing blocks.
//
//****************************************************************************
    _EXPORT_ NANDErase_32_3
NANDErase_32_3 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r2, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r3, [r2]

    //
    // Assert CLE.
    //
    orr     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Write the 'auto block erase setup' command.
    //
    ldr     r4, =0x60
    strb    r4, [r0]

    //
    // Deassert CLE.
    //
    bic     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Assert ALE.
    //
    orr     r3, r3, _CONST_ HwPortABCD_NAND_ALE
    str     r3, [r2]

    //
    // Write the page address.
    //
    mov     r1, r1, lsl _CONST_ 5
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r3, r3, _CONST_ HwPortABCD_NAND_ALE
    str     r3, [r2]

    //
    // Assert CLE.
    //
    orr     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Write the 'erase' command.
    //
    ldr     r4, =0xd0
    strb    r4, [r0]

    //
    // Deassert CLE.
    //
    bic     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4}

    //
    // Return to the caller.
    //
    mov     pc, lr

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
    stmdb   r13!, {r4}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r2, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r3, [r2]

    //
    // Assert CLE.
    //
    orr     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Write the 'auto block erase setup' command.
    //
    ldr     r4, =0x60
    strb    r4, [r0]

    //
    // Deassert CLE.
    //
    bic     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Assert ALE.
    //
    orr     r3, r3, _CONST_ HwPortABCD_NAND_ALE
    str     r3, [r2]

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
    bic     r3, r3, _CONST_ HwPortABCD_NAND_ALE
    str     r3, [r2]

    //
    // Assert CLE.
    //
    orr     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Write the 'erase' command.
    //
    ldr     r4, =0xd0
    strb    r4, [r0]

    //
    // Deassert CLE.
    //
    bic     r3, r3, _CONST_ HwPortABCD_NAND_CLE
    str     r3, [r2]

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4}

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// NANDReadRedt_512_3 reads 16 bytes of redundant data from a NAND FLASH device
// which has 512 bytes per page and uses three address cycles.
//
//****************************************************************************
    _EXPORT_ NANDReadRedt_512_3
NANDReadRedt_512_3 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r7}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r3, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r4, [r3]

    //
    // Assert CLE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Write the 'read3' command.
    //
    ldr     r5, =0x50
    strb    r5, [r0]

    //
    // Deassert CLE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Assert ALE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_ALE
    str     r4, [r3]

    //
    // Write the page address.
    //
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_ALE
    str     r4, [r3]

#ifdef HwPortABCD_NAND_Ready

    //
    // Wait for the page to copy from the cell array by sampling the
    // READY/nBUSY signal from the NAND device.
    //
NANDReadRedt_512_3_1 _LABEL_
        ldr     r5, [r3]
        tst     r5, _CONST_ HwPortABCD_NAND_Ready
        beq     NANDReadRedt_512_3_1

#else

    //
    // Wait for the page to copy from the cell array by delaying.
    //
    ldr     r5, =DELAY
NANDReadRedt_512_3_1 _LABEL_
        subs    r5, r5, _CONST_ 1
        bne     NANDReadRedt_512_3_1

#endif

    //
    // Read the 16 redundant bytes from this page.
    //
    ldr     r5, =16
NANDReadRedt_512_3_2 _LABEL_
        ldrb    r7, [r0]
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl _CONST_ 8
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl _CONST_ 16
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl _CONST_ 24
        str     r7, [r2], _CONST_ 4
        subs    r5, r5, _CONST_ 4
        bne     NANDReadRedt_512_3_2

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4-r7}

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// NANDReadRedt_512_4 reads 16 bytes of redundant data from a NAND FLASH device
// which has 512 bytes per page and uses four address cycles.
//
//****************************************************************************
    _EXPORT_ NANDReadRedt_512_4
NANDReadRedt_512_4 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r7}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r3, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r4, [r3]

    //
    // Assert CLE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Write the 'read3' command.
    //
    ldr     r5, =0x50
    strb    r5, [r0]

    //
    // Deassert CLE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Assert ALE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_ALE
    str     r4, [r3]

    //
    // Write the page address.
    //
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_ALE
    str     r4, [r3]

#ifdef HwPortABCD_NAND_Ready

    //
    // Wait for the page to copy from the cell array by sampling the
    // READY/nBUSY signal from the NAND device.
    //
NANDReadRedt_512_4_1 _LABEL_
        ldr     r5, [r3]
        tst     r5, _CONST_ HwPortABCD_NAND_Ready
        beq     NANDReadRedt_512_4_1

#else

    //
    // Wait for the page to copy from the cell array by delaying.
    //
    ldr     r5, =DELAY
NANDReadRedt_512_4_1 _LABEL_
        subs    r5, r5, _CONST_ 1
        bne     NANDReadRedt_512_4_1

#endif

    //
    // Read the 16 redundant bytes from this page.
    //
    ldr     r5, =16
NANDReadRedt_512_4_2 _LABEL_
        ldrb    r7, [r0]
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl _CONST_ 8
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl _CONST_ 16
        ldrb    r6, [r0]
        orr     r7, r7, r6, lsl _CONST_ 24
        str     r7, [r2], _CONST_ 4
        subs    r5, r5, _CONST_ 4
        bne     NANDReadRedt_512_4_2

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4-r7}

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// NANDWriteRedt_512_3 writes 16 bytes of redundant data to a NAND FLASH device
// which has 512 bytes per page and uses three address cycles for writing
// pages.
//
//****************************************************************************
    _EXPORT_ NANDWriteRedt_512_3
NANDWriteRedt_512_3 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r6}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r3, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r4, [r3]

    //
    // Assert CLE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Write the 'serial data input' command.
    //
    ldr     r5, =0x50
    strb    r5, [r0]
    ldr     r5, =0x80
    strb    r5, [r0]

    //
    // Deassert CLE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Assert ALE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_ALE
    str     r4, [r3]

    //
    // Write the page address.
    //
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_ALE
    str     r4, [r3]

    //
    // Write 16 redundant bytes to this page.
    //
    ldr     r5, =16
NANDWriteRedt_512_3_1 _LABEL_
        ldr     r6, [r2], _CONST_ 4
        strb    r6, [r0]
        mov     r6, r6, lsr _CONST_ 8
        strb    r6, [r0]
        mov     r6, r6, lsr _CONST_ 8
        strb    r6, [r0]
        mov     r6, r6, lsr _CONST_ 8
        strb    r6, [r0]
        subs    r5, r5, _CONST_ 4
        bne     NANDWriteRedt_512_3_1

    //
    // Assert CLE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Write the 'auto program' command.
    //
    ldr     r5, =0x10
    strb    r5, [r0]

    //
    // Deassert CLE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4-r6}

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// NANDWriteRedt_512_4 writes 16 bytes of redundant data to a NAND FLASH device
// which has 512 bytes per page and uses four address cycles for writing pages.
//
//****************************************************************************
    _EXPORT_ NANDWriteRedt_512_4
NANDWriteRedt_512_4 _LABEL_
    //
    // Save the non-volatile registers which we will use.
    //
    stmdb   r13!, {r4-r6}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r3, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r4, [r3]

    //
    // Assert CLE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Write the 'serial data input' command.
    //
    ldr     r5, =0x00
    strb    r5, [r0]
    ldr     r5, =0x80
    strb    r5, [r0]

    //
    // Deassert CLE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Assert ALE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_ALE
    str     r4, [r3]

    //
    // Write the page address.
    //
    ldr     r5, =0x00
    strb    r5, [r0]
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]
    mov     r1, r1, lsr _CONST_ 8
    strb    r1, [r0]

    //
    // Deassert ALE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_ALE
    str     r4, [r3]

    //
    // Write 16 redundant bytes to this page.
    //
    ldr     r5, =16
NANDWriteRedt_512_4_1 _LABEL_
        ldr     r6, [r2], _CONST_ 4
        strb    r6, [r0]
        mov     r6, r6, lsr _CONST_ 8
        strb    r6, [r0]
        mov     r6, r6, lsr _CONST_ 8
        strb    r6, [r0]
        mov     r6, r6, lsr _CONST_ 8
        strb    r6, [r0]
        subs    r5, r5, _CONST_ 4
        bne     NANDWriteRedt_512_4_1

    //
    // Assert CLE.
    //
    orr     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Write the 'auto program' command.
    //
    ldr     r5, =0x10
    strb    r5, [r0]

    //
    // Deassert CLE.
    //
    bic     r4, r4, _CONST_ HwPortABCD_NAND_CLE
    str     r4, [r3]

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4-r6}

    //
    // Return to the caller.
    //
    mov     pc, lr

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
    ldr     r1, =HwBaseAddress

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
    stmdb   r13!, {r4}

    //
    // Load a pointer to the internal registers.
    //
    ldr     r1, =HwBaseAddress

    //
    // Get the current value of the GPIO pins.
    //
    ldr     r2, [r1]

    //
    // Assert CLE.
    //
    orr     r2, r2, _CONST_ HwPortABCD_NAND_CLE
    str     r2, [r1]

    //
    // Write the 'read status' command.
    //
    ldr     r3, =0x70
    strb    r3, [r0]

    //
    // Deassert CLE.
    //
    bic     r2, r2, _CONST_ HwPortABCD_NAND_CLE
    str     r2, [r1]

    //
    // Wait until the operation has completed.
    //
    ldr     r4, =0x00000040
NANDWaitTilNotBusy_1 _LABEL_
        ldrb    r3, [r0]
        and     r3, r3, r4
        cmp     r3, r4
        bne     NANDWaitTilNotBusy_1

    //
    // Restore the non-volatile registers.
    //
    ldmia   r13!, {r4}

    //
    // Return to the caller.
    //
    mov     pc, lr

#endif

    _END_
