//****************************************************************************
//
// FLASHER.S - Code which is loaded into the EP72XX and EP73XX via the boot
//             ROM to program the FLASH.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************

#include "../asmdefs.h"

//****************************************************************************
//
// The read-only code area.
//
//****************************************************************************
    _TEXT_

    //
    // This is the entry point of the program.
    //
    _ENTRY_

    //
    // The processor is in ARM mode when we are called, but we want to run in
    // Thumb mode.  So, switch to thumb mode.  Note that this code sequence
    // will not work if any code or data is inserted between the "orr" and the
    // "thumb_entry" label.
    //
    orr     r0, pc, _CONST_ 0x01
    bx      r0

    //
    // Switch the assembler into Thumb mode.
    //
    _CODE16_

    //
    // Now we're in thumb mode.
    //
_THUMB_LABEL_
thumb_entry _LABEL_

    //
    // Set up the stack pointer.
    //
    mov     r0, _CONST_ 0x10
    lsl     r0, r0, _CONST_ 16
    mov     r1, _CONST_ 0x96
    orr     r0, r0, r1
    lsl     r0, r0, _CONST_ 8
    mov     r13, r0

    //
    // Check the FLASH to see if it is something that we recognize and can
    // program.
    //
    bl      check_flash

    //
    // See if we recognize the FLASH.
    //
    mov     r1, _CONST_ 0x01
    cmp     r0, r1
    beq     loop

    //
    // Send a 'X' to the host to indicate that we will not be able to program
    // the FLASH.
    //
    mov     r0, _CONST_ 0x58 // 'X'
    bl      SendChar

    //
    // Loop forever.
    //
    b       .

    //
    // Loop forever reading commands from the host and performing them.
    //
_THUMB_LABEL_
loop _LABEL_
        //
        // Write the command prompt to the host.
        //
        mov     r0, _CONST_ 0x3f // '?'
        bl      SendChar

        //
        // Read a command from the host.
        //
        bl      ReceiveChar

        //
        // Determine how to handle this command.  Is this a 'B'?
        //
        cmp     r0, _CONST_ 0x42 // 'B'
        bne     loop1

            //
            // Change the baud rate of the serial port.
            //
            bl      SetBaud
            b       loop

       //
       // Is this a 'F'?
       //
_THUMB_LABEL_
loop1 _LABEL_
       cmp     r0, _CONST_ 0x46 // 'F'
       bne     loop

           //
           // Program data into the on-board FLASH.
           //
           bl      ProgramFlash
           b       loop

//****************************************************************************
//
// Waits for an internal erase or program operation to complete on a 16-bit
// wide Intel FLASH (B3, C3, or J3).
//
//****************************************************************************
_THUMB_LABEL_
intel_wait_16 _LABEL_
    //
    // Load the bit mask for which we are waiting.
    //
    mov     r1, _CONST_ 0x80

    //
    // Read from the FLASH memory until bit 7 is one.
    //
_THUMB_LABEL_
intel_wait_16_loop _LABEL_
        ldrh    r2, [r0]
        and     r2, r2, r1
        cmp     r2, r1
        bne     intel_wait_16_loop

    //
    // Clear the status register in the FLASH.
    //
    mov     r1, _CONST_ 0x50
    strh    r1, [r0]

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// Waits for an internal erase or program operation to complete on a 16-bit
// wide Intel FLASH (B3, C3, or J3) with a reversed data bus.
//
//****************************************************************************
_THUMB_LABEL_
intel_wait_16_rev _LABEL_
    //
    // Load the bit mask for which we are waiting.
    //
    mov     r1, _CONST_ 0x01
    lsl     r1, r1, _CONST_ 8

    //
    // Read from the FLASH memory until bit 7 is one.
    //
_THUMB_LABEL_
intel_wait_16_rev_loop _LABEL_
        ldrh    r2, [r0]
        and     r2, r2, r1
        cmp     r2, r1
        bne     intel_wait_16_rev_loop

    //
    // Clear the status register in the FLASH.
    //
    mov     r1, _CONST_ 0x0a
    lsl     r1, r1, _CONST_ 8
    strh    r1, [r0]

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// Waits for an internal erase or program operation to complete on a 32-bit
// wide Intel FLASH (B3, C3, or J3).
//
//****************************************************************************
_THUMB_LABEL_
intel_wait_32 _LABEL_
    //
    // Load the bit mask for which we are waiting.
    //
    mov     r2, _CONST_ 0x80
    mov     r1, r2
    lsl     r1, r1, _CONST_ 16
    orr     r1, r1, r2

    //
    // Read from the FLASH memory until bit 7 is one.
    //
_THUMB_LABEL_
intel_wait_32_loop _LABEL_
        ldr     r2, [r0]
        and     r2, r2, r1
        cmp     r2, r1
        bne     intel_wait_32_loop

    //
    // Clear the status register in the FLASH.
    //
    mov     r2, _CONST_ 0x50
    mov     r1, r2
    lsl     r1, r1, _CONST_ 16
    orr     r1, r1, r2
    str     r1, [r0]

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// The routine to erase a Intel B3 FLASH.  This must be a branch to the routine
// for a single 16-bit wide FLASH, immediately followed by the routine for a
// 32-bit wide FLASH (i.e. two 16-bit wide FLASHes).
//
//****************************************************************************
_THUMB_LABEL_
intel_b3_erase _LABEL_
    b       intel_b3_erase_16

//****************************************************************************
//
// The routine for erasing a pair of 16-bit wide Intel B3 FLASHes in a 32-bit
// configuration.
//
//****************************************************************************
_THUMB_LABEL_
intel_b3_erase_32 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the length of the FLASH to be erased.
    //
    mov     r4, r0

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Get the commands to erase a block of the FLASH.
    //
    mov     r5, _CONST_ 0x20
    mov     r6, r5
    lsl     r6, r6, _CONST_ 16
    orr     r6, r6, r5
    mov     r5, _CONST_ 0xd0
    mov     r7, r5
    lsl     r7, r7, _CONST_ 16
    orr     r7, r7, r5

    //
    // Start erasing the FLASH from block zero.
    //
    mov     r5, _CONST_ 0x00
_THUMB_LABEL_
intel_b3_erase_32_block _LABEL_
        //
        // See if we have erased all the required blocks.
        //
        cmp     r5, r4
        bge     intel_b3_erase_32_done

        //
        // Erase this block.
        //
        str     r6, [r0, r5]
        str     r7, [r0, r5]

        //
        // Wait until the erase has completed.
        //
        bl      intel_wait_32

        //
        // Increment the block address.  The first 8 blocks of the FLASH have a
        // block size of 16K and the remaining blocks have a size of 128K.  So,
        // if the offset is less than 128K, increment it by 16K, otherwise
        // increment it by 128K.
        //
        mov     r1, _CONST_ 0x02
        lsl     r1, r1, _CONST_ 16
        cmp     r5, r1
        blt     intel_b3_erase_32_small
        mov     r1, _CONST_ 0x20
        b       intel_b3_erase_32_increment
