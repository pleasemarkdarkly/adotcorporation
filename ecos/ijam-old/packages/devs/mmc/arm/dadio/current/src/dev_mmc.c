#include <cyg/error/codes.h>
#include <cyg/io/devtab.h>
#include <cyg/io/config_keys.h>
#include <cyg/io/io.h>
#include <dadio/io/pm.h>

#include <dadio/io/sdapi.h>//for geom struct,for mmc_drive_open... specify almost full path
#include <dadio/io/blk_dev.h>//for block struct
#include <dadio/io/dev_mmc.h>
#include <dadio/io/mmcoem.h>

//#define __DEBUG_MMC
#ifdef __DEBUG_MMC
#include <cyg/infra/diag.h>
#define DEBUG_MMC(s...) diag_printf(##s)
#else
#define DEBUG_MMC(s...) /**/
#endif


#define NUM_DRIVES 2
#define MAX_RETRIES 5   /* number of times to retry read or write operations */
#define CARD0_NOT_PRESENT 0x08
#define CARD1_NOT_PRESENT 0x10

#define DRIVENO(x)     ( (x) & 0x1)
#define PRESENT(x)     (((x) & 0x2) >> 1)
#define SET_PRESENT(x) (((x) |=   0x02 ))
#define CLR_PRESENT(x) (((x) &= (~0x02)))

//since static, can't place these in MMC.h
static bool dev_mmc_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo dev_mmc_lookup(struct cyg_devtab_entry **tab, struct cyg_devtab_entry *sub_tab,const char *name);//NOT same arg as suggested in docs!!!
static Cyg_ErrNo dev_mmc_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len);
static Cyg_ErrNo dev_mmc_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
static Cyg_ErrNo dev_mmc_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo dev_mmc_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len);

//init DEV struct var
DEVIO_TABLE(mmc_devio,dev_mmc_write,dev_mmc_read,dev_mmc_get_config,dev_mmc_set_config);
DEVTAB_ENTRY(dev_mmc_Zero,"/dev/hda","/dev/pm",&mmc_devio,dev_mmc_init,dev_mmc_lookup,(void*)0);//init all devtab members
DEVTAB_ENTRY(dev_mmc_One,"/dev/hdb","/dev/pm",&mmc_devio,dev_mmc_init,dev_mmc_lookup,(void*)1);

static Cyg_ErrNo dev_res_to_errno(int driveno, cyg_uint8 dev_res);
static Cyg_ErrNo reset_and_open(int driveno);
static Cyg_ErrNo peek_media_status(int driveno);
static Cyg_ErrNo get_media_status(int driveno);

/* TODO seperate state information (mutex) for each drive */
static cyg_mutex_t mmc_mutex;
static cyg_io_handle_t pm_devH = 0;
static Cyg_ErrNo media_status[NUM_DRIVES] = { -ENOMED, -ENOMED };

