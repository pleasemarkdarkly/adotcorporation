//=============================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// INSERT COPYRIGHT HERE
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   toddm
// Contributors:danc
// Date:        2000-06-12
// Purpose:     Thread to handle mass storage class protocol
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/io/io.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

#include <dadio/io/blk_dev.h>
#include <dadio/io/dev_mmc.h>
#include <dadio/io/pm.h>

#include <dadio/io/usb100.h>
#include <dadio/io/pdiusbd12.h>
#include <dadio/io/mass_storage.h>
#include <dadio/io/rbc.h>
#include <dadio/io/usb.h>

#define BLOCK_SIZE 512

cyg_io_handle_t bulk_endpointH = 0;
cyg_io_handle_t blk_devH[MAX_LUN] = { 0 };
request_sense_data_t sense_data;
extern cyg_mutex_t bulk_thread_safe_to_reset;
#define DATA_BUF_SIZE (64 * 1024)
char * data_buf = 0;		/* buffer used for transfer of the actual data */

#define OP_READING 1
#define OP_WRITING 2
#define OP_STATUS  3
#define OP_GEOMETRY 4

void
set_sense_data(int op, Cyg_ErrNo res)
{
    switch (res) {
	case -ENOMED:
	    sense_data.sense_key = NOT_READY;
	    sense_data.asc = ASC(MEDIUM_NOT_PRESENT);
	    sense_data.ascq = ASCQ(MEDIUM_NOT_PRESENT);
	    break;

	case -EMEDCHG:
	    sense_data.sense_key = UNIT_ATTENTION;
	    sense_data.asc = ASC(MEDIUM_CHANGED);
	    sense_data.ascq = ASCQ(MEDIUM_CHANGED);
	    break;

	default:
	    /* assume -EIO error */
	    sense_data.sense_key = MEDIUM_ERROR;
	    switch (op) {
		case OP_READING: 
		    sense_data.asc = ASC(READ_ERROR);
		    sense_data.ascq = ASCQ(READ_ERROR);
		    break;

		case OP_WRITING:
		    sense_data.asc = ASC(WRITE_ERROR);
		    sense_data.ascq = ASCQ(WRITE_ERROR);
		    break;

		case OP_STATUS:
		    diag_printf("OP_STATUS this should never be called\n");
		    break;

		case OP_GEOMETRY:
		    /* TODO find a better asc for this case */
		    sense_data.asc = ASC(READ_ERROR);
		    sense_data.ascq = ASCQ(READ_ERROR);
		    break;
	    }
	    break;
    }
}

void
bulk_thread(cyg_addrword_t data)
{
    cbw_t cbw;
    csw_t csw;
    cyg_uint32 len;
    Cyg_ErrNo err;
    char lun;
    char dev_name[] = "/dev/hda";
    
    /* get device handle to usb */
    err = cyg_io_lookup("/dev/usb2", &bulk_endpointH);
    if (err == ENOERR) {
	diag_printf("bulk_thread: found /dev/usb2 handle\n");
    }
    else {
	diag_printf("ERROR bulk thread: could not find /dev/usb2 handle\n");
    }
    
    /* get device handle to drives */
    for (lun = 0; lun < MAX_LUN; ++lun) {
	if (blk_devH[lun] == 0) { /* only do this once */
	    dev_name[7] = 'a' + lun;
	    if (cyg_io_lookup(dev_name, &blk_devH[lun]) == ENOERR) {
		diag_printf("bulk_thread: found %s handle\n", dev_name);
		len = sizeof(len);
		cyg_io_set_config(blk_devH[lun], IO_PM_SET_CONFIG_REGISTER, &len, &len);
	    }
	    else {
		diag_printf("ERROR bulk_thread: could not find %s handle\n", dev_name);
	    }
	}
    }

    /* initialize data buffer */
    if (data_buf == 0) {
	data_buf = (char *)malloc(DATA_BUF_SIZE);
	if (!data_buf) {
	    diag_printf("could not allocate data buffer of 64k\n");
	    diag_printf("ERROR: STOPPING BULK_THREAD\n");
	    for (;;)
		;
	}
    }
    
    /* initialize sense_data */
    memset(&sense_data, 0, sizeof(sense_data));
    sense_data.sense_key = UNIT_ATTENTION;
    sense_data.asc = ASC(RESET_OCCURRED);
    sense_data.ascq = ASCQ(RESET_OCCURRED);
    
    /* bulk-only transport main loop */
    while (1) {
	len = sizeof(cbw);
	/* TODO move stall check into usb_read */
	/* have not seen this case occur yet anywhere, though */
	while (d12_select_endpoint(4) & 0x02)
	    ;
	err = cyg_io_read(bulk_endpointH, &cbw, &len);
	if (len != sizeof(cbw)) {
	    diag_printf("cbw: len = %2x\n", len);
	}

	/* validate CBW */
	if (len == sizeof(cbw) && cbw.dCBWSignature == CBW_SIGNATURE) {
	    if (rbc_handler(&cbw, &csw)) {
		/* send CSW */
		csw.dCSWSignature = CSW_SIGNATURE;
		csw.dCSWTag = cbw.dCBWTag;
		/* TODO move stall check into usb_write */
		/* although this is the only place a stall is encountered currently */
		while (d12_select_endpoint(5) & 0x02) /* 0x02 == endpoint stalled */
		    ;
		len = sizeof(csw);
		err = cyg_io_write(bulk_endpointH, &csw, &len);
	    }
	    else {
		diag_printf("b\n");
		/* CBW not meaningful */
		/* device response to this unspecified according to spec */
	    }
	}
	else {
	    diag_printf("c\n");
	    /* stall bulk-in endpoint */
	    d12_set_endpoint_status(5, 1);
	    /* can either stall the bulk-out endpoint, or discard any data read, i choose the easier */
	    d12_set_endpoint_status(4, 1);

	    /* this state is maintained until Reset Recovery received */
	}
    }
}

