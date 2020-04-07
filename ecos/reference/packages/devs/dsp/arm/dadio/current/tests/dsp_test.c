#include <cyg/kernel/kapi.h>            /* All the kernel specific stuff */
#include <cyg/io/io.h>                  /* I/O functions */
#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>

#include <dadio/io/dac.h>
#include <dadio/io/dsp.h>
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
static cyg_io_handle_t mixerH;

/* FUNCTIONS */

static void
play_loop(void)
{
    int i;
    lbuf_t* wave_buf;
    cyg_uint32 tmp;
    
    for (i = 0; i < AUDIO_LENGTH; i += 4608) {
	cyg_io_get_config(handle, IO_DSP_GET_CONFIG_BUFFER_HANDLE, &wave_buf, &tmp);
	memcpy(wave_buf->data, (((unsigned char*)audio)+i), 4608);
	tmp = 4608;
	cyg_io_write(handle, wave_buf, &tmp);
    }
}

static void
dsp_thread(CYG_ADDRESS data)
{
    int i;
    cyg_uint32 tmp;
    unsigned short * audio_interleave;
    int vol;
    
    CYG_TEST_INFO("Starting dsp test");
    
    if (cyg_io_lookup("/dev/dsp", &handle) != ENOERR) {
	CYG_TEST_EXIT("Could not find /dev/dsp\n");
    }
    diag_printf("Got handle to /dev/dsp\n");

    if (cyg_io_lookup("/dev/mixer", &mixerH) != ENOERR) {
	CYG_TEST_EXIT("Could not find /dev/mixer");
    }
    diag_printf("Got handle to /dev/mixer\n");

#if 0
    if (cyg_io_set_config(mixerH, IO_PM_SET_CONFIG_REGISTER, &i, &i) != ENOERR) {
	CYG_TEST_EXIT("Could not register /dev/mixer with pm");
    }
    diag_printf("Registered /dev/mixer with pm\n");
    vol = -90;
    if (cyg_io_set_config(mixerH, IO_DAC_SET_CONFIG_VOLUME_ABSOLUTE, &vol, &tmp) != ENOERR) {
	CYG_TEST_EXIT("Could not set volume to -10dB");
    }
    diag_printf("Set volume to %ddB\n", vol);
#endif
    
    audio_interleave = audio;
    for (i = 0; i < (AUDIO_LENGTH / 2); ++i) {
	*audio_interleave++ = left_channel[i];
	*audio_interleave++ = right_channel[i];
    }

    diag_printf("Playing loop\n");
    for (;;) {
	play_loop();
    }
    
    diag_printf("Playing loop crescendo\n");
    while (vol < 0) {
	i = 5;
	cyg_io_set_config(mixerH, IO_DAC_SET_CONFIG_VOLUME_RELATIVE, &i, &tmp);
	play_loop();
	cyg_io_get_config(mixerH, IO_DAC_GET_CONFIG_VOLUME, &vol, &tmp);
    }

    diag_printf("Playling loop decrescendo\n");
    while (vol > -90) {
	i = -5;
	cyg_io_set_config(mixerH, IO_DAC_SET_CONFIG_VOLUME_RELATIVE, &i, &tmp);
	play_loop();
	cyg_io_get_config(mixerH, IO_DAC_GET_CONFIG_VOLUME, &vol, &tmp);
    }

    vol = -10;
    cyg_io_set_config(mixerH, IO_DAC_SET_CONFIG_VOLUME_ABSOLUTE, &vol, &tmp);
    diag_printf("Playing unmuted, vol at %ddB\n", vol);
    play_loop();
    cyg_io_set_config(mixerH, IO_DAC_SET_CONFIG_MUTE, &vol, &tmp);
    diag_printf("Playing muted, vol at %ddB\n", vol);
    play_loop();
    cyg_io_set_config(mixerH, IO_DAC_SET_CONFIG_UNMUTE, &vol, &tmp);
    diag_printf("Playing unmuted again, vol at %ddB\n", vol);
    play_loop();
    
    if (cyg_io_set_config(mixerH, IO_PM_SET_CONFIG_UNREGISTER, &i, &i) != ENOERR) {
	CYG_TEST_EXIT("Could not unregister /dev/mixer with power management");
    }
    diag_printf("Unregistered /dev/mixer with power management\n");
    CYG_TEST_PASS_FINISH("dsp OK");
}

void cyg_user_start(void)
{
    CYG_TEST_INIT();
    cyg_thread_create(10, dsp_thread, (cyg_addrword_t) 0, "dsp_thread",
		      (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);
}


