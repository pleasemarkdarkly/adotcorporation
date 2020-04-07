#include <cyg/infra/diag.h>
#include <dadio/io/ata.h>
#include <dadio/io/edk.h>
#include <dadio/io/edk_hw.h>

#ifdef DDOIMP_IO_ATAPI_USE_ATA

/* check signature return values */
#define NOT_PACKET_DEVICE 0
#define PACKET_DEVICE     1

/* internal function prototypes */
extern BYTE PollStatus(BYTE clear_bits, BYTE set_bits);                       
static unsigned char send_ata_cmd(ata_task_file_t * task_file);
static unsigned char ata_process_interrupt(ata_task_file_t * task_file);
static unsigned char check_signature(void);
static unsigned int get_lba_last_error(void);

/* state */
/* TODO make this thread safe */
unsigned int lba_last_error;

/* exported functions */

unsigned char
init_ata(void)
{
    ata_task_file_t task_file;
    unsigned char ata_err;
    unsigned char error;
    
    memset(&task_file, 0, sizeof(task_file));
    task_file.device_head = SELECT_MASTER;
    task_file.cmd = ATA_EXECUTE_DEVICE_DIAGNOSTIC;
    ata_err = send_ata_cmd(&task_file);
    if (ata_err == NO_ERROR) {
	if (check_signature() == NOT_PACKET_DEVICE) {
	    diag_printf("Device does not support packet interface\n");
	}
	else {
	    diag_printf("Device supports packet interface\n");
	}
	return NO_ERROR;
    }
    else if (ata_err == !NO_ERROR) {
	/* error recovery */
	ReadError(error);
	diag_printf("Execute device dianostic error, 0x%x\n", error);
	return ERROR_DIAGNOSTIC_FAILURE;
    }
    else {
	return ata_err;
    }
}

unsigned char
ata_identify(ata_identify_device_t * data)
{
    ata_task_file_t task_file;
    unsigned char ata_err;
    unsigned char error;
    
    memset(&task_file, 0, sizeof(task_file));
    task_file.data = (short *)data;
    task_file.data_len = 256;	/* number of words */
    task_file.device_head = SELECT_MASTER;
    task_file.cmd = ATA_IDENTIFY_DEVICE;
    ata_err = send_ata_cmd(&task_file);
    if (ata_err == NO_ERROR) {
	return NO_ERROR;
    }
    else if (ata_err == !NO_ERROR) {
	ReadError(error);
	if (error & ERROR_ABRT) {
	    if (check_signature() == PACKET_DEVICE) {
		/* TODO issue ATA_IDENTIFY_PACKET_DEVICE command ?? */
		return ERROR_INVALID_COMMAND;
	    }
	    else {
		diag_printf("Unknown identify error, device should give packet signature\n");
		return !NO_ERROR;
	    }
	}
	else {
	    diag_printf("Unknown identify error, should not have occurred\n");
	    return !NO_ERROR;
	}
    }
    else {
	return ata_err;
    }
}
    
