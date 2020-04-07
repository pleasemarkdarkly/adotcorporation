/****************************************************************************/
/*                                                                          */
/* FLASHER.LDS - LD script to link the FLASH programming code.              */
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
        __rolimit__ = .;
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
