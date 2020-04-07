#include <dadio/io/i2c.h>
#include <cyg/hal/hal_edb7xxx.h>

/* i2c lines on port b */
#define I2C_SCL 0x08
#define I2C_SDA 0x04

#define I2C_WRITE 0x00
#define I2C_READ  0x01

#define WAIT_1MS 0x4800
#define WAIT_1US 0x000b

static void
wait(unsigned int cnt)
{
    while(cnt--)
	;
}

static void
write_dac(cyg_uint8 dat)
{
    unsigned int i;
    cyg_uint8 c = 0x80;

    for (i = 0; i < 8; i++) {
	*((volatile cyg_uint8 *)PBDR) &= ~I2C_SCL;			// Clock low
	wait(WAIT_1US * 8);

	if(dat & c)							// Set data
	    *((volatile cyg_uint8 *)PBDR) |= I2C_SDA;
	else
	    *((volatile cyg_uint8 *)PBDR) &= ~I2C_SDA;
	wait(WAIT_1US * 16);

	*((volatile cyg_uint8 *)PBDR) |= I2C_SCL;			// Clock high
	wait(WAIT_1US * 16);

	c = c >> 1;							// Shift bit
    }
    *((volatile cyg_uint8 *)PBDR) &= ~I2C_SCL;			// Clock low
    wait(WAIT_1US * 8);
    *((volatile cyg_uint8 *)PBDR) &= ~I2C_SDA;			// Data low
    wait(WAIT_1US * 4);

    *((volatile cyg_uint8 *)PBDDR) &= ~I2C_SDA;			// Turn SDA into input for ACK
    wait(WAIT_1US * 4);

    *((volatile cyg_uint8 *)PBDR) |= I2C_SCL;			// Clock high for ACK
    wait(WAIT_1US * 16);

    *((volatile cyg_uint8 *)PBDR) &= ~I2C_SCL;			// Clock low
    wait(WAIT_1US * 4);

    *((volatile cyg_uint8 *)PBDDR) |= I2C_SDA;			// Turn SDA back to output
    wait(WAIT_1US * 4);
}

void
i2c_write(cyg_uint8 addr, cyg_uint8 reg, cyg_uint8 value)
{
    wait(WAIT_1MS);
    *((volatile cyg_uint8 *)PBDR) &= ~I2C_SDA;	// Start 
    wait(WAIT_1US * 16);
    
    write_dac(addr|I2C_WRITE);
    write_dac(reg);
    write_dac(value);
    
    *((volatile cyg_uint8 *)PBDR) |= I2C_SCL;	// Clock high
    wait(WAIT_1US * 16);
    
    wait(WAIT_1US * 8);
    *((volatile cyg_uint8 *)PBDR) |= I2C_SDA;	// Stop 
    wait(WAIT_1US * 16);
}

void
i2c_init(void)
{
    //    *((volatile unsigned char *)PBDDR) |= (DAC_SCL|DAC_SDA); /* 0x0c */
    *((volatile unsigned char *)PBDR) &= ~(I2C_SCL|I2C_SDA);
    *((volatile unsigned char *)PBDR) |= (I2C_SCL|I2C_SDA);
    wait(WAIT_1MS);
}
