/********************************************************************************
* Filename:    sdmmc.c
*
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1997 - 1999 SanDisk Corporation
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
* Description:
*       This module can be used to interface to the MMC card in MMC or
*       SPI modes. It supports a multiple card environment. The reset and
*       identification processes assume multiple cards on the bus.
*       In MMC mode, the card number is used as the RCA operand in the
*       comands.
*       In SPI mode, the card number passed to the low level hardware drivers
*       where the appropriate CS signal should be asserted.
*
********************************************************************************/

#include <dadio/io/sdconfig.h>

#if (USE_SPI || USE_MMC || USE_SPI_EMULATION || USE_MMC_EMULATION)

#include <dadio/io/sdmmc.h>


/*******************************************************************************
* Name: mmcReset
*
* Description:
*       Resets the cards and the MMC or SPI HW port.
*       It must be called after power on before any other commands.
*
*       This function sends reset command (CMD0) to the card and waits for the
*       card to complete the initialization process by polling its status using
*       send_op_condition command (CMD1) to complete its reset process.
*
*       In MMC mode, this function can be used to query a card stack for the
*       common voltage range (by sending an all ZEROs OCR) or to set a voltage
*       range and initialize the cards to be ready.
*
*       In SPI mode, the reset is card specific. The card number is used for the
*       device to be intialized for the card to be ready.
*
* Input:
*       setupInfo       In MMC mode, Host operating voltage range.
*                       In SPI mode, Card Address.
*
* Output:
*       None
*
* Returns:
*       Completion code
* 
********************************************************************************/
MMC_CC mmcReset( PDEVICE_CONTROLLER pc, ULONG setupInfo )
{
	MMC_CC  resErr;

	UINT16  i;

	pc->timer = RESET_DELAY << 2;

	/* Reset all devices on the bus */
	mmcCommandAndResponse ( pc,
			GO_IDLE_STATE, 
			0L,
			0,
			R0 );

	/* TODO look at this fix more closely */
	/* LastResponse is of type UINT16[8], however CID_BYTE_LENGTH is 17 bytes so this
	 * means that the last byte of the CID will not be in a known state.  in fact, it
	 * is the member mmcStatus in the pc structure.  mmcStatus is only referenced in
	 * MMCAnalysisResponse, and then only assigned to, so it doesn't appear to be used
	 * anywhere meaning the fix below is safe. */
	for (i = 0; i <= (CID_BYTE_LENGTH/2); pc->LastResponse[i++] = 0); /* Clear buffer */

	OS_WAIT(10);

	/* Wait for all devices to be ready */
	while (pc->timer)
	{
		resErr = mmcCommandAndResponse ( pc,
					SEND_OP_COND, 
					setupInfo,
					0,
					R3 );

		     /* Time out error on card response */
		if (resErr == MMC_CARD_IS_NOT_RESPONDING)
		{
			pc->error_code = MMC_INTERFACE_ERROR;
			return MMC_INTERFACE_ERROR;
		}

		if ( resErr == MMC_NO_ERROR )      /* If no error */
		{
			/* Make sure all devices are ready */
			if ( (pc->LastResponse[0] & 0x8000) )
			{
				break;
			}

			pc->timer--;
			OS_WAIT(1);    /* Delay it for awhile */
			if ( !pc->timer )
			{
				pc->error_code = MMC_CARD_IS_NOT_READY;
				return MMC_CARD_IS_NOT_READY;
			}
		}
	}

	/* Assign IDs to all devices found */
	for (i = 0;; i++)
	{
		/* Get device information and assign an ID to it */
		if ( mmcIdentify (pc, i) != MMC_NO_ERROR )
		{
			/* There isn't any more device found */
			break;        
		}
	}

	/* Make sure there is at least one device on the bus */
	if ( i == 0 )
		return MMC_INTERFACE_ERROR;

	pc->error_code = MMC_NO_ERROR;
	return MMC_NO_ERROR;
}


#if (USE_MMC || USE_MMC_EMULATION)

/***********************************************************************************
* Name: mmcIdentify
*
* Description:
*       Available in MMC mode only. Identifiies and assigns an RCA
*       for a selected MMC card on the bus.
*
*       This function starts card identification cycle and (if a
*       valid response is received) assigns the RCA of the identified
*       card. The CID of the identified card is returned to the
*       application.                                                                     
*
* Input:
*       RCA     Card number for the identified card.
* 
* Output:
*       None.
*
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC mmcIdentify (PDEVICE_CONTROLLER pc, UINT16 RCA)
{
	MMC_CC  resErr;
	UINT16 i;

	i = 0;
	resErr = mmcCommandAndResponse ( pc,
				ALL_SEND_CID, 
				0L,
				0,
				R2 );
	if ( resErr != MMC_NO_ERROR )
	{
		pc->error_code = (UINT16)resErr;
		return resErr;
	}


	/* Assgin an ID to the device.  The assigned RCA is starting
	   by adding an 1 to the given RCA to form the real ID.
	*/
	resErr = mmcCommandAndResponse ( pc,
				SET_RELATIVE_ADDR, 
				((ULONG)(RCA + 1) << 16),
				0,
				R1 );

	pc->error_code = (UINT16)resErr;
	return resErr;
}

/*******************************************************************************
* Name: mmcSetStandbyState
*
* Description:
*       In MMC mode A deselect card command is sent to the card.
*
* Input:
*       RCA     The address of the card to be selected.
*
* Output:
*       None.
*
* Returns:
*       Completion code.
*
********************************************************************************/
MMC_CC mmcSetStandbyState ( PDEVICE_CONTROLLER pc, UINT16 dRCA)
{
	MMC_CC  resErr;

	dRCA = dRCA;

	resErr = mmcCommandAndResponse ( pc,
				SEND_STATUS,
				((ULONG)dRCA << 16),
				0,
				R1 );
	if (resErr == MMC_NO_ERROR )
	{
		if (pc->currentState != STANDBY)
			resErr = mmcCommandAndResponse ( pc,
				 SELECT_DESELECT_CARD,
				 0L,
				 0,
				 R1 );
	}

	pc->error_code = (UINT16)resErr;
	return (resErr);
}

