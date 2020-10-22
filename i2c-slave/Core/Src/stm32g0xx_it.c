/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"

extern UART_HandleTypeDef huart2;
extern uint8_t Rx_Byte;
extern u8 UART_DATA_REG;
extern Running_State running_state;
extern DMA_HandleTypeDef dma_uart2_rx;
extern DMA_HandleTypeDef dma_uart2_tx;

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_uart2_rx);
}

/**
  * @brief This function handles DMA1 channel 2 and channel 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_uart2_tx);
}


/* USER CODE BEGIN 1 */
void EXTI4_15_IRQHandler(void)
{
	
	if (__HAL_GPIO_EXTI_GET_RISING_IT(I2C_SCL_PIN) != 0x00u){
		__HAL_GPIO_EXTI_CLEAR_RISING_IT(I2C_SCL_PIN);
		
		if(running_state == MASTER){
			Master_EXTI_Rising_Callback();
		}
		else{
			Slave_SCL_EXTI_Rising_Callback();
		}
	}
	else if (__HAL_GPIO_EXTI_GET_FALLING_IT(I2C_SCL_PIN) != 0x00u){
		__HAL_GPIO_EXTI_CLEAR_FALLING_IT(I2C_SCL_PIN);
		
		if(running_state == MASTER){
			Master_EXTI_Falling_Callback();
		}
		else{
			Slave_SCL_EXTI_Falling_Callback();
		}
	}
	
	
	else if (__HAL_GPIO_EXTI_GET_RISING_IT(I2C_SDA_PIN) != 0x00u){
		__HAL_GPIO_EXTI_CLEAR_RISING_IT(I2C_SDA_PIN);
		
		if(running_state == MASTER){
			Master_EXTI_Rising_Callback();
		}
		else{
			Slave_SDA_EXTI_Rising_Callback();
		}
	}
	else if (__HAL_GPIO_EXTI_GET_FALLING_IT(I2C_SDA_PIN) != 0x00u){
		__HAL_GPIO_EXTI_CLEAR_FALLING_IT(I2C_SDA_PIN);
		
		if(running_state == MASTER){
			Master_EXTI_Falling_Callback();
		}
		else{
			Slave_SDA_EXTI_Falling_Callback();
		}
	}
	
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

void USART2_IRQHandler(void)
{
  if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET){
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_DMAStop(&huart2);
		uart_rx_cnt = UART_RX_BUFF_SIZE - __HAL_DMA_GET_COUNTER(huart2.hdmarx);
		if(uart_rx_cnt > 0U){
			UART_DATA_REG = UART_DATA_OK;
		}
	}
	HAL_UART_IRQHandler(&huart2);
	HAL_UART_Receive_DMA(&huart2, (u8 *)UART_Rx_Buffer, UART_RX_BUFF_SIZE);
	//HAL_UART_Receive_IT(&huart2, (uint8_t *)&Rx_Byte, UART_RECEIVE_BYTE_NUMBER);
  
}

/**
	*	@biref This function handles TIM3
	*
	*/
void TIM3_IRQHandler(void){
	if(I2C_SCL_READ()){
		I2C_Bus_state = I2C_BUS_IDLE;
		HAL_TIM_Base_Stop_IT(&tim3);
	}
}

