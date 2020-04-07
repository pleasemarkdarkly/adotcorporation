#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/devtab.h>
#include <dadio/io/usb.h>

#define STACK_SIZE (4 * 4096)

static cyg_handle_t thread;
static cyg_thread thread_obj;
static char stack[STACK_SIZE];

void
usb_callback(int key)
{
    diag_printf("key %d\n", key);
}

void
usb_test(CYG_ADDRESS data)
{
    cyg_io_handle_t usbH;
    cyg_uint32 len;
    Cyg_ErrNo status;
    
    diag_printf("usb_test\n");

    status = cyg_io_lookup("/dev/usb", &usbH);
    if (status != ENOERR) {
	diag_printf("Error getting handle to /dev/usb\n");
    }

    len = sizeof(usb_callback);
    status = cyg_io_set_config(usbH, IO_USB_SET_CONFIG_CALLBACK, usb_callback, &len);
    if (status != ENOERR) {
	diag_printf("Error setting callback function for usb\n");
    }
}


void
cyg_user_start(void)
{
    cyg_thread_create(10, usb_test, (cyg_addrword_t)0, "usb_test",
		      (void *)stack, STACK_SIZE, &thread, &thread_obj);
    cyg_thread_resume(thread);
}
