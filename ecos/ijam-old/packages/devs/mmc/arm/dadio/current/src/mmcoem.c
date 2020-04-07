/*****************************************************************************
* FileName: MMCOEM.C
*
* Description:
*
*       The happiness that makes my world go round.
*
******************************************************************************/

#include <dadio/io/sdapi.h>
#include <dadio/io/mmcoem.h>


#if (USE_MMC || USE_MMC_EMULATION)

#if (USE_MMC_EMULATION)

static UINT32 portIO;
static INT16 clkRate[2];

MMC_CC (*mmc_exchange_data)(UCHAR,UCHAR*);
UCHAR  (*MMCGetData)( SDVOID );
SDVOID (*MMCSendData)( UCHAR odata );
SDVOID (*MMCSendCommand)( UINT32 Arg, UINT16 Cmd, UINT16 crcData );
MMC_CC (*MMCReceive)( UCHAR* dBuf, UINT16 dataLength );
MMC_CC (*MMCTransmit)( UCHAR* dBuf, UINT16 dataLength );
MMC_CC (*mmc_get_response)( UCHAR* resp_bytes, UINT16 respBitLength );


SDVOID mmcPowerOnOff(INT16 controllerno, INT16 power_state) /*__fn__*/
{
    switch (power_state) {
	case 0:
	    /* turn off controller */
	    *(volatile cyg_uint8 *)PDDDR &= ~0xE7; /* set cmd and data,CLK to outputs */
	    *(volatile cyg_uint8 *)PDDR &= ~0xE7; /* set cmd and data ,CLK,low */
	    *(volatile cyg_uint8 *)PEDR |= 0x01;
	    break;

	case 1:
	    /* turn on controller */
	    *(volatile cyg_uint8 *)PDDDR &= ~0xE7; /* set cmd and data ,CLK,to outputs */
	    *(volatile cyg_uint8 *)PDDR |= 0xE7; /* set cmd and data CLK, high  for clean Restart*/
	    *(volatile cyg_uint8 *)PEDR &= ~0x01;
	    break;
    }
}


SDVOID mmc_hw_init(INT16 controllerno) /*__fn__*/
{
  MMCSelectController( controllerno );
  setMMCClockRate( 5 );
  
  mmcPowerOnOff(controllerno, 0);         /* Power off the reader */
  OS_WAIT(10);			/* 10ms doesn't seem to be long enough, but
				 * no info in spec regarding this time */
  mmcPowerOnOff(controllerno, 1);         /* Power on the reader */
  OS_WAIT(5);
}



SDVOID resetMMCController(UINT16 ctrlno)
{
    /* Make sure the power is set */
  mmcPowerOnOff(ctrlno, 1);         /* Power on the reader */
  OS_WAIT(3);
}


/**************************************************************************
* Name: startMMCClock - Start the MMC Host Controller clock
*
* Description:
*       The MMC clock is started by the.       
*
* Input:
*       portAddress     Controller bas address
*
* Output:
*       Starting the clock.
*
* Return:
*       None
*
**************************************************************************/

SDVOID startMMCClock( SDVOID )
{
}

/* determines whether MMCExchangeData or the asm optimized routines are used */
SDBOOL setMMCClockRate(UINT16 clkrate)
{
  clkRate[ portIO ] = clkrate;

  if( clkrate ) {
    if( portIO ) {
      mmc_get_response   = mmc_get_response_slow_c1;
      mmc_exchange_data  = MMCExchangeDataSlow_c1;
      MMCSendCommand     = MMCSendCommandSlow_c1;
    } else {
      mmc_get_response   = mmc_get_response_slow_c0;
      mmc_exchange_data  = MMCExchangeDataSlow_c0;
      MMCSendCommand     = MMCSendCommandSlow_c0;
    }
  } else {
    if( portIO ) {
      mmc_get_response   = mmc_get_response_c1;
      mmc_exchange_data  = MMCExchangeData_c1;
      MMCSendCommand     = MMCSendCommand_c1;
    } else {
      mmc_get_response   = mmc_get_response_c0;
      mmc_exchange_data  = MMCExchangeData_c0;
      MMCSendCommand     = MMCSendCommand_c0;
    }
  }
  return YES;
}


SDVOID startMMC80Clocks(INT16 controller_no)
{
  INT16   i;
  UCHAR   iData;

  MMCSelectController( controller_no );

    /* Send 80 clocks */
  for (i = 0; i < 80; i++)
  {
    mmc_exchange_data(0xff, &iData);
  }
}