bool
data_in(cbw_t * cbw, csw_t * csw, int (*cmd_f)(unsigned char, rbc_cmd_t *, char *, int))
{
    rbc_cmd_t * cmd = (rbc_cmd_t *)&cbw->CBWCB;
    int len;
    Cyg_ErrNo err;
    
    if (cbw->dCBWDataTransferLength > DATA_BUF_SIZE) {
	char * new_data_buf = 0;
	
	diag_printf("di tl %x\n", cbw->dCBWDataTransferLength);
	/* try to re-allocate data buffer to larger size */
	new_data_buf = (char *)realloc(data_buf, cbw->dCBWDataTransferLength);
	if (new_data_buf == 0) {
	    diag_printf("could not allocate necessary space\n");
	    diag_printf("ERROR STOPPING BULK THREAD\n");
	    for (;;)
		;
	}
	data_buf = new_data_buf;
    }
    
    /* this template should work for all data-in transfers */
    if (cbw->bmCBWFlags & CBW_FLAGS_DIRECTION) {
	len = cmd_f(cbw->bCBWLUN, cmd, data_buf, cbw->dCBWDataTransferLength);
	if (len == cbw->dCBWDataTransferLength) {
	    err = cyg_io_write(bulk_endpointH, data_buf, &len);
	    csw->bCSWStatus = CSW_STATUS_PASS;
	    csw->dCSWDataResidue = 0;
	}
	else if (len < cbw->dCBWDataTransferLength) {
	    if (len >= 0) {
		/* transfer less data than requested */
		err = cyg_io_write(bulk_endpointH, data_buf, &len);
		/* stall bulk-in endpoint */
		d12_set_endpoint_status(5, 1);
		csw->bCSWStatus = CSW_STATUS_PASS;
	    }
	    else {
		diag_printf("3\n");
		len = 0; /* cmd failed, no data processed */
		err = cyg_io_write(bulk_endpointH, data_buf, &len);

		/* stall bulk-in endpoint */
		d12_set_endpoint_status(5, 1);
		csw->bCSWStatus = CSW_STATUS_FAIL;
	    }
	    csw->dCSWDataResidue = cbw->dCBWDataTransferLength - len; /* amount of pad or unsent data */
	}
	else {	/* len > cbw->dCBWDataTransferLength */
	    diag_printf("4\n");
	    d12_set_endpoint_status(5, 1); /* stall bulk-in endpoint */
	    csw->bCSWStatus = CSW_STATUS_PHASE_ERROR;
	}
    }
    else {
	diag_printf("5\n");
	d12_set_endpoint_status(5, 1); /* stall bulk-in endpoint */
	csw->bCSWStatus = CSW_STATUS_PHASE_ERROR;
    }
    return true;
}

