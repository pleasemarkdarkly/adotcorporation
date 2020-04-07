/*****************************************************************************
* FileName: MMCDRV.C
*
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1997 - 1999 SanDisk Corporation
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
* Description:
*       MMC driver
*
******************************************************************************/

#include <dadio/io/sdconfig.h>

#if (USE_MMC || USE_MMC_EMULATION)

#include <dadio/io/sdmmc.h>



/***********************************************************************************
*                                  Public Functions
*                                  ----------------
************************************************************************************/

/***********************************************************************************
* Name: mmcBusStart
*
* Description:
*       Starts the MMC bus by sending 80 clock cycles.
*       Reset the MMC devices. Put them in IDLE mode and configure all
*       devices with proper OCR and wait for ready.
*
* Input:
*       INT16   controller_no           Controller Number
*
* Output:
*       None.
*
* Returns:
*       None.
*
************************************************************************************/
SDBOOL mmcBusStart( INT16 ctrlNo )
{

	/* Reset the MMC host controller */
	resetMMCController(ctrlNo);

	/* IDENTIFICATION STATE process in OPEN-DRAIN mode. */
	/* This process is operated at low clock rate 250KHz */
	if ( !setMMCClockRate(5) )
		return NO;

	/* Enable MMC interface in open drain mode and turn on the clock. */
	startMMC80Clocks(ctrlNo);

	return YES;
}

/*****************************************************************************
* Name: mmc_init_setup
*
* Description:
*       Initialize MMC devices. The MMC controller is operated in
*       OPEN-DRAIN mode.
*               - 80 clocks
*               - IDLE STATE
*               - OPERATIING VOLTAGE CONDITION
*
* Input:
*       None
*
* Output:
*       All devices should be ready
*
* Returns:
*       YES if successful
*       NO if failure
*       
*****************************************************************************/
void MMCSelectController(INT16);

SDBOOL mmc_init_setup(INT16 controllerno) /*__fn__*/
{
	PDEVICE_CONTROLLER pc;

	pc = &controller_s[ controllerno ];
	MMCSelectController( controllerno );

	if( mmcBusStart( controllerno ) ) {
	  if( mmcReset( pc, 0xFFC000 ) == MMC_NO_ERROR ) {
	    setMMCClockRate( 0 );
	  } else {
	    return NO;
	  }
	} else {
	  return NO;
	}

	return (YES);
	
#if 0
	INT16   ctrlNo;
	INT16   nFailures;

	nFailures = 0;

	/* This is a starting point in the IDENTIFICATION STATE process. */
	/* Apply to all MMC controllers */

	for (ctrlNo = 0; ctrlNo < N_INTERFACES; ctrlNo++)
	{
		pc = &controller_s[ctrlNo];

		MMCSelectController( ctrlNo );
		
		/* Send 80 clocks to start the bus */
		if ( mmcBusStart(ctrlNo) )   /* Put all device in idle mode */
		{
			/* Reset all devices on the bus and wait for ready. */
			/*
			Make sure all devices are READY.
			- IDLE STATE
			- OPERATIING VOLTAGE CONDITION
			*/
			if ( mmcReset ( pc, 0xFFC000 ) == MMC_NO_ERROR )
			{
				/* Set max. clock rate */
				setMMCClockRate(0);
			}
			else
			{
				nFailures++;
			}
		}
	}

	if ( nFailures >= N_INTERFACES )
		return NO;
#endif
	return (YES);
}

/****************************************************************************
* Name: mmcConfigDevice
*
* Description:
*       Configure MMC devices and assign device ID.
*
* Input:
*       pc              Device Controller structure
*       driveno         Drive Number
*
* Output:
*       None.
*
* Returns:
*       Complete code.
*
****************************************************************************/
MMC_CC mmcConfigDevice( INT16 driveno )
{
	PDEVICE_CONTROLLER pc;
	INT16   phys_drive;

	/* THIS IS A INDENTIFICATION STATE phase */

#if (N_INTERFACES > 1)
	pc = dev_drno_to_controller(driveno);
	phys_drive = dev_drno_to_phys(driveno);
#else
	pc = &controller_s[0];
	phys_drive = driveno;
#endif

	/* Allow all card to identify itself and
	   Assign ID to the device.
	*/
	return (mmcIdentify( pc, phys_drive ));
}


