#ifndef I2C_H
#define I2C_H

#include "stm32g0xx_hal.h"
#include <stdlib.h>

typedef		 uint32_t    u32;
typedef		 uint16_t    u16;
typedef		 uint8_t     u8;




/**************************************************************************/
/***********                    GPIO              ************************/
/**************************************************************************/


//SET SDA LEVEL
#define I2C_SDA_1() 			I2C_SDA_PORT->BSRR = I2C_SDA_PIN							// Set SDA = 1
#define I2C_SDA_0() 			I2C_SDA_PORT->BRR = I2C_SDA_PIN								// Set SDA = 0


//SET SCL LEVEL
#define I2C_SCL_1() 			I2C_PORT_GPIO->BSRR = I2C_SCL_PIN							// Set SCL = 1 
#define I2C_SCL_0() 			I2C_PORT_GPIO->BRR = I2C_SCL_PIN  						// Set SCL = 0 


//#define I2C_SDA_READ() (I2C_PORT_GPIO->IDR & I2C_SDA_PIN)							// Read SDA status
//#define I2C_SCL_READ() (I2C_PORT_GPIO->IDR & I2C_SCL_PIN)							// Read SCL status
#define I2C_SDA_READ() 				HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN)		// Read SDA level
#define I2C_SCL_READ() 				HAL_GPIO_ReadPin(I2C_SCL_PORT, I2C_SCL_PIN)		// Read SCL level


//LED Control
#define LED_ON HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET)        // LED4 UP, PA5 = high level
#define LED_OFF HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET)     // LED4 DOWN, PA5 = low level


void		 I2C_GPIO_Slave_To_Master(void);
void		 I2C_GPIO_Master_To_Slave(void);





/**************************************************************************/
/***********                    LED              **************************/
/**************************************************************************/

#define LED_GREEN_Pin               GPIO_PIN_5            
#define LED_GREEN_GPIO_Port         GPIOA

void		  LED(u32);
void 			LED_GPIO_Init(void);





/**************************************************************************/
/***********                    SLAVE              ************************/
/**************************************************************************/


/*      Define length and size                        */
#define BIT_LENGTH 					    		    ((u8)0x08)       // A bit length
#define DEFAULT_BUFF_SIZE               ((u8)0x0A)	  	 // Receive/send buff length
#define I2C_para_length 			    		  ((u8)0x05)	  	 // The number of arguments


/*      Define address and offet                      */
#define base_addr 					    	      ((u8)0x00)	     // Receive/send buff base index
#define time_offset     	    		      ((u8)0x01)	  	 // Data byte offset
#define units_offset                    ((u8)0x02)	  	 // Units byte offset
#define addr_offet 				    			   	((u8)0x02)	  	 // Frequency address offset


/*      Define the rate and unit of time of I2C        */
#define I2C_PD                          ((u8)0x0C)	  	 // A pulse width
#define I2C_S 				    						  ((u8)0x00)       // A second
#define I2C_MS 				    							((u8)0x01)       // A millisecond
#define I2C_US 													((u8)0x02)       // A microsecond
#define I2C_S_TO_US 				  				  ((u32)1000000)     // second = microsecond * 1000000
#define I2C_MS_TO_US 								    ((u32)1000)		     // millisecond = microsecond * 1000


// I2C RISING ENABLE AND DISABLE ADDR
#define I2C_SCL_EXTI_ENABLE_ADDR 	         	((u8)0x20)      // SCL EXTI enable register
#define I2C_SCL_EXTI_DISABLE_ADDR           ~((u8)0x020)     // SCL EXTI disable register


// LED port select
#define LED_GREEN_Pin 				  			     GPIO_PIN_5         // LED GPIO
#define LED_GREEN_GPIO_Port                GPIOA              // LED IOPORT


// Slave address
#define I2C_ADDRESS                     ((u8)0xA0)          // As a slave address
#define SELF_ADDRESS_READ				    		((u8)0xA0)          // The master's write is equal to slave's read
#define SELF_ADDRESS_WRITE			    		((u8)0xA1)          // The master's read is equal to slave's write




//I2C port select
#define I2C_PORT_GPIO 						  				GPIOC                 // I2C IOPORT
#define I2C_SDA_PORT						            I2C_PORT_GPIO         // I2C SDA IOPORT
#define I2C_SCL_PORT							          I2C_PORT_GPIO         // I2C SCL IOPORT
#define I2C_SDA_PIN 								        GPIO_PIN_4            // I2C SDA GPIO
#define I2C_SCL_PIN 								        GPIO_PIN_5            // I2C SCL GPIO


// Command Set or Get
#define Set 											 ((u8)0xFF)           // Set the frequency and duration of the LED
#define Get 											 ((u8)0x00)           // Get the frequency and duration of the LED


// light on and light off frequency , duration of the Green LED 
#define LED_frequency							    ((u8)0x0F)        // LED frequency command
#define LED_duration 							    ((u8)0xF0)        // LED duration command
#define LED_duration_frequency				((u8)0xFF)        // LED frequency and duration command


//Unit of time   -----    s,  ms, us
typedef enum Option{
					read   =    ((u8)0x0000),             /*          Receive data state          */
					write,														 	  /*          Write data  state           */
					ret,																  /*          Initialization state        */
}Option;




//The time delay function
void 			delay_us(u32);
void 			delay_ms(u32);


//GPIO Init
void 		 I2C_Slave_SCL_Falling_Exti_Enable(void);
void		 I2C_Slave_SCL_Falling_Exti_Reable(void);
void		 I2C_Slave_SCL_Falling_Exti_Disable(void);
void		 I2C_Slave_SCL_Rising_Exti_Disable(void);
void		 I2C_Slave_SCL_Rising_Exti_Enable(void);
void		 I2C_Slave_SDA_GPIO_Output_OD_Init(void);


//Start and stop signal
u8 			 is_I2C_Slave_Start(void);
u8 			 is_I2C_Slave_Stop(void);


//Acknowledge
void 		 I2C_Slave_SendAck(void);
void 		 I2C_Slave_SendNAck(void);
u8 			 I2C_Slave_WaitAck(void);




/**************************************************************************/
/***********                    MASTER              ***********************/
/**************************************************************************/

// Command Set or Get
#define Slave_Set            			((u8)0xFF)
#define Slave_Get 								((u8)0x00)

//Slave write and read address: write(0), read(1)
#define I2C_WRITE_ADDRESS 0xA0
#define I2C_READ_ADDRESS 0xA1


//Send and receive data
void 	   I2C_Master_SendByte(u8);
u32		   I2C_Master_Write(u8, u8 *, u32);
u32		   I2C_Master_Read(u8, u8 *, u8);


//Acknowledge
void 		 I2C_Master_SendAck(void);
void 		 I2C_Master_SendNAck(void);


//start and stop signal
void		 I2C_Master_Start(void);
void		 I2C_Master_Stop(void);

void I2C_Master_GPIO_Output_OD_Init(void);

#endif



