/**
  ******************************************************************************
  * @file           : i2c.c
  * @brief          : I2C related code
  */

#include "i2c.h"


I2C_TYPE 		 I2C_Bus_state							 			   =  I2C_BUS_IDLE;
I2C_TYPE 		 I2C_buff[DEFAULT_BUFF_SIZE] 		     =  {0};
I2C_TYPE 		 I2C_receive_buff[DEFAULT_BUFF_SIZE] =  {0};
I2C_TYPE 		 UART_DATA_REG											 = 	NO_UART_DATA;



/**************************************************************************/
/***********                    LED              **************************/
/**************************************************************************/


/**
	* @brief  Control function 
	* @retval void
	*/
void LED(u32 up_time){
	LED_ON;
	delay_us(up_time);
	LED_OFF;
}


/**
	* @brief Initialize GPIO of the LED
	* @retval void
	*/
void LED_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();					//GPIOA Ports Clock Enable


  /*Configure GPIO pin : LED - PA5 */
  GPIO_InitStruct.Pin    =   LED_GREEN_Pin;
  GPIO_InitStruct.Mode   =   GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull   =   GPIO_NOPULL;
  GPIO_InitStruct.Speed  =   GPIO_SPEED_FREQ_VERY_HIGH;
	
  HAL_GPIO_Init(LED_GREEN_GPIO_Port, &GPIO_InitStruct);
	
}







/**************************************************************************/
/***********                    GPIO              ************************/
/**************************************************************************/


/////////**
////////	* @brief 	Set SCL to high level and busy waiting
////////	* @retval void
////////	*/
////////void I2C_SCL_1(){
////////	I2C_SCL_S_1();
////////	while(I2C_SCL_READ() != I2C_LEVEL_HIGH);
////////}








/**************************************************************************/
/***********                    SLAVE              ************************/
/**************************************************************************/


/**
	* @brief Sleep function, the units is microsecond
	* @param us: sleep time(microsecond)
	* @retval void
	*/
void delay_us(uint32_t us)
{
	__IO int i = us * 5;
	while(i--);
}

/**
	* @brief Sleep function, the units is millisecond
	* @param[in] us: sleep time(millisecond)
	* @retval void
	*/
void delay_ms(uint32_t ms)
{
	while(ms--) {
		delay_us(909);
	}
}

/**
	* @brief Determine if the signal is a start signal
	* @retval I2C_TYPE, GPIO_PIN_SET(high level) or GPIO_PIN_RESET(low level)
	*/
I2C_TYPE is_I2C_Slave_Start(){
	if(I2C_SCL_READ()){
		return GPIO_PIN_SET;
	}
	return GPIO_PIN_RESET;
}

/**
	* @brief Determine if the signal is a stop signal
	* @retval I2C_TYPE, GPIO_PIN_SET(high level) or GPIO_PIN_RESET(low level)
	*/
I2C_TYPE is_I2C_Slave_Stop(){
	if(I2C_SCL_READ()){
		return GPIO_PIN_SET;
	}
	return GPIO_PIN_RESET;
}


/**
	* @brief Wait a Ack
	* @retval I2C_TYPE: received a ack from SDA
	*/
I2C_TYPE I2C_Slave_WaitAck(){
	__IO I2C_TYPE re_value = 1U;
	if(I2C_SCL_READ()){
		re_value = I2C_SDA_READ();
		delay_us(I2C_PD);
	}
	return re_value;
}

/**
	* @brief Enable SCL Falling exti ----   SCL - PC5
	* @retval void
	*/
void I2C_Slave_SCL_Falling_Exti_Enable(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure I2C GPIO pins : SCL -- PC5 */
  GPIO_InitStruct.Pin    =  SLAVE_EXTI_PIN;
  GPIO_InitStruct.Mode   =  GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull   =  GPIO_NOPULL;
  GPIO_InitStruct.Speed  =  GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SLAVE_EXTI_PORT, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);								//Set Priority
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);												//Enable EXTI 4-15
}


/**
	* @brief Reable SCL Falling exti
	* @retval void
	*/
void I2C_Slave_SCL_Falling_Exti_Reable(){
	delay_us(I2C_PD);
	EXTI->FTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR;
}

/**
	* @brief Disable SCL Falling exti
	* @retval void
	*/
void I2C_Slave_SCL_Falling_Exti_Disable(){
	EXTI->FTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR;
}


/**
	* @brief Enable SCL Rising exti
	* @retval void
	*/
