//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   toddm
// Contributors:  toddm
// Date:        2000-01-04
// Purpose:     Top level dac driver
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/io/devtab.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/infra/diag.h>
#include <pkgconf/io_dac_arm_dadio_cs4341.h>
#include <pkgconf/io_pm_arm_dadio.h>
#include <dadio/io/dac.h>
#include <dadio/io/pm.h>

#define WAIT_1MS	0x00004800
#define WAIT_1US	0x0000000b
static void wait(unsigned int cnt);
static void write_dac_reg(unsigned char reg, unsigned char dat);
static void write_dac(unsigned char dat);

#define DAC_VOL_MAX	0x00	/* 0 dB */
#define DAC_VOL_MIN	0x5a	/* -90 dB */
static int dac_volume = 20; /* initial volume, -20 dB */

static bool dac_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo dac_lookup(struct cyg_devtab_entry **tab, 
			    struct cyg_devtab_entry *sub_tab,
			    const char *name);
static Cyg_ErrNo dac_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len);
static Cyg_ErrNo dac_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
static Cyg_ErrNo dac_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo dac_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len);

DEVIO_TABLE(io_dac_devio,
            dac_write,
            dac_read,
            dac_get_config,
            dac_set_config
    );

typedef struct
{
    cyg_io_handle_t pm_devH;
} dac_priv_info_t;
static dac_priv_info_t dac_priv_info;
DEVTAB_ENTRY(io_dac_devtab,
             DDODAT_IO_DAC_NAME,
             DDODAT_IO_PM_NAME,
             &io_dac_devio,
             dac_init, 
             dac_lookup,
             &dac_priv_info
    );

/* port B pins, DAC_SDA also used on port D */
#define DAC_SCL		0x08				// DAC I2C SCL
#define DAC_SDA		0x04				// DAC I2C SDA

/* I2C values */
#define DAC_I2C_ADDRESS 0x22	/* 7 bit I2C address, top 6 bits are 001000b, LSB must be same as AD0 (hi) */
#define I2C_WRITE       0x00
#define I2C_READ        0x01

/* CS4341 registers */
#define MODE_CONTROL 0x01	/* mode control register */
#define AMUTE        0x80	/* auto mute */
#define DIF_1	     0x01	/* format 1, I2S, up to 24 bit data, 32 x Fx internal SCLK */
#define DIF_2        0x02	/* format 2, left justified, up to 24 bit data */
#define DEM_44KHZ    0x04	/* 44.1kHz de-emphasis */
#define POR          0x02	/* power on,off quiescent voltage ramp */
#define PDN          0x01	/* power down */

#define VOL_MIX      0x02	/* volume and mixing control register */
#define AB           0x80	/* A=B, channel A volume = channel B volume */
#define SOFT         0x40	/* soft control */
#define ZERO         0x20	/* zero cross detection control */
#define ATAPI_STEREO 0x09	/* ATAPI channel mixing and muting */

#define CHANNEL_A    0x03	/* channel A control register */
#define CHANNEL_B    0x04	/* channel B control register */
#define MUTE         0x80       /* channel A,B volume control */

static char dac_config = (AMUTE|DIF_2|DEM_44KHZ|POR); /* default mode control settings */

static bool
dac_init(struct cyg_devtab_entry * tab)
{
    /* make sure GPIO pins are set up correctly for the DAC, same config for both hw models */
    *((volatile unsigned char *)PBDDR) |= (DAC_SCL|DAC_SDA); /* 0x0c */
    *((volatile unsigned char *)PBDR) &= ~(DAC_SCL|DAC_SDA); /* 0xf3 */
    //    *((volatile unsigned char *)PDDDR) &= 0xfb;	/* TODO wtf has port D got to do with it? */
    *((volatile unsigned char *)PBDR) |= (DAC_SCL|DAC_SDA);
    wait(WAIT_1MS);

    write_dac_reg(MODE_CONTROL, dac_config|PDN);	/* start in powered down state */
    write_dac_reg(VOL_MIX, AB|SOFT|ATAPI_STEREO);
    write_dac_reg(CHANNEL_A, dac_volume);
    write_dac_reg(CHANNEL_B, 0x00);	/* disable mute */
    
    return true;
}

static Cyg_ErrNo
dac_lookup(struct cyg_devtab_entry **tab, 
	   struct cyg_devtab_entry *sub_tab,
	   const char *name)
{
    /* set pointer to pm device in private data */
    dac_priv_info_t * priv = (dac_priv_info_t *)(*tab)->priv;
    priv->pm_devH = (cyg_io_handle_t)sub_tab;    
    return ENOERR;
}

static Cyg_ErrNo
dac_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len)
{
    /* Writing (and reading) not permitted on our DAC */
    return -EPERM;
}

static Cyg_ErrNo
dac_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len)
{
    return -EPERM;
}

static Cyg_ErrNo
dac_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len)
{
    switch (key) {
	case IO_DAC_GET_CONFIG_VOLUME:
	    *((int *)buf) = -dac_volume; /* dB are inverted */
	    return ENOERR;
	    break;

	default:
	    return -ENOSYS;
	    break;
    }
}

