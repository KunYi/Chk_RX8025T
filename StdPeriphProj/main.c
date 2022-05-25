
#include "stm32f10x.h"
#include "sw_i2c.h"

int main(void) {
	uint8_t data[3];
	swI2CBusInit();
	data[0] = swI2CReadByte(0x32, 0x0E);
	if (data[0] & (1<<1))
	{
    data[0] = (1 << 1); // REG_EXT
    data[1] = 0;        // REG_FLAG
    data[2] = (1 << 6); // REG_CTRL
		swI2CWriteBytes(0x32, 0x0D, data, 3);
	}
	while(1);
	//return 0;
}
