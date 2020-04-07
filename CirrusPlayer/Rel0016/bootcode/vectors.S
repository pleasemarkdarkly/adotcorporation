//****************************************************************************
//
// VECTORS.S - Bootup code for the Internet audio player.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
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
// Memory buffer used to contain the SVC mode stack.
//
//****************************************************************************
SVCStack _LABEL_
    _SPACE_ 0x400
SVCStackEnd _LABEL_

//****************************************************************************
//
// The following contains the address of the first free word if internal SRAM.
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
// one instruction, so we use that instruction to load the program counter with
// the address of the handler routine, which is stored in a table following the
// vector table.
//
//****************************************************************************
    _ENTRY_

    b       ResetHandler
    ldr     pc, UndefV
    ldr     pc, SWIV
    ldr     pc, PAbortV
    ldr     pc, DAbortV
    ldr     pc, UnusedV
    ldr     pc, IRQV
    ldr     pc, FIQV

//****************************************************************************
//
// There is an additional "vector" at 0x24 which can be called by the player to
// start an update.
//
//****************************************************************************

    b       UpdateHandler

//****************************************************************************
//
// The following is the actual vector table.  It contains the addresses of the
// routines which handle each exception type.
//
//****************************************************************************
UndefV _LABEL_
    _WORD_  HwPlayerAddress + 0x04
SWIV _LABEL_
    _WORD_  HwPlayerAddress + 0x08
PAbortV _LABEL_
    _WORD_  HwPlayerAddress + 0x0c
DAbortV _LABEL_
    _WORD_  HwPlayerAddress + 0x10
UnusedV _LABEL_
    _WORD_  HwPlayerAddress + 0x14
IRQV _LABEL_
    _WORD_  HwPlayerAddress + 0x18
FIQV _LABEL_
    _WORD_  HwPlayerAddress + 0x1c

//****************************************************************************
//
// The table used to compute the CRC32 value of the player image.
//
//****************************************************************************
CRCTable _LABEL_
    _WORD_  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba
    _WORD_  0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3
    _WORD_  0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988
    _WORD_  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91
    _WORD_  0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de
    _WORD_  0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7
    _WORD_  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec
    _WORD_  0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5
    _WORD_  0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172
    _WORD_  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b
    _WORD_  0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940
    _WORD_  0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59
    _WORD_  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116
    _WORD_  0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f
    _WORD_  0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924
    _WORD_  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d
    _WORD_  0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a
    _WORD_  0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433
    _WORD_  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818
    _WORD_  0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01
    _WORD_  0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e
    _WORD_  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457
    _WORD_  0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c
    _WORD_  0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65
    _WORD_  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2
    _WORD_  0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb
    _WORD_  0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0
    _WORD_  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9
    _WORD_  0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086
    _WORD_  0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f
    _WORD_  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4
    _WORD_  0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad
    _WORD_  0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a
    _WORD_  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683
    _WORD_  0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8
    _WORD_  0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1
    _WORD_  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe
    _WORD_  0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7
    _WORD_  0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc
    _WORD_  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5
    _WORD_  0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252
    _WORD_  0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b
    _WORD_  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60
    _WORD_  0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79
    _WORD_  0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236
    _WORD_  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f
    _WORD_  0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04
    _WORD_  0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d
    _WORD_  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a
    _WORD_  0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713
    _WORD_  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38
    _WORD_  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21
    _WORD_  0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e
    _WORD_  0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777
    _WORD_  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c
    _WORD_  0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45
    _WORD_  0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2
    _WORD_  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db
    _WORD_  0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0
    _WORD_  0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9
    _WORD_  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6
    _WORD_  0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf
    _WORD_  0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94
    _WORD_  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d