_THUMB_LABEL_
intel_b3_erase_32_small _LABEL_
        mov     r1, _CONST_ 0x04
_THUMB_LABEL_
intel_b3_erase_32_increment _LABEL_
        lsl     r1, r1, _CONST_ 12
        add     r5, r5, r1

        //
        // Loop back.
        //
        b       intel_b3_erase_32_block

    //
    // We're done erasing the FLASH, so return to the caller.
    //
_THUMB_LABEL_
intel_b3_erase_32_done _LABEL_
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine for erasing a single 16-bit wide Intel B3 FLASH.
//
//****************************************************************************
_THUMB_LABEL_
intel_b3_erase_16 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the length of the FLASH to be erased.
    //
    mov     r4, r0

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Get the commands to erase a block of the FLASH.
    //
    mov     r6, _CONST_ 0x20
    mov     r7, _CONST_ 0xd0

    //
    // Start erasing the FLASH from block zero.
    //
    mov     r5, _CONST_ 0x00
_THUMB_LABEL_
intel_b3_erase_16_block _LABEL_
        //
        // See if we have erased all the required blocks.
        //
        cmp     r5, r4
        bge     intel_b3_erase_16_done

        //
        // Erase this block.
        //
        strh    r6, [r0, r5]
        strh    r7, [r0, r5]

        //
        // Wait until the erase has completed.
        //
        bl      intel_wait_16 

        //
        // Increment the block address.  The first 8 blocks of the FLASH have a
        // block size of 8K and the remaining blocks have a size of 64K.  So,
        // if the offset is less than 64K, increment it by 8K, otherwise
        // increment it by 64K.
        //
        mov     r1, _CONST_ 0x01
        lsl     r1, r1, _CONST_ 16
        cmp     r5, r1
        blt     intel_b3_erase_16_small
        mov     r1, _CONST_ 0x10
        b       intel_b3_erase_16_increment
_THUMB_LABEL_
intel_b3_erase_16_small _LABEL_
        mov     r1, _CONST_ 0x02
_THUMB_LABEL_
intel_b3_erase_16_increment _LABEL_
        lsl     r1, r1, _CONST_ 12
        add     r5, r5, r1

        //
        // Loop back.
        //
        b       intel_b3_erase_16_block

    //
    // We're done erasing the FLASH, so return to the caller.
    //
_THUMB_LABEL_
intel_b3_erase_16_done _LABEL_
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine to program a Intel B3/C3 FLASH.  This must be a branch to the
// routine for a single 16-bit wide FLASH, immediately followed by the routine
// for a 32-bit wide FLASH (i.e. two 16-bit wide FLASHes).
//
//****************************************************************************
_THUMB_LABEL_
intel_b3_c3_program _LABEL_
    b       intel_b3_c3_program_16

//****************************************************************************
//
// The routine for programming a pair of 16-bit wide Intel B3/C3 FLASHes in a
// 32-bit configuration.
//
//****************************************************************************
_THUMB_LABEL_
intel_b3_c3_program_32 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the offset, length, and pointer to the data to be programmed into
    // the FLASH.
    //
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Get the command to program a block of the FLASH.
    //
    mov     r1, _CONST_ 0x40
    mov     r7, r1
    lsl     r7, r7, _CONST_ 16
    orr     r7, r7, r1

    //
    // Program the FLASH a word at a time.
    //
_THUMB_LABEL_
intel_b3_c3_program_32_word _LABEL_
        //
        // See if we have programmed all the required words.
        //
        cmp     r6, _CONST_ 0
        beq     intel_b3_c3_program_32_done

        //
        // Load the next word to be programmed.
        //
        ldr     r1, [r4]
        add     r4, r4, _CONST_ 4

        //
        // Program the next word.
        //
        str     r7, [r0, r5]
        str     r1, [r0, r5]

        //
        // Wait until the word has been programmed.
        //
        bl      intel_wait_32

        //
        // Increment the address to be programmed.
        //
        add     r5, r5, _CONST_ 4

        //
        // Decrement the count of words to be programmed.
        //
        sub     r6, r6, _CONST_ 4

        //
        // Loop back.
        //
        b       intel_b3_c3_program_32_word

    //
    // We're done programming the FLASH, so put the FLASH into read array mode.
    //
_THUMB_LABEL_
intel_b3_c3_program_32_done _LABEL_
    mov     r2, _CONST_ 0xff
    mov     r1, r2
    lsl     r1, r1, _CONST_ 16
    orr     r1, r1, r2
    str     r1, [r0]

    //
    // Return to the caller.
    //
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine for programming a 16-bit wide Intel B3/C3 FLASH.
//
//****************************************************************************
_THUMB_LABEL_
intel_b3_c3_program_16 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the offset, length, and pointer to the data to be programmed into
    // the FLASH.
    //
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Get the command to program a block of the FLASH.
    //
    mov     r7, _CONST_ 0x40

    //
    // Program the FLASH a word at a time.
    //
_THUMB_LABEL_
intel_b3_c3_program_16_word _LABEL_
        //
        // See if we have programmed all the required words.
        //
        cmp     r6, _CONST_ 0
        beq     intel_b3_c3_program_16_done

        //
        // Load the next word to be programmed.
        //
        ldrh    r1, [r4]
        add     r4, r4, _CONST_ 2

        //
        // Program the next word.
        //
        strh    r7, [r0, r5]
        strh    r1, [r0, r5]

        //
        // Wait until the word has been programmed.
        //
        bl      intel_wait_16

        //
        // Increment the address to be programmed.
        //
        add     r5, r5, _CONST_ 2

        //
        // Decrement the count of words to be programmed.
        //
        sub     r6, r6, _CONST_ 2

        //
        // Loop back.
        //
        b       intel_b3_c3_program_16_word

    //
    // We're done programming the FLASH, so put the FLASH into read array mode.
    //
_THUMB_LABEL_
intel_b3_c3_program_16_done _LABEL_
    mov     r1, _CONST_ 0xff
    str     r1, [r0]

    //
    // Return to the caller.
    //
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine to erase a Intel C3 FLASH.  This must be a branch to the routine
// for a single 16-bit wide FLASH, immediately followed by the routine for a
// 32-bit wide FLASH (i.e. two 16-bit wide FLASHes).
//
//****************************************************************************
_THUMB_LABEL_
intel_c3_erase _LABEL_
    b       intel_c3_erase_16

//****************************************************************************
//
// The routine for erasing a pair of 16-bit wide Intel C3 FLASHes in a 32-bit
// configuration.
//
//****************************************************************************
_THUMB_LABEL_
intel_c3_erase_32 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the length of the FLASH to be erased.
    //
    mov     r4, r0

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Get the command to unlock a block of the FLASH.
    //
    mov     r5, _CONST_ 0x60
    mov     r3, r5
    lsl     r3, r3, _CONST_ 16
    orr     r3, r3, r5

    //
    // Get the commands to erase a block of the FLASH.
    //
    mov     r5, _CONST_ 0x20
    mov     r6, r5
    lsl     r6, r6, _CONST_ 16
    orr     r6, r6, r5
    mov     r5, _CONST_ 0xd0
    mov     r7, r5
    lsl     r7, r7, _CONST_ 16
    orr     r7, r7, r5

    //
    // Start erasing the FLASH from block zero.
    //
    mov     r5, _CONST_ 0x00
