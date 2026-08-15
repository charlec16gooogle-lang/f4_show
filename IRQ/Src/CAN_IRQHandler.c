#include "CAN_IRQHandler.h"
#include "can.h"
#include "app_protocol.h" // 引入我们刚刚写的协议大纲 (替代了以前的 fsm.h)

/* ======================================================== */
/* 1. 全局变量区：用来存放收到的指令，相当于以前的状态机模式 */
/* ======================================================== */
volatile uint8_t  g_breathe_enable = 0;       // 呼吸灯开关：1 开, 0 关 (默认开启)
volatile uint16_t g_breathe_period_ms = 1000; // 呼吸周期：默认 1000ms
volatile uint8_t  g_beep_remaining_count = 0; // 蜂鸣器还要响几次

CAN_RxHeaderTypeDef RxHeader;
CAN_TxHeaderTypeDef TxHeader;
uint8_t RxData[8];
uint8_t TxData[8];
uint32_t TxMailbox;

/* ======================================================== */
/* 2. CAN 发送函数 (修复了之前把 id 清零的 Bug) */
/* ======================================================== */
void CAN_Send_Data(uint32_t id, uint8_t *data)
{
    TxHeader.ExtId = id;
    // TxHeader.ExtId = 0x00; <--- 这个致命的覆盖 Bug 已经被删掉了！
    TxHeader.IDE = CAN_ID_EXT;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.DLC = 8;
    TxHeader.TransmitGlobalTime = DISABLE;

    // 数据拷贝到你的 TxData 数组
    for(int i = 0; i < 8; i++)
    {
        TxData[i] = data[i];
    }

    // 压入发送邮箱
    HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
}

/* ======================================================== */
/* 3. CAN 接收中断回调函数 (代替你在楼下等快递的动作) */
/* ======================================================== */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance == CAN1)
    {
        // 从 FIFO0 邮箱里把快递取出来，信封给 RxHeader，数据给 RxData
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            // 情况 A：看信封，如果是主板发来的呼吸灯控制指令 (标准帧 0x101)
            if (RxHeader.IDE == CAN_ID_STD && RxHeader.StdId == CAN_ID_BREATHE_CTRL)
            {
                // 把收到的 RxData 强行套上我们在 app_protocol.h 里画好的模板
                BreatheCtrlPacket_t *pkt = (BreatheCtrlPacket_t *)RxData;
                
                // 把模板里的开关状态，存到全局变量里
                g_breathe_enable = pkt->enable; 
                
                // 把模板里的周期数值，存到全局变量里 (加个限制防止瞎填个0卡死)
                if (pkt->period_ms >= 100) 
                {
                    g_breathe_period_ms = pkt->period_ms;
                }
            }
            // 情况 B：看信封，如果是 CANable 发来的蜂鸣器定次响指令 (标准帧 0x301)
            else if (RxHeader.IDE == CAN_ID_STD && RxHeader.StdId == CAN_ID_BEEP_TRIGGER)
            {
                // 套上蜂鸣器指令的模板
                BeepCtrlPacket_t *pkt = (BeepCtrlPacket_t *)RxData;
                
                // 拿到要响几声，存进全局变量里
                g_beep_remaining_count = pkt->beep_count;
            }
        }
    }
    // CAN2 的分支已经被彻底删除了，保持代码清爽！
}