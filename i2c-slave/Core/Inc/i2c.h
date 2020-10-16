/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for i2c.c file.

  ******************************************************************************
  */

#ifndef I2C_H
#define I2C_H

#include <usart.h>
#include <stdlib.h>

#include <string.h>
//#include "stm32g0xx_hal.h"

typedef		 uint32_t    u32;
typedef		 uint16_t    u16;
typedef		 uint8_t     u8;


/**************************************************************************/
/***********                 Type Conversion          ************************/
/**************************************************************************/

#define I2C_TYPE										u8

u32 String_To_Hex_Of_Units(char);
u32 String_To_Hex_Of_Data(char *, u32);
I2C_TYPE  I2C_To_UART(I2C_TYPE *);




/**************************************************************************/
/***********                 State Control         ************************/
/**************************************************************************/

#define MASTER 											((I2C_TYPE)0x00)  				 /*    Running state is master   */
#define SLAVE 											((I2C_TYPE)0x01)  				 /*    Running state is slave    */
#define PROCESS_SUCCESS             ((I2C_TYPE)0x00)					 /*    return a success          */


/*     Record running state           */
typedef enum Running_State {
				master   =   MASTER,
	      slave    =   SLAVE
}Running_State;

extern Running_State 		running_state;
extern void 						(* running)(void);
extern void		 					master_start(void);
extern void		 					slave_start(void);
extern I2C_TYPE 				I2C_Bus_state;






/**************************************************************************/
/***********                    GPIO               ************************/
/**************************************************************************/

/*      I2C port select																		  */
#define I2C_PORT_GPIO 						  				GPIOC                		 // I2C IOPORT
#define I2C_SDA_PORT						            I2C_PORT_GPIO       	   // I2C SDA IOPORT
#define I2C_SCL_PORT							          I2C_PORT_GPIO         	 // I2C SCL IOPORT
#define I2C_SDA_PIN 								        GPIO_PIN_4            	 // I2C SDA GPIO
#define I2C_SCL_PIN 								        GPIO_PIN_5            	 // I2C SCL GPIO


/*      EXTI GPIO Select                                    */
#define MASTER_EXTI_PORT										GPIOC										// Master EXTI IOPORT
#define SLAVE_EXTI_PORT							  			GPIOC										// Slave EXTI IOPORT
#define MASTER_EXTI_PIN											GPIO_PIN_4							// Master EXTI GPIO
#define SLAVE_EXTI_PIN											GPIO_PIN_5							// Slave EXTI GPIO


/*      I2C Level High / Low                                */
#define I2C_LEVEL_HIGH							((I2C_TYPE)0x01)
#define I2C_LEVEL_LOW 							((I2C_TYPE)0x00)

/*      SET SDA LEVEL																			  */
#define I2C_SDA_1() 			I2C_SDA_PORT->BSRR = I2C_SDA_PIN							// Set SDA = 1
#define I2C_SDA_0() 			I2C_SDA_PORT->BRR  = I2C_SDA_PIN								// Set SDA = 0


/*			SET SCL LEVEL																				*/
#define I2C_SCL_1() 				{I2C_PORT_GPIO->BSRR = I2C_SCL_PIN; while(I2C_SCL_READ() != I2C_LEVEL_HIGH);}							// Set SCL = 1 
#define I2C_SCL_0() 				I2C_PORT_GPIO->BRR = I2C_SCL_PIN  																												// Set SCL = 0 


//#define I2C_SDA_READ() 			((I2C_SDA_PORT->IDR & I2C_SDA_PIN) == I2C_SDA_PIN)											// Read SDA status
//#define I2C_SCL_READ() 			((I2C_SCL_PORT->IDR & I2C_SCL_PIN) == I2C_SCL_PIN)											// Read SCL status
#define I2C_SDA_READ() 				HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN)															// Read SDA level
#define I2C_SCL_READ() 				HAL_GPIO_ReadPin(I2C_SCL_PORT, I2C_SCL_PIN)															// Read SCL level
#define MASTER_EXTI_PIN_READ	HAL_GPIO_ReadPin(MASTER_EXTI_PORT, MASTER_EXTI_PIN)											// Read Mater EXTI Pin level


/*      LED Control                                  			  */
#define LED_ON HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET)        						// LED4 UP, PA5 = high level
#define LED_OFF HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET)     						// LED4 DOWN, PA5 = low level











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


/*      Define the rate and unit of time of I2C        */
#define I2C_S_TO_US 				  				  ((u32)1000000)     // second = microsecond * 1000000
#define I2C_MS_TO_US 								    ((u32)1000)		     // millisecond = microsecond * 1000



/*			  LED port select 								            */
#define LED_GREEN_Pin 				  			     GPIO_PIN_5         // LED GPIO
#define LED_GREEN_GPIO_Port                GPIOA              // LED IOPORT