_THUMB_LABEL_
intel_c3_erase_32_block _LABEL_
        //
        // See if we have erased all the required blocks.
        //
        cmp     r5, r4
        bge     intel_c3_erase_32_done

        //
        // Unlock this block.
        //
        str     r3, [r0, r5]
        str     r7, [r0, r5]

        //
        // Erase this block.
        //
        str     r6, [r0, r5]
        str     r7, [r0, r5]

        //
        // Wait until the erase has completed.
        //
        bl      intel_wait_32

        //
        // Increment the block address.  The first 8 blocks of the FLASH have a
        // block size of 16K and the remaining blocks have a size of 128K.  So,
        // if the offset is less than 128K, increment it by 16K, otherwise
        // increment it by 128K.
        //
        mov     r1, _CONST_ 0x02
        lsl     r1, r1, _CONST_ 16
        cmp     r5, r1
        blt     intel_c3_erase_32_small
        mov     r1, _CONST_ 0x20
        b       intel_c3_erase_32_increment
_THUMB_LABEL_
intel_c3_erase_32_small _LABEL_
        mov     r1, _CONST_ 0x04
_THUMB_LABEL_
intel_c3_erase_32_increment _LABEL_
        lsl     r1, r1, _CONST_ 12
        add     r5, r5, r1

        //
        // Loop back.
        //
        b       intel_c3_erase_32_block

    //
    // We're done erasing the FLASH, so return to the caller.
    //
_THUMB_LABEL_
intel_c3_erase_32_done _LABEL_
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine for erasing a single 16-bit wide Intel C3 FLASH.
//
//****************************************************************************
_THUMB_LABEL_
intel_c3_erase_16 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the length of the FLASH to be erased.
    //
    mov     r4, r0

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Get the command to unlock a block of the FLASH.
    //
    mov     r3, _CONST_ 0x60

    //
    // Get the commands to erase a block of the FLASH.
    //
    mov     r6, _CONST_ 0x20
    mov     r7, _CONST_ 0xd0

    //
    // Start erasing the FLASH from block zero.
    //
    mov     r5, _CONST_ 0x00
_THUMB_LABEL_
intel_c3_erase_16_block _LABEL_
        //
        // See if we have erased all the required blocks.
        //
        cmp     r5, r4
        bge     intel_c3_erase_16_done

        //
        // Unlock this block.
        //
        strh    r3, [r0, r5]
        strh    r7, [r0, r5]

        //
        // Erase this block.
        //
        strh    r6, [r0, r5]
        strh    r7, [r0, r5]

        //
        // Wait until the erase has completed.
        //
        bl      intel_wait_16

        //
        // Increment the block address.  The first 8 blocks of the FLASH have a
        // block size of 8K and the remaining blocks have a size of 64K.  So,
        // if the offset is less than 64K, increment it by 8K, otherwise
        // increment it by 64K.
        //
        mov     r1, _CONST_ 0x01
        lsl     r1, r1, _CONST_ 16
        cmp     r5, r1
        blt     intel_c3_erase_16_small
        mov     r1, _CONST_ 0x10
        b       intel_c3_erase_16_increment
_THUMB_LABEL_
intel_c3_erase_16_small _LABEL_
        mov     r1, _CONST_ 0x02
_THUMB_LABEL_
intel_c3_erase_16_increment _LABEL_
        lsl     r1, r1, _CONST_ 12
        add     r5, r5, r1

        //
        // Loop back.
        //
        b       intel_c3_erase_16_block

    //
    // We're done erasing the FLASH, so return to the caller.
    //
_THUMB_LABEL_
intel_c3_erase_16_done _LABEL_
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine to erase a Intel J3 FLASH.  This must be a branch to the routine
// for a single 16-bit wide FLASH, immediately followed by a branch to the
// routine for a 32-bit wide FLASH (i.e. two 16-bit wide FLASHes), immediately
// followed by the routien for a single 16-bit wide FLASH with a reversed data
// bus.
//
//****************************************************************************
_THUMB_LABEL_
intel_j3_erase _LABEL_
    b       intel_j3_erase_16
    b       intel_j3_erase_32

//****************************************************************************
//
// The routine for erasing a 16-bit wide Intel J3 FLASH with a reversed data
// bus.
//
//****************************************************************************
_THUMB_LABEL_
intel_j3_erase_16_rev _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the length of the FLASH to be erased.
    //
    mov     r4, r0

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Get the commands to erase a block of the FLASH.
    //
    mov     r6, _CONST_ 0x04
    lsl     r6, r6, _CONST_ 8
    mov     r7, _CONST_ 0x0b
    lsl     r7, r7, _CONST_ 8

    //
    // Start erasing the FLASH from block zero.
    //
    mov     r5, _CONST_ 0x00
_THUMB_LABEL_
intel_j3_erase_16_rev_block _LABEL_
        //
        // See if we have erased all the required blocks.
        //
        cmp     r5, r4
        bge     intel_j3_erase_16_rev_done

        //
        // Erase this block.
        //
        strh    r6, [r0, r5]
        strh    r7, [r0, r5]

        //
        // Wait until the erase has completed.
        //
        bl      intel_wait_16_rev

        //
        // Increment the block address.  Each block is 128K.
        //
        mov     r1, _CONST_ 0x02
        lsl     r1, r1, _CONST_ 16
        add     r5, r5, r1

        //
        // Loop back.
        //
        b       intel_j3_erase_16_rev_block

    //
    // We're done erasing the FLASH, so return to the caller.
    //
_THUMB_LABEL_
intel_j3_erase_16_rev_done _LABEL_
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine for erasing a pair of 16-bit wide Intel J3 FLASHes in a 32-bit
// configuration.
//
//****************************************************************************
_THUMB_LABEL_
intel_j3_erase_32 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the length of the FLASH to be erased.
    //
    mov     r4, r0

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Get the commands to erase a block of the FLASH.
    //
    mov     r5, _CONST_ 0x20
    mov     r6, r5
    lsl     r6, r6, _CONST_ 16
    orr     r6, r6, r5
    mov     r5, _CONST_ 0xd0
    mov     r7, r5
    lsl     r7, r7, _CONST_ 16
    orr     r7, r7, r5

    //
    // Start erasing the FLASH from block zero.
    //
    mov     r5, _CONST_ 0x00
_THUMB_LABEL_
intel_j3_erase_32_block _LABEL_
        //
        // See if we have erased all the required blocks.
        //
        cmp     r5, r4
        bge     intel_j3_erase_32_done

        //
        // Erase this block.
        //
        str     r6, [r0, r5]
        str     r7, [r0, r5]

        //
        // Wait until the erase has completed.
        //
        bl      intel_wait_32

        //
        // Increment the block address.  Each block is 256K.
        //
        mov     r1, _CONST_ 0x04
        lsl     r1, r1, _CONST_ 16
        add     r5, r5, r1

        //
        // Loop back.
        //
        b       intel_j3_erase_32_block

    //
    // We're done erasing the FLASH, so return to the caller.
    //
