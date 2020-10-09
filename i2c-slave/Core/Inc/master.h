#ifndef MASTER_H
#define MASTER_H


#include "i2c.h"
#include "usart.h"



// frequency and duration of the Green LED 
#define param_error 0x00
#define success_set_frequency  0x0F
#define success_set_duration   0xF0

extern u8 Rx_Byte;


void		 master_start(void);
void 		 Master_EXTI_Rising_Callback(u16);
void		 Master_EXTI_Falling_Callback(u16);

#endif
