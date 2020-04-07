#ifndef DSP_H
#define DSP_H
// ====================================================================
//
//      dsp.h
//
//      Device I/O 
//
// ====================================================================
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   danc
// Contributors:   danc toddm
// Date:        2000-3-1
// Purpose:     Internal interfaces for dsp I/O driver
// Description: rewrite of toddm's original dsp driver
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <pkgconf/system.h>
#include <cyg/io/io.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>

#define SYSCON3_I2SSEL  (1<<3) // Enable i2s instead of ssi#2

#define I2S_CTL         0x80002000 // I2S (Audio interface) control
#define I2S_CTL_FLAG    0x0404     // Magic
#define I2S_CTL_EN      (1<<16)    // Enable interface
#define I2S_CTL_ECS     (1<<17)    // External clock select
#define I2S_CTL_LCTM    (1<<19)    // Left channel transmit interrupt
#define I2S_CTL_LCRM    (1<<20)    // Left channel receive interrupt
#define I2S_CTL_RCTM    (1<<21)    // Right channel transmit interrupt
#define I2S_CTL_RCRM    (1<<22)    // Right channel receive interrupt
#define I2S_CTL_LBM     (1<<23)    // Loop-back mode
#define I2S_RIGHT_FIFO  0x80002040 // Right channel FIFO access
#define I2S_LEFT_FIFO   0x80002080 // Left channel FIFO access
#define I2S_FIFO_CTL    0x800020C0 // FIFO control
#define I2S_FIFO_CTL_RIGHT_ENABLE  0x00118000
#define I2S_FIFO_CTL_RIGHT_DISABLE 0x00110000
#define I2S_FIFO_CTL_LEFT_ENABLE   0x000D8000
#define I2S_FIFO_CTL_LEFT_DISABLE  0x000D0000
#define I2S_STAT        0x80002100 // I2S interface status
#define I2S_STAT_RCTSR  (1<<0)     // Right channel transmit service request
#define I2S_STAT_RCRSR  (1<<1)     // Right channel receive service request
#define I2S_STAT_LCTSR  (1<<2)     // Left channel transmit service request
#define I2S_STAT_LCRSR  (1<<3)     // Left channel receive service request
#define I2S_STAT_RCTUR  (1<<4)     // Right channel transmit FIFO underrun
#define I2S_STAT_RCROR  (1<<5)     // Right channel receive FIFO overrun
#define I2S_STAT_LCTUR  (1<<6)     // Left channel transmit FIFO underrun
#define I2S_STAT_LCROR  (1<<7)     // Left channel receive FIFO overrun
#define I2S_STAT_RCTNF  (1<<8)     // Right channel transmit FIFO not full
#define I2S_STAT_RCRNE  (1<<9)     // Right channel receive FIFO not empty
#define I2S_STAT_LCTNF  (1<<10)    // Left channel transmit FIFO not full
#define I2S_STAT_LCRNE  (1<<11)    // Left channel receive FIFO not empty
#define I2S_STAT_FIFO   (1<<12)    // A FIFO operation has completed

#define CYGNUM_HAL_INTERRUPT_I2SINT 22 /* MCPINT on the 7211 */
/* End in 1.2.10 */

//#define EQ_AVAILABLE

/* TODO move to config_keys.h */
#define IO_DSP_SET_CONFIG_DISABLE           0x0401
#define IO_DSP_SET_CONFIG_ENABLE            0x0402
#define IO_DSP_SET_CONFIG_RESET             0x0403
#define IO_DSP_SET_CONFIG_VOLUME            0x0404 /* volume is cyg_int8 */
#define IO_DSP_GET_CONFIG_BUFFER_HANDLE     0x0410
#define IO_DSP_GET_CONFIG_VOLUME            0x0411
#ifdef EQ_AVAILABLE
#define IO_DSP_SET_CONFIG_BASS              0x0405 /* bass boost is cyg_int8 */
#define IO_DSP_SET_CONFIG_TREBLE            0x0406 /* treble boost is cyg_int8 */
#define IO_DSP_GET_CONFIG_BASS              0x0412
#define IO_DSP_GET_CONFIG_TREBLE            0x0413
#endif

/* TODO Move to config file */
#define DSP_NUM_BUFS  8
#define DSP_BUF_SIZE (1152 * 2 * 2)   /* MP3_MAX_PCM_LENGTH * sizeof(short) * num_channels */

/* digital volume ranges */
#define DSP_MAX_VOLUME   0
#define DSP_MIN_VOLUME  -90

#ifdef EQ_AVAILABLE
#define DSP_MAX_BASS     12
#define DSP_MIN_BASS      0
#define DSP_MAX_TREBLE  DSP_MAX_BASS
#define DSP_MIN_TREBLE  DSP_MIN_BASS
#endif

/* danc: rather than force the dsp to stretch data across buffers, we force
 * programs that dump audio data out to make sure the buffers are filled,
 * so the buffer structure is simpler now
 */

/* lbuf_t type */
typedef struct _lbuf_t {
  unsigned char*    data;   /* ptr to the data        */
  unsigned int      len;    /* size of the data block */
  struct _lbuf_t*   next;   /* ptr to next element    */
} lbuf_t;

typedef struct {
  lbuf_t* wr_lbuf;   /* buffer to write to  */
  lbuf_t* rd_lbuf;   /* buffer to read from */
  bool    do_reset;  /* does the i2s need a reset ? */
  bool    vsr_unmasked;  /* is the vsr unmasked ? */
} iome_dsp_data_t;

extern iome_dsp_data_t iome_dsp_data;

#endif /* DSP_H */