/*******************************************************************************
* Name: mmcSetXferState
*
* Description:
*       In MMC mode, a select card command is sent to the card.
*
* Input:
*       RCA             The address of the card to be selected.
*
* Output:
*       None.
*
* Returns:
*       Completion code.
*
********************************************************************************/
MMC_CC mmcSetXferState ( PDEVICE_CONTROLLER pc, UINT16 dRCA)
{
	MMC_CC  resErr;

	resErr = mmcCommandAndResponse ( pc,
				SEND_STATUS,
				((ULONG)dRCA << 16),
				0,
				R1 );
	if (resErr == MMC_NO_ERROR )
	{
		if (pc->currentState == STANDBY)
			resErr = mmcCommandAndResponse ( pc,
					SELECT_DESELECT_CARD, 
					((ULONG)(dRCA) << 16),
					0,
					R1 );
	}

	pc->error_code = (UINT16)resErr;
	return resErr;
}
#endif  /* (USE_MMC || USE_MMC_EMULATION) */

/*******************************************************************************
* Name: mmcGetCardIdentification
*       
* Description:
*       Get the card Identification (reads the card CID reg.)
*
* Input:
*       mmcIdent        Pointer to CID buffer
*       RCA             Card address
*
* Output:
*       mmcIdent        Card Identification record.
*
* Returns:
*       Completion code
*
********************************************************************************/
MMC_CC mmcGetCardIdentification ( PDEVICE_CONTROLLER pc,
				UINT16 RCA,
				UCHAR *mmcIdent )
{
#if (USE_MMC || USE_MMC_EMULATION)
	UCHAR   *resp_bytes;
	INT16   ij;
#endif
	MMC_CC  resErr;
	UINT16  dRCA;

	dRCA = RCA;

	resp_bytes = (UCHAR *)pc->LastResponse;

	dRCA = RCA + 1;

	mmcSetStandbyState( pc, dRCA );
	resErr = mmcCommandAndResponse ( pc,
				SEND_CID, 
				((ULONG)(dRCA)<< 16),
				0,
				R2 );

	if ( resErr == MMC_NO_ERROR )
	{
		for (ij = 0; ij < (CID_BYTE_LENGTH-1); ij++)
		{
			mmcIdent[ij] = resp_bytes[ij+1];
		}
	}
		
	pc->error_code = (UINT16)resErr;
	return resErr;
}



/*******************************************************************************
* Name: mmcGetConfiguration
* 
* Description:
*       Get the card configuration parameters (reads the card CSD reg.)
* 
* Input:
*       UINT16  RCA             Card address.
*       UCHAR   respCSD         Pointer to CSD information
*
* Output:
*       respCSD         Card configuration record.
*
* Returns:
*       Completion code 
* 
********************************************************************************/
MMC_CC mmcGetConfiguration( PDEVICE_CONTROLLER pc,
			UINT16 RCA,
			UCHAR *respCSD )
{
#if (USE_MMC || USE_MMC_EMULATION)
	UCHAR   *resp_bytes;
	UINT16  ij;
#endif
	MMC_CC  resErr;
	UINT16  dRCA;


	dRCA = RCA;

	resp_bytes = (UCHAR *)pc->LastResponse;

	dRCA = RCA + 1;                 /* Correct RCA when assigned the ID to the card */
	mmcSetStandbyState( pc, dRCA );
	resErr = mmcCommandAndResponse ( pc,
				SEND_CSD, 
				((ULONG)dRCA << 16),
				0,
				R2 );
	

	if ( resErr == MMC_NO_ERROR )
	{
		/* Get all CSD data field */
		for (ij = 0; ij < (CID_BYTE_LENGTH - 1); ij++)
			respCSD[ij] = resp_bytes[ij+1];
	}

	pc->error_code = (UINT16)resErr;
	return resErr;
}

/*******************************************************************************
* Name: mmcSetTempWP
*
* Description:
*       Sets/Clears the temporary WP bit of the card.
*
* Input:
*       RCA             card address
*       temp_wp         Set reset flag.
*
* Output:
*       None.
*
* Returns:
*       Completion code
*
********************************************************************************/
MMC_CC mmcSetTempWP ( PDEVICE_CONTROLLER pc, UINT16 RCA, SDBOOL temp_WP )
{
	MMC_CC  resErr;
	UINT16  dRCA;
	UINT16  i;
	UCHAR   resp_bytes[18];

	dRCA = RCA;
	i = 0;

	pc->user_address = (USERADDRESS)resp_bytes;

	dRCA = RCA + 1;
	mmcSetStandbyState( pc, dRCA );
	resErr = mmcCommandAndResponse ( pc,
				SEND_CSD, 
				((ULONG)dRCA << 16),
				0,
				R2 );
	if ( resErr != MMC_NO_ERROR )
	{
		pc->error_code = (UINT16)resErr;
		return resErr;
	}

	for (i = 0; i < 16; i++)
		resp_bytes[i] = ((UCHAR *)pc->LastResponse)[i+1];

	/* Check for error */
	if ( resErr )
	{
		pc->error_code = (UINT16)resErr;
		return resErr;
	}

	if ( temp_WP )    
		/* Set temporary write protection bit to 1 */
		resp_bytes[14]= (UCHAR)(resp_bytes[14] | (UCHAR)TMPWPBIT_ON);
	else
		/* Clear temporary write protection */
		resp_bytes[14]= (UCHAR)(resp_bytes[14] & (UCHAR)TMPWPBIT_OFF);

	i = appendCmdCRC( resp_bytes, 16 ); /* calculated new CRC */
	resp_bytes[15] = (UCHAR)i;

	mmcSetXferState( pc, dRCA );

	resErr = mmcCommandAndResponse ( pc,
				PROGRAM_CSD, 
				0L,
				0,
				R1 );

	pc->user_address = (USERADDRESS)resp_bytes;
	if ( resErr == MMC_NO_ERROR )
		resErr = send_data( pc, 16 );


	pc->error_code = (UINT16)resErr;
	return resErr;
}


