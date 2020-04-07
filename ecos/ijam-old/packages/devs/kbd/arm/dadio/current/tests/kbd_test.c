#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/io/io.h>
#include <dadio/io/kbd_support.h>
#include <dadio/io/pm.h>

#define STACK_SIZE (4 * 4096)

static cyg_handle_t thread;
static cyg_thread thread_obj;
static char stack[STACK_SIZE];

void
kbdtest(CYG_ADDRESS data)
{
    cyg_uint8 kbd_event;
    cyg_io_handle_t pm_devH;
    Cyg_ErrNo err;
    cyg_uint32 len;
    
    CYG_TEST_INFO("Starting kbd test");
    
    kbd_init();
    kbd_event = 0;
    while (kbd_event != 0x2) {
	kbd_event = (cyg_uint8)cyg_mbox_get(kbd_events_mbox_handle);
	diag_printf("%3x\n", kbd_event);
    }
    err = cyg_io_lookup("/dev/pm", &pm_devH);
    len = sizeof(len);
    err = cyg_io_set_config(pm_devH, IO_PM_SET_CONFIG_POWER_DOWN_ALL, &len, &len);
    diag_printf("woken up\n");
    err = cyg_io_set_config(pm_devH, IO_PM_SET_CONFIG_POWER_UP_ALL, &len, &len);
    while (1) {
	kbd_event = (cyg_uint8)cyg_mbox_get(kbd_events_mbox_handle);
	diag_printf("%3x\n", kbd_event);
    }

    CYG_TEST_PASS_FINISH("kbd OK");
}

void
cyg_user_start(void)
{
    CYG_TEST_INIT();
    cyg_thread_create(10, kbdtest, (cyg_addrword_t)0, "kbdtest",
		      (void *)stack, STACK_SIZE, &thread, &thread_obj);
    cyg_thread_resume(thread);
}
