//==========================================================================
//#####Descriptionbegin####
//
// Author(s):   danc
// Contributors:  danc toddm
// Date:        2000-4-26
// Purpose:     clik! ATAPI driver.
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/io.h>
#include <pkgconf/io_atapi_arm_dadio.h>
#include <cyg/io/devtab.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/infra/diag.h>

#include <dadio/io/atapi.h>
#include <dadio/io/blk_dev.h>
#include <dadio/io/edk.h>	/* atapi low level */
#include <dadio/io/ata.h>	/* ata low level */

#define MAX_RETRIES 5		/* number of times to retry read or write operations */

#ifdef DDOIMP_IO_ATAPI_USE_ATA
#define DEVICE_RESET() ResetAtapi()
#define DEVICE_INIT() init_ata()
#define DEVICE_READ(lba,len,buf) read_ata(lba,len,buf)
#define DEVICE_WRITE(lba,len,buf) write_ata(lba,len,buf)
#define DEVICE_GET_GEOMETRY(buf) ata_get_geometry(buf)
#else
#define DEVICE_RESET() ResetAtapi()
#define DEVICE_INIT() InitAtapi()
#define DEVICE_READ(lba,len,buf) ReadAtapi(lba,len,buf)
#define DEVICE_WRITE(lba,len,buf) WriteAtapi(lba,len,buf)
#define DEVICE_GET_GEOMETRY(buf) get_geometry_atapi(buf)
#define DEVICE_MEDIA_STATUS() InitAtapi()
#define DEVICE_SLEEP() SleepDrive()
#endif /* DDOIMP_IO_ATAPI_USE_ATA */

static cyg_mutex_t atapi_mutex;

static cyg_uint8 reset_and_init(void);
static Cyg_ErrNo dev_res_to_errno(cyg_uint8 dev_res);

static bool atapi_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo atapi_lookup(struct cyg_devtab_entry **tab, 
			      struct cyg_devtab_entry *sub_tab,
			      const char *name);
static Cyg_ErrNo atapi_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len);
static Cyg_ErrNo atapi_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
static Cyg_ErrNo atapi_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo atapi_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len);

DEVIO_TABLE(io_atapi_devio,
            atapi_write,
            atapi_read,
            atapi_get_config,
            atapi_set_config
    );

DEVTAB_ENTRY(io_atapi_devtab,
	     DDODAT_IO_ATAPI_NAME,
             0,
             &io_atapi_devio,
             atapi_init, 
             atapi_lookup,
             0			/* this word used as a variable, not a pointer */
    );

static bool
atapi_init(struct cyg_devtab_entry *tab)
{
    unsigned int delay;
    cyg_uint8 dev_res;

    /* do a hard power on/power off for the device */
    //*(volatile cyg_uint8 *)PBDDR |= 0x01;
    *(volatile cyg_uint8 *)PBDR |= 0x01;
    for (delay = 0; delay < 100000; delay++)
	;
    *(volatile cyg_uint8 *)PBDR &= ~0x01;
    for (delay = 0; delay < 100000; delay++)
	;
    *(volatile cyg_uint8 *)PBDR |= 0x01;    
    //*(volatile cyg_uint8 *)PBDDR &= ~0x01;

    cyg_mutex_init(&atapi_mutex);
    tab->priv = 0;

    dev_res = DEVICE_RESET();
    if (dev_res != NO_ERROR) {
	diag_printf("atapi_init: could not reset device\n");
	return false;
    }    
    
    return true;
}

static Cyg_ErrNo
atapi_lookup(struct cyg_devtab_entry **tab, 
	     struct cyg_devtab_entry *sub_tab,
	     const char *name)
{
    Cyg_ErrNo res = ENOERR;

    /* do nothing
     * mounting of disks will take place later */
#if 0
    //diag_printf("atapi_lookup enter\n");
    cyg_mutex_lock(&atapi_mutex);
    res = reset_and_init();
    if (res != ENOERR) {
	diag_printf("Error on reset and initialization on ATAPI device\n");
    }
    cyg_mutex_unlock(&atapi_mutex);
    //diag_printf("atapi_lookup exit\n");
#endif
    return res;
}

static Cyg_ErrNo
atapi_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 *len)
{
    const blk_request_t * blk_req = (const blk_request_t *)buf;
    Cyg_ErrNo res = ENOERR;
    cyg_uint8 dev_res;

    cyg_mutex_lock(&atapi_mutex);
    dev_res = DEVICE_WRITE(blk_req->lba, blk_req->num_blks, blk_req->buf);
    res = dev_res_to_errno(dev_res);
    if (res == -EIO) {
	int num_retries;
	for (num_retries = 0; num_retries < MAX_RETRIES; ++num_retries) {
	    dev_res = reset_and_init();
	    res = dev_res_to_errno(dev_res);
	    if (res == ENOERR) {
		dev_res = DEVICE_WRITE(blk_req->lba, blk_req->num_blks, blk_req->buf);
		res = dev_res_to_errno(dev_res);
		if (res == ENOERR) {
		    break;
		}
	    }
	    else if (res != -EIO) {
		diag_printf("reset and init failed\n");
		break;
	    }
	}
    }
    cyg_mutex_unlock(&atapi_mutex);
    return res;
}

