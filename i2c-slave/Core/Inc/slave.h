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
u32 			get_units_mul(u8);
void  		flag_reset(void);
void 			param_assert(void);

/*				LED operation functions														*/
void 			set_led_frequency(u32);
void 			set_led_duration(u32);

/*				Slave related functions														*/
void 			slave_start(void); 
void 			Slave_EXTI_Rising_Callback(u16);
void			Slave_EXTI_Falling_Callback(u16);
u8 				Date_To_I2CBuff(void);

#endif
