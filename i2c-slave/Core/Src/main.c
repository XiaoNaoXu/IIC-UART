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

extern u8 I2C_buff[DEFAULT_BUFF_SIZE];
extern u8 I2C_receive_buff[DEFAULT_BUFF_SIZE];


Running_State running_state = MASTER;
void (* running)(void) = &master_start;

//Running_State running_state = SLAVE;
//void (* running)(void) = &slave_start;

/*                    I2C ADDRESS                    */


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  HAL_Init();																							//Init HAL

  SystemClock_Config();																		//Config System Clock
	
	MX_USART2_UART_Init();																	//Init UART2 GPIO
	
	//HAL_UART_Receive_IT(&huart2, (uint8_t *)&Rx_Byte, 1);
	
	//slave_start();
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

u8 UART_Process_Param(UART_HandleTypeDef *huart){

	__IO u8 uart_buff_location = 0, command_buff_size = 0, command_buff_length = 0;
	
	/*          Process parameter from UART                                    */
	char command[MAX_COMMAND_BUFF_SIZE][MAX_COMMAND_LENGH] = {'\0'};
	memset(I2C_receive_buff, '\0', DEFAULT_BUFF_SIZE);
	
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
	
	/*            Get info option                                                 */
	if(!strcmp(GET, command[0])){
		
		/*          Get running state                                              */
		if(!strcmp(RUNNING_STATE, command[1])){
			if(running_state == MASTER){
				HAL_UART_Transmit(huart, (u8 *)RUNSTAT_MASTER, strlen(RUNSTAT_MASTER),UART_TR_TIMEOUT);
			}
			else if(running_state == SLAVE){
				HAL_UART_Transmit(huart, (u8 *)RUNSTAT_SLAVE, strlen(RUNSTAT_SLAVE),UART_TR_TIMEOUT);
			}
		}
		
		/*           Get slave's light duration or frequency of led                  */
		else if(!strcmp(LED_DURA, command[1]) || !strcmp(LED_FREQ, command[1]) || !strcmp(LED_DURA_FREQ, command[1])){
			/*         This address is is self       										    			     */
			if( (u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) == I2C_ADDRESS){
				if(running_state == MASTER){
					HAL_UART_Transmit(huart, (u8 *)MASTER_NO_LED, strlen(MASTER_NO_LED), UART_TR_TIMEOUT);
					return 0;
				}
				else if(running_state == SLAVE){
					Date_To_I2CBuff();
					HAL_UART_Transmit(huart, (u8 *)I2C_buff, DEFAULT_BUFF_SIZE - 1, UART_TR_TIMEOUT);
					return 0;
				}
			}
			
			/*        This address is another slave or master address                 */
			/*        Slave no permissions to read other device information           */
			if(running_state == SLAVE){
				HAL_UART_Transmit(huart, (u8 *)SLAVE_ERR_NO_READ, strlen(SLAVE_ERR_NO_READ), 0xFFFF);
				return 0;
			}
			
			/*        Self running state is master, can read another slave info       */
			I2C_Master_Read((u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) + I2C_READ, I2C_buff, I2C_PARA_LENGTH);
			if(!strcmp(LED_DURA, command[1])){
				HAL_UART_Transmit(huart, (u8 *)(I2C_buff), DEFAULT_BUFF_SIZE - 3, UART_TR_TIMEOUT);
			}
			else if(!strcmp(LED_FREQ, command[1])){
				HAL_UART_Transmit(huart, (u8 *)(I2C_buff + 2), DEFAULT_BUFF_SIZE - 3, UART_TR_TIMEOUT);
			}
			else{
				HAL_UART_Transmit(huart, (u8 *)I2C_buff, DEFAULT_BUFF_SIZE - 1, UART_TR_TIMEOUT);
			}
		}
	}
	
	/*            Set info option                                                 */
	else if(!strcmp(SET, command[0])){
		
		/*          Set    running state                                            */
		if(!strcmp(RUNNING_STATE, command[1])){
			if(!strcmp(command[2], "slave")){
				if(running_state == SLAVE){
					HAL_UART_Transmit(huart, (u8 *)RUNSTAT_SLAVE_TO_SLAVE, strlen(RUNSTAT_SLAVE_TO_SLAVE),UART_TR_TIMEOUT);
				}
				else{
					running_state = SLAVE;
					running = &slave_start;
					if(running_state == SLAVE){
						HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
					}
					else{
						HAL_UART_Transmit(huart, (u8 *)DEFAULT, strlen(DEFAULT),UART_TR_TIMEOUT);
					}
				}
			}
			else if(!strcmp(command[2], "master")){
				if(running_state == MASTER){
					HAL_UART_Transmit(huart, (u8 *)RUNSTAT_MASTER_TO_MASTER, strlen(RUNSTAT_MASTER_TO_MASTER), UART_TR_TIMEOUT);
				}
				else{
					running_state = MASTER;
					running = &master_start;
					if(running_state == MASTER){
						HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
					}
					else{
						HAL_UART_Transmit(huart, (u8 *)DEFAULT, strlen(DEFAULT),UART_TR_TIMEOUT);
					}
				}
			}
		}
		
		/*           Set slave's light duration or frequency of led                  */
		else if(!strcmp(LED_DURA, command[1]) || !strcmp(LED_FREQ, command[1]) || !strcmp(LED_DURA_FREQ, command[1])){
			/*         This address is is self       										    			     */
			if( (u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) == I2C_ADDRESS){
				if(running_state == MASTER){
					HAL_UART_Transmit(huart, (u8 *)MASTER_NO_LED, strlen(MASTER_NO_LED), UART_TR_TIMEOUT);
					return 0;
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
					return 0;
				}
			}
			
			/*        This address is another slave or master address                 */
			/*        Slave no permissions to set other device information            */
			if(running_state == SLAVE){
				HAL_UART_Transmit(huart, (u8 *)SLAVE_ERR_NO_READ, strlen(SLAVE_ERR_NO_READ), 0xFFFF);
				return 0;
			}
			
			/*        Self running state is master, can set another slave info        */ 
			
			
			if(!strcmp(LED_DURA, command[1])){
				I2C_buff[0] = LED_duration;
				I2C_buff[TIME_OFFSET] = String_To_Hex_Of_Data(command[3], strlen(command[3]));
				I2C_buff[UNITS_OFFSET] = (u8)command[4][0];
				I2C_Master_Write((u8)String_To_Hex_Of_Data(command[2], strlen(command[2])), I2C_buff, I2C_PARA_LENGTH - 2);
				
				delay_ms(1);
				I2C_Master_Read((u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) + I2C_READ, I2C_receive_buff, I2C_PARA_LENGTH - 1);
				I2C_receive_buff[ADDR_OFFSET] = '\0';
				if(!strcmp((char *)(I2C_buff + 1), (char *)I2C_receive_buff)){
					HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
				}
				else{
					HAL_UART_Transmit(huart, (u8 *)DEFAULT, strlen(DEFAULT),UART_TR_TIMEOUT);
				}
			}
			else if(!strcmp(LED_FREQ, command[1])){
				I2C_buff[BASE_ADDR] = LED_frequency;
				I2C_buff[TIME_OFFSET] = String_To_Hex_Of_Data(command[3], strlen(command[3]));
				I2C_buff[UNITS_OFFSET] = (u8)command[4][0];
				I2C_Master_Write((u8)String_To_Hex_Of_Data(command[2], strlen(command[2])), I2C_buff, I2C_PARA_LENGTH - 2);
				
				delay_ms(1);
				I2C_Master_Read((u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) + I2C_READ, I2C_receive_buff, I2C_PARA_LENGTH - 1);
				if(!strcmp((char *)(I2C_receive_buff + ADDR_OFFSET), (char *)(I2C_buff + 1))){
					HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
				}
				else{
					HAL_UART_Transmit(huart, (u8 *)DEFAULT, strlen(DEFAULT),UART_TR_TIMEOUT);
				}
			}
			else{
				I2C_buff[0] = LED_duration_frequency;
				I2C_buff[TIME_OFFSET] = String_To_Hex_Of_Data(command[3], strlen(command[3]));
				I2C_buff[UNITS_OFFSET] = (u8)command[4][0];
				I2C_buff[ADDR_OFFSET + TIME_OFFSET] = String_To_Hex_Of_Data(command[3 + ADDR_OFFSET], strlen(command[3 + ADDR_OFFSET]));
				I2C_buff[ADDR_OFFSET + UNITS_OFFSET] = (u8)command[4 + ADDR_OFFSET][0];
				I2C_Master_Write((u8)String_To_Hex_Of_Data(command[2], strlen(command[2])), I2C_buff, I2C_PARA_LENGTH);
				
				delay_ms(1);
				I2C_Master_Read((u8)String_To_Hex_Of_Data(command[2], I2C_ADDRESS_LEN) + I2C_READ, I2C_receive_buff, I2C_PARA_LENGTH - 1);
				if(!strcmp((char *)(I2C_buff + 1), (char *)I2C_receive_buff)){
					HAL_UART_Transmit(huart, (u8 *)OK, strlen(OK),UART_TR_TIMEOUT);
				}
				else{
					HAL_UART_Transmit(huart, (u8 *)DEFAULT, strlen(DEFAULT),UART_TR_TIMEOUT);
				}
				return 0U;
			}
		}
			
	}
	

	else{
		HAL_UART_Transmit(huart, (u8 *)COMMAND_ERR, strlen(COMMAND_ERR), 0xFFFF);
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
		HAL_UART_Transmit(huart, (uint8_t *)"FFFFFF", 10,UART_TR_TIMEOUT); 	
        
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

