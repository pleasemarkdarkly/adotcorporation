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
// Purpose:     Interface to PDIUSBD12 controller.
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#include <cyg/hal/drv_api.h>
#include <cyg/infra/diag.h>
#include <assert.h>

#include <dadio/io/usb.h>
#include <dadio/io/pdiusbd12.h>
#include <dadio/io/usb_shared_data.h>

#undef DEBUG

#define D12_DATA    ((unsigned char *)(0x40000000))
#define D12_COMMAND ((unsigned char *)(0x40000000 + 1))

/* these (in|out)_usb are a necessary hack to get the timing right on
 * the usb register read and writes.  the times between reads and writes
 * must be 500ns.
 */
static void
out_usb(unsigned char * addr, unsigned char data)
{
    *addr = data;
}

static unsigned char
in_usb(unsigned char * addr)
{
    return *addr;
}

void
d12_set_address_enable(unsigned char address, unsigned char enable)
{
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    out_usb(D12_COMMAND, 0xd0);
    if (enable)
	address |= 0x80;
    out_usb(D12_DATA, address);

    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
}

void
d12_set_endpoint_enable(unsigned char enable) 
{
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    out_usb(D12_COMMAND, 0xd8);
    if (enable)
	out_usb(D12_DATA, 0x01);
    else
	out_usb(D12_DATA, 0x00);

    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
}

void
d12_set_mode(unsigned char config, unsigned char clock_div)
{
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    out_usb(D12_COMMAND, 0xf3);
    out_usb(D12_DATA, config);
    out_usb(D12_DATA, clock_div);
        
    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
}

void
d12_set_dma(unsigned char config)
{
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();
    
    out_usb(D12_COMMAND, 0xfb);
    out_usb(D12_DATA, config);
    
    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
}

unsigned short
d12_read_interrupt_register(void)
{
    unsigned char tmp;
    unsigned short int_reg;
    
#if 0				/* only called from within dsr */
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();
#endif
    
    out_usb(D12_COMMAND, 0xf4);
    tmp = in_usb(D12_DATA);
    int_reg = in_usb(D12_DATA);
    int_reg <<= 8;
    int_reg += tmp;

#if 0
    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
#endif
    
    return int_reg;
}

unsigned char
d12_select_endpoint(unsigned char endpoint)
{
    unsigned char status;

    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    out_usb(D12_COMMAND, endpoint);
    status = in_usb(D12_DATA);

    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();

    return status;
}

unsigned char
d12_read_last_transaction_status(unsigned char endpoint)
{
    unsigned char status;
    
#if 0				/* called within dsr only */
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();
#endif
    
    out_usb(D12_COMMAND, 0x40 + endpoint);
    status = in_usb(D12_DATA);

#if 0
    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
#endif
    return status;
}

unsigned char
d12_read_endpoint_status(unsigned char endpoint)
{
    unsigned char status;

    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    out_usb(D12_COMMAND, 0x80 + endpoint);
    status = in_usb(D12_DATA);
            
    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();

    return status;
}

void
d12_set_endpoint_status(unsigned char endpoint, unsigned char stalled)
{
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    out_usb(D12_COMMAND, 0x40 + endpoint);
    out_usb(D12_DATA, stalled);

    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
}

void
d12_send_resume(void)
{
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();
    
    out_usb(D12_COMMAND, 0xf6);
    
    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
}

unsigned short
d12_read_current_frame_number(void)
{
    unsigned short frame_num, msb;
    
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();
    
    out_usb(D12_COMMAND, 0xf5);
    frame_num = in_usb(D12_DATA);
    msb = in_usb(D12_DATA);
    frame_num += msb << 8;
    
    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();

    return frame_num;
}

unsigned short
d12_read_chip_id(void)
{
    unsigned short tmp, chip_id;
        
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    out_usb(D12_COMMAND, 0xfd);
    chip_id = in_usb(D12_DATA);
    tmp = in_usb(D12_DATA);
    chip_id += tmp << 8;
    
    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();

    return chip_id;
}

unsigned char
d12_read_endpoint(unsigned char endpoint, char * buf, unsigned char len)
{
    unsigned char num_bytes = 0, i, j;
    unsigned char num_bufs = 1;	/* double buf on main endpoint */
    
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    if (endpoint == 4 && len > 64) {	/* may have both buffers full on main endpoint */
	out_usb(D12_COMMAND, 0x84);
	if ((in_usb(D12_DATA) & 0x60) == 0x60) {
	    num_bufs = 2;
	}
    }

    while (num_bufs) {
	/* select endpoint */
	out_usb(D12_COMMAND, endpoint);
	if ((in_usb(D12_DATA) & D12_FULL_EMPTY) == 0) {
	    if (!usb_in_dsr)
		cyg_drv_dsr_unlock();
	    return 0;
	}
    
	/* read buffer */
	out_usb(D12_COMMAND, 0xf0);
	j = in_usb(D12_DATA);
	j = in_usb(D12_DATA);
#if 0
	assert(j <= len);
#else
	if (j > len)
	    j = len;
#endif
    
	for (i = 0; i < j; ++i) {
	    *buf++ = in_usb(D12_DATA);
	}
	num_bytes += j;
		
	/* clear buffer full flag */
	out_usb(D12_COMMAND, 0xf2);
	--num_bufs;
    }
    
    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
    
    return num_bytes;
}

unsigned char
d12_write_endpoint(unsigned char endpoint, char * buf, unsigned char len)
{
    unsigned char i;

    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    /* select endpoint */
    out_usb(D12_COMMAND, endpoint);
    i = in_usb(D12_DATA);
    
    /* write buffer */
    out_usb(D12_COMMAND, 0xf0);
    out_usb(D12_DATA, 0);
    out_usb(D12_DATA, len);

    for (i = 0; i < len; ++i) {
	out_usb(D12_DATA, *(buf+i));
    }
    
    /* validate buffer */
    out_usb(D12_COMMAND, 0xfa);

    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
    
    return len;
}

void
d12_acknowledge_endpoint(unsigned char endpoint)
{
    if (!usb_in_dsr)
	cyg_drv_dsr_lock();

    out_usb(D12_COMMAND, endpoint);
    out_usb(D12_COMMAND, 0xf1);
    if (endpoint == 0)
	out_usb(D12_COMMAND, 0xf2);

    if (!usb_in_dsr)
	cyg_drv_dsr_unlock();
}


