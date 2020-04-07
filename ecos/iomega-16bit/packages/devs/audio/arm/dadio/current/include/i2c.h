#ifndef I2C_H
#define I2C_H

#include <cyg/kernel/kapi.h>

void i2c_init(void);
void i2c_write(cyg_uint8 addr, cyg_uint8 reg, cyg_uint8 val);

#endif /* I2C_H */
