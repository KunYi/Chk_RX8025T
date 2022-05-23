
#include "i2c.h"

/**
  * *brief  delay func.
  * *param  delay time = t * 14ns
  * *retval none
  */

void I2C_Delay(uint32_t t)
{
    SysTick->LOAD = tick;
    SysTick->CTRL = 0x00000005;
    while(!(SysTick->CTRL & 0x00010000));
    SysTick->CTRL = 0x00000000;
}

/**
  * @brief  i2c start func.
  * @param  none
  * @retval none
  */

void I2C_Start(void)
{
    I2C_SDA_W(1);
    I2C_Delay(Tsu_dat);     // Tsu;dat  S > 250ns, F > 100ns
    I2C_SCL_W(1);
    I2C_Delay(Thigh);       // Thigh    S > 4.0us, F > 0.6us

    I2C_SCL_W(1);
    I2C_Delay(Tsu_sta);     // Tsu;sta  S > 4.7us, F > 0.6us
    I2C_SDA_W(0);
    I2C_Delay(Thd_sta);     // Thd;sta  S > 4.0us, F > 0.6us
    I2C_SCL_W(0);
    I2C_Delay(Tlow);        // Tlow     S > 4.7us, F > 1.3us
}

/**
  * @brief  i2c stop func.
  * @param  none
  * @retval none
  */

void I2C_Stop(void)
{
    I2C_SCL_W(0);
    I2C_Delay(Tlow);        // Tlow     S > 4.7us, F > 1.3us
    I2C_SDA_W(0);
    I2C_Delay(Tsu_dat);     // Tsu;dat  S > 250ns, F > 100ns
    I2C_SCL_W(1);
    I2C_Delay(Tsu_sto);     // Tsu;sto  S > 4.0us, F > 0.6us
    I2C_SDA_W(1);
    I2C_Delay(Tbuf);        // Tbuf(Tsp)S > 4.7us, F > 1.3us, for RX8025 > 61us
}

/**
  * @brief  i2c send data func.
  * @param  1 byte data
  * @retval slave response signal: ACK(0) or NACK(1)
  */

uint8_t I2C_SendByte(uint8_t dat)
{
    uint8_t i;
    for(i = 0; i < 8; i ++)
    {
        if((dat >> 7) & 1)  // MSB first
        {
            I2C_SDA_W(1);
        }
        else
        {
            I2C_SDA_W(0);
        }

        dat <<= 1;
        I2C_Delay(Tsu_dat); // Tsu;dat  S > 250ns, F > 100ns, for rx8025 > 200ns
        I2C_SCL_W(1);
        I2C_Delay(Thigh);   // Thigh    S > 4.0us, F > 0.6us
        I2C_SCL_W(0);
        I2C_Delay(Tlow);    // Tlow     S > 4.7us, F > 1.3us
    }

    I2C_SDA_W(1);
    I2C_Delay(Tsu_dat);     // Tsu;dat  S > 250ns, F > 100ns, for rx8025 > 200ns
    I2C_SCL_W(1);
    I2C_Delay(Thigh);       // Thigh    S > 4.0us, F > 0.6us

    i = 0;
    if( I2C_SDA_R() )
    {
        i = 1;
    }
    I2C_SCL_W(0);
    I2C_Delay(Tlow);        // Tlow     S > 4.7us, F > 1.3us

    return i;               // ACK == 0, NACK == 1
}

/**
  * @brief  i2c read data func.
  * @param  master response signal: ACK(0) or NACK(1)
  * @retval 1 byte data
  */

uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t   a;
    uint8_t   dat = 0;
    I2C_SDA_W(1);           // master release sda and go high
    I2C_Delay(Tsu_dat);     // Tsu;dat  S > 250ns, F > 100ns, for rx8025 > 200ns
    for(a = 0; a < 8; a ++)
    {
        I2C_SCL_W(1);
        I2C_Delay(Thigh);
        dat <<= 1;
        if( I2C_SDA_R() )
            dat |= 1;
        I2C_SCL_W(0);
        I2C_Delay(Tlow);
    }

    if(ack)                 // master response signal
    {
        I2C_SDA_W(1);       // master nack
    }
    else
    {
        I2C_SDA_W(0);       // master ack
    }

    I2C_Delay(Tsu_dat);     // Tsu;dat  S > 250ns, F > 100ns, for rx8025 > 200ns
    I2C_SCL_W(1);
    I2C_Delay(Thigh);       // Thigh    S > 4.0us, F > 0.6us
    I2C_SCL_W(0);
    I2C_Delay(Tlow);        // Tlow     S > 4.7us, F > 1.3us

    return dat;
}