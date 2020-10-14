/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for i2c.c file.

  ******************************************************************************
  */

#ifndef I2C_H
#define I2C_H

#include "stm32g0xx_hal.h"
#include <stdlib.h>

typedef		 uint32_t    u32;
typedef		 uint16_t    u16;
typedef		 uint8_t     u8;



/**************************************************************************/
/***********                 State Control         ************************/
/**************************************************************************/

#define MASTER 											((u8)0x00)  				 /*    Running state is master   */
#define SLAVE 											((u8)0x01)  				 /*    Running state is slave    */
#define PROCESS_SUCCESS             ((u8)0x00)					 /*    return a success          */


/*     Record running state           */
typedef enum Running_State {
				master   =   MASTER,
	      slave    =   SLAVE
}Running_State;

extern Running_State running_state;
extern void (* running)(void);
extern void		 master_start(void);
extern u8 			I2C_Bus_state;


/**************************************************************************/
/***********                    GPIO              ************************/
/**************************************************************************/

/*      I2C port select																		  */
#define I2C_PORT_GPIO 						  				GPIOC                 // I2C IOPORT
#define I2C_SDA_PORT						            I2C_PORT_GPIO         // I2C SDA IOPORT
#define I2C_SCL_PORT							          I2C_PORT_GPIO         // I2C SCL IOPORT
#define I2C_SDA_PIN 								        GPIO_PIN_4            // I2C SDA GPIO
#define I2C_SCL_PIN 								        GPIO_PIN_5            // I2C SCL GPIO


/*      EXTI GPIO Select                                    */
#define MASTER_EXTI_PORT										GPIOC										// Master EXTI IOPORT
#define SLAVE_EXTI_PORT							  			GPIOC										// Slave EXTI IOPORT
#define MASTER_EXTI_PIN											GPIO_PIN_4							// Master EXTI GPIO
#define SLAVE_EXTI_PIN											GPIO_PIN_5							// Slave EXTI GPIO


/*      I2C Level High / Low                                */
#define I2C_LEVEL_HIGH							((u8)0x01)
#define I2C_LEVEL_LOW 							((u8)0x00)

/*      SET SDA LEVEL																			  */
#define I2C_SDA_1() 			I2C_SDA_PORT->BSRR = I2C_SDA_PIN							// Set SDA = 1
#define I2C_SDA_0() 			I2C_SDA_PORT->BRR = I2C_SDA_PIN								// Set SDA = 0


/*			SET SCL LEVEL																				*/
#define I2C_SCL_S_1() 			I2C_PORT_GPIO->BSRR = I2C_SCL_PIN							// Set SCL = 1 
#define I2C_SCL_0() 			I2C_PORT_GPIO->BRR = I2C_SCL_PIN  							// Set SCL = 0 


//#define I2C_SDA_READ() (I2C_PORT_GPIO->IDR & I2C_SDA_PIN)														// Read SDA status
//#define I2C_SCL_READ() (I2C_PORT_GPIO->IDR & I2C_SCL_PIN)														// Read SCL status
#define I2C_SDA_READ() 				HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN)							// Read SDA level
#define I2C_SCL_READ() 				HAL_GPIO_ReadPin(I2C_SCL_PORT, I2C_SCL_PIN)							// Read SCL level
#define MASTER_EXTI_PIN_READ	HAL_GPIO_ReadPin(MASTER_EXTI_PORT, MASTER_EXTI_PIN)			// Read Mater EXTI Pin level


/*      LED Control                                  			  */
#define LED_ON HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET)        // LED4 UP, PA5 = high level
#define LED_OFF HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET)     // LED4 DOWN, PA5 = low level


/*   		 Set SCL = high level														    */
void 		 I2C_SCL_1(void);





/**************************************************************************/
/***********                    LED              **************************/
/**************************************************************************/

/*        Define LED GPIO															*/
#define LED_GREEN_Pin               GPIO_PIN_5            
#define LED_GREEN_GPIO_Port         GPIOA

/*        LED control																  */
void		  LED(u32);
void 			LED_GPIO_Init(void);





/**************************************************************************/
/***********                    SLAVE              ************************/
/**************************************************************************/


/*      Define length and size                        */
#define BIT_LENGTH 					    		    ((u8)0x08)       // A bit length
#define DEFAULT_BUFF_SIZE               ((u8)0x0F)	  	 // Receive/send buff length
#define I2C_PARA_LENGTH 			    		  ((u8)0x05)	  	 // The number of arguments


/*      Define address and offet                      */
#define START_ADDR 					    	      ((u8)0x00)	     // Receive/send buff start index
#define BASE_ADDR 					    	      ((u8)0x01)	     // Receive/send buff base index
#define TIME_OFFSET     	    		      ((u8)0x01)	  	 // Data byte offset
#define UNITS_OFFSET                    ((u8)0x02)	  	 // Units byte offset
#define ADDR_OFFSET 				    			  ((u8)0x02)	  	 // Frequency address offset
#define STATE_OFFSET 				    		    ((u8)0x01)	  	 // Command address offset
#define RUNNING_STATE_OFFSET 				    ((u8)0x02)	  	 // State address offset


/*      Define the rate and unit of time of I2C        */
#define I2C_PD                          ((u8)0x05)	  	 // A pulse width
#define I2C_S 				    						  ((u8)0x73)       // A second
#define I2C_MS 				    							((u8)0x6D)       // A millisecond
#define I2C_US 													((u8)0x75)       // A microsecond
#define I2C_S_TO_US 				  				  ((u32)1000000)     // second = microsecond * 1000000
#define I2C_MS_TO_US 								    ((u32)1000)		     // millisecond = microsecond * 1000


