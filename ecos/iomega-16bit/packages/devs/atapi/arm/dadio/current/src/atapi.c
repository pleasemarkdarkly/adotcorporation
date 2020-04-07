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
#include <pkgconf/io_pm_arm_dadio.h>
#include <cyg/io/devtab.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/infra/diag.h>

#include <dadio/io/atapi.h>
#include <dadio/io/blk_dev.h>
#include <dadio/io/edk.h>	/* atapi low level */
#include <dadio/io/ata.h>	/* ata low level */
#include <dadio/io/pm.h>

#define MAX_RETRIES 5		/* number of times to retry read or write operations */
#define PHASE_3

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
static cyg_io_handle_t pm_devH = 0;
cyg_ucount32 lba_last_error_index;

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
	     DDODAT_IO_PM_NAME,
             &io_atapi_devio,
             atapi_init, 
             atapi_lookup,
             0
    );

static bool
atapi_init(struct cyg_devtab_entry *tab)
{
    cyg_mutex_init(&atapi_mutex);
    lba_last_error_index = cyg_thread_new_data_index();
    /* save the rest of initialization for POWER_UP */
    return true;
}

static Cyg_ErrNo
atapi_lookup(struct cyg_devtab_entry **tab, 
	     struct cyg_devtab_entry *sub_tab,
	     const char *name)
{
    Cyg_ErrNo res = ENOERR;

    if (pm_devH == 0) {
	pm_devH = (cyg_io_handle_t)sub_tab;
    }
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
				diag_printf("reset and init failed: %x\n", res);
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

	case IO_ATAPI_GET_CONFIG_LBA_LAST_ERROR:
	{
	    *(unsigned int *)_buf = (unsigned int)cyg_thread_get_data(lba_last_error_index);
	    break;
	}
	
	default:
	    res = -ENOSYS;
	    break;
    }
    cyg_mutex_unlock(&atapi_mutex);
    return res;
}

/* DO NOT REMOVE the diag_printf's from this function. */
static Cyg_ErrNo
atapi_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len)
{
    static cyg_tick_count_t sleep_time = 0;
    Cyg_ErrNo res = ENOERR;
    cyg_uint8 dev_res;
    cyg_uint32 _len;

    cyg_mutex_lock(&atapi_mutex);
    switch (key) {
	case IO_ATAPI_SET_CONFIG_OPEN: 
	{
	    //diag_printf("%s %d\n", __FILE__, __LINE__);
	    dev_res = DEVICE_INIT();
	    res = dev_res_to_errno(dev_res);
	    break;
	}

	case IO_ATAPI_SET_CONFIG_SLEEP:
	{
	    //diag_printf("%s %d\n", __FILE__, __LINE__);
	    dev_res = DEVICE_SLEEP();
	    sleep_time = cyg_current_time();
	    res = dev_res_to_errno(dev_res);
	    break;
	}

	case IO_ATAPI_SET_CONFIG_WAKEUP:
	{
	    int num_retries;
	    cyg_tick_count_t wakeup_time;

	    //diag_printf("%s %d\n", __FILE__, __LINE__);
	    
	    /* wait a minimum of 100ms between sleep and wakeup */
	    wakeup_time = cyg_current_time();
	    if ((wakeup_time - sleep_time) < 10) {
		cyg_thread_delay(10 - (wakeup_time - sleep_time));
	    }

	    /* just in case the above didn't work, be extra paranoid
	     * about not hanging the device when waking up */
	    num_retries = 0;
	    do {
		//diag_printf("%s %d\n", __FILE__, __LINE__);
		dev_res = DEVICE_RESET();
		if (dev_res == NO_ERROR) {
		    dev_res = DEVICE_INIT();
		}
		++num_retries;
	    } while (dev_res != NO_ERROR && num_retries < MAX_RETRIES);
	    res = dev_res_to_errno(dev_res);
	    break;
	}

	case IO_PM_SET_CONFIG_POWER_DOWN: 
	{
	    diag_printf("Powering down clik drive\n");
	    /* put the drive in reset mode */
	    *(volatile cyg_uint8 *)PBDR &= ~0x01;
#ifdef PHASE_3
	    /* turn off power to the drive */
	    *(volatile cyg_uint8 *)PDDR |= 0x04;
#endif /* PHASE_3 */
	    break;
	}

	case IO_PM_SET_CONFIG_POWER_UP: 
	{
	    unsigned int delay;
	    cyg_uint8 dev_res;

	    diag_printf("Power up clik drive\n");
#ifdef PHASE_3
	    /* power on drive */
	    *(volatile cyg_uint8 *)PDDR &= ~0x04;
#endif /* PHASE_3 */
	    
	    /* do a hard power on/power off for the device */
	    *(volatile cyg_uint8 *)PBDR |= 0x01;
	    for (delay = 0; delay < 100000; delay++)
		;
	    *(volatile cyg_uint8 *)PBDR &= ~0x01;
	    for (delay = 0; delay < 100000; delay++)
		;
	    *(volatile cyg_uint8 *)PBDR |= 0x01;    

	    /* delay 100ms.  this is only necessary for 16 bit model */
	    cyg_thread_delay(10);
	    
	    dev_res = DEVICE_RESET();
	    if (dev_res != NO_ERROR) {
		diag_printf("%s %d\n", __FILE__, __LINE__);
		diag_printf("atapi_init: could not reset device\n");
		res = -EIO;
	    }
	    //diag_printf("%s %d\n", __FILE__, __LINE__);
	    break;
	}

	    /* handle these by passing them down to the pm device */
	case IO_PM_SET_CONFIG_REGISTER:
	case IO_PM_SET_CONFIG_UNREGISTER:
	    cyg_mutex_unlock(&atapi_mutex); /* this may call into this function again
					     * on power up, so unlock the mutex */
	    _len = sizeof(cyg_io_handle_t);
	    return cyg_io_set_config(pm_devH, key, handle, &_len);
	    
	default:
	    res = -ENOSYS;
	    break;
    }
    cyg_mutex_unlock(&atapi_mutex);
    //diag_printf("%s %d %d\n", __FILE__, __LINE__, res);
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
	case 4:  // ASC = 0x04, logical unit not ready
	    return -ENOMED;
	    
	case ERROR_MEDIA_CHANGED:
	    return -EMEDCHG;

	case ERROR_SLEEP:
	    return -ESLEEP;
	    	    
	case ERROR_OUT_OF_PHASE:
	case ERROR_TIMEOUT:
	    return -EIO;
	    
	case ERROR_READ_ERROR:
	    return -EREAD;

	default:
	    diag_printf("Unaccounted for error %x\n", dev_res);
	    return -EIO;
    }
}
