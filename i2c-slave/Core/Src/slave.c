/**
  ******************************************************************************
  * @file           : slave.c
  * @brief          : Slave related code
  */

#include "slave.h"


#define receive_buff sent_buff    														// Receive and send use the same array

__IO u8    bit_location = 0U;        													//Records the location of the bits received
__IO u8    a_bit_value = 0U;																	//Store a full byte received
__IO u8    receive_cnt = 0;																		//The number that has been sent
__IO u8    receive_len = 3;															   		//The number when need to send
__IO u8    sent_cnt = 0;												      				//The number that has been received
__IO u32   led_frequency = 0;									        				//The frequency of the led
__IO u32   led_duration = 0;										        			//The duration of the led
Option     option = ret;												      				//Control read/write state

u8         sent_buff[DEFAULT_BUFF_SIZE] = {5, LED_DURATION_FREQUENCY, 1, I2C_S,1, I2C_S};	   	//Send array/buff
extern u8  I2C_buff[DEFAULT_BUFF_SIZE];




/**
  * @brief  Slave entry point.
  * @retval None
  */
void slave_start(){
	
	LED_GPIO_Init();																				//Init Green LED GPIO --- PA5
	
	I2C_Slave_SDA_GPIO_Output_OD_Init();									  //Init SDA GPIO       --- PC4
	
	I2C_Slave_SCL_Falling_Exti_Enable();										//Enable SCL Falling exti
	
	param_assert();
  while(running_state == SLAVE)
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
  * @brief  Reset flag.
  * @retval None
  */
void flag_reset(){
	option = ret;
	bit_location = 0;
	receive_cnt = 0;
	a_bit_value = 0;
	receive_len = 0;
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
	u8 state = receive_buff[BASE_ADDR], offset = START_ADDR;
	if((receive_buff[BASE_ADDR] & LED_DURATION_FREQUENCY)){
		Date_To_I2CBuff(receive_buff[BASE_ADDR]);
	}
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
}



/**
  * @brief  This function is slave's falling exti callback.
  * @retval None
  */
void Slave_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	if(is_I2C_Slave_Start()){
		
		I2C_Slave_SCL_Rising_Exti_Enable();
		I2C_Slave_SCL_Falling_Exti_Disable();
		flag_reset();
		//HAL_UART_Transmit(&huart2, (u8 *)P_RECEIVED_START_SIGNAL, strlen(P_RECEIVED_START_SIGNAL),0xFF);
	}
}


/**
  * @brief  This function is slave's rising exti callback.
  * @retval None
  */
