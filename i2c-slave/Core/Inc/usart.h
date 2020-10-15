/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  */

#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/*      Include                                                         */
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_uart.h"
#include "stm32g0xx_hal_uart_ex.h"

/*      UART return information                                                  */
#define MAX_COMMAND_LENGH 								((u8)0x14)				 /*    Max command legth         */
#define MAX_COMMAND_BUFF_SIZE 						((u8)0x14)				 /*    Get command buff size     */


/*      UART command                                                            */
#define COMMAND_RUNNING_STATE 						   			  "state"      				 /*    Running state      								*/
#define COMMAND_GET 								 				"get"								 /*    Get                    				    */
#define COMMAND_SET 						  					"set"								 /*    Set     					      				    */
#define LED_DURA 								   			 		"dura"							 /*    Led duration         				      */
#define LED_FREQ 						    			  		"freq"							 /*    Led frequency        				      */
#define LED_DURA_FREQ  								  		"dura_freq"			     /*    Led frequency and  frequency       */
#define UART_GET_LED_INFO_PARA_NUM								((u8)0x03)
#define UART_SET_LED_INFO_PARA_NUM								((u8)0x05)
#define UART_GET_RUNSTATE_PARA_NUM								((u8)0x01)
#define UART_SET_RUNSTATE_PARA_NUM								((u8)0x03)


/*      UART return information                                                  */
#define RUNSTAT_SLAVE_TO_MASTER              "\nRunning state from slave to master.\n"
#define RUNSTAT_MASTER_TO_SLAVE              "\nRunning state from master to slave.\n"
#define RUNSTAT_SLAVE              					 "\nRunning state is slave.\n"
#define RUNSTAT_MASTER             					 "\nRunning state is master.\n"
#define COMMAND_ERR              						 "\nCommand error: please check the command.\n"
#define RUNSTAT_MASTER_TO_MASTER             "\nCan't change master to master of the running state.\n"
#define RUNSTAT_SLAVE_TO_SLAVE               "\nCan't change slave to slave of the running state.\n"
#define OK 																	 "\nSuccess.\n"
#define FAILED 															 "\nFailed.\n"
#define MASTER_NO_LED 											 "\nMaster no LED.\n"
#define SLAVE_ERR_NO_READ 									 "\nSlave no permissions to read other device information.\n"
#define BUS_BUSY                             "\nThe bus is in a busy state.\n"
#define PARA_LENGTH_ERR											 "\nParameter more than 255 bytes.\n"
#define DURA 																 "\nDuration: "
#define FREQ 																 "\nFrequency: "




/*			Define UART related constant																		*/
#define UART_TR_TIMEOUT								 			((u16)0xFFFF)
#define UART_RX_BUFF_SIZE 						 			((u8)0xFF)
#define UART_RECEIVE_BYTE_NUMBER						((u8)0x01)


#define UART_DATA_OK												((u8)0x01)
#define NO_UART_DATA												((u8)0x00)

/*			Uart modle enable																								*/
#define HAL_UART_MODULE_ENABLED

void 		MX_USART2_UART_Init(void);
extern void		Error_Handler(void);


#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */


