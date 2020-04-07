/**************************************************************************/
/*                                                                        */
/*                   P H I L I P S   P R O P R I E T A R Y                */
/*                                                                        */ 
/*               COPYRIGHT (c)   1997 BY PHILIPS SINGAPORE.               */
/*                     --  ALL RIGHTS RESERVED  --                        */
/*                                                                        */ 
/* File Name:        MAINLOOP.H                                           */
/* Author:           Wenkai Du                                            */
/* Created:          19 Dec 97                                            */
/* Modified:                                                              */
/* Revision: 		 2.1                                                  */
/*                                                                        */ 
/**************************************************************************/
/*                                                                        */ 
/* 98/11/25			Added DMA disable bit. (WK)                           */ 
/* 99/09/01			Support USB mass storage class (Buan Huat)            */
/**************************************************************************/

#include <cyg/kernel/kapi.h>

#ifndef __MAINLOOP_H__
#define __MAINLOOP_H__

#ifndef __USB_H__
#define __USB_H__
#endif

/**************************************************************************/
/* basic #defines                                                         */
/**************************************************************************/

#define MAX_ENDPOINTS      (unsigned char)0x3

#define EP0_TX_FIFO_SIZE   16
#define EP0_RX_FIFO_SIZE   16
#define EP0_PACKET_SIZE    16

#define EP1_TX_FIFO_SIZE   16
#define EP1_RX_FIFO_SIZE   16
#define EP1_PACKET_SIZE    16

#define EP2_TX_FIFO_SIZE   64
#define EP2_RX_FIFO_SIZE   64
#define EP2_PACKET_SIZE    64


#define USB_IDLE           0
#define USB_TRANSMIT       1
#define USB_RECEIVE        2

//#define USB_CLASS_CODE_TEST_CLASS_DEVICE     0xdc
//#define USB_SUBCLASS_CODE_TEST_CLASS_D12     0xA0
//#define USB_PROTOCOL_CODE_TEST_CLASS_D12     0xB0
#define USB_CLASS_CODE_TEST_CLASS_DEVICE     0x08
#define USB_SUBCLASS_CODE_TEST_CLASS_D12     0x01
//#define USB_PROTOCOL_CODE_TEST_CLASS_D12     0x00
#define USB_PROTOCOL_CODE_TEST_CLASS_D12     0x50

/**************************************************************************/
/* masks                                                                  */
/**************************************************************************/

#define USB_RECIPIENT            (unsigned char)0x1F
#define USB_RECIPIENT_DEVICE     (unsigned char)0x00
#define USB_RECIPIENT_INTERFACE  (unsigned char)0x01
#define USB_RECIPIENT_ENDPOINT   (unsigned char)0x02

#define USB_REQUEST_TYPE_MASK    (unsigned char)0x60
#define USB_STANDARD_REQUEST     (unsigned char)0x00
#define USB_CLASS_REQUEST        (unsigned char)0x20
#define USB_VENDOR_REQUEST       (unsigned char)0x40

#define USB_REQUEST_MASK         (unsigned char)0x0F

#define DEVICE_ADDRESS_MASK      0x7F



/**************************************************************************/
/* macro                                                                  */
/**************************************************************************/
#ifdef __C51__
	#define SWAP(x)   ((((x) & 0xFF) << 8) | (((x) >> 8) & 0xFF))
	#define SWAP4(x)  ( (((x) & 0xFF) << 24) | (((x) & 0xFF00) << 8) | (((x) & 0xFF0000) >> 8) | (((x) & 0xFF000000) >> 24))

#else
	#define SWAP(X)   (X)
#define SWAP4(X) (X)
	#define code
	#define idata
#endif

#define MSB(x)    (((x) >> 8) & 0xFF)
#define LSB(x)    ((x) & 0xFF)

#define FALSE   0
#define TRUE    (!FALSE)



/**************************************************************************/
/* Basic typedefs                                                         */
/**************************************************************************/
#ifndef _SDTYPES_H_
typedef unsigned char   UCHAR;
#endif
typedef unsigned short  USHORT;
#ifndef _SDTYPES_H_
typedef unsigned long   ULONG;
#endif
typedef unsigned char   BOOL;