/*******************************************************************************
* Name: mmcGetWPMap
*
* Description:
*       Reads the card's group-write-protect map. It will return the
*       WP state of 32 groups starting ast the addressed group. A '1'
*       In the returned block means group is write protected.
*
* Input:
*       dataAddress     WP group address
*       RCA             card address
* 
* Output:
*       bufPtr                  read data
*
* Returns:
*       Completion code.
*
********************************************************************************/
MMC_CC mmcGetWPMap ( PDEVICE_CONTROLLER pc,
		UINT16 RCA,
		ULONG dataAddress )
{
	MMC_CC  resErr;

	UINT16  saveBlkLen;

	mmcSetXferState( pc, (RCA+1) );

	saveBlkLen = pc->block_size;
	resErr = mmcCommandAndResponse ( pc,
				SET_BLOCKLEN, 
				4L,
				0,
				R1 );
	if ( resErr == MMC_NO_ERROR )
	{
		resErr = mmcCommandAndResponse ( pc,
				SEND_WRITE_PROT, 
				(dataAddress << 18),
				0,
				R1 );

		if ( resErr == MMC_NO_ERROR )
			resErr = receive_data( pc, 4 );
	}

#if (USE_MMC || USE_MMC_EMULATION)
	if ( resErr == MMC_NO_ERROR )
	{
		mmcSetXferState( pc, (RCA+1) );

		pc->block_size = saveBlkLen;
		resErr = mmcCommandAndResponse ( pc,
				SET_BLOCKLEN, 
				(ULONG)saveBlkLen,
				0,
				R1 );
	}
#endif

	pc->error_code = (UINT16)resErr;
	return resErr;
}


/*******************************************************************************
* Name: mmcSetGroupWP
*
* Description:
*       Sets the write protection bit of the addressed group
*
* Input:
*       dataAddress     Write protect group address
*       RCA             Card address.
*
* Output:
*       None.
*
* Returns:
*       Completion code
*
********************************************************************************/
MMC_CC mmcSetGroupWP ( PDEVICE_CONTROLLER pc,
			UINT16 RCA,
			ULONG dataAddress )
{
	MMC_CC  resErr;
	UINT16  dRCA;

	dRCA = RCA;

	dRCA += 1;
	mmcSetXferState( pc, dRCA );

	resErr = mmcCommandAndResponse ( pc,
				SET_WRITE_PROT, 
				(dataAddress << 18),
				0,
				R1 );

	pc->error_code = (UINT16)resErr;
	return resErr;
}

/*******************************************************************************
* Name: mmcClearGroupWP
*
* Description:
*       Clears the write protection bit of the addressed group
* 
*
* Input:
*       dataAddress     Write protect group address
*       RCA             Card address.
*
* Output:
*       None.
*
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC mmcClearGroupWP ( PDEVICE_CONTROLLER pc,
			UINT16 RCA,
			ULONG dataAddress )
{
	MMC_CC  resErr;
	UINT16  dRCA;

	dRCA = RCA;

#if (USE_SPI || USE_SPI_EMULATION)
	selectChip( dRCA );
#else   /* This is for MMC */
	dRCA += 1;
	mmcSetXferState( pc, dRCA );
#endif

	resErr = mmcCommandAndResponse ( pc,
				CLR_WRITE_PROT, 
				(dataAddress << 18),
				0,
				R1 );

#if (USE_SPI || USE_SPI_EMULATION)
	/* DeSelect the card */
	deSelectChip( dRCA );
#endif

	pc->error_code = (UINT16)resErr;
	return (resErr);
}


/*******************************************************************************
* Name: mmcBlkLengthConfiguration
*
* Description:
*       Set the block length
* 
* Input:
*       pc                      Device structure information
*       RCA                     Card address.
*
* Output:
*       None.
*
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC mmcBlkLengthConfiguration( PDEVICE_CONTROLLER pc, UINT16 RCA )
{
	MMC_CC  resErr;
	UINT16  dRCA;

	dRCA = RCA;

#if (USE_SPI || USE_SPI_EMULATION)
	selectChip( dRCA );
#else   /* This is for MMC */
	dRCA += 1;
	mmcSetXferState( pc, dRCA );
#endif

	resErr = mmcCommandAndResponse ( pc,
				SET_BLOCKLEN, 
				(ULONG)pc->block_size,
				0,
				R1 );

#if (USE_SPI || USE_SPI_EMULATION)
	/* DeSelect the card */
	deSelectChip( dRCA );
#endif

	pc->error_code = (UINT16)resErr;
	return (resErr);
}
  

#if (!USE_MULTI)        /* Single block Read/Write */
/***********************************************************************************
* Name: mmcRead - Reads one sector from card
*                                                                                                                                                                       |
* Description:
*       This function will (unless already selecetd) select
*       an mmc card using the given RCA (CMD7) set the block
*       length to 512 bytes (CMD16) and read one data
*       block using single block read (CMD17)
* 
* Input:
*       dataAddress -    Sector address
*       RCA         -    card address
*
* Output:
*       bufPtr      -    read data
*
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC mmcRead( PDEVICE_CONTROLLER pc,
		UINT16 RCA,
		ULONG dataAddress,
		UINT16 noBlocks )
{
	MMC_CC  resErr;
	UINT16  dRCA;

	dRCA = RCA;

#if (USE_SPI || USE_SPI_EMULATION)
	selectChip( dRCA );
#else   /* This is for MMC */
	dRCA += 1;
	mmcSetXferState( pc, dRCA );
#endif

	resErr = mmcCommandAndResponse ( pc,
				READ_BLOCK, 
				(dataAddress * (ULONG)pc->block_size), 
				noBlocks,
				R1 );
	if (resErr == MMC_NO_ERROR)
	{
		resErr = receive_data( pc, pc->block_size );
	}

#if (USE_SPI || USE_SPI_EMULATION)
	/* DeSelect the card */
	deSelectChip( dRCA );
#endif

	pc->error_code = (UINT16)resErr;
	return resErr;
}



/***********************************************************************************
* Name: mmcWrite
*
* Description:
*       Writes one block of data to the device card sector
*
*       This function will (unless already selecetd) select an
*       mmc card using the given RCA (CMD7) set the block
*       length to 512 bytes (CMD16) and write one data
*       block using single block write (CMD24) 
* 
* Input:
*       dataAddress     -       Sector address
*       RCA             -       Card address.
* 
* Output:
*       None.
*                                                                                                     
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC mmcWrite ( PDEVICE_CONTROLLER pc,
		UINT16 RCA,
		ULONG dataAddress,
		UINT16 noBlocks )
{
	MMC_CC  resErr;
	UINT16  dRCA;

	dRCA = RCA;


#if (USE_SPI || USE_SPI_EMULATION)
	selectChip( dRCA );
#else   /* This is for MMC */
	dRCA += 1;
	mmcSetXferState( pc, dRCA );
#endif

	resErr = mmcCommandAndResponse ( pc,
				WRITE_BLOCK, 
				(dataAddress * (ULONG)pc->block_size), 
				noBlocks,
				R1 );
	if ( resErr == MMC_NO_ERROR )
		resErr = send_data( pc, pc->block_size );

