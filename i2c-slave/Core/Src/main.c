/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  */


#include "main.h"
#include "gpio.h"
#include "i2c.h"


void SystemClock_Config(void);
void (* Execute_Address)(void);
void  flag_reset();

__IO u8 bit_location = 0U;
__IO u8 a_bit_value = 0U;
u8 receive_buff[DEFAULT_BUFF_SIZE] = {0};
u8 sent_buff[4] = {10,9,8,7};
__IO u8 receive_cnt = 0;
__IO u8 sent_cnt = 4;
Option option = ret;

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
	if(is_i2c_Start() && GPIO_Pin == GPIO_PIN_5){
		i2c_slave_SCL_Rising_Exti_Enable();
		//option = ret;
		//flag_reset();
		//I2C_Read();
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
					a_bit_value = sent_buff[0];
				}
				else{
					i2c_slave_SCL_Rising_Exti_Disable();
					return;
				}
			}
			else{
				i2c_slave_SCL_Rising_Exti_Disable();
				return;
			}
			i2c_SendAck();
			//LED(I2C_PD);
			flag_reset();
		}
	}
	else if(option == read){
		bit_location++;
		if(bit_location < 9){
			a_bit_value <<= 1U;
			a_bit_value |= I2C_SDA_READ();
		}
		if(bit_location == 9){
			if(receive_cnt < DEFAULT_BUFF_SIZE){
				i2c_SendAck();
				receive_buff[receive_cnt++] = a_bit_value;
			}
			else{
				i2c_SendNAck();
			}
			flag_reset();
		}
	}
	else if(option == write){
		bit_location++;
		if(bit_location < 9){
			(a_bit_value & 0x80) ? (I2C_SDA_1()) : (I2C_SDA_0());
			a_bit_value <<= 1U;
		}
		if(bit_location == 9){
			if(I2C_SDA_READ()){
				i2c_slave_SCL_Rising_Exti_Disable();
				return;
			}
			if(sent_cnt > 0){
				a_bit_value = sent_buff[--sent_cnt];
			}
			else{
				i2c_slave_SCL_Rising_Exti_Disable();
				return;
			}
			flag_reset();
		}
	}
	
}

void flag_reset(){
	bit_location = 0;
	//a_bit_value = 0;
}

void buff_reset(u8 *buff_ptr, u8 buff_length){

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

}

