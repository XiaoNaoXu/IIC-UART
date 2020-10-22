/**
  ******************************************************************************
  * @file           : master.c
  * @brief          : Master related code
  */

#include "master.h"



/**
  * @brief  Master entry point.
  * @retval None
  */
void master_start(){
	
	//I2C_Master_SendByte(I2C_ADDRESS);
	I2C_SCL_Falling_Enable();
	while(running_state == MASTER){
		if(UART_DATA_REG == UART_DATA_OK){
			UART_Process_Param(&huart2);
		}
	}
	
}

/**
  * @brief  This function is master's falling exti callback.
  * @retval None
  */
void Master_EXTI_Falling_Callback()
{
	if(TIM3->DIER){
		TIM3->CNT = I2C_BUS_TIMEOUT;
	}
	else{
		I2C_Bus_state = I2C_BUS_BUSY;
		HAL_UART_Transmit_DMA(&huart2, (I2C_TYPE *)BUS_BUSY, strlen(BUS_BUSY));
		HAL_TIM_Base_Start_IT(&tim3);
	}
}


/**
  * @brief  This function is master's rising exti callback.
  * @retval None
  */
void Master_EXTI_Rising_Callback()
{
	if(I2C_SCL_READ()){
		I2C_Bus_state = I2C_BUS_IDLE;
		I2C_SCL_Rising_Disable();
	}
}
