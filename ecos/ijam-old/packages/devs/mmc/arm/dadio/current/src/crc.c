/*******************************************************************************
* Filename:     crc.c
*
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1997-1999 SanDisk Corporation
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
* Description:
*       This module contains CRC generation/validation functions. The
*       7 and 16 bit CRC polynimials used by the MMC protocol are
*       supported. These functions are used by the low level hardware
*       drivers (if there is no hardware support for CRC).
*
*********************************************************************************/

#include <dadio/io/sdconfig.h>

#if (USE_SPI || USE_MMC || USE_SPI_EMULATION || USE_MMC_EMULATION )

#include <dadio/io/sdapi.h>



/* CRC calculation parameters */
#define         CRC_LEN         0x07
#define         CRC_POLYN       0x89
#define         CRC_HIGHBIT     0x80
#define         CRC_BITS        0x7F

#define         D_CRC_LEN       0x10
#define         D_CRC_POLYN     0x11021L
#define         D_CRC_HIGHBIT   0x10000L
#define         D_CRC_BITS      0x0FFFFL
#define         WRONG_DATA_CRC16        0x08
#define         WRONG_CMD_RESPONSE      0x04


#define CMD_BYTE_LENGTH         6       /* Length in bytes of MMC commands  */



#if (!USE_SET_FEATURES && (USE_SPI || USE_SPI_EMULATION))

#else

UINT16 calculateCRC7(UCHAR *dBuff, UINT16 offset, UINT16 dLength, SDBOOL cmdORresp);

/*******************************************************************************
* Name: calculateDataCRC16
*
* Description:
*       Calculates the 16 bit crc number appended to each of the
*       data block. This function is a SW implementation of the
*               X^16 + X^12 + X^5 + 1 CRC polynomial.
*
* Input:
*       data            - pointer to the data buffer
*       data_length     - Length, in bytes, of the data buffer
*
* Output:
*       None.
*
* Returns:
*       16 bit crc number
*
********************************************************************************/
UINT16 calculateDataCRC16 ( UCHAR *pData, UINT16 data_length )
{
	UCHAR   *ppData;
	UINT32  rreg, rtemp;
	UINT16  ibyte, ibit;

	rreg = 0L;
	ppData = (UCHAR *)pData;

	for (ibyte = 0; ibyte < data_length; ibyte++)
	{
		rtemp = ((UINT32)ppData[ibyte]) << (D_CRC_LEN - 7);
		for (ibit = 0; ibit < 8; ibit++)
		{
			rreg <<= 1;
			rreg ^= ( ((rtemp ^ rreg) & D_CRC_HIGHBIT) ? D_CRC_POLYN : 0);
			rtemp <<= 1;
		}
	}

	return ((UINT16)(rreg & D_CRC_BITS));
}


UINT16 calculateCRC7(UCHAR *dBuff, UINT16 offset, UINT16 dLength, SDBOOL cmdORresp)
{
	UINT16  ibyte, ibit;
	UINT16  rreg, crc_byte, dtmp;

	if ( cmdORresp )
		crc_byte = CRC_POLYN;
	else
		crc_byte = 0x09;

	rreg = 0;
	for (ibyte = offset; ibyte < (dLength - 1); ibyte++)
	{
		dtmp = (UINT16)(dBuff[ibyte]); 
		for (ibit = 0; ibit < 8; ibit++)
		{
			rreg <<= 1;
			dtmp &= 0xFF;
			rreg ^= (((dtmp ^ rreg) & CRC_HIGHBIT) ? crc_byte : 0);
			dtmp <<= 1;
		}
	}

	return (rreg&0xFF);
}

/*******************************************************************************
* Name: checkResponseCRC
*
* Description:
*       Applies the 7 bit CRC calculation the an SPI response
*       buffer that includes the CRC number in its last byte.
*       This function handles SPI responses of formats R1 and R2.
*       If The provided CRC number is valid the new one
*       must be zero. This function is a SW implementation of the
*               X^7 + X^3 + 1 CRC polynomial.
*
* Input:
*       Response        Pointer to the response buffer
*       offset          A byte ofset into the buffer. The input to the
*                         CRC calculation will start from this bytes.
*       response_length Length of the response buffer
*
* Output:
*       None.
*
* Returns:
*       NO_ERROR        If the CRC number is valid
*       WRONG_CMD_RESPONSE      Otherwise.
*
********************************************************************************/
UINT16 checkResponseCRC ( UCHAR *response, UINT16 offset, UINT16 response_length )
{
	UINT16  crc_byte;

	crc_byte = calculateCRC7(response, offset, response_length, YES);
	crc_byte = (((crc_byte & CRC_BITS) << 1) | 0x01);

	if ( crc_byte != response[(response_length - 1)])
		return WRONG_CMD_RESPONSE;

	return (MMC_NO_ERROR);
}


/*******************************************************************************
* Name: appendCmdCRC
*
* Description:
*       Calculates the 7 bit crc number appended to each MMC command frame.
*       The CRC number is inserted in the last byte of the command structure.
*       This function is a software implementation of the
*               X^7 + X^3 + 1 CRC polynomial.
*
* Input:
*       cmd_bytes       Pointer to the 6 bytes command buffer
*
* Output:
*       cmd_bytes[]     7 bit CRC number in the last byte in the buffer
*
* Returns:
*       None.
*
********************************************************************************/
UINT16 appendCmdCRC ( UCHAR *cmd_bytes, UINT16 Length )
{
	UINT16   rreg;

	rreg = calculateCRC7(cmd_bytes, 0, Length, NO);
	return (((rreg << 1) | 0x01));

}
#endif  /* (!USE_SET_FEATURES && (USE_SPI && !USE_SET_FEATURES)) */



/*******************************************************************************
* Name: calculateCmdCRC
*
* Description:
*       Formats the command frame. The command index + the start
*       and the host bits are inserted into the first byte.
*       The command argument and the CRC number inserted into
*       the other 5 bytes.
*
* Input:
*       cmd_index       MMC command code.
*       arg             commman argument.
*
* Output:
*       None
*
* Returns:
*       CRC7
*
********************************************************************************/
UINT16 calculateCmdCRC ( UINT32 args, UINT16 cmd_index )
{
	UINT16  i;
	UCHAR   cmd_bytes[CMD_BYTE_LENGTH];


	cmd_bytes[0] = (UCHAR)(cmd_index);

	/* MSB first */
	for (i = 4; i > 0; i--)
	{
		cmd_bytes[i] = (UCHAR)(args & 0xFFL);
		args >>= 8;
	}

#if (!USE_SET_FEATURES && (USE_SPI || USE_SPI_EMULATION))
        if ( cmd_index == 0x40 )
		i = 0x95;
	else
		i = 0xFF;
#else
	i = appendCmdCRC(cmd_bytes, CMD_BYTE_LENGTH);
#endif

	return i;
}

#endif /* (USE_SPI || USE_MMC || USE_SPI_EMULATION || USE_MMC_EMULATION ) */


