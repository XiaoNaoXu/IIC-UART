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
	delay_us(I2C_PD);
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

