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
#ifndef PDIUSBD12_H
#define PDIUSBD12_H

/* Set Mode configuration byte */
#define D12_NO_LAZYCLOCK	0x02
#define D12_CLOCK_RUNNING       0x04
#define D12_INTERRUPT_MODE	0x08
#define D12_SOFT_CONNECT	0x10
/* endpoint configuration */
#define D12_NON_ISO		0x00
#define D12_ISO_OUT		0x40
#define D12_ISO_IN		0x80
#define D12_ISO_IO		0xC0

/* Set Mode clock division factor byte */
#define D12_CLOCK_12MHZ		0x03
#define D12_CLOCK_4MHZ		0x0b
#define D12_SET_TO_ONE          0x40
#define D12_SOF_ONLY		0x80

/* Set DMA configuration register */
/* dma burst length */
#define D12_DMA_SINGLE		0x00
#define D12_BURST_4		0x01
#define D12_BURST_8		0x02
#define D12_BURST_16		0x03
#define D12_DMA_ENABLE          0x04
#define D12_DMA_DIRECTION	0x08
#define D12_AUTO_RELOAD		0x10
#define D12_INTERRUPT_PIN_MODE	0x20
#define D12_ENDP4_INT_ENABLE	0x40
#define D12_ENDP5_INT_ENABLE	0x80

/* Read Interrupt Register byte 1 */
#define D12_INT_ENDP0_OUT	0x01
#define D12_INT_ENDP0_IN	0x02
#define D12_INT_ENDP1_OUT	0x04
#define D12_INT_ENDP1_IN	0x08
#define D12_INT_ENDP2_OUT	0x10
#define D12_INT_ENDP2_IN	0x20
#define D12_INT_BUS_RESET	0x40
#define D12_INT_SUSPEND_CHANGE	0x80
/* byte 2 */
#define D12_INT_DMA_EOT		0x0100

/* Read Last Transaction Status Register */
#define D12_SUCCESS             0x01
#define D12_SETUP_PACKET	0x20
#define D12_DATA_01_PACKET      0x40
#define D12_PREVIOUS_STATUS_NOT_READ 0x80

/* Undocumented Read Endpoint Status */
#define D12_BUFFER0_FULL	0x20
#define D12_BUFFER1_FULL	0x40

/* Select Endpoint optional read */
#define D12_FULL_EMPTY		0x01
#define D12_STALL		0x02

/* Initialization commands */
void d12_set_address_enable(unsigned char address, unsigned char enable);
void d12_set_endpoint_enable(unsigned char enable);
void d12_set_mode(unsigned char config, unsigned char clock_div);
void d12_set_dma(unsigned char config);

/* Data flow commands */
unsigned short d12_read_interrupt_register(void);
unsigned char d12_select_endpoint(unsigned char endpoint);
unsigned char d12_read_last_transaction_status(unsigned char endpoint);
unsigned char d12_read_endpoint_status(unsigned char endpoint);
void d12_set_endpoint_status(unsigned char endpoint, unsigned char stalled);
void d12_acknowledge_endpoint(unsigned char endpoint);
unsigned char d12_read_endpoint(unsigned char endpoint, char * buf, unsigned char len); /* may return up to 128 bytes on
											 * main endpoint */
unsigned char d12_write_endpoint(unsigned char endpoint, char * buf, unsigned char len);

/* General commands */
void d12_send_resume(void);
unsigned short d12_read_current_frame_number(void);
unsigned short d12_read_chip_id(void);

#endif /* PDIUSBD12_H */
