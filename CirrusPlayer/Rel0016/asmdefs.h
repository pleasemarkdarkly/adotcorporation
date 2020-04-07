//****************************************************************************
//
// ASMDEFS.H - A set of defines for abstracting the assembler directivies.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// The following set of defines are for use with the GNU assembler.
//
//****************************************************************************
#ifdef gcc

//
// Put the assembler into ARM assembly mode.
//
#define _CODE32_                        .code 32

//
// Put the assembler into Thumb assembly mode.
//
#define _CODE16_                        .code 16

//
// Define a read-only code section.
//
#define _TEXT_                          .text

//
// Define a read-only code section for the MMU page table.
//
#define _MMU_                           .text

//
// Define the alignment of the MMU page table section.
//
#define _MMU_ALIGN_                     .align 14

//
// Define a read-write data section.
//
#define _DATA_                          .data

//
// Define a read-write, zero-initialized data section.
//
#define _BSS_                           .bss

//
// Allocate memory at the current location within the section.
//
#define _SPACE_                         .space

//
// Indicate that the following value is a constant.
//
#define _CONST_                         #

//
// Indicate that the following label is a Thumb entry point.
//
#define _THUMB_LABEL_                   .thumb_func

//
// Indicate that a label has been defined.
//
#define _LABEL_                         :

//
// Mark the current location as the entry point of the program.
//
#define _ENTRY_

//
// Declare a label as being defined in another file.
//
#define _IMPORT_                        .extern

//
// Make a label accessible in another file.
//
#define _EXPORT_                        .global

//
// Allocate a word of memory at the current location with this section.
//
#define _WORD_                          .word

//
// Tell the assembler to output the literal pool at the current location.
//
#define _LTORG_                         .ltorg

//
// Tell the assembler that it has reached the end of the file.
//
#define _END_

//
// The name of the symbol that contains the end of the read-only section.
//
#define _ROLIMIT_                       __rolimit__

//
// The name of the symbol that contains the beginning of the read-write
// section.
//
#define _RWBASE_                        __rwbase__

//
// The name of the symbol that contains the beginning of the zero-initialized
// read-write section.
//
#define _ZIBASE_                        __zibase__

//
// The name of the symbol that contains the end of the read-write section.
//
#define _RWLIMIT_                       __rwlimit__

//
// The modulo arithmetic operation.
//
#define _MOD_                           %

//
// The logical AND operation.
//
#define _AND_                           &

//
// Coprocessor 15.
//
#define _CP15_                          p15

//
// Coprocessor register 0.
//
#define _CR0_                           c0

//
// Coprocessor register 1.
//
#define _CR1_                           c1

//
// Coprocessor register 2.
//
#define _CR2_                           c2

//
// Coprocessor register 3.
//
#define _CR3_                           c3

//
// Coprocessor register 7.
//
#define _CR7_                           c7

//
// Coprocessor register 8.
//
#define _CR8_                           c8

#endif

//****************************************************************************
//
// The following set of defines are for use with the ARM SDT 2.5 assembler.
//
//****************************************************************************
#ifdef sdt25

//
// Put the assembler into ARM assembly mode.
//
#define _CODE32_                        CODE32

//
// Put the assembler into Thumb assembly mode.
//
#define _CODE16_                        CODE16

//
// Define a read-only code section.
//
#define _TEXT_                          AREA Text, CODE, READONLY

//
// Define a read-only code section for the MMU page table.
//
#define _MMU_                           AREA ZText, DATA, READONLY, ALIGN=14

//
// Define the alignment of the MMU page table section.
//
#define _MMU_ALIGN_

//
// Define a read-write data section.
//
#define _DATA_                          AREA Data, DATA, READWRITE

//
// Define a read-write, zero-initialized data section.
//
#define _BSS_                           AREA Bss, DATA, READWRITE, NOINIT

//
// Allocate memory at the current location within the section.
//
#define _SPACE_                         %

//
// Indicate that the following value is a constant.
//
#define _CONST_                         #

//
// Indicate that the following label is a Thumb entry point.
//
#define _THUMB_LABEL_

//
// Indicate that a label has been defined.
//
#define _LABEL_

//
// Mark the current location as the entry point of the program.
//
#define _ENTRY_                         ENTRY

//
// Declare a label as being defined in another file.
//
#define _IMPORT_                        IMPORT

//
// Make a label accessible in another file.
//
#define _EXPORT_                        EXPORT