bool
data_out(cbw_t * cbw, csw_t * csw, int (*cmd_f)(unsigned char, rbc_cmd_t *, char *, int))
{
    rbc_cmd_t * cmd = (rbc_cmd_t *)&cbw->CBWCB;
    int len;
    Cyg_ErrNo err;
    
    if (cbw->dCBWDataTransferLength > DATA_BUF_SIZE) {
	char * new_data_buf = 0;
	
	diag_printf("do tl %x\n", cbw->dCBWDataTransferLength);
	/* try to re-allocate data buffer to larger size */
	new_data_buf = (char *)realloc(data_buf, cbw->dCBWDataTransferLength);
	if (new_data_buf == 0) {
	    diag_printf("could not allocate necessary space\n");
	    diag_printf("ERROR STOPPING BULK THREAD\n");
	    for (;;)
		;
	}
	data_buf = new_data_buf;
    }

    /* this template should work for all data-out transfers */
    if (!(cbw->bmCBWFlags & CBW_FLAGS_DIRECTION)) {
	len = cbw->dCBWDataTransferLength; /* it's always acceptable to read all the data the host sends */
	err = cyg_io_read(bulk_endpointH, data_buf, &len);
	len = cmd_f(cbw->bCBWLUN, cmd, data_buf, cbw->dCBWDataTransferLength);
	if (len <= cbw->dCBWDataTransferLength) {
	    if (len < 0) {
		diag_printf("A\n");
		csw->bCSWStatus = CSW_STATUS_FAIL;
	    }
	    else {
		csw->bCSWStatus = CSW_STATUS_PASS;
		csw->dCSWDataResidue = cbw->dCBWDataTransferLength - len;
	    }
	}
	else {	/* needed more data than host sent */
	    diag_printf("B\n");
	    csw->bCSWStatus = CSW_STATUS_PHASE_ERROR;
	}
    }
    else {
	diag_printf("C\n");
	d12_set_endpoint_status(4, 1); /* stall bulk-out endpoint */
	csw->bCSWStatus = CSW_STATUS_PHASE_ERROR;
    }
    return true;
}

bool
rbc_handler(cbw_t * cbw, csw_t * csw)
{
    rbc_cmd_t * cmd = (rbc_cmd_t *)&cbw->CBWCB;

    if (cbw->reserved0 == 0 && cbw->reserved1 == 0) {
	diag_printf("LUN[%d] ", cbw->bCBWLUN);
	if (cbw->bCBWLUN < MAX_LUN) {
	    switch (cmd->operation_code) {
		
		case INQUIRY:
		    diag_printf("INQUIRY\n");
		    return data_in(cbw, csw, spc_inquiry);
		    break;
		    
		case READ:
		    diag_printf("READ\n");
		    return data_in(cbw, csw, spc_read);
		    break;
		    
		case READ_CAPACITY:
		    diag_printf("READ_CAPACITY\n");
		    return data_in(cbw, csw, spc_read_capacity);
		    break;
		    
		case REQUEST_SENSE:
		    diag_printf("REQUEST_SENSE\n");
		    return data_in(cbw, csw, spc_request_sense);
		    break;
		
		case TEST_UNIT_READY:
		case VERIFY:
		{
		    Cyg_ErrNo res;
		    cyg_uint32 len;
		    
		    /* verify does the same thing as test unit ready because the operation is supposed to verify
		     * the medium and since the mmc card takes care of that itself, we can fake verify with just
		     * checking whether the medium is present */
		    if (cmd->operation_code == TEST_UNIT_READY) {
			diag_printf("TEST_UNIT_READY\n");
		    }
		    else {
			diag_printf("VERIFY\n");
		    }
		    
		    if (sense_data.sense_key == UNIT_ATTENTION) {
			csw->bCSWStatus = CSW_STATUS_FAIL;
			csw->dCSWDataResidue = cbw->dCBWDataTransferLength;
		    }
		    else {
			len = sizeof(len);
			cyg_mutex_lock(&bulk_thread_safe_to_reset);
			res = cyg_io_get_config(blk_devH[ cbw->bCBWLUN ], IO_BLK_GET_CONFIG_MEDIA_STATUS, &len, &len);
			cyg_mutex_unlock(&bulk_thread_safe_to_reset);
			if (res == ENOERR) {
			    csw->bCSWStatus = CSW_STATUS_PASS;
			    csw->dCSWDataResidue = cbw->dCBWDataTransferLength;
			}
			else {
			    csw->bCSWStatus = CSW_STATUS_FAIL;
			    csw->dCSWDataResidue = cbw->dCBWDataTransferLength;
			    set_sense_data(OP_STATUS, res);
			    return true;
			}
		    }		
		    return true;
		    break;
		}
		
		case WRITE:
		    diag_printf("WRITE\n");
		    return data_out(cbw, csw, spc_write);
		    break;
		    
		case MODE_SENSE:
		    diag_printf("MODE_SENSE\n");
		    return data_in(cbw, csw, spc_mode_sense);
		    break;
		    
		case READ_FORMAT_CAPACITIES:
		    diag_printf("READ_FORMAT_CAPACITIES\n");
		    return data_in(cbw, csw, spc_read_format_capacities);
		    break;
		    
		default:
		    /* command not supported */
		    diag_printf("XXX UNSUPPORTED COMMAND %x XXX\n", cmd->operation_code);
		    if (sense_data.sense_key != UNIT_ATTENTION) {
			sense_data.sense_key = ILLEGAL_REQUEST;
			sense_data.asc = ASC(INVALID_COMMAND_OPERATION_CODE);
			sense_data.ascq = ASCQ(INVALID_COMMAND_OPERATION_CODE);
		    }
		    csw->bCSWStatus = CSW_STATUS_FAIL;
		    csw->dCSWDataResidue = cbw->dCBWDataTransferLength;
		    return true;
		    break;
	    }
	}
	else {
	    /* lun invalid */
	    diag_printf("invalid lun\n");
	    if (sense_data.sense_key != UNIT_ATTENTION) {
		sense_data.sense_key = ILLEGAL_REQUEST;
		sense_data.asc = ASC(LOGICAL_UNIT_NOT_SUPPORTED);
		sense_data.ascq = ASCQ(LOGICAL_UNIT_NOT_SUPPORTED);
	    }
	    csw->bCSWStatus = CSW_STATUS_FAIL;
	    csw->dCSWDataResidue = cbw->dCBWDataTransferLength;
	    return true;
	}
    }
    else {
	/* unmeaningful CBW */
	return false;
    }
    return false;
}

