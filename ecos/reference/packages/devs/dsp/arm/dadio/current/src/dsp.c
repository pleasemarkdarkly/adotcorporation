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
#include <cyg/io/devtab.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <pkgconf/io_dsp_arm_dadio.h>
#include <dadio/io/dsp.h>
#include <dadio/io/dac.h>

extern void i2s_FIQ(void);

//#define DSP_DATA_IN_SRAM
#ifdef DSP_DATA_IN_SRAM
#define _SRAMVAR(x)  __attribute__ ((section(".sram"))) x
#else
#define _SRAMVAR(x)  /**/
#endif

iome_dsp_data_t iome_dsp_data _SRAMVAR( = {0} );      /* our buffers */

static unsigned char playback_buf[DSP_NUM_BUFS][DSP_BUF_SIZE] _SRAMVAR( = {{0}} );  /* data buffers */
static lbuf_t playback_lbuf[DSP_NUM_BUFS] _SRAMVAR( = {{0}} );   /* lbuf structures */

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
             DDODAT_IO_DSP_NAME,
             0,
             &io_dsp_devio,
             dsp_init, 
             dsp_lookup,
             &iome_dsp_data
    );

/* internal functions */
#define DSP_RESET_PARTIAL 0
#define DSP_RESET_FULL    1
static void dsp_do_reset( iome_dsp_data_t* dsp_data, int level );

/* danc: dsp_write() is simple now because the buffer it is getting handed should just be the
 * buffer to write to
 */


#define WAIT_1MS	0x00004800
#define WAIT_1US	0x0000000b

#define DAC_VOL_MAX	0x00	/* 0 dB */
#define DAC_VOL_MIN	0x5a	/* -90 dB */

/* port B pins, DAC_SDA also used on port D */
#define DAC_SCL		0x08				// DAC I2C SCL
#define DAC_SDA		0x04				// DAC I2C SDA

/* I2C values */
#define DAC_I2C_ADDRESS 0x20
#define I2C_WRITE       0x00
#define I2C_READ        0x01

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

/* MODE_2_REG bits */
#define DIF_LEFT_24             0x02

/* MODE_REG bits */
#define BBCF_200HZ              0x80
#define TBCF_7KHZ               0x20
#define AB                      0x80


/* Dac Register Shadow .......................*/
static unsigned char DacVolume			= 0x00;
static unsigned char DacBase			= 0x00;
static unsigned char DacTreble			= 0x00;
static unsigned char DacLimiterAttack	= 0x20;
static unsigned char DacLimiterRelease  = 0x10;
static unsigned char DacMute			= 0x00;
 
