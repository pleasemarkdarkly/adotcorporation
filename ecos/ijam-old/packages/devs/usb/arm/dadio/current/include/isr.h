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
#ifndef ISR_H
#define ISR_H

cyg_uint32 usb_isr(cyg_vector_t vector, cyg_addrword_t data);
void usb_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
void usb_thread(cyg_addrword_t data);    

#endif /* ISR_H */