SDVOID MMCSelectController(INT16 ctrlNo)
{
    // only set function ptrs if we really need to
  if( portIO != ctrlNo || mmc_exchange_data == 0 ) {
    
    *(( volatile unsigned char*) PDDDR) = 0xff;
  
    portIO = ctrlNo;

    if( portIO ) {
      DDR_CLK2_OUTPUT();
      DDR_CMD2_OUTPUT();
      DDR_DATA2_INPUT();

      if( clkRate[ 1 ] ) {
	mmc_exchange_data = MMCExchangeDataSlow_c1;
        mmc_get_response  = mmc_get_response_slow_c1;
        MMCSendCommand    = MMCSendCommandSlow_c1;
      } else {
	mmc_exchange_data = MMCExchangeData_c1;
        mmc_get_response  = mmc_get_response_c1;
        MMCSendCommand    = MMCSendCommand_c1;
      }
      
      MMCGetData        = MMCGetData_c1;
      MMCSendData       = MMCSendData_c1;
      MMCReceive        = MMCReceive_c1;
      MMCTransmit       = MMCTransmit_c1;
    } else {
      DDR_CLK1_OUTPUT();
      DDR_CMD1_OUTPUT();
      DDR_DATA1_INPUT();

      if( clkRate[ 0 ] ) {
	mmc_exchange_data = MMCExchangeDataSlow_c0;
        mmc_get_response  = mmc_get_response_slow_c0;
        MMCSendCommand    = MMCSendCommandSlow_c0;
      } else {
	mmc_exchange_data = MMCExchangeData_c0;
        mmc_get_response  = mmc_get_response_c0;
        MMCSendCommand    = MMCSendCommand_c0;
      }
      
      MMCGetData        = MMCGetData_c0;
      MMCSendData       = MMCSendData_c0;
      MMCReceive        = MMCReceive_c0;
      MMCTransmit       = MMCTransmit_c0;
    }
  }
}

SDBOOL MMCPrepareAndSetup(UINT32 Arg, UINT16 Cmd, UINT16 noBlocks, UINT16 Resp)
{
  UINT16 crcDATA;

  crcDATA = calculateCmdCRC(Arg, Cmd);
	
    /* Setup the command */
  MMCSendCommand( Arg, Cmd, crcDATA );

  return YES;
}

/***************************************************************************
* Name: getMMCResponseInfo - Get the response from MMC card
*
* Description:
*       Get the response from MMC card
*
* Input:
*       RespBuff        Response buffer
*       RespLength      Length of the response
*
* Output:
*       Response information
*
* Return:
*
***************************************************************************/
MMC_CC getMMCResponseInfo(UCHAR *pRespBuff, UINT16 respLength, UINT16 respType)
{
    /* The response length can be 6 or 16 bytes long */
  if (respLength > BYTE_LENGTH)
  {
    if (mmc_get_response( pRespBuff, R2_BIT_LENGTH ))
      return MMC_CARD_IS_NOT_RESPONDING;      /* The card isn't responding */
			
    if (checkResponseCRC( pRespBuff, 1, R2_BYTE_LENGTH))
      return MMC_CMD_CRC_ERROR;       /* Detected CRC-error */
  }        
  else
  {
    if (mmc_get_response( pRespBuff, R1R3_BIT_LENGTH ))
      return MMC_CARD_IS_NOT_RESPONDING;      /* The card isn't responding */

    if (respType == 3)
      return (MMC_NO_ERROR);

    if (checkResponseCRC(pRespBuff, 0, R1R3_BYTE_LENGTH))
      return MMC_CMD_CRC_ERROR;       /* Detected CRC-error */
  }

  return MMC_NO_ERROR;
}



/***********************************************************************************
* Name: checkCardBusy
*
* Description:
*       Checks the card programming status
*
* Input:
*       INT16   driveno         Drive Number
*
* Output:
*       None.
*
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC checkCardBusy( INT16 controller_no )
{
  INT16   i;
  UCHAR   iByte, tmp;

  if( controller_no != portIO ) {
    MMCSelectController( controller_no );
  }

  i = 0xfff;
  
  if( portIO ) {
    while( i-- ) {
      MMC_READ_PORT_C1( iByte, tmp );
      if( iByte & PD_MMC_DATA2 ) {
	return MMC_NO_ERROR;
      }
    }
  } else {
    while( i-- ) {
      MMC_READ_PORT_C0( iByte, tmp );
      if( iByte & PD_MMC_DATA1 ) {
	return MMC_NO_ERROR;
      }
    }
  }
  
  return MMC_CARD_IS_BUSY;
}

int mmc_card_present( INT16 driveno ) 
{
  char port = *(( volatile unsigned char*) PDDR);

  if( ( driveno && !( port & PD_MMC_CARD2)) ||
      (!driveno && !( port & PD_MMC_CARD1)))
    return YES;
  else
    return NO;
}

#endif /* (USE_MMC) */

#endif  /* (USE_MMC || USE_MMC_EMULATION) */




