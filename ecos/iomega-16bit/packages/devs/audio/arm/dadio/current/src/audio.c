//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   danc
// Contributors:  danc toddm
// Date:        2000-3-1
// Purpose:     Top level dsp driver
// Description: rewrite based on todd's original dsp driver
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/io.h>
#include <pkgconf/io_audio_arm_dadio.h>
#include <pkgconf/io_pm_arm_dadio.h>
#include <cyg/io/devtab.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <dadio/io/audio.h>
#include <dadio/io/i2c.h>
#include <dadio/io/pm.h>

extern void i2s_FIQ(void);

#undef DSP_DATA_IN_SRAM
#ifdef DSP_DATA_IN_SRAM
#define _SRAMVAR(x)  __attribute__ ((section(".sram"))) x
#else
#define _SRAMVAR(x)  /**/
#endif

iome_dsp_data_t iome_dsp_data _SRAMVAR( = {0} );      /* our buffers */

static unsigned char playback_buf[DSP_NUM_BUFS][DSP_BUF_SIZE] _SRAMVAR( = {{0}} );  /* data buffers */
static lbuf_t playback_lbuf[DSP_NUM_BUFS] _SRAMVAR( = {{0}} );   /* lbuf structures */
static cyg_io_handle_t pm_devH;
static bool dac_initialized = false;

static bool dsp_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo dsp_lookup(struct cyg_devtab_entry **tab, 
			    struct cyg_devtab_entry *sub_tab,
			    const char *name);
static Cyg_ErrNo dsp_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len);
static Cyg_ErrNo dsp_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
static Cyg_ErrNo dsp_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo dsp_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len);

DEVIO_TABLE(io_dsp_devio,
            dsp_write,
            dsp_read,
            dsp_get_config,
            dsp_set_config
    );

DEVTAB_ENTRY(io_dsp_devtab,
             DDODAT_IO_AUDIO_NAME,
             DDODAT_IO_PM_NAME,
             &io_dsp_devio,
             dsp_init, 
             dsp_lookup,
             &iome_dsp_data
    );

/* internal functions */
#define DSP_RESET_PARTIAL 0
#define DSP_RESET_FULL    1
static void dsp_do_reset(iome_dsp_data_t* dsp_data, int level);
static void dsp_enable(void);
static void dsp_disable(iome_dsp_data_t * dsp_data);
static void dac_enable(void);
static void dac_disable(void);

/* danc: dsp_write() is simple now because the buffer it is getting handed should just be the
 * buffer to write to
 */

/* i2c values */
#define DAC_I2C_ADDRESS         0x20

/* CS43L43 registers */
#define POWER_MUTE_REG          0x01	// power and muting control
#define A_VOL_REG		0x02	// channel a analog attenuation control
#define B_VOL_REG		0x03	// channel b analog attenuation control
#define A_DVOL_REG		0x04	// channel a digital volume
#define B_DVOL_REG		0x05	// channel b dig volume
#define TONE_REG		0x06	// tone control
#define MODE_REG		0x07	// mode control
#define ARATE_REG		0x08	// limiter attack rate
#define RRATE_REG		0x09	// limiter release rate
#define VOL_MIX_REG		0x0a	// volume and mixing
#define MODE_2_REG		0x0b	// mode control 2

/* POWER_MUTE_REG bits */
#define AMUTE                   0x80
#define SZC_RAMPED              0x40
#define POR                     0x10
#define PDN                     0x02
#define CP_EN                   0x01

/* VOL_MIX_REG bits */
#define TC_EN                   0x20
#define LIM_EN                  0x10
#define ATAPI_LR                0x09
// (epg,8/29/2000): it is reported that our channels are reversed, so I'll change the default setting to the following RL version.
#define ATAPI_RL				0x06

/* MODE_2_REG bits */
#define DIF_LEFT_24             0x02

