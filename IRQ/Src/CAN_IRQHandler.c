#include "CAN_IRQHandler.h"
#include "can.h"
#include "beep.h"
#include "fsm.h"

CAN_RxHeaderTypeDef RxHeader;
CAN_TxHeaderTypeDef TxHeader;
uint8_t RxData[8];
uint8_t TxData[8];
uint32_t TxMailbox;

void CAN_Send_Data(uint32_t id, uint8_t *data)
{
    TxHeader.ExtId = id;
    TxHeader.ExtId = 0x00;
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

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

    if (hcan->Instance == CAN1)
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            // todo
            uint8_t ack_data[8] = {'O', 'K', 0, 0, 0, 0, 0, 0};
            if (RxHeader.IDE == CAN_ID_EXT &&RxHeader.ExtId == 0x01020201)
            {
               uint8_t ack_data[8] = {'O', 'K', 0, 0, 0, 0, 0, 0};
               CAN_Send_Data(0x02010201, ack_data); 
               Beep_Trigger = 1;
               FSM_SetMode(MODE_WATER);
            }
            else if (RxHeader.IDE == CAN_ID_EXT && RxHeader.ExtId == 0x01020101)
            {
               uint8_t ack_data[8] = {'O', 'K', 0, 0, 0, 0, 0, 0};
               CAN_Send_Data(0x02010101, ack_data); 
               Beep_Trigger = 0;
               FSM_SetMode(MODE_BREATH);
            }
        
        }
    }
    else if (hcan->Instance == CAN2) //可以删掉吧
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            //处理CAN2的接收消息
        
        }   
    }
}