static void
wait(unsigned int cnt)
{
    while(cnt--)
	;
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

    *((volatile unsigned char *)PBDDR) &= ~DAC_SDA;			// Turn SDA into input for ACK
    wait(WAIT_1US * 4);

    *((volatile unsigned char *)PBDR) |= DAC_SCL;			// Clock high for ACK
    wait(WAIT_1US * 16);

    *((volatile unsigned char *)PBDR) &= ~DAC_SCL;			// Clock low
    wait(WAIT_1US * 4);

    *((volatile unsigned char *)PBDDR) |= DAC_SDA;			// Turn SDA back to output
    wait(WAIT_1US * 4);
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

void dac_init() 
{
    /* TODO move most of this to power on ioctl */
    volatile cyg_uint32 delay;
    
    /* make sure EXTCLK is on */
    *(volatile cyg_uint8 *)PEDR &= ~0x01;
        
    /* fix for pop guard */
    /* initial state */
    *(volatile cyg_uint8 *)PEDR |= 0x02; /* take /RST high */
    *(volatile cyg_uint8 *)PBDR |= 0x40; /* take headphone mute high */
    wait(WAIT_1MS);
    /* reset action */
    *(volatile cyg_uint8 *)PEDR &= ~0x02;
    *(volatile cyg_uint8 *)PBDR &= ~0x40;
    wait(300 * WAIT_1MS);
    *(volatile cyg_uint8 *)PEDR |= 0x02;
    wait(600 * WAIT_1MS);
    *(volatile cyg_uint8 *)PBDR |= 0x40;

    /* make sure GPIO pins are set up correctly for communication with DAC */
    //    *((volatile unsigned char *)PBDDR) |= (DAC_SCL|DAC_SDA); /* 0x0c */
    *((volatile unsigned char *)PBDR) &= ~(DAC_SCL|DAC_SDA);
    *((volatile unsigned char *)PBDR) |= (DAC_SCL|DAC_SDA);
    wait(WAIT_1MS);

    /* configure and power on DAC */
    write_dac_reg(0x01, 0xd3);
    write_dac_reg(0x0a, 0x39);
    write_dac_reg(0x0b, 0x02);
    write_dac_reg(0x01, 0xd1);
    for (delay = 0; delay < 65536 * 32; ++delay)
	;
    write_dac_reg(0x06, 0x06);
    write_dac_reg(0x07, 0xa8);
    write_dac_reg(0x04, 0x00);

	DacVolume			= 0x00;
	DacBase				= 0x00;
	DacTreble			= 0x00;
	DacLimiterAttack	= 0x20;
	DacLimiterRelease	= 0x10;
	DacMute				= 0x00;

}

static Cyg_ErrNo
dsp_write( cyg_io_handle_t handle, const void* _buf, cyg_uint32* len ) 
{
    iome_dsp_data_t* dsp_data = (iome_dsp_data_t*) ((cyg_devtab_entry_t*) handle)->priv;
    Cyg_ErrNo res = ENOERR;
  
    if( dsp_data->do_reset ) {
	*((volatile unsigned int *)SYSCON3) |= (SYSCON3_I2SSEL | 0x00000200);
	*((volatile unsigned int *)I2S_CTL) = (I2S_CTL_FLAG | I2S_CTL_ECS | I2S_CTL_RCTM);
	*((volatile unsigned int *)I2S_STAT) = 0xffffffff;
	*((volatile unsigned int *)I2S_CTL) |= (I2S_CTL_EN);
	while (!(*((volatile unsigned int *)I2S_STAT) & I2S_STAT_FIFO))
	    ;
	*((volatile unsigned int *)I2S_FIFO_CTL) = I2S_FIFO_CTL_RIGHT_ENABLE;
	while (!(*((volatile unsigned int *)I2S_STAT) & I2S_STAT_FIFO))
	    ;
	*((volatile unsigned int *)I2S_FIFO_CTL) = I2S_FIFO_CTL_LEFT_ENABLE;
	while (!(*((volatile unsigned int *)I2S_STAT) & I2S_STAT_FIFO))
	    ;
	dsp_data->do_reset = false;
	/* TODO: Check if dac_init() needs to be called every time or just the first */  
	dac_init();

    }

    if( ( lbuf_t* ) _buf != dsp_data->wr_lbuf ) {
	res = -EINTR;
    } else {
	dsp_data->wr_lbuf->len = *len;
	dsp_data->wr_lbuf = dsp_data->wr_lbuf->next;
	if( !dsp_data->vsr_unmasked ) {
	    dsp_data->vsr_unmasked = true;
	    cyg_drv_interrupt_unmask( CYGNUM_HAL_INTERRUPT_I2SINT );
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
dsp_get_config( cyg_io_handle_t handle, cyg_uint32 key, void* buf, cyg_uint32* len ) 
{
    iome_dsp_data_t* dsp_data = (iome_dsp_data_t*) ((cyg_devtab_entry_t*) handle)->priv;
    lbuf_t* lbuf = dsp_data->wr_lbuf;
    volatile unsigned int* length = ( volatile unsigned int*) &( lbuf->len );
	
    switch(key) {
	case IO_DSP_GET_CONFIG_BUFFER_HANDLE:
	{
	    while( *length != 0 ) cyg_thread_delay(10);  // busy wait until the buffer has been emptied
	    *( (lbuf_t**) buf ) = (lbuf_t*) lbuf;
	    return ENOERR;
	    break ;
	}

	case IO_DSP_GET_VOLUME:
	{
		unsigned char* pchar = (unsigned char*)buf;
		if(*len < 1) return -ENOSYS;
		*pchar = DacVolume;
		*len = 1;
		return ENOERR;
		break;
	}
	case IO_DSP_GET_BASE:
	{
		unsigned char* pchar = (unsigned char*)buf;
		if(*len < 1) return -ENOSYS;
		*pchar = DacBase;
		*len = 1;
		return ENOERR;
		break;
	}
	case IO_DSP_GET_TREBLE:
	{
		unsigned char* pchar = (unsigned char*)buf;
		if(*len < 1) return -ENOSYS;
		*pchar = DacTreble;
		*len = 1;
		return ENOERR;
		break;
	}
	case IO_DSP_GET_LIMITER:
	{
		unsigned int ATTACKandRELEASE;
		unsigned int* pint = (unsigned int*)buf;
		if(*len < 2) return -ENOSYS;
		ATTACKandRELEASE = (((unsigned int)DacLimiterAttack) << 8);
		ATTACKandRELEASE |= (((unsigned int)DacLimiterRelease) & 0x00FF); 
		*pint = ATTACKandRELEASE;
		*len = 2;
		return ENOERR;
		break;
	}
	case IO_DSP_GET_MUTE:
	{
		unsigned char* pchar = (unsigned char*)buf;
		*pchar = DacMute;
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
dsp_set_config( cyg_io_handle_t handle, cyg_uint32 key, const void* buf, cyg_uint32* len ) 
{
    iome_dsp_data_t* dsp_data = (iome_dsp_data_t*) ((cyg_devtab_entry_t*) handle)->priv;
    volatile  unsigned int* length;
  
    lbuf_t* lbuf;
    lbuf_t* endbuf;

    switch( key ) {
	case IO_DSP_SET_CONFIG_DISABLE:
	{
	    /* wait until all the data is played */
	    lbuf = endbuf = dsp_data->rd_lbuf;
	    do {
		length = ( volatile  unsigned int*) &( lbuf->len );
		while( *length != 0 ) ;
		lbuf = lbuf->next;
	    } while( lbuf != endbuf );

	    *(( volatile unsigned int* ) I2S_CTL) &= ~( I2S_CTL_EN );
	    cyg_drv_interrupt_mask( CYGNUM_HAL_INTERRUPT_I2SINT );
	    dsp_data->vsr_unmasked = false;
	    //      dsp_do_reset( DSP_RESET_PARTIAL );
	    return ENOERR;
	    break;
	}
	case IO_DSP_SET_CONFIG_ENABLE:
	{
	    *(( volatile unsigned int* ) I2S_CTL) |= ( I2S_CTL_EN );
	    return ENOERR;
	    break;
	}
	case IO_DSP_SET_CONFIG_RESET:
	{
	    dsp_do_reset( dsp_data, DSP_RESET_FULL );
	    return ENOERR;
	    break;
	}

	case IO_DSP_SET_VOLUME:
	{
		unsigned char* pchar = (unsigned char*)buf;
		if((*len) < 1) return -ENOSYS;
		DacVolume = *pchar;
		write_dac_reg(0x02, DacVolume);
		return ENOERR;
		break;
	}
	case IO_DSP_SET_BASE:
	{
		unsigned char* pchar = (unsigned char*)buf;
		unsigned char  BASEandTREB = 0x00;
		if((*len) < 1) return -ENOSYS;
		DacBase = *pchar;
		BASEandTREB  = (DacBase << 4);
		BASEandTREB |= (0xF0 & DacTreble);
		write_dac_reg(0x06, BASEandTREB);
		return ENOERR;
		break;
	}
	case IO_DSP_SET_TREBLE:
	{
		unsigned char* pchar = (unsigned char*)buf;
		unsigned char  BASEandTREB = 0x00;
		if((*len) < 1) return -ENOSYS;
		DacTreble = *pchar;
		BASEandTREB  = (DacBase << 4);
		BASEandTREB |= (0xF0 & DacTreble);
		write_dac_reg(0x06, BASEandTREB);
		return ENOERR;
		break;
	}
	case IO_DSP_SET_LIMITER:
	{
		unsigned int* pint = (unsigned int*)buf;
		unsigned int  ATTACKandRELEASE;
		if((*len) < 2) return -ENOSYS;
		ATTACKandRELEASE = *pint;
		DacLimiterRelease = (unsigned char)(ATTACKandRELEASE & 0x00FF);
		DacLimiterAttack  = (unsigned char)((ATTACKandRELEASE >> 8) & 0x00FF);
		write_dac_reg(0x08, DacLimiterAttack);
		write_dac_reg(0x09, DacLimiterRelease);
		return ENOERR;
		break;
	}

	/* Note: Functionality not implemented Yet - But we have a state variable anyway*/
	case IO_DSP_SET_MUTE:
	{
		unsigned char* pchar = (unsigned char*)buf;
		DacMute = *pchar;
		return ENOERR;
		break;
	}

	default:
	{
	    return -ENOSYS;
	    break;
	}
    }
}

static bool
dsp_init( struct cyg_devtab_entry* tab ) 
{
    dsp_do_reset( (iome_dsp_data_t*) tab->priv, DSP_RESET_FULL );
  
    HAL_VSR_SET( CYGNUM_HAL_VECTOR_FIQ, i2s_FIQ, NULL );
    ((iome_dsp_data_t*) tab->priv)->vsr_unmasked = false;

    return true;
}

// danc: reset the internal DSP state to its beginning state
static void dsp_do_reset( iome_dsp_data_t* dsp_data, int level ) 
{
    int i;

    for( i = 0; i < DSP_NUM_BUFS; i++ ) {
	if( level == DSP_RESET_FULL ) {
	    memset( playback_buf[i], 0, DSP_BUF_SIZE );
	}
    
	playback_lbuf[i].data = playback_buf[i];
	playback_lbuf[i].len  = 0;

	if (i < (DSP_NUM_BUFS - 1)) {
	    playback_lbuf[i].next = &playback_lbuf[i + 1];
	} else {
	    playback_lbuf[i].next = &playback_lbuf[0];
	}
    }

    dsp_data->wr_lbuf = &playback_lbuf[0];
    dsp_data->rd_lbuf = &playback_lbuf[0];
    dsp_data->do_reset = true;
}
      

static Cyg_ErrNo
dsp_lookup(struct cyg_devtab_entry **tab, 
	   struct cyg_devtab_entry *sub_tab,
	   const char *name)
{
    return ENOERR;
}

