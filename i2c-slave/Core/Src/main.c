/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  */


#include "main.h"




void SystemClock_Config(void);


char UART_Rx_Buffer[UART_RX_BUFF_SIZE] = {0};
u8 Rx_Byte = 0;
u8 uart_rx_cnt = 0;

u8 I2C_buff[DEFAULT_BUFF_SIZE] = {0};

void (* running)(void) = &master_start;


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  HAL_Init();																							//Init HAL

  SystemClock_Config();																		//Config System Clock
	
	MX_USART2_UART_Init();																	//Init UART2 GPIO
	
	//slave_start();
	
	change_state: running();
	
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
}




/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

}

/**
  * @brief  get end index of para.
  * @retval None
  */
u8 get_para_location(char *begin_address){
	__IO u8 i = 0;
	while(begin_address[i] != ' '){
		++i;
	}
	return i;
}

u8 UART_Process_Param(u8 *UART_buff, u8 uart_buff_length, u8 *I2C_buff, u8 I2C_buff_length){
//	u8 *buff_ptr = UART_buff;
//	__IO u8 buff_length = uart_buff_length, re_value = 0U;
//	if(buff_ptr[2] == 0xFF && buff_length != 7){
//		return param_error;
//	}
//	if((buff_ptr[2] == 0xF0 || buff_ptr[2] == 0x0F) && buff_length != 5){
//		return param_error;
//	}
//	if(buff_ptr[0] == Slave_Get){
//		I2C_Master_Read(I2C_READ_ADDRESS, I2C_buff, 5);
//	}
//	else if(buff_ptr[0] == Slave_Set){
//		I2C_Master_Write(buff_ptr[1], buff_ptr + 2, (buff_ptr[2] == 0xFF)?(5):(3));
//		delay_ms(1);
//		I2C_Master_Read(I2C_READ_ADDRESS, I2C_buff, 4);
//	}
//	else{
//	 return param_error;
//	}
	__IO u8 uart_buff_location = 0, command_buff_size = 0, command_buff_length = 0;
	
	char command[MAX_COMMAND_BUFF_SIZE][MAX_COMMAND_LENGH] = {'\0'};
	//memset(command, '\0', MAX_COMMAND_BUFF_SIZE*MAX_COMMAND_LENGH);
	while(uart_buff_location < (uart_rx_cnt - 2)){
		if(UART_Rx_Buffer[uart_buff_location] == ' '){
			command[command_buff_size][command_buff_length] = '\0';
			command_buff_size++;
			command_buff_length = 0;
		}
		else{
			command[command_buff_size][command_buff_length++] = UART_Rx_Buffer[uart_buff_location];
		}
		++uart_buff_location;
	}

	if(!strcmp(GET_RUNNING_STATE, command[0])){
		return 1;
	}
	else if(!strcmp(CHANGE_RUNNING_STATE, command[0])){
		return 1;
	}
	return 0u;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
	if(uart_rx_cnt >= 255)
	{
		uart_rx_cnt = 0;
		memset(UART_Rx_Buffer,0x00,sizeof(UART_Rx_Buffer));
		HAL_UART_Transmit(huart, (uint8_t *)"FFFFFF", 10,0xFFFF); 	
        
	}
	else
	{
		UART_Rx_Buffer[uart_rx_cnt++] = Rx_Byte;   
		if(UART_Rx_Buffer[uart_rx_cnt-1] == '\n')
		{
      while(HAL_UART_GetState(huart) == HAL_UART_STATE_BUSY_TX);
			UART_Process_Param(UART_Rx_Buffer+1, uart_rx_cnt - 2, I2C_buff, DEFAULT_BUFF_SIZE);
			HAL_UART_Transmit(huart, (char *)UART_Rx_Buffer, uart_rx_cnt,0xFFFF);
			uart_rx_cnt = 0;
			memset(UART_Rx_Buffer,0x00,sizeof(UART_Rx_Buffer));
		}
	}
}