#if (USE_SPI || USE_SPI_EMULATION)
	/* DeSelect the card */
	deSelectChip( dRCA );
#endif

	pc->error_code = (UINT16)resErr;
	return resErr;
}
#endif  /* (!USEMUTI) */


#if (USE_MULTI)
#if (USE_MMC || USE_MMC_EMULATION)

/***********************************************************************************
* Name: mmcStopTransmission
*
* Description:
*       Stop Read multiple card sectors (CMD12)
*
* Input:
*
* Output:
*
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC mmcStopTransmission ( PDEVICE_CONTROLLER pc, UINT16 RCA)
{
	RCA = RCA;
	return(mmcCommandAndResponse ( pc, 
				STOP_TRANSMISSION,
				0L,
				0,
				R1 ));
}


/*******************************************************************************
* Name: mmcReadMultiple -  (MMC mode only)
*
* Description:
*       Initiate a multiple block read operation.
*
*       This function will (unless already selecetd) select an MMC
*       card using the given RCA (CMD7) set the block length to
*       512 bytes (CMD16) and starts the multiple read (CMD18).
*
* Input:
*       dataAddress -    Sector address
*       RCA         -    card address
* 
* Output:
*       bufPtr      -    read data
*
* Returns:
*       Completion code
*
********************************************************************************/
MMC_CC mmcReadMultiple( PDEVICE_CONTROLLER pc,
			UINT16 RCA,
			ULONG dataAddress,
			UINT16 noBlocks )
{
	MMC_CC  resErr;
	MMC_CC  tmpErr;

	resErr = mmcSetXferState( pc, (RCA+1) );

	if ( resErr == MMC_NO_ERROR )
	{
		resErr = mmcCommandAndResponse ( pc,
				READ_MULTIPLE_BLOCK,
				(dataAddress * (ULONG)pc->block_size),
				noBlocks,
				R1 );
	}

	if ( resErr != MMC_NO_ERROR )
	{                
		pc->error_code = (UINT16)resErr;
		return resErr;
	}

	while ( pc->sectors_remaining )
	{
		resErr = receive_data( pc, pc->block_size );
		if ( resErr != MMC_NO_ERROR )
		{
			break;
		}

		pc->sectors_remaining--;

		/* Next Block address */
#if (WORD_ACCESS_ONLY)
		pc->user_address += (pc->block_size >> 1);
#else
		pc->user_address += pc->block_size;
#endif
	}

	/* Stop receiving data */
	tmpErr = mmcStopTransmission( pc, (UINT16)RCA );

	if ( resErr == MMC_NO_ERROR )
		resErr = tmpErr;

	if ( resErr )
	{
		pc->error_code = (UINT16)resErr;
		return resErr;
	}

	/* Get device status */
	pc->mmcRdyState = NO;
	pc->sectors_remaining = 10;                        
	while ( (pc->mmcRdyState != YES) && (pc->sectors_remaining > 0) )
	{
		/* Get the device status */
		resErr = mmcGetStatus( pc, (UINT16)RCA );
		if ( resErr )
			return NO;
		pc->sectors_remaining--;

		/* Delay it for awhile */
		OS_WAIT(10);
	}

	pc->error_code = (UINT16)resErr;
	return resErr;
}


/***********************************************************************************
* Name: mmcWriteMultiple (MMC mode only) 
*
* Description:
*       Initiates a multiple block write operation.              
*
*       This function will (unless already selecetd) select
*       an mmc card using the given RCA (CMD7) set the block
*       length to a specified block length (CMD16) and
*       starts the multiple write (CMD15).
*
* Input:
*       dataAddress     -       Sector address
*       bufPtr          -       Data to be written.
*       RCA             -       card address
*                                                                                                                                                                       |
*                                                                                                                                                                       |
* Returns:
*       Completion code                                 
*
************************************************************************************/
MMC_CC mmcWriteMultiple( PDEVICE_CONTROLLER pc,
			UINT16 RCA,
			ULONG dataAddress,
			UINT16 noBlocks )
{
	MMC_CC  resErr;
	MMC_CC  tmpErr;

	resErr = mmcSetXferState( pc, (RCA+1) );

	if ( resErr == MMC_NO_ERROR )
	{
		resErr = mmcCommandAndResponse ( pc,
				WRITE_MULTIPLE_BLOCK,
				(dataAddress * (ULONG)pc->block_size),
				noBlocks,
				R1 );
	}

	if ( resErr != MMC_NO_ERROR )
	{                
		pc->error_code = (UINT16)resErr;
		return resErr;
	}

	while ( pc->sectors_remaining )
	{
		resErr = send_data( pc, pc->block_size );
		if ( resErr != MMC_NO_ERROR )
		{
			break;
		}

		pc->sectors_remaining--;

		/* Next Block address */
#if (WORD_ACCESS_ONLY)
		pc->user_address += (pc->block_size >> 1);
#else
		pc->user_address += pc->block_size;
#endif
	}

	/* Stop sending data */
	tmpErr = mmcStopTransmission( pc, (UINT16)RCA );

	if ( resErr == MMC_NO_ERROR )
		resErr = tmpErr;

	if ( resErr )
	{
		pc->error_code = (UINT16)resErr;
		return resErr;
	}


	/* Get device status */
	pc->mmcRdyState = NO;
	pc->sectors_remaining = 10;                        
	while ( (pc->mmcRdyState != YES) && (pc->sectors_remaining > 0) )
	{
		/* Get the device status */
		resErr = mmcGetStatus( pc, (UINT16)RCA );
		if ( resErr )
			return NO;
		pc->sectors_remaining--;

		/* Delay it for awhile */
		OS_WAIT(10);
	}

	pc->error_code = (UINT16)resErr;
	return resErr;
}

#endif  /* USE_MMC || USE_MMC_EMULATION */
#endif  /* USE_MULTI */

/************************************************************************************
* Name: mmcEraseSectors
*
* Description:
*       Erases a sector range on the card. The rane must be within
*       a single erase group.
*
* Input:
*       PDEVICE_CONTROLLER      pc
*       UINT16  RCA     Card address. 
*       ULONG   Start   Adsress of first sector in the erase range.
*       ULONG   End     Address of the last sector in the erase range.
*       ULONG   untag   Adress of one sector in the range to skip (not erase)
*
* Output:
*       None
*
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC mmcEraseSectors ( PDEVICE_CONTROLLER pc, 
			UINT16 RCA,
			ULONG start,
			ULONG end,
			ULONG untag )
{
	MMC_CC  resErr;
	UINT16  dRCA;

	dRCA = RCA;

#if (USE_SPI || USE_SPI_EMULATION)
	selectChip( dRCA );
#else   /* This is for MMC */
	dRCA += 1;
	mmcSetStandbyState( pc, dRCA );
