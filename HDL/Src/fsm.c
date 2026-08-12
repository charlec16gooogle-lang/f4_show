#include "fsm.h"
#include "beep.h"
#include "led.h"
#include "tim.h"

// static封装当前状态变量，防止外部直接访问
static SystemMode_t current_mode = MODE_OFF;

/**
  * @brief 状态机初始化
  */
void FSM_Init(void)
{
    current_mode = MODE_OFF;
}

/**
  * @brief 设置新状态
  */
void FSM_SetMode(SystemMode_t new_mode)
{
    current_mode = new_mode;
}

/**
  * @brief 获取当前状态
  */
SystemMode_t FSM_GetMode(void)
{
    return current_mode;
}

/**
  * @brief 状态机核心业务轮询函数
  */
void FSM_Process(void)
{
    uint32_t current_time = HAL_GetTick();
    static uint32_t last_water_time = 0;
    static uint32_t last_breath_time = 0;
    static uint8_t water_step = 0;
    static uint16_t pwm_val = 0;
    static int8_t pwm_dir = 5;

    switch (current_mode)
    {
        case MODE_OFF:
            // 所有 LED 熄灭
            HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0); 
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
            break;

        case MODE_WATER:
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0); 
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
            // 流水灯
            Led_Water();
            break;

        case MODE_BREATH:
            HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
            //呼吸灯
            TIM_PeriodElapsedCallback();
            break;
    }
}