#include "i2c.h"


u32 buff_size = 10U;


//the time delay function

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
u8 is_i2c_Start(){
	//LED(2);
	if(!I2C_SDA_READ()){
		//LED(2);
		return GPIO_PIN_SET;
	}
	return GPIO_PIN_RESET;
}

/**
* @brief Determine if the signal is a stop signal
* @retval u8, GPIO_PIN_SET(high level) or GPIO_PIN_RESET(low level)
*/
u8 is_i2c_Stop(){
	if(I2C_SCL_READ()){
		return GPIO_PIN_SET;
	}
	return GPIO_PIN_RESET;
}

/**
* @brief Send a Ack
* @retval void
*/
void i2c_SendAck(void){
	I2C_SDA_0();
	delay_us(2);
}

/**
* @brief Send a NAck
* @retval void
*/
void i2c_SendNAck(void){
	I2C_SDA_1();
	delay_us(2);
	I2C_SDA_0();
}

/**
* @brief Wait a Ack
* @retval u8: received a ack from SDA
*/
u8 i2c_WaitAck(){
	__IO u8 re_value = 1U;
	if(I2C_SCL_READ()){
		re_value = I2C_SDA_READ();
		delay_us(2);
	}
	return re_value;
}


/**
* @brief send a byte
* @param i: Control loop variable
* @retval void
*/
void I2C_SendByte(u8 *data_byte){
	__IO u8 i = 0, j = 0;
	for(i = 0; i < 8; ++i){
		((*data_byte) & 0x80)? (I2C_SDA_1()):(I2C_SDA_0());
		delay_us(2);
		I2C_SDA_0();
		*data_byte <<= 1U;
	}
}


/**
* @brief read a byte
* @param i: Control loop variable
* @retval uint8_t: received byte from SDA
*/
u8 I2C_ReadByte(){
	__IO u8 i, value = 0, j;
	for(i = 0; i < 8; ++i){
		value <<= 1U;
		while(1){
			if(I2C_SCL_READ()){
			  value |= I2C_SDA_READ();
				//LED(0);
			  break;
			}
		}
		delay_us(2);
	} 
	return value;
}


/**
* @brief Send an amount of data
* @param pdata: A pointer to outgoing data
* @param len: Length of data to be sent
* @retval uint32_t: It may be modified
*/
u32 I2C_Write(u8 *data, u32 data_length){
	u8 *pdata = data;
	__IO u32 len = data_length;
	while(len--){
		I2C_SendByte(&pdata[len]);
		if(!i2c_WaitAck()){
			continue;
		}
		else{
			break;
		}
	}
	return 0;
}

/**
* @brief Read an amount of data
* @retval uint32_t: It may be modified
*/
u32 I2C_Read(){
	u8 *rdata = (u8 *)malloc(sizeof(u8) * buff_size);
	u8 wdata[] = {0, 1, 2, 3};
	u32 i = 0;
	__IO u8 slave_addr = I2C_ReadByte();
	if(slave_addr == self_addr_write){
		i2c_SendAck();
		I2C_Write(wdata, 4);
	}
	else if(slave_addr == self_addr_read){
		i2c_SendAck();
		
	}
	free(rdata);
	i2c_slave_SCL_Falling_Exti_Enable();
	return 0;
}

/**
* @brief Enable SCL Falling exti
* @retval void
*/
void i2c_slave_SCL_Falling_Exti_Enable(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure I2C GPIO pins : SCL -- PC5 */
  GPIO_InitStruct.Pin = I2C_SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(I2C_SCL_PORT, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);								//Set Priority
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);												//Enable EXTI 4-15
}


/**
* @brief Disable SCL Falling exti
* @retval void
*/
void i2c_slave_SCL_Falling_Exti_Disable(){

	HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
	
}

void i2c_slave_SDA_GPIO_Input_Init(){
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	// Configure I2C GPIO pins : SDA - PC4
  GPIO_InitStruct.Pin = I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);
}


////Test function
void LED(u32 up_time){
	LED_ON;
	delay_us(up_time);
	LED_OFF;
}
void callback(){
	LED_ON;
	delay_us(2);
	LED_OFF;
}

