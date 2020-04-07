#include <cyg/kernel/kapi.h>            /* All the kernel specific stuff */
#include <cyg/io/io.h>                  /* I/O functions */
#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>

#include <dadio/io/audio.h>
#include <dadio/io/pm.h>
#if 1
#include "short_audio_left.h"
#include "short_audio_right.h"
#define AUDIO_LENGTH (13 * 4608)
#else
#include "long_audio_left.h"
#include "long_audio_right.h"
#define AUDIO_LENGTH (147 * 4608)
#endif

/* DEFINES */

#define NTHREADS 1
#define STACKSIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL + (16 * 4096))

/* STATICS */

static cyg_handle_t thread[NTHREADS];
static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];

static short audio[AUDIO_LENGTH];
static cyg_io_handle_t handle;

/* FUNCTIONS */

static void
play_loop(void)
{
  int i;
  lbuf_t* wave_buf;
  cyg_uint32 len;
  
  for (i = 0; i < AUDIO_LENGTH; i += 4608) {
    len = sizeof(lbuf_t);
    cyg_io_get_config(handle, IO_DSP_GET_CONFIG_BUFFER_HANDLE, &wave_buf, &len);
    memcpy(wave_buf->data, (((unsigned char*)audio)+i), 4608);
    len = 4608;
    cyg_io_write(handle, wave_buf, &len);
  }
}

static void
dsp_thread(CYG_ADDRESS data)
{
  int i;
  unsigned short * audio_interleave;
  cyg_uint32 len;
  cyg_int8 vol;
#ifdef EQ_AVAILABLE
  cyg_int8 bass;
  cyg_int8 treble;
#endif
  
  CYG_TEST_INFO("Starting dsp test");
  
  if (cyg_io_lookup("/dev/dsp", &handle) != ENOERR) {
    CYG_TEST_EXIT("Could not find /dev/dsp\n");
  }
  diag_printf("Got handle to /dev/dsp\n");
  
  len = sizeof(len);
  if (cyg_io_set_config(handle, IO_PM_SET_CONFIG_REGISTER, &len, &len) != ENOERR) {
    CYG_TEST_EXIT("Could not register /dev/dsp with pm");
  }
  diag_printf("Registered /dev/dsp with pm\n");
  
  audio_interleave = audio;
  for (i = 0; i < (AUDIO_LENGTH / 2); ++i) {
    *audio_interleave++ = left_channel[i];
    *audio_interleave++ = right_channel[i];
  }
  
#if 0
  for (;;) {
    diag_printf("playing\n");
    play_loop();
    cyg_thread_delay(100);	/* wait 1s */
    len = sizeof(len);
    cyg_io_set_config(handle, IO_PM_SET_CONFIG_POWER_DOWN, &len, &len);
    cyg_thread_delay(100);	/* wait 1s */
    cyg_io_set_config(handle, IO_PM_SET_CONFIG_POWER_UP, &len, &len);
  }
#endif
  
  for (;;) {
    
    diag_printf("Playing loop crescendo\n");
    vol = DSP_MIN_VOLUME;
    while (vol < DSP_MAX_VOLUME) {
      len = sizeof(vol);
      cyg_io_set_config(handle, IO_DSP_SET_CONFIG_VOLUME, &vol, &len);
      cyg_io_get_config(handle, IO_DSP_GET_CONFIG_VOLUME, &vol, &len);
      diag_printf("Volume is %ddB\n", vol);
      play_loop();
      vol += 5;
    }
    
    diag_printf("Playing loop decrescendo\n");
    while (vol > DSP_MIN_VOLUME) {
      len = sizeof(vol);
      cyg_io_set_config(handle, IO_DSP_SET_CONFIG_VOLUME, &vol, &len);
      cyg_io_get_config(handle, IO_DSP_GET_CONFIG_VOLUME, &vol, &len);
      diag_printf("Volume is %ddB\n", vol);
      play_loop();
      vol -= 5;
    }
    
    vol = 0;
    len = sizeof(vol);
    cyg_io_set_config(handle, IO_DSP_SET_CONFIG_VOLUME, &vol, &len);
    diag_printf("Volume set to 0dB\n");
    
#ifdef EQ_AVAILABLE
    diag_printf("Playing loop bass boost crescendo\n");
    bass = DSP_MIN_BASS;
    while (bass < DSP_MAX_BASS) {
      len = sizeof(bass);
      cyg_io_set_config(handle, IO_DSP_SET_CONFIG_BASS, &bass, &len);
      cyg_io_get_config(handle, IO_DSP_GET_CONFIG_BASS, &bass, &len);
      diag_printf("Bass boost is %ddB\n", bass);
      play_loop();
      bass += 2;
    }
    
    diag_printf("Playing loop bass boost decrescendo\n");
    bass = DSP_MAX_BASS;
    while (bass > DSP_MIN_BASS) {
      len = sizeof(bass);
      cyg_io_set_config(handle, IO_DSP_SET_CONFIG_BASS, &bass, &len);
      cyg_io_get_config(handle, IO_DSP_GET_CONFIG_BASS, &bass, &len);
      diag_printf("Bass boost is %ddB\n", bass);
      play_loop();
      bass -= 2;
    }
    
    diag_printf("Playing loop treble boost crescendo\n");
    treble = DSP_MIN_TREBLE;
    while (treble < DSP_MAX_TREBLE) {
      len = sizeof(treble);
      cyg_io_set_config(handle, IO_DSP_SET_CONFIG_TREBLE, &treble, &len);
      cyg_io_get_config(handle, IO_DSP_GET_CONFIG_TREBLE, &treble, &len);
      diag_printf("Treble boost is %ddB\n", treble);
      play_loop();
      treble += 2;
    }
    
    diag_printf("Playing loop treble boost decrescendo\n");
    treble = DSP_MAX_TREBLE;
    while (treble > DSP_MIN_TREBLE) {
      len = sizeof(treble);
      cyg_io_set_config(handle, IO_DSP_SET_CONFIG_TREBLE, &treble, &len);
      cyg_io_get_config(handle, IO_DSP_GET_CONFIG_TREBLE, &treble, &len);
      diag_printf("Treble boost is %ddB\n", treble);
      play_loop();
      treble -= 2;
    }
#endif // #ifdef EQ_AVAILABLE
    
    diag_printf("dsp OK\n");
    diag_printf("Repeating test\n");
  }
  
  CYG_TEST_PASS_FINISH("dsp OK");
}

void cyg_user_start(void)
{
  diag_printf("Starting\n");
  CYG_TEST_INIT();
  
  cyg_thread_create(10, dsp_thread, (cyg_addrword_t) 0, "dsp_thread",
    (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
  cyg_thread_resume(thread[0]);
}