_THUMB_LABEL_
intel_j3_erase_32_done _LABEL_
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine for erasing a 16-bit wide Intel J3 FLASH.
//
//****************************************************************************
_THUMB_LABEL_
intel_j3_erase_16 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the length of the FLASH to be erased.
    //
    mov     r4, r0

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Get the commands to erase a block of the FLASH.
    //
    mov     r6, _CONST_ 0x20
    mov     r7, _CONST_ 0xd0

    //
    // Start erasing the FLASH from block zero.
    //
    mov     r5, _CONST_ 0x00
_THUMB_LABEL_
intel_j3_erase_16_block _LABEL_
        //
        // See if we have erased all the required blocks.
        //
        cmp     r5, r4
        bge     intel_j3_erase_16_done

        //
        // Erase this block.
        //
        strh    r6, [r0, r5]
        strh    r7, [r0, r5]

        //
        // Wait until the erase has completed.
        //
        bl      intel_wait_16

        //
        // Increment the block address.  Each block is 128K.
        //
        mov     r1, _CONST_ 0x02
        lsl     r1, r1, _CONST_ 16
        add     r5, r5, r1

        //
        // Loop back.
        //
        b       intel_j3_erase_16_block

    //
    // We're done erasing the FLASH, so return to the caller.
    //
_THUMB_LABEL_
intel_j3_erase_16_done _LABEL_
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine to program a Intel J3 FLASH.  This must be a branch to the
// routine for a single 16-bit wide FLASH, immediately followed by a branch to
// the routine for a 32-bit wide FLASH (i.e. two 16-bit wide FLASHes),
// immediately followed by the routine for a single 16-bit wide FLASH with a
// reversed data bus.
//
//****************************************************************************
_THUMB_LABEL_
intel_j3_program _LABEL_
    b       intel_j3_program_16
    b       intel_j3_program_32

//****************************************************************************
//
// The routine for programming a 16-bit wide Intel J3 FLASH with a reversed
// data bus.
//
//****************************************************************************
_THUMB_LABEL_
intel_j3_program_16_rev _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the offset, length, and pointer to the data to be programmed into
    // the FLASH.
    //
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Program the FLASH a sixteen words at a time.
    //
_THUMB_LABEL_
intel_j3_program_16_rev_16words _LABEL_
        //
        // See if we have programmed all the required words.
        //
        cmp     r6, _CONST_ 0
        beq     intel_j3_program_16_rev_done

        //
        // Send the write to buffer command.
        //
        mov     r1, _CONST_ 0x17
        lsl     r1, r1, _CONST_ 8
        strh    r1, [r0, r5]
        mov     r1, _CONST_ 0xf0
        lsl     r1, r1, _CONST_ 8
        strh    r1, [r0, r5]

        //
        // Write the next 16 words to the FLASH.
        //
        mov     r2, _CONST_ 0x10
        mov     r3, r5
_THUMB_LABEL_
intel_j3_program_16_rev_word _LABEL_
            //
            // Load the next word to be programmed.
            //
            ldrh    r1, [r4]
            add     r4, r4, _CONST_ 2

            //
            // Write it to the FLASH.
            //
            strh    r1, [r0, r3]
            add     r3, r3, _CONST_ 2

            //
            // Decrement the count of words to write.
            //
            sub     r2, r2, _CONST_ 1
            bne     intel_j3_program_16_rev_word

        //
        // Write the program confirm command.
        //
        mov     r1, _CONST_ 0x0b
        lsl     r1, r1, _CONST_ 8
        strh    r1, [r0, r5]

        //
        // Wait until the words have been programmed.
        //
        bl      intel_wait_16_rev

        //
        // Increment the address to be programmed.
        //
        add     r5, r5, _CONST_ 32

        //
        // Decrement the count of words to be programmed.
        //
        sub     r6, r6, _CONST_ 32

        //
        // Loop back.
        //
        b       intel_j3_program_16_rev_16words

    //
    // We're done programming the FLASH, so put the FLASH into read array mode.
    //
_THUMB_LABEL_
intel_j3_program_16_rev_done _LABEL_
    mov     r1, _CONST_ 0xff
    lsl     r1, r1, _CONST_ 8
    strh    r1, [r0]

    //
    // Return to the caller.
    //
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine for programming a pair of 16-bit wide Intel J3 FLASHes.
//
//****************************************************************************
_THUMB_LABEL_
intel_j3_program_32 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the offset, length, and pointer to the data to be programmed into
    // the FLASH.
    //
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Program the FLASH sixteen words at a time.
    //
_THUMB_LABEL_
intel_j3_program_32_16words _LABEL_
        //
        // See if we have programmed all the required words.
        //
        cmp     r6, _CONST_ 0
        beq     intel_j3_program_32_done

        //
        // Send the write to buffer command.
        //
        mov     r2, _CONST_ 0xe8
        mov     r1, r2
        lsl     r1, r1, _CONST_ 16
        orr     r1, r1, r2
        str     r1, [r0, r5]
        mov     r2, _CONST_ 0x0f
        mov     r1, r2
        lsl     r1, r1, _CONST_ 16
        orr     r1, r1, r2
        str     r1, [r0, r5]

        //
        // Write the next 16 words to the FLASH.
        //
        mov     r2, _CONST_ 0x10
        mov     r3, r5
_THUMB_LABEL_
intel_j3_program_32_word _LABEL_
            //
            // Load the next word to be programmed.
            //
            ldr     r1, [r4]
            add     r4, r4, _CONST_ 4

            //
            // Write it to the FLASH.
            //
            str     r1, [r0, r3]
            add     r3, r3, _CONST_ 4

            //
            // Decrement the count of words to write.
            //
            sub     r2, r2, _CONST_ 1
            bne     intel_j3_program_32_word

        //
        // Write the program confirm command.
        //
        mov     r2, _CONST_ 0xd0
        mov     r1, r2
        lsl     r1, r1, _CONST_ 16
        orr     r1, r1, r2
        str     r1, [r0, r5]

        //
        // Wait until the words have been programmed.
        //
        bl      intel_wait_32

        //
        // Increment the address to be programmed.
        //
        add     r5, r5, _CONST_ 64

        //
        // Decrement the count of words to be programmed.
        //
        sub     r6, r6, _CONST_ 64

        //
        // Loop back.
        //
        b       intel_j3_program_32_16words

    //
    // We're done programming the FLASH, so put the FLASH into read array mode.
    //
_THUMB_LABEL_
intel_j3_program_32_done _LABEL_
    mov     r2, _CONST_ 0xff
    mov     r1, r2
    lsl     r1, r1, _CONST_ 16
    orr     r1, r1, r2
    str     r1, [r0]

    //
    // Return to the caller.
    //
    pop     {r4-r7, pc}

