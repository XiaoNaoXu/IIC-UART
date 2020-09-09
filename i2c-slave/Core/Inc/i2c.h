#ifndef I2C_H
#define I2C_H

#include "stm32g0xx_hal.h"
#include <stdlib.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define I2C_PD 10

//LED port select
#define LED_GREEN_Pin GPIO_PIN_5
#define LED_GREEN_GPIO_Port GPIOA


//Slave address
#define I2C_WRITE_ADDRESS 0xA0
#define I2C_READ_ADDRESS 0xA1
#define self_addr_read 0xA0
#define self_addr_write 0xA1



//I2C port select
#define I2C_PORT_GPIO GPIOC
#define I2C_SDA_PORT GPIOC
#define I2C_SCL_PORT GPIOC
#define I2C_SDA_PIN GPIO_PIN_4
#define I2C_SCL_PIN GPIO_PIN_5


//Command Set ----   Set the flicker frequency and duration of the Green LED 
#define Set_Frq_And_Dura 0x03
#define Set_Frq 0x01
#define Set_Dura 0x02


//Command Set ----   Get the flicker frequency and duration of the Green LED 
#define Get_Frq_And_Dura 0x30
#define Get_Frq 0x20
#define Get_Dura 0x10


//Unit of time   -----    s,  ms, us
enum Unit_Of_Time{
					us  = 0x05,
					ms,
					s,
};



//SET SCL LEVEL
//#define I2C_SCL_1() I2C_PORT_GPIO->BSRR = I2C_SCL_PIN											  						// SCL = 1 
//#define I2C_SCL_0() I2C_PORT_GPIO->BSRR = (uint32_t)I2C_SCL_PIN << 16U  								// SCL = 0 
#define I2C_SCL_1() HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET)						//Set SCL = high level
#define I2C_SCL_0() HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_RESET)					//Set SCL = low level


//SET SDA LEVEL
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


//The time delay function
void delay_us(u32);
void delay_ms(u32);


//GPIO Init
void i2c_slave_SCL_Falling_Exti_Enable(void);
void i2c_slave_SCL_Falling_Exti_Disable(void);
void i2c_slave_SCL_Rising_Exti_Enable(void);
void i2c_slave_SDA_GPIO_Input_Init(void);
//void i2c_slave_SDA_GPIO_Init(void);

//Send or Read a bit in Exti
//void i2c_ReadBit(void);
//void i2c_SenddBit(void);


//Start and stop signal
u8 is_i2c_Start(void);
u8 is_i2c_Stop(void);


//Acknowledge
void i2c_SendAck(void);
void i2c_SendNAck(void);
u8 i2c_WaitAck(void);


//Send and receive data
void i2c_SendByte(u8 *data_byte);
u32 i2c_ReadByte( void);
void I2C_Write(u8 *data, u32 data_length);
void I2C_Read(void);


//Test function
void test(void);
void callback(void);
void LED(u32);


#endif



