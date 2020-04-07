//==========================================================================
//
//        kbd_support.c
//
//        IOME button support functions
//
//==========================================================================
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     toddm danc
// Contributors:  toddm
// Date:          1999-12-21
// Description:   Functions used to support IOME buttons
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <dadio/io/kbd_support.h>

//#define DEBUG_KBD
#ifdef DEBUG_KBD
#define DEBUG(s...) DEBUG(##s)
#else
#define DEBUG(s...) /**/
#endif

#define KBD_NUM_KEYS 8
#define KBD_NUM_COLUMNS 1
#define STACK_SIZE (4*4096)

static char kbd_server_stack[STACK_SIZE];
static cyg_thread kbd_server_thread_data;
static cyg_handle_t kbd_server_thread_handle;

static cyg_interrupt kbd_interrupt;
static cyg_handle_t kbd_interrupt_handle;
static cyg_sem_t kbd_sem;

static cyg_mbox kbd_events_mbox;
cyg_handle_t kbd_events_mbox_handle = (cyg_handle_t)(0);

static cyg_uint8 col_state[KBD_NUM_COLUMNS];

static void
kbd_delay(void)
{
  volatile int i;
  
  for (i = 0;  i < 250;  i++)
    ;
}

static void
kbd_scan(void)
{
  int i;
  cyg_uint8 port_data;
  
  // Turn off drive (de-select) all columns
  *(volatile cyg_uint32 *)SYSCON1 = (*(volatile cyg_uint32 *)SYSCON1 & ~SYSCON1_KBD_CTL) |
    SYSCON1_KBD_LOW;
  for (i = 0;  i < KBD_NUM_COLUMNS;  i++) {
    // Select column 'i'
    *(volatile cyg_uint32 *)SYSCON1 = (*(volatile cyg_uint32 *)SYSCON1 & ~SYSCON1_KBD_CTL) |
      SYSCON1_KBD_COL(i);
    // Small pause to let the wires charge up :-)
    kbd_delay();
    // Grab the data
    col_state[i] = port_data = *(volatile cyg_uint8 *)PADR;
    // De-Select column 'i'
    *(volatile cyg_uint32 *)SYSCON1 = (*(volatile cyg_uint32 *)SYSCON1 & ~SYSCON1_KBD_CTL) |
      SYSCON1_KBD_TRISTATE;
    // Allow line to go slack
    kbd_delay();
  }
  // Turn on drive (select) all columns - necessary to see interrupts
  *(volatile cyg_uint32 *)SYSCON1 = (*(volatile cyg_uint32 *)SYSCON1 & ~SYSCON1_KBD_CTL) |
    SYSCON1_KBD_HIGH;
}

static cyg_uint8     kbd_state[KBD_NUM_KEYS];
static cyg_uint8     kbd_new_state[KBD_NUM_KEYS];

static cyg_uint32 kbd_map[KBD_NUM_KEYS] = {
#ifdef PHASE_3
  KBD_Play,
    KBD_Stop,
    KBD_Fwd,
    KBD_Rew,
    KBD_Lock,
    KBD_Down,
    KBD_Select,
    KBD_Up
#else
    KBD_Play,
    KBD_Stop,
    KBD_Fwd,
    KBD_Rew,
    KBD_Lock,
    KBD_Up,
    KBD_Select,
    KBD_Down
#endif // PHASE_3
};

// This ISR is called when the keyboard interrupt occurs
static int
keyboard_isr(cyg_vector_t vector, cyg_addrword_t data)
{
  cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_KBDINT);
  return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}

// This DSR handles the keyboard [logical] processing
static void
keyboard_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
  // Tell the keyboard processing thread to give it a shot
  cyg_semaphore_post(&kbd_sem);
}