//****************************************************************************
//
// The routine for programming a 16-bit wide Intel J3 FLASH.
//
//****************************************************************************
_THUMB_LABEL_
intel_j3_program_16 _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r7, lr}

    //
    // Save the offset, length, and pointer to the data to be programmed into
    // the FLASH.
    //
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2

    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Program the FLASH a sixteen words at a time.
    //
_THUMB_LABEL_
intel_j3_program_16_16words _LABEL_
        //
        // See if we have programmed all the required words.
        //
        cmp     r6, _CONST_ 0
        beq     intel_j3_program_16_done

        //
        // Send the write to buffer command.
        //
        mov     r1, _CONST_ 0xe8
        strh    r1, [r0, r5]
        mov     r1, _CONST_ 0x0f
        strh    r1, [r0, r5]

        //
        // Write the next 16 words to the FLASH.
        //
        mov     r2, _CONST_ 0x10
        mov     r3, r5
_THUMB_LABEL_
intel_j3_program_16_word _LABEL_
            //
            // Load the next word to be programmed.
            //
            ldrh    r1, [r4]
            add     r4, r4, _CONST_ 2

            //
            // Write it to the FLASH.
            //
            strh    r1, [r0, r3]
            add     r3, r3, _CONST_ 2

            //
            // Decrement the count of words to write.
            //
            sub     r2, r2, _CONST_ 1
            bne     intel_j3_program_16_word

        //
        // Write the program confirm command.
        //
        mov     r1, _CONST_ 0xd0
        strh    r1, [r0, r5]

        //
        // Wait until the words have been programmed.
        //
        bl      intel_wait_16

        //
        // Increment the address to be programmed.
        //
        add     r5, r5, _CONST_ 32

        //
        // Decrement the count of words to be programmed.
        //
        sub     r6, r6, _CONST_ 32

        //
        // Loop back.
        //
        b       intel_j3_program_16_16words

    //
    // We're done programming the FLASH, so put the FLASH into read array mode.
    //
_THUMB_LABEL_
intel_j3_program_16_done _LABEL_
    mov     r1, _CONST_ 0xff
    strh    r1, [r0]

    //
    // Return to the caller.
    //
    pop     {r4-r7, pc}

//****************************************************************************
//
// SendChar sends a character to UART1.
//
//****************************************************************************
_THUMB_LABEL_
SendChar _LABEL_
    //
    // Load a pointer to the system status register.
    //
    mov     r1, _CONST_ 0x80
    lsl     r1, r1, _CONST_ 20
    mov     r2, _CONST_ 0x14
    orr     r1, r1, r2
    lsl     r1, r1, _CONST_ 4

    //
    // Wait until there is space in the transmit FIFO for UART1.
    //
    mov     r3, _CONST_ 0x80
    lsl     r3, r3, _CONST_ 16
_THUMB_LABEL_
sendchar0 _LABEL_
        ldr     r2, [r1]
        tst     r2, r3
        bne     sendchar0

    //
    // Write the character to UART1.
    //
    mov     r2, _CONST_ 0x34
    lsl     r2, r2, _CONST_ 4
    add     r1, r1, r2
    str     r0, [r1]

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// ReceiveChar receives a character from UART1.
//
//****************************************************************************
_THUMB_LABEL_
ReceiveChar _LABEL_
    //
    // Load a pointer to the system status register.
    //
    mov     r1, _CONST_ 0x80
    lsl     r1, r1, _CONST_ 20
    mov     r2, _CONST_ 0x14
    orr     r1, r1, r2
    lsl     r1, r1, _CONST_ 4

    //
    // Wait until there is a character in the receive FIFO for UART1.
    //
    mov     r2, _CONST_ 0x40
    lsl     r2, r2, _CONST_ 16
_THUMB_LABEL_
receivechar0 _LABEL_
        ldr     r0, [r1]
        tst     r0, r2
        bne     receivechar0

    //
    // Read the character from UART1.
    //
    mov     r2, _CONST_ 0x34
    lsl     r2, r2, _CONST_ 4
    add     r1, r1, r2
    ldr     r0, [r1]
    mov     r1, _CONST_ 0xff
    and     r0, r0, r1

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// SetBaud sets the baud rate to the specified rate.
//
//****************************************************************************
_THUMB_LABEL_
SetBaud _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {lr}

    //
    // Read the baud rate indicator from the host.
    //
    bl      ReceiveChar

    //
    // Get the baud rate divisor based on the requested baud rate.
    //
    mov     r1, _CONST_ 0x17
    cmp     r0, _CONST_ 0x30
    beq     setbaud0
    mov     r1, _CONST_ 0x0b
    cmp     r0, _CONST_ 0x31
    beq     setbaud0
    mov     r1, _CONST_ 0x05
    cmp     r0, _CONST_ 0x32
    beq     setbaud0
    mov     r1, _CONST_ 0x03
    cmp     r0, _CONST_ 0x33
    beq     setbaud0
    mov     r1, _CONST_ 0x01

    //
    // Set the data length to 8 bits per character and enable the FIFO.
    //
_THUMB_LABEL_
setbaud0 _LABEL_
    mov     r0, _CONST_ 0x07
    lsl     r0, r0, _CONST_ 16
    orr     r1, r1, r0

    //
    // Change the configuration of UART1.
    //
    mov     r0, _CONST_ 0x80
    lsl     r0, r0, _CONST_ 20
    mov     r2, _CONST_ 0x4c
    orr     r0, r0, r2
    lsl     r0, r0, _CONST_ 4
    str     r1, [r0]

    //
    // Wait until we receive a '-' character from the host.
    //
_THUMB_LABEL_
setbaud1 _LABEL_
        bl      ReceiveChar
        cmp     r0, _CONST_ 0x2d
        bne     setbaud1

    //
    // Return to the caller.
    //
    pop     {pc}

//****************************************************************************
//
// ReadLong reads a 32-bit value from the serial port.
//
//****************************************************************************
_THUMB_LABEL_
ReadLong _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4, lr}

    //
    // Read the first byte.
    //
    bl      ReceiveChar

    //
    // The first byte is the least significant 8 bits of the long value.
    //
    mov     r4, r0

    //
    // Read the second byte.
    //
    bl      ReceiveChar

    //
    // The second byte is the next 8 significant bits of the long value.
    //
    lsl     r0, r0, _CONST_ 8
    orr     r4, r4, r0

    //
    // Read the third byte.
    //
    bl      ReceiveChar

    //
    // The third byte is the next 8 significant bits of the long value.
    //
    lsl     r0, r0, _CONST_ 16
    orr     r4, r4, r0

    //
    // Read the fourth byte.
    //
    bl      ReceiveChar

    //
    // The fourth byte is the most significant 8 bits of the long value.
    //
    lsl     r0, r0, _CONST_ 24
    orr     r0, r0, r4

    //
    // Return to the caller.
    //
    pop     {r4, pc}

//****************************************************************************
//
// ReadBlock reads a 1K block of data from the host.
//
//****************************************************************************
_THUMB_LABEL_
ReadBlock _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {r4-r5, lr}

    //
    // Load a pointer to the data buffer.
    //
    ldr     r4, =DataBuffer

    //
    // Read 1K bytes of data.
    //
    mov     r5, _CONST_ 0x04
    lsl     r5, r5, _CONST_ 8