#endif

	/* START */
	resErr = mmcCommandAndResponse ( pc,
				TAG_SECTOR_START,
				(start << 9),
				0,
				R1 );
	if ( resErr == MMC_NO_ERROR )
	{
		/* END */
		resErr = mmcCommandAndResponse ( pc,
				TAG_SECTOR_END,
				(end << 9),
				0,
				R1 );
	}
	if ( resErr == MMC_NO_ERROR )
	{
		/* UNTAG */
		if (untag)
		{
			resErr = mmcCommandAndResponse ( pc,
					UNTAG_SECTOR,
					(untag << 9),
					0,
					R1 );
		}
	}
	if ( resErr == MMC_NO_ERROR )
	{
		/* ERASE */
		resErr = mmcCommandAndResponse ( pc,
				ERASE_SECTORS, 
				0,
				0,
				R1 );
	}

#if (USE_SPI || USE_SPI_EMULATION)
	/* DeSelect the card */
	deSelectChip( dRCA );
#endif

	pc->error_code = (UINT16)resErr;
	return (resErr);
}

/***********************************************************************************
* Name: mmcEraseGroup
*
* Description:
*       Erases an erase group range on the card.
*
* Input:
*       PDEVICE_CONTROLLER      pc
*       RCA             Card address.
*       Start   Adsress of first earse group in the erase range.
*       End             Address of the last erase group in the erase range.
*       untag   Adress of one earse group in the range to skip (not erase)
*
* Output:
*       None.
*       
* Returns:
*       Completion code.
*
************************************************************************************/
MMC_CC mmcEraseGroup( PDEVICE_CONTROLLER pc,
			UINT16 RCA,
			ULONG start, 
			ULONG end, 
			ULONG untag )
{
	MMC_CC  resErr;
	UINT16  dRCA;

	dRCA = RCA;

#if (USE_SPI || USE_SPI_EMULATION)
	selectChip( dRCA );
#else   /* This is for MMC */
	dRCA += 1;
	mmcSetStandbyState( pc, dRCA );
#endif

	/* START */             
	resErr = mmcCommandAndResponse ( pc,
				TAG_ERASE_GROUP_START, 
				(start << 13),
				0,
				R1 );
	if ( resErr == MMC_NO_ERROR )
	{
		/* END */
		resErr = mmcCommandAndResponse ( pc,
				TAG_ERASE_GROUP_END, 
				(end << 13),
				0,
				R1 );
	}
	if ( resErr == MMC_NO_ERROR )
	{
		/* UNTAG */
		if(untag)
		{
			resErr = mmcCommandAndResponse ( pc,
					UNTAG_ERASE_GROUP, 
					(untag << 13),
					0,
					R1 );
		}
	}

	if ( resErr == MMC_NO_ERROR )
	{
		/* ERASE */
		resErr = mmcCommandAndResponse ( pc,
				ERASE_SECTORS, 
				0L,
				0,
				R1 );
	}

#if (USE_SPI || USE_SPI_EMULATION)
	/* DeSelect the card */
	deSelectChip( dRCA );
#endif

	pc->error_code = (UINT16)resErr;
	return (resErr);
}

/***********************************************************************************
* Name: mmcErase
*
* Description:
*       Erases a sector range on the card.
* 
* Input:
*       PDEVICE_CONTROLLER      pc
*       UINT16  RCA     Device address
*       ULONG   Start   Adsress of first sector in the erase range.
*       ULONG   End     Address of the last sector in the erase range. 
* 
* Output:
*       None.
* 
* Returns:
*       Completion code.
* 
************************************************************************************/
MMC_CC mmcErase( PDEVICE_CONTROLLER pc,
		UINT16 RCA,
		ULONG start, 
		ULONG end )
{
	ULONG   numSectStartGR, numSectEndGR;
	ULONG   S_EraseGroup, E_EraseGroup;
	MMC_CC  resErr;


	/* START PARAMETERS */
	numSectStartGR = DEFAULT_ERASE_GROUP - (start % DEFAULT_ERASE_GROUP);

	/* END PARAMETERS */
	numSectEndGR = end % DEFAULT_ERASE_GROUP;

	S_EraseGroup = start / DEFAULT_ERASE_GROUP;
	E_EraseGroup = end / DEFAULT_ERASE_GROUP;
		
	if ( (E_EraseGroup - S_EraseGroup) > 1 )
	{
		if (numSectStartGR)
		{
			S_EraseGroup += 1;
			resErr = mmcEraseSectors( pc,
					RCA,
					start, 
					(start + numSectStartGR), 
					0 );
			if ( resErr )
			{
				pc->error_code = (UINT16)resErr;
				return resErr;
			}
		}
		if (numSectEndGR)
		{
			E_EraseGroup -= 1;
			resErr = mmcEraseSectors( pc,
					RCA,
					(end - numSectEndGR + 1), 
					end, 
					0 );
			if ( resErr )
			{
				pc->error_code = (UINT16)resErr;
				return resErr;
			}
		}

		resErr = mmcEraseGroup( pc,
				RCA,
				S_EraseGroup, 
				E_EraseGroup, 
				0 );
		if ( resErr )
		{
			pc->error_code = (UINT16)resErr;
			return resErr;
		}
	}
	else if ((E_EraseGroup - S_EraseGroup) < 1)
	{
		resErr = mmcEraseSectors( pc,
				RCA,
				start,
				end,
				0 );
		if ( resErr )
		{
			pc->error_code = (UINT16)resErr;
			return resErr;
		}
	}       
	else if ((E_EraseGroup - S_EraseGroup) == 1)
	{
		if ( numSectStartGR )
		{
			resErr = mmcEraseSectors( pc,
					RCA,
					start, 
					(start + numSectStartGR), 
					0 );
			if ( resErr )
			{
				pc->error_code = (UINT16)resErr;
				return resErr;
			}
		}
		if (numSectEndGR)
		{
			resErr = mmcEraseSectors( pc,
					RCA,
					(end - numSectEndGR + 1), 
					end, 
					0 );
			if ( resErr )
			{
				pc->error_code = (UINT16)resErr;
				return resErr;
			}
		}
	}

	pc->error_code = MMC_NO_ERROR;
	return (MMC_NO_ERROR);
}