void Slave_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	/*receive first bit : its address and read/write bit.  */
	if(option == ret){
		bit_location++;
		if(bit_location <= BIT_LENGTH){
			a_bit_value <<= 1U;
			a_bit_value |= I2C_SDA_READ();
			
			if(bit_location == BIT_LENGTH){
				delay_us(I2C_PD);
				I2C_SDA_0();
			}
		}
		else{
 			if((a_bit_value & (I2C_ADDRESS + I2C_WRITE)) == a_bit_value){
				//HAL_UART_Transmit(&huart2, (u8 *)P_SLAVE_ADDR_CPMPARE_SUCCESS, strlen(P_SLAVE_ADDR_CPMPARE_SUCCESS),0xFF);
				//HAL_UART_Transmit(&huart2, (u8 *)P_SLAVE_IN_RECEIVING, strlen(P_SLAVE_IN_RECEIVING),0xFF);
				option = read;
			}
			else if((a_bit_value & (I2C_ADDRESS + I2C_READ)) == a_bit_value){
				//HAL_UART_Transmit(&huart2, (u8 *)P_SLAVE_ADDR_CPMPARE_SUCCESS, strlen(P_SLAVE_ADDR_CPMPARE_SUCCESS),0xFF);
				//HAL_UART_Transmit(&huart2, (u8 *)P_SLAVE_IN_SENDING, strlen(P_SLAVE_IN_SENDING),0xFF);
				option = write;
				if(receive_buff[BASE_ADDR] == LED_DURATION_FREQUENCY){
					receive_len = I2C_PARA_LENGTH;
				}
				else{
					receive_len = I2C_PARA_LENGTH - 2;
				}
				a_bit_value = I2C_buff[sent_cnt++];
			}
			else{
				//HAL_UART_Transmit(&huart2, (u8 *)P_SLAVE_ADDR_CPMPARE_FILED, strlen(P_SLAVE_ADDR_CPMPARE_FILED),0xFF);
				bit_location = 0;
				I2C_Slave_SCL_Falling_Exti_Reable();
				I2C_Slave_SCL_Rising_Exti_Disable();
				return;
			}
			I2C_Slave_SendAck();
			if(option == write){
				(a_bit_value & 0x80) ? (I2C_SDA_1()) : (I2C_SDA_0());
				a_bit_value <<= 1U;
			}
			bit_location = 0;
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
				delay_us(I2C_PD);
				if(receive_cnt  < receive_len){
					I2C_SDA_0();
				}
				else{
					I2C_SDA_1();
					
				}
			}
		}
		else{
			/*       Store this byte and wait acknowledge              */
			/*       How to store first byte                           */
			if(!receive_cnt){
				receive_len = a_bit_value;
			}
			
			/*       How to store next byte received                   */
			receive_buff[receive_cnt++] = a_bit_value;
			
			
			/*      Send acknowledge and reable/disable EXTI            */
			if(receive_cnt < receive_len){
				I2C_Slave_SendAck();
			}
			else{
				I2C_Slave_SendNAck();
				I2C_Slave_SCL_Rising_Exti_Disable();
				I2C_Slave_SCL_Falling_Exti_Reable();	
				//HAL_UART_Transmit(&huart2, (u8 *)P_SLAVE_END_RESIVING, strlen(P_SLAVE_END_RESIVING),0xFF);				
				param_assert();
			}
			bit_location = 0;
		}
	}
	/*   To be confirmed   																					*/
	else if(option == write){
		bit_location++;
		
		if(bit_location <= BIT_LENGTH){
			
			delay_us(I2C_PD);
			if(bit_location == BIT_LENGTH){
				I2C_SDA_1();
				return;
			}
			if((a_bit_value & 0x80) == I2C_LEVEL_HIGH){
				I2C_SDA_1();
			}
			else if((a_bit_value & 0x80) == I2C_LEVEL_LOW){
				I2C_SDA_0();
			}
			a_bit_value <<= 1U;
		}
		else{
			if(I2C_SDA_READ()){
				I2C_Slave_SCL_Falling_Exti_Reable();
				I2C_Slave_SCL_Rising_Exti_Disable();
				HAL_UART_Transmit(&huart2, (u8 *)P_SLAVE_END_SENDING, strlen(P_SLAVE_END_SENDING),0xFF);
				return;
			}
			if(sent_cnt <= receive_len){
				
				a_bit_value = I2C_buff[sent_cnt++];
				delay_us(I2C_PD);
				if((a_bit_value & 0x80) == I2C_LEVEL_HIGH){
					I2C_SDA_1();
				}
				else if((a_bit_value & 0x80) == I2C_LEVEL_LOW){
					I2C_SDA_0();
				}
				a_bit_value <<= 1U;
			}
			else{
				I2C_Slave_SCL_Falling_Exti_Reable();
				I2C_Slave_SCL_Rising_Exti_Disable();
			}
			bit_location = 0;
		}
	}
}


void Data_Transfer(u32 para, u8 offset){
	
	if((para / I2C_S_TO_US) <= 0xFF && (para / I2C_S_TO_US) >= 0x01 ){
		I2C_buff[offset + BASE_ADDR + TIME_OFFSET] = (u8)(para/I2C_S_TO_US);
		I2C_buff[offset + BASE_ADDR + UNITS_OFFSET] = (u8)'s';
	}
	else if((para / I2C_MS_TO_US) <= 0xFF  && (para / I2C_S_TO_US) >= 0x01 ){
		I2C_buff[offset + BASE_ADDR + TIME_OFFSET] = (u8)(para/I2C_MS_TO_US);
		I2C_buff[offset + BASE_ADDR + UNITS_OFFSET] = (u8)'m';
	}
	else{
		I2C_buff[offset + BASE_ADDR + TIME_OFFSET] = (u8)(para/I2C_MS_TO_US);
		I2C_buff[offset + BASE_ADDR + UNITS_OFFSET] = (u8)'u';
	}
}

/**
  * @brief  This function is get LED duration and frequency 
	* 				when running state is slave but receive a get command.
  * @retval None
  */
u8 Date_To_I2CBuff(u8 state){
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

