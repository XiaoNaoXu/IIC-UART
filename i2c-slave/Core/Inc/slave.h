#ifndef SLAVE_H
#define SLAVE_H
#include "i2c.h"

void 			slave_start(void); 

void  		flag_reset(void);
void 			param_assert(void);

void 			set_led_frequency(u32);
void 			set_led_duration(u32);

u32 			get_units_mul(u8);

void 			Slave_EXTI_Rising_Callback(u16);
void			Slave_EXTI_Falling_Callback(u16);

u8 				Date_To_I2CBuff(void);

#endif
