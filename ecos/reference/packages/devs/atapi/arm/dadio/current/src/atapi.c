//==========================================================================
//#####DESCRIPTIONBEGIN####
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
#endif /* DDOIMP_IO_ATAPI_USE_ATA */

static cyg_mutex_t atapi_mutex;

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

    /* do a hard power on/power off for the device */
    /* reset pin is on PB1 for both ATA and ATAPI devices */
    *(volatile cyg_uint8 *)PBDDR |= 0x02;
    *(volatile cyg_uint8 *)PBDR |= 0x02;
    for (delay = 0; delay < 100000; delay++)
	;
    *(volatile cyg_uint8 *)PBDR &= ~0x02;
    for (delay = 0; delay < 100000; delay++)
	;
    *(volatile cyg_uint8 *)PBDR |= 0x2;    
    *(volatile cyg_uint8 *)PBDDR &= ~0x02;

    cyg_mutex_init(&atapi_mutex);
    tab->priv = 0;
    
    return true;
}

static Cyg_ErrNo
atapi_lookup(struct cyg_devtab_entry **tab, 
	     struct cyg_devtab_entry *sub_tab,
	     const char *name)
{
    Cyg_ErrNo res = ENOERR;

    cyg_mutex_lock(&atapi_mutex);
    res = DEVICE_RESET();
    if (res != NO_ERROR) {
	diag_printf("Error on reset on ATAPI device\n");
	res = -EIO;
    }
    res = DEVICE_INIT();
    if (res != NO_ERROR) {
	diag_printf("Error on device init on ATAPI device\n");
	res = -EIO;
    }
    cyg_mutex_unlock(&atapi_mutex);
    return res;
}

static Cyg_ErrNo
atapi_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 *len)
{
    const blk_request_t * blk_req = (const blk_request_t *)buf;
    Cyg_ErrNo res = ENOERR;

    cyg_mutex_lock(&atapi_mutex);
    res = DEVICE_WRITE(blk_req->lba, blk_req->num_blks, blk_req->buf);
    if (res != NO_ERROR) {
	res = -EIO;
    }
    cyg_mutex_unlock(&atapi_mutex);
  
    return res;
}

static Cyg_ErrNo
atapi_read(cyg_io_handle_t handle, void * buf, cyg_uint32 *len)
{
    blk_request_t * blk_req = (blk_request_t *)buf;
    Cyg_ErrNo res = ENOERR;
    
    cyg_mutex_lock(&atapi_mutex);
    res = DEVICE_READ(blk_req->lba, blk_req->num_blks, blk_req->buf);
    if (res != NO_ERROR) {
	res = -EIO;
    }
    cyg_mutex_unlock(&atapi_mutex);
  
    return res;
}

static Cyg_ErrNo
atapi_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * _buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t * t = (cyg_devtab_entry_t *)handle;
    Cyg_ErrNo res = ENOERR;
    
    cyg_mutex_lock(&atapi_mutex);
    switch (key) {
	case IO_ATAPI_GET_CONFIG_GEOMETRY: 
	{
	    drive_geometry_t * buf = (drive_geometry_t *)_buf;
	    res = DEVICE_GET_GEOMETRY(buf);
	    if (res != NO_ERROR) {
		res = -EIO;
	    }
	    break;
	}

	case IO_ATAPI_GET_CONFIG_MEDIA_PRESENT:
	{
	    int * buf = (int *)_buf;
	    
#ifdef DDOIMP_IO_ATAPI_USE_ATA
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
#if 0
	    /* TODO untested */
	    *(int*)buf = ( (*(volatile unsigned char*) CLIK_PORT & CLIK_CD1) == 0 ? 1 : 0 );
	    if( (int)(tab->priv) == 0 && *(int*)buf == 1 ) {
		ResetAtapi();
		(int)(tab->priv) = InitAtapi();
		*(int*)buf = (int)(tab->priv);
	    }
#endif
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

static Cyg_ErrNo atapi_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len)
{
    Cyg_ErrNo res = ENOERR;
  
    switch (key) {
	default:
	    res = -ENOSYS;
	    break;
    }
    return res;
}
