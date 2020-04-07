//****************************************************************************
//
// VECTORS.S - Code for all the exception vectors of the ARM processor.
//
// Copyright (c) 1998,1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

#include "../asmdefs.h"
#include "../hwport.h"
#include "../hwdefs.h"

//****************************************************************************
//
// Zero-initialized read/write data area.
//
//****************************************************************************
    _BSS_

//****************************************************************************
//
// Memory buffers to contain the stacks for the various processor modes which
// we will be using.
//
//****************************************************************************
SVCStack _LABEL_
#ifdef gcc
    _SPACE_ 0x800
#else
    _SPACE_ 0x400
#endif
SVCStackEnd _LABEL_

IRQStack _LABEL_
    _SPACE_ 0x200
IRQStackEnd _LABEL_

FIQStack _LABEL_
    _SPACE_ 0x10
FIQStackEnd _LABEL_

//****************************************************************************
//
// The following variable contains a set of flags that indicate various
// conditions/requests within the system.
//
//****************************************************************************
    _EXPORT_ ulSystemFlags
ulSystemFlags _LABEL_
    _WORD_  0

//****************************************************************************
//
// The following contains the address of the first free word of internal SRAM.
//
//****************************************************************************
    _EXPORT_ ulEndOfRAM
ulEndOfRAM _LABEL_
    _WORD_  0

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
// one instruction, so we use that instruction to branch to the handler for the
// vector.
//
//****************************************************************************
    _ENTRY_

vectortable _LABEL_
    b       ResetHandler
    b       UndefHandler
    b       SWIHandler
    b       PAbortHandler
    b       DAbortHandler
    b       UnusedHandler
    b       IRQHandler
    b       FIQHandler

//****************************************************************************
//
// Immediately following the ARM vector table is a structure that describes
// the software image.  It has the following definition:
//
//     struct
//     {
//         //
//         // The memory address of this structure in the eyes of the linker.
//         //
//         unsigned long ulBaseAddress;
//
//         //
//         // The length of the program image.
//         //
//         unsigned long ulLength;
//
//         //
//         // The hardware ID of the device on which this software image runs.
//         //
//         unsigned long ulHardwareID;
//
//         //
//         // The CRC of the program image.
//         //
//         unsigned long ulCRC32;
//     };
//
// The ulLength and ulCRC32 fields are filled in by the package program.
//
//****************************************************************************
    _WORD_  .
    _WORD_  0
    _WORD_  HardwareID
    _WORD_  0