/* MODE_REG bits */
#define BBCF_200HZ              0x80
#define TBCF_7KHZ               0x20
#define AB                      0x08

static cyg_int8 dsp_volume = 0x00;
static cyg_int8 dsp_bass = 0x00;
static cyg_int8 dsp_treble = 0x00;
#define TONE(bass,treble) (((bass & 0xf) << 4) | (treble & 0xf))

static bool
dsp_init(struct cyg_devtab_entry * tab)
{
    /* reset internal data structures */
    dsp_do_reset((iome_dsp_data_t*)tab->priv, DSP_RESET_FULL);

    /* install FIQ handler that feeds the samples to the interface */
    HAL_VSR_SET(CYGNUM_HAL_VECTOR_FIQ, i2s_FIQ, NULL);
    ((iome_dsp_data_t *)tab->priv)->vsr_unmasked = false;

    return true;
}

static Cyg_ErrNo
dsp_write(cyg_io_handle_t handle, const void * _buf, cyg_uint32 * len) 
{
    iome_dsp_data_t* dsp_data = (iome_dsp_data_t*) ((cyg_devtab_entry_t*)handle)->priv;
    Cyg_ErrNo res = ENOERR;
  
    if (dsp_data->do_reset) {
	dsp_enable();
	dac_enable();
	dsp_data->do_reset = false;
    }

    if ((lbuf_t*)_buf != dsp_data->wr_lbuf) {
	res = -EINTR;
    }
    else {
	/* set pointer to data to send to I2S FIFOs */
	dsp_data->wr_lbuf->len = *len;
	dsp_data->wr_lbuf = dsp_data->wr_lbuf->next;
	/* make sure that the I2S interrupt is unmasked so that the FIFOs will fill up */
	if (!dsp_data->vsr_unmasked) {
	    dsp_data->vsr_unmasked = true;
	    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_I2SINT);
	}
    }
    return res;
}

static Cyg_ErrNo
dsp_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len)
{
    /* Reading (recording) not available on this device */
    return -EPERM;
}

static Cyg_ErrNo
dsp_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len) 
{
    iome_dsp_data_t * dsp_data = (iome_dsp_data_t *) ((cyg_devtab_entry_t *) handle)->priv;
    lbuf_t * lbuf = dsp_data->wr_lbuf;
    volatile unsigned int * length = (volatile unsigned int *) &(lbuf->len);
	
    switch(key) {
	case IO_DSP_GET_CONFIG_BUFFER_HANDLE:
	{
	    while(*length != 0) {
		cyg_thread_delay(10);  // busy wait until the buffer has been emptied
	    }
	    *((lbuf_t**)buf) = (lbuf_t*) lbuf;
	    return ENOERR;
	    break ;
	}

	case IO_DSP_GET_CONFIG_VOLUME:
	{
	    cyg_int8 * vol = (cyg_int8 *)buf;
	    if (*len < 1) {
		return -EINVAL;
	    }
	    *vol = dsp_volume;
	    *len = 1;
	    return ENOERR;
	    break;
	}
	
	case IO_DSP_GET_CONFIG_BASS:
	{
	    cyg_int8 * bass = (cyg_int8 *)buf;
	    if (*len < 1) {
		return -EINVAL;
	    }
	    *bass = dsp_bass;
	    *len = 1;
	    return ENOERR;
	    break;
	}
	
	case IO_DSP_GET_CONFIG_TREBLE:
	{
	    cyg_int8 * treble = (cyg_int8 *)buf;
	    if (*len < 1) {
		return -EINVAL;
	    }
	    *treble = dsp_treble;
	    *len = 1;
	    return ENOERR;
	    break;
	}
	
	default:
	    return -ENOSYS;
	    break;
    }
}

