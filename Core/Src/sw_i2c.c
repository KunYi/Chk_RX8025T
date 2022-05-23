
#include "stm32f1xx_hal.h"
#include <stdint.h>


#define CMD_WRITE       (0)
#define CMD_READ        (1)
#define SCL_Pin         GPIO_PIN_14
#define SCL_GPIO_Port   GPIOB
#define SDA_Pin         GPIO_PIN_13
#define SDA_GPIO_Port   GPIOB

#define SCL_L()         HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET)
#define SCL_H()         HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET)
#define READ_SCL()      (HAL_GPIO_ReadPin(SCL_GPIO_Port, SCL_Pin) == GPIO_PIN_SET)
#define SDA_L()         HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET)
#define SDA_H()         HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET)
#define READ_SDA()      (HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin) == GPIO_PIN_SET)

#define set_SCL()       HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET)
#define clear_SCL()     HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET)
#define read_SCL()      (HAL_GPIO_ReadPin(SCL_GPIO_Port, SCL_Pin) == GPIO_PIN_SET)
#define set_SDA()       HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET)
#define clear_SDA()     HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET)
#define read_SDA()      (HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin) == GPIO_PIN_SET)
#define I2C_delay()     delayUs(24)

#define delayParameters (200)
static uint8_t started = 0;

static void delayUs(volatile uint32_t nCount) {
  for (; nCount != 0; nCount--)
    ;
}

void rtcBusInit(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();

  SDA_H();
  SCL_H();

  /*Configure GPIO pins : SCL_Pin and SDA_Pin */
  GPIO_InitStruct.Pin = SCL_Pin | SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	I2C_delay();
}

void i2c_start_cond(void) {
  if (started) {
    // if started, do a restart condition
    // set SDA to 1
    set_SDA();
    I2C_delay();
    set_SCL();
    while (read_SCL() == 0) { // Clock stretching
      // You should add timeout to this loop
    }
    // Repeated start setup time, minimum 4.7us
    I2C_delay();
  }

  if (read_SDA() == 0) {
    // arbitration_lost();
  }

  // SCL is high, set SDA from 1 to 0.
  clear_SDA();
  I2C_delay();
  clear_SCL();
  started = 1;
}

void i2c_stop_cond(void) {
  // set SDA to 0
  clear_SDA();
  I2C_delay();

  set_SCL();
  // Clock stretching
  while (read_SCL() == 0) {
    // add timeout to this loop.
  }

  // Stop bit setup time, minimum 4us
  I2C_delay();

  // SCL is high, set SDA from 0 to 1
  set_SDA();
  I2C_delay();

  if (read_SDA() == 0) {
    // arbitration_lost();
  }

  started = 0;
}

// Write a bit to I2C bus
void i2c_write_bit(uint8_t bit) {
  if (bit) {
    set_SDA();
  } else {
    clear_SDA();
  }

  // SDA change propagation delay
  I2C_delay();

  // Set SCL high to indicate a new valid SDA value is available
  set_SCL();

  // Wait for SDA value to be read by target, minimum of 4us for standard mode
  I2C_delay();

  while (read_SCL() == 0) { // Clock stretching
    // You should add timeout to this loop
  }

  // SCL is high, now data is valid
  // If SDA is high, check that nobody else is driving SDA
  if (bit && (read_SDA() == 0)) {
    // arbitration_lost();
  }

  // Clear the SCL to low in preparation for next change
  clear_SCL();
}

// Read a bit from I2C bus
uint8_t i2c_read_bit(void) {
  uint8_t bit;

  // Let the target drive data
  set_SDA();

  // Wait for SDA value to be written by target, minimum of 4us for standard
  // mode
  I2C_delay();

  // Set SCL high to indicate a new valid SDA value is available
  set_SCL();

  while (read_SCL() == 0) { // Clock stretching
    // You should add timeout to this loop
  }

  // Wait for SDA value to be written by target, minimum of 4us for standard
  // mode
  I2C_delay();

  // SCL is high, read out bit
  bit = read_SDA();

  // Set SCL low in preparation for next operation
  clear_SCL();

  return bit;
}

// Write a byte to I2C bus. Return 0 if ack by the target.
uint8_t i2c_write_byte(uint8_t send_start, uint8_t send_stop,
                       uint8_t byte) {
  uint8_t bit;
  uint8_t nack;

  if (send_start) {
    i2c_start_cond();
		delayUs(1);
  }

  for (bit = 0; bit < 8; ++bit) {
    i2c_write_bit((byte & 0x80) != 0);
    byte <<= 1;
  }

  nack = i2c_read_bit();

  if (send_stop) {
    i2c_stop_cond();
  }

  return nack;
}

