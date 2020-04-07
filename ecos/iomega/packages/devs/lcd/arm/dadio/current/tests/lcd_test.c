#include <pkgconf/io_pm_arm_dadio.h>
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/io/io.h>
#include <dadio/io/pm.h>
#include <dadio/io/lcd.h>

/* cyg_io_* calls return without calling the handler if len == 0, that's why the set_config calls
 * look like they do */

//#undef INCLUDE_KBD_SUPPORT
#define INCLUDE_KBD_SUPPORT
#ifdef INCLUDE_KBD_SUPPORT
#include <dadio/io/kbd_support.h>
#endif /* INCLUDE_KBD_SUPPORT */

#define STACKSIZE 4096
char stack[STACKSIZE];
cyg_handle_t thread;
cyg_thread thread_obj;

/* lcd info */
#define BITS_PER_PIXEL   1
#define PIXEL_WIDTH     98
#define PIXEL_HEIGHT    64
#define BYTE_HEIGHT	( PIXEL_HEIGHT/8 )
#define BYTE_WIDTH 	( PIXEL_WIDTH )
#define DISPLAY_BUF_SIZE ( (PIXEL_HEIGHT/8) * (PIXEL_WIDTH) )
#define INVERT_BUF
cyg_uint8 ucBuf[DISPLAY_BUF_SIZE];

void
lcd_thread(cyg_addrword_t data)
{
    cyg_io_handle_t lcdH;
    cyg_io_handle_t pmH;
    cyg_uint32 len;
    
    CYG_TEST_INFO("Starting LCD test");
    len = 4;
#ifdef INCLUDE_KBD_SUPPORT
    kbd_init();
#endif /* INCLUDE_KBD_SUPPORT */

    if (cyg_io_lookup("/dev/lcd", &lcdH) != ENOERR) {
	CYG_TEST_EXIT("Could not get handle to lcd device");
	return;
    }
    diag_printf("Got handle to /dev/lcd\n");
    if (cyg_io_lookup("/dev/pm", &pmH) != ENOERR) {
	CYG_TEST_EXIT("Could not get handle to pm device");
	return;
    }
    diag_printf("Got handle to /dev/pm\n");
    
    /* test registering/unregistering one device */
    CYG_TEST_INFO("Test register/unregister of one device");
    if (cyg_io_set_config(lcdH, IO_PM_SET_CONFIG_REGISTER, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not register lcd device");
    }
    diag_printf("Registered /dev/lcd\n");

    cyg_io_set_config(lcdH, IO_LCD_SET_CONFIG_CLEAR_DISPLAY, &len, &len);
    cyg_io_set_config(lcdH, IO_LCD_SET_CONFIG_INVERT_DISPLAY, &len, &len);
    diag_printf("Entire LCD turned on\n");

    diag_printf("Unregistering /dev/lcd\n");
    if (cyg_io_set_config(lcdH, IO_PM_SET_CONFIG_UNREGISTER, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not unregister lcd device");
    }

    /* test power down, up all */
    CYG_TEST_INFO("Test power down/up all");
    if (cyg_io_set_config(lcdH, IO_PM_SET_CONFIG_REGISTER, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not register lcd device");
    }
    diag_printf("Registered /dev/lcd\n");
    diag_printf("About to power down all devices\n");
    if (cyg_io_set_config(pmH, IO_PM_SET_CONFIG_POWER_DOWN_ALL, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not power down all");
    }
    diag_printf("Powering up all devices\n");
    if (cyg_io_set_config(pmH, IO_PM_SET_CONFIG_POWER_UP_ALL, &len, &len) != ENOERR) {
	CYG_TEST_EXIT("Could not power up all");
    }

    CYG_TEST_PASS_FINISH("LCD OK");
}

void
cyg_user_start(void)
{
    CYG_TEST_INIT();
    cyg_thread_create(10, lcd_thread, (cyg_addrword_t)0, "lcd_thread",
		      (void *)stack, STACKSIZE, &thread, &thread_obj);
    cyg_thread_resume(thread);
}
