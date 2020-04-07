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
// Purpose:     ISR/DSR for USB driver.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_edb7xxx.h>

#include <dadio/io/pdiusbd12.h>
#include <dadio/io/usb_shared_data.h>
#include <dadio/io/usb.h>
#include <dadio/io/isr.h>

extern void dump_stack_stats(void);
extern cyg_sem_t master_sem;

void bus_reset(void);
void dma_eot(void);
void suspend_change(void);
void control_handler(void);
void rx_handler(usb_endp_t * endp);
void tx_handler(usb_endp_t * endp);
void transmit_packet(usb_endp_t * endp, bool start_transmitting);
void receive_packet(usb_endp_t * endp, bool start_receiving);

cyg_uint32
usb_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EINT1);
    return CYG_ISR_CALL_DSR;
}

void
usb_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    unsigned short int_reg;
    usb_endp_t * endp = (usb_endp_t *)data;

    usb_in_dsr = true;
    int_reg = d12_read_interrupt_register();
    if (int_reg) {
	if (int_reg & D12_INT_BUS_RESET) {
	    bus_reset();
	}
	else {
	    if (int_reg & D12_INT_DMA_EOT) {
		dma_eot();
	    }
	    if (int_reg & D12_INT_SUSPEND_CHANGE) {
		suspend_change();
	    }
	    if (int_reg & D12_INT_ENDP0_IN)
		tx_handler(&endp[0]);
	    if (int_reg & D12_INT_ENDP0_OUT)
		rx_handler(&endp[0]);
	    if (int_reg & D12_INT_ENDP1_IN)
		tx_handler(&endp[1]);
	    if (int_reg & D12_INT_ENDP1_OUT)
		rx_handler(&endp[1]);
	    if (int_reg & D12_INT_ENDP2_IN)
		tx_handler(&endp[2]);
	    if (int_reg & D12_INT_ENDP2_OUT)
		rx_handler(&endp[2]);
	}
    }
    usb_in_dsr = false;
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_EINT1);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT1);
}

void
bus_reset(void)
{
    diag_printf("bus_reset\n");

    /* reset threads */
    cyg_semaphore_post(&master_sem);
}

void
dma_eot(void)
{
    /* should not have received this */
    diag_printf("dma_eot\n");
}

void
suspend_change(void)
{
    cyg_uint8 suspend_pin;
    
    diag_printf("suspend change\n");

    /* read value of suspend pin */
    *(volatile cyg_uint8 *)PBDDR &= ~0x10;/* change to input */
    suspend_pin = *(volatile cyg_uint8 *)PBDR & 0x10;

    /* change value of suspend pin */
    *(volatile cyg_uint8 *)PBDDR |= 0x10;/* change to output */
    if (suspend_pin) {
	diag_printf("leaving suspend state\n");
	*(volatile cyg_uint8 *)PBDR &= ~0x10;
    }
    else {
	diag_printf("entering suspend state\n");
	*(volatile cyg_uint8 *)PBDR |= 0x10;
    }
    
    /* print some diagnostic info */
    dump_stack_stats();
}

void
rx_handler(usb_endp_t * endp)
{
    unsigned char status;
    
    status = d12_read_last_transaction_status(2 * endp->num);
    if (status & D12_SUCCESS) {
#if 0
	/* this happens a lot on bulk writes.
	 * choice is between checking this and calling receive_packet(endp) twice,
	 * or leaving it as currently implemented which is probably a little faster */
	if (status & D12_PREVIOUS_STATUS_NOT_READ) {
	    diag_printf("prev status not read\n");
	}
#endif
	receive_packet(endp, RECEIVE_CONTINUE);
    }
    else {
	diag_printf("endp%d_rx_handler transaction status: %x\n", 2 * endp->num, status);
    }
}

void
receive_packet(usb_endp_t * endp, bool start_receiving)
{
    char buf[128];
    cbuf_t * cbuf = (cbuf_t *)&endp->in;
    unsigned char i = 0, len;
    
    /* if no data is requested, return immediately
     * usb_read will take care of calling this function itself to get data when the
     * interrupt is ignored */
    if (cbuf->get == 0 && !start_receiving)
	return;
    
    /* read data from endpoint */
    if ((endp->num == 2) && (cbuf->get > endp->packet_size)) {
	len = d12_read_endpoint(2 * endp->num, buf, endp->packet_size * 2); /* both buffers could be full */
    }
    else {
	len = d12_read_endpoint(2 * endp->num, buf, endp->packet_size);
    }

    /* more data in a packet than we were expecting */
    if (cbuf->put + len > cbuf->get) {
	diag_printf("USB ERROR %x > %x\n", cbuf->put, cbuf->get);
	{
	    int i;
	    for (i = 0; i < len; ++i)
		diag_printf("%x ", buf[i]);
	    diag_printf("\n");
	}
	/* TODO come up with a better fix than this.
	 * this only prevents us from overwriting the user buffer */
	len = cbuf->get - cbuf->put;
    }

    /* copy packet read into cbuf */
    for (i = 0; i < len; ++i) {
	cbuf->data[cbuf->put++] = buf[i];
    }

    /* if enough data read, wake reader */
    if (cbuf->put == cbuf->get) {
	cbuf->get = 0;
	cyg_semaphore_post(&cbuf->sem);
    }
    /* TODO this else for debugging purposes only */
    else if (cbuf->put > cbuf->get) {
	diag_printf("put %x > get %x\n", cbuf->put, cbuf->get);
	cbuf->get = 0;
	cyg_semaphore_post(&cbuf->sem);
    }
}

void
tx_handler(usb_endp_t * endp)
{
    unsigned char status;

    status = d12_read_last_transaction_status(2 * endp->num + 1);
    if (status & D12_SUCCESS) {
	transmit_packet(endp, TRANSMIT_CONTINUE);
    }
    else {
	diag_printf("endp%d_tx_handler transaction status: %x\n", 2 * endp->num + 1, status);
    }
}

void
transmit_packet(usb_endp_t * endp, bool start_transmitting)
{
    char buf[128];
    cbuf_t * cbuf = (cbuf_t *)&endp->out;
    unsigned char i = 0;
    
    /* the tx_active flag is necessary because on the bulk endpoint (2), a full packet
     * will be transmitted without sending a 0 length ack packet as the next one.  and the
     * tx_active ensures that the semaphore will only get posted to when it should be.
     */
    if (start_transmitting)
	endp->tx_active = true;
    if (!endp->tx_active)
	return;
    
    /* copy buf into local packet buffer */
    while (i < endp->packet_size && cbuf->get != cbuf->put) {
	buf[i++] = cbuf->data[cbuf->get++];
    }

    if (i == endp->packet_size) {
	d12_write_endpoint(2 * endp->num + 1, buf, i);
    }
    else if (i == 0 && endp->num == 2) {
	/* if a 0 length packet is requested write that,
	 * but don't write a 0 length packet at the end of
	 * any other request */
	if (start_transmitting) {
	    d12_write_endpoint(2 * endp->num + 1, buf, i);
	}
	else {
	    endp->tx_active = false;
	    cyg_semaphore_post(&cbuf->sem);
	}
    }
    else {
	d12_write_endpoint(2 * endp->num + 1, buf, i);
	endp->tx_active = false;
	cyg_semaphore_post(&cbuf->sem);
    }
}