//****************************************************************************
//
// ResetHandler is the startup code to be used when the ARM processor first
// starts executing after a reset.  This is responsible for setting up the
// device, configuring the MMU, and launching the actual application.
//
//****************************************************************************
ResetHandler _LABEL_
    //
    // Put the ARM processor into IRQ mode.
    //
    ldr     r0, =0x000000d2
    msr     cpsr_cf, r0

    //
    // Set up the stack pointer for IRQ mode.
    //
    ldr     r13, =IRQStackEnd

    //
    // Put the ARM processor into FIQ mode.
    //
    ldr     r0, =0x000000d1
    msr     cpsr_cf, r0

    //
    // Initialize the values of the FIQ mode banked registers.
    //
    ldr     r8, =0x00000000
    ldr     r9, =0x00000000
    ldr     r10, =0x00000000
    ldr     r11, =0x00000000
    ldr     r12, =0x00000000

    //
    // Set up the stack pointer for FIQ mode.
    //
    ldr     r13, =FIQStackEnd

    //
    // Put the ARM processor into SVC mode.
    //
    ldr     r0, =0x000000d3
    msr     cpsr_cf, r0

    //
    // Set up the stack pointer.
    //
    ldr     r13, =SVCStackEnd

    //
    // Set up the MMU.  Start by flushing the cache and TLB.
    //
    ldr     r0, =0x00000000
    mcr     _CP15_, 0, r0, _CR7_, _CR7_
    mcr     _CP15_, 0, r0, _CR8_, _CR7_

    //
    // Set user mode access for all 16 domains.
    //
    ldr     r0, =0x55555555
    mcr     _CP15_, 0, r0, _CR3_, _CR0_

    //
    // Tell the MMU where to find the page table.
    //
    _IMPORT_ PageTable
    ldr     r0, =PageTable
    mcr     _CP15_, 0, r0, _CR2_, _CR0_

    //
    // Turn on the ROM protection in the MMU.
    //
    ldr     r0, =0x00000270
    mcr     _CP15_, 0, r0, _CR1_, _CR0_

    //
    // Enable the MMU.
    //
    orr     r0, r0, _CONST_ 0x0000000d
    mcr     _CP15_, 0, r0, _CR1_, _CR0_

    //
    // There should always be two NOP instructions following the enable or
    // disable of the MMU.
    //
    nop
    nop

    //
    // Load pointers to the EP7209 internal registers.
    //
    ldr     r0, =HwBaseAddress
    add     r1, r0, _CONST_ 0x00001000
    add     r2, r0, _CONST_ 0x00002000

    //
    // If the system was reset by nPOR, then program the real time clock with
    // the default system time (Nov 30, 1999 @ 9:49am).
    //
    ldr     r3, [r0, _CONST_ HwStatus]
    tst     r3, _CONST_ HwStatusColdStartFlag
    ldrne   r3, =0x38439d8c
    strne   r3, [r0, _CONST_ HwRtcData]

    //
    // Clear the startup reason flags.
    //
    str     r0, [r0, _CONST_ HwStartFlagClear]

    //
    // Disable all the interrupt sources except for the USB interrupt and the
    // timer 1 interrupt.
    //
    ldr     r3, =HwIrqTimer1
    str     r3, [r0, _CONST_ HwIntMask]
    ldr     r3, =0x00000000
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
    // Program the system control registers.  Enable the first serial port, set
    // the processor clock rate to 74MHz, and select prescale mode for timer 1.
    //
    ldr     r3, =HwControl1_Value
    str     r3, [r0, _CONST_ HwControl]
    ldr     r3, =HwControl2_Value
    str     r3, [r1, _CONST_ (HwControl2 - 0x1000)]
    ldr     r3, =HwControl3_Value
    str     r3, [r2, _CONST_ (HwControl3 - 0x2000)]

    //
    // Set the divisor for timer 1 such that it generates an interrupt every
    // 5ms.
    //
    ldr     r3, =0x0000000a
    str     r3, [r0, _CONST_ HwTimer1Data]

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
    // Copy the read-write data block from ROM to RAM.
    //
    _IMPORT_ _ROLIMIT_
    _IMPORT_ _RWBASE_
    _IMPORT_ _ZIBASE_
    ldr     r0, =_ROLIMIT_
    ldr     r1, =_RWBASE_
    ldr     r2, =_ZIBASE_
copy_rw _LABEL_
        cmp     r1, r2
        ldrne   r3, [r0], _CONST_ 4
        strne   r3, [r1], _CONST_ 4
        bne     copy_rw

    //
    // Fill the zero initialized data block in RAM with zeros.
    //
    _IMPORT_ _RWLIMIT_
    ldr     r2, =_RWLIMIT_
    ldr     r3, =0x00000000
zero _LABEL_
        cmp     r1, r2
        strne   r3, [r1], _CONST_ 4
        bne     zero

    //
    // Initialize the value of ulEndOfRAM.
    //
    ldr     r0, =ulEndOfRAM
    str     r1, [r0]

    //
    // We've completed the setup of the processor, peripherals, and program
    // data, so enable the IRQ and FIQ interrupts.
    //
    mrs     r0, cpsr
    bic     r0, r0, _CONST_ 0x000000c0
    msr     cpsr_cf, r0

    //
    // Initialize the remainder of the hardware/software environment required
    // by the player.
    //
    _IMPORT_ initialize
    bl      initialize

    //
    // Branch to the actual program.  We set the link register to zero so that
    // if the program does happen to return, we will re-initialize the system.
    //
    ldr     lr, =0x00000000
    _IMPORT_ entry
    b       entry

    //
    // Tell the assembler to put in-line data here.
    //
    _LTORG_