static Cyg_ErrNo
dsp_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * len) 
{
    iome_dsp_data_t * dsp_data = (iome_dsp_data_t *) ((cyg_devtab_entry_t *)handle)->priv;
    volatile  unsigned int * length;
    lbuf_t * lbuf;
    lbuf_t * endbuf;
    
    switch (key) {
	case IO_DSP_SET_CONFIG_DISABLE:
	{
	    /* wait until all the data is played */
	    lbuf = endbuf = dsp_data->rd_lbuf;
	    do {
		length = (volatile  unsigned int *)&(lbuf->len);
		while(*length != 0)
		    ;
		lbuf = lbuf->next;
	    } while(lbuf != endbuf);

	    dsp_disable(dsp_data);
	    return ENOERR;
	    break;
	}
	
	case IO_DSP_SET_CONFIG_ENABLE:
	{
	    dsp_data->do_reset = true; /* delay enabling device until audio is written */
	    return ENOERR;
	    break;
	}
	case IO_DSP_SET_CONFIG_RESET:
	{
	    dsp_do_reset(dsp_data, DSP_RESET_FULL);
	    return ENOERR;
	    break;
	}

	case IO_DSP_SET_CONFIG_VOLUME:
	{
	    cyg_int8 * vol = (cyg_int8 *)buf;
	    if (*len < 1) {
		return -EINVAL;
	    }
	    dsp_volume = *vol;
	    if (dsp_volume > DSP_MAX_VOLUME) {
		dsp_volume = DSP_MAX_VOLUME;
	    }
	    if (dsp_volume < DSP_MIN_VOLUME) {
		dsp_volume = DSP_MIN_VOLUME;
	    }
	    if (dac_initialized) {
#ifdef DIGITAL_VOLUME
		i2c_write(DAC_I2C_ADDRESS, A_DVOL_REG, dsp_volume);
#else
		if (dsp_volume >= -25) {
		    i2c_write(DAC_I2C_ADDRESS, A_DVOL_REG, -12);
		    i2c_write(DAC_I2C_ADDRESS, A_VOL_REG, dsp_volume);
		}
		else {
		    i2c_write(DAC_I2C_ADDRESS, A_VOL_REG, -25);
		    i2c_write(DAC_I2C_ADDRESS, A_DVOL_REG, dsp_volume);
		}
#endif /* DIGITAL_VOLUME */
	    }
	    return ENOERR;
	    break;
	}
	
	case IO_DSP_SET_CONFIG_BASS:
	{
	    cyg_int8 * bass = (cyg_int8 *)buf;

	    if (*len < 1) {
		return -EINVAL;
	    }
	    dsp_bass = *bass;
	    if (dsp_bass > DSP_MAX_BASS) {
		dsp_bass = DSP_MAX_BASS;
	    }
	    if (dsp_bass < DSP_MIN_BASS) {
		dsp_bass = DSP_MIN_BASS;
	    }
	    if (dac_initialized) {
		i2c_write(DAC_I2C_ADDRESS, TONE_REG, TONE(dsp_bass, dsp_treble));
	    }
	    return ENOERR;
	    break;
	}
	
	case IO_DSP_SET_CONFIG_TREBLE:
	{
	    cyg_int8 * treble = (cyg_int8 *)buf;

	    if (*len < 1) {
		return -EINVAL;
	    }
	    dsp_treble = *treble;
	    if (dsp_treble > DSP_MAX_TREBLE) {
		dsp_treble = DSP_MAX_TREBLE;
	    }
	    if (dsp_treble < DSP_MIN_TREBLE) {
		dsp_treble = DSP_MIN_TREBLE;
	    }
	    if (dac_initialized) {
		i2c_write(DAC_I2C_ADDRESS, TONE_REG, TONE(dsp_bass, dsp_treble));
	    }
	    return ENOERR;
	    break;
	}

	case IO_PM_SET_CONFIG_REGISTER:
	case IO_PM_SET_CONFIG_UNREGISTER:
	{
	    cyg_uint32 len = sizeof(cyg_io_handle_t);
	    return cyg_io_set_config(pm_devH, key, handle, &len);
	}

	case IO_PM_SET_CONFIG_POWER_DOWN:
	{
	    diag_printf("/dev/dsp power down\n");
	    dac_disable();
	    dsp_disable(dsp_data);
	    return ENOERR;
	}
	
	case IO_PM_SET_CONFIG_POWER_UP:
	{
	    diag_printf("/dev/dsp power up\n");

	    /* clear the internal structures and let the next write
	     * do the power up sequence */
	    dsp_do_reset(dsp_data, DSP_RESET_FULL);
	    return ENOERR;
	}
	
	default:
	{
	    return -ENOSYS;
	    break;
	}
    }
}     

