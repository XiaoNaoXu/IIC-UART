#ifndef I2C_H
#define I2C_H

#include "stm32g0xx_hal.h"
#include "main.h"

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

//Slave write and read address: write(0), read(1)
#define I2C_WRITE_ADDRESS 0xA0
#define I2C_READ_ADDRESS 0xA1

//I2C port select
#define I2C_PORT_GPIO GPIOC
#define I2C_SDA_PORT GPIOC
#define I2C_SCL_PORT GPIOC
#define I2C_SDA_PIN GPIO_PIN_4
#define I2C_SCL_PIN GPIO_PIN_5

//SCL
//#define I2C_SCL_1() I2C_PORT_GPIO->BSRR = I2C_SCL_PIN											  						// SCL = 1 
//#define I2C_SCL_0() I2C_PORT_GPIO->BSRR = (uint32_t)I2C_SCL_PIN << 16U  								// SCL = 0 
#define I2C_SCL_1() HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET)						//Set SCL = high level
#define I2C_SCL_0() HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_RESET)					//Set SCL = low level

//SDA
//#define I2C_SDA_1() I2C_PORT_GPIO->BSRR = I2C_SDA_PIN																		// SDA = 1
//#define I2C_SDA_0() I2C_PORT_GPIO->BSRR = (uint32_t)I2C_SDA_PIN << 16U									// SDA = 0
#define I2C_SDA_1() HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET)						//Set SDA = high level
#define I2C_SDA_0() HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_RESET)					//Set SDA = low level

//#define I2C_SDA_READ() (I2C_PORT_GPIO->IDR & I2C_SDA_PIN)																// read SDA status
//#define I2C_SCL_READ() (I2C_PORT_GPIO->IDR & I2C_SCL_PIN)																// read SCL status
#define I2C_SDA_READ() HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN)												//Read SDA level
#define I2C_SCL_READ() HAL_GPIO_ReadPin(I2C_SCL_PORT, I2C_SCL_PIN)												//Read SCL level

//LED Control
#define LED_ON HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET)        // LED4 UP, PA5 = high level
#define LED_OFF HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET)     // LED4 DOWN, PA5 = low level

//the time delay function
void delay_us(u32);
void delay_ms(u32);

//start and stop signal
void i2c_Start(void);
void i2c_Stop(void);

//acknowledge
void i2c_SendAck(void);
void i2c_SendNAck(void);
u8 i2c_WaitAck(void);

//Send and receive data
void i2c_SendByte(u8 *data_byte);
u32 i2c_ReadByte( void);
u32 I2C_Write(u8, u8 *, u32);
u32 I2C_Read(u8, u8 *, u8);


//i2c gpio init
void I2C_GPIO_Init(void);

//delay function
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

void LED(u32);
void test(void);

#endif



