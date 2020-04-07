/*****************************************************************************
* FileName: MMCOEM_c1.C
*
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1997 - 1999 SanDisk Corporation
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
* Description:
*
*       MMC platform specific routines.
*
******************************************************************************/

#include <dadio/io/sdapi.h>
#include <dadio/io/mmcoem.h>

#if (USE_MMC || USE_MMC_EMULATION)
#if (USE_MMC_EMULATION)

MMC_CC MMCExchangeData_c1(UCHAR odata, UCHAR *idata)
{
  odata &= 0xc0; // PD_MMC_CMD2 | PD_MMC_DATA2
  *(( volatile unsigned char*) PDDR ) = odata;
  *(( volatile unsigned char*) PDDR ) = (odata | PD_MMC_CLK2);
  *idata = *(( volatile unsigned char*) PDDR );
  *(( volatile unsigned char*) PDDR ) = odata;
  
  return (MMC_NO_ERROR);
}


#ifndef USE_ASM
/********************************************************************
* Read or Write a byte of data to the MMC device
*
********************************************************************/
UCHAR MMCGetData_c1( SDVOID )
{
  INT16   nbit;
  UCHAR   iByte, iData, tmp;

  iByte = 0;
  iData = 0;

  for( nbit = 0x80; nbit; nbit >>= 1 ) {
    MMC_READ_PORT_C1( iByte, tmp );
    iData |= (UCHAR) ( iByte & PD_MMC_DATA2 ? nbit : 0 );
    iByte = 0;
  }
  
  return (iData);
}

SDVOID MMCSendData_c1( UCHAR odata )
{
  INT16   nBit;
  UCHAR   oByte = *(( volatile unsigned char*) PDDR );

  for( nBit = 0x80; nBit ; nBit >>= 1 ) {
    oByte &= ~PD_MMC_DATA2;
    oByte |= ( odata & nBit ? PD_MMC_DATA2 : 0x00 );
    MMC_WRITE_PORT_C1( oByte );
  }
}

SDVOID MMCSendCommand_c1( UINT32 Arg, UINT16 Cmd, UINT16 crcDATA )
{
  static  UINT08  cmd_bytes[BYTE_LENGTH]; // static so it wont be recreated every time
  UINT16  i;
  UCHAR   nByte;
  UCHAR   tranByte;
  INT16   nBit;

  
  for (i = 0; i < 2; i++)
    MMC_WRITE_PORT_C1( 0x0e );

  cmd_bytes[0] = (UCHAR)(Cmd);

    /* MSB first */
  for (i = 4; i > 0; i--)
  {
    cmd_bytes[i] = (UCHAR)(Arg & 0xFFL);
    Arg >>= 8;
  }

  cmd_bytes[5] = (UINT08)crcDATA;

  for( i = 0; i < BYTE_LENGTH; i++ )
  {
    tranByte = cmd_bytes[i];

    for( nBit = 0x80; nBit ; nBit >>= 1 )
    {
      nByte = (UCHAR)( 0xa0 | ( tranByte & nBit ? PD_MMC_CMD2 : 0x00 ));
      MMC_WRITE_PORT_C1( nByte );
    }
  }
}


/***********************************************************************
* Name: MMCReceive - Handle data from target to host
*
* Description:
*       Read data from Receive FIFO
*
* Input:
*       portAddress     I/O address
*
* Output:
*
*
* Return:
*
***********************************************************************/
MMC_CC MMCReceive_c1(UCHAR *dBuf, UINT16 dataLength)
{
  USERADDRESS dPtr;
  UINT16  i, tmp;
  UCHAR   iByte=0;
  UCHAR   bitmask;

  for( i = 0xFFFF; i ; i-- ) {
    MMC_READ_PORT_C1( iByte, tmp );  // rCRC is a temp var here

    iByte &= PD_MMC_DATA2;
    if ( !(iByte) )
    {       /* Start Bit detected */
      break;
    }
  } /* END WHILE */


  if ( !i ) {
    return MMC_CARD_IS_NOT_RESPONDING;
  }

    /* Get data */
  dPtr = (USERADDRESS)dBuf;

    // register usage:
    // r4   dataLength
    // r5   dPtr
    // r6   port D
    // r7   original port value
    // r8   clock high port value, or value read back

  for(i = 0; i < dataLength; i++) {
    
    for( bitmask = 0x80; bitmask; bitmask >>= 1 ) {
      iByte = 0;
      MMC_READ_PORT_C1( iByte, tmp );
      
      dPtr[i] <<= 1;  // doing the shift in asm doesn't seem to work
      
      asm volatile (
	"and   r4, %1, #0x80;"
	"mov   r4, r4, lsr #7;"
	"orr   %0, %0, r4"
	: "=r"(dPtr[i])
	: "r"(iByte)
	: "r4"
	);
    }
  }
    /* Get CRC */
  i = ( (UINT16)MMCGetData_c1() << 8 ) | (UINT16)MMCGetData_c1();
#if 1
  if( i != calculateDataCRC16( dBuf, dataLength ) ) {
    return MMC_DATA_CRC_ERROR;
  }
#endif
  return MMC_NO_ERROR;

}


