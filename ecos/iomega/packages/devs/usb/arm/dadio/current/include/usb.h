#ifndef USB_H
#define USB_H

/* TODO move to config_keys.h */
#define IO_USB_SET_CONFIG_CALLBACK 0x0701 /* buf is void (*callback)(int key) */

/* callback keys */
#define USB_ENABLED  1
#define USB_DISABLED 2

#endif /* USB_H */
