/*****************************************************************************
* FileName:  UTIL.C - Contains string manipulation and byte order conversion
*
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1996-1999 SanDisk Corporation
* Copyright EBS Inc. 1996
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
******************************************************************************/
/* UTIL.C - Contains string manipulation and byte order conversion routines */

#include <dadio/io/sdapi.h>



/*****************************************************************************
* Name: COPYBUFF  - Copy one buffer to another
*
* Description
*       Essentially memcpy. Copy number of BYTES from source to target
*       buffer.
*
* Entries:
*       SDVOID *vto       Target buffer
*       SDVOID *vfrom     Source buffer
*       INT16 size      Number of bytes to copy
*
*Returns
*        None
*
******************************************************************************/
#ifndef dev_copybuff                /* might be using memcpy */
SDVOID dev_copybuff(SDVOID *vto, SDVOID *vfrom, INT16 size) /* __fn__*/
{
        UINT16 *to;
        UINT16 *from;
        UINT16 dsize;

        dsize = size >> 1;

        to = (UINT16 *) vto;
        from = (UINT16 *) vfrom;

        while ( dsize-- )
                *to++ = *from++;

#if (!UTEXT_SIZE)
        if (size & 1)
                *((UCHAR *)to) = *((UCHAR *)from);
#endif
}
#endif


/*****************************************************************************
* Name: COMPBUFF  - Compare two buffers
*
* Description
*       Compare n characters of Source and Target string buffers.
*
* Entries:
*       SDVOID *vfrom     Source buffer
*       SDVOID *vto       Target buffer
*       INT16 size        Number of bytes to compare
*
* Returns
*       -1      Source < Target
*        0      Source = Target
*       +1      Source > Target
*
******************************************************************************/
#ifndef dev_compbuff                /* might be using memcmp */
INT16 dev_compbuff(SDVOID *vfrom, SDVOID *vto, INT16 size) /* __fn__*/
{
        UCHAR *str1 = (UCHAR *) vfrom;
        UCHAR *str2 = (UCHAR *) vto;

        while ( size-- )
        {
                if ( *str1 < *str2 )
                        return -1;
                if ( *str1 > *str2 )
                        return  1;
                str1++;
                str2++;
        }

        return 0;
}
#endif



/*****************************************************************************
* Name: PC_MEMFILL  - Fill a buffer with a character
*
* Description
*        Fill the target buffer with size instances of c
*
* Entries:
*       SDVOID *vto       Target buffer
*       INT16 size      Number of bytes to do
*       UTINY c         Value to be filled
*
* Returns:
*        None
*
******************************************************************************/
#ifndef dev_pc_memfill              /* might be using memfill */
SDVOID dev_pc_memfill(SDVOID *vto, INT16 size, UTINY c) /* __fn__*/
{
        UINT16 dsize;

#if (UTEXT_SIZE)
        UINT16 *to = (UINT16 *) vto;

        dsize = size >> 1;
#else
        UCHAR *to = (UCHAR *) vto;

        dsize = size;
#endif

        while ( dsize-- )
                *to++ = c;
}
#endif


UINT16 dev_swap_hi_low_byte (UINT16 inword)
{
        UINT16 tmpWord;

        tmpWord = inword;
        inword <<= 8;
        tmpWord = (tmpWord >> 8) | inword;

        return (tmpWord);
}


/*****************************************************************************
* Name: pc_strcat - strcat
*
* Description
*       Essential strcat function. Copy one buffer to another.  The source
*       buffer must be ended with a NULL.
*
* Entries:
*       TEXT *to        Target buffer
*       TEXT *from      Source buffer
*
* Returns
*        Nothing
*
******************************************************************************/
#ifndef dev_pc_strcat               /* might be using strcat */
SDVOID dev_pc_strcat(TEXT *to, TEXT *from) /* __fn__*/
{
        while ( *to )
                to++;

        while ( *from )
                *to++ = *from++;
        *to = 0;
}
#endif



/*****************************************************************************
PC_CNVRT -  Convert intel byte order to native byte order.


Summary
    #include "pcdisk.h"

    ULONG to_DWORD (from)  Convert intel style 32 bit to native 32 bit
        UTINY *from;

    UINT16 to_WORD (from)  Convert intel style 16 bit to native 16 bit
        UTINY *from;

    SDVOID fr_WORD (to,from) Convert native 16 bit to 16 bit intel
        UTINY *to;
        UCOUNT from;

    SDVOID fr_DWORD (to,from) Convert native 32 bit to 32 bit intel
        UTINY *to;
        ULONG from;

 Description
    This code is known to work on 68K and 808x machines. It has been left
    as generic as possible. You may wish to hardwire it for your CPU/Code
    generator to shave off a few bytes and microseconds, be careful though
    the addresses are not guaranteed to be word aligned in fact to_WORD AND
    fr_WORD's arguments are definately NOT word alligned when working on odd
    number indeces in 12 bit fats. (see pc_faxx and pc_pfaxx().

    Note: Optimize at your own peril, and after everything else is debugged.

    Bit shift operators are used to convert intel ordered storage
    to native. The host byte ordering should not matter.

Returns

Example:
    See other sources.

******************************************************************************/