//****************************************************************************
//
// The following is a handler for several of the ARM exception vectors:
//
//     1) The undefined instruction handler.  This is called when an
//        instruction reaches the execute stage of the ARM pipeline, but is
//        not an instruction which is recognized by the ARM processor.
//     2) The instruction pre-fetch abort handler.  This is called when an
//        instruction reaches the execute state of the ARM pipeline, but the
//        pre-fetch of the instruction failed due to a MMU error.
//     3) The data abort handler.  This is called when a load or store
//        instruction is executed and the memory access failed due to a MMU
//        error.
//     4) The unused handler.  This is a legacy vector which is no longer used
//        on ARM7 processors and therefore will (should) never be called.
//
// In the event that we enter any of these exception vectors, we shutdown down
// the player as best we can and branch back to the reset handler (so that we
// start over with hopefully a clean slate).
//
//****************************************************************************
UndefHandler _LABEL_
PAbortHandler _LABEL_
DAbortHandler _LABEL_
UnusedHandler _LABEL_
    //
    // Put the ARM processor into SVC mode and disable interrupts.
    //
    ldr     r0, =0x000000d3
    msr     cpsr_cf, r0

    //
    // Disable the USB controller.
    //
    _IMPORT_ USBDisable
    bl      USBDisable

    //
    // Disable the MMU.
    //
    mov     r0, _CONST_ 0x00000070
    mcr     _CP15_, 0, r0, _CR1_, _CR0_

    //
    // There should always be two NOP instructions following the enable or
    // disable of the MMU.
    //
    nop
    nop

    //
    // Flush the cache and TLB.
    //
    ldr     r0, =0x00000000
    mcr     _CP15_, 0, r0, _CR7_, _CR7_
    mcr     _CP15_, 0, r0, _CR8_, _CR7_

    //
    // Branch back to the reset handler.
    //
    b       ResetHandler

//****************************************************************************
//
// The software interrupt handler.  We do not support any SWIs, so simply
// return to the caller.
//
//****************************************************************************
SWIHandler _LABEL_
    movs    pc, lr

//****************************************************************************
//
// The IRQ interrupt handler.  This is called when the IRQ line going into the
// ARM processor goes high, indicating an external device is requesting the
// attention of the processor.
//
// We handle the USB interrupt, the timer interrupt for timer1, and the
// interrupts for SmartMedia if they exist.
//
//****************************************************************************
IRQHandler _LABEL_
    //
    // Save r0 through r3 to the stack.
    //
    stmdb   r13!, {r0-r3}

    //
    // Load r0 with a pointer to the EP7209 internal registers.
    //
    ldr     r0, =HwBaseAddress

    //
    // Read the interrupt status register to see which interrupts are asserted.
    // Use the value of the interrupt mask register to mask off asserted
    // interrupts which are currently masked out so that we do not service
    // them.
    //
    ldr     r1, [r0, _CONST_ HwIntStatus]
    ldr     r2, [r0, _CONST_ HwIntMask]
    and     r1, r1, r2

    //
    // See if timer1 caused the interrupt.
    //
    tst     r1, _CONST_ HwIrqTimer1
    beq     timer1_done

        //
        // See if we should call the output processing code.
        //
        ldr     r2, =ulSystemFlags
        ldr     r2, [r2]
        tst     r2, _CONST_ 8
        beq     output_done

            //
            // Save r0, r1, r12 and r14 to the stack.
            //
            stmdb   r13!, {r0, r1, r12, r14}

            //
            // Call the output processing code.
            //
            _IMPORT_ OutputFilter
            bl      OutputFilter

            //
            // Restore r0, r1, r12 and r14 from the stack.
            //
            ldmia   r13!, {r0, r1, r12, r14}

        //
        // Increment the keyboard counter.
        //
