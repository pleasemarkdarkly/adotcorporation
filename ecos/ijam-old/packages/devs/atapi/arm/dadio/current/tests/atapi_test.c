/* THIS SOFTWARE IS PROVIDED "AS IS" WITH NO WARRANTIES EXPRESSED OR IMPLIED,
   INCLUDING WITHOUT LIMITATION WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
   PARTICULAR PURPOSE OR NON-INFRINGEMENT. LICENSEE BEARS ALL RISK OF 
   NON-PERFORMANCE, LOSS OF DATA, AND OTHER PROBLEMS AND LICENSOR WILL NOT BE
   LIABLE UNDER ANY CONTRACT, NEGLIGENCE, STRICT LIABILITY OR OTHER THEORY
   FOR ANY DAMAGES INCLUDING, WITHOUT LIMITATION, DIRECT, INCIDENTAL OR 
   CONSEQUENTIAL DAMAGES OR COST OF PROCUREMENT OF SUBSTITUTE GOODS, SERVICES
   OR TECHNOLOGY */

#include <stdlib.h>   /* rand, RAND_MAX*/
#include <malloc.h>   /* malloc */
#include <string.h>   /* memset */

#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <dadio/io/blk_dev.h>
#include <dadio/io/edk.h>

/* DEFINES */

#define NTHREADS 1
#define STACKSIZE ( (8 * 4096) )

/* STATICS */

static cyg_handle_t thread[NTHREADS];
static cyg_thread thread_obj[NTHREADS];
static char _stack[NTHREADS][STACKSIZE];

#define MAX_SECTORS 128
#define MAX_LBA 0x13000

void atapi_thread(unsigned int);

void
cyg_user_start(void)
{
    cyg_thread_create(10, atapi_thread, (cyg_addrword_t) 0, "atapi_thread",
		      (void *)_stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);
}
                 
/* Globals */
cyg_uint8 packet[12];

void CompareData(cyg_uint8 * read_buffer, cyg_uint8 * write_buffer, cyg_uint16 length);
cyg_uint8 MultiProcess(void);
cyg_uint8 WaitForCartridge(void);

