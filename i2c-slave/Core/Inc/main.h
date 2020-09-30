/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.

  ******************************************************************************
  */
/* USER CODE END Header */


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
#define GET_RUNNING_STATE 							    "get_state"          /*    Get running state         */
#define CHANGE_RUNNING_STATE 						    "change_state"       /*    Change running state      */
#define SET_LED_DURATION 								    "set_dura"					 /*    Set led duration          */
#define SET_LED_FREQUENCY 						      "set_freq"					 /*    Set led frequency         */
#define GET_LED_DURATION 								    "get_dura"					 /*    Get led duration          */
#define GET_LED_FREQUENCY 						      "get_freq"					 /*    Get led frequency         */
#define SET_LED_DURATION_FREQUENCY 				  "set_dura_freq"			 /*    Set led frequency         */
#define GET_LED_DURATION_FREQUENCY 					"get_dura_freq"			 /*    Get led duration          */


/*      UART return information                                                  */
#define RUNSTAT_SLAVE_TO_MASTER              "Running state from slave to master."
#define RUNSTAT_MASTER_TO_SLAVE              "Running state from master to slave."
#define RUNSTAT_SLAVE              					 "Running state is slave."
#define RUNSTAT_MASTER             					 "Running state is master."
#define COMMAND_ERR              						 "Command error: please check the command."
#define RUNSTAT_MASTER_TO_MASTER             "Can't change master to master of the running state."
#define RUNSTAT_SLAVE_TO_SLAVE               "Can't change slave to slave of the running state."
#define OK 																	 "OK."
#define MASTER_NO_LED 											 "Master no LED."
#define SLAVE_ERR_NO_READ 									 "Slave no permissions to read other device information."




/*      Change of running state 																					       */
u8 			Change_Running_state(u8);
u8 			Get_Running_state(u8);

/*      UART processing parameters																							 */
u8 			UART_Process_Param(UART_HandleTypeDef *);
u8 			get_para_location(char *);

void		Error_Handler(void);




#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