/****************************************************************************
* Name: receive_data
*
* Description:
*       While monitoring the MMC bus for data start bit condition, waits for
*       certain time before issue CARD NOT RESPONSE condition.
*       The data is read and stored after the start bit is detected. MMC timeouts
*       are observed and the received CRC number is validated.
*
* Input:
*       buffDATA   -   pointer to the data buffer.
*       length     -   Expected length of data block.
*
* Output:
*       buffData[] -   Received data.
*
* Returns:
*       Completion code
*
****************************************************************************/
MMC_CC receive_data( PDEVICE_CONTROLLER pc, UINT16 dLength )
{
	MMC_CC ret_val;

	/* Data is ready.  Get it from the MMC BUFFER */
	ret_val = MMCReceive((UCHAR *)pc->user_address, dLength);
	if ( ret_val != MMC_NO_ERROR )
		return ret_val;

	/*
	Check for multiple block mode and start the clock again
	except for the last block
	*/
        if ( (pc->mode) && (pc->sectors_remaining >= 1) )
	{
		/* Start the clock for next block */
		startMMCClock();
	}

	return MMC_NO_ERROR;
}


/****************************************************************************
* Name: send_data
*
* Description:
*       Writes data block to the card.
*
* Input:
*       pc              Controller structure pointer.
*       dataLength      Length of data block.
*
* Output:
*       Data are sent to the device.
*
* Returns:
*       Completion code
*
****************************************************************************/
MMC_CC send_data( PDEVICE_CONTROLLER pc, UINT16 dLength )
{
	MMC_CC  errCode;
	UINT16  busyLoop;

	/* Write data to the device */
	errCode = MMCTransmit((UCHAR *)pc->user_address, dLength);
	if ( errCode == MMC_NO_ERROR )
	{
		/*
		It is important to check the device for READY before
		get the next block of data.  The device could be in a
		wrong state or the MMC Controller shows wrong status.
		*/
		busyLoop = 10;
		while ( busyLoop )
		{
			/* Make sure the device is ready for next block */
			errCode = IsCardBusy(pc);
			busyLoop--;
			if (errCode == MMC_NO_ERROR )
				return MMC_NO_ERROR;
		}

		/* SHOULD NOT BE HERE. */
		/* Too bad, the device is still busy (hang up, wrong state)....*/
	}

	return errCode;
}


/***************************************************************************
* Name: MMCReceivedResponse - Get the response from MMC card
*
* Description:
*       Get the response from MMC card
*
* Input:
*       pc      Device information structure
*       Resp    Response Type
*
* Output:
*
* Return:
*
***************************************************************************/
MMC_CC MMCReceivedResponse(PDEVICE_CONTROLLER pc, RESP_TYPE Resp)
{
	UINT08  *respBuff;
	UINT16  respLength;


	if ( Resp == R0 )
		return (MMC_NO_ERROR);

	respLength = BYTE_LENGTH;               /* 6 bytes R1 or R3 response */
	if ( Resp == R2 )
		respLength = CID_BYTE_LENGTH -1;/* (17-1) bytes R2 response */

	respBuff = (UINT08 *)pc->LastResponse;

	/* Read status and wait for response. */
        return (getMMCResponseInfo(respBuff, respLength, (UINT16)Resp));
}


