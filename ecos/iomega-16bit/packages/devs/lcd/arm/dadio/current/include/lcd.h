#ifndef LCD_H
#define LCD_H

/* ioctls */
//#define IO_LCD_GET_CONFIG 0x0801
//#define IO_LCD_SET_CONFIG 0x0881
#define IO_LCD_SET_CONFIG_INVERT_DISPLAY  0x0881 /* this will toggle between normal and inverted mode */
#define IO_LCD_SET_CONFIG_CLEAR_DISPLAY   0x0882
#define IO_LCD_SET_CONFIG_BACKLIGHT_ON    0x0883
#define IO_LCD_SET_CONFIG_BACKLIGHT_OFF   0x0884
#define IO_LCD_SET_CONFIG_REFRESH_DISPLAY 0x0885

#define IO_LCD_GET_CONFIG_VIDEO_POINTER   0x0801 /* buf = video buffer, *len = sizeof video buffer */

/* the pm ioctls are also supported */
#define IO_PM_SET_CONFIG_POWER_DOWN_ALL 0x0683
#define IO_PM_SET_CONFIG_POWER_UP_ALL   0x0684
/* the following two ioctls need to be supported by devices that use the pm device */
/* also need some means of unregistering device, since there is no device close, suggest
 *  using IO_PM_SET_CONFIG_UNREGISTER from above and passing it down */
#define IO_PM_SET_CONFIG_POWER_DOWN     0x0685
#define IO_PM_SET_CONFIG_POWER_UP       0x0686

#endif /* LCD_H */
