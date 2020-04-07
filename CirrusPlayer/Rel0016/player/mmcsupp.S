//****************************************************************************
//
// MMCSUPP.S - Routines to read/write bytes from the MMC card.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************

#include "../asmdefs.h"
#include "../hwport.h"
#include "../hwdefs.h"

//****************************************************************************
//
// Determine the register, bit, and shift required to access the command line
// of the MMC bus.
//
//****************************************************************************
#ifdef HwPortABCD_MMC_CMD
#define MMC_CMD_PORT                            HwPortABCD
#define MMC_CMD_DIR                             HwDdrABCD
#define MMC_CMD_BIT                             HwPortABCD_MMC_CMD
#define MMC_CMD_SHIFT                           HwPortABCD_MMC_CMD_Shift
#endif
#ifdef HwPortE_MMC_CMD
#define MMC_CMD_PORT                            HwPortE
#define MMC_CMD_DIR                             HwDdrE
#define MMC_CMD_BIT                             HwPortE_MMC_CMD
#define MMC_CMD_SHIFT                           HwPortE_MMC_CMD_Shift
#endif

//****************************************************************************
//
// Determine the register, bit, and shift required to access the data line of
// the MMC bus.
//
//****************************************************************************
#ifdef HwPortABCD_MMC_DAT
#define MMC_DAT_PORT                            HwPortABCD
#define MMC_DAT_DIR                             HwDdrABCD
#define MMC_DAT_BIT                             HwPortABCD_MMC_DAT
#define MMC_DAT_SHIFT                           HwPortABCD_MMC_DAT_Shift
#endif
#ifdef HwPortE_MMC_DAT
#define MMC_DAT_PORT                            HwPortE
#define MMC_DAT_DIR                             HwDdrE
#define MMC_DAT_BIT                             HwPortE_MMC_DAT
#define MMC_DAT_SHIFT                           HwPortE_MMC_DAT_Shift
#endif

//****************************************************************************
//
// Only include this code if we have MMC data and command lines defined.
//
//****************************************************************************
#if defined(MMC_DAT_BIT) && defined(MMC_CMD_PORT)

//****************************************************************************
//
// Read-only code area.
//
//****************************************************************************
    _TEXT_

//****************************************************************************
//
// MMCReadByte reads a byte from the MMC card via the data line.
//
//****************************************************************************
    _EXPORT_ MMCReadByte
MMCReadByte _LABEL_
    //
    // Get a pointer to the MMC data space so that we can toggle the MMC clock.
    //
    ldr     r1, =HwMMCAddress

    //
    // Get a pointer to the internal registers.
    //
    ldr     r2, =HwBaseAddress

    //
    // Clock the MMC bus.
    //
    ldrb    r3, [r1]

    //
    // Read the GPIO data register.
    //
    ldr     r3, [r2, _CONST_ MMC_DAT_PORT]

    //
    // Mask off the data bit.
    //
    and     r3, r3, _CONST_ MMC_DAT_BIT

    //
    // Shift the data bit to the MSB.
    //
#if MMC_DAT_SHIFT != 7
    mov     r0, r3, ror _CONST_ ((MMC_DAT_SHIFT + 25) _MOD_ 32)
#else
    mov     r0, r3
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r3, [r1]

    //
    // Read the GPIO data register.
    //
    ldr     r3, [r2, _CONST_ MMC_DAT_PORT]

    //
    // Mask off the data bit.
    //
    and     r3, r3, _CONST_ MMC_DAT_BIT

    //
    // Shift the data bit to the second MSB.
    //
#if MMC_DAT_SHIFT != 6
    orr     r0, r0, r3, ror _CONST_ ((MMC_DAT_SHIFT + 26) _MOD_ 32)
#else
    orr     r0, r0, r3
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r3, [r1]

    //
    // Read the GPIO data register.
    //
    ldr     r3, [r2, _CONST_ MMC_DAT_PORT]

    //
    // Mask off the data bit.
    //
    and     r3, r3, _CONST_ MMC_DAT_BIT

    //
    // Shift the data bit to the third MSB.
    //
#if MMC_DAT_SHIFT != 5
    orr     r0, r0, r3, ror _CONST_ ((MMC_DAT_SHIFT + 27) _MOD_ 32)