static void
kbd_server(cyg_addrword_t p)
{
  int col, bit, key, event, timeout;
  cyg_count32 num_msg_in_queue;
  
  DEBUG("KBD server here\n");
  while (1) {
    cyg_semaphore_wait(&kbd_sem);
    // As long as keys are pressed, scan and update
    timeout = 0;
    while (1) {
      // Wait for 20ms - time to debounce keyboard
      cyg_thread_delay(2);
      // Scan keyboard
      kbd_scan();
      // Reset state
      for (key = 0;  key < sizeof(kbd_new_state);  key++) {
        kbd_new_state[key] = 0;
      }
      // Check state of all keys and send events for those that change
      for (col = 0;  col < KBD_NUM_COLUMNS;  col++) {
        for (bit = 0;  bit < 8;  bit++) {
          if (col_state[col] & (1<<bit)) {
#ifdef USE_KBD_MSGBOX_TIMED_GET
            key = (bit + (8*col));
            //			if (kbd_map[key] != KBD_Empty) {
            if (key != KBD_Empty) {
#else
            key = kbd_map[ (bit + (8*col)) ];
            if (key != KBD_Empty) {
#endif
              kbd_new_state[key] = 1;
            }
          }
        }
      }
      // Compare new and old (known) states, generate events for changes
      // First key up events
      for (key = 0;  key < sizeof(kbd_new_state);  key++) {
        if (kbd_state[key] != kbd_new_state[key]) {
          if (kbd_state[key]) {
            // Key going up
            event = kbd_map[key];
          } else {
            // Key going down
            event = kbd_map[key] + KBD_Press;
          }
          num_msg_in_queue = cyg_mbox_peek(kbd_events_mbox_handle);
		  // (epg,8/25/2000): making it more stringent for keydown events, allowing an extra for key ups.  
		  //	Hopefully this will at least ameliorate the case where the user hits next track repeatedly and ends up dropping the last up event.
		  //	There will still be the case where the user holds down A, presses and releases B, and then releases A: this will end up with A pressed.  But then
		  //	the user is being somewhat silly, and we just don't give a damn.
          if ((num_msg_in_queue < CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE - 6) || ( num_msg_in_queue < CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE - 5 && kbd_state[key] )) {
            if (!cyg_mbox_tryput(kbd_events_mbox_handle, (void *)event)) {
              DEBUG("KBD event lost: %x\n", event);
            }
          }
          else {
            DEBUG("Dropping kbd event\n");
          }
        }
        kbd_state[key] = kbd_new_state[key];
      }
      // Clear interrupt (true when keys are pressed)
      cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_KBDINT);
      if (*(volatile cyg_uint32 *)INTSR2 & INTSR2_KBDINT) {
        timeout = 0;
      } else if (++timeout == 5) {
        // No keys for 100ms
        break;
      }
    }
    // Allow interrupts to happen again
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_KBDINT);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_KBDINT);
  }
}

void
kbd_init(void)
{
  // This semaphore is set when there is a keypress
  cyg_semaphore_init(&kbd_sem, 0);
  // Initialize environment, setup interrupt handler
  cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_KBDINT,
    10,                     // Priority - what goes here?
    0,                      // Data item passed to interrupt handler
    (cyg_ISR_t *)keyboard_isr,
    (cyg_DSR_t *)keyboard_dsr,
    &kbd_interrupt_handle,
    &kbd_interrupt);
  cyg_drv_interrupt_attach(kbd_interrupt_handle);
  cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_KBDINT);
  cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_KBDINT);

  // Set up the mbox for keyboard data
  if(!kbd_events_mbox_handle) {
    cyg_mbox_create(&kbd_events_mbox_handle, &kbd_events_mbox);
  }
  // Create a thread whose job it is to de-bounce the keyboard and
  // actually process the input, turning it into a series of events
  cyg_thread_create(10,                           // Priority - just a number
    kbd_server,                   // entry
    0,                            // initial parameter
    "KBD_server",                 // Name
    &kbd_server_stack[0],         // Stack
    STACK_SIZE,                   // Size
    &kbd_server_thread_handle,    // Handle
    &kbd_server_thread_data       // Thread data structure
    );
  cyg_thread_resume(kbd_server_thread_handle);  // Start it
}