output_done _LABEL_
        ldr     r2, =ulSystemFlags
        ldr     r3, [r2]
        add     r3, r3, _CONST_ 0x00000001
        bic     r3, r3, _CONST_ 0x00000004
        str     r3, [r2]
        and     r3, r3, _CONST_ 0x00000003

        //
        // If the count is not zero, then do not scan the keyboard.
        //
        cmp     r3, _CONST_ 0x00000000
        bne     keyboard_done

            //
            // Save r0, r1, r12, and r14 to the stack.
            //
            stmdb   r13!, {r0, r1, r12, r14}

            //
            // Call the UI ISR.
            //
            _IMPORT_ UIISR
            bl      UIISR

            //
            // Check on the USB connection.
            //
#ifdef HwRegUSBCable
            _IMPORT_ USBCheck
            bl      USBCheck
#endif

            //
            // Restore r0, r1, r12, and r14 from the stack.
            //
            ldmia   r13!, {r0, r1, r12, r14}

        //
        // Clear the timer1 interrupt.
        //
keyboard_done _LABEL_
        str     r0, [r0, _CONST_ HwTimer1EOI]

    //
    // See if the USB caused the interrupt.
    //
timer1_done _LABEL_
    tst     r1, _CONST_ HwIrqUSB
    beq     usb_done

        //
        // Save r0, r1, r12, and r14 to the stack.
        //
        stmdb   r13!, {r0, r1, r12, r14}

        //
        // Call the USB interrupt handler.
        //
        _IMPORT_ USBISR
        bl      USBISR

        //
        // Restore r0, r1, r12, and r14 from the stack.
        //
        ldmia   r13!, {r0, r1, r12, r14}

    //
    // See if either SmartMedia interrupt caused the interupt.
    //
usb_done _LABEL_
#ifdef HwIrqSmartMediaInsert
    tst     r1, _CONST_ (HwIrqSmartMediaInsert | HwIrqSmartMediaRemove)
    beq     smartmedia_done

        //
        // Save r0, r1, r12, and r14 to the stack.
        //
        stmdb   r13!, {r0, r1, r12, r14}

        //
        // Call the SmartMedia interrupt handler.
        //
        _IMPORT_ SMISR
        bl      SMISR

        //
        // Restore r0, r1, r12, adn r14 from the stack.
        //
        ldmia   r13!, {r0, r1, r12, r14}
#endif

    //
    // Restore r0 through r3 from the stack.
    //
smartmedia_done _LABEL_
    ldmia   r13!, {r0-r3}

    //
    // Return to the interrupted code.
    //
    subs    pc, lr, _CONST_ 4

    //
    // Tell the assembler to put in-line data here.
    //
    _LTORG_

//****************************************************************************
//
// The FIQ interrupt handler.  This is called when the FIQ line going into the
// ARM processor goes high, indicating an external device is requesting the
// attention of the processor.
//
// The only FIQ which we handle is the DAI interrupt.  Because of this, we
// define the banked FIQ registers (r8-r12) as follows:
//
//     r8  => current play buffer (left channel)
//     r9  => current play buffer (right channel)
//     r10 => play buffer samples left
//     r11 => current record buffer
//     r12 => record buffer space left
//
// With this arrangement, we do not need to go to memory for buffer pointers
// or sizes unless we've reached the end of the buffer (which is relatively
// infrequent).  For added efficiency, we assume that the play and record
// buffers are a multiple of 4 samples in size (since we process 4 samples per
// DAI interrupt).
//
//****************************************************************************
FIQHandler _LABEL_
    //
    // Save r0 through r3 to the stack.
    //
    stmdb   r13!, {r0-r3}

    //
    // Load r1 with a pointer to the DAI registers.
    //
    ldr     r1, =(HwBaseAddress + 0x00002000)

    //
    // Handle writing data into the transmit FIFO.  First, see if our current
    // play buffer pointer needs to be re-loaded.
    //
    cmp     r10, _CONST_ 0x00000000
    bne     play_fill

        //
        // We need to re-load the play buffer pointers.  First, get a pointer
        // to the play buffer.
        //
        _IMPORT_ sPlayBuffer
        ldr     r0, =sPlayBuffer

        //
        // Get the read and write pointers for the buffer.
        //
        ldr     r2, [r0, _CONST_ 0x0c]
        ldr     r3, [r0, _CONST_ 0x10]

        //
        // Is the buffer empty?
        //
        subs    r10, r3, r2
        bne     play_next

            //
            // Since there is no more data available, write silence to the
            // digital audio interface.
            //
            str     r10, [r1, _CONST_ (HwDAIData1 - 0x2000)]
            str     r10, [r1, _CONST_ (HwDAIData0 - 0x2000)]
            str     r10, [r1, _CONST_ (HwDAIData1 - 0x2000)]
            str     r10, [r1, _CONST_ (HwDAIData0 - 0x2000)]
            str     r10, [r1, _CONST_ (HwDAIData1 - 0x2000)]
            str     r10, [r1, _CONST_ (HwDAIData0 - 0x2000)]
            str     r10, [r1, _CONST_ (HwDAIData1 - 0x2000)]
            str     r10, [r1, _CONST_ (HwDAIData0 - 0x2000)]

            //
            // We're done handling the play FIFO.
            //
            b       play_done

