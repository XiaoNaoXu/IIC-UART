
#include "tim.h"

TIM_HandleTypeDef tim3;

void TIM3_Init(){
	TIM_ClockConfigTypeDef Tim3Clock = {0};
	
	tim3.Instance									=  	TIM3;
	tim3.Init.Prescaler					  =  	63;
	tim3.Init.CounterMode 				= 	TIM_COUNTERMODE_UP;
	tim3.Init.Period 							= 	I2C_BUS_TIMEOUT;
	tim3.Init.ClockDivision				= 	TIM_CLOCKDIVISION_DIV1;
	tim3.Init.AutoReloadPreload 	= 	TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&tim3);
	
	Tim3Clock.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&tim3, &Tim3Clock);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  }
}