#define STACKSIZE 4096
static cyg_handle_t medchg_poll_thread;
static cyg_thread medchg_poll_thread_obj;
static char medchg_poll_stack[STACKSIZE];
static void medchg_poll(cyg_addrword_t data);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool
dev_mmc_init(struct cyg_devtab_entry * tab)
{
    static bool initialized = false;
    
    DEBUG_MMC("mmc init\n");
    if (!initialized) {
	cyg_mutex_init(&mmc_mutex);
	
	/* create thread to poll for media changes */
	/* TODO adjust priority ? */
	cyg_thread_create(5, medchg_poll, (cyg_addrword_t)0, "medchg_poll",
			  (void *)medchg_poll_stack, STACKSIZE, &medchg_poll_thread, &medchg_poll_thread_obj);
	cyg_thread_resume(medchg_poll_thread);
	
	initialized = true;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Cyg_ErrNo
dev_mmc_lookup(struct cyg_devtab_entry ** tab,
	       struct cyg_devtab_entry * sub_tab,
	       const char * name)
{
    Cyg_ErrNo res = ENOERR;

    //DEBUG_MMC("mmc lookup\n");
    pm_devH = (cyg_io_handle_t)sub_tab;//pass subtab dev entry to pm_h handle
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Cyg_ErrNo
dev_mmc_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 * len)
{
    const blk_request_t* blk_req = (const blk_request_t*)buf;
    cyg_devtab_entry_t* tab = (cyg_devtab_entry_t*)handle;
    Cyg_ErrNo res = ENOERR;
    cyg_uint8 dev_res;
    int DriveNum;
    int num_retries;

    cyg_mutex_lock(&mmc_mutex);

    /* check media status before trying write */
    DriveNum = DRIVENO((int)(tab->priv));
    if (peek_media_status(DriveNum) == ENOERR) {
	
	/* write requested blocks */
	dev_res = mmc_write(DriveNum, blk_req->lba, blk_req->buf, blk_req->num_blks);
	
	/* if write failed, retry */
	if (dev_res == NO) {
	    for (num_retries=0; num_retries < MAX_RETRIES; ++num_retries) {
		DEBUG_MMC("retrying write\n");
		
		/* reset drive before retrying write */
		res = reset_and_open(DriveNum);
		
		/* if reset succeeded, retry write */
		if (res == ENOERR) {
		    dev_res = mmc_write(DriveNum, blk_req->lba, blk_req->buf, blk_req->num_blks);

		/* if retry succeeded, return */
		res = dev_res_to_errno(DriveNum, dev_res);
		if (res == ENOERR) {
		    break;
		}
		}
		/* reset failed, so nothing we can really do at this point except
		   return error condition */
		else if (res != -EIO) {
		    DEBUG_MMC("reset and init failed\n");
		    break;
		}
	    }
	}
    }
    else {
	res = get_media_status(DriveNum);
    }
    
    cyg_mutex_unlock(&mmc_mutex);
    return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Cyg_ErrNo
dev_mmc_read(cyg_io_handle_t handle, void * buf, cyg_uint32 * len)
{
    blk_request_t* blk_req = (blk_request_t *)buf;
    cyg_devtab_entry_t* tab = (cyg_devtab_entry_t*)handle;
    Cyg_ErrNo res = ENOERR;
    cyg_uint8 dev_res;
    int DriveNum;
    int num_retries;
    
    cyg_mutex_lock(&mmc_mutex);

    /* check media status before reading */
    DriveNum = DRIVENO((int)(tab->priv));
    if (peek_media_status(DriveNum) == ENOERR) {
    
	/* read the requested blocks */
	dev_res = mmc_read(DriveNum, blk_req->lba, blk_req->buf, blk_req->num_blks);
	
	/* if read failed, retry */
	if(dev_res == NO) {
	    for (num_retries = 0; num_retries < MAX_RETRIES; ++num_retries) {
		DEBUG_MMC("retrying read\n");
		
		/* reset drive before retrying read */
		res = reset_and_open(DriveNum);
		
		/* if reset succeeded, retry the read */
		if (res == ENOERR) {
		    dev_res = mmc_read(DriveNum, blk_req->lba, blk_req->buf, blk_req->num_blks);
		    
		    /* if retry succeeded, return */
		    res = dev_res_to_errno(DriveNum, dev_res);
		    if (res == ENOERR) {
			break;
		    }
		}
		/* reset failed, so return error condition to caller */
		else if (res != -EIO) {
		    DEBUG_MMC("reset and init failed\n");
		    break;
		}
	    }
	}
    }
    else {
	res = get_media_status(DriveNum);
    }
    
    cyg_mutex_unlock(&mmc_mutex);
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Cyg_ErrNo
dev_mmc_get_config(cyg_io_handle_t handle, cyg_uint32 key,void * buf, cyg_uint32 * len)
{
    cyg_devtab_entry_t * tab = (cyg_devtab_entry_t*)handle;
    Cyg_ErrNo res = ENOERR;
    cyg_uint8 dev_res;
    int DriveNum;

    cyg_mutex_lock(&mmc_mutex);
    switch(key)
    {
	case IO_BLK_GET_CONFIG_GEOMETRY:
	{
	    drive_geometry_t * dg = (drive_geometry_t *)buf;
	    DRV_GEOMETRY_DESC GeomDesc;
	    int i;
	    
	    /* check media status before touching drive */
	    DriveNum = DRIVENO((int)(tab->priv));
	    if (peek_media_status(DriveNum) == ENOERR) {
		
		/* get drive geometry from mmc */
		dev_res = mmc_read_serial(DriveNum, &GeomDesc);
		res = dev_res_to_errno(DriveNum, dev_res);

		/* translate mmc's drive geometry description to generic
		   block device one */
		dg->cyl = GeomDesc.dfltCyl;
		dg->hd = GeomDesc.dfltHd;
		dg->sec = GeomDesc.dfltSct;
		dg->bytes_p_sec = GeomDesc.dfltBytesPerSect;
		dg->num_blks = GeomDesc.totalLBA;
		/* TODO make the bounds checks for the dg structure also */
		for (i = 0; i < sizeof(GeomDesc.serialNum); ++i) {
		    dg->serial_num[i] = GeomDesc.serialNum[i];
		}
		for (i = 0; i < sizeof(GeomDesc.modelNum); ++i) {
		    dg->model_num[i] = GeomDesc.modelNum[i];
		}
	    }
	    else {
		res = get_media_status(DriveNum);
	    }
	    break;
	}

	case IO_BLK_GET_CONFIG_MEDIA_STATUS:
	{
	    /* get current media status to return to caller */
	    DriveNum = (short)DRIVENO((int)(tab->priv));
	    res = get_media_status(DriveNum);
	    break;
	}
	
	default:
	    res = -ENOSYS;
	    break;
    }
    cyg_mutex_unlock(&mmc_mutex);
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Cyg_ErrNo dev_mmc_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t* tab=(cyg_devtab_entry_t*) handle;
    Cyg_ErrNo res=ENOERR;//init returned res
    cyg_uint32 _len;
    int DriveNum = 0;
    int delay;

    cyg_mutex_lock(&mmc_mutex);//lock mutex,binary semaphore
    switch(key)
    {
	case IO_BLK_SET_CONFIG_OPEN://called in reset_and_open()
	    //DEBUG_MMC("OPEN, does nothing\n");
	    break;

	case IO_BLK_SET_CONFIG_ERASE://never gets called so commented out
	    DEBUG_MMC("ERASE, should not see this\n");
	    break;

	    /* TODO power up/down are waiting for hardware fix ,2000/09/05 */
	case IO_PM_SET_CONFIG_POWER_DOWN: //use GPIO driver as sub tab?key TBD//PE0 is used to power down/up MMC
	    DEBUG_MMC("powering down mmc\n");

	    /* even though only one drive is selected, both drives will be powered down
	     * since they both go through only one pin */
	    DriveNum = (short)DRIVENO((int)(tab->priv));
	    mmcPowerOnOff(DriveNum, 0);
	    break;

	case IO_PM_SET_CONFIG_POWER_UP://key TBD //PE0 is used to power down/up MMC
	    DEBUG_MMC("powering up mmc\n");
	    
	    /* even though only one drive is selected, both drives will be powered up
	     * since they both go through only one pin */
	    DriveNum = (short)DRIVENO((int)(tab->priv));
	    mmcPowerOnOff(DriveNum, 1);
	    break;

	case IO_PM_SET_CONFIG_REGISTER://TBD
	case IO_PM_SET_CONFIG_UNREGISTER:
	    cyg_mutex_unlock(&mmc_mutex); /* unlock mutex before returning from this function */
	    _len = sizeof(cyg_io_handle_t);
	    return cyg_io_set_config(pm_devH, key, handle, &_len);

	default:
	    res = -ENOSYS;
	    break;
    }
    cyg_mutex_unlock(&mmc_mutex);
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Cyg_ErrNo
dev_res_to_errno(int driveno, cyg_uint8 dev_res)
{
    Cyg_ErrNo res = -EIO;	/* assume generic IO error */

    switch (dev_res) {
	/* mmc operation succeeded */
	case YES:
	    res = ENOERR;
	    break;
	    
	    /* mmc operation failed */
	case NO:
	    /* media is present on entry to this function, so if error occured we can assume
	       either no media or io error */
	    /* if media not present, return -ENOMED */
	    if (!mmc_card_present(driveno)) {
		res = -ENOMED;
	    }
	    break;
	    
	default:
	    /* should never enter this case since all mmc ops return only YES or NO */
	    DEBUG_MMC("mmc device called returned neither YES nor NO\n");
	    break;
    }
    
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Cyg_ErrNo reset_and_open(int driveno)
{
    cyg_uint8 dev_res;
    Cyg_ErrNo res = ENOERR;
    int num_retries;
    
    /* this actually does do a hard reset here */
    num_retries = 0;
    do {
	DEBUG_MMC("reset and open %d\n", num_retries);
	dev_res = mmc_init(driveno);
	if (dev_res == YES) {
	    dev_res = mmc_drive_open(driveno);
	}
	res = dev_res_to_errno(driveno, dev_res);
	++num_retries;
    } while (res != ENOERR &&
	     res != -ENOMED &&
	     num_retries < MAX_RETRIES);
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void
medchg_poll(cyg_addrword_t data)
{
    static cyg_uint8 media_present_status;
    
    for (;;) {
	cyg_mutex_lock(&mmc_mutex);
	
	/* get media status on both cards */
	media_present_status = *(volatile cyg_uint8 *)PDDR;
	
	/* check card0 */
	
	/* if card is not present set status to -ENOMED */
	if (media_present_status & CARD0_NOT_PRESENT) {
	    if (media_status[0] != -ENOMED) {
		media_status[0] = -ENOMED;
#if 0
		/* original mmc code does not call this on cleanup
		 * although it doesn't do anything, anyway */
		mmc_drive_close(0); 
#endif
	    }
	}
	/* card is present so check previous status and set media_status
	   to -EMEDCHG if no card was present last time status was polled */
	else if (media_status[0] == -ENOMED) {
	    media_status[0] = -EMEDCHG;
	    if (reset_and_open(0) != ENOERR) {
		DEBUG_MMC("reset_and_open(0) failed from medchg poll\n");
	    }
	}
	/* note that there's no change to ENOERR, that will be done when the
	   client calls GET_MEDIA_STATUS */

	/* check card1 */
	
	if (media_present_status & CARD1_NOT_PRESENT) {
	    if (media_status[1] != -ENOMED) {
		media_status[1] = -ENOMED;
#if 0
		/* original mmc code does not call this on cleanup
		 * although it does nothing, anyway */
		mmc_drive_close(1);
#endif
	    }
	}
	else if (media_status[1] == -ENOMED) {
	    media_status[1] = -EMEDCHG;
	    if (reset_and_open(1) != ENOERR) {
		DEBUG_MMC("reset_and_open(1) failed from medchg poll\n");
	    }
	}
	
	cyg_mutex_unlock(&mmc_mutex);

	/* TODO tune this between 50 and 10 */
	cyg_thread_delay(50);	/* poll every 0.5s */
    }
}

/* see what the current media status is without affecting the queue */
static Cyg_ErrNo
peek_media_status(int driveno) 
{
    return media_status[driveno];
}

/* get the current media status and update the queue */
static Cyg_ErrNo
get_media_status(int driveno)
{
    Cyg_ErrNo res;
    
    res = media_status[driveno];
    
    /* if there is a card, set media status to normal.
       in other words, clear the queue (which is only 1 event now) */
    if (res != -ENOMED) {
	media_status[driveno] = ENOERR;
    }
    
    return res;
}
