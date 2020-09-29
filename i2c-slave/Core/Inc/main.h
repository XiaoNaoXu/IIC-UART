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


#define MAX_COMMAND_LENGH 								((u8)0x14)				 /*    Max command legth         */
#define MAX_COMMAND_BUFF_SIZE 						((u8)0x14)				 /*    Get command buff size     */


#define MASTER 											((uint8_t)0x00)  				 /*    Running state is master   */
#define SLAVE 											((uint8_t)0x01)  				 /*    Running state is slave    */


#define GET_RUNNING_STATE 							"get_state"          /*    Get running state         */
#define CHANGE_RUNNING_STATE 						"change_state"       /*    Change running state      */
#define SET_LED_DURATION 								"set_dura"					 /*    Set led duration          */
#define SET_LED_FREQUENCY 						  "set_dura"					 /*    Set led frequency         */
#define GET_LED_DURATION 								"get_dura"					 /*    Get led duration          */
#define GET_LED_FREQUENCY 						  "get_dura"					 /*    Get led frequency         */



/*     Record running state           */
typedef enum Running_State {
				master   =   MASTER,
	      slave    =   SLAVE
}Running_State;


/*       Change of running state       */
u8 Change_Running_state(u8);
u8 Get_Running_state(u8);

//UART processing parameters
u8 UART_Process_Param(u8 *, u8, u8 *, u8);
u8 get_para_location(char *);

void Error_Handler(void);




#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


