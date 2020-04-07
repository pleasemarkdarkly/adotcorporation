/****************************************************************************/
/*                                                                          */
/* PLAYER.LDS - LD script to link the Internet Audio Player.                */
/*                                                                          */
/* Copyright (c) 2001 Cirrus Logic, Inc.                                    */
/*                                                                          */
/****************************************************************************/

/*
 * Define where to put the various object files in the player.
 */
SECTIONS
{
    /*
     * Vectors.o must reside first.
     */
    .text ROBASE :
    {
        __robase__ = .;
        vectors.o(.text)
    }

    /*
     * Place all other read-only objects following the vectors.
     */
    .text :
    {
        *(EXCLUDE_FILE(mmu.o) .text);
        *(.rodata);
    }

    /*
     * Place the page table at the end of the ROM.
     */
    .text :
    {
        mmu.o(.text);
        __rolimit__ = .;
    }

    /*
     * Put all of the read-write data sections at the beginning of internal
     * SRAM, with their initializers residing at the end of the ROM section.
     */
    .data RWBASE : AT(ADDR(.text) + SIZEOF(.text))
    {
        __rwbase__ = .;
        *(.data);
    }

    /*
     * Place all the zero-init sections after the read-write data sections.
     */
    .bss . :
    {
        __zibase__ = .;
        *(.bss);
        __rwlimit__ = .;
    }
}
