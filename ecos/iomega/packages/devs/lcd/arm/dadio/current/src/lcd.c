#include <pkgconf/io_lcd_arm_dadio.h>
#include <pkgconf/io_pm_arm_dadio.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <dadio/io/lcd.h>
#include <dadio/io/pm.h>

#undef FULL_LCD_DRIVER		/* TODO this has not been tested */
//#define FULL_LCD_DRIVER

/* lcd control */
#define LCD_BACKLIGHT  0x02
#define LCD_nRESET     0x08
#define LCD_CMD  0x30000000
#define LCD_DATA 0x30000001
#define LCD_CMD_EN        0xae
#define OFF                  0
#define ON                   1
#define LCD_CMD_ALL_ON    0xa4
#define LCD_CMD_ICON_EN   0xac
#define ICON_ON              3
#define LCD_CMD_ICON_ATTR 0x00
#define LCD_CMD_BIAS_VAL  0xa2
#define BIAS_1_9	     0
#define LCD_CMD_SEG_SEQ	  0xa0
#define	DNORM		     0
#define LCD_CMD_COM_SEQ	  0xc0
#define	DREV		     8
#define LCD_CMD_V5GAIN_VAL 0x20
#define LCD_CMD_CONTRAST_EN     0x81
#define LCD_CMD_CONTRAST_VAL	0x00
#define LCD_CMD_PWR_CTRL_VAL	0x28

#define	LCD_NUM_PAGES		8
#define	LCD_NUM_COLS		98
#define LCD_CMD_COL_VAL_HI	0x10
#define LCD_CMD_COL_VAL_LO	0x00
#define LCD_CMD_PAG_VAL		0xb0
#define LCD_CMD_MODE		0xa6
#define	NORM			0
#define REV                     1
#define LCD_CMD_LIN_VAL		0x40
#define LCD_CMD_PAG_VAL		0xb0

#ifdef FULL_LCD_DRIVER
/* lcd parameters */
#define BITS_PER_PIXEL   1
#define PIXEL_WIDTH     98
#define PIXEL_HEIGHT    64
#define BYTE_HEIGHT	(PIXEL_HEIGHT / 8)
#define BYTE_WIDTH 	(PIXEL_WIDTH)
#define DISPLAY_BUF_SIZE ((PIXEL_HEIGHT/8) * (PIXEL_WIDTH))

/* video buffer */
static cyg_uint8 video_buf[DISPLAY_BUF_SIZE];
#endif /* FULL_LCD_DRIVER */

static bool lcd_init(struct cyg_devtab_entry * tab);
static Cyg_ErrNo lcd_lookup(struct cyg_devtab_entry ** tab,
			    struct cyg_devtab_entry * sub_tab,
			    const char * name);
static Cyg_ErrNo lcd_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 * len);
static Cyg_ErrNo lcd_read(cyg_io_handle_t handle, void * buf, cyg_uint32 * len);
static Cyg_ErrNo lcd_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len);
static Cyg_ErrNo lcd_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * len);

static cyg_io_handle_t pm_devH;
#ifdef FULL_LCD_DRIVER
static void clear_display(void);
static void invert_display(void);
static void refresh(unsigned char * buf, int size);
#endif /* FULL_LCD_DRIVER */

DEVIO_TABLE(io_lcd_devio,
	    lcd_write,
	    lcd_read,
	    lcd_get_config,
	    lcd_set_config
    );

DEVTAB_ENTRY(io_lcd_devtab,
	     DDODAT_IO_LCD_NAME,
	     DDODAT_IO_PM_NAME,
	     &io_lcd_devio,
	     lcd_init,
	     lcd_lookup,
	     0
    );

