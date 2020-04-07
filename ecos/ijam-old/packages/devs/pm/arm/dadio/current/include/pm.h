#ifndef PM_H
#define PM_H

/* when using POWER_{DOWN|UP}_ALL, make sure interrupts are set up correctly so that the device will
 * wake up under the right situation only */

#if 0				/* these options are in pkgconf/io_pm_arm_dadio.h */
#define DDODAT_IO_PM_NAME          "/dev/pm"   /* TODO move this to a pkgconf file */
#define DDONUM_IO_PM_TABLE_ENTRIES 4 	       /* maximum number of devices that can be registered at one time */
#endif

/* ioctls */
#define IO_PM_SET_CONFIG_REGISTER       0x0681 /* TODO move thess to config_keys.h */
#define IO_PM_SET_CONFIG_UNREGISTER     0x0682

/* pass SOFT_RESET as the arg to power down all in order to do a soft reset instead
 * of a power down */
#define NO_SOFT_RESET 0x00
#define SOFT_RESET 0x05
#define IO_PM_SET_CONFIG_POWER_DOWN_ALL 0x0683

#define IO_PM_SET_CONFIG_POWER_UP_ALL   0x0684

/* the following two ioctls need to be supported by devices that use the pm device */
/* also need some means of unregistering device, since there is no device close, suggest
 *  using IO_PM_SET_CONFIG_UNREGISTER from above and passing it down */
#define IO_PM_SET_CONFIG_POWER_DOWN     0x0685
#define IO_PM_SET_CONFIG_POWER_UP       0x0686

#endif /* PM_H */
