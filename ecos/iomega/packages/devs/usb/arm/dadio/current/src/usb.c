#include <pkgconf/io_usb_arm_dadio.h>
#include <pkgconf/io_pm_arm_dadio.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/hal/drv_api.h>
#include <cyg/infra/diag.h>
#include <cyg/io/devtab.h>
#include <dadio/io/usb.h>
#include <dadio/io/pm.h>
#include <dadio/io/atapi.h>

//#define INCLUDE_WATCHDOG

#ifdef INCLUDE_WATCHDOG
extern void cyg_watchdog_reset(void);
#endif // INCLUDE_WATCHDOG

#define STACK_SIZE (4 * 4096)
#define PHASE_3

static char usb_thread_stack[STACK_SIZE];
static cyg_thread usb_thread_data;
static cyg_handle_t usb_thread_handle;

static cyg_interrupt usb_interrupt;
static cyg_handle_t usb_interrupt_handle;

static cyg_sem_t usb_sem;
static void (*callback)(int key) = 0;
static cyg_io_handle_t pm_devH = 0;

static bool usb_init(struct cyg_devtab_entry * tab);
static Cyg_ErrNo usb_lookup(struct cyg_devtab_entry ** tab,
			    struct cyg_devtab_entry * sub_tab,
			    const char * name);
static Cyg_ErrNo usb_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 * len);
static Cyg_ErrNo usb_read(cyg_io_handle_t handle, void * buf, cyg_uint32 * len);
static Cyg_ErrNo usb_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len);
static Cyg_ErrNo usb_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * len);
static int usb_isr(cyg_vector_t vector, cyg_addrword_t data);
static void usb_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static void usb_thread(cyg_addrword_t data);
static void usb_delay(void);

DEVIO_TABLE(io_usb_devio,
	    usb_write,
	    usb_read,
	    usb_get_config,
	    usb_set_config
    );

DEVTAB_ENTRY(io_usb_devtab,
	     DDODAT_IO_USB_NAME,
	     DDODAT_IO_PM_NAME,
	     &io_usb_devio,
	     usb_init,
	     usb_lookup,
	     0
    );

static void
usb_delay(void)
{
    volatile int delay;
    for (delay = 0; delay < 250; ++delay)
	;
}

static int
usb_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EINT1);
    return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

static void
usb_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_semaphore_post(&usb_sem);
}

static void
usb_thread(cyg_addrword_t data)
{
#ifdef PHASE_3
    cyg_io_handle_t blk_devH;
    cyg_uint32 len;
    Cyg_ErrNo res;
    volatile unsigned int delay;
#endif /* PHASE_3 */
    cyg_tick_count_t timeout;
    cyg_tick_count_t current_time;
        
    diag_printf("usb_thread\n");

#ifdef PHASE_3
    /* get handle to clik drive for power management purposes */
    cyg_io_lookup("/dev/hda", &blk_devH);
#endif /* PHASE_3 */
    
    for (;;) {
	/* wait for usb insertion interrupt */
	cyg_semaphore_wait(&usb_sem);
	diag_printf("usb interrupt received\n");
	/* de-bounce VBUS_SENSE */
	usb_delay();
	/* once VBUS_SENSE goes high, we have a good connection */
	timeout = cyg_current_time() + 300; /* wait for 3 seconds for VBUS to go high */
	do {
	    current_time = cyg_current_time();
	} while (!(*(volatile cyg_uint8 *)PBDR & 0x80) && (current_time < timeout));

	if (current_time < timeout) {

	    diag_printf("VBUS_SENSE high, ATA_EN set low\n");
	    if (callback) {
		callback(USB_ENABLED);
	    }
	    
#ifdef PHASE_3
	    /* register clik drive in use */
	    /* as a side effect, this will also turn on power to mystic
	     * since they are both controlled by the same pin */
	    len = sizeof(len);
	    cyg_io_set_config(blk_devH, IO_PM_SET_CONFIG_REGISTER, &len, &len);

	    /* force clik drive to sleep */
	    res = cyg_io_get_config(blk_devH, IO_ATAPI_GET_CONFIG_MEDIA_STATUS, &len, &len);
	    if (res != -ESLEEP) {
		cyg_io_set_config(blk_devH, IO_ATAPI_SET_CONFIG_SLEEP, &len, &len);
	    }

	    /* power cycle clik drive
	     * theoretically all the power management should be down through register
	     * and unregister ioctls.  apparently this power cycle is necessary to keep
	     * the host from crashing */
	    *(volatile cyg_uint8 *)PDDR |= 0x04;
	    /* wait 10 ms */
	    for (delay = 0; delay < 10 * 0x4800; ++delay)
		;
	    *(volatile cyg_uint8 *)PDDR &= ~0x04;
#endif /* PHASE_3 */
	
	    /* enable mystic */
	    *(volatile cyg_uint8 *)PBDR &= ~0x02;

	    /* mystic is running, so wait for unplug */
	    while (*(volatile cyg_uint8 *)PBDR & 0x80) {
#ifdef INCLUDE_WATCHDOG
		cyg_watchdog_reset();
#endif /* INCLUDE_WATCHDOG */
	    }
	    diag_printf("usb unplugged\n");
	
	    /* disable mystic */
	    *(volatile cyg_uint8 *)PBDR |= 0x02;
	    diag_printf("ATA_EN set high\n");
	    if (callback) {
		callback(USB_DISABLED);
	    }

#ifdef PHASE_3
	    /* unregister clik drive in use */
	    /* as a side effect this may turn off mystic, but only if the clik
	     * drive actually gets turned off too */
	    len = sizeof(len);
	    cyg_io_set_config(blk_devH, IO_PM_SET_CONFIG_UNREGISTER, &len, &len);
#endif /* PHASE_3 */
	}
	else {
	    diag_printf("USB timed out waiting for VBUS_SENSE\n");
	}
	cyg_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT1);
    }
}