int
spc_inquiry(unsigned char lun, inquiry_cmd_t * cmd, char * buf, int len)
{
    inquiry_data_t * inquiry_data;
    supported_vital_product_data_pages_t * supported_pages;
    unit_serial_number_page_t * serial_number;
    drive_geometry_t dg;
    int i;
    Cyg_ErrNo res;

    /* TODO move this to a config file */
    char vid[8] = "I-Jam   ";
    char pid[16] = "WMA Player      ";
    char version[4] = "1.00";

    /* validate cmd */
    if (cmd->cmddt || /* unsupported */
	(!cmd->evpd && !cmd->cmddt && cmd->page_op_code) || /* both zero and op_code non-zero */
	(cmd->evpd && cmd->cmddt) /* both one */) {
	diag_printf("invalid inquiry request\n");
	/* TODO if there is a pending unit attention condition, then do not clear it...
	 * so is the following the correct semantics?  what should happen to this data? */
	if (sense_data.sense_key != UNIT_ATTENTION) {
	    sense_data.sense_key = ILLEGAL_REQUEST;
	    sense_data.asc = ASC(INVALID_FIELD_IN_CDB);
	    sense_data.ascq = ASCQ(INVALID_FIELD_IN_CDB);
	}
	return -1;
    }

    if (!cmd->evpd) {
	/* fill out standard inquiry data structure */
	/* TODO any changes needed for multiple LUNs? */
	if (len < sizeof(inquiry_data_t)) {
	    /* host did not request enough data */
	    return sizeof(inquiry_data_t);
	}
	inquiry_data = (inquiry_data_t * )buf;
	memset(inquiry_data, 0, sizeof(inquiry_data_t));
	inquiry_data->peripheral_device_type = PERIPHERAL_DEVICE_TYPE_DIRECT_ACCESS;
	inquiry_data->peripheral_qualifier = PERIPHERAL_QUALIFIER_CONNECTED;
	
	inquiry_data->rmb = RMB_REMOVABLE;
	
	inquiry_data->version = VERSION_NO_CONFORMANCE;
	
	inquiry_data->response_data_format = 0x01; /* obsolete, copied from iomega usb zip 100 */
	inquiry_data->hisup = HISUP_NOT_SUPPORTED; /* no support for hierarchical addressing to assign LUNs */
	inquiry_data->normaca = NORMACA_NOT_SUPPORTED; /* no support for setting NACA bit in control byte of the cdb */
	inquiry_data->aerc = AERC_NOT_SUPPORTED;	/* no support for asynchronous event reporting */
	
	inquiry_data->additional_length = sizeof(inquiry_data_t) - 4; /* no additional data */
	
	inquiry_data->sccs = SCCS_NOT_SUPPORTED; /* device does not contain a storage array container */
	
	inquiry_data->bque = CMD_QUEUEING_NOT_SUPPORTED; /* no support for tagged tasks per SAM-2 */
	inquiry_data->encserv = ENCSERV_NO_COMPONENT; /* device does not contain a enclosure services component */
	inquiry_data->multip = MULTIP_NOT_MULTIPORT; /* device is not a multi-port device */
	inquiry_data->mchngr = MCHNGR_NOT_IN_MEDIA_CHANGER;
	
	inquiry_data->reladr = 0; /* no support for relative addressing or linked commands */
	inquiry_data->linked = 0;
	inquiry_data->cmdque = CMD_QUEUEING_NOT_SUPPORTED; /* no support for tagged tasks per SAM-2 */
	
	for (i = 0; i < 8; ++i)
	    inquiry_data->vendor_identification[i] = vid[i];
	for (i = 0; i < 16; ++i)
	    inquiry_data->product_identification[i] = pid[i];
	for (i = 0; i < 4; ++i)
	    inquiry_data->product_revision_level[i] = version[i];

	return sizeof(inquiry_data_t);
    }
    else {
	/* vital product data page support */
	diag_printf("vital product data page support\n");

	/* TODO test data page support */
	switch (cmd->page_op_code) {
	    case SUPPORTED_VPD_PAGES:
		diag_printf("SUPPORTED_VPD_PAGES\n");
		if (len < sizeof(supported_vital_product_data_pages_t) + NUMBER_SUPPORTED_PAGES) {
		    /* host did not request enough data */
		    return sizeof(supported_vital_product_data_pages_t) + NUMBER_SUPPORTED_PAGES;
		}
		supported_pages = (supported_vital_product_data_pages_t *)buf;
		memset(supported_pages, 0, sizeof(supported_vital_product_data_pages_t) + NUMBER_SUPPORTED_PAGES);
		supported_pages->peripheral_device_type = PERIPHERAL_DEVICE_TYPE_DIRECT_ACCESS;
		supported_pages->peripheral_qualifier = PERIPHERAL_QUALIFIER_CONNECTED;
		supported_pages->page_code = SUPPORTED_VPD_PAGES;
		/* both pages implemented for all LUNs */
		supported_pages->page_length = NUMBER_SUPPORTED_PAGES;
		if (len - sizeof(supported_vital_product_data_pages_t) > 0)
		    supported_pages->supported_page_list[0] = SUPPORTED_VPD_PAGES;
		if (len - sizeof(supported_vital_product_data_pages_t) > 1)
		    supported_pages->supported_page_list[1] = UNIT_SERIAL_NUMBER_PAGE;

		return sizeof(supported_vital_product_data_pages_t) + NUMBER_SUPPORTED_PAGES;
		break;

	    case UNIT_SERIAL_NUMBER_PAGE:
		diag_printf("UNIT_SERIAL_NUMBER_PAGE\n");
		/* TODO sizeof(dg.serial_num) is now hardcoded to 16, which is the
		 * size of MMC CID, which is what WMDM expects the serial number to be */
		if (len < sizeof(unit_serial_number_page_t) + 16) {
		    /* host did not request enough data */
		    return sizeof(unit_serial_number_page_t) + 16;
		}
		serial_number = (unit_serial_number_page_t *)buf;
		memset(serial_number, 0, sizeof(unit_serial_number_page_t) + 16);
		serial_number->peripheral_device_type = PERIPHERAL_DEVICE_TYPE_DIRECT_ACCESS;
		serial_number->peripheral_qualifier = PERIPHERAL_QUALIFIER_CONNECTED;
		serial_number->page_code = UNIT_SERIAL_NUMBER_PAGE;
		serial_number->page_length = 16;
		cyg_mutex_lock(&bulk_thread_safe_to_reset);
		i = sizeof(dg);
		res = cyg_io_get_config(blk_devH[lun], IO_BLK_GET_CONFIG_GEOMETRY, &dg, &i);
		cyg_mutex_unlock(&bulk_thread_safe_to_reset);
		if (res == ENOERR) {
		    for (i = 0; i < 16 && i < (len - sizeof(unit_serial_number_page_t)); ++i) {
			serial_number->product_serial_number[i] = dg.serial_num[i];
		    }
		}
		else {
		    /* fill serial number with spaces */
		    for (i = 0; i < 16 && i < (len - sizeof(unit_serial_number_page_t)); ++i) {
			serial_number->product_serial_number[i] = ' ';
		    }
		}

		i = (16 < (len - sizeof(unit_serial_number_page_t)) ? 16 :
		     (len - sizeof(unit_serial_number_page_t)));
		return sizeof(unit_serial_number_page_t) + i;
		break;
	}
    }
    return -1;			/* make compiler happy */
}