void I2C_Slave_SCL_Rising_Exti_Enable(){
	EXTI->RTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR;
}

/**
	* @brief Disable SCL Falling exti
	* @retval void
	*/
void I2C_Slave_SCL_Rising_Exti_Disable(){
	EXTI->RTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR;
}


/**
	* @brief Init I2C GPIO  ---  SDA - PC4
	* @retval void
	*/
void I2C_Slave_SDA_GPIO_Output_OD_Init(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();														//GPIOC Ports Clock Enable
	
	// Configure I2C GPIO pins : SDA - PC4
  GPIO_InitStruct.Pin    =  I2C_SDA_PIN;
  GPIO_InitStruct.Mode   =  GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull   =  GPIO_NOPULL;
  GPIO_InitStruct.Speed  =  GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);
	I2C_SDA_1();

}




/**************************************************************************/
/***********                    MASTER              ***********************/
/**************************************************************************/


/**
	* @biref  I2C SDA Falling EXTI Init  -----    SDA - PC4
	* @retval void
	*/
void I2C_Master_SDA_Output_OD_Init(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure I2C GPIO pins : SDA - PC4 */
  GPIO_InitStruct.Pin		 = 	MASTER_EXTI_PIN;
  GPIO_InitStruct.Mode	 = 	GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull	 = 	GPIO_PULLUP;
	GPIO_InitStruct.Speed  =  GPIO_SPEED_FREQ_VERY_HIGH;
	
  HAL_GPIO_Init(MASTER_EXTI_PORT, &GPIO_InitStruct);
	I2C_SDA_1();

}



/**
	* @brief Start signal function
	* @retval void
	*/
void I2C_Master_Start(){
	I2C_SCL_1();
	I2C_SDA_1();
	delay_us(I2C_PD);
	I2C_SDA_0();
}

/**
	* @brief Start stop function
	* @retval void
	*/
void I2C_Master_Stop(){
	I2C_SCL_0();
	I2C_SDA_0();
	delay_us(I2C_PD);
	I2C_SCL_1();
	I2C_SDA_1();
}

/**
	* @brief Send a acknowledge
	* @retval void
	*/
void I2C_Master_SendAck(void){
	I2C_SCL_0();
	I2C_SDA_0();
	delay_us(I2C_PD);
	I2C_SCL_1();
	delay_us(I2C_PD);
	I2C_SDA_1();
	I2C_SCL_0();
}

/**
	* @brief Send a not acknowledge
	* @retval void
	*/
void I2C_Master_SendNAck(void){
	I2C_SCL_0();
	delay_us(I2C_PD);
	I2C_SDA_1();
	I2C_SCL_1();
	delay_us(I2C_PD);
}

/**
	* @brief wiat slave or master return ack
	* @param re_value: A ack or a Nack
	* @retval uint8_t:acknowledge
	*/
I2C_TYPE I2C_Master_WaitAck(){
	I2C_TYPE re_value;
	I2C_SCL_0();
	I2C_SDA_1();
	//I2C_SDA_0();	
	delay_us(I2C_PD);
	I2C_SCL_1();
	delay_us(I2C_PD);
	re_value = I2C_SDA_READ();
	return re_value;
}


/**
	* @brief send a byte
	* @param i: Control loop variable
	* @retval void
	*/
I2C_TYPE I2C_Master_SendByte(I2C_TYPE data_byte){
	__IO I2C_TYPE i = 0, temp = 0;
	for(i = 0; i < BIT_LENGTH; ++i){
		I2C_SCL_0();	
		if((data_byte) & 0x80)
		{
			I2C_SDA_1();
			if(I2C_SDA_READ() != I2C_LEVEL_HIGH){
				return I2C_BUS_BUSY;
			}
		}
		else{
			I2C_SDA_0();
		}
		delay_us(I2C_PD);	
		I2C_SCL_1();
		delay_us(I2C_PD);
		data_byte <<= 1U;
	}
	return I2C_BUS_IDLE;
	
}


/**
	* @brief read a byte
	* @param i: Control loop variable
	* @retval uint8_t: received byte from SDA
	*/
I2C_TYPE I2C_Master_ReadByte(){
	__IO I2C_TYPE i, value = 0;
	for(i = 0; i < BIT_LENGTH; ++i){
		I2C_SCL_0();
		delay_us(I2C_PD);
		value <<= 1U;
		I2C_SCL_1();
		delay_us(I2C_PD);
		value |= I2C_SDA_READ();
	}
	return value;
}