//****************************************************************************
//
// ResetHandler is the startup code which is executed when the ARM processor
// first starts executing after a reset.  This is responsible for choosing the
// correct image to execute (the player code or the update code).
//
//****************************************************************************
ResetHandler _LABEL_
    //
    // Put the ARM processor into SVC mode and disable the IRQ and FIQ
    // interrupts.
    //
    ldr     r0, =0x000000d3
    msr     cpsr_cf, r0

    //
    // Program the memory controller for nCS0 to speed access to the program
    // ROM.
    //
    ldr     r0, =0x80000000
    ldr     r1, =(HwMemConfig1_Value _AND_ 0x000000ff)
    str     r1, [r0, _CONST_ HwMemConfig1]

    //
    // Set the ARM clock rate to 74MHz.
    //
    add     r0, r0, _CONST_ 0x00002000
    ldr     r1, =0x00000006
    str     r1, [r0, _CONST_ (HwControl3 - 0x2000)]

//
// TEMPORARY TEMPORARY TEMPORARY
//
    //
    // If PA0 is high, then ignore the player image (even if it is valid).
    //
    sub     r0, r0, _CONST_ 0x00002000
    ldr     r0, [r0, _CONST_ HwPortABCD]
    tst     r0, _CONST_ 0x00000001
    bne     skip_image
//
// TEMPORARY TEMPORARY TEMPORARY
//

    //
    // Check the player image to see if it is valid.
    //
    ldr     r0, =HwPlayerAddress
    bl      CheckImage

    //
    // If the player image is valid, then branch to it.
    //
    cmp     r0, _CONST_ 0x00000000
    ldrne   pc, =HwPlayerAddress

    //
    // Copy the code image from ROM to RAM.
    //
//
// TEMPORARY TEMPORARY TEMPORARY
//
skip_image _LABEL_
//
// TEMPORARY TEMPORARY TEMPORARY
//
    _IMPORT_ _ROLIMIT_
    ldr     r1, =0x00000000
    ldr     r2, =0x60000000
    ldr     r3, =_ROLIMIT_
    bic     r3, r3, _CONST_ 0x00f00000
reset_copy_ro _LABEL_
        ldr     r0, [r1], _CONST_ 4
        str     r0, [r2], _CONST_ 4
        subs    r3, r3, _CONST_ 4
        bne     reset_copy_ro

    //
    // Copy the read-write data block from ROM to RAM.
    //
    _IMPORT_ _RWBASE_
    _IMPORT_ _ZIBASE_
    mov     r0, r1
    ldr     r1, =_RWBASE_
    bic     r1, r1, _CONST_ 0x00f00000
    orr     r1, r1, _CONST_ 0x60000000
    ldr     r2, =_ZIBASE_
    bic     r2, r2, _CONST_ 0x00f00000
    orr     r2, r2, _CONST_ 0x60000000
reset_copy_rw _LABEL_
        cmp     r1, r2
        ldrne   r3, [r0], _CONST_ 4
        strne   r3, [r1], _CONST_ 4
        bne     reset_copy_rw

    //
    // Fill the zero initialized data block in RAM with zeros.
    //
    _IMPORT_ _RWLIMIT_
    ldr     r2, =_RWLIMIT_
    bic     r2, r2, _CONST_ 0x00f00000
    orr     r2, r2, _CONST_ 0x60000000
    ldr     r3, =0x00000000
reset_fill_zi _LABEL_
        cmp     r1, r2
        strne   r3, [r1], _CONST_ 4
        bne     reset_fill_zi

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
    bic     r0, r0, _CONST_ 0x00f00000
    orr     r0, r0, _CONST_ 0x60000000
    mcr     _CP15_, 0, r0, _CR2_, _CR0_

    //
    // Enable the MMU.
    //
    ldr     r0, =0x00000071
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
    ldr     r3, =HwControl1_Value
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
    // Set up the stack pointer.
    //
    ldr     r13, =SVCStackEnd

    //
    // Initialize the value of ulEndOfRAM.
    //
    ldr     r0, =ulEndOfRAM
    ldr     r1, =_RWLIMIT_
    str     r1, [r0]

    //
    // Call the update code.
    //
    mov     r0, _CONST_ 0x00000000
    mov     lr, pc
    _IMPORT_ Update
    ldr     pc, =Update

    //
    // Disable the MMU.
    //
    ldr     r0, =0x00000070
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
    // Branch back to the reset vector.
    //
    mov     pc, r0

    //
    // Tell the assembler to put in-line data here.
    //
    _LTORG_