int
spc_request_sense(unsigned char lun, request_sense_cmd_t * cmd, char * buf, int len)
{
    request_sense_data_t * sense = (request_sense_data_t *)buf;
    
    /* check that buf is big enough, if not return number of bytes desired */
    if (len < sizeof(request_sense_data_t)) {
	return sizeof(request_sense_data_t);
    }
    
    /* fill out request sense data */
    memset(sense, 0, sizeof(request_sense_data_t));
    sense->response_code = CURRENT_ERRORS;
    sense->sense_key = sense_data.sense_key;
    sense->additional_length = sizeof(request_sense_data_t) - 7; /* no additional data */
    sense->asc = sense_data.asc;
    sense->ascq = sense_data.ascq;

    /* clear sense data */
    sense_data.sense_key = NO_SENSE;
    sense_data.asc = ASC(NO_ADDITIONAL_SENSE_INFORMATION);
    sense_data.ascq = ASCQ(NO_ADDITIONAL_SENSE_INFORMATION);
    
    return sizeof(request_sense_data_t);
}

int
spc_mode_sense(unsigned char lun, mode_sense_cmd_t * cmd, char * buf, int len)
{
    mode_sense_header_t * hdr;
    mode_sense_da_block_descriptor_t * blk_desc;
    rw_error_recovery_page_t * error_pg;
    flexible_disk_page_t * disk_pg;
    caching_page_t * cache_pg;
    vendor_specific_page_t * vendor_pg;
    drive_geometry_t dg;	/* for flexible disk page fields */
    cyg_uint32 tmp_len;
    Cyg_ErrNo res;
    
    /* this is to tell if this command ever gets executed when no disk present */
    diag_printf("spc_mode_sense %x\n", cmd->page_code);
    
    if (sense_data.sense_key == UNIT_ATTENTION) {
	return -1;
    }
    
    memset(buf, 0, len);
    switch (cmd->page_code) {
	case 0x00:		/* vendor-specific, does not require a page format */
	    hdr = (mode_sense_header_t *)buf;
	    hdr->mode_data_length = sizeof(mode_sense_header_t) +
		sizeof(mode_sense_da_block_descriptor_t) - 1;
	    if (len < hdr->mode_data_length + 1) {
		/* not enough bytes in len */
		return hdr->mode_data_length + 1;
	    }
	    hdr->medium_type = PERIPHERAL_DEVICE_TYPE_DIRECT_ACCESS;
	    hdr->block_descriptor_length = sizeof(mode_sense_da_block_descriptor_t);
	    blk_desc = (mode_sense_da_block_descriptor_t *)((char *)hdr + sizeof(mode_sense_header_t));
	    blk_desc->block_length = SWAP_16(BLOCK_SIZE);
	    return (hdr->mode_data_length + 1);
	    break;

	case 0x3f:		/* return all mode pages */
	    /* TODO figure out what place this occupies in the protocol */
	    /* page 0x01, read-write error recovery mode page, and
	     * page 0x08, caching mode, and
	     * page 0x2f, vendor specific
	     * don't have correct meaning according to sbc doc from the zip sniff
	     * so return only the flexible disk page
	     */
	    hdr = (mode_sense_header_t *)buf;
	    hdr->mode_data_length = sizeof(mode_sense_header_t) +
		sizeof(mode_sense_da_block_descriptor_t) +
		sizeof(rw_error_recovery_page_t) +
		sizeof(flexible_disk_page_t) +
		sizeof(caching_page_t) +
		sizeof(vendor_specific_page_t) - 1;
	    if (len < hdr->mode_data_length + 1) {
		return hdr->mode_data_length + 1;
	    }
	    hdr->medium_type = PERIPHERAL_DEVICE_TYPE_DIRECT_ACCESS;
	    hdr->block_descriptor_length = sizeof(mode_sense_da_block_descriptor_t);
	    
	    blk_desc = (mode_sense_da_block_descriptor_t *)((char *)hdr + sizeof(mode_sense_header_t));
	    blk_desc->block_length = SWAP_16(BLOCK_SIZE);

	    error_pg = (rw_error_recovery_page_t *)((char *)blk_desc + sizeof(mode_sense_da_block_descriptor_t));
	    error_pg->page_code = 0x01;
	    error_pg->page_length = sizeof(rw_error_recovery_page_t) - 2;
	    error_pg->eer = 0x01;
	    error_pg->arre = 0x01;
	    error_pg->awre = 0x01;
	    error_pg->read_retry_count = 0x16;
	    
	    /* get disk geometry for flexible disk page */
	    /* TODO add some error checking, although this command doesn't get called
	     * when no card is inserted */
	    tmp_len = sizeof(dg);
	    cyg_mutex_lock(&bulk_thread_safe_to_reset);
	    res = cyg_io_get_config(blk_devH[lun], IO_BLK_GET_CONFIG_GEOMETRY, &dg, &tmp_len);
	    cyg_mutex_unlock(&bulk_thread_safe_to_reset);
	    /* fill out flexible disk page */
	    disk_pg = (flexible_disk_page_t *)((char *)error_pg + sizeof(rw_error_recovery_page_t));
	    disk_pg->page_code = 0x05;
	    disk_pg->page_length = sizeof(flexible_disk_page_t) - 2;
	    disk_pg->transfer_rate = SWAP_16(0x8000); /* TODO this value copied from zip 100, 32kB/s */
	    disk_pg->number_of_heads = dg.hd;
	    disk_pg->sectors_per_track = dg.sec;
	    disk_pg->data_bytes_per_sector = SWAP_16(BLOCK_SIZE);
	    disk_pg->number_of_cylinders = SWAP_16(dg.cyl);
	    disk_pg->medium_rotation_rate = SWAP_16(0x0b7d); /* TODO copied from zip 100, 2941 rpm */

	    cache_pg = (caching_page_t *)((char *)disk_pg + sizeof(flexible_disk_page_t));
	    cache_pg->page_code = 0x08;
	    cache_pg->page_length = sizeof(caching_page_t) - 2;
	    cache_pg->wce = 0x01;
	    cache_pg->disable_pre_fetch_transfer_length = SWAP_16(0xffff);
	    cache_pg->minimum_pre_fetch = SWAP_16(0x0000);
	    cache_pg->maximum_pre_fetch = SWAP_16(0xffff);
	    cache_pg->maximum_pre_fetch_ceiling = SWAP_16(0xffff);

	    vendor_pg = (vendor_specific_page_t *)((char *)cache_pg + sizeof(caching_page_t));
	    vendor_pg->page_code = 0x2f;
	    vendor_pg->page_length = sizeof(vendor_specific_page_t) - 2;
	    vendor_pg->offtr = 0x01;
	    vendor_pg->ridi = 0x01;
	    vendor_pg->ftme = 0x01;
	    vendor_pg->scrub = 0x01;
	    vendor_pg->spin_down_timer = 0x0f;
	    vendor_pg->head_park_timer = 0xff;
	    vendor_pg->seek_retry_count = 0x0f;
	    
	    return (hdr->mode_data_length + 1);
	    break;
	    
	default:
	    sense_data.sense_key = ILLEGAL_REQUEST;
	    sense_data.asc = ASC(INVALID_FIELD_IN_CDB);
	    sense_data.ascq = ASCQ(INVALID_FIELD_IN_CDB);
	    return -1;
	    break;
    }
}	    

