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
#include "main.h"
#include "stm32g0xx_hal_uart.h"
#include "stm32g0xx_hal_uart_ex.h"


/*			Define UART related constant																		*/
#define UART_TR_TIMEOUT								 			((u16)0xFFFF)
#define UART_RX_BUFF_SIZE 						 			((u8)0x10)
#define UART_RECEIVE_BYTE_NUMBER						((u8)0x01)


/*			Uart modle enable																								*/
#define HAL_UART_MODULE_ENABLED

extern  UART_HandleTypeDef huart2;
void 		MX_USART2_UART_Init(void);



#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */


