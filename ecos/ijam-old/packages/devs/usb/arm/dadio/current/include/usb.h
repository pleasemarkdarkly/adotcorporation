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
#ifndef USB_H
#define USB_H

#include <cyg/kernel/kapi.h>
#include <cyg/hal/drv_api.h>
#include <cyg/error/codes.h>

/* TODO move to config_keys.h */
#define IO_USB_SET_CONFIG_CALLBACK 0x0701 /* buf is void (*callback)(int key) */

/* callback keys */
#define USB_ENABLED  1
#define USB_DISABLED 2

#define DEBUG

#define ENDP0_PACKET_SIZE 16
#define ENDP1_PACKET_SIZE 16
#define ENDP2_PACKET_SIZE 64

/* buffer indices used for endpoints */
typedef struct cbuf_s
{
    unsigned char * data;
    volatile int put;
    volatile int get;
    cyg_sem_t sem;
} cbuf_t;

/* data internal to each endpoint */
typedef struct usb_endp_s
{
    int num;
    unsigned char packet_size;
    bool tx_active;
    bool init;
    cbuf_t in;
    cbuf_t out;
} usb_endp_t;

/* values for second arg to transmit_packet/receive_packet */
#define TRANSMIT_CONTINUE 0
#define TRANSMIT_START    1
#define RECEIVE_CONTINUE  0
#define RECEIVE_START     1

/* utility macros for dealing with rbc byte ordering */
#define SWAP_32(x) ((((x) & 0xff000000) >> 24) | \
                    (((x) & 0x00ff0000) >> 8) | \
                    (((x) & 0x0000ff00) << 8) | \
                    (((x) & 0x000000ff) << 24))
#define SWAP_16(x) ((((x) & 0xff00) >> 8) | \
                    (((x) & 0x00ff) << 8))

#endif /* USB_H */