int
spc_read_capacity(unsigned char lun, read_capacity_cmd_t * cmd, char * buf, int len)
{
    read_capacity_data_t * data = (read_capacity_data_t *)buf;
    drive_geometry_t dg;
    cyg_uint32 tmp_len;
    Cyg_ErrNo res;
    
    if (sense_data.sense_key == UNIT_ATTENTION) {
	return -1;
    }

    tmp_len = sizeof(dg);
    cyg_mutex_lock(&bulk_thread_safe_to_reset);
    res = cyg_io_get_config(blk_devH[lun], IO_BLK_GET_CONFIG_GEOMETRY, &dg, &tmp_len);
    cyg_mutex_unlock(&bulk_thread_safe_to_reset);
    if (res == ENOERR) {
	if (len < sizeof(read_capacity_data_t)) {
	    /* buf is too small */
	    return sizeof(read_capacity_data_t);
	}
	memset(buf, 0, len);
	data->lba = SWAP_32(dg.num_blks - 1);
	/* TODO bytes_p_sec broken in sandisk code */
	dg.bytes_p_sec = BLOCK_SIZE;
	data->blk_len = SWAP_32(dg.bytes_p_sec);
	return sizeof(read_capacity_data_t);
    }
    /* failed to get drive geometry */
    else {
	set_sense_data(OP_GEOMETRY, res);
    }
    return -1;
}

