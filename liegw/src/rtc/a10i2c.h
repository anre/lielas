#ifndef A10I2C_H
#define A10I2C_H

#include <stdint.h>

  void i2c_init(int address, int i2c);
  int i2c_read(uint8_t *buf, int len);
  int i2c_writeb(uint8_t adr, uint8_t *buf, int len);


#endif