/***********************************************************************************
* Name: mmcGetStatus
*
* Description:
*       Retrieves the card current status.
* 
* Input:
*       PDEVICE_CONTROLLER      pc      Pointer to the controller structure
*       UINT16  RCA     Card address.
* 
* Output:
*       status  A 32 bit status word. Error/status bits.
* 
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC mmcGetStatus ( PDEVICE_CONTROLLER pc,
			UINT16 RCA)
{
	UINT16  dRCA;
	MMC_CC  resErr;


#if (USE_MMC || USE_MMC_EMULATION)
	dRCA = RCA + 1;
	resErr = mmcCommandAndResponse ( pc,
				SEND_STATUS, 
				(((ULONG)dRCA) << 16),
				0,
				R1 );

#else
	dRCA = RCA;
	selectChip ( dRCA );

	resErr = mmcCommandAndResponse ( pc,
				SEND_STATUS, 
				0L,
				0,
				R2 );

	deSelectChip( dRCA );
#endif


	pc->error_code = (UINT16)resErr;
	return (resErr);
}


#if (USE_SPI || USE_SPI_EMULATION)
#if (USE_SET_FEATURES)
/***********************************************************************************
* Name: mmcTurnCRCOnOff - (SPI mode only)
*
* Description:
*       Turns the CRC option ON or OFF.
*
* Input:
*       PDEVICE_CONTROLLER      pc      Pointer to the controller structure
*       UINT16  opt             Enable/Disable CRC feature
*               - 1, turns the option ON.
*               - 0, turns the option OFF.
*       UINT16  RCA             Card address.
*
* Output:
*       None.
*
* Returns:
*       Completion code
*
************************************************************************************/
MMC_CC mmcTurnCRCOnOff( PDEVICE_CONTROLLER pc, UINT16 RCA, UINT16 opt )
{
	MMC_CC  resErr;

	selectChip ( RCA );

	resErr = mmcCommandAndResponse ( pc,
				CRC_ON_OFF, 
				(ULONG)opt,
				0,
				R1 );

	deSelectChip ( RCA );

	pc->error_code = (UINT16)resErr;
	return (resErr);
}

#endif  /* (USE_SET_FEATURES) */
#endif  /* (USE_SPI || USE_SPI_EMULATION) */



/*********************************************************************************
* Name: mmedia_io
*
* Description:
*       Read/write handler. Single and multiple READ/WRITE are supported.
*
* Input:
*       INT16 driveno
*       ULONG sector
*       UCHAR *buffer
*       UCOUNT scount
*       INT16 op
*
* Output:
*
* Return:
*
**********************************************************************************/
MMC_CC mmedia_io( INT16 driveno, ULONG sector, UCHAR *buffer, UCOUNT scount, INT16 op ) /*__fn__*/
{
	PDEVICE_CONTROLLER pc;
	ULONG   starting_sector;
	UINT16  phys_drive;
	MMC_CC  ret_val;


	starting_sector = sector;

#if (N_CONTROLLERS > 1)
	pc = dev_drno_to_controller(driveno);
#else
	pc = &controller_s[0];
#endif

	if ( !scount || !pc )             /* Must have a count */
	{
		pc->error_code = MMC_ADDRESS_ERROR;
		return (MMC_ADDRESS_ERROR);
	}

#if (N_CONTROLLERS > 1)
	phys_drive = dev_drno_to_phys(driveno);
	// select this drive now
	MMCSelectController( dev_drno_to_controller_no( driveno ) );
#else
	phys_drive = driveno;
#endif

	/* Get the block length */
	pc->block_size = pc->drive[phys_drive].block_size;

	if ( (starting_sector+scount) > pc->drive[phys_drive].total_lba )
	{
		pc->error_code = BUS_ERC_ADDR_RANGE;
		return (MMC_ADDRESS_ERROR);
	}

	ret_val = MMC_NO_ERROR;

	/* Set up a counter for data transfer */
	pc->sectors_remaining = scount;

	pc->user_address = (USERADDRESS)buffer;
	pc->mode = 0;

	if (op == READING)
	{

#if (USE_MULTI) /* Read multiple */
 #if (USE_SPI || USE_SPI_EMULATION)
		pc->error_code = MMC_ILLEGAL_MODE;
		return MMC_ILLEGAL_MODE;

 #endif

 #if (USE_MMC || USE_MMC_EMULATION)

		pc->mode = 1;
		ret_val = mmcReadMultiple(pc,
					(UINT16)phys_drive,
					starting_sector,
					scount);
		if (ret_val != MMC_NO_ERROR )
		{
			pc->error_code = (UINT16)ret_val;
			return ret_val;
		}

 #endif

#else   /* Single read */

		while (pc->sectors_remaining)
		{
			ret_val = mmcRead(pc,
					(UINT16)phys_drive,
					starting_sector,
					1);

			if ( ret_val != MMC_NO_ERROR )
			{
				pc->error_code = (UINT16)ret_val;
				return ret_val;
			}

			pc->sectors_remaining--;
			starting_sector++;

			/* Next Block address */
#if (WORD_ACCESS_ONLY)
			pc->user_address += (pc->block_size >> 1);
#else
			pc->user_address += pc->block_size;
#endif
		}
#endif
	}

#if (RTFS_WRITE)
	else if (op == WRITING)
	{
#if (USE_MULTI) /* Write multiple */

 #if (USE_SPI || USE_SPI_EMULATION)
		pc->error_code = MMC_ILLEGAL_MODE;
		return MMC_ILLEGAL_MODE;

 #endif

 #if (USE_MMC || USE_MMC_EMULATION)

		pc->mode = 1;
		ret_val = mmcWriteMultiple( pc,
					(UINT16)phys_drive,
					starting_sector,
					scount );

		if ( ret_val != MMC_NO_ERROR )
		{
			/* If there is any READ/WRITE error, then return */
			pc->error_code = (UINT16)ret_val;
			return ret_val;
		}

		/*
		Wait for BUSY to go off before the next transfer. The
		device may very well be programmed data into the flash memory.
		*/
		if ( ret_val )            /* If there is any READ/WRITE error, then return */
		{
			pc->error_code = (UINT16)ret_val;
			return ret_val;
		}
 #endif

#else           /* Single write */

		while ( pc->sectors_remaining )
		{
			ret_val = mmcWrite( pc,
					(UINT16)phys_drive,
					starting_sector,
					1 );

			if ( ret_val != MMC_NO_ERROR )
			{
				pc->error_code = (UINT16)ret_val;
				return (ret_val);
			}

			pc->sectors_remaining--;        /* Next block */
			starting_sector++;
#if (WORD_ACCESS_ONLY)
			pc->user_address += (pc->block_size >> 1);
#else
			pc->user_address += pc->block_size;
#endif
		}
#endif
	}

#endif  /* (RTFS_WRITE) */
	else if ( op == ERASING )
	{
		ret_val = mmcEraseSectors( pc,
					(UINT16)phys_drive,
					starting_sector,
					(starting_sector + (ULONG)scount),
					0L );

		if ( ret_val )
		{
			pc->error_code = (UINT16)ret_val;
			return (ret_val);
		}

		/* Get device status */
		pc->mmcRdyState = NO;
		pc->sectors_remaining = 10;                        
		while ( !pc->mmcRdyState && (pc->sectors_remaining > 0) )
		{
			/* Get the device status */
			ret_val = mmcGetStatus( pc, (UINT16)phys_drive );
			if ( ret_val )
				return NO;
			pc->sectors_remaining--;

			/* Delay it for awhile */
			OS_WAIT(10);
		}
	}

	pc->error_code = (UINT16)ret_val;
	return (ret_val);
}


