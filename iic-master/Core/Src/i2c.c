#include "i2c.h"


void I2C_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();																		 
	
	/*Configure I2C GPIO pins : SDA -- PC4 */
  GPIO_InitStruct.Pin = I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);									//Init SDA
	HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET);			//Set SDA = high level 
	
	
	/*Configure I2C GPIO pins : SCL -- PC5 */
	GPIO_InitStruct.Pin = I2C_SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(I2C_SCL_PORT, &GPIO_InitStruct);               		//Init SCL
	HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET);			//SET SCL = high level
}




/**
* @brief sleep function
* @param us: sleep time(microsecond)
* @retval void
*/
void delay_us(uint32_t us)
{
	__IO int i = us * 5;
	while(i--);
}


/**
* @brief sleep function
* @param us: sleep time(millisecond)
* @retval void
*/
void delay_ms(uint32_t ms)
{
	while(ms--) {
		delay_us(909);
	}
}


/**
* @brief Start signal function
* @retval void
*/
void i2c_Start(){
	I2C_SCL_1();
	I2C_SDA_1();
	delay_us(I2C_PD);
	I2C_SDA_0();
	delay_us(I2C_PD);
}

/**
* @brief Start stop function
* @retval void
*/
void i2c_Stop(){
	I2C_SDA_0();
	delay_us(I2C_PD);
	I2C_SCL_1();
	I2C_SDA_1();
}

/**
* @brief Send a acknowledge
* @retval void
*/
void i2c_SendAck(void){
	I2C_SDA_0();
	I2C_SCL_1();
	delay_us(I2C_PD);
	I2C_SCL_0();
	I2C_SDA_1();
}

/**
* @brief Send a not acknowledge
* @retval void
*/
void i2c_SendNAck(void){
	I2C_SDA_1();
	I2C_SCL_1();
	delay_us(I2C_PD);
	I2C_SCL_0();
}

/**
* @brief wiat slave or master return ack
* @param re_value: A ack or a Nack
* @retval uint8_t:acknowledge
*/
u8 i2c_WaitAck(){
	u8 re_value;
	I2C_SDA_PORT->MODER &= 0xFFFCFFFF;
	I2C_SDA_1();
	I2C_SCL_1();
	delay_us(I2C_PD);
	re_value = I2C_SDA_READ();
	I2C_SCL_0();
	I2C_SDA_0();
	I2C_SDA_PORT->MODER |= 0x00010000;
	return re_value;
}

/**
* @brief send a byte
* @param i: Control loop variable
* @retval void
*/
void I2C_SendByte(u8 data_byte){
	__IO u8 i = 0, j = 0;
	I2C_SCL_0();
	delay_us(I2C_PD);
	for(i = 0; i < 8; ++i){
		//
		if((data_byte) & 0x80)
		{
			I2C_SDA_1();
		}
		else{
			I2C_SDA_0();
		}
		I2C_SCL_1();
		delay_us(I2C_PD);
		I2C_SCL_0();
		I2C_SDA_0();
		data_byte <<= 1U;
		delay_us(I2C_PD);
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
u8 I2C_ReadByte(){
	__IO u8 i, value = 0;
	for(i = 0; i < 8; ++i){
		value <<= 1U;
		I2C_SCL_1();
		delay_us(I2C_PD);
		value |= I2C_SDA_READ();
		I2C_SCL_0();
	}
	return value;
}

/**
* @brief Send an amount of data
* @param pdata: A pointer to outgoing data
* @param len: Length of data to be sent
* @retval uint32_t: It may be modified
*/
u32 I2C_Write(u8 slave_addr, u8 *data, u32 data_length){
	u8 *pdata = data;
	u32 len = data_length;
	
	//Send start signal
	i2c_Start();
	
	//Send slave address and w/r bit
	I2C_SendByte(slave_addr);
	
	//Wait and analyze for acknowledge
	while(len){
		--len;
		if(!i2c_WaitAck()){
			I2C_SendByte(pdata[len]);
			//
		}
		else{
			break;
		}
	}
	
	//i2c_WaitAck();
	//Send end signal
	i2c_Stop();		
	return 0;
}

/**
* @brief Read an amount of data
* @retval uint32_t: It may be modified
*/
u32 I2C_Read(u8 slave_addr, u8 *buff, u8 numByteToRead){
	u8 *pdata = buff;
	u32 len = numByteToRead;
	
	//Send start signal
	i2c_Start();
	
	//Send slave address and w/r bit
	I2C_SendByte(slave_addr);
	
	//Wait and analyze for acknowledge
	if(i2c_WaitAck()){
		while(len){
			pdata[--len] = I2C_ReadByte();
			if(!len){
				i2c_SendNAck();
				break;
			}
			i2c_SendAck();
		}
	}
	//Send end signal
	i2c_Stop();		
	return 0;
}


//
u8 UART_Process_Param(u8 *Buff, u8 length){
	u8 *buff_point = Buff;
	__IO u8 buff_length = length, re_value = 0U;
	if(buff_point[2] == 0xFF && buff_length != 5){
		return param_error;
	}
	if((buff_point[2] == 0xF0 || buff_point[2] == 0x0F) && buff_length != 4){
		return param_error;
	}
	if(buff_point[0] == Slave_Get){
		//I2C_Read();
	}
	else if(buff_point[0] == Slave_Set){
		I2C_Write(buff_point[1], buff_point + 2, (buff_point[2] == 0xFF)?(2):(1));
		//re_value = I2C_Read(buff_point[1] + 1U, );
	}
	else{
	 return param_error;
	}
}


/**
*test function
**/
void test1(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};																	 
	
	/*Configure I2C GPIO pins : SDA -- PC4 */
  GPIO_InitStruct.Pin = I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);									//Init SDA
}
void test2(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};																	 
	
	/*Configure I2C GPIO pins : SDA -- PC4 */
  GPIO_InitStruct.Pin = I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);									//Init SDA
}

void LED(u32 up_time){
	LED_ON;
	delay_us(up_time);
	LED_OFF;
}
