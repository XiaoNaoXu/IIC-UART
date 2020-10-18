/**
  ******************************************************************************
  * @file           : slave.c
  * @brief          : Slave related code
  */

#include "slave.h"


#define receive_buff sent_buff    														// Receive and send use the same array

__IO I2C_TYPE    bit_location = 0U;        													//Records the location of the bits received
__IO I2C_TYPE    a_bit_value = 0U;																	//Store a full byte received
__IO I2C_TYPE    receive_cnt = 0;																		//The number that has been sent
__IO I2C_TYPE    receive_len = 3;															   		//The number when need to send
__IO I2C_TYPE    sent_count = 0;												      				//The number that has been received
__IO u32  			 led_frequency = 0;									        				//The frequency of the led
__IO u32   			 led_duration = 0;										        			//The duration of the led
Option     			 option = ret;												      				//Control read/write state

I2C_TYPE         sent_buff[DEFAULT_BUFF_SIZE] = {I2C_PARA_LENGTH, LED_DURATION_FREQUENCY, 1, I2C_S, 1, I2C_S};	   	//Send array/buff



/**
  * @brief  Slave entry point.
  * @retval None
  */
void slave_start(){
	
	param_assert();
	
	I2C_SDA_Falling_Enable();
	
  while(running_state == SLAVE)
  {
		if(UART_DATA_REG == UART_DATA_OK){
			UART_Process_Param(&huart2);
		}
		
		if(led_duration){
			LED(led_duration);
		}
		if(led_frequency){
			delay_us(led_frequency);
		}
  }
}


/**
  * @brief  Reset flag.
  * @retval None
  */