/****************************************************************************************
* Name: mmedia_drive_open
*
* Description:
*       Initialize a selected drive and set up data structure.
*
* Input:
*       INT16 driveno
*
* Output:
*
* Return:
*       YES if successful
*       NO if failure
*
*****************************************************************************************/
SDBOOL mmedia_drive_open ( INT16 driveno ) /*__fn__*/
{
	PDEVICE_CONTROLLER pc;
	ULONG   ltemp;
	UINT16  heads;
	UINT16  sectors;
	INT16   phys_drive;
#if (USE_FILE_SYSTEM)
	UCHAR   *buf;

        buf = (UCHAR *)lock_temporary_buffer();
#else
	UCHAR   buf[128];
#endif

#if (N_CONTROLLERS > 1)
	pc = dev_drno_to_controller(driveno);
	phys_drive = dev_drno_to_phys(driveno);
	// select this drive now
	MMCSelectController( dev_drno_to_controller_no( driveno ) );
	
#else
	pc = &controller_s[0];
	phys_drive = driveno;
#endif

	if (!pc)
		return(NO);

	if ( !pc->opencount )
	{
#if (N_CONTROLLERS > 1)
		pc->controller_number = dev_drno_to_controller_no(driveno);
#else
		pc->controller_number = 0;
#endif

		/* Set up OS operating layer and interrupt service routines */
		OS_CONTROLLER_INIT(pc->controller_number);
		pc->opencount = YES;
	}


	/* First complete the IDENTIFICATION STATE on a selected card. */
#if (USE_MMC || USE_MMC_EMULATION)
	/* Assign a relative device ID to a selected device */
/*        if ( mmcConfigDevice(driveno) != MMC_NO_ERROR )
		return(NO);
*/
#else
	if( mmcReset(pc, (ULONG)driveno) != MMC_NO_ERROR )
		return(NO);
#endif

#if (USE_SET_FEATURES)
 #if (USE_SPI || USE_SPI_EMULATION)
	/* Change the default value of CRC to ON */
	if ( mmcTurnCRCOnOff(pc, (UINT16)(driveno), YES) )
		return(NO);
 #endif
#endif


	/* Get device information */
	if ( mmcGetCardIdentification(pc, (UINT16)phys_drive, buf) != MMC_NO_ERROR )
		return(NO);

	/* Get device configuration */
	if ( mmcGetConfiguration(pc, (UINT16)phys_drive, (buf+64)) != MMC_NO_ERROR )
		return(NO);

	/* Get the current block length */
	heads = (((UINT16)buf[64+5]) & 0x0F);
	sectors = (UINT16)(1 << heads);

	/*
	If the block length is not 512-byte block length, configure it
	back to 512-byte block length. 
	*/

	/* Initialize default block length */
	pc->block_size = DEFAULT_BLK_LEN;
#if (USE_MMC || USE_MMC_EMULATION)
	/* Set the card with default block length */
	if ( mmcBlkLengthConfiguration( pc, phys_drive ) != MMC_NO_ERROR ) 
		return NO;
#endif

	/* The block length changed,  get CSD again */
	if ( sectors != DEFAULT_BLK_LEN )
	{
		/* Get device configuration again with new block length */
		if ( mmcGetConfiguration(pc, (UINT16)phys_drive, (buf+64)) != MMC_NO_ERROR )
			return(NO);

	}

	/* Get the device capacity */
	/* Device size */
	sectors = (((UINT16)buf[64+6]) & 3);
	sectors <<= 8;
	ltemp = ((ULONG)buf[64+7]) + (ULONG)sectors;
	ltemp <<= 2;
	sectors = ((UINT16)(buf[64+8]) >> 6) & 3;
	ltemp += ((ULONG)sectors + 1L);

	/* Device size multiplier */
	heads = ((UINT16)buf[64+9] & 3);
	heads <<= 1;
	sectors = ((((UINT16)buf[64+10]) >> 7) & 1);

	/* Totoal device capacity */
	ltemp <<= ((heads + sectors + 2));

	heads   = 2;            /* Number of heads */
	if ( ltemp > 0xFFFF )  /* 32 MB in size */
		heads = 4;

	sectors  = 32;          /* Number of sectors per track */

	pc->drive[phys_drive].total_lba = ltemp;
	ltemp = ltemp / (ULONG)(heads * sectors);

	pc->drive[phys_drive].max_multiple = 1;
	pc->drive[phys_drive].num_cylinders = (UINT16)ltemp;

#if (USE_ONLY_LBA)
	pc->drive[phys_drive].supports_lba = YES;
#else
	pc->drive[phys_drive].supports_lba = NO;
#endif

	/* Initialize the block size for each device */
	pc->drive[phys_drive].block_size = pc->block_size;

	/* Our view of the drive for block to track::sector::head xlations */
	pc->drive[phys_drive].num_heads =  (UCHAR)heads;
	pc->drive[phys_drive].sec_p_track = (UCHAR)sectors;
/*        pc->drive[phys_drive].sec_p_cyl = (UINT16)(heads * sectors); */

#if (USE_FILE_SYSTEM)
	release_temporary_buffer();
#endif

	return (YES);
}


