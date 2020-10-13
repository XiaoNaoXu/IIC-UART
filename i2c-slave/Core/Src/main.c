/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  */


#include "main.h"

u8 Rx_Byte = 0;
u8 uart_rx_cnt = 0;
char UART_Rx_Buffer[UART_RX_BUFF_SIZE] = {0};


extern u8 I2C_buff[DEFAULT_BUFF_SIZE];
extern u8 I2C_receive_buff[DEFAULT_BUFF_SIZE];



//Running_State running_state = master;
//void (* running)(void) = &master_start;

Running_State running_state = slave;
void (* running)(void) = &slave_start;


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  HAL_Init();																							//Init HAL

  SystemClock_Config();																		//Config System Clock
	
	MX_USART2_UART_Init();																	//Init UART2 GPIO
	
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&Rx_Byte, UART_RECEIVE_BYTE_NUMBER);
	
	while(1){
		running();
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


u8 I2C_To_UART(u8 *I2C_ptr){
	__IO u8 i = 0, temp, rx = 0, div = '1';
	__IO u8 data_len = I2C_ptr[START_ADDR], reg = I2C_ptr[BASE_ADDR];

	memset(UART_Rx_Buffer,0x00,sizeof(UART_Rx_Buffer));

	if((reg & LED_DURATION) == LED_DURATION || (reg & LED_FREQUENCY) == LED_FREQUENCY){
		for(i = 1; i < data_len; ++i){
			if(i%2 == 0){
				UART_Rx_Buffer[rx++] = I2C_ptr[BASE_ADDR + i];
				UART_Rx_Buffer[rx++] = '\n';
			}
			else{
				if((reg & LED_DURATION) == LED_DURATION){
					memcpy(UART_Rx_Buffer, DURA, strlen(DURA));
					rx += strlen(DURA);
					reg &= ~LED_DURATION;
				}
				else if((reg & LED_FREQUENCY) == LED_FREQUENCY){
					memcpy(UART_Rx_Buffer + rx, FREQ, strlen(FREQ));
					rx += strlen(FREQ);
					reg &= ~LED_FREQUENCY;
				}
				temp = I2C_ptr[BASE_ADDR + i];
				div = 100;
				while(temp){
					if(temp/div > 0){
						UART_Rx_Buffer[rx++] = temp / div + 0x30;
					}
					temp %= div;
					div /= 10;
				}
			}
		}
	}
	else if(reg == RUNNING_STATE){
		if(I2C_ptr[RUNNING_STATE_OFFSET] == MASTER){
			rx += strlen(RUNSTAT_MASTER);
			memcpy(UART_Rx_Buffer, RUNSTAT_MASTER, strlen(RUNSTAT_MASTER));
		}
		else if(I2C_ptr[RUNNING_STATE_OFFSET] == SLAVE){
			rx += strlen(RUNSTAT_SLAVE);
			memcpy(UART_Rx_Buffer, RUNSTAT_SLAVE, strlen(RUNSTAT_SLAVE));
		}
	}
	return rx;
}

/**
  * @brief  This function is transition string to hex of the date.
	*					Transiton slave addrss.
	* @retval uint32_t result: The slave address of an integer
  */
u32 String_To_Hex_Of_Data(char *string, u32 len){
	__IO u32 i = 0, result = 0;
	for(i = 0; i < len; ++i){
		result <<= 4U;
		if(string[i] >= '0' && string[i] <= '9'){
			result += string[i] - 0x30 + 0x00;
		}
		else if(string[i] >= 'A' && string[i] <= 'F'){
			result += string[i] - 0x41 + 0x0A;
		}
		else if(string[i] >= 'a' && string[i] <= 'f'){
			result += string[i] - 0x61 + 0x0A;
		}
	}
	return result;
}


/**
  * @brief  This function is transition string to hex of the time.
	* @retval u32 I2C_S: It's meaning second(s)
	* @retval u32 I2C_MS: It's meaning ms
	* @retval u32 I2C_US: It's meaning us
  */
u32 String_To_Hex_Of_Units(char string){
	if(string == 's' || string == 'S'){
		return I2C_S;
	}
	else if(string == 'm' || string == 'M'){
		return I2C_MS;
	}
	else if(string == 'u' || string == 'U'){
		return I2C_US;
	}
	return I2C_S;
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
u8 UART_Process_Param(UART_HandleTypeDef *huart){
	
	__IO u8 uart_buff_location = 0, command_buff_size = 0, command_buff_length = 0;
	
	/*          Process parameter from UART                                    */
	char command[MAX_COMMAND_BUFF_SIZE][MAX_COMMAND_LENGH] = {'\0'};
	memset(I2C_receive_buff, 0U, DEFAULT_BUFF_SIZE);
	memset(command, '\0', MAX_COMMAND_BUFF_SIZE * MAX_COMMAND_LENGH);
		
	while(uart_buff_location < uart_rx_cnt){
		if(UART_Rx_Buffer[uart_buff_location] == ' '){
			while((UART_Rx_Buffer[uart_buff_location + 1] == ' ') & (uart_buff_location < uart_rx_cnt )){
				++uart_buff_location;
			}
			if(command_buff_length != 0){
				command[command_buff_size][command_buff_length] = '\0';
				command_buff_size++;
				command_buff_length = 0;
			}
		}
		else if(UART_Rx_Buffer[uart_buff_location] == '\r' || UART_Rx_Buffer[uart_buff_location] == '\n'){
			break;
		}
		else{
			command[command_buff_size][command_buff_length++] = UART_Rx_Buffer[uart_buff_location];
		}
		++uart_buff_location;
	}
	
	if(command_buff_size < UART_GET_RUNSTATE_PARA_NUM){
		HAL_UART_Transmit(huart, (u8 *)COMMAND_ERR, strlen(COMMAND_ERR), UART_TR_TIMEOUT);
		return PROCESS_SUCCESS;
	}
	
	/*            Get info option                                                 */
	if(!strcmp(COMMAND_GET, command[0])){
		
		/*          Get running state                                              */
		if(!strcmp(COMMAND_RUNNING_STATE, command[1])){
			if(running_state == MASTER){
				HAL_UART_Transmit(huart, (u8 *)RUNSTAT_MASTER, strlen(RUNSTAT_MASTER),UART_TR_TIMEOUT);
			}
			else if(running_state == SLAVE){
				HAL_UART_Transmit(huart, (u8 *)RUNSTAT_SLAVE, strlen(RUNSTAT_SLAVE),UART_TR_TIMEOUT);
			}
		}
		
		/*           Get slave's light duration or frequency of led                  */
		else if(!strcmp(LED_DURA, command[1]) || !strcmp(LED_FREQ, command[1]) || !strcmp(LED_DURA_FREQ, command[1])){
			if(command_buff_size < UART_GET_LED_INFO_PARA_NUM){
				HAL_UART_Transmit(huart, (u8 *)COMMAND_ERR, strlen(COMMAND_ERR), UART_TR_TIMEOUT);
				return PROCESS_SUCCESS;
			}
			
			/*         This address is is self       										    			     */
			if( (u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) == I2C_ADDRESS){
				if(running_state == MASTER){
					HAL_UART_Transmit(huart, (u8 *)MASTER_NO_LED, strlen(MASTER_NO_LED), UART_TR_TIMEOUT);
					return PROCESS_SUCCESS;
				}
				else if(running_state == SLAVE){
					//Date_To_I2CBuff(command[1]);
					uart_rx_cnt = I2C_To_UART(I2C_buff);
					HAL_UART_Transmit(huart, (u8 *)UART_Rx_Buffer, uart_rx_cnt, UART_TR_TIMEOUT);
					return PROCESS_SUCCESS;
				}
			}
			
			/*        This address is another slave or master address                 */
			/*        Slave no permissions to read other device information           */
			if(running_state == SLAVE){
				HAL_UART_Transmit(huart, (u8 *)SLAVE_ERR_NO_READ, strlen(SLAVE_ERR_NO_READ), 0xFFFF);
				return PROCESS_SUCCESS;
			}
			
			/*        Self running state is master, can read another slave info       */
			if(I2C_BUS_BUSY == I2C_Master_Read((u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) + I2C_READ, I2C_buff)){
				HAL_UART_Transmit(huart, (u8 *)BUS_BUSY, strlen(BUS_BUSY), UART_TR_TIMEOUT);
				return PROCESS_SUCCESS;
			}
			uart_rx_cnt = I2C_To_UART(I2C_buff);
			HAL_UART_Transmit(huart, (u8 *)UART_Rx_Buffer, uart_rx_cnt, UART_TR_TIMEOUT);

			// if(!strcmp(LED_DURA, command[1])){
			// 	HAL_UART_Transmit(huart, (u8 *)(I2C_buff), DEFAULT_BUFF_SIZE - 3, UART_TR_TIMEOUT);
			// }
			// else if(!strcmp(LED_FREQ, command[1])){
			// 	HAL_UART_Transmit(huart, (u8 *)(I2C_buff + 2), DEFAULT_BUFF_SIZE - 3, UART_TR_TIMEOUT);
			// }
			// else{
			// 	uart_rx_cnt = I2C_To_UART(I2C_buff);
			// 	HAL_UART_Transmit(huart, (u8 *)UART_Rx_Buffer, uart_rx_cnt, UART_TR_TIMEOUT);
			// }
		}
	}
	
	/*            Set info option                                                 */
	else if(!strcmp(COMMAND_SET, command[0])){
		
		/*          Set    running state                                            */
		if(!strcmp(COMMAND_RUNNING_STATE, command[1])){
			
			if(command_buff_size < UART_SET_RUNSTATE_PARA_NUM){
				HAL_UART_Transmit(huart, (u8 *)COMMAND_ERR, strlen(COMMAND_ERR), UART_TR_TIMEOUT);
				return PROCESS_SUCCESS;
			}
			
			if(!strcmp(command[2], "slave")){
				if(running_state == SLAVE){
					HAL_UART_Transmit(huart, (u8 *)RUNSTAT_SLAVE_TO_SLAVE, strlen(RUNSTAT_SLAVE_TO_SLAVE),UART_TR_TIMEOUT);
				}
				else{
					running_state = slave;
					running = &slave_start;
					if(running_state == SLAVE){
						HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
					}
					else{
						HAL_UART_Transmit(huart, (u8 *)FAILED, strlen(FAILED),UART_TR_TIMEOUT);
					}
				}
			}
			else if(!strcmp(command[2], "master")){
				if(running_state == MASTER){
					HAL_UART_Transmit(huart, (u8 *)RUNSTAT_MASTER_TO_MASTER, strlen(RUNSTAT_MASTER_TO_MASTER), UART_TR_TIMEOUT);
				}
				else{
					running_state = master;
					running = &master_start;
					if(running_state == MASTER){
						HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
					}
					else{
						HAL_UART_Transmit(huart, (u8 *)FAILED, strlen(FAILED),UART_TR_TIMEOUT);
					}
				}
			}
		}
		
		/*           Set slave's light duration or frequency of led                  */
		else if(!strcmp(LED_DURA, command[1]) || !strcmp(LED_FREQ, command[1]) || !strcmp(LED_DURA_FREQ, command[1])){
			
			if(command_buff_size < UART_SET_LED_INFO_PARA_NUM){
				HAL_UART_Transmit(huart, (u8 *)COMMAND_ERR, strlen(COMMAND_ERR), UART_TR_TIMEOUT);
				return PROCESS_SUCCESS;
			}
			
			/*         This address is is self       										    			     */
			if( (u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) == I2C_ADDRESS){
				if(running_state == MASTER){
					HAL_UART_Transmit(huart, (u8 *)MASTER_NO_LED, strlen(MASTER_NO_LED), UART_TR_TIMEOUT);
					return PROCESS_SUCCESS;
				}
				else if(running_state == SLAVE){					
					if(!strcmp(LED_DURA, command[1])){
						set_led_duration((u8)String_To_Hex_Of_Data(command[2+TIME_OFFSET], strlen(command[2+TIME_OFFSET])) * get_units_mul((u8)command[2+UNITS_OFFSET][0]));
					}
					else if(!strcmp(LED_FREQ, command[1])){
						set_led_frequency((u8)String_To_Hex_Of_Data(command[2+TIME_OFFSET], strlen(command[2+TIME_OFFSET])) * get_units_mul((u8)command[2+UNITS_OFFSET][0]));
					}
					else{
						set_led_duration((u8)String_To_Hex_Of_Data(command[2+TIME_OFFSET], strlen(command[2+TIME_OFFSET])) * get_units_mul((u8)command[2+UNITS_OFFSET][0]));
						set_led_frequency((u8)String_To_Hex_Of_Data(command[2+TIME_OFFSET+ADDR_OFFSET], strlen(command[2+TIME_OFFSET+ADDR_OFFSET])) * get_units_mul((u8)command[2+UNITS_OFFSET+ADDR_OFFSET][0]));
					}
					HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK), UART_TR_TIMEOUT);
					return PROCESS_SUCCESS;
				}
			}
			
			/*        This address is another slave or master address                 */
			/*        Slave no permissions to set other device information            */
			if(running_state == SLAVE){
				HAL_UART_Transmit(huart, (u8 *)SLAVE_ERR_NO_READ, strlen(SLAVE_ERR_NO_READ), 0xFFFF);
				return PROCESS_SUCCESS;
			}
			
			/*        Self running state is master, can set another slave info        */ 	
			if(!strcmp(LED_DURA, command[1])){
				I2C_buff[START_ADDR] = I2C_PARA_LENGTH - 2;
				I2C_buff[BASE_ADDR] = LED_DURATION;
				I2C_buff[BASE_ADDR + TIME_OFFSET] = String_To_Hex_Of_Data(command[3], strlen(command[3]));
				I2C_buff[BASE_ADDR + UNITS_OFFSET] = (u8)command[4][0];
				if(I2C_BUS_BUSY == I2C_Master_Write((u8)String_To_Hex_Of_Data(command[2], strlen(command[2])) + I2C_WRITE, I2C_buff)){
					HAL_UART_Transmit(huart, (u8 *)BUS_BUSY, strlen(BUS_BUSY), UART_TR_TIMEOUT);
					return I2C_BUS_BUSY;
				}
				
				delay_ms(1);
				if(I2C_BUS_BUSY == I2C_Master_Read((u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) + I2C_READ, I2C_receive_buff)){
					HAL_UART_Transmit(huart, (u8 *)BUS_BUSY, strlen(BUS_BUSY), UART_TR_TIMEOUT);
					return I2C_BUS_BUSY;
				}
				I2C_receive_buff[ADDR_OFFSET] = '\0';
				if(!strcmp((char *)(I2C_buff + 1), (char *)I2C_receive_buff)){
					HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
				}
				else{
					HAL_UART_Transmit(huart, (u8 *)FAILED, strlen(FAILED),UART_TR_TIMEOUT);
				}
			}
			else if(!strcmp(LED_FREQ, command[1])){
				I2C_buff[START_ADDR] = I2C_PARA_LENGTH - 2;
				I2C_buff[BASE_ADDR] = LED_FREQUENCY;
				I2C_buff[BASE_ADDR + TIME_OFFSET] = String_To_Hex_Of_Data(command[3], strlen(command[3]));
				I2C_buff[BASE_ADDR + UNITS_OFFSET] = (u8)command[4][0];
				if(I2C_BUS_BUSY == I2C_Master_Write((u8)String_To_Hex_Of_Data(command[2], strlen(command[2])) + I2C_WRITE, I2C_buff)){
					HAL_UART_Transmit(huart, (u8 *)BUS_BUSY, strlen(BUS_BUSY), UART_TR_TIMEOUT);
					return I2C_BUS_BUSY;
				}
				
				delay_ms(1);
				if(I2C_BUS_BUSY == I2C_Master_Read((u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) + I2C_READ, I2C_receive_buff)){
					HAL_UART_Transmit(huart, (u8 *)BUS_BUSY, strlen(BUS_BUSY), UART_TR_TIMEOUT);
					return I2C_BUS_BUSY;
				}
				if(!strcmp((char *)(I2C_receive_buff + ADDR_OFFSET), (char *)(I2C_buff + 1))){
					HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
				}
				else{
					HAL_UART_Transmit(huart, (u8 *)FAILED, strlen(FAILED),UART_TR_TIMEOUT);
				}
			}
			else{
				I2C_buff[START_ADDR] = I2C_PARA_LENGTH;
				I2C_buff[BASE_ADDR] = LED_DURATION_FREQUENCY;
				I2C_buff[BASE_ADDR + TIME_OFFSET] = String_To_Hex_Of_Data(command[3], strlen(command[3]));
				I2C_buff[BASE_ADDR + UNITS_OFFSET] = (u8)command[4][0];
				I2C_buff[BASE_ADDR + ADDR_OFFSET + TIME_OFFSET] = String_To_Hex_Of_Data(command[3 + ADDR_OFFSET], strlen(command[3 + ADDR_OFFSET]));
				I2C_buff[BASE_ADDR + ADDR_OFFSET + UNITS_OFFSET] = (u8)command[4 + ADDR_OFFSET][0];
				if(I2C_BUS_BUSY == I2C_Master_Write((u8)String_To_Hex_Of_Data(command[2], strlen(command[2])) + I2C_WRITE, I2C_buff)){
					HAL_UART_Transmit(huart, (u8 *)BUS_BUSY, strlen(BUS_BUSY), UART_TR_TIMEOUT);
					return I2C_BUS_BUSY;
				}
				
				delay_ms(1);
				if(I2C_BUS_BUSY == I2C_Master_Read((u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) + I2C_READ, I2C_receive_buff)){
					HAL_UART_Transmit(huart, (u8 *)BUS_BUSY, strlen(BUS_BUSY), UART_TR_TIMEOUT);
					return I2C_BUS_BUSY;
				}
				if(!strcmp((char *)I2C_buff, (char *)I2C_receive_buff)){
					HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
				}
				else{
					HAL_UART_Transmit(huart, (u8 *)FAILED, strlen(FAILED),UART_TR_TIMEOUT);
				}
			}
			uart_rx_cnt = I2C_To_UART(I2C_receive_buff);
			HAL_UART_Transmit(huart, (u8 *)UART_Rx_Buffer, uart_rx_cnt,UART_TR_TIMEOUT);
		}		
	}
	else{
		HAL_UART_Transmit(huart, (u8 *)COMMAND_ERR, strlen(COMMAND_ERR), UART_TR_TIMEOUT);
	}
	return PROCESS_SUCCESS;
}


