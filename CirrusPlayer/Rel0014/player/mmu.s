;*****************************************************************************
;
; MMU.S - Builds the MMU page table.
;
; Copyright (c) 1999-2001 Cirrus Logic, Inc.
;
;*****************************************************************************

;*****************************************************************************
;
; The MMU page table.  The virtual memory map created by this page table
; is as follows:
;
;     0x00000000 - 0x000fffff => 1Meg of program ROM (nCS0)
;     0x00100000 - 0x001fffff => 1Meg of internal SRAM (only 37.5K exists)
;     0x00200000 - 0x002fffff => EP7209 internal registers
;     0x00300000 - 0x003fffff => NAND FLASH interface (nCS1)
;     0x00400000 - 0x004fffff => USB controller (nCS4)
;     0x00500000 - 0x005fffff => LCD controller (nCS2)
;
; The area for the page table is located at the end of the ROM in an effort to
; cut down on the padding added before the page table (since it must be
; aligned on a 16K byte boundary).  This really should have all 4K entries
; populated, but we simply populate the first six entries since that is all
; that we use.  Unpredictable results will occur if the program inadvertently
; accesses a memory location past 6Meg.
;
;*****************************************************************************

    AREA    |ZReadOnlyEndOfROM$$PageTable|, DATA, READONLY, ALIGN=14

    EXPORT  PageTable
PageTable
    ;
    ; Program ROM (nCS0), cached
    ;
    DCD     0x00000c1a

    ;
    ; EP7209 internal SRAM, cached and write buffered
    ;
    DCD     0x60000c1e

    ;
    ; EP7209 internal registers
    ;
    DCD     0x80000c12

    ;
    ; NAND FLASH (nCS1)
    ;
    DCD     0x10000c12

    ;
    ; USB controller (nCS4)
    ;
    DCD     0x40000c12

    ;
    ; LCD controller (nCS2)
    ;
    DCD     0x20000c12

    END