play_next _LABEL_
        //
        // If the difference between the read and write pointers is negative,
        // then only read samples up to the end of the buffer.
        //
        ldrmi   r3, [r0, _CONST_ 0x08]
        submi   r10, r3, r2

        //
        // Get pointers to the beginning of the buffers.
        //
        ldr     r8, [r0, _CONST_ 0x00]
        ldr     r9, [r0, _CONST_ 0x04]

        //
        // Increment the buffer pointers by the current read pointer.
        //
        add     r8, r8, r2, lsl _CONST_ 1
        add     r9, r9, r2, lsl _CONST_ 1

play_fill _LABEL_
    //
    // Read four samples from the play buffer and write them to the DAI FIFO.
    //
    ldrh    r0, [r8], _CONST_ 2
    str     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldrh    r0, [r9], _CONST_ 2
    str     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    ldrh    r0, [r8], _CONST_ 2
    str     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldrh    r0, [r9], _CONST_ 2
    str     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    ldrh    r0, [r8], _CONST_ 2
    str     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldrh    r0, [r9], _CONST_ 2
    str     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    ldrh    r0, [r8], _CONST_ 2
    str     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldrh    r0, [r9], _CONST_ 2
    str     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]

    //
    // Decrement the sample count by 4.
    //
    sub     r10, r10, _CONST_ 4

    //
    // Load a pointer to the play buffer.
    //
    ldr     r0, =sPlayBuffer

    //
    // Get the read pointer for the buffer.
    //
    ldr     r2, [r0, _CONST_ 0x0c]

    //
    // Increment the read pointer by 4.
    //
    add     r2, r2, _CONST_ 4

    //
    // Get the length of the buffer.
    //
    ldr     r3, [r0, _CONST_ 0x08]

    //
    // If the read pointer is at the end of the buffer, then reset it to the
    // beginning.
    //
    cmp     r2, r3
    moveq   r2, _CONST_ 0x00000000

    //
    // Save the read pointer for the buffer.
    //
    str     r2, [r0, _CONST_ 0x0c]

play_done _LABEL_
#ifdef REQUIRE_RECORD
    //
    // See if our current record buffer pointer needs to be re-loaded.
    //
    cmp     r12, _CONST_ 0x00000000
    bne     record_fill

        //
        // We need to re-load the record buffer pointer.  First, get a pointer
        // to the record buffer.
        //
        _IMPORT_ sRecordBuffer
        ldr     r0, =sRecordBuffer

        //
        // Get a pointer to the beginning of the buffer.
        //
        ldr     r11, [r0, _CONST_ 0x00]

        //
        // Get the length of the buffer.
        //
        ldr     r12, [r0, _CONST_ 0x08]

        //
        // Is the length zero?
        //
        cmp     r12, _CONST_ 0x00000000
        bne     record_fill

            //
            // Throw away the samples in the receive FIFO.
            //
            ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
            ldr     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
            ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
            ldr     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
            ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
            ldr     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
            ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
            ldr     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]

            //
            // We're done handling the record FIFO.
            //
            b       record_done

