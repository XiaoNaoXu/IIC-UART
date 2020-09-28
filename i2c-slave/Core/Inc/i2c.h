#ifndef I2C_H
#define I2C_H

#include "stm32g0xx_hal.h"
#include <stdlib.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;


#define BIT_LENGTH 8
#define base_addr 0x00
#define time_offset 0x01
#define units_offset 0x02
#define addr_offet 0x02

//
#define I2C_PD 12
#define ADDRESS_INDEX 0
#define DEFAULT_BUFF_SIZE 255
#define I2C_S 0x00
#define I2C_MS 0x01
#define I2C_US 0x02
#define I2C_S_TO_US 1000000
#define I2C_MS_TO_US 1000

#define I2C_para_length 0x05

//I2C RISING ENABLE AND DISABLE ADDR
#define I2C_SCL_EXTI_ENABLE_ADDR 0x0020
#define I2C_SCL_EXTI_DISABLE_ADDR ~(0x0020)

//LED port select
#define LED_GREEN_Pin GPIO_PIN_5
#define LED_GREEN_GPIO_Port GPIOA


//Slave address
#define I2C_WRITE_ADDRESS 0xA0
#define I2C_READ_ADDRESS 0xA1
#define SELF_ADDRESS_READ 0xA0
#define SELF_ADDRESS_WRITE 0xA1
#define self_addr_read 0xA0
#define self_addr_write 0xA1



//I2C port select
#define I2C_PORT_GPIO GPIOC
#define I2C_SDA_PORT GPIOC
#define I2C_SCL_PORT GPIOC
#define I2C_SDA_PIN GPIO_PIN_4
#define I2C_SCL_PIN GPIO_PIN_5


// Command Set or Get
#define Set 0xFF
#define Get 0x00


// light on and light off frequency , duration of the Green LED 
#define LED_frequency 0x0F
#define LED_duration 0xF0
#define LED_duration_frequency 0xFF


//Unit of time   -----    s,  ms, us
typedef enum Option{
					read  = 0x00,
					write,
					ret,
}Option;



//SET SDA LEVEL
#define I2C_SDA_1() I2C_SDA_PORT->BSRR = I2C_SDA_PIN																			// SDA = 1
#define I2C_SDA_0() I2C_SDA_PORT->BRR = I2C_SDA_PIN																			// SDA = 0


//SET SCL LEVEL
#define I2C_SCL_1() I2C_PORT_GPIO->BSRR = I2C_SCL_PIN											  						// SCL = 1 
#define I2C_SCL_0() I2C_PORT_GPIO->BRR = I2C_SCL_PIN  																	// SCL = 0 


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
void i2c_slave_SCL_Falling_Exti_Reable(void);
void i2c_slave_SCL_Falling_Exti_Disable(void);
void i2c_slave_SCL_Rising_Exti_Disable(void);
void i2c_slave_SCL_Rising_Exti_Enable(void);
void i2c_slave_SDA_GPIO_Input_Init(void);
void i2c_slave_SDA_GPIO_Output_PP_Init(void);


//Start and stop signal
u8 is_i2c_Start(void);
u8 is_i2c_Stop(void);
void i2c_Start(void);
void i2c_Stop(void);


//Acknowledge
void i2c_SendAck(void);
void i2c_SendNAck(void);
u8 i2c_WaitAck(void);


//Send and receive data
void I2C_SendByte(u8 *data_byte);
u32 i2c_ReadByte( void);
u32 I2C_Write(u8, u8 *, u32);
void I2C_Read(void);


//Test function
void LED(u32);


#endif



