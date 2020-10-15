/**
  ******************************************************************************
  * @file           : slave.h
  * @brief          : Header for slave.c file.

  ******************************************************************************
  */

#ifndef SLAVE_H
#define SLAVE_H

/*				Include																						*/
#include "i2c.h"


/*				Process parameter functions												*/
u32 			get_units_mul(I2C_TYPE);
void  		flag_reset(void);
void 			param_assert(void);

/*				LED operation functions														*/
void 			set_led_frequency(u32);
void 			set_led_duration(u32);

/*				Slave related functions														*/
void 			slave_start(void); 
void 			Slave_SDA_EXTI_Rising_Callback(void);
void			Slave_SDA_EXTI_Falling_Callback(void);
void 			Slave_SCL_EXTI_Rising_Callback(void);
void			Slave_SCL_EXTI_Falling_Callback(void);
I2C_TYPE 				Date_To_I2CBuff(I2C_TYPE);

#endif