/* 			I2C address																	*/
#define I2C_ADDRESS                     ((I2C_TYPE)0xB0)          // As a slave address
#define I2C_ADDRESS_LEN                 ((I2C_TYPE)0x02)					// A address length


/*		  light on and light off frequency , duration of the Green LED */
#define LED_FREQUENCY							    ((I2C_TYPE)0x02)        // LED frequency command
#define LED_DURATION 							    ((I2C_TYPE)0x01)        // LED duration command
#define LED_DURATION_FREQUENCY				((I2C_TYPE)0x03)        // LED frequency and duration command
#define RUNNING_STATE									((I2C_TYPE)0x04)				// running state command


/*			Unit of time   -----    s,  ms, us     											 */
typedef enum Option{
					read   =    ((I2C_TYPE)0x0000),             /*          Receive data state          */
					write,														 	  /*          Write data  state           */
					ret,																  /*          Initialization state        */
}Option;


/*				The time delay function  																   */
void 			delay_us(u32);
void 			delay_ms(u32);


/*			 Start and stop signal																				*/
I2C_TYPE 			 is_I2C_Slave_Start(void);
I2C_TYPE 			 is_I2C_Slave_Stop(void);


/*			 Acknowledge																							    */
#define I2C_Slave_SendAck()												I2C_SDA_0()
#define I2C_Slave_SendNAck()											I2C_SDA_1()
I2C_TYPE 			 I2C_Slave_WaitAck(void);




/**************************************************************************/
/***********                    MASTER              ***********************/
/**************************************************************************/

/*                   I2C Bus Status                    				 			  */
#define I2C_BUS_IDLE 															((I2C_TYPE)0x00)
#define I2C_BUS_BUSY															((I2C_TYPE)0xFF)


/*		  Command Set or Get  												 	 				        */
#define Slave_Set            			((I2C_TYPE)0xFF)
#define Slave_Get 								((I2C_TYPE)0x00)

/*			Slave write and read address: write(0), read(1)								*/
#define I2C_WRITE 									((I2C_TYPE)0x00)
#define I2C_READ 										((I2C_TYPE)0x01)



/*			 Send and receive data																	  		*/
I2C_TYPE	 	   I2C_Master_SendByte(I2C_TYPE);
u32		   I2C_Master_Write(I2C_TYPE, I2C_TYPE *);
u32		   I2C_Master_Read(I2C_TYPE, I2C_TYPE *);


/*			 Acknowledge																									*/
void 		 I2C_Master_SendAck(void);
void 		 I2C_Master_SendNAck(void);


//start and stop signal
void		 I2C_Master_Start(void);
void		 I2C_Master_Stop(void);








/**************************************************************************/
/***********           BOTH MASTER AND SLAVE        ***********************/
/**************************************************************************/

#define SEND_HIGHEST_BIT(BYTE)							(BYTE & 0x80) ? (I2C_SDA_1()) : (I2C_SDA_0())

/*			  I2C RISING ENABLE AND DISABLE ADDR					*/
#define I2C_SCL_EXTI_ENABLE_ADDR 	         	((u16)0x0020)       // SCL EXTI enable register
#define I2C_SCL_EXTI_DISABLE_ADDR           ~((u16)0x0020)     	// SCL EXTI disable register
#define I2C_SDA_EXTI_ENABLE_ADDR 	         	((u16)0x0010)       // SCL EXTI enable register
#define I2C_SDA_EXTI_DISABLE_ADDR           ~((u16)0x0010)     	// SCL EXTI disable register



/*			SCL EXTI ENABLE/DISABLE																				 */
#define I2C_SCL_Rising_Disable()										(EXTI->RTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR)
#define I2C_SCL_Rising_Enable()											(EXTI->RTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR)
#define I2C_SCL_Falling_Disable()										(EXTI->FTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR)
#define I2C_SCL_Falling_Enable()										(EXTI->FTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR)
#define I2C_SCL_FallingDisable_RisingEnable()				{(EXTI->FTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR), (EXTI->RTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR);}
#define I2C_SCL_FallingEnable_RisingDisable()				{(EXTI->FTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR), (EXTI->RTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR);}
#define I2C_SCL_Falling_Rising_Enable()							{(EXTI->RTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR), (EXTI->FTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR);}
#define I2C_SCL_Falling_Rising_Disable()						{(EXTI->RTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR), (EXTI->FTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR);}


/*			SDA EXTI ENABLE/DISABLE																				 */
#define I2C_SDA_Rising_Disable()					(EXTI->RTSR1 &= I2C_SDA_EXTI_DISABLE_ADDR)
#define I2C_SDA_Rising_Enable()						(EXTI->RTSR1 |= I2C_SDA_EXTI_ENABLE_ADDR)
#define I2C_SDA_Falling_Disable()					(EXTI->FTSR1 &= I2C_SDA_EXTI_DISABLE_ADDR)
#define I2C_SDA_Falling_Enable()					(EXTI->FTSR1 |= I2C_SDA_EXTI_ENABLE_ADDR)
#define I2C_SDA_Falling_Rising_Enable()		{(EXTI->RTSR1 |= I2C_SDA_EXTI_ENABLE_ADDR), (EXTI->FTSR1 |= I2C_SDA_EXTI_ENABLE_ADDR);}
#define I2C_SDA_Falling_Rising_Disable()	{(EXTI->RTSR1 &= I2C_SDA_EXTI_DISABLE_ADDR), (EXTI->FTSR1 &= I2C_SDA_EXTI_DISABLE_ADDR);}


