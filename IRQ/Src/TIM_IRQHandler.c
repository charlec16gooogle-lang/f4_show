#include "TIM_IRQHandler.h"

uint16_t tim_count = 0;
static uint8_t dir = 1; 
uint16_t time = 0;

void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2)
  {
    if (dir == 1)
    {
      tim_count++;
    }
    else 
    {
      tim_count--;
    }
    if (tim_count >= 1000)
    {
      dir = 0;
    }
    else if (tim_count == 0)
    {
      dir = 1;
    }
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, tim_count);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, tim_count);
    
  }
}