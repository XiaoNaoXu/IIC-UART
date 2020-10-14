/**
  ******************************************************************************
  * @file           : master.c
  * @brief          : Master related code
  */

#include "master.h"


u8 							I2C_receive_buff[DEFAULT_BUFF_SIZE] = {0};



/**
  * @brief  Master entry point.
  * @retval None
  */
void master_start(){
	
	I2C_Master_SCL_Output_OD_Init();
	
	I2C_Master_SDA__OutputOD_Rising_Falling_Init();
	
	I2C_Master_SendByte(I2C_ADDRESS);
	
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
		I2C_Master_SDA_Rising_Enable();
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
		I2C_Master_SDA_Rising_Disable();
	}
}
