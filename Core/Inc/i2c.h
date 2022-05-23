#ifndef _I2C_H_
#define _I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x_gpio.h"

/*******************************************************************************
 * 注意: RX8025的Tbuf为1300ns 但在通信中发生计时数据的进位时 
 *       需要61000ns(Tsp)的总线空闲时间更新寄存器
 ******************************************************************************/
/****** I2C时序        参数说明                  标准模式  高速模式 RX8025 ****/
#define Thd_sta 46  // (重复)起始条件的保持时间  4000ns    600ns    600ns
#define Tlow    95  // SCL时钟的低电平周期       4700ns    1300ns   1300ns
#define Thigh   46  // SCL时钟的高电平周期       4000ns    600ns    600ns
#define Tsu_sta 46  // 重复起始条件的建立时间    4700ns    600ns    600ns
#define Thd_dat 0   // SDA数据保持时间           0         0        0
#define Tsu_dat 15  // SDA数据建立时间           250ns     100ns    200ns
#define Tsu_sto 46  // 停止条件的建立时间        4000ns    600ns    600ns
#define Tbuf    4500// 停止和启动之间的总线空闲  1300ns    1300ns   61000ns(Tsp)
#define ACK     0   // 应答   (SDA低电平)
#define NACK    1   // 无应答 (SDA高电平)

/* SCL PB14, SDA PB13
/******************** SCL PA0 ************* SDA PA1 ***************************/
#define I2C_SCL_W(X)  X ? (GPIOB->BSRR = (1 << 14)):(GPIOB->BSRR = (1 << 30));
#define I2C_SDA_W(X)  X ? (GPIOB->BSRR = (1 << 13)):(GPIOB->BSRR = (1 << 29));
#define I2C_SCL_R()   ((GPIOA->IDR & (1<<14) != 0)    // PB13
#define I2C_SDA_R()   ((GPIOA->IDR & (1<<13) != 0)    // PB14

void    I2C_Delay(uint32_t t);
void    I2C_Start(void);
void    I2C_Stop(void);
uint8_t I2C_SendByte(uint8_t dat);
uint8_t I2C_ReadByte(uint8_t ack);

#ifdef __cplusplus
}
#endif

#endif