int
spc_read_format_capacities(unsigned char lun, read_format_capacities_cmd_t * cmd, char * buf, int len)
{
    capacity_list_header_t * hdr;
    current_capacity_t * current;
    formattable_capacity_t * formattable;
    drive_geometry_t dg;
    cyg_uint32 tmp_len;
    Cyg_ErrNo err;

    if (sense_data.sense_key == UNIT_ATTENTION) {
	return -1;
    }

    /* determine if disk/card is inserted and get some useful info for later */
    tmp_len = sizeof(dg);
    cyg_mutex_lock(&bulk_thread_safe_to_reset);
    err = cyg_io_get_config(blk_devH[lun], IO_BLK_GET_CONFIG_GEOMETRY, &dg, &tmp_len);
    cyg_mutex_unlock(&bulk_thread_safe_to_reset);
    
    memset(buf, 0, len);
    hdr = (capacity_list_header_t *)buf;
    if (!err) {
	hdr->list_length = sizeof(current_capacity_t) + sizeof(formattable_capacity_t);
    }
    else {
	hdr->list_length = sizeof(current_capacity_t);
    }

    /* see if buf is big enough */
    if (len < sizeof(capacity_list_header_t) + hdr->list_length) {
	return sizeof(capacity_list_header_t) + hdr->list_length;
    }
    
    current = (current_capacity_t *)((char *)hdr + sizeof(capacity_list_header_t));
    if (!err) {
	current->number_of_blocks = SWAP_32(dg.num_blks);
	current->descriptor_code = FORMATTED_MEDIA;
	current->block_length = SWAP_16(BLOCK_SIZE);
    }
    else {
	/* TODO this is the maximum formattable capacity */
	/* assume 128MB for now, check later whether it's ok to just return failure on this command */
	current->number_of_blocks = SWAP_32(0x40000);
	current->descriptor_code = NO_MEDIA_PRESENT;
	current->block_length = SWAP_16(BLOCK_SIZE);
    }

    if (!err) {
	formattable = (formattable_capacity_t *)((char *)current + sizeof(current_capacity_t));
	formattable->format_type = 0x00; /* param is block length in bytes */
	formattable->number_of_blocks = SWAP_32(dg.num_blks);
	formattable->param = SWAP_16(BLOCK_SIZE);
    }

    return (sizeof(capacity_list_header_t) + hdr->list_length);
}    

