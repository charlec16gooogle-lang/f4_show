#include "CAN_IRQHandler.h"
#include "can.h"

CAN_RxHeaderTypeDef RxHeader;
CAN_TxHeaderTypeDef TxHeader;
uint8_t RxData[8];
uint8_t TxData[8];
uint32_t TxMailbox;

void CAN_Send_Data(uint16_t id, uint8_t *data)
{
    TxHeader.StdId = id;
    TxHeader.ExtId = 0x00;
    TxHeader.IDE = CAN_ID_STD;
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
            if (RxHeader.StdId == 0x201)
            {
               Beep_Trigger = 1;
            }
            else if (
                RxHeader.StdId == 0x202)
            {
                //这是二号电机发来的反馈...
            }
        
        }
    }
    else if (hcan->Instance == CAN2)
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            //处理CAN2的接收消息
        
        }   
    }
}




