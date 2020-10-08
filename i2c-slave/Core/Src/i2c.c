#include "i2c.h"



u8 I2C_buff[DEFAULT_BUFF_SIZE] = {0};

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
  GPIO_InitStruct.Pin = LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LED_GREEN_GPIO_Port, &GPIO_InitStruct);
	
}



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
* @retval u8, GPIO_PIN_SET(high level) or GPIO_PIN_RESET(low level)
*/
u8 is_I2C_Slave_Start(){
	if(!I2C_SDA_READ()){
		return GPIO_PIN_SET;
	}
	return GPIO_PIN_RESET;
}

/**
* @brief Determine if the signal is a stop signal
* @retval u8, GPIO_PIN_SET(high level) or GPIO_PIN_RESET(low level)
*/
u8 is_I2C_Slave_Stop(){
	if(I2C_SCL_READ()){
		return GPIO_PIN_SET;
	}
	return GPIO_PIN_RESET;
}


/**
* @brief Send a Ack
* @retval void
*/
void I2C_Slave_SendAck(void){
	I2C_SDA_0();
	delay_us(I2C_PD);
	I2C_SDA_1();
}

/**
* @brief Send a NAck
* @retval void
*/
void I2C_Slave_SendNAck(void){
	I2C_SDA_1();
	delay_us(I2C_PD);
}

/**
* @brief Wait a Ack
* @retval u8: received a ack from SDA
*/
u8 I2C_Slave_WaitAck(){
	__IO u8 re_value = 1U;
	if(I2C_SCL_READ()){
		re_value = I2C_SDA_READ();
		delay_us(I2C_PD);
	}
	return re_value;
}

/**
* @brief Enable SCL Falling exti
* @retval void
*/
void I2C_Slave_SCL_Falling_Exti_Enable(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure I2C GPIO pins : SCL -- PC5 */
  GPIO_InitStruct.Pin = I2C_SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(I2C_SCL_PORT, &GPIO_InitStruct);
	
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
  GPIO_InitStruct.Pin = I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);
	I2C_SDA_1();

}




/**************************************************************************/
/***********                    MASTER              ***********************/
/**************************************************************************/


/**
* @brief Init I2C GPIO  ---  SDA - PC4 , SCL - PC5
* @retval void
*/
void I2C_Master_GPIO_Output_OD_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();																		 
	
	/*Configure I2C GPIO pins : SDA -- PC4 */
  GPIO_InitStruct.Pin = I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);									//Init SDA
	HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET);			//Set SDA = high level 
	
	
	/*Configure I2C GPIO pins : SCL -- PC5 */
	GPIO_InitStruct.Pin = I2C_SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(I2C_SCL_PORT, &GPIO_InitStruct);               		//Init SCL
	HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET);			//SET SCL = high level
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
	delay_us(I2C_PD);
	I2C_SCL_1();
	I2C_SDA_0();
	delay_us(I2C_PD);
	I2C_SDA_1();
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
u8 I2C_Master_WaitAck(){
	u8 re_value;
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
void I2C_Master_SendByte(u8 data_byte){
	__IO u8 i = 0;
	for(i = 0; i < BIT_LENGTH; ++i){
		I2C_SCL_0();
		delay_us(I2C_PD);		
		if((data_byte) & 0x80)
		{
			I2C_SDA_1();
		}
		else{
			I2C_SDA_0();
		}
		I2C_SCL_1();
		delay_us(I2C_PD);
		data_byte <<= 1U;
	}
	
//	SCL_L()
//	sda_L/H
//	delay_ms()
//	SCL_H()
//	delay_ms()
	
}


/**
* @brief read a byte
* @param i: Control loop variable
* @retval uint8_t: received byte from SDA
*/
u8 I2C_Master_ReadByte(){
	__IO u8 i, value = 0;
	for(i = 0; i < BIT_LENGTH; ++i){
		I2C_SCL_0();
		//I2C_SDA_0();
		
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
u32 I2C_Master_Write(u8 slave_addr, u8 *data, u32 data_length){
	u8 *pdata = data;
	u32 len = 0;
	
	//Send start signal
	I2C_Master_Start();
	
	//Send slave address and w/r bit
	I2C_Master_SendByte(slave_addr);
	
	//Wait and analyze for acknowledge
	while(len < data_length){
		if(!I2C_Master_WaitAck()){
			I2C_Master_SendByte(pdata[len++]);
		}
		else{
			break;
		}
	}
	I2C_Master_Stop();		
	return 0;
}

/**
* @brief Read an amount of data
* @retval uint32_t: It may be modified
*/
u32 I2C_Master_Read(u8 slave_addr, u8 *buff, u8 numByteToRead){
	u8 *pdata = buff;
	u32 len = 0;
	
	//Send start signal
	I2C_Master_Start();
	
	//Send slave address and w/r bit
	I2C_Master_SendByte(slave_addr);
	
	//Wait and analyze for acknowledge
	if(!I2C_Master_WaitAck()){
		while(len < numByteToRead){
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
	return 0;
}