/**************************************************************************/
/* Structure and union definitions                                        */
/**************************************************************************/
typedef union _epp_flags
{
	struct _flags
	{
		unsigned char timer               	: 1;
		unsigned char bus_reset           	: 1;
		unsigned char suspend             	: 1;
		unsigned char setup_packet  	  	: 1;
		unsigned char remote_wakeup	  	: 1;
		unsigned char in_isr		      	: 1;
		unsigned char control_state		: 2;

		unsigned char configuration		: 1;
		unsigned char verbose			: 1;
		unsigned char ep1_rxdone		: 1;
		unsigned char setup_dma			: 1;
		unsigned char dma_state      		: 2;
		unsigned char dma_disable		: 1; // V2.1
	} bits;
	unsigned short value;
} EPPFLAGS;

typedef struct _device_request
{
	unsigned char bmRequestType;
	unsigned char bRequest;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
}__attribute__((packed)) DEVICE_REQUEST;



typedef struct _IO_REQUEST {
	unsigned short	uAddressL;
	unsigned char	bAddressH;
	unsigned short	uSize;
	unsigned char	bCommand;
} IO_REQUEST, *PIO_REQUEST;

#define MAX_CONTROLDATA_SIZE	8

typedef struct _control_xfer
{
	DEVICE_REQUEST DeviceRequest;
	unsigned short wLength;
	unsigned short wCount;
	unsigned char * pData;
	unsigned char dataBuffer[MAX_CONTROLDATA_SIZE];
} CONTROL_XFER;



typedef union _D12FLAGS 
{
	struct _D12FSM_FLAGS
	{	
	unsigned char    bus_reset		: 1;
	unsigned char    suspend		: 1;
	unsigned char    At_IRQL1		: 1;
	unsigned char    configuration	: 1;
	unsigned char    remote_wakeup	: 1;
	unsigned char    Reserved00		: 3;
	unsigned char    DCP_state		: 3;    /* Default Control Pipe FSM state */
	unsigned char    DCP_IOorMEM	: 1;    /* Access Data via DCP from/to IO data port or Memory*/
	unsigned char    BOT_state		: 3;    /* Default Bulk Out Pipe FSM state */
	unsigned char    BOT_IOorMEM	: 1;    /* Store Data via Bulk Out Pipe to IO port(ATA dev) or Memory */
	unsigned char    verbose		: 1;    /* Debug info*/
	unsigned char    timer			: 1;
	unsigned char    Reserved20		: 6;
	unsigned char    Reserved30;
	} bits;
	unsigned long value;
} D12FLAGS;







/*************************************************************************/
/* USB utility functions                                                 */ 
/*************************************************************************/
#ifndef __C51__
//void interrupt timer_isr(void);
//void interrupt usb_isr(void);
#endif

//void fn_usb_isr();
cyg_uint32 fn_usb_isr(cyg_vector_t vector, cyg_addrword_t data);
void fn_usb_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

extern void suspend_change(void);
extern void stall_ep0(void);
extern void disconnect_USB(void);
extern void connect_USB(void);
extern void reconnect_USB(void);
extern void init_unconfig(void);
extern void init_config(void);
extern void single_transmit(unsigned char * pData, unsigned char len);
extern void code_transmit(unsigned char code * pRomData, unsigned short len);
extern void code_transmitEP2(unsigned char code * pRomData, unsigned short len);
extern void single_transmit_BOT(unsigned char * pData, unsigned char len);

extern void control_handler();
extern void check_key_LED(void);
extern void setup_dma();
#ifndef __C51__
extern void setup_io(); // V2.1, x86 only
#endif

void dma_start(PIO_REQUEST);

#define IN_TOKEN_DMA 	1
#define OUT_TOKEN_DMA 	0

#ifndef __C51__
	#define DMA_BUFFER_SIZE		64000
#else
	#define DMA_BUFFER_SIZE		16384
#endif

#define DMA_IDLE	0
#define DMA_RUNNING	1
#define DMA_PENDING	2

#define SETUP_DMA_REQUEST 		0x0471
#define GET_FIRMWARE_VERSION    0x0472
#define GET_SET_TWAIN_REQUEST   0x0473

typedef struct _TWAIN_FILEINFO {
	unsigned char	bPage;    // bPage bit 7 - 5 map to uSize bit 18 - 16
	unsigned char	uSizeH;    // uSize bit 15 - 8
	unsigned char	uSizeL;    // uSize bit 7 - 0
}__attribute__((packed)) TWAIN_FILEINFO, *PTWAIN_FILEINFO;

#endif