/***************************************************************************
* Name: MMCAnalysisResponse - Analyze the MMC response
*
* Description:
*       Analyze the response from MMC card and convert the RAW
*       information to the known status
*
* Input:
*       pc      Device information structure
*
* Output:
*       None
*
* Return:
*       MMC completion code
*
***************************************************************************/
MMC_CC MMCAnalysisResponse( PDEVICE_CONTROLLER pc )
{
	UINT32  statusInfo;
	UINT08 *respBuff;

	respBuff = (UINT08 *)pc->LastResponse;

	/* Get the RAW MMC status */
	statusInfo = (UINT32)(respBuff[1] & 0xFF);
	statusInfo <<= 8;
	statusInfo |= (UINT32)(respBuff[2] & 0xFF);
	statusInfo <<= 8;
	statusInfo |= (UINT32)(respBuff[3] & 0xE0);

	/* Get the MMC state */
	pc->currentState = (UINT16)(respBuff[3] & 0x1F);
	pc->mmcStatus = statusInfo;
	pc->mmcRdyState = pc->currentState & 01;
	pc->currentState >>= 1;

	/* Check status for error. Convert RAW status to driver error code */
	if (statusInfo == 0)
		return MMC_NO_ERROR;

	if (statusInfo & OUT_OF_RANGE_ERROR)
		return  MMC_OUT_OF_RANGE;

	if (statusInfo & ADDRESS_ERROR)
		return  MMC_ADDRESS_ERROR;

	if (statusInfo & BLK_LENGTH_ERROR)
		return  MMC_DATA_LENGTH_ERROR;

	if (statusInfo & ERASE_SEQ_ERROR)
		return  MMC_ERASE_SEQ_ERROR;

	if (statusInfo & ERASE_PARAM)
		return  MMC_ERASE_PARAM;

	if (statusInfo & WP_VIOLATION)
		return  MMC_WP_VIOLATION;

	if (statusInfo & CMD_CRC_ERROR)
		return MMC_CMD_CRC_ERROR;

	if (statusInfo & COMUNC_ILLEG_COM)
		return  MMC_COMUNC_ILLEG_COM;

	if (statusInfo & CARD_ECC_FAILED)
		return  MMC_CARD_ECC_FAILED;

	if (statusInfo & CONTROLLER_ERROR)
		return  MMC_INTERFACE_ERROR;

	if (statusInfo & EERROR)
		return  MMC_ERROR;

	if (statusInfo & UNDERRUN)
		return  MMC_UNDERRUN;

	if (statusInfo & OVERRUN)
		return  MMC_OVERRUN;

	if (statusInfo & CIDCSD_OVERWRITE)
		return  MMC_CIDCSD_OVERWRITE;

	if (statusInfo & WP_ERASE_SKIP)
		return  MMC_WP_ERASE_SKIP;

	if (statusInfo & CARD_ECC_DISABLED)
		return  MMC_CARD_ECC_DISABLED;

	if (statusInfo & ERASE_RESET)
		return  MMC_ERASE_RESET;

	return MMC_CARD_IS_NOT_RESPONDING;
}


/***************************************************************************
* Name: MMCCommandAndResponse - Send the command to the MMC card
*
* Description:
*       Send the command to the MMC card
*
* Input:
*       pc              Device information structure
*       Cmd             MMC Command
*       Arg             MMC Argument
*       noBlocks        Number of blocks to transfer        
*       Resp            Response Type
*
* Output:
*       Command is sent and Response is analyzed
*
* Return:
*       MMC Completion Code
*
***************************************************************************/
MMC_CC mmcCommandAndResponse(PDEVICE_CONTROLLER pc, UINT16 Cmd, UINT32 Arg, UINT16 noBlocks, RESP_TYPE Resp )
{
	INT16   i;
	MMC_CC  resErr;
	/*printf("Cmd:%08lx Arg:%08lx noBlocks:%08lx\n",(UINT32)Cmd,(UINT32)Arg,(UINT32)noBlocks);*/
	/* Setup the command */
	if ( !MMCPrepareAndSetup(Arg, Cmd, noBlocks, (UINT16)Resp) )
		return MMC_INTERFACE_ERROR;

	if ( !Resp )                    /* Command without response (R0) */
		return MMC_NO_ERROR;

	/* TODO look at this fix further */
	/* refer to the comment in sdmmc.c regarding this (search for CID_BYTE_LENGTH) */
	for (i = 0; i <= (CID_BYTE_LENGTH>>1); i++)
		pc->LastResponse[i] = 0;/* Clear buffer */

	/* Check the response */
	resErr = MMCReceivedResponse( pc, Resp );

	if ( !resErr && ( Resp == R1 ) )
		resErr = MMCAnalysisResponse(pc);

	return resErr;
}


