/****************************************************************************/
/*                                                                          */
/* SW_UPDT.LDS - LD script to link the software update code.                */
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
        *(.text);
        *(.rodata);
        __rolimit__ = .;
    }

    /*
     * The read-write data secitons, with their initializers residing at the
     * end of the .text section.
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
