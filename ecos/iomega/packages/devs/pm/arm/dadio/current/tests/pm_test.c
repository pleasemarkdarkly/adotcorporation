#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/io/io.h>
#include <dadio/io/pm.h>
#include <pkgconf/io_pm_arm_dadio.h>

/*
 * generic driver to test pm reference counting
 */

#include <cyg/io/devtab.h>
#include <dadio/io/pm.h>
#include "generic.h"

static bool generic_init(struct cyg_devtab_entry * tab);
static Cyg_ErrNo generic_lookup(struct cyg_devtab_entry ** tab,
			   struct cyg_devtab_entry * sub_tab,
			   const char * name);
static Cyg_ErrNo generic_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 * len);
static Cyg_ErrNo generic_read(cyg_io_handle_t handle, void * buf, cyg_uint32 * len);
static Cyg_ErrNo generic_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len);
static Cyg_ErrNo generic_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * len);

typedef struct
{
    cyg_io_handle_t pm_devH;
} generic_private_info_t;

DEVIO_TABLE(io_generic_devio,
	    generic_write,
	    generic_read,
	    generic_get_config,
	    generic_set_config
    );

static generic_private_info_t generic_private_info; 
DEVTAB_ENTRY(io_generic_devtab,
	     IO_GENERIC_ARM_DADIO_NAME,
	     DDODAT_IO_PM_NAME,
	     &io_generic_devio,
	     generic_init,
	     generic_lookup,
	     &generic_private_info
    );

static bool
generic_init(struct cyg_devtab_entry * tab)
{
    /* don't need to do anything at this stage to enable power management */
    return true;
}

static Cyg_ErrNo
generic_lookup(struct cyg_devtab_entry ** tab,
	       struct cyg_devtab_entry * sub_tab,
	       const char * name)
{
    /* set pointer to pm device in private data */
    generic_private_info_t * priv = (generic_private_info_t *)(*tab)->priv;
    priv->pm_devH = (cyg_io_handle_t)sub_tab;
    return ENOERR;
}

static Cyg_ErrNo
generic_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 * len)
{
    return -ENOSYS;
}

static Cyg_ErrNo
generic_read(cyg_io_handle_t handle, void * buf, cyg_uint32 * len)
{
    return -ENOSYS;
}

static Cyg_ErrNo
generic_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len)
{
    return -ENOSYS;
}

static Cyg_ErrNo
generic_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * _len)
{
    cyg_devtab_entry_t * t = (cyg_devtab_entry_t *)handle;
    generic_private_info_t * priv = (generic_private_info_t *)t->priv;
    cyg_uint32 len;
    
    switch (key)
    {
	case IO_PM_SET_CONFIG_POWER_DOWN:
	    diag_printf("Powering down generic device\n");
	    return ENOERR;

	case IO_PM_SET_CONFIG_POWER_UP:
	    diag_printf("Powering up generic device\n");
	    return ENOERR;

	    /* handle these by passing them down to the pm device */
	case IO_PM_SET_CONFIG_REGISTER:
	case IO_PM_SET_CONFIG_UNREGISTER:
	    len = sizeof(cyg_io_handle_t);
	    return cyg_io_set_config(priv->pm_devH, key, handle, &len);
		
	default:
	    return -EINVAL;
    }
}

/*
 * main test program
 */

//#undef INCLUDE_KBD_SUPPORT
#define INCLUDE_KBD_SUPPORT
#ifdef INCLUDE_KBD_SUPPORT
#include <dadio/io/kbd_support.h>
#endif /* INCLUDE_KBD_SUPPORT */

#define INCLUDE_USB_SUPPORT
#ifdef INCLUDE_USB_SUPPORT
#include <dadio/io/usb.h>
void usb_callback(int key) 
{
    diag_printf("usb_callback %d\n", key);
}
#endif /* INCLUDE_USB_SUPPORT */

/* cyg_io_* calls return without calling the handler if len == 0, that's why the set_config calls
 * look like they do */

