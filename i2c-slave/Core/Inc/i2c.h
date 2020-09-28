#ifndef I2C_H
#define I2C_H

#include "stm32g0xx_hal.h"
#include <stdlib.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

/*          Define length and size                    */
#define     BIT_LENGTH 							    ((u8)0x0008)     // A bit length
#define     DEFAULT_BUFF_SIZE           ((u8)0x000A)		 // Receive/send buff length
#define     I2C_para_length 					  ((u8)0x0005)		 // The number of arguments


/*          Define address and offet                  */
#define     base_addr 						      ((u8)0x0000)	   // Receive/send buff base index
#define     time_offset     			      ((u8)0x0001)		 // Data byte offset
#define     units_offset                ((u8)0x0002)		 // Units byte offset
#define     addr_offet 							   	((u8)0x0002)		 // Frequency address offset


/*          Define the rate and unit of time of I2C   */
#define     I2C_PD                      ((u8)0x000C)		 // A pulse width
#define     I2C_S 										  ((u8)0x0000)     // A second
#define     I2C_MS 											((u8)0x0001)     // A millisecond
#define     I2C_US 											((u8)0x0002)     // A microsecond
#define     I2C_S_TO_US 						  ((u32)1000000)     // second = microsecond * 1000000
#define     I2C_MS_TO_US 								 ((u32)1000)		 // millisecond = microsecond * 1000


//I2C RISING ENABLE AND DISABLE ADDR
#define     I2C_SCL_EXTI_ENABLE_ADDR 	   	((u8)0x0020)     // SCL EXTI enable register
#define     I2C_SCL_EXTI_DISABLE_ADDR    ~((u8)0x0020)     // SCL EXTI disable register

//LED port select
#define     LED_GREEN_Pin 							  GPIO_PIN_5     // LED GPIO
#define     LED_GREEN_GPIO_Port                GPIOA     // LED IOPORT


//Slave address
#define     I2C_ADDRESS                 ((u8)0x00A0)     // As a slave address
#define     SELF_ADDRESS_READ						((u8)0x00A0)     // The master's write is equal to slave's read
#define     SELF_ADDRESS_WRITE					((u8)0x00A1)     // The master's read is equal to slave's write




//I2C port select
#define     I2C_PORT_GPIO 										GPIOC     // I2C IOPORT
#define     I2C_SDA_PORT						  I2C_PORT_GPIO     // I2C SDA IOPORT
#define     I2C_SCL_PORT							I2C_PORT_GPIO     // I2C SCL IOPORT
#define     I2C_SDA_PIN 								 GPIO_PIN_4     // I2C SDA GPIO
#define     I2C_SCL_PIN 								 GPIO_PIN_5     // I2C SCL GPIO


// Command Set or Get
#define     Set 											 ((u8)0x00FF)     // Set the frequency and duration of the LED
#define     Get 											 ((u8)0x0000)     // Get the frequency and duration of the LED


// light on and light off frequency , duration of the Green LED 
#define     LED_frequency							    ((u8)0x000F)     // LED frequency command
#define     LED_duration 							    ((u8)0x00F0)     // LED duration command
#define     LED_duration_frequency				((u8)0x00FF)     // LED frequency and duration command


//Unit of time   -----    s,  ms, us
typedef enum Option{
					read   =    ((u8)0x0000),             /*          Receive data state          */
					write,														 	  /*          Write data  state           */
					ret,																  /*          Initialization state        */
}Option;



//SET SDA LEVEL
#define     I2C_SDA_1() 			I2C_SDA_PORT->BSRR = I2C_SDA_PIN							// SDA = 1
#define     I2C_SDA_0() 			I2C_SDA_PORT->BRR = I2C_SDA_PIN								// SDA = 0


//SET SCL LEVEL
#define     I2C_SCL_1() 			I2C_PORT_GPIO->BSRR = I2C_SCL_PIN							// SCL = 1 
#define     I2C_SCL_0() 			I2C_PORT_GPIO->BRR = I2C_SCL_PIN  						// SCL = 0 


//#define     I2C_SDA_READ() (I2C_PORT_GPIO->IDR & I2C_SDA_PIN)							// read SDA status
//#define     I2C_SCL_READ() (I2C_PORT_GPIO->IDR & I2C_SCL_PIN)							// read SCL status
#define     I2C_SDA_READ() 		HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN)		// Read SDA level
#define     I2C_SCL_READ() 		HAL_GPIO_ReadPin(I2C_SCL_PORT, I2C_SCL_PIN)		// Read SCL level


//LED Control
#define     LED_ON HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET)        // LED4 UP, PA5 = high level
#define     LED_OFF HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET)     // LED4 DOWN, PA5 = low level


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