static bool
lcd_init(struct cyg_devtab_entry * tab)
{
#ifdef FULL_LCD_DRIVER
    /* release the reset state */
    *(volatile cyg_uint8 *)PDDR |= LCD_nRESET;
    
    /* intialized state (default) */
    /* from the datasheet:
     * display off, normal display, adc normal,
     * power control 000, lcd bias 1/9, all-indicator lamps off,
     * power saving clear, read modify write off, static indicator off
     * display start line set to first line, column addr set to 0
     * page addr set to 0, electronic volume clear, test mode clear */

    /* power saver start */
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_EN | OFF;/* should be redundant */
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_ALL_ON | ON;
    
    /* function setup */
    /* lcd bias setting */
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_BIAS_VAL | BIAS_1_9;/* should be redundant */
    /* adc selection */
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_SEG_SEQ | DNORM;
    /* common output state selection */
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_COM_SEQ | DREV;
    /* set resistance radio for V5 voltage regulation */
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_V5GAIN_VAL | 2;
    /* electronic volume control, aka contrast */
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_CONTRAST_EN;
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_CONTRAST_VAL | 0x2f;
    
    /* power saver off */
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_ALL_ON | OFF;

    /* power control setting */
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_PWR_CTRL_VAL | 7;
#endif /* FULL_LCD_DRIVER */

    return true;
}

static Cyg_ErrNo
lcd_lookup(struct cyg_devtab_entry ** tab,
	   struct cyg_devtab_entry * sub_tab,
	   const char * name)
{
    /* set pointer to pm device */
    pm_devH = (cyg_io_handle_t)sub_tab;
    return ENOERR;
}

static Cyg_ErrNo
lcd_write(cyg_io_handle_t handle, const void * buf, cyg_uint32 * len)
{
    return -ENOSYS;
}

static Cyg_ErrNo
lcd_read(cyg_io_handle_t handle, void * buf, cyg_uint32 * len)
{
    return -ENOSYS;
}

static Cyg_ErrNo
lcd_get_config(cyg_io_handle_t handle, cyg_uint32 key, void * buf, cyg_uint32 * len)
{
    Cyg_ErrNo res = ENOERR;

    switch (key)
    {
#ifdef FULL_LCD_DRIVER
	case IO_LCD_GET_CONFIG_VIDEO_POINTER:
	    buf = video_buf;
	    *len = DISPLAY_BUF_SIZE;
	    return ENOERR;
#endif /* FULL_LCD_DRIVER */
	default:
	    res = -EINVAL;
	    break;
    }
    return res;
}

