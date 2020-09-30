#include "master.h"




void master_start(){
	
	I2C_Master_GPIO_Output_OD_Init();
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&Rx_Byte, 1);
	while(running_state == MASTER);
	
}


//