//****************************************************************************
//
// UpdateHandler is the routine which is called by the player when a software
// update needs to be performed.
//
//****************************************************************************
UpdateHandler _LABEL_
    //
    // Disable the FIQ and IRQ interrupts.
    //
    mrs     r0, cpsr
    orr     r0, r0, _CONST_ 0x000000c0
    msr     cpsr_cf, r0

    //
    // Load pointers to the EP7209 internal registers.
    //
    ldr     r0, =HwBaseAddress
    add     r1, r0, _CONST_ 0x00001000
    add     r2, r0, _CONST_ 0x00002000

    //
    // Disable all the interrupt sources.
    //
    ldr     r3, =0x00000000
    str     r3, [r0, _CONST_ HwIntMask]
    str     r3, [r1, _CONST_ (HwIntMask2 - 0x1000)]
    str     r3, [r2, _CONST_ (HwIntMask3 - 0x2000)]

    //
    // Disable the MMU.
    //
    ldr     r0, =0x00000070
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
    // Copy the code image from ROM to RAM.
    //
    ldr      r1, =0x00000000
    ldr      r2, =0x60000000
    ldr      r3, =_ROLIMIT_
    bic      r3, r3, _CONST_ 0x00f00000
update_copy_ro _LABEL_
        ldr     r0, [r1], _CONST_ 4
        str     r0, [r2], _CONST_ 4
        subs    r3, r3, _CONST_ 4
        bne     update_copy_ro

    //
    // Copy the read-write data block from ROM to RAM.
    //
    mov     r0, r1
    ldr     r1, =_RWBASE_
    bic     r1, r1, _CONST_ 0x00f00000
    orr     r1, r1, _CONST_ 0x60000000
    ldr     r2, =_ZIBASE_
    bic     r2, r2, _CONST_ 0x00f00000
    orr     r2, r2, _CONST_ 0x60000000
update_copy_rw _LABEL_
        cmp     r1, r2
        ldrne   r3, [r0], _CONST_ 4
        strne   r3, [r1], _CONST_ 4
        bne     update_copy_rw

    //
    // Fill the zero initialized data block in RAM with zeros.
    //
    ldr     r2, =_RWLIMIT_
    bic     r2, r2, _CONST_ 0x00f00000
    orr     r2, r2, _CONST_ 0x60000000
    ldr     r3, =0x00000000
update_fill_zi _LABEL_
        cmp     r1, r2
        strne   r3, [r1], _CONST_ 4
        bne     update_fill_zi

    //
    // Tell the MMU where to find the new page table.
    //
    _IMPORT_ PageTable
    ldr     r0, =PageTable
    bic     r0, r0, _CONST_ 0x00f00000
    orr     r0, r0, _CONST_ 0x60000000
    mcr     _CP15_, 0, r0, _CR2_, _CR0_

    //
    // Enable the MMU.
    //
    ldr     r0, =0x00000071
    mcr     _CP15_, 0, r0, _CR1_, _CR0_

    //
    // There should always be two NOP instructions following the enable or
    // disable of the MMU.
    //
    nop
    nop

    //
    // Set up the stack pointer.
    //
    ldr     r13, =SVCStackEnd

    //
    // Initialize the value of ulEndOfRAM.
    //
    ldr     r0, =ulEndOfRAM
    add     r1, r0, _CONST_ 0x00000004
    str     r1, [r0]

    //
    // Call the update code.  We set the link register to zero so that when the
    // update code returns, we will re-initialize the system.
    //
    mov     r0, _CONST_ 0x00000001
    mov     lr, pc
    ldr     pc, =Update

    //
    // Disable the MMU.
    //
    ldr     r0, =0x00000070
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
    // Branch back to the reset vector.
    //
    mov     pc, r0

    //
    // Tell the assembler to put in-line data here.
    //
    _LTORG_

