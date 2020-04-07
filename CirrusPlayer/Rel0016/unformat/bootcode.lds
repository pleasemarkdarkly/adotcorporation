/****************************************************************************/
/*                                                                          */
/* BOOTCODE.LDS - LD script to link the unformat code.                      */
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
     * Put the read-only sections first.
     */
    .text ROBASE :
    {
        __robase__ = .;
        *(.text .rodata);
        __rolimit_ = .;
    }

    /*
     * Put all of the read-write data sections after the read-only sections.
     */
    .data . :
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
