//=============================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// INSERT COPYRIGHT HERE
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   toddm
// Contributors:
// Date:        2000-06-12
// Purpose:     Top level USB driver.
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#include <cyg/kernel/kapi.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/devtab.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/kernel/test/stackmon.h>

#include <dadio/io/pdiusbd12.h>
#include <dadio/io/usb.h>
#include <dadio/io/isr.h>
#include <dadio/io/usb_shared_data.h>

//#define CONTROL_THREAD_STACK_SIZE (768 + 512) /* first number is very tight stack size, could overflow */
//#define BULK_THREAD_STACK_SIZE (1536 + 512)   /* second number is room for overflow */
#define CONTROL_THREAD_STACK_SIZE (32 * 1024)
#define BULK_THREAD_STACK_SIZE    (64 * 1024)
#define MASTER_THREAD_STACK_SIZE  (16 * 1024)
#define USB_IDLE_THREAD_STACK_SIZE (2 * 1024)

#define MASTER_THREAD_PRIORITY   7
#define CONTROL_THREAD_PRIORITY  8
#define BULK_THREAD_PRIORITY     8
#define USB_IDLE_THREAD_PRIORITY 9

cyg_handle_t control_threadH, bulk_threadH, master_threadH, usb_idle_threadH;
cyg_thread control_thread_obj, bulk_thread_obj, master_thread_obj, usb_idle_thread_obj;
char control_thread_stack[CONTROL_THREAD_STACK_SIZE], bulk_thread_stack[BULK_THREAD_STACK_SIZE],
    master_thread_stack[MASTER_THREAD_STACK_SIZE], usb_idle_thread_stack[USB_IDLE_THREAD_STACK_SIZE];

cyg_sem_t master_sem;
cyg_mutex_t bulk_thread_safe_to_reset;
bool usb_idle_thread_running = false;
static void (*callback)(int key) = 0;
void usb_idle_thread(cyg_addrword_t data);
void master_thread(cyg_addrword_t data);
extern void control_thread(cyg_addrword_t data);
extern void bulk_thread(cyg_addrword_t data);
extern void transmit_packet(usb_endp_t * endp, bool start_transmitting);
extern void receive_packet(usb_endp_t * endp, bool start_receiving);

static cyg_interrupt usb_interrupt;
static cyg_handle_t usb_interrupt_handler;

usb_endp_t usb_endpoint[3] =
{
    {0, ENDP0_PACKET_SIZE, false, false},
    {1, ENDP1_PACKET_SIZE, false, false},
    {2, ENDP2_PACKET_SIZE, false, false},
};

static Cyg_ErrNo usb_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len);
static Cyg_ErrNo usb_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
static Cyg_ErrNo usb_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo usb_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len);

DEVIO_TABLE(usb_devio,
            usb_write,
            usb_read,
            usb_get_config,
            usb_set_config
    );

static bool usb_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo usb_lookup(struct cyg_devtab_entry **tab, 
			    struct cyg_devtab_entry *sub_tab,
			    const char *name);

DEVTAB_ENTRY(iome_usb0, 
             "/dev/usb0",	/* WARNING do not change this string */
             0,			/* see iosys.c in the eCos src for more info */
             &usb_devio,
             usb_init, 
             usb_lookup,
             &usb_endpoint[0]
     );

DEVTAB_ENTRY(iome_usb2,
	     "/dev/usb2",	/* WARNING see comment above */
	     0,
	     &usb_devio,
	     usb_init,
	     usb_lookup,
	     &usb_endpoint[2]
    );

static bool
usb_init(struct cyg_devtab_entry *tab)
{
    usb_endp_t * endp = (usb_endp_t *)tab->priv;

    if (endp->init)
	return true;

    cyg_semaphore_init(&endp->in.sem, 0);
    cyg_semaphore_init(&endp->out.sem, 0);
    endp->init = true;
    
    if (endp->num == 0) {
	/* create master thread and start it
	 * priority must be higher than other usb threads */
	cyg_semaphore_init(&master_sem, 0);
	cyg_thread_create(MASTER_THREAD_PRIORITY, master_thread, (cyg_addrword_t)0,
			  "master_thread", (void *)master_thread_stack, MASTER_THREAD_STACK_SIZE,
			  &master_threadH, &master_thread_obj);
	cyg_thread_resume(master_threadH);

	/* create control thread and start it */
	cyg_thread_create(CONTROL_THREAD_PRIORITY, control_thread, (cyg_addrword_t)0,
			  "control_thread", (void *)control_thread_stack, CONTROL_THREAD_STACK_SIZE,
			  &control_threadH, &control_thread_obj);
	cyg_thread_resume(control_threadH);

	/* create bulk transfer thread and start it */
	cyg_mutex_init(&bulk_thread_safe_to_reset);/* this is used to prevent bulk transfer thread
						    * from being killed while accessing the mmc */
	cyg_thread_create(BULK_THREAD_PRIORITY, bulk_thread, (cyg_addrword_t)0,
			  "bulk_thread", (void *)bulk_thread_stack, BULK_THREAD_STACK_SIZE,
			  &bulk_threadH, &bulk_thread_obj);
	cyg_thread_resume(bulk_threadH);

	/* create interrupt and attach it */
	cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EINT1,
				 10, /* Priority */
				 (cyg_addrword_t)usb_endpoint,
				 usb_isr,
				 usb_dsr,
				 &usb_interrupt_handler,
				 &usb_interrupt);
	cyg_drv_interrupt_attach(usb_interrupt_handler);
#if 0
	cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT1); /* TODO move this into ioctl for sync purposes? */
