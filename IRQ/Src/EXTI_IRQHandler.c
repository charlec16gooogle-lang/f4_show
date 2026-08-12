#include "EXTI_IRQHandler.h"

volatile uint8_t Beep_Trigger = 0;
volatile uint8_t key_flag = 0; // 0: 无动作, 1: 短按触发, 2: 长按触发
/* USER CODE END PV */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == INPUT_1_Pin)
  {
    
    Beep_Trigger = 1;
  }

    // 判断中断是否来自按键引脚
    if (GPIO_Pin == INPUT_1_Pin) 
    {
        static uint32_t press_time = 0;

        // 读取当前引脚电平
        if (HAL_GPIO_ReadPin(INPUT_1_GPIO_Port, INPUT_1_Pin) == GPIO_PIN_SET)
        {
            // 电平变为高电平 -> 上升沿触发 -> 代表【按键刚按下】
            press_time = HAL_GetTick(); // 记录按下时刻
        }
        else
        {
            // 电平变为低电平 -> 下降沿触发 -> 代表【按键刚松开】
            uint32_t duration = HAL_GetTick() - press_time; // 计算按压持续时长

            if (duration >= 1000)
            {
                key_flag = 2; // 判定为长按
            }
            else if (duration >= 50)
            {
                key_flag = 1; // 判定为短按（过滤 50ms 以内的抖动）
            }
        }
    }
}