/**
  * @brief  This function is UART callback.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
	if(uart_rx_cnt >= 255)
	{
		uart_rx_cnt = 0;
		memset(UART_Rx_Buffer,0x00,sizeof(UART_Rx_Buffer));
		HAL_UART_Transmit(huart, (u8 *)PARA_LENGTH_ERR, strlen(PARA_LENGTH_ERR), UART_TR_TIMEOUT); 	
        
	}
	else
	{
		UART_Rx_Buffer[uart_rx_cnt++] = Rx_Byte;   
		if(UART_Rx_Buffer[uart_rx_cnt-1] == '\n')
		{
      while(HAL_UART_GetState(huart) == HAL_UART_STATE_BUSY_TX);
			UART_Process_Param(huart);
			uart_rx_cnt = 0;
			memset(UART_Rx_Buffer,0x00,sizeof(UART_Rx_Buffer));
		}
	}
}


/**
  * @brief  This function is falling exti callback.
  * @retval None
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	if(running_state == MASTER && GPIO_Pin == MASTER_EXTI_PIN){
		Master_EXTI_Falling_Callback(GPIO_Pin);
	}
	else if(running_state == SLAVE && GPIO_Pin == SLAVE_EXTI_PIN){
		Slave_EXTI_Falling_Callback(GPIO_Pin);
	}
}


/**
  * @brief  This function is rising exti callback.
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	if(running_state == MASTER && GPIO_Pin == MASTER_EXTI_PIN){
		Master_EXTI_Rising_Callback(GPIO_Pin);
	}
	else if(running_state == SLAVE && GPIO_Pin == SLAVE_EXTI_PIN){
		Slave_EXTI_Rising_Callback(GPIO_Pin);
	}
}


