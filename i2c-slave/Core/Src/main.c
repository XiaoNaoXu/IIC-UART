/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  */


#include "main.h"
#include "gpio.h"
#include "i2c.h"

#define receive_buff sent_buff

void SystemClock_Config(void);
void (* Execute_Address)(void);
void  flag_reset(void);
void param_assert(void);

__IO u8 bit_location = 0U;
__IO u8 a_bit_value = 0U;
//u8 receive_buff[DEFAULT_BUFF_SIZE] = {0};
u8 sent_buff[DEFAULT_BUFF_SIZE] = {1,0,1,0};
__IO u8 receive_cnt = 0;
__IO u8 receive_len = 1;
__IO u8 sent_cnt = 5;
Option option = ret;
u32 led_frequency = 1000000;
u32 led_duration = 1000000;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	Execute_Address = &i2c_ReadBit;

  HAL_Init();																					//Init HAL

  SystemClock_Config();																//Config System Clock
	
  MX_GPIO_Init();																			//Init Green LED GPIO --- PA5
	
	i2c_slave_SDA_GPIO_Input_Init();									  //Init SDA GPIO       --- PC4
	
	i2c_slave_SCL_Falling_Exti_Enable();								//Enable SCL Falling exti
  while (1)
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
	//LED(0);
	if(option == ret){
		bit_location++;
		if(bit_location < 9){
			a_bit_value <<= 1U;
			a_bit_value |= I2C_SDA_READ();
		}
		if(bit_location == 9){
			if((a_bit_value & SELF_ADDRESS_READ) == a_bit_value){
				option = read;
			}
			else if((a_bit_value & SELF_ADDRESS_WRITE) == a_bit_value){
				option = write;
				if(sent_cnt > 0){
					a_bit_value = sent_buff[--sent_cnt];
				}
				else{
					bit_location = 0;
					i2c_slave_SCL_Falling_Exti_Reable();
					i2c_slave_SCL_Rising_Exti_Disable();
					return;
				}
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
	else if(option == read){
		bit_location++;
		if(bit_location < 9){
			a_bit_value <<= 1U;
			a_bit_value |= I2C_SDA_READ();
		}
		if(bit_location == 9){
			receive_buff[receive_cnt++] = a_bit_value;
			if(receive_len == 1 && receive_buff[0] == LED_duration_frequency){
				receive_len += 4;
			}
			else if(receive_len == 1 && (receive_buff[0] == LED_frequency || receive_buff[0] == LED_duration)){
				receive_len += 2;
			}
			if(receive_cnt < receive_len){
				i2c_SendAck();
			}
			else if(receive_cnt == receive_len){
				i2c_SendNAck();
				sent_cnt = receive_len;
				i2c_slave_SCL_Rising_Exti_Disable();
				i2c_slave_SCL_Falling_Exti_Reable();				
				param_assert();
			}
			bit_location = 0;
		}
	}
	else if(option == write){
		bit_location++;
		if(bit_location < 9){
			(a_bit_value & 0x80) ? (I2C_SDA_1()) : (I2C_SDA_0());
			a_bit_value <<= 1U;
			if(bit_location == 8){
				delay_us(I2C_PD);
				I2C_SDA_1();
			}
		}
		if(bit_location == 9){
			if(I2C_SDA_READ()){
				I2C_SDA_1();
				i2c_slave_SCL_Falling_Exti_Reable();
				i2c_slave_SCL_Rising_Exti_Disable();
			}
			if(sent_cnt > 0){
				a_bit_value = sent_buff[--sent_cnt];
			}
			else{
				i2c_slave_SCL_Falling_Exti_Reable();
				i2c_slave_SCL_Rising_Exti_Disable();
			}
			bit_location = 0;
		}
	}
	
}

void flag_reset(){
	option = ret;
	bit_location = 0;
	receive_cnt = 0;
	receive_len = 1;
	//a_bit_value = 0;
}

void set_led_frequency(u32 frequency){
	led_frequency = frequency;
}

void set_led_duration(u32 duration){
	led_duration = duration;
}

u32 get_units_mul(u8 units){
	switch(units){
		case I2C_S: return I2C_S_TO_US;
		case I2C_MS: return I2C_MS_TO_US;
		default: return I2C_US;
	}
}

void param_assert(){
	__IO u8 i = 0;
	u32 addr_led_frequency = 0, addr_led_duration = 0;
	if(receive_buff[base_addr] == LED_duration){
		set_led_duration(receive_buff[time_offset] * get_units_mul(receive_buff[units_offset]));
	}
	else if(receive_buff[base_addr] == LED_frequency){
		set_led_frequency(receive_buff[time_offset] * get_units_mul(receive_buff[units_offset]));
	}
	else if(receive_buff[base_addr] == LED_duration_frequency){
		set_led_duration(receive_buff[time_offset] * get_units_mul(receive_buff[units_offset]));
		set_led_frequency(receive_buff[time_offset + 2] * get_units_mul(receive_buff[units_offset + 2]));
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

}

