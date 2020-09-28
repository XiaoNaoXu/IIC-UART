/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  */


#include "main.h"
#include "gpio.h"
#include "i2c.h"

#define receive_buff sent_buff    									// Receive and send use the same array

void SystemClock_Config(void);
void  flag_reset(void);
void param_assert(void);

__IO u8 bit_location = 0U;        									//Records the location of the bits received
__IO u8 a_bit_value = 0U;														//Store a full byte received
u8 sent_buff[DEFAULT_BUFF_SIZE] = {0xFF,1,0,1,0};		//Send array/buff
__IO u8 receive_cnt = 0;														//The number that has been sent
__IO u8 receive_len = 3;														//The number when need to send
__IO u8 sent_cnt = 0;																//The number that has been received
Option option = ret;																//Control read/write state
__IO u32 led_frequency = 0;													//The frequency of the led
__IO u32 led_duration = 0;													//The duration of the led

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  HAL_Init();																					//Init HAL

  SystemClock_Config();																//Config System Clock
	
  MX_GPIO_Init();																			//Init Green LED GPIO --- PA5
	
	i2c_slave_SDA_GPIO_Input_Init();									  //Init SDA GPIO       --- PC4
	
	i2c_slave_SCL_Falling_Exti_Enable();								//Enable SCL Falling exti
	
	param_assert();
  while(1)
  {
		if(led_duration){
			LED(led_duration);
		}
		if(led_frequency){
			delay_us(led_frequency);
		}
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
  * @brief  This function is falling exti callback.
  * @retval None
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	if(is_i2c_Start() && GPIO_Pin == I2C_SCL_PIN){
		i2c_slave_SCL_Rising_Exti_Enable();
		i2c_slave_SCL_Falling_Exti_Disable();
		option = ret;
		flag_reset();
	}
}

/**
  * @brief  This function is rising exti callback.
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	/*receive first bit : its address and read/write bit.  */
	if(option == ret){
		bit_location++;
		if(bit_location < BIT_LENGTH + 1){
			a_bit_value <<= 1U;
			a_bit_value |= I2C_SDA_READ();
		}
		else{
 			if((a_bit_value & SELF_ADDRESS_READ) == a_bit_value){
				option = read;
			}
			else if((a_bit_value & SELF_ADDRESS_WRITE) == a_bit_value){
				option = write;
				a_bit_value = sent_buff[++sent_cnt];
			}
			else{
				bit_location = 0;
				i2c_slave_SCL_Falling_Exti_Reable();
				i2c_slave_SCL_Rising_Exti_Disable();
				return;
			}
			i2c_SendAck();
			bit_location = 0;
		}
	}
	
	/*    Receive data    */
	else if(option == read){
		
		/*  Receive a byte  */
		bit_location++;
		if(bit_location < BIT_LENGTH + 1){
			a_bit_value <<= 1U;
			a_bit_value |= I2C_SDA_READ();
		}
		else{
			/*            Store this byte and wait acknowledge              */
			
			/*   How to store first byte  */
			if(!receive_cnt){
				receive_buff[base_addr] = a_bit_value;
			}
			
			/*   How to store next byte received   */
			else{
				/*   This byte use for duration only    */
				if(receive_buff[base_addr] == LED_duration){
					receive_buff[base_addr + receive_cnt] = a_bit_value;
					receive_len = I2C_para_length - 2;
				}
				/*   This byte use for frequency only   */
				else if(receive_buff[base_addr] == LED_frequency){
					receive_buff[base_addr + receive_cnt + addr_offet] = a_bit_value;
					receive_len = I2C_para_length - 2;
				}
				/*   This byte use for duration/frequency only   */
				else{
					receive_buff[receive_cnt] = a_bit_value;
					receive_len = I2C_para_length;
				}
			}
			receive_cnt++;
			
			/*   Send acknowledge and reable/disable EXTI   */
			if(receive_cnt < receive_len){
				i2c_SendAck();
			}
			else{
				i2c_SendNAck();
				i2c_slave_SCL_Rising_Exti_Disable();
				i2c_slave_SCL_Falling_Exti_Reable();				
				param_assert();
			}
			bit_location = 0;
		}
	}
	/*   To be confirmed   */
	else if(option == write){
		bit_location++;
		if(bit_location < BIT_LENGTH + 1){
			(a_bit_value & 0x80) ? (I2C_SDA_1()) : (I2C_SDA_0());
			a_bit_value <<= 1U;
			if(bit_location == BIT_LENGTH){
				delay_us(I2C_PD);
				I2C_SDA_1();
			}
		}
		else{
			if(I2C_SDA_READ()){
				I2C_SDA_1();
				i2c_slave_SCL_Falling_Exti_Reable();
				i2c_slave_SCL_Rising_Exti_Disable();
				return;
			}
			if(sent_cnt < I2C_para_length){
				a_bit_value = sent_buff[++sent_cnt];
			}
			else{
				i2c_slave_SCL_Falling_Exti_Reable();
				i2c_slave_SCL_Rising_Exti_Disable();
			}
			bit_location = 0;
		}
	}
	
}


/**
  * @brief  Reset flag.
  * @retval None
  */
void flag_reset(){
	option = ret;
	bit_location = 0;
	receive_cnt = 0;
	a_bit_value = 0;
	sent_cnt = 0;
}

/**
  * @brief  Set led frequency.
  * @retval None
  */
void set_led_frequency(u32 frequency){
	led_frequency = frequency;
}

/**
  * @brief  Set led duration.
  * @retval None
  */
void set_led_duration(u32 duration){
	led_duration = duration;
}


/**
  * @brief  Convert s/ms to us.
  * @retval None
  */
u32 get_units_mul(u8 units){
	switch(units){
		case I2C_S: return I2C_S_TO_US;
		case I2C_MS: return I2C_MS_TO_US;
		default: return I2C_US;
	}
}

/**
  * @brief  Assert parameter.
  * @retval None
  */
void param_assert(){
	if((receive_buff[base_addr] & LED_duration) == LED_duration){
		set_led_duration(receive_buff[time_offset] * get_units_mul(receive_buff[units_offset]));
	}
	if((receive_buff[base_addr] & LED_frequency) == LED_frequency){
		set_led_frequency(receive_buff[time_offset + addr_offet] * get_units_mul(receive_buff[units_offset + addr_offet]));
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

}