//****************************************************************************
//
// CheckImage determines if the image at the specified location is "valid".
// We check to see that the hardware ID in the image matches the hardware ID of
// the player and that the CRC32 of the image is correct.
//
//****************************************************************************
CheckImage _LABEL_
    //
    // Load the base address from the image.
    //
    ldr     r1, [r0, _CONST_ 0x20]

    //
    // Compare the base address of the image to its actual location in memory.
    //
    add     r2, r0, _CONST_ 0x20
    cmp     r1, r2
    movne   r0, _CONST_ 0x00000000
    movne   pc, lr

    //
    // Load the hardware ID from the image.
    //
    ldr     r1, [r0, _CONST_ 0x28]

    //
    // Compare the hardware ID of the image to be sure that it matches the
    // hardware ID of the player.
    //
    ldr     r2, =HardwareID
    cmp     r1, r2
    movne   r0, _CONST_ 0x00000000
    movne   pc, lr

    //
    // Load a pointer to the CRC32 data table.
    //
    ldr     r1, =CRCTable
    bic     r1, r1, _CONST_ 0x00f00000

    //
    // Get the length of the image.
    //
    ldr     r2, [r0, _CONST_ 0x24]

    //
    // If the length of the image is zero, greater than 1Meg, or not a multiple
    // of 4 bytes, then assume there is an error with the image.
    //
    cmp     r2, _CONST_ 0x00000000
    moveq   r0, _CONST_ 0x00000000
    moveq   pc, lr
    cmp     r2, _CONST_ 0x00100000
    movhi   r0, _CONST_ 0x00000000
    movhi   pc, lr
    tst     r2, _CONST_ 0x00000003
    movne   r0, _CONST_ 0x00000000
    movne   pc, lr

    //
    // Get the CRC of the image.
    //
    ldr     r6, [r0, _CONST_ 0x2c]

    //
    // The CRC32 starts with the first word after the image descriptor
    // structure.
    //
    add     r0, r0, _CONST_ 0x30
    sub     r2, r2, _CONST_ 0x30

    //
    // Set the initial value of the CRC32.
    //
    mvn     r3, _CONST_ 0x00000000

    //
    // While there are more words in the image, load them and add them to the
    // checksum.
    //
check_word _LABEL_
        //
        // Load the next word from the image.
        //
        ldr     r4, [r0], _CONST_ 4

        //
        // Generate the next CRC32 value based on the first byte.
        //
        eor     r5, r3, r4
        and     r5, r5, _CONST_ 0xff
        ldr     r5, [r1, r5, lsl _CONST_ 2]
        mov     r3, r3, lsr _CONST_ 8
        eor     r3, r3, r5

        //
        // Generate the next CRC32 value based on the second byte.
        //
        eor     r5, r3, r4, lsr _CONST_ 8
        and     r5, r5, _CONST_ 0xff
        ldr     r5, [r1, r5, lsl _CONST_ 2]
        mov     r3, r3, lsr _CONST_ 8
        eor     r3, r3, r5

        //
        // Generate the next CRC32 value based on the third byte.
        //
        eor     r5, r3, r4, lsr _CONST_ 16
        and     r5, r5, _CONST_ 0xff
        ldr     r5, [r1, r5, lsl _CONST_ 2]
        mov     r3, r3, lsr _CONST_ 8
        eor     r3, r3, r5

        //
        // Generate the next CRC32 value based on the fourth byte.
        //
        eor     r5, r3, r4, lsr _CONST_ 24
        and     r5, r5, _CONST_ 0xff
        ldr     r5, [r1, r5, lsl _CONST_ 2]
        mov     r3, r3, lsr _CONST_ 8
        eor     r3, r3, r5

        //
        // Decrement the count of bytes and loop back if there are more.
        //
        subs    r2, r2, _CONST_ 4
        bne     check_word

    //
    // See if the CRC32 mataches.
    //
    cmp     r3, r6

    //
    // Indicate if the CRC32 matched.
    //
    movne   r0, _CONST_ 0x00000000
    moveq   r0, _CONST_ 0x00000001

    //
    // Return.
    //
    mov     pc, lr

    _END_
