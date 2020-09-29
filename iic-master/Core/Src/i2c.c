#include "i2c.h"


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
}

/**
* @brief Start stop function
* @retval void
*/
void i2c_Stop(){
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
void i2c_SendAck(void){
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
void i2c_SendNAck(void){
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
u8 i2c_WaitAck(){
	u8 re_value;
	I2C_SCL_0();
	//I2C_SDA_0();	
	delay_us(I2C_PD);
	I2C_SDA_1();
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
void I2C_SendByte(u8 data_byte){
	__IO u8 i = 0;
	for(i = 0; i < BIT_LENGTH; ++i){
		I2C_SCL_0();
		//I2C_SDA_0();
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
u8 I2C_ReadByte(){
	__IO u8 i, value = 0;
	for(i = 0; i < BIT_LENGTH; ++i){
		I2C_SCL_0();
		//I2C_SDA_0();
		delay_us(I2C_PD);
		value <<= 1U;
		I2C_SCL_1();
		I2C_SDA_1();
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
u32 I2C_Write(u8 slave_addr, u8 *data, u32 data_length){
	u8 *pdata = data;
	u32 len = 0;
	
	//Send start signal
	i2c_Start();
	
	//Send slave address and w/r bit
	I2C_SendByte(slave_addr);
	
	//Wait and analyze for acknowledge
	while(len < data_length){
		if(!i2c_WaitAck()){
			I2C_SendByte(pdata[len++]);
		}
		else{
			break;
		}
	}
	i2c_Stop();		
	return 0;
}

/**
* @brief Read an amount of data
* @retval uint32_t: It may be modified
*/
u32 I2C_Read(u8 slave_addr, u8 *buff, u8 numByteToRead){
	u8 *pdata = buff;
	u32 len = 0;
	
	//Send start signal
	i2c_Start();
	
	//Send slave address and w/r bit
	I2C_SendByte(slave_addr);
	
	//Wait and analyze for acknowledge
	if(!i2c_WaitAck()){
		while(len < numByteToRead){
			pdata[len++] = I2C_ReadByte();
			if(len == numByteToRead){
				i2c_SendNAck();
				break;
			}
			else{
				i2c_SendAck();
			}
		}
	}
	//Send end signal
	i2c_Stop();		
	return 0;
}


//
u8 UART_Process_Param(u8 *UART_buff, u8 uart_buff_length, u8 *I2C_buff, u8 I2C_buff_length){
	u8 *buff_ptr = UART_buff;
	__IO u8 buff_length = uart_buff_length, re_value = 0U;
	if(buff_ptr[2] == 0xFF && buff_length != 7){
		return param_error;
	}
	if((buff_ptr[2] == 0xF0 || buff_ptr[2] == 0x0F) && buff_length != 5){
		return param_error;
	}
	if(buff_ptr[0] == Slave_Get){
		I2C_Read(I2C_READ_ADDRESS, I2C_buff, 5);          //test
	}
	else if(buff_ptr[0] == Slave_Set){
		I2C_Write(buff_ptr[1], buff_ptr + 2, (buff_ptr[2] == 0xFF)?(5):(3));
		delay_ms(1);
		I2C_Read(I2C_READ_ADDRESS, I2C_buff, 4);          //test
	}
	else{
	 return param_error;
	}
}


/**
*test function
**/

void LED(u32 up_time){
	LED_ON;
	delay_us(up_time);
	LED_OFF;
}