/**
	* @brief Send an amount of data
	* @param pdata: A pointer to outgoing data
	* @param len: Length of data to be sent
	* @retval uint32_t: It may be modified
	*/
u32 I2C_Master_Write(I2C_TYPE slave_addr, I2C_TYPE *data){
	I2C_TYPE *pdata 			 = 			data;
	u32 len					 = 			0;
	
	/*       I2C SDA Init               */
	if(I2C_Bus_state == I2C_BUS_BUSY ){
		return I2C_BUS_BUSY;
	}
	
	I2C_SCL_Falling_Disable();
	//Send start signal
	I2C_Master_Start();
	//while(1);
	
	//Send slave address and w/r bit
	if(I2C_BUS_BUSY == I2C_Master_SendByte(slave_addr)){
		return I2C_BUS_BUSY;
	}
	
	//Wait and analyze for acknowledge
	while(len < data[START_ADDR] + 1){
		if(!I2C_Master_WaitAck()){
			if(I2C_BUS_BUSY == I2C_Master_SendByte(pdata[len++])){
				return I2C_BUS_BUSY;
			}
		}
		else{
			break;
		}
	}
	
	I2C_Master_Stop();
	I2C_SCL_Falling_Enable();
	
	return I2C_BUS_IDLE;
}

/**
	* @brief Read an amount of data
	* @retval uint32_t: It may be modified
	*/
u32 I2C_Master_Read(I2C_TYPE slave_addr, I2C_TYPE *buff){
	I2C_TYPE *pdata         =   buff;
	u32 len           =   0;
	I2C_TYPE numByteToRead  =   0;
	
	/*       I2C SDA Init               */
	if(I2C_Bus_state == I2C_BUS_BUSY ){
		return I2C_BUS_BUSY;
	}
	I2C_SCL_Falling_Disable();
	//Send start signal
	I2C_Master_Start();
	
	//Send slave address and w/r bit
	if(I2C_BUS_BUSY == I2C_Master_SendByte(slave_addr)){
		return I2C_BUS_BUSY;
	}
	
	//Wait and analyze for acknowledge
	if(!I2C_Master_WaitAck()){
		numByteToRead = I2C_Master_ReadByte() + 1;
		I2C_Master_SendAck();
		pdata[len++] = numByteToRead - 1;
		while(len <= numByteToRead){
			pdata[len++] = I2C_Master_ReadByte();
			if(len == numByteToRead){
				I2C_Master_SendNAck();
				break;
			}
			else{
				I2C_Master_SendAck();
			}
		}
	}
	
	//Send end signal
	I2C_Master_Stop();
	I2C_SCL_Falling_Enable();
	
	return I2C_BUS_IDLE;
}









/**************************************************************************/
/***********           BOTH MASTER AND SLAVE        ***********************/
/**************************************************************************/


/**
	* @brief Init I2C GPIO  ---   SDA - PC4
					 Init I2C GPIO  ---   SCL - PC5
	* @retval void
	*/
void I2C_GPIO_Init(void){
	__HAL_RCC_GPIOC_CLK_ENABLE();																							//Enable RCC GPIOC CLOCK
	GPIO_InitTypeDef GPIO_InitStruct = {0};																	 
	
	/*Configure I2C GPIO pins : SDA - PC4,  SCL -- PC5  */
  GPIO_InitStruct.Pin		 = 	I2C_SDA_PIN | I2C_SCL_PIN;
  GPIO_InitStruct.Mode	 = 	GPIO_MODE_OUTPUT_OD  | GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull	 = 	GPIO_NOPULL;
	GPIO_InitStruct.Speed  =  GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);														// Init SDA
	I2C_SDA_Falling_Rising_Disable();																					// Disable SDA EXTI Falling and Rising
	I2C_SCL_Rising_Disable();
	I2C_SDA_1();																															// SET SDA = high level
	I2C_SCL_1();																														//SET SCL = high level
		
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 5, 0);																//Set Priority
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);																				//Enable EXTI 4-15
}










/**************************************************************************/
/***********           Parameter Process      	    ***********************/
/**************************************************************************/


/**
  * @brief  This function is combining a string
	* @retval uint32_t rx: String length
  */
I2C_TYPE I2C_To_UART(I2C_TYPE *I2C_ptr){
	__IO I2C_TYPE i = 0, temp, rx = 0, div = '1';
	__IO I2C_TYPE data_len = I2C_ptr[START_ADDR], reg = I2C_ptr[BASE_ADDR];

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





