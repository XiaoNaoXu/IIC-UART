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
	__IO int i = us * 6;
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
	delay_us(2);
	I2C_SDA_0();
	I2C_SCL_0();
	delay_us(12);
}

/**
* @brief Start stop function
* @retval void
*/
void i2c_Stop(){
	I2C_SDA_0();
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
	delay_us(2);
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
	delay_us(2);
	I2C_SCL_0();
}

/**
* @brief wiat slave or master return ack
* @param re_value: A ack or a Nack
* @retval uint8_t:acknowledge
*/
u8 i2c_WaitAck(){
	u8 re_value;
	I2C_SCL_1();
	delay_us(2);
	re_value = I2C_SDA_READ();
	I2C_SCL_0();
	return re_value;
}

/**
* @brief send a byte
* @param i: Control loop variable
* @retval void
*/
void I2C_SendByte(u8 data_byte){
	__IO u8 i = 0, j = 0;
	for(i = 0; i < 8; ++i){
		//
		j = (data_byte) & 0x80;
		(j) ? (I2C_SDA_1()):(I2C_SDA_0());
		I2C_SCL_1();
		delay_us(2);
		I2C_SCL_0();
		I2C_SDA_0();
		data_byte <<= 1U;
	}
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
		delay_us(2);
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
		if(i2c_WaitAck()){
//			I2C_SendByte(pdata[--len]);
			break;
		}
	}

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
	if(!i2c_WaitAck()){
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



/**
*test function
**/
void test(void){
	I2C_SCL_1();
	__IO u8 i = I2C_SCL_READ();
	if(I2C_SCL_READ()){
		LED_ON;
		delay_us(2);
	}
	delay_us(2);
	I2C_SCL_0();
	if(!I2C_SCL_READ()){
		LED_OFF;
		delay_us(2);
	}
}

void LED(u32 up_time){
	LED_ON;
	delay_us(up_time);
	LED_OFF;
}
