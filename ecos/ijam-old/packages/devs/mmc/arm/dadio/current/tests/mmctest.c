
#include <stdio.h>
#include <malloc.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/io/io.h>//for cyg_lookup
#include <cyg/kernel/kapi.h>//for Cygthread..
#include <cyg/hal/hal_edb7xxx.h>//for PDDR
#include <dadio/io/blk_dev.h>
#include <dadio/io/dev_mmc.h>//key use
#include <dadio/io/pm.h>//key use

#define KByte 1024
#define MAX_SECTORS 16  /* this is malloc'd so be really careful on the ijam device */
#define MAX_LBA 0x7000 /* set to 14mb worth of sectors->14m/512=0x7000 */

//int  drive_open_flag[TOTAL_DRIVES];
//DRV_GEOMETRY_DESC  idDrv[TOTAL_DRIVES];

void func(unsigned int);
void CompareData(cyg_uint8 * read_buffer, cyg_uint8 * write_buffer, cyg_uint16 length);

#define NTHREADS 1
#define STACKSIZE ((16 * 4096))

static cyg_handle_t thread[NTHREADS];
static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cyg_user_start(void) 
{
    cyg_thread_create(10,func,(cyg_addrword_t)0,"func",(void*)stack[0],STACKSIZE,&thread[0],&thread_obj[0]);  
    cyg_thread_resume(thread[0]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void func(unsigned int cheese)
{  
    //static cyg_uint8 read_buffer[MAX_SECTORS * 512], write_buffer[MAX_SECTORS * 512];
    cyg_uint8 * read_buffer, * write_buffer;
    cyg_uint32 length;
    cyg_uint32 lba;//N/U
    unsigned int i;
    cyg_uint32 len;
    Cyg_ErrNo err;
    cyg_io_handle_t blk_devH;
    blk_request_t blk_req;
    drive_geometry_t dg;
    
    srand(0x5834971);

#if 1
    /* Initialize data buffers */
    read_buffer =malloc(MAX_SECTORS*512);  
    write_buffer=malloc(MAX_SECTORS*512);
    diag_printf("read_buffer %x\nwrite_buffer %x\n", read_buffer, write_buffer);
    
    /* Initialize Write/Read Buffers */
    diag_printf("initializing write/read buffers\n");
    for(i=0;i<(MAX_SECTORS*512);i++) write_buffer[i]=(cyg_uint8)(i&0xff);
    memset(read_buffer,0,MAX_SECTORS*512);
    diag_printf("done initializing\n");
#endif
    
#if 0
    if (cyg_io_lookup("/dev/hda", &blk_devH) != ENOERR) CYG_TEST_EXIT("Could not get handle to /dev/hda");
    diag_printf("got handle to /dev/hda\n");
#else
    if (cyg_io_lookup("/dev/hdb", &blk_devH) != ENOERR) CYG_TEST_EXIT("Could not get handle to /dev/hdb");   
    diag_printf("got handle to /dev/hdb\n");
#endif

#if 1
    /* Test media present */
    diag_printf("Testing media status IOCTL\n");
    //for (;;) {
    len = sizeof(len);
    diag_printf("%d\n", cyg_io_get_config(blk_devH, IO_BLK_GET_CONFIG_MEDIA_STATUS, &len, &len));
    //}
#endif
    
    len=sizeof(len);
    if ((err = cyg_io_set_config(blk_devH, IO_PM_SET_CONFIG_REGISTER, &len, &len)) != ENOERR) 
    {
	diag_printf("Error registering drive %d\n", err);
    }
    cyg_io_set_config(blk_devH, IO_BLK_SET_CONFIG_OPEN, &len, &len);

#if 1 /* can't test this now, broken hw */
    /* Test power down/up */
    for(;;)
	{
	diag_printf("Testing power down/up\n");
#if 1
    lba = rand() / (RAND_MAX/MAX_LBA);
    length = (cyg_uint16)(((cyg_uint32)rand()) / (RAND_MAX/MAX_SECTORS));
    length += 1; // do at least one sector
    if (length > MAX_SECTORS) length = MAX_SECTORS;
    diag_printf("LBA: %06x Sectors: %04x\n", lba, length);	

    diag_printf("Write a sector while still powered on\n");
    blk_req.lba = lba;		
    blk_req.num_blks=length;
    blk_req.buf=write_buffer;
    if((err=cyg_io_write(blk_devH,&blk_req,&blk_req.num_blks))!=ENOERR) diag_printf("Error writing %d\n", err);
    else diag_printf("Write OK\n");

    diag_printf("Power down drive\n");
    len = sizeof(len);
#endif
    
	cyg_io_set_config(blk_devH, IO_PM_SET_CONFIG_POWER_DOWN, &len, &len);
#if 1
    diag_printf("Write a sector while powered off\n");
    /* TODO remove card while powered down, test for medchg */
	//because write does retry and reset therefore pwr MMC back up,caution for error interpretation
    if((err=cyg_io_write(blk_devH,&blk_req,&blk_req.num_blks))==ENOERR) diag_printf("Error Retry on Pwr Dn writing %d\n", err);
    else diag_printf("Pwr Dn OK\n");//should never get here since reset powers MMC back up.
    
    diag_printf("Power up drive\n");
    len = sizeof(len);
#endif
	cyg_io_set_config(blk_devH, IO_PM_SET_CONFIG_POWER_UP, &len, &len);

#if 1   
    diag_printf("Write a sector while powered on\n");
    if((err=cyg_io_write(blk_devH,&blk_req,&blk_req.num_blks))!=ENOERR) diag_printf("Error Pwr up writing %d\n", err);
    else diag_printf("Pwr Up OK\n");
#endif

	}//for
#endif

#if 0
    /* Test get geometry */
    diag_printf("Getting drive geometry\n");
    len = sizeof(dg);
    if((err=cyg_io_get_config(blk_devH, IO_BLK_GET_CONFIG_GEOMETRY, &dg, &len))!=ENOERR)
    {
	diag_printf("Error getting geometry %d\n", err);
    }
    else
    {
	diag_printf("Get geometry succeeded\n");
	diag_printf("Total LBA %d\n", dg.num_blks);
	diag_printf("C/H/S %d/%d/%d\n", dg.cyl, dg.hd, dg.sec);
	diag_printf("Serial #%s, Model #%s\n", dg.serial_num, dg.model_num);
    }
#endif
    
#if 0
    /* Test random read/write/compare */
    diag_printf("Testing random read/write compares\n");
    for(;;)
    {
	lba = rand() / (RAND_MAX/MAX_LBA);
	length = (cyg_uint16)(((cyg_uint32)rand()) / (RAND_MAX/MAX_SECTORS));
	length += 1; // do at least one sector
	if (length > MAX_SECTORS) length = MAX_SECTORS;
	diag_printf("LBA: %06x Sectors: %04x\n", lba, length);
	
	blk_req.lba = lba;		
	blk_req.num_blks=length;
	blk_req.buf=write_buffer;
	diag_printf("w");			
	if((err=cyg_io_write(blk_devH,&blk_req,&blk_req.num_blks))!=ENOERR) 
	{
	    diag_printf("Error writing %d\n", err);
	    continue;
	}
				
	memset(read_buffer, 0, 512*MAX_SECTORS);	   
	blk_req.lba = lba;
	blk_req.num_blks = length;
	blk_req.buf = read_buffer;
	diag_printf("r");
	if((err=cyg_io_read(blk_devH,&blk_req,&blk_req.num_blks))!=ENOERR)
	{
	    diag_printf("Error reading %d\n", err);
	    continue;
	}
	
	CompareData(read_buffer, write_buffer,length);
	diag_printf("\n");    
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CompareData(cyg_uint8 * read_buffer, cyg_uint8 * write_buffer, cyg_uint16 length)
{    
    cyg_uint32 i;

    for (i=0; i < length * 512; i++)    
    {
	if(write_buffer[i]!=read_buffer[i]) break;	    
    }
	
    if(i==length*(512)) diag_printf("c");	
    else 
    {
	diag_printf(" MISCOMPARE @[%d] W%04X R=%04X\n", i, write_buffer[i], read_buffer[i]);		
	for (;;) ;
    }
}

