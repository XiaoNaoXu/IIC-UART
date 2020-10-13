/**
  ******************************************************************************
  * @file           : master.c
  * @brief          : Master related code
  */

#include "master.h"


u8 							I2C_receive_buff[DEFAULT_BUFF_SIZE] = {0};
extern u8 			I2C_Bus_state;


/**
  * @brief  Master entry point.
  * @retval None
  */
void master_start(){
	
	I2C_Master_SCL_Output_OD_Init();
	
	I2C_Master_SDA_Rising_Falling_Init();
	
	while(running_state == MASTER);
	
}

/**
  * @brief  This function is master's falling exti callback.
  * @retval None
  */
void Master_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	if(I2C_SCL_READ()){
		I2C_Bus_state = I2C_BUS_BUSY;
		//HAL_UART_Transmit(&huart2, (u8 *)P_RECEIVED_START_SIGNAL, strlen(P_RECEIVED_START_SIGNAL),UART_TR_TIMEOUT);
		//HAL_UART_Transmit(&huart2, (u8 *)P_I2C_BUS_BUSY, strlen(P_I2C_BUS_BUSY),UART_TR_TIMEOUT);
	}
}


/**
  * @brief  This function is master's rising exti callback.
  * @retval None
  */
void Master_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	if(I2C_SCL_READ()){
		I2C_Bus_state = I2C_BUS_FREE;
		//HAL_UART_Transmit(&huart2, (u8 *)P_RECEIVED_STOP_SIGNAL, strlen(P_RECEIVED_STOP_SIGNAL),UART_TR_TIMEOUT);
		//HAL_UART_Transmit(&huart2, (u8 *)P_I2C_BUS_FREE, strlen(P_I2C_BUS_FREE),UART_TR_TIMEOUT);
	}
}
