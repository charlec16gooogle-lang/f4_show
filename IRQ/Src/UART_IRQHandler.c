#include "UART_IRQHandler.h"
#include "EXTI_IRQHandler.h"

uint8_t rx_buffer[100] = {0};
uint8_t tx_buffer[100] = {0};

void UART_Start_Recieve(void)
{
    //HAL_UART_Receive_IT(&huart1, rx_buffer, 5);//这个是定长中断接收
    //HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer)); //这个是空闲中断接收
    //HAL_UART_Receive_DMA(&huart1, rx_buffer, 5); //这个是DMA接收
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer)); //这个是空闲中断+DMA接收 夯爆了 首选
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // todo
        if (rx_buffer[0] == 0xFF)
        {

            for (uint8_t i = 0; i < 5; i++)
            {
                if (rx_buffer[i] == 1)
                {
                    Beep_Trigger++;
                }
            }
            
        }

        //HAL_UART_Receive_IT(&huart1, rx_buffer, 5);
        HAL_UART_Receive_DMA(&huart1, rx_buffer, 5); 
    }
}

 void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) //--空闲中断
{
   if (huart->Instance == USART1)
{
   //todo
 if(rx_buffer[0] == 0xFF)
{

     for (uint8_t i = 1; i < Size; i++)
   {
       if (rx_buffer[i] == 1)
       {
           Beep_Trigger++;
       }
     }
   
 }
 //HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer));
 HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));

}

}