/***************************************************************************
* Name: MMCTransmit - Handle data from host to device
*
* Description:
*       Write data to transmit FIFO
*
* Input:
*
* Output:
*
* Return:
*
*
***************************************************************************/
MMC_CC MMCTransmit_c1(UCHAR *dBuf, UINT16 dataLength)
{
  USERADDRESS dPtr;
  UINT16  i;
  UINT16  wCRC;
  UCHAR   oByte;
  UCHAR   CRCstatus;


  DDR_CMD2_INPUT( );
        
    /* 2 clk - delay before transmitting DATA */
  for (i = 0; i < 2; i++)       /* MMC_Nwr_MIN == 2 */
    MMC_WRITE_PORT_C1( 0xc0 );

  DDR_DATA2_OUTPUT( );

    /* Calculate CRC */
  wCRC = calculateDataCRC16( dBuf, dataLength );

    /* Start Bit of DATA */
    //  MMCExchangeData(0x0D, &oByte);
  MMCExchangeData_c1( 0x40, &oByte );
//  MMC_WRITE_PORT_C0( 0x0a );

  dPtr = (USERADDRESS)dBuf;

    // TODO: fix this
  for (i = 0; i < dataLength; i++)
  {
    MMCSendData_c0( dPtr[i] );
  }
  
    /* Send CRC */
  oByte = (UCHAR)(wCRC >> 8);
  MMCSendData_c1(oByte);
  oByte = (UCHAR)(wCRC);
  MMCSendData_c1(oByte);

  DDR_DATA2_INPUT( );
    /* End bit + 2 - Z_bit */
    //	for (i = 0; i < 4; i++)
  for(i = 0; i < 3; i++)
    MMC_WRITE_PORT_C1( 0xc0 );
  

  oByte = 0;
  MMC_READ_PORT_C1( oByte, wCRC );    // reuse wCRC

  if (oByte & PD_MMC_DATA2)   {            /* if NO Start Bit */
    DDR_CMD2_OUTPUT( );
    return MMC_NO_CRC_STATUS;
  } else            /* if Start Bit of CRC_status detected */
  {
      
    for (i = 0, CRCstatus = 0; i < 4 ; i++)
    {

      CRCstatus <<= 1;
      MMC_READ_PORT_C1( oByte, wCRC );    // reuse wCRC
      oByte &= PD_MMC_DATA2;
      CRCstatus |= ( oByte >> 7 );
    }

    CRCstatus &= 0x0F;

    DDR_CMD2_OUTPUT( );
                
    if (CRCstatus == 0x5)
      return MMC_NO_ERROR;
	
    return MMC_DATA_STATUS_CRC_ERROR;
  }

  DDR_CMD2_OUTPUT( );

    /* Time out before finishing data.  Sheeee... it happens */
    /* don't listen to them, this code never executes DC */
  return MMC_TIME_OUT_RCVD;       /* not received data */
}