unsigned char
write_ata(unsigned int lba, unsigned short length, unsigned char * data)
{
    ata_task_file_t task_file;
    unsigned char ata_err;
    unsigned char error;
    
    memset(&task_file, 0, sizeof(task_file));
    task_file.data = (short *)data;
    task_file.data_len = length * 256;

    if (length == 256) {
	task_file.sec_cnt = 0;
    }
    else {
	task_file.sec_cnt = length;
    }
    task_file.sec_num = lba & 0xff;
    task_file.cyl_low = (lba >>= 8) & 0xff;
    task_file.cyl_hi = (lba >>= 8) & 0xff;
    task_file.device_head = SELECT_MASTER | SELECT_LBA | (lba & 0xf);
    task_file.cmd = ATA_WRITE_SECTORS;
    ata_err = send_ata_cmd(&task_file);
    if (ata_err == NO_ERROR) {
	return NO_ERROR;
    }
    else if (ata_err == !NO_ERROR) {
	lba_last_error = get_lba_last_error();
	ReadError(error);
	if (error & ERROR_WP) {
	    diag_printf("Media is write protecteunsigned char
d\n");
	    return ERROR_WRITE_PROTECTED;
	}
	if (error & ERROR_MC) {
	    diag_printf("Media has changed\n");
	    return ERROR_MEDIA_CHANGED;
	}
	if (error & ERROR_IDNF) {
	    diag_printf("Address out of range\n");
	    return ERROR_LBA_OUT_OF_RANGE;
	}
	if (error & ERROR_MCR) {
	    diag_printf("Media change request detected\n");
	    return !NO_ERROR;	/* TODO determine appropriate error code */
	}
	if (error & ERROR_ABRT) {
	    /* could be either command not supported or address out of range if IDNF is not set */
	    diag_printf("Command not supported\n");
	    return ERROR_INVALID_COMMAND;
	}
	if (error & ERROR_NM) {
	    diag_printf("No media present\n");
	    return ERROR_NO_MEDIA;
	}
    }
    else {
	return ata_err;
    }
}

unsigned char
read_ata(unsigned int lba, unsigned short length, unsigned char * data)
{
    ata_task_file_t task_file;
    unsigned char ata_err;
    unsigned char error;
        
    memset(&task_file, 0, sizeof(task_file));
    task_file.data = (short *)data;
    task_file.data_len = length * 256;

    if (length == 256) {
	task_file.sec_cnt = 0;
    }
    else {
	task_file.sec_cnt = length;
    }
    task_file.sec_num = lba & 0xff;
    task_file.cyl_low = (lba >>= 8) & 0xff;
    task_file.cyl_hi = (lba >>= 8) & 0xff;
    task_file.device_head = SELECT_MASTER | SELECT_LBA | (lba & 0xf);
    task_file.cmd = ATA_READ_SECTORS;
    ata_err = send_ata_cmd(&task_file);
    if (ata_err == NO_ERROR) {
	return NO_ERROR;
    }
    else if (ata_err == !NO_ERROR) {
	lba_last_error = get_lba_last_error();
	ReadError(error);
	if (error & ERROR_ABRT) {
	    if (check_signature() == PACKET_DEVICE) {
		diag_printf("Device is packet device\n");
	    }
	    else {
		diag_printf("Command not supported\n");
	    }
	    return ERROR_INVALID_COMMAND;
	}
	if (error & ERROR_UNC) {
	    diag_printf("Data is uncorrectable\n");
	}
	if (error & ERROR_MC) {
	    diag_printf("Media has changed\n");
	    return ERROR_MEDIA_CHANGED;
	}
	if (error & ERROR_IDNF) {
	    diag_printf("Address out of bounds\n");
	    return ERROR_LBA_OUT_OF_RANGE;
	}
	if (error & ERROR_MCR) {
	    diag_printf("Media change request detected\n");
	    return !NO_ERROR;	/* TODO pick better code */
	}
	if (error & ERROR_NM) {
	    diag_printf("No media present\n");
	    return ERROR_NO_MEDIA;
	}
    }
    else {
	return ata_err;
    }
}

unsigned char
ata_get_geometry(drive_geometry_t * dg)
{
    ata_identify_device_t identify_info;
    unsigned char ata_err;
    int i;
    
    ata_err = ata_identify(&identify_info);
    if (ata_err != NO_ERROR) {
	return ata_err;
    }
    dg->cyl = identify_info.num_cyl;
    dg->hd = identify_info.num_hds;
    dg->sec = identify_info.num_sec_trk;
    dg->bytes_p_sec = 0x200;
    if (identify_info.capabilities & LBA_SUPPORT) {
	dg->num_blks = identify_info.total_lba_sec;
    }
    for (i = 0; i < 20; ++i) {
	dg->serial_num[i] = identify_info.serial_num[i];
	dg->model_num[i] = identify_info.model_num[i];
    }
    for (; i < 40; ++i) {
	dg->model_num[i] = identify_info.model_num[i];
    }
    return ata_err;
}

unsigned char
ata_check_power_mode(unsigned char * data)
{
    ata_task_file_t task_file;
    unsigned char ata_err;
    unsigned char error;
    unsigned char sec_cnt;
    
    memset(&task_file, 0, sizeof(task_file));
    task_file.device_head = SELECT_MASTER;
    task_file.cmd = ATA_CHECK_POWER_MODE;
    ata_err = send_ata_cmd(&task_file);
    if (ata_err == NO_ERROR) {
	ReadSectorCount(sec_cnt);
	*data = sec_cnt;
	return NO_ERROR;
    }
    else if (ata_err == !NO_ERROR) {
	lba_last_error = get_lba_last_error();
	ReadError(error);
	if (error & ERROR_ABRT) {
	    diag_printf("Command not supported\n");
	    return ERROR_INVALID_COMMAND;
	}
	else {
	    diag_printf("Unknown error, should not have occcured\n");
	    return !NO_ERROR;
	}
    }
    else {
	return ata_err;
    }
}

/* internal functions */

static unsigned char
check_signature(void)
{
    unsigned char sec_cnt;
    unsigned char sec_num;
    unsigned char cyl_low;
    unsigned char cyl_hi;
    unsigned char dev_hd;

    /* read the signature */
    ReadSectorCount(sec_cnt);
    ReadSectorNumber(sec_num);
    ReadCylinderLow(cyl_low);
    ReadCylinderHigh(cyl_hi);
    ReadDeviceHead(dev_hd);
    if (sec_cnt == 0x01 && sec_num == 0x01 && cyl_low == 0x00 &&
	cyl_hi == 0x00 && dev_hd == 0x00) {
	return NOT_PACKET_DEVICE;
    }
    else if (sec_cnt == 0x01 && sec_num == 0x01 && cyl_low == 0x14 &&
	     cyl_hi == 0xeb && ((dev_hd & 0xef) == 0x00)) {
	return PACKET_DEVICE;
    }
}

static unsigned int
get_lba_last_error(void)
{
    unsigned char sec_num;
    unsigned char cyl_low;
    unsigned char cyl_hi;
    unsigned char dev_hd;
    unsigned int lba;

    ReadSectorNumber(sec_num);
    ReadCylinderLow(cyl_low);
    ReadCylinderHigh(cyl_hi);
    ReadDeviceHead(dev_hd);
    lba = sec_num | (cyl_low << 8) | (cyl_hi << 16) | ((dev_hd & 0xf) << 24);
    return lba;
}

static unsigned char
send_ata_cmd(ata_task_file_t * task_file)
{
    if (sleep_flag)
	return ERROR_SLEEP;
    
    /* check for media present before attempting to access task file */
    if (!MediaPresent()) {
	diag_printf("No media present\n");
	return ERROR_NO_MEDIA;
    }

    /* bus idle protocol */
  bi_check_status:
	//diag_printf("send_ata_cmd\n");
    if (PollStatus(STATUS_BSY | STATUS_DRQ, 0)) {
	return ERROR_TIMEOUT;
    }
    if (task_file->device_head & 0x10) {
	/* wrong device (slave) selected */
	/* TODO add support for master and slave configurations */
	goto bi_device_select;
    }
    goto bi_write_parameters;

  bi_device_select:
    task_file->device_head &= ~0x10;
#ifndef IO_ATA_16BIT_ACCESS
    /* can't write to this register in 16bit access mode, since it will also write to
     * the command register */
    WriteDeviceHead(task_file->device_head);
#endif /* !IO_ATA_16BIT_ACCESS */
    goto bi_check_status;

  bi_write_parameters:
#ifdef IO_ATA_16BIT_ACCESS
    /* need to write parameters and command at the same time, since each write will
     * write to two registers at the same time */
    WriteFeatures(task_file->features);
    WriteSector16(task_file->sec_cnt, task_file->sec_num);
    WriteCylinder16(task_file->cyl_low, task_file->cyl_hi);
#else
    WriteFeatures(task_file->features);
    WriteSectorCount(task_file->sec_cnt);
    WriteSectorNumber(task_file->sec_num);
    WriteCylinderLow(task_file->cyl_low);
    WriteCylinderHigh(task_file->cyl_hi);
    WriteDeviceHead(task_file->device_head);
#endif /* IO_ATA_16BIT_ACCESS */
    goto bi_write_command;

  bi_write_command:
#ifdef IO_ATA_16BIT_ACCESS
    WriteDeviceCmd16(task_file->device_head, task_file->cmd);
#else
    WriteCommand(task_file->cmd);    
#endif /* IO_ATA_16BIT_ACCESS */
    return ata_process_interrupt(task_file);
}

static unsigned char
ata_process_interrupt(ata_task_file_t * task_file)
{
    unsigned char status;
    unsigned char error;
    int len;

    switch (task_file->cmd) {
	/* non-data command protocol */
	case ATA_CHECK_POWER_MODE:
	{
	  nd_intrq_wait:
	    /* TODO wait for INTRQ to assert */
	    goto nd_check_status;

	  nd_check_status:
		//diag_printf("nd_check_status\n");
	    if (PollStatus(STATUS_BSY, 0)) {
		return ERROR_TIMEOUT;
	    }
	    ReadStatus(status);
	    if (status & STATUS_CHECK) {
		return !NO_ERROR;
	    }
	    return NO_ERROR;
	}

	/* pio data-in command protocol */
	case ATA_IDENTIFY_DEVICE:
	case ATA_READ_SECTORS:
	{
	  di_intrq_wait:
	    /* this state only entered when nIEN = 0, which is never for polled operation */
	    /* TODO wait for INTRQ to assert */
	    goto di_check_status;

	  di_check_status:
		//diag_printf("di_check_status\n");
	    if (PollStatus(STATUS_BSY, 0)) {
		return ERROR_TIMEOUT;
	    }
	    ReadStatus(status);
	    if (status & STATUS_DRQ) {
		goto di_transfer_data;
	    }
	    /* error occured */
	    return !NO_ERROR;
	    
	  di_transfer_data:
	    len = task_file->data_len > 256 ? 256 : task_file->data_len;
	    ReadDataBulk(task_file->data, len);
	    task_file->data += len;
	    task_file->data_len -= len;
	    if (task_file->data_len) {
		goto di_check_status;
	    }
	    /* optional read for status register */
	    return NO_ERROR;
	}

	/* pio data-out command protocol */
	case ATA_WRITE_SECTORS:
	{	    
	  do_intrq_wait:
	    /* this state only entered when nIEN = 0, which is never for polled operation */
	    /* TODO wait for INTRQ to assert */
	    goto do_check_status;
	    
	  do_check_status:
		//diag_printf("do_check_status\n");
		if (PollStatus(STATUS_BSY, 0)) {
		return ERROR_TIMEOUT;
	    }
	    ReadStatus(status);
	    if (status & STATUS_DRQ) {
		goto do_transfer_data;
	    }
	    if (status & STATUS_CHECK) {
		/* error occured */
		return !NO_ERROR;
	    }
	    return NO_ERROR;

	  do_transfer_data:
	    len = task_file->data_len > 256 ? 256 : task_file->data_len;
	    WriteDataBulk(task_file->data, len);
	    task_file->data += len;
	    task_file->data_len -= len;
	    /* TODO if interrupt driven, goto intrq_wait instead of check_status */
	    goto do_check_status;
	}   

	/* dma command protocol */
	/* TODO not implemented */

	/* packet command protocol */
	/* handled by ATAPI commands */

	/* dma queued command protocol */
	/* TODO not implemented */

	/* execute device diagnostic command protocol */
	case ATA_EXECUTE_DEVICE_DIAGNOSTIC:
	{
	  edd_wait:
	    /* TODO wait 2 ms */
	    goto edd_check_status;
	    
	  edd_intrq_wait:
	    /* TODO wait for INTRQ to assert */
	    goto edd_check_status;

	  edd_check_status:
		//diag_printf("edd_check_status\n");
		if (PollStatus(STATUS_BSY, 0)) {
		return ERROR_TIMEOUT;
	    }
	    
	    /* read the diagnostic code */
	    ReadError(error);
	    if (error == NO_ERROR_DETECTED) {
		return NO_ERROR;
	    }
	    else {
		return !NO_ERROR;
	    }
	}

#if 0
	/* device reset command protocol */
	case ATA_DEVICE_RESET:
	{
	  dr_wait:
	    /* TODO wait 400ns */
	    goto dr_check_status;

	  dr_check_status:
		//diag_printf("dr_check_status\n");
	    if (PollStatus(STATUS_BSY, 0)) {
		return ERROR_TIMEOUT;
	    }
	    ReadStatus(status);
	    if (status & STATUS_CHECK) {
		return !NO_ERROR;
	    }
	    return NO_ERROR;
	}
#endif
    }
}

#endif /* DDOIMP_IO_ATAPI_USE_ATA */