static Cyg_ErrNo
atapi_read(cyg_io_handle_t handle, void * buf, cyg_uint32 *len)
{
    blk_request_t * blk_req = (blk_request_t *)buf;
    Cyg_ErrNo res = ENOERR;
    cyg_uint8 dev_res;

    cyg_mutex_lock(&atapi_mutex);
    dev_res = DEVICE_READ(blk_req->lba, blk_req->num_blks, blk_req->buf);
    res = dev_res_to_errno(dev_res);
    if (res == -EIO) {
	int num_retries;
	for (num_retries = 0; num_retries < MAX_RETRIES; ++num_retries) {
	    dev_res = reset_and_init();
	    res = dev_res_to_errno(dev_res);
	    if (res == ENOERR) {
		dev_res = DEVICE_READ(blk_req->lba, blk_req->num_blks, blk_req->buf);
		res = dev_res_to_errno(dev_res);
		if (res == ENOERR) {
		    break;
		}
	    }
	    else if (res != -EIO) {
		diag_printf("reset and init failed\n");
		break;
	    }
	}
    }
    cyg_mutex_unlock(&atapi_mutex);
    return res;
}

static Cyg_ErrNo
atapi_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * _buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t * t = (cyg_devtab_entry_t *)handle;
    Cyg_ErrNo res = ENOERR;
    cyg_uint8 dev_res;

    cyg_mutex_lock(&atapi_mutex);
    switch (key) {
	case IO_ATAPI_GET_CONFIG_GEOMETRY: 
	{
	    drive_geometry_t * buf = (drive_geometry_t *)_buf;
	    dev_res = DEVICE_GET_GEOMETRY(buf);
	    res = dev_res_to_errno(dev_res);
	    break;
	}

	case IO_ATAPI_GET_CONFIG_MEDIA_STATUS:
	{
#ifdef DDOIMP_IO_ATAPI_USE_ATA
	    int * buf = (int *)_buf;
	    
	    *buf = ((*(volatile cyg_uint8 *)PBDR) & 0x40) == 0 ? 1 : 0;
	    if ((int)t->priv == 0 && *buf == 1) {
		res = DEVICE_RESET();
		if (res != NO_ERROR) {
		    res = -EIO;
		}
		(int)t->priv = DEVICE_INIT();	/* TODO figure out how to do correct error code on this */
		*buf = (int)t->priv;
	    }
#else
	    dev_res = DEVICE_MEDIA_STATUS();
	    res = dev_res_to_errno(dev_res);
#endif
	    break;
	}
	
	default:
	    res = -ENOSYS;
	    break;
    }
    cyg_mutex_unlock(&atapi_mutex);
    return res;
}

static Cyg_ErrNo
atapi_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len)
{
    Cyg_ErrNo res = ENOERR;
    cyg_uint8 dev_res;
    
    switch (key) {
	case IO_ATAPI_SET_CONFIG_OPEN:
	    dev_res = DEVICE_INIT();
	    res = dev_res_to_errno(dev_res);
	    break;

	case IO_ATAPI_SET_CONFIG_SLEEP:
	    dev_res = DEVICE_SLEEP();
	    res = dev_res_to_errno(dev_res);
	    break;

	case IO_ATAPI_SET_CONFIG_WAKEUP:
	    dev_res = DEVICE_RESET();
	    if (dev_res == NO_ERROR) {
		dev_res = DEVICE_INIT();
	    }
	    res = dev_res_to_errno(dev_res);
	    break;
	    
	default:
	    res = -ENOSYS;
	    break;
    }
    return res;
}

static cyg_uint8
reset_and_init(void)
{
    cyg_uint8 dev_res;

    dev_res = DEVICE_RESET();
    if (dev_res != NO_ERROR) {
	diag_printf("Error on reset on ATAPI device %x\n", dev_res);
    }
    dev_res = DEVICE_INIT();
    if (dev_res != NO_ERROR) {
	diag_printf("Error on device init on ATAPI device %x\n", dev_res);
    }
    return dev_res;
}

static Cyg_ErrNo
dev_res_to_errno(cyg_uint8 dev_res) 
{
    switch (dev_res) {
	case NO_ERROR:
	    return ENOERR;
	    
	case ERROR_NO_MEDIA:
	    return -ENOMED;
	    
	case ERROR_MEDIA_CHANGED:
	    return -EMEDCHG;

	case ERROR_SLEEP:
	    return -ESLEEP;
	    	    
	case ERROR_OUT_OF_PHASE:
	case ERROR_TIMEOUT:
	    return -EIO;
	    
	default:
	    diag_printf("Unaccounted for error %x\n", dev_res);
	    return -EIO;
    }
}