MMC_CC mmc_get_response_c1( UCHAR *resp_bytes, UINT16 respBitLength)
{
  UINT16 i,x;
  UCHAR iByte, tmp, bitmask;

  iByte = 0;

  DDR_CMD2_INPUT( );

  for( i = 0xFFFF; i ; i-- ) {
    MMC_READ_PORT_C1( iByte, tmp );

    iByte &= PD_MMC_CMD2;
    if ( !iByte  )  /* start bit of cmd's response detected */
      break;
    if( i == 1 ) {
      DDR_CMD2_OUTPUT( );
      return MMC_CARD_IS_NOT_RESPONDING;
    }
  }

  bitmask = 0x80;
  x = 0;

  for (i = 0; i < respBitLength; i++)
  {   /* Receiving & storing CMD_response */
    iByte &= PD_MMC_CMD2;
    resp_bytes[x] |= ( iByte ? bitmask : 0 );
    bitmask >>= 1;
    if( !bitmask ) {
      bitmask = 0x80;
      x++;
    }
    MMC_READ_PORT_C1( iByte, tmp );
  }
  DDR_CMD2_OUTPUT( );

  return MMC_NO_ERROR;
}

#endif   // USE_ASM

/* slow routines */

static int delay;
#define WAIT_1US()  for( delay = 0x03; delay; delay-- )


MMC_CC MMCExchangeDataSlow_c1(UCHAR odata, UCHAR *idata)
{
  odata &= 0xc0;
  
  *(( volatile unsigned char*) PDDR ) = odata;
  WAIT_1US();
  *(( volatile unsigned char*) PDDR ) = (odata | PD_MMC_CLK2);
  WAIT_1US();
  *idata = *(( volatile unsigned char*) PDDR );
  *(( volatile unsigned char*) PDDR ) = odata;
  
  return (MMC_NO_ERROR);
}

SDVOID MMCSendCommandSlow_c1( UINT32 Arg, UINT16 Cmd, UINT16 crcDATA )
{
  static  UINT08  cmd_bytes[BYTE_LENGTH]; // static so it wont be recreated every time
  UINT16  i;
  UCHAR   nByte, tmp;
  UCHAR   tranByte;
  INT16   nBit;

  
  for (i = 0; i < 2; i++)
    MMCExchangeDataSlow_c1( 0xf0, &nByte );
//    MMC_WRITE_PORT_C0( 0x0f );

  cmd_bytes[0] = (UCHAR)(Cmd);

    /* MSB first */
  for (i = 4; i > 0; i--)
  {
    cmd_bytes[i] = (UCHAR)(Arg & 0xFFL);
    Arg >>= 8;
  }

  cmd_bytes[5] = (UINT08)crcDATA;

  for( i = 0; i < BYTE_LENGTH; i++ )
  {
    tranByte = cmd_bytes[i];

    for( nBit = 0x80; nBit ; nBit >>= 1 )
    {
      nByte = (UCHAR)( 0xa0 | ( tranByte & nBit ? PD_MMC_CMD2 : 0x00 ));
      MMCExchangeDataSlow_c1( nByte, &tmp );
//      MMC_WRITE_PORT_C0( nByte );
    }
  }
}

MMC_CC mmc_get_response_slow_c1( UCHAR *resp_bytes, UINT16 respBitLength)
{
  UINT16 i,x;
  UCHAR iByte, tmp, bitmask;

  iByte = 0;

  DDR_CMD2_INPUT( );

  for( i = 0xFFFF; i ; i-- ) {
    MMCExchangeDataSlow_c1( 0xf0, &iByte );
//    MMC_READ_PORT_C0( iByte, tmp );

    if ( !(iByte & PD_MMC_CMD2) )  /* start bit of cmd's response detected */
      break;
    if( i == 1 ) {
      DDR_CMD2_OUTPUT( );
      return MMC_CARD_IS_NOT_RESPONDING;
    }
  }

  bitmask = 0x80;
  x = 0;

  for (i = 0; i < respBitLength; i++)
  {   /* Receiving & storing CMD_response */
    resp_bytes[x] |= ( iByte & PD_MMC_CMD2 ? bitmask : 0 );
    bitmask >>= 1;
    if( !bitmask ) {
      bitmask = 0x80;
      x++;
    }
    MMCExchangeDataSlow_c1( 0xf0, &iByte );
//    MMC_READ_PORT_C0( iByte, tmp );
  }
  DDR_CMD2_OUTPUT( );

  return MMC_NO_ERROR;
}


#endif /* (USE_MMC) */
#endif  /* (USE_MMC || USE_MMC_EMULATION) */