static Cyg_ErrNo
lcd_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void * buf, cyg_uint32 * _len)
{
    cyg_uint32 len;
    
    switch (key)
    {
#ifdef FULL_LCD_DRIVER
	case IO_LCD_SET_CONFIG_REFRESH_DISPLAY:
	    refresh(buf, *_len);
	    return ENOERR;
	    
	case IO_LCD_SET_CONFIG_INVERT_DISPLAY:
	    invert_display();
	    return ENOERR;
	    
	case IO_LCD_SET_CONFIG_CLEAR_DISPLAY:
	    clear_display();
	    return ENOERR;
	    
	case IO_LCD_SET_CONFIG_BACKLIGHT_ON:
	    *(volatile cyg_uint8 *)PBDR |= LCD_BACKLIGHT;
	    return ENOERR;

	case IO_LCD_SET_CONFIG_BACKLIGHT_OFF:
	    *(volatile cyg_uint8 *)PBDR &= ~LCD_BACKLIGHT;
	    return ENOERR;
#endif /* FULL_LCD_DRIVER */
	    
	case IO_PM_SET_CONFIG_POWER_DOWN:
	    diag_printf("Powering down LCD\n");
	    /* power saver on command */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_EN | OFF;    /* turn display off */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_ALL_ON | ON; /* display all points on */
	    /* power state will now be sleep */

	    *(volatile cyg_uint8 *)PDDR &= ~LCD_nRESET;
	    return ENOERR;

	case IO_PM_SET_CONFIG_POWER_UP:
	    diag_printf("Powering up LCD\n");
#if 0
	    /* power saver off command */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_ALL_ON | OFF; /* display all points off */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_EN | ON; /* turn display on */
#else
	    /* release the reset state */
	    *(volatile cyg_uint8 *)PDDR |= LCD_nRESET;
    
	    /* intialized state (default) */
	    /* from the datasheet:
	     * display off, normal display, adc normal,
	     * power control 000, lcd bias 1/9, all-indicator lamps off,
	     * power saving clear, read modify write off, static indicator off
	     * display start line set to first line, column addr set to 0
	     * page addr set to 0, electronic volume clear, test mode clear */

	    /* power saver start */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_EN | OFF;/* should be redundant */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_ALL_ON | ON;
    
	    /* function setup */
	    /* lcd bias setting */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_BIAS_VAL | BIAS_1_9;/* should be redundant */
	    /* adc selection */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_SEG_SEQ | DNORM;
	    /* common output state selection */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_COM_SEQ | DREV;
	    /* set resistance radio for V5 voltage regulation */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_V5GAIN_VAL | 2;
	    /* electronic volume control, aka contrast */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_CONTRAST_EN;
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_CONTRAST_VAL | 0x2f;
	    
	    /* power saver off */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_ALL_ON | OFF;

	    /* power control setting */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_PWR_CTRL_VAL | 7;

	    /* clear the display */
	    {
		int i;
		int j;
		int StartAddr;						// Local calculation Register
		int EndAddr;						// Local Calculation Register

		for (j = 0; j < LCD_NUM_PAGES; ++j) {			// A Page is 8 lines
		    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_PAG_VAL + j; //  (3) Page Address Value
		    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_COL_VAL_HI + 0;//  (4) Column Address Value Upper 	(A7-A4)
		    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_COL_VAL_LO + 0;//  (4) Column Address Value Lower 	(A3-A0)

		    StartAddr = LCD_NUM_COLS * j;		    	// Calculations Outside of i loop
		    EndAddr = LCD_NUM_COLS * (j+1);
		    for(i = StartAddr; i < EndAddr; ++i) {
			*(volatile cyg_uint8 *)LCD_DATA = 0;	// Write Data to Display
		    }
		}
	    }

	    /* reverse the display */
	    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_MODE | REV;
#endif
	    return ENOERR;

	    /* handle these by passing them down to the pm device */
	case IO_PM_SET_CONFIG_REGISTER:
	case IO_PM_SET_CONFIG_UNREGISTER:
	    len = sizeof(cyg_io_handle_t);
	    return cyg_io_set_config(pm_devH, key, handle, &len);
	    
	default:
	    return -EINVAL;
	    break;
    }
}

#ifdef FULL_LCD_DRIVER
static void
clear_display(void)
{
    int i;
    int j;
    int StartAddr;						// Local calculation Register
    int EndAddr;						// Local Calculation Register

    for (j = 0; j < LCD_NUM_PAGES; ++j) {			// A Page is 8 lines
	*(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_PAG_VAL + j; 		//  (3) Page Address Value
	*(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_COL_VAL_HI + 0;	//  (4) Column Address Value Upper 	(A7-A4)
	*(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_COL_VAL_LO + 0;	//  (4) Column Address Value Lower 	(A3-A0)

	StartAddr = LCD_NUM_COLS * j;		    	// Calculations Outside of i loop
	EndAddr = LCD_NUM_COLS * (j+1);
	for(i = StartAddr; i < EndAddr; ++i) {
	    *(volatile cyg_uint8 *)LCD_DATA = 0;	// Write Data to Display
	}
    }
    return;
}

static void
invert_display(void)
{
    static cyg_uint8 display_mode = NORM;
    
    display_mode = (1 - display_mode);
    *(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_MODE | display_mode; //  (9) Display Mode
    return;
}

static void
refresh(unsigned char * buf, int size)
{
    int i;
    int j;

    for (j = 0; j < LCD_NUM_PAGES; ++j) {				// A Page is 8 lines
	*(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_LIN_VAL + 0;      	//  (2) Display Start Line Value
	*(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_PAG_VAL + j; 		//  (3) Page Address Value
	*(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_COL_VAL_HI + 0;	//  (4) Column Address Value Upper 	(A7-A4)
	*(volatile cyg_uint8 *)LCD_CMD = LCD_CMD_COL_VAL_LO + 0;	//  (4) Column Address Value Lower 	(A3-A0)
	for (i = 0; i < (LCD_NUM_COLS * 8); i += 8) {
	    *(volatile cyg_uint8 *)LCD_DATA = video_buf[i + (7 - j)];	// Write Data to Display
	}
    }
    return;
}
#endif /* FULL_LCD_DRIVER */
