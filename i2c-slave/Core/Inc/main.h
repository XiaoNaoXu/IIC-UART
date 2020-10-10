/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.

  ******************************************************************************
  */


#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32g0xx_hal.h"
#include "gpio.h"
#include "slave.h"
#include "master.h"
#include "usart.h"
#include <string.h>


/*      UART return information                                                  */
#define MAX_COMMAND_LENGH 								((u8)0x14)				 /*    Max command legth         */
#define MAX_COMMAND_BUFF_SIZE 						((u8)0x14)				 /*    Get command buff size     */


/*      UART command                                                            */
#define RUNNING_STATE 						   			  "state"      				 /*    Running state      								*/
#define GET 								 						    "get"								 /*    Get                    				    */
#define SET 						  							    "set"								 /*    Set     					      				    */
#define LED_DURA 								   			 		"dura"							 /*    Led duration         				      */
#define LED_FREQ 						    			  		"freq"							 /*    Led frequency        				      */
#define LED_DURA_FREQ  								  		"dura_freq"			     /*    Led frequency and  frequency       */


/*      UART return information                                                  */
#define RUNSTAT_SLAVE_TO_MASTER              "Running state from slave to master.\n"
#define RUNSTAT_MASTER_TO_SLAVE              "Running state from master to slave.\n"
#define RUNSTAT_SLAVE              					 "Running state is slave.\n"
#define RUNSTAT_MASTER             					 "Running state is master.\n"
#define COMMAND_ERR              						 "Command error: please check the command.\n"
#define RUNSTAT_MASTER_TO_MASTER             "Can't change master to master of the running state.\n"
#define RUNSTAT_SLAVE_TO_SLAVE               "Can't change slave to slave of the running state.\n"
#define OK 																	 "Success.\n"
#define FAILED 															 "Failed.\n"
#define MASTER_NO_LED 											 "Master no LED.\n"
#define SLAVE_ERR_NO_READ 									 "Slave no permissions to read other device information.\n"
#define BUS_BUSY                             "The bus is in a busy state.\n"
#define PARA_LENGTH_ERR											 "Parameter more than 255 bytes.\n"



/*      Change of running state 																					       */
u8 			Change_Running_state(u8);
u8 			Get_Running_state(u8);

/*      UART processing parameters																							 */
u8 			UART_Process_Param(UART_HandleTypeDef *);
u32 		String_To_Hex_Of_Units(char);
u32 		String_To_Hex_Of_Data(char *, u32);

/*			MCU Initial																															*/
void		Error_Handler(void);
void 		SystemClock_Config(void);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


