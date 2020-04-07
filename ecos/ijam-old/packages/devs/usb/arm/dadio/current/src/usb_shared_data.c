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
#include <dadio/io/usb_shared_data.h>
#include <dadio/io/mass_storage.h>

bool usb_in_dsr;

unsigned char usb_configuration;
unsigned char usb_remote_wakeup;
