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
// Purpose:     Shared data between dsr, threads
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#ifndef USB_SHARED_DATA_H
#define USB_SHARED_DATA_H

#include <cyg/kernel/kapi.h>

extern bool usb_in_dsr;

extern unsigned char usb_configuration;
extern unsigned char usb_remote_wakeup;

#endif /* USB_SHARED_DATA_H */