static Cyg_ErrNo
dsp_lookup(struct cyg_devtab_entry ** tab, 
	   struct cyg_devtab_entry * sub_tab,
	   const char * name)
{
    pm_devH = (cyg_io_handle_t)sub_tab;
    return ENOERR;
}

// danc: reset the internal DSP state to its beginning state
static void
dsp_do_reset(iome_dsp_data_t * dsp_data, int level) 
{
    int i;

    for (i = 0; i < DSP_NUM_BUFS; i++) {
	if (level == DSP_RESET_FULL) {
	    memset(playback_buf[i], 0, DSP_BUF_SIZE);
	}
    
	playback_lbuf[i].data = playback_buf[i];
	playback_lbuf[i].len  = 0;

	if (i < (DSP_NUM_BUFS - 1)) {
	    playback_lbuf[i].next = &playback_lbuf[i + 1];
	}
	else {
	    playback_lbuf[i].next = &playback_lbuf[0];
	}
    }

    dsp_data->wr_lbuf = &playback_lbuf[0];
    dsp_data->rd_lbuf = &playback_lbuf[0];
    dsp_data->do_reset = true;
}

static bool s_bDSPEnabled = false;

static void
dsp_disable(iome_dsp_data_t * dsp_data)
{
    if (s_bDSPEnabled)
    {
	/* disable I2S FIFOs */
	while ((*(volatile cyg_uint32 *)I2S_STAT & I2S_STAT_FIFO) == 0)
	    ;
	*(volatile cyg_uint32 *)I2S_STAT = I2S_STAT_FIFO;
	*(volatile cyg_uint32 *)I2S_FIFO_CTL = I2S_FIFO_CTL_RIGHT_DISABLE;
	while ((*(volatile cyg_uint32 *)I2S_STAT & I2S_STAT_FIFO) == 0)
	    ;
	*(volatile cyg_uint32 *)I2S_STAT = I2S_STAT_FIFO;
	*(volatile cyg_uint32 *)I2S_FIFO_CTL = I2S_FIFO_CTL_LEFT_DISABLE;
	while ((*(volatile cyg_uint32 *)I2S_STAT & I2S_STAT_FIFO) == 0)
	    ;
	*(volatile cyg_uint32 *)I2S_STAT = I2S_STAT_FIFO;
    
	/* disable I2S interrupt */
	cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_I2SINT);
	dsp_data->vsr_unmasked = false;
    
	/* disable I2S interface, this disables the I2S clocks puts this interface
	 * into low power mode */
	*(volatile cyg_uint32 *)I2S_CTL = 0;

	/* turn off EXTCLK */	    
	*(volatile cyg_uint8 *)PEDR |= 0x01;

	s_bDSPEnabled = false;
    }
}