/*			  I2C RISING ENABLE AND DISABLE ADDR					*/
#define I2C_SCL_EXTI_ENABLE_ADDR 	         	((u16)0x0020)       // SCL EXTI enable register
#define I2C_SCL_EXTI_DISABLE_ADDR           ~((u16)0x0020)     // SCL EXTI disable register
#define I2C_SDA_EXTI_ENABLE_ADDR 	         	((u16)0x0008)       // SCL EXTI enable register
#define I2C_SDA_EXTI_DISABLE_ADDR           ~((u16)0x0008)     // SCL EXTI disable register


/*			  LED port select 								            */
#define LED_GREEN_Pin 				  			     GPIO_PIN_5         // LED GPIO
#define LED_GREEN_GPIO_Port                GPIOA              // LED IOPORT


/* 			I2C address																	*/
#define I2C_ADDRESS                     ((u8)0xB0)          // As a slave address
#define I2C_ADDRESS_LEN                 ((u8)0x02)					// A address length



/*      Command Set or Get   																		     */
#define SET 											 ((u8)0xFF)           // Set the frequency and duration of the LED
#define GET 											 ((u8)0x00)           // Get the frequency and duration of the LED


/*		  light on and light off frequency , duration of the Green LED */
#define LED_FREQUENCY							    ((u8)0x02)        // LED frequency command
#define LED_DURATION 							    ((u8)0x01)        // LED duration command
#define LED_DURATION_FREQUENCY				((u8)0x03)        // LED frequency and duration command
#define RUNNING_STATE									((u8)0x04)				// running state command


/*			Unit of time   -----    s,  ms, us     											 */
typedef enum Option{
					read   =    ((u8)0x0000),             /*          Receive data state          */
					write,														 	  /*          Write data  state           */
					ret,																  /*          Initialization state        */
}Option;




/*				The time delay function  																   */
void 			delay_us(u32);
void 			delay_ms(u32);


/*			 GPIO Init																 	  								*/
void 		 I2C_Slave_SCL_Falling_Exti_Enable(void);
void		 I2C_Slave_SCL_Falling_Exti_Reable(void);
void		 I2C_Slave_SCL_Falling_Exti_Disable(void);
void		 I2C_Slave_SCL_Rising_Exti_Disable(void);
void		 I2C_Slave_SCL_Rising_Exti_Enable(void);
void		 I2C_Slave_SDA_GPIO_Output_OD_Init(void);


/*			 Start and stop signal																				*/
u8 			 is_I2C_Slave_Start(void);
u8 			 is_I2C_Slave_Stop(void);


/*			 Acknowledge																							    */
void 		 I2C_Slave_SendAck(void);
void 		 I2C_Slave_SendNAck(void);
u8 			 I2C_Slave_WaitAck(void);




/**************************************************************************/
/***********                    MASTER              ***********************/
/**************************************************************************/

/*                   I2C Bus Status                    				 			  */
#define I2C_BUS_FREE 															((u8)0x00)
#define I2C_BUS_BUSY															((u8)0xFF)


/*		  Command Set or Get  												 	 				        */
#define Slave_Set            			((u8)0xFF)
#define Slave_Get 								((u8)0x00)

/*			Slave write and read address: write(0), read(1)								*/
#define I2C_WRITE 									((u8)0x00)
#define I2C_READ 										((u8)0x01)


/*      I2C SDA RISING ENABLE AND DISABLE ADDR          	    			  */
#define I2C_SDA_EXTI_ENABLE_ADDR 	         	((u16)0x0008)       // SCL EXTI enable register
#define I2C_SDA_EXTI_DISABLE_ADDR           ~((u16)0x0008)     // SCL EXTI disable register


/*			 Send and receive data																	  		*/
u8	 	   I2C_Master_SendByte(u8);
u32		   I2C_Master_Write(u8, u8 *);
u32		   I2C_Master_Read(u8, u8 *);


/*			 Acknowledge																									*/
void 		 I2C_Master_SendAck(void);
void 		 I2C_Master_SendNAck(void);


//start and stop signal
void		 I2C_Master_Start(void);
void		 I2C_Master_Stop(void);


/*			 Master GPIO Init and EXTI Init																*/
void 		 I2C_Master_SCL_Output_OD_Init(void);
void 		 I2C_Master_SDA__OutputOD_Rising_Falling_Init(void);
void 		 I2C_Master_SDA_Falling_Disable(void);
void 		 I2C_Master_SDA_Falling_Enable(void);
void 		 I2C_Master_SDA_Rising_Enable(void);
void 		 I2C_Master_SDA_Rising_Disable(void);

#endif



/**************************************************************************/
/***********                UART PRINT              ***********************/
/**************************************************************************/


#define P_STATE_MASTER_TO_SLAVE							"\nState changed from master to slave.\n"
#define P_STATE_SLAVE_TO_MASTER							"\nState changed from slave to master.\n"
#define P_RECEIVED_START_SIGNAL							"\nReceived a start signal.\n"
#define P_RECEIVED_STOP_SIGNAL							"\nReceived a start signal.\n"
#define P_I2C_BUS_FREE											"\nI2C bus in free state.\n"
#define P_I2C_BUS_BUSY											"\nI2C bus in busy state.\n"
#define P_SLAVE_IN_RECEIVING								"\nIn receiving data.\n"
#define P_SLAVE_END_RESIVING								"\nEnd receiving data.\n"
#define P_SLAVE_IN_SENDING									"\nIn sending  data.\n"
#define P_SLAVE_END_SENDING									"\nEnd sending  data.\n"
#define P_SLAVE_ADDR_CPMPARE_SUCCESS				"\nAddress matching successful.\n"
#define P_SLAVE_ADDR_CPMPARE_FILED				  "\nAddress matching failed, exit.\n"