//
// Allocate a word of memory at the current location with this section.
//
#define _WORD_                          DCD

//
// Tell the assembler to output the literal pool at the current location.
//
#define _LTORG_                         LTORG

//
// Tell the assembler that it has reached the end of the file.
//
#define _END_                           END

//
// The name of the symbol that contains the end of the read-only section.
//
#define _ROLIMIT_                       |Image$$RO$$Limit|

//
// The name of the symbol that contains the beginning of the read-write
// section.
//
#define _RWBASE_                        |Image$$RW$$Base|

//
// The name of the symbol that contains the beginning of the zero-initialized
// read-write section.
//
#define _ZIBASE_                        |Image$$ZI$$Base|

//
// The name of the symbol that contains the end of the read-write section.
//
#define _RWLIMIT_                       |Image$$RW$$Limit|

//
// The modulo arithmetic operation.
//
#define _MOD_                           :MOD:

//
// The logical AND operation.
//
#define _AND_                           :AND:

//
// Coprocessor 15.
//
#define _CP15_                          p15

//
// Coprocessor register 0.
//
#define _CR0_                           c0

//
// Coprocessor register 1.
//
#define _CR1_                           c1

//
// Coprocessor register 2.
//
#define _CR2_                           c2

//
// Coprocessor register 3.
//
#define _CR3_                           c3

//
// Coprocessor register 7.
//
#define _CR7_                           c7

//
// Coprocessor register 8.
//
#define _CR8_                           c8

#endif

//****************************************************************************
//
// The following set of defines are for use with the ARM ADS 1.x assembler.
//
//****************************************************************************
#ifdef ads

//
// Put the assembler into ARM assembly mode.
//
#define _CODE32_                        CODE32

//
// Put the assembler into Thumb assembly mode.
//
#define _CODE16_                        CODE16

//
// Define a read-only code section.
//
#define _TEXT_                          AREA Text, CODE, READONLY

//
// Define a read-only code section for the MMU page table.
//
#define _MMU_                           AREA ZText, DATA, READONLY, ALIGN=14

//
// Define the alignment of the MMU page table section.
//
#define _MMU_ALIGN_

//
// Define a read-write data section.
//
#define _DATA_                          AREA Data, DATA, READWRITE

//
// Define a read-write, zero-initialized data section.
//
#define _BSS_                           AREA Bss, DATA, READWRITE, NOINIT

//
// Allocate memory at the current location within the section.
//
#define _SPACE_                         %

//
// Indicate that the following value is a constant.
//
#define _CONST_                         #

//
// Indicate that the following label is a Thumb entry point.
//
#define _THUMB_LABEL_

//
// Indicate that a label has been defined.
//
#define _LABEL_

//
// Mark the current location as the entry point of the program.
//
#define _ENTRY_                         ENTRY

//
// Declare a label as being defined in another file.
//
#define _IMPORT_                        IMPORT

//
// Make a label accessible in another file.
//
#define _EXPORT_                        EXPORT

//
// Allocate a word of memory at the current location with this section.
//
#define _WORD_                          DCD

//
// Tell the assembler to output the literal pool at the current location.
//
#define _LTORG_                         LTORG

//
// Tell the assembler that it has reached the end of the file.
//
#define _END_                           END

//
// The name of the symbol that contains the end of the read-only section.
//
#define _ROLIMIT_                       |Image$$RO$$Limit|

//
// The name of the symbol that contains the beginning of the read-write
// section.
//
#define _RWBASE_                        |Image$$RW$$Base|

//
// The name of the symbol that contains the beginning of the zero-initialized
// read-write section.
//
#define _ZIBASE_                        |Image$$ZI$$Base|

//
// The name of the symbol that contains the end of the read-write section.
//
#define _RWLIMIT_                       |Image$$RW$$Limit|

//
// The modulo arithmetic operation.
//
#define _MOD_                           :MOD:

//
// The logical AND operation.
//
#define _AND_                           :AND:

//
// Coprocessor 15.
//
#define _CP15_                          p15

//
// Coprocessor register 0.
//
#define _CR0_                           c0

//
// Coprocessor register 1.
//
#define _CR1_                           c1

//
// Coprocessor register 2.
//
#define _CR2_                           c2

//
// Coprocessor register 3.
//
#define _CR3_                           c3

//
// Coprocessor register 7.
//
#define _CR7_                           c7

//
// Coprocessor register 8.
//
#define _CR8_                           c8

#endif
