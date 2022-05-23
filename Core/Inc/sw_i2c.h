#ifndef _SW_I2C_H_
#define _SW_I2C_H_

void swI2CBusInit(void);
uint8_t swI2CWriteBytes(uint8_t addr, uint8_t reg,const uint8_t *array, uint8_t len);
uint8_t swI2CWriteByte(uint8_t addr, uint8_t reg, uint8_t val);
uint8_t swI2CReadByte(uint8_t addr, uint8_t reg);
uint8_t swI2CReadBytes(uint8_t addr, uint8_t reg, uint8_t *array, uint8_t len);
uint8_t chkDevOnBus(uint8_t addr);

#endif /* End of _SW_I2C_H_ */
