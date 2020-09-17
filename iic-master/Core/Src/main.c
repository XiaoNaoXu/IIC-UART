
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"
#include <string.h>

void SystemClock_Config(void);

u8 UART_Rx_Buffer[UART_RX_BUFF_SIZE] = {0};
u8 Rx_Byte = 0;
u8 uart_rx_cnt = 0;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	u8 test[] = {0, 1, 2, 3, 4, 5};
  HAL_Init();																		//Init HAL
  SystemClock_Config();													//Config CLOCK
  MX_GPIO_Init();																//Init GREEN LED GPIO
  MX_USART2_UART_Init();												//Init UART2 GPIO
	I2C_GPIO_Init();															//Init I2C GPIO
	//I2C_Write(I2C_WRITE_ADDRESS, test, 6);        //test
	I2C_Read(I2C_READ_ADDRESS, test, 4);          //test
	/**      LOOP      **/
  HAL_UART_Receive_IT(&huart2, (uint8_t *)&Rx_Byte, 1);
	while (1)
  {
  }
	
	
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
	//HAL_UART_Transmit(huart,(u8 *)UART_Rx_Buffer, 5, 0xFFFF);
	if(uart_rx_cnt >= 255)  //????
	{
		uart_rx_cnt = 0;
		memset(UART_Rx_Buffer,0x00,sizeof(UART_Rx_Buffer));
		HAL_UART_Transmit(huart, (uint8_t *)"FFFFFF", 10,0xFFFF); 	
        
	}
	else
	{
		UART_Rx_Buffer[uart_rx_cnt++] = Rx_Byte;   
		if((UART_Rx_Buffer[0] == 0xE0)&&(UART_Rx_Buffer[uart_rx_cnt-1] == 0xE1)) 
		{
			HAL_UART_Transmit(huart, (uint8_t *)UART_Rx_Buffer, uart_rx_cnt,0xFFFF);
      while(HAL_UART_GetState(huart) == HAL_UART_STATE_BUSY_TX);
			UART_Process_Param(UART_Rx_Buffer+1, uart_rx_cnt - 2);
			uart_rx_cnt = 0;
			memset(UART_Rx_Buffer,0x00,sizeof(UART_Rx_Buffer)); 
		}
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

}