#define STACKSIZE 4096
char stack[STACKSIZE];
cyg_handle_t thread;
cyg_thread thread_obj;

void
pm_thread(cyg_addrword_t data)
{
    cyg_io_handle_t genericH, generic2H;
    cyg_io_handle_t pmH;
    cyg_uint32 len = sizeof(cyg_uint32);
#ifdef INCLUDE_USB_SUPPORT
    cyg_io_handle_t usbH;
#endif /* INCLUDE_USB_SUPPORT */

    CYG_TEST_INFO("Starting pm test");
#ifdef INCLUDE_KBD_SUPPORT
    kbd_init();
#endif /* INCLUDE_KBD_SUPPORT */
#ifdef INCLUDE_USB_SUPPORT
    cyg_io_lookup("/dev/usb", &usbH);
    len = sizeof(usb_callback);
    cyg_io_set_config(usbH, IO_USB_SET_CONFIG_CALLBACK, usb_callback, &len);
    cyg_io_set_config(usbH, IO_PM_SET_CONFIG_REGISTER, &len, &len);
#endif /* INCLUDE_USB_SUPPORT */
    if (cyg_io_lookup("/dev/generic", &genericH) != ENOERR) {
	CYG_TEST_EXIT("Could not get handle to generic device");
	return;
    }
    diag_printf("Got handle to /dev/generic\n");
    if (cyg_io_lookup("/dev/pm", &pmH) != ENOERR) {
	CYG_TEST_EXIT("Could not get handle to pm device");
	return;
    }
    diag_printf("Got handle to /dev/pm\n");

    /* test registering/unregistering one device */
    CYG_TEST_INFO("Test register/unregister of one device");
    if (cyg_io_set_config(genericH, IO_PM_SET_CONFIG_REGISTER, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not register generic device");
    }
    diag_printf("Registered /dev/generic\n");
    diag_printf("Unregistering /dev/generic\n");
    if (cyg_io_set_config(genericH, IO_PM_SET_CONFIG_UNREGISTER, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not unregister generic device");
    }

    /* test power down, up all */
    CYG_TEST_INFO("Test power down/up all");
    if (cyg_io_set_config(genericH, IO_PM_SET_CONFIG_REGISTER, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not register generic device");
    }
    diag_printf("Registered /dev/generic\n");
    diag_printf("About to power down all devices\n");
    if (cyg_io_set_config(pmH, IO_PM_SET_CONFIG_POWER_DOWN_ALL, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not power down all");
    }
    diag_printf("Powering up all devices\n");
    if (cyg_io_set_config(pmH, IO_PM_SET_CONFIG_POWER_UP_ALL, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not power up all");
    }

    /* test multiple users of device */
    CYG_TEST_INFO("Test reference counting, i.e. multiple users of a single device");
    if (cyg_io_lookup("/dev/generic", &generic2H) != ENOERR) {
	CYG_TEST_EXIT("Could not get handle to generic device");
    }
    diag_printf("Got second handle to /dev/generic\n");
    if (cyg_io_set_config(generic2H, IO_PM_SET_CONFIG_REGISTER, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not register generic2 device");
    }
    diag_printf("Registered second /dev/generic\n");
    /* generic should still be registered from power down, up all test */
    diag_printf("Unregistering /dev/generic\n");
    if (cyg_io_set_config(genericH, IO_PM_SET_CONFIG_UNREGISTER, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not unregister generic device");
    }
    diag_printf("Unregistering second /dev/generic\n");
    if (cyg_io_set_config(generic2H, IO_PM_SET_CONFIG_UNREGISTER, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not unregister generic2H device");
    }

    CYG_TEST_PASS_FINISH("pm OK");
}

void
cyg_user_start(void)
{
    CYG_TEST_INIT();
    cyg_thread_create(10, pm_thread, (cyg_addrword_t)0, "pm_thread",
		      (void *)stack, STACKSIZE, &thread, &thread_obj);
    cyg_thread_resume(thread);
}