static bool
usb_init(struct cyg_devtab_entry * tab)
{
    diag_printf("usb_init\n");
    
    cyg_semaphore_init(&usb_sem, 0);
    
    cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EINT1,
			     10,
			     0,
			     (cyg_ISR_t *)usb_isr,
			     (cyg_DSR_t *)usb_dsr,
			     &usb_interrupt_handle,
			     &usb_interrupt);
    cyg_drv_interrupt_attach(usb_interrupt_handle);
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_EINT1);
    /* wait till /dev/usb is registered to unmask interrupt */

    cyg_thread_create(5,	/* higher priority thread than anything else
				 * this is so that everything else is stopped */
		      usb_thread,
		      0,
		      "usb_thread",
		      &usb_thread_stack[0],
		      STACK_SIZE,
		      &usb_thread_handle,
		      &usb_thread_data
	);
    cyg_thread_resume(usb_thread_handle);

    diag_printf("usb_init done\n");
    return true;
}

static Cyg_ErrNo
usb_lookup(struct cyg_devtab_entry ** tab,
	   struct cyg_devtab_entry * sub_tab,
	   const char * name)
{
    if (pm_devH == 0) {
	pm_devH = (cyg_io_handle_t)sub_tab;
    }
    return ENOERR;
}

static Cyg_ErrNo
usb_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 * len)
{
    return -ENOSYS;
}

static Cyg_ErrNo
usb_read(cyg_io_handle_t handle, void * buf, cyg_uint32 * len)
{
    return -ENOSYS;
}

static Cyg_ErrNo
usb_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len)
{
    return -EINVAL;
}

static Cyg_ErrNo
usb_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * len)
{
    Cyg_ErrNo ret_val = ENOERR;
    cyg_uint32 _len;
    
    switch (key) {
	case IO_USB_SET_CONFIG_CALLBACK:
	    callback = buf;
	    break;

	case IO_PM_SET_CONFIG_POWER_DOWN:
	    diag_printf("Powering down usb device\n");
	    /* device actually manages its own power inside usb_thread
	     * mask the interrupt here so that the device won't wakeup when
	     * usb is plugged in when in off mode */
	    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EINT1);
	    return ENOERR;

	case IO_PM_SET_CONFIG_POWER_UP:
	    diag_printf("Powering up usb device\n");
	    /* see comment in POWER_DOWN */
	    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT1);

	    /* user may have plugged in usb while asleep, so check for that
	     * and run the usb if they did */
	    if (*(volatile cyg_uint8 *)PBDR & 0x80) {
		cyg_semaphore_post(&usb_sem);
	    }
	    return ENOERR;

	    /* handle these by passing them down to the pm device */
	case IO_PM_SET_CONFIG_REGISTER:
	case IO_PM_SET_CONFIG_UNREGISTER:
	    _len = sizeof(cyg_io_handle_t);
	    return cyg_io_set_config(pm_devH, key, handle, &_len);
	    
	default:
	    ret_val = -EINVAL;
	    break;
    }
    return ret_val;
}