int
spc_read(unsigned char lun, read_cmd_t * cmd, char * buf, int len)
{
    blk_request_t blk_req = {SWAP_32(cmd->lba),
			     (cmd->transfer_length == 0) ? SWAP_16(256) : SWAP_16(cmd->transfer_length),
			     buf};
    cyg_uint32 bytes_wanted, n;
    Cyg_ErrNo res;

    if (sense_data.sense_key == UNIT_ATTENTION) {
	return -1;
    }
    
    bytes_wanted = SWAP_16(cmd->transfer_length) * BLOCK_SIZE;
    if (bytes_wanted > len) {
	return bytes_wanted;
    }
    else {
	n = 1;		/* one block request */
	cyg_mutex_lock(&bulk_thread_safe_to_reset);
	res = cyg_io_read(blk_devH[lun], &blk_req, &n);
	cyg_mutex_unlock(&bulk_thread_safe_to_reset);
	if (res == ENOERR) {
	    return blk_req.num_blks * BLOCK_SIZE;
	}
	/* command failed */
	else {
	    set_sense_data(OP_READING, res);
	}
	return -1;
    }
}

int
spc_write(unsigned char lun, write_cmd_t * cmd, char * buf, int len)
{
    blk_request_t blk_req = {SWAP_32(cmd->lba),
			     (cmd->transfer_length == 0) ? SWAP_16(256) : SWAP_16(cmd->transfer_length),
			     buf};
    cyg_uint32 bytes_needed, n;
    Cyg_ErrNo res;
    
    if (sense_data.sense_key == UNIT_ATTENTION) {
	return -1;
    }
    
    bytes_needed = SWAP_16(cmd->transfer_length) * BLOCK_SIZE;
    if (len < bytes_needed) {
	return bytes_needed;
    }
    else {
	n = 1;
	cyg_mutex_lock(&bulk_thread_safe_to_reset);
	res = cyg_io_write(blk_devH[lun], &blk_req, &n);
	cyg_mutex_unlock(&bulk_thread_safe_to_reset);
	if (res == ENOERR) {
	    return blk_req.num_blks * BLOCK_SIZE;
	}
	/* LBA could be out of range as an error */
	else {
	    set_sense_data(OP_WRITING, res);
	}
	return -1;
    }
}
