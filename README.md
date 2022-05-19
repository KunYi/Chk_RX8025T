check RX-8025T
===

the repositry for me verify RTC RX-8025T with STM32F103RC
to simualtion an I2C master by software

--------------

### Hardward configuration of board

* MCU: STM32F103RC
* SWD: enabled for download and debugging
* UART1: GPIOA.9/GPIOA.10 for debug message output
* HSE: Crystal/8 MHz and to configuration SYSCLK to 72MHz
* RTC(RX8025T): connect with GPIOB.13(SDA)/GPIOB.14(SCL)
* LED: GPIOB.0(LED1) and GPIOC.5(LED2)
