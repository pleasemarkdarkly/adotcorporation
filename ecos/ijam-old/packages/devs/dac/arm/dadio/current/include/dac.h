#ifndef DAC_H
#define DAC_H
// ====================================================================
//
//      dac.h
//
//      Device I/O 
//
// ====================================================================
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   toddm
// Contributors:        toddm
// Date:        2000-01-04
// Purpose:     Internal interfaces for dac I/O driver
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

/* the volume range is 0 to -90 on the 4341 and 18 to -90
 * on the 43l43, so don't misinterpret the volume values
 */

/* TODO move to config_keys.h */
#define IO_DAC_GET_CONFIG_VOLUME              0x0501
#define IO_DAC_SET_CONFIG_VOLUME_RELATIVE     0x0581
#define IO_DAC_SET_CONFIG_VOLUME_ABSOLUTE     0x0582
#define IO_DAC_SET_CONFIG_MUTE                0x0583
#define IO_DAC_SET_CONFIG_UNMUTE              0x0584
/* device supports power management, so IO_PM_* ioctls are supported */

#endif /* DAC_H */