void flag_reset(){
	option 					= 	ret;
	bit_location 		= 	0U;
	receive_cnt 		= 	0U;
	a_bit_value	  	= 	0U;
	receive_len 		= 	3U;
	sent_count 			= 	0U;
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
u32 get_units_mul(I2C_TYPE units){
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
	I2C_TYPE state = receive_buff[BASE_ADDR], offset = START_ADDR;
	
	while(state){
		/*     This byte use for duration only                   */
		if((state & LED_DURATION)== LED_DURATION){
			set_led_duration(receive_buff[BASE_ADDR + TIME_OFFSET + offset] * get_units_mul(receive_buff[BASE_ADDR + offset + UNITS_OFFSET]));
			state &= ~LED_DURATION;
			offset = ADDR_OFFSET;
		}
		
		/*     This byte use for frequency only                  */
		else if((state & LED_FREQUENCY) == LED_FREQUENCY){
			set_led_frequency(receive_buff[BASE_ADDR + TIME_OFFSET + offset] * get_units_mul(receive_buff[BASE_ADDR + offset + UNITS_OFFSET]));
			state &= ~LED_FREQUENCY;
		}
		
		else if(state == RUNNING_STATE){
			if(receive_buff[BASE_ADDR + STATE_OFFSET] == MASTER){
				running_state = master;
				running = &master_start;
			}
			else{
				running_state = slave;
				running = &slave_start;
			}
		}
	}
	if((receive_buff[BASE_ADDR] & LED_DURATION_FREQUENCY)){
		Date_To_I2CBuff(receive_buff[BASE_ADDR]);
	}
}



/**
  * @brief  This function is slave's falling exti callback.
  * @retval None
  */
void Slave_SDA_EXTI_Falling_Callback()
{
	if(is_I2C_Slave_Start()){
		I2C_SDA_Falling_Disable();
		I2C_SCL_Rising_Enable();
		flag_reset();
	}
}


/**
  * @brief  This function is slave's falling exti callback.
  * @retval None
  */
void Slave_SDA_EXTI_Rising_Callback()
{
	if(is_I2C_Slave_Stop()){
		I2C_SCL_Falling_Rising_Disable();
		I2C_SDA_Falling_Enable();
	}
}


/**
  * @brief  This function is slave's falling exti callback.
  * @retval None
  */
void Slave_SCL_EXTI_Falling_Callback()
{
	bit_location++;
	if(option == write){
		if(bit_location <= BIT_LENGTH){
			SEND_HIGHEST_BIT(a_bit_value);
			a_bit_value <<= 1U;
			if(bit_location == BIT_LENGTH){
				I2C_SCL_FallingDisable_RisingEnable();
				return;
			}
		}
	}
	else{
		if(bit_location == BIT_LENGTH + 1){
			if(receive_cnt < receive_len){
				I2C_Slave_SendAck();
			}
			else{
				I2C_Slave_SendNAck();
				I2C_SCL_Falling_Rising_Enable();
				I2C_SDA_Falling_Enable();
			}
		}
		else if(bit_location == BIT_LENGTH + 2){
			bit_location = 0;
			I2C_SDA_1();
			I2C_SCL_FallingDisable_RisingEnable();
		}
	}
}



/**
  * @brief  This function is slave's rising exti callback.
  * @retval None
  */
void Slave_SCL_EXTI_Rising_Callback()
{
	/*receive first bit : its address and read/write bit.  */
	//LED(0);
	if(option == ret){
		bit_location++;
		if(bit_location <= BIT_LENGTH){
			a_bit_value <<= 1U;
			a_bit_value |= I2C_SDA_READ();
			if(bit_location == BIT_LENGTH){
				if((a_bit_value & (I2C_ADDRESS + I2C_WRITE)) == a_bit_value){
					I2C_SCL_FallingEnable_RisingDisable();
					option = read;
				}
				else if((a_bit_value & (I2C_ADDRESS + I2C_READ)) == a_bit_value){
					I2C_SCL_FallingEnable_RisingDisable();
					option = write;
					a_bit_value = I2C_buff[sent_count++];
				}
				else{
					bit_location = 0;
					I2C_SCL_FallingDisable_RisingEnable();
					return;
				}
			}
		}
	}
		
	/*    		Receive data   						 	      	  */
	else if(option == read){
		
		/* 			Receive a byte  		         	      	*/
		bit_location++;
		if(bit_location <= BIT_LENGTH){
			a_bit_value <<= 1U;
			a_bit_value |= I2C_SDA_READ();
			
			if(bit_location == BIT_LENGTH){
				/*       Store this byte and wait acknowledge              */
				/*       How to store first byte                           */
				if(!receive_cnt){
					receive_len = a_bit_value;
				}
				receive_buff[receive_cnt++] = a_bit_value;
				bit_location = 0;
				I2C_SCL_FallingEnable_RisingDisable();
			}
		}
	}
	else if(option == write){
		bit_location = 0;
		if(I2C_SDA_READ()){
			I2C_SCL_FallingEnable_RisingDisable();
			return;
		}
		if(sent_count <= I2C_buff[START_ADDR]){
			a_bit_value = I2C_buff[sent_count++];
		}
	}
}






void Data_Transfer(u32 para, I2C_TYPE offset){
	
	if((para / I2C_S_TO_US) <= 0xFF && (para / I2C_S_TO_US) >= 0x01 ){
		I2C_buff[offset + BASE_ADDR + TIME_OFFSET] = (I2C_TYPE)(para/I2C_S_TO_US);
		I2C_buff[offset + BASE_ADDR + UNITS_OFFSET] = (I2C_TYPE)'s';
	}
	else if((para / I2C_MS_TO_US) <= 0xFF  && (para / I2C_S_TO_US) >= 0x01 ){
		I2C_buff[offset + BASE_ADDR + TIME_OFFSET] = (I2C_TYPE)(para/I2C_MS_TO_US);
		I2C_buff[offset + BASE_ADDR + UNITS_OFFSET] = (I2C_TYPE)'m';
	}
	else{
		I2C_buff[offset + BASE_ADDR + TIME_OFFSET] = (I2C_TYPE)(para/I2C_MS_TO_US);
		I2C_buff[offset + BASE_ADDR + UNITS_OFFSET] = (I2C_TYPE)'u';
	}
}

/**
  * @brief  This function is get LED duration and frequency 
	* 				when running state is slave but receive a get command.
  * @retval None
  */
I2C_TYPE Date_To_I2CBuff(I2C_TYPE state){
	I2C_buff[START_ADDR] = (state == LED_DURATION_FREQUENCY) ? (I2C_PARA_LENGTH):(I2C_PARA_LENGTH - 2);
	I2C_buff[BASE_ADDR] = state;
	while(state){
		if((state & LED_DURATION) == LED_DURATION){
			Data_Transfer(led_duration, START_ADDR);
			state &= ~LED_DURATION;
		}
		else if((state & LED_FREQUENCY) == LED_FREQUENCY){
			Data_Transfer(led_frequency, ADDR_OFFSET);
			state &= ~LED_FREQUENCY;
		}
	}
	return PROCESS_SUCCESS;
}