static Cyg_ErrNo
dac_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * len)
{
    cyg_devtab_entry_t * t = (cyg_devtab_entry_t *)handle;
    dac_priv_info_t * priv = (dac_priv_info_t *)t->priv;
    cyg_uint32 _len;
    
    switch (key) {
	case IO_DAC_SET_CONFIG_VOLUME_RELATIVE:
	    dac_volume -= *((int *)buf);
	    if (dac_volume < DAC_VOL_MAX) {
		dac_volume = DAC_VOL_MAX;
	    }    
	    else if (dac_volume > DAC_VOL_MIN) {
		dac_volume = DAC_VOL_MIN;
	    }

	    write_dac_reg(CHANNEL_A, dac_volume);
	    return ENOERR;
	    break;

	case IO_DAC_SET_CONFIG_VOLUME_ABSOLUTE:
	    dac_volume = -*((int *)buf);
	    if (dac_volume < DAC_VOL_MAX) {
		dac_volume = DAC_VOL_MAX;
	    }
	    else if (dac_volume > DAC_VOL_MIN) {
		dac_volume = DAC_VOL_MIN;
	    }

	    write_dac_reg(CHANNEL_A, dac_volume);
	    return ENOERR;
	    break;

	case IO_DAC_SET_CONFIG_MUTE:
	    diag_printf("DAC mute\n");
	    write_dac_reg(CHANNEL_A, dac_volume|MUTE);
	    return ENOERR;
	    break;

	case IO_DAC_SET_CONFIG_UNMUTE:
	    diag_printf("DAC unmute\n");
	    write_dac_reg(CHANNEL_A, dac_volume);
	    return ENOERR;
	    break;
	    
	case IO_PM_SET_CONFIG_POWER_DOWN:
	    diag_printf("DAC entering power down state\n");
	    write_dac_reg(MODE_CONTROL, dac_config|PDN);
	    /* NOTE datasheet says that power down state should be min time to allow
	     * DC-blocking capacitors to completely discharge to avoid a transient
	     * when powered back up */
	    
	    /* TODO disable PE1 to disable OE on oscillator for DAC */
	    
	    /* turn off amplifier */
	    /* TODO check if this is correct */
	    *(volatile cyg_uint8 *)PEDR |= 0x04;/* PE2 */
	    return ENOERR;
	    break;

	case IO_PM_SET_CONFIG_POWER_UP:
	    diag_printf("DAC entering power up state\n");
	    write_dac_reg(MODE_CONTROL, dac_config);

	    /* TODO disable PE1 to disable OE on oscillator for DAC */
	    
	    /* turn on amplifier */
	    *(volatile cyg_uint8 *)PEDR &= ~0x04;/* PE2 */
	    return ENOERR;
	    break;

	    /* handle these by passing them down to the pm device */
	case IO_PM_SET_CONFIG_REGISTER:
	case IO_PM_SET_CONFIG_UNREGISTER:
	    _len = sizeof(cyg_io_handle_t);
	    return cyg_io_set_config(priv->pm_devH, key, handle, &_len);
	    
	default:
	    return -ENOSYS;
	    break;
    }
}

static void
wait(unsigned int cnt)
{
    while(cnt--)
	;
}

static void
write_dac_reg(unsigned char reg, unsigned char value)
{
    wait(WAIT_1MS);
    *((volatile unsigned char *)PBDR) &= ~DAC_SDA;	// Start 
    wait(WAIT_1US * 16);
    
    write_dac(DAC_I2C_ADDRESS|I2C_WRITE);
    write_dac(reg);
    write_dac(value);
    
    *((volatile unsigned char *)PBDR) |= DAC_SCL;	// Clock high
    wait(WAIT_1US * 16);
    
    wait(WAIT_1US * 8);
    *((volatile unsigned char *)PBDR) |= DAC_SDA;	// Stop 
    wait(WAIT_1US * 16);
}


static void
write_dac(unsigned char dat)
{
    unsigned int i;
    unsigned char c = 0x80;

    for (i = 0; i < 8; i++) {
	*((volatile unsigned char *)PBDR) &= ~DAC_SCL;			// Clock low
	wait(WAIT_1US * 8);

	if(dat & c)							// Set data
	    *((volatile unsigned char *)PBDR) |= DAC_SDA;
	else
	    *((volatile unsigned char *)PBDR) &= ~DAC_SDA;
	wait(WAIT_1US * 16);

	*((volatile unsigned char *)PBDR) |= DAC_SCL;			// Clock high
	wait(WAIT_1US * 16);

	c = c >> 1;							// Shift bit
    }
    *((volatile unsigned char *)PBDR) &= ~DAC_SCL;			// Clock low
    wait(WAIT_1US * 8);
    *((volatile unsigned char *)PBDR) &= ~DAC_SDA;			// Data low
    wait(WAIT_1US * 4);

    /* TODO check whether PDDDR really is a typo */
    //    *((volatile unsigned char *)PDDDR) |= DAC_SDA;			// Turn SDA into input for ACK
    *((volatile unsigned char *)PBDDR) &= ~DAC_SDA;			// Turn SDA into input for ACK
    wait(WAIT_1US * 4);

    *((volatile unsigned char *)PBDR) |= DAC_SCL;			// Clock high for ACK
    wait(WAIT_1US * 16);

    *((volatile unsigned char *)PBDR) &= ~DAC_SCL;			// Clock low
    wait(WAIT_1US * 4);

    //    *((volatile unsigned char *)PDDDR) &= ~DAC_SDA;			// Turn SDA back to output
    *((volatile unsigned char *)PBDDR) |= DAC_SDA;			// Turn SDA back to output
    wait(WAIT_1US * 4);
}