#else
    orr     r0, r0, r3
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r3, [r1]

    //
    // Read the GPIO data register.
    //
    ldr     r3, [r2, _CONST_ MMC_DAT_PORT]

    //
    // Mask off the data bit.
    //
    and     r3, r3, _CONST_ MMC_DAT_BIT

    //
    // Shift the data bit to the fourth MSB.
    //
#if MMC_DAT_SHIFT != 4
    orr     r0, r0, r3, ror _CONST_ ((MMC_DAT_SHIFT + 28) _MOD_ 32)
#else
    orr     r0, r0, r3
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r3, [r1]

    //
    // Read the GPIO data register.
    //
    ldr     r3, [r2, _CONST_ MMC_DAT_PORT]

    //
    // Mask off the data bit.
    //
    and     r3, r3, _CONST_ MMC_DAT_BIT

    //
    // Shift the data bit to the fifth MSB.
    //
#if MMC_DAT_SHIFT != 3
    orr     r0, r0, r3, ror _CONST_ ((MMC_DAT_SHIFT + 29) _MOD_ 32)
#else
    orr     r0, r0, r3
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r3, [r1]

    //
    // Read the GPIO data register.
    //
    ldr     r3, [r2, _CONST_ MMC_DAT_PORT]

    //
    // Mask off the data bit.
    //
    and     r3, r3, _CONST_ MMC_DAT_BIT

    //
    // Shift the data bit to the sixth MSB.
    //
#if MMC_DAT_SHIFT != 2
    orr     r0, r0, r3, ror _CONST_ ((MMC_DAT_SHIFT + 30) _MOD_ 32)
#else
    orr     r0, r0, r3
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r3, [r1]

    //
    // Read the GPIO data register.
    //
    ldr     r3, [r2, _CONST_ MMC_DAT_PORT]

    //
    // Mask off the data bit.
    //
    and     r3, r3, _CONST_ MMC_DAT_BIT

    //
    // Shift the data bit to the seventh MSB.
    //
#if MMC_DAT_SHIFT != 1
    orr     r0, r0, r3, ror _CONST_ ((MMC_DAT_SHIFT + 31) _MOD_ 32)
#else
    orr     r0, r0, r3
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r3, [r1]

    //
    // Read the GPIO data register.
    //
    ldr     r3, [r2, _CONST_ MMC_DAT_PORT]

    //
    // Mask off the data bit.
    //
    and     r3, r3, _CONST_ MMC_DAT_BIT

    //
    // Shift the data bit to the LSB.
    //
#if MMC_DAT_SHIFT != 0
    orr     r0, r0, r3, lsr _CONST_ MMC_DAT_SHIFT
#else
    orr     r0, r0, r3
#endif

    //
    // Return the byte to the caller.
    //
    mov     pc, lr

//****************************************************************************
//
// MMCWriteByte writes a byte to the MMC card via the data line.
//
//****************************************************************************
    _EXPORT_ MMCWriteByte
MMCWriteByte _LABEL_
    //
    // Save lr to the stack.
    //
    stmdb   r13!, {lr}

    //
    // Get a pointer to the MMC data space so that we can toggle the MMC clock.
    //
    ldr     r2, =HwMMCAddress

    //
    // Get a pointer to the internal registers.
    //
    ldr     r3, =HwBaseAddress

    //
    // Load the current value of the GPIO data register.
    //
    ldr     r14, [r3, _CONST_ MMC_DAT_PORT]

    //
    // Clear the data bit.
    //
    bic     r14, r14, _CONST_ MMC_DAT_BIT

    //
    // Get the MSB of the byte to be written.
    //