#endif

	/* configure controller */
	d12_set_mode(0, D12_SET_TO_ONE | D12_CLOCK_12MHZ);
	d12_set_dma(D12_ENDP4_INT_ENABLE | D12_ENDP5_INT_ENABLE);
	d12_set_mode(D12_SOFT_CONNECT, D12_SET_TO_ONE | D12_CLOCK_12MHZ);

	/* put controller in suspend mode */
	*(volatile cyg_uint8 *)PBDDR |= 0x10;/* set as output */
	*(volatile cyg_uint8 *)PBDR |= 0x10;
    }
    return true;
}

static Cyg_ErrNo usb_lookup(struct cyg_devtab_entry **tab, 
			    struct cyg_devtab_entry *sub_tab,
			    const char *name)
{
  return ENOERR;
}

static Cyg_ErrNo
usb_write(cyg_io_handle_t handle, const void * _buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t * t = (cyg_devtab_entry_t *)handle;
    usb_endp_t * endp = (usb_endp_t *)t->priv;
    cbuf_t * cbuf = (cbuf_t *)&endp->out;
    char * buf = (char *)_buf;
    Cyg_ErrNo res = ENOERR;

    /* write data */
    cyg_drv_dsr_lock();
    cbuf->get = 0;
    cbuf->put = *len;
    cbuf->data = buf;
    transmit_packet(endp, TRANSMIT_START);
    cyg_drv_dsr_unlock();

    /* wait until data has been put on the wire before returning */
    cyg_semaphore_wait(&cbuf->sem);

    return res;
}

static Cyg_ErrNo
usb_read(cyg_io_handle_t handle, void * _buf, cyg_uint32 *len) 
{
    cyg_devtab_entry_t * t = (cyg_devtab_entry_t *)handle;
    usb_endp_t * endp = (usb_endp_t *)t->priv;
    cbuf_t * cbuf = (cbuf_t *)&endp->in;
    Cyg_ErrNo res = ENOERR;
    char * buf = (char *)_buf;

    /* read data */
    cyg_drv_dsr_lock();
    cbuf->get = *len;
    cbuf->put = 0;
    cbuf->data = buf;
    receive_packet(endp, RECEIVE_START);
    cyg_drv_dsr_unlock();

    /* wait until all data has been read */
    cyg_semaphore_wait(&cbuf->sem);

    return res;
}

static Cyg_ErrNo
usb_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len)
{
    return -EPERM;
}

static Cyg_ErrNo
usb_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len) 
{
    Cyg_ErrNo res = ENOERR;
    
    switch (key) {
    	case IO_USB_SET_CONFIG_CALLBACK:
	    if (callback == 0) {
		callback = buf;
		cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT1);
	    }
	    else {
		callback = buf;
	    }
	    break;

	default:
	    res = -EINVAL;
    }
    
    return res;
}

void
start_usb_idle_thread(void)
{
    if (usb_idle_thread_running) {
	return;
    }
    else {
	diag_printf("USB inserted\n");
	callback(USB_ENABLED);
	cyg_thread_create(USB_IDLE_THREAD_PRIORITY, usb_idle_thread, (cyg_addrword_t)0,
			  "usb_idle_thread", (void *)usb_idle_thread_stack, USB_IDLE_THREAD_STACK_SIZE,
			  &usb_idle_threadH, &usb_idle_thread_obj);
	cyg_thread_resume(usb_idle_threadH);
	usb_idle_thread_running = true;
    }
}

void
stop_usb_idle_thread(void)
{
    if (usb_idle_thread_running) {
	diag_printf("USB removed\n");
	callback(USB_DISABLED);
	cyg_thread_kill(usb_idle_threadH);
	usb_idle_thread_running = false;
    }
}

void
usb_idle_thread(cyg_addrword_t data)
{
    for (;;)
	;
}

void
master_thread(cyg_addrword_t data)
{
    for (;;) {
	cyg_semaphore_wait(&master_sem);
	diag_printf("master_thread\n");

	/* kill usb idle thread */
	stop_usb_idle_thread();
	
	/* restart control thread */
	cyg_thread_kill(control_threadH);
	cyg_thread_create(CONTROL_THREAD_PRIORITY, control_thread, (cyg_addrword_t)0,
			  "control_thread", (void *)control_thread_stack, CONTROL_THREAD_STACK_SIZE,
			  &control_threadH, &control_thread_obj);
	cyg_thread_resume(control_threadH);

	/* restart bulk transfer thread */
	cyg_mutex_lock(&bulk_thread_safe_to_reset);
	cyg_thread_kill(bulk_threadH);
	cyg_thread_create(BULK_THREAD_PRIORITY, bulk_thread, (cyg_addrword_t)0,
			  "bulk_thread", (void *)bulk_thread_stack, BULK_THREAD_STACK_SIZE,
			  &bulk_threadH, &bulk_thread_obj);
	cyg_thread_resume(bulk_threadH);
	cyg_mutex_unlock(&bulk_thread_safe_to_reset);
    }
}

void
dump_stack_stats(void)
{
    cyg_test_dump_thread_stack_stats("control_thread", control_threadH);
    cyg_test_dump_thread_stack_stats("bulk_thread", bulk_threadH);
    cyg_test_dump_thread_stack_stats("master_thread", master_threadH);
}
