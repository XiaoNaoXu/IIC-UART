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


#include "slave.h"
#include "master.h"



/*      Change of running state 																					       */
I2C_TYPE 			Change_Running_state(I2C_TYPE);
I2C_TYPE 			Get_Running_state(I2C_TYPE);

/*      UART processing parameters																							 */

u32 		String_To_Hex_Of_Units(char);
u32 		String_To_Hex_Of_Data(char *, u32);

/*			MCU Initial																															*/
void		Error_Handler(void);
void 		SystemClock_Config(void);
void 		MX_DMA_Init(void);
I2C_TYPE 			UART_Process_Param(UART_HandleTypeDef *);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