MMC_CC IsCardBusy(PDEVICE_CONTROLLER pc)
{
        return (checkCardBusy(pc->controller_number) );
}


/*******************************************************************************
* Name: is_device_changed
* 
* Description:
*       Check for device removal and insertion       
* 
* Input:
*       INT16   driveno         Drive number
*
* Output:
*       None
*
* Returns:
*       YES if the device is removed
*       NO otherwise
*
********************************************************************************/
SDBOOL is_device_changed (INT16 driveno) /*__fn__*/
{
	DRV_GEOMETRY_DESC  idDrv;

	if ( mmc_read_serial( driveno, &idDrv ) )
	{
		return (NO);
	}
	else
	{
		return (YES);
	}
}

#if (USE_MMC)

/***************************************************************************
* Name: setupMMcHostDataRegister - Configure MMC info. for MMC controller
*
* Description:
*       Set up the information to configure the MMC controller
*
* Input:
*       cmdIndex        Command index
*       cmdDatContReg   Control information of CMD_DAT_CON_REG
*       noBlocks        Number of blocks to transfer
*       blkLength       Block data length
*
* Output:
*       None
*       
* Return:
*       None
*         
***************************************************************************/
SDVOID setupMMcHostDataRegister(UINT16 cmdIndex, UINT16 *cmdDatContReg, UINT16 *noBlocks, UINT16 *blkLength)
{
        switch ( cmdIndex )
	{ 
                case    READ_BLOCK:             /* DATA TRANSFER */
                case    READ_MULTIPLE_BLOCK:    /* DATA TRANSFER */
                case    READ_DAT_UNTIL_STOP:    /* STREAM READ */
                        *cmdDatContReg = DATA_ENABLE;
                        *blkLength = DEFAULT_BLK_LENGTH;
                        break;

                case    WRITE_BLOCK:            /* DATA TRANSFER */
                case    WRITE_MULTIPLE_BLOCK:   /* DATA TRANSFER */
                case    WRITE_DAT_UNTIL_STOP:   /* STREAM WRITE */
                        *cmdDatContReg = (DATA_ENABLE + DATA_WRITE_SET);
                        *cmdDatContReg |= BUSY_SET;
                        *blkLength = DEFAULT_BLK_LENGTH;
                        break;

                case    SEND_WRITE_PROT:
                        *cmdDatContReg = DATA_ENABLE;
                        *blkLength = 0;
                        break;

		case    PROGRAM_CSD:
		case    PROGRAM_CID:
                        *cmdDatContReg = (DATA_ENABLE + DATA_WRITE_SET);
                        *blkLength = 16;
                        *noBlocks = 1;
                        break;

                case    ERASE_SECTORS:
                        *cmdDatContReg = NO;
                        *blkLength = 0;
                        break;

                case    STOP_TRANSMISSION:
                        *cmdDatContReg = (BUSY_SET | DATA_WRITE_SET);
                        *blkLength = 0;
                        break;

                /* NO DATA TRANSFER */
                case    GO_IDLE_STATE:
                case    SEND_OP_COND:
                case    ALL_SEND_CID:
                case    SEND_CID:
                case    SEND_CSD:
		case    SEND_STATUS:
                        *cmdDatContReg = NO;
                        *blkLength = 0;
                        *noBlocks = 0;
                        break;

        } /* END SWITCH */
}

#endif /* USE_MMC */

#endif  /* (USE_MMC || USE_MMC_EMULATION) */

