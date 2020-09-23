#include "i2c.h"


__IO u8 bit_register = 2;

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
	if(!I2C_SDA_READ()){
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
	delay_us(I2C_PD);
	I2C_SDA_1();
}

/**
* @brief Send a NAck
* @retval void
*/
void i2c_SendNAck(void){
	I2C_SDA_1();
	delay_us(I2C_PD);
}

/**
* @brief Wait a Ack
* @retval u8: received a ack from SDA
*/
u8 i2c_WaitAck(){
	__IO u8 re_value = 1U;
	if(I2C_SCL_READ()){
		re_value = I2C_SDA_READ();
		delay_us(I2C_PD);
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
//		if(I2C_SCL_READ()){
//		((*data_byte) & 0x80)? (I2C_SDA_1()):(I2C_SDA_0());
//		}
//		delay_us(I2C_PD);
//		I2C_SDA_0();
//		*data_byte <<= 1U;
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
		delay_us(I2C_PD);
		value <<= 1U;
		while(1){
			if(I2C_SCL_READ()){
			  value |= I2C_SDA_READ();
			 break;
			}
		}
		delay_us(I2C_PD);
	}
	return value;
}


/**
* @brief Send an amount of data
* @param pdata: A pointer to outgoing data
* @param len: Length of data to be sent
* @retval uint32_t: It may be modified
*/
void I2C_Write(u8 *data, u32 data_length){
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
}

/**
* @brief Read an amount of data
* @retval uint32_t: It may be modified
*/
void I2C_Read(){
	u8 *rdata = (u8 *)malloc(sizeof(u8) * DEFAULT_BUFF_SIZE);
	u8 wdata[] = {4, 5, 6, 7, 8, 9};
	u32 i = 6;
	__IO u8 slave_addr = I2C_ReadByte();
	if(rdata == NULL){
		return;
	}
	if(slave_addr == self_addr_write){
		i2c_SendAck();
		I2C_Write(wdata, 2);
	}
	else if(slave_addr == self_addr_read){
		i2c_SendAck();
		while(i){
			rdata[--i] = I2C_ReadByte();
			if(i == 0){
				i2c_SendNAck();
			}
			i2c_SendAck();
		}
	}
	free(rdata);
	i2c_slave_SCL_Falling_Exti_Enable();
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
	
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);								//Set Priority
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);												//Enable EXTI 4-15
}


/**
* @brief Reable SCL Falling exti
* @retval void
*/
void i2c_slave_SCL_Falling_Exti_Reable(){
	EXTI->FTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR;
}

/**
* @brief Disable SCL Falling exti
* @retval void
*/
void i2c_slave_SCL_Falling_Exti_Disable(){
	EXTI->FTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR;
}


/**
* @brief Enable SCL Rising exti
* @retval void
*/
void i2c_slave_SCL_Rising_Exti_Enable(){
	EXTI->RTSR1 |= I2C_SCL_EXTI_ENABLE_ADDR;
}

/**
* @brief Disable SCL Falling exti
* @retval void
*/
void i2c_slave_SCL_Rising_Exti_Disable(){
	EXTI->RTSR1 &= I2C_SCL_EXTI_DISABLE_ADDR;
}


/**
* @brief Init I2C GPIO  ---  SDA - PC4
* @retval void
*/
void i2c_slave_SDA_GPIO_Input_Init(){
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	// Configure I2C GPIO pins : SDA - PC4
  GPIO_InitStruct.Pin = I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);
	I2C_SDA_1();
	//I2C_SDA_PORT->MODER &= 0xFFFFFCFF;
}


void i2c_ReadBit(){
	
}
void i2c_SenddBit(void);

////Test function
void LED(u32 up_time){
	LED_ON;
	delay_us(up_time);
	LED_OFF;
}
void callback(){
	LED_ON;
	delay_us(I2C_PD);
	LED_OFF;
}