// Read a byte from I2C bus
uint8_t i2c_read_byte(uint8_t nack, uint8_t send_stop) {
  uint8_t byte = 0;
  uint8_t bit;

  for (bit = 0; bit < 8; ++bit) {
    byte = (byte << 1) | i2c_read_bit();
  }

  i2c_write_bit((nack & 0x01) == 1);

  if (send_stop) {
    i2c_stop_cond();
  }

  return byte;
}
#if 0
static void rtcI2CStart(void) {
  SDA_H();
  SCL_H();
  delayUs(delayParameters);
  SDA_L();
  delayUs(delayParameters);
}

static void rtcI2CStop(void) {
  SDA_L();
  SCL_H();
  delayUs(delayParameters);
  SDA_H();
  delayUs(delayParameters);
}

static uint8_t rtcI2CAck(void) {
  int i;
  // RTC_SDA_IN_MODE();
  SCL_L();
  delayUs(delayParameters);
  SCL_H();
  delayUs(delayParameters);
  for (i = 0; i < 20; i++) {
    if (READ_SDA() == 0)
      return 1;
  }
  return 0;
}

static void rtcI2CSendNack(void) {
  SDA_H();
  SCL_L();
  delayUs(delayParameters);
  SCL_H();
  delayUs(delayParameters);
}

void rtcI2CDataOut(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    if ((data & (0x80 >> i)) != 0)
      SDA_H();
    else
      SDA_L();
    SCL_L();
    delayUs(delayParameters);
    SCL_H();
    delayUs(delayParameters);
  }
}

uint8_t rtcI2CDataIn(void) {
  // uint8_t result = 0;
  uint8_t i;
  uint8_t d;
  d = 0;

  SDA_H();
  for (i = 0; i < 8; i++) {
    SCL_L();
    delayUs(delayParameters);
    SCL_H();
    delayUs(delayParameters);
    if (READ_SDA())
      d++;
    d <<= 1;
  }
  // RTC_SDA_OUT_MODE();
  delayUs(delayParameters);
  return d;
}
#endif

uint8_t rtcI2CWriteBytes(uint8_t addr, uint8_t reg,const uint8_t *array, uint8_t len)
{
  uint8_t ack;
	unsigned i;
	
  ack = i2c_write_byte(1, 0, (addr << 1) + CMD_WRITE);
  if (ack) {
    while(1); // failed
  }

  ack = i2c_write_byte(0, 0, reg << 4);
  if (ack) {
    while(1); // failed
  }
	for (i = 0; i < (len-1); i++) {
		ack = i2c_write_byte(0, 0, array[i]);
		if (ack) {
			while(1); // failed
		}
	}
	ack = i2c_write_byte(0, 1, array[i]);
	if (ack) {
		while(1); // failed
	}
  return 1;	
}

uint8_t rtcI2CWriteByte(uint8_t addr, uint8_t reg, uint8_t val) {
  uint8_t ack;
  ack = i2c_write_byte(1, 0, (addr << 1) + CMD_WRITE);
  if (ack) {
    while(1); // failed
  }

  ack = i2c_write_byte(0, 0, reg << 4);
  if (ack) {
    while(1); // failed
  }

  ack = i2c_write_byte(0, 1, val);
  if (ack) {
    while(1); // failed
  }
  return 1;
}

uint8_t rtcI2CReadByte(uint8_t addr, uint8_t reg) {
  uint8_t result;
  uint8_t ack;

  ack = i2c_write_byte(1, 0, (addr << 1) + CMD_WRITE);
  if (ack) {
    while(1); // failed
  }

  ack = i2c_write_byte(0, 0, reg);
  if (ack) {
    while(1); // failed
  }

  ack = i2c_write_byte(1, 0, (addr << 1) + CMD_READ);
  if (ack) {
    while(1); // failed
  }

  result = i2c_read_byte(1, 1);
  return result;
}

uint8_t rtcSimpleReadByte(uint8_t addr, uint8_t reg)
{
  uint8_t result;
  uint8_t ack;
  ack = i2c_write_byte(1, 0, (addr << 1) + CMD_WRITE);
  if (ack) {
    while(1); // failed
  }
  ack = i2c_write_byte(0, 0, (reg << 4) + 4); // for simple read
  if (ack) {
    while(1); // failed
  }

  result = i2c_read_byte(1, 1);
  return result;
}

uint8_t rtcSimpleReadBytes(uint8_t addr, uint8_t reg, uint8_t *array, uint8_t len)
{
  uint8_t result;
  uint8_t ack;
	uint8_t i;
  ack = i2c_write_byte(1, 0, (addr << 1) + CMD_WRITE);
  if (ack) {
    while(1); // failed
  }
  ack = i2c_write_byte(0, 0, (reg << 4) + 4); // for simple read
  if (ack) {
    while(1); // failed
  }
	
	for (i = 0; i < len - 1; i++)
	{
		array[i] = i2c_read_byte(0, 0);
	}
  array[i] = i2c_read_byte(1, 1);
  return result;	
}

uint8_t chkDevOnBus(uint8_t addr)
{
  uint8_t ack;
  ack = i2c_write_byte(1, 0, (addr << 1) + CMD_WRITE);
	i2c_stop_cond();
  return (ack) ? 0 : 1;
}
