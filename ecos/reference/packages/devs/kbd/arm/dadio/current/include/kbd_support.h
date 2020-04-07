#ifndef __KBD_SUPPORT_H__
#define __KBD_SUPPORT_H__

#include <cyg/kernel/kapi.h>

#define DDOHWR_CES		/* TODO figure out how to integrate this correctly */

// we split things up so buttons in column one use the high byte of a short
#ifdef USE_KBD_MSGBOX_TIMED_GET
#define KBD__modifier    1
#else
#define KBD__modifier    0
#endif

#if defined(DDOHWR_CES)
#define KBD_Exit         0 + KBD__modifier
#define KBD_Pad_Up       1 + KBD__modifier   // 0x0002
#define KBD_Pad_Down     2 + KBD__modifier   // 0x0004
#define KBD_Pad_Right    3 + KBD__modifier   // 0x0008
#define KBD_Pad_Left     4 + KBD__modifier   // 0x0010
#define KBD_Jog_Up       5 + KBD__modifier   // 0x0020
#define KBD_Jog_In       6 + KBD__modifier 
#define KBD_Jog_Down     7 + KBD__modifier   // 0x0080
#define KBD_Pad_A        8 + KBD__modifier   // 0x0100
#define KBD_Pad_B        9 + KBD__modifier   // 0x0200
#elif defined(DDOHWR_COMDEX)
#define KBD_S101          0x00
#define KBD_S102          0x10
#define KBD_S103          0x20
#define KBD_S104          0x30
#define KBD_S105          0x40
#define KBD_S106          0x50
#define KBD_S107          0x60
#define KBD_S108          0x70
#else
#err Invalid Dadio board selected for keyboard configuration
#endif

#define KBD_Press      0x80  // Event has this bit when the key is pressed
#define KBD_Empty      0xFF

/*
MT 4/1/00 - Incremented the defines by one so that there wouldn't be a conflict
			with the key release event of KBD_Exit and a timeout with a call to
			message box timed get.  Sorry if I broke any code, but you shouldn't
			be using magic numbers.
*/
/*
MT 4/1/00 - kbd_events_mbox_handle is now initialized to 0 at time of definition.
			  kbd_init() will create a message box only if kbd_events_mbox_handle is 0
			  so you can now specify which message box you'd like it to use by creating
			  a message box and setting kbd_event_mbox_handle equal to the handle of
			  that message box before calling kbd_init().
*/

#ifdef __cplusplus
extern "C" {
#endif
  extern cyg_handle_t kbd_events_mbox_handle;
  void kbd_init(void);
#ifdef __cplusplus
};
#endif

#endif /* __KBD_SUPPORT_H__ */