/*      Define the rate and unit of time of I2C        */
#define I2C_PD                          ((I2C_TYPE)0x05)	  	 // A pulse width
#define I2C_S 				    						  ((I2C_TYPE)0x73)       // A second
#define I2C_MS 				    							((I2C_TYPE)0x6D)       // A millisecond
#define I2C_US 													((I2C_TYPE)0x75)       // A microsecond


/*******Define length and size                     						   				*/
#define BIT_LENGTH 					    		    ((I2C_TYPE)0x08)       // A bit length
#define DEFAULT_BUFF_SIZE               ((I2C_TYPE)0x0F)	  	 // Receive/send buff length
#define I2C_PARA_LENGTH 			    		  ((I2C_TYPE)0x05)	  	 // The number of arguments


/*      Define address and offet                      									*/
#define START_ADDR 					    	      ((I2C_TYPE)0x00)	     // Receive/send buff start index
#define BASE_ADDR 					    	      ((I2C_TYPE)0x01)	     // Receive/send buff base index
#define TIME_OFFSET     	    		      ((I2C_TYPE)0x01)	  	 // Data byte offset
#define UNITS_OFFSET                    ((I2C_TYPE)0x02)	  	 // Units byte offset
#define ADDR_OFFSET 				    			  ((I2C_TYPE)0x02)	  	 // Frequency address offset
#define STATE_OFFSET 				    		    ((I2C_TYPE)0x01)	  	 // Command address offset
#define RUNNING_STATE_OFFSET 				    ((I2C_TYPE)0x02)	  	 // State address offset


#define UART_I2C_OK												((I2C_TYPE)0x01)
#define NO_I2C_DATA												((I2C_TYPE)0x00)


#define I2C_GPIO_EXTI_Rising_Callback(GPIO_Pin)   

#define I2C_GPIO_EXTI_Falling_Callback(GPIO_Pin)

#define I2C_GPIO_EXTI_IRQHandler(GPIO_Pin)	 {if (__HAL_GPIO_EXTI_GET_RISING_IT(GPIO_Pin) != 0x00u){\
																								 __HAL_GPIO_EXTI_CLEAR_RISING_IT(GPIO_Pin);\
																								 HAL_GPIO_EXTI_Rising_Callback(GPIO_Pin);}\
																						 else if (__HAL_GPIO_EXTI_GET_FALLING_IT(GPIO_Pin) != 0x00u){\
																								 __HAL_GPIO_EXTI_CLEAR_FALLING_IT(GPIO_Pin);\
																								 HAL_GPIO_EXTI_Falling_Callback(GPIO_Pin);}\
																						 }


extern I2C_TYPE  		I2C_buff[DEFAULT_BUFF_SIZE];
extern I2C_TYPE 		I2C_receive_buff[DEFAULT_BUFF_SIZE];


/*			 I2C GPIO Init 																									*/
void 		 I2C_GPIO_Init(void);





/**************************************************************************/
/***********                UART PRINT              ***********************/
/**************************************************************************/


#define P_STATE_MASTER_TO_SLAVE							"\nState changed from master to slave.\n"
#define P_STATE_SLAVE_TO_MASTER							"\nState changed from slave to master.\n"
#define P_RECEIVED_START_SIGNAL							"\nReceived a start signal.\n"
#define P_RECEIVED_STOP_SIGNAL							"\nReceived a start signal.\n"
#define P_I2C_BUS_IDLE											"\nI2C bus in free state.\n"
#define P_I2C_BUS_BUSY											"\nI2C bus in busy state.\n"
#define P_SLAVE_IN_RECEIVING								"\nIn receiving data.\n"
#define P_SLAVE_END_RESIVING								"\nEnd receiving data.\n"
#define P_SLAVE_IN_SENDING									"\nIn sending  data.\n"
#define P_SLAVE_END_SENDING									"\nEnd sending  data.\n"
#define P_SLAVE_ADDR_CPMPARE_SUCCESS				"\nAddress matching successful.\n"
#define P_SLAVE_ADDR_CPMPARE_FILED				  "\nAddress matching failed, exit.\n"


extern I2C_TYPE 									UART_Process_Param(UART_HandleTypeDef *);
extern UART_HandleTypeDef 				huart2;
extern I2C_TYPE 		 						  UART_DATA_REG;
extern I2C_TYPE										uart_rx_cnt;
extern char 											UART_Rx_Buffer[UART_RX_BUFF_SIZE];






#endif