static void
dsp_enable(void) 
{
    /* enable EXTCLK, which paces the I2S interface on the 7212
     * this is the MCLK referred to in the documentation */
    /* do this first so that it can stabilize before we need to use it */
    *(volatile cyg_uint8 *)PEDR &= ~0x01;
    
    /* select the I2S interface */
    *((volatile cyg_uint32 *)SYSCON3) |= (SYSCON3_I2SSEL | 0x00000200);
    
    /* NOTE the order on these operations is taken from the Cirrus demo code
     * the datasheet indicates a slightly different order */
    
    /* program the I2S control register to use the external clock (MCLK) and
     * interrupt when the right channel transmit FIFO is half empty */
    *((volatile cyg_uint32 *)I2S_CTL) = (I2S_CTL_FLAG | I2S_CTL_ECS | I2S_CTL_RCTM);
    
    /* clear the overflow and underflow status bits */
    *((volatile cyg_uint32 *)I2S_STAT) = 0xffffffff;
    
    /* enable the I2S interface */
    /* we have 1024 LRCK cycles from this point to enable the control port on the DAC */
    *((volatile cyg_uint32 *)I2S_CTL) |= (I2S_CTL_EN);
    
    /* enable the FIFOs for the left and right channels */
    while (!(*((volatile cyg_uint32 *)I2S_STAT) & I2S_STAT_FIFO))
	;
    *((volatile cyg_uint32 *)I2S_FIFO_CTL) = I2S_FIFO_CTL_RIGHT_ENABLE;
    while (!(*((volatile cyg_uint32 *)I2S_STAT) & I2S_STAT_FIFO))
	;
    *((volatile cyg_uint32 *)I2S_FIFO_CTL) = I2S_FIFO_CTL_LEFT_ENABLE;
    while (!(*((volatile cyg_uint32 *)I2S_STAT) & I2S_STAT_FIFO))
	;

    s_bDSPEnabled = true;
}

static void
dac_disable(void)
{
    /* start DAC power down sequence */
    i2c_write(DAC_I2C_ADDRESS, POWER_MUTE_REG, AMUTE|SZC_RAMPED|POR|PDN|CP_EN);

    /* need to wait a bit to discharge capacitors  */
    cyg_thread_delay(40);

    /* reset dac */
    *(volatile cyg_uint8 *)PEDR &= ~0x02;
    
    dac_initialized = false;
}

static void
dac_enable(void)
{
    volatile unsigned int delay;
    
    if (!dac_initialized) {
	/* take the DAC out of reset */
	*(volatile cyg_uint8 *)PEDR |= 0x02;
	
	i2c_init();
	/* enable the control port and configure the DAC */
	i2c_write(DAC_I2C_ADDRESS, POWER_MUTE_REG, AMUTE|SZC_RAMPED|POR|PDN|CP_EN);
	i2c_write(DAC_I2C_ADDRESS, VOL_MIX_REG, TC_EN|LIM_EN|ATAPI_RL);
	i2c_write(DAC_I2C_ADDRESS, MODE_2_REG, DIF_LEFT_24);
	
	/* start the DAC power on sequence */
	i2c_write(DAC_I2C_ADDRESS, POWER_MUTE_REG, AMUTE|SZC_RAMPED|POR|CP_EN);
	
	/* set volume and tone to default values */
	/* this must be done before disabling the headphone mute, otherwise
	 * we get that nice pop */
	i2c_write(DAC_I2C_ADDRESS, TONE_REG, TONE(dsp_bass, dsp_treble));
	i2c_write(DAC_I2C_ADDRESS, MODE_REG, BBCF_200HZ|TBCF_7KHZ|AB);
#ifdef DIGITAL_VOLUME
	i2c_write(DAC_I2C_ADDRESS, A_DVOL_REG, dsp_volume);
#else
	i2c_write(DAC_I2C_ADDRESS, A_DVOL_REG, -12);
	i2c_write(DAC_I2C_ADDRESS, A_VOL_REG, dsp_volume);
#endif /* DIGITAL_VOLUME */

#if 0
	/* audio output won't begin for approx 10,000 LRCK cycles,
	 * so wait ~.25s to minimize lost audio */
	delay = 0x4800 * 250;	/* 1ms * 250 */
	while (delay--)
	    ;
#endif
	
	dac_initialized = true;
    }
}

    
