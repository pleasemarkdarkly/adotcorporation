#include <pkgconf/io_pm_arm_dadio.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <dadio/io/pm.h>
#include <cyg/hal/hal_cache.h>

static bool pm_init(struct cyg_devtab_entry * tab);
static Cyg_ErrNo pm_lookup(struct cyg_devtab_entry ** tab,
			   struct cyg_devtab_entry * sub_tab,
			   const char * name);
static Cyg_ErrNo pm_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 * len);
static Cyg_ErrNo pm_read(cyg_io_handle_t handle, void * buf, cyg_uint32 * len);
static Cyg_ErrNo pm_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len);
static Cyg_ErrNo pm_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * len);
static void pm_standby(void);

DEVIO_TABLE(io_pm_devio,
	    pm_write,
	    pm_read,
	    pm_get_config,
	    pm_set_config
    );

typedef struct 
{
    cyg_io_handle_t devH;
    int ref_cnt;
} pm_table_entry_t;
static pm_table_entry_t pm_table[DDONUM_IO_PM_TABLE_ENTRIES];
DEVTAB_ENTRY(io_pm_devtab,
	     DDODAT_IO_PM_NAME,
	     0,
	     &io_pm_devio,
	     pm_init,
	     pm_lookup,
	     &pm_table
    );

static bool
pm_init(struct cyg_devtab_entry * tab)
{
    int i;
    /* clear out registered devices table */
    for (i = 0; i < DDONUM_IO_PM_TABLE_ENTRIES; ++i) {
	pm_table[i].devH = 0;
	pm_table[i].ref_cnt = 0;
    }
    return true;
}

static Cyg_ErrNo
pm_lookup(struct cyg_devtab_entry ** tab,
	  struct cyg_devtab_entry * sub_tab,
	  const char * name)
{
    /* nothing to do here */
    return ENOERR;
}

static Cyg_ErrNo
pm_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 * len)
{
    return -ENOSYS;
}

static Cyg_ErrNo
pm_read(cyg_io_handle_t handle, void * buf, cyg_uint32 * len)
{
    return -ENOSYS;
}

static Cyg_ErrNo
pm_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len)
{
    Cyg_ErrNo res = ENOERR;
    
    switch (key)
    {
	/* no get configs supported at this time */
	default:
	    res = -EINVAL;
	    break;
    }
    return res;
}

static Cyg_ErrNo
pm_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * _len)
{
    cyg_uint32 len;
    int i;
    static bool devices_powered_down = false;
    
    switch (key)
    {
	case IO_PM_SET_CONFIG_REGISTER:
	    /* do first pass to see if device is already in table */
	    for (i = 0; i < DDONUM_IO_PM_TABLE_ENTRIES; ++i) {
		if (pm_table[i].devH == (cyg_io_handle_t)buf) {
		    /* found it */
		    ++pm_table[i].ref_cnt;
		    return ENOERR;
		}
	    }
	    
	    /* device not in table, find a free entry and add it */
	    for (i = 0; i < DDONUM_IO_PM_TABLE_ENTRIES; ++i) {
		if (pm_table[i].devH == 0) {
		    /* free entry */
		    pm_table[i].devH = (cyg_io_handle_t)buf;
		    pm_table[i].ref_cnt = 1;
		    len = sizeof(len);
		    if (cyg_io_set_config(pm_table[i].devH, IO_PM_SET_CONFIG_POWER_UP, &len, &len) != ENOERR) {
			diag_printf("ERROR: Could not power up device\n");
		    }
		    return ENOERR;
		}
	    }
	    diag_printf("No free entries in pm table, could not register device\n");
	    return -EMFILE;	/* too many open files. either this or ENOMEM, this seemed more appropriate */
	    break;

	case IO_PM_SET_CONFIG_UNREGISTER:
	    for (i = 0; i < DDONUM_IO_PM_TABLE_ENTRIES; ++i) {
		if (pm_table[i].devH == (cyg_io_handle_t)buf) {
		    /* entry found */
		    --pm_table[i].ref_cnt;
		    if (pm_table[i].ref_cnt < 0) {
			diag_printf("WARNING: Negative reference count after unregistering device\n");
			pm_table[i].ref_cnt = 0;
		    }
		    if (pm_table[i].ref_cnt == 0) {
			/* tell device to power itself off */
			len = sizeof(len);
			if (cyg_io_set_config(pm_table[i].devH, IO_PM_SET_CONFIG_POWER_DOWN, &len, &len) != ENOERR) {
			    diag_printf("WARNING: Could not power down device\n");
			}
			/* clear entry from pm table */
			pm_table[i].devH = 0;
		    }
		    return ENOERR;
		}
	    }
	    return -ENODEV;	/* no such device */
	    break;

	case IO_PM_SET_CONFIG_POWER_DOWN_ALL:
	    if (!devices_powered_down) {
		/* power down all devices */
		for (i = 0; i < DDONUM_IO_PM_TABLE_ENTRIES; ++i) {
		    if (pm_table[i].devH) {
			len = sizeof(len);
			if (cyg_io_set_config(pm_table[i].devH, IO_PM_SET_CONFIG_POWER_DOWN, &len, &len) != ENOERR) {
			    diag_printf("WARNING: Could not power down device\n");
			}
			
			/* don't touch reference count, but warn if reference count is > 1 */
			if (pm_table[i].ref_cnt > 1) {
			    diag_printf("WARNING: Powering down device whose reference count is > 1\n");
			}
		    }
		}
		devices_powered_down = true;
	    }

	    /* power down CPU */
	    if ((*(int *)buf) != SOFT_RESET) {
		pm_standby();
	    }
	    else {
		void (*s_reset)(void) = hal_vsr_table[0];		
		HAL_UCACHE_SYNC();
		HAL_UCACHE_DISABLE();
		HAL_UCACHE_INVALIDATE_ALL();
		cyg_interrupt_disable();
		s_reset();
	    }
	    return ENOERR;
	    break;

	case IO_PM_SET_CONFIG_POWER_UP_ALL:
	    if (devices_powered_down) {
		/* power up all devices */
		/* CPU should already be powered up since this code is running */
//		cyg_scheduler_lock();/* make sure that all devices are powered back up before
//				      * allowing any to run */
		for (i = 0; i < DDONUM_IO_PM_TABLE_ENTRIES; ++i) {
		    if (pm_table[i].devH) {
			len = sizeof(len);
			if (cyg_io_set_config(pm_table[i].devH, IO_PM_SET_CONFIG_POWER_UP, &len, &len) != ENOERR) {
			    diag_printf("ERROR: Could not power up device\n");
			}
			/* don't touch reference count */
		    }
		}
//		cyg_scheduler_unlock();
		devices_powered_down = false;
	    }
	    return ENOERR;
	    break;
	    
	default:
	    return -EINVAL;
	    break;
    }
}

static void
pm_standby(void)
{
    /* can write anything to the stdby register to put the board into deep sleep */
    *((unsigned int *)STDBY) = 1;
    /* need two no-ops after coming out of wakeup */
    asm volatile ("nop;"
		  "nop;"
	);
}