_THUMB_LABEL_
read_byte _LABEL_
        //
        // Read the next byte of data.
        //
        bl      ReceiveChar

        //
        // Store this character in the data buffer.
        //
        strb    r0, [r4]
        add     r4, r4, _CONST_ 1

        //
        // Decrement the count of bytes.
        //
        sub     r5, r5, _CONST_ 1
        bne     read_byte

    //
    // Return to the caller.
    //
    pop     {r4-r5, pc}

//****************************************************************************
//
// ProgramFlash programs the data receive via the serial port into the on-board
// FLASH.
//
//****************************************************************************
_THUMB_LABEL_
ProgramFlash _LABEL_
    //
    // Save the link register to the stack.
    //
    push    {lr}

    //
    // Read the length of the data to be programmed.
    //
    bl      ReadLong
    mov     r4, r0

    //
    // See if the data is small enough to fit into the FLASH.
    //
    mov     r1, r8
    cmp     r0, r1
    blt     size_ok

        //
        // Tell the host that the data is too large.
        //
        mov     r0, _CONST_ 0x26 // '&'
        bl      SendChar

        //
        // We're done.
        //
        pop     {pc}

    //
    // Erase the FLASH.
    //
_THUMB_LABEL_
size_ok _LABEL_
    mov     lr, pc
    mov     pc, r9

    //
    // Tell the host that we're ready to start receiving file data.
    //
    mov     r0, _CONST_ 0x21 // '!'
    bl      SendChar

    //
    // Now, read data in 1K blocks until we've read all the file data.
    //
    mov     r5, _CONST_ 0x00
    mov     r11, r5
_THUMB_LABEL_
read_block _LABEL_
        //
        // Read 1K bytes from the serial port.
        //
        bl      ReadBlock

        //
        // Read the checksum from the host.
        //
        bl      ReadLong

        //
        // Checksum the data we read.
        //
        mov     r1, _CONST_ 0x04
        lsl     r1, r1, _CONST_ 8
        ldr     r2, =DataBuffer
_THUMB_LABEL_
checksum _LABEL_
            //
            // Read this byte of the data buffer.
            //
            ldrb    r3, [r2]
            add     r2, r2, _CONST_ 1

            //
            // Subtract this byte from the checksum from the host.
            //
            sub     r0, r0, r3

            //
            // Decrement the count of bytes.
            //
            sub     r1, r1, _CONST_ 1
            bne     checksum

        //
        // See if the checksum matched (i.e. the counter is now zero).
        //
        cmp     r0, _CONST_ 0x00
        beq     checksum_ok

            //
            // The checksum was bad, so ask the host to send this block again.
            //
            mov     r0, _CONST_ 0x40 // '@'
            bl      SendChar
            b       read_block

        //
        // The checksum matched, so program this block of data into the FLASH.
        // First, get some values which we'll need.
        //
_THUMB_LABEL_
checksum_ok _LABEL_
        ldr     r0, =DataBuffer
        mov     r1, r5
        mov     r2, _CONST_ 0x04
        lsl     r2, r2, _CONST_ 8
        mov     lr, pc
        mov     pc, r10

        //
        // Verify that the data was programmed correctly.
        //
        ldr     r0, =DataBuffer
        mov     r1, _CONST_ 0x70
        lsl     r1, r1, _CONST_ 24
        add     r1, r1, r5
        mov     r2, _CONST_ 0x04
        lsl     r2, r2, _CONST_ 8
_THUMB_LABEL_
verify_word _LABEL_
            //
            // Load this word from FLASH and from the internal buffer.
            //
            ldr     r6, [r0]
            add     r0, r0, _CONST_ 4
            ldr     r7, [r1]
            add     r1, r1, _CONST_ 4

            //
            // See if the two words are identical.
            //
            cmp     r6, r7
            beq     word_ok

                //
                // Indicate that there was an error.
                //
                mov     r11, r0

            //
            // Decrement the word count.
            //
_THUMB_LABEL_
word_ok _LABEL_
            sub     r2, r2, _CONST_ 4
            bne     verify_word

        //
        // Tell the host that we're done with this block and are ready for the
        // next one.
        //
        mov     r0, _CONST_ 0x23 // '_CONST_ '
        bl      SendChar

        //
        // Increment the block address.
        //
        mov     r0, _CONST_ 0x04
        lsl     r0, r0, _CONST_ 8
        add     r5, r5, r0

        //
        // Decrement the count of bytes to be programmed.
        //
        sub     r4, r4, r0

        //
        // Loop back to the next block.
        //
        bne     read_block

    //
    // See if there was an error programming the FLASH.
    //
    mov     r0, r11
    cmp     r0, _CONST_ 0x00
    beq     program_done

        //
        // Tell the host that there was an error programming the FLASH.
        //
        mov     r0, _CONST_ 0x5e // '^'
        bl      SendChar

    //
    // Tell the host that we're done programming the FLASH.
    //
_THUMB_LABEL_
program_done _LABEL_
    mov     r0, _CONST_ 0x2a // '*'
    bl      SendChar

    //
    // Return to the caller.
    //
    pop     {pc}

//****************************************************************************
//
// Checks the FLASH to determine if it is a device that we know how to program.
//
//****************************************************************************
_THUMB_LABEL_
check_flash _LABEL_
    //
    // Load a pointer to the FLASH.
    //
    mov     r0, _CONST_ 0x70
    lsl     r0, r0, _CONST_ 24

    //
    // Load the value of the system status register.
    //
    ldr     r1, =0x80000140
    ldr     r1, [r1]

    //
    // Get the BOOTBIT field of the system status register.
    //
    mov     r2, _CONST_ 0x18
    lsl     r2, r2, _CONST_ 24
    and     r1, r1, r2

    //
    // See if the BOOTBIT field is zero, indicating that the FLASH connected to
    // nCS0 is 32-bits wide.
    //
    mov     r2, _CONST_ 0x00
    cmp     r1, r2
    bne     check_flash_16

    //
    // The FLASH is 32-bits wide.
    //
_THUMB_LABEL_
check_flash_32 _LABEL_
    mov     r7, _CONST_ 0x00

    //
    // Write the read identifier command to the FLASH.
    //
    ldr     r1, =0x00900090
    str     r1, [r0]

    //
    // Read back the manufacturer ID from each 16-bit wide FLASH.
    //
    ldr     r1, [r0]

    //
    // See if the IDs are Intel.
    //
    ldr     r2, =0x00890089
    cmp     r1, r2
    bne     unknown_device

    //
    // Load the device IDs from each 16-bit wide FLASH.
    //
    ldrh    r1, [r0, _CONST_ 4]
    ldrh    r2, [r0, _CONST_ 6]

    //
    // See if the IDs are identical.
    //
    cmp     r1, r2
    beq     id_is_ok

    //
    // This is a FLASH configuration we can not handle, either an unknown
    // manufacturer, an unknown device ID, or the FLASH devices do not match.
    // Return an error.
    //
