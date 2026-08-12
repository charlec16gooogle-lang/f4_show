#include "vofa.h"
#include "usart.h" // 需要用到 huart1
#include "beep.h"  // 需要用到蜂鸣器函数
#include <math.h>  // 需要用到 sinf() 函数

// 定义全局变量
static Vofa_Frame_t vofa_frame = {0.0f, {0x00, 0x00, 0x80, 0x7F}};
static float sine_angle = 0.0f;
uint8_t rx_data = 0; // 存放串口收到的单字节数据

/**
  * @brief  VOFA 模块初始化
  */
void VOFA_Init(void)
{
    // 开启串口 1 接收中断，等待电脑发消息
    // （如果你的串口是 USART2，请把 &huart1 改成 &huart2）
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
}

/**
  * @brief  VOFA 周期发送正弦波任务
  * @note   建议放在 while(1) 中调用
  */
void VOFA_SendTask(void)
{
    static uint32_t last_vofa_time = 0;

    // 非阻塞方式：每 10ms 发送一次数据帧
    if (HAL_GetTick() - last_vofa_time >= 10)
    {
        last_vofa_time = HAL_GetTick();

        // 1. 计算正弦波数值
        vofa_frame.channel1 = 10.0f * sinf(sine_angle);
        sine_angle += 0.05f; 
        if (sine_angle >= 6.283185f) sine_angle = 0.0f; // 满 2π 归零

        // 2. 通过串口发送 8 个字节
        HAL_UART_Transmit(&huart1, (uint8_t *)&vofa_frame, sizeof(vofa_frame), 10);
    }
}
