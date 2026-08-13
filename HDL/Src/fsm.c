#include "fsm.h"
#include "beep.h"
#include "led.h"
#include "tim.h"
#include "CAN_IRQHandler.h"

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

    // CAN 控制变量
    static uint32_t last_can_time = 0; // 记录上次发 CAN 报文的时间
    int16_t target_speed = 500;        // 假定正常运行时的目标速度为 500

   
    /* CAN 非阻塞发送 (每 10ms 周期执行一次) */
    
    if (current_time - last_can_time >= 10)
    {
        last_can_time = current_time;
        uint8_t can_data[8] = {0}; // 准备一个干净的 8 字节数组

        if (current_mode == MODE_OFF)
        {
            // 关机模式：发送全 0 安全保护报文
            can_data[0] = 0x00; // Byte 0 = 0 (使能关闭)
            can_data[1] = 0x00; // Byte 1 = 0
            can_data[2] = 0x00; // Byte 2 = 0
        }
        else // 如果是 MODE_WATER 或 MODE_BREATH (代表系统在运行)
        {
           
            can_data[0] = 0x01; // Byte 0 = 1 (使能开启)
            can_data[1] = (uint8_t)(target_speed >> 8);   // 高 8 位
            can_data[2] = (uint8_t)(target_speed & 0xFF); // 低 8 位
        }

        // 发送给电机
        CAN_Send_Data(0x01020201, can_data);
    }

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
            break;
    }
}