_THUMB_LABEL_
unknown_device _LABEL_
    mov     r0, _CONST_ 0x00
    mov     pc, lr

    //
    // The FLASH is 16-bits wide.
    //
_THUMB_LABEL_
check_flash_16 _LABEL_
    mov     r7, _CONST_ 0x01

    //
    // Write the read identifier command to the FLASH.
    //
    mov     r1, _CONST_ 0x90
    strh    r1, [r0]

    //
    // Read back the manufacturer ID from the FLASH.
    //
    ldrh    r1, [r0]

    //
    // See if the ID is Intel.
    //
    cmp     r1, _CONST_ 0x89
    beq     is_intel

    //
    // Rev. B of the EP7209 reference design board has the data bus to the
    // FLASH reversed, so try the read identifier command bit reversed.
    //
    mov     r1, _CONST_ 0x09
    lsl     r1, r1, _CONST_ 8
    strh    r1, [r0]

    //
    // Re-load the manufacturer ID (bit reversed?).
    //
    ldrh    r1, [r0]

    //
    // See if the ID is Intel, bit reversed.
    //
    mov     r2, _CONST_ 0x91
    lsl     r2, r2, _CONST_ 8
    cmp     r1, r2
    bne     unknown_device

    //
    // The data bus is reversed, so set the flag to indicate this condition.
    //
    mov     r1, _CONST_ 0x02
    orr     r7, r7, r1

    //
    // We have an Intel FLASH, so read the device ID.
    //
_THUMB_LABEL_
is_intel _LABEL_
    ldrh    r1, [r0, _CONST_ 2]

    //
    // On the EP73XX evaluation board, the FLASH can be jumpered to be either
    // 32 or 16 bits wide.  If it is jumpered to be 16 bits wide, the LSB of
    // the FLASH (which must be high to read the device ID) is connected to A2
    // of the processor.  In this case, the read at offset 2 will actually read
    // the manufacturer ID.  So, read the manufacturer ID and see if it matches
    // the device ID.
    //
    ldrh    r2, [r0]
    cmp     r1, r2
    bne     have_id

    //
    // The manufacturer ID matches the device ID, so read the device ID from
    // from offset 4 (which will assert the LSB of the FLASH, giving the real
    // device ID).
    //
    ldrh    r1, [r0, _CONST_ 4]

    //
    // We now have the device ID of the FLASH.  See if it is bit reversed.
    //
_THUMB_LABEL_
have_id _LABEL_
    mov     r2, _CONST_ 0x02
    tst     r7, r2
    beq     id_is_ok

    //
    // The device ID is bit reversed, so we need to reverse it.  We will go
    // through a sequence of reversing sets of bits in a manner similar to a
    // binary traversal.  If we consider the 16 bits to be as follows:
    //
    //     fedcba98 76543210
    //
    // Then we will reverse the bits in a sequence that will make the bits look
    // as follows (if viewed at each step of the sequence):
    //
    //     76543210 fedcba98
    //     32107654 ba98fedc
    //     10325476 98badcfe
    //     01234567 89abcdef
    //
    // At each step, a smaller set of bits is swapped, and the same swap is
    // performed across the entire 16-bit word.
    //
    // Start by swapping the two bytes.
    //
    mov     r2, r1
    lsl     r2, r2, _CONST_ 8
    lsr     r1, r1, _CONST_ 8
    orr     r1, r1, r2

    //
    // Now, swap the two nibbles of each byte.
    //
    mov     r2, r1
    ldr     r3, =0x0000f0f0
    lsl     r2, r2, _CONST_ 4
    and     r1, r1, r3
    and     r2, r2, r3
    lsr     r1, r1, _CONST_ 4
    orr     r1, r1, r2

    //
    // Now, swap the two halves of each nibble.
    //
    mov     r2, r1
    ldr     r3, =0x0000cccc
    lsl     r2, r2, _CONST_ 2
    and     r1, r1, r3
    and     r2, r2, r3
    lsr     r1, r1, _CONST_ 2
    orr     r1, r1, r2

    //
    // Finally, swap each pair of bits.
    //
    mov     r2, r1
    ldr     r3, =0x0000aaaa
    lsl     r2, r2, _CONST_ 1
    and     r1, r1, r3
    and     r2, r2, r3
    lsr     r1, r1, _CONST_ 1
    orr     r1, r1, r2

    //
    // We now have the device ID.  See if this is a 32Mb Intel J3 FLASH.
    //
_THUMB_LABEL_
id_is_ok _LABEL_
    cmp     r1, _CONST_ 0x16
    beq     intel_j3_32Mb

    //
    // See if this is a 64Mb Intel J3 FLASH.
    //
    cmp     r1, _CONST_ 0x17
    beq     intel_j3_64Mb

    //
    // See if this is a 128Mb Intel J3 FLASH.
    //
    cmp     r1, _CONST_ 0x18
    beq     intel_j3_128Mb

    //
    // The remaining FLASH device IDs that we recognize are all 0x88.., so load
    // 0x8800 into r3.
    //
    mov     r3, _CONST_ 0x88
    lsl     r3, r3, _CONST_ 8

    //
    // See if this is a 16Mb Intel B3 FLASH.
    //
    mov     r2, _CONST_ 0x91
    orr     r2, r2, r3
    cmp     r1, r2
    beq     intel_b3_16Mb

    //
    // See if this is a 8Mb Intel B3 FLASH.
    //
    mov     r2, _CONST_ 0x93
    orr     r2, r2, r3
    cmp     r1, r2
    beq     intel_b3_8Mb

    //
    // See if this is a 4Mb Intel B3 FLASH.
    //
    mov     r2, _CONST_ 0x95
    orr     r2, r2, r3
    cmp     r1, r2
    beq     intel_b3_4Mb

    //
    // See if this is a 32Mb Intel B3 FLASH.
    //
    mov     r2, _CONST_ 0x97
    orr     r2, r2, r3
    cmp     r1, r2
    beq     intel_b3_32Mb

    //
    // See if this is a 8Mb Intel C3 FLASH.
    //
    mov     r2, _CONST_ 0xc1
    orr     r2, r2, r3
    cmp     r1, r2
    beq     intel_c3_8Mb

    //
    // See if this is a 16Mb Intel C3 FLASH.
    //
    mov     r2, _CONST_ 0xc3
    orr     r2, r2, r3
    cmp     r1, r2
    beq     intel_c3_16Mb

    //
    // See if this is a 32Mb Intel C3 FLASH.
    //
    mov     r2, _CONST_ 0xc5
    orr     r2, r2, r3
    cmp     r1, r2
    beq     intel_c3_32Mb

    //
    // See if this is a 64Mb Intel C3 FLASH.
    //
    mov     r2, _CONST_ 0xcd
    orr     r2, r2, r3
    cmp     r1, r2
    beq     intel_c3_64Mb

    //
    // We do not recognize the device ID, so this is an unknown device.
    //
    b       unknown_device

    //
    // This is a 4Mb Intel B3 FLASH.  Set the size to 1, which will get shifted
    // by 19 to create 512KB.
    //
