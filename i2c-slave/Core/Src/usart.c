/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  */

#include "usart.h"



UART_HandleTypeDef 		huart2;
DMA_HandleTypeDef 		dma_uart2_rx;
DMA_HandleTypeDef 		dma_uart2_tx;



/* USART2 init function */
void USART2_UART_Init(void)
{
	/* 		USART2 init  																								*/
  huart2.Instance 										= USART2;
  huart2.Init.BaudRate 								= 115200;
  huart2.Init.WordLength 							= UART_WORDLENGTH_8B;
  huart2.Init.StopBits 								= UART_STOPBITS_1;
  huart2.Init.Parity 									= UART_PARITY_NONE;
  huart2.Init.Mode 										= UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl								= UART_HWCONTROL_NONE;
  huart2.Init.OverSampling 						= UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling 					= UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler 					= UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit 	= UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
	
	HAL_UART_Receive_DMA(&huart2, (uint8_t *)UART_Rx_Buffer, 0xFF);
	HAL_NVIC_SetPriority(USART2_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);

}



/**USART2 GPIO Configuration
PA2     ------> USART2_TX
PA3     ------> USART2_RX
*/
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
		
   
    __HAL_RCC_USART2_CLK_ENABLE();												 /* USART2 clock enable */
	
    __HAL_RCC_GPIOA_CLK_ENABLE();													 /* GPIOC clock enable */
    
    GPIO_InitStruct.Pin 				= GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode 				= GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull 				= GPIO_NOPULL;
    GPIO_InitStruct.Speed 			= GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate 	= GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		
		/*		DMA channel1 init and link with UART2 rx		*/
		dma_uart2_rx.Instance 									= DMA1_Channel1;
		dma_uart2_rx.Init.Request 							= DMA_REQUEST_USART2_RX;
		dma_uart2_rx.Init.Direction 						= DMA_PERIPH_TO_MEMORY;
		dma_uart2_rx.Init.MemInc 								= DMA_MINC_ENABLE;
		dma_uart2_rx.Init.PeriphInc 						= DMA_PINC_DISABLE;
		dma_uart2_rx.Init.PeriphDataAlignment 	= DMA_PDATAALIGN_BYTE;
		dma_uart2_rx.Init.MemDataAlignment 			= DMA_MDATAALIGN_BYTE;
		dma_uart2_rx.Init.Mode 									= DMA_NORMAL;
		dma_uart2_rx.Init.Priority							= DMA_PRIORITY_LOW;
		HAL_DMA_Init(&dma_uart2_rx);
		__HAL_LINKDMA(uartHandle, hdmarx, dma_uart2_rx);
		
		
		/*		DMA channel2 init and link with UART2 tx		*/
		dma_uart2_tx.Instance 									= DMA1_Channel2;
		dma_uart2_tx.Init.Direction 						= DMA_MEMORY_TO_PERIPH;
		dma_uart2_tx.Init.Request 							= DMA_REQUEST_USART2_TX;
		dma_uart2_tx.Init.PeriphInc 						= DMA_PINC_DISABLE;
		dma_uart2_tx.Init.MemInc 								= DMA_MINC_ENABLE;
		dma_uart2_tx.Init.PeriphDataAlignment 	= DMA_PDATAALIGN_BYTE;
		dma_uart2_tx.Init.MemDataAlignment 			= DMA_MDATAALIGN_BYTE;
		dma_uart2_tx.Init.Mode 									= DMA_NORMAL;
		dma_uart2_tx.Init.Priority = DMA_PRIORITY_LOW;
		HAL_DMA_Init(&dma_uart2_tx);
		__HAL_LINKDMA(uartHandle, hdmatx, dma_uart2_tx);
  }
}



/**USART2 GPIO Configuration
PA2     ------> USART2_TX
PA3     ------> USART2_RX
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
    __HAL_RCC_USART2_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
		
		HAL_NVIC_DisableIRQ(USART2_IRQn);
		
		HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

  }
}



