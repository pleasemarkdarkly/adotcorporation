//****************************************************************************
//
// MMU.S - Builds the MMU page table.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

#include "../asmdefs.h"

//****************************************************************************
//
// The MMU page table.  The virtual memory map created by this page table
// is as follows:
//
//     0x00000000 - 0x000fffff => 1Meg of program ROM (nCS0)
//     0x00100000 - 0x001fffff => 1Meg of internal SRAM (only 37.5K exists)
//     0x00200000 - 0x002fffff => EP7209 internal registers
//     0x00300000 - 0x003fffff => NAND FLASH interface (nCS1)
//     0x00400000 - 0x004fffff => USB controller (nCS4)
//     0x00500000 - 0x005fffff => LCD controller (nCS2)
//     0x00600000 - 0x006fffff => MMC interface (nCS3)
//     0x00700000 - 0xffffffff => Inaccessible memory
//
// The area for the page table is located at the end of the ROM in an effort to
// cut down on the padding added before the page table (since it must be
// aligned on a 16K byte boundary).
//
//****************************************************************************

    _MMU_ALIGN_
    _MMU_

    _EXPORT_ PageTable
PageTable _LABEL_
    //
    // Program ROM (nCS0), cached, write prohibited
    //
    _WORD_  0x0000001a

    //
    // EP7209 internal SRAM, cached and write buffered
    //
    _WORD_  0x60000c1e

    //
    // EP7209 internal registers
    //
    _WORD_  0x80000c12

    //
    // NAND FLASH (nCS1)
    //
    _WORD_  0x10000c12

    //
    // USB controller (nCS4)
    //
    _WORD_  0x40000c12

    //
    // LCD controller (nCS2)
    //
    _WORD_  0x20000c12

    //
    // MMC (nCS3)
    //
    _WORD_  0x30000c12

    //
    // Fill the remainder of the page table with zeros.
    //
    _SPACE_ 0x3fe4

    _END_
