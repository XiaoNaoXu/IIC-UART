#include "slave.h"


#define receive_buff sent_buff    														// Receive and send use the same array

__IO u8    bit_location = 0U;        													//Records the location of the bits received
__IO u8    a_bit_value = 0U;																	//Store a full byte received
__IO u8    receive_cnt = 0;																		//The number that has been sent
__IO u8    receive_len = 3;															   		//The number when need to send
__IO u8    sent_cnt = 0;												      				//The number that has been received
Option     option = ret;												      				//Control read/write state
__IO u32   led_frequency = 0;									        				//The frequency of the led
__IO u32   led_duration = 0;										        			//The duration of the led


u8         sent_buff[DEFAULT_BUFF_SIZE] = {0xFF,1,0,1,0};	   	//Send array/buff
extern u8  I2C_buff[DEFAULT_BUFF_SIZE];




/* */
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
  * @brief  This function is falling exti callback.
  * @retval None
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	if(is_I2C_Slave_Start() && GPIO_Pin == I2C_SCL_PIN){
		
		I2C_Slave_SCL_Rising_Exti_Enable();
		I2C_Slave_SCL_Falling_Exti_Disable();
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
 			if((a_bit_value & (I2C_ADDRESS + I2C_WRITE)) == a_bit_value){
				option = read;
			}
			else if((a_bit_value & (I2C_ADDRESS + I2C_READ)) == a_bit_value){
				option = write;
				a_bit_value = sent_buff[++sent_cnt];
			}
			else{
				bit_location = 0;
				I2C_Slave_SCL_Falling_Exti_Reable();
				I2C_Slave_SCL_Rising_Exti_Disable();
				return;
			}
			I2C_Slave_SendAck();
			bit_location = 0;
		}
	}
		
	/*    		Receive data   						 	      	  */
	else if(option == read){
		
		/* 			Receive a byte  		         	      	*/
		bit_location++;
		if(bit_location < BIT_LENGTH + 1){
			a_bit_value <<= 1U;
			a_bit_value |= I2C_SDA_READ();
		}
		else{
			/*       Store this byte and wait acknowledge              */
			/*       How to store first byte                           */
			if(!receive_cnt){
				receive_buff[base_addr] = a_bit_value;
			}
			
			/*       How to store next byte received                   */
			else{
				
				/*     This byte use for duration only                   */
				if(receive_buff[base_addr] == LED_duration){
					receive_buff[base_addr + receive_cnt] = a_bit_value;
					receive_len = I2C_PARA_LENGTH - 2;
				}
				
				/*     This byte use for frequency only                  */
				else if(receive_buff[base_addr] == LED_frequency){
					receive_buff[base_addr + receive_cnt + addr_offet] = a_bit_value;
					receive_len = I2C_PARA_LENGTH - 2;
				}
				
				/*    This byte use for duration/frequency only          */
				else{
					receive_buff[receive_cnt] = a_bit_value;
					receive_len = I2C_PARA_LENGTH;
				}
			}
			receive_cnt++;
			
			/*      Send acknowledge and reable/disable EXTI            */
			if(receive_cnt < receive_len){
				I2C_Slave_SendAck();
			}
			else{
				I2C_Slave_SendNAck();
				I2C_Slave_SCL_Rising_Exti_Disable();
				I2C_Slave_SCL_Falling_Exti_Reable();				
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
				I2C_Slave_SCL_Falling_Exti_Reable();
				I2C_Slave_SCL_Rising_Exti_Disable();
				return;
			}
			if(sent_cnt < I2C_PARA_LENGTH){
				a_bit_value = sent_buff[++sent_cnt];
			}
			else{
				I2C_Slave_SCL_Falling_Exti_Reable();
				I2C_Slave_SCL_Rising_Exti_Disable();
			}
			bit_location = 0;
		}
	}
}


/**
  * @brief  This function is rising exti callback.
  * @retval None
  */
u8 Date_To_I2CBuff(){
	if((led_duration / I2C_S_TO_US) >= 0xFF ){
		I2C_buff[0] = (u8)(led_duration/I2C_S_TO_US);
		I2C_buff[1] = (u8)'s';
	}
	else if((led_duration / I2C_MS_TO_US) >= 0xFF){
		I2C_buff[0] = (u8)(led_duration/I2C_MS_TO_US);
		I2C_buff[1] = (u8)'m';
	}
	else{
		I2C_buff[0] = (u8)(led_duration/I2C_MS_TO_US);
		I2C_buff[1] = (u8)'u';
	}
	if((led_frequency / I2C_S_TO_US) >= 0xFF ){
		I2C_buff[2] = (u8)(led_frequency/I2C_S_TO_US);
		I2C_buff[3] = (u8)'s';
	}
	else if((led_frequency / I2C_MS_TO_US) >= 0xFF){
		I2C_buff[2] = (u8)(led_frequency/I2C_MS_TO_US);
		I2C_buff[3] = (u8)'m';
	}
	else{
		I2C_buff[2] = (u8)(led_frequency/I2C_MS_TO_US);
		I2C_buff[3] = (u8)'u';
	}
	return PROCESS_SUCCESS;
}