/**************************************************************************************** 
* Name: mmedia_drive_close
*
* Description:
*       Remove all related information to the selected device.
*
* Input:
*       INT16 driveno
*
* Output:
*
* Return:
*       YES if successful
*       NO if failure
*
*****************************************************************************************/ 
SDBOOL mmedia_drive_close ( INT16 driveno ) /*__fn__*/
{
	OS_CONTROLLER_CLOSE(driveno);
	driveno = driveno;
	return (YES);
}


#define BASE_10 10
SDLOCAL UINT16 number_to_ASCII_Str(UCHAR *asciiStrNum, UCHAR *buf);

/*************************************************************************************** 
* Name: number_to_ascii
*
* Description:
*       Convert unsigned long number to ASCII string

* Inputs:
*       UCHAR *asciiStrNum   - card serial number
*
* Returns:
*       UINT16 count
****************************************************************************************/
SDLOCAL UINT16 number_to_ASCII_Str(UCHAR *asciiStrNum, UCHAR *buf)
{
	ULONG  serialNum;
	UINT16 remainder;
	UINT16 index;
        UINT16 ind;
	UTINY arrayIndex;
	UTINY tailIndex;
	UCHAR tmpChar;

	serialNum = 0L;
	for (index=0; index < 3; index++)
	{
		serialNum <<= 8;
		serialNum |= (ULONG)buf[index];
	}

	index = 0;
	do
	{
		remainder = (UINT16)(serialNum % BASE_10);
		asciiStrNum[index++] = (UCHAR)(remainder+0x30);
		serialNum /= (ULONG)BASE_10;
	} while (serialNum);

	asciiStrNum[index] = '\x0';

	tailIndex = (UTINY)(index-1);

	arrayIndex = (UTINY)(index >> 1);
	for (ind=0; ind < arrayIndex; ind++, tailIndex--)
	{
		tmpChar = asciiStrNum[ind];
		asciiStrNum[ind] = asciiStrNum[tailIndex];
		asciiStrNum[tailIndex] = tmpChar;
	}

	return (index);
}


/*************************************************************************************** 
* Name: mmedia_read_serial
*
* Description:
*       This routine does the following operations:
*       . calls identify drive to get serial no
*       . copies the string into the output string and null terminate  
*
* Inputs:
*       driveno   - drive number
*       pserialno -  pointer to a buffer to hold the model number (at least 21 bytes)
*
* Returns:
*       YES on success else NO.
*       If NO, pc->error_no will contain the error.
*   
****************************************************************************************/
SDBOOL mmedia_read_serial(INT16 driveno, PDRV_GEOMETRY_DESC idDrvPtr) /* __fn__*/
{
	PDEVICE_CONTROLLER pc;
	UINT16  phys_drive;
	UINT16  count;
#if (USE_FILE_SYSTEM)
	UCHAR   *buf;

        buf = (UCHAR *)lock_temporary_buffer();
#else
	UCHAR   buf[128];
#endif


#if (N_CONTROLLERS > 1)
	pc = dev_drno_to_controller(driveno);
	phys_drive = dev_drno_to_phys(driveno);
#else
	pc = &controller_s[0];
	phys_drive = driveno;
#endif

	if ( !pc )
	{
		pc->error_code = MMC_INTERFACE_ERROR;
		return (NO);
	}


	/*  Use the CID information. */
	pc->user_address = (USERADDRESS)&buf[0];

	if ( mmcGetCardIdentification(pc, (UINT16)phys_drive, buf) != MMC_NO_ERROR )
		return (NO);

	idDrvPtr->dfltCyl = (UINT16)pc->drive[phys_drive].num_cylinders;
	idDrvPtr->dfltHd  = (UCHAR)pc->drive[phys_drive].num_heads;  
	idDrvPtr->dfltSct = (UCHAR)pc->drive[phys_drive].sec_p_track;

#if 0
	count = number_to_ASCII_Str(((UCHAR *)buf + 32), ((UCHAR *)buf + 11));

	dev_copybuff(idDrvPtr->serialNum, ((UCHAR *)buf + 32), (INT16)count);
	idDrvPtr->serialNum[count] = 0;
#else
	/* copy entire CID to serialNum structure */
	dev_copybuff(idDrvPtr->serialNum, buf, 16);
#endif
	for (count = 0; count < 7; count++)
		buf[count] = buf[count+3];

	dev_copybuff(idDrvPtr->modelNum, ((UCHAR *)buf), 7);
	idDrvPtr->modelNum[7] = 0;

	idDrvPtr->totalLBA = (UINT32)pc->drive[phys_drive].total_lba;

#if (USE_FILE_SYSTEM)
	release_temporary_buffer();
#endif

	return(YES);
}


SDBOOL setBlockSize( INT16 driveno, UINT16 blockSize )
{
	PDEVICE_CONTROLLER pc;
	INT16   phys_drive;
	UINT16  saveBlockSize;

#if (N_INTERFACES > 1)
	pc = dev_drno_to_controller(driveno);
	phys_drive = dev_drno_to_phys(driveno);
#else
	pc = &controller_s[0];
	phys_drive = driveno;
#endif

	saveBlockSize = pc->block_size;

	pc->block_size = blockSize;
	if ( mmcBlkLengthConfiguration( pc, driveno ) != MMC_NO_ERROR )
	{
		pc->block_size = saveBlockSize;
		pc->drive[driveno].block_size = saveBlockSize;
		return MMC_COMMAND_PARAMETER_ERROR;
	}

	return MMC_NO_ERROR;
}


INT16 get_extended_error(INT16 driveno) /* __fn__*/
{
	PDEVICE_CONTROLLER pc;
	INT16   phys_drive;

#if (N_INTERFACES > 1)
	pc = dev_drno_to_controller(driveno);
	phys_drive = dev_drno_to_phys(driveno);
#else
	pc = &controller_s[0];
	phys_drive = driveno;
#endif

	if ( !pc )
	{
		pc->error_code = BUS_ERC_CNTRL_INIT;
		return (BUS_ERC_CNTRL_INIT);
	}

	return (pc->error_code);
}


#endif  /* (USE_SPI || USE_MMC || USE_SPI_EMULATION || USE_MMC_EMULATION) */



