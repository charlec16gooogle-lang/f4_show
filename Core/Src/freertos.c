/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

#include "app_protocol.h"
#include "can.h"
#include "tim.h"
#include <math.h>

// 告诉单片机：这三个变量在 CAN_IRQHandler.c 里已经存在了，直接用
extern volatile uint8_t  g_breathe_enable;
extern volatile uint16_t g_breathe_period_ms;
extern volatile uint8_t  g_beep_remaining_count;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId WaterLedTaskHandle;
osThreadId BreatheTaskHandle;
osThreadId Feedback100HzTaHandle;
osThreadId Noise500HzTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartWaterLedTask(void const * argument);
void StartBreatheTask(void const * argument);
void StartFeedback100HzTask(void const * argument);
void StartNoise500HzTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of WaterLedTask */
  osThreadDef(WaterLedTask, StartWaterLedTask, osPriorityLow, 0, 128);
  WaterLedTaskHandle = osThreadCreate(osThread(WaterLedTask), NULL);

  /* definition and creation of BreatheTask */
  osThreadDef(BreatheTask, StartBreatheTask, osPriorityNormal, 0, 128);
  BreatheTaskHandle = osThreadCreate(osThread(BreatheTask), NULL);

  /* definition and creation of Feedback100HzTa */
  osThreadDef(Feedback100HzTa, StartFeedback100HzTask, osPriorityAboveNormal, 0, 256);
  Feedback100HzTaHandle = osThreadCreate(osThread(Feedback100HzTa), NULL);

  /* definition and creation of Noise500HzTask */
  osThreadDef(Noise500HzTask, StartNoise500HzTask, osPriorityNormal, 0, 128);
  Noise500HzTaskHandle = osThreadCreate(osThread(Noise500HzTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartWaterLedTask */
/**
* @brief Function implementing the WaterLedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWaterLedTask */
void StartWaterLedTask(void const * argument)
{
  /* USER CODE BEGIN StartWaterLedTask */
 uint8_t led_step = 0; // 记录当前该亮哪个灯
  /* Infinite loop */
  for(;;)
  {
    // 1. 两颗流水灯状态指示
    if (led_step == 0)
    {
      HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);   // 亮 1 号灯
      HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET); // 灭 2 号灯
      led_step = 1;
    }
    else
    {
      HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET); // 灭 1 号灯
      HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);   // 亮 2 号灯
      led_step = 0;
    }

    // 2. 蜂鸣器定次鸣叫响应
    if (g_beep_remaining_count > 0)
    {
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);   // 蜂鸣器响
      osDelay(100);
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); // 蜂鸣器停
      
      g_beep_remaining_count--;                                     // 剩余鸣叫次数减 1
      osDelay(100);                                                 // 间隔 100ms
    }
    else
    {
      // 蜂鸣器不响时，流水灯正常以 200ms 间隔交替
      osDelay(200); 
    }
  }
  /* USER CODE END StartWaterLedTask */
}

/* USER CODE BEGIN Header_StartBreatheTask */
/**
* @brief Function implementing the BreatheTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBreatheTask */
void StartBreatheTask(void const * argument)
{
  /* USER CODE BEGIN StartBreatheTask */
 uint16_t pwm_val = 0;   // 当前的亮度（0 ~ 1000）
  int8_t   step_dir = 1;  // 变化方向：1 代表正在变亮，-1 代表正在变暗

  /* Infinite loop */
  for(;;)
  {
    // 1. 如果主板允许呼吸灯开启
    if (g_breathe_enable == 1)
    {
      // 计算下一步的亮度 
      if (step_dir == 1) // 正在吸气（变亮）
      {
        pwm_val += 10;
        if (pwm_val >= 1000) 
        {
          pwm_val = 1000;
          step_dir = -1; // 到达最亮，掉头开始变暗
        }
      }
      else // 正在呼气（变暗）
      {
        if (pwm_val <= 10)
        {
          pwm_val = 0;
          step_dir = 1;  // 到达最暗，掉头开始变亮
        }
        else
        {
          pwm_val -= 10;
        }
      }

      // 把新亮度写进硬件定时器 (PA6, PA7) 
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_val);
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwm_val);

      // 根据主板下发的周期，动态计算每一步要睡多久 
      uint32_t step_delay = g_breathe_period_ms / 200;
      if (step_delay < 1) 
      {
        step_delay = 1; // 至少延时 1ms，防止除法除成 0
      }

      osDelay(step_delay); // 把 CPU 让出来，给其他任务运行
    }
    else
    {
      // 2. 如果主板发来指令关停呼吸灯：直接熄灭，并休眠省电
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
      
      osDelay(100); // 停止状态下不要写 osDelay(1)，每 100ms 检查一次开关即可（强调）
    }
  }
  /* USER CODE END StartBreatheTask */
}