record_fill _LABEL_
    //
    // Read four samples from the DAI FIFO and write them to the record buffer.
    //
    ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldr     r2, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    mov     r2, r2, lsl _CONST_ 1
    strh    r2, [r11], _CONST_ 2
    ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldr     r2, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    mov     r2, r2, lsl _CONST_ 1
    strh    r2, [r11], _CONST_ 2
    ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldr     r2, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    mov     r2, r2, lsl _CONST_ 1
    strh    r2, [r11], _CONST_ 2
    ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldr     r2, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    mov     r2, r2, lsl _CONST_ 1
    strh    r2, [r11], _CONST_ 2

    //
    // Decrement the sample count by 4.
    //
    sub     r12, r12, _CONST_ 4

    //
    // Load a pointer to the record buffer.
    //
    ldr     r0, =sRecordBuffer

    //
    // Get the write pointer for the buffer.
    //
    ldr     r2, [r0, _CONST_ 0x10]

    //
    // Increment the write pointer by 4.
    //
    add     r2, r2, _CONST_ 4

    //
    // If the write pointer is at the end of the buffer, then reset it to the
    // beginning.
    //
    cmp     r12, _CONST_ 0x00000000
    moveq   r2, _CONST_ 0x00000000

    //
    // Save the write pointer for the buffer.
    //
    str     r2, [r0, _CONST_ 0x10]

record_done _LABEL_
#else
    //
    // Throw away the samples in the receive FIFO.
    //
    ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldr     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldr     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldr     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
    ldr     r0, [r1, _CONST_ (HwDAIData1 - 0x2000)]
    ldr     r0, [r1, _CONST_ (HwDAIData0 - 0x2000)]
#endif

    //
    // Clear the overrun and underrun bits in the DAI status register.
    //
    ldr     r0, =0xffffffff
    str     r0, [r1, _CONST_ (HwDAIStatus - 0x2000)]

    //
    // Restore r0 through r3 from the stack.
    //
    ldmia   r13!, {r0-r3}

    //
    // Return to the interrupted code.
    //
    subs    pc, lr, _CONST_ 4

    //
    // Tell the assembler to put in-line data here.
    //
    _LTORG_

//****************************************************************************
//
// SupportUpdate determines if the current environment support a software
// update.
//
//****************************************************************************
    _EXPORT_ SupportUpdate
SupportUpdate _LABEL_
    //
    // Get the address of our vector table.
    //
    ldr     r0, =vectortable

    //
    // See if our vector table is at location 0, the actual vector table
    // location.
    //
    ldr     r1, =0x00000000
    cmp     r0, r1

    //
    // If the vector table is at 0, then we do not support software updates.
    // If it is not at 0, then we dos upport software updates (since it is the
    // update code which resides at location 0, not our code).
    //
    moveq   r0, _CONST_ 0
    movne   r0, _CONST_ 1

    //
    // Return to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// StartUpdate transfers control to the update code at the beginning of the
// ROM.  This should only be called if SupportUpdate returns TRUE.
//
//****************************************************************************
    _EXPORT_ StartUpdate
StartUpdate _LABEL_
    //
    // Branch to the update "vector" at the beginning of ROM.
    //
    ldr     pc, =0x00000020

//****************************************************************************
//
// A constant which contains the address of the last word of RAM.
//
//****************************************************************************
    _EXPORT_ ulExtentOfRAM
ulExtentOfRAM _LABEL_
#if defined(PROCESSOR_EP72XX)
    _WORD_  HwSRAMAddress + 0x9600
#elif defined(PROCESSOR_EP73XX)
    _WORD_  HwSRAMAddress + 0xc000
#endif

    _END_
