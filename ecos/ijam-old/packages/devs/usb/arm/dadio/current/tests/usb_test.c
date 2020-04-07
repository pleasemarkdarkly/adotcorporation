#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/io/io.h>
#include <dadio/io/usb.h>

#define NTHREADS 1
#define STACKSIZE (16 * 4096)

static cyg_handle_t thread[NTHREADS];
static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];

static void
usb_callback(int key)
{
    diag_printf("callback %d\n", key);
}

void
prog(CYG_ADDRESS data)
{
    cyg_io_handle_t usbH;
    cyg_uint32 len;
    
    cyg_io_lookup("/dev/usb0", &usbH);
    len = sizeof(usb_callback);
    cyg_io_set_config(usbH, IO_USB_SET_CONFIG_CALLBACK, usb_callback, &len);
    
    for (;;) {
	diag_printf("P\n");
	cyg_thread_delay(500);
    }
}

void
cyg_user_start(void)
{
    cyg_thread_create(1, prog, (cyg_addrword_t) 0, "prog",
		      (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);    
}