/* USER CODE BEGIN Header_StartFeedback100HzTask */
/**
* @brief Function implementing the Feedback100HzTa thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFeedback100HzTask */
void StartFeedback100HzTask(void const * argument)
{
  /* USER CODE BEGIN StartFeedback100HzTask */
 CAN_TxHeaderTypeDef tx_header;
  uint32_t tx_mailbox;
  FeedbackPacket_t packet; // 我们在 app_protocol.h 里定义好的数据包
  float angle = 0.0f;      // 用来算正弦波的角度

  //准备好 CAN 发送的信封
  tx_header.StdId = CAN_ID_SLAVE_FEEDBACK; // 标准帧 ID: 0x201
  tx_header.IDE   = CAN_ID_STD;            // 标准帧模式
  tx_header.RTR   = CAN_RTR_DATA;          // 数据帧 (不是远程请求帧)
  tx_header.DLC   = sizeof(FeedbackPacket_t); // 数据长度 (7 个字节)
  tx_header.TransmitGlobalTime = DISABLE;

  /* Infinite loop */
  for(;;)
  {
    //制造一个连续变化的 float 浮点数 
    packet.data_value = 100.0f * sinf(angle);
    angle += 0.05f;
    if (angle >= 6.283185f) // 超过 2*PI (360度) 就归零重新开始
    {
      angle = 0.0f;
    }

    // 把呼吸灯当前的参数一起装进包裹，供主板打 log 
    packet.period_log = (uint16_t)g_breathe_period_ms;
    packet.state_log  = (uint8_t)g_breathe_enable;

    // 检查 CAN 硬件发送邮箱，如果有空位就发出去 
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0)
    {
      HAL_CAN_AddTxMessage(&hcan1, &tx_header, (uint8_t *)&packet, &tx_mailbox);
    }

    osDelay(10);
  }
  /* USER CODE END StartFeedback100HzTask */
}

/* USER CODE BEGIN Header_StartNoise500HzTask */
/**
* @brief Function implementing the Noise500HzTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartNoise500HzTask */
void StartNoise500HzTask(void const * argument)
{
  /* USER CODE BEGIN StartNoise500HzTask */
 CAN_TxHeaderTypeDef tx_header;
  uint32_t tx_mailbox;
  
  // 准备 8 个垃圾字节（随便填什么都行，比如 0xDE, 0xAD...）
  uint8_t dummy_data[8] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};

  //  准备扩展帧信封 
  tx_header.ExtId = CAN_ID_NOISE_EXT; // 扩展帧 ID: 0x02010101 (定义在 app_protocol.h)
  tx_header.IDE   = CAN_ID_EXT;       // 声明这是 29 位扩展帧！
  tx_header.RTR   = CAN_RTR_DATA;     // 数据帧
  tx_header.DLC   = 8;                // 填满 8 字节
  tx_header.TransmitGlobalTime = DISABLE;

  /* Infinite loop */
  for(;;)
  {
    // 检查是否有空闲邮箱，有就往总线上狂发
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0)
    {
      HAL_CAN_AddTxMessage(&hcan1, &tx_header, dummy_data, &tx_mailbox);
    }
    osDelay(2);
  }
  /* USER CODE END StartNoise500HzTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