_THUMB_LABEL_
intel_b3_4Mb _LABEL_
    mov     r0, _CONST_ 0x01

    //
    // Shift the size by 19 bits.
    //
_THUMB_LABEL_
intel_b3 _LABEL_
    lsl     r0, r0, _CONST_ 19

    //
    // Save the size of the FLASH.
    //
    mov     r8, r0

    //
    // Save the pointer to the routine to erase an Intel B3 FLASH.
    //
    ldr     r0, =intel_b3_erase
    mov     r9, r0

    //
    // Save the pointer to the routine to program an Intel B3 FLASH.
    //
    ldr     r0, =intel_b3_c3_program
    mov     r10, r0

    //
    // Perform the size check.
    //
    b       check_size

    //
    // This is a 8Mb Intel B3 FLASH.  Set the size to 2, which will get shifted
    // by 19 to create 1MB.
    //
_THUMB_LABEL_
intel_b3_8Mb _LABEL_
    mov     r0, _CONST_ 0x02

    //
    // The remainder of the handling is common.
    //
    b       intel_b3

    //
    // This is a 16Mb Intel B3 FLASH.  Set the size to 4, which will get
    // shifted by 19 to create 2MB.
    //
_THUMB_LABEL_
intel_b3_16Mb _LABEL_
    mov     r0, _CONST_ 0x04

    //
    // The remainder of the handling is common.
    //
    b       intel_b3

    //
    // This is a 32Mb Intel B3 FLASH.  Set the size to 8, which will get
    // shifted by 19 to create 4MB.
    //
_THUMB_LABEL_
intel_b3_32Mb _LABEL_
    mov     r0, _CONST_ 0x08

    //
    // The remainder of the handling is common.
    //
    b       intel_b3

    //
    // This is a 8Mb Intel C3 FLASH.  Set the size to 1, which will get shifted
    // by 20 to create 1MB.
    //
_THUMB_LABEL_
intel_c3_8Mb _LABEL_
    mov     r0, _CONST_ 0x01

    //
    // Shift the size by 20 bits.
    //
_THUMB_LABEL_
intel_c3 _LABEL_
    lsl     r0, r0, _CONST_ 20

    //
    // Save the size of the FLASH.
    //
    mov     r8, r0

    //
    // Save the pointer to the routine to erase an Intel C3 FLASH.
    //
    ldr     r0, =intel_c3_erase
    mov     r9, r0

    //
    // Save the pointer to the routine to program an Intel C3 FLASH.
    //
    ldr     r0, =intel_b3_c3_program
    mov     r10, r0

    //
    // Perform the size check.
    //
    b       check_size

    //
    // This is a 16Mb Intel C3 FLASH.  Set the size to 2, which will get
    // shifted by 20 to create 2MB.
    //
_THUMB_LABEL_
intel_c3_16Mb _LABEL_
    mov     r0, _CONST_ 0x02

    //
    // The remainder of the handling is common.
    //
    b       intel_c3

    //
    // This is a 32Mb Intel C3 FLASH.  Set the size to 4, which will get
    // shifted by 20 to create 4MB.
    //
_THUMB_LABEL_
intel_c3_32Mb _LABEL_
    mov     r0, _CONST_ 0x04

    //
    // The remainder of the handling is common.
    //
    b       intel_c3

    //
    // This is a 64Mb Intel C3 FLASH.  Set the size to 4, which will get
    // shifted by 20 to create 8MB.
    //
_THUMB_LABEL_
intel_c3_64Mb _LABEL_
    mov     r0, _CONST_ 0x08

    //
    // The remainder of the handling is common.
    //
    b       intel_c3

    //
    // This is a 32Mb Intel J3 FLASH.  Set the size to 1, which will get
    // shifted by 22 to create 4MB.
    //
_THUMB_LABEL_
intel_j3_32Mb _LABEL_
    mov     r0, _CONST_ 0x01

    //
    // Shift the size by 22 bits.
    //
_THUMB_LABEL_
intel_j3 _LABEL_
    lsl     r0, r0, _CONST_ 22

    //
    // Save the size of the FLASH.
    //
    mov     r8, r0

    //
    // Get a pointer to the routines for erasing and program an Intel J3 FLASH.
    //
    ldr     r0, =intel_j3_erase
    ldr     r1, =intel_j3_program

    //
    // See if we have a reversed data bus.
    //
    mov     r2, _CONST_ 2
    tst     r7, r2
    beq     save_j3

    //
    // Increment the address of the erase and program routines by 4, so that we
    // will use the bit reversed versions.
    //
    add     r0, r0, _CONST_ 4
    add     r1, r1, _CONST_ 4

    //
    // Save the pointers to the routines to erase and program an Intel J3
    // FLASH.
    //
_THUMB_LABEL_
save_j3 _LABEL_
    mov     r9, r0
    mov     r10, r1

    //
    // Perform the size check.
    //
    b       check_size

    //
    // This is a 64Mb Intel J3 FLASH.  Set the size to 2, which will get
    // shifted by 22 to create 8MB.
    //
_THUMB_LABEL_
intel_j3_64Mb _LABEL_
    mov     r0, _CONST_ 0x02

    //
    // The remainder of the handling is common.
    //
    b       intel_j3

    //
    // This is a 128Mb Intel J3 FLASH.  Set the size to 4, which will get
    // shifted by 22 to create 16MB.
    //
_THUMB_LABEL_
intel_j3_128Mb _LABEL_
    mov     r0, _CONST_ 0x04

    //
    // The remainder of the handling is common.
    //
    b       intel_j3

    //
    // We now have the size of the FLASH and pointers to the routines that will
    // erase and program it.  See if we are in a 32-bit wide configuration, in
    // which case we must double the size of the FLASH (since there are two 16
    // bit wides devices creating the 32-bit wide "device").
    //
_THUMB_LABEL_
check_size _LABEL_
    mov     r0, _CONST_ 0x01
    tst     r7, r0
    bne     check_done

    //
    // The FLASH is 32-bits wide, so double the size.
    //
    mov     r0, r8
    lsl     r0, r0, _CONST_ 1
    mov     r8, r0

    //
    // Increment the address of the erase and program routines by 2, so that we
    // will use the 32-bit wide versions instead of the 16-bit wide versions.
    //
    mov     r0, r9
    add     r0, r0, _CONST_ 2
    mov     r9, r0
    mov     r0, r10
    add     r0, r0, _CONST_ 2
    mov     r10, r0

    //
    // We've completed our checking successfully.  Return success.
    //
_THUMB_LABEL_
check_done _LABEL_
    mov     r0, _CONST_ 0x01
    mov     pc, lr

//****************************************************************************
//
// The zero-initialized read-write data used by the application.
//
//****************************************************************************
    _BSS_

//****************************************************************************
//
// A buffer to contain 1K bytes of data read from the serial port that is to be
// programmed into the FLASH.
//
//****************************************************************************
DataBuffer _LABEL_
    _SPACE_ 0x400

    _END_