#if 0
/******************************************************************************
* Name: to_DWORD - Convert to 32-bit portable data
*
* Description
*       Convert a 32 bit intel item to a portable 32 bit data.
*
* Entries:
*       UTINY *from     Source buffer data
*
* Returns:
*       32-bit data
*
******************************************************************************/
ULONG to_DWORD ( UCHAR *from ) /*__fn__*/
{
        ULONG res = 0;
#if (LITTLE_ENDIAN)
        //        res = (ULONG) (*((ULONG *)from));
        memcpy( &res, from, 4);
        
#else
        ULONG t;
        t = ((ULONG) *(from + 3)) & 0x00FF;
        res = (t << 24);
        t = ((ULONG) *(from + 2)) & 0x00FF;
        res |= (t << 16);
        t = ((ULONG) *(from + 1)) & 0x00FF;
        res |= (t << 8);
        t = ((ULONG) *from) & 0x00FF;
        res |= t;
#endif
        return (res);
}


/******************************************************************************
* Name: to_WORD - Covert to a portable 16-bit data
*
* Description
*       Convert a 16 bit intel item to a portable 16 bit.
*
* Entries:
*       UTINY *from     Source buffer data
*
* Returns:
*        16-bit data
*
******************************************************************************/
UINT16 to_WORD ( UCHAR *from ) /*__fn__*/
{
        UINT16 nres;

#if (LITTLE_ENDIAN)
        memcpy( &nres, from, 2);
        
        //        nres = ((UINT16) *((UINT16 *)from));
#else
        UINT16 t;
        t = (UINT16) (((UINT16) *(from + 1)) & 0x00FF);
        nres = (UINT16) (t << 8);
        t = (UINT16) (((UINT16) *from) & 0x00FF);
        nres |= t;
#endif
    return (nres);
}

#if (RTFS_WRITE)
/******************************************************************************
* Name:  fr_WORD  - Convert to 16-bit INTEL format
*
* Description
*        Convert a portable 16-bit to a 16-bit intel item.
*
* Entries:
*       UTINY *to       Target buffer      
*       UINT16 from     Source data
*
* Returns:
*        Nothing
*
******************************************************************************/
SDVOID fr_WORD ( UCHAR *to, UINT16 from ) /*__fn__*/
{
#if (LITTLE_ENDIAN)
  memcpy( to, &from, 2);
//        *((UINT16 *)to) = from;
#else
        UINT16 *tptr;
        UINT16 t1, t2;

        tptr = (UINT16 *)to;
        t1  =   (from << 8);
        t2  =  ((from >> 8) & 0x00FF);

        *tptr = (t1 | t2);
#endif
}

/******************************************************************************
* Name:  fr_DWORD  - Convert to 32-bit INTEL format
*
* Description
*        Convert a portable 32 bit to a 32 bit intel item.
*
* Entries:
*       UTINY *to       Target buffer
*       ULONG from      Source data
*
* Returns:
*        None
*
******************************************************************************/
SDVOID fr_DWORD ( UCHAR *to, ULONG from ) /*__fn__*/
{
#if (LITTLE_ENDIAN)
  memcpy( to, &from, 4);
//        *((ULONG *)to) = from;
#else
        ULONG *tptr;
        ULONG tt;

        tt = 0;
        tptr = (ULONG *)to;
        *tptr = 0;

        tt = (from & 0xFF);
        tt <<= 8;
        tt |= ((from >> 8) & 0x00FF);
        tt <<= 8;
        tt |= ((from >> 16) & 0x00FF);
        tt <<= 8;
        tt |= ((from >> 24) & 0x00FF);
        *tptr = tt;
#endif
}
#endif  /* (RTFS_WRITE) */
#endif

#if (CHAR_16BIT)

/******************************************************************************
* Name: b_unpack
*
* Description
*       Convert a byte data buffer to a portable word data buffer with
*       leading bytes as 0x00.
*
* Entries:
*       UINT16 *from    Source data buffer
*       UTINY  *to      Target data buffer 
*       UINT16 length   Length of string to convert
*       UINT16 offset   Byte offset of target data buffer 
*
* Returns:
*        16-bit target data buffer with leading upper bytes as 0x00.
*
******************************************************************************/
SDVOID b_unpack(UTINY *to, UINT16 *from, UINT16 length, UINT16 offset) /*__fn__*/
{
        UINT16 i, woffset = (offset >> 1);

        for (i = 0; i < length; i++, offset++)
	{
#if (LITTLE_ENDIAN)
                if ( offset & 1 )
		{
			to[i] = (UTINY)(from[woffset] >> 8);
			woffset++;
		}
		else
                        to[i] = (UTINY)(from[woffset] & 0x00FF);
#else
                if ( offset & 1 )
		{
                        to[i] = (UTINY)(from[woffset] & 0x00FF);
			woffset++;
		}
		else
			to[i] = (UTINY)(from[woffset] >> 8);
#endif
	}
}

#endif
