#ifndef __KBD_SUPPORT_H__
#define __KBD_SUPPORT_H__

#include <cyg/kernel/kapi.h>

#define DDOHWR_CES		/* TODO figure out how to integrate this correctly */

#define USE_KBD_MSGBOX_TIMED_GET

// we split things up so buttons in column one use the high byte of a short
#ifdef USE_KBD_MSGBOX_TIMED_GET
#define KBD__modifier    1  // changed from 1
#else
#define KBD__modifier    0
#endif

#define KBD_Rew          0 + KBD__modifier
#define KBD_Play         1 + KBD__modifier
#define KBD_Fwd          2 + KBD__modifier
#define KBD_Select       3 + KBD__modifier
#define KBD_Stop         4 + KBD__modifier
#define KBD_Lock         5 + KBD__modifier
#define KBD_Down         6 + KBD__modifier
#define KBD_Up           7 + KBD__modifier

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