#if MMC_DAT_SHIFT != 7
    mov     r1, r0, ror _CONST_ ((39 - MMC_DAT_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_DAT_BIT
#else
    and     r1, r0, _CONST_ MMC_DAT_BIT
#endif

    //
    // Set the data line to the next bit.
    //
    orr     r1, r1, r14
    str     r1, [r3, _CONST_ MMC_DAT_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the second MSB of the byte to be written.
    //
#if MMC_DAT_SHIFT != 6
    mov     r1, r0, ror _CONST_ ((38 - MMC_DAT_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_DAT_BIT
#else
    and     r1, r0, _CONST_ MMC_DAT_BIT
#endif

    //
    // Set the data line to the next bit.
    //
    orr     r1, r1, r14
    str     r1, [r3, _CONST_ MMC_DAT_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the third MSB of the byte to be written.
    //
#if MMC_DAT_SHIFT != 5
    mov     r1, r0, ror _CONST_ ((37 - MMC_DAT_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_DAT_BIT
#else
    and     r1, r0, _CONST_ MMC_DAT_BIT
#endif

    //
    // Set the data line to the next bit.
    //
    orr     r1, r1, r14
    str     r1, [r3, _CONST_ MMC_DAT_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the fourth MSB of the byte to be written.
    //
#if MMC_DAT_SHIFT != 4
    mov     r1, r0, ror _CONST_ ((36 - MMC_DAT_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_DAT_BIT
#else
    and     r1, r0, _CONST_ MMC_DAT_BIT
#endif

    //
    // Set the data line to the next bit.
    //
    orr     r1, r1, r14
    str     r1, [r3, _CONST_ MMC_DAT_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the fifth MSB of the byte to be written.
    //
#if MMC_DAT_SHIFT != 3
    mov     r1, r0, ror _CONST_ ((35 - MMC_DAT_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_DAT_BIT
#else
    and     r1, r0, _CONST_ MMC_DAT_BIT
#endif

    //
    // Set the data line to the next bit.
    //
    orr     r1, r1, r14
    str     r1, [r3, _CONST_ MMC_DAT_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the sixth MSB of the byte to be written.
    //
#if MMC_DAT_SHIFT != 2
    mov     r1, r0, ror _CONST_ ((34 - MMC_DAT_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_DAT_BIT
#else
    and     r1, r0, _CONST_ MMC_DAT_BIT
#endif

    //
    // Set the data line to the next bit.
    //
    orr     r1, r1, r14
    str     r1, [r3, _CONST_ MMC_DAT_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the seventh MSB of the byte to be written.
    //
#if MMC_DAT_SHIFT != 1
    mov     r1, r0, ror _CONST_ ((33 - MMC_DAT_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_DAT_BIT
#else
    and     r1, r0, _CONST_ MMC_DAT_BIT
#endif

    //
    // Set the data line to the next bit.
    //
    orr     r1, r1, r14
    str     r1, [r3, _CONST_ MMC_DAT_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the LSB of the byte to be written.
    //
#if MMC_DAT_SHIFT != 0
    mov     r1, r0, ror _CONST_ ((32 - MMC_DAT_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_DAT_BIT
#else
    and     r1, r0, _CONST_ MMC_DAT_BIT
#endif

    //
    // Set the data line to the next bit.
    //
    orr     r1, r1, r14
    str     r1, [r3, _CONST_ MMC_DAT_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Return to the caller.
    //
    ldmia   r13!, {pc}

//****************************************************************************
//
// MMCReadResponse reads a response from the MMC card via the command line.
//
//****************************************************************************
    _EXPORT_ MMCReadResponse
MMCReadResponse _LABEL_
    //
    // Save r4 and lr to the stack.
    //
    stmdb   r13!, {r4, lr}

    //
    // Get a pointer to the MMC data space so that we can toggle the MMC clock.
    //
    ldr     r2, =HwMMCAddress

    //
    // Get a pointer to the internal registers.
    //
    ldr     r3, =HwBaseAddress

    //
    // Make the command line an input.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_DIR]
#if MMC_CMD_SHIFT >= 24
    orr     r4, r4, _CONST_ MMC_CMD_BIT
#else
    bic     r4, r4, _CONST_ MMC_CMD_BIT
#endif
    str     r4, [r3, _CONST_ MMC_CMD_DIR]

    //
    // Clock the MMC bus up to 8192 times waiting for the device to be ready.
    //
    mov     r14, _CONST_ 0x00002000

    //
    // Clock the MMC bus.
    //
WaitTilReady _LABEL_
    ldrb    r4, [r2]

    //
    // Read the GPIO data register.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Mask off the command bit.
    //
    and     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Stop clocking if the command line is low.
    //
    cmp     r4, _CONST_ 0x00000000
    beq     ReadNextByte

    //
    // Decrement the count of clocks.
    //
    subs     r14, r14, _CONST_ 1

    //
    // Loop back if the count is non-zero.
    //
    bne      WaitTilReady

    //
    // We did not receive a response within 8192 MMC bus clocks, so return a
    // failure.
    //
    mov      r0, _CONST_ 0x00000000
    ldmia    r13!, {r4, pc}

    //
    // Read the GPIO data register.
    //
ReadNextByte _LABEL_
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Mask off the command bit.
    //
    and     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Shift the command bit to the MSB.
    //
#if MMC_CMD_SHIFT != 7
    mov     r14, r4, ror _CONST_ ((MMC_CMD_SHIFT + 25) _MOD_ 32)
#else
    mov     r14, r4
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r4, [r2]

    //
    // Read the GPIO data register.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Mask off the command bit.
    //
    and     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Shift the command bit to the second MSB.
    //
#if MMC_CMD_SHIFT != 6
    orr     r14, r14, r4, ror _CONST_ ((MMC_CMD_SHIFT + 26) _MOD_ 32)
#else
    orr     r14, r14, r4
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r4, [r2]

    //
    // Read the GPIO data register.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Mask off the command bit.
    //
    and     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Shift the command bit to the third MSB.
    //
#if MMC_CMD_SHIFT != 5
    orr     r14, r14, r4, ror _CONST_ ((MMC_CMD_SHIFT + 27) _MOD_ 32)
#else
    orr     r14, r14, r4
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r4, [r2]

    //
    // Read the GPIO data register.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Mask off the command bit.
    //
    and     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Shift the command bit to the fourth MSB.
    //
#if MMC_CMD_SHIFT != 4
    orr     r14, r14, r4, ror _CONST_ ((MMC_CMD_SHIFT + 28) _MOD_ 32)
#else
    orr     r14, r14, r4
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r4, [r2]

    //
    // Read the GPIO data register.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Mask off the command bit.
    //
    and     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Shift the command bit to the fifth MSB.
    //
#if MMC_CMD_SHIFT != 3
    orr     r14, r14, r4, ror _CONST_ ((MMC_CMD_SHIFT + 29) _MOD_ 32)
#else
    orr     r14, r14, r4
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r4, [r2]

    //
    // Read the GPIO data register.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Mask off the command bit.
    //
    and     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Shift the command bit to the sixth MSB.
    //
#if MMC_CMD_SHIFT != 2
    orr     r14, r14, r4, ror _CONST_ ((MMC_CMD_SHIFT + 30) _MOD_ 32)
#else
    orr     r14, r14, r4
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r4, [r2]

    //
    // Read the GPIO data register.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Mask off the command bit.
    //
    and     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Shift the command bit to the seventh MSB.
    //
#if MMC_CMD_SHIFT != 1
    orr     r14, r14, r4, ror _CONST_ ((MMC_CMD_SHIFT + 31) _MOD_ 32)
#else
    orr     r14, r14, r4
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r4, [r2]

    //
    // Read the GPIO data register.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Mask off the command bit.
    //
    and     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Shift the command bit to the LSB.
    //
#if MMC_CMD_SHIFT != 0
    orr     r14, r14, r4, lsr _CONST_ MMC_CMD_SHIFT
#else
    orr     r14, r14, r4
#endif

    //
    // Clock the MMC bus.
    //
    ldrb    r4, [r2]

    //
    // Save this byte.
    //
    strb    r14, [r0], _CONST_ 1

    //
    // Decrement the count of bytes to read.
    //
    subs    r1, r1, _CONST_ 1

    //
    // Loop back if there are more bytes to read.
    //
    bne     ReadNextByte

    //
    // Restore r4 from the stack and return to the caller.
    //
    mov     r0, _CONST_ 0x00000001
    ldmia   r13!, {r4, pc}

//****************************************************************************
//
// MMCWriteCommand writes a command to the MMC card via the command line.
//
//****************************************************************************
    _EXPORT_ MMCWriteCommand
MMCWriteCommand _LABEL_
    //
    // Save r4, r5, and lr to the stack.
    //
    stmdb   r13!, {r4, r5, lr}

    //
    // Get a pointer to the MMC data space so that we can toggle the MMC clock.
    //
    ldr     r2, =HwMMCAddress

    //
    // Get a pointer to the internal registers.
    //
    ldr     r3, =HwBaseAddress

    //
    // Make the command line an output.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_DIR]
#if MMC_CMD_SHIFT >= 24
    bic     r4, r4, _CONST_ MMC_CMD_BIT
#else
    orr     r4, r4, _CONST_ MMC_CMD_BIT
#endif
    str     r4, [r3, _CONST_ MMC_CMD_DIR]

    //
    // Load the current value of the GPIO data register.
    //
    ldr     r4, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Clear the command bit.
    //
    bic     r4, r4, _CONST_ MMC_CMD_BIT

    //
    // Send six bytes to the MMC card.
    //
    mov     r14, _CONST_ 0x00000006

    //
    // Get the next byte to be sent.
    //
WriteNextByte _LABEL_
    ldrb    r5, [r0], _CONST_ 1

    //
    // Get the MSB of the byte to be written.
    //
#if MMC_CMD_SHIFT != 7
    mov     r1, r5, ror _CONST_ ((39 - MMC_CMD_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_CMD_BIT
#else
    and     r1, r5, _CONST_ MMC_CMD_BIT
#endif

    //
    // Set the command line to the next bit.
    //
    orr     r1, r1, r4
    str     r1, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the second MSB of the byte to be written.
    //
#if MMC_CMD_SHIFT != 6
    mov     r1, r5, ror _CONST_ ((38 - MMC_CMD_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_CMD_BIT
#else
    and     r1, r5, _CONST_ MMC_CMD_BIT
#endif

    //
    // Set the command line to the next bit.
    //
    orr     r1, r1, r4
    str     r1, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the third MSB of the byte to be written.
    //
#if MMC_CMD_SHIFT != 5
    mov     r1, r5, ror _CONST_ ((37 - MMC_CMD_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_CMD_BIT
#else
    and     r1, r5, _CONST_ MMC_CMD_BIT
#endif

    //
    // Set the command line to the next bit.
    //
    orr     r1, r1, r4
    str     r1, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the fourth MSB of the byte to be written.
    //
#if MMC_CMD_SHIFT != 4
    mov     r1, r5, ror _CONST_ ((36 - MMC_CMD_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_CMD_BIT
#else
    and     r1, r5, _CONST_ MMC_CMD_BIT
#endif

    //
    // Set the command line to the next bit.
    //
    orr     r1, r1, r4
    str     r1, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the fifth MSB of the byte to be written.
    //
#if MMC_CMD_SHIFT != 3
    mov     r1, r5, ror _CONST_ ((35 - MMC_CMD_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_CMD_BIT
#else
    and     r1, r5, _CONST_ MMC_CMD_BIT
#endif

    //
    // Set the command line to the next bit.
    //
    orr     r1, r1, r4
    str     r1, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the sixth MSB of the byte to be written.
    //
#if MMC_CMD_SHIFT != 2
    mov     r1, r5, ror _CONST_ ((34 - MMC_CMD_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_CMD_BIT
#else
    and     r1, r5, _CONST_ MMC_CMD_BIT
#endif

    //
    // Set the command line to the next bit.
    //
    orr     r1, r1, r4
    str     r1, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the seventh MSB of the byte to be written.
    //
#if MMC_CMD_SHIFT != 1
    mov     r1, r5, ror _CONST_ ((33 - MMC_CMD_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_CMD_BIT
#else
    and     r1, r5, _CONST_ MMC_CMD_BIT
#endif

    //
    // Set the command line to the next bit.
    //
    orr     r1, r1, r4
    str     r1, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Get the LSB of the byte to be written.
    //
#if MMC_CMD_SHIFT != 0
    mov     r1, r5, ror _CONST_ ((32 - MMC_CMD_SHIFT) _MOD_ 32)
    and     r1, r1, _CONST_ MMC_CMD_BIT
#else
    and     r1, r5, _CONST_ MMC_CMD_BIT
#endif

    //
    // Set the command line to the next bit.
    //
    orr     r1, r1, r4
    str     r1, [r3, _CONST_ MMC_CMD_PORT]

    //
    // Clock the MMC bus.
    //
    ldrb    r1, [r2]

    //
    // Decrement the count of bytes to send.
    //
    subs    r14, r14, _CONST_ 1

    //
    // Loop back if there are more bytes to send.
    //
    bne     WriteNextByte

    //
    // Restore r4 and r5 from the stack, and return to the caller.
    //
    ldmia   r13!, {r4, r5, pc}

#endif

    _END_