void
atapi_thread(unsigned int ignored)
{
    //static cyg_uint8 read_buffer[MAX_SECTORS * 512], write_buffer[MAX_SECTORS * 512];
    cyg_uint8 * read_buffer, * write_buffer;
    cyg_uint16 length;
    cyg_uint32 lba;
    unsigned int i;
    cyg_uint32 len;
    Cyg_ErrNo err;
    int skip;
    
    cyg_io_handle_t blk_devH;
    blk_request_t blk_req;
    
    srand(0x5834971);
    
    /* Initialize data buffers */
    read_buffer  = malloc(MAX_SECTORS * 512);  
    write_buffer = malloc(MAX_SECTORS * 512);
    diag_printf("read_buffer %x\nwrite_buffer %x\n", read_buffer, write_buffer);
    
    /* Initialize Write/Read Buffers */
    diag_printf("initializing write/read buffers\n");
    for (i=0; i < (MAX_SECTORS * 512); i++) {
	write_buffer[i] = (cyg_uint8) (i&0xff);
    }
    memset(read_buffer,0,MAX_SECTORS*512);
    diag_printf("done initializing\n");

    if (cyg_io_lookup("/dev/hda", &blk_devH) != ENOERR) {
	CYG_TEST_EXIT("Could not get handle to /dev/hda");
    }
    diag_printf("got handle to /dev/hda\n");
    len = sizeof(len);
    cyg_io_set_config(blk_devH, IO_ATAPI_SET_CONFIG_OPEN, &len, &len);

#if 0
    /* TODO */
    for (;;) {
	static int media_present = 0;
	static int old_media_present = 0;
	static cyg_uint32 len = sizeof(media_present);
	if (cyg_io_get_config(blk_devH, IO_ATAPI_GET_CONFIG_MEDIA_PRESENT, &media_present, &len) == ENOERR) {
	    if (media_present != old_media_present) {
		old_media_present = media_present;
		if (media_present == 0) {
		    diag_printf("No media present\n");
		}
		else {
		    diag_printf("Media present\n");
		}
	    }
	}
    }
#endif
    
    /* perform write/read tests */	
    diag_printf("Test write/read\n");
    i = 'y';
    
    if (i == 'y' || i == 'Y')
    {
	//diag_printf("\nPreforming 10 random write read compares");

	skip = 1;
	for(;;)
	{
	    lba = rand() / (RAND_MAX/MAX_LBA);
	    length = (cyg_uint16)(((cyg_uint32)rand()) / (RAND_MAX/MAX_SECTORS));
	    length += 1; // do at least one sector
	    if (length > MAX_SECTORS) {
		length = MAX_SECTORS;
	    }
	    diag_printf("LBA: %06x Sectors: %04x\n", lba, length);
#if 0
	    if (skip && (lba != 0x9de2 || length != 0x67)) {
		continue;
	    }
	    else {
		skip = 0;
	    }
#endif
	    blk_req.lba = lba;
	    blk_req.num_blks = length;
	    blk_req.buf = write_buffer;
	    if ((err = cyg_io_write(blk_devH, &blk_req, &blk_req.num_blks)) != ENOERR) {
		diag_printf("Error writing %d\n", err);
		continue;
	    }
	    diag_printf("w");
            
            memset(read_buffer, 0, 512*MAX_SECTORS);

	    blk_req.lba = lba;
	    blk_req.num_blks = length;
	    blk_req.buf = read_buffer;
	    if ((err = cyg_io_read(blk_devH, &blk_req, &blk_req.num_blks)) != ENOERR) {
		diag_printf("Error reading %d\n", err);
		continue;
	    }
            diag_printf("r");
            
            CompareData(read_buffer, write_buffer, length);
	    diag_printf("\n");
	}  
    }
    diag_printf("\nrandom write read compares done\n");
            
#if 0
    /* Test prevent and allow */
    diag_printf("\nTest prevent/allow? ");
    //	i = _getch();
    i = 'y';
    if (i =='y' || i == 'Y')	
    {
	diag_printf("\nPrevent (try to eject cartridge)");
	ret_val = PreventAllow(1);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\nPress any key to continue\n", ret_val);
	//		_getch();
	
	diag_printf("Allow ");
	ret_val = PreventAllow(0);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\n", ret_val);
	//		_getch();

	/* wait for cartridge insertion */
	WaitForCartridge();
    }
#endif

#if 0
    /* Test prevent and allow */
    diag_printf("\nTest cartridge eject? ");
    //	i = _getch(); 
	
    if (i =='y' || i == 'Y')	
    {
	ret_val = EjectMedia();
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\n", ret_val);
	/* wait for cartridge insertion */
	WaitForCartridge();
    }	
#endif

#if 0
    /* Test Cartridge Protect */
    diag_printf("\nTest cartridge protect? ");
    //	i = _getch();
    
    if (i == 'y' || i == 'Y')
    {
	diag_printf("\nWrite Protect");
	ret_val = CartridgeProtect(2,read_buffer,0);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\n", ret_val);

	diag_printf("Trying to write");
	ret_val = WriteAtapi(0x100, 0x1, write_buffer);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\n", ret_val);

	diag_printf("Trying to read");
	ret_val = ReadAtapi(0x100, 0x1, read_buffer);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\n", ret_val);
		
	diag_printf("Unprotect");                      
	ret_val = CartridgeProtect(0,read_buffer,0);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\n", ret_val);

	diag_printf("Trying to write");
	ret_val = WriteAtapi(0x100, 0x1, write_buffer);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\n", ret_val);

	diag_printf("Trying to read");
	ret_val = ReadAtapi(0x100, 0x1, read_buffer);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\n", ret_val);
    }
#endif

#if 0
    /* Test Get Serial Number? */
    diag_printf("\nTest Get Serial Number? ");
    i = i;
    
    if (i == 'y' || i == 'Y')
    {
     	ret_val = GetMediaID(read_buffer);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
	diag_printf(" EC:%02X\n", ret_val);
	if (!ret_val)
	{
	    read_buffer[40] = 0;
	    diag_printf("Serial Number %s", read_buffer);
	}
    }
#endif

#if 0
    /* Test Format Cartridge */
    diag_printf("\nTest format cartridge? ");
    i = i;
    
    if (i == 'y' || i == 'Y')
    {
     	ret_val = FormatDrive(write_buffer);
	diag_printf(" EC:%02X\n", ret_val);
    }

    diag_printf("\nDriveStatus");
    ret_val = DriveStatus();
    if (ret_val == ERROR_WAIT_FOR_DEVICE)
	ret_val = MultiProcess();
    diag_printf(" EC:%02X\n", ret_val);

    diag_printf("\nSleepDrive");
    ret_val = SleepDrive();
    diag_printf(" EC:%02X\n", ret_val);
	
    diag_printf("\nDriveStatus");
    ret_val = DriveStatus();
    if (ret_val == ERROR_WAIT_FOR_DEVICE)
	ret_val = MultiProcess();
    diag_printf(" EC:%02X\n", ret_val);
#endif
}                                     


void CompareData(cyg_uint8 * read_buffer, cyg_uint8 * write_buffer, cyg_uint16 length)
{
    cyg_uint32 i;

    for (i=0; i < length * 512; i++)
    {
	if (write_buffer[i] != read_buffer[i])
	    break;
    }
	
    if (i == length * (512))
	diag_printf("c");
    else {
	diag_printf(" MISCOMPARE @[%d] W%04X R=%04X\n", i, write_buffer[i], read_buffer[i]);
	for (;;)
	    ;
    }
}


cyg_uint8 MultiProcess(void)
{
    cyg_uint8  ret_val;

    do
    {
	ret_val = ProcessInterrupt();
    }
    while(ret_val == 0xFE);
    return(ret_val);
}


cyg_uint8 WaitForCartridge(void)
{
    cyg_uint8 ret_val;

	
    memset(packet,0,12);
    ret_val = SendAtapiPacket(packet, 0);
    if (ret_val == ERROR_WAIT_FOR_DEVICE)
	ret_val = MultiProcess();
    while (ret_val != NO_ERROR)
    {
	diag_printf("Put cartridge back in and press any key\n");
	ret_val = SendAtapiPacket(packet, 0);
	if (ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = MultiProcess();
    }
    return(ret_val